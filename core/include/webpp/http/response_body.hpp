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

    struct bad_cross_talk : stl::invalid_argument {
        using stl::invalid_argument::invalid_argument;
    };

    template <Traits TraitsType>
    struct callback_response_body_communicator {
        using traits_type   = TraitsType;
        using char_type     = traits::char_type<traits_type>;
        using function_type = istl::function<void()>; // Oops; no concepts allowed!

        // todo
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

        [[nodiscard]] stl::streamsize read(byte_type* data, stl::streamsize count) const {
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
        using stream_char_type = typename istl::remove_shared_ptr_t<stream_communicator_type>::char_type;
        using string_char_type = typename string_communicator_type::value_type;
        using blob_byte_type   = typename blob_communicator_type::byte_type;

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
                    // There's not cross-talk for this; maybe for blobs, but not for streams unless we're
                    // willing to convert the body communicator to string type which is a bad idiom to let the
                    // user support
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
                    // todo: see if you can get the size if the stream body supports but don't let it give false positives.
                    // todo: should we return npos?
                    // todo: should we return blob's size when we're not letting the user to read it through ".data()"?
                    // there's not cross-talky way of knowing the size for all stream types; (blobs can have
                    // but not required at this point, that's why I check if the blob communicator supports it
                    // or not)
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
                // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                if (auto* writer = stl::get_if<string_communicator_type>(&communicator)) {
                    writer->append(data, count);
                } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&communicator)) {
                    (*stream_writer)->write(data, static_cast<stl::streamsize>(count));
                } else if (auto* blob_writer = stl::get_if<blob_communicator_type>(&communicator)) {
                    auto*           byte_data = reinterpret_cast<blob_byte_type const*>(data);
                    auto            size      = static_cast<stl::streamsize>(count);
                    stl::streamsize ret_size; // NOLINT(cppcoreguidelines-init-variables)
                    do {
                        ret_size = blob_writer->write(byte_data, size);
                        byte_data += ret_size;
                        size -= ret_size;
                    } while (ret_size > 0);
                } else {
                    communicator.template emplace<string_communicator_type>();
                    auto& text_writer = stl::get<string_communicator_type>(communicator);
                    text_writer.append(data, count);
                }
                // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            }
        }

        constexpr void clear() {
            if constexpr (TextBasedBodyWriter<elist_type>) {
                elist_type::clear();
            } else {
                if (auto* string_writer = stl::get_if<string_communicator_type>(&communicator)) {
                    string_writer->clear();
                } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&communicator)) {
                    (*stream_writer)->clear(); // clear the state
                    (*stream_writer)
                      ->ignore(std::numeric_limits<std::streamsize>::max()); // ignore the data in the stream
                } else {
                    // todo: blob based doesn't have a way to clear the input
                    reset();
                }
            }
        }

        // Reset the body content
        constexpr void reset() {
            communicator.template emplace<stl::monostate>();
        }


        constexpr stl::streamsize write(byte_type const* data, stl::streamsize count) {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::write(data, count);
            } else {
                // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                if (auto* writer = stl::get_if<blob_communicator_type>(&communicator)) {
                    return writer->write(data, count);
                } else if (auto* string_writer = stl::get_if<string_communicator_type>(&communicator)) {
                    string_writer->append(reinterpret_cast<string_char_type const*>(data),
                                          static_cast<stl::size_t>(count));
                    return count;
                } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&communicator)) {
                    (*stream_writer)->write(reinterpret_cast<stream_char_type const*>(data), count);
                    return count;
                } else {
                    communicator.template emplace<blob_communicator_type>();
                    auto& blob_writer = stl::get<blob_communicator_type>(communicator);
                    return blob_writer.write(data, count);
                }
                // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            }
        }

        constexpr stl::streamsize read(byte_type* data, stl::streamsize count) const {
            if constexpr (BlobBasedBodyWriter<elist_type>) {
                return elist_type::read(data, count);
            } else {
                // Attention: cross-talks (writing to one communicator and reading from another) are
                // discouraged

                // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
                if (auto* reader = stl::get_if<blob_communicator_type>(&communicator)) {
                    return reader->read(data, count);
                } else if (auto* stream_reader = stl::get_if<stream_communicator_type>(&communicator)) {
                    // todo: this is kinda implementation defined, it may falsely return 0
                    return (*stream_reader)->readsome(reinterpret_cast<stream_char_type*>(data), count);
                } else if (auto* string_reader = stl::get_if<string_communicator_type>(&communicator)) {
                    auto* begin = reinterpret_cast<string_char_type*>(data);
                    return stl::copy_n(string_reader->data(), static_cast<stl::size_t>(count), begin) - begin;
                } else {
                    return 0LL; // nothing is read because we can't read it
                }
                // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
            }
        }

        constexpr decltype(auto) rdbuf() const {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&communicator)) {
                return (*stream_reader)->rdbuf();
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/blob-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        constexpr stl::streamsize readsome(stream_char_type* data, stl::streamsize count) {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&communicator)) {
                return (*stream_reader)->readsome(data, count);
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/blob-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        template <typename T>
        constexpr response_body& operator<<(T&& obj) {
            if (auto* stream_writer = stl::get_if<stream_communicator_type>(&communicator)) {
                **stream_writer << stl::forward<T>(obj);
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk("Bad Cross-Talk error (you previously wrote to a different body "
                                     "communicator, but now you're trying to write to stream based body "
                                     "communicator which doesn't know how to convert your object to "
                                     "text/blob-based-body communicators. Be consistent in your "
                                     "calls. Cross-Talks are discouraged.)");
            }
            return *this;
        }

        template <typename T>
        constexpr response_body const& operator>>(T& obj) const {
            if (auto* stream_writer = stl::get_if<stream_communicator_type>(&communicator)) {
                **stream_writer >> obj;
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/blob-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
            return *this;
        }

        // This member function will tell you this body contains what
        [[nodiscard]] constexpr http::communicator_type which_communicator() const noexcept {
            return static_cast<http::communicator_type>(communicator.index());
        }

        [[nodiscard]] constexpr bool operator==(response_body const& body) const noexcept {
            if constexpr (requires { elist_type::operator==(body); }) {
                return elist_type::operator==(body);
            } else {
                if (&body == this) {
                    return true;
                }
                auto this_communicator = which_communicator();
                if (this_communicator != body.which_communicator()) {
                    return false;
                }
                switch (this_communicator) {
                    case communicator_type::nothing: return true;
                    case communicator_type::text_based: {
                        const auto this_size = size();
                        return this_size == body.size() &&
                               stl::equal(data(), data() + this_size, body.data());
                    }
                    case communicator_type::stream_based: // we can't check equality of streams without
                                                          // changing them
                    case communicator_type::blob_based: {
                        return false; // blobs don't have a mechanism to read but don't modify, so always
                                      // false too
                    }
                }
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
        constexpr response_body& add(T&& obj) {
            if constexpr (requires { elist_type::add(stl::forward<T>(obj)); }) {
                elist_type::add(stl::forward<T>(obj));
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
        constexpr response_body& set(T&& obj) {
            clear();
            add(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr response_body& operator=(T&& obj) {
            if constexpr (requires { elist_type::operator=(stl::forward<T>(obj)); }) {
                elist_type::operator=(stl::forward<T>(obj));
            } else {
                set(stl::forward<T>(obj));
            }
            return *this;
        }

        template <typename T>
        constexpr response_body& operator+=(T&& obj) {
            if constexpr (requires { elist_type::operator+=(stl::forward<T>(obj)); }) {
                elist_type::operator+=(stl::forward<T>(obj));
            } else {
                add(stl::forward<T>(obj));
            }
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
