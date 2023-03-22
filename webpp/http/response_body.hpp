#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "../extensions/extension.hpp"
#include "../std/functional.hpp"
#include "../std/vector.hpp"
#include "../traits/enable_traits.hpp"
#include "body.hpp"
#include "http_concepts.hpp"

#include <algorithm>
#include <variant>

namespace webpp::http {

    /**
     * @brief Response Body
     *
     * Implements: CStreamBasedBodyCommunicator, StreamBasedBodyCommunicator, TextBasedBodyCommunicator
     */
    template <Traits TraitsType, typename EList>
    struct response_body : public EList, public body_writer<TraitsType> {
        using traits_type               = TraitsType;
        using elist_type                = EList;
        using string_communicator_type  = string_response_body_communicator<traits_type>;
        using cstream_communicator_type = cstream_response_body_communicator<traits_type>;
        using stream_communicator_type  = stream_response_body_communicator<traits_type>;
        using stream_type               = typename stream_communicator_type::element_type;
        using body_communicator_type    = body_writer<TraitsType>;

        template <HTTPResponseBodyCommunicator NewBodyCommunicator>
        using rebind_body_communicator_type = response_body<traits_type, NewBodyCommunicator>;


        static constexpr auto log_cat = "ResBody";

      public:
        constexpr response_body(response_body const&)                = default;
        constexpr response_body(response_body&&) noexcept            = default;
        constexpr response_body& operator=(response_body const&)     = delete; // todo
        constexpr response_body& operator=(response_body&&) noexcept = default;
        constexpr ~response_body() noexcept                          = default;

        using body_communicator_type::operator=;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, response_body> &&
                     stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et) : elist_type{et},
                                           body_communicator_type{et.get_traits()} {}

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, response_body> &&
                     stl::is_default_constructible_v<elist_type> && !stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et) : elist_type{},
                                           body_communicator_type{et.get_traits()} {}



        template <EnabledTraits ET, typename T>
            requires(stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et, T&& obj) : elist_type{et},
                                                    body_communicator_type{et.get_traits()} {
            this->template set<T>(stl::forward<T>(obj));
        }

        template <EnabledTraits ET, typename T>
            requires(stl::is_default_constructible_v<elist_type> && !stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et, T&& obj) : elist_type{},
                                                    body_communicator_type{et.get_traits()} {
            this->template set<T>(stl::forward<T>(obj));
        }


        // NOLINTEND(bugprone-forwarding-reference-overload)


        /**
         * Get a new response body type which uses the specified communicator type passed in the template
         * params.
         */
        template <HTTPResponseBodyCommunicator CommunicatorType, typename T>
        constexpr auto rebind_body(T&& obj) const {
            using new_body_type = rebind_body_communicator_type<CommunicatorType>;
            if constexpr (EnabledTraits<response_body>) {
                new_body_type res{this->get_traits()};
                res = stl::forward<T>(obj);
                return res;
            } else {
                new_body_type res;
                res = stl::forward<T>(obj);
                return res;
            }
        }
    };


    struct response_body_descriptor {

        template <typename ExtensionType>
        using extractor_type = typename ExtensionType::response_body_extensions;

        template <typename RootExtensions, typename TraitsType, typename EList>
        using mid_level_extensie_type = response_body<TraitsType, EList>;
    };

    template <Traits TraitsType, Extension... E>
    using simple_response_body =
      typename extension_pack<E...>::template extensie_type<TraitsType, response_body_descriptor>;

} // namespace webpp::http

#endif // WEBPP_RESPONSE_BODY_HPP
