// Created by moisrex on 9/7/20.

#ifndef WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
#define WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP

#include "../configs/constants.hpp"
#include "../http/status_code.hpp"
#include "../server/server_concepts.hpp"
#include "../traits/enable_traits.hpp"
#include "../traits/traits.hpp"

#include <array>

namespace webpp::http::shosted {

    /**
     * For a self-hosted server, the session manager class will be created once for each request (well,
     * actually for each connection to be exact); this might not be the case for other server types.
     *
     * todo: see if we need a "shosted request manager" type too because of HTTP/2.0 that can handle multiple requests within one connection
     * todo: Send 204 (No Content) when you don't want the application fails to get you a body
     */
    template <Traits TraitsType, typename AppWrapperType, HTTPRequest RequestType, Connection ConnectionType>
    struct self_hosted_session_manager : public ConnectionType {
        static constexpr auto buffer_size     = default_buffer_size;
        static constexpr auto logger_category = "SelfHosted/Session";

        using connection_type  = ConnectionType;
        using traits_type      = TraitsType;
        using string_view_type = traits::string_view<traits_type>;
        using char_type        = istl::char_type_of_t<string_view_type>;
        using allocator_type   = traits::allocator_type_of<traits_type, char_type>;
        using buffer_type      = stl::array<char_type, buffer_size>;
        using request_type     = RequestType;
        using app_wrapper_type = AppWrapperType;

        // we use this way so if the application doesn't have any fields, it'll be easier just to
        // use a copy of it instead of using a reference
        // todo: add "trivial" here; it's a defect to use copy-ctor or move-ctor
        using app_wrapper_ref =
          stl::conditional_t<sizeof(app_wrapper_type) == 0 && stl::is_move_constructible_v<app_wrapper_type>,
                             app_wrapper_type,
                             app_wrapper_type&>;


      private:
        [[no_unique_address]] app_wrapper_ref app;
        request_type                          req;
        buffer_type                           _buffer{}; // todo: should we use char_type here?

      public:
        self_hosted_session_manager(app_wrapper_ref the_app, request_type request, auto&&... args)
          : app{the_app},
            req{stl::move(request)},
            connection_type{stl::forward<decltype(args)>(args)...} {}

        /**
         * read a batch of input
         *
         * Errors that this method should identify:
         *   - 413 (Entity Too Large): max http header values is about 4k usually
         *
         * But this method is in charge of identifying some error codes, but it doesn't produce the output
         */
        status_code read(stl::size_t transferred_bytes) noexcept {}

        // making the output
        string_view_type output() noexcept {}

        [[nodiscard]] bool keep_connection() const noexcept {
            return false;
        }
    };

} // namespace webpp::http::shosted

#endif // WEBPP_SELF_HOSTED_SESSION_MANAGER_HPP
