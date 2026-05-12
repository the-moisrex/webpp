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
    concept EventOf = requires(T obj, Tag tag) {
        obj(tag); // object must be callable with the specified tag
    };

    /**
     * Middleware Node is one node in the Intrusive Linked List part of the middleware tree.
     */
    template <EventTag... Tags>
    struct [[nodiscard]] basic_middleware_node : private Tags::node_type... {
        // the nodes are being inheritted privately in order to allow compilers to optimize more aggresively to make
        // sure multiple inheritance can be optimized to the same level that a flat design can be optimized. Virtual
        // functions are costly, and having them come from multiple inherited base types is even more costly; so we make
        // them private, thus the compiler can be sure that it can't be converted to all those base classes, thus it can
        // optimize it.

        static_assert(sizeof...(Tags) > 0, "At least one single tag is needed.");

        constexpr basic_middleware_node() noexcept                                   = default;
        constexpr basic_middleware_node(basic_middleware_node const&)                = default;
        constexpr basic_middleware_node(basic_middleware_node&&) noexcept            = default;
        constexpr basic_middleware_node& operator=(basic_middleware_node const&)     = default;
        constexpr basic_middleware_node& operator=(basic_middleware_node&&) noexcept = default;
        constexpr ~basic_middleware_node() noexcept override                         = default;

        using Tags::node_type::trigger...;

        constexpr void set_child(basic_middleware_node* node) noexcept {
            child = node;
        }

        [[nodiscard]] constexpr basic_middleware_node* get_child() const noexcept {
            return child;
        }

        template <EventTag Tag>
        constexpr void next(Tag tag) const {
            if (child == nullptr) {
                return;
            }
            return child->trigger(tag);
        }

      private:
        // intrusive linked list
        basic_middleware_node* child = nullptr;
    };

    /// on connection close event
    static constexpr struct [[nodiscard]] close_tag {
        struct [[nodiscard]] node_type {
            node_type() noexcept                       = default;
            node_type(node_type const&)                = default;
            node_type(node_type&&) noexcept            = default;
            node_type& operator=(node_type const&)     = default;
            node_type& operator=(node_type&&) noexcept = default;
            virtual ~node_type()                       = default;
            virtual void trigger(close_tag)            = 0;
        };

        template <typename T, typename Base>
        struct [[nodiscard]] impl_type : Base {
          private:
            void trigger(close_tag const tag) final {
                // call the user's function
                static_cast<T*>(this)->operator()(tag);

                // call the next event no matter what
                static_cast<Base*>(this)->next(tag);
            }
        };
    } on_close;

    /// middleware event
    static constexpr struct [[nodiscard]] middleware_tag {
        struct [[nodiscard]] node_type {
            node_type() noexcept                       = default;
            node_type(node_type const&)                = default;
            node_type(node_type&&) noexcept            = default;
            node_type& operator=(node_type const&)     = default;
            node_type& operator=(node_type&&) noexcept = default;
            virtual ~node_type()                       = default;
            virtual void trigger(middleware_tag)       = 0;
        };

        template <typename T, typename Base>
        struct [[nodiscard]] impl_type : Base {
          private:
            void trigger(middleware_tag const tag) final {
                // call the user's function
                static_cast<T*>(this)->operator()(tag);
                // in middleware we don't call next, the user gets to choose if they need to call the next middleware or
                // not.
            }
        };
    } on_middleware;

    /// Middleware Node is the node part of the tree. This makes the middleware an intrusive linked list.
    /// If you need to add support to more events, add those events here.
    using complete_middleware_node = basic_middleware_node<middleware_tag, close_tag>;

    namespace details {

        template <typename Tag>
        struct root_child {
            basic_middleware_node<Tag>* child = nullptr;
        };

        template <typename... Tags>
        struct [[nodiscard]] root_children : root_child<Tags>... {
            template <typename Tag>
            [[nodiscard]] constexpr basic_middleware_node<Tag>* get_child() const noexcept {
                return static_cast<root_child<Tag> const*>(this)->child;
            }

            template <typename Tag>
            constexpr void add_child(basic_middleware_node<Tag>* new_child) noexcept {
                // only register it if we support it
                if constexpr (requires { static_cast<root_child<Tag>*>(this); }) {
                    auto& head = static_cast<root_child<Tag>*>(this)->child;
                    if (head == nullptr) {
                        head = new_child;
                    } else {
                        // Traverse the specific event's intrusive linked list
                        // Upcast middleware_node to the specific ChildType::node_type
                        for (auto* current = head;;) {
                            auto* next = current->get_child();
                            if (next == nullptr) {
                                current->set_child(new_child);
                                break;
                            }
                            current = next;
                        }
                    }
                }
            }

            template <typename MW>
            constexpr void register_middleware(MW* mw_ptr) noexcept {
                auto* node = mw_ptr->get_node();
                add_child(node);
            }
        };

        template <typename T, typename Pool, typename Res = basic_middleware_node<>>
        struct middleware_node_picker {};

        // Add the tag
        template <typename T, typename Tag, typename... PoolTags, typename... ResTags>
            requires EventOf<Tag, T>
        struct middleware_node_picker<T, basic_middleware_node<Tag, PoolTags...>, basic_middleware_node<ResTags...>>
          : middleware_node_picker<T, basic_middleware_node<PoolTags...>, basic_middleware_node<ResTags..., Tag>> {};

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

        template <typename T, EventTag Tag>
        struct [[nodiscard]] impl_trigger {
            void trigger(Tag tag = {}) {
                static_cast<T*>(this)->operator()(tag);
            }
        };

        /// Base = base class
        ///    T = for CRTP usage
        /// Reuslts: Impl3<Impl2<Impl1<Base>>>
        /// Or more accurately: Impl3<T, Impl2<T, Impl1<T, Base>>>
        ///
        /// The reason why we're linearifying the virtual function implementations is due to performance.
        /// The non-linear inheritance will create the "diamond problem" which potentially a tiny bit slower to fix
        /// using "virtual inheritance".
        template <typename Base, typename T, template <typename, typename> typename... Impls>
        struct linearify {};

        template <typename Base,
                  typename T,
                  template <typename, typename> typename Impl1,
                  template <typename, typename> typename... Impls>
        struct linearify<Base, T, Impl1, Impls...> : linearify<Impl1<T, Base>, T, Impls...> {};

        template <typename Base, typename T>
        struct linearify<Base, T> {
            using type = Base;
        };

        template <typename Base, typename T, template <typename, typename> typename... Impls>
        using linearify_type = typename linearify<Base, T, Impls...>::type;
    } // namespace details

    /**
     * This is the CRTP base class that turns classes into middlewares.
     */
    template <typename T, EventTag... Tags>
    struct [[nodiscard]]
    middleware_base : public details::linearify_type<basic_middleware_node<Tags...>, T, Tags::template impl_type...> {
        // using middleware_node_type = details::trimmed_middleware_node<T>;
        using middleware_node_type = basic_middleware_node<Tags...>;

        [[nodiscard]] constexpr middleware_node_type* get_node() noexcept {
            return static_cast<middleware_node_type*>(this);
        }
    };

    template <typename NodeType = complete_middleware_node>
    struct [[nodiscard]] basic_middlewares_root {};

    /// Special case where all children nodes are present and set to nullptr by default.
    /// Root node requires to have all of the children but other nodes are only required to have only one child.
    /// This is the root of a tree, it by itself is not a node of the tree.
    template <EventTag... Tags>
    struct [[nodiscard]] basic_middlewares_root<basic_middleware_node<Tags...>> {
      private:
        details::root_children<Tags...> children;

      public:
        template <EventTag Tag>
        [[nodiscard]] constexpr basic_middleware_node<Tag>* get_child() const noexcept {
            return children.template get_child<Tag>();
        }

        /// Trigger the events in order
        template <EventTag Tag = middleware_tag>
        constexpr void operator()(Tag tag = {}) const {
            // todo: this only runs one event
            if (auto* child = get_child<Tag>(); child != nullptr) [[likely]] {
                child->trigger(tag);
            }
        }

        /// Register a middleware in the tree
        /// Each middleware instance can have multiple events and will be pointed to multiple times.
        template <typename MW>
        constexpr void register_middleware(MW* inp_middleware) noexcept {
            children.register_middleware(inp_middleware);
        }

        template <typename MW>
        constexpr basic_middlewares_root& operator+=(MW* inp_middleware) noexcept {
            children.register_middleware(inp_middleware);
            return *this;
        }

        template <typename MW>
        constexpr basic_middlewares_root& operator+=(MW& inp_middleware) noexcept {
            children.register_middleware(&inp_middleware);
            return *this;
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

        // template <typename Tag>
        // constexpr void operator()([[maybe_unused]] Tag) {
        //     if (root() == nullptr) {
        //         return;
        //     }
        //     root()->trigger(Tag{});
        // }

    } middlewares;

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CONCEPTS_HPP
