#include "../../webpp/unicode/unicode.hpp"
#include "../benchmark.hpp"
#include "../common_utils_pch.hpp"

// NOLINTBEGIN(*-magic-numbers)
namespace v1 {

    using namespace webpp;
    using namespace webpp::unicode;
    using namespace webpp::unicode::unchecked;

    template <stl::bidirectional_iterator Iter          = char8_t const*,
              stl::bidirectional_iterator EIter         = Iter,
              UTF32                       CodePointType = char32_t>
    [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos, EIter end) noexcept {
        using code_point_type    = CodePointType;
        using char_type          = typename stl::iterator_traits<Iter>::value_type;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;

        if (pos == end) {
            return static_cast<code_point_type>(0);
        }

        // double casting to make sure negative values can't come out of it
        auto val = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
        if (pos == end) {
            return val;
        }
        if constexpr (UTF16<char_type>) {
            if ((val & 0xFC00U) == 0xD800U) {
                // we have two chars
                val  &= 0x3FFU;
                val <<= 10U;
                val  |= static_cast<code_point_type>(*pos++) & 0x3FFU;
                val  += 0x1'0000U;
                return val;
            }
            return val; // this is the only char
        } else if constexpr (UTF8<char_type>) {
            if ((val & 0b1000'0000U) == 0) {
                // we have one char
                return val;
            }
            if ((val & 0b1110'0000U) == 0b1100'0000U) {
                // we have 2 chars
                val  &= 0b0001'1111U;
                val <<= 6U;
                val  |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                return val;
            }
            if ((val & 0b1111'0000U) == 0b1110'0000U) {
                // we have 3 chars
                val  &= 0b0000'1111U;
                val <<= 12U;
                val  |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U;
                if (++pos == end) {
                    return *stl::prev(--pos); // bad code point found, return the first code unit
                }
                val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                return val;
            }
            if ((val & 0b1111'1000U) == 0b1111'0000U) {
                // we have 4 chars
                val  &= 0b0000'0111U;
                val <<= 18U;
                val  |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 12U;
                if (++pos == end) {
                    return *stl::prev(--pos); // bad code point found, return the first code unit
                }
                val |= (static_cast<code_point_type>(*pos) & 0b0011'1111U) << 6U;
                if (++pos == end) {
                    stl::advance(pos, -2);
                    return *stl::prev(pos); // bad code point found, return the first code unit
                }
                val |= static_cast<code_point_type>(*pos++) & 0b0011'1111U;
                return val;
            }
            return val; // return this one anyway
        } else {
            return val;
        }
    }

    std::u32string utf8_to_utf32(std::u8string const& src) {
        std::u32string out;
        out.reserve(src.length() * 4); // Estimate maximum size of UTF-8 string

        auto pos = src.begin();
        while (pos != src.end()) {
            auto const code_point = next_code_point(pos, src.end());
            if (code_point == 0) {
                break;
            }
            out.push_back(code_point);
        }
        out.resize(pos - src.begin());

        return out;
    }


} // namespace v1

namespace v2 {
    using namespace webpp;
    using namespace webpp::unicode;
    using namespace webpp::unicode::checked;

    template <error_handling              ErrorHandling = error_handling::return_unchanged,
              UTF32                       CodePointType = char32_t,
              stl::random_access_iterator Iter          = char8_t const*>
    [[nodiscard]] static constexpr CodePointType next_code_point(Iter& pos, Iter const& end) noexcept {
        using enum error_handling;
        using code_point_type    = CodePointType;
        using iter_traits        = stl::iterator_traits<Iter>;
        using char_type          = typename iter_traits::value_type;
        using unsigned_char_type = stl::make_unsigned_t<char_type>;
        using difference_type    = typename iter_traits::difference_type;

        if (pos == end) {
            return static_cast<code_point_type>(0); // return \0 if we're at the end already
        }

        auto const cu1        = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
        auto       code_point = cu1;

        // we're in a constexpr land, we can't use goto; damn all of you developers who think goto
        // is not good enough for you; well, guess what, you're not smart enough to use goto.
        for (;;) {
            // double casting to make sure negative values can't come out of it
            if constexpr (UTF32<char_type>) {
                return cu1;
            } else if constexpr (UTF16<char_type>) {
                if (pos == end) [[unlikely]] {
                    break;
                }
                auto const cu2     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                bool       error   = (cu1 & 0xFC00U) != 0xD800U;
                error             |= (cu2 & 0xFC00U) != 0xDC00U;
                // we have two chars
                code_point        &= 0x3FFU;
                code_point       <<= 10U;
                code_point        |= cu2 & 0x3FFU;
                code_point        += 0x1'0000U;
                if (error) [[unlikely]] {
                    --pos;
                    code_point = cu1;
                    break;
                }
                return code_point;
            } else if constexpr (UTF8<char_type>) {
                auto const len = required_length_of<char_type, difference_type>(cu1);
                if (end - pos < len - 1) [[unlikely]] {
                    break;
                }
                switch (len) {
                    case 1:
                        if ((cu1 & 0b1000'0000U) != 0) [[unlikely]] {
                            break;
                        }
                        return cu1;
                    case 2: {
                        auto const cu2 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool error   = (cu1 & 0b1110'0000U) != 0b1100'0000U;
                        error       |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        code_point  &= 0b0001'1111U;
                        code_point <<= 6U;
                        code_point  |= cu2 & 0b0011'1111U;
                        if (error || code_point < 0x80 || 0x7ff < code_point) [[unlikely]] {
                            --pos;
                            code_point = cu1;
                            break;
                        }
                        return code_point;
                    }
                    case 3: {
                        auto const cu2 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu3 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool error   = (cu1 & 0b1111'0000U) != 0b1110'0000U;
                        error       |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        error       |= (cu3 & 0b1100'0000U) != 0b1000'0000U;
                        code_point  &= 0b0000'1111U;
                        code_point <<= 12U;
                        code_point  |= (cu2 & 0b0011'1111U) << 6U;
                        code_point  |= cu3 & 0b0011'1111U;
                        if (error || code_point < 0x800U || 0xFFFFU < code_point ||
                            (0xD7FFU < code_point && code_point < 0xE000U)) [[unlikely]]
                        {
                            stl::advance(pos, -2);
                            code_point = cu1;
                            break;
                        }
                        return code_point;
                    }
                    case 4: {
                        auto const cu2 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu3 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu4 =
                          static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool error   = (cu1 & 0b1111'0000) != 0b1111'0000;
                        error       |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        error       |= (cu3 & 0b1100'0000U) != 0b1000'0000U;
                        error       |= (cu4 & 0b1100'0000U) != 0b1000'0000U;
                        code_point  &= 0b0000'0111U;
                        code_point <<= 18U;
                        code_point  |= (cu2 & 0b0011'1111U) << 12U;
                        code_point  |= (cu3 & 0b0011'1111U) << 6U;
                        code_point  |= cu4 & 0b0011'1111U;
                        if (error || code_point <= 0xFFFFU || 0x10'FFFFU < code_point) [[unlikely]] {
                            stl::advance(pos, -3);
                            code_point = cu1;
                            break;
                        }
                        return code_point;
                    }
                    default: break;
                }
                break;

            } else {
                static_assert_false(char_type, "Invalid code unit type.");
                return cu1;
            }
        }

        // handle errors:
        if constexpr (ErrorHandling == return_replacement_char) {
            return replacement_char<code_point_type>;
        } else if constexpr (ErrorHandling == return_negated_char) {
            return -code_point;
        } else {
            return code_point;
        }
    }

    std::u32string utf8_to_utf32(std::u8string const& src) {
        std::u32string out;
        out.reserve(src.length() * 4); // Estimate maximum size of UTF-8 string

        auto pos = src.begin();
        while (pos != src.end()) {
            auto const code_point = next_code_point(pos, src.end());
            if (code_point == 0) {
                break;
            }
            out.push_back(code_point);
        }
        out.resize(pos - src.begin());

        return out;
    }

} // namespace v2

auto const str = str8_generator(10'000);

static void UTFConv_v1(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v1::utf8_to_utf32(str);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v1);

static void UTFConv_v2(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32(str);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v2);

// NOLINTEND(*-magic-numbers)
