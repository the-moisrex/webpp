/* SPDX-License-Identifier: MIT */
#ifndef LIB_URING_H
#define LIB_URING_H

// #define _POSIX_C_SOURCE 200112L

#ifndef _XOPEN_SOURCE
#    define _XOPEN_SOURCE 500 /* Required for glibc to expose sigset_t */
#endif

#ifndef _GNU_SOURCE
#    define _GNU_SOURCE /* Required for musl to expose cpu_set_t */
#endif



extern "C" {
#include "compat.h"
#include "io_uring_types.h"

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <linux/swab.h>
#include <sched.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#ifndef uring_unlikely
#    define uring_unlikely(cond) __builtin_expect(!!(cond), 0)
#endif

#ifndef uring_likely
#    define uring_likely(cond) __builtin_expect(!!(cond), 1)
#endif


#ifdef __alpha__
/*
 * alpha and mips are the exceptions, all other architectures have
 * common numbers for new system calls.
 */
#    ifndef __NR_io_uring_setup
#        define __NR_io_uring_setup 535
#    endif
#    ifndef __NR_io_uring_enter
#        define __NR_io_uring_enter 536
#    endif
#    ifndef __NR_io_uring_register
#        define __NR_io_uring_register 537
#    endif
#elif defined __mips__
#    ifndef __NR_io_uring_setup
#        define __NR_io_uring_setup (__NR_Linux + 425)
#    endif
#    ifndef __NR_io_uring_enter
#        define __NR_io_uring_enter (__NR_Linux + 426)
#    endif
#    ifndef __NR_io_uring_register
#        define __NR_io_uring_register (__NR_Linux + 427)
#    endif
#else /* !__alpha__ and !__mips__ */
#    ifndef __NR_io_uring_setup
#        define __NR_io_uring_setup 425
#    endif
#    ifndef __NR_io_uring_enter
#        define __NR_io_uring_enter 426
#    endif
#    ifndef __NR_io_uring_register
#        define __NR_io_uring_register 427
#    endif
#endif



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
#    include "arch/x86_syscall.h"
#elif defined(__aarch64__)
#    include "arch/aarch64_syscall.hpp"
#elif defined(__riscv) && __riscv_xlen == 64
#    include "arch/riscv64_syscall.hpp"
#else
#    include "arch/generic_syscall.hpp"
#endif


#include "barrier.h"
static inline int __sys_open(const char* pathname, int flags, mode_t mode) {
    /*
     * Some architectures don't have __NR_open, but __NR_openat.
     */
#ifdef __NR_open
    return (int) __do_syscall3(__NR_open, pathname, flags, mode);
#else
    return (int) __do_syscall4(__NR_openat, AT_FDCWD, pathname, flags, mode);
#endif
}

static inline ssize_t __sys_read(int fd, void* buffer, size_t size) {
    return (ssize_t) __do_syscall3(__NR_read, fd, buffer, size);
}

static inline void* __sys_mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset) {
    int nr;

#if defined(__NR_mmap2)
    nr = __NR_mmap2;
    offset >>= 12;
#else
    nr = __NR_mmap;
#endif
    return (void*) __do_syscall6(nr, addr, length, prot, flags, fd, offset);
}

static inline int __sys_munmap(void* addr, size_t length) {
    return (int) __do_syscall2(__NR_munmap, addr, length);
}

static inline int __sys_madvise(void* addr, size_t length, int advice) {
    return (int) __do_syscall3(__NR_madvise, addr, length, advice);
}

static inline int __sys_getrlimit(int resource, struct rlimit* rlim) {
    return (int) __do_syscall2(__NR_getrlimit, resource, rlim);
}

static inline int __sys_setrlimit(int resource, const struct rlimit* rlim) {
    return (int) __do_syscall2(__NR_setrlimit, resource, rlim);
}

static inline int __sys_close(int fd) {
    return (int) __do_syscall1(__NR_close, fd);
}

static inline int
__sys_io_uring_register(unsigned int fd, unsigned int opcode, const void* arg, unsigned int nr_args) {
    return (int) __do_syscall4(__NR_io_uring_register, fd, opcode, arg, nr_args);
}

static inline int __sys_io_uring_setup(unsigned int entries, struct io_uring_params* p) {
    return (int) __do_syscall2(__NR_io_uring_setup, entries, p);
}

static inline int __sys_io_uring_enter2(unsigned int fd,
                                        unsigned int to_submit,
                                        unsigned int min_complete,
                                        unsigned int flags,
                                        sigset_t*    sig,
                                        size_t       sz) {
    return (int) __do_syscall6(__NR_io_uring_enter, fd, to_submit, min_complete, flags, sig, sz);
}

static inline int __sys_io_uring_enter(unsigned int fd,
                                       unsigned int to_submit,
                                       unsigned int min_complete,
                                       unsigned int flags,
                                       sigset_t*    sig) {
    return __sys_io_uring_enter2(fd, to_submit, min_complete, flags, sig, _NSIG / 8);
}


enum {
    INT_FLAG_REG_RING     = 1,
    INT_FLAG_REG_REG_RING = 2,
    INT_FLAG_APP_MEM      = 4,
};


#ifndef offsetof
#    define offsetof(TYPE, FIELD) ((size_t) & ((TYPE*) 0)->FIELD)
#endif

#ifndef container_of
#    define container_of(PTR, TYPE, FIELD)                         \
        ({                                                         \
            __typeof__(((TYPE*) 0)->FIELD)* __FIELD_PTR = (PTR);   \
            (TYPE*) ((char*) __FIELD_PTR - offsetof(TYPE, FIELD)); \
        })
#endif

#define __maybe_unused __attribute__((__unused__))
#define __hot          __attribute__((__hot__))
#define __cold         __attribute__((__cold__))


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

/*
 * Library interface to io_uring
 */
struct io_uring_sq {
    unsigned* khead;
    unsigned* ktail;
    // Deprecated: use `ring_mask` instead of `*kring_mask`
    unsigned* kring_mask;
    // Deprecated: use `ring_entries` instead of `*kring_entries`
    unsigned*            kring_entries;
    unsigned*            kflags;
    unsigned*            kdropped;
    unsigned*            array;
    struct io_uring_sqe* sqes;

    unsigned sqe_head;
    unsigned sqe_tail;

    size_t ring_sz;
    void*  ring_ptr;

    unsigned ring_mask;
    unsigned ring_entries;

    unsigned pad[2];
};

struct io_uring_cq {
    unsigned* khead;
    unsigned* ktail;
    // Deprecated: use `ring_mask` instead of `*kring_mask`
    unsigned* kring_mask;
    // Deprecated: use `ring_entries` instead of `*kring_entries`
    unsigned*            kring_entries;
    unsigned*            kflags;
    unsigned*            koverflow;
    struct io_uring_cqe* cqes;

    size_t ring_sz;
    void*  ring_ptr;

    unsigned ring_mask;
    unsigned ring_entries;

    unsigned pad[2];
};

struct io_uring {
    struct io_uring_sq sq;
    struct io_uring_cq cq;
    unsigned           flags;
    int                ring_fd;

    unsigned features;
    int      enter_ring_fd;
    __u8     int_flags;
    __u8     pad[3];
    unsigned pad2;
};

/*
 * Library interface
 */

/*
 * return an allocated io_uring_probe structure, or NULL if probe fails (for
 * example, if it is not available). The caller is responsible for freeing it
 */
struct io_uring_probe* io_uring_get_probe_ring(struct io_uring* ring);
/* same as io_uring_get_probe_ring, but takes care of ring init and teardown */
struct io_uring_probe* io_uring_get_probe(void);

/*
 * frees a probe allocated through io_uring_get_probe() or
 * io_uring_get_probe_ring()
 */
void io_uring_free_probe(struct io_uring_probe* probe);

static inline int io_uring_opcode_supported(const struct io_uring_probe* p, int op) {
    if (op > p->last_op)
        return 0;
    return (p->ops[op].flags & IO_URING_OP_SUPPORTED) != 0;
}

int      io_uring_queue_init_mem(unsigned                entries,
                                 struct io_uring*        ring,
                                 struct io_uring_params* p,
                                 void*                   buf,
                                 size_t                  buf_size);
int      io_uring_queue_init_params(unsigned entries, struct io_uring* ring, struct io_uring_params* p);
int      io_uring_queue_init(unsigned entries, struct io_uring* ring, unsigned flags);
int      io_uring_queue_mmap(int fd, struct io_uring_params* p, struct io_uring* ring);
int      io_uring_ring_dontfork(struct io_uring* ring);
void     io_uring_queue_exit(struct io_uring* ring);
unsigned io_uring_peek_batch_cqe(struct io_uring* ring, struct io_uring_cqe** cqes, unsigned count);
int      io_uring_wait_cqes(struct io_uring*          ring,
                            struct io_uring_cqe**     cqe_ptr,
                            unsigned                  wait_nr,
                            struct __kernel_timespec* ts,
                            sigset_t*                 sigmask);
int      io_uring_wait_cqe_timeout(struct io_uring*          ring,
                                   struct io_uring_cqe**     cqe_ptr,
                                   struct __kernel_timespec* ts);
int      io_uring_submit(struct io_uring* ring);
int      io_uring_submit_and_wait(struct io_uring* ring, unsigned wait_nr);
int      io_uring_submit_and_wait_timeout(struct io_uring*          ring,
                                          struct io_uring_cqe**     cqe_ptr,
                                          unsigned                  wait_nr,
                                          struct __kernel_timespec* ts,
                                          sigset_t*                 sigmask);

int io_uring_register_buffers(struct io_uring* ring, const struct iovec* iovecs, unsigned nr_iovecs);
int io_uring_register_buffers_tags(struct io_uring*    ring,
                                   const struct iovec* iovecs,
                                   const __u64*        tags,
                                   unsigned            nr);
int io_uring_register_buffers_sparse(struct io_uring* ring, unsigned nr);
int io_uring_register_buffers_update_tag(struct io_uring*    ring,
                                         unsigned            off,
                                         const struct iovec* iovecs,
                                         const __u64*        tags,
                                         unsigned            nr);
int io_uring_unregister_buffers(struct io_uring* ring);

int io_uring_register_files(struct io_uring* ring, const int* files, unsigned nr_files);
int io_uring_register_files_tags(struct io_uring* ring, const int* files, const __u64* tags, unsigned nr);
int io_uring_register_files_sparse(struct io_uring* ring, unsigned nr);
int io_uring_register_files_update_tag(struct io_uring* ring,
                                       unsigned         off,
                                       const int*       files,
                                       const __u64*     tags,
                                       unsigned         nr_files);

int io_uring_unregister_files(struct io_uring* ring);
int io_uring_register_files_update(struct io_uring* ring, unsigned off, const int* files, unsigned nr_files);
int io_uring_register_eventfd(struct io_uring* ring, int fd);
int io_uring_register_eventfd_async(struct io_uring* ring, int fd);
int io_uring_unregister_eventfd(struct io_uring* ring);
int io_uring_register_probe(struct io_uring* ring, struct io_uring_probe* p, unsigned nr);
int io_uring_register_personality(struct io_uring* ring);
int io_uring_unregister_personality(struct io_uring* ring, int id);
int io_uring_register_restrictions(struct io_uring*             ring,
                                   struct io_uring_restriction* res,
                                   unsigned int                 nr_res);
int io_uring_enable_rings(struct io_uring* ring);
int __io_uring_sqring_wait(struct io_uring* ring);
int io_uring_register_iowq_aff(struct io_uring* ring, size_t cpusz, const cpu_set_t* mask);
int io_uring_unregister_iowq_aff(struct io_uring* ring);
int io_uring_register_iowq_max_workers(struct io_uring* ring, unsigned int* values);
int io_uring_register_ring_fd(struct io_uring* ring);
int io_uring_unregister_ring_fd(struct io_uring* ring);
int io_uring_close_ring_fd(struct io_uring* ring);
int io_uring_register_buf_ring(struct io_uring* ring, struct io_uring_buf_reg* reg, unsigned int flags);
int io_uring_unregister_buf_ring(struct io_uring* ring, int bgid);
int io_uring_register_sync_cancel(struct io_uring* ring, struct io_uring_sync_cancel_reg* reg);

int io_uring_register_file_alloc_range(struct io_uring* ring, unsigned off, unsigned len);

int io_uring_get_events(struct io_uring* ring);
int io_uring_submit_and_get_events(struct io_uring* ring);

/*
 * io_uring syscalls.
 */
int io_uring_enter(unsigned int fd,
                   unsigned int to_submit,
                   unsigned int min_complete,
                   unsigned int flags,
                   sigset_t*    sig);
int io_uring_enter2(unsigned int fd,
                    unsigned int to_submit,
                    unsigned int min_complete,
                    unsigned int flags,
                    sigset_t*    sig,
                    size_t       sz);
