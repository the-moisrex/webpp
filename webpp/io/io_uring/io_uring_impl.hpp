#ifndef WEBPP_IOURING_IMPL_HPP
#define WEBPP_IOURING_IMPL_HPP

/**
 * This file is a modified version of liburing. Here's the license of liburing.
 * Portions derived from liburing (https://github.com/axboe/liburing)
 * Modified by The Moisrex, 2026.
 *
 * Copyright 2020 Jens Axboe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <linux/fs.h>
#include <linux/openat2.h>
#include <linux/time_types.h>
#include <linux/types.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// NOLINTBEGIN(*-magic-numbers, *-identifier-length)
namespace webpp::io::inline iouring_impl {
    struct io_uring_sq {
        unsigned            *khead;
        unsigned            *ktail;
        unsigned            *kring_mask;
        unsigned            *kring_entries;
        unsigned            *kflags;
        unsigned            *kdropped;
        unsigned            *array;
        struct io_uring_sqe *sqes;

        unsigned sqe_head;
        unsigned sqe_tail;

        size_t ring_sz;
        void  *ring_ptr;

        unsigned ring_mask;
        unsigned ring_entries;

        unsigned sqes_sz;
        unsigned pad;
    };

    struct io_uring_cqe {
        __u64 user_data;
        __s32 res;
        __u32 flags;

        // __u64 big_cqe[];
    };

    struct io_uring_cq {
        unsigned            *khead;
        unsigned            *ktail;
        unsigned            *kring_mask;
        unsigned            *kring_entries;
        unsigned            *kflags;
        unsigned            *koverflow;
        struct io_uring_cqe *cqes;

        size_t ring_sz;
        void  *ring_ptr;

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

    struct io_uring_zcrx_rq {
        __u32   *khead;
        __u32   *ktail;
        __u32    rq_tail;
        unsigned ring_entries;

        struct io_uring_zcrx_rqe *rqes;
        void                     *ring_ptr;
    };

    struct io_uring_sqe_command_fields {
        __u32 cmd_op;
        __u32 __pad1;
    };

    struct io_uring_sqe_socket_option_fields {
        __u32 level;
        __u32 optname;
    };

    struct io_uring_sqe_address_fields {
        __u16 addr_len;
        __u16 __pad3[1];
    };

    struct io_uring_sqe_third_address_fields {
        __u64 addr3;
        __u64 __pad2[1];
    };

    struct io_uring_sqe_attribute_fields {
        __u64 attr_ptr;
        __u64 attr_type_mask;
    };

    struct io_uring_sqe {
        __u8  opcode;
        __u8  flags;
        __u16 ioprio;
        __s32 fd;

        union {
            __u64 off;
            __u64 addr2;

            io_uring_sqe_command_fields command;
        };

        union {
            __u64 addr;
            __u64 splice_off_in;

            io_uring_sqe_socket_option_fields socket_option;
        };

        __u32 len;

        union {
            __kernel_rwf_t rw_flags;
            __u32          fsync_flags;
            __u16          poll_events;
            __u32          poll32_events;
            __u32          sync_range_flags;
            __u32          msg_flags;
            __u32          timeout_flags;
            __u32          accept_flags;
            __u32          cancel_flags;
            __u32          open_flags;
            __u32          statx_flags;
            __u32          fadvise_advice;
            __u32          splice_flags;
            __u32          rename_flags;
            __u32          unlink_flags;
            __u32          hardlink_flags;
            __u32          xattr_flags;
            __u32          msg_ring_flags;
            __u32          uring_cmd_flags;
            __u32          waitid_flags;
            __u32          futex_flags;
            __u32          install_fd_flags;
            __u32          nop_flags;
            __u32          pipe_flags;
        };

        __u64 user_data;

        union __attribute__((packed)) {
            __u16 buf_index;
            __u16 buf_group;
        };

        __u16 personality;

        union {
            __s32 splice_fd_in;
            __u32 file_index;
            __u32 zcrx_ifq_idx;
            __u32 optlen;

            io_uring_sqe_address_fields address;
        };

        union {
            io_uring_sqe_third_address_fields third_address;

            io_uring_sqe_attribute_fields attribute;

            __u64 optval;
            // __u8  cmd[0];
        };
    };

    struct io_uring_reg_wait {
        struct __kernel_timespec ts;
        __u32                    min_wait_usec;
        __u32                    flags;
        __u64                    sigmask;
        __u32                    sigmask_sz;
        __u32                    pad[3];
        __u64                    pad2[2];
    };

    struct io_uring_rsrc_update {
        __u32 offset;
        __u32 resv;
        __u64 data __attribute__((aligned(8)));
    };

    struct io_uring_rsrc_update2 {
        __u32 offset;
        __u32 resv;
        __u64 data __attribute__((aligned(8)));
        __u64 tags __attribute__((aligned(8)));
        __u32 nr;
        __u32 resv2;
    };

    struct io_uring_getevents_arg {
        __u64 sigmask;
        __u32 sigmask_sz;
        __u32 min_wait_usec;
        __u64 ts;
    };

    enum io_uring_sqe_flags_bit {
        IOSQE_FIXED_FILE_BIT,
        IOSQE_IO_DRAIN_BIT,
        IOSQE_IO_LINK_BIT,
        IOSQE_IO_HARDLINK_BIT,
        IOSQE_ASYNC_BIT,
        IOSQE_BUFFER_SELECT_BIT,
        IOSQE_CQE_SKIP_SUCCESS_BIT
    };

    enum io_uring_op {
        IORING_OP_NOP,
        IORING_OP_READV,
        IORING_OP_WRITEV,
        IORING_OP_FSYNC,
        IORING_OP_READ_FIXED,
        IORING_OP_WRITE_FIXED,
        IORING_OP_POLL_ADD,
        IORING_OP_POLL_REMOVE,
        IORING_OP_SYNC_FILE_RANGE,
        IORING_OP_SENDMSG,
        IORING_OP_RECVMSG,
        IORING_OP_TIMEOUT,
        IORING_OP_TIMEOUT_REMOVE,
        IORING_OP_ACCEPT,
        IORING_OP_ASYNC_CANCEL,
        IORING_OP_LINK_TIMEOUT,
        IORING_OP_CONNECT,
        IORING_OP_FALLOCATE,
        IORING_OP_OPENAT,
        IORING_OP_CLOSE,
        IORING_OP_FILES_UPDATE,
        IORING_OP_STATX,
        IORING_OP_READ,
        IORING_OP_WRITE,
        IORING_OP_FADVISE,
        IORING_OP_MADVISE,
        IORING_OP_SEND,
        IORING_OP_RECV,
        IORING_OP_OPENAT2,
        IORING_OP_EPOLL_CTL,
        IORING_OP_SPLICE,
        IORING_OP_PROVIDE_BUFFERS,
        IORING_OP_REMOVE_BUFFERS,
        IORING_OP_TEE,
        IORING_OP_SHUTDOWN,
        IORING_OP_RENAMEAT,
        IORING_OP_UNLINKAT,
        IORING_OP_MKDIRAT,
        IORING_OP_SYMLINKAT,
        IORING_OP_LINKAT,
        IORING_OP_MSG_RING,
        IORING_OP_FSETXATTR,
        IORING_OP_SETXATTR,
        IORING_OP_FGETXATTR,
        IORING_OP_GETXATTR,
        IORING_OP_SOCKET,
        IORING_OP_URING_CMD,
        IORING_OP_SEND_ZC,
        IORING_OP_SENDMSG_ZC,
        IORING_OP_READ_MULTISHOT,
        IORING_OP_WAITID,
        IORING_OP_FUTEX_WAIT,
        IORING_OP_FUTEX_WAKE,
        IORING_OP_FUTEX_WAITV,
        IORING_OP_FIXED_FD_INSTALL,
        IORING_OP_FTRUNCATE,
        IORING_OP_BIND,
        IORING_OP_LISTEN,
        IORING_OP_RECV_ZC,
        IORING_OP_EPOLL_WAIT,
        IORING_OP_READV_FIXED,
        IORING_OP_WRITEV_FIXED,
        IORING_OP_PIPE,
        IORING_OP_NOP128,
        IORING_OP_URING_CMD128,
        IORING_OP_LAST,
    };

    enum io_uring_register_op {
        IORING_REGISTER_BUFFERS          = 0,
        IORING_UNREGISTER_BUFFERS        = 1,
        IORING_REGISTER_FILES            = 2,
        IORING_UNREGISTER_FILES          = 3,
        IORING_REGISTER_EVENTFD          = 4,
        IORING_UNREGISTER_EVENTFD        = 5,
        IORING_REGISTER_FILES_UPDATE     = 6,
        IORING_REGISTER_EVENTFD_ASYNC    = 7,
        IORING_REGISTER_PROBE            = 8,
        IORING_REGISTER_PERSONALITY      = 9,
        IORING_UNREGISTER_PERSONALITY    = 10,
        IORING_REGISTER_RESTRICTIONS     = 11,
        IORING_REGISTER_ENABLE_RINGS     = 12,
        IORING_REGISTER_FILES2           = 13,
        IORING_REGISTER_FILES_UPDATE2    = 14,
        IORING_REGISTER_BUFFERS2         = 15,
        IORING_REGISTER_BUFFERS_UPDATE   = 16,
        IORING_REGISTER_IOWQ_AFF         = 17,
        IORING_UNREGISTER_IOWQ_AFF       = 18,
        IORING_REGISTER_IOWQ_MAX_WORKERS = 19,
        IORING_REGISTER_RING_FDS         = 20,
        IORING_UNREGISTER_RING_FDS       = 21,
        IORING_REGISTER_PBUF_RING        = 22,
        IORING_UNREGISTER_PBUF_RING      = 23,
        IORING_REGISTER_SYNC_CANCEL      = 24,
        IORING_REGISTER_FILE_ALLOC_RANGE = 25,
        IORING_REGISTER_PBUF_STATUS      = 26,
        IORING_REGISTER_NAPI             = 27,
        IORING_UNREGISTER_NAPI           = 28,
        IORING_REGISTER_CLOCK            = 29,
        IORING_REGISTER_CLONE_BUFFERS    = 30,
        IORING_REGISTER_SEND_MSG_RING    = 31,
        IORING_REGISTER_ZCRX_IFQ         = 32,
        IORING_REGISTER_RESIZE_RINGS     = 33,
        IORING_REGISTER_MEM_REGION       = 34,
        IORING_REGISTER_QUERY            = 35,
        IORING_REGISTER_ZCRX_CTRL        = 36,
        IORING_REGISTER_BPF_FILTER       = 37,
        IORING_REGISTER_LAST,
        IORING_REGISTER_USE_REGISTERED_RING = 1U << 31
    };

    enum io_uring_socket_op {
        SOCKET_URING_OP_SIOCINQ = 0,
        SOCKET_URING_OP_SIOCOUTQ,
        SOCKET_URING_OP_GETSOCKOPT,
        SOCKET_URING_OP_SETSOCKOPT,
        SOCKET_URING_OP_TX_TIMESTAMP,
        SOCKET_URING_OP_GETSOCKNAME,
    };

    enum io_uring_msg_ring_flags {
        IORING_MSG_DATA,
        IORING_MSG_SEND_FD
    };

    struct io_sqring_offsets {
        __u32 head;
        __u32 tail;
        __u32 ring_mask;
        __u32 ring_entries;
        __u32 flags;
        __u32 dropped;
        __u32 array;
        __u32 resv1;
        __u64 user_addr;
    };

    struct io_cqring_offsets {
        __u32 head;
        __u32 tail;
        __u32 ring_mask;
        __u32 ring_entries;
        __u32 overflow;
        __u32 cqes;
        __u32 flags;
        __u32 resv1;
        __u64 user_addr;
    };

    struct io_uring_params {
        __u32                    sq_entries     = 0;
        __u32                    cq_entries     = 0;
        __u32                    flags          = 0;
        __u32                    sq_thread_cpu  = 0;
        __u32                    sq_thread_idle = 0;
        __u32                    features       = 0;
        __u32                    wq_fd          = 0;
        __u32                    resv[3]{};
        struct io_sqring_offsets sq_off{};
        struct io_cqring_offsets cq_off{};
    };

    struct io_uring_probe_op {
        __u8  op;
        __u8  resv;
        __u16 flags;
        __u32 resv2;
    };

    struct io_uring_buf_reg {
        __u64 ring_addr    = 0;
        __u32 ring_entries = 0;
        __u16 bgid         = 0;
        __u16 flags        = 0;
        __u32 min_left     = 0;
        __u32 resv[5]{};
    };

    struct io_uring_buf {
        __u64 addr;
        __u32 len;
        __u16 bid;
        __u16 resv;
    };

    struct io_uring_buf_ring_tail_fields {
        __u64 resv1;
        __u32 resv2;
        __u16 resv3;
        __u16 tail;
    };

    struct io_uring_buf_ring {
        union {
            io_uring_buf_ring_tail_fields tail_data;
        };
    };

    struct io_uring_probe {
        __u8  last_op;
        __u8  ops_len;
        __u16 resv;
        __u32 resv2[3];
    };

    struct io_uring_cqe_iter {
        struct io_uring_cqe *cqes;
        uint32_t             mask;
        uint32_t             shift;
        uint32_t             head;
        uint32_t             tail;
    };

    static constexpr auto IORING_FILE_INDEX_ALLOC         = ~0U;
    static constexpr auto IOSQE_FIXED_FILE                = 1U << IOSQE_FIXED_FILE_BIT;
    static constexpr auto IOSQE_IO_DRAIN                  = 1U << IOSQE_IO_DRAIN_BIT;
    static constexpr auto IOSQE_IO_LINK                   = 1U << IOSQE_IO_LINK_BIT;
    static constexpr auto IOSQE_IO_HARDLINK               = 1U << IOSQE_IO_HARDLINK_BIT;
    static constexpr auto IOSQE_ASYNC                     = 1U << IOSQE_ASYNC_BIT;
    static constexpr auto IOSQE_BUFFER_SELECT             = 1U << IOSQE_BUFFER_SELECT_BIT;
    static constexpr auto IOSQE_CQE_SKIP_SUCCESS          = 1U << IOSQE_CQE_SKIP_SUCCESS_BIT;
    static constexpr auto IORING_SETUP_IOPOLL             = 1U << 0;
    static constexpr auto IORING_SETUP_SQPOLL             = 1U << 1;
    static constexpr auto IORING_SETUP_SQ_AFF             = 1U << 2;
    static constexpr auto IORING_SETUP_CQSIZE             = 1U << 3;
    static constexpr auto IORING_SETUP_CLAMP              = 1U << 4;
    static constexpr auto IORING_SETUP_ATTACH_WQ          = 1U << 5;
    static constexpr auto IORING_SETUP_R_DISABLED         = 1U << 6;
    static constexpr auto IORING_SETUP_SUBMIT_ALL         = 1U << 7;
    static constexpr auto IORING_SETUP_COOP_TASKRUN       = 1U << 8;
    static constexpr auto IORING_SETUP_TASKRUN_FLAG       = 1U << 9;
    static constexpr auto IORING_SETUP_SQE128             = 1U << 10;
    static constexpr auto IORING_SETUP_CQE32              = 1U << 11;
    static constexpr auto IORING_SETUP_SINGLE_ISSUER      = 1U << 12;
    static constexpr auto IORING_SETUP_DEFER_TASKRUN      = 1U << 13;
    static constexpr auto IORING_SETUP_NO_MMAP            = 1U << 14;
    static constexpr auto IORING_SETUP_REGISTERED_FD_ONLY = 1U << 15;
    static constexpr auto IORING_SETUP_NO_SQARRAY         = 1U << 16;
    static constexpr auto IORING_SETUP_HYBRID_IOPOLL      = 1U << 17;
    static constexpr auto IORING_SETUP_CQE_MIXED          = 1U << 18;
    static constexpr auto IORING_SETUP_SQE_MIXED          = 1U << 19;
    static constexpr auto IORING_SETUP_SQ_REWIND          = 1U << 20;
    static constexpr auto IORING_SQ_NEED_WAKEUP           = 1U << 0;
    static constexpr auto IORING_SQ_CQ_OVERFLOW           = 1U << 1;
    static constexpr auto IORING_SQ_TASKRUN               = 1U << 2;
    static constexpr auto IORING_ENTER_GETEVENTS          = 1U << 0;
    static constexpr auto IORING_ENTER_SQ_WAKEUP          = 1U << 1;
    static constexpr auto IORING_ENTER_SQ_WAIT            = 1U << 2;
    static constexpr auto IORING_ENTER_EXT_ARG            = 1U << 3;
    static constexpr auto IORING_ENTER_REGISTERED_RING    = 1U << 4;
    static constexpr auto IORING_ENTER_ABS_TIMER          = 1U << 5;
    static constexpr auto IORING_ENTER_EXT_ARG_REG        = 1U << 6;
    static constexpr auto IORING_ENTER_NO_IOWAIT          = 1U << 7;
    static constexpr auto IORING_OFF_SQ_RING              = 0ULL;
    static constexpr auto IORING_OFF_CQ_RING              = 0x800'0000ULL;
    static constexpr auto IORING_OFF_SQES                 = 0x1000'0000ULL;
    static constexpr auto IORING_OFF_PBUF_RING            = 0x8000'0000ULL;
    static constexpr auto IORING_OFF_PBUF_SHIFT           = 16;
    static constexpr auto IORING_OFF_MMAP_MASK            = 0xf800'0000ULL;
    static constexpr auto INT_FLAGS_MASK                  = IORING_ENTER_REGISTERED_RING | IORING_ENTER_NO_IOWAIT;
    static constexpr auto IORING_FEAT_SINGLE_MMAP         = 1U << 0;
    static constexpr auto IORING_FEAT_NODROP              = 1U << 1;
    static constexpr auto IORING_FEAT_SUBMIT_STABLE       = 1U << 2;
    static constexpr auto IORING_FEAT_RW_CUR_POS          = 1U << 3;
    static constexpr auto IORING_FEAT_CUR_PERSONALITY     = 1U << 4;
    static constexpr auto IORING_FEAT_FAST_POLL           = 1U << 5;
    static constexpr auto IORING_FEAT_POLL_32BITS         = 1U << 6;
    static constexpr auto IORING_FEAT_SQPOLL_NONFIXED     = 1U << 7;
    static constexpr auto IORING_FEAT_EXT_ARG             = 1U << 8;
    static constexpr auto IORING_FEAT_NATIVE_WORKERS      = 1U << 9;
    static constexpr auto IORING_FEAT_RSRC_TAGS           = 1U << 10;
    static constexpr auto IORING_FEAT_CQE_SKIP            = 1U << 11;
    static constexpr auto IORING_FEAT_LINKED_FILE         = 1U << 12;
    static constexpr auto IORING_FEAT_REG_REG_RING        = 1U << 13;
    static constexpr auto IORING_FEAT_RECVSEND_BUNDLE     = 1U << 14;
    static constexpr auto IORING_FEAT_MIN_TIMEOUT         = 1U << 15;
    static constexpr auto IORING_FEAT_RW_ATTR             = 1U << 16;
    static constexpr auto IORING_FEAT_NO_IOWAIT           = 1U << 17;
    static constexpr auto IORING_CQE_F_BUFFER             = 1U << 0;
    static constexpr auto IORING_CQE_F_MORE               = 1U << 1;
    static constexpr auto IORING_CQE_F_SOCK_NONEMPTY      = 1U << 2;
    static constexpr auto IORING_CQE_F_NOTIF              = 1U << 3;
    static constexpr auto IORING_CQE_F_BUF_MORE           = 1U << 4;
    static constexpr auto IORING_CQE_F_SKIP               = 1U << 5;
    static constexpr auto IORING_CQE_F_32                 = 1U << 15;
    static constexpr auto IORING_CQE_BUFFER_SHIFT         = 16;
    static constexpr auto LIBURING_UDATA_TIMEOUT          = static_cast<__u64>(-1);
    static constexpr auto KERN_MAX_ENTRIES                = 32'768;
    static constexpr auto KERN_MAX_CQ_ENTRIES             = 2 * KERN_MAX_ENTRIES;

    /* FIXME */
    static constexpr size_t huge_page_size = 2 * 1024 * 1024;

    static constexpr auto KRING_SIZE = 64;

    enum {
        INT_FLAG_REG_RING     = IORING_ENTER_REGISTERED_RING,
        INT_FLAG_NO_IOWAIT    = IORING_ENTER_NO_IOWAIT,
        INT_FLAG_REG_REG_RING = 1,
        INT_FLAG_APP_MEM      = 2,
        INT_FLAG_CQ_ENTER     = 4,
    };

    inline void
    io_uring_prep_rw(int op, struct io_uring_sqe *sqe, int fd, void const *addr, uint32_t len, __u64 offset) noexcept {
        sqe->opcode = static_cast<__u8>(op);
        sqe->fd     = fd;
        sqe->off    = offset;
        sqe->addr   = reinterpret_cast<uint64_t>(addr);
        sqe->len    = len;
    }

    inline void io_uring_prep_splice(
      struct io_uring_sqe *sqe,
      int                  fd_in,
      int64_t              off_in,
      int                  fd_out,
      int64_t              off_out,
      uint32_t             nbytes,
      uint32_t             splice_flags) noexcept {
        io_uring_prep_rw(IORING_OP_SPLICE, sqe, fd_out, nullptr, nbytes, static_cast<__u64>(off_out));
        sqe->splice_off_in = static_cast<__u64>(off_in);
        sqe->splice_fd_in  = fd_in;
        sqe->splice_flags  = splice_flags;
    }

    inline void io_uring_prep_tee(
      struct io_uring_sqe *sqe,
      int                  fd_in,
      int                  fd_out,
      uint32_t             nbytes,
      uint32_t             splice_flags) noexcept {
        io_uring_prep_rw(IORING_OP_TEE, sqe, fd_out, nullptr, nbytes, 0);
        sqe->splice_off_in = 0;
        sqe->splice_fd_in  = fd_in;
        sqe->splice_flags  = splice_flags;
    }

    inline void io_uring_prep_readv(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset) noexcept {
        io_uring_prep_rw(IORING_OP_READV, sqe, fd, iovecs, nr_vecs, offset);
    }

    inline void io_uring_prep_readv2(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset,
      int                  flags) noexcept {
        io_uring_prep_readv(sqe, fd, iovecs, nr_vecs, offset);
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
    }

    inline void io_uring_prep_read_fixed(
      struct io_uring_sqe *sqe,
      int                  fd,
      void                *buf,
      uint32_t             nbytes,
      __u64                offset,
      int                  buf_index) noexcept {
        io_uring_prep_rw(IORING_OP_READ_FIXED, sqe, fd, buf, nbytes, offset);
        sqe->buf_index = static_cast<__u16>(buf_index);
    }

    inline void io_uring_prep_readv_fixed(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset,
      int                  flags,
      int                  buf_index) noexcept {
        io_uring_prep_readv2(sqe, fd, iovecs, nr_vecs, offset, flags);
        sqe->opcode    = IORING_OP_READV_FIXED;
        sqe->buf_index = static_cast<__u16>(buf_index);
    }

    inline void io_uring_prep_writev(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset) noexcept {
        io_uring_prep_rw(IORING_OP_WRITEV, sqe, fd, iovecs, nr_vecs, offset);
    }

    inline void io_uring_prep_writev2(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset,
      int                  flags) noexcept {
        io_uring_prep_writev(sqe, fd, iovecs, nr_vecs, offset);
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
    }

    inline void io_uring_prep_write_fixed(
      struct io_uring_sqe *sqe,
      int                  fd,
      void const          *buf,
      uint32_t             nbytes,
      __u64                offset,
      int                  buf_index) noexcept {
        io_uring_prep_rw(IORING_OP_WRITE_FIXED, sqe, fd, buf, nbytes, offset);
        sqe->buf_index = static_cast<__u16>(buf_index);
    }

    inline void io_uring_prep_writev_fixed(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct iovec  *iovecs,
      uint32_t             nr_vecs,
      __u64                offset,
      int                  flags,
      int                  buf_index) noexcept {
        io_uring_prep_writev2(sqe, fd, iovecs, nr_vecs, offset, flags);
        sqe->opcode    = IORING_OP_WRITEV_FIXED;
        sqe->buf_index = static_cast<__u16>(buf_index);
    }

    inline void io_uring_prep_recvmsg(struct io_uring_sqe *sqe, int fd, struct msghdr *msg, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_RECVMSG, sqe, fd, msg, 1, 0);
        sqe->msg_flags = flags;
    }

    inline void
    io_uring_prep_recvmsg_multishot(struct io_uring_sqe *sqe, int fd, struct msghdr *msg, uint32_t flags) noexcept {
        io_uring_prep_recvmsg(sqe, fd, msg, flags);
        sqe->ioprio |= (1U << 1);
    }

    inline void
    io_uring_prep_sendmsg(struct io_uring_sqe *sqe, int fd, const struct msghdr *msg, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_SENDMSG, sqe, fd, msg, 1, 0);
        sqe->msg_flags = flags;
    }

    inline uint32_t __io_uring_prep_poll_mask(uint32_t poll_mask) noexcept {
        return poll_mask;
    }

    inline void io_uring_prep_poll_add(struct io_uring_sqe *sqe, int fd, uint32_t poll_mask) noexcept {
        io_uring_prep_rw(IORING_OP_POLL_ADD, sqe, fd, nullptr, 0, 0);
        sqe->poll32_events = __io_uring_prep_poll_mask(poll_mask);
    }

    inline void io_uring_prep_poll_multishot(struct io_uring_sqe *sqe, int fd, uint32_t poll_mask) noexcept {
        io_uring_prep_poll_add(sqe, fd, poll_mask);
        sqe->len = (1U << 0);
    }

    inline void io_uring_prep_poll_remove(struct io_uring_sqe *sqe, __u64 user_data) noexcept {
        io_uring_prep_rw(IORING_OP_POLL_REMOVE, sqe, -1, nullptr, 0, 0);
        sqe->addr = user_data;
    }

    inline void io_uring_prep_poll_update(
      struct io_uring_sqe *sqe,
      __u64                old_user_data,
      __u64                new_user_data,
      uint32_t             poll_mask,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(IORING_OP_POLL_REMOVE, sqe, -1, nullptr, flags, new_user_data);
        sqe->addr          = old_user_data;
        sqe->poll32_events = __io_uring_prep_poll_mask(poll_mask);
    }

    inline void io_uring_prep_fsync(struct io_uring_sqe *sqe, int fd, uint32_t fsync_flags) noexcept {
        io_uring_prep_rw(IORING_OP_FSYNC, sqe, fd, nullptr, 0, 0);
        sqe->fsync_flags = fsync_flags;
    }

    inline void io_uring_prep_nop(struct io_uring_sqe *sqe) noexcept {
        io_uring_prep_rw(IORING_OP_NOP, sqe, -1, nullptr, 0, 0);
    }

    inline void io_uring_prep_nop128(struct io_uring_sqe *sqe) noexcept {
        io_uring_prep_rw(IORING_OP_NOP128, sqe, -1, nullptr, 0, 0);
    }

    inline void io_uring_prep_timeout(
      struct io_uring_sqe            *sqe,
      const struct __kernel_timespec *ts,
      uint32_t                        count,
      uint32_t                        flags) noexcept {
        io_uring_prep_rw(IORING_OP_TIMEOUT, sqe, -1, ts, 1, count);
        sqe->timeout_flags = flags;
    }

    inline void io_uring_prep_timeout_remove(struct io_uring_sqe *sqe, __u64 user_data, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_TIMEOUT_REMOVE, sqe, -1, nullptr, 0, 0);
        sqe->addr          = user_data;
        sqe->timeout_flags = flags;
    }

    inline void io_uring_prep_timeout_update(
      struct io_uring_sqe            *sqe,
      const struct __kernel_timespec *ts,
      __u64                           user_data,
      uint32_t                        flags) noexcept {
        io_uring_prep_rw(IORING_OP_TIMEOUT_REMOVE, sqe, -1, nullptr, 0, reinterpret_cast<uintptr_t>(ts));
        sqe->addr          = user_data;
        sqe->timeout_flags = flags | (1U << 1);
    }

    inline __u64 uring_ptr_to_u64(void const *ptr) noexcept {
        return static_cast<__u64>(reinterpret_cast<uintptr_t>(ptr));
    }

    inline void io_uring_prep_accept(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct sockaddr     *addr,
      socklen_t           *addrlen,
      int                  flags) noexcept {
        io_uring_prep_rw(IORING_OP_ACCEPT, sqe, fd, addr, 0, uring_ptr_to_u64(addrlen));
        sqe->accept_flags = static_cast<__u32>(flags);
    }

    inline void internal__io_uring_set_target_fixed_file(struct io_uring_sqe *sqe, uint32_t file_index) noexcept {
        sqe->file_index = file_index + 1;
    }

    inline void io_uring_prep_accept_direct(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct sockaddr     *addr,
      socklen_t           *addrlen,
      int                  flags,
      uint32_t             file_index) noexcept {
        io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
        if (file_index == (~0U)) {
            file_index--;
        }
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    inline void io_uring_prep_multishot_accept(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct sockaddr     *addr,
      socklen_t           *addrlen,
      int                  flags) noexcept {
        io_uring_prep_accept(sqe, fd, addr, addrlen, flags);
        sqe->ioprio |= (1U << 0);
    }

    inline void io_uring_prep_multishot_accept_direct(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct sockaddr     *addr,
      socklen_t           *addrlen,
      int                  flags) noexcept {
        io_uring_prep_multishot_accept(sqe, fd, addr, addrlen, flags);
        internal__io_uring_set_target_fixed_file(sqe, (~0U) - 1);
    }

    inline void io_uring_prep_cancel64(struct io_uring_sqe *sqe, __u64 user_data, int flags) noexcept {
        io_uring_prep_rw(IORING_OP_ASYNC_CANCEL, sqe, -1, nullptr, 0, 0);
        sqe->addr         = user_data;
        sqe->cancel_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_cancel(struct io_uring_sqe *sqe, void const *user_data, int flags) noexcept {
        io_uring_prep_cancel64(sqe, static_cast<__u64>(reinterpret_cast<uintptr_t>(user_data)), flags);
    }

    inline void io_uring_prep_cancel_fd(struct io_uring_sqe *sqe, int fd, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_ASYNC_CANCEL, sqe, fd, nullptr, 0, 0);
        sqe->cancel_flags = static_cast<__u32>(flags) | (1U << 1);
    }

    inline void
    io_uring_prep_link_timeout(struct io_uring_sqe *sqe, const struct __kernel_timespec *ts, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_LINK_TIMEOUT, sqe, -1, ts, 1, 0);
        sqe->timeout_flags = flags;
    }

    inline void
    io_uring_prep_connect(struct io_uring_sqe *sqe, int fd, const struct sockaddr *addr, socklen_t addrlen) noexcept {
        io_uring_prep_rw(IORING_OP_CONNECT, sqe, fd, addr, 0, addrlen);
    }

    inline void
    io_uring_prep_bind(struct io_uring_sqe *sqe, int fd, const struct sockaddr *addr, socklen_t addrlen) noexcept {
        io_uring_prep_rw(IORING_OP_BIND, sqe, fd, addr, 0, addrlen);
    }

    inline void io_uring_prep_listen(struct io_uring_sqe *sqe, int fd, int backlog) noexcept {
        io_uring_prep_rw(IORING_OP_LISTEN, sqe, fd, 0, static_cast<uint32_t>(backlog), 0);
    }

    inline void io_uring_prep_epoll_wait(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct epoll_event  *events,
      int                  maxevents,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(IORING_OP_EPOLL_WAIT, sqe, fd, events, static_cast<uint32_t>(maxevents), 0);
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
    }

    inline void io_uring_prep_files_update(struct io_uring_sqe *sqe, int *fds, uint32_t nr_fds, int offset) noexcept {
        io_uring_prep_rw(IORING_OP_FILES_UPDATE, sqe, -1, fds, nr_fds, static_cast<__u64>(offset));
    }

    inline void io_uring_prep_fallocate(struct io_uring_sqe *sqe, int fd, int mode, __u64 offset, __u64 len) noexcept {
        io_uring_prep_rw(IORING_OP_FALLOCATE, sqe, fd, 0, static_cast<uint32_t>(mode), static_cast<__u64>(offset));
        sqe->addr = static_cast<__u64>(len);
    }

    inline void
    io_uring_prep_openat(struct io_uring_sqe *sqe, int dfd, char const *path, int flags, mode_t mode) noexcept {
        io_uring_prep_rw(IORING_OP_OPENAT, sqe, dfd, path, mode, 0);
        sqe->open_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_openat_direct(
      struct io_uring_sqe *sqe,
      int                  dfd,
      char const          *path,
      int                  flags,
      mode_t               mode,
      uint32_t             file_index) noexcept {
        io_uring_prep_openat(sqe, dfd, path, flags, mode);
        if (file_index == (~0U)) {
            file_index--;
        }
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    inline void io_uring_prep_open(struct io_uring_sqe *sqe, char const *path, int flags, mode_t mode) noexcept {
        io_uring_prep_openat(sqe, -100, path, flags, mode);
    }

    inline void io_uring_prep_open_direct(
      struct io_uring_sqe *sqe,
      char const          *path,
      int                  flags,
      mode_t               mode,
      uint32_t             file_index) noexcept {
        io_uring_prep_openat_direct(sqe, -100, path, flags, mode, file_index);
    }

    inline void io_uring_prep_close(struct io_uring_sqe *sqe, int fd) noexcept {
        io_uring_prep_rw(IORING_OP_CLOSE, sqe, fd, nullptr, 0, 0);
    }

    inline void io_uring_prep_close_direct(struct io_uring_sqe *sqe, uint32_t file_index) noexcept {
        io_uring_prep_close(sqe, 0);
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    inline void
    io_uring_prep_read(struct io_uring_sqe *sqe, int fd, void *buf, uint32_t nbytes, __u64 offset) noexcept {
        io_uring_prep_rw(IORING_OP_READ, sqe, fd, buf, nbytes, offset);
    }

    inline void io_uring_prep_read_multishot(
      struct io_uring_sqe *sqe,
      int                  fd,
      uint32_t             nbytes,
      __u64                offset,
      int                  buf_group) noexcept {
        io_uring_prep_rw(IORING_OP_READ_MULTISHOT, sqe, fd, nullptr, nbytes, offset);
        sqe->buf_group = static_cast<__u16>(buf_group);
        sqe->flags     = (1U << IOSQE_BUFFER_SELECT_BIT);
    }

    inline void
    io_uring_prep_write(struct io_uring_sqe *sqe, int fd, void const *buf, uint32_t nbytes, __u64 offset) noexcept {
        io_uring_prep_rw(IORING_OP_WRITE, sqe, fd, buf, nbytes, offset);
    }

    inline void io_uring_prep_statx(
      struct io_uring_sqe *sqe,
      int                  dfd,
      char const          *path,
      int                  flags,
      uint32_t             mask,
      struct statx        *statxbuf) noexcept {
        io_uring_prep_rw(IORING_OP_STATX, sqe, dfd, path, mask, uring_ptr_to_u64(statxbuf));
        sqe->statx_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_fadvise(struct io_uring_sqe *sqe, int fd, __u64 offset, __u32 len, int advice) noexcept {
        io_uring_prep_rw(IORING_OP_FADVISE, sqe, fd, nullptr, static_cast<__u32>(len), offset);
        sqe->fadvise_advice = static_cast<__u32>(advice);
    }

    inline void io_uring_prep_madvise(struct io_uring_sqe *sqe, void *addr, __u32 length, int advice) noexcept {
        io_uring_prep_rw(IORING_OP_MADVISE, sqe, -1, addr, static_cast<__u32>(length), 0);
        sqe->fadvise_advice = static_cast<__u32>(advice);
    }

    inline void
    io_uring_prep_fadvise64(struct io_uring_sqe *sqe, int fd, __u64 offset, off_t len, int advice) noexcept {
        io_uring_prep_rw(IORING_OP_FADVISE, sqe, fd, nullptr, 0, offset);
        sqe->addr           = static_cast<__u64>(len);
        sqe->fadvise_advice = static_cast<__u32>(advice);
    }

    inline void io_uring_prep_madvise64(struct io_uring_sqe *sqe, void *addr, off_t length, int advice) noexcept {
        io_uring_prep_rw(IORING_OP_MADVISE, sqe, -1, addr, 0, static_cast<__u64>(length));
        sqe->fadvise_advice = static_cast<__u32>(advice);
    }

    inline void
    io_uring_prep_send(struct io_uring_sqe *sqe, int sockfd, void const *buf, size_t len, int flags) noexcept {
        io_uring_prep_rw(IORING_OP_SEND, sqe, sockfd, buf, static_cast<__u32>(len), 0);
        sqe->msg_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_send_bundle(struct io_uring_sqe *sqe, int sockfd, size_t len, int flags) noexcept {
        io_uring_prep_send(sqe, sockfd, nullptr, len, flags);
        sqe->ioprio |= (1U << 4);
    }

    inline void
    io_uring_prep_send_set_addr(struct io_uring_sqe *sqe, const struct sockaddr *dest_addr, __u16 addr_len) noexcept {
        sqe->addr2            = reinterpret_cast<uint64_t>(dest_addr);
        sqe->address.addr_len = addr_len;
    }

    inline void io_uring_prep_sendto(
      struct io_uring_sqe   *sqe,
      int                    sockfd,
      void const            *buf,
      size_t                 len,
      int                    flags,
      const struct sockaddr *addr,
      socklen_t              addrlen) noexcept {
        io_uring_prep_send(sqe, sockfd, buf, len, flags);
        io_uring_prep_send_set_addr(sqe, addr, static_cast<__u16>(addrlen));
    }

    inline void io_uring_prep_send_zc(
      struct io_uring_sqe *sqe,
      int                  sockfd,
      void const          *buf,
      size_t               len,
      int                  flags,
      uint32_t             zc_flags) noexcept {
        io_uring_prep_rw(IORING_OP_SEND_ZC, sqe, sockfd, buf, static_cast<__u32>(len), 0);
        sqe->msg_flags = static_cast<__u32>(flags);
        sqe->ioprio    = static_cast<__u16>(zc_flags);
    }

    inline void io_uring_prep_send_zc_fixed(
      struct io_uring_sqe *sqe,
      int                  sockfd,
      void const          *buf,
      size_t               len,
      int                  flags,
      uint32_t             zc_flags,
      uint32_t             buf_index) noexcept {
        io_uring_prep_send_zc(sqe, sockfd, buf, len, flags, zc_flags);
        sqe->ioprio    |= (1U << 2);
        sqe->buf_index  = static_cast<__u16>(buf_index);
    }

    inline void
    io_uring_prep_sendmsg_zc(struct io_uring_sqe *sqe, int fd, const struct msghdr *msg, uint32_t flags) noexcept {
        io_uring_prep_sendmsg(sqe, fd, msg, flags);
        sqe->opcode = IORING_OP_SENDMSG_ZC;
    }

    inline void io_uring_prep_sendmsg_zc_fixed(
      struct io_uring_sqe *sqe,
      int                  fd,
      const struct msghdr *msg,
      uint32_t             flags,
      uint32_t             buf_index) noexcept {
        io_uring_prep_sendmsg_zc(sqe, fd, msg, flags);
        sqe->ioprio    |= (1U << 2);
        sqe->buf_index  = static_cast<__u16>(buf_index);
    }

    inline void io_uring_prep_recv(struct io_uring_sqe *sqe, int sockfd, void *buf, size_t len, int flags) noexcept {
        io_uring_prep_rw(IORING_OP_RECV, sqe, sockfd, buf, static_cast<__u32>(len), 0);
        sqe->msg_flags = static_cast<__u32>(flags);
    }

    inline void
    io_uring_prep_recv_multishot(struct io_uring_sqe *sqe, int sockfd, void *buf, size_t len, int flags) noexcept {
        io_uring_prep_recv(sqe, sockfd, buf, len, flags);
        sqe->ioprio |= (1U << 1);
    }

    inline void
    io_uring_prep_openat2(struct io_uring_sqe *sqe, int dfd, char const *path, const struct open_how *how) noexcept {
        io_uring_prep_rw(
          IORING_OP_OPENAT2,
          sqe,
          dfd,
          path,
          sizeof(*how),
          static_cast<uint64_t>(reinterpret_cast<uintptr_t>(how)));
    }

    inline void io_uring_prep_openat2_direct(
      struct io_uring_sqe   *sqe,
      int                    dfd,
      char const            *path,
      const struct open_how *how,
      uint32_t               file_index) noexcept {
        io_uring_prep_openat2(sqe, dfd, path, how);
        if (file_index == (~0U)) {
            file_index--;
        }
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    inline void
    io_uring_prep_epoll_ctl(struct io_uring_sqe *sqe, int epfd, int fd, int op, const struct epoll_event *ev) noexcept {
        io_uring_prep_rw(IORING_OP_EPOLL_CTL, sqe, epfd, ev, static_cast<__u32>(op), static_cast<__u32>(fd));
    }

    inline void
    io_uring_prep_provide_buffers(struct io_uring_sqe *sqe, void *addr, int len, int nr, int bgid, int bid) noexcept {
        io_uring_prep_rw(IORING_OP_PROVIDE_BUFFERS, sqe, nr, addr, static_cast<__u32>(len), static_cast<__u64>(bid));
        sqe->buf_group = static_cast<__u16>(bgid);
    }

    inline void io_uring_prep_remove_buffers(struct io_uring_sqe *sqe, int nr, int bgid) noexcept {
        io_uring_prep_rw(IORING_OP_REMOVE_BUFFERS, sqe, nr, nullptr, 0, 0);
        sqe->buf_group = static_cast<__u16>(bgid);
    }

    inline void io_uring_prep_shutdown(struct io_uring_sqe *sqe, int fd, int how) noexcept {
        io_uring_prep_rw(IORING_OP_SHUTDOWN, sqe, fd, nullptr, static_cast<__u32>(how), 0);
    }

    inline void io_uring_prep_unlinkat(struct io_uring_sqe *sqe, int dfd, char const *path, int flags) noexcept {
        io_uring_prep_rw(IORING_OP_UNLINKAT, sqe, dfd, path, 0, 0);
        sqe->unlink_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_unlink(struct io_uring_sqe *sqe, char const *path, int flags) noexcept {
        io_uring_prep_unlinkat(sqe, -100, path, flags);
    }

    inline void io_uring_prep_renameat(
      struct io_uring_sqe *sqe,
      int                  olddfd,
      char const          *oldpath,
      int                  newdfd,
      char const          *newpath,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_RENAMEAT,
          sqe,
          olddfd,
          oldpath,
          static_cast<__u32>(newdfd),
          static_cast<uint64_t>(reinterpret_cast<uintptr_t>(newpath)));
        sqe->rename_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_rename(struct io_uring_sqe *sqe, char const *oldpath, char const *newpath) noexcept {
        io_uring_prep_renameat(sqe, -100, oldpath, -100, newpath, 0);
    }

    inline void
    io_uring_prep_sync_file_range(struct io_uring_sqe *sqe, int fd, uint32_t len, __u64 offset, int flags) noexcept {
        io_uring_prep_rw(IORING_OP_SYNC_FILE_RANGE, sqe, fd, nullptr, len, offset);
        sqe->sync_range_flags = static_cast<__u32>(flags);
    }

    inline void io_uring_prep_mkdirat(struct io_uring_sqe *sqe, int dfd, char const *path, mode_t mode) noexcept {
        io_uring_prep_rw(IORING_OP_MKDIRAT, sqe, dfd, path, mode, 0);
    }

    inline void io_uring_prep_mkdir(struct io_uring_sqe *sqe, char const *path, mode_t mode) noexcept {
        io_uring_prep_mkdirat(sqe, -100, path, mode);
    }

    inline void
    io_uring_prep_symlinkat(struct io_uring_sqe *sqe, char const *target, int newdirfd, char const *linkpath) noexcept {
        io_uring_prep_rw(
          IORING_OP_SYMLINKAT,
          sqe,
          newdirfd,
          target,
          0,
          static_cast<uint64_t>(reinterpret_cast<uintptr_t>(linkpath)));
    }

    inline void io_uring_prep_symlink(struct io_uring_sqe *sqe, char const *target, char const *linkpath) noexcept {
        io_uring_prep_symlinkat(sqe, target, -100, linkpath);
    }

    inline void io_uring_prep_linkat(
      struct io_uring_sqe *sqe,
      int                  olddfd,
      char const          *oldpath,
      int                  newdfd,
      char const          *newpath,
      int                  flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_LINKAT,
          sqe,
          olddfd,
          oldpath,
          static_cast<__u32>(newdfd),
          static_cast<uint64_t>(reinterpret_cast<uintptr_t>(newpath)));
        sqe->hardlink_flags = static_cast<__u32>(flags);
    }

    inline void
    io_uring_prep_link(struct io_uring_sqe *sqe, char const *oldpath, char const *newpath, int flags) noexcept {
        io_uring_prep_linkat(sqe, -100, oldpath, -100, newpath, flags);
    }

    inline void io_uring_prep_msg_ring_cqe_flags(
      struct io_uring_sqe *sqe,
      int                  fd,
      uint32_t             len,
      __u64                data,
      uint32_t             flags,
      uint32_t             cqe_flags) noexcept {
        io_uring_prep_rw(IORING_OP_MSG_RING, sqe, fd, nullptr, len, data);
        sqe->msg_ring_flags = (1U << 1) | flags;
        sqe->file_index     = cqe_flags;
    }

    inline void
    io_uring_prep_msg_ring(struct io_uring_sqe *sqe, int fd, uint32_t len, __u64 data, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_MSG_RING, sqe, fd, nullptr, len, data);
        sqe->msg_ring_flags = flags;
    }

    inline void io_uring_prep_msg_ring_fd(
      struct io_uring_sqe *sqe,
      int                  fd,
      int                  source_fd,
      int                  target_fd,
      __u64                data,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_MSG_RING,
          sqe,
          fd,
          reinterpret_cast<void *>(static_cast<uintptr_t>(IORING_MSG_SEND_FD)),
          0,
          data);
        sqe->third_address.addr3 = static_cast<__u64>(source_fd);
        if (static_cast<uint32_t>(target_fd) == (~0U)) {
            target_fd--;
        }
        internal__io_uring_set_target_fixed_file(sqe, static_cast<uint32_t>(target_fd));
        sqe->msg_ring_flags = flags;
    }

    inline void io_uring_prep_msg_ring_fd_alloc(
      struct io_uring_sqe *sqe,
      int                  fd,
      int                  source_fd,
      __u64                data,
      uint32_t             flags) noexcept {
        io_uring_prep_msg_ring_fd(sqe, fd, source_fd, static_cast<int>(~0U), data, flags);
    }

    inline void io_uring_prep_getxattr(
      struct io_uring_sqe *sqe,
      char const          *name,
      char                *value,
      char const          *path,
      uint32_t             len) noexcept {
        io_uring_prep_rw(IORING_OP_GETXATTR, sqe, 0, name, len, static_cast<__u64>(reinterpret_cast<uintptr_t>(value)));
        sqe->third_address.addr3 = static_cast<__u64>(reinterpret_cast<uintptr_t>(path));
        sqe->xattr_flags         = 0;
    }

    inline void io_uring_prep_setxattr(
      struct io_uring_sqe *sqe,
      char const          *name,
      char const          *value,
      char const          *path,
      int                  flags,
      uint32_t             len) noexcept {
        io_uring_prep_rw(IORING_OP_SETXATTR, sqe, 0, name, len, static_cast<__u64>(reinterpret_cast<uintptr_t>(value)));
        sqe->third_address.addr3 = static_cast<__u64>(reinterpret_cast<uintptr_t>(path));
        sqe->xattr_flags         = static_cast<__u32>(flags);
    }

    inline void
    io_uring_prep_fgetxattr(struct io_uring_sqe *sqe, int fd, char const *name, char *value, uint32_t len) noexcept {
        io_uring_prep_rw(
          IORING_OP_FGETXATTR,
          sqe,
          fd,
          name,
          len,
          static_cast<__u64>(reinterpret_cast<uintptr_t>(value)));
        sqe->xattr_flags = 0;
    }

    inline void io_uring_prep_fsetxattr(
      struct io_uring_sqe *sqe,
      int                  fd,
      char const          *name,
      char const          *value,
      int                  flags,
      uint32_t             len) noexcept {
        io_uring_prep_rw(
          IORING_OP_FSETXATTR,
          sqe,
          fd,
          name,
          len,
          static_cast<__u64>(reinterpret_cast<uintptr_t>(value)));
        sqe->xattr_flags = static_cast<__u32>(flags);
    }

    inline void
    io_uring_prep_socket(struct io_uring_sqe *sqe, int domain, int type, int protocol, uint32_t flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_SOCKET,
          sqe,
          domain,
          nullptr,
          static_cast<uint32_t>(protocol),
          static_cast<__u64>(type));
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
    }

    inline void io_uring_prep_socket_direct(
      struct io_uring_sqe *sqe,
      int                  domain,
      int                  type,
      int                  protocol,
      uint32_t             file_index,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_SOCKET,
          sqe,
          domain,
          nullptr,
          static_cast<uint32_t>(protocol),
          static_cast<__u64>(type));
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
        if (file_index == (~0U)) {
            file_index--;
        }
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    inline void io_uring_prep_socket_direct_alloc(
      struct io_uring_sqe *sqe,
      int                  domain,
      int                  type,
      int                  protocol,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(
          IORING_OP_SOCKET,
          sqe,
          domain,
          nullptr,
          static_cast<uint32_t>(protocol),
          static_cast<__u64>(type));
        sqe->rw_flags = static_cast<__kernel_rwf_t>(flags);
        internal__io_uring_set_target_fixed_file(sqe, (~0U) - 1);
    }

    inline void __io_uring_prep_uring_cmd(struct io_uring_sqe *sqe, int op, __u32 cmd_op, int fd) noexcept {
        sqe->opcode         = static_cast<__u8>(op);
        sqe->fd             = fd;
        sqe->command.cmd_op = cmd_op;
        sqe->command.__pad1 = 0;
        sqe->addr           = 0UL;
        sqe->len            = 0;
    }

    inline void io_uring_prep_uring_cmd(struct io_uring_sqe *sqe, int cmd_op, int fd) noexcept {
        __io_uring_prep_uring_cmd(sqe, IORING_OP_URING_CMD, static_cast<__u32>(cmd_op), fd);
    }

    inline void io_uring_prep_uring_cmd128(struct io_uring_sqe *sqe, int cmd_op, int fd) noexcept {
        __io_uring_prep_uring_cmd(sqe, IORING_OP_URING_CMD128, static_cast<__u32>(cmd_op), fd);
    }

    inline void io_uring_prep_cmd_sock(
      struct io_uring_sqe *sqe,
      int                  cmd_op,
      int                  fd,
      int                  level,
      int                  optname,
      void                *optval,
      int                  optlen) noexcept {
        io_uring_prep_uring_cmd(sqe, cmd_op, fd);
        sqe->optval                = reinterpret_cast<uint64_t>(optval);
        sqe->socket_option.optname = static_cast<__u32>(optname);
        sqe->optlen                = static_cast<__u32>(optlen);
        sqe->socket_option.level   = static_cast<__u32>(level);
    }

    inline void io_uring_prep_cmd_getsockname(
      struct io_uring_sqe *sqe,
      int                  fd,
      struct sockaddr     *sockaddr,
      socklen_t           *sockaddr_len,
      int                  peer) noexcept {
        io_uring_prep_uring_cmd(sqe, SOCKET_URING_OP_GETSOCKNAME, fd);
        sqe->addr                = reinterpret_cast<uintptr_t>(sockaddr);
        sqe->third_address.addr3 = reinterpret_cast<uint64_t>(sockaddr_len);
        sqe->optlen              = static_cast<__u32>(peer);
    }

    inline void io_uring_prep_waitid(
      struct io_uring_sqe *sqe,
      idtype_t             idtype,
      id_t                 id,
      siginfo_t           *infop,
      int                  options,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(IORING_OP_WAITID, sqe, static_cast<int>(id), nullptr, static_cast<uint32_t>(idtype), 0);
        sqe->waitid_flags = flags;
        sqe->file_index   = static_cast<__u32>(options);
        sqe->addr2        = reinterpret_cast<uint64_t>(infop);
    }

    inline void io_uring_prep_futex_wake(
      struct io_uring_sqe *sqe,
      uint32_t const      *futex,
      uint64_t             val,
      uint64_t             mask,
      uint32_t             futex_flags,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(IORING_OP_FUTEX_WAKE, sqe, static_cast<int>(futex_flags), futex, 0, val);
        sqe->futex_flags         = flags;
        sqe->third_address.addr3 = mask;
    }

    inline void io_uring_prep_futex_wait(
      struct io_uring_sqe *sqe,
      uint32_t const      *futex,
      uint64_t             val,
      uint64_t             mask,
      uint32_t             futex_flags,
      uint32_t             flags) noexcept {
        io_uring_prep_rw(IORING_OP_FUTEX_WAIT, sqe, static_cast<int>(futex_flags), futex, 0, val);
        sqe->futex_flags         = flags;
        sqe->third_address.addr3 = mask;
    }

    inline void io_uring_prep_futex_waitv(
      struct io_uring_sqe      *sqe,
      const struct futex_waitv *futex,
      uint32_t                  nr_futex,
      uint32_t                  flags) noexcept {
        io_uring_prep_rw(IORING_OP_FUTEX_WAITV, sqe, 0, futex, nr_futex, 0);
        sqe->futex_flags = flags;
    }

    inline void io_uring_prep_fixed_fd_install(struct io_uring_sqe *sqe, int fd, uint32_t flags) noexcept {
        io_uring_prep_rw(IORING_OP_FIXED_FD_INSTALL, sqe, fd, nullptr, 0, 0);
        sqe->flags            = (1U << IOSQE_FIXED_FILE_BIT);
        sqe->install_fd_flags = flags;
    }

    inline void io_uring_prep_ftruncate(struct io_uring_sqe *sqe, int fd, loff_t len) noexcept {
        io_uring_prep_rw(IORING_OP_FTRUNCATE, sqe, fd, 0, 0, static_cast<__u64>(len));
    }

    inline void io_uring_prep_cmd_discard(struct io_uring_sqe *sqe, int fd, uint64_t offset, uint64_t nbytes) noexcept {
        io_uring_prep_uring_cmd(
          sqe,
          (((0U) << (((0 + 8) + 8) + 14)) | (((18)) << (0 + 8)) | (((0)) << 0) | ((0) << ((0 + 8) + 8))),
          fd);
        sqe->addr                = offset;
        sqe->third_address.addr3 = nbytes;
    }

    inline void io_uring_prep_pipe(struct io_uring_sqe *sqe, int *fds, int pipe_flags) {
        io_uring_prep_rw(IORING_OP_PIPE, sqe, 0, fds, 0, 0);
        sqe->pipe_flags = static_cast<__u32>(pipe_flags);
    }

    inline void io_uring_prep_pipe_direct(struct io_uring_sqe *sqe, int *fds, int pipe_flags, uint32_t file_index) {
        io_uring_prep_pipe(sqe, fds, pipe_flags);
        if (file_index == (~0U)) {
            file_index--;
        }
        internal__io_uring_set_target_fixed_file(sqe, file_index);
    }

    template <typename T>
    inline void IO_URING_WRITE_ONCE(T &var, T val) noexcept {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<T> *>(&var), val, std::memory_order_relaxed);
    }

    template <typename T>
    inline T IO_URING_READ_ONCE(T const &var) noexcept {
        return std::atomic_load_explicit(reinterpret_cast<std::atomic<T> const *>(&var), std::memory_order_relaxed);
    }

    template <typename T>
    inline void io_uring_smp_store_release(T *p, T v) noexcept {
        std::atomic_store_explicit(reinterpret_cast<std::atomic<T> *>(p), v, std::memory_order_release);
    }

    template <typename T>
    inline T io_uring_smp_load_acquire(T const *p) noexcept {
        return std::atomic_load_explicit(reinterpret_cast<std::atomic<T> const *>(p), std::memory_order_acquire);
    }

    inline void io_uring_smp_mb() noexcept {
        std::atomic_thread_fence(std::memory_order_seq_cst);
    }

    static unsigned internal__io_uring_flush_sq(struct io_uring *ring) {
        struct io_uring_sq *sq   = &ring->sq;
        unsigned            tail = sq->sqe_tail;

        if (ring->flags & IORING_SETUP_SQ_REWIND) {
            sq->sqe_tail = 0;
            return tail;
        }

        if (sq->sqe_head != tail) {
            sq->sqe_head = tail;
            if (!(ring->flags & IORING_SETUP_SQPOLL)) {
                *sq->ktail = tail;
            } else {
                io_uring_smp_store_release(sq->ktail, tail);
            }
        }
        return tail - IO_URING_READ_ONCE(*sq->khead);
    }

    struct get_data {
        unsigned submit;
        unsigned wait_nr;
        unsigned get_flags;
        int      sz;
        int      has_ts;
        void    *arg;
    };

    static inline bool sq_ring_needs_enter(struct io_uring *ring, unsigned submit, unsigned *flags) {
        if (!submit) {
            return false;
        }

        if (!(ring->flags & IORING_SETUP_SQPOLL)) {
            return true;
        }

        /*
         * Ensure the kernel can see the store to the SQ tail before we read
         * the flags.
         */
        io_uring_smp_mb();

        if (IO_URING_READ_ONCE(*ring->sq.kflags) & IORING_SQ_NEED_WAKEUP) [[unlikely]] {
            *flags |= IORING_ENTER_SQ_WAKEUP;
            return true;
        }

        return false;
    }

    static inline int ring_enter_flags(struct io_uring *ring) {
        return ring->int_flags & INT_FLAGS_MASK;
    }

    static inline bool cq_ring_needs_flush(struct io_uring *ring) {
        return IO_URING_READ_ONCE(*ring->sq.kflags) & (IORING_SQ_CQ_OVERFLOW | IORING_SQ_TASKRUN);
    }

    static inline bool cq_ring_needs_enter(struct io_uring *ring) {
        return (ring->int_flags & INT_FLAG_CQ_ENTER) || cq_ring_needs_flush(ring);
    }

    static inline unsigned io_uring_cqe_nr(const struct io_uring_cqe *cqe) noexcept {
        uint32_t const shift = !!(cqe->flags & IORING_CQE_F_32);

        return 1U << shift;
    }

    inline void io_uring_cq_advance(struct io_uring *ring, uint32_t nr) noexcept {
        if (nr) {
            struct io_uring_cq *cq = &ring->cq;
            io_uring_smp_store_release(cq->khead, *cq->khead + nr);
        }
    }

    inline bool io_uring_skip_cqe(struct io_uring *ring, struct io_uring_cqe *cqe, int *err) {
        if (!(cqe->flags & IORING_CQE_F_SKIP)) {
            if (ring->features & IORING_FEAT_EXT_ARG) {
                return false;
            }
            if (cqe->user_data != LIBURING_UDATA_TIMEOUT) {
                return false;
            }
            if (cqe->res < 0) {
                *err = cqe->res;
            }
        }
        io_uring_cq_advance(ring, io_uring_cqe_nr(cqe));
        return !*err;
    }

    inline uint32_t io_uring_cqe_shift_from_flags(uint32_t flags) noexcept {
        return !!(flags & (1U << 11));
    }

    inline uint32_t io_uring_cqe_shift(const struct io_uring *ring) noexcept {
        return io_uring_cqe_shift_from_flags(ring->flags);
    }

    inline int
    internal__io_uring_peek_cqe(struct io_uring *ring, struct io_uring_cqe **cqe_ptr, unsigned *nr_available) noexcept {
        struct io_uring_cqe *cqe;
        int                  err = 0;
        unsigned             available;
        unsigned             mask  = ring->cq.ring_mask;
        unsigned             shift = io_uring_cqe_shift(ring);

        for (;;) {
            unsigned tail = io_uring_smp_load_acquire(ring->cq.ktail);
            unsigned head = io_uring_smp_load_acquire(ring->cq.khead);

            cqe       = nullptr;
            available = tail - head;
            if (!available) {
                break;
            }

            cqe = &ring->cq.cqes[(head & mask) << shift];
            if (!io_uring_skip_cqe(ring, cqe, &err)) {
                break;
            }
            cqe = nullptr;
        }

        *cqe_ptr = cqe;
        if (nr_available) {
            *nr_available = available;
        }
        return err;
    }

    static inline int internal__sys_io_uring_enter2(
      uint32_t fd,
      uint32_t to_submit,
      uint32_t min_complete,
      uint32_t flags,
      void    *arg,
      size_t   sz) {
        int ret = static_cast<int>(syscall(__NR_io_uring_enter, fd, to_submit, min_complete, flags, arg, sz));
        return (ret < 0) ? -errno : ret;
    }

    static inline int _io_uring_get_cqe(struct io_uring *ring, struct io_uring_cqe **cqe_ptr, struct get_data *data) {
        struct io_uring_cqe *cqe    = nullptr;
        bool                 looped = false;
        int                  err    = 0;

        for (;;) {
            bool     need_enter = false;
            unsigned flags      = static_cast<unsigned>(ring_enter_flags(ring));
            unsigned nr_available;

            auto ret = internal__io_uring_peek_cqe(ring, &cqe, &nr_available);
            if (ret != 0) [[unlikely]] {
                if (err == 0) {
                    err = ret;
                }
                break;
            }
            if (!cqe && !data->wait_nr && !data->submit) {
                if (looped || !cq_ring_needs_enter(ring)) {
                    if (err == 0) [[unlikely]] {
                        err = -EAGAIN;
                    }
                    break;
                }
                need_enter = true;
            }
            if (data->wait_nr > nr_available || need_enter) {
                flags      |= IORING_ENTER_GETEVENTS | data->get_flags;
                need_enter  = true;
            }
            if (sq_ring_needs_enter(ring, data->submit, &flags)) {
                need_enter = true;
            }
            if (!need_enter) {
                break;
            }
            if (looped && data->has_ts) {
                auto *const arg = static_cast<io_uring_getevents_arg *>(data->arg);

                if (cqe == nullptr && arg->ts && !err) {
                    err = -ETIME;
                }
                break;
            }

            ret = internal__sys_io_uring_enter2(
              static_cast<uint32_t>(ring->enter_ring_fd),
              data->submit,
              data->wait_nr,
              flags,
              data->arg,
              static_cast<size_t>(data->sz));
            if (ret < 0) [[unlikely]] {
                if (err == 0) {
                    err = ret;
                }
                break;
            }

            data->submit -= static_cast<unsigned>(ret);
            if (cqe) {
                break;
            }
            if (!looped) {
                looped = true;
                err    = ret;
            }
        }

        *cqe_ptr = cqe;
        return err;
    }

    static inline int internal__io_uring_get_cqe(
      struct io_uring      *ring,
      struct io_uring_cqe **cqe_ptr,
      unsigned              submit,
      unsigned              wait_nr,
      sigset_t             *sigmask) {
        struct get_data data = {
          .submit    = submit,
          .wait_nr   = wait_nr,
          .get_flags = 0,
          .sz        = _NSIG / 8,
          .has_ts    = 0,
          .arg       = sigmask,
        };

        return _io_uring_get_cqe(ring, cqe_ptr, &data);
    }

    inline uint32_t io_uring_sqe_shift_from_flags(uint32_t flags) noexcept {
        return !!(flags & (1U << 10));
    }

    inline uint32_t io_uring_sqe_shift(const struct io_uring *ring) noexcept {
        return io_uring_sqe_shift_from_flags(ring->flags);
    }

    int io_uring_submit_and_wait_reg(
      struct io_uring      *ring,
      struct io_uring_cqe **cqe_ptr,
      uint32_t              wait_nr,
      int                   reg_index) noexcept {
        uint64_t        offset = static_cast<uint64_t>(reg_index) * sizeof(struct io_uring_reg_wait);
        struct get_data data   = {
            .submit    = internal__io_uring_flush_sq(ring),
            .wait_nr   = wait_nr,
            .get_flags = (1U << 3) | (1U << 6),
            .sz        = sizeof(struct io_uring_reg_wait),
            .has_ts    = true,
            .arg       = reinterpret_cast<void *>(static_cast<uintptr_t>(offset))};
        if (!(ring->features & (1U << 8))) {
            return -22;
        }
        return _io_uring_get_cqe(ring, cqe_ptr, &data);
    }

    inline void io_uring_initialize_sqe(io_uring_sqe *sqe) noexcept {
        sqe->flags                   = 0;
        sqe->ioprio                  = 0;
        sqe->rw_flags                = 0;
        sqe->buf_index               = 0;
        sqe->personality             = 0;
        sqe->file_index              = 0;
        sqe->third_address.addr3     = 0;
        sqe->third_address.__pad2[0] = 0;
    }

    inline uint32_t io_uring_load_sq_head(const struct io_uring *ring) noexcept {
        if (ring->flags & (1U << 1)) {
            return io_uring_smp_load_acquire(ring->sq.khead);
        }
        return *ring->sq.khead;
    }

    inline struct io_uring_sqe *_io_uring_get_sqe(struct io_uring *ring) noexcept {
        io_uring_sq *sq   = &ring->sq;
        uint32_t     head = io_uring_load_sq_head(ring);
        uint32_t     tail = sq->sqe_tail;
        if (tail - head >= sq->ring_entries) {
            return nullptr;
        }
        io_uring_sqe *sqe = &sq->sqes[(tail & sq->ring_mask) << io_uring_sqe_shift(ring)];
        sq->sqe_tail      = tail + 1;
        io_uring_initialize_sqe(sqe);
        return sqe;
    }

    inline struct io_uring_sqe *io_uring_get_sqe(struct io_uring *ring) noexcept {
        return _io_uring_get_sqe(ring);
    }

    static inline int internal__sys_io_uring_enter(
      uint32_t  fd,
      uint32_t  to_submit,
      uint32_t  min_complete,
      uint32_t  flags,
      sigset_t *sig) {
        return internal__sys_io_uring_enter2(fd, to_submit, min_complete, flags, sig, _NSIG / 8);
    }

    static inline int
    internal__io_uring_submit(struct io_uring *ring, unsigned submitted, unsigned wait_nr, bool getevents) {
        bool cq_needs_enter = getevents || wait_nr || cq_ring_needs_enter(ring);
        auto flags          = static_cast<unsigned>(ring_enter_flags(ring));

        // liburing_sanitize_ring(ring);

        if (sq_ring_needs_enter(ring, submitted, &flags) || cq_needs_enter) {
            if (cq_needs_enter) {
                flags |= IORING_ENTER_GETEVENTS;
            }

            return internal__sys_io_uring_enter(
              static_cast<uint32_t>(ring->enter_ring_fd),
              submitted,
              wait_nr,
              flags,
              nullptr);
        }
        return static_cast<int>(submitted);
    }

    static inline int internal__io_uring_submit_and_wait(struct io_uring *ring, unsigned wait_nr) {
        return internal__io_uring_submit(ring, internal__io_uring_flush_sq(ring), wait_nr, false);
    }

    static inline int io_uring_submit(io_uring *ring) {
        return internal__io_uring_submit_and_wait(ring, 0);
    }

    static inline int internal__io_uring_submit_timeout(io_uring *ring, unsigned wait_nr, __kernel_timespec *ts) {
        auto *sqe = io_uring_get_sqe(ring);
        if (!sqe) {
            auto const ret = io_uring_submit(ring);
            if (ret < 0) {
                return ret;
            }
            sqe = io_uring_get_sqe(ring);
            if (!sqe) {
                return -EAGAIN;
            }
        }
        io_uring_prep_timeout(sqe, ts, wait_nr, 0);
        sqe->user_data = LIBURING_UDATA_TIMEOUT;
        return static_cast<int>(internal__io_uring_flush_sq(ring));
    }

    static inline int internal__io_uring_submit_and_wait_timeout(
      io_uring          *ring,
      io_uring_cqe     **cqe_ptr,
      uint32_t           wait_nr,
      __kernel_timespec *ts,
      uint32_t           min_wait,
      sigset_t          *sigmask) {
        unsigned to_submit = 0;
        if (ts != nullptr) {
            if (ring->features & (1U << 8)) {
                struct io_uring_getevents_arg arg = {
                  .sigmask       = reinterpret_cast<uint64_t>(sigmask),
                  .sigmask_sz    = (64 + 1) / 8,
                  .min_wait_usec = min_wait,
                  .ts            = reinterpret_cast<uint64_t>(ts)};
                struct get_data data = {
                  .submit    = internal__io_uring_flush_sq(ring),
                  .wait_nr   = wait_nr,
                  .get_flags = (1U << 3),
                  .sz        = sizeof(arg),
                  .has_ts    = ts != nullptr,
                  .arg       = &arg};
                return _io_uring_get_cqe(ring, cqe_ptr, &data);
            }
            to_submit = static_cast<unsigned>(internal__io_uring_submit_timeout(ring, wait_nr, ts));
            if (to_submit < 0) {
                return static_cast<int>(to_submit);
            }
        } else {
            to_submit = internal__io_uring_flush_sq(ring);
        }
        return internal__io_uring_get_cqe(ring, cqe_ptr, to_submit, wait_nr, sigmask);
    }

    static inline int io_uring_submit_and_wait_min_timeout(
      struct io_uring          *ring,
      struct io_uring_cqe     **cqe_ptr,
      uint32_t                  wait_nr,
      struct __kernel_timespec *ts,
      uint32_t                  min_wait,
      sigset_t                 *sigmask) noexcept {
        if (!(ring->features & (1U << 15))) {
            return -22;
        }
        return internal__io_uring_submit_and_wait_timeout(ring, cqe_ptr, wait_nr, ts, min_wait, sigmask);
    }

    static inline int io_uring_submit_and_wait_timeout(
      struct io_uring          *ring,
      struct io_uring_cqe     **cqe_ptr,
      uint32_t                  wait_nr,
      struct __kernel_timespec *ts,
      sigset_t                 *sigmask) noexcept {
        return internal__io_uring_submit_and_wait_timeout(ring, cqe_ptr, wait_nr, ts, 0, sigmask);
    }

    static inline int io_uring_submit_and_wait(struct io_uring *ring, uint32_t wait_nr) noexcept {
        return internal__io_uring_submit_and_wait(ring, wait_nr);
    }

    static inline int internal__fls(int x) {
        if (!x) {
            return 0;
        }
        return 8 * static_cast<int>(sizeof(x)) - __builtin_clz(static_cast<unsigned int>(x));
    }

    static unsigned roundup_pow2(unsigned depth) {
        return 1U << static_cast<uint32_t>(internal__fls(static_cast<int>(depth - 1)));
    }

    static inline int get_sq_cq_entries(unsigned entries, io_uring_params *p, unsigned *sq, unsigned *cq) {
        unsigned cq_entries;

        if (!entries) {
            return -EINVAL;
        }
        if (entries > KERN_MAX_ENTRIES) {
            if (!(p->flags & IORING_SETUP_CLAMP)) {
                return -EINVAL;
            }
            entries = KERN_MAX_ENTRIES;
        }

        entries = roundup_pow2(entries);
        if (p->flags & IORING_SETUP_CQSIZE) {
            if (!p->cq_entries) {
                return -EINVAL;
            }
            cq_entries = p->cq_entries;
            if (cq_entries > KERN_MAX_CQ_ENTRIES) {
                if (!(p->flags & IORING_SETUP_CLAMP)) {
                    return -EINVAL;
                }
                cq_entries = KERN_MAX_CQ_ENTRIES;
            }
            cq_entries = roundup_pow2(cq_entries);
            if (cq_entries < entries) {
                return -EINVAL;
            }
        } else {
            cq_entries = 2 * entries;
        }

        *sq = entries;
        *cq = cq_entries;
        return 0;
    }

    static inline int __sys_munmap(void *addr, size_t length) {
        auto const ret = munmap(addr, length);
        return (ret < 0) ? -errno : ret;
    }

    void io_uring_unmap_rings(struct io_uring_sq *sq, struct io_uring_cq *cq) {
        if (sq->ring_sz) {
            __sys_munmap(sq->ring_ptr, sq->ring_sz);
        }
        if (cq->ring_ptr && cq->ring_sz && cq->ring_ptr != sq->ring_ptr) {
            __sys_munmap(cq->ring_ptr, cq->ring_sz);
        }
    }

    void io_uring_setup_ring_pointers(io_uring_params *p, struct io_uring_sq *sq, struct io_uring_cq *cq) {
        sq->khead         = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.head);
        sq->ktail         = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.tail);
        sq->kring_mask    = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.ring_mask);
        sq->kring_entries = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.ring_entries);
        sq->kflags        = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.flags);
        sq->kdropped      = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.dropped);
        if (!(p->flags & IORING_SETUP_NO_SQARRAY)) {
            sq->array = reinterpret_cast<unsigned *>(static_cast<char *>(sq->ring_ptr) + p->sq_off.array);
        }

        cq->khead         = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.head);
        cq->ktail         = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.tail);
        cq->kring_mask    = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.ring_mask);
        cq->kring_entries = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.ring_entries);
        cq->koverflow     = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.overflow);
        cq->cqes          = reinterpret_cast<io_uring_cqe *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.cqes);
        if (p->cq_off.flags) {
            cq->kflags = reinterpret_cast<unsigned *>(static_cast<char *>(cq->ring_ptr) + p->cq_off.flags);
        }

        sq->ring_mask    = *sq->kring_mask;
        sq->ring_entries = *sq->kring_entries;
        cq->ring_mask    = *cq->kring_mask;
        cq->ring_entries = *cq->kring_entries;
    }

    static size_t params_sqes_size(io_uring_params const *p, unsigned sqes) {
        sqes <<= io_uring_sqe_shift_from_flags(p->flags);
        return sqes * sizeof(struct io_uring_sqe);
    }

    static size_t params_cq_size(io_uring_params const *p, unsigned cqes) {
        cqes <<= io_uring_cqe_shift_from_flags(p->flags);
        return cqes * sizeof(struct io_uring_cqe);
    }

    static inline void *ERR_PTR(intptr_t n) noexcept {
        return reinterpret_cast<void *>(n);
    }

    static inline int PTR_ERR(void const *ptr) noexcept {
        return static_cast<int>(reinterpret_cast<intptr_t>(ptr));
    }

    static inline bool IS_ERR(void const *ptr) noexcept {
        return reinterpret_cast<uintptr_t>(ptr) >= static_cast<uintptr_t>(-4095L);
    }

    static inline int internal__sys_madvise(void *addr, size_t length, int advice) noexcept {
        auto const ret = madvise(addr, length, advice);
        return (ret < 0) ? -errno : ret;
    }

    static inline int64_t get_page_size() noexcept {
        auto page_size = sysconf(_SC_PAGESIZE);
        if (page_size < 0) {
            page_size = 4096;
        }

        return page_size;
    }

    static inline int internal__sys_io_uring_setup(uint32_t entries, io_uring_params *p) {
        auto const ret = static_cast<int>(syscall(__NR_io_uring_setup, entries, p));
        return (ret < 0) ? -errno : ret;
    }

    static inline int internal__sys_close(int fd) {
        auto const ret = close(fd);
        return (ret < 0) ? -errno : ret;
    }

    static inline int internal__sys_io_uring_register(uint32_t fd, uint32_t opcode, void const *arg, uint32_t nr_args) {
        auto const ret = static_cast<int>(syscall(__NR_io_uring_register, fd, opcode, arg, nr_args));
        return (ret < 0) ? -errno : ret;
    }

    static inline int do_register(struct io_uring *ring, uint32_t opcode, void const *arg, uint32_t nr_args) {
        int fd = 0;

        // liburing_sanitize_address(arg);

        if (ring->int_flags & INT_FLAG_REG_REG_RING) {
            opcode |= IORING_REGISTER_USE_REGISTERED_RING;
            fd      = ring->enter_ring_fd;
        } else {
            fd = ring->ring_fd;
        }

        return internal__sys_io_uring_register(static_cast<uint32_t>(fd), opcode, arg, nr_args);
    }

    static inline int io_uring_unregister_ring_fd(struct io_uring *ring) {
        struct io_uring_rsrc_update up = {.offset = static_cast<__u32>(ring->enter_ring_fd), .resv = 0, .data = 0};
        int                         ret;

        if (!(ring->int_flags & INT_FLAG_REG_RING)) {
            return -EINVAL;
        }

        ret = do_register(ring, IORING_UNREGISTER_RING_FDS, &up, 1);
        if (ret == 1) {
            ring->enter_ring_fd  = ring->ring_fd;
            ring->int_flags     &= ~(INT_FLAG_REG_RING | INT_FLAG_REG_REG_RING);
        }
        return ret;
    }

    static inline void *__sys_mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset) {
        auto *const ret = mmap(addr, length, prot, flags, fd, offset);
        return (ret == MAP_FAILED) ? ERR_PTR(-errno) : ret;
    }

    static inline int io_uring_mmap(int fd, io_uring_params *p, struct io_uring_sq *sq, struct io_uring_cq *cq) {
        size_t sqes_sz;
        int    ret;

        sq->ring_sz = p->sq_off.array + p->sq_entries * sizeof(unsigned);
        cq->ring_sz = p->cq_off.cqes + params_cq_size(p, p->cq_entries);

        if (p->features & IORING_FEAT_SINGLE_MMAP) {
            if (cq->ring_sz > sq->ring_sz) {
                sq->ring_sz = cq->ring_sz;
            }
            cq->ring_sz = sq->ring_sz;
        }
        sq->ring_ptr =
          __sys_mmap(0, sq->ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQ_RING);
        if (IS_ERR(sq->ring_ptr)) {
            return PTR_ERR(sq->ring_ptr);
        }

        if (p->features & IORING_FEAT_SINGLE_MMAP) {
            cq->ring_ptr = sq->ring_ptr;
        } else {
            cq->ring_ptr =
              __sys_mmap(0, cq->ring_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_CQ_RING);
            if (IS_ERR(cq->ring_ptr)) {
                ret          = PTR_ERR(cq->ring_ptr);
                cq->ring_ptr = nullptr;
                goto err;
            }
        }

        sqes_sz     = params_sqes_size(p, p->sq_entries);
        sq->sqes_sz = static_cast<uint32_t>(sqes_sz);
        if (sq->sqes_sz != sqes_sz) {
            ret = -EINVAL;
            goto err;
        }

        sq->sqes = reinterpret_cast<io_uring_sqe *>(
          __sys_mmap(0, sq->sqes_sz, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, fd, IORING_OFF_SQES));
        if (IS_ERR(sq->sqes)) {
            ret = PTR_ERR(sq->sqes);
        err:
            io_uring_unmap_rings(sq, cq);
            return ret;
        }

        io_uring_setup_ring_pointers(p, sq, cq);
        return 0;
    }

    static inline int io_uring_queue_mmap(int fd, io_uring_params *p, struct io_uring *ring) {
        memset(ring, 0, sizeof(*ring));
        return io_uring_mmap(fd, p, &ring->sq, &ring->cq);
    }

    static size_t io_uring_sqes_size(const struct io_uring *ring) {
        return (ring->sq.ring_entries << io_uring_sqe_shift(ring)) * sizeof(struct io_uring_sqe);
    }

    static inline int io_uring_ring_dontfork(struct io_uring *ring) {
        size_t len;
        int    ret;

        if (!ring->sq.ring_ptr || !ring->sq.sqes || !ring->cq.ring_ptr) {
            return -EINVAL;
        }

        len = io_uring_sqes_size(ring);
        ret = internal__sys_madvise(ring->sq.sqes, len, MADV_DONTFORK);
        if (ret < 0) {
            return ret;
        }

        len = ring->sq.ring_sz;
        ret = internal__sys_madvise(ring->sq.ring_ptr, len, MADV_DONTFORK);
        if (ret < 0) {
            return ret;
        }

        if (ring->cq.ring_ptr != ring->sq.ring_ptr) {
            len = ring->cq.ring_sz;
            ret = internal__sys_madvise(ring->cq.ring_ptr, len, MADV_DONTFORK);
            if (ret < 0) {
                return ret;
            }
        }

        return 0;
    }

    /*
     * Returns negative for error, or number of bytes used in the buffer on success
     */
    static inline int io_uring_alloc_huge(
      unsigned            entries,
      io_uring_params    *p,
      struct io_uring_sq *sq,
      struct io_uring_cq *cq,
      void               *buf,
      size_t              buf_size) {
        auto const page_size = static_cast<uint64_t>(get_page_size());
        unsigned   sq_entries;
        unsigned   cq_entries;
        size_t     sqes_size = 0;
        size_t     ring_mem;
        size_t     sqes_mem;
        uint64_t   mem_used = 0;
        void      *ptr;
        int        ret;

        ret = get_sq_cq_entries(entries, p, &sq_entries, &cq_entries);
        if (ret) {
            return ret;
        }

        sqes_mem = params_sqes_size(p, sq_entries);
        if (!(p->flags & IORING_SETUP_NO_SQARRAY)) {
            sqes_mem += sq_entries * sizeof(unsigned);
        }
        sqes_mem = (sqes_mem + page_size - 1) & ~(page_size - 1);

        ring_mem  = KRING_SIZE;
        ring_mem += params_cq_size(p, cq_entries);
        mem_used  = sqes_mem + ring_mem;
        mem_used  = (mem_used + page_size - 1) & ~(page_size - 1);

        /*
         * A maxed-out number of CQ entries with IORING_SETUP_CQE32 fills a 2MB
         * huge page by itself, so the SQ entries won't fit in the same huge
         * page. For SQEs, that shouldn't be possible given KERN_MAX_ENTRIES,
         * but check that too to future-proof (e.g. against different huge page
         * sizes). Bail out early so we don't overrun.
         */
        if (!buf && (sqes_mem > huge_page_size || ring_mem > huge_page_size)) {
            return -ENOMEM;
        }

        if (buf) {
            if (mem_used > buf_size) {
                return -ENOMEM;
            }
            ptr = buf;
        } else {
            int map_hugetlb = 0;
            if (sqes_mem <= page_size) {
                buf_size = page_size;
            } else {
                buf_size    = huge_page_size;
                map_hugetlb = MAP_HUGETLB;
            }
            sqes_size = buf_size;
            ptr =
              __sys_mmap(nullptr, sqes_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | map_hugetlb, -1, 0);
            if (IS_ERR(ptr)) {
                return PTR_ERR(ptr);
            }
        }

        sq->sqes    = reinterpret_cast<io_uring_sqe *>(ptr);
        sq->sqes_sz = static_cast<uint32_t>(sqes_size);
        if (mem_used <= buf_size) {
            sq->ring_ptr = static_cast<char *>(static_cast<void *>(sq->sqes)) + sqes_mem;
            /* clear ring sizes, we have just one mmap() to undo */
            cq->ring_sz  = 0;
            sq->ring_sz  = 0;
        } else {
            int map_hugetlb = 0;
            if (ring_mem <= page_size) {
                buf_size = page_size;
            } else {
                buf_size    = huge_page_size;
                map_hugetlb = MAP_HUGETLB;
            }
            ptr =
              __sys_mmap(nullptr, buf_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS | map_hugetlb, -1, 0);
            if (IS_ERR(ptr)) {
                if (sqes_size) {
                    __sys_munmap(sq->sqes, sqes_size);
                }
                return PTR_ERR(ptr);
            }
            sq->ring_ptr = ptr;
            sq->ring_sz  = buf_size;
            cq->ring_sz  = 0;
        }

        cq->ring_ptr        = static_cast<void *>(sq->ring_ptr);
        p->sq_off.user_addr = reinterpret_cast<uint64_t>(sq->sqes);
        p->cq_off.user_addr = reinterpret_cast<uint64_t>(sq->ring_ptr);
        return static_cast<int>(mem_used);
    }

    static inline int
    __io_uring_queue_init_params(unsigned entries, io_uring *ring, io_uring_params *p, void *buf, size_t buf_size) {
        int       fd, ret = 0;
        unsigned *sq_array;
        unsigned  sq_entries, index;

        memset(ring, 0, sizeof(*ring));

        /*
         * The kernel does this check already, but checking it here allows us
         * to avoid handling it below.
         */
        if (p->flags & IORING_SETUP_REGISTERED_FD_ONLY && !(p->flags & IORING_SETUP_NO_MMAP)) {
            return -EINVAL;
        }

        if (p->flags & IORING_SETUP_NO_MMAP) {
            ret = io_uring_alloc_huge(entries, p, &ring->sq, &ring->cq, buf, buf_size);
            if (ret < 0) {
                return ret;
            }
            if (buf) {
                ring->int_flags |= INT_FLAG_APP_MEM;
            }
        }

        fd = internal__sys_io_uring_setup(entries, p);
        if (fd < 0) {
            if ((p->flags & IORING_SETUP_NO_MMAP) && !(ring->int_flags & INT_FLAG_APP_MEM)) {
                __sys_munmap(ring->sq.sqes, ring->sq.sqes_sz);
                io_uring_unmap_rings(&ring->sq, &ring->cq);
            }
            return fd;
        }

        if (!(p->flags & IORING_SETUP_NO_MMAP)) {
            ret = io_uring_queue_mmap(fd, p, ring);
            if (ret) {
                internal__sys_close(fd);
                return ret;
            }
        } else {
            io_uring_setup_ring_pointers(p, &ring->sq, &ring->cq);
        }

        /*
         * Directly map SQ slots to SQEs
         */
        sq_entries = ring->sq.ring_entries;

        if (!(p->flags & IORING_SETUP_NO_SQARRAY)) {
            sq_array = ring->sq.array;
            for (index = 0; index < sq_entries; index++) {
                sq_array[index] = index;
            }
        }
        ring->features      = p->features;
        ring->flags         = p->flags;
        ring->enter_ring_fd = fd;
        if (p->flags & IORING_SETUP_REGISTERED_FD_ONLY) {
            ring->ring_fd    = -1;
            ring->int_flags |= INT_FLAG_REG_RING | INT_FLAG_REG_REG_RING;
        } else {
            ring->ring_fd = fd;
        }
        /*
         * IOPOLL always needs to enter, except if SQPOLL is set as well.
         * Use an internal flag to check for this.
         */
        if ((ring->flags & (IORING_SETUP_IOPOLL | IORING_SETUP_SQPOLL)) == IORING_SETUP_IOPOLL) {
            ring->int_flags |= INT_FLAG_CQ_ENTER;
        }

        return ret;
    }

    static inline int io_uring_queue_init_try_nosqarr(
      unsigned         entries,
      struct io_uring *ring,
      io_uring_params *p,
      void            *buf,
      size_t           buf_size) {
        unsigned flags = p->flags;
        int      ret;

        p->flags |= IORING_SETUP_NO_SQARRAY;
        ret       = __io_uring_queue_init_params(entries, ring, p, buf, buf_size);

        /* don't fallback if explicitly asked for NOSQARRAY */
        if (ret != -EINVAL || (flags & IORING_SETUP_NO_SQARRAY)) {
            return ret;
        }

        p->flags = flags;
        return __io_uring_queue_init_params(entries, ring, p, buf, buf_size);
    }

    /*
     * Like io_uring_queue_init_params(), except it allows the application to pass
     * in a pre-allocated memory range that is used for the shared data between
     * the kernel and the application. This includes the sqes array, and the two
     * rings. The memory must be contiguous, the use case here is that the app
     * allocates a huge page and passes it in.
     *
     * Returns the number of bytes used in the buffer, the app can then reuse
     * the buffer with the returned offset to put more rings in the same huge
     * page. Returns -ENOMEM if there's not enough room left in the buffer to
     * host the ring.
     */
    static inline int
    io_uring_queue_init_mem(unsigned entries, struct io_uring *ring, io_uring_params *p, void *buf, size_t buf_size) {
        /* should already be set... */
        p->flags |= IORING_SETUP_NO_MMAP;
        return io_uring_queue_init_try_nosqarr(entries, ring, p, buf, buf_size);
    }

    static inline int io_uring_queue_init_params(unsigned entries, struct io_uring *ring, io_uring_params *p) {
        int ret;

        ret = io_uring_queue_init_try_nosqarr(entries, ring, p, nullptr, 0);
        return ret >= 0 ? 0 : ret;
    }

    /*
     * Returns -errno on error, or zero on success. On success, 'ring'
     * contains the necessary information to read/write to the rings.
     */
    static inline int io_uring_queue_init(unsigned entries, struct io_uring *ring, unsigned flags) noexcept {
        struct io_uring_params p;

        memset(&p, 0, sizeof(p));
        p.flags = flags;

        return io_uring_queue_init_params(entries, ring, &p);
    }

    static void io_uring_queue_exit(struct io_uring *ring) {
        struct io_uring_sq *sq = &ring->sq;
        struct io_uring_cq *cq = &ring->cq;

        if (!(ring->int_flags & INT_FLAG_APP_MEM)) {
            __sys_munmap(sq->sqes, sq->sqes_sz);
            io_uring_unmap_rings(sq, cq);
        }

        /*
         * Not strictly required, but frees up the slot we used now rather
         * than at process exit time.
         */
        if (ring->int_flags & INT_FLAG_REG_RING) {
            io_uring_unregister_ring_fd(ring);
        }
        if (ring->ring_fd != -1) {
            internal__sys_close(ring->ring_fd);
        }
    }

    static inline int io_uring_register_probe(struct io_uring *ring, io_uring_probe *p, uint32_t nr_ops) {
        return do_register(ring, IORING_REGISTER_PROBE, p, nr_ops);
    }

    static inline io_uring_probe *io_uring_get_probe_ring(struct io_uring *ring) {
        io_uring_probe *probe;
        size_t          len;
        int             r;

        len   = sizeof(*probe) + 256 * sizeof(struct io_uring_probe_op);
        probe = reinterpret_cast<io_uring_probe *>(malloc(len));
        if (!probe) {
            return nullptr;
        }
        memset(probe, 0, len);

        r = io_uring_register_probe(ring, probe, 256);
        if (r >= 0) {
            return probe;
        }

        free(probe);
        return nullptr;
    }

    static inline io_uring_probe *io_uring_get_probe(void) {
        struct io_uring ring;
        io_uring_probe *probe;
        int             r;

        r = io_uring_queue_init(2, &ring, 0);
        if (r < 0) {
            return nullptr;
        }

        probe = io_uring_get_probe_ring(&ring);
        io_uring_queue_exit(&ring);
        return probe;
    }

    static inline void io_uring_free_probe(io_uring_probe *probe) {
        free(probe);
    }

    static inline size_t rings_size(io_uring_params *p, unsigned entries, unsigned cq_entries, int64_t page_size) {
        auto const _page_size = static_cast<uint64_t>(page_size) - 1;

        size_t cq_size  = params_cq_size(p, cq_entries);
        cq_size        += KRING_SIZE;
        cq_size         = (cq_size + _page_size) & ~_page_size;
        auto pages      = cq_size / static_cast<size_t>(page_size);

        size_t sq_size  = params_sqes_size(p, entries);
        sq_size         = (sq_size + _page_size) & ~_page_size;
        pages          += sq_size / static_cast<size_t>(page_size);
        return pages * static_cast<size_t>(page_size);
    }

    static inline ssize_t io_uring_memory_size_params(unsigned entries, io_uring_params *p) {
        unsigned sq, cq;
        long     page_size;
        ssize_t  ret;

        if (!entries) {
            return -EINVAL;
        }
        if (entries > KERN_MAX_ENTRIES) {
            if (!(p->flags & IORING_SETUP_CLAMP)) {
                return -EINVAL;
            }
            entries = KERN_MAX_ENTRIES;
        }

        ret = get_sq_cq_entries(entries, p, &sq, &cq);
        if (ret) {
            return ret;
        }

        page_size = get_page_size();
        return static_cast<ssize_t>(rings_size(p, sq, cq, page_size));
    }

    static inline ssize_t io_uring_memory_size(unsigned entries, unsigned ring_flags) {
        struct io_uring_params p = {
          .flags = ring_flags,
        };

        return io_uring_memory_size_params(entries, &p);
    }

    static inline ssize_t io_uring_mlock_size_params(unsigned entries, io_uring_params *p) {
        struct io_uring_params lp;
        struct io_uring        ring;
        ssize_t                ret;

        memset(&lp, 0, sizeof(lp));

        ret = io_uring_queue_init_params(entries, &ring, &lp);
        if (!ret) {
            io_uring_queue_exit(&ring);
        }

        if (lp.features & IORING_FEAT_NATIVE_WORKERS) {
            return 0;
        }

        return io_uring_memory_size_params(entries, p);
    }

    static inline ssize_t io_uring_mlock_size(unsigned entries, unsigned flags) {
        struct io_uring_params p;

        memset(&p, 0, sizeof(p));
        p.flags = flags;
        return io_uring_mlock_size_params(entries, &p);
    }

    static inline int io_uring_register_buf_ring(struct io_uring *ring, struct io_uring_buf_reg *reg, uint32_t flags) {
        reg->flags |= flags;
        return do_register(ring, IORING_REGISTER_PBUF_RING, reg, 1);
    }


