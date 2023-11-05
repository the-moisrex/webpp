// Created by moisrex on 3/7/23.

#ifndef WEBPP_URI_PATH_TRAVERSER_HPP
#define WEBPP_URI_PATH_TRAVERSER_HPP

#include "../std/optional.hpp"
#include "path.hpp"

namespace webpp::uri {


    template <Slug SlugType>
    struct path_traverser {
        using slug_type     = SlugType;
        using path_type     = basic_path<slug_type>;
        using path_iterator = typename path_type::const_iterator;

      private:
        // todo: do we actually need the end? can't we just construct it ourselves?

        path_iterator beg; // beginning
        path_iterator pos; // beginning (will be moved)
        path_iterator fin; // end

      public:
        constexpr explicit path_traverser(path_type const& path) noexcept
          : beg{path.begin()},
            pos{path.begin()},
            fin{path.end()} {}

        constexpr path_traverser& operator=(path_type const& path) noexcept {
            beg = path.begin();
            pos = path.begin();
            fin = path.end();
            return *this;
        }

        constexpr path_traverser(path_traverser const&) noexcept       = default;
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

        constexpr void reset() noexcept {
            pos = beg;
        }

        constexpr void next() noexcept {
            ++pos;
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

        [[nodiscard]] constexpr slug_type const& segment() const noexcept {
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
            return pos == fin;
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

        template <istl::StringViewifiable StrV>
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            if (!at_end() && *pos == istl::string_viewify(stl::forward<StrV>(slug))) {
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
    template <istl::String SlugType = stl::string, istl::StringView StringViewType = stl::string_view>
    struct basic_path_iterator {
        using string_type      = SlugType;
        using slug_type        = string_type;
        using iterator         = typename slug_type::const_iterator;
        using size_type        = typename slug_type::size_type;
        using char_type        = istl::char_type_of_t<string_type>;
        using string_view_type = StringViewType;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

      private:
        slug_type seg; // segment
        iterator  beg;
        iterator  pos;
        iterator  fin; // todo: technically it's possible to remove this

      public:
        constexpr explicit basic_path_iterator(string_type const& path)
          : seg{path.get_allocator()},
            beg{stl::begin(path)},
            pos{stl::begin(path)},
            fin{stl::end(path)} {}

        constexpr basic_path_iterator& operator=(string_type const& path) {
            beg = stl::begin(path);
            pos = stl::begin(path);
            fin = stl::end(path);
            seg.clear();
            return *this;
        }

        constexpr basic_path_iterator(basic_path_iterator const&)                = default;
        constexpr basic_path_iterator(basic_path_iterator&&) noexcept            = default;
        constexpr basic_path_iterator& operator=(basic_path_iterator&&) noexcept = default;
        constexpr basic_path_iterator& operator=(basic_path_iterator const&)     = default;
        constexpr ~basic_path_iterator()                                         = default;

        /**
         * Get a copy
         * Used for branching the traversal position
         */
        constexpr basic_path_iterator branch() const {
            return {*this};
        }

        [[nodiscard]] constexpr size_type size() const noexcept {
            return static_cast<size_type>(fin - pos);
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return pos == fin;
        }

        constexpr bool next() {
            if (at_end()) {
                return false;
            }
            const auto slash_start = stl::find(pos, fin, char_type{'/'});
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

        constexpr basic_path_iterator& operator++() noexcept {
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
            return next() && seg == istl::string_viewify_of<string_view_type>(stl::forward<StrV>(slug));
        }
    };


    template <Traits TraitsType = default_traits>
    using path_iterator =
      basic_path_iterator<traits::general_string<TraitsType>, traits::string_view<TraitsType>>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_TRAVERSER_HPP
