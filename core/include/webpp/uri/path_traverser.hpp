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

        /**
         * Check if the specified segment is a match, if it is, increment the segment pointer
         */
        [[nodiscard]] constexpr bool check_segment(slug_type const& slug) noexcept {
            if (it != fin && *it == slug) {
                next();
                return true;
            }
            return false;
        }

        template <istl::StringViewifiable StrV>
        [[nodiscard]] constexpr bool check_segment(StrV&& slug) noexcept {
            if (it != fin && *it == istl::string_viewify(stl::forward<StrV>(slug))) {
                next();
                return true;
            }
            return false;
        }
    };
} // namespace webpp::uri

#endif // WEBPP_URI_PATH_TRAVERSER_HPP
