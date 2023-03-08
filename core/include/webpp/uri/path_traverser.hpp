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
        path_type     path;
        path_iterator iterator = path.begin();

      public:
        template <typename... Args>
        constexpr path_traverser(Args&&... args) noexcept(stl::is_nothrow_constructible_v<path_type, Args...>)
          : path{stl::forward<Args>(args)...},
            iterator{path.begin()} {}

        constexpr path_traverser&
        operator=(path_type&& new_path) noexcept(stl::is_nothrow_move_assignable_v<path_type>) {
            path     = stl::move(new_path);
            iterator = path.begin();
            return *this;
        }

        constexpr path_traverser&
        operator=(path_type const& new_path) noexcept(stl::is_nothrow_copy_assignable_v<path_type>) {
            path     = new_path;
            iterator = path.begin();
            return *this;
        }

        constexpr path_traverser(path_traverser const&) noexcept(
          stl::is_nothrow_copy_constructible_v<path_type>) = default;

        constexpr path_traverser(path_traverser&&) noexcept = default;

        constexpr path_traverser& operator=(path_traverser&&) noexcept = default;

        constexpr path_traverser&
        operator=(path_traverser const&) noexcept(stl::is_nothrow_copy_assignable_v<path_type>) = default;

        constexpr ~path_traverser() = default;

        /**
         * Get a copy
         * Used for branching the traversal position
         */
        constexpr path_traverser branch() const {
            return {*this};
        }

        constexpr void next() noexcept {
            ++iterator;
        }

        constexpr void prev() noexcept {
            --iterator;
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
        [[nodiscard]] constexpr bool check_segment(slug_type slug) noexcept {
            if (iterator != path.end() && *iterator == slug) {
                next();
                return true;
            }
            return false;
        }
    };
} // namespace webpp::uri

#endif // WEBPP_URI_PATH_TRAVERSER_HPP