int io_uring_setup(unsigned int entries, struct io_uring_params* p);
int io_uring_register(unsigned int fd, unsigned int opcode, const void* arg, unsigned int nr_args);

/*
 * Mapped buffer ring alloc/register + unregister/free helpers
 */
struct io_uring_buf_ring*
io_uring_setup_buf_ring(struct io_uring* ring, unsigned int nentries, int bgid, unsigned int flags, int* ret);
int io_uring_free_buf_ring(struct io_uring*          ring,
                           struct io_uring_buf_ring* br,
                           unsigned int              nentries,
                           int                       bgid);

/*
 * Helper for the peek/wait single cqe functions. Exported because of that,
 * but probably shouldn't be used directly in an application.
 */
int __io_uring_get_cqe(struct io_uring*      ring,
                       struct io_uring_cqe** cqe_ptr,
                       unsigned              submit,
                       unsigned              wait_nr,
                       sigset_t*             sigmask);

#define LIBURING_UDATA_TIMEOUT              ((__u64) -1)

/*
 * Calculates the step size for CQE iteration.
 * 	For standard CQE's its 1, for big CQE's its two.
 */
#define io_uring_cqe_shift(ring)            (!!((ring)->flags & IORING_SETUP_CQE32))

#define io_uring_cqe_index(ring, ptr, mask) (((ptr) & (mask)) << io_uring_cqe_shift(ring))

#define io_uring_for_each_cqe(ring, head, cqe)                                              \
    /*                                                                                      \
     * io_uring_smp_load_acquire() enforces the order of tail                               \
     * and CQE reads.                                                                       \
     */                                                                                     \
    for (head = *(ring)->cq.khead;                                                          \
         (cqe = (head != io_uring_smp_load_acquire((ring)->cq.ktail)                        \
                   ? &(ring)->cq.cqes[io_uring_cqe_index(ring, head, (ring)->cq.ring_mask)] \
                   : NULL));                                                                \
         head++)

/*
 * Must be called after io_uring_for_each_cqe()
 */
static inline void io_uring_cq_advance(struct io_uring* ring, unsigned nr) {
    if (nr) {
        struct io_uring_cq* cq = &ring->cq;

        /*
         * Ensure that the kernel only sees the new value of the head
         * index after the CQEs have been read.
         */
        io_uring_smp_store_release(cq->khead, *cq->khead + nr);
    }
}

/*
 * Must be called after io_uring_{peek,wait}_cqe() after the cqe has
 * been processed by the application.
 */
static inline void io_uring_cqe_seen(struct io_uring* ring, struct io_uring_cqe* cqe) {
    if (cqe)
        io_uring_cq_advance(ring, 1);
}

/*
 * Command prep helpers
 */

/*
 * Associate pointer @data with the sqe, for later retrieval from the cqe
 * at command completion time with io_uring_cqe_get_data().
 */
static inline void io_uring_sqe_set_data(struct io_uring_sqe* sqe, void* data) {
    sqe->user_data = (unsigned long) data;
}

static inline void* io_uring_cqe_get_data(const struct io_uring_cqe* cqe) {
    return (void*) (uintptr_t) cqe->user_data;
}

/*
 * Assign a 64-bit value to this sqe, which can get retrieved at completion
 * time with io_uring_cqe_get_data64. Just like the non-64 variants, except
 * these store a 64-bit type rather than a data pointer.
 */
static inline void io_uring_sqe_set_data64(struct io_uring_sqe* sqe, __u64 data) {
    sqe->user_data = data;
}

static inline __u64 io_uring_cqe_get_data64(const struct io_uring_cqe* cqe) {
    return cqe->user_data;
}

/*
 * Tell the app the have the 64-bit variants of the get/set userdata
 */
#define LIBURING_HAVE_DATA64

static inline void io_uring_sqe_set_flags(struct io_uring_sqe* sqe, unsigned flags) {
    sqe->flags = (__u8) flags;
}

static inline void __io_uring_set_target_fixed_file(struct io_uring_sqe* sqe, unsigned int file_index) {
    /* 0 means no fixed files, indexes should be encoded as "index + 1" */
    sqe->file_index = file_index + 1;
}

static inline void
io_uring_prep_rw(int op, struct io_uring_sqe* sqe, int fd, const void* addr, unsigned len, __u64 offset) {
    sqe->opcode        = (__u8) op;
    sqe->flags         = 0;
    sqe->ioprio        = 0;
    sqe->fd            = fd;
    sqe->off           = offset;
    sqe->addr          = (unsigned long) addr;
    sqe->len           = len;
    sqe->rw_flags      = 0;
    sqe->buf_index     = 0;
    sqe->personality   = 0;
    sqe->file_index    = 0;
    sqe->addr3.addr3   = 0;
    sqe->addr3.pad2[0] = 0;
}

/*
 * io_uring_prep_splice() - Either @fd_in or @fd_out must be a pipe.
 *
 * - If @fd_in refers to a pipe, @off_in is ignored and must be set to -1.
 *
 * - If @fd_in does not refer to a pipe and @off_in is -1, then @nbytes are read
 *   from @fd_in starting from the file offset, which is incremented by the
 *   number of bytes read.
 *
 * - If @fd_in does not refer to a pipe and @off_in is not -1, then the starting
 *   offset of @fd_in will be @off_in.
 *
 * This splice operation can be used to implement sendfile by splicing to an
 * intermediate pipe first, then splice to the final destination.
 * In fact, the implementation of sendfile in kernel uses splice internally.
 *
 * NOTE that even if fd_in or fd_out refers to a pipe, the splice operation
 * can still fail with EINVAL if one of the fd doesn't explicitly support splice
 * operation, e.g. reading from terminal is unsupported from kernel 5.7 to 5.11.
 * Check issue #291 for more information.
 */
static inline void io_uring_prep_splice(struct io_uring_sqe* sqe,
                                        int                  fd_in,
                                        int64_t              off_in,
                                        int                  fd_out,
                                        int64_t              off_out,
                                        unsigned int         nbytes,
                                        unsigned int         splice_flags) {
    io_uring_prep_rw(IORING_OP_SPLICE, sqe, fd_out, NULL, nbytes, (__u64) off_out);
    sqe->splice_off_in = (__u64) off_in;
    sqe->splice_fd_in  = fd_in;
    sqe->splice_flags  = splice_flags;
}

static inline void io_uring_prep_tee(struct io_uring_sqe* sqe,
                                     int                  fd_in,
                                     int                  fd_out,
                                     unsigned int         nbytes,
                                     unsigned int         splice_flags) {
    io_uring_prep_rw(IORING_OP_TEE, sqe, fd_out, NULL, nbytes, 0);
    sqe->splice_off_in = 0;
    sqe->splice_fd_in  = fd_in;
    sqe->splice_flags  = splice_flags;
}

static inline void io_uring_prep_readv(struct io_uring_sqe* sqe,
                                       int                  fd,
                                       const struct iovec*  iovecs,
                                       unsigned             nr_vecs,
                                       __u64                offset) {
    io_uring_prep_rw(IORING_OP_READV, sqe, fd, iovecs, nr_vecs, offset);
}

static inline void io_uring_prep_readv2(struct io_uring_sqe* sqe,
                                        int                  fd,
                                        const struct iovec*  iovecs,
                                        unsigned             nr_vecs,
                                        __u64                offset,
                                        int                  flags) {
    io_uring_prep_readv(sqe, fd, iovecs, nr_vecs, offset);
    sqe->rw_flags = flags;
}

static inline void io_uring_prep_read_fixed(struct io_uring_sqe* sqe,
                                            int                  fd,
                                            void*                buf,
                                            unsigned             nbytes,
                                            __u64                offset,
                                            int                  buf_index) {
    io_uring_prep_rw(IORING_OP_READ_FIXED, sqe, fd, buf, nbytes, offset);
    sqe->buf_index = (__u16) buf_index;
}

static inline void io_uring_prep_writev(struct io_uring_sqe* sqe,
                                        int                  fd,
                                        const struct iovec*  iovecs,
                                        unsigned             nr_vecs,
                                        __u64                offset) {
    io_uring_prep_rw(IORING_OP_WRITEV, sqe, fd, iovecs, nr_vecs, offset);
}

static inline void io_uring_prep_writev2(struct io_uring_sqe* sqe,
                                         int                  fd,
                                         const struct iovec*  iovecs,
                                         unsigned             nr_vecs,
                                         __u64                offset,
                                         int                  flags) {
    io_uring_prep_writev(sqe, fd, iovecs, nr_vecs, offset);
    sqe->rw_flags = flags;
}

static inline void io_uring_prep_write_fixed(struct io_uring_sqe* sqe,
                                             int                  fd,
                                             const void*          buf,
                                             unsigned             nbytes,
                                             __u64                offset,
                                             int                  buf_index) {
    io_uring_prep_rw(IORING_OP_WRITE_FIXED, sqe, fd, buf, nbytes, offset);
    sqe->buf_index = (__u16) buf_index;
}

static inline void
io_uring_prep_recvmsg(struct io_uring_sqe* sqe, int fd, struct msghdr* msg, unsigned flags) {
    io_uring_prep_rw(IORING_OP_RECVMSG, sqe, fd, msg, 1, 0);
    sqe->msg_flags = flags;
}

static inline void
io_uring_prep_recvmsg_multishot(struct io_uring_sqe* sqe, int fd, struct msghdr* msg, unsigned flags) {
    io_uring_prep_recvmsg(sqe, fd, msg, flags);
    sqe->ioprio |= IORING_RECV_MULTISHOT;
}

static inline void
io_uring_prep_sendmsg(struct io_uring_sqe* sqe, int fd, const struct msghdr* msg, unsigned flags) {
    io_uring_prep_rw(IORING_OP_SENDMSG, sqe, fd, msg, 1, 0);
    sqe->msg_flags = flags;
}

static inline unsigned __io_uring_prep_poll_mask(unsigned poll_mask) {
#if __BYTE_ORDER == __BIG_ENDIAN
    poll_mask = __swahw32(poll_mask);
#endif
    return poll_mask;
}

static inline void io_uring_prep_poll_add(struct io_uring_sqe* sqe, int fd, unsigned poll_mask) {
    io_uring_prep_rw(IORING_OP_POLL_ADD, sqe, fd, NULL, 0, 0);
    sqe->poll32_events = __io_uring_prep_poll_mask(poll_mask);
}

static inline void io_uring_prep_poll_multishot(struct io_uring_sqe* sqe, int fd, unsigned poll_mask) {
    io_uring_prep_poll_add(sqe, fd, poll_mask);
    sqe->len = IORING_POLL_ADD_MULTI;
}

static inline void io_uring_prep_poll_remove(struct io_uring_sqe* sqe, __u64 user_data) {
    io_uring_prep_rw(IORING_OP_POLL_REMOVE, sqe, -1, NULL, 0, 0);
    sqe->addr = user_data;
}

static inline void io_uring_prep_poll_update(struct io_uring_sqe* sqe,
                                             __u64                old_user_data,
                                             __u64                new_user_data,
                                             unsigned             poll_mask,
                                             unsigned             flags) {
    io_uring_prep_rw(IORING_OP_POLL_REMOVE, sqe, -1, NULL, flags, new_user_data);
    sqe->addr          = old_user_data;
    sqe->poll32_events = __io_uring_prep_poll_mask(poll_mask);
}

static inline void io_uring_prep_fsync(struct io_uring_sqe* sqe, int fd, unsigned fsync_flags) {
    io_uring_prep_rw(IORING_OP_FSYNC, sqe, fd, NULL, 0, 0);
    sqe->fsync_flags = fsync_flags;
}

static inline void io_uring_prep_nop(struct io_uring_sqe* sqe) {
    io_uring_prep_rw(IORING_OP_NOP, sqe, -1, NULL, 0, 0);
}

static inline void io_uring_prep_timeout(struct io_uring_sqe*      sqe,
                                         struct __kernel_timespec* ts,
                                         unsigned                  count,
                                         unsigned                  flags) {
    io_uring_prep_rw(IORING_OP_TIMEOUT, sqe, -1, ts, 1, count);
    sqe->timeout_flags = flags;
}

static inline void io_uring_prep_timeout_remove(struct io_uring_sqe* sqe, __u64 user_data, unsigned flags) {
    io_uring_prep_rw(IORING_OP_TIMEOUT_REMOVE, sqe, -1, NULL, 0, 0);
    sqe->addr          = user_data;
    sqe->timeout_flags = flags;
}

static inline void io_uring_prep_timeout_update(struct io_uring_sqe*      sqe,
                                                struct __kernel_timespec* ts,
                                                __u64                     user_data,
                                                unsigned                  flags) {
    io_uring_prep_rw(IORING_OP_TIMEOUT_REMOVE, sqe, -1, NULL, 0, (uintptr_t) ts);
    sqe->addr          = user_data;
    sqe->timeout_flags = flags | IORING_TIMEOUT_UPDATE;
}

