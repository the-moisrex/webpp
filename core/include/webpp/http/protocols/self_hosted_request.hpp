// Created by moisrex on 10/24/20.

#ifndef WEBPP_SELF_HOSTED_REQUEST_HPP
#define WEBPP_SELF_HOSTED_REQUEST_HPP

#include "common/common_http_request.hpp"
#include "protocol_concepts.hpp"

namespace webpp::http::shosted {


    template <Traits TraitsType,
              typename SessionManager,
              typename ConnectionType,
              HTTPRequestExtensionParent REL,
              RootExtensionList          RootExtensions>
    struct self_hosted_request : public common_http_request<TraitsType, REL, RootExtensions>,
                                 private ConnectionType {
      private:
        using super = common_http_request<TraitsType, REL, RootExtensions>;

      public:
        using session_manager_type = SessionManager;

      private:
        session_manager_type& session;

      public:
        template <typename... Args>
        self_hosted_request(session_manager_type& _session, Args&&... args)
          : super(stl::forward<Args>(args)...),
            session{_session} {}


        //#define WEBPP_SHOSTED_HEADER(name, value)
        //    [[nodiscard]] string_view_type name() const noexcept {
        //        return protocol_type::header(value);
        //    }
        //
        //
        //#undef WEBPP_SHOSTED_HEADER
    };


} // namespace webpp::http::shosted

#endif // WEBPP_SELF_HOSTED_REQUEST_HPP
