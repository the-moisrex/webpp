#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../platform/posix.hpp"
#ifdef webpp_posix // check if it's okay to use unix stuff here

#    include "../../std/cassert.hpp"
#    include "../../std/vector.hpp"
#    include "../../traits/enable_traits.hpp"
#    include "../../traits/traits_concepts.hpp"
#    include "../server_concepts.hpp"
#    include "./posix_connection.hpp"
#    include "./posix_constants.hpp"
#    include "./posix_thread_pool.hpp"

#    include <netdb.h>
#    include <sys/socket.h>
#    include <sys/types.h>

namespace webpp::posix {

    struct bindable_endpoint {
        addrinfo* endpoint;

        bool is_ipv6() const noexcept {

        }

        bool is_ipv4() const noexcept {

        }


    };


    /**
     * This class has the ability to obtain all the IP addresses and Port number that the server can bind to
     */
    struct bindable_endpoints {
      private:
        addrinfo hints{.ai_flags     = AI_PASSIVE,  // passive to make it a server not a client
                       .ai_family    = AF_UNSPEC,   // enable IPv6 and IPv4
                       .ai_socktype  = SOCK_STREAM, // TCP only
                       .ai_protocol  = 0,
                       .ai_addrlen   = 0,
                       .ai_addr      = nullptr,
                       .ai_canonname = nullptr,
                       .ai_next      = nullptr}


        // yes, we're going to use std::string, we require "char", and not using allocators does not affect
        // performance that much since this part of code is only needed to start the application and
        // we don't care about its performance.
        stl::string _service; // or port
        addrinfo*   _result = nullptr;

      public:
        bindable_endpoints(istl::Stringifiable auto&& v_service     = "http") noexcept
          : _service{istl::to_string(stl::forward<decltype(v_service)>(v_service))} {}

        bindable_endpoints(bindable_endpoints const&) =
          delete; // I don't wanna deal with memory management for now
        bindable_endpoints(bindable_endpoints&&) noexcept = default;

        ~bindable_endpoints() noexcept {
            // don't need to free anything in "hints" because it's pointers are always nullptr
            if (result)
                freeaddrinfo(result);
        }

        const addrinfo* result() const noexcept {
            return _result;
        }

        const stl::string& service() const noexcept {
            return _service;
        }

        void resolve() noexcept {
            if (result == nullptr) {
                int res = getaddrinfo(nullptr, service.data(), &hints, &result);
                webpp_assert(res == 0, stl::format("posix::getaddrinfo error: {}", gai_strerror(res)));
            }
        }

        void enable_ipv6() noexcept {
            switch (hints.ai_family) {
                case AF_INET6:
                case AF_UNSPEC: break; // nothing to do
                case AF_INET: hints.ai_family = AF_UNSPEC; break;
            }
        }

        void disable_ipv6() noexcept {
            webpp_assert(hints.ai_family != AF_INET6, "cannot disable both ipv4 and ipv6; "
                                                      "enable ipv4 before disabling ipv6 to "
                                                      "prevent this error from happening.");
            hints.ai_family = AF_INET;
        }

        void enable_ipv4() noexcept {
            switch (hints.ai_family) {
                case AF_INET:
                case AF_UNSPEC: break; // nothing to do
                case AF_INET6: hints.ai_family = AF_UNSPEC; break;
            }
        }

        void disable_ipv4() noexcept {
            webpp_assert(hints.ai_family != AF_INET, "cannot disable both ipv4 and ipv6; "
                                                     "enable ipv6 before disabling ipv4 to "
                                                     "prevent this error from happening.");
            hints.ai_family = AF_INET6;
        }
    };


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
        using thread_pool_type = ThreadPoolType;

      private:
        static constexpr auto                      logger_cat = "Posix/Server";
        istl::vector<traits_type, connection_type> connections;
        istl::vector<traits_type, acceptor_type>   acceptors;
        thread_pool_type                           pool{};

      public:
        bindable_endpoints endpoints;

        posix_server(auto&&... args)
          : etraits{stl::forward<decltype(args)>(args)...},
            connections{etraits::get_allocator()} acceptors{etraits::get_allocator()} {};

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

        void start_accepting() noexcept {}

        /**
         * Initialize the connections based on the endpoints
         */
        void bind() noexcept {
            endpoints.resolve(); // resolve the servers

            // looping over the results of a /etc/host or DNS query
            for (auto* it = endpoints.result(); it != NULL; it = it->ai_next) {
                socket_type sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                if (sock == -1) {
                    etraits::logger.warning(
                      logger_cat,
                      stl::format("Can't open a socket for {}:{}; trying the next one if exists",
                                  ep.server_name(), ep.service()),
                      errno);
                    continue;
                }

                int optval = 1;
                if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
                    etraits::logger.warning(logger_cat,
                                            "We weren't able to set necessary options for the specified socket",
                                            errno
                                            );
                    close(sock);
                    continue;
                }

                if (bind(sock, it->ai_addr, it->ai_addrlen) == -1) {
                    etraits::logger.warning(
                      logger_cat,
                      stl::format("Can't bind to a socket for {}:{}; trying the next one if exists",
                                  ep.server_name(), ep.service()),
                      errno);
                    close(sock); // close it because for some reason we can't bind to it
                    continue;
                }

                connections.emplace_back(sock); // pass the socket to a connection
            }
        }

      public:
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
                          etraits::logger.info(logger_cat,
                                               "Finished all the IO tasks in the thread successfully.");
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

} // namespace webpp::posix

#endif // webpp_posix

#endif // WEBPP_INTERFACES_COMMON_SERVER_H