static inline void
io_uring_prep_accept(struct io_uring_sqe* sqe, int fd, struct sockaddr* addr, socklen_t* addrlen, int flags) {
    io_uring_prep_rw(IORING_OP_ACCEPT, sqe, fd, addr, 0, (__u64) (unsigned long) addrlen);
    sqe->accept_flags = (__u32) flags;
}

/* accept directly into the fixed file table */
static inline void io_uring_prep_accept_direct(struct io_uring_sqe* sqe,
                                               int                  fd,
                                               struct sockaddr*     addr,
                                               socklen_t*           addrlen,
                                               int                  flags,
                                               unsigned int         file_index) {
    io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
    /* offset by 1 for allocation */
    if (file_index == IORING_FILE_INDEX_ALLOC)
        file_index--;
    __io_uring_set_target_fixed_file(sqe, file_index);
}

static inline void io_uring_prep_multishot_accept(struct io_uring_sqe* sqe,
                                                  int                  fd,
                                                  struct sockaddr*     addr,
                                                  socklen_t*           addrlen,
                                                  int                  flags) {
    io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
    sqe->ioprio |= IORING_ACCEPT_MULTISHOT;
}

/* multishot accept directly into the fixed file table */
static inline void io_uring_prep_multishot_accept_direct(struct io_uring_sqe* sqe,
                                                         int                  fd,
                                                         struct sockaddr*     addr,
                                                         socklen_t*           addrlen,
                                                         int                  flags) {
    io_uring_prep_multishot_accept(sqe, fd, addr, addrlen, flags);
    __io_uring_set_target_fixed_file(sqe, IORING_FILE_INDEX_ALLOC - 1);
}

static inline void io_uring_prep_cancel64(struct io_uring_sqe* sqe, __u64 user_data, int flags) {
    io_uring_prep_rw(IORING_OP_ASYNC_CANCEL, sqe, -1, NULL, 0, 0);
    sqe->addr         = user_data;
    sqe->cancel_flags = (__u32) flags;
}

static inline void io_uring_prep_cancel(struct io_uring_sqe* sqe, void* user_data, int flags) {
    io_uring_prep_cancel64(sqe, (__u64) (uintptr_t) user_data, flags);
}

static inline void io_uring_prep_cancel_fd(struct io_uring_sqe* sqe, int fd, unsigned int flags) {
    io_uring_prep_rw(IORING_OP_ASYNC_CANCEL, sqe, fd, NULL, 0, 0);
    sqe->cancel_flags = (__u32) flags | IORING_ASYNC_CANCEL_FD;
}

static inline void
io_uring_prep_link_timeout(struct io_uring_sqe* sqe, struct __kernel_timespec* ts, unsigned flags) {
    io_uring_prep_rw(IORING_OP_LINK_TIMEOUT, sqe, -1, ts, 1, 0);
    sqe->timeout_flags = flags;
}

static inline void
io_uring_prep_connect(struct io_uring_sqe* sqe, int fd, const struct sockaddr* addr, socklen_t addrlen) {
    io_uring_prep_rw(IORING_OP_CONNECT, sqe, fd, addr, 0, addrlen);
}

static inline void
io_uring_prep_files_update(struct io_uring_sqe* sqe, int* fds, unsigned nr_fds, int offset) {
    io_uring_prep_rw(IORING_OP_FILES_UPDATE, sqe, -1, fds, nr_fds, (__u64) offset);
}

static inline void
io_uring_prep_fallocate(struct io_uring_sqe* sqe, int fd, int mode, __u64 offset, __u64 len) {
    io_uring_prep_rw(IORING_OP_FALLOCATE, sqe, fd, 0, (unsigned int) mode, (__u64) offset);
    sqe->addr = (__u64) len;
}

static inline void
io_uring_prep_openat(struct io_uring_sqe* sqe, int dfd, const char* path, int flags, mode_t mode) {
    io_uring_prep_rw(IORING_OP_OPENAT, sqe, dfd, path, mode, 0);
    sqe->open_flags = (__u32) flags;
}

/* open directly into the fixed file table */
static inline void io_uring_prep_openat_direct(struct io_uring_sqe* sqe,
                                               int                  dfd,
                                               const char*          path,
                                               int                  flags,
                                               mode_t               mode,
                                               unsigned             file_index) {
    io_uring_prep_openat(sqe, dfd, path, flags, mode);
    /* offset by 1 for allocation */
    if (file_index == IORING_FILE_INDEX_ALLOC)
        file_index--;
    __io_uring_set_target_fixed_file(sqe, file_index);
}

static inline void io_uring_prep_close(struct io_uring_sqe* sqe, int fd) {
    io_uring_prep_rw(IORING_OP_CLOSE, sqe, fd, NULL, 0, 0);
}

static inline void io_uring_prep_close_direct(struct io_uring_sqe* sqe, unsigned file_index) {
    io_uring_prep_close(sqe, 0);
    __io_uring_set_target_fixed_file(sqe, file_index);
}

static inline void
io_uring_prep_read(struct io_uring_sqe* sqe, int fd, void* buf, unsigned nbytes, __u64 offset) {
    io_uring_prep_rw(IORING_OP_READ, sqe, fd, buf, nbytes, offset);
}

static inline void
io_uring_prep_write(struct io_uring_sqe* sqe, int fd, const void* buf, unsigned nbytes, __u64 offset) {
    io_uring_prep_rw(IORING_OP_WRITE, sqe, fd, buf, nbytes, offset);
}

struct statx;
static inline void io_uring_prep_statx(struct io_uring_sqe* sqe,
                                       int                  dfd,
                                       const char*          path,
                                       int                  flags,
                                       unsigned             mask,
                                       struct statx*        statxbuf) {
    io_uring_prep_rw(IORING_OP_STATX, sqe, dfd, path, mask, (__u64) (unsigned long) statxbuf);
    sqe->statx_flags = (__u32) flags;
}

static inline void
io_uring_prep_fadvise(struct io_uring_sqe* sqe, int fd, __u64 offset, off_t len, int advice) {
    io_uring_prep_rw(IORING_OP_FADVISE, sqe, fd, NULL, (__u32) len, offset);
    sqe->fadvise_advice = (__u32) advice;
}

static inline void io_uring_prep_madvise(struct io_uring_sqe* sqe, void* addr, off_t length, int advice) {
    io_uring_prep_rw(IORING_OP_MADVISE, sqe, -1, addr, (__u32) length, 0);
    sqe->fadvise_advice = (__u32) advice;
}

static inline void
io_uring_prep_send(struct io_uring_sqe* sqe, int sockfd, const void* buf, size_t len, int flags) {
    io_uring_prep_rw(IORING_OP_SEND, sqe, sockfd, buf, (__u32) len, 0);
    sqe->msg_flags = (__u32) flags;
}

static inline void
io_uring_prep_send_set_addr(struct io_uring_sqe* sqe, const struct sockaddr* dest_addr, __u16 addr_len) {
    sqe->addr2             = (unsigned long) (const void*) dest_addr;
    sqe->addr_len.addr_len = addr_len;
}

static inline void io_uring_prep_sendto(struct io_uring_sqe*   sqe,
                                        int                    sockfd,
                                        const void*            buf,
                                        size_t                 len,
                                        int                    flags,
                                        const struct sockaddr* addr,
                                        socklen_t              addrlen) {
    io_uring_prep_send(sqe, sockfd, buf, len, flags);
    io_uring_prep_send_set_addr(sqe, addr, addrlen);
}

static inline void io_uring_prep_send_zc(struct io_uring_sqe* sqe,
                                         int                  sockfd,
                                         const void*          buf,
                                         size_t               len,
                                         int                  flags,
                                         unsigned             zc_flags) {
    io_uring_prep_rw(IORING_OP_SEND_ZC, sqe, sockfd, buf, (__u32) len, 0);
    sqe->msg_flags = (__u32) flags;
    sqe->ioprio    = zc_flags;
}

static inline void io_uring_prep_send_zc_fixed(struct io_uring_sqe* sqe,
                                               int                  sockfd,
                                               const void*          buf,
                                               size_t               len,
                                               int                  flags,
                                               unsigned             zc_flags,
                                               unsigned             buf_index) {
    io_uring_prep_send_zc(sqe, sockfd, buf, len, flags, zc_flags);
    sqe->ioprio |= IORING_RECVSEND_FIXED_BUF;
    sqe->buf_index = buf_index;
}

static inline void
io_uring_prep_sendmsg_zc(struct io_uring_sqe* sqe, int fd, const struct msghdr* msg, unsigned flags) {
    io_uring_prep_sendmsg(sqe, fd, msg, flags);
    sqe->opcode = IORING_OP_SENDMSG_ZC;
}

static inline void
io_uring_prep_recv(struct io_uring_sqe* sqe, int sockfd, void* buf, size_t len, int flags) {
    io_uring_prep_rw(IORING_OP_RECV, sqe, sockfd, buf, (__u32) len, 0);
    sqe->msg_flags = (__u32) flags;
}

static inline void
io_uring_prep_recv_multishot(struct io_uring_sqe* sqe, int sockfd, void* buf, size_t len, int flags) {
    io_uring_prep_recv(sqe, sockfd, buf, len, flags);
    sqe->ioprio |= IORING_RECV_MULTISHOT;
}

static inline struct io_uring_recvmsg_out*
io_uring_recvmsg_validate(void* buf, int buf_len, struct msghdr* msgh) {
    unsigned long header = msgh->msg_controllen + msgh->msg_namelen + sizeof(struct io_uring_recvmsg_out);
    if (buf_len < 0 || (unsigned long) buf_len < header)
        return NULL;
    return (struct io_uring_recvmsg_out*) buf;
}

static inline void* io_uring_recvmsg_name(struct io_uring_recvmsg_out* o) {
    return (void*) &o[1];
}

static inline struct cmsghdr* io_uring_recvmsg_cmsg_firsthdr(struct io_uring_recvmsg_out* o,
                                                             struct msghdr*               msgh) {
    if (o->controllen < sizeof(struct cmsghdr))
        return NULL;

    return (struct cmsghdr*) ((unsigned char*) io_uring_recvmsg_name(o) + msgh->msg_namelen);
}

static inline struct cmsghdr*
io_uring_recvmsg_cmsg_nexthdr(struct io_uring_recvmsg_out* o, struct msghdr* msgh, struct cmsghdr* cmsg) {
    unsigned char* end;

    if (cmsg->cmsg_len < sizeof(struct cmsghdr))
        return NULL;
    end  = (unsigned char*) io_uring_recvmsg_cmsg_firsthdr(o, msgh) + o->controllen;
    cmsg = (struct cmsghdr*) ((unsigned char*) cmsg + CMSG_ALIGN(cmsg->cmsg_len));

    if ((unsigned char*) (cmsg + 1) > end)
        return NULL;
    if (((unsigned char*) cmsg) + CMSG_ALIGN(cmsg->cmsg_len) > end)
        return NULL;

    return cmsg;
}

static inline void* io_uring_recvmsg_payload(struct io_uring_recvmsg_out* o, struct msghdr* msgh) {
    return (void*) ((unsigned char*) io_uring_recvmsg_name(o) + msgh->msg_namelen + msgh->msg_controllen);
}

static inline unsigned int
io_uring_recvmsg_payload_length(struct io_uring_recvmsg_out* o, int buf_len, struct msghdr* msgh) {
    unsigned long payload_start, payload_end;

    payload_start = (unsigned long) io_uring_recvmsg_payload(o, msgh);
    payload_end   = (unsigned long) o + buf_len;
    return (unsigned int) (payload_end - payload_start);
}

static inline void
io_uring_prep_openat2(struct io_uring_sqe* sqe, int dfd, const char* path, struct open_how* how) {
    io_uring_prep_rw(IORING_OP_OPENAT2, sqe, dfd, path, sizeof(*how), (uint64_t) (uintptr_t) how);
}

/* open directly into the fixed file table */
static inline void io_uring_prep_openat2_direct(struct io_uring_sqe* sqe,
                                                int                  dfd,
                                                const char*          path,
                                                struct open_how*     how,
                                                unsigned             file_index) {
    io_uring_prep_openat2(sqe, dfd, path, how);
    /* offset by 1 for allocation */
    if (file_index == IORING_FILE_INDEX_ALLOC)
        file_index--;
    __io_uring_set_target_fixed_file(sqe, file_index);
}

struct epoll_event;
static inline void
io_uring_prep_epoll_ctl(struct io_uring_sqe* sqe, int epfd, int fd, int op, struct epoll_event* ev) {
    io_uring_prep_rw(IORING_OP_EPOLL_CTL, sqe, epfd, ev, (__u32) op, (__u32) fd);
}

