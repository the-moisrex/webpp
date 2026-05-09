#ifndef WEBPP_MIDDLEWARE_CONCEPTS_HPP
#define WEBPP_MIDDLEWARE_CONCEPTS_HPP

#include "../http/http_concepts.hpp"
#include "../traits/dynamic_scoping.hpp"

#include <concepts>

namespace webpp {

    template <typename T>
    concept MiddlewareReturnType =
      stl::same_as<T, bool> // Conditional termination
      || stl::is_void_v<T>  // No return
      || http::HTTPResponse<T>;

    /**
     * Onion Architecture.
     *
     * Call `next()` whenever you need to.
     */
    template <typename T>
    concept OnionMiddleware = stl::invocable<T>;

    /**
     * Two-Way Interceptor Middleware.
     *
     * Has a `.post` and a `.pre` member function.
     */
    template <typename T>
    concept TwoWayMiddleware = requires(T obj) {
        { obj.pre() } -> MiddlewareReturnType;
        { obj.post() } -> MiddlewareReturnType;
    } || requires(T obj) {
        { obj.pre() } -> MiddlewareReturnType;  // only pre
    } || requires(T obj) {
        { obj.post() } -> MiddlewareReturnType; // only post
    };



    template <typename T>
    concept Middleware = OnionMiddleware<T> || TwoWayMiddleware<T>;

    // todo: RouterLevelMiddleware
    // todo: ProtocolLevelMiddleware


    struct [[nodiscard]] close_tag {};

    struct [[nodiscard]] middleware_tag {};

    template <typename Tag, typename T>
    concept EventOf = stl::invocable<T, Tag>;

    struct [[nodiscard]] middleware_node {
        constexpr middleware_node() noexcept                             = default;
        constexpr middleware_node(middleware_node const&)                = default;
        constexpr middleware_node(middleware_node&&) noexcept            = default;
        constexpr middleware_node& operator=(middleware_node const&)     = default;
        constexpr middleware_node& operator=(middleware_node&&) noexcept = default;
        virtual ~middleware_node()                                       = default;

        virtual void operator()(close_tag) = 0;
        virtual void operator()()          = 0;
    };

    static constexpr struct [[nodiscard]] basic_middlewares final {
      private:
        middleware_node* root = nullptr;

      public:
        consteval basic_middlewares() noexcept = default;

        void trigger(close_tag) {
            if (root == nullptr) {
                return;
            }
            root->operator()(close_tag{});
        }

        void trigger() {
            if (root == nullptr) {
                return;
            }
            root->operator()();
        }

    } middlewares;

    template <typename T>
    struct [[nodiscard]] middleware_base : middleware_node {
        template <typename Tag>
        using next_type = stl::conditional_t<EventOf<Tag, T>, middleware_node*, istl::nothing_type>;

      private:
        [[no_unique_address]] next_type<close_tag> _child_close;

      public:
        template <typename Tag = middleware_tag>
        void trigger(Tag = {}) {
            if constexpr (EventOf<Tag, T>) {
                static_cast<T*>(this)->operator()(Tag{});
            }
        }

        void next(close_tag) const {
            if constexpr (EventOf<close_tag, T>) {
                _child_close->operator()(close_tag{});
            }
        }

        void operator()(close_tag) final {
            trigger(close_tag{});
        }

        void operator()() final {
            trigger();
        }
    };

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CONCEPTS_HPP
