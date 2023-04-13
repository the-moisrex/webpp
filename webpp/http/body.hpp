// Created by moisrex on 2/16/23.

#ifndef WEBPP_HTTP_BODY_HPP
#define WEBPP_HTTP_BODY_HPP

#include "../common/meta.hpp"
#include "../std/concepts.hpp"
#include "../std/string_concepts.hpp"
#include "../std/type_traits.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"
#include "./http_concepts.hpp"

#include <exception>
#include <variant>

// This file is for common types for bodies; request and response bodies alike
namespace webpp::http {


    struct bad_cross_talk : stl::invalid_argument {
        using stl::invalid_argument::invalid_argument;
    };

    template <Traits TraitsType>
    struct callback_response_body_communicator {
        using traits_type = TraitsType;
        using char_type   = traits::char_type<traits_type>;
        // using function_type = istl::function<void()>; // Oops; no concepts allowed!

        // todo
      private:
      public:
    };

    template <Traits TraitsType>
    using string_response_body_communicator = traits::general_string<TraitsType>;

    template <Traits TraitsType>
    using stream_response_body_communicator = stl::shared_ptr<
      stl::basic_stringstream<traits::char_type<TraitsType>,
                              stl::char_traits<traits::char_type<TraitsType>>,
                              traits::general_allocator<TraitsType, traits::char_type<TraitsType>>>>;


    /**
     * CStreamBasedBodyCommunicator + SizableBody (Even though we don't need to support SizableBody but can be
     * used to get a better performance)
     */
    template <Traits TraitsType>
    struct cstream_response_body_communicator : istl::vector<stl::byte, TraitsType> {
        using traits_type     = TraitsType;
        using byte_type       = stl::byte;
        using vector_type     = istl::vector<stl::byte, traits_type>;
        using iterator        = typename vector_type::iterator;
        using difference_type = typename stl::iterator_traits<iterator>::difference_type;

        using istl::vector<stl::byte, TraitsType>::vector; // ctors


      private:
        mutable stl::size_t index = 0;

      public:
        [[nodiscard]] constexpr stl::streamsize write(byte_type const* data, stl::streamsize count) {
            this->insert(this->begin(),
                         data,
                         data + count); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            return count;
        }

        [[nodiscard]] constexpr stl::streamsize read(byte_type* data, stl::streamsize count) const {
            count =
              stl::clamp(count, stl::streamsize{0LL}, static_cast<stl::streamsize>(this->size() - index));
            stl::copy_n(this->begin() + static_cast<difference_type>(index), count, data);
            index += static_cast<stl::size_t>(count);
            return count;
        }

        constexpr void seek(stl::streamsize count) noexcept {
            index = stl::clamp(static_cast<stl::size_t>(count), stl::size_t{0ul}, this->size());
        }
    };


    /**
     * This is the dynamic parent for body readers and body writers.
     */
    template <Traits TraitsType>
    struct body_communicator : enable_traits<TraitsType> {
        using traits_type               = TraitsType;
        using char_type                 = traits::char_type<traits_type>;
        using string_communicator_type  = string_response_body_communicator<traits_type>;
        using cstream_communicator_type = cstream_response_body_communicator<traits_type>;
        using stream_communicator_type  = stream_response_body_communicator<traits_type>;
        using stream_type               = typename stream_communicator_type::element_type;

        using byte_type = stl::byte; // required by CStreamBasedBodyWriter
        using value_type =
          typename string_communicator_type::value_type; // required by the TextBasedBodyWriter

        // the order of types in this variant must match the order of http::communicator_type enum
        using communicator_storage_type = stl::variant<stl::monostate,
                                                       string_communicator_type,
                                                       cstream_communicator_type,
                                                       stream_communicator_type>;


        static_assert(TextBasedBodyCommunicator<string_communicator_type>,
                      "Response body Text Based Body Communicator is not a valid TBBC.");
        static_assert(StreamBasedBodyCommunicator<stream_communicator_type>,
                      "Response body Stream Based Body Communicator is not a valid SBBC.");
        static_assert(CStreamBasedBodyCommunicator<cstream_communicator_type>,
                      "Response body CStream Based Body Communicator is not a valid BBBC.");

