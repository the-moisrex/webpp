// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../strings/peek.hpp"
#include "details/constants.hpp"
#include "details/uri_components_encoding.hpp"
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
        static constexpr bool has_normalized_windows_driver_letter(parsing_uri_context<T...>& ctx) noexcept {
            return ASCII_ALPHA.contains(*ctx.pos) && ctx.pos[1] == ':';
        }

        // todo: remove this if it's not needed anymore
        template <typename... T>
        static constexpr void append_path(
          parsing_uri_context<T...>&                   ctx,
          typename parsing_uri_context<T...>::iterator start,
          typename parsing_uri_context<T...>::iterator end,
          bool const needs_encoding) noexcept(parsing_uri_context<T...>::is_nothrow) {
            using ctx_type = parsing_uri_context<T...>;

            if ((ctx_type::is_segregated || ctx_type::is_modifiable) && needs_encoding) { // slow path
                if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable) {
                    auto path_ref = ctx.out.path_ref();
                    istl::collection::emplace_one(path_ref, path_ref.get_allocator());
                    encode_uri_component<uri_encoding_policy::encode_chars>(
                      start,
                      end,
                      path_ref.back(),
                      PATH_ENCODE_SET);
                } else if constexpr (ctx_type::is_modifiable) {
                    encode_uri_component<uri_encoding_policy::encode_chars>(
                      start,
                      end,
                      ctx.out.path_ref(),
                      PATH_ENCODE_SET);
                }
            } else { // quicker
                if constexpr (ctx_type::is_segregated && ctx_type::is_modifiable) {
                    istl::collection::emplace_one(
                      ctx.out.path_ref(),
                      start,
                      end,
                      ctx.out.path_ref().get_allocator());
                } else if constexpr (ctx_type::is_modifiable) {
                    ctx.out.path_ref().append(start, end);
                    // } else {
                    //     ctx.out.path(start, end);
                }
            }
        }

        /// Handle special cases:
        ///   /.
        ///   /..
        ///   /%2e
        ///   /%2e%2e
        ///   /.%2e
        ///   /%2e.
        ///
        /// %2E or %2e is equal to a "." (dot)
        template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
        static constexpr void handle_dots_in_paths(
          parsing_uri_context<T...>& ctx,
          component_encoder<components::path, parsing_uri_context<T...>>&
            encoder) noexcept(parsing_uri_context<T...>::is_nothrow) {
            using ctx_type = parsing_uri_context<T...>;

            auto pos = ctx.pos + 1;
            switch (*pos) {
                [[unlikely]] case '%':
                    if (ascii::inc_if(pos, ctx.end, '2') && (*pos == 'e' || *pos == 'E')) {
                        break;
                    }
                    return;
                case '.':
                    break;
                [[likely]] default:
                    return;
            }
            ++pos;
            if (pos == ctx.end) {
                encoder.clear_segment();
                return;
            }

            switch (*pos) {
                [[unlikely]] case '\\':
                    if (!ctx.is_special) {
                        return;
                    }
                    set_warning(ctx.status, uri_status::reverse_solidus_used);
                    [[fallthrough]];
                [[likely]] case '/':
                    encoder.clear_segment();
                    return;
                [[unlikely]] case '%':
                    if (ascii::inc_if(pos, ctx.end, '2') && (*pos == 'e' || *pos == 'E')) {
                        break;
                    }
                    return;
                [[likely]] case '.':
                    break;
                default: return;
            }

            ++pos;
            if (pos != ctx.end) {
                switch (*pos) {
                    [[unlikely]] case '\\':
                        if (!ctx.is_special) {
                            return;
                        }
                        set_warning(ctx.status, uri_status::reverse_solidus_used);
                        [[fallthrough]];
                    [[likely]] case '/':
                        break;
                    default: return;
                }
            }


            // remove the last segment as well
            slash_loc_cache >>= a_byte;
            if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                auto hint = static_cast<difference_type>(slash_loc_cache & slash_mask);
                if (hint == 0) { // the cache is empty, too many /../../../.. in
                                 // the URL.
                    // find the last slash
                    for (auto pos = ctx.pos; pos == ctx.beg || *pos == '/'; --pos) {
                        ++hint;
                    }
                }
                encoder.pop_back(hint);
            } else {
                encoder.pop_back();
            }
            encoder.reset_segment_start();
        }
    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_opaque_path(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state

        using ctx_type = parsing_uri_context<T...>;

        webpp_static_constexpr auto interesting_characters = details::ascii_bitmap('%', '#', '?');

        details::component_encoder<details::components::path, ctx_type> encoder(ctx);
        encoder.start_segment();
        for (;;) {
            if (encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                  details::C0_CONTROL_ENCODE_SET,
                  interesting_characters))
            {
                set_valid(ctx.status, uri_status::valid);
                encoder.end_segment();
                encoder.set_value();
                break;
            }
            switch (*ctx.pos) {
                case '?': set_valid(ctx.status, uri_status::valid_queries); break;
                case '#': set_valid(ctx.status, uri_status::valid_fragment); break;
                case '%':
                    if (encoder.validate_percent_encode()) {
                        continue;
                    }
                    [[fallthrough]];
                default:
                    ++ctx.pos;
                    set_warning(ctx.status, uri_status::invalid_character);
                    continue;
            }
            encoder.end_segment();
            encoder.set_value();
            ++ctx.pos; // it's okay, we're not at the end
            break;
        }
    }

    template <uri_parsing_options Options = uri_parsing_options{}, typename... T>
    static constexpr void parse_path(parsing_uri_context<T...>& ctx) noexcept(
      parsing_uri_context<T...>::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using details::ascii_bitmap;

        using ctx_type        = parsing_uri_context<T...>;
        using iterator        = typename ctx_type::iterator;
        using difference_type = typename stl::iterator_traits<iterator>::difference_type;


        webpp_static_constexpr auto encode_set =
          ctx_type::is_modifiable || ctx_type::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        webpp_static_constexpr auto interesting_chars =
          ascii_bitmap(encode_set, ascii_bitmap{'\\', '\0', '/', '?', '#', '%'});


        if (ctx.pos == ctx.end) {
            ctx.out.clear_path();
            set_valid(ctx.status, uri_status::valid);
            return;
        }

        if (!ctx.is_special) {
            parse_opaque_path(ctx);
            return;
        }

        bool const is_windows_path =
          details::has_normalized_windows_driver_letter(ctx) && ctx.out.get_scheme() == "file";


        webpp_static_constexpr auto a_byte =
          static_cast<stl::uint8_t>(stl::numeric_limits<stl::uint8_t>::digits);
        webpp_static_constexpr auto slash_mask =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint8_t>::max());
        stl::uint64_t slash_loc_cache = 0;

        bool is_done = false;

        details::component_encoder<details::components::path, ctx_type> encoder{ctx};
        encoder.start_segment();
        for (;;) {
            if (!encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                  details::PATH_ENCODE_SET,
                  interesting_chars))
            {
                switch (*ctx.pos) {
                    case '\\':
                        if (ctx.is_special) {
                            set_warning(ctx.status, uri_status::reverse_solidus_used);
                        } else {
                            encoder.skip_separator();
                            continue;
                        }
                        [[fallthrough]];
                    [[likely]] case '/':

                        if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                            slash_loc_cache <<= a_byte;
                            if (static_cast<stl::uint64_t>(ctx.pos - encoder.segment_begin()) < slash_mask) {
                                slash_loc_cache |=
                                  static_cast<stl::uint64_t>(ctx.pos - encoder.segment_begin());
                            }
                        }

                        details::handle_dots_in_paths(ctx, encoder);

                        break;
                    case '?':
                        set_valid(ctx.status, uri_status::valid_queries);
                        is_done = true;
                        break;
                    case '#':
                        set_valid(ctx.status, uri_status::valid_fragment);
                        is_done = true;
                        break;
                    [[likely]] case '%':
                        if (ctx.pos + 2 >= ctx.end) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            break;
                        }
                        if (encoder.validate_percent_encode()) {
                            continue;
                        }
                        set_warning(ctx.status, uri_status::invalid_character);
                        continue;
                    [[unlikely]] case '\0':
                        if constexpr (Options.eof_is_valid) {
                            is_done = true; // todo: is this correct?
                            break;
                        }
                        [[fallthrough]];
                    default: set_warning(ctx.status, uri_status::invalid_character); break;
                }
            } else {
                is_done = true;
            }


            // Append the last segment (not the current one)

            if (!is_done) {
                encoder.next_segment();
                continue;
            }

            encoder.end_segment();
            break;
        }
        encoder.set_value();

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end) {
            ++ctx.pos;
        } else {
            set_valid(ctx.status, uri_status::valid);
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

        static constexpr string_view_type parent_dir    = "..";
        static constexpr string_view_type current_dir   = ".";
        static constexpr string_view_type separator     = "/"; // todo: make sure the user can use ":" as well
        static constexpr auto             allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except
                                                                                         // slash char

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
            requires(
              !istl::cvref_as<T, basic_path> && istl::cvref_as<typename T::allocator_type, allocator_type>)
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
                stl::size_t const slash_start = path.find(separator);
                stl::size_t const the_size    = stl::min(slash_start, path.size());
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
                stl::size_t const slash_start = path.find(separator);
                stl::size_t const the_size    = stl::min(slash_start, path.size());
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
            auto const lhs_size = this->size();
            auto const rhs_size = rhs.size();
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

        constexpr void normalize(bool const remove_empty_segments = false) {
            remove_dot_segments(is_absolute(), remove_empty_segments);
        }

        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool const remove_leading,
                                           bool const remove_empty_segments = false) {
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
                        auto const last_el = std::prev(pos);
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
                ++pos;
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
