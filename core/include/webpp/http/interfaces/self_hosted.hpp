// Created by moisrex on 9/4/20.

#ifndef WEBPP_SELF_HOSTED_HPP
#define WEBPP_SELF_HOSTED_HPP

#include "../../../version.hpp"
#include "../../server/server_concepts.hpp"
#include "../../std/string_view.hpp"
#include "../application.hpp"
#include "../request.hpp"
#include "../response.hpp"
#include "./shosted/self_hosted_session_manager.hpp"

namespace webpp {

    template <Traits TraitsType, typename REL, Interface IfaceType>
    struct self_hosted_request : public REL, enable_traits<TraitsType> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using interface_type         = stl::remove_cvref_t<IfaceType>;
        using request_extension_list = REL;
        using allocator_type   = typename traits_type::template allocator<typename traits_type::char_type>;
        using application_type = typename interface_type::application_type;
        using logger_type      = typename traits_type::logger_type;
        using logger_ref       = typename logger_type::logger_ref;
        using etraits          = enable_traits<traits_type>;


        template <typename AllocType>
        self_hosted_request(logger_ref logger = logger_type{}, auto const& alloc = AllocType{}) noexcept
          : etraits(logger, alloc) {}


#define WEBPP_SHOSTED_HEADER(name, value)                  \
    [[nodiscard]] stl::string_view name() const noexcept { \
        return interface_type::header(value);              \
    }


#undef WEBPP_SHOSTED_HEADER

#define WEBPP_SHOSTED_STR_METHOD(name, value)         \
    [[nodiscard]] stl::string name() const noexcept { \
        return value;                                 \
    }

        // todo: add openssl or other stuff's version as well here
        WEBPP_SHOSTED_STR_METHOD(server_software, stl::format("WEB++/{}", webpp_version));

#undef WEBPP_SHOSTED_STR_METHOD
    };

    template <ServerTraits ServerTraitsType, Application App, ExtensionList EList = empty_extension_pack>
    struct self_hosted : public enable_traits<typename ServerTraitsType::traits_type> {
      public:
        using server_traits_type = SeverTraitsType;
        using traits_type        = typename server_traits_type::traits_type;
        using application_type   = App;
        using extension_list     = stl::remove_cvref_t<EList>;
        using interface_type     = self_hosted<traits_type, application_type, extension_list>;
        using str_view_type      = typename TraitsType::string_view_type;
        using str_type           = typename TraitsType::string_type;
        using ostream_t          = typename TraitsType::ostream_type;
        using request_type = simple_request<traits_type, self_hosted_request, interface_type, extension_list>;
        using allocator_type           = typename request_type::allocator_type;
        using logger_type              = typename traits_type::logger_type;
        using logger_ref               = typename logger_type::logger_ref;
        using etraits                  = enable_traits<traits_type>;
        using application_wrapper_type = application_wrapper<traits_type, application_type>;
        using server_type              = typename server_traits_type::template server_type<
          shosted::self_hosted_session_manager<traits_type, request_type>>;

        server_type              server;
        application_wrapper_type app;

        template <typename AllocType = allocator_type>
        self_hosted(logger_ref logger = logger_type{}, AllocType const& alloc = AllocType{})
          : etraits{logger, alloc},
            app{logger, alloc} {}

        void operator()() noexcept {
            server();
        }
    };

} // namespace webpp

#endif // WEBPP_SELF_HOSTED_HPP
