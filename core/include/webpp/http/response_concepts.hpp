// Created by moisrex on 5/9/20.

#ifndef WEBPP_RESPONSE_CONCEPTS_H
#define WEBPP_RESPONSE_CONCEPTS_H

#include "./request_concepts.hpp"
#include "../application/application_concepts.hpp"
#include "../extensions/extension.hpp"
#include "../std/optional.hpp"
#include "../traits/std_traits.hpp"
#include "./body_concepts.hpp"
#include "./header_concepts.hpp"

namespace webpp {

    namespace details {

        template <typename ResType>
        concept Response = requires(ResType res) {
            //            requires ResponseBody<typename ResType::body_type>;
            //            requires ResponseHeader<typename ResType::headers_type>;
            requires Traits<typename ResType::traits_type>;
            {res.body};
            {res.headers};
        };

        template <typename T>
        concept good_response_types = Response<stl::remove_cvref_t<T>> || stl::is_void_v<T> ||
          stl::same_as<T, bool> || stl::is_integral_v<T> || istl::StringViewifiable<T>;

        template <typename T>
        struct is_optional_of_response {
            static constexpr bool value = good_response_types<stl::remove_cvref_t<T>>;
        };
    } // namespace details

    template <typename ResType>
    concept Response = details::Response<stl::remove_cvref_t<ResType>>;

    template <typename T>
    concept AcceptableAsResponse =
      details::good_response_types<T> || istl::OptionalOf<details::is_optional_of_response, T>;

    template <typename App, typename ReqType>
    concept ApplicationAcceptingRequest = Application<App> && Request<ReqType> && requires(App app) {
        requires requires(stl::add_lvalue_reference_t<ReqType> req_ref) {
            { app(req_ref) } -> Response;
        } || requires(stl::add_const_t<stl::add_lvalue_reference_t<ReqType>> req_cref) {
            { app(req_cref) } -> Response;
        } || requires(ReqType req) {
            { app(req) } -> Response;
        };
    };


    template <typename T>
    concept ConvertibleToResponse =
      !stl::is_same_v<T, bool> && stl::is_integral_v<T> && (Response<T> || istl::StringViewifiable<T>);

    template <typename ResponseType, typename T>
    concept ConstructibleWithResponse = stl::is_constructible_v<ResponseType, T>;

    template <typename T>
    concept ResponseExtension = Extension<T>;

    template <typename E>
    struct is_response_extension_list {
        static constexpr bool value = ResponseExtension<E>;
    };

    template <typename T>
    concept ResponseExtensionList = ExtensionListOf<T, is_response_extension_list>;

} // namespace webpp

#endif // WEBPP_RESPONSE_CONCEPTS_H
