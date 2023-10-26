#include "../../webpp/std/string.hpp"
#include "../../webpp/std/string_view.hpp"
#include "../../webpp/strings/charset.hpp"
#include "../../webpp/strings/hex.hpp"
#include "../benchmark.hpp"

#include <vector>


constexpr std::string_view encoded_str = "%D9%84%D8%A8%D9%86%D9%85%DB%8C%D8%B3%D8%AA%D9%84%D8%A8%D9%86%D9"
                                         "%85%DB%8C%D8%B3%D8%AA%D9%84%D8%A7%D8%A8%DB%8C%D9%86%D9%85%D8%B3"
                                         "%D8%AA%D9%84%D8%A7%D8%A8%DB%8C%DA%A9%D8%B3%D8%AA%D9%84%D8%A8%DA"
                                         "%A9%D9%85%DB%8C%D8%A8%D9%86%D8%AA%DA%A9%D8%AB%D8%B6%D9%85%D9%87"
                                         "%D8%A8%DB%8C%DA%A9%D9%85%D8%B3%D9%84%D8%AF%D8%A8%DA%A9%DB%8C%D9"
                                         "%85%D8%B3%D9%84%DA%A9%D8%AF%D9%82%D8%AB%D8%B4%D8%B9%D9%81%D9%84"
                                         "%DA%A9%D9%82%D9%85%DB%8C%D8%B3%D9%87%D8%B9%D9%84%DA%A9%D8%A8%D9"
                                         "%85%DB%8C%D9%87%D8%B3%D8%B9%D9%84%DA%A9%D9%85%D8%A8%DB%8C%D9%87"
                                         "%D8%B3%D8%B9%D9%84%DA%A9%D8%AF%D8%A8%D9%87%DB%8C%D8%B3%D8%B9%DA"
                                         "%A9%D9%84%D8%AF%D9%85%D8%A8%D9%87%DB%8C%D8%B3%D8%B9%DA%A9%D9%84"
                                         "%D9%85%D8%A8%D9%87%DB%8C%D8%B3%DA%A9%D8%AF%D9%85%D9%84%D8%A8%D9"
                                         "%87%D8%B9%DB%8C%D8%B3%DA%A9%D9%85%D9%84%D9%87%D8%B9%D8%A8%DB%8C"
                                         "%D8%B3%DA%A9%D9%85%D9%84%D9%87%D8%B9%D8%A8%D8%AF%DA%A9%DB%8C%D9"
                                         "%85%D8%B3%D9%87%D9%84%D8%B9%D8%AF%D8%A8%DA%A9%D9%85%DB%8C%D8%B3"
                                         "%D9%87%D8%B9%D9%84%DA%A9%D8%A8%DB%8C%D9%85%D8%B3%D9%87%D8%B9%D9"
                                         "%84%DA%A9%D8%A8%D9%85%DB%8C%D8%B3%D9%87%D8%B9%D9%84%DA%A9%D8%A8"
                                         "%D9%85%DB%8C%D8%B3%D9%87%D8%B9%D9%84%D8%A8%DB%8C%DA%A9%D9%85%D8"
                                         "%B3%D9%87%D8%B9%D8%AF%D9%84%D8%A8%DA%A9%D9%85%DB%8C%D8%B3%D9%87"
                                         "%D8%B9%D9%84%D8%A8%DA%A9%D9%85%DB%8C%D8%B3%D9%87%D8%B9%D9%84%DA"
                                         "%A9%D8%A8%DB%8C%D9%85%D8%B3%D9%87%D8%B9%D9%84%D8%A8%D8%AF%DA%A9"
                                         "%D9%85%DB%8C%D8%B3%D9%87%D8%AF%D8%B9%D8%A8%D9%84%D8%AF%DA%A9%D9"
                                         "%85%DB%8C%D8%B3%D8%B9%D8%AF%D9%84%D8%A8%DA%A9%D9%85%DB%8C%D8%B3"
                                         "%D9%87%D8%B9%D9%84%D8%AF%D8%A8%DB%8C%DA%A9%D8%B3%D9%85%D9%87%D8"
                                         "%B9%D9%84%D8%A8%DB%8C%DA%A9%D9%85%D8%B3%D9%87%D8%B9%D9%84%DA%A9"
                                         "%D8%AF%D8%AB%D8%AE%D8%B5%D9%87%D8%B9%D9%84%D8%AE%D8%AB%DA%A9%D8"
                                         "%B5%D8%B9%D9%84%DA%A9%D9%82%D8%AB%D9%87%20%D8%AE%D8%AB%D8%B5%D9"
                                         "%87%D9%84%DA%A9%D9%85%D9%82%D8%AB%D9%87%D8%B5%D8%B9%20%D8%B0%D8"
                                         "%AF%D9%84%DA%A9%D9%82%D8%AB%D8%B5%D8%B9%20%D8%AF%D9%84%DA%A9%D9"
                                         "%82%D9%85%D8%AB%D9%87%D8%B9%D8%B5%20%D8%AF%D9%84%DA%A9%D9%82%D8"
                                         "%AB%D9%87%D8%B9%D8%B5%20%D8%AF%DA%A9%D9%84%D9%85%D8%A8%DB%8C%D8"
                                         "%B3%D8%AA%D9%84%DA%A9%D8%A8%D9%87%DB%8C%D8%B3%20%D8%AF%DA%A9%D9"
                                         "%84%D8%A8%D9%87%DB%8C%D8%B3%D8%B9%20%D9%84%DA%A9%D9%85%D8%A8%DB"
                                         "%8C%D9%87%D8%B3%D8%B9%20%DA%A9%D9%84%D9%85%D8%A8%DB%8C%D8%B3%D9"
                                         "%87%DA%A9";


