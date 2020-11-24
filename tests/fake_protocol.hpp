#ifndef WEBPP_FAKE_INTERFACE_H
#define WEBPP_FAKE_INTERFACE_H

#include "./common_pch.hpp"

// clang-format off
#include webpp_include(application/application_concepts)
#include webpp_include(convert/casts)
#include webpp_include(http/app_wrapper)
#include webpp_include(http/protocols/protocol_concepts)
#include webpp_include(http/request)
#include webpp_include(http/response)
#include webpp_include(http/routes/router)
#include webpp_include(std/string_view)
#include webpp_include(strings/to_case)
#include webpp_include(traits/enable_traits)
#include webpp_include(traits/std_traits)
// clang-format on



namespace webpp {


    // I'm not using "Protocol" here because it's most likely a non-complete-type when it's passed
    template <Traits TraitsType, typename REL, /* Protocol */ typename ProtoType>
    struct fake_proto_request : public stl::remove_cvref_t<REL>, public enable_traits<TraitsType> {
        using traits_type      = TraitsType;
        using protocol_type    = ProtoType;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename protocol_type::application_type;
        using string_type      = typename traits_type::string_type;
        using etraits          = enable_traits<traits_type>;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;

      public:
        fake_proto_request() noexcept : etraits{} {}
        fake_proto_request(logger_ref  logger_obj = logger_type{},
                           auto const& alloc      = allocator_type{}) noexcept
          : etraits{logger_obj, alloc} {}

        istl::map<traits_type, string_type, string_type> data{};

        stl::string_view get_data(auto&& str) noexcept {
            if (auto res = stl::find(data.begin(), data.end(), str)) {
                return *res;
            } else {
                return "";
            }
        }


        [[nodiscard]] stl::string_view server_software() const noexcept {
            return get_data("SERVER_SOFTWARE");
        }


        [[nodiscard]] stl::string_view server_name() const noexcept {
            return get_data("SERVER_NAME");
        }


        [[nodiscard]] stl::string_view gateway_interface() const noexcept {
            return get_data("GATEWAY_INTERFACE");
        }


        [[nodiscard]] stl::string_view server_protocol() const noexcept {
            return get_data("SERVER_PROTOCOL");
        }


        [[nodiscard]] stl::string_view server_port() const noexcept {
            return get_data("SERVER_PORT");
        }


        [[nodiscard]] stl::string_view request_method() const noexcept {
            return get_data("REQUEST_METHOD");
        }


        [[nodiscard]] stl::string_view path_info() const noexcept {
            return get_data("PATH_INFO");
        }


        [[nodiscard]] stl::string_view path_translated() const noexcept {
            return get_data("PATH_TRANSLATED");
        }


        [[nodiscard]] stl::string_view script_name() const noexcept {
            return get_data("SCRIPT_NAME");
        }


        [[nodiscard]] stl::string_view query_string() const noexcept {
            return get_data("QUERY_STRING");
        }


        [[nodiscard]] stl::string_view remote_host() const noexcept {
            return get_data("REMOTE_HOST");
        }


        [[nodiscard]] stl::string_view remote_addr() const noexcept {
            return get_data("REMOTE_ADDR");
        }


        [[nodiscard]] stl::string_view auth_type() const noexcept {
            return get_data("AUTH_TYPE");
        }


        [[nodiscard]] stl::string_view remote_user() const noexcept {
            if (auto a = get_data("REMOTE_USER"); !a.empty())
                return a;
            return get_data("AUTH_USER");
        }


        [[nodiscard]] stl::string_view auth_user() const noexcept {
            if (auto a = get_data("AUTH_USER"); !a.empty())
                return a;
            return get_data("REMOTE_USER");
        }


        [[nodiscard]] stl::string_view remote_ident() const noexcept {
            return get_data("REMOTE_IDENT");
        }


        [[nodiscard]] stl::string_view request_scheme() const noexcept {
            return get_data("REQUEST_SCHEME");
        }


        [[nodiscard]] stl::string_view remote_port() const noexcept {
            return get_data("REMOTE_PORT");
        }


        [[nodiscard]] stl::string_view server_addr() const noexcept {
            return get_data("SERVER_ADDR");
        }


        [[nodiscard]] stl::string_view request_uri() const noexcept {
            return get_data("REQUEST_URI");
        }


        [[nodiscard]] stl::string_view content_type() const noexcept {
            return get_data("CONTENT_LENGTH");
        }


        [[nodiscard]] stl::string_view content_length() const noexcept {
            return get_data("CONTENT_LENGTH");
        }


        [[nodiscard]] stl::string_view document_root() const noexcept {
            return get_data("DOCUMENT_ROOT");
        }


        [[nodiscard]] stl::string_view https() const noexcept {
            return get_data("HTTPS");
        }


        [[nodiscard]] stl::string_view server_admin() const noexcept {
            return get_data("SERVER_ADMIN");
        }


        [[nodiscard]] stl::string_view path() const noexcept {
            return get_data("PATH");
        }


        [[nodiscard]] stl::string_view script_filename() const noexcept {
            return get_data("SCRIPT_FILENAME");
        }


        [[nodiscard]] stl::string_view header(stl::string_view const& name) const noexcept {
            return protocol_type::header(stl::string(name));
        }


        [[nodiscard]] stl::string_view headers() const noexcept {
            return protocol_type::headers();
        }


        [[nodiscard]] stl::string_view body() const noexcept {
            return protocol_type::body();
        }
    };


    template <Traits TraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct fake_proto : public enable_traits<TraitsType> {
      public:
        using traits_type      = TraitsType;
        using application_type = http_app_wrapper<traits_type, App>;
        using extension_list   = EList;
        using protocol_type    = fake_proto<traits_type, App, extension_list>;
        using str_view_type    = typename TraitsType::string_view_type;
        using str_type         = typename TraitsType::string_type;
        using request_type     = simple_request<traits_type, fake_proto_request, protocol_type, EList>;
        using allocator_type   = typename request_type::allocator_type;
        using etraits          = enable_traits<traits_type>;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;

        application_type app;
        request_type req;

        fake_proto(logger_ref logger_obj = logger_type{}, auto const& alloc = allocator_type{}) noexcept
          : etraits{logger_obj, alloc},
            app{logger_obj, alloc},
            req{logger_obj, alloc} {}


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
