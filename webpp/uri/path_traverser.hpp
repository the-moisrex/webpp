// Created by moisrex on 3/7/23.

#ifndef WEBPP_URI_PATH_TRAVERSER_HPP
#define WEBPP_URI_PATH_TRAVERSER_HPP

#include "../std/optional.hpp"
#include "path.hpp"

namespace webpp::uri {


    template <Slug SlugType>
    struct path_traverser {
        using slug_type        = SlugType;
        using path_type        = basic_path<slug_type>;
        using path_iterator    = typename path_type::const_iterator;
        using char_type        = istl::char_type_of_t<slug_type>;
        using string_view_type = stl::basic_string_view<char_type>;
        using slug_ref         = typename path_type::reference;
        using slug_cref        = typename path_type::const_reference;

      private:
        path_type     path;
        path_iterator pos{path.begin()}; // beginning (will be moved)

      public:
        constexpr explicit path_traverser(path_type const& inp_path) : path{inp_path}, pos(path.begin()) {}

        constexpr explicit path_traverser(path_type&& inp_path) : path{stl::move(inp_path)}, pos(path.begin()) {}

        template <istl::StringViewifiable StrT = string_view_type>
        constexpr explicit path_traverser(StrT&& inp_path_str)
          : path{stl::forward<StrT>(inp_path_str)},
            pos(path.begin()) {}

        constexpr path_traverser& operator=(path_type const& inp_path) {
            path = inp_path;
            pos  = path.begin();
            return *this;
        }

        constexpr path_traverser& operator=(path_type&& inp_path) {
            path = stl::move(inp_path);
            pos  = path.begin();
            return *this;
        }

        template <istl::StringViewifiable StrT = string_view_type>
        constexpr path_traverser& operator=(StrT&& inp_path_str) {
            path = stl::forward<StrT>(inp_path_str);
            pos  = path.begin();
            return *this;
        }

        constexpr path_traverser(path_traverser const&)                = default;
        constexpr path_traverser(path_traverser&&) noexcept            = default;
        constexpr path_traverser& operator=(path_traverser&&) noexcept = default;
        constexpr path_traverser& operator=(path_traverser const&)     = default;
        constexpr ~path_traverser()                                    = default;

        /**
         * Get a copy
         * Used for branching the traversal position
         */
        constexpr path_traverser branch() const {
            return {*this};
        }

        constexpr path_type const& get_path() const noexcept {
            return path;
        }

        constexpr void reset() noexcept {
            pos = path.begin();
        }

        constexpr bool next() noexcept {
            if (at_end()) {
                return false;
            }
            ++pos;
            return true;
        }

        [[nodiscard]] constexpr stl::optional<slug_type> peek() const noexcept {
            if (at_end()) {
                return stl::nullopt;
            }
            return *pos;
        }

        constexpr void prev() noexcept {
            --pos;
        }

        [[nodiscard]] constexpr auto operator*() const noexcept {
            return pos.operator*();
        }

        [[nodiscard]] constexpr auto operator->() const noexcept {
            return pos.operator->();
        }

        [[nodiscard]] constexpr slug_type segment() const noexcept {
            return *pos;
        }

        constexpr path_traverser& operator++() noexcept {
            next();
            return *this;
        }

        constexpr path_traverser& operator--() noexcept {
            prev();
            return *this;
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return pos == path.end();
        }

        [[nodiscard]] constexpr bool at_beginning() const noexcept {
            return pos == path.begin();
        }

        /**
         * Check if the specified segment is a match, if it is, increment the segment pointer
         */
        [[nodiscard]] constexpr bool check_segment(slug_type const& slug) noexcept {
            if (!at_end() && *pos == slug) {
                next();
                return true;
            }
            return false;
        }

