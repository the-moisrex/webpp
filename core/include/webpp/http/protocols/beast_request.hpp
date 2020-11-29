// Created by moisrex on 10/24/20.

#ifndef WEBPP_BEAST_REQUEST_HPP
#define WEBPP_BEAST_REQUEST_HPP

#include "../../std/string_view.hpp"
#include "../../traits/traits_concepts.hpp"
#include "./protocol_concepts.hpp"
#include "common/common_request.hpp"

namespace webpp {

    template <Traits TraitsType, typename REL, typename ProtoType>
    struct beast_request : public common_request<TraitsType, REL> {
        using traits_type            = stl::remove_cvref_t<TraitsType>;
        using protocol_type          = stl::remove_cvref_t<ProtoType>;
        using request_extension_list = REL;
        using string_type            = typename traits_type::string_type;
        using string_view_type       = typename traits_type::string_view_type;

      private:
        using super = common_request<TraitsType, REL>;

      public:
        template <typename... Args>
        beast_request(Args&&... args) noexcept : super(stl::forward<Args>(args)...) {}
    };

} // namespace webpp

#endif // WEBPP_BEAST_REQUEST_HPP
