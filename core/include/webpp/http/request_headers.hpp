// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../std/format.hpp"
#include "../std/vector.hpp"
#include "../traits/traits_concepts.hpp"
#include "./headers.hpp"
#include "../std/string_view.hpp"

namespace webpp {


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     */
    template <Traits TraitsType, typename HeaderEList, typename HeaderFieldType>
    class request_headers : public istl::vector<TraitsType, HeaderFieldType>, public HeaderEList {

        //        using super =
        //          istl::unordered_multiset<TraitsType, HeaderFieldType,
        //          request_header_field_hash<HeaderFieldType>,
        //                                   request_header_field_equals<HeaderFieldType>>;
        using super = istl::vector<TraitsType, HeaderFieldType>;

      public:
        using traits_type = TraitsType;
        using string_type = typename traits_type::string_type;
        using string_view_type = typename traits_type::string_view_type;
        using field_type  = HeaderFieldType;

        template <typename... Args>
        constexpr request_headers(Args&&... args) noexcept
          : super{stl::forward<Args>(args)...},
            HeaderEList{} {}


        // todo: fix this
        constexpr request_headers(istl::StringViewfiable auto &&header_string, auto&&...args)
        :   super{stl::forward< decltype(args)>(args)...}
        {
            parse_header_string(istl::to_string_view(header_string));
        }

    };


    struct request_header_field_descriptor {

        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::request_header_field_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::request_header_field_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = header_field_view<TraitsType, EList>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type = EList;
    };



    struct request_headers_descriptor {
        template <typename ExtensionType>
        struct has_related_extension_pack {
            static constexpr bool value = requires {
                typename ExtensionType::request_headers_extensions;
            };
        };

        template <typename ExtensionType>
        using related_extension_pack_type = typename ExtensionType::request_headers_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = request_headers<
          TraitsType, EList,
          typename ExtensionListType::template extensie_type<TraitsType, request_header_field_descriptor>>;

        // empty final extensie
        template <typename ExtensionListType, typename TraitsType, typename EList>
        using final_extensie_type = EList;
    };


}

#endif // WEBPP_REQUEST_HEADERS_HPP
