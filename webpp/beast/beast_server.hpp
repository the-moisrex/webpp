#ifndef WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
#define WEBPP_HTTP_PROTO_BEAST_SERVER_HPP

#include "../configs/constants.hpp"
#include "../http/http_concepts.hpp"
#include "../http/http_version.hpp"
#include "../http/request.hpp"
#include "../libs/asio.hpp"
#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../traits/enable_traits.hpp"
#include "../uri/uri.hpp"
#include "beast_request.hpp"
#include "beast_string_body.hpp"

#include <list>
#include <mutex>
#include <thread>

#ifdef WEBPP_BOOST_ASIO
#    include <boost/asio/ip/address.hpp>
#    include <boost/asio/ip/tcp.hpp>
#    include <boost/asio/post.hpp>
#    include <boost/asio/signal_set.hpp>
#    include <boost/asio/strand.hpp>
#    include <boost/asio/thread_pool.hpp>
#else
#    include <asio/ip/address.hpp>
#    include <asio/ip/tcp.hpp>
#    include <asio/post.hpp>
#    include <asio/signal_set.hpp>
#    include <asio/strand.hpp>
#    include <asio/thread_pool.hpp>
#endif

#include <boost/beast/core.hpp>
#include <boost/beast/http/read.hpp>
#include <boost/beast/http/write.hpp>

namespace webpp::beast_proto {


    template <typename ServerT>
    struct http_worker : enable_traits<typename ServerT::etraits> {
        using server_type           = ServerT;
        using etraits               = enable_traits<typename server_type::etraits>;
        using duration              = typename server_type::duration;
        using acceptor_type         = typename server_type::acceptor_type;
        using traits_type           = typename server_type::traits_type;
        using endpoint_type         = asio::ip::tcp::endpoint;
        using steady_timer          = asio::steady_timer;
        using request_type          = typename server_type::request_type;
        using buffer_type           = boost::beast::flat_buffer;
        using request_header_type   = typename request_type::headers_type;
        using request_body_type     = typename request_type::body_type;
        using char_allocator_type   = traits::string_allocator<traits_type>;
        using fields_allocator_type = traits::string_allocator<traits_type>;
        using beast_fields_type     = boost::beast::http::basic_fields<fields_allocator_type>;
        using string_type           = traits::string<traits_type>;
        using beast_body_type       = string_body_of<string_type>;
        using beast_response_type   = boost::beast::http::response<beast_body_type, beast_fields_type>;
        using beast_response_serializer_type =
          boost::beast::http::response_serializer<beast_body_type, beast_fields_type>;
        using socket_type      = asio::ip::tcp::socket;
        using stream_type      = boost::beast::tcp_stream;
        using string_view_type = traits::string_view<traits_type>;

        using beast_request_type = boost::beast::http::request<beast_body_type, beast_fields_type>;
        using beast_request_parser_type =
          boost::beast::http::request_parser<beast_body_type, char_allocator_type>;

        static constexpr auto log_cat = "BeastWorker";


        static_assert(http::HTTPRequestHeaders<request_header_type>,
                      "Mistakes has been made in request headers type.");
        static_assert(http::HTTPRequest<request_type>, "Request type should match HTTPRequest concept.");



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