#if defined(__hppa__)
    static struct io_uring_buf_ring *
    br_setup(struct io_uring *ring, uint32_t nentries, int bgid, uint32_t flags, int *err) {
        struct io_uring_buf_ring *br;
        struct io_uring_buf_reg   reg;
        size_t                    ring_size;
        off_t                     off;
        int                       lret;

        memset(&reg, 0, sizeof(reg));
        reg.ring_entries = nentries;
        reg.bgid         = bgid;
        reg.flags        = IOU_PBUF_RING_MMAP;

        *err = 0;
        lret = io_uring_register_buf_ring(ring, &reg, flags);
        if (lret) {
            *err = lret;
            return nullptr;
        }

        off       = IORING_OFF_PBUF_RING | (uint64_t long) bgid << IORING_OFF_PBUF_SHIFT;
        ring_size = nentries * sizeof(struct io_uring_buf);
        br        = __sys_mmap(NULL, ring_size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_POPULATE, ring->ring_fd, off);
        if (IS_ERR(br)) {
            *err = PTR_ERR(br);
            io_uring_unregister_buf_ring(ring, bgid);
            return nullptr;
        }

        return br;
    }
#else
    static struct io_uring_buf_ring *
    br_setup(struct io_uring *ring, uint32_t nentries, int bgid, uint32_t flags, int *err) {
        struct io_uring_buf_ring *br;
        struct io_uring_buf_reg   reg;
        size_t                    ring_size;
        int                       lret;

        memset(&reg, 0, sizeof(reg));
        ring_size = nentries * sizeof(struct io_uring_buf);
        br        = reinterpret_cast<io_uring_buf_ring *>(
          __sys_mmap(NULL, ring_size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0));
        if (IS_ERR(br)) {
            *err = PTR_ERR(br);
            return nullptr;
        }

        reg.ring_addr    = reinterpret_cast<uint64_t>(br);
        reg.ring_entries = nentries;
        reg.bgid         = static_cast<__u16>(bgid);

        *err = 0;
        lret = io_uring_register_buf_ring(ring, &reg, flags);
        if (lret) {
            __sys_munmap(br, ring_size);
            *err = lret;
            br   = nullptr;
        }

        return br;
    }