static inline void
io_uring_prep_provide_buffers(struct io_uring_sqe* sqe, void* addr, int len, int nr, int bgid, int bid) {
    io_uring_prep_rw(IORING_OP_PROVIDE_BUFFERS, sqe, nr, addr, (__u32) len, (__u64) bid);
    sqe->buf_group = (__u16) bgid;
}

static inline void io_uring_prep_remove_buffers(struct io_uring_sqe* sqe, int nr, int bgid) {
    io_uring_prep_rw(IORING_OP_REMOVE_BUFFERS, sqe, nr, NULL, 0, 0);
    sqe->buf_group = (__u16) bgid;
}

static inline void io_uring_prep_shutdown(struct io_uring_sqe* sqe, int fd, int how) {
    io_uring_prep_rw(IORING_OP_SHUTDOWN, sqe, fd, NULL, (__u32) how, 0);
}

static inline void io_uring_prep_unlinkat(struct io_uring_sqe* sqe, int dfd, const char* path, int flags) {
    io_uring_prep_rw(IORING_OP_UNLINKAT, sqe, dfd, path, 0, 0);
    sqe->unlink_flags = (__u32) flags;
}

static inline void io_uring_prep_unlink(struct io_uring_sqe* sqe, const char* path, int flags) {
    io_uring_prep_unlinkat(sqe, AT_FDCWD, path, flags);
}

static inline void io_uring_prep_renameat(struct io_uring_sqe* sqe,
                                          int                  olddfd,
                                          const char*          oldpath,
                                          int                  newdfd,
                                          const char*          newpath,
                                          unsigned int         flags) {
    io_uring_prep_rw(IORING_OP_RENAMEAT,
                     sqe,
                     olddfd,
                     oldpath,
                     (__u32) newdfd,
                     (uint64_t) (uintptr_t) newpath);
    sqe->rename_flags = (__u32) flags;
}

static inline void io_uring_prep_rename(struct io_uring_sqe* sqe, const char* oldpath, const char* newpath) {
    io_uring_prep_renameat(sqe, AT_FDCWD, oldpath, AT_FDCWD, newpath, 0);
}

static inline void
io_uring_prep_sync_file_range(struct io_uring_sqe* sqe, int fd, unsigned len, __u64 offset, int flags) {
    io_uring_prep_rw(IORING_OP_SYNC_FILE_RANGE, sqe, fd, NULL, len, offset);
    sqe->sync_range_flags = (__u32) flags;
}

static inline void io_uring_prep_mkdirat(struct io_uring_sqe* sqe, int dfd, const char* path, mode_t mode) {
    io_uring_prep_rw(IORING_OP_MKDIRAT, sqe, dfd, path, mode, 0);
}

static inline void io_uring_prep_mkdir(struct io_uring_sqe* sqe, const char* path, mode_t mode) {
    io_uring_prep_mkdirat(sqe, AT_FDCWD, path, mode);
}

static inline void
io_uring_prep_symlinkat(struct io_uring_sqe* sqe, const char* target, int newdirfd, const char* linkpath) {
    io_uring_prep_rw(IORING_OP_SYMLINKAT, sqe, newdirfd, target, 0, (uint64_t) (uintptr_t) linkpath);
}

static inline void io_uring_prep_symlink(struct io_uring_sqe* sqe, const char* target, const char* linkpath) {
    io_uring_prep_symlinkat(sqe, target, AT_FDCWD, linkpath);
}

static inline void io_uring_prep_linkat(struct io_uring_sqe* sqe,
                                        int                  olddfd,
                                        const char*          oldpath,
                                        int                  newdfd,
                                        const char*          newpath,
                                        int                  flags) {
    io_uring_prep_rw(IORING_OP_LINKAT, sqe, olddfd, oldpath, (__u32) newdfd, (uint64_t) (uintptr_t) newpath);
    sqe->hardlink_flags = (__u32) flags;
}

static inline void
io_uring_prep_link(struct io_uring_sqe* sqe, const char* oldpath, const char* newpath, int flags) {
    io_uring_prep_linkat(sqe, AT_FDCWD, oldpath, AT_FDCWD, newpath, flags);
}

static inline void io_uring_prep_msg_ring_cqe_flags(struct io_uring_sqe* sqe,
                                                    int                  fd,
                                                    unsigned int         len,
                                                    __u64                data,
                                                    unsigned int         flags,
                                                    unsigned int         cqe_flags) {
    io_uring_prep_rw(IORING_OP_MSG_RING, sqe, fd, NULL, len, data);
    sqe->msg_ring_flags = IORING_MSG_RING_FLAGS_PASS | flags;
    sqe->file_index     = cqe_flags;
}

static inline void
io_uring_prep_msg_ring(struct io_uring_sqe* sqe, int fd, unsigned int len, __u64 data, unsigned int flags) {
    io_uring_prep_rw(IORING_OP_MSG_RING, sqe, fd, NULL, len, data);
    sqe->msg_ring_flags = flags;
}

static inline void io_uring_prep_msg_ring_fd(struct io_uring_sqe* sqe,
                                             int                  fd,
                                             int                  source_fd,
                                             int                  target_fd,
                                             __u64                data,
                                             unsigned int         flags) {
    io_uring_prep_rw(IORING_OP_MSG_RING, sqe, fd, (void*) (uintptr_t) IORING_MSG_SEND_FD, 0, data);
    sqe->addr3.addr3 = source_fd;
    /* offset by 1 for allocation */
    if ((unsigned int) target_fd == IORING_FILE_INDEX_ALLOC)
        target_fd--;
    __io_uring_set_target_fixed_file(sqe, target_fd);
    sqe->msg_ring_flags = flags;
}

static inline void io_uring_prep_msg_ring_fd_alloc(struct io_uring_sqe* sqe,
                                                   int                  fd,
                                                   int                  source_fd,
                                                   __u64                data,
                                                   unsigned int         flags) {
    io_uring_prep_msg_ring_fd(sqe, fd, source_fd, IORING_FILE_INDEX_ALLOC, data, flags);
}

static inline void io_uring_prep_getxattr(struct io_uring_sqe* sqe,
                                          const char*          name,
                                          char*                value,
                                          const char*          path,
                                          unsigned int         len) {
    io_uring_prep_rw(IORING_OP_GETXATTR, sqe, 0, name, len, (__u64) (uintptr_t) value);
    sqe->addr3.addr3 = (__u64) (uintptr_t) path;
    sqe->xattr_flags = 0;
}

static inline void io_uring_prep_setxattr(struct io_uring_sqe* sqe,
                                          const char*          name,
                                          const char*          value,
                                          const char*          path,
                                          int                  flags,
                                          unsigned int         len) {
    io_uring_prep_rw(IORING_OP_SETXATTR, sqe, 0, name, len, (__u64) (uintptr_t) value);
    sqe->addr3.addr3 = (__u64) (uintptr_t) path;
    sqe->xattr_flags = flags;
}

static inline void
io_uring_prep_fgetxattr(struct io_uring_sqe* sqe, int fd, const char* name, char* value, unsigned int len) {
    io_uring_prep_rw(IORING_OP_FGETXATTR, sqe, fd, name, len, (__u64) (uintptr_t) value);
    sqe->xattr_flags = 0;
}

static inline void io_uring_prep_fsetxattr(struct io_uring_sqe* sqe,
                                           int                  fd,
                                           const char*          name,
                                           const char*          value,
                                           int                  flags,
                                           unsigned int         len) {
    io_uring_prep_rw(IORING_OP_FSETXATTR, sqe, fd, name, len, (__u64) (uintptr_t) value);
    sqe->xattr_flags = flags;
}

static inline void
io_uring_prep_socket(struct io_uring_sqe* sqe, int domain, int type, int protocol, unsigned int flags) {
    io_uring_prep_rw(IORING_OP_SOCKET, sqe, domain, NULL, protocol, type);
    sqe->rw_flags = flags;
}

static inline void io_uring_prep_socket_direct(struct io_uring_sqe* sqe,
                                               int                  domain,
                                               int                  type,
                                               int                  protocol,
                                               unsigned             file_index,
                                               unsigned int         flags) {
    io_uring_prep_rw(IORING_OP_SOCKET, sqe, domain, NULL, protocol, type);
    sqe->rw_flags = flags;
    /* offset by 1 for allocation */
    if (file_index == IORING_FILE_INDEX_ALLOC)
        file_index--;
    __io_uring_set_target_fixed_file(sqe, file_index);
}

static inline void io_uring_prep_socket_direct_alloc(struct io_uring_sqe* sqe,
                                                     int                  domain,
                                                     int                  type,
                                                     int                  protocol,
                                                     unsigned int         flags) {
    io_uring_prep_rw(IORING_OP_SOCKET, sqe, domain, NULL, protocol, type);
    sqe->rw_flags = flags;
    __io_uring_set_target_fixed_file(sqe, IORING_FILE_INDEX_ALLOC - 1);
}


#define UNUSED(x) (void) (x)

/*
 * Prepare commands for sockets
 */
static inline void io_uring_prep_cmd_sock(struct io_uring_sqe* sqe,
                                          int                  cmd_op,
                                          int                  fd,
                                          int                  level,
                                          int                  optname,
                                          void*                optval,
                                          int                  optlen) {
    /* This will be removed once the get/setsockopt() patches land */
    UNUSED(optlen);
    UNUSED(optval);
    UNUSED(level);
    UNUSED(optname);
    io_uring_prep_rw(IORING_OP_URING_CMD, sqe, fd, NULL, 0, 0);
    sqe->cmd_op.cmd_op = cmd_op;
}

/*
 * Returns number of unconsumed (if SQPOLL) or unsubmitted entries exist in
 * the SQ ring
 */
static inline unsigned io_uring_sq_ready(const struct io_uring* ring) {
    unsigned khead = *ring->sq.khead;

    /*
     * Without a barrier, we could miss an update and think the SQ wasn't
     * ready. We don't need the load acquire for non-SQPOLL since then we
     * drive updates.
     */
    if (ring->flags & IORING_SETUP_SQPOLL)
        khead = io_uring_smp_load_acquire(ring->sq.khead);

    /* always use real head, to avoid losing sync for short submit */
    return ring->sq.sqe_tail - khead;
}

/*
 * Returns how much space is left in the SQ ring.
 */
static inline unsigned io_uring_sq_space_left(const struct io_uring* ring) {
    return ring->sq.ring_entries - io_uring_sq_ready(ring);
}

/*
 * Only applicable when using SQPOLL - allows the caller to wait for space
 * to free up in the SQ ring, which happens when the kernel side thread has
 * consumed one or more entries. If the SQ ring is currently non-full, no
 * action is taken. Note: may return -EINVAL if the kernel doesn't support
 * this feature.
 */
static inline int io_uring_sqring_wait(struct io_uring* ring) {
    if (!(ring->flags & IORING_SETUP_SQPOLL))
        return 0;
    if (io_uring_sq_space_left(ring))
        return 0;

    return __io_uring_sqring_wait(ring);
}

/*
 * Returns how many unconsumed entries are ready in the CQ ring
 */
static inline unsigned io_uring_cq_ready(const struct io_uring* ring) {
    return io_uring_smp_load_acquire(ring->cq.ktail) - *ring->cq.khead;
}

/*
 * Returns true if there are overflow entries waiting to be flushed onto
 * the CQ ring
 */
static inline bool io_uring_cq_has_overflow(const struct io_uring* ring) {
    return IO_URING_READ_ONCE(*ring->sq.kflags) & IORING_SQ_CQ_OVERFLOW;
}

/*
 * Returns true if the eventfd notification is currently enabled
 */
static inline bool io_uring_cq_eventfd_enabled(const struct io_uring* ring) {
    if (!ring->cq.kflags)
        return true;

    return !(*ring->cq.kflags & IORING_CQ_EVENTFD_DISABLED);
}

/*
 * Toggle eventfd notification on or off, if an eventfd is registered with
 * the ring.
 */
static inline int io_uring_cq_eventfd_toggle(struct io_uring* ring, bool enabled) {
    uint32_t flags;

    if (!!enabled == io_uring_cq_eventfd_enabled(ring))
        return 0;

    if (!ring->cq.kflags)
        return -EOPNOTSUPP;

    flags = *ring->cq.kflags;

    if (enabled)
        flags &= ~IORING_CQ_EVENTFD_DISABLED;
    else
        flags |= IORING_CQ_EVENTFD_DISABLED;

    IO_URING_WRITE_ONCE(*ring->cq.kflags, flags);

    return 0;
}

/*
 * Return an IO completion, waiting for 'wait_nr' completions if one isn't
 * readily available. Returns 0 with cqe_ptr filled in on success, -errno on
 * failure.
 */
static inline int
io_uring_wait_cqe_nr(struct io_uring* ring, struct io_uring_cqe** cqe_ptr, unsigned wait_nr) {
    return __io_uring_get_cqe(ring, cqe_ptr, 0, wait_nr, NULL);
}

