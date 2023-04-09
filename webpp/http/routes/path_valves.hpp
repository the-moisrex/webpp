// Created by moisrex on 4/3/23.

#ifndef WEBPP_HTTP_ROUTES_PATH_VALVES_HPP
#define WEBPP_HTTP_ROUTES_PATH_VALVES_HPP

#include "../../std/string.hpp"
#include "../../std/string_view.hpp"
#include "std/concepts.hpp"
#include "std/string_concepts.hpp"
#include "valve_traits.hpp"

namespace webpp::http {

    static constexpr struct endpath_valve {

        template <Traits TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            return ctx.path_traverser().at_end();
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(" endpath");
        }
    } endpath;


    /**
     * A Series of Path Segments
     */
    template <typename... CallableSegments>
    struct segment_valve : valve<segment_valve<CallableSegments...>>, stl::tuple<CallableSegments...> {
        static_assert((true && ... && !istl::StringLiteral<CallableSegments>), "");
        using valve_type = valve<segment_valve<CallableSegments...>>;
        using tuple_type = stl::tuple<CallableSegments...>;

        template <typename... Args>
            requires stl::constructible_from<tuple_type, Args...>
        constexpr segment_valve(Args&&... args) noexcept(stl::is_nothrow_constructible_v<tuple_type, Args...>)
          : tuple_type{stl::forward<Args>(args)...} {}

        constexpr segment_valve(segment_valve const&)                     = default;
        constexpr segment_valve(segment_valve&&) noexcept                 = default;
        constexpr segment_valve& operator=(segment_valve&&) noexcept      = default;
        constexpr segment_valve& operator=(segment_valve const&) noexcept = default;
        constexpr ~segment_valve()                                        = default;

        using valve_type::operator();

        template <Traits TraitsType>
        constexpr bool operator()(basic_context<TraitsType>& ctx) {
            using context_type = basic_context<TraitsType>;
            return stl::apply(
              [&ctx]<typename... T>(T&&... callables) constexpr {
                  return (valve_traits<T, context_type>::call_set_get(stl::forward<T>(callables), ctx) &&
                          ...);
              },
              as_tuple());
        }

        [[nodiscard]] constexpr tuple_type& as_tuple() noexcept {
            return *static_cast<tuple_type*>(this);
        }

        [[nodiscard]] constexpr tuple_type const& as_tuple() const noexcept {
            return *static_cast<tuple_type const*>(this);
        }

        constexpr void to_string(istl::String auto& out) const {
            stl::apply(
              [&out]<typename... T>(T&&... callables) constexpr {
                  (([&out](auto&& callable) constexpr {
                       out.append(" /");
                       valve_to_string(out, callable);
                   })(callables),
                   ...);
              },
              as_tuple());
        }


        template <typename RouterT>
            requires((ValveRequiresSetup<RouterT, CallableSegments> || ...))
        constexpr void setup(RouterT& inp_router) {
            stl::apply(
              [&inp_router]<typename... T>(T&&... callables) constexpr {
                  (([]([[maybe_unused]] auto&& callable, [[maybe_unused]] RouterT& router) constexpr {
                       if constexpr (ValveRequiresSetup<RouterT, T>) {
                           callable.setup(router);
                       }
                   })(callables, inp_router),
                   ...);
              },
              as_tuple());
        }
    };




    template <typename Segment>
    struct segment_string {
      private:
        Segment seg;

      public:
        constexpr segment_string(Segment&& inp_seg) noexcept : seg{stl::move(inp_seg)} {}
        constexpr segment_string(Segment const& inp_seg) : seg{inp_seg} {}

        constexpr segment_string(segment_string const&)                = default;
        constexpr segment_string(segment_string&&) noexcept            = default;
        constexpr ~segment_string()                                    = default;
        constexpr segment_string& operator=(segment_string const&)     = default;
        constexpr segment_string& operator=(segment_string&&) noexcept = default;

        template <typename TraitsType>
        [[nodiscard]] constexpr bool operator()(basic_context<TraitsType>& ctx) const noexcept {
            return ctx.check_segment(seg);
        }

        constexpr void to_string(istl::String auto& out) const {
            out.append(seg);
        }
    };

    template <typename Seg>
    segment_string(Seg&&) -> segment_string<stl::remove_cvref_t<Seg>>;

    template <istl::StringLiteral StrT>
    struct valvify<StrT> {
        template <istl::cvref_as<StrT> TT>
        [[nodiscard]] static constexpr auto call(TT&& next) noexcept {
            return segment_string{istl::string_viewify(stl::forward<TT>(next))};
        }
    };

    // String Views Valvifier
    template <istl::StringView T>
    struct valvify<T> {
        template <istl::cvref_as<T> TT>
        [[nodiscard]] static constexpr auto call(TT&& next) noexcept {
            return segment_string{stl::forward<TT>(next)};
        }
    };

    // String object is passed
    template <istl::String T>
    struct valvify<T> {
        template <istl::cvref_as<T> TT>
        [[nodiscard]] static constexpr auto call(TT&& next) {
            return segment_string{stl::forward<TT>(next)};
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_ROUTES_PATH_VALVES_HPP
