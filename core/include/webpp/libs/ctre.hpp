// Created by moisrex on 7/27/20.

#ifndef WEBPP_CTRE_H
#define WEBPP_CTRE_H

#include <ctre.hpp>
#if (__cpp_nontype_template_parameter_class || (__cpp_nontype_template_args >= 201911L))
#define WEBPP_CTRE_WITH_TEMPLATE 1
#else
#define WEBPP_CTRE_WITH_LITERALS 1
#endif


#endif // WEBPP_CTRE_H
