// Created by moisrex on 11/8/20.

#ifndef WEBPP_LEXICAL_CAST_HPP
#define WEBPP_LEXICAL_CAST_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../utils/allocators.hpp"
#include "./casts.hpp"
#include "../std/format.hpp"

namespace webpp::lexical {

    /**
     * Convert stuff to other stuff:
     *   - string types        to string view types
     *   - string view types   to string
     *   - integer types       to integer types
     *   - integer types       to string types (but not string_view types)
     *
     * This method can have a list of allocators or types that has allocators as well, this will help
     * strings to be allocated with the specified allocator.
     */
    template <typename Target, typename Source, typename... AllocList>
    [[nodiscard]] static constexpr Target cast(Source&& source, AllocList const&... allocs) noexcept {
        using src_t    = stl::remove_cvref_t<Source>;
        using target_t = stl::remove_cvref_t<Target>;

        if constexpr (stl::same_as<target_t, src_t>) {
            // Target == Source
            return stl::forward<Source>(source);
        } else if constexpr (istl::StringView<target_t> && istl::StringViewifiableOf<target_t, src_t>) {
            // Target == string_view and Source is convertible
            return istl::string_viewify_of<Target>(stl::forward<Source>(source));
        } else if constexpr (istl::String<target_t>) {
            // Target == string
            const auto the_alloc = extract_allocator_of_or_default<istl::allocator_type_of<target_t>>(allocs..., source);
            if constexpr (istl::StringifiableOf<target_t, src_t>) {
                // Source is convertible to string
                return istl::stringify_of<Target>(stl::forward<Source>(source), the_alloc);
            } else if constexpr (istl::String<target_t> && stl::is_integral_v<src_t>) {
                // Source is integer type
                Target     res{the_alloc};
                append_to(res, stl::forward<Source>(source));
                return res;
            } else if constexpr (istl::String<target_t> && stl::is_floating_point_v<src_t>) {
                // Source is floating type
                // technically we should be able to use std::to_chars for floats too but it's not yet
                // implemented so we're asking fmt for help; maybe we should use std::to_string
                // todo: you can make this faster
                Target res{the_alloc};
                stl::format_to(stl::back_inserter(res), FMT_COMPILE("{}"), stl::forward<Source>(source));
                return res;
            } else {
                // todo: you can make this faster
                Target res{the_alloc};
                stl::format_to(stl::back_inserter(res), FMT_COMPILE("{}"), stl::forward<Source>(source));
                return res;
            }
        } else if constexpr (stl::is_integral_v<target_t>) {
            // Target == integer
            if constexpr (stl::is_integral_v<src_t>) {
                // Source == integer as well
                return static_cast<Target>(stl::forward<Source>(source));
            } else {
                // Source == could be anything now
                // todo: add more else ifs
                return to<Target>(stl::forward<Source>(source));
            }
        } else if constexpr (requires {
                                 Target{stl::forward<Source>(source), stl::forward<AllocList>(allocs)...};
                             }) {
            return Target{stl::forward<Source>(source), stl::forward<AllocList>(allocs)...};
        } else if constexpr (requires {
                                 Target{stl::forward<Source>(source),
                                        extract_allocator_or_default(stl::forward<AllocList>(allocs)..., source)};
                             }) {
            return Target{stl::forward<Source>(source),
                   extract_allocator_or_default(stl::forward<AllocList>(allocs)..., source)};
        } else if constexpr (requires { Target{stl::forward<Source>(source)}; }) {
            return Target{stl::forward<Source>(source)};
        }
    }

    template <template<typename...> typename Target, typename Source, typename... AllocList>
    [[nodiscard]] static constexpr auto cast(Source&& source, AllocList const&... allocs) noexcept {
        using src_t    = stl::remove_cvref_t<Source>;
//        constexpr bool is_target_string_view = istl::StringView<decltype(Target{""})>;
//        constexpr bool is_target_string = istl::String<decltype(Target{""})>;
//        static_assert(!is_target_string_view);
//        static_assert(is_target_string);

        if constexpr (istl::StringViewifiableOfTemplate<Target, src_t> && requires {
            istl::string_viewify_of<Target>(stl::forward<Source>(source));
                                                                          }) {
            return istl::string_viewify_of<Target>(stl::forward<Source>(source));
        } else if constexpr (istl::StringifiableOfTemplate<Target, src_t> && requires {
            istl::stringify_of<Target>(stl::forward<Source>(source), extract_allocator_or_default(allocs..., source));
                                                                             }) {
            const auto the_alloc = extract_allocator_or_default(allocs..., source);
            return istl::stringify_of<Target>(stl::forward<Source>(source), the_alloc);
        } else if constexpr (requires {
            Target{stl::forward<Source>(source), stl::forward<AllocList>(allocs)...};
        }) {
            return Target{stl::forward<Source>(source), stl::forward<AllocList>(allocs)...};
        } else if constexpr (requires {
            Target{stl::forward<Source>(source),
                   extract_allocator_or_default(stl::forward<AllocList>(allocs)..., source)};
        }) {
            return Target{stl::forward<Source>(source),
                          extract_allocator_or_default(stl::forward<AllocList>(allocs)..., source)};
        } else if constexpr (requires { Target{stl::forward<Source>(source)}; }) {
            return Target{stl::forward<Source>(source)};
        }
    }


    // todo: implement safe_cast
    // todo: implement floating point
    // todo: implement complex numbers
    // todo: implement bool special cases
    // todo: implement other integer types than "char" as string's character
    // todo: implement locale-aware casts as well

} // namespace webpp::lexical

#endif // WEBPP_LEXICAL_CAST_HPP