        template <istl::StringViewifiable StrV = string_view_type>
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            if (!at_end() && *pos == istl::view_of<string_view_type>(stl::forward<StrV>(slug))) {
                next();
                return true;
            }
            return false;
        }
    };

    /**
     * Iterator through a path, and parse while we go through it
     *
     * https://httpwg.org/specs/rfc9110.html#uri.references
     */
    template <istl::CharType CharT = char, Allocator AllocT = stl::allocator<CharT>>
    struct path_iterator {
        using char_type        = CharT;
        using allocator_type   = AllocT;
        using string_type      = stl::basic_string<char_type, stl::char_traits<char_type>, allocator_type>;
        using slug_type        = string_type;
        using string_view_type = stl::basic_string_view<char_type>;
        using iterator         = typename string_view_type::const_iterator;
        using size_type        = typename string_view_type::size_type;

      private:
        static constexpr char_type parent_dir_arr[]  = {char_type{'.'}, char_type{'.'}};
        static constexpr char_type current_dir_arr[] = {char_type{'.'}};

      public:
        static constexpr string_view_type parent_dir  = string_view_type{parent_dir_arr, 2U};
        static constexpr string_view_type current_dir = string_view_type{current_dir_arr, 1U};
        static constexpr auto allowed_chars           = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

      private:
        slug_type        seg;                                                                      // segment
        string_view_type path;
        iterator         beg;
        iterator         pos;
        iterator         fin; // todo: technically it's possible to remove this

      public:
        constexpr explicit path_iterator(string_view_type const inp_path,
                                         allocator_type const&  in_alloc = allocator_type{})
          : seg{in_alloc},
            path{inp_path},
            beg{stl::begin(path)},
            pos{stl::begin(path)},
            fin{stl::end(path)} {}

        constexpr path_iterator& operator=(string_view_type const inp_path) {
            path = inp_path;
            beg  = stl::begin(path);
            pos  = stl::begin(path);
            fin  = stl::end(path);
            seg.clear();
            return *this;
        }

        constexpr path_iterator(path_iterator const&)                = default;
        constexpr path_iterator(path_iterator&&) noexcept            = default;
        constexpr path_iterator& operator=(path_iterator&&) noexcept = default;
        constexpr path_iterator& operator=(path_iterator const&)     = default;
        constexpr ~path_iterator()                                   = default;

        /**
         * Get a copy
         * Used for branching the traversal position
         */
        constexpr path_iterator branch() const {
            return {*this};
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return static_cast<size_type>(fin - pos);
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return pos == fin;
        }

        [[nodiscard]] constexpr bool at_beginning() const noexcept {
            return pos == beg;
        }

        constexpr bool next() {
            if (at_end()) {
                return false;
            }
            auto const slash_start = stl::find(pos, fin, char_type{'/'});
            if (slash_start == pos) { // the first slash, or two or more contiguous slashes
                ++pos;
                return next();
            }
            seg.clear();
            if (!decode_uri_component(string_view_type(pos, slash_start), seg, allowed_chars)) {
                return false;
            }
            pos = slash_start;
            if (pos != fin) {
                ++pos;
            }
            return true;
        }

        constexpr stl::optional<slug_type> peek() const {
            if (at_end()) {
                return stl::nullopt;
            }
            iterator pit = pos;
            iterator slash_start;
            for (;;) {
                slash_start = stl::find(pit, fin, char_type{'/'});
                if (slash_start == pit) { // the first slash, or two or more contiguous slashes
                    ++pit;
                } else {
                    break;
                }
            }
            slug_type slug;
            if (!decode_uri_component(string_view_type(pit, slash_start), slug, allowed_chars)) {
                return stl::nullopt;
            }
            return slug;
        }

        [[nodiscard]] explicit constexpr operator bool() const noexcept {
            return !at_end();
        }

        constexpr path_iterator& operator++() noexcept {
            next();
            return *this;
        }

        [[nodiscard]] constexpr slug_type const& operator*() const noexcept {
            return seg;
        }

        [[nodiscard]] constexpr slug_type const* operator->() const noexcept {
            return *seg;
        }

        [[nodiscard]] constexpr slug_type const& segment() const noexcept {
            return seg;
        }

        constexpr void reset() noexcept {
            pos = beg;
            seg.clear();
        }

        /**
         * Check if the specified segment is a match, if it is, increment the segment pointer
         */
        [[nodiscard]] constexpr bool check_segment(slug_type const& slug) noexcept {
            return next() && *pos == slug;
        }

        template <typename StrV>
            requires(istl::StringViewifiableOf<string_view_type, StrV>)
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            return next() && seg == istl::view_of<string_view_type>(stl::forward<StrV>(slug));
        }
    };

    template <typename CharT, stl::size_t N>
    path_iterator(CharT const (&)[N]) -> path_iterator<CharT>;

    template <typename CharT>
    path_iterator(stl::basic_string_view<CharT>) -> path_iterator<CharT>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_TRAVERSER_HPP
