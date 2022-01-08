// Created by moisrex on 5/9/20.

#ifndef WEBPP_CONTEXT_CONCEPTS_H
#define WEBPP_CONTEXT_CONCEPTS_H

#include "../request_concepts.hpp"
//#include "../response_concepts.hpp"
#include "../../traits/enable_traits.hpp"


namespace webpp::http {

    enum struct extension_method {
        pre_subroute,
        post_subroute,
        pre_entryroute,
        post_entryroute,
        pre_firstroute,
        post_lastroute
    };

    namespace details {
        template <typename A, extension_method em, typename ContextArgType>
        static constexpr bool has_method() noexcept {
            switch (em) {
                case extension_method::pre_subroute:
                    return requires(A obj, ContextArgType & ctx) {
                        obj.pre_subroute(ctx);
                    };
                    case extension_method::post_subroute: return requires(A obj, ContextArgType & ctx) {
                        obj.post_subroute(ctx);
                    };
                    case extension_method::pre_entryroute: return requires(A obj, ContextArgType & ctx) {
                        obj.pre_entryroute(ctx);
                    };
                    case extension_method::post_entryroute: return requires(A obj, ContextArgType & ctx) {
                        obj.post_entryroute(ctx);
                    };
                    case extension_method::pre_firstroute: return requires(A obj, ContextArgType & ctx) {
                        obj.pre_firstroute(ctx);
                    };
                    case extension_method::post_lastroute: return requires(A obj, ContextArgType & ctx) {
                        obj.post_lastroute(ctx);
                    };
                    default: return false;
            }
        }
    } // namespace details

    template <typename A, extension_method em, typename ContextArgType>
    concept has_context_extension_method = details::has_method<A, em, ContextArgType>();


    template <typename T>
    concept ContextExtension = Extension<T>;

    template <typename T>
    concept Context = requires(stl::remove_cvref_t<T> c) {
        requires EnabledTraits<typename stl::remove_cvref_t<T>>;
        requires HTTPRequest<typename stl::remove_cvref_t<T>::request_type>;
        requires HTTPResponse<typename stl::remove_cvref_t<T>::response_type>;
        //        {c.request};
    };


} // namespace webpp::http

#endif // WEBPP_CONTEXT_CONCEPTS_H
