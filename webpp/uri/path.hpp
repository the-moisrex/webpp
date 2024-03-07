// Created by moisrex on 11/4/20.

#ifndef WEBPP_URI_PATH_HPP
#define WEBPP_URI_PATH_HPP

#include "../memory/allocators.hpp"
#include "../std/collection.hpp"
#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../strings/peek.hpp"
#include "./details/uri_helpers.hpp"
#include "details/constants.hpp"
#include "details/special_schemes.hpp"
#include "details/uri_components_encoding.hpp"
#include "details/windows_drive_letter.hpp"
#include "encoding.hpp"

#include <bit>
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


        static constexpr auto a_byte = static_cast<stl::uint8_t>(stl::numeric_limits<stl::uint8_t>::digits);
        static constexpr auto slash_mask =
          static_cast<stl::uint64_t>(stl::numeric_limits<stl::uint8_t>::max());

        /// Handle special cases:
        ///   /.
        ///   /..
        ///   /%2e
        ///   /%2e%2e
        ///   /.%2e
        ///   /%2e.
        ///
        /// %2E or %2e is equal to a "." (dot)
        ///
        /// It's possible to have newlines and tabs in between these things
        /// @returns true if we found one or two dots
        template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
        [[nodiscard]] static constexpr bool handle_dots_in_paths(
          component_encoder<components::path, CtxT>& encoder,
          stl::uint64_t&                             slash_loc_cache) noexcept(CtxT::is_nothrow) {
            using ctx_type        = CtxT;
            using char_type       = typename ctx_type::char_type;
            using iterator        = typename ctx_type::iterator;
            using difference_type = typename stl::iterator_traits<iterator>::difference_type;

            // NOLINTBEGIN(*-magic-numbers)
            auto& ctx    = encoder.context();
            auto  pos    = ctx.pos + 1;
            auto  status = ctx.status;

            stl::uint8_t state = 0;
            char_type    prev  = 0;

            for (;; ++pos) {
                if (pos == ctx.end) {
                    break;
                }

                switch (*pos) {
                    case '%':
                        if (prev != 0) {
                            return false;
                        }
                        prev = '%';
                        continue;
                    case '2':
                        if (prev != '%') {
                            return false;
                        }
                        prev = '2';
                        continue;
                    case 'e':
                    case 'E':
                        if (prev != '2') {
                            return false;
                        }
                        prev = 0;
                        ++state;
                        continue;
                    case '.':
                        ++state;
                        continue;
                    [[unlikely]] case '\0' : {
                        if constexpr (Options.eof_is_valid) {
                            break;
                        } else {
                            return false;
                        }
                    }
                    [[unlikely]] case '\\':
                        if (!is_special_scheme(ctx.scheme)) {
                            return false;
                        }
                        set_warning(status, uri_status::reverse_solidus_used);
                        [[fallthrough]];
                    case '#':
                    case '?':
                    case '/':
                        break;
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(status, uri_status::invalid_character);
                            continue;
                        }
                        [[fallthrough]];
                        [[likely]] default : {
                            return false;
                        }
                }
                break;
            }

            switch (state) {
                // single dot found:
                case 0b0000'0001U: // .
                    ctx.pos    = pos;
                    ctx.status = status;
                    encoder.reset_segment_start();
                    return true;

                // two dots found:
                case 0b0000'0010U: // ..
                    break;
                default: return false;
            }
            // NOLINTEND(*-magic-numbers)


            // remove the last segment as well
            if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                auto slash_loc = static_cast<difference_type>(slash_loc_cache & slash_mask);

                // the cache is empty, too many /../../../.. in the URL that can't be stored in one single
                // uint64, or the difference is larger than 255 characters.
                if (slash_loc == 0) {
                    // find the last slash
                    auto const beg = encoder.get_output().begin();
                    auto       cur = beg + encoder.get_output().size();
                    if (cur != beg) {
                        ++slash_loc;
                        --cur;
                    }
                    for (; cur != beg && *cur != '/'; --cur) {
                        ++slash_loc;
                    }
                }
                encoder.pop_back(slash_loc);
                slash_loc_cache >>= a_byte;
            } else {
                encoder.pop_back();
            }
            ctx.status = status;
            ctx.pos    = pos;
            encoder.reset_segment_start();
            return true;
        }

    } // namespace details

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_opaque_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#cannot-be-a-base-url-path-state

        using ctx_type = CtxT;

        // todo: URI Code Points are among interesting characters as well
        webpp_static_constexpr auto interesting_characters =
          details::ascii_bitmap('\0', '%', '#', '?', '\r', '\t', '\n');

        set_opaque(ctx, true);

        details::component_encoder<components::path, ctx_type> encoder(ctx);
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
                [[unlikely]] case '\0':
                    if constexpr (Options.eof_is_valid) {
                        set_valid(ctx.status, uri_status::valid);
                        break;
                    } else {
                        ++ctx.pos;
                        set_warning(ctx.status, uri_status::invalid_character);
                        continue;
                    }
                case '?':
                    clear<components::queries>(ctx);
                    set_valid(ctx.status, uri_status::valid_queries);
                    break;
                case '#':
                    clear<components::fragment>(ctx);
                    set_valid(ctx.status, uri_status::valid_fragment);
                    break;
                case '%':
                    if (encoder.template validate_percent_encode<Options.ignore_tabs_or_newlines>()) {
                        continue;
                    }
                    [[fallthrough]];
                [[unlikely]] case '\r':
                [[unlikely]] case '\n':
                [[unlikely]] case '\t':
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

    template <uri_parsing_options Options = uri_parsing_options{}, ParsingURIContext CtxT>
    static constexpr void parse_path(CtxT& ctx) noexcept(CtxT::is_nothrow) {
        // https://url.spec.whatwg.org/#path-state

        using details::ascii_bitmap;

        using ctx_type = CtxT;


        webpp_static_constexpr auto encode_set =
          ctx_type::is_modifiable || ctx_type::is_segregated ? details::PATH_ENCODE_SET : ascii_bitmap();

        webpp_static_constexpr auto interesting_chars =
          ascii_bitmap(encode_set, ascii_bitmap{'\\', '\0', '/', '?', '#', '%', '\r', '\n', '\t'});

        // attention:
        // we should not check to see if we're at the end of the string because if the path is empty and we're
        // in a special scheme, we have to add "/" to it

        if (!is_special_scheme(ctx.scheme)) {
            parse_opaque_path<Options>(ctx);
            return;
        }

        set_opaque(ctx, false);



        // store the slashes that we find in each byte of this variable if it fits
        stl::uint64_t slash_loc_cache = 0;

        details::component_encoder<components::path, ctx_type> encoder{ctx};
        encoder.start_segment();
        details::handle_windows_driver_letter<Options>(ctx, encoder);
        for (;;) {
            bool is_done = false;
            if (!encoder.template encode_or_validate<uri_encoding_policy::encode_chars>(
                  details::PATH_ENCODE_SET,
                  interesting_chars))
            {
                if (details::handle_dots_in_paths<Options>(encoder, slash_loc_cache)) {
                    continue;
                }

                switch (*ctx.pos) {
                    case '\\':
                        if (is_special_scheme(ctx.scheme)) {
                            set_warning(ctx.status, uri_status::reverse_solidus_used);
                        } else {
                            encoder.skip_separator();
                            continue;
                        }
                        [[fallthrough]];
                    [[likely]] case '/':
                        if constexpr (ctx_type::is_modifiable && !ctx_type::is_segregated) {
                            auto const loc_diff =
                              static_cast<stl::uint64_t>(ctx.pos - encoder.segment_begin());
                            slash_loc_cache <<= details::a_byte;
                            if (loc_diff < details::slash_mask) {
                                slash_loc_cache |= loc_diff;
                            }
                        }
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
                        if (encoder.template validate_percent_encode<Options.ignore_tabs_or_newlines>()) {
                            continue;
                        }
                        set_warning(ctx.status, uri_status::invalid_character);
                        continue;
                    [[unlikely]] case '\r':
                    [[unlikely]] case '\n':
                    [[unlikely]] case '\t':
                        if constexpr (Options.ignore_tabs_or_newlines) {
                            set_warning(ctx.status, uri_status::invalid_character);
                            encoder.ignore_character();
                            continue;
                        } else {
                            set_warning(ctx.status, uri_status::invalid_character);
                            break;
                        }
                    [[unlikely]] case '\0':
                        if constexpr (Options.eof_is_valid) {
                            is_done = true;
                            break;
                        }
                        [[fallthrough]];
                    default: set_warning(ctx.status, uri_status::invalid_character); break;
                }
            } else {
                is_done = true;
            }


            if (!is_done) {
                if (ctx.pos == ctx.end || (Options.eof_is_valid && *ctx.pos == '\0')) {
                    break;
                }
                encoder.next_segment_of('/');
                continue;
            }

            encoder.end_segment();
            break;
        }
        encoder.set_value();

        // ignore the last "?" or "#" character
        if (ctx.pos != ctx.end && (Options.eof_is_valid && *ctx.pos != '\0')) {
            ++ctx.pos;
        } else {
            // handling empty paths
            if constexpr (ctx_type::is_modifiable) {
                if (is_special_scheme(ctx.scheme) && !has_value<components::path>(ctx)) {
                    encoder.next_segment_of('/', 0);
                }
            }

            set_valid(ctx.status, uri_status::valid);
        }
    }

    /// Serialize path
    template <typename StorageType, istl::String StrT>
    static constexpr void render_path(StorageType const& storage, StrT& out, bool const is_opaque = false) {
        // https://url.spec.whatwg.org/#url-serializing
        // https://url.spec.whatwg.org/#url-path-serializer
        if (is_opaque) {
            out += storage.front();
        } else {
            for (auto const& seg : storage) {
                out += '/';
                out += seg;
            }
        }
    }

    /**
     * Including normal string and string view types
     */
    template <typename T>
    concept Slug = istl::StringLike<T>;

    /**
     * @brief Basic Structured URI Path
     * @tparam SlugType The type of each segment of the path to use in the vector
     *                  If the slug type is a string view (and not a string), some of the parsing features
     *                  will be disabled, and that might be a security problem for you. So if the input is
     *                  from an untrusted source, make sure to slug type is a modifiable string.
     */
    template <Slug SlugType = stl::string, typename AllocT = allocator_type_from_t<SlugType>>
    struct basic_path {
        using slug_type           = SlugType;
        using slug_allocator_type = AllocT;
        using allocator_type      = rebind_allocator<slug_allocator_type, slug_type>; // vector's alloc
        using container_type      = stl::vector<slug_type, allocator_type>;
        using value_type          = slug_type;
        using char_type           = istl::char_type_of_t<slug_type>;
        using string_type         = istl::defaulted_string<value_type, allocator_type>;
        using string_view_type    = istl::string_view_type_of<value_type>;
        using path_type           = basic_path;
        using iterator            = typename container_type::iterator;
        using const_iterator      = typename container_type::const_iterator;

        using size_type       = typename container_type::size_type;
        using reference       = typename container_type::reference;
        using const_reference = typename container_type::const_reference;
        using vector_type     = container_type; // used in uri's uri_components

        static constexpr bool is_modifiable = istl::ModifiableString<value_type>;
        static constexpr bool is_segregated = true;
        static constexpr bool is_nothrow    = false;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr string_view_type separator   = "/"; // todo: make sure the user can use ":" as well

        // except slash char
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>;

      private:
        container_type storage;
        bool           m_is_opaque = false;

      public:
        template <uri_parsing_options Options = uri_parsing_options{}, typename Iter = iterator>
        constexpr uri_status_type parse(Iter beg, Iter end) noexcept(is_nothrow) {
            using iterator_type = typename string_view_type::iterator;
            parsing_uri_component_context<components::path, basic_path*, iterator_type> ctx;
            ctx.beg    = beg;
            ctx.end    = end;
            ctx.pos    = beg;
            ctx.out    = this;
            ctx.scheme = scheme_type::special_scheme;
            parse_path<Options>(ctx);
            return ctx.status;
        }

        template <uri_parsing_options Options = uri_parsing_options{}, istl::StringViewifiable StrT>
        constexpr uri_status_type parse(StrT&& inp_str) noexcept(is_nothrow) {
            auto const str = istl::string_viewify(stl::forward<StrT>(inp_str));
            return parse<Options>(str.begin(), str.end());
        }

        template <typename... T>
            requires(stl::is_constructible_v<container_type, T...>)
        explicit constexpr basic_path(T&&... args) : storage{stl::forward<T>(args)...} {}

        // NOLINTBEGIN(*-forwarding-reference-overload)
        template <istl::StringViewifiable T, typename InpAlloc = allocator_type>
            requires(!istl::cvref_as<T, basic_path>)
        explicit constexpr basic_path(T&& str, InpAlloc const& alloc = {}) : storage{alloc} {
            parse(stl::forward<T>(str));
        }

        template <istl::String T>
            requires(
              !istl::cvref_as<T, basic_path> && istl::cvref_as<typename T::allocator_type, allocator_type>)
        explicit constexpr basic_path(T&& str) : container_type{str.get_allocator()} {
            parse(stl::forward<T>(str));
        }

        // NOLINTEND(*-forwarding-reference-overload)

        template <istl::StringViewifiable SegStrT>
        constexpr basic_path& operator/=(SegStrT&& seg_str) {
            parse(stl::forward<SegStrT>(seg_str));
            return *this;
        }

        constexpr basic_path& operator=(value_type str) {
            storage.clear();
            parse(stl::move(str));
            return *this;
        }

        [[nodiscard]] constexpr allocator_type const& get_allocator() const noexcept {
            return storage.get_allocator();
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return storage.size();
        }

        [[nodiscard]] constexpr decltype(auto) begin() const noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() const noexcept {
            return storage.end();
        }

        [[nodiscard]] constexpr decltype(auto) begin() noexcept {
            return storage.begin();
        }

        [[nodiscard]] constexpr decltype(auto) end() noexcept {
            return storage.end();
        }

        template <typename Arg>
        [[nodiscard]] constexpr auto operator[](Arg&& arg) noexcept {
            return storage.operator[](stl::forward<Arg>(arg));
        }

        [[nodiscard]] constexpr stl::partial_ordering operator<=>(basic_path const& rhs) const noexcept {
            auto const lhs_size = storage.size();
            auto const rhs_size = rhs.size();
            if (lhs_size != rhs_size) {
                return stl::compare_partial_order_fallback(lhs_size, rhs_size);
            }
            if (stl::equal(storage.begin(), storage.end(), rhs.begin(), rhs.end())) {
                return stl::partial_ordering::equivalent;
            }
            return stl::partial_ordering::unordered;
        }

        template <istl::StringViewifiable SegStrT>
            requires(!stl::same_as<stl::remove_cvref_t<SegStrT>, basic_path>)
        [[nodiscard]] constexpr auto operator<=>(SegStrT&& rhs) const {
            // todo: optimize this
            auto const      path_str = istl::string_viewify_of<string_view_type>(stl::forward<SegStrT>(rhs));
            path_type const rhs_path{path_str, storage.get_allocator()};
            return *this <=> rhs_path;
        }

        [[nodiscard]] constexpr bool is_absolute() const noexcept {
            return !storage.empty() && storage.front().empty();
        }

        [[nodiscard]] constexpr bool is_relative() const noexcept {
            return !is_absolute();
        }

        constexpr void normalize(bool const remove_empty_segments = false) {
            remove_dot_segments(is_absolute(), remove_empty_segments);
        }

        /**
         * @brief check if we have value
         * @return false if we don't have anything
         */
        [[nodiscard]] constexpr bool has_value() const noexcept {
            return !storage.empty() && !(storage.size() == 1 && storage.front().empty());
        }

        /**
         * @brief Replace the values with the specified raw data, without parsing
         * @param beg start of the value
         * @param end the end of the value
         */
        constexpr void assign(iterator beg, iterator end) {
            storage.clear();
            if constexpr (is_modifiable) {
                istl::emplace_one(storage, beg, end, storage.get_allocator());
            } else {
                istl::emplace_one(storage, beg, end);
            }
        }

        constexpr void set_opaque(bool const value = false) noexcept {
            m_is_opaque = value;
        }

        constexpr void clear() {
            return storage.clear();
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            return storage.empty();
        }

        constexpr void pop_back() noexcept {
            return storage.pop_back();
        }

        /**
         * Remove Dot Segments from https://tools.ietf.org/html/rfc3986#section-5.2.4
         * Refer to uri_normalize_benchmark for more related algorithms of this
         */
        constexpr void remove_dot_segments(bool const remove_leading,
                                           bool const remove_empty_segments = false) {
            if (storage.empty()) {
                return;
            }

            auto pos = storage.begin();

            // handle the first part
            while (pos != storage.end()) {
                if (remove_empty_segments && pos->empty()) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == current_dir) {
                    pos = storage.erase(pos);
                    continue;
                }
                if (*pos == parent_dir) {
                    if (pos != storage.begin()) {
                        auto const last_el = std::prev(pos);
                        if (last_el->empty()) {
                            // remove just this one
                            pos = storage.erase(pos);
                            continue;
                        }
                        if (*last_el != parent_dir) {
                            // remove the previous one and this one
                            pos = storage.erase(last_el, std::next(pos));
                            if (pos == storage.begin()) {
                                return;
                            }
                            --pos;
                            continue;
                        }
                    } else if (remove_leading) {
                        pos = storage.erase(pos);
                        continue;
                    }
                }
                ++pos;
            }
        }

        [[nodiscard]] constexpr bool is_opaque() const noexcept {
            return m_is_opaque;
        }

        template <istl::String NStrT = string_type>
        constexpr void to_string(NStrT& out) const {
            render_path(storage_ref(), out, is_opaque());
        }

        template <istl::String NStrT = string_type, typename... Args>
        [[nodiscard]] constexpr NStrT as_string(Args&&... args) const {
            NStrT out{stl::forward<Args>(args)...};
            to_string(out);
            return out;
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
            }() + storage.size() - 1;
        }

        constexpr void trim() {
            // remove the last empty string
            if (!storage.empty() && storage.back().empty()) {
                stl::ignore(storage.pop_back());
            }
        }

        template <typename... Args>
        decltype(auto) emplace_back(Args&&... args) {
            return storage.emplace_back(stl::forward<Args>(args)...);
        }

        [[nodiscard]] constexpr auto& storage_ref() noexcept {
            return storage;
        }

        [[nodiscard]] constexpr auto const& storage_ref() const noexcept {
            return storage;
        }

        /// Equality check
        /// Attention: this function doesn't parse your input
        template <istl::StringViewifiable NStrT = stl::basic_string_view<char_type>>
        [[nodiscard]] constexpr bool operator==(NStrT&& inp_str) const noexcept {
            auto str = istl::string_viewify(stl::forward<NStrT>(inp_str));
            for (auto const& piece : storage) {
                if (!str.starts_with(piece)) {
                    return false;
                }
                str.remove_prefix(piece.size() + 1);
            }
            return true;
        }

        [[nodiscard]] constexpr bool operator==(basic_path const& other) const noexcept {
            return storage == other.storage_ref();
        }
    };

    template <istl::Stringifiable S>
    basic_path(S&& str) -> basic_path<stl::remove_cvref_t<decltype(istl::stringify(stl::forward<S>(str)))>>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_HPP