#endif

    static inline void io_uring_buf_ring_init(struct io_uring_buf_ring *br) noexcept {
        br->tail_data.tail = 0;
    }

    static inline struct io_uring_buf_ring *
    io_uring_setup_buf_ring(struct io_uring *ring, uint32_t nentries, int bgid, uint32_t flags, int *err) {
        io_uring_buf_ring *br;

        br = br_setup(ring, nentries, bgid, flags, err);
        if (br) {
            io_uring_buf_ring_init(br);
        }

        return br;
    }

    static inline int io_uring_unregister_buf_ring(struct io_uring *ring, int bgid) {
        io_uring_buf_reg reg = {.bgid = static_cast<__u16>(bgid)};

        return do_register(ring, IORING_UNREGISTER_PBUF_RING, &reg, 1);
    }

    static inline int
    io_uring_free_buf_ring(struct io_uring *ring, struct io_uring_buf_ring *br, uint32_t nentries, int bgid) {
        int ret;

        ret = io_uring_unregister_buf_ring(ring, bgid);
        if (ret) {
            return ret;
        }

        __sys_munmap(br, nentries * sizeof(struct io_uring_buf));
        return 0;
    }

    static inline void io_uring_sqe_set_data(struct io_uring_sqe *sqe, void *data) noexcept {
        sqe->user_data = reinterpret_cast<uint64_t>(data);
    }

    static inline void io_uring_sqe_set_data64(struct io_uring_sqe *sqe, __u64 data) noexcept {
        sqe->user_data = data;
    }

    static inline void *io_uring_cqe_get_data(const struct io_uring_cqe *cqe) noexcept {
        return reinterpret_cast<void *>(static_cast<uintptr_t>(cqe->user_data));
    }

    static inline __u64 io_uring_cqe_get_data64(const struct io_uring_cqe *cqe) {
        return cqe->user_data;
    }

    static inline int
    io_uring_wait_cqe_nr(struct io_uring *ring, struct io_uring_cqe **cqe_ptr, uint32_t wait_nr) noexcept {
        return internal__io_uring_get_cqe(ring, cqe_ptr, 0, wait_nr, nullptr);
    }

    static inline int io_uring_wait_cqe(struct io_uring *ring, struct io_uring_cqe **cqe_ptr) noexcept {
        if (!internal__io_uring_peek_cqe(ring, cqe_ptr, nullptr) && *cqe_ptr) {
            return 0;
        }
        return io_uring_wait_cqe_nr(ring, cqe_ptr, 1);
    }

    static inline void io_uring_cqe_seen(struct io_uring *ring, struct io_uring_cqe *cqe) noexcept {
        if (cqe) {
            io_uring_cq_advance(ring, io_uring_cqe_nr(cqe));
        }
    }

    static inline int io_uring_peek_cqe(struct io_uring *ring, struct io_uring_cqe **cqe_ptr) noexcept {
        if (!internal__io_uring_peek_cqe(ring, cqe_ptr, nullptr) && *cqe_ptr) {
            return 0;
        }
        return io_uring_wait_cqe_nr(ring, cqe_ptr, 0);
    }

