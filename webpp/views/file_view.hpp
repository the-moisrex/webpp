// Created by moisrex on 14/04/22.

#ifndef WEBPP_VIEWS_FILE_VIEW_HPP
#define WEBPP_VIEWS_FILE_VIEW_HPP

#include "../std/string.hpp"
#include "view_concepts.hpp"

namespace webpp::views {

    /**
     * File view
     *
     * File view does no rendering or parsing. The exact input is outputted.
     */
    template <Traits TraitsType>
    struct file_view {
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using char_type        = traits::char_type<traits_type>;
        using string_type      = traits::string<traits_type>; // to satisfy View concept

        using data_view_type = istl::nothing_type;
        using data_type      = istl::nothing_type;

      private:
        string_type data;

      public:
        constexpr file_view() = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, file_view>)
        constexpr file_view(ET&& et) : data{general_alloc_for<string_type>(et)} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)

        constexpr file_view(file_view const&)     = default;
        constexpr file_view(file_view&&) noexcept = default;
        constexpr ~file_view() noexcept           = default;

        constexpr file_view& operator=(file_view const&)     = default;
        constexpr file_view& operator=(file_view&&) noexcept = default;

        constexpr data_view_type generate_data_view(auto&&) const noexcept {
            return {};
        }

        [[nodiscard]] constexpr bool has_scheme() const noexcept {
            return !data.empty();
        }

        constexpr void scheme(string_view_type str) {
            data = str;
        }

        constexpr void scheme(string_type&& str) noexcept {
            data.swap(str);
        }

        template <typename... DataType>
        constexpr void render(istl::String auto& out, [[maybe_unused]] DataType&&...) const {
            out.append(data);
        }

        template <typename... DataType>
        constexpr void render(istl::StringView auto& out, [[maybe_unused]] DataType&&...) const noexcept {
            out = data;
        }
    };



} // namespace webpp::views

#endif // WEBPP_VIEWS_FILE_VIEW_HPP
