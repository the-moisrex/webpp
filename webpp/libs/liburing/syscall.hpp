/* SPDX-License-Identifier: MIT */

#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <unistd.h>

/*
 * Don't put this below the #include "arch/$arch_syscall.hpp", that
 * file may need it.
 */
struct io_uring_params;

static inline void* ERR_PTR(intptr_t n) {
    return (void*) n;
}

static inline int PTR_ERR(const void* ptr) {
    return (int) (intptr_t) ptr;
}

static inline bool IS_ERR(const void* ptr) {
    return uring_unlikely((uintptr_t) ptr >= (uintptr_t) -4095UL);
}

#if defined(__x86_64__) || defined(__i386__)
#    include "arch/x86_syscall.hpp"
#elif defined(__aarch64__)
#    include "arch/aarch64_syscall.hpp"
#elif defined(__riscv) && __riscv_xlen == 64
#    include "arch/riscv64_syscall.hpp"
#else
#    include "arch/generic_syscall.hpp"
#endif

int io_uring_enter(unsigned int fd,
                   unsigned int to_submit,
                   unsigned int min_complete,
                   unsigned int flags,
                   sigset_t*    sig) {
    return __sys_io_uring_enter(fd, to_submit, min_complete, flags, sig);
}

int io_uring_enter2(unsigned int fd,
                    unsigned int to_submit,
                    unsigned int min_complete,
                    unsigned int flags,
                    sigset_t*    sig,
                    size_t       sz) {
    return __sys_io_uring_enter2(fd, to_submit, min_complete, flags, sig, sz);
}

int io_uring_setup(unsigned int entries, struct io_uring_params* p) {
    return __sys_io_uring_setup(entries, p);
}

int io_uring_register(unsigned int fd, unsigned int opcode, const void* arg, unsigned int nr_args) {
    return __sys_io_uring_register(fd, opcode, arg, nr_args);
}
