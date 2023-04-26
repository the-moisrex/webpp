#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../../../configs/constants.hpp"
#include "../../../libs/asio.hpp"
#include "../../../memory/object.hpp"
#include "../../../std/format.hpp"
#include "../../../std/string_view.hpp"
#include "../../../traits/enable_traits.hpp"
#include "../../../uri/uri.hpp"
#include "../../http_concepts.hpp"
#include "../../request.hpp"
#include "../../version.hpp"
#include "beast_request.hpp"
#include "beast_string_body.hpp"

#include <list>
#include <mutex>
#include <thread>

// clang-format off
#include asio_include(ip/address)
#include asio_include(post)
#include asio_include(thread_pool)
#include asio_include(ip/tcp)
#include asio_include(signal_set)
#include asio_include(strand)
// clang-format on

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace webpp::http::beast_proto {


    template <typename ServerT>
    struct http_worker : enable_traits<typename ServerT::etraits> {
        using server_type         = ServerT;
        using etraits             = enable_traits<typename server_type::etraits>;
        using duration            = typename server_type::duration;
        using acceptor_type       = typename server_type::acceptor_type;
        using traits_type         = typename server_type::traits_type;
        using root_extensions     = typename server_type::root_extensions;
        using endpoint_type       = asio::ip::tcp::endpoint;
        using steady_timer        = asio::steady_timer;
        using request_type        = simple_request<server_type, beast_request>;
        using buffer_type         = boost::beast::flat_buffer;
        using allocator_pack_type = typename server_type::allocator_pack_type;
        using request_header_type = typename request_type::headers_type;
        using request_body_type   = typename request_type::body_type;
        using char_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char>;
        using fields_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char>;
        using beast_fields_type   = boost::beast::http::basic_fields<fields_allocator_type>;
        using string_type         = traits::general_string<traits_type>;
        using beast_body_type     = string_body_of<string_type>;
        using beast_response_type = boost::beast::http::response<beast_body_type, beast_fields_type>;
        using beast_response_serializer_type =
          boost::beast::http::response_serializer<beast_body_type, beast_fields_type>;
        using socket_type      = asio::ip::tcp::socket;
        using stream_type      = boost::beast::tcp_stream;
        using string_view_type = traits::string_view<traits_type>;

        using beast_request_type = boost::beast::http::request<beast_body_type, beast_fields_type>;
        using beast_request_parser_type =
          boost::beast::http::request_parser<beast_body_type, char_allocator_type>;

        static constexpr auto log_cat = "BeastWorker";


        static_assert(HTTPRequestHeaders<request_header_type>,
                      "Mistakes has been made in request headers type.");
        static_assert(HTTPRequest<request_type>, "Request type should match HTTPRequest concept.");



      private:
        stl::optional<stream_type>                    stream{stl::nullopt};
        stl::optional<beast_response_type>            bres{stl::nullopt};
        stl::optional<beast_response_serializer_type> str_serializer{stl::nullopt};
        server_type*                                  server;
        stl::optional<request_type>                   req{stl::nullopt};
        stl::optional<beast_request_parser_type>      parser{stl::nullopt};
        buffer_type buf{default_buffer_size}; // fixme: see if this is using our allocator

        template <typename StrT>
        constexpr string_view_type string_viewify(StrT&& str) const noexcept {
            return istl::string_viewify_of<string_view_type>(stl::forward<StrT>(str));
        }


      public:
        http_worker(http_worker const&)                = delete;
        http_worker(http_worker&&) noexcept            = delete;
        http_worker& operator=(http_worker const&)     = delete;
        http_worker& operator=(http_worker&&) noexcept = delete;
        ~http_worker()                                 = default;

        http_worker(server_type* in_server)
          : etraits{*in_server},
            server{in_server},
            req{*server},
            parser{
              stl::in_place,
              stl::piecewise_construct,
              stl::make_tuple(), // body args
              stl::make_tuple(
                alloc::featured_alloc_for<alloc::sync_pool_features, beast_fields_type>(*this)) // fields args
            } {}

        /**
         * Running async_read_request directly in the constructor will not make
         * make_shared (or alike) functions work properly.
         */
        void set_socket(socket_type&& in_sock) {
            stream.emplace(stl::move(in_sock));
        }

        void start() noexcept {
            async_read_request();
        }

        [[nodiscard]] bool is_idle() noexcept {
            return !stream.has_value();
        }

      private:
        void make_beast_response() noexcept {
            using std::swap;
            using stl::swap;

            // putting the beast's request into webpp's request
            req->set_beast_parser(*parser);

            HTTPResponse auto res        = server->call_app(*req);
            using response_type          = stl::remove_cvref_t<decltype(res)>;
            using body_type              = typename response_type::body_type;
            using beast_body_string_type = typename beast_body_type::value_type;
            using beast_char_type        = typename beast_body_string_type::value_type;

            // putting the user's response into beast's response
            bres.emplace();
            res.calculate_default_headers();
            bres->version(parser->get().version());
            for (auto const& h : res.headers) {
                bres->set(h.name, h.value);
            }

            if constexpr (TextBasedBodyReader<body_type>) {
                if constexpr (stl::same_as<body_type, typename beast_body_type::value_type>) {
                    swap(bres->body(), res.body);
                } else {
                    bres->body().replace(0,
                                         bres->body().size(),
                                         static_cast<beast_char_type const*>(res.body.data()),
                                         res.body.size() * sizeof(beast_char_type) /
                                           sizeof(istl::char_type_of<body_type>));
                }
            } else if constexpr (BlobBasedBodyReader<body_type>) {
                // todo: string optimizations
                while (stl::streamsize read_size =
                         res.body.read(buf.data(), static_cast<stl::streamsize>(buf.size()))) {
                    bres->body().append(buf.data(), read_size);
                }
            } else if constexpr (StreamBasedBodyReader<body_type>) {
                if constexpr (stl::same_as<body_type, beast_body_string_type> &&
                              requires { res.body.str(); }) {
                    swap(bres->body(), res.body.str());
                } else if constexpr (requires {
                                         res.body.tellp();
                                         res.body.seekg(0);
                                         res.body.rdbuf();
                                     }) {
                    bres->body().resize(res.body.tellp());
                    auto g = res.body.tellg();
                    res.body.seekg(0);
                    res.body.rdbuf()->sgetn(bres->body().data(), res.body.tellp());
                    res.body.seekg(g);
                } else {
                    res.body >> bres->body();
                }
            } else {
                static_assert_false(body_type,
                                    "We don't know how to read your response's body "
                                    "thus we don't know how to send it to the user.");
            }
            // bres.content_length(res.body.size());
            bres->prepare_payload();
            str_serializer.emplace(*bres);
        }


        // Asynchronously receive a complete request message.
        void async_read_request() noexcept {
            stream->expires_after(server->timeout);
            boost::beast::http::async_read(
              *stream,
              buf,
              *parser,
              [this](boost::beast::error_code ec, [[maybe_unused]] std::size_t bytes_transferred) {
                  if (!ec) [[likely]] {
                      async_write_response();
                  } else [[unlikely]] {

                      // This means they closed the connection
                      if (ec == boost::beast::http::error::end_of_stream) {
                          // try sending shutdown signal
                          // don't need to log if it fails
                          stream->socket().shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                          reset();
                      } else {

                          this->logger.warning(log_cat, "Connection error.", ec);

                          // if we don't reset here, the connection will hang if there are too many concurrent
                          // connections for some reason.
                          // fixme: are we hard-closing the connection without letting the shutdown signal to be sent?
                          reset();
                      }
                  }
              });
        }


        void async_write_response() noexcept {
            make_beast_response();
            boost::beast::http::async_write(
              *stream,
              *str_serializer,
              [this](boost::beast::error_code ec, stl::size_t) noexcept {
                  if (ec) [[unlikely]] {
                      this->logger.warning(log_cat, "Write error on socket.", ec);
                  } else {
                      // todo: check if we need the else part of this condition to be an else stmt.
                      stream->socket().shutdown(asio::ip::tcp::socket::shutdown_send, ec);
                      if (ec) [[unlikely]] {
                          this->logger.warning(log_cat, "Error on sending shutdown into socket.", ec);
                      }
                  }
                  reset();
              });
        }


      public:
        void reset() noexcept {

            // todo: half of these things can be yanked out with the help of allocators
            boost::beast::error_code ec;
            stream->socket().close(ec);
            if (ec) [[unlikely]] {
                this->logger.warning(log_cat, "Error on closing the connection.", ec);
            }

            // destroy the request type + be ready for the next request
            req.emplace(*server);
            parser.emplace(
              stl::piecewise_construct,
              stl::make_tuple(), // body args
              stl::make_tuple(
                alloc::featured_alloc_for<alloc::sync_pool_features, beast_fields_type>(*this)) // fields args
            );



            str_serializer.reset();
            bres.reset();

            // Sleep indefinitely until we're given a new deadline.
            stream->expires_never();

            stream.reset(); // go in the idle mode
        }


        void stop() noexcept {
            if (stream) {
                stream->cancel();
            }
        }
    };


    /**
     * A single thread worker which will include multiple http workers.
     * More info:
     *   https://stackoverflow.com/a/63717201/4987470
     */
    template <typename ServerT>
    struct thread_worker {
        using server_type                           = ServerT;
        using etraits                               = typename server_type::etraits;
        using allocator_pack_type                   = typename etraits::allocator_pack_type;
        using http_worker_type                      = http_worker<server_type>;
        static constexpr auto worker_alloc_features = alloc::feature_pack{alloc::sync};
        using http_worker_allocator_type =
          typename allocator_pack_type::template best_allocator<worker_alloc_features, http_worker_type>;
        using http_workers_type = stl::list<http_worker_type, http_worker_allocator_type>;
        using socket_type       = asio::ip::tcp::socket;

        static constexpr auto log_cat = "Beast";

        thread_worker(thread_worker const&)                = delete;
        thread_worker(thread_worker&&) noexcept            = delete;
        thread_worker& operator=(thread_worker const&)     = delete;
        thread_worker& operator=(thread_worker&&) noexcept = delete;
        ~thread_worker()                                   = default;

        thread_worker(server_type& input_server)
          : server(&input_server),
            http_workers{alloc::featured_alloc_for<worker_alloc_features, http_workers_type>(*server)} {
            for (stl::size_t i = 0ul; i != server->http_worker_count; ++i) {
                http_workers.emplace_back(server);
            }
            worker = http_workers.begin();
        }


        void start_work(socket_type&& sock) {
            http_worker_type* worker_ptr = nullptr;
            {
                stl::scoped_lock    lock{worker_mutex};
                worker_ptr = worker.operator->();
                worker_ptr->set_socket(stl::move(sock));
                next_worker();
            }
            worker_ptr->start();
        }

        void stop() {
            for (auto& hworker : http_workers) {
                hworker.stop();
            }
        }

      private:
        // get the next available worker
        void next_worker() noexcept {
            // todo: a cooler algorithm can be used here, right? You can even give the user a choice
            do {
                ++worker;
                if (worker == http_workers.end()) {
                    worker = http_workers.begin();
                }
            } while (!worker->is_idle());
        }

        server_type*                         server;
        http_workers_type                    http_workers;
        typename http_workers_type::iterator worker;
        stl::mutex                           worker_mutex;
    };

} // namespace webpp::http::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