        static constexpr auto log_cat = "Body";

      private:
        using stream_char_type  = typename istl::remove_shared_ptr_t<stream_communicator_type>::char_type;
        using string_char_type  = typename string_communicator_type::value_type;
        using cstream_byte_type = typename cstream_communicator_type::byte_type;

        communicator_storage_type communicator_var{stl::monostate{}};

      public:
        using enable_traits<TraitsType>::enable_traits;

        // NOLINTBEGIN(bugprone-forwarding-reference-overload)
        template <EnabledTraits ET>
            requires(!istl::cvref_as<ET, body_communicator>)
        constexpr body_communicator(ET&& et) : enable_traits<TraitsType>(et) {}
        // NOLINTEND(bugprone-forwarding-reference-overload)

        template <EnabledTraits ET, typename ComT>
            requires(istl::part_of<stl::remove_cvref_t<ComT>,
                                   string_communicator_type,
                                   stream_communicator_type,
                                   cstream_communicator_type>)
        constexpr body_communicator(ET&& et, ComT&& inp_communicator)
          : enable_traits<TraitsType>(et),
            communicator_var{stl::forward<ComT>(inp_communicator)} {}

        constexpr body_communicator(body_communicator const&)                = default;
        constexpr body_communicator(body_communicator&&) noexcept            = default;
        constexpr body_communicator& operator=(body_communicator const&)     = default;
        constexpr body_communicator& operator=(body_communicator&&) noexcept = default;
        constexpr ~body_communicator() noexcept                              = default;

        [[nodiscard]] communicator_storage_type& communicator() noexcept {
            return communicator_var;
        }

        [[nodiscard]] communicator_storage_type const& communicator() const noexcept {
            return communicator_var;
        }


        // This member function will tell you this body contains what
        [[nodiscard]] constexpr http::communicator_type which_communicator() const noexcept {
            return static_cast<http::communicator_type>(communicator().index());
        }
    };


    template <Traits TraitsType>
    struct body_reader : body_communicator<TraitsType> {
        using traits_type               = TraitsType;
        using char_type                 = traits::char_type<traits_type>;
        using string_communicator_type  = string_response_body_communicator<traits_type>;
        using cstream_communicator_type = cstream_response_body_communicator<traits_type>;
        using stream_communicator_type  = stream_response_body_communicator<traits_type>;
        using stream_type               = typename stream_communicator_type::element_type;

        using stream_char_type  = typename istl::remove_shared_ptr_t<stream_communicator_type>::char_type;
        using string_char_type  = typename string_communicator_type::value_type;
        using cstream_byte_type = typename cstream_communicator_type::byte_type;

        using byte_type = stl::byte; // required by CStreamBasedBodyReader

        static constexpr auto log_cat = "BodyReader";

        using body_communicator<TraitsType>::body_communicator;
        constexpr body_reader(body_reader const& other)
          : body_communicator<TraitsType>{other.get_traits(), other.as_string_communicator()} {}
        constexpr body_reader(body_reader&&) noexcept = default;
        constexpr body_reader& operator=(body_reader const& other) {
            if (this != &other) {
                this->communicator().template emplace<string_communicator_type>(
                  other.as_string_communicator());
            }
            return *this;
        }
        constexpr body_reader& operator=(body_reader&&) noexcept = default;
        constexpr ~body_reader() noexcept                        = default;


        // Get the data pointer if available, returns nullptr otherwise
        [[nodiscard]] constexpr char_type const* data() const noexcept {
            if (auto const* reader = stl::get_if<string_communicator_type>(&this->communicator())) {
                return reader->data();
            } else {
                // There's not cross-talk for this; maybe for c-streams, but not for streams unless we're
                // willing to convert the body communicator to string type which is a bad idiom to let the
                // user support
                return nullptr;
            }
        }

