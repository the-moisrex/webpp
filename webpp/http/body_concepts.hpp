// Created by moisrex on 7/14/23.

#ifndef WEBPP_HTTP_BODY_CONCEPTS_HPP
#define WEBPP_HTTP_BODY_CONCEPTS_HPP

#include "../std/tag_invoke.hpp"
#include "../std/type_traits.hpp"

namespace webpp::http {


    template <typename T>
    concept SizableBody = requires(T body) {
        {
            body.size()
        } -> stl::same_as<stl::size_t>;
    };

    /**
     * @brief C Stream Based Body Reader
     */
    template <typename T>
    concept CStreamBasedBodyReader = requires {
        // requires stl::copy_constructible<T>;
        typename T::byte_type;
        requires requires(T communicator, typename T::byte_type* data, stl::streamsize size) {
            {
                communicator.read(data, size)
            } -> stl::same_as<stl::streamsize>;
            {
                communicator.empty()
            } -> stl::same_as<bool>;
        };
    };

    /**
     * @brief C Stream Based Body Writer
     */
    template <typename T>
    concept CStreamBasedBodyWriter = requires {
        // requires stl::copy_constructible<T>;
        typename T::byte_type;
        requires requires(T communicator, typename T::byte_type const* data, stl::streamsize size) {
            {
                communicator.write(data, size)
            } -> stl::same_as<stl::streamsize>;
            communicator.seek(size);
            communicator.clear();
        };
    };

    /**
     * @brief C Stream Based Body Communicator (CBBC);
     *
     * The c-steram based body communicator is the way that the framework internals talk to the developers.
     */
    template <typename T>
    concept CStreamBasedBodyCommunicator = CStreamBasedBodyReader<T> && CStreamBasedBodyWriter<T>;

    /**
     * @brief Text Based Body Reader
     */
    template <typename T>
    concept TextBasedBodyReader = requires(T body) {
        // requires stl::copy_constructible<T>;
        requires SizableBody<T>;
        body.data();
        {
            body.empty()
        } -> stl::same_as<bool>;
    };

    /**
     * @brief Text Based Body Writer
     */
    template <typename T>
    concept TextBasedBodyWriter = requires(T body) {
        // requires stl::copy_constructible<T>;
        typename T::value_type;
        requires requires(typename T::value_type const* data, stl::size_t count) {
            body.append(data, count); // Append a string
            body.clear();
        };
    };

    /**
     * @brief Text Based Body Communicator (TBBC);
     *
     * The text based body communicator is the way that the framework internals talk to the developers.
     * This means that the way the body is stored is by the means of storing a "string"; thus the whole
     * body is available in the string itself (unless a special kind of string is used).
     */
    template <typename T>
    concept TextBasedBodyCommunicator = TextBasedBodyReader<T> && TextBasedBodyWriter<T>;



    /**
     * @brief Stream Based Body Reader
     * Since streams are not copyable but we want the response body to be copyable, we allow pointers to
     * streams as well.
     */
    template <typename T>
    concept StreamBasedBodyReader =
      requires(istl::remove_shared_ptr_t<stl::remove_pointer_t<T>> body, void*& val) {
          body >> val;
          body.rdbuf();
          body.tellg();
          body.seekg(0);
          {
              body.eof()
          } -> stl::same_as<bool>;
          typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type;
          requires requires(typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type* data,
                            stl::streamsize                                                          count) {
              {
                  body.readsome(data, count)
              } -> stl::same_as<stl::streamsize>;
          };
      };

    /**
     * @brief Stream Based Body Reader
     * Since streams are not copyable but we want the response body to be copyable, we allow pointers to
     * streams as well.
     */
    template <typename T>
    concept StreamBasedBodyWriter =
      requires(istl::remove_shared_ptr_t<stl::remove_pointer_t<T>> body, void const* val) {
          body << val;
          body.ignore(INT_MAX); // clear the content inside the stream
          body.clear();         // clear the state
          typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type;
          requires requires(typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type* data,
                            stl::streamsize count) { body.write(data, count); };
      };


