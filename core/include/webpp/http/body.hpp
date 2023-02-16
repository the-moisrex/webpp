// Created by moisrex on 2/16/23.

#ifndef WEBPP_HTTP_BODY_HPP
#define WEBPP_HTTP_BODY_HPP

#include "../std/string_concepts.hpp"
#include "../std/type_traits.hpp"
#include "../std/vector.hpp"
#include "../traits/traits.hpp"

#include <exception>

// This file is for common types for bodies; request and response bodies alike
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

} // namespace webpp::http

#endif // WEBPP_HTTP_BODY_HPP
