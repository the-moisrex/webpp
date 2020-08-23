#ifndef WEBPP_INTERFACE_FCGI_PROTOCOL
#define WEBPP_INTERFACE_FCGI_PROTOCOL

#include "../../../std/string_view.hpp"

#include <limits>

// https://github.com/eddic/fastcgipp
// http://www.mit.edu/~yandros/doc/specs/fcgi-spec.html

namespace webpp::fcgi {

    static constexpr unsigned chunk_size = 8u;

    template <typename T, stl::size_t Index = 0>
    struct indexed_value {
        T                            value;
        static constexpr stl::size_t index = Index;

        constexpr indexed_value(T value) noexcept : value(value) {}
    };

    /**
     * Join the pieces of integer into one bigger integer
     */
    template <typename Full, typename PieceType, uint8_t... Index>
    constexpr Full join_pieces(indexed_value<PieceType, Index> const... pieces) noexcept {
        constexpr stl::uint8_t piece_size = sizeof(PieceType) * 8u;
        return ((static_cast<Full>(pieces.value) << (pieces.index * piece_size)) | ...);
    }

    template <typename Full, typename PieceType>
    constexpr Full join_pieces(const PieceType* pieces) noexcept {
        constexpr stl::uint8_t piece_count = sizeof(Full) / sizeof(PieceType);
        if constexpr (piece_count == 4) {
            return join_pieces<Full, PieceType, 3, 2, 1, 0>(pieces[0], pieces[1], pieces[2], pieces[3]);
        } else if constexpr (3 == piece_count) {
            return join_pieces<Full, PieceType, 2, 1, 0>(pieces[0], pieces[1], pieces[2]);
        } else if constexpr (2 == piece_count) {
            return join_pieces<Full, PieceType, 1, 0>(pieces[0], pieces[1]);
        } else if constexpr (1 == piece_count) {
            return join_pieces<Full, PieceType, 0>(pieces[0]);
        } else {
            throw stl::invalid_argument("We're not able to handle these types.");
        }
    }


    template <typename Full, typename PieceType, uint8_t... Index>
    constexpr void split_pieces(Full value, indexed_value<PieceType&, Index>... pieces) noexcept {
        constexpr uint8_t   pieces_size = sizeof(PieceType) * 8u;
        constexpr PieceType mask        = stl::numeric_limits<PieceType>::max();
        ((pieces.value = static_cast<PieceType>((value >> (pieces.index * pieces_size)) & mask)), ...);
    }


    /**
     * It is very important that this record_type's size be uint8_t
     */
    enum struct record_type : uint8_t {
        begin_request     = 1,
        abort_request     = 2,
        end_request       = 3,
        params            = 4,
        std_in            = 5,
        std_out           = 6,
        std_err           = 7,
        data              = 8,
        get_values        = 9,
        get_values_result = 10,
        unknown_type      = 11
    };

    // Defines the possible roles a FastCGI application may play
    enum struct role : uint16_t { responder = 1, authorizer = 2, filter = 3 };

    //! possible statuses a request may declare when complete
    enum struct protocol_status : uint8_t {
        request_complete = 0,
        cant_mpx_conn    = 1,
        overloaded       = 2,
        unknown_role     = 3
    };

    /**
     * This structure defines the body used in FastCGI UNKNOWN_TYPE records.
     * It can be casted to raw 8 byte blocks of data and transmitted as is.
     * An UNKNOWN_TYPE record is sent as a reply to record types that are not recognized.
     */
    struct unknown_type {
        record_type type;
        uint8_t     reserved[7];
    };

    /**
     * This is the version 1.0 of the FastCGI Record protocol
     */
    struct header {
        /* Identifies the FastCGI protocol version. This is the version 1 */
        uint8_t version = 1;

        /* Identifies the FastCGI record type, i.e. the general function
         * that the record performs. Specific record types and their
         * functions are detailed in later sections */
        record_type type;

        /* Identifies the FastCGI request to which the record belongs */
        uint8_t request_id_b1;
        uint8_t request_id_b0;

        /* The number of bytes in the contentData component of the record */
        uint8_t content_length_b1;
        uint8_t content_length_b0;

        /* The number of bytes in the paddingData component of the record */
        uint8_t padding_length;

        /* reserved for later use */
        uint8_t reserved = 0;

        constexpr header(record_type _type, uint16_t _req_id, uint16_t _content_length,
                         uint8_t _padd_len) noexcept
          : type{_type},
            request_id_b1{static_cast<uint8_t>(_req_id >> 8u)},
            request_id_b0{static_cast<uint8_t>(_req_id)},
            content_length_b1{static_cast<uint8_t>(_content_length >> 8u)},
            content_length_b0{static_cast<uint8_t>(_content_length)},
            padding_length{_padd_len} {}

