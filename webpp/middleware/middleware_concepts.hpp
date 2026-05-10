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

    /// on connection close event
    struct [[nodiscard]] close_tag {
        struct [[nodiscard]] node_type {
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
        struct [[nodiscard]] node_type {
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
    /// If you need to add support to more events, add those events here.
    using complete_middleware_node = basic_middleware_node<middleware_tag, close_tag>;

    namespace details {

        template <typename EventType>
        struct root_child {
          protected:
            basic_middleware_node<EventType>* child = nullptr;
        };

        template <typename... EventTypes>
        struct [[nodiscard]] root_children : root_child<EventTypes>... {
            template <typename ChildType>
            [[nodiscard]] constexpr ChildType* get_child() const noexcept {
                return static_cast<root_child<ChildType> const*>(this)->child;
            }

            template <typename ChildType>
            constexpr void add_child(ChildType* new_child) noexcept {
                // only register it if we support it
                if constexpr (requires { static_cast<root_child<ChildType>*>(this); }) {
                    auto& head = static_cast<root_child<ChildType>*>(this)->child;
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
                complete_middleware_node* node = mw_ptr->get_node();
                (add_child<EventTypes>(node), ...);
            }
        };

        template <typename T, typename Pool, typename Res = basic_middleware_node<>>
        struct middleware_node_picker {};

        // Add the tag
        template <typename T, typename Tag, typename... PoolTags, typename... ResTags>
            requires EventOf<Tag, T>
        struct middleware_node_picker<T, basic_middleware_node<Tag, PoolTags...>, basic_middleware_node<ResTags...>> {
            using type = basic_middleware_node<ResTags..., Tag>;
        };

        // Remove the tag
        template <typename T, typename Tag, typename... PoolTags, typename... ResTags>
        struct middleware_node_picker<T, basic_middleware_node<Tag, PoolTags...>, basic_middleware_node<ResTags...>>
          : middleware_node_picker<T, basic_middleware_node<PoolTags...>, basic_middleware_node<ResTags...>> {};

        // Stop condition
        template <typename T, typename... ResTags>
        struct middleware_node_picker<T, basic_middleware_node<>, basic_middleware_node<ResTags...>> {
            using type = basic_middleware_node<ResTags...>;
        };

        /**
         * Return a basic_middleware_node<...> that only contains the events they actually have implemented.
         */
        template <typename T, typename Pool = complete_middleware_node>
        using trimmed_middleware_node = typename middleware_node_picker<T, Pool>::type;

    } // namespace details

    /**
     * This is the CRTP base class that turns classes into middlewares.
     */
    template <typename T>
    struct [[nodiscard]] middleware_base : details::trimmed_middleware_node<T> {
        using middleware_node_type = details::trimmed_middleware_node<T>;

      private:
        middleware_node_type* child = nullptr;

      public:
        /// Trigger the event
        template <EventTag Tag = middleware_tag>
        void trigger(Tag = {}) const {
            if constexpr (EventOf<Tag, T>) {
                static_cast<T*>(this)->operator()(Tag{});
            }
        }

        [[nodiscard]] constexpr middleware_node_type* get_node() noexcept {
            return static_cast<middleware_node_type*>(this);
        }

        /// Call the next middleware
        template <EventTag Tag>
        decltype(auto) next(Tag) const {
            if constexpr (EventOf<Tag, T>) {
                assert(child != nullptr);
                return child->operator()(Tag{});
            }
        }
    };

    template <typename NodeType = complete_middleware_node>
    struct [[nodiscard]] basic_middlewares_root {};

    /// Special case where all children nodes are present and set to nullptr by default
    /// Root node requires to have all of the children but other nodes are only required to have only one child
    template <EventTag... Tags>
    struct [[nodiscard]] basic_middlewares_root<basic_middleware_node<Tags...>> {
      private:
        details::root_children<Tags...> children;

      public:
        template <EventTag Tag>
        [[nodiscard]] constexpr basic_middleware_node<Tag>* get_child() const noexcept {
            return children.template get_child<Tag>();
        }

        /// Trigger the event
        template <EventTag Tag = middleware_tag>
        void trigger(Tag tag = {}) const {
            get_child<Tag>()->trigger(tag);
        }

        /// Register a middleware in the tree
        /// Each middleware instance can have multiple events and will be pointed to multiple times.
        template <typename MW>
        void register_middleware(MW* inp_middleware) noexcept {
            children.template register_middleware<MW>(inp_middleware);
        }
    };

    using middlewares_root = basic_middlewares_root<>;

    /**
     * Middleware dynamically scoped global customization point.
     * This is where the middleware's root's pointer is being stored.
     */
    inline constexpr struct [[nodiscard]] basic_middlewares final : global_binding<complete_middleware_node> {
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