/*
 * Internal helper, don't use directly in applications. Use one of the
 * "official" versions of this, io_uring_peek_cqe(), io_uring_wait_cqe(),
 * or io_uring_wait_cqes*().
 */
static inline int
__io_uring_peek_cqe(struct io_uring* ring, struct io_uring_cqe** cqe_ptr, unsigned* nr_available) {
    struct io_uring_cqe* cqe;
    int                  err = 0;
    unsigned             available;
    unsigned             mask  = ring->cq.ring_mask;
    int                  shift = 0;

    if (ring->flags & IORING_SETUP_CQE32)
        shift = 1;

    do {
        unsigned tail = io_uring_smp_load_acquire(ring->cq.ktail);
        unsigned head = *ring->cq.khead;

        cqe       = NULL;
        available = tail - head;
        if (!available)
            break;

        cqe = &ring->cq.cqes[(head & mask) << shift];
        if (!(ring->features & IORING_FEAT_EXT_ARG) && cqe->user_data == LIBURING_UDATA_TIMEOUT) {
            if (cqe->res < 0)
                err = cqe->res;
            io_uring_cq_advance(ring, 1);
            if (!err)
                continue;
            cqe = NULL;
        }

        break;
    } while (1);

    *cqe_ptr = cqe;
    if (nr_available)
        *nr_available = available;
    return err;
}

/*
 * Return an IO completion, if one is readily available. Returns 0 with
 * cqe_ptr filled in on success, -errno on failure.
 */
static inline int io_uring_peek_cqe(struct io_uring* ring, struct io_uring_cqe** cqe_ptr) {
    if (!__io_uring_peek_cqe(ring, cqe_ptr, NULL) && *cqe_ptr)
        return 0;

    return io_uring_wait_cqe_nr(ring, cqe_ptr, 0);
}

/*
 * Return an IO completion, waiting for it if necessary. Returns 0 with
 * cqe_ptr filled in on success, -errno on failure.
 */
static inline int io_uring_wait_cqe(struct io_uring* ring, struct io_uring_cqe** cqe_ptr) {
    if (!__io_uring_peek_cqe(ring, cqe_ptr, NULL) && *cqe_ptr)
        return 0;

    return io_uring_wait_cqe_nr(ring, cqe_ptr, 1);
}

/*
 * Return an sqe to fill. Application must later call io_uring_submit()
 * when it's ready to tell the kernel about it. The caller may call this
 * function multiple times before calling io_uring_submit().
 *
 * Returns a vacant sqe, or NULL if we're full.
 */
static inline struct io_uring_sqe* _io_uring_get_sqe(struct io_uring* ring) {
    struct io_uring_sq* sq = &ring->sq;
    unsigned int        head, next = sq->sqe_tail + 1;
    int                 shift = 0;

    if (ring->flags & IORING_SETUP_SQE128)
        shift = 1;
    if (!(ring->flags & IORING_SETUP_SQPOLL))
        head = IO_URING_READ_ONCE(*sq->khead);
    else
        head = io_uring_smp_load_acquire(sq->khead);

    if (next - head <= sq->ring_entries) {
        struct io_uring_sqe* sqe;

        sqe          = &sq->sqes[(sq->sqe_tail & sq->ring_mask) << shift];
        sq->sqe_tail = next;
        return sqe;
    }

    return NULL;
}

/*
 * Return the appropriate mask for a buffer ring of size 'ring_entries'
 */
static inline int io_uring_buf_ring_mask(__u32 ring_entries) {
    return ring_entries - 1;
}

static inline void io_uring_buf_ring_init(struct io_uring_buf_ring* br) {
    br->tail.tail = 0;
}

/*
 * Assign 'buf' with the addr/len/buffer ID supplied
 */
static inline void io_uring_buf_ring_add(struct io_uring_buf_ring* br,
                                         void*                     addr,
                                         unsigned int              len,
                                         unsigned short            bid,
                                         int                       mask,
                                         int                       buf_offset) {
    struct io_uring_buf* buf = &br->bufs[(br->tail.tail + buf_offset) & mask];

    buf->addr = (unsigned long) (uintptr_t) addr;
    buf->len  = len;
    buf->bid  = bid;
}

/*
 * Make 'count' new buffers visible to the kernel. Called after
 * io_uring_buf_ring_add() has been called 'count' times to fill in new
 * buffers.
 */
static inline void io_uring_buf_ring_advance(struct io_uring_buf_ring* br, int count) {
    unsigned short new_tail = br->tail.tail + count;

    io_uring_smp_store_release(&br->tail.tail, new_tail);
}

static inline void __io_uring_buf_ring_cq_advance(struct io_uring*          ring,
                                                  struct io_uring_buf_ring* br,
                                                  int                       cq_count,
                                                  int                       buf_count) {
    br->tail.tail += buf_count;
    io_uring_cq_advance(ring, cq_count);
}

/*
 * Make 'count' new buffers visible to the kernel while at the same time
 * advancing the CQ ring seen entries. This can be used when the application
 * is using ring provided buffers and returns buffers while processing CQEs,
 * avoiding an extra atomic when needing to increment both the CQ ring and
 * the ring buffer index at the same time.
 */
static inline void
io_uring_buf_ring_cq_advance(struct io_uring* ring, struct io_uring_buf_ring* br, int count) {
    __io_uring_buf_ring_cq_advance(ring, br, count, count);
}

#ifndef LIBURING_INTERNAL
static inline struct io_uring_sqe* io_uring_get_sqe(struct io_uring* ring) {
    return _io_uring_get_sqe(ring);
}
#else
struct io_uring_sqe* io_uring_get_sqe(struct io_uring* ring);
#endif

ssize_t io_uring_mlock_size(unsigned entries, unsigned flags);
ssize_t io_uring_mlock_size_params(unsigned entries, struct io_uring_params* p);


/*
 * Returns true if we're not using SQ thread (thus nobody submits but us)
 * or if IORING_SQ_NEED_WAKEUP is set, so submit thread must be explicitly
 * awakened. For the latter case, we set the thread wakeup flag.
 * If no SQEs are ready for submission, returns false.
 */
static inline bool sq_ring_needs_enter(struct io_uring* ring, unsigned submit, unsigned* flags) {
    if (!submit)
        return false;

    if (!(ring->flags & IORING_SETUP_SQPOLL))
        return true;

    /*
     * Ensure the kernel can see the store to the SQ tail before we read
     * the flags.
     */
    io_uring_smp_mb();

    if (uring_unlikely(IO_URING_READ_ONCE(*ring->sq.kflags) & IORING_SQ_NEED_WAKEUP)) {
        *flags |= IORING_ENTER_SQ_WAKEUP;
        return true;
    }

    return false;
}

static inline bool cq_ring_needs_flush(struct io_uring* ring) {
    return IO_URING_READ_ONCE(*ring->sq.kflags) & (IORING_SQ_CQ_OVERFLOW | IORING_SQ_TASKRUN);
}

static inline bool cq_ring_needs_enter(struct io_uring* ring) {
    return (ring->flags & IORING_SETUP_IOPOLL) || cq_ring_needs_flush(ring);
}

struct get_data {
    unsigned submit;
    unsigned wait_nr;
    unsigned get_flags;
    int      sz;
    int      has_ts;
    void*    arg;
};

static int _io_uring_get_cqe(struct io_uring* ring, struct io_uring_cqe** cqe_ptr, struct get_data* data) {
    struct io_uring_cqe* cqe    = NULL;
    bool                 looped = false;
    int                  err    = 0;

    do {
        bool     need_enter = false;
        unsigned flags      = 0;
        unsigned nr_available;
        int      ret;

        ret = __io_uring_peek_cqe(ring, &cqe, &nr_available);
        if (ret) {
            if (!err)
                err = ret;
            break;
        }
        if (!cqe && !data->wait_nr && !data->submit) {
            /*
             * If we already looped once, we already entered
             * the kernel. Since there's nothing to submit or
             * wait for, don't keep retrying.
             */
            if (looped || !cq_ring_needs_enter(ring)) {
                if (!err)
                    err = -EAGAIN;
                break;
            }
            need_enter = true;
        }
        if (data->wait_nr > nr_available || need_enter) {
            flags      = IORING_ENTER_GETEVENTS | data->get_flags;
            need_enter = true;
        }
        if (sq_ring_needs_enter(ring, data->submit, &flags))
            need_enter = true;
        if (!need_enter)
            break;
        if (looped && data->has_ts) {
            auto* arg = (io_uring_getevents_arg*) data->arg;

            if (!cqe && arg->ts && !err)
                err = -ETIME;
            break;
        }

        if (ring->int_flags & INT_FLAG_REG_RING)
            flags |= IORING_ENTER_REGISTERED_RING;
        ret =
          __sys_io_uring_enter2(ring->enter_ring_fd, data->submit, data->wait_nr, flags, data->arg, data->sz);
        if (ret < 0) {
            if (!err)
                err = ret;
            break;
        }

        data->submit -= ret;
        if (cqe)
            break;
        if (!looped) {
            looped = true;
            err    = ret;
        }
    } while (1);

    *cqe_ptr = cqe;
    return err;
}

int __io_uring_get_cqe(struct io_uring*      ring,
                       struct io_uring_cqe** cqe_ptr,
                       unsigned              submit,
                       unsigned              wait_nr,
                       sigset_t*             sigmask) {
    struct get_data data = {
      .submit    = submit,
      .wait_nr   = wait_nr,
      .get_flags = 0,
      .sz        = _NSIG / 8,
      .arg       = sigmask,
    };

    return _io_uring_get_cqe(ring, cqe_ptr, &data);
}

int io_uring_get_events(struct io_uring* ring) {
    int flags = IORING_ENTER_GETEVENTS;

    if (ring->int_flags & INT_FLAG_REG_RING)
        flags |= IORING_ENTER_REGISTERED_RING;
    return __sys_io_uring_enter(ring->enter_ring_fd, 0, 0, flags, NULL);
}

/*
 * Fill in an array of IO completions up to count, if any are available.
 * Returns the amount of IO completions filled.
 */
unsigned io_uring_peek_batch_cqe(struct io_uring* ring, struct io_uring_cqe** cqes, unsigned count) {
    unsigned ready;
    bool     overflow_checked = false;
    int      shift            = 0;

    if (ring->flags & IORING_SETUP_CQE32)
        shift = 1;

again:
    ready = io_uring_cq_ready(ring);
    if (ready) {
        unsigned head = *ring->cq.khead;
        unsigned mask = ring->cq.ring_mask;
        unsigned last;
        int      i = 0;

        count = count > ready ? ready : count;
        last  = head + count;
        for (; head != last; head++, i++)
            cqes[i] = &ring->cq.cqes[(head & mask) << shift];

        return count;
    }

    if (overflow_checked)
        return 0;

    if (cq_ring_needs_flush(ring)) {
        io_uring_get_events(ring);
        overflow_checked = true;
        goto again;
    }

    return 0;
}

/*
 * Sync internal state with kernel ring state on the SQ side. Returns the
 * number of pending items in the SQ ring, for the shared ring.
 */
static unsigned __io_uring_flush_sq(struct io_uring* ring) {
    struct io_uring_sq* sq   = &ring->sq;
    unsigned            tail = sq->sqe_tail;

    if (sq->sqe_head != tail) {
        sq->sqe_head = tail;
        /*
         * Ensure kernel sees the SQE updates before the tail update.
         */
        if (!(ring->flags & IORING_SETUP_SQPOLL))
            IO_URING_WRITE_ONCE(*sq->ktail, tail);
        else
            io_uring_smp_store_release(sq->ktail, tail);
    }
    /*
     * This _may_ look problematic, as we're not supposed to be reading
     * SQ->head without acquire semantics. When we're in SQPOLL mode, the
     * kernel submitter could be updating this right now. For non-SQPOLL,
     * task itself does it, and there's no potential race. But even for
     * SQPOLL, the load is going to be potentially out-of-date the very
     * instant it's done, regardless or whether or not it's done
     * atomically. Worst case, we're going to be over-estimating what
     * we can submit. The point is, we need to be able to deal with this
     * situation regardless of any perceived atomicity.
     */
    return tail - *sq->khead;
}

/*
 * If we have kernel support for IORING_ENTER_EXT_ARG, then we can use that
 * more efficiently than queueing an internal timeout command.
 */
static int io_uring_wait_cqes_new(struct io_uring*          ring,
                                  struct io_uring_cqe**     cqe_ptr,
                                  unsigned                  wait_nr,
                                  struct __kernel_timespec* ts,
                                  sigset_t*                 sigmask) {
    struct io_uring_getevents_arg arg  = {.sigmask    = (unsigned long) sigmask,
                                          .sigmask_sz = _NSIG / 8,
                                          .ts         = (unsigned long) ts};
    struct get_data               data = {.wait_nr   = wait_nr,
                                          .get_flags = IORING_ENTER_EXT_ARG,
                                          .sz        = sizeof(arg),
                                          .has_ts    = ts != NULL,
                                          .arg       = &arg};

    return _io_uring_get_cqe(ring, cqe_ptr, &data);
}

