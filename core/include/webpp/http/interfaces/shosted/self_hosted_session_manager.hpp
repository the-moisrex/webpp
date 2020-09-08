// Created by moisrex on 9/7/20.

#ifndef WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
#define WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP

#include "../../../server/server_concepts.hpp"
#include "../../../traits/traits_concepts.hpp"
#include "../../../traits/enable_traits.hpp"

#include <array>

namespace webpp::shosted {

    template <Traits TraitsType>
    struct self_hosted_session_manager : public enable_traits<TraitsType> {
        static constexpr unsigned buffer_size = 1024 * 1024 * 1024; // 1MiB
        static constexpr auto logger_category = "SHosted/Session";

        using etraits = enable_traits<TraitsType>;
        using traits_type = typename etraits::traits_type;
        using char_type = typename traits_type::char_type;
        using allocator_type = typename traits_type::allocator<char_type>;
        using buffer_type = stl::array<char_type, buffer_size>;

        using etraits::etraits;

        // read a batch of input
        void input(stl::size_t transferred_bytes, buffer_type buffer) noexcept {

        }

        // making the output
        void output() noexcept {

        };

    };

}

#endif // WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
