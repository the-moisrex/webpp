#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../libs/asio.hpp"
// clang-format off
#include asio_include(ip/tcp)
// clang-format on

#include "../server_concepts.hpp"
#include "../../traits/traits_concepts.hpp"
#include "./asio_thread_pool.hpp"
#include "./asio_connection.hpp"
#include "./asio_constants.hpp"
#include "../../std/vector.hpp"
#include "../../traits/enable_traits.hpp"

#include <memory>

namespace webpp {

    /**
     * This class is the server and the connection manager.
     */
    template <Traits TraitsType, SessionManager SessionType, ThreadPool ThreadPoolType = asio_thread_pool>
    struct asio_server : public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using etraits          = enable_traits<traits_type>;
        using session_type     = SessionType;
        using connection_type  = asio_connection<traits_type, session_type>;
        using socket_type      = asio::ip::tcp::socket;
        using endpoint_type    = asio::ip::tcp::endpoint;
        using acceptor_type    = asio::ip::tcp::acceptor;
        using io_context_type  = asio::io_context;
        using thread_pool_type = ThreadPoolType;


        // I share this publicly because I know this file will not be used in a
        // header file so the final user doesn't have access to this class.
        io_context_type io;

      private:
        static constexpr auto                      logger_cat = "Asio/Server";
        istl::vector<traits_type, connection_type> connections;
        istl::vector<traits_type, acceptor_type>   acceptors;
        thread_pool_type                           pool{};

      public:
        asio_server(auto&&...args)
          : etraits{stl::forward<decltype(args)>(args)...},
            connections{etraits::get_allocator()}
            acceptors{etraits::get_allocator()}
            {};

      private:
        void accept(asio::error_code const& ec, socket_type socket) noexcept {
            // todo: check whether or not the server was stopped by a signal

            // todo: before this completes, handler had a chance to run
            //                                    if (!acceptor.is_open()) {
            //                                        return;
            //                                    }

            if (!ec) {
                connections.emplace_back(stl::move(socket));
            } else {
                logger.info(logger_cat, "Could not accept the user.", ec);
            }
        }

        void start_accepting() noexcept {
            // todo: locks and mutexes
            acceptors.emplace_back(io);
            auto acceptor = acceptors.back();
            acceptor.async_accept([this](asio::error_code const& ec, socket_type socket) noexcept {
                accept(ec, stl::move(socket));
                if (!io.stopped()) {
                    start_accepting();
                }
            });
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
            io.stop();
        }
    };

} // namespace webpp

#endif // WEBPP_INTERFACES_COMMON_SERVER_H
