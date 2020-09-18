// Created by moisrex on 9/7/20.

#ifndef WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
#define WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP

#include "../../../server/server_concepts.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../traits/traits_concepts.hpp"

#include <array>

namespace webpp::shosted {

    template <Traits TraitsType, typename RequestType>
    struct self_hosted_session_manager : public enable_traits<TraitsType> {
        static constexpr unsigned buffer_size     = 1024 * 1024 * 1024; // 1 MiB
        static constexpr auto     logger_category = "SelfHosted/Session";

        using etraits          = enable_traits<TraitsType>;
        using traits_type      = typename etraits::traits_type;
        using char_type        = typename traits_type::char_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename traits_type::template allocator<char_type>;
        using buffer_type      = stl::array<char_type, buffer_size>;
        using request_type     = RequestType;

        using etraits::etraits;

      private:
        buffer_type _buffer{}; // todo: should we use char_type here?
        bool        _keep_connection = false;

      public:
        buffer_type& buffer() noexcept {
            return _buffer;
        }

        // read a batch of input
        bool read(stl::size_t transferred_bytes) noexcept {

        }

        // making the output
        string_view_type output() noexcept {

        };

        bool keep_connection() const noexcept {
            return _keep_connection;
        }
    };

} // namespace webpp::shosted

#endif // WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
