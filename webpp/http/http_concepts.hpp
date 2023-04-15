// Created by moisrex on 5/9/20.

#ifndef WEBPP_HTTP_CONCEPTS_HPP
#define WEBPP_HTTP_CONCEPTS_HPP

#include "../application/application_concepts.hpp"
#include "../extensions/extension.hpp"
#include "../std/optional.hpp"
#include "../traits/std_traits.hpp"

namespace webpp::http {



    ////////////////////////////// Headers //////////////////////////////

    /**
     * This concept is what the underlying Protocols expect to see in a response's header from apps.
     */
    template <typename T>
    concept HTTPHeaders = requires(stl::remove_cvref_t<T> h) { typename stl::remove_cvref_t<T>::field_type; };

    template <typename T>
    concept HTTPRequestHeaders =
      HTTPHeaders<T> && requires(stl::remove_cvref_t<T> h) { h["content-length"]; };

    template <typename T>
    concept HTTPResponseHeaders = HTTPHeaders<T> && requires(stl::remove_cvref_t<T> h) {
                                                        { h.string() } -> istl::StringViewifiable;
                                                    };

    template <typename T>
    concept HTTPHeaderField = requires(T f) {
                                  typename T::string_type;
                                  typename T::name_type;
                                  typename T::value_type;
                                  typename T::root_extensions; // for http::headers and http::response
                                  requires requires(typename T::name_type name) {
                                               { f.is_name(name) } -> stl::same_as<bool>;
                                           };
                                  { f.name } -> istl::StringViewifiable;
                                  { f.value } -> istl::StringViewifiable;
                              };


    /**
     * @brief The classes that implement this concept are "HTTP request header fields provider" means they
     * provide header fields to the other part of the HTTP header system.
     * The thing about these classes are that they might not own the header fields they're selling.
     */
    template <typename T>
    concept HTTPRequestHeaderFieldsProvider = requires(T obj) {
                                                  obj.begin();
                                                  obj.end();
                                                  typename T::field_type;
                                                  typename T::name_type;
                                                  typename T::value_type;
                                              };

    /**
     * The class that implements this concept is a http request header fields provider which just provides
     * and owns the http fields. The big thing about this is that it owns what it sells.
     */
    template <typename T>
    concept HTTPRequestHeaderFieldsOwner =
      HTTPRequestHeaderFieldsProvider<T> &&
      requires(T obj, typename T::name_type name, typename T::value_type value) {
          obj.emplace(name, value);

          // an example is implemented in "header_fields_provider" in request_headers.hpp file
          obj.as_view();
      };





    ////////////////////////////// Body //////////////////////////////

    template <typename T>
    concept SizableBody = requires(T body) {
                              { body.size() } -> stl::same_as<stl::size_t>;
                          };

    /**
     * @brief C Stream Based Body Reader
     */
    template <typename T>
    concept CStreamBasedBodyReader =
      requires {
          requires stl::copy_constructible<T>;
          typename T::byte_type;
          requires requires(T communicator, typename T::byte_type * data, stl::streamsize size) {
                       { communicator.read(data, size) } -> stl::same_as<stl::streamsize>;
                       { communicator.empty() } -> stl::same_as<bool>;
                   };
      };