    /**
     * @brief Stream Based Body Communicator (SBBC);
     *
     * The stream based body communicator is the way that the framework internals talk to the developers.
     * This means that the way the body is stored is by the means of storing a "stream"; thus the whole
     * body is not available at one point of time unless buffers are used.
     */
    template <typename T>
    concept StreamBasedBodyCommunicator = StreamBasedBodyReader<T> && StreamBasedBodyWriter<T>;


    /**
     * This is the primitives that the Protocols should understand and can read.
     * Other BodyCommunicators can be derived from these
     */
    template <typename T>
    concept BodyCommunicatorPrimitives =
      CStreamBasedBodyCommunicator<stl::remove_cvref_t<T>> ||
      TextBasedBodyCommunicator<stl::remove_cvref_t<T>> ||
      StreamBasedBodyCommunicator<stl::remove_cvref_t<T>>;


    /**
     * @brief Callback Based Body Communicator (CBBC);
     */
    template <typename T>
    concept CallbackBasedBodyCommunicator = requires(T communicator) {
        requires requires {
            // Returns a primitive
            {
                communicator()
            } -> BodyCommunicatorPrimitives;
        };
    };


    /**
     * @brief Optional Based Body Communicator (OBBC);
     */
    template <typename T>
    concept OptionalBasedBodyCommunicator = istl::Optional<T> && requires {
        typename T::value_type;
        requires BodyCommunicatorPrimitives<typename T::value_type> ||
                   CallbackBasedBodyCommunicator<typename T::value_type>;
    };


    /**
     * The whole idea of Body Communicator is represented here;
     * Body Communicator is the way the developers will communicate with the underlying
     * Protocol(CGI/FCGI/...) from the body of the request/response point of view.
     *
     * The final response that the Protocol gets requires the response' body to be a BodyCommunicator
     */
    template <typename T>
    concept BodyCommunicator =
      OptionalBasedBodyCommunicator<stl::remove_cvref_t<T>> ||
      CallbackBasedBodyCommunicator<stl::remove_cvref_t<T>> || BodyCommunicatorPrimitives<T>;


    template <typename T>
    concept BodyReader = CStreamBasedBodyReader<T> || TextBasedBodyReader<T> || StreamBasedBodyReader<T>;

    template <typename T>
    concept BodyWriter = CStreamBasedBodyWriter<T> || TextBasedBodyWriter<T> || StreamBasedBodyWriter<T>;

    /**
     * Request body only need read
     *
     * In order to write to it, the Protocol has to invent its own way (and that's because of performance);
     * It's Protocol-Specific anyway so the protocol is providing this type so it has control over it.
     */
    template <typename T>
    concept HTTPRequestBodyCommunicator = BodyReader<stl::remove_cvref_t<T>>;

    /**
     * Response body communicator requires both read and write.
     * The user Writes; and the Protocol, Reads.
     */
    template <typename T>
    concept HTTPResponseBodyCommunicator = BodyCommunicator<T>;


    /**
     * Request body:
     *   - Can be istl::nothing_type or
     *   - Contains Request Body Communicator
     */
    template <typename T>
    concept HTTPRequestBody =
      HTTPRequestBodyCommunicator<stl::remove_cvref_t<T>> || stl::same_as<T, istl::nothing_type> ||
      stl::is_void_v<T>;


    /**
     * Respone Body:
     *   - Contains a body communicator
     */
    template <typename T>
    concept HTTPResponseBody = HTTPResponseBodyCommunicator<T>;

    template <typename T>
    concept HTTPBody = HTTPRequestBody<T> || HTTPResponseBody<T>;

    // The order of this enum matters; the result of std::variant::index might be converted to this enum here
    enum communicator_type {
        nothing    = 0, // contains nothing (monostate)
        text_based = 1,
        cstream_based,
        stream_based
    };

