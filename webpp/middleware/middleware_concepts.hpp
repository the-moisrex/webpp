#ifndef WEBPP_MIDDLEWARE_CONCEPTS_HPP
#define WEBPP_MIDDLEWARE_CONCEPTS_HPP

#include "../http/http_concepts.hpp"
#include "../traits/dynamic_scoping.hpp"

#include <concepts>

namespace webpp {

    struct [[nodiscard]] middleware_tag;

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
    concept OnionMiddleware = stl::invocable<T, middleware_tag> || stl::invocable<T>;

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

    /**
     * An event is a general concept containing two things:
     *   1. Hooks
     *   2. Middlewares
     *
     * Hooks are events that when triggered, all of them will be called in order.
     * Middlewares are events that they get to decide if the next middleware should be called or not.
     */
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
    struct [[nodiscard]] basic_event_node : private Tags::node_type... {
        // the nodes are being inheritted privately in order to allow compilers to optimize more aggresively to make
        // sure multiple inheritance can be optimized to the same level that a flat design can be optimized. Virtual
        // functions are costly, and having them come from multiple inherited base types is even more costly; so we make
        // them private, thus the compiler can be sure that it can't be converted to all those base classes, thus it can
        // optimize it.

        static_assert(sizeof...(Tags) > 0, "At least one single tag is needed.");

        constexpr basic_event_node() noexcept                              = default;
        constexpr basic_event_node(basic_event_node const&)                = default;
        constexpr basic_event_node(basic_event_node&&) noexcept            = default;
        constexpr basic_event_node& operator=(basic_event_node const&)     = default;
        constexpr basic_event_node& operator=(basic_event_node&&) noexcept = default;
        constexpr ~basic_event_node() noexcept override                    = default;

        using Tags::node_type::trigger...;

        template <typename Tag = typename istl::first_type_t<Tags...>>
        [[nodiscard]] constexpr typename Tag::node_type const* get_node([[maybe_unused]] Tag = {}) const noexcept {
            return static_cast<typename Tag::node_type const*>(this);
        }

        template <typename Tag = typename istl::first_type_t<Tags...>>
        [[nodiscard]] constexpr typename Tag::node_type* get_node([[maybe_unused]] Tag = {}) noexcept {
            return static_cast<typename Tag::node_type*>(this);
        }

        template <typename Tag>
        constexpr void set_child(typename Tag::node_type* node) noexcept {
            get_node<Tag>()->set_child(node);
        }

        template <typename Tag = typename istl::first_type_t<Tags...>>
        [[nodiscard]] constexpr typename Tag::node_type* get_child([[maybe_unused]] Tag = {}) const noexcept {
            return get_node<Tag>()->get_child();
        }