    /**
     * @brief C Stream Based Body Writer
     */
    template <typename T>
    concept CStreamBasedBodyWriter =
      requires {
          requires stl::copy_constructible<T>;
          typename T::byte_type;
          requires requires(T communicator, typename T::byte_type const* data, stl::streamsize size) {
                       { communicator.write(data, size) } -> stl::same_as<stl::streamsize>;
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
                                      requires stl::copy_constructible<T>;
                                      requires SizableBody<T>;
                                      body.data();
                                      { body.empty() } -> stl::same_as<bool>;
                                  };

    /**
     * @brief Text Based Body Writer
     */
    template <typename T>
    concept TextBasedBodyWriter =
      requires(T body) {
          requires stl::copy_constructible<T>;
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
          { body.eof() } -> stl::same_as<bool>;
          typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type;
          requires requires(typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type * data,
                            stl::streamsize count) {
                       { body.readsome(data, count) } -> stl::same_as<stl::streamsize>;
                   };
      };

    /**
     * @brief Stream Based Body Reader
     * Since streams are not copyable but we want the response body to be copyable, we allow pointers to
     * streams as well.
     */
    template <typename T>
    concept StreamBasedBodyWriter =
      requires(istl::remove_shared_ptr_t<stl::remove_pointer_t<T>> body, const void* val) {
          body << val;
          body.ignore(INT_MAX); // clear the content inside the stream
          body.clear();         // clear the state
          typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type;
          requires requires(typename istl::remove_shared_ptr_t<stl::remove_pointer_t<T>>::char_type * data,
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
    concept BodyCommunicatorPrimitives = CStreamBasedBodyCommunicator<stl::remove_cvref_t<T>> ||
                                         TextBasedBodyCommunicator<stl::remove_cvref_t<T>> ||
                                         StreamBasedBodyCommunicator<stl::remove_cvref_t<T>>;


    /**
     * @brief Callback Based Body Communicator (CBBC);
     */
    template <typename T>
    concept CallbackBasedBodyCommunicator = requires(T communicator) {
                                                requires requires {
                                                             // Returns a primitive
                                                             { communicator() } -> BodyCommunicatorPrimitives;
                                                         };
                                            };


    /**
     * @brief Optional Based Body Communicator (OBBC);
     */
    template <typename T>
    concept OptionalBasedBodyCommunicator =
      istl::Optional<T> && requires {
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
    concept HTTPRequestBody = HTTPRequestBodyCommunicator<stl::remove_cvref_t<T>> ||
                              stl::same_as<T, istl::nothing_type> || stl::is_void_v<T>;


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


    namespace details {
        template <typename T, typename BodyType>
        concept DeserializablePrimitive = requires(BodyType obj) {
                                              requires(
                                                requires {
                                                    { deserialize_request_body<T>(obj) } -> stl::same_as<T>;
                                                } ||
                                                requires {
                                                    { deserialize_response_body<T>(obj) } -> stl::same_as<T>;
                                                } ||
                                                requires {
                                                    { deserialize_body<T>(obj) } -> stl::same_as<T>;
                                                });
                                          };

        template <typename T, typename BodyType>
        concept SerializablePrimitive =
          requires(BodyType body, T obj) {
              requires(
                requires { serialize_request_body(obj, body); } ||
                requires { serialize_response_body(obj, body); } || requires { serialize_body(obj, body); });
          };
    } // namespace details

    template <typename T, typename BodyType>
    concept HTTPDeserializableBody =
      details::DeserializablePrimitive<T, stl::remove_cvref_t<BodyType>> ||
      details::DeserializablePrimitive<stl::remove_cvref_t<T>, stl::remove_cvref_t<BodyType>>;

    template <typename T, typename BodyType>
    concept HTTPSerializableBody =
      details::SerializablePrimitive<T, stl::remove_cvref_t<BodyType>> ||
      details::SerializablePrimitive<stl::remove_cvref_t<T>, stl::remove_cvref_t<BodyType>>;


    ////////////////////////////// Request //////////////////////////////


    /**
     * HTTP Request types:
     *
     * Initial Request: some protocols may use copy constructor to make a copy of the initial request object
     * instead of re-constructing the request from scratch each time. With this, it's possible to calculate
     * anything that you need for every request in the request's constructor.
     */
    template <typename T>
    concept HTTPRequest = requires(stl::remove_cvref_t<T> req) {
                              requires EnabledTraits<stl::remove_cvref_t<T>>;
                              typename stl::remove_cvref_t<T>::headers_type;
                              typename stl::remove_cvref_t<T>::body_type;
                              requires HTTPRequestHeaders<typename stl::remove_cvref_t<T>::headers_type>;
                              requires HTTPRequestBody<typename stl::remove_cvref_t<T>::body_type>;
                              req.headers;
                              req.body;
                              req.uri();

                              // so we can make a copy of it (initial request)
                              // requires stl::copy_constructible<stl::remove_cvref_t<T>>;


                              // requires Protocol<typename stl::remove_cvref_t<T>::protocol_type>
                          };



    ////////////////////////////// Response //////////////////////////////

    namespace details {

        template <typename ResType>
        concept HTTPResponse =
          requires(ResType res) {
              typename ResType::body_type;
              typename ResType::headers_type;
              requires HTTPResponseBody<typename ResType::body_type>;
              requires HTTPHeaders<typename ResType::headers_type>;
              res.body;
              res.headers;
              requires stl::same_as<stl::remove_cvref_t<decltype(res.body)>, typename ResType::body_type>;
              requires stl::same_as<stl::remove_cvref_t<decltype(res.headers)>,
                                    typename ResType::headers_type>;
          };

        template <typename T>
        concept good_response_types =
          HTTPResponse<stl::remove_cvref_t<T>> || stl::is_void_v<T> || stl::same_as<T, bool> ||
          stl::is_integral_v<T> || istl::StringViewifiable<T>;

        template <typename T>
        struct is_optional_of_response {
            static constexpr bool value = good_response_types<stl::remove_cvref_t<T>>;
        };
    } // namespace details

    template <typename ResType>
    concept HTTPResponse = details::HTTPResponse<stl::remove_cvref_t<ResType>>;

    template <typename T>
    concept AcceptableAsHTTPResponse =
      details::good_response_types<T> || istl::OptionalOf<details::is_optional_of_response, T>;


    template <typename T>
    concept ConvertibleToResponse = !
    stl::is_same_v<T, bool> && !stl::is_integral_v<T> &&
      (HTTPResponse<T> || istl::StringViewifiable<T> || istl::StringViewifiable<T>);




    ////////////////////////////// Protocols //////////////////////////////

    template <typename App, typename ReqType>
    concept ApplicationAcceptingRequest =
      Application<App> && HTTPRequest<ReqType> && stl::invocable<App, ReqType>;


    /**
     * This is a minimal "Protocol concept" where it's used internally in places like "Requests"
     * where it's already specified in HTTPProtocol's concept and thus make a circular dependency of concepts.
     * In order to avoid that problem, it's a good idea to use this concept for internal use and
     * use HTTPProtocol for the users and other places where the Protocol's type is fully known.
     */
    template <typename T>
    concept HTTPCommunicator =
      requires(T proto) {
          requires EnabledTraits<T>;
          requires HTTPRequestBodyCommunicator<typename T::request_body_communicator>;
          // requires HTTPResponseBodyCommunicator<typename T::response_body_communicator>;
          typename T::root_extensions;
      };

    /**
     * Protocol is a "Protocol Type" based on the information that I said in the "server/server_concepts"
     * file.
     */
    template <typename T>
    concept HTTPProtocol = requires(T proto) {
                               requires HTTPCommunicator<T>;
                               requires HTTPRequest<typename T::request_type>;
                               requires Application<typename T::application_type>;
                               requires ApplicationWrapper<typename T::app_wrapper_type>;
                               { proto.app } -> ApplicationWrapper; // get the app
                               // should be able to pass an app to it as well

                               { proto.is_ssl_available() } -> stl::same_as<bool>;
                               { proto() } -> stl::same_as<int>;
                           };


    struct http_protocol_descriptor {
        template <RootExtensionList REL>
        using extractor_type = typename REL::protocol_extensions;
    };

    // this will apply only the "Mother Extension" and gives you the result of that.
    // this does not apply the child extensions
    template <Traits TraitsType, RootExtensionList REList>
    using apply_protocol_extensions =
      typename REList::template mother_extensie_type<TraitsType, http_protocol_descriptor>;


    //////////////////////////// Default Serializers /////////////////////////////




    template <typename T, typename BodyType>
        requires((stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>) &&
                 (HTTPRequest<BodyType> || HTTPRequestBody<BodyType>) )
    constexpr istl::nothing_type deserialize_request_body(HTTPRequestBody auto const&) noexcept {
        // request body is empty
        return {};
    }

    template <typename T, typename BodyType>
        requires((stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>) &&
                 (HTTPResponse<BodyType> || HTTPResponseBody<BodyType>) )
    constexpr istl::nothing_type deserialize_response_body(BodyType const&) noexcept {
        // request body is empty
        return {};
    }


    template <typename T, typename BodyType>
        requires((stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>) &&
                 (HTTPRequest<BodyType> || HTTPResponse<BodyType> || HTTPBody<BodyType>) )
    constexpr istl::nothing_type deserialize_body(BodyType const&) noexcept {
        // request body is empty
        return {};
    }

    template <typename T>
        requires(stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>)
    constexpr void serialize_request_body(T&&, auto&) noexcept {
        // request body is empty
    }

    template <typename T>
        requires(stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>)
    constexpr void serialize_response_body(T&&, auto&) noexcept {
        // response body is empty
    }

    template <typename T>
        requires(stl::is_void_v<T> || stl::same_as<T, istl::nothing_type>)
    constexpr void serialize_body(T&&, auto&) noexcept {
        // response body is empty
    }



    /**
     * This is designed to enable this syntax in the request and response bodies:
     * @code
     *   std::string body = ctx.request.body.as();
     * @endcode
     */
    template <typename BodyType>
    struct auto_converter {
      private:
        BodyType* obj; // body or request

      public:
        constexpr auto_converter(auto_converter const&) noexcept = default;
        constexpr auto_converter(auto_converter&&) noexcept      = default;
        constexpr auto_converter(BodyType& inp_obj) noexcept : obj{&inp_obj} {}
        constexpr auto_converter(BodyType const& inp_obj) noexcept : obj{&inp_obj} {}
        constexpr auto_converter& operator=(auto_converter const&) noexcept = default;
        constexpr auto_converter& operator=(auto_converter&&) noexcept      = default;
        constexpr ~auto_converter() noexcept                                = default;


        template <typename T>
        explicit(HTTPRequest<T> || HTTPResponse<T>) constexpr operator T() const {
            return obj->template as<stl::remove_cvref_t<T>>();
        }
    };

    /**
     * A helper to get the right value out of the body
     */
    template <typename T, typename ObjT>
        requires(HTTPBody<ObjT> || HTTPResponse<ObjT> || HTTPRequest<ObjT>)
    [[nodiscard]] static constexpr decltype(auto) as(ObjT&& obj) noexcept(noexcept(obj.template as<T>())) {
        return obj.template as<T>();
    }


    template <typename T, typename BodyType, typename... NotThese>
    concept HTTPConvertibleBody = istl::is_specialization_of_v<stl::remove_cvref_t<T>, auto_converter> && !
    istl::part_of<stl::remove_cvref_t<T>, BodyType, NotThese...>&& HTTPDeserializableBody<T, BodyType>;

} // namespace webpp::http

#endif // WEBPP_HTTP_CONCEPTS_HPP
