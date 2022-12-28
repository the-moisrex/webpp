// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../extensions/extension.hpp"
#include "http_concepts.hpp"


namespace webpp::http {

    /**
     * Request body
     *
     * All request bodies will come from here, you can add all the nice features for the request body here.
     */
    template <typename TraitsType, HTTPRequestBodyCommunicator Communicator, typename EList>
    struct request_body : public EList, public Communicator {
        using traits_type               = TraitsType;
        using elist_type                = EList;
        using request_body_communicator = Communicator; // the way that the Protocol gives us the body
        using char_type                 = traits::char_type<traits_type>;
        using size_type                 = stl::streamsize;

        template <typename... Args>
            requires(stl::is_constructible_v<elist_type, Args...>)
        constexpr request_body(Args&&... args) noexcept(stl::is_nothrow_constructible_v<elist_type, Args...>)
          : elist_type{stl::forward<Args>(args)...} {}

        constexpr request_body()
            requires(stl::is_default_constructible_v<elist_type>)
        = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(stl::is_constructible_v<elist_type, ET>)
        constexpr request_body(ET&& et) noexcept(stl::is_nothrow_constructible_v<elist_type, ET>)
          : elist_type{stl::forward<ET>(et)} {}

        template <EnabledTraits ET>
        constexpr request_body([[maybe_unused]] ET&&) noexcept(
          stl::is_nothrow_default_constructible_v<elist_type>)
          : elist_type{} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)


        /**
         * Read the body; this member function should be used by the custom bodies to fill their own bellies
         * with the goodies that the user sends in the request body.
         * This function is intentionally designed like POSIX "read" function.
         */
        constexpr size_type read(char_type* data, size_type count) const {
            return request_body_communicator::read(data, count);
        }

        /**
         * Read all the body and return the read size
         */
        constexpr size_type read(char_type* data) const {
            return request_body_communicator::read(data);
        }


        template <typename T>
        constexpr T as() const {
            if constexpr (requires {
                              { deserialize_request_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_request_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request body to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        template <typename T>
        constexpr operator T() const {
            return as<T>();
        }



        template <typename T>
        constexpr request_body& set(T&& obj) {
            if constexpr (requires { serialize_request_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_request_body<T>(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_body<T>(stl::forward<T>(obj), *this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the specified object to a request body."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
            return *this;
        }

        template <typename T>
            requires(!requires(T obj) { elist_type::operator=(obj); })
        constexpr request_body& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }
    };

    template <typename ServerType>
    struct request_body_descriptor {
        static_assert(HTTPRequestBodyCommunicator<typename ServerType::request_body_communicator>,
                      "The specified ServerType doesn't seem to have a request body communicator; "
                      "the Protocol is not providing the correct types.");

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_body_extensions;

        template <typename RootExtensions, typename TraitsType, typename BEList>
        using mid_level_extensie_type =
          request_body<TraitsType, typename ServerType::request_body_communicator, BEList>;
    };

    template <Traits TraitsType, typename ServerType>
    using simple_request_body =
      typename ServerType::root_extensions::template extensie_type<TraitsType,
                                                                   request_body_descriptor<ServerType>>;


} // namespace webpp::http

#endif // WEBPP_REQUEST_BODY_HPP
