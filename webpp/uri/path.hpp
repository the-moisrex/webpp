// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "details/constants.hpp"
#include "details/special_schemes.hpp"
#include "details/uri_components.hpp"
#include "encoding.hpp"

#include <compare>
#include <numeric>

namespace webpp::uri {

    namespace details {


        // /// A leading surrogate is a code point that is in the range U+D800 to U+DBFF, inclusive.
        // /// https://infra.spec.whatwg.org/#leading-surrogate
        // template <typename CharT = char>
        // static constexpr auto leading_surrogate = charset_range<CharT, 0xD800, 0xDBFF>();

        // /// A trailing surrogate is a code point that is in the range U+DC00 to U+DFFF, inclusive.
        // /// https://infra.spec.whatwg.org/#trailing-surrogate
        // template <typename CharT = char>
        // static constexpr auto trailing_surrogate = charset_range<CharT, 0xDC00, 0xDFFF>();

        // /// A surrogate is a leading surrogate or a trailing surrogate.
        // /// https://infra.spec.whatwg.org/#surrogate
        // template <typename CharT = char>
        // static constexpr auto surrogate = charset(leading_surrogate<CharT>, trailing_surrogate<CharT>);


        // template <typename CharT = char>
        // static constexpr auto url_code_points =
        //   charset(ALPHA_DIGIT<CharT>,
        //           charset<CharT, 19>('!',
        //                              '$',
        //                              '&',
        //                              '(',
        //                              ')',
        //                              '\'',
        //                              '*',
        //                              '+',
        //                              ',',
        //                              '-',
        //                              '.',
        //                              '/',
        //                              ':',
        //                              ';',
        //                              '=',
        //                              '?',
        //                              '@',
        //                              '_',
        //                              '~'),
        //           // and code points in the range U+00A0 to U+10FFFD,
        //           // inclusive, excluding surrogates and noncharacters.
        //           charset_range<CharT, 0x00A0, 0x10FFFD>().except(surrogate<CharT>));


        template <typename... T>
        static constexpr bool
        has_normalized_windows_driver_letter(uri::parsing_uri_context<T...>& ctx) noexcept {
            return ASCII_ALPHA.contains(*ctx.pos) && ctx.pos[1] == ':';
        }


        template <typename... T>
        static constexpr void
        append_path(uri::parsing_uri_context<T...>&                   ctx,
                    typename uri::parsing_uri_context<T...>::iterator start,
                    typename uri::parsing_uri_context<T...>::iterator end,
                    bool const needs_encoding) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
            using ctx_type = uri::parsing_uri_context<T...>;