        template <typename Tag>
        constexpr void add_child(typename Tag::node_type* new_child) noexcept {
            // only register it if we support it
            auto* head = get_child<Tag>();
            if (head == nullptr) {
                this->template set_child<Tag>(new_child);
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

        /// Register a middleware in the tree
        /// Each middleware instance can have multiple events and will be pointed to multiple times.
        template <typename MW>
        constexpr void register_middleware(MW* inp_middleware) noexcept {
            // we can use C++23's `template for` here.
            auto const register_tag = [this, inp_middleware]<typename Tag>(Tag) constexpr noexcept {
                if constexpr (stl::is_base_of_v<typename Tag::node_type, MW>) {
                    this->template add_child<Tag>(inp_middleware->template get_node<Tag>());
                }
            };
            (register_tag(Tags{}), ...);
            // (add_child(inp_middleware->template get_child<Tags>()), ...);
        }

        template <typename MW>
        constexpr basic_event_node& operator+=(MW* inp_middleware) noexcept {
            register_middleware(inp_middleware);
            return *this;
        }

        template <typename MW>
        constexpr basic_event_node& operator+=(MW& inp_middleware) noexcept {
            register_middleware(&inp_middleware);
            return *this;
        }

        template <EventTag Tag>
        constexpr void next(Tag tag) const {
            auto* child = get_child(tag);
            if (child == nullptr) {
                return;
            }
            child->trigger(tag);
        }

        /// Shortcut to trigger the next event without the need to specify which event.
        constexpr void next() const
            requires(sizeof...(Tags) == 1)
        {
            auto* const child = get_child();
            if (child == nullptr) {
                return;
            }
            (child->trigger(Tags{}), ...);
        }
    };

    template <EventTag Tag, typename T, typename Base>
    struct [[nodiscard]] generic_hook_trigger : Base {
        using Base::trigger;

        void trigger(Tag const tag) final {
            // call the user's function
            static_cast<T*>(this)->operator()(tag);

            // call the next event no matter what
            static_cast<Base*>(this)->next(tag);
        }
    };

    template <typename Tag>
    struct [[nodiscard]] generic_node_type {
      private:
        // intrusive linked list
        generic_node_type* child = nullptr;

      public:
        generic_node_type() noexcept                               = default;
        generic_node_type(generic_node_type const&)                = default;
        generic_node_type(generic_node_type&&) noexcept            = default;
        generic_node_type& operator=(generic_node_type const&)     = default;
        generic_node_type& operator=(generic_node_type&&) noexcept = default;
        virtual ~generic_node_type()                               = default;
        virtual void trigger(Tag)                                  = 0;

        constexpr void set_child(generic_node_type* node) noexcept {
            child = node;
        }

        [[nodiscard]] constexpr generic_node_type* get_child() const noexcept {
            return child;
        }
    };

    template <int ID>
    struct [[nodiscard]] generic_hook {
        using node_type = generic_node_type<generic_hook>;

        template <typename T, typename Base>
        using impl_type = generic_hook_trigger<generic_hook, T, Base>;
    };

    /// on connection close hook
    using close_tag = generic_hook<0>;
    static constexpr close_tag on_close;

    /// middleware event
    static constexpr struct [[nodiscard]] middleware_tag {
        using node_type = generic_node_type<middleware_tag>;

        template <typename T, typename Base>
        struct [[nodiscard]] impl_type : Base {
            using Base::trigger;

            void trigger(middleware_tag const tag) final {
                // call the user's function
                if constexpr (OnionMiddleware<T>) {
                    if constexpr (stl::invocable<T, middleware_tag>) {
                        static_cast<T*>(this)->operator()(tag);
                    } else if constexpr (stl::invocable<T>) {
                        static_cast<T*>(this)->operator()();
                    } else {
                        static_assert_false(T, "This can't happen.");
                    }
                    // in onion middleware we don't call next, the user gets to choose if they need to call the next
                    // middleware or not.
                } else if constexpr (TwoWayMiddleware<T>) {
                    if constexpr (requires(T obj) { obj.pre(); }) {
                        static_cast<T*>(this)->pre();
                    }
                    static_cast<Base*>(this)->next(tag);
                    if constexpr (requires(T obj) { obj.post(); }) {
                        static_cast<T*>(this)->post();
                    }
                } else {
                    static_assert_false(T, "This is not a valid middleware");
                }
            }
        };
    } on_middleware;

    namespace details {

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
    event : public details::linearify_type<basic_event_node<Tags...>, T, Tags::template impl_type...> {
        using impl_chain_type = details::linearify_type<basic_event_node<Tags...>, T, Tags::template impl_type...>;
        using node_type       = basic_event_node<Tags...>;
    };

    template <typename T>
    using middleware = event<T, middleware_tag>;

    /// Middleware Node is the node part of the tree. This makes the middleware an intrusive linked list.
    /// If you need to add support to more events, add those events here.
    using complete_middleware_node = basic_event_node<middleware_tag, close_tag>;

    template <typename NodeType = complete_middleware_node>
    struct [[nodiscard]] basic_events_root {};

    /// Special case where all children nodes are present and set to nullptr by default.
    /// Root node requires to have all of the children but other nodes are only required to have only one child.
    /// This is the root of a tree, it by itself is not a node of the tree.
    template <EventTag... Tags>
    struct [[nodiscard]]
    basic_events_root<basic_event_node<Tags...>> : event<basic_events_root<basic_event_node<Tags...>>, Tags...> {
        /// Trigger the events in order
        template <EventTag Tag = middleware_tag>
        constexpr void operator()(Tag tag = {}) const {
            if (auto* child = this->template get_child<Tag>(); child != nullptr) [[likely]] {
                child->trigger(tag);
            }
        }
    };

    using events_root = basic_events_root<>;

    /**
     * Middleware dynamically scoped global customization point.
     * This is where the middleware's root's pointer is being stored.
     */
    inline constexpr struct [[nodiscard]] basic_events final : global_binding<events_root> {
        /// Get the root node
        [[nodiscard]] constexpr pointer root() const noexcept {
            return ptr();
        }

        template <typename Tag>
        constexpr void operator()([[maybe_unused]] Tag) {
            if (root() == nullptr) [[unlikely]] {
                return;
            }
            root()->operator()(Tag{});
        }

    } middlewares;

} // namespace webpp

#endif // WEBPP_MIDDLEWARE_CONCEPTS_HPP
