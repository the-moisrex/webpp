// Created by moisrex on 9/23/23.

#ifndef WEBPP_COMPAT_HPP
#define WEBPP_COMPAT_HPP

#include <linux/version.h>

/// __kernel_timespec

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
typedef int __kernel_rwf_t;
#endif



/// __kernel_timespec

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
#    include <stdint.h>
struct __kernel_timespec {
    int64_t   tv_sec;
    long long tv_nsec;
};
/* <linux/time_types.h> is not available, so it can't be included */
#    define UAPI_LINUX_IO_URING_H_SKIP_LINUX_TIME_TYPES_H 1
#else
#    include <linux/time_types.h>
/* <linux/time_types.h> is included above and not needed again */
#    define UAPI_LINUX_IO_URING_H_SKIP_LINUX_TIME_TYPES_H 1
#endif




/// open_how

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 6, 0)
#    include <inttypes.h>
struct open_how {
    uint64_t flags;
    uint64_t mode;
    uint64_t resolve;
};
#else
#    include <linux/openat2.h>
#endif


/// Since webpp itself is using C++20,23 feature, glibc will have statx, so we don't need to check.

#endif // WEBPP_COMPAT_HPP
