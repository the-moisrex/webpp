// Created by moisrex on 5/9/20.

#ifndef WEBPP_HTTP_CONCEPTS_HPP
#define WEBPP_HTTP_CONCEPTS_HPP

#include "../application/application_concepts.hpp"
#include "../std/optional.hpp"
#include "../traits/std_traits.hpp"
#include "./body_concepts.hpp"

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
    concept HTTPResponseHeaders =
      HTTPHeaders<T> && requires(stl::remove_cvref_t<T> h, stl::string out) { h.to_string(out); };

    template <typename T>
    concept HTTPHeaderField = requires(T f) {
        typename T::string_type;
        typename T::name_type;
        typename T::value_type;
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

    template <typename T>
    concept HTTPResponseHeaderFieldsProvider = HTTPRequestHeaderFieldsProvider<T>;


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

    template <typename T>
    concept HTTPResponseHeaderFieldsOwner =
      HTTPResponseHeaderFieldsProvider<T> &&
      requires(T obj, typename T::name_type name, typename T::value_type value) {
          obj.emplace(name, value);

          // an example is implemented in "header_fields_provider" in request_headers.hpp file
          obj.as_view();
      };



    template <typename T>
    concept HTTPHeaderFieldsProvider = requires(T provider) {
        provider.begin();
        provider.end();
    };



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
        concept HTTPResponse = requires(ResType res) {
            typename ResType::body_type;
            typename ResType::headers_type;
            requires HTTPResponseBody<typename ResType::body_type>;
            requires HTTPHeaders<typename ResType::headers_type>;
            res.body;
            res.headers;
            requires stl::same_as<stl::remove_cvref_t<decltype(res.body)>, typename ResType::body_type>;
            requires stl::same_as<stl::remove_cvref_t<decltype(res.headers)>, typename ResType::headers_type>;
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
    concept ConvertibleToResponse =
      !stl::is_same_v<T, bool> && !stl::is_integral_v<T> &&
      (HTTPResponse<T> || istl::StringViewifiable<T> || istl::StringViewifiable<T>);



    ////////////////////////////// Request, Response Common //////////////////////////////


    template <typename T>
    concept HTTPHeadersHolder = requires(stl::remove_cvref_t<T> server) {
        { server.headers } -> HTTPHeaders;
    };


    template <typename T>
    concept HTTPBodyHolder = requires(stl::remove_cvref_t<T> server) {
        { server.body } -> HTTPBody;
    };


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
    concept HTTPCommunicator = requires(T proto) {
        requires EnabledTraits<T>;
        requires HTTPRequestBodyCommunicator<typename T::request_body_communicator>;
        // requires HTTPResponseBodyCommunicator<typename T::response_body_communicator>;
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
    concept HTTPConvertibleBody = istl::is_specialization_of_v<stl::remove_cvref_t<T>, auto_converter> &&
                                  !istl::part_of<stl::remove_cvref_t<T>, BodyType, NotThese...> &&
                                  HTTPGenerallyDeserializableBody<T, BodyType>;

} // namespace webpp::http

#endif // WEBPP_HTTP_CONCEPTS_HPP