/*
 * Like io_uring_wait_cqe(), except it accepts a timeout value as well. Note
 * that an sqe is used internally to handle the timeout. For kernel doesn't
 * support IORING_FEAT_EXT_ARG, applications using this function must never
 * set sqe->user_data to LIBURING_UDATA_TIMEOUT!
 *
 * For kernels without IORING_FEAT_EXT_ARG (5.10 and older), if 'ts' is
 * specified, the application need not call io_uring_submit() before
 * calling this function, as we will do that on its behalf. From this it also
 * follows that this function isn't safe to use for applications that split SQ
 * and CQ handling between two threads and expect that to work without
 * synchronization, as this function manipulates both the SQ and CQ side.
 *
 * For kernels with IORING_FEAT_EXT_ARG, no implicit submission is done and
 * hence this function is safe to use for applications that split SQ and CQ
 * handling between two threads.
 */
static int __io_uring_submit_timeout(struct io_uring* ring, unsigned wait_nr, struct __kernel_timespec* ts) {
    struct io_uring_sqe* sqe;
    int                  ret;

    /*
     * If the SQ ring is full, we may need to submit IO first
     */
    sqe = io_uring_get_sqe(ring);
    if (!sqe) {
        ret = io_uring_submit(ring);
        if (ret < 0)
            return ret;
        sqe = io_uring_get_sqe(ring);
        if (!sqe)
            return -EAGAIN;
    }
    io_uring_prep_timeout(sqe, ts, wait_nr, 0);
    sqe->user_data = LIBURING_UDATA_TIMEOUT;
    return __io_uring_flush_sq(ring);
}

int io_uring_wait_cqes(struct io_uring*          ring,
                       struct io_uring_cqe**     cqe_ptr,
                       unsigned                  wait_nr,
                       struct __kernel_timespec* ts,
                       sigset_t*                 sigmask) {
    int to_submit = 0;

    if (ts) {
        if (ring->features & IORING_FEAT_EXT_ARG)
            return io_uring_wait_cqes_new(ring, cqe_ptr, wait_nr, ts, sigmask);
        to_submit = __io_uring_submit_timeout(ring, wait_nr, ts);
        if (to_submit < 0)
            return to_submit;
    }

    return __io_uring_get_cqe(ring, cqe_ptr, to_submit, wait_nr, sigmask);
}

int io_uring_submit_and_wait_timeout(struct io_uring*          ring,
                                     struct io_uring_cqe**     cqe_ptr,
                                     unsigned                  wait_nr,
                                     struct __kernel_timespec* ts,
                                     sigset_t*                 sigmask) {
    int to_submit;

    if (ts) {
        if (ring->features & IORING_FEAT_EXT_ARG) {
            struct io_uring_getevents_arg arg  = {.sigmask    = (unsigned long) sigmask,
                                                  .sigmask_sz = _NSIG / 8,
                                                  .ts         = (unsigned long) ts};
            struct get_data               data = {.submit    = __io_uring_flush_sq(ring),
                                                  .wait_nr   = wait_nr,
                                                  .get_flags = IORING_ENTER_EXT_ARG,
                                                  .sz        = sizeof(arg),
                                                  .has_ts    = ts != NULL,
                                                  .arg       = &arg};

            return _io_uring_get_cqe(ring, cqe_ptr, &data);
        }
        to_submit = __io_uring_submit_timeout(ring, wait_nr, ts);
        if (to_submit < 0)
            return to_submit;
    } else
        to_submit = __io_uring_flush_sq(ring);

    return __io_uring_get_cqe(ring, cqe_ptr, to_submit, wait_nr, sigmask);
}

/*
 * See io_uring_wait_cqes() - this function is the same, it just always uses
 * '1' as the wait_nr.
 */
int io_uring_wait_cqe_timeout(struct io_uring*          ring,
                              struct io_uring_cqe**     cqe_ptr,
                              struct __kernel_timespec* ts) {
    return io_uring_wait_cqes(ring, cqe_ptr, 1, ts, NULL);
}

/*
 * Submit sqes acquired from io_uring_get_sqe() to the kernel.
 *
 * Returns number of sqes submitted
 */
static int __io_uring_submit(struct io_uring* ring, unsigned submitted, unsigned wait_nr, bool getevents) {
    bool     cq_needs_enter = getevents || wait_nr || cq_ring_needs_enter(ring);
    unsigned flags;
    int      ret;

    flags = 0;
    if (sq_ring_needs_enter(ring, submitted, &flags) || cq_needs_enter) {
        if (cq_needs_enter)
            flags |= IORING_ENTER_GETEVENTS;
        if (ring->int_flags & INT_FLAG_REG_RING)
            flags |= IORING_ENTER_REGISTERED_RING;

        ret = __sys_io_uring_enter(ring->enter_ring_fd, submitted, wait_nr, flags, NULL);
    } else
        ret = submitted;

    return ret;
}

static int __io_uring_submit_and_wait(struct io_uring* ring, unsigned wait_nr) {
    return __io_uring_submit(ring, __io_uring_flush_sq(ring), wait_nr, false);
}

/*
 * Submit sqes acquired from io_uring_get_sqe() to the kernel.
 *
 * Returns number of sqes submitted
 */
int io_uring_submit(struct io_uring* ring) {
    return __io_uring_submit_and_wait(ring, 0);
}

/*
 * Like io_uring_submit(), but allows waiting for events as well.
 *
 * Returns number of sqes submitted
 */
int io_uring_submit_and_wait(struct io_uring* ring, unsigned wait_nr) {
    return __io_uring_submit_and_wait(ring, wait_nr);
}

int io_uring_submit_and_get_events(struct io_uring* ring) {
    return __io_uring_submit(ring, __io_uring_flush_sq(ring), 0, true);
}

#ifdef LIBURING_INTERNAL
struct io_uring_sqe* io_uring_get_sqe(struct io_uring* ring) {
    return _io_uring_get_sqe(ring);
}
#endif

int __io_uring_sqring_wait(struct io_uring* ring) {
    int flags = IORING_ENTER_SQ_WAIT;

    if (ring->int_flags & INT_FLAG_REG_RING)
        flags |= IORING_ENTER_REGISTERED_RING;

    return __sys_io_uring_enter(ring->enter_ring_fd, 0, 0, flags, NULL);
}
static inline int
do_register(struct io_uring* ring, unsigned int opcode, const void* arg, unsigned int nr_args) {
    int fd;

    if (ring->int_flags & INT_FLAG_REG_REG_RING) {
        opcode |= IORING_REGISTER_USE_REGISTERED_RING;
        fd = ring->enter_ring_fd;
    } else {
        fd = ring->ring_fd;
    }

    return __sys_io_uring_register(fd, opcode, arg, nr_args);
}

int io_uring_register_buffers_update_tag(struct io_uring*    ring,
                                         unsigned            off,
                                         const struct iovec* iovecs,
                                         const __u64*        tags,
                                         unsigned            nr) {
    struct io_uring_rsrc_update2 up = {
      .offset = off,
      .data   = (unsigned long) iovecs,
      .tags   = (unsigned long) tags,
      .nr     = nr,
    };

    return do_register(ring, IORING_REGISTER_BUFFERS_UPDATE, &up, sizeof(up));
}

int io_uring_register_buffers_tags(struct io_uring*    ring,
                                   const struct iovec* iovecs,
                                   const __u64*        tags,
                                   unsigned            nr) {
    struct io_uring_rsrc_register reg = {
      .nr   = nr,
      .data = (unsigned long) iovecs,
      .tags = (unsigned long) tags,
    };

    return do_register(ring, IORING_REGISTER_BUFFERS2, &reg, sizeof(reg));
}

int io_uring_register_buffers_sparse(struct io_uring* ring, unsigned nr) {
    struct io_uring_rsrc_register reg = {
      .flags = IORING_RSRC_REGISTER_SPARSE,
      .nr    = nr,
    };

    return do_register(ring, IORING_REGISTER_BUFFERS2, &reg, sizeof(reg));
}

int io_uring_register_buffers(struct io_uring* ring, const struct iovec* iovecs, unsigned nr_iovecs) {
    return do_register(ring, IORING_REGISTER_BUFFERS, iovecs, nr_iovecs);
}

int io_uring_unregister_buffers(struct io_uring* ring) {
    return do_register(ring, IORING_UNREGISTER_BUFFERS, NULL, 0);
}

int io_uring_register_files_update_tag(struct io_uring* ring,
                                       unsigned         off,
                                       const int*       files,
                                       const __u64*     tags,
                                       unsigned         nr_files) {
    struct io_uring_rsrc_update2 up = {
      .offset = off,
      .data   = (unsigned long) files,
      .tags   = (unsigned long) tags,
      .nr     = nr_files,
    };

    return do_register(ring, IORING_REGISTER_FILES_UPDATE2, &up, sizeof(up));
}

/*
 * Register an update for an existing file set. The updates will start at
 * 'off' in the original array, and 'nr_files' is the number of files we'll
 * update.
 *
 * Returns number of files updated on success, -ERROR on failure.
 */
int io_uring_register_files_update(struct io_uring* ring, unsigned off, const int* files, unsigned nr_files) {
    struct io_uring_files_update up = {
      .offset = off,
      .fds    = (unsigned long) files,
    };

    return do_register(ring, IORING_REGISTER_FILES_UPDATE, &up, nr_files);
}

static int increase_rlimit_nofile(unsigned nr) {
    int           ret;
    struct rlimit rlim;

    ret = __sys_getrlimit(RLIMIT_NOFILE, &rlim);
    if (ret < 0)
        return ret;

    if (rlim.rlim_cur < nr) {
        rlim.rlim_cur += nr;
        __sys_setrlimit(RLIMIT_NOFILE, &rlim);
    }

    return 0;
}

int io_uring_register_files_sparse(struct io_uring* ring, unsigned nr) {
    struct io_uring_rsrc_register reg = {
      .flags = IORING_RSRC_REGISTER_SPARSE,
      .nr    = nr,
    };
    int ret, did_increase = 0;

    do {
        ret = do_register(ring, IORING_REGISTER_FILES2, &reg, sizeof(reg));
        if (ret >= 0)
            break;
        if (ret == -EMFILE && !did_increase) {
            did_increase = 1;
            increase_rlimit_nofile(nr);
            continue;
        }
        break;
    } while (1);

    return ret;
}

int io_uring_register_files_tags(struct io_uring* ring, const int* files, const __u64* tags, unsigned nr) {
    struct io_uring_rsrc_register reg = {
      .nr   = nr,
      .data = (unsigned long) files,
      .tags = (unsigned long) tags,
    };
    int ret, did_increase = 0;

    do {
        ret = do_register(ring, IORING_REGISTER_FILES2, &reg, sizeof(reg));
        if (ret >= 0)
            break;
        if (ret == -EMFILE && !did_increase) {
            did_increase = 1;
            increase_rlimit_nofile(nr);
            continue;
        }
        break;
    } while (1);

    return ret;
}

int io_uring_register_files(struct io_uring* ring, const int* files, unsigned nr_files) {
    int ret, did_increase = 0;

    do {
        ret = do_register(ring, IORING_REGISTER_FILES, files, nr_files);
        if (ret >= 0)
            break;
        if (ret == -EMFILE && !did_increase) {
            did_increase = 1;
            increase_rlimit_nofile(nr_files);
            continue;
        }
        break;
    } while (1);

    return ret;
}

int io_uring_unregister_files(struct io_uring* ring) {
    return do_register(ring, IORING_UNREGISTER_FILES, NULL, 0);
}

int io_uring_register_eventfd(struct io_uring* ring, int event_fd) {
    return do_register(ring, IORING_REGISTER_EVENTFD, &event_fd, 1);
}

int io_uring_unregister_eventfd(struct io_uring* ring) {
    return do_register(ring, IORING_UNREGISTER_EVENTFD, NULL, 0);
}

int io_uring_register_eventfd_async(struct io_uring* ring, int event_fd) {
    return do_register(ring, IORING_REGISTER_EVENTFD_ASYNC, &event_fd, 1);
}

int io_uring_register_probe(struct io_uring* ring, struct io_uring_probe* p, unsigned int nr_ops) {
    return do_register(ring, IORING_REGISTER_PROBE, p, nr_ops);
}

int io_uring_register_personality(struct io_uring* ring) {
    return do_register(ring, IORING_REGISTER_PERSONALITY, NULL, 0);
}

int io_uring_unregister_personality(struct io_uring* ring, int id) {
    return do_register(ring, IORING_UNREGISTER_PERSONALITY, NULL, id);
}

