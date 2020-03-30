// Created by moisrex on 7/20/19.

#ifndef WEBPP_META_H
#define WEBPP_META_H

#if __cplusplus >= 199711L
#define CXX98
#endif

#if __cplusplus >= 201103L
#define CXX11
#endif

#if __cplusplus >= 201402L
#define CXX14
#endif

#if __cplusplus >= 201703L
#define CXX17
#endif

// TODO: change this in the yaer 2020
#if __cplusplus > 201707L
#define CXX20
#endif

#endif // WEBPP_META_H
