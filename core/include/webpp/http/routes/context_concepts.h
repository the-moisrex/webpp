// Created by moisrex on 5/9/20.

#ifndef WEBPP_CONTEXT_CONCEPTS_H
#define WEBPP_CONTEXT_CONCEPTS_H

#include "../../extensions/extension.h"
#include "../../traits/traits_concepts.h"
#include "../interfaces/basic_interface_concepts.h"
#include "../request_concepts.h"
#include "../response_concepts.h"

#include <type_traits>

namespace webpp {

    enum class extension_method : uint_fast8_t {
        pre_subroute,
        post_subroute,
        pre_entryroute,
        post_entryroute,
        pre_firstroute,
        post_lastroute
    };

    template <typename A, extension_method em, typename ContextArgType,
              typename = void>
    struct has_context_extension_method : ::std::false_type {};

    /**
     * pre_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_subroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().pre_subroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};

    /**
     * post_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_subroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().post_subroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};


    /**
     * pre_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_entryroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().pre_entryroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};

    /**
     * post_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_entryroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().post_entryroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};

    /**
     * pre first route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_firstroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().pre_firstroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};

    /**
     * post last route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_lastroute, ContextArgType,
      ::std::void_t<decltype(
        ::std::declval<A>().post_lastroute(::std::declval<ContextArgType&>()),
        (void)0)>> : ::std::true_type {};

    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    concept ContextExtension = Extension<T>;

    template <typename T>
    concept Context = Traits<typename T::traits_type>and
            Interface<typename T::interface_type>and
            Request<typename T::request_type>and
            Response<typename T::response_type>and
            ContextExtension<typename T::extension_type>and ::std::
              is_copy_constructible_v<T>and ::std::is_move_constructible_v<
                T>and requires(T c) {
        { c.response }
        ->Response;
    }
    and requires(T c) {
        { c.request }
        ->Request;
    };


} // namespace webpp

#endif // WEBPP_CONTEXT_CONCEPTS_H
