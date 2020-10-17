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
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

namespace webpp::posix {


    struct endpoint {
      private:
        addrinfo hints {
          .ai_flags    = 0,
          .ai_family   = AF_UNSPEC,   // enable IPv6 and IPv4
            .ai_socktype = SOCK_STREAM, // TCP only
            .ai_protocol = IPPROTO_TCP,
            .ai_addrlen  = 0,
            .ai_addr = nullptr,
            .ai_canonname = nullptr,
            .ai_next = nullptr
        }


        // yes, we're going to use std::string, we require "char", and not using allocators does not affect
        // performance that much since this part of code is only needed to start the application and
        // we don't care about its performance.
        stl::string _server_name;
        stl::string _service; // or port
        addrinfo* _result = nullptr;

      public:
        endpoint(istl::ConvertibleToString auto &&v_server_name = "localhost", istl::ConvertibleToString auto&& v_service = "http") noexcept
          : _server_name{istl::to_string(stl::forward<decltype(v_server_name)>(v_server_name))},
            _service{istl::to_string(stl::forward<decltype(v_service)>(v_service))} {}

        endpoint(endpoint const&) = delete; // I don't wanna deal with memory management for now
        endpoint(endpoint&&) noexcept = default;

        ~endpoint() noexcept {
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

        const stl::string& server_name() const noexcept {
            return _server_name;
        }

        void resolve() noexcept {
            if (result == nullptr) {
                int  res   = getaddrinfo(server_name.data(), service.data(), &hints, &result);
                webpp_assert(res == 0, stl::format("posix::getaddrinfo error: {}", gai_strerror(res)));
            }
        }

        void enable_ipv6() noexcept {
            switch (hints->ai_family) {
                case AF_INET6:
                case AF_UNSPEC:
                    break; // nothing to do
                case AF_INET:
                    hints->ai_family = AF_UNSPEC;
                    break;
            }
        }

        void disable_ipv6() noexcept {
            webpp_assert(hints->ai_family != AF_INET6, "cannot disable both ipv4 and ipv6; "
                                                                "enable ipv4 before disabling ipv6 to "
                                                                "prevent this error from happening.");
            hints->ai_family = AF_INET;
        }

        void enable_ipv4() noexcept {
            switch (hints->ai_family) {
                case AF_INET:
                case AF_UNSPEC:
                    break; // nothing to do
                case AF_INET6:
                    hints->ai_family = AF_UNSPEC;
                    break;
            }
        }

        void disable_ipv4() noexcept {
            webpp_assert(hints->ai_family != AF_INET,  "cannot disable both ipv4 and ipv6; "
                                                                "enable ipv6 before disabling ipv4 to "
                                                                "prevent this error from happening.");
            hints->ai_family = AF_INET6;
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
        stl::list<endpoint> endpoints;

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

        /**
         * Initialize the connections based on the endpoints
         */
        void bind() noexcept {
            if (endpoints.empty()) {
                etraits::logger.warning(logger_cat, "Call to bind without any endpoints specified.");
                return;
            }
            for (auto& ep : endpoints) {
                ep.resolve(); // resolve the servers

                // looping over the results of a /etc/host or DNS query
                for (auto* it = ep.result; it != NULL; it = it->ai_next) {
                    socket_type sock = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
                    if (sock == -1) {
                        etraits::logger.warning(logger_cat, stl::format("Can't open a socket for {}:{}; trying the next one if exists", ep.server_name(), ep.service()), errno);
                        continue;
                    }

                    if (bind(sock, it->ai_addr, it->ai_addrlen) == -1) {
                        etraits::logger.warning(logger_cat, stl::format("Can't bind to a socket for {}:{}; trying the next one if exists", ep.server_name(), ep.service()), errno);
                        close(sock); // close it because for some reason we can't bind to it
                        continue;
                    }

                    connections.emplace_back(sock); // pass the socket to a connection
                }
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
