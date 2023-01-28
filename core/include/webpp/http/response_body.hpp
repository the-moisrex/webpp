#ifndef WEBPP_RESPONSE_BODY_HPP
#define WEBPP_RESPONSE_BODY_HPP

#include "../extensions/extension.hpp"
#include "../std/functional.hpp"
#include "../std/vector.hpp"
#include "../traits/enable_traits.hpp"
#include "http_concepts.hpp"

#include <algorithm>
#include <variant>

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
    using string_response_body_communicator = traits::general_string<TraitsType>;

    template <Traits TraitsType>
    using stream_response_body_communicator = stl::shared_ptr<
      stl::basic_iostream<traits::char_type<TraitsType>, stl::char_traits<traits::char_type<TraitsType>>>>;


    /**
     * BlobBasedBodyCommunicator + SizableBody (Even though we don't need to support SizableBody but can be
     * used to get a better performance)
     */
    template <Traits TraitsType>
    struct blob_response_body_communicator : istl::vector<stl::byte, TraitsType> {
        using traits_type = TraitsType;
        using byte_type   = stl::byte;

        using istl::vector<stl::byte, TraitsType>::vector; // ctors

        [[nodiscard]] stl::streamsize write(byte_type const* data, stl::streamsize count) {
            this->insert(this->begin(),
                         data,
                         data + count); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return count;
        }

        [[nodiscard]] stl::streamsize read(byte_type* data, stl::streamsize count) {
            stl::copy_n(this->begin(), count, data);
            return count;
        }
    };



    /**
     * @brief Response Body
     *
     * Implements: BlobBasedBodyCommunicator, StreamBasedBodyCommunicator, TextBasedBodyCommunicator
     */
    template <Traits TraitsType, typename EList>
    struct response_body : public enable_traits_with<TraitsType, EList> {
        using traits_type              = TraitsType;
        using elist_type               = enable_traits_with<TraitsType, EList>;
        using char_type                = traits::char_type<traits_type>;
        using string_communicator_type = string_response_body_communicator<traits_type>;
        using blob_communicator_type   = blob_response_body_communicator<traits_type>;
        using stream_communicator_type = stream_response_body_communicator<traits_type>;

        using byte_type = stl::byte; // required by BlobBasedBodyWriter
        using value_type =
          typename string_communicator_type::value_type; // required by the TextBasedBodyWriter

        // the order of types in this variant must match the order of http::communicator_type enum
        using communicator_storage_type = stl::
          variant<stl::monostate, string_communicator_type, blob_communicator_type, stream_communicator_type>;

        template <HTTPResponseBodyCommunicator NewBodyCommunicator>
        using rebind_body_communicator_type = response_body<traits_type, NewBodyCommunicator>;


        static_assert(TextBasedBodyCommunicator<string_communicator_type>,
                      "Response body Text Based Body Communicator is not a valid TBBC.");
        static_assert(StreamBasedBodyCommunicator<stream_communicator_type>,
                      "Response body Stream Based Body Communicator is not a valid SBBC.");
        static_assert(BlobBasedBodyCommunicator<blob_communicator_type>,
                      "Response body Blob Based Body Communicator is not a valid BBBC.");


      private:
        communicator_storage_type communicator{stl::monostate{}};

      public:
        constexpr response_body(response_body const&)                = default;
        constexpr response_body(response_body&&) noexcept            = default;
        constexpr response_body& operator=(response_body const&)     = default;
        constexpr response_body& operator=(response_body&&) noexcept = default;
        constexpr ~response_body() noexcept                          = default;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET>
            requires(!stl::same_as<stl::remove_cvref_t<ET>, response_body>)
        constexpr response_body(ET&& et) noexcept(stl::is_nothrow_constructible_v<elist_type, ET>)
          : elist_type{et} {}



        template <EnabledTraits ET, typename T>
            requires(stl::is_constructible_v<elist_type, ET>)
        constexpr response_body(ET&& et, T&& obj) : elist_type{et} {
            this->set<T>(stl::forward<T>(obj));
        }


        // NOLINTEND(bugprone-forwarding-reference-overload)


        // Get the data pointer if available, returns nullptr otherwise
        [[nodiscard]] constexpr char_type const* data() const noexcept {
            if constexpr (TextBasedBodyReader<elist_type>) {
                return elist_type::data();
            } else {
                if (auto const* reader = stl::get_if<string_communicator_type>(&communicator)) {
                    return reader->data();
                } else {
                    return nullptr;
                }
            }
        }

        // Get the size of the response body if possible. returns `npos` if it's not available
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            if constexpr (SizableBody<elist_type>) {
                return elist_type::size();
            } else {
                if (auto const* reader = stl::get_if<string_communicator_type>(&communicator)) {
                    return reader->size();
                } else {
                    if constexpr (SizableBody<blob_communicator_type>) {
                        if (auto const* blob_reader = stl::get_if<blob_communicator_type>(&communicator)) {
                            return blob_reader->size();
                        }
                    }
                    return string_communicator_type::npos;
                }
            }
        }

        constexpr void append(char_type const* data, stl::size_t count) {
            if constexpr (TextBasedBodyWriter<elist_type>) {
                elist_type::append(data, count);
            } else {
                if (auto* writer = stl::get_if<string_communicator_type>(&communicator)) {
                    writer->append(data, count);
                } else {
                    communicator.template emplace<string_communicator_type>();
                    auto& text_writer = stl::get<string_communicator_type>(communicator);
                    text_writer.append(data, count);
                }
            }
        }


        constexpr stl::streamsize write(byte_type const* data, stl::streamsize count) {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::write(data, count);
            } else {
                if (auto* writer = stl::get_if<blob_communicator_type>(&communicator)) {
                    return writer->write(data, count);
                } else {
                    communicator.template emplace<blob_communicator_type>();
                    auto& blob_writer = stl::get<blob_communicator_type>(communicator);
                    return blob_writer.write(data, count);
                }
            }
        }

        constexpr stl::streamsize read(byte_type const* data, stl::streamsize count) {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::read(data, count);
            } else {
                if (auto* reader = stl::get_if<blob_communicator_type>(&communicator)) {
                    return reader->read(data, count);
                } else {
                    return 0LL; // nothing is read because we can't read it
                }
            }
        }

        // This member function will tell you this body contains what
        [[nodiscard]] constexpr http::communicator_type witch_communicator() const noexcept {
            return static_cast<http::communicator_type>(communicator.index());
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
            using requested_type = stl::remove_cvref_t<T>;
            if constexpr (requires {
                              { deserialize_response_body<T>(*this) } -> stl::same_as<T>;
                          }) {
                return deserialize_response_body<T>(*this);
            } else if constexpr (requires {
                                     { deserialize_body<T>(*this) } -> stl::same_as<T>;
                                 }) {
                return deserialize_body<T>(*this);
            } else if constexpr (!stl::same_as<T, requested_type>) {
                return as<requested_type>();
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
            if constexpr (requires { elist_type::set(stl::forward<T>(obj)); }) {
                elist_type::set(stl::forward<T>(obj));
            } else if constexpr (requires { elist_type::operator=(stl::forward<T>(obj)); }) {
                elist_type::operator=(stl::forward<T>(obj));
            } else if constexpr (requires { serialize_response_body(stl::forward<T>(obj), *this); }) {
                serialize_response_body(stl::forward(obj), *this);
            } else if constexpr (requires { serialize_body(stl::forward<T>(obj), *this); }) {
                serialize_body(stl::forward<T>(obj), *this);
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
            set<T>(stl::forward<T>(obj));
            return *this;
        }


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