        // Get the size of the response body if possible. returns `npos` if it's not available
        [[nodiscard]] constexpr stl::size_t size() const noexcept {
            if (auto const* reader = stl::get_if<string_communicator_type>(&this->communicator())) {
                return reader->size();
            } else if (stl::holds_alternative<stl::monostate>(this->communicator())) {
                return 0;
            } else {
                // todo: see if you can get the size if the stream body supports but don't let it give false positives.
                // todo: should we return npos?
                // todo: should we return c-stream's size when we're not letting the user to read it through ".data()"?
                // there's not cross-talky way of knowing the size for all stream types; (c-streams can
                // have but not required at this point, that's why I check if the c-stream communicator
                // supports it or not)
                if constexpr (SizableBody<cstream_communicator_type>) {
                    if (auto const* cstream_reader =
                          stl::get_if<cstream_communicator_type>(&this->communicator())) {
                        return cstream_reader->size();
                    }
                }
                return string_communicator_type::npos;
            }
        }

        [[nodiscard]] constexpr bool empty() const noexcept {
            if (auto const* str_reader = stl::get_if<string_communicator_type>(&this->communicator())) {
                return str_reader->empty();
            } else if (auto const* stream_reader =
                         stl::get_if<stream_communicator_type>(&this->communicator())) {
                return (*stream_reader)->eof();
            } else if (auto* cstr_reader = stl::get_if<cstream_communicator_type>(&this->communicator())) {
                return cstr_reader->empty();
            } else {
                return true;
            }
        }

        [[nodiscard]] constexpr bool eof() const noexcept {
            return empty();
        }


        constexpr stl::streamsize read(byte_type* data, stl::streamsize count) const {
            // Attention: cross-talks (writing to one communicator and reading from another) are
            // discouraged

            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            if (auto* reader = stl::get_if<cstream_communicator_type>(&this->communicator())) {
                return reader->read(data, count);
            } else if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                // this->logger.warning(log_cat, "Stream to CStream Cross-Talk is discouraged.");
                // todo: this is kinda implementation defined, it may falsely return 0
                return (*stream_reader)->readsome(reinterpret_cast<stream_char_type*>(data), count);
            } else if (auto* string_reader = stl::get_if<string_communicator_type>(&this->communicator())) {
                // this->logger.warning(log_cat, "Text to CStream Cross-Talk is discouraged.");
                auto* begin = reinterpret_cast<string_char_type*>(data);
                stl::copy_n(string_reader->data(), static_cast<stl::size_t>(count), begin);
                return 0; // return 0 to skip the loop
            } else {
                return 0LL; // nothing is read because we can't read it
            }
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }



