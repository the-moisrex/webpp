// Created by moisrex on 7/20/19.

#ifndef WEBPP_STRING_VIEW_H
#define WEBPP_STRING_VIEW_H

#include "../meta.h"

//#include <boost/config.hpp>
//#if defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
#ifdef CXX17
#  include <string_view>
#else
#  include <string>
   namespace std {
       using string_view = const ::std::string&;
   }
#endif


#endif // WEBPP_STRING_VIEW_H
