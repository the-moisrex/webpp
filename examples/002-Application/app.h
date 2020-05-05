// Created by moisrex on 5/5/20.

#ifndef WEBPP_APP_H
#define WEBPP_APP_H

#include "./common.h"

#include <webpp/application>


struct app : webpp::application<traits, iface, router> {};

#endif // WEBPP_APP_H