    template <typename T>
    concept UnifiedBodyReader = BodyReader<T> && requires(T body) {
        {
            body.which_communicator()
        } -> stl::same_as<communicator_type>;
    };

    ////////////////////////////// Deserialize //////////////////////////////

    /// Deserialize Body Tag
    inline constexpr struct deserialize_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(BodyType&& body) const
          noexcept(stl::nothrow_tag_invocable<deserialize_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(stl::type_identity<T>, BodyType&& body) const
          noexcept(stl::nothrow_tag_invocable<deserialize_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        // default implementation for void types
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_body_tag, stl::type_identity<void>, auto&&) noexcept {
            return {};
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_body_tag, stl::type_identity<istl::nothing_type>, auto&&) noexcept {
            return {};
        }
    } deserialize_body;

    /// get the type of the deserialized body
    template <typename T, typename BodyType>
    using deserialized_body_type =
      stl::tag_invoke_result_t<deserialize_body_tag, stl::type_identity<T>, BodyType>;

    /// Check if the type T Deserializable from BodyType
    template <typename T, typename BodyType>
    concept DeserializableBody =
      stl::tag_invocable<deserialize_body_tag, stl::type_identity<T>, BodyType> &&
      stl::is_convertible_v<deserialized_body_type<T, BodyType>, T>;

    /// Deserialize Request Body
    inline constexpr struct deserialize_request_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_request_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(BodyType&& body) const noexcept(
          stl::nothrow_tag_invocable<deserialize_request_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_request_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(stl::type_identity<T>, BodyType&& body) const noexcept(
          stl::nothrow_tag_invocable<deserialize_request_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        // default implementation for void types
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_request_body_tag, stl::type_identity<void>, auto&&) noexcept {
            return {};
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_request_body_tag, stl::type_identity<istl::nothing_type>, auto&&) noexcept {
            return {};
        }
    } deserialize_request_body;

    /// get the type of the deserialized request body
    template <typename T, typename BodyType>
    using deserialized_request_body_type =
      stl::tag_invoke_result_t<deserialize_request_body_tag, stl::type_identity<T>, BodyType>;

    /// Check if the type T Deserializable from BodyType
    template <typename T, typename BodyType>
    concept DeserializableRequestBody =
      stl::tag_invocable<deserialize_request_body_tag, stl::type_identity<T>, BodyType> &&
      stl::is_convertible_v<deserialized_request_body_type<T, BodyType>, T>;

    /// Deserialize Response Body
    inline constexpr struct deserialize_response_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_response_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(BodyType&& body) const noexcept(
          stl::nothrow_tag_invocable<deserialize_response_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        template <typename T, typename BodyType>
            requires stl::tag_invocable<deserialize_response_body_tag, stl::type_identity<T>, BodyType>
        [[nodiscard]] constexpr T operator()(stl::type_identity<T>, BodyType&& body) const noexcept(
          stl::nothrow_tag_invocable<deserialize_response_body_tag, stl::type_identity<T>, BodyType>) {
            return stl::tag_invoke(*this, stl::type_identity<T>{}, stl::forward<BodyType>(body));
        }

        // default implementation for void types
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_response_body_tag, stl::type_identity<void>, auto&&) noexcept {
            return {};
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(deserialize_response_body_tag, stl::type_identity<istl::nothing_type>, auto&&) noexcept {
            return {};
        }
    } deserialize_response_body;

    /// get the type of the deserialized response body
    template <typename T, typename BodyType>
    using deserialized_response_body_type =
      stl::tag_invoke_result_t<deserialize_response_body_tag, stl::type_identity<T>, BodyType>;

    /// Check if the type T Deserializable from BodyType
    template <typename T, typename BodyType>
    concept DeserializableResponseBody =
      stl::tag_invocable<deserialize_response_body_tag, stl::type_identity<T>, BodyType> &&
      stl::is_convertible_v<deserialized_response_body_type<T, BodyType>, T>;

    ////////////////////////////// Serialize //////////////////////////////