        constexpr decltype(auto) rdbuf() const {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                return (*stream_reader)->rdbuf();
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        constexpr stl::streamsize readsome(stream_char_type* data, stl::streamsize count) {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                return (*stream_reader)->readsome(data, count);
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }


        constexpr typename stream_type::pos_type tellg() {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                return (*stream_reader)->tellg();
            } else {
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        constexpr body_reader& seekg(typename stream_type::pos_type pos) {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                (*stream_reader)->seekg(pos);
                return *this;
            } else {
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        constexpr body_reader& seekg(typename stream_type::off_type off, stl::ios_base::seekdir dir) {
            if (auto* stream_reader = stl::get_if<stream_communicator_type>(&this->communicator())) {
                (*stream_reader)->seekg(off, dir);
                return *this;
            } else {
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
        }

        template <typename T>
        constexpr body_reader const& operator>>(T& obj) const {
            if (auto* stream_writer = stl::get_if<stream_communicator_type>(&this->communicator())) {
                **stream_writer >> obj;
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk(
                  "Bad Cross-Talk error (you previously wrote to a different body "
                  "communicator, but now you're trying to read from a stream based body "
                  "communicator which doesn't know how to convert the text/cstream-based-body "
                  "communicators to your object type. Be consistent in your "
                  "calls. Cross-Talks are discouraged.)");
            }
            return *this;
        }


        template <typename T>
        // requires(HTTPDeserializableBody<T, body_reader>)
        constexpr T as() {
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

        template <typename T>
            requires(HTTPDeserializableBody<T, body_reader>)
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

        [[nodiscard]] constexpr auto_converter<body_reader> as() const noexcept {
            return {*this};
        }

        [[nodiscard]] constexpr auto_converter<body_reader> as() noexcept {
            return {*this};
        }

        [[nodiscard]] constexpr string_communicator_type as_string_communicator() const {
            return as_string();
        }

        [[nodiscard]] constexpr string_communicator_type as_string_communicator() {
            return as_string();
        }

        [[nodiscard]] constexpr traits::general_string<traits_type> as_string() const {
            return as<traits::general_string<traits_type>>();
        }

        [[nodiscard]] constexpr traits::general_string<traits_type> as_string() {
            return as<traits::general_string<traits_type>>();
        }


        [[nodiscard]] constexpr bool operator==(body_reader const& body) const noexcept {
            if (&body == this) {
                return true;
            }
            auto const this_communicator = this->which_communicator();
            if (this_communicator != body.which_communicator()) {
                return false;
            }
            switch (this_communicator) {
                case communicator_type::nothing: return true;
                case communicator_type::text_based: {
                    const auto this_size = size();
                    return this_size == body.size() && stl::equal(data(), data() + this_size, body.data());
                }
                case communicator_type::stream_based: // we can't check equality of streams without
                                                      // changing them
                case communicator_type::cstream_based: {
                    return false; // c-streams don't have a mechanism to read but don't modify, so always
                    // false too
                }
            }
            return false;
        }


        [[nodiscard]] constexpr bool operator!=(body_reader const& body) const noexcept {
            return !operator==(body);
        }
    };


    template <Traits TraitsType>
    struct body_writer : body_reader<TraitsType> {
        using traits_type               = TraitsType;
        using char_type                 = traits::char_type<traits_type>;
        using string_communicator_type  = string_response_body_communicator<traits_type>;
        using cstream_communicator_type = cstream_response_body_communicator<traits_type>;
        using stream_communicator_type  = stream_response_body_communicator<traits_type>;
        using stream_type               = typename stream_communicator_type::element_type;

        using stream_char_type  = typename istl::remove_shared_ptr_t<stream_communicator_type>::char_type;
        using string_char_type  = typename string_communicator_type::value_type;
        using cstream_byte_type = typename cstream_communicator_type::byte_type;

        using byte_type = stl::byte; // required by CStreamBasedBodyWriter
        using value_type =
          typename string_communicator_type::value_type; // required by the TextBasedBodyWriter

        static constexpr auto log_cat = "BodyWriter";

        using body_reader<TraitsType>::body_reader;
        constexpr body_writer(body_writer const&)                = default;
        constexpr body_writer(body_writer&&) noexcept            = default;
        constexpr body_writer& operator=(body_writer const&)     = default;
        constexpr body_writer& operator=(body_writer&&) noexcept = default;
        constexpr ~body_writer() noexcept                        = default;


        constexpr void append(char_type const* data, stl::size_t count) {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            if (auto* writer = stl::get_if<string_communicator_type>(&this->communicator())) {
                writer->append(data, count);
            } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&this->communicator())) {
                (*stream_writer)->write(data, static_cast<stl::streamsize>(count));
            } else if (auto* cstream_writer = stl::get_if<cstream_communicator_type>(&this->communicator())) {
                auto*           byte_data = reinterpret_cast<cstream_byte_type const*>(data);
                auto            size      = static_cast<stl::streamsize>(count);
                stl::streamsize ret_size; // NOLINT(cppcoreguidelines-init-variables)
                do {
                    ret_size = cstream_writer->write(byte_data, size);
                    byte_data += ret_size;
                    size -= ret_size;
                } while (ret_size > 0);
            } else {
                this->communicator().template emplace<string_communicator_type>(
                  alloc::general_alloc_for<string_communicator_type>(*this));
                auto& text_writer = stl::get<string_communicator_type>(this->communicator());
                text_writer.append(data, count);
            }
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }


        constexpr void clear() {
            if (auto* string_writer = stl::get_if<string_communicator_type>(&this->communicator())) {
                string_writer->clear();
            } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&this->communicator())) {
                (*stream_writer)->clear(); // clear the state
                (*stream_writer)
                  ->ignore(std::numeric_limits<std::streamsize>::max()); // ignore the data in the stream
            } else if (auto* cstream_writer = stl::get_if<cstream_communicator_type>(&this->communicator())) {
                cstream_writer->clear();
            } else {
                // todo: c-stream based doesn't have a way to clear the input
                reset();
            }
        }

        // Reset the body content
        constexpr void reset() {
            this->communicator().template emplace<stl::monostate>();
        }


        constexpr stl::streamsize write(byte_type const* data, stl::streamsize count) {
            // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast)
            if (auto* writer = stl::get_if<cstream_communicator_type>(&this->communicator())) {
                return writer->write(data, count);
            } else if (auto* string_writer = stl::get_if<string_communicator_type>(&this->communicator())) {
                string_writer->append(reinterpret_cast<string_char_type const*>(data),
                                      static_cast<stl::size_t>(count));
                return count;
            } else if (auto* stream_writer = stl::get_if<stream_communicator_type>(&this->communicator())) {
                (*stream_writer)->write(reinterpret_cast<stream_char_type const*>(data), count);
                return count;
            } else {
                this->communicator().template emplace<cstream_communicator_type>(
                  alloc::general_alloc_for<cstream_communicator_type>(*this));
                auto& cstream_writer = stl::get<cstream_communicator_type>(this->communicator());
                return cstream_writer.write(data, count);
            }
            // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast)
        }

        template <typename T>
        constexpr body_writer& operator<<(T&& obj) {
            if (auto* stream_writer = stl::get_if<stream_communicator_type>(&this->communicator())) {
                **stream_writer << stl::forward<T>(obj);
            } else if (stl::holds_alternative<stl::monostate>(this->communicator())) {
                init_stream();
                *stl::get<stream_communicator_type>(this->communicator()) << stl::forward<T>(obj);
            } else {
                // todo: should we log, or should we blow up with an exception?
                throw bad_cross_talk("Bad Cross-Talk error (you previously wrote to a different body "
                                     "communicator, but now you're trying to write to stream based body "
                                     "communicator which doesn't know how to convert your object to "
                                     "text/cstream-based-body communicators. Be consistent in your "
                                     "calls. Cross-Talks are discouraged.)");
            }
            return *this;
        }

        template <typename T>
        constexpr body_writer& add(T&& obj) {
            if constexpr (requires { serialize_response_body(stl::forward<T>(obj), *this); }) {
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
        constexpr body_writer& set(T&& obj) {
            clear();
            if constexpr (BodyReader<T> && requires {
                                               {
                                                   obj.as_string_communicator()
                                                   } -> stl::same_as<string_communicator_type>;
                                           }) {
                this->communicator().template emplace<string_communicator_type>(obj.as_string_communicator());
            } else if constexpr (stl::constructible_from<string_communicator_type, T>) {
                this->communicator().template emplace<string_communicator_type>(stl::forward<T>(obj));
            } else if constexpr (stl::constructible_from<stream_communicator_type, T>) {
                this->communicator().template emplace<stream_communicator_type>(stl::forward<T>(obj));
            } else if constexpr (stl::constructible_from<cstream_communicator_type, T>) {
                this->communicator().template emplace<cstream_communicator_type>(stl::forward<T>(obj));
            } else {
                add(stl::forward<T>(obj));
            }
            return *this;
        }

        template <typename T>
        constexpr body_writer& operator=(T&& obj) {
            set(stl::forward<T>(obj));
            return *this;
        }

        template <typename T>
        constexpr body_writer& operator+=(T&& obj) {
            add(stl::forward<T>(obj));
            return *this;
        }

      private:
        void init_stream() {
            this->communicator().template emplace<stream_communicator_type>(
              stl::allocate_shared<stream_type>(alloc::general_allocator<stream_type>(*this),
                                                std::ios_base::in | std::ios_base::out));
        }
    };

} // namespace webpp::http

#endif // WEBPP_HTTP_BODY_HPP
