#ifndef WEBPP_INTERFACE_FCGI_PROTOCOL
#define WEBPP_INTERFACE_FCGI_PROTOCOL

#include "../../../std/string_view.hpp"

namespace webpp::protocol {

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

        header(record_type _type, uint16_t _req_id, uint16_t _content_length, uint8_t _padd_len) noexcept
          : type{_type},
            request_id_b1{static_cast<uint8_t>(_req_id >> 8u)},
            request_id_b0{static_cast<uint8_t>(_req_id)},
            content_length_b1{static_cast<uint8_t>(_content_length >> 8u)},
            content_length_b0{static_cast<uint8_t>(_content_length)},
            padding_length{_padd_len} {}

        inline uint16_t request_id() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(request_id_b1, request_id_b0);
        }

        inline void request_id(uint16_t req_id) noexcept {
            request_id_b1 = static_cast<uint8_t>(req_id >> 8u);
            request_id_b0 = static_cast<uint8_t>(req_id);
        }

        inline uint16_t content_length() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(content_length_b1, content_length_b0);
        }

        inline void content_length(uint16_t _content_length) noexcept {
            content_length_b1 = static_cast<uint8_t>(_content_length >> 8u);
            content_length_b0 = static_cast<uint8_t>(_content_length);
        }

        /**
         * Generally there are two types of records, Managements and the Application.
         */
        inline bool is_management_record() const noexcept {
            return request_id() == 0;
        }
    };

    class begin_request {
        uint8_t                  role_b1;
        uint8_t                  role_b0;
        uint8_t                  flags;
        [[maybe_unused]] uint8_t reserved[5] = {};

        [[nodiscard]] uint16_t role() const noexcept {
            return join_pieces<uint16_t, uint8_t, 1, 0>(role_b1, role_b0);
        }
    };

    struct end_request {
        uint8_t                  app_status_b3;
        uint8_t                  app_status_b2;
        uint8_t                  app_status_b1;
        uint8_t                  app_status_b0;
        uint8_t                  protocol_status;
        [[maybe_unused]] uint8_t reserved[3] = {};

        void app_status(uint32_t status_code) noexcept {
            app_status_b3 = static_cast<uint8_t>(status_code >> 24u);
            app_status_b2 = static_cast<uint8_t>(status_code >> 16u & 0xFFu);
            app_status_b1 = static_cast<uint8_t>(status_code >> 8u & 0xFFu);
            app_status_b0 = static_cast<uint8_t>(status_code);
        }

        [[nodiscard]] uint32_t app_status() const noexcept {
            return join_pieces<uint32_t, uint8_t, 3, 2, 1, 0>(app_status_b3, app_status_b2, app_status_b1,
                                                              app_status_b0);
        }
    };

    struct unknown_type {
        record_type type;
        uint8_t     reserved[7];
    };

    template <stl::size_t NAMELENGTH, stl::size_t VALUELENGTH, stl::size_t PADDINGLENGTH>
    struct management_reply {
      private:
        header  _header;
        uint8_t name_length  = NAMELENGTH;
        uint8_t value_length = VALUELENGTH;
        uint8_t name[NAMELENGTH];
        uint8_t value[VALUELENGTH];
        uint8_t padding[PADDINGLENGTH] = {};

      public:
        constexpr management_reply(char const* _name, char const* _value) noexcept
          : _header{record_type::get_values_result, 0u, NAMELENGTH + VALUELENGTH, PADDINGLENGTH} {
            auto _name_end  = _name + NAMELENGTH;
            auto _value_end = _value + VALUELENGTH;
            auto name_ptr   = name;
            auto value_ptr  = value;
            while (_name != _name_end)
                *name_ptr = *_name++;
            while (_value != _value_end)
                *value_ptr = *_value++;
        }
    };

    /**
     * Automatically calculate the management reply required template lengths
     */
    template <typename NameT, typename ValueT>
    management_reply(NameT name, ValueT value)
      -> management_reply<(sizeof(NameT) / sizeof(char)), (sizeof(ValueT) / sizeof(char)),
                          ((sizeof(int_fast8_t) * 8u) -
                           ((sizeof(management_reply<1, 1, 1>) - 3 + (sizeof(NameT) / sizeof(char)) +
                             (sizeof(ValueT) / sizeof(char))) %
                            (sizeof(int_fast8_t) * 8u)))>;

    management_reply default_max_conns_reply{"FCGI_MAX_CONNS", "10"};
    management_reply default_max_reqs_reply{"FCGI_MAX_REQS", "50"};
    management_reply default_mpxs_conns_reply{"FCGI_MPXS_CONNS", "1"};

} // namespace webpp::protocol

#endif
