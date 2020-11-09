// Created by moisrex on 11/8/20.

#ifndef WEBPP_LEXICAL_CAST_HPP
#define WEBPP_LEXICAL_CAST_HPP

#include "../std/string.hpp"
#include "../std/string_view.hpp"
#include "../utils/allocators.hpp"
#include "./casts.hpp"

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
            if constexpr (stl::same_as<Target, Source>) {
                return stl::forward<Source>(source);
            } else {
            }
        } else if constexpr (istl::StringView<target_t> && istl::StringViewifiableOf<target_t, src_t>) {
            return istl::string_viewify_of<Target>(stl::forward<Source>(source));
        } else if constexpr (istl::String<target_t> && istl::StringifiableOf<target_t, src_t>) {
            const auto the_alloc = extract_allocator_or_default(allocs..., source);
            return istl::stringify_of<Target>(stl::forward<Source>(source), the_alloc);
        } else if constexpr (istl::String<target_t> && stl::is_integral_v<src_t>) {
            const auto the_alloc = extract_allocator_or_default(allocs..., source);
            Target res{the_alloc};
            append_to(res, stl::forward<Source>(source));
            return res;
        } else if constexpr (istl::String<target_t> && stl::is_floating_point_v<src_t>) {
            // todo: doesn't work at all now because GCC has not implement it yet after 4 years
            const auto the_alloc = extract_allocator_or_default(allocs..., source);
            Target res{the_alloc};
            append_to(res, stl::forward<Source>(source), stl::chars_format::fixed);
            return res;
        } else if constexpr (stl::is_integral_v<target_t>) {
            if constexpr (stl::is_integral_v<src_t>) {
                return static_cast<Target>(stl::forward<Source>(source));
            } else {
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
