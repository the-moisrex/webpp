// Created by moisrex on 8/19/20.

#ifndef WEBPP_REQUEST_HEADERS_HPP
#define WEBPP_REQUEST_HEADERS_HPP

#include "../extensions/extension.hpp"
#include "../std/format.hpp"
#include "../std/string_view.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "./headers.hpp"

namespace webpp::http {


    /**
     * Setting non-ascii characters in the value section of the headers should
     * result in transforming the value to the "Encoded-Word" syntax (RFC 2047).
     * For example:
     *   In the header:  "Subject: =?iso-8859-1?Q?=A1Hola,_se=F1or!?="
     *   Interpreted as: "Subject: ¡Hola, señor!"
     *
     */
    template <typename HeaderEList, typename HeaderFieldType, Allocator AllocType>
    class request_headers : public stl::vector<HeaderFieldType, AllocType>,
                            public extension_wrapper<HeaderEList> {
        using super      = stl::vector<HeaderFieldType, AllocType>;
        using elist_type = extension_wrapper<HeaderEList>;

      public:
        using field_type = HeaderFieldType;

        using super::vector;

        // todo: fix this
        constexpr request_headers(istl::StringViewifiable auto&& header_string, auto&&... args)
          : super{stl::forward<decltype(args)>(args)...},
            elist_type{} {
            parse_header_string(istl::string_viewify(header_string));
        }
    };


    struct request_header_field_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_header_field_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = header_field_view<TraitsType, EList>;

    };



    template <Allocator AllocType>
    struct request_headers_descriptor {
        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::request_headers_extensions;

        template <typename ExtensionListType, typename TraitsType, typename EList>
        using mid_level_extensie_type = request_headers<
          EList,
          typename ExtensionListType::template extensie_type<TraitsType, request_header_field_descriptor>,
          AllocType>;

    };


    template <Traits TraitsType, typename EList, Allocator AllocType>
    using simple_request_headers =
      typename EList::template extensie_type<TraitsType, request_headers_descriptor<AllocType>>;

} // namespace webpp::http

#endif // WEBPP_REQUEST_HEADERS_HPP
