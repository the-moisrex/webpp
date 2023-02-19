#ifndef WEBPP_TEST_FAKE_PROTOCOL_HPP
#define WEBPP_TEST_FAKE_PROTOCOL_HPP

#include "../core/include/webpp/convert/casts.hpp"
#include "../core/include/webpp/http/app_wrapper.hpp"
#include "../core/include/webpp/http/protocols/common/common_http_protocol.hpp"
#include "../core/include/webpp/http/request.hpp"
#include "../core/include/webpp/http/request_view.hpp"
#include "../core/include/webpp/http/response.hpp"
#include "../core/include/webpp/http/routes/router.hpp"
#include "../core/include/webpp/std/string_view.hpp"
#include "../core/include/webpp/strings/to_case.hpp"
#include "../core/include/webpp/traits/enable_traits.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"
#include "common_pch.hpp"



namespace webpp {
    using namespace webpp::http;


    // I'm not using "Protocol" here because it's most likely a non-complete-type when it's passed
    template <typename CommonHTTPRequest>
    struct fake_proto_request : public CommonHTTPRequest, http::details::request_view_interface {
        using super       = CommonHTTPRequest;
        using traits_type = typename super::traits_type;
        using string_type = traits::general_string<traits_type>;
        using string_view = traits::string_view<traits_type>;

        stl::map<string_type, string_type> data{};

      protected:
        using pstring_type = typename basic_request_view::string_type;

        template <typename T>
        [[nodiscard]] inline pstring_type pstringify(T&& str) const {
            return istl::stringify_of<pstring_type>(stl::forward<T>(str),
                                                    alloc::general_alloc_for<pstring_type>(*this));
        }

        // get the dynamic request object
        inline basic_request_view const& dreq() const noexcept {
            return static_cast<basic_request_view const&>(*this);
        }

        [[nodiscard]] pstring_type get_method() const override {
            return pstringify(this->method());
        }

        [[nodiscard]] pstring_type get_uri() const override {
            return pstringify(this->uri());
        }

        [[nodiscard]] http::version get_version() const noexcept override {
            return this->version();
        }

      public:
        using super::super;

        fake_proto_request(fake_proto_request const&)     = default;
        fake_proto_request(fake_proto_request&)           = default;
        fake_proto_request(fake_proto_request&&) noexcept = default;

        void reload() {
            for (auto const& [att, val] : data) {
                this->headers.emplace(att, val);
            }
        }

        string_view get_data(auto&& str) const noexcept {
            if (auto res = data.find(str); res != data.end()) {
                return res->second;
            } else {
                return "";
            }
        }

        constexpr bool is_ssl_available() noexcept {
            return false;
        }



        [[nodiscard]] string_view server_software() const noexcept {
            return get_data("SERVER_SOFTWARE");
        }


        [[nodiscard]] string_view server_name() const noexcept {
            return get_data("SERVER_NAME");
        }


        [[nodiscard]] string_view gateway_interface() const noexcept {
            return get_data("GATEWAY_INTERFACE");
        }


        [[nodiscard]] string_view server_protocol() const noexcept {
            return get_data("SERVER_PROTOCOL");
        }

        [[nodiscard]] http::version version() const noexcept {
            return http::version::from_server_protocol(server_protocol());
        }

        [[nodiscard]] string_view server_port() const noexcept {
            return get_data("SERVER_PORT");
        }


        [[nodiscard]] string_view method() const noexcept {
            return get_data("REQUEST_METHOD");
        }


        [[nodiscard]] string_view path_info() const noexcept {
            return get_data("PATH_INFO");
        }


        [[nodiscard]] string_view path_translated() const noexcept {
            return get_data("PATH_TRANSLATED");
        }


        [[nodiscard]] string_view script_name() const noexcept {
            return get_data("SCRIPT_NAME");
        }


        [[nodiscard]] string_view query_string() const noexcept {
            return get_data("QUERY_STRING");
        }


        [[nodiscard]] string_view remote_host() const noexcept {
            return get_data("REMOTE_HOST");
        }


        [[nodiscard]] string_view remote_addr() const noexcept {
            return get_data("REMOTE_ADDR");
        }


        [[nodiscard]] string_view auth_type() const noexcept {
            return get_data("AUTH_TYPE");
        }


        [[nodiscard]] string_view remote_user() const noexcept {
            if (auto a = get_data("REMOTE_USER"); !a.empty())
                return a;
            return get_data("AUTH_USER");
        }


