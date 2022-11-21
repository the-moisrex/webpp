// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "../application/application_concepts.hpp"
#include "../extensions/extension.hpp"
#include "../std/optional.hpp"
#include "../traits/std_traits.hpp"
#include "response_body.hpp"

namespace webpp::http {



    ////////////////////////////// Headers //////////////////////////////

    /**
     * This concept is what the underlying Protocols expect to see in a response's header from apps.
     */
    template <typename T>
    concept HTTPHeaders = requires(stl::remove_cvref_t<T> h) {
        typename stl::remove_cvref_t<T>::field_type;
    };

    template <typename T>
    concept HTTPRequestHeaders = HTTPHeaders<T> && requires(stl::remove_cvref_t<T> h) {
        h["content-length"];
    };

    template <typename T>
    concept HTTPResponseHeaders = HTTPHeaders<T> && requires(stl::remove_cvref_t<T> h) {
        { h.str() } -> istl::StringViewifiable;
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
     * @brief The calsses that implement this concept are "HTTP request header fields provider" means they
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
    concept HTTPRequestHeaderFieldsOwner = HTTPRequestHeaderFieldsProvider<T> &&
      requires(T obj, typename T::name_type name, typename T::value_type value) {
        obj.emplace(name, value);

        // an example is implemented in "header_fields_provider" in request_headers.hpp file
        obj.template as_view<default_dynamic_traits>();
    };


    ////////////////////////////// Request //////////////////////////////


    template <typename T>
    concept HTTPRequestBody = requires(stl::remove_cvref_t<T> body) {
        { body.str() } -> istl::StringViewifiable;
    }
    || stl::same_as<T, istl::nothing_type>;


    /**
     * HTTP Request types:
     *
     * Initial Request: some protocols may use copy constructor to make a copy of the initial request object
     * instead of re-constructing the request from scratch each time. With this, it's possible to calculate
     * anything that you need for every request in the request's constructor.
     */
    template <typename T>
    concept HTTPRequest = requires(stl::remove_cvref_t<T> req) {
        typename T::root_extensions;
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

    template <typename T>
    concept HTTPResponseBody = requires(T res) {
        { res.str() } -> istl::StringViewifiable;
    };

    namespace details {

        template <typename ResType>
        concept HTTPResponse = requires(ResType res) {
            requires HTTPResponseBody<typename ResType::body_type>;
            requires HTTPHeaders<typename ResType::headers_type>;
            {res.body};
            {res.headers};
            res.calculate_default_headers();
        };

        template <typename T>
        concept good_response_types = HTTPResponse<stl::remove_cvref_t<T>> || stl::is_void_v<T> ||
          stl::same_as<T, bool> || stl::is_integral_v<T> || istl::StringViewifiable<T>;

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

    template <typename App, typename ReqType>
    concept ApplicationAcceptingRequest = Application<App> && HTTPRequest<ReqType> && requires(App app) {
        requires requires(stl::add_lvalue_reference_t<ReqType> req_ref) {
            { app(req_ref) } -> HTTPResponse;
        } || requires(stl::add_const_t<stl::add_lvalue_reference_t<ReqType>> req_cref) {
            { app(req_cref) } -> HTTPResponse;
        } || requires(ReqType req) {
            { app(req) } -> HTTPResponse;
        };
    };


    template <typename T>
    concept ConvertibleToResponse =
      !stl::is_same_v<T, bool> && !stl::is_integral_v<T> &&
      (HTTPResponse<T> || istl::StringViewifiable<T> || istl::StringViewifiable<T>);

    template <typename ResponseType, typename T>
    concept ConstructibleWithResponse = stl::is_constructible_v<ResponseType, T>;

    template <typename ResponseBodyType, typename T>
    concept ConstructibleWithResponseBody = stl::is_constructible_v<ResponseBodyType, T>;





    ////////////////////////////// Protocols //////////////////////////////


    /**
     * Protocol is a "Protocol Type" based on the information that I said in the "server/server_concepts"
     * file.
     */
    template <typename T>
    concept HTTPProtocol = requires(T proto) {
        requires EnabledTraits<T>;
        requires HTTPRequest<typename T::request_type>;
        requires Application<typename T::application_type>;
        requires ApplicationWrapper<typename T::app_wrapper_type>;
        { proto.app } -> ApplicationWrapper; // get the app
        // should be able to pass an app to it as well

        { proto.is_ssl_available() } -> stl::same_as<bool>;
        { proto() } -> stl::same_as<int>;
    };

    // todo
    template <typename T>
    concept HTTPProtocolExtensionList = ExtensionList<T>;


    struct http_protocol_descriptor {
        template <RootExtensionList REL>
        using extractor_type = typename REL::protocol_extensions;
    };

    // this will apply only the "Mother Extension" and gives you the result of that.
    // this does not apply the child extensions
    template <Traits TraitsType, RootExtensionList REList>
    using apply_protocol_extensions =
      typename REList::template mother_extensie_type<TraitsType, http_protocol_descriptor>;


} // namespace webpp::http

#endif // WEBPP_RESPONSE_CONCEPTS_H
