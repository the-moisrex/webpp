#ifndef WEBPP_INTERFACES_COMMON_SERVER_H
#define WEBPP_INTERFACES_COMMON_SERVER_H

#include "../../namespace.hpp"
// clang-format off
#include asio(ip/tcp)
// clang-format on

#include "../../traits/traits_concepts.hpp"
#include "./asio_thread_pool.hpp"
#include "./asio_connection.hpp"
#include "./asio_constants.hpp"
#include "../../std/vector.hpp"

#include <memory>

namespace webpp {

    /**
     * This class is the server and the connection manager.
     */
    template <Traits TraitsType, typename SessionType>
    struct asio_server {
        using traits_type      = TraitsType;
        using session_type     = SessionType;
        using connection_type  = connection<session_type>;
        using socket_type      = asio::ip::tcp::socket;
        using endpoint_type    = asio::ip::tcp::endpoint;
        using acceptor_type    = asio::ip::tcp::acceptor;
        using io_context_type  = asio::io_context;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using thread_pool_type = typename TraitsType::thread_pool_type;


        // I share this publicly because I know this file will not be used in a
        // header file so the final user doesn't have access to this class.
        io_context_type io;

      private:
        static constexpr auto                      logger_cat = "Asio/Server";
        istl::vector<traits_type, connection_type> connections;
        istl::vector<traits_type, acceptor_type>   acceptors;
        thread_pool_type                           pool{};
        [[no_unique_address]] logger_ref           logger;

      public:
        template <
          typename AllocatorType = stl::remove_cvref_t<decltype(decltype(connections)::get_allocator())>>
        explicit asio_server(logger_ref           logger    = logger_type{},
                             AllocatorType const& allocator = Allocator{}) noexcept
          : connections{allocator},
            acceptors{allocator},
            logger{logger} {};

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
        void run() noexcept {
            pool.post(
              [this]() {
                  // Run until the tasks finishes normally.
                  // Don't worry, we'll accept another connection when we finish one of them
                  for (;;) {
                      try {
                          start_accepting();
                          logger.info(logger_cat, "Starting running IO tasks in a thread.");
                          io.run();
                          logger.info(logger_cat, "Finished all the IO tasks in the thread successfully.");
                          break;
                      } catch (stl::exception const& err) {
                          logger.error(logger_cat, "Server tasks finished with errors.", err);
                      }
                  }
              },
              connections.get_allocator());
        }

        void stop() noexcept {
            // this will stop all threads
            logger.info(logger_cat, "Stopping all the IO tasks.");
            io.stop();
        }
    };

} // namespace webpp

#endif // WEBPP_INTERFACES_COMMON_SERVER_H
