#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../platform/posix.hpp"
#ifdef webpp_posix // check if it's okay to use unix stuff here

#include "../server_concepts.hpp"
#include "../../traits/traits_concepts.hpp"
#include "./posix_thread_pool.hpp"
#include "./posix_connection.hpp"
#include "./posix_constants.hpp"
#include "../../std/vector.hpp"
#include "../../traits/enable_traits.hpp"
#include "../../std/cassert.hpp"

#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace webpp::posix {

    /**
     * This class is the server and the connection manager.
     */
    template <Traits TraitsType, SessionManager SessionType, ThreadPool ThreadPoolType = posix_thread_pool>
    struct posix_server : public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using etraits          = enable_traits<traits_type>;
        using session_type     = SessionType;
        using connection_type  = posix_connection<traits_type, session_type>;
        using socket_type      = int;
        using endpoint_type    = stl::unique_ptr<struct addrinfo>;
        using thread_pool_type = ThreadPoolType;

      private:
        static constexpr auto                      logger_cat = "Posix/Server";
        istl::vector<traits_type, connection_type> connections;
        istl::vector<traits_type, acceptor_type>   acceptors;
        thread_pool_type                           pool{};
        endpoint_type endpoints = nullptr;
        endpoint_type endpoint_hints = nullptr;

      public:
        posix_server(auto&&...args)
          : etraits{stl::forward<decltype(args)>(args)...},
            connections{etraits::get_allocator()}
            acceptors{etraits::get_allocator()}
            {};

      private:
        void accept(socket_type socket) noexcept {
            // todo: check whether or not the server was stopped by a signal

            // todo: before this completes, handler had a chance to run
            if (!ec) {
                connections.emplace_back(stl::move(socket));
            } else {
                logger.info(logger_cat, "Could not accept the user.", ec);
            }
        }

        void start_accepting() noexcept {
        }

      public:

        /**
         * Set endpoints directly; we suggest you not to use this unless you have a good reason. This is
         * low level POSIX stuff.
         * @param new_endpoints
         */
        void set_endpoints(endpoint_type new_endpoints) noexcept {
            endpoints = new_endpoints;
        }

        void resolve() noexcept {
            getaddrinfo()
        }

        void enable_ipv6() noexcept {
            set_default_hints();
            switch (endpoint_hints->ai_family) {
                case AF_INET6:
                case AF_UNSPEC:
                    break; // nothing to do
                case AF_INET:
                    endpoint_hints->ai_family = AF_UNSPEC;
                    break;
            }
        }

        void disable_ipv6() noexcept {
            set_default_hints();
            webpp_assert(endpoint_hints->ai_family != AF_INET6, "cannot disable both ipv4 and ipv6; "
                                                                "enable ipv4 before disabling ipv6 to "
                                                                "prevent this error from happening.");
            endpoint_hints->ai_family = AF_INET;
        }

        void enable_ipv4() noexcept {
            set_default_hints();
            switch (endpoint_hints->ai_family) {
                case AF_INET:
                case AF_UNSPEC:
                    break; // nothing to do
                case AF_INET6:
                    endpoint_hints->ai_family = AF_UNSPEC;
                    break;
            }
        }

        void disable_ipv4() noexcept {
            set_default_hints();
            webpp_assert(endpoint_hints->ai_family != AF_INET,  "cannot disable both ipv4 and ipv6; "
                                                                "enable ipv6 before disabling ipv4 to "
                                                                "prevent this error from happening.");
            endpoint_hints->ai_family = AF_INET6;
        }

        void set_default_hints() noexcept {
            if (!endpoint_hints) {
                endpoint_hints = stl::make_unique<struct addrinfo>();
                endpoint_hints->ai_family = AF_UNSPEC; // enable IPv6 and IPv4
                endpoint_hints->ai_socktype = SOCK_STREAM; // TCP only
            }
        }


        void operator()() noexcept {
            pool.post(
              [this]() {
                  // Run until the tasks finishes normally.
                  // Don't worry, we'll accept another connection when we finish one of them
                  for (;;) {
                      try {
                          start_accepting();
                          etraits::logger.info(logger_cat, "Starting running IO tasks in a thread.");
                          io.run();
                          etraits::logger.info(logger_cat, "Finished all the IO tasks in the thread successfully.");
                          break;
                      } catch (stl::exception const& err) {
                          etraits::logger.error(logger_cat, "Server tasks finished with errors.", err);
                      }
                  }
              },
              connections.get_allocator());
        }

        void stop() noexcept {
            // this will stop all threads
            etraits::logger.info(logger_cat, "Stopping all the IO tasks.");
        }
    };

} // namespace webpp

#endif // webpp_posix

#endif // WEBPP_INTERFACES_COMMON_SERVER_H