#define io_uring_for_each_cqe(ring, __head__, cqe)                                                     \
    for (struct io_uring_cqe_iter __ITER__ = io_uring_cqe_iter_init(ring);                             \
         (__head__)                        = __ITER__.head, io_uring_cqe_iter_next(&__ITER__, &(cqe)); \
         (void) (__head__))

    static inline struct io_uring_cqe_iter io_uring_cqe_iter_init(const struct io_uring *ring) noexcept {
        return io_uring_cqe_iter{
          .cqes  = ring->cq.cqes,
          .mask  = ring->cq.ring_mask,
          .shift = io_uring_cqe_shift(ring),
          .head  = *ring->cq.khead,
          .tail  = io_uring_smp_load_acquire(ring->cq.ktail)};
    }

    static inline bool io_uring_cqe_iter_next(struct io_uring_cqe_iter *iter, struct io_uring_cqe **cqe) noexcept {
        if (iter->head == iter->tail) {
            return false;
        }
        *cqe = &iter->cqes[(iter->head++ & iter->mask) << iter->shift];
        if ((*cqe)->flags & (1U << 15)) {
            iter->head++;
        }
        return true;
    }


} // namespace webpp::io::inline iouring_impl

// NOLINTEND(*-magic-numbers, *-identifier-length)

#endif // WEBPP_IOURING_IMPL_HPP
