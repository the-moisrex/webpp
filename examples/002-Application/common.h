// Created by moisrex on 5/5/20.

#ifndef WEBPP_COMMON_H
#define WEBPP_COMMON_H

#include <webpp/http/request>
#include <webpp/http/response>
#include <webpp/interfaces/cgi>
#include <webpp/traits/traits>

using traits   = webpp::std_tratis;
using iface    = webpp::cgi<traits>;
using request  = webpp::request_t<traits, iface>;
using response = webpp::response_t<traits, iface>;

#endif // WEBPP_COMMON_H