int io_uring_register_restrictions(struct io_uring*             ring,
                                   struct io_uring_restriction* res,
                                   unsigned int                 nr_res) {
    return do_register(ring, IORING_REGISTER_RESTRICTIONS, res, nr_res);
}

int io_uring_enable_rings(struct io_uring* ring) {
    return do_register(ring, IORING_REGISTER_ENABLE_RINGS, NULL, 0);
}

int io_uring_register_iowq_aff(struct io_uring* ring, size_t cpusz, const cpu_set_t* mask) {
    if (cpusz >= (1U << 31))
        return -EINVAL;

    return do_register(ring, IORING_REGISTER_IOWQ_AFF, mask, (int) cpusz);
}

int io_uring_unregister_iowq_aff(struct io_uring* ring) {
    return do_register(ring, IORING_UNREGISTER_IOWQ_AFF, NULL, 0);
}

int io_uring_register_iowq_max_workers(struct io_uring* ring, unsigned int* val) {
    return do_register(ring, IORING_REGISTER_IOWQ_MAX_WORKERS, val, 2);
}

int io_uring_register_ring_fd(struct io_uring* ring) {
    struct io_uring_rsrc_update up = {
      .data   = ring->ring_fd,
      .offset = -1U,
    };
    int ret;

    if (ring->int_flags & INT_FLAG_REG_RING)
        return -EEXIST;

    ret = do_register(ring, IORING_REGISTER_RING_FDS, &up, 1);
    if (ret == 1) {
        ring->enter_ring_fd = up.offset;
        ring->int_flags |= INT_FLAG_REG_RING;
        if (ring->features & IORING_FEAT_REG_REG_RING) {
            ring->int_flags |= INT_FLAG_REG_REG_RING;
        }
    }
    return ret;
}


int io_uring_unregister_ring_fd(struct io_uring* ring) {
    struct io_uring_rsrc_update up = {
      .offset = ring->enter_ring_fd,
    };
    int ret;

    if (!(ring->int_flags & INT_FLAG_REG_RING))
        return -EINVAL;

    ret = do_register(ring, IORING_UNREGISTER_RING_FDS, &up, 1);
    if (ret == 1) {
        ring->enter_ring_fd = ring->ring_fd;
        ring->int_flags &= ~(INT_FLAG_REG_RING | INT_FLAG_REG_REG_RING);
    }
    return ret;
}

int io_uring_close_ring_fd(struct io_uring* ring) {
    if (!(ring->features & IORING_FEAT_REG_REG_RING))
        return -EOPNOTSUPP;
    if (!(ring->int_flags & INT_FLAG_REG_RING))
        return -EINVAL;
    if (ring->ring_fd == -1)
        return -EBADF;

    __sys_close(ring->ring_fd);
    ring->ring_fd = -1;
    return 1;
}

int io_uring_register_buf_ring(struct io_uring*            ring,
                               struct io_uring_buf_reg*    reg,
                               unsigned int __maybe_unused flags) {
    return do_register(ring, IORING_REGISTER_PBUF_RING, reg, 1);
}

int io_uring_unregister_buf_ring(struct io_uring* ring, int bgid) {
    struct io_uring_buf_reg reg = {.bgid = bgid};

    return do_register(ring, IORING_UNREGISTER_PBUF_RING, &reg, 1);
}

int io_uring_register_sync_cancel(struct io_uring* ring, struct io_uring_sync_cancel_reg* reg) {
    return do_register(ring, IORING_REGISTER_SYNC_CANCEL, reg, 1);
}

int io_uring_register_file_alloc_range(struct io_uring* ring, unsigned off, unsigned len) {
    struct io_uring_file_index_range range = {.off = off, .len = len};

    return do_register(ring, IORING_REGISTER_FILE_ALLOC_RANGE, &range, 0);
}



#define KERN_MAX_ENTRIES    32768
#define KERN_MAX_CQ_ENTRIES (2 * KERN_MAX_ENTRIES)

static inline int __fls(int x) {
    if (!x)
        return 0;
    return 8 * sizeof(x) - __builtin_clz(x);
}

static unsigned roundup_pow2(unsigned depth) {
    return 1U << __fls(depth - 1);
}

static int get_sq_cq_entries(unsigned entries, struct io_uring_params* p, unsigned* sq, unsigned* cq) {
    unsigned cq_entries;

    if (!entries)
        return -EINVAL;
    if (entries > KERN_MAX_ENTRIES) {
        if (!(p->flags & IORING_SETUP_CLAMP))
            return -EINVAL;
        entries = KERN_MAX_ENTRIES;
    }

    entries = roundup_pow2(entries);
    if (p->flags & IORING_SETUP_CQSIZE) {
        if (!p->cq_entries)
            return -EINVAL;
        cq_entries = p->cq_entries;
        if (cq_entries > KERN_MAX_CQ_ENTRIES) {
            if (!(p->flags & IORING_SETUP_CLAMP))
                return -EINVAL;
            cq_entries = KERN_MAX_CQ_ENTRIES;
        }
        cq_entries = roundup_pow2(cq_entries);
        if (cq_entries < entries)
            return -EINVAL;
    } else {
        cq_entries = 2 * entries;
    }

    *sq = entries;
    *cq = cq_entries;
    return 0;
}

static void io_uring_unmap_rings(struct io_uring_sq* sq, struct io_uring_cq* cq) {
    if (sq->ring_sz)
        __sys_munmap(sq->ring_ptr, sq->ring_sz);
    if (cq->ring_ptr && cq->ring_sz && cq->ring_ptr != sq->ring_ptr)
        __sys_munmap(cq->ring_ptr, cq->ring_sz);
}

static void
io_uring_setup_ring_pointers(struct io_uring_params* p, struct io_uring_sq* sq, struct io_uring_cq* cq) {
    sq->khead         = sq->ring_ptr + p->sq_off.head;
    sq->ktail         = sq->ring_ptr + p->sq_off.tail;
    sq->kring_mask    = sq->ring_ptr + p->sq_off.ring_mask;
    sq->kring_entries = sq->ring_ptr + p->sq_off.ring_entries;
    sq->kflags        = sq->ring_ptr + p->sq_off.flags;
    sq->kdropped      = sq->ring_ptr + p->sq_off.dropped;
    sq->array         = sq->ring_ptr + p->sq_off.array;

    cq->khead         = cq->ring_ptr + p->cq_off.head;
    cq->ktail         = cq->ring_ptr + p->cq_off.tail;
    cq->kring_mask    = cq->ring_ptr + p->cq_off.ring_mask;
    cq->kring_entries = cq->ring_ptr + p->cq_off.ring_entries;
    cq->koverflow     = cq->ring_ptr + p->cq_off.overflow;
    cq->cqes          = cq->ring_ptr + p->cq_off.cqes;
    if (p->cq_off.flags)
        cq->kflags = cq->ring_ptr + p->cq_off.flags;

    sq->ring_mask    = *sq->kring_mask;
    sq->ring_entries = *sq->kring_entries;
    cq->ring_mask    = *cq->kring_mask;
    cq->ring_entries = *cq->kring_entries;
}

static int io_uring_mmap(int fd, struct io_uring_params* p, struct io_uring_sq* sq, struct io_uring_cq* cq) {
    size_t size;
    int    ret;

    size = sizeof(struct io_uring_cqe);
    if (p->flags & IORING_SETUP_CQE32)
        size += sizeof(struct io_uring_cqe);

    sq->ring_sz = p->sq_off.array + p->sq_entries * sizeof(unsigned);
    cq->ring_sz = p->cq_off.cqes + p->cq_entries * size;

    if (p->features & IORING_FEAT_SINGLE_MMAP) {
        if (cq->ring_sz > sq->ring_sz)
            sq->ring_sz = cq->ring_sz;
        cq->ring_sz = sq->ring_sz;
    }
    sq->ring_ptr =
      __sys_mmap(0, sq->ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQ_RING);
    if (IS_ERR(sq->ring_ptr))
        return PTR_ERR(sq->ring_ptr);

    if (p->features & IORING_FEAT_SINGLE_MMAP) {
        cq->ring_ptr = sq->ring_ptr;
    } else {
        cq->ring_ptr = __sys_mmap(0,
                                  cq->ring_sz,
                                  PROT_READ | PROT_WRITE,
                                  MAP_SHARED | MAP_POPULATE,
                                  fd,
                                  IORING_OFF_CQ_RING);
        if (IS_ERR(cq->ring_ptr)) {
            ret          = PTR_ERR(cq->ring_ptr);
            cq->ring_ptr = NULL;
            goto err;
        }
    }

    size = sizeof(struct io_uring_sqe);
    if (p->flags & IORING_SETUP_SQE128)
        size += 64;
    sq->sqes = __sys_mmap(0,
                          size * p->sq_entries,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED | MAP_POPULATE,
                          fd,
                          IORING_OFF_SQES);
    if (IS_ERR(sq->sqes)) {
        ret = PTR_ERR(sq->sqes);
    err:
        io_uring_unmap_rings(sq, cq);
        return ret;
    }

    io_uring_setup_ring_pointers(p, sq, cq);
    return 0;
}

/*
 * For users that want to specify sq_thread_cpu or sq_thread_idle, this
 * interface is a convenient helper for mmap()ing the rings.
 * Returns -errno on error, or zero on success.  On success, 'ring'
 * contains the necessary information to read/write to the rings.
 */
__cold int io_uring_queue_mmap(int fd, struct io_uring_params* p, struct io_uring* ring) {
    memset(ring, 0, sizeof(*ring));
    return io_uring_mmap(fd, p, &ring->sq, &ring->cq);
}

/*
 * Ensure that the mmap'ed rings aren't available to a child after a fork(2).
 * This uses madvise(..., MADV_DONTFORK) on the mmap'ed ranges.
 */
__cold int io_uring_ring_dontfork(struct io_uring* ring) {
    size_t len;
    int    ret;

    if (!ring->sq.ring_ptr || !ring->sq.sqes || !ring->cq.ring_ptr)
        return -EINVAL;

    len = sizeof(struct io_uring_sqe);
    if (ring->flags & IORING_SETUP_SQE128)
        len += 64;
    len *= ring->sq.ring_entries;
    ret = __sys_madvise(ring->sq.sqes, len, MADV_DONTFORK);
    if (ret < 0)
        return ret;

    len = ring->sq.ring_sz;
    ret = __sys_madvise(ring->sq.ring_ptr, len, MADV_DONTFORK);
    if (ret < 0)
        return ret;

    if (ring->cq.ring_ptr != ring->sq.ring_ptr) {
        len = ring->cq.ring_sz;
        ret = __sys_madvise(ring->cq.ring_ptr, len, MADV_DONTFORK);
        if (ret < 0)
            return ret;
    }

    return 0;
}

/* FIXME */
static size_t huge_page_size = 2 * 1024 * 1024;

/*
 * Returns negative for error, or number of bytes used in the buffer on success
 */
static int io_uring_alloc_huge(unsigned                entries,
                               struct io_uring_params* p,
                               struct io_uring_sq*     sq,
                               struct io_uring_cq*     cq,
                               void*                   buf,
                               size_t                  buf_size) {
    unsigned long page_size = get_page_size();
    unsigned      sq_entries, cq_entries;
    size_t        ring_mem, sqes_mem;
    unsigned long mem_used = 0;
    void*         ptr;
    int           ret;

    ret = get_sq_cq_entries(entries, p, &sq_entries, &cq_entries);
    if (ret)
        return ret;

    sqes_mem = sq_entries * sizeof(struct io_uring_sqe);
    sqes_mem = (sqes_mem + page_size - 1) & ~(page_size - 1);
    ring_mem = cq_entries * sizeof(struct io_uring_cqe);
    if (p->flags & IORING_SETUP_CQE32)
        ring_mem *= 2;
    ring_mem += sq_entries * sizeof(unsigned);
    mem_used = sqes_mem + ring_mem;
    mem_used = (mem_used + page_size - 1) & ~(page_size - 1);

    /*
     * A maxed-out number of CQ entries with IORING_SETUP_CQE32 fills a 2MB
     * huge page by itself, so the SQ entries won't fit in the same huge
     * page. For SQEs, that shouldn't be possible given KERN_MAX_ENTRIES,
     * but check that too to future-proof (e.g. against different huge page
     * sizes). Bail out early so we don't overrun.
     */
    if (!buf && (sqes_mem > huge_page_size || ring_mem > huge_page_size))
        return -ENOMEM;

    if (buf) {
        if (mem_used > buf_size)
            return -ENOMEM;
        ptr = buf;
    } else {
        int map_hugetlb = 0;
        if (sqes_mem <= page_size)
            buf_size = page_size;
        else {
            buf_size    = huge_page_size;
            map_hugetlb = MAP_HUGETLB;
        }
        ptr =
          __sys_mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | map_hugetlb, -1, 0);
        if (IS_ERR(ptr))
            return PTR_ERR(ptr);
    }

    sq->sqes = ptr;
    if (mem_used <= buf_size) {
        sq->ring_ptr = (void*) sq->sqes + sqes_mem;
        /* clear ring sizes, we have just one mmap() to undo */
        cq->ring_sz = 0;
        sq->ring_sz = 0;
    } else {
        int map_hugetlb = 0;
        if (ring_mem <= page_size)
            buf_size = page_size;
        else {
            buf_size    = huge_page_size;
            map_hugetlb = MAP_HUGETLB;
        }
        ptr =
          __sys_mmap(NULL, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | map_hugetlb, -1, 0);
        if (IS_ERR(ptr)) {
            __sys_munmap(sq->sqes, 1);
            return PTR_ERR(ptr);
        }
        sq->ring_ptr = ptr;
        sq->ring_sz  = buf_size;
        cq->ring_sz  = 0;
    }

    cq->ring_ptr        = (void*) sq->ring_ptr;
    p->sq_off.user_addr = (unsigned long) sq->sqes;
    p->cq_off.user_addr = (unsigned long) sq->ring_ptr;
    return (int) mem_used;
}

