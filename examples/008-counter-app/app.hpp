// Created by moisrex on 5/5/20.

#ifndef WEBPP_EXAMPLE_COUNTER_APP_HPP
#define WEBPP_EXAMPLE_COUNTER_APP_HPP

#include "configs.hpp"

#include <webpp/http/routes/dynamic_router.hpp>
#include <webpp/std/utility.hpp>


namespace website {

    using namespace webpp::http;
    using namespace webpp;


    struct app_impl;

    struct app : enable_traits<traits_type> {
        using etraits = enable_traits<traits_type>;
        stl::unique_ptr<app_impl> the_app;

        app(etraits const& et);

        dynamic_response<traits_type> operator()(dynamic_request<traits_type> const& req);
    };


} // namespace website


#endif
