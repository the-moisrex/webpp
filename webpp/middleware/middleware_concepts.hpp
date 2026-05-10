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

    template <typename T>
    concept EventTag = requires { typename T::node_type; };

    // todo: RouterLevelMiddleware
    // todo: ProtocolLevelMiddleware


    template <typename Tag, typename T>
    concept EventOf = stl::invocable<T, Tag>;

    /**
     * Middleware Node is one node in the Intrusive Linked List part of the middleware tree.
     */
    template <EventTag... EventType>
    struct [[nodiscard]] basic_middleware_node : private EventType::node_type... {
        // the nodes are being inheritted privately in order to allow compilers to optimize more aggresively to make
        // sure multiple inheritance can be optimized to the same level that a flat design can be optimized. Virtual
        // functions are costly, and having them come from multiple inherited base types is even more costly; so we make
        // them private, thus the compiler can be sure that it can't be converted to all those base classes, thus it can
        // optimize it.


        constexpr basic_middleware_node() noexcept                                   = default;
        constexpr basic_middleware_node(basic_middleware_node const&)                = default;
        constexpr basic_middleware_node(basic_middleware_node&&) noexcept            = default;
        constexpr basic_middleware_node& operator=(basic_middleware_node const&)     = default;
        constexpr basic_middleware_node& operator=(basic_middleware_node&&) noexcept = default;
        constexpr ~basic_middleware_node() noexcept override                         = default;

        template <typename... T>
        constexpr decltype(auto) operator()(T&&... args) {
            return this->operator()(stl::forward<T>(args)...);
        }
    };

    template <typename T>
    struct [[nodiscard]] base_node_type {
        using pointer = T*;

        [[nodiscard]] constexpr pointer next_node() const noexcept {
            return _next;
        }

        constexpr void next_node(pointer ptr) noexcept {
            _next = ptr;
        }

      private:
        pointer _next = nullptr;
    };

    /// on connection close event
    struct [[nodiscard]] close_tag {
        struct [[nodiscard]] node_type : base_node_type<node_type> {
            node_type() noexcept                       = default;
            node_type(node_type const&)                = default;
            node_type(node_type&&) noexcept            = default;
            node_type& operator=(node_type const&)     = default;
            node_type& operator=(node_type&&) noexcept = default;
            virtual ~node_type()                       = default;
            virtual void operator()(close_tag)         = 0;
        };
    };

    /// middleware event
    struct [[nodiscard]] middleware_tag {
        struct [[nodiscard]] node_type : base_node_type<node_type> {
            node_type() noexcept                       = default;
            node_type(node_type const&)                = default;
            node_type(node_type&&) noexcept            = default;
            node_type& operator=(node_type const&)     = default;
            node_type& operator=(node_type&&) noexcept = default;
            virtual ~node_type()                       = default;
            virtual void operator()()                  = 0;
        };
    };

    /// Middleware Node is the node part of the tree. This makes the middleware an intrusive linked list.
    using middleware_node = basic_middleware_node<middleware_tag, close_tag>;

    namespace details {
        template <typename MiddlewareType, typename EventType>
        struct node_child {};

        template <typename MiddlewareType, typename EventType>
            requires(EventOf<EventType, MiddlewareType> || stl::same_as<MiddlewareType, middleware_node>)
        struct node_child<MiddlewareType, EventType> {
          protected:
            middleware_node* child = nullptr;
        };

        template <typename MiddlewareType, typename Node = middleware_node>
        struct node_children {};

        /**
         * Only children that are supported by the MiddlewareType will have children
         * This is essentially a conditional tuple
         */
        template <typename MiddlewareType, typename... EventTypes>
        struct node_children<MiddlewareType, basic_middleware_node<EventTypes...>>
          : node_child<MiddlewareType, EventTypes>... {
            template <typename ChildType>
            [[nodiscard]] constexpr ChildType* get_child() const noexcept {
                return static_cast<node_child<MiddlewareType, ChildType> const*>(this)->child;
            }

            template <typename ChildType>
            constexpr void register_child(ChildType* new_child) noexcept {
                // only register it if we support it
                if constexpr (requires { static_cast<node_child<MiddlewareType, ChildType>*>(this); }) {
                    auto& head = static_cast<node_child<MiddlewareType, ChildType>*>(this)->child;
                    if (head == nullptr) {
                        head = new_child;
                    } else {
                        // Traverse the specific event's intrusive linked list
                        // Upcast middleware_node to the specific ChildType::node_type
                        auto* current = static_cast<typename ChildType::node_type*>(head);
                        while (current->next_node != nullptr) {
                            current = current->next_node;
                        }
                        current->next_node = static_cast<typename ChildType::node_type*>(new_child);
                    }
                }
            }

            template <typename MW>
            constexpr void register_middleware(MW* mw_ptr) noexcept {
                middleware_node* node = mw_ptr->get_node();
                (register_child<EventTypes>(node), ...);
            }
        };
    } // namespace details

    /**
     * This is the CRTP base class that turns classes into middlewares.
     */
    template <typename T>
    struct [[nodiscard]] middleware_base : middleware_node {
        /// Root node requires to have all of the children but other nodes are only required to have children that the
        /// node itself supports
        static constexpr bool is_root_node = stl::same_as<T, middleware_node>;

      private:
        [[no_unique_address]] details::node_children<T> children;

      public:
        /// Trigger the event
        template <EventTag Tag = middleware_tag>
        void trigger(Tag = {}) const {
            if constexpr (EventOf<Tag, T>) {
                static_cast<T*>(this)->operator()(Tag{});
            }
        }

        [[nodiscard]] constexpr middleware_node* get_node() noexcept {
            return static_cast<middleware_node*>(this);
        }

        /// Call the next middleware
        template <EventTag Tag>
        decltype(auto) next(Tag) const {
            if constexpr (EventOf<Tag, T>) {
                auto* child = children.template get_child<Tag>();
                assert(child != nullptr);
                return child->operator()(Tag{});
            }
        }

        /// Register a middleware in the tree
        /// Each middleware instance can have multiple events and will be pointed to multiple times.
        template <typename MW>
        void register_middleware(MW* inp_middleware) noexcept {
            children.template register_middleware<MW>(inp_middleware);
        }
    };

    /// Special case where all children nodes are present and set to nullptr by default
    using middleware_root = middleware_base<middleware_node>;

    /**
     * Middleware dynamically scoped global customization point.
     * This is where the middleware's root's pointer is being stored.
     */
    inline constexpr struct [[nodiscard]] basic_middlewares final : global_binding<middleware_node> {
        /// Get the root node
        [[nodiscard]] constexpr pointer root() const noexcept {
            return ptr();
        }

        template <typename Tag>
        void trigger([[maybe_unused]] Tag) {
            if (root() == nullptr) {
                return;
            }
            root()->operator()(Tag{});
        }

    } middlewares;

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CONCEPTS_HPP
