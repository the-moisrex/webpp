// Created by moisrex on 4/8/23.

#ifndef WEBPP_HTTP_ROUTER_MEMBER_FUNCTION_VALVES_HPP
#define WEBPP_HTTP_ROUTER_MEMBER_FUNCTION_VALVES_HPP

#include "valve_traits.hpp"

#include <any>
#include <typeinfo>

namespace webpp::http {

    /**
     * This valve helps to store a member function pointer until the setup function can be called with the
     * object itself; then when the router calls this valve, the member function of
     * that object will be called.
     */
    template <typename MemPtr>
    struct member_function_valve : valve<member_function_valve<MemPtr>> {
        using valve_type      = valve<member_function_valve<MemPtr>>;
        using member_ptr_type = MemPtr;
        using mem_traits      = istl::member_function_pointer_traits<member_ptr_type>;
        using object_type     = typename mem_traits::type;
        using object_ptr      = object_type*;

      private:
        struct method_holder {
            member_ptr_type mem_ptr;
            object_ptr      obj = nullptr;

            template <typename... Args>
                requires(mem_traits::template is_invocable<Args...>)
            constexpr decltype(auto) operator()(Args&&... args) const noexcept(mem_traits::is_noexcept) {
                return (obj->*mem_ptr)(stl::forward<Args>(args)...);
            }
        } holder;

      public:
        constexpr member_function_valve(member_function_valve const&) noexcept            = default;
        constexpr member_function_valve(member_function_valve&&) noexcept                 = default;
        constexpr member_function_valve& operator=(member_function_valve const&) noexcept = default;
        constexpr member_function_valve& operator=(member_function_valve&&) noexcept      = default;
        constexpr member_function_valve() noexcept                                        = default;
        constexpr ~member_function_valve() noexcept                                       = default;

        constexpr member_function_valve(member_ptr_type inp_func) noexcept : holder{.mem_ptr = inp_func} {}

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr void operator()(basic_context<TraitsType>& ctx) {
            using context_type    = basic_context<TraitsType>;
            using callable_traits = valve_traits<method_holder, context_type>;
            callable_traits::call_set(holder, ctx);
        }

        constexpr void set_object(object_ptr inp_obj) noexcept {
            holder.obj = inp_obj;
        }


        /**
         * @brief Set up the router with the specified router.
         *
         * If the router is a static router, it sets the object to the specified object type.
         * If the router is a dynamic router, it searches for an object of the specified type in the router's
         * objects. If a matching object is found, it sets the object to that type. If no matching object is
         * found, it either default constructs the object (if the object type is default constructible), or
         * constructs the object using the router as a parameter (if the object type is constructible with the
         * router). If none of these conditions are met, an error message is logged.
         *
         * @tparam RouterType The type of the router.
         */
        template <typename RouterType>
        constexpr void setup(RouterType& router) {
            using objects_type = typename RouterType::objects_type;
            if constexpr (istl::Tuple<objects_type>) {
                // static router's object

                if constexpr (istl::contains_parameter_of<objects_type, object_type>) {
                    set_object(stl::addressof(get<object_type>(router.objects)));
                } else {
                    static_assert_false(object_type,
                                        "The router doesn't include an object type that "
                                        "we can call the specified member function pointer on.");
                }
            } else {
                // dynamic router's object:

                for (auto& object : router.objects) {
                    if (object.type() == typeid(object_type)) {
                        set_object(stl::any_cast<object_type>(stl::addressof(object)));
                        return;
                    }
                }

                // default constructing it if it's possible and use that object
                if constexpr (stl::is_default_constructible_v<object_type>) {
                    router.objects.emplace_back(object_type{});
                    set_object(stl::any_cast<object_type>(stl::addressof(router.objects.back())));
                } else if constexpr (stl::is_constructible_v<object_type, RouterType&>) {
                    router.objects.emplace_back(object_type{router});
                    set_object(stl::any_cast<object_type>(stl::addressof(router.objects.back())));
                } else {
                    router.logger.error(
                      "DRouter",
                      fmt::format(
                        "You have not specified an object with typeid of '{}' in your dynamic router,"
                        " but you've tried to register a member function pointer of that type in router."
                        " Try registering your objects before registering the objects in the router"
                        " to get rid of this error.",
                        typeid(object_type).name()));
                }
            }
        }


        /**
         * @brief Converts the member function to a string representation.
         *
         * This function converts the value of a variable to its string representation and
         * stores it in the provided output string.
         *
         * @param out The output string where the converted value will be stored.
         * @tparam T The type of the variable.
         */
        constexpr void to_string(istl::String auto& out) const {
            valve_to_string(out, holder.mem_ptr);
        }
    };



    // Member Function Pointer Valvifier
    template <typename T>
        requires stl::is_member_function_pointer_v<stl::remove_cvref_t<T>>
    [[nodiscard]] static constexpr member_function_valve<stl::remove_cvref_t<T>>
    tag_invoke(valvify_tag, T&& next) noexcept {
        return {stl::forward<T>(next)};
    }

    // Free Functions
    template <typename T>
        requires stl::is_function_v<stl::remove_cvref_t<T>>
    [[nodiscard]] static constexpr member_function_valve<stl::remove_cvref_t<T>>
    tag_invoke(valvify_tag, T&& next) noexcept {
        return {stl::forward<T>(next)};
    }


} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTER_MEMBER_FUNCTION_VALVES_HPP
