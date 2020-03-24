#ifndef WEBPP_HTTP_PARSER_H
#define WEBPP_HTTP_PARSER_H


#include <stdexcept>
namespace webpp::http {


  template <std::size_t version>
  void parse_headers(std::string_view const& headers) {
    throw std::invalid_argument("You need to specify a valid HTTP version.");
  }



}


#endif
