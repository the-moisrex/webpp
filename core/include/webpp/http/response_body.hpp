#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "../extensions/extension.hpp"
#include "../std/functional.hpp"
#include "../std/variant.hpp"
#include "http_concepts.hpp"

namespace webpp::http {


    template <Traits TraitsType>
    struct callback_response_body_communicator {
        using traits_type   = TraitsType;
        using char_type     = traits::char_type<traits_type>;
        using function_type = istl::function<void()>; // Oops; no concepts allowed!

      private:
      public:
    };

    template <Traits TraitsType>
    struct string_response_body_communicator : traits::general_string<TraitsType> {
        using traits_type = TraitsType;
    };

    template <Traits TraitsType>
    struct stream_response_body_communicator
      : stl::basic_iostream<traits::char_type<TraitsType>, stl::char_traits<traits::char_type<TraitsType>>> {
        using traits_type = TraitsType;
        using stream_type = stl::basic_iostream<traits::char_type<traits_type>,
                                                stl::char_traits<traits::char_type<traits_type>>>;
        // todo: stl::char_traits can be contained in the traits type
    };


    template <Traits TraitsType>
    struct blob_response_body_communicator {
        using traits_type = TraitsType;
        // todo
    };



    /**
     * @brief Response Body
     */
    template <Traits TraitsType, typename EList>
    struct response_body : public EList {
        using traits_type              = TraitsType;
        using elist_type               = EList;
        using char_type                = traits::char_type<traits_type>;
        using string_communicator_type = string_response_body_communicator<traits_type>;
        using stream_communicator_type = stream_response_body_communicator<traits_type>;
        using blob_communicator_type   = blob_response_body_communicator<traits_type>;

        using communicator_storage_type = stl::
          variant<stl::monostate, string_communicator_type, stream_communicator_type, blob_communicator_type>;

        template <HTTPResponseBodyCommunicator NewBodyCommunicator>
        using rebind_body_communicator_type = response_body<traits_type, NewBodyCommunicator>;


      private:
        communicator_storage_type communicator{stl::monostate{}};

      public:
        using EList::EList;

        constexpr response_body()
            requires(stl::is_default_constructible_v<elist_type>)
        = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et) noexcept(stl::is_nothrow_constructible_v<elist_type, ET>)
          : elist_type{et} {}


        template <EnabledTraits ET>
        constexpr response_body([[maybe_unused]] ET&&) noexcept(
          stl::is_nothrow_default_constructible_v<elist_type>)
          : elist_type{} {}

        // NOLINTEND(bugprone-forwarding-reference-overload)


        // Get the data pointer if available, returns nullptr otherwise
        [[nodiscard]] constexpr char_type const* data() const noexcept {
            if constexpr (TextBasedBodyReader<elist_type>) {
                return elist_type::data();
            } else {
                if (auto const* reader = stl::get_if<string_communicator_type>(communicator)) {
                    return reader->data();
                } else {
                    return nullptr;
                }
            }
        }

        // Get the size of the response body if possible. returns `npos` if it's not available
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            if constexpr (TextBasedBodyReader<elist_type>) {
                return elist_type::size();
            } else {
                if (auto const* reader = stl::get_if<string_communicator_type>(communicator)) {
                    return reader->size();
                } else {
                    return string_communicator_type::npos;
                }
            }
        }

        constexpr stl::streamsize write(char_type const* data, stl::streamsize count) {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::write(data, count);
            } else {
                if (auto* writer = stl::get_if<blob_communicator_type>(communicator)) {
                    return writer->write(data, count);
                } else {
                    communicator.template emplace<blob_communicator_type>();
                    auto& blob_writer = stl::get<blob_communicator_type>(communicator);
                    return blob_writer.write(data, count);
                }
            }
        }

        constexpr stl::streamsize read(char_type const* data, stl::streamsize count) {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::read(data, count);
            } else {
                if (auto* reader = stl::get_if<blob_communicator_type>(communicator)) {
                    return reader->read(data, count);
                } else {
                    return 0ll; // nothing is read because we can't read it
                }
            }
        }

        [[nodiscard]] constexpr bool operator==(response_body const& body) const noexcept {
            if constexpr (requires { elist_type::operator==(body); }) {
                return elist_type::operator==(body);
            } else if constexpr (TextBasedBodyReader<elist_type>) {
                const auto this_size = size();
                return this_size == body.size() && stl::equal(data(), data() + this_size, body.data());
            } else {
                // todo
            }
        }


        [[nodiscard]] constexpr bool operator!=(response_body const& body) const noexcept {
            return !operator==(body);
        }


        template <typename T>
        constexpr T as() const {
            if constexpr (requires {
                              { deserialize_response_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_response_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the request body to the specified type."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
        }

        constexpr auto as() const {
            return auto_converter<response_body>{.obj = *this};
        }


        template <typename T>
        constexpr response_body& set(T&& obj) {
            if constexpr (requires { elist_type::template set<T>(stl::forward<T>(obj)); }) {
                elist_type::template set<T>(stl::forward<T>(obj));
            } else if constexpr (requires { elist_type::template operator=<T>(stl::forward<T>(obj)); }) {
                elist_type::template operator=<T>(stl::forward<T>(obj));
            } else if constexpr (requires { serialize_response_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_response_body<T>(stl::forward<T>(obj), *this);
            } else if constexpr (requires { serialize_body<T>(stl::forward<T>(obj), *this); }) {
                serialize_body<T>(stl::forward<T>(obj), *this);
            } else {
                static_assert_false(T,
                                    "We don't know how to convert the specified object to a response body."
                                    " Did you import the right header?"
                                    " You can always write your own custom body (de)serializer functions.");
            }
            return *this;
        }

        template <typename T>
        constexpr response_body& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }


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
