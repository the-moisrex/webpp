#ifndef WEBPP_FAKE_INTERFACE_H
#define WEBPP_FAKE_INTERFACE_H

#include "../core/include/webpp/convert/casts.hpp"
#include "../core/include/webpp/http/app_wrapper.hpp"
#include "../core/include/webpp/http/protocols/common/common_protocol.hpp"
#include "../core/include/webpp/http/protocols/common/common_request.hpp"
#include "../core/include/webpp/http/response.hpp"
#include "../core/include/webpp/http/routes/router.hpp"
#include "../core/include/webpp/std/string_view.hpp"
#include "../core/include/webpp/strings/to_case.hpp"
#include "../core/include/webpp/traits/enable_traits.hpp"
#include "../core/include/webpp/traits/std_traits.hpp"
#include "./common_pch.hpp"
#include "webpp/application/request.hpp"



namespace webpp {
    using namespace webpp::http;


    // I'm not using "Protocol" here because it's most likely a non-complete-type when it's passed
    template <Traits TraitsType, typename REL>
    struct fake_proto_request : public common_request<TraitsType, REL> {
        using super       = common_request<TraitsType, REL>;
        using traits_type = TraitsType;
        using string_type = traits::general_string<traits_type>;
        using string_view = traits::string_view<traits_type>;

        stl::map<string_type, string_type> data{};

        template <typename... Args>
        fake_proto_request(Args&&... args) noexcept : super{stl::forward<Args>(args)...} {}
        fake_proto_request(fake_proto_request const&)     = default;
        fake_proto_request(fake_proto_request&&) noexcept = default;


        string_view get_data(auto&& str) noexcept {
            if (auto res = stl::find(data.begin(), data.end(), str)) {
                return *res;
            } else {
                return "";
            }
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


        [[nodiscard]] string_view server_port() const noexcept {
            return get_data("SERVER_PORT");
        }


        [[nodiscard]] string_view request_method() const noexcept {
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


        [[nodiscard]] string_view request_uri() const noexcept {
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


        [[nodiscard]] string_view header(string_view const& name) const noexcept {
            return this->header(stl::string(name));
        }


        [[nodiscard]] string_view headers() const noexcept {
            return this->headers();
        }


        [[nodiscard]] string_view body() const noexcept {
            return this->body();
        }
    };


    template <Traits TraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct fake_proto : public common_protocol<TraitsType, App, EList> {
        using super        = common_protocol<TraitsType, App, EList>;
        using traits_type  = TraitsType;
        using request_type = simple_request<traits_type, EList, fake_proto_request>;

        request_type req;

        template <typename... Args>
        fake_proto(Args&&... args) noexcept
          : super{stl::forward<Args>(args)...},
            req{this->logger, this->get_allocator()} {}


        void operator()() noexcept {
            auto res = app(req);
            res.calculate_default_headers();
            auto header_str = res.headers.str();
            auto str        = res.body.str();

            stl::stringstream data;
            data << "Status: " << res.headers.status_code << " "
                 << status_reason_phrase(res.headers.status_code) << "\r\n";

            data << header_str;
            data << str;
            // todo: what should I do with this request
        }
    };


} // namespace webpp

#endif