            if (needs_encoding) { // slow path
                if constexpr (ctx_type::is_segregated) {
                    auto path_ref = ctx.out.path_ref();
                    istl::collection::emplace(path_ref, path_ref.get_allocator());
                    encode_uri_component<uri_encoding_policy::disallowed_chars>(start,
                                                                                end,
                                                                                path_ref().back(),
                                                                                details::PATH_ENCODE_SET);
                } else if constexpr (ctx_type::is_modifiable) {
                    encode_uri_component<uri_encoding_policy::disallowed_chars>(start,
                                                                                end,
                                                                                ctx.out.path_ref(),
                                                                                details::PATH_ENCODE_SET);
                }
            } else { // quicker
                if constexpr (ctx_type::is_segregated) {
                    istl::collection::emplace(ctx.out.path_ref(),
                                              start,
                                              end,
                                              ctx.out.path_ref().get_allocator());
                } else if constexpr (ctx_type::is_modifiable) {
                    ctx.out.path_ref().append(start, end);
                }
            }
        }

    } // namespace details

    template <typename... T>
    static constexpr void parse_opaque_path(uri::parsing_uri_context<T...>& ctx) noexcept(
      uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state

        auto path_end = ctx.pos;
        while (path_end != ctx.end) {
            switch (*ctx.pos) {
                case '?':
                    ctx.out.clear_queries();
                    ++path_end;
                    uri::set_valid(ctx.status, uri_status::valid_queries);
                    break;
                case '#':
                    ctx.out.clear_fragment();
                    ++path_end;
                    uri::set_valid(ctx.status, uri_status::valid_fragment);
                    break;
                default: {

                    // todo: validate uri_status::invalid_character here
                    // 1. If c is not the EOF code point, not a URL code point, and not U+0025 (%),
                    // invalid-URL-unit validation error.
                    // 2. If c is U+0025 (%) and remaining does not start with two ASCII hex digits,
                    // invalid-URL-unit validation error.
                    // 3. If c is not the EOF code point, UTF-8 percent-encode c using the C0 control
                    // percent-encode set and append the result to url’s path.

                    ++path_end;
                    continue;
                }
            }
            break;
        }
        ctx.out.path(ctx.pos, path_end);
        ctx.pos = path_end;
    }

    template <typename... T>
    static constexpr void
    parse_path(uri::parsing_uri_context<T...>& ctx) noexcept(uri::parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using details::ascii_bitmap;

        using ctx_type = uri::parsing_uri_context<T...>;
        webpp_static_constexpr auto encode_set =
          ctx_type::is_modifiable || ctx_type::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        webpp_static_constexpr auto interesting_chars =
          ascii_bitmap(encode_set, ascii_bitmap{'\\', '\0', '/', '?', '#', '%', '.'});


        if (ctx.pos == ctx.end) {
            ctx.out.clear_path();
            uri::set_valid(ctx.status, uri_status::valid);
            return;
        }

        bool const is_windows_path =
          details::has_normalized_windows_driver_letter(ctx) && ctx.out.scheme() == "file";

        if constexpr (ctx_type::is_modifiable) {
            // encode_uri_component_set_capacity(ctx.pos, ctx.end, ctx.out.host_ref());
        }


        auto const   beg                  = ctx.pos;
        unsigned int dotted_segment_count = 0;

        // start position of last 2 segments:
        auto prev_segment_start = ctx.end;
        auto segment_start      = beg;

        // boolean to check if we need to encode the path or not:
        bool prev_segment_needs_encoding = false;
        bool segment_needs_encoding      = false;

        bool is_done = false;

        for (;;) {

            ctx.pos = interesting_chars.find_first_in(ctx.pos, ctx.end);
            if (ctx.pos == ctx.end) {
                uri::set_valid(ctx.status, uri_status::valid);

                // set the previous segment
                if (prev_segment_start != ctx.end) {
                    details::append_path(ctx, prev_segment_start, segment_start, prev_segment_needs_encoding);
                }
                break;
            }
            switch (*ctx.pos) {
                case '.':
                    if (segment_start + dotted_segment_count == ctx.pos) {
                        ++dotted_segment_count;
                    }
                    ++ctx.pos;
                    continue;
                case '\\':
                    if (ctx.is_special) {
                        uri::set_warning(ctx.status, uri_status::reverse_solidus_used);
                    } else {
                        // todo: do we need to enable encoding for this character?
                        ++ctx.pos;
                        continue;
                    }
                    [[fallthrough]];
                case '/': ++ctx.pos; break;
                case '\0':
                    is_done = true; // todo: is this correct?
                    break;
                case '?':
                    uri::set_valid(ctx.status, uri_status::valid_queries);
                    is_done = true;
                    break;
                case '#':
                    uri::set_valid(ctx.status, uri_status::valid_fragment);
                    is_done = true;
                    break;
                case '%':
                    if (ctx.pos + 2 < ctx.end) {
                        // %2E or %2e is equal to a "." (dot)
                        if (*ctx.pos != '2' || ctx.pos[1] == 'e' || ctx.pos[1] == 'E') {
                            ctx.pos += 2;
                            dotted_segment_count += 3;
                            continue;
                        }
                        if (validate_percent_encode(ctx.pos, ctx.end)) {
                            continue;
                        }
                    }
                    uri::set_warning(ctx.status, uri_status::invalid_character);
                    continue;
                default: segment_needs_encoding = true; continue;
            }

            if ((dotted_segment_count & 0b1U) == 0b1U) { // single dot
                // Number of chars of each dot (or %2e):
                //   3 3 = 6  0b110 > double dot
                //   3 1 = 4  0b100 > double dot
                //   1 3 = 4  0b100 > double dot
                //   1 1 = 2  0b010 > double dot
                //   1   = 1  0b001 > single dot
                //   3   = 3  0b011 > single dot
                //                ^
                //         The deciding bit
                dotted_segment_count = 0;
                prev_segment_start   = segment_start;
                segment_start        = ctx.pos;
                continue; // ignore this path segment
            }
            if (dotted_segment_count != 0) { // double dot
                // remove the last segment as well
                prev_segment_start   = ctx.end;
                segment_start        = ctx.pos;
                dotted_segment_count = 0;
                continue;
            }

            // Append the last segment (not the current one)
            if (prev_segment_start != ctx.end) {
                details::append_path(ctx, prev_segment_start, segment_start, prev_segment_needs_encoding);
            }
            dotted_segment_count        = 0; // zeroing out the dot count
            prev_segment_start          = segment_start;
            segment_start               = ctx.pos;
            prev_segment_needs_encoding = segment_needs_encoding;
            segment_needs_encoding      = false;

            if (!is_done) {
                continue;
            }

            break;
        }
        if constexpr (!ctx_type::is_segregated && !ctx_type::is_modifiable) {
            // append the whole thing right now, no encoding, no nothing
            ctx.out.path(beg, ctx.pos);
        } else {
            // Append the last segment
            details::append_path(ctx, segment_start, ctx.pos, segment_needs_encoding);
        }

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
        }
    }

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::String<T>;

    template <Slug SlugType = stl::string>
    struct basic_path
      : public stl::vector<stl::remove_cvref_t<SlugType>,
                           rebind_allocator<typename stl::remove_cvref_t<SlugType>::allocator_type,
                                            stl::remove_cvref_t<SlugType>>> {
        using slug_type           = stl::remove_cvref_t<SlugType>;
        using slug_allocator_type = typename slug_type::allocator_type;
        using allocator_type      = rebind_allocator<slug_allocator_type, slug_type>; // vector
        using container_type      = stl::vector<slug_type, allocator_type>;
        using value_type          = slug_type;
        using char_type           = istl::char_type_of_t<slug_type>;
        using string_type         = stl::
          conditional_t<istl::String<value_type>, value_type, stl::basic_string<char_type, allocator_type>>;
        using string_view_type = istl::string_view_type_of<value_type>;
        using path_type        = basic_path;
        using iterator         = typename container_type::iterator;
        using const_iterator   = typename container_type::const_iterator;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

        template <typename... T>
            requires(stl::is_constructible_v<container_type, T...>)
        explicit constexpr basic_path(T&&... args) : container_type{stl::forward<T>(args)...} {}

        // NOLINTBEGIN(*-forwarding-reference-overload)
        template <istl::StringViewifiable T>
            requires(!istl::cvref_as<T, basic_path>)
        explicit constexpr basic_path(T&& str, allocator_type const& alloc = allocator_type{})
          : container_type{alloc} {
            parse(istl::string_viewify_of<string_view_type>(stl::forward<T>(str)));
        }

        template <istl::String T>
            requires(!istl::cvref_as<T, basic_path> &&
                     istl::cvref_as<typename T::allocator_type, allocator_type>)
        explicit constexpr basic_path(T&& str) : container_type{str.get_allocator()} {
            parse(istl::string_viewify_of<string_view_type>(stl::forward<T>(str)));
        }
        // NOLINTEND(*-forwarding-reference-overload)

        template <istl::StringifiableOf<string_view_type> StrT>
        constexpr bool parse(StrT&& str) {
            auto path = istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
            if (path.empty()) {
                return true;
            }

            for (;;) {
                const stl::size_t slash_start = path.find(separator);
                const stl::size_t the_size    = stl::min(slash_start, path.size());
                value_type        val{this->get_allocator()};
                if (!decode_uri_component(path.substr(0, the_size), val, allowed_chars)) {
                    // error: invalid string passed as a path
                    this->clear();
                    // val = path.substr(0, the_size); // put the non-decoded value
                    return false;
                }
                this->push_back(stl::move(val));
                if (slash_start == string_view_type::npos) {
                    break;
                }
                path.remove_prefix(slash_start + 1);
            }
            return true;
        }

        template <istl::StringViewifiable SegStrT>
        constexpr basic_path& operator/=(SegStrT&& seg_str) {
            auto path = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(seg_str));
            if (path.empty()) {
                return *this;
            }

            for (;;) {
                const stl::size_t slash_start = path.find(separator);
                const stl::size_t the_size    = stl::min(slash_start, path.size());
                value_type        val{this->get_allocator()};
                if (!decode_uri_component(path.substr(0, the_size), val, allowed_chars)) {
                    // error: invalid string passed as a path
                    val = path.substr(0, the_size); // put the non-decoded value
                }
                this->push_back(stl::move(val));
                if (slash_start == string_view_type::npos) {
                    break;
                }
                path.remove_prefix(slash_start + 1);
            }
            return *this;
        }

        constexpr basic_path& operator=(value_type str) {
            this->clear();
            parse(stl::move(str));
            return *this;
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(basic_path const& rhs) const noexcept {
            const auto lhs_size = this->size();
            const auto rhs_size = rhs.size();
            if (lhs_size != rhs_size) {
                return stl::compare_partial_order_fallback(lhs_size, rhs_size);
            }
            if (stl::equal(this->begin(), this->end(), rhs.begin(), rhs.end())) {
                return stl::partial_ordering::equivalent;
            }
            return stl::partial_ordering::unordered;
        }


        template <istl::StringViewifiable SegStrT>
            requires(!stl::same_as<stl::remove_cvref_t<SegStrT>, basic_path>)
        [[nodiscard]] constexpr auto operator<=>(SegStrT&& rhs) const {
            // todo: optimize this
            auto const      path_str = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(rhs));
            path_type const rhs_path{path_str, this->get_allocator()};
            return *this <=> rhs_path;
        }


        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !this->empty() && this->front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        constexpr void normalize(bool remove_empty_segments = false) {
            remove_dot_segments(is_absolute(), remove_empty_segments);
        }


        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool remove_leading, bool remove_empty_segments = false) {
            if (this->empty()) {
                return;
            }

            auto pos = this->begin();

            // handle the first part
            while (pos != this->end()) {
                if (remove_empty_segments && pos->empty()) {
                    pos = this->erase(pos);
                    continue;
                }
                if (*pos == current_dir) {
                    pos = this->erase(pos);
                    continue;
                }
                if (*pos == parent_dir) {
                    if (pos != this->begin()) {
                        const auto last_el = std::prev(pos);
                        if (last_el->empty()) {
                            // remove just this one
                            pos = this->erase(pos);
                            continue;
                        }
                        if (*last_el != parent_dir) {
                            // remove the previous one and this one
                            pos = this->erase(last_el, std::next(pos));
                            if (pos == this->begin()) {
                                return;
                            }
                            --pos;
                            continue;
                        }
                    } else if (remove_leading) {
                        pos = this->erase(pos);
                        continue;
                    }
                }
                pos++;
            }
        }

        constexpr basic_path const& append_to(istl::String auto& str) const {
            if (this->size() == 0) {
                return *this;
            }

            auto pos = this->cbegin();

            // handling empty this special path: "/"
            if (pos->empty() && this->size() == 1) {
                str.append(separator);
                return *this;
            }

            str.append(*pos++);
            for (; pos != this->cend(); ++pos) {
                str.append(separator);
                encode_uri_component(*pos, str, allowed_chars);
            }
            return *this;
        }


        [[nodiscard]] constexpr string_type to_string() const {
            string_type str{this->get_allocator()};
            append_to(str);
            return str;
        }


        /**
         * Get the raw string non-decoded size
         */
        [[nodiscard]] constexpr stl::size_t raw_string_size() const noexcept {
            // todo: we could remove lambda; or we even can use an iterator_wrapper and use "std::reduce"
            // http://www.boost.org/doc/libs/1_64_0/libs/iterator/doc/transform_iterator.html
            return [this]() noexcept -> stl::size_t {
                stl::size_t sum = 0;
                for (auto const& slug : *this) {
                    sum += slug.size();
                }
                return sum;
            }() + this->size() - 1;
        }


        constexpr void fix() {
            // remove the last empty string
            if (this->size() && this->back().empty()) {
                (void) this->pop_back();
            }
        }
    };


    template <istl::Stringifiable S>
    basic_path(S&& str) -> basic_path<stl::remove_cvref_t<decltype(istl::stringify(stl::forward<S>(str)))>>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
