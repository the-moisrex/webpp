// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "../application/application_concepts.hpp"
#include "../extensions/extension.hpp"
#include "../std/optional.hpp"
#include "../traits/std_traits.hpp"
#include "body_concepts.hpp"
#include "header_concepts.hpp"
#include "request_concepts.hpp"

namespace webpp::http {

    namespace details {

        template <typename ResType>
        concept HTTPResponse = requires(ResType res) {
            //            requires ResponseBody<typename ResType::body_type>;
            //            requires ResponseHeader<typename ResType::headers_type>;
            requires Traits<typename ResType::traits_type>;
            {res.body};
            {res.headers};
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

    template <typename T>
    concept HTTPResponseExtension = Extension<T>;

    template <typename E>
    struct is_response_extension_list {
        static constexpr bool value = HTTPResponseExtension<E>;
    };

    template <typename T>
    concept HTTPResponseExtensionList = ExtensionListOf<T, is_response_extension_list>;

} // namespace webpp::http

#endif // WEBPP_RESPONSE_CONCEPTS_H
