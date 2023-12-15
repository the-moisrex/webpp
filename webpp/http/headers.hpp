// Created by moisrex on 12/23/22.

#ifndef WEBPP_HTTP_HEADERS_COMMON_HPP
#define WEBPP_HTTP_HEADERS_COMMON_HPP

#include "../convert/lexical_cast.hpp"
#include "../std/tuple.hpp"

#include <algorithm>

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
    struct header_field_reference {
        using container_type = HeadersContainerType;
        using field_type     = typename container_type::field_type;
        using name_type      = typename field_type::name_type;
        using value_type     = typename field_type::value_type;

      private:
        container_type* container;
        name_type       m_name;
        value_type      m_value;

      public:
        constexpr header_field_reference(
          container_type& inp_container,
          name_type       inp_name,
          value_type      inp_val)
          : container{&inp_container},
            m_name{stl::move(inp_name)},
            m_value{stl::move(inp_val)} {}

        constexpr header_field_reference(header_field_reference const&)                = default;
        constexpr header_field_reference(header_field_reference&&) noexcept            = default;
        constexpr header_field_reference& operator=(header_field_reference const&)     = default;
        constexpr header_field_reference& operator=(header_field_reference&&) noexcept = default;
        constexpr ~header_field_reference()                                            = default;

        [[nodiscard]] explicit constexpr operator value_type() const noexcept {
            return value;
        }

        [[nodiscard]] constexpr name_type name() const noexcept {
            return m_name;
        }

        [[nodiscard]] constexpr value_type value() const noexcept {
            return m_value;
        }

        template <typename T>
        constexpr header_field_reference& operator=(T&& new_value) {
            m_value = stl::forward<T>(new_value);
            container->set(m_name, m_value);
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
            return container == field.container && m_name == field.m_name && m_value == field.m_value;
        }

        [[nodiscard]] constexpr bool operator!=(header_field_reference const& field) const noexcept {
            return !operator==(field);
        }
    };

    template <typename Container>
    struct headers_container : public Container {
        using container_type = Container;

        using field_type           = typename container_type::field_type;
        using name_type            = typename field_type::name_type;
        using value_type           = typename field_type::value_type;
        using reference_type       = header_field_reference<headers_container>;
        using const_reference_type = header_field_reference<headers_container const>;

        using Container::Container;

        template <HTTPHeadersHolder H>
        explicit constexpr headers_container(H& holder) noexcept(
          stl::is_nothrow_constructible_v<Container, H&>)
          : Container{holder.headers} {}

        template <EnabledTraits ET>
            requires(!HTTPHeadersHolder<ET>)
        explicit constexpr headers_container(ET&& et) noexcept : Container{stl::forward<ET>(et)} {}

        constexpr headers_container(headers_container const&)                = default;
        constexpr headers_container(headers_container&&) noexcept            = default;
        constexpr headers_container& operator=(headers_container&&) noexcept = default;
        constexpr headers_container& operator=(headers_container const&)     = default;
        constexpr ~headers_container()                                       = default;

        /**
         * Get an iterator pointing to the field value that holds the specified header name
         */
        [[nodiscard]] constexpr auto iter(name_type name) const noexcept {
            return stl::find_if(this->begin(), this->end(), [name](field_type const& field) noexcept {
                return field.is_name(name);
            });
        }

        /**
         * Get the field value that holds the specified header name
         */
        [[nodiscard]] constexpr stl::optional<field_type> field(name_type name) const noexcept {
            auto const res = iter(name);
            return res == this->end() ? stl::nullopt : *res;
        }

        /**
         * Get the value of a header
         * Returns an empty string if there are no header with that name
         */
        [[nodiscard]] constexpr value_type get(name_type name) const noexcept {
            auto const res = iter(name);
            return res == this->end() ? value_type{} : res->value;
        }

        /**
         * Get multiple header values as a tuple
         * returns stl::tuple<value_type, value_type, ...> if you give multiple names
         */
        template <typename... NameType>
            requires(sizeof...(NameType) > 1)
        [[nodiscard]] constexpr auto get(NameType&&... name) const noexcept {
            return stl::make_tuple(get(name)...);
        }

        [[nodiscard]] constexpr const_reference_type operator[](name_type name) const noexcept {
            return const_reference_type{*this, name, get(name)};
        }

        [[nodiscard]] constexpr reference_type operator[](name_type name) noexcept {
            return reference_type{*this, name, get(name)};
        }

// This is a C++23 feature
#if __cpp_multidimensional_subscript
        /**
         * Get multiple header values as a tuple
         * This is the same as ".get(...)" member function
         * returns stl::tuple<value_type, value_type, ...> if you give multiple names
         */
        template <typename... NameType>
            requires(sizeof...(NameType) > 1)
        [[nodiscard]] constexpr auto operator[](NameType&&... name) const noexcept {
            return stl::make_tuple(get(name)...);
        }
#endif


        /**
         * Check if the specified names are in headers
         * returns stl::tuple<bool, bool, ...> if you give multiple names
         * or
         * returns bool if you give one name
         */
        template <typename... NameType>
        [[nodiscard]] constexpr auto has(NameType&&... name) const noexcept {
            if constexpr (sizeof...(NameType) == 1) {
                return stl::find(this->begin(), this->end(), name...) != this->end();
            } else if constexpr (sizeof...(NameType) > 1) {
                return stl::make_tuple((stl::find(this->begin(), this->end(), name) != this->end())...);
            } else {
                return true;
            }
        }

        constexpr void set(name_type name, value_type new_value) {
            this->emplace(stl::move(name), stl::move(new_value));
        }

        template <typename VT>
        constexpr void set(name_type name, VT&& new_value) {
            set(stl::move(name),
                lexical::cast<value_type>(stl::forward<VT>(new_value), this->get_allocator()));
        }

        template <typename NT>
        constexpr void set(NT&& name, value_type new_value) {
            set(lexical::cast<name_type>(stl::forward<NT>(name), this->get_allocator()),
                stl::move(new_value));
        }

        template <typename NT, typename VT>
        constexpr void set(NT&& name, VT&& new_value) {
            set(lexical::cast<name_type>(stl::forward<NT>(name), this->get_allocator()),
                lexical::cast<value_type>(stl::forward<VT>(new_value), this->get_allocator()));
        }

        /**
         * Check if the header is empty or not
         */
        [[nodiscard]] constexpr bool empty() const noexcept {
            if constexpr (requires { container_type::empty(); }) {
                return container_type::empty();
            } else {
                return this->begin() == this->end();
            }
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_HEADERS_COMMON_HPP