    /// Serialize Body
    inline constexpr struct serialize_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<serialize_body_tag, T, stl::add_lvalue_reference_t<BodyType>>
        constexpr void operator()(T&& val, BodyType& body) const
          noexcept(stl::nothrow_tag_invocable<serialize_body_tag, T, stl::add_lvalue_reference_t<BodyType>>) {
            stl::tag_invoke(*this, stl::forward<T>(val), body);
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(serialize_body_tag, istl::nothing_type, auto&) noexcept {
            return {};
        }

    } serialize_body;

    /// Check if the type T Serializable from BodyType
    template <typename T, typename BodyType>
    concept SerializableBody =
      stl::tag_invocable<serialize_body_tag, T, stl::add_lvalue_reference_t<BodyType>>;

    /// Serialize Request Body
    inline constexpr struct serialize_request_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<serialize_request_body_tag, T, stl::add_lvalue_reference_t<BodyType>>
        constexpr void operator()(T&& val, BodyType& body) const noexcept(
          stl::nothrow_tag_invocable<serialize_request_body_tag, T, stl::add_lvalue_reference_t<BodyType>>) {
            stl::tag_invoke(*this, stl::forward<T>(val), body);
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(serialize_request_body_tag, istl::nothing_type, auto&) noexcept {
            return {};
        }

    } serialize_request_body;

    /// Check if the type T Serializable from BodyType
    template <typename T, typename BodyType>
    concept SerializableRequestBody =
      stl::tag_invocable<serialize_request_body_tag, T, stl::add_lvalue_reference_t<BodyType>>;

    /// Serialize Response Body
    inline constexpr struct serialize_response_body_tag {
        // Customization Point
        template <typename T, typename BodyType>
            requires stl::tag_invocable<serialize_response_body_tag, T, stl::add_lvalue_reference_t<BodyType>>
        constexpr void operator()(T&& val, BodyType& body) const noexcept(
          stl::nothrow_tag_invocable<serialize_response_body_tag, T, stl::add_lvalue_reference_t<BodyType>>) {
            stl::tag_invoke(*this, stl::forward<T>(val), body);
        }

        // default implementation for nothing type
        [[nodiscard]] friend constexpr istl::nothing_type
        tag_invoke(serialize_response_body_tag, istl::nothing_type, auto&) noexcept {
            return {};
        }

    } serialize_response_body;

    /// Check if the type T Serializable from BodyType
    template <typename T, typename BodyType>
    concept SerializableResponseBody =
      stl::tag_invocable<serialize_response_body_tag, T, stl::add_lvalue_reference_t<BodyType>>;

    ////////////////////////////// General (De)Serialize //////////////////////////////

    namespace details {
        template <typename T, typename BodyType>
        concept DeserializablePrimitive =
          DeserializableRequestBody<T, BodyType> || DeserializableResponseBody<T, BodyType> ||
          DeserializableBody<T, BodyType>;

        template <typename T, typename BodyType>
        concept SerializablePrimitive =
          SerializableRequestBody<T, BodyType> || SerializableResponseBody<T, BodyType> ||
          SerializableBody<T, BodyType>;
    } // namespace details

    /// Check if it's possible to deserialize the BodyType to T in any way (whether or not it's a request or a
    /// response)
    template <typename T, typename BodyType>
    concept HTTPGenerallyDeserializableBody =
      details::DeserializablePrimitive<T, stl::remove_cvref_t<BodyType>> ||
      details::DeserializablePrimitive<stl::remove_cvref_t<T>, stl::remove_cvref_t<BodyType>>;

    /// Check if it's possible to Serialize T to the BodyType in any way (whether or not it's a request or a
    /// response)
    template <typename T, typename BodyType>
    concept HTTPGenerallySerializableBody =
      details::SerializablePrimitive<T, stl::remove_cvref_t<BodyType>> ||
      details::SerializablePrimitive<stl::remove_cvref_t<T>, stl::remove_cvref_t<BodyType>>;


} // namespace webpp::http

#endif // WEBPP_HTTP_BODY_CONCEPTS_HPP