        explicit http_worker(server_type* in_server)
          : etraits{*in_server},
            server{in_server},
            req{*server},
            parser{
              stl::in_place,
              stl::piecewise_construct,
              stl::make_tuple(),                                       // body args
              stl::make_tuple(get_allocator<beast_fields_type>(*this)) // fields args
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
        template <http::TextBasedBodyReader BodyType>
        void set_response_body_string(BodyType& body) {
            using body_type              = stl::remove_cvref_t<BodyType>;
            using beast_body_string_type = typename beast_body_type::value_type;
            using beast_char_type        = typename beast_body_string_type::value_type;
            if constexpr (stl::same_as<body_type, typename beast_body_type::value_type>) {
                swap(bres->body(), body);
            } else {
                using body_char_type = stl::remove_pointer_t<stl::remove_cvref_t<decltype(body.data())>>;
                static constexpr stl::size_t char_type_size = sizeof(body_char_type);
                auto const                   body_size      = body.size();
                auto const                   body_data = static_cast<beast_char_type const*>(body.data());
                if (body_size == 0 || body_data == nullptr) {
                    return;
                }
                bres->body().replace(0,
                                     bres->body().size(),
                                     body_data,
                                     body_size * sizeof(beast_char_type) / char_type_size);
            }
        }

        template <http::CStreamBasedBodyReader BodyType>
        void set_response_body_cstream(BodyType& body) {
            using body_type = stl::remove_cvref_t<BodyType>;
            using byte_type = typename body_type::byte_type;
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            // NOLINTBEGIN(cppcoreguidelines-pro-type-member-init)
            stl::array<char, default_buffer_size> static_buf;
            while (stl::streamsize read_size = body.read(reinterpret_cast<byte_type*>(static_buf.data()),
                                                         static_cast<stl::streamsize>(static_buf.size())))
            {
                bres->body().append(static_buf.data(), static_cast<stl::size_t>(read_size));
            }
            // NOLINTEND(cppcoreguidelines-pro-type-member-init)
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        template <http::StreamBasedBodyReader BodyType>
        void set_response_body_stream(BodyType& body) {
            using body_type              = stl::remove_cvref_t<BodyType>;
            using beast_body_string_type = typename beast_body_type::value_type;
            if constexpr (stl::same_as<body_type, beast_body_string_type> && requires { body.str(); }) {
                swap(bres->body(), body.str());
            } else if constexpr (requires {
                                     body.tellp();
                                     body.seekg(0);
                                     body.rdbuf();
                                 })
            {
                bres->body().resize(body.tellp());
                auto const g = body.tellg();
                body.seekg(0);
                body.rdbuf()->sgetn(bres->body().data(), body.tellp());
                body.seekg(g);
            } else {
                body >> bres->body();
            }
        }

        template <typename BodyType>
        void set_response_body(BodyType& body) {
            using body_type = stl::remove_cvref_t<BodyType>;
            if constexpr (http::UnifiedBodyReader<body_type>) {
                switch (body.which_communicator()) {
                    using enum http::communicator_type;
                    case nothing: return;
                    case text_based: set_response_body_string(body); return;
                    case cstream_based: set_response_body_cstream(body); return;
                    case stream_based: set_response_body_stream(body); return;
                    default: stl::unreachable();
                }
            } else if constexpr (http::TextBasedBodyReader<body_type>) {
                set_response_body_string(body);
            } else if constexpr (http::CStreamBasedBodyReader<body_type>) {
                set_response_body_cstream(body);
            } else if constexpr (http::StreamBasedBodyReader<body_type>) {
                set_response_body_stream(body);
            } else {
                static_assert_false(body_type,
                                    "We don't know how to read your response's body "
                                    "thus we don't know how to send it to the user.");
            }
        }

        void make_beast_response() noexcept {
            // putting the beast's request into webpp's request
            req->set_beast_parser(*parser);

            http::HTTPResponse auto res = server->call_app(*req);

            // putting the user's response into beast's response
            bres.emplace();
            res.calculate_default_headers();
            bres->version(parser->get().version());
            for (auto const& hdr : res.headers) {
                bres->set(hdr.name, hdr.value);
            }

            // bres.content_length(res.body.size());
            set_response_body(res.body);
            bres->prepare_payload();
            str_serializer.emplace(*bres);
        }

        // Asynchronously receive a complete request message.
        void async_read_request() noexcept {
            stream->expires_after(server->timeout());
            boost::beast::http::async_read(
              *stream,
              buf,
              *parser,
              [this](boost::beast::error_code err, [[maybe_unused]] std::size_t bytes_transferred) {
                  if (!err) [[likely]] {
                      async_write_response();
                  } else [[unlikely]] {
                      // This means they closed the connection
                      if (err == boost::beast::http::error::end_of_stream) {
                          // try sending shutdown signal
                          // don't need to log if it fails
                          stream->socket().shutdown(asio::ip::tcp::socket::shutdown_send, err);
                          reset();
                      } else {
                          this->logger.warning(log_cat, "Connection error.", err);

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
              [this](boost::beast::error_code err, stl::size_t) noexcept {
                  if (err) [[unlikely]] {
                      this->logger.warning(log_cat, "Write error on socket.", err);
                  } else {
                      // todo: check if we need the else part of this condition to be an else stmt.
                      stream->socket().shutdown(asio::ip::tcp::socket::shutdown_send, err);
                      if (err) [[unlikely]] {
                          this->logger.warning(log_cat, "Error on sending shutdown into socket.", err);
                      }
                  }
                  reset();
              });
        }


      public:
        void reset() noexcept {
            // todo: half of these things can be yanked out with the help of allocators
            boost::beast::error_code err;
            stream->socket().close(err);
            if (err) [[unlikely]] {
                this->logger.warning(log_cat, "Error on closing the connection.", err);
            }

            // destroy the request type + be ready for the next request
            req.emplace(*server);
            parser.emplace(stl::piecewise_construct,
                           stl::make_tuple(),                                       // body args
                           stl::make_tuple(get_allocator<beast_fields_type>(*this)) // fields args
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
        using server_type                = ServerT;
        using etraits                    = typename server_type::etraits;
        using traits_type                = typename etraits::traits_type;
        using http_worker_type           = http_worker<server_type>;
        using http_worker_allocator_type = traits::allocator_type_of<traits_type, http_worker_type>;
        using http_workers_type          = stl::list<http_worker_type, http_worker_allocator_type>;
        using socket_type                = asio::ip::tcp::socket;

        static constexpr auto log_cat = "Beast";

        thread_worker(thread_worker const&)                = delete;
        thread_worker(thread_worker&&) noexcept            = delete;
        thread_worker& operator=(thread_worker const&)     = delete;
        thread_worker& operator=(thread_worker&&) noexcept = delete;
        ~thread_worker()                                   = default;

        explicit thread_worker(server_type& input_server)
          : server(&input_server),
            http_workers{get_allocator<http_workers_type>(*server)} {
            for (stl::size_t i = 0UL; i != server->http_worker_count; ++i) {
                http_workers.emplace_back(server);
            }
            worker = http_workers.begin();
        }

        void start_work(socket_type&& sock) {
            http_worker_type* worker_ptr; // NOLINT(cppcoreguidelines-init-variables)
            {
                [[maybe_unused]] stl::scoped_lock lock{worker_mutex};

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
            for (;;) {
                ++worker;
                if (worker == http_workers.end()) {
                    worker = http_workers.begin();
                }
                if (worker->is_idle()) {
                    break;
                }
            }
        }

        server_type*                         server;
        http_workers_type                    http_workers;
        typename http_workers_type::iterator worker;
        stl::mutex                           worker_mutex;
    };

} // namespace webpp::beast_proto

#endif // WEBPP_HTTP_PROTO_BEAST_SERVER_HPP
