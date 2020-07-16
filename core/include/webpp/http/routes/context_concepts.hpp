// Created by moisrex on 5/9/20.

#ifndef WEBPP_CONTEXT_CONCEPTS_H
#define WEBPP_CONTEXT_CONCEPTS_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits_concepts.hpp"
#include "../interfaces/basic_interface_concepts.hpp"
#include "../request_concepts.hpp"
#include "../response_concepts.hpp"

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

    template <typename A, extension_method em, typename ContextArgType, typename = void>
    struct has_context_extension_method : stl::false_type {};

    /**
     * pre_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_subroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().pre_subroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};

    /**
     * post_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_subroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_subroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};


    /**
     * pre_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_entryroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().pre_entryroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};

    /**
     * post_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_entryroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_entryroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};

    /**
     * pre first route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_firstroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().pre_firstroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};

    /**
     * post last route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_lastroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_lastroute(stl::declval<ContextArgType&>()), (void)0)>>
      : stl::true_type {};

    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    concept ContextExtension = Extension<T>;

    template <typename T>
    concept Context = Traits<typename T::traits_type>&& Request<typename T::request_type>&&
      stl::is_copy_constructible_v<T>&& stl::is_move_constructible_v<T>&& requires(T c) {
        {c.request};
    };

    struct fake_context_type {
        using traits_type = fake_traits_type;
        using request_type = fake_request_type;
        request_type request{};
        fake_context_type() = default;
        fake_context_type(request_type req) : request{req} {}
        fake_context_type(fake_context_type const&) = default;
        fake_context_type(fake_context_type&&) noexcept = default;
    };

} // namespace webpp

#endif // WEBPP_CONTEXT_CONCEPTS_H
