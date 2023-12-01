// Created by moisrex on 7/12/23.

#ifndef WEBPP_TAG_INVOKE_HPP
#define WEBPP_TAG_INVOKE_HPP

#include "std.hpp"

#include <type_traits>
#include <utility>

/**
 * `tag_invoke` is a C++ proposal
 *   - Proposal Link: https://wg21.link/P1895
 */
namespace webpp::stl {

    namespace tag_invoke_fn_ns {
        void tag_invoke() = delete;

        struct tag_invoke_fn {
            template <typename Tag, typename... Args>
                requires requires(Tag tag, Args&&... args) {
                    tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...);
                }
            constexpr auto operator()(Tag tag, Args&&... args) const
              noexcept(noexcept(tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...)))
                -> decltype(tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...)) {
                return tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...);
            }
        };
    } // namespace tag_invoke_fn_ns

    inline namespace tag_invoke_ns {
        inline constexpr tag_invoke_fn_ns::tag_invoke_fn tag_invoke = {};
    } // namespace tag_invoke_ns

    template <typename Tag, typename... Args>
    concept tag_invocable =
      requires(Tag tag, Args... args) { tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...); };

    template <typename Tag, typename... Args>
    concept nothrow_tag_invocable = tag_invocable<Tag, Args...> && requires(Tag tag, Args... args) {
        {
            tag_invoke(stl::forward<Tag>(tag), stl::forward<Args>(args)...)
        } noexcept;
    };

    template <typename Tag, typename... Args>
    using tag_invoke_result = invoke_result<decltype(tag_invoke), Tag, Args...>;

    template <typename Tag, typename... Args>
    using tag_invoke_result_t = invoke_result_t<decltype(tag_invoke), Tag, Args...>;

    template <auto& Tag>
    using tag_t = decay_t<decltype(Tag)>;


} // namespace webpp::stl

#endif // WEBPP_TAG_INVOKE_HPP