namespace v1 {
    using namespace webpp;

    enum struct uri_encoding_policy { allowed_chars, disallowed_chars };



    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              typename Iter              = char*,
              typename ConstIter         = char const*>
    [[nodiscard]] bool
    decode_uri_component_inplace(Iter& pos, ConstIter end, CharSet auto const& chars) noexcept {
        using char_type = istl::char_type_of_t<Iter>;

        static_assert(stl::same_as<char_type, istl::char_type_of_t<ConstIter>>,
                      "The beginning and the end of the same string are required.");

        stl::size_t digits_left  = 0;
        char_type   decoded_char = 0;
        char_type*  out          = pos;
        for (; pos != end; ++pos) {
            if (digits_left) {
                decoded_char <<= 4;
                if (*pos >= '0' && *pos <= '9') { // DIGITS
                    decoded_char += static_cast<char_type>(*pos - '0');
                } else if (*pos >= 'A' && *pos <= 'F') {                     // UPPER_HEX
                    decoded_char += static_cast<char_type>(*pos - 'A' + 10); // NOLINT(*-avoid-magic-numbers)
                } else if (*pos >= 'a' && *pos <= 'f') {                     // LOWER_HEX
                    decoded_char += static_cast<char_type>(*pos - 'a' + 10); // NOLINT(*-avoid-magic-numbers)
                } else {
                    pos  = out;
                    *pos = '\0';
                    return false;
                }
                --digits_left;

                if (digits_left == 0) {
                    *out++ = decoded_char;
                }
            } else if (*pos == '%') {
                // resetting:
                digits_left  = 2;
                decoded_char = 0;
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                }
                *out++ += *pos;
            }
        }
        pos = out;
        if (pos != end)
            *pos = '\0';
        return true;
    }

} // namespace v1

namespace v2 {
    using namespace webpp;
    enum struct uri_encoding_policy { allowed_chars, disallowed_chars };


    /**
     * in-place version of uri component decoding, this is also nothrow since encoded version is always
     * longer than or equal to the decoded version thus we don't need allocations.
     */
    template <uri_encoding_policy Policy = uri_encoding_policy::allowed_chars,
              typename Iter              = char*,
              typename ConstIter         = char const*>
    [[nodiscard]] static constexpr bool
    decode_uri_component_inplace(Iter& pos, ConstIter end, CharSet auto const& chars) noexcept {
        using char_type = istl::char_type_of_t<Iter>;

        static_assert(stl::same_as<char_type, istl::char_type_of_t<ConstIter>>,
                      "The beginning and the end of the same string are required.");

        char_type* out = pos;
        for (; pos != end; ++pos) {
            if (*pos == '%') {
                if (pos++ >= end - 2) [[unlikely]] {
                    return false;
                }

                int decoded_char = ascii::hex_digit_value<int>(*pos++, -1) << 4u;
                decoded_char |= ascii::hex_digit_value<int>(*pos, -1);

                if ((decoded_char & ~0xFF) == 0) [[likely]] { // NOLINT(*-avoid-magic-numbers)
                    *out++ = static_cast<char_type>(decoded_char);
                } else {
                    pos  = out;
                    *pos = '\0';
                    return false;
                }
            } else {
                if constexpr (uri_encoding_policy::allowed_chars == Policy) {
                    if (!chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                } else {
                    if (chars.contains(*pos)) {
                        pos  = out;
                        *pos = '\0';
                        return false; // bad chars
                    }
                }
                *out++ += *pos;
            }
        }
        pos = out;
        if (pos != end)
            *pos = '\0';
        return true;
    }


} // namespace v2

static void URIDecodeV1(benchmark::State& state) {
    for (auto _ : state) {
        std::string out{encoded_str.data(), encoded_str.size()};
        auto        ptr = out.data();
        auto        res = v1::decode_uri_component_inplace(ptr, ptr + out.size(), webpp::ALPHA_DIGIT<char>);
        out.resize(static_cast<std::size_t>(ptr - out.data()));
        benchmark::DoNotOptimize(out);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(URIDecodeV1);


static void URIDecodeV2(benchmark::State& state) {
    for (auto _ : state) {
        std::string out{encoded_str.data(), encoded_str.size()};
        auto        ptr = out.data();
        auto        res = v2::decode_uri_component_inplace(ptr, ptr + out.size(), webpp::ALPHA_DIGIT<char>);
        out.resize(static_cast<std::size_t>(ptr - out.data()));
        benchmark::DoNotOptimize(out);
        benchmark::DoNotOptimize(res);
    }
}
BENCHMARK(URIDecodeV2);
