// Created by moisrex on 9/7/20.

#ifndef WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
#define WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP

#include "../../../server/server_concepts.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../traits/traits_concepts.hpp"
#include "../../../configs/constants.hpp"
#include "../../status_code.hpp"

#include <array>

namespace webpp::shosted {

    template <Traits TraitsType, Connection ConnectionType, Request RequestType>
    struct self_hosted_session_manager : public ConnectionType {
        static constexpr auto buffer_size     = default_buffer_size;
        static constexpr auto logger_category = "SelfHosted/Session";

        using traits_type      = typename etraits::traits_type;
        using char_type        = typename traits_type::char_type;
        using string_view_type = typename traits_type::string_view_type;
        using allocator_type   = typename traits_type::template allocator<char_type>;
        using buffer_type      = stl::array<char_type, buffer_size>;
        using request_type     = RequestType;


      private:
        buffer_type _buffer{}; // todo: should we use char_type here?
        request_type const& initial_request;

        [[nodiscard]] request_type copy_request() {
            return initial_request;
        }

      public:

        /**
         * read a batch of input
         *
         * Errors that this method should identify:
         *   - 413 (Entity Too Large): max http header values is about 4k usually
         *
         * But this method is in charge of identifying some error codes, but it doesn't produce the output
         */
        status_code read(stl::size_t transferred_bytes) noexcept {

        }

        auto remote_addr() const {

        }

        // making the output
        string_view_type output() noexcept {

        }

        [[nodiscard]] bool keep_connection() const noexcept {
            return false;
        }
    };

} // namespace webpp::shosted

#endif // WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
