// Created by moisrex on 5/9/20.

#ifndef WEBPP_CONTEXT_CONCEPTS_H
#define WEBPP_CONTEXT_CONCEPTS_H

#include "../../extensions/extension.hpp"
#include "../../traits/traits_concepts.hpp"
#include "../protocols/protocol_concepts.hpp"
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
      stl::void_t<decltype(stl::declval<A>().pre_subroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};

    /**
     * post_subroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_subroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_subroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};


    /**
     * pre_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_entryroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().pre_entryroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};

    /**
     * post_entryroute
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_entryroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_entryroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};

    /**
     * pre first route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::pre_firstroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().pre_firstroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};

    /**
     * post last route
     */
    template <typename A, typename ContextArgType>
    struct has_context_extension_method<
      A, extension_method::post_lastroute, ContextArgType,
      stl::void_t<decltype(stl::declval<A>().post_lastroute(stl::declval<ContextArgType&>()), (void) 0)>>
      : stl::true_type {};

    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    concept ContextExtension = Extension<T>;

    template <typename T>
    concept Context = requires(stl::remove_cvref_t<T> c) {
        Traits<typename stl::remove_cvref_t<T>::traits_type>;
        Request<typename stl::remove_cvref_t<T>::request_type>;
        //        {c.request};
    };

    template <typename T>
    struct ContextTempl {
        static constexpr bool value = Context<T>;
    };

    //    template <typename ...E>
    //    struct fake_context_type_impl: public E... {
    //        using traits_type  = std_traits;
    //        using request_type = fake_request_type;
    //        request_type request{};
    //        fake_context_type_impl() = default;
    //        fake_context_type_impl(request_type req) : request{req} {}
    //        fake_context_type_impl(fake_context_type_impl const&)     = default;
    //        fake_context_type_impl(fake_context_type_impl&&) noexcept = default;
    //
    //        template <Extension... EE>
    //        constexpr auto clone() const noexcept {
    //            return fake_context_type_impl<EE...>{*this};
    //        }
    //    };
    //
    //    using fake_context_type = fake_context_type_impl<>;

} // namespace webpp

#endif // WEBPP_CONTEXT_CONCEPTS_H