        [[nodiscard]] constexpr uint16_t request_id() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(request_id_b1, request_id_b0);
        }

        void request_id(uint16_t req_id) noexcept {
            split_pieces<uint16_t, uint8_t, 1, 0>(req_id, request_id_b1, request_id_b0);
        }

        [[nodiscard]] constexpr uint16_t content_length() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(content_length_b1, content_length_b0);
        }

        void content_length(uint16_t _content_length) noexcept {
            split_pieces<uint16_t, uint8_t, 1, 0>(_content_length, content_length_b1, content_length_b0);
        }

        /**
         * Generally there are two types of records, Managements and the Application.
         */
        [[nodiscard]] constexpr bool is_management_record() const noexcept {
            return request_id() == 0;
        }
    };

    struct begin_request {
        static constexpr auto keep_connection_flag = 1u;

        uint8_t role_b1;
        uint8_t role_b0;
        uint8_t flags;
        uint8_t reserved[5] = {};

        [[nodiscard]] uint16_t role_value() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(role_b1, role_b0);
        }

        [[nodiscard]] enum role role() const noexcept {
            return static_cast<enum role>(role_value());
        }
        /*!
         * If this value is false, the socket should be closed on our side when the request is complete.
         * If true, the other side will close the socket when done and potentially reuse the socket and
         * multiplex other requests on it.
         *
         * @return Boolean value as to whether or not the connection is kept alive.
         */
        [[nodiscard]] constexpr bool kill() const noexcept {
            return !(flags & keep_connection_flag);
        }
    };
    /*
     * This structure defines the body used in FastCGI END_REQUEST records.
     * It can be casted to raw 8 byte blocks of data and transmitted as is.
     * An END_REQUEST record is sent when this side wishes to terminate a request.
     * This can be simply because it is complete or because of a problem.
     */
    struct end_request {
        uint8_t app_status_b3;
        uint8_t app_status_b2;
        uint8_t app_status_b1;
        uint8_t app_status_b0;
        uint8_t protocol_status_value;
        uint8_t reserved[3] = {};

        void app_status(uint32_t status_code) noexcept {
            split_pieces<uint32_t, uint8_t, 3, 2, 1, 0>(status_code, app_status_b3, app_status_b2,
                                                        app_status_b1, app_status_b0);
        }

        [[nodiscard]] uint32_t app_status() const noexcept {
            return join_pieces<uint32_t, uint8_t, 3, 2, 1, 0>(app_status_b3, app_status_b2, app_status_b1,
                                                              app_status_b0);
        }

        [[nodiscard]] enum protocol_status protocol_status() const noexcept {
            return static_cast<enum protocol_status>(protocol_status_value);
        }
    };

    template <stl::size_t NAME_LENGTH, stl::size_t VALUE_LENGTH>
    struct management_reply {
        static constexpr uint8_t     real_name_length  = NAME_LENGTH - 1;
        static constexpr uint8_t     real_value_length = VALUE_LENGTH - 1;
        static constexpr unsigned    fast_chunk_size   = sizeof(int_fast8_t) * chunk_size;
        static constexpr stl::size_t padding_length =
          (fast_chunk_size) -
          (sizeof(header) + (2 * sizeof(uint8_t)) + real_name_length + real_value_length) % fast_chunk_size;

        header  _header;
        uint8_t name_length  = real_name_length;
        uint8_t value_length = real_value_length;
        uint8_t name[real_name_length];
        uint8_t value[real_value_length];
        uint8_t padding[padding_length] = {};

      public:
        constexpr management_reply(char const (&_name)[NAME_LENGTH],
                                   char const (&_value)[VALUE_LENGTH]) noexcept
          : _header{record_type::get_values_result, 0u, real_name_length + real_value_length,
                    padding_length} {
            stl::copy_n(_name, real_name_length, name);
            stl::copy_n(_value, real_value_length, value);
        }


        /*
         * Determine the optimal record size given a requested content length
         *
         * @param[in] content_length Desired content length in record
         * @return Length of record including content, header and padding.
         */
        [[nodiscard]] stl::size_t record_size(stl::size_t content_length) noexcept {
            // Of course the maximum content length is in fact 0xffff bytes so any passed content length > 0xffff will be assumed 0xffff.
            if (content_length > 0xFFFFu)
                content_length = 0xFFFFU;
            return (content_length + sizeof(header) + chunk_size - 1) / chunk_size * chunk_size;
        }


    };

    static constexpr management_reply default_max_conns_reply{"FCGI_MAX_CONNS", "10"};
    static constexpr management_reply default_max_reqs_reply{"FCGI_MAX_REQS", "50"};
    static constexpr management_reply default_mpxs_conns_reply{"FCGI_MPXS_CONNS", "1"};

} // namespace webpp::protocol

#endif
