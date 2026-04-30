// Created by moisrex on 12/23/22.

#ifndef WEBPP_HTTP_HEADERS_COMMON_HPP
#define WEBPP_HTTP_HEADERS_COMMON_HPP

#include "./http_concepts.hpp"

#include <tuple>

namespace webpp::http {

    /**
     * This class is what gets returned when you call operator[] on the headers
     * @code
     *   header_field_reference ref = request.headers["Content-Type"];
     *
     *   // usage:
     *   std::string value = request.headers["Content-Type"];
     *   response.headers["Content-Type"] = "application/json"; // set the content type
     * @endcode
     */
    template <typename HeadersContainerType>
    struct [[nodiscard]] header_field_reference {
        using container_type = HeadersContainerType;

      private:
        static constexpr bool is_const = stl::is_const_v<HeadersContainerType>;
        using provider_ptr             = stl::conditional_t<is_const, istl::nothing_type, HeadersContainerType*>;

        header_id_type                     hid;
        stl::string_view                   m_value;
        [[no_unique_address]] provider_ptr provider;

      public:
        constexpr header_field_reference(
          container_type&        inp_provider,
          header_id_type const   inp_id,
          stl::string_view const inp_val) noexcept
            requires(!is_const)
          : hid{inp_id},
            m_value{inp_val},
            provider{&inp_provider} {}

        constexpr header_field_reference(header_id_type const inp_id, stl::string_view const inp_val) noexcept
            requires(is_const)
          : hid{inp_id},
            m_value{inp_val} {}

        constexpr header_field_reference(header_field_reference const&)                = default;
        constexpr header_field_reference(header_field_reference&&) noexcept            = default;
        constexpr header_field_reference& operator=(header_field_reference const&)     = default;
        constexpr header_field_reference& operator=(header_field_reference&&) noexcept = default;
        constexpr ~header_field_reference()                                            = default;

        [[nodiscard]] explicit constexpr operator header_id_type() const noexcept {
            return hid;
        }

        [[nodiscard]] explicit constexpr operator stl::string_view() const noexcept {
            return m_value;
        }

        [[nodiscard]] constexpr header_id_type id() const noexcept {
            return hid;
        }

        [[nodiscard]] constexpr stl::string_view value() const noexcept {
            return m_value;
        }

        template <typename T>
            requires(!is_const)
        constexpr header_field_reference& operator=(T&& new_value) {
            m_value = stl::forward<T>(new_value);
            provider->set(hid, m_value);
            return *this;
        }

        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, header_field_reference>)
        [[nodiscard]] constexpr bool operator==(T&& val) const noexcept {
            return m_value == stl::forward<T>(val);
        }

        template <typename T>
            requires(!stl::same_as<stl::remove_cvref_t<T>, header_field_reference>)
        [[nodiscard]] constexpr bool operator!=(T&& val) const noexcept {
            return m_value != stl::forward<T>(val);
        }

        [[nodiscard]] constexpr bool operator==(header_field_reference const& field) const noexcept {
            return provider == field.provider && hid == field.hid && m_value == field.m_value;
        }

        [[nodiscard]] constexpr bool operator!=(header_field_reference const& field) const noexcept {
            return !operator==(field);
        }
    };

    /**
     * Headers API
     */
    template <HeadersProvider H>
    struct [[nodiscard]] basic_headers : H {
        using H::H;

        /**
         * Get the field value that holds the specified header name
         */
        [[nodiscard]] constexpr stl::optional<stl::string_view> field(stl::string_view name) const noexcept {
            auto const value = this->get(name);
            return value.empty() ? stl::nullopt : value;
        }

        /**
         * Get multiple header values as a tuple
         * returns stl::tuple<stl::string_view, stl::string_view, ...> if you give multiple names
         */
        template <typename... NameType>
            requires(sizeof...(NameType) > 1)
        [[nodiscard]] constexpr auto get(NameType&&... name) const noexcept {
            return stl::make_tuple(get(name)...);
        }

        [[nodiscard]] constexpr header_field_reference<basic_headers const> operator[](
          header_id_type const hid) const noexcept {
            return {*this, hid, this->get(hid)};
        }

        [[nodiscard]] constexpr header_field_reference<basic_headers> operator[](header_id_type const hid) noexcept {
            return {*this, hid, get(hid)};
        }

        [[nodiscard]] constexpr header_field_reference<basic_headers> operator[](stl::string_view const name) noexcept {
            return operator[](header_id(name));
        }

        [[nodiscard]] constexpr header_field_reference<basic_headers const> operator[](
          stl::string_view const name) const noexcept {
            return operator[](header_id(name));
        }

// This is a C++23 feature
#ifdef __cpp_multidimensional_subscript
        /**
         * Get multiple header values as a tuple
         * This is the same as ".get(...)" member function
         * returns stl::tuple<stl::string_view, stl::string_view, ...> if you give multiple names
         */
        template <typename... NameType>
            requires(sizeof...(NameType) > 1)
        [[nodiscard]] constexpr auto operator[](NameType const&... name) const noexcept {
            return stl::make_tuple(get(name)...);
        }
#endif


        template <typename... NameType>
            requires(sizeof...(NameType) > 1)
        [[nodiscard]] constexpr auto has(NameType const&... name) const noexcept {
            return ((!this->get(name).empty()) && ...);
        }

        /**
         * Check if the specified name is in headers
         */
        [[nodiscard]] constexpr bool has(stl::string_view name) const noexcept {
            return !this->get(name).empty();
        }

        [[nodiscard]] constexpr bool has(header_id_type hid) const noexcept {
            return !this->get(hid).empty();
        }

        /**
         * Check if the header is empty or not
         */
        [[nodiscard]] constexpr bool empty() const noexcept {
            if constexpr (requires { H::empty(); }) {
                return H::empty();
            } else {
                return this->begin() == this->end();
            }
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_COMMON_HPP
