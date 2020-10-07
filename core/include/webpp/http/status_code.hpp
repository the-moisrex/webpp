// Created by moisrex on 10/7/20.

#ifndef WEBPP_STATUS_CODE_HPP
#define WEBPP_STATUS_CODE_HPP

#include "../std/std.hpp"

#include <cstdint>

namespace webpp {

    using status_code_type = stl::uint_fast16_t;

    enum struct status_code : status_code_type {
        no_status                       = 0,
        switching_protocols             = 101,
        ok                              = 200,
        created                         = 201,
        accepted                        = 202,
        no_content                      = 204,
        partial_content                 = 206,
        multiple_choices                = 300,
        moved_permanently               = 301,
        found                           = 302,
        see_other                       = 303,
        not_modified                    = 304,
        moved_temporarily               = 307,
        bad_request                     = 400,
        unauthorized                    = 401,
        forbidden                       = 403,
        not_found                       = 404,
        request_entity_too_large        = 413,
        requested_range_not_satisfiable = 416,
        internal_server_error           = 500,
        not_implemented                 = 501,
        bad_gateway                     = 502,
        service_unavailable             = 503,
        version_not_supported           = 505
    };

} // namespace webpp

#endif // WEBPP_STATUS_CODE_HPP