static int __io_uring_queue_init_params(unsigned                entries,
                                        struct io_uring*        ring,
                                        struct io_uring_params* p,
                                        void*                   buf,
                                        size_t                  buf_size) {
    int       fd, ret = 0;
    unsigned* sq_array;
    unsigned  sq_entries, index;

    memset(ring, 0, sizeof(*ring));

    /*
     * The kernel does this check already, but checking it here allows us
     * to avoid handling it below.
     */
    if (p->flags & IORING_SETUP_REGISTERED_FD_ONLY && !(p->flags & IORING_SETUP_NO_MMAP))
        return -EINVAL;

    if (p->flags & IORING_SETUP_NO_MMAP) {
        ret = io_uring_alloc_huge(entries, p, &ring->sq, &ring->cq, buf, buf_size);
        if (ret < 0)
            return ret;
        if (buf)
            ring->int_flags |= INT_FLAG_APP_MEM;
    }

    fd = __sys_io_uring_setup(entries, p);
    if (fd < 0) {
        if ((p->flags & IORING_SETUP_NO_MMAP) && !(ring->int_flags & INT_FLAG_APP_MEM)) {
            __sys_munmap(ring->sq.sqes, 1);
            io_uring_unmap_rings(&ring->sq, &ring->cq);
        }
        return fd;
    }

    if (!(p->flags & IORING_SETUP_NO_MMAP)) {
        ret = io_uring_queue_mmap(fd, p, ring);
        if (ret) {
            __sys_close(fd);
            return ret;
        }
    } else {
        io_uring_setup_ring_pointers(p, &ring->sq, &ring->cq);
    }

    /*
     * Directly map SQ slots to SQEs
     */
    sq_array   = ring->sq.array;
    sq_entries = ring->sq.ring_entries;
    for (index = 0; index < sq_entries; index++)
        sq_array[index] = index;

    ring->features      = p->features;
    ring->flags         = p->flags;
    ring->enter_ring_fd = fd;
    if (p->flags & IORING_SETUP_REGISTERED_FD_ONLY) {
        ring->ring_fd = -1;
        ring->int_flags |= INT_FLAG_REG_RING | INT_FLAG_REG_REG_RING;
    } else {
        ring->ring_fd = fd;
    }

    return ret;
}

/*
 * Like io_uring_queue_init_params(), except it allows the application to pass
 * in a pre-allocated memory range that is used for the shared data between
 * the kernel and the application. This includes the sqes array, and the two
 * rings. The memory must be contigious, the use case here is that the app
 * allocates a huge page and passes it in.
 *
 * Returns the number of bytes used in the buffer, the app can then reuse
 * the buffer with the returned offset to put more rings in the same huge
 * page. Returns -ENOMEM if there's not enough room left in the buffer to
 * host the ring.
 */
int io_uring_queue_init_mem(unsigned                entries,
                            struct io_uring*        ring,
                            struct io_uring_params* p,
                            void*                   buf,
                            size_t                  buf_size) {
    /* should already be set... */
    p->flags |= IORING_SETUP_NO_MMAP;
    return __io_uring_queue_init_params(entries, ring, p, buf, buf_size);
}

int io_uring_queue_init_params(unsigned entries, struct io_uring* ring, struct io_uring_params* p) {
    int ret;

    ret = __io_uring_queue_init_params(entries, ring, p, NULL, 0);
    return ret >= 0 ? 0 : ret;
}

/*
 * Returns -errno on error, or zero on success. On success, 'ring'
 * contains the necessary information to read/write to the rings.
 */
__cold int io_uring_queue_init(unsigned entries, struct io_uring* ring, unsigned flags) {
    struct io_uring_params p;

    memset(&p, 0, sizeof(p));
    p.flags = flags;

    return io_uring_queue_init_params(entries, ring, &p);
}

__cold void io_uring_queue_exit(struct io_uring* ring) {
    struct io_uring_sq* sq = &ring->sq;
    struct io_uring_cq* cq = &ring->cq;
    size_t              sqe_size;

    if (!sq->ring_sz) {
        sqe_size = sizeof(struct io_uring_sqe);
        if (ring->flags & IORING_SETUP_SQE128)
            sqe_size += 64;
        __sys_munmap(sq->sqes, sqe_size * sq->ring_entries);
        io_uring_unmap_rings(sq, cq);
    } else {
        if (!(ring->int_flags & INT_FLAG_APP_MEM)) {
            __sys_munmap(sq->sqes, *sq->kring_entries * sizeof(struct io_uring_sqe));
            io_uring_unmap_rings(sq, cq);
        }
    }

    /*
     * Not strictly required, but frees up the slot we used now rather
     * than at process exit time.
     */
    if (ring->int_flags & INT_FLAG_REG_RING)
        io_uring_unregister_ring_fd(ring);
    if (ring->ring_fd != -1)
        __sys_close(ring->ring_fd);
}

__cold struct io_uring_probe* io_uring_get_probe_ring(struct io_uring* ring) {
    struct io_uring_probe* probe;
    size_t                 len;
    int                    r;

    len   = sizeof(*probe) + 256 * sizeof(struct io_uring_probe_op);
    probe = malloc(len);
    if (!probe)
        return NULL;
    memset(probe, 0, len);

    r = io_uring_register_probe(ring, probe, 256);
    if (r >= 0)
        return probe;

    free(probe);
    return NULL;
}

__cold struct io_uring_probe* io_uring_get_probe(void) {
    struct io_uring        ring;
    struct io_uring_probe* probe;
    int                    r;

    r = io_uring_queue_init(2, &ring, 0);
    if (r < 0)
        return NULL;

    probe = io_uring_get_probe_ring(&ring);
    io_uring_queue_exit(&ring);
    return probe;
}

__cold void io_uring_free_probe(struct io_uring_probe* probe) {
    free(probe);
}

static size_t npages(size_t size, long page_size) {
    size--;
    size /= page_size;
    return __fls((int) size);
}

#define KRING_SIZE 320

static size_t rings_size(struct io_uring_params* p, unsigned entries, unsigned cq_entries, long page_size) {
    size_t pages, sq_size, cq_size;

    cq_size = sizeof(struct io_uring_cqe);
    if (p->flags & IORING_SETUP_CQE32)
        cq_size += sizeof(struct io_uring_cqe);
    cq_size *= cq_entries;
    cq_size += KRING_SIZE;
    cq_size = (cq_size + 63) & ~63UL;
    pages   = (size_t) 1 << npages(cq_size, page_size);

    sq_size = sizeof(struct io_uring_sqe);
    if (p->flags & IORING_SETUP_SQE128)
        sq_size += 64;
    sq_size *= entries;
    pages += (size_t) 1 << npages(sq_size, page_size);
    return pages * page_size;
}

/*
 * Return the required ulimit -l memlock memory required for a given ring
 * setup, in bytes. May return -errno on error. On newer (5.12+) kernels,
 * io_uring no longer requires any memlock memory, and hence this function
 * will return 0 for that case. On older (5.11 and prior) kernels, this will
 * return the required memory so that the caller can ensure that enough space
 * is available before setting up a ring with the specified parameters.
 */
__cold ssize_t io_uring_mlock_size_params(unsigned entries, struct io_uring_params* p) {
    struct io_uring_params lp;
    struct io_uring        ring;
    unsigned               cq_entries, sq;
    long                   page_size;
    ssize_t                ret;
    int                    cret;

    memset(&lp, 0, sizeof(lp));

    /*
     * We only really use this inited ring to see if the kernel is newer
     * or not. Newer kernels don't require memlocked memory. If we fail,
     * it's most likely because it's an older kernel and we have no
     * available memlock space. Just continue on, lp.features will still
     * be zeroed at this point and we'll do the right thing.
     */
    ret = io_uring_queue_init_params(entries, &ring, &lp);
    if (!ret)
        io_uring_queue_exit(&ring);

    /*
     * Native workers imply using cgroup memory accounting, and hence no
     * memlock memory is needed for the ring allocations.
     */
    if (lp.features & IORING_FEAT_NATIVE_WORKERS)
        return 0;

    if (!entries)
        return -EINVAL;
    if (entries > KERN_MAX_ENTRIES) {
        if (!(p->flags & IORING_SETUP_CLAMP))
            return -EINVAL;
        entries = KERN_MAX_ENTRIES;
    }

    cret = get_sq_cq_entries(entries, p, &sq, &cq_entries);
    if (cret)
        return cret;

    page_size = get_page_size();
    return rings_size(p, sq, cq_entries, page_size);
}

/*
 * Return required ulimit -l memory space for a given ring setup. See
 * @io_uring_mlock_size_params().
 */
__cold ssize_t io_uring_mlock_size(unsigned entries, unsigned flags) {
    struct io_uring_params p;

    memset(&p, 0, sizeof(p));
    p.flags = flags;
    return io_uring_mlock_size_params(entries, &p);
}

#if defined(__hppa__)
static struct io_uring_buf_ring*
br_setup(struct io_uring* ring, unsigned int nentries, int bgid, unsigned int flags, int* ret) {
    struct io_uring_buf_ring* br;
    struct io_uring_buf_reg   reg;
    size_t                    ring_size;
    off_t                     off;
    int                       lret;

    memset(&reg, 0, sizeof(reg));
    reg.ring_entries = nentries;
    reg.bgid         = bgid;
    reg.flags        = IOU_PBUF_RING_MMAP;

    *ret = 0;
    lret = io_uring_register_buf_ring(ring, &reg, flags);
    if (lret) {
        *ret = lret;
        return NULL;
    }

    off       = IORING_OFF_PBUF_RING | (unsigned long long) bgid << IORING_OFF_PBUF_SHIFT;
    ring_size = nentries * sizeof(struct io_uring_buf);
    br = __sys_mmap(NULL, ring_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring->ring_fd, off);
    if (IS_ERR(br)) {
        *ret = PTR_ERR(br);
        return NULL;
    }

    return br;
}
#else
static struct io_uring_buf_ring*
br_setup(struct io_uring* ring, unsigned int nentries, int bgid, unsigned int flags, int* ret) {
    struct io_uring_buf_ring* br;
    struct io_uring_buf_reg   reg;
    size_t                    ring_size;
    int                       lret;

    memset(&reg, 0, sizeof(reg));
    ring_size = nentries * sizeof(struct io_uring_buf);
    br        = __sys_mmap(NULL, ring_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (IS_ERR(br)) {
        *ret = PTR_ERR(br);
        return NULL;
    }

    reg.ring_addr    = (unsigned long) (uintptr_t) br;
    reg.ring_entries = nentries;
    reg.bgid         = bgid;

    *ret = 0;
    lret = io_uring_register_buf_ring(ring, &reg, flags);
    if (lret) {
        __sys_munmap(br, ring_size);
        *ret = lret;
        br   = NULL;
    }

    return br;
}
#endif

struct io_uring_buf_ring* io_uring_setup_buf_ring(struct io_uring* ring,
                                                  unsigned int     nentries,
                                                  int              bgid,
                                                  unsigned int     flags,
                                                  int*             ret) {
    struct io_uring_buf_ring* br = br_setup(ring, nentries, bgid, flags, ret);
    if (br)
        io_uring_buf_ring_init(br);

    return br;
}

int io_uring_free_buf_ring(struct io_uring*          ring,
                           struct io_uring_buf_ring* br,
                           unsigned int              nentries,
                           int                       bgid) {
    int ret = io_uring_unregister_buf_ring(ring, bgid);
    if (ret)
        return ret;

    __sys_munmap(br, nentries * sizeof(struct io_uring_buf));
    return 0;
}
}
#endif