        [[nodiscard]] string_view auth_user() const noexcept {
            if (auto a = get_data("AUTH_USER"); !a.empty())
                return a;
            return get_data("REMOTE_USER");
        }


        [[nodiscard]] string_view remote_ident() const noexcept {
            return get_data("REMOTE_IDENT");
        }


        [[nodiscard]] string_view request_scheme() const noexcept {
            return get_data("REQUEST_SCHEME");
        }


        [[nodiscard]] string_view remote_port() const noexcept {
            return get_data("REMOTE_PORT");
        }


        [[nodiscard]] string_view server_addr() const noexcept {
            return get_data("SERVER_ADDR");
        }


        [[nodiscard]] string_view uri() const noexcept {
            return get_data("REQUEST_URI");
        }


        [[nodiscard]] string_view content_type() const noexcept {
            return get_data("CONTENT_LENGTH");
        }


        [[nodiscard]] string_view content_length() const noexcept {
            return get_data("CONTENT_LENGTH");
        }


        [[nodiscard]] string_view document_root() const noexcept {
            return get_data("DOCUMENT_ROOT");
        }


        [[nodiscard]] string_view https() const noexcept {
            return get_data("HTTPS");
        }


        [[nodiscard]] string_view server_admin() const noexcept {
            return get_data("SERVER_ADMIN");
        }


        [[nodiscard]] string_view path() const noexcept {
            return get_data("PATH");
        }


        [[nodiscard]] string_view script_filename() const noexcept {
            return get_data("SCRIPT_FILENAME");
        }


        [[nodiscard]] string_view get_header(string_view const& name) const noexcept {
            return this->header(stl::string(name));
        }


        [[nodiscard]] string_view get_headers() const noexcept {
            return this->headers();
        }


        [[nodiscard]] string_view get_body() const noexcept {
            return this->body();
        }
    };

    template <typename Proto>
    struct fake_request_body_communicator {
        using size_type = stl::streamsize;
        using byte_type = stl::byte;
        using char_type = char;

      private:
        stl::string_view content;

      public:
        [[nodiscard]] size_type read(byte_type* data, size_type count) const {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            stl::copy_n(content.data(), static_cast<stl::size_t>(count), reinterpret_cast<char_type*>(data));
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            return count;
        }

        [[nodiscard]] size_type size() const noexcept {
            return content.size();
        }
    };


    template <Traits TraitsType, Application App, RootExtensionList EList = empty_extension_pack>
    struct fake_proto : public common_http_protocol<TraitsType, App, EList> {
        using super               = common_http_protocol<TraitsType, App, EList>;
        using traits_type         = TraitsType;
        using root_extensions     = EList;
        using allocator_pack_type = traits::allocator_pack_type<traits_type>;
        using char_type           = traits::char_type<traits_type>;
        using fields_allocator_type =
          typename allocator_pack_type::template best_allocator<alloc::sync_pool_features, char_type>;
        // using fields_allocator_type = traits::general_allocator<traits_type, char_type>;
        using fields_provider           = header_fields_provider<traits_type, root_extensions>;
        using request_body_communicator = fake_request_body_communicator<fake_proto>;
        using request_headers_type      = simple_request_headers<fields_provider>;
        using request_body_type =
          simple_request_body<traits_type, root_extensions, request_body_communicator>;

        using request_type = simple_request<fake_proto_request, request_headers_type, request_body_type>;

        static_assert(HTTPRequest<request_type>, "request type is not request; why?");

        request_type req;

        template <typename... Args>
        fake_proto(Args&&... args) noexcept : super{stl::forward<Args>(args)...},
                                              req{*this} {}

        constexpr bool is_ssl_available() noexcept {
            return false;
        }


        int operator()() noexcept {
            req.body.content = "fake";
            auto res         = app(req);
            res.calculate_default_headers();
            auto header_str = res.headers.string();
            auto str        = res.body.string();

            stl::stringstream data;
            data << "Status: " << res.headers.status_code << " "
                 << status_reason_phrase(res.headers.status_code) << "\r\n";

            data << header_str;
            data << str;
            // todo: what should I do with this request

            return 0;
        }
    };


    struct fake_app {

        auto operator()([[maybe_unused]] auto&& req) {
            return false;
        }
    };

    static_assert(HTTPProtocol<fake_proto<default_traits, fake_app>>,
                  "FakeProto is not really a valid Protocol");


} // namespace webpp

#endif
