// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_BODY_HPP
#define WEBPP_REQUEST_BODY_HPP

#include "../common/meta.hpp"
#include "http_concepts.hpp"

namespace webpp::http {


    /**
     * Request body
     *
     * All request bodies will come from here, you can add all the nice features for the request body here.
     */
    template <Traits TraitsType, HTTPRequestBodyCommunicator Communicator>
    struct request_body : public Communicator {
        using traits_type               = TraitsType;
        using request_body_communicator = Communicator; // the way that the Protocol gives us the body
        using char_type                 = traits::char_type<traits_type>;
        using size_type                 = stl::streamsize;

        constexpr request_body(request_body const&)                     = default;
        constexpr request_body(request_body&&) noexcept                 = default;
        constexpr request_body& operator=(request_body&&) noexcept      = default;
        constexpr request_body& operator=(request_body const&) noexcept = default;
        constexpr ~request_body()                                       = default;

        constexpr request_body()
            requires(stl::is_default_constructible_v<request_body_communicator>)
        = default;


        using Communicator::Communicator;

        template <typename T>
        // requires(HTTPGenerallyDeserializableBody<T, request_body>)
        [[nodiscard]] constexpr T as() {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (DeserializableRequestBody<T, request_body>) {
                return deserialize_request_body(stl::type_identity<T>{}, *this);
            } else if constexpr (DeserializableBody<T, request_body>) {
                return deserialize_body(stl::type_identity<T>{}, *this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request body to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        template <typename T>
        // requires(HTTPGenerallyDeserializableBody<T, request_body>)
        [[nodiscard]] constexpr T as() const {
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (DeserializableRequestBody<T, request_body>) {
                return deserialize_request_body(stl::type_identity<T>{}, *this);
            } else if constexpr (DeserializableBody<T, request_body>) {
                return deserialize_body(stl::type_identity<T>{}, *this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request body to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        [[nodiscard]] constexpr auto_converter<request_body> as() const {
            return {*this};
        }

        [[nodiscard]] constexpr auto_converter<request_body> as() {
            return {*this};
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, request_body>)
        [[nodiscard]] explicit constexpr operator T() const {
            return as<T>();
        }

        template <typename T>
            requires(HTTPConvertibleBody<T, request_body>)
        [[nodiscard]] explicit constexpr operator T() {
            return as<T>();
        }

        constexpr void clear() {
            if constexpr (TextBasedBodyWriter<request_body_communicator>) {
                request_body_communicator::clear();
            } else if constexpr (StreamBasedBodyWriter<request_body_communicator>) {
                request_body_communicator::clear(); // clear the state

                // ignore the data in the stream
                request_body_communicator::ignore(std::numeric_limits<std::streamsize>::max());
            } else if constexpr (CStreamBasedBodyWriter<request_body_communicator>) {
                request_body_communicator::clear();
            } else {
                static_assert_false(request_body_communicator,
                                    "We're not able to clear the content of the request; "
                                    "the 'request body communicator' type doesn't support such feature.");
            }
        }

        template <typename T>
        constexpr request_body& add(T&& obj) {
            if constexpr (SerializableRequestBody<T, request_body>) {
                serialize_request_body(stl::forward<T>(obj), *this);
            } else if constexpr (SerializableBody<T, request_body>) {
                serialize_body(stl::forward<T>(obj), *this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the specified object to a request body."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
            return *this;
        }

        template <typename T>
        constexpr request_body& set(T&& obj) {
            clear();
            add(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr request_body& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr request_body& operator+=(T&& obj) {
            add(stl::forward<T>(obj));
            return *this;
        }
    };


} // namespace webpp::http

#endif // WEBPP_REQUEST_BODY_HPP
