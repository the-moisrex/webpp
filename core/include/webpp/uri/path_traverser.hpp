// Created by moisrex on 3/7/23.

#ifndef WEBPP_URI_PATH_TRAVERSER_HPP
#define WEBPP_URI_PATH_TRAVERSER_HPP

#include "path.hpp"

namespace webpp::uri {


    template <Slug SlugType>
    struct path_traverser {
        using slug_type     = SlugType;
        using path_type     = basic_path<slug_type>;
        using path_iterator = typename path_type::iterator;

      private:
        // todo: do we actually need the end? can't we just construct it ourselves?

        path_iterator it;  // beginning (will be moved)
        path_iterator fin; // end

      public:
        constexpr path_traverser(path_type const& path) noexcept : it{path.begin()}, fin{path.end()} {}

        constexpr path_traverser& operator=(path_type const& path) noexcept {
            it  = path.begin();
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

        constexpr void next() noexcept {
            ++it;
        }

        constexpr void prev() noexcept {
            --it;
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
            return it == fin;
        }

        /**
         * Check if the specified segment is a match, if it is, increment the segment pointer
         */
        [[nodiscard]] constexpr bool check_segment(slug_type const& slug) noexcept {
            if (!at_end() && *it == slug) {
                next();
                return true;
            }
            return false;
        }

        template <istl::StringViewifiable StrV>
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            if (!at_end() && *it == istl::string_viewify(stl::forward<StrV>(slug))) {
                next();
                return true;
            }
            return false;
        }
    };





    /**
     * Iterator through a path, and parse while we go through it
     */
    template <istl::String SlugType = stl::string, istl::StringView StringViewType = stl::string_view>
    struct basic_path_iterator {
        using string_type      = SlugType;
        using slug_type        = string_type;
        using iterator         = typename slug_type::iterator;
        using size_type        = typename slug_type::size_type;
        using char_type        = istl::char_type_of<string_type>;
        using string_view_type = StringViewType;

        static constexpr string_view_type parent_dir  = "..";
        static constexpr string_view_type current_dir = ".";
        static constexpr auto allowed_chars = details::PCHAR_NOT_PCT_ENCODED<char_type>; // except slash char

      private:
        slug_type seg; // segment
        iterator  it;
        iterator  fin; // todo: technically it's possible to remove this

      public:
        constexpr basic_path_iterator(string_type const& path)
          : seg{path.get_allocator()},
            it{seg.begin()},
            fin{seg.end()} {}

        constexpr basic_path_iterator& operator=(string_type const& path) {
            it  = path.begin();
            fin = path.end();
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
            return static_cast<size_type>(fin - it);
        }

        constexpr bool next() {
            if (at_end()) {
                return false;
            }
            const auto slash_start = stl::find(it, fin, char_type{'/'});
            if (!decode_uri_component(string_view_type(it, slash_start), seg, allowed_chars)) {
                // error: invalid string passed as a path
                seg.clear();
                return false;
            }
            if (slash_start == fin) {
                seg.clear();
                it = fin;
            } else {
                it = slash_start;
                ++it;
            }
            return true;
        }

        constexpr operator bool() const noexcept {
            return !at_end();
        }

        constexpr basic_path_iterator& operator++() noexcept {
            next();
            return *this;
        }

        [[nodiscard]] constexpr bool at_end() const noexcept {
            return it == fin;
        }

        /**
         * Check if the specified segment is a match, if it is, increment the segment pointer
         */
        [[nodiscard]] constexpr bool check_segment(slug_type const& slug) noexcept {
            return next() && *it == slug;
        }

        template <typename StrV>
            requires(istl::StringViewifiableOf<string_view_type, StrV>)
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            auto const slug_str = istl::string_viewify_of<string_view_type>(stl::forward<StrV>(slug));
            return next() && size() == slug_str.size() && stl::equal(it, fin, slug_str.data());
        }
    };


    template <Traits TraitsType = default_traits>
    using path_iterator =
      basic_path_iterator<traits::general_string<TraitsType>, traits::string_view<TraitsType>>;

} // namespace webpp::uri

#endif // WEBPP_URI_PATH_TRAVERSER_HPP
