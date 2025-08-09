#include "../../webpp/unicode/unicode.hpp"
#include "../benchmark.hpp"
#include "../common_utils_pch.hpp"

#include <cstring>

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
        using char_type          = stl::iter_value_t<Iter>;
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
        out.resize_and_overwrite(src.size(), [&](char32_t* buf, std::size_t buf_size) {
            auto pos = src.begin();
            while (pos != src.end()) {
                auto const code_point = next_code_point(pos, src.end());
                if (code_point == 0) {
                    break;
                }
                *buf++ = code_point;
            }
            return pos - src.begin();
        });
        return out;
    }

    std::u32string utf8_to_utf32_variant(std::u8string const& src) {
        std::u32string out;
        out.reserve(src.size());

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
                bool const requires_2_units = (cu1 & 0xFC00U) == 0xD800U;
                bool       error            = false;
                if (requires_2_units) {
                    code_point  &= 0x3FFU;
                    code_point <<= 10U;
                    if (pos == end) [[unlikely]] {
                        break;
                    }
                    auto const cu2  = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                    error          |= (cu1 & 0xFC00U) != 0xD800U;
                    error          |= (cu2 & 0xFC00U) != 0xDC00U;
                    code_point     |= cu2 & 0x3FFU;
                    code_point     += 0x1'0000U;
                }
                if (error || is_surrogate(code_point)) [[unlikely]] {
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
                        auto const cu2     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1110'0000U) != 0b1100'0000U;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        code_point        &= 0b0001'1111U;
                        code_point       <<= 6U;
                        code_point        |= cu2 & 0b0011'1111U;
                        if (error || code_point < 0x80 || 0x7ff < code_point) [[unlikely]] {
                            --pos;
                            code_point = cu1;
                            break;
                        }
                        return code_point;
                    }
                    case 3: {
                        auto const cu2     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu3     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1111'0000U) != 0b1110'0000U;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        error             |= (cu3 & 0b1100'0000U) != 0b1000'0000U;
                        code_point        &= 0b0000'1111U;
                        code_point       <<= 12U;
                        code_point        |= (cu2 & 0b0011'1111U) << 6U;
                        code_point        |= cu3 & 0b0011'1111U;
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
                        auto const cu2     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu3     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        auto const cu4     = static_cast<code_point_type>(static_cast<unsigned_char_type>(*pos++));
                        bool       error   = (cu1 & 0b1111'0000) != 0b1111'0000;
                        error             |= (cu2 & 0b1100'0000U) != 0b1000'0000U;
                        error             |= (cu3 & 0b1100'0000U) != 0b1000'0000U;
                        error             |= (cu4 & 0b1100'0000U) != 0b1000'0000U;
                        code_point        &= 0b0000'0111U;
                        code_point       <<= 18U;
                        code_point        |= (cu2 & 0b0011'1111U) << 12U;
                        code_point        |= (cu3 & 0b0011'1111U) << 6U;
                        code_point        |= cu4 & 0b0011'1111U;
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
        } else if constexpr (ErrorHandling == return_negated) {
            static_assert(stl::is_unsigned_v<code_point_type>,
                          "The code point type should support negative values if you want us to return "
                          "negative values as errors.");
            return -code_point;
        } else {
            return code_point;
        }
    }

    std::u32string utf8_to_utf32(std::u8string const& src) {
        std::u32string out;
        out.resize_and_overwrite(src.size(), [&](char32_t* buf, std::size_t buf_size) {
            auto pos = src.begin();
            while (pos != src.end()) {
                auto const code_point = next_code_point(pos, src.end());
                if (code_point == 0) {
                    break;
                }
                *buf++ = code_point;
            }
            return pos - src.begin();
        });
        return out;
    }

    std::u32string utf8_to_utf32_variant(std::u8string const& src) {
        std::u32string out;
        out.reserve(src.size());

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

namespace other_impl {
    /// this is not my implementation
    size_t utf8_to_utf32(char8_t const* buf, size_t len, char32_t* out) {
        uint8_t const* ptr = reinterpret_cast<uint8_t const*>(buf);
        size_t         pos = 0;
        char32_t*      start{out};
        while (pos < len) {
            if (pos + 16 <= len) {
                uint64_t v1;
                std::memcpy(&v1, ptr + pos, sizeof(uint64_t));
                uint64_t v2;
                std::memcpy(&v2, ptr + pos + sizeof(uint64_t), sizeof(uint64_t));
                uint64_t v{v1 | v2};
                if ((v & 0x8080'8080'8080'8080) == 0) {
                    size_t final_pos = pos + 16;
                    while (pos < final_pos) {
                        *out++ = char32_t(buf[pos]);
                        pos++;
                    }
                    continue;
                }
            }
            uint8_t cu1 = ptr[pos];
            if (cu1 < 0b1000'0000) {
                *out++ = char32_t(cu1);
                pos++;
            } else if ((cu1 & 0b1110'0000) == 0b1100'0000) {
                // We have a two-byte UTF-8
                if (pos + 1 >= len) {
                    return 0;
                } // minimal bound checking
                if ((ptr[pos + 1] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }
                // range check
                uint32_t cp = (cu1 & 0b0001'1111) << 6 | (ptr[pos + 1] & 0b0011'1111);
                if (cp < 0x80 || 0x7ff < cp) {
                    return 0;
                }
                *out++  = char32_t(cp);
                pos    += 2;
            } else if ((cu1 & 0b1111'0000) == 0b1110'0000) {
                if (pos + 2 >= len) {
                    return 0;
                }

                if ((ptr[pos + 1] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }
                if ((ptr[pos + 2] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }
                // range check
                uint32_t cp =
                  (cu1 & 0b0000'1111) << 12 | (ptr[pos + 1] & 0b0011'1111) << 6 | (ptr[pos + 2] & 0b0011'1111);
                if (cp < 0x800 || 0xffff < cp || (0xd7ff < cp && cp < 0xe000)) {
                    return 0;
                }
                *out++  = char32_t(cp);
                pos    += 3;
            } else if ((cu1 & 0b1111'1000) == 0b1111'0000) { // 0b11110000
                if (pos + 3 >= len) {
                    return 0;
                }
                if ((ptr[pos + 1] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }
                if ((ptr[pos + 2] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }
                if ((ptr[pos + 3] & 0b1100'0000) != 0b1000'0000) {
                    return 0;
                }

                uint32_t cp = (cu1 & 0b0000'0111) << 18 | (ptr[pos + 1] & 0b0011'1111) << 12 |
                              (ptr[pos + 2] & 0b0011'1111) << 6 | (ptr[pos + 3] & 0b0011'1111);
                if (cp <= 0xffff || 0x10'ffff < cp) {
                    return 0;
                }
                *out++  = char32_t(cp);
                pos    += 4;
            } else {
                return 0;
            }
        }
        return out - start;
    }

    std::u32string utf8_to_utf32(std::u8string const& src) {
        std::u32string out;
        out.resize_and_overwrite(src.size(), [&](char32_t* buf, std::size_t buf_size) {
            return utf8_to_utf32(src.data(), src.size(), buf);
        });
        return out;
    }

} // namespace other_impl

auto const str8 = str8_generator(10'000);
auto const str  = str_generator<std::u8string>(10'000);

static void UTFConv_v1(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v1::utf8_to_utf32(str8);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v1);

static void UTFConv_v1_variant(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v1::utf8_to_utf32_variant(str8);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v1_variant);

static void UTFConv_v2(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32(str8);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v2);

static void UTFConv_v2_variant(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32_variant(str8);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v2_variant);

static void UTFConv_OtherImpl(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32(str8);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_OtherImpl);

//////////////////////////////// ASCII ////////////////////////////

static void UTFConv_v1_ascii(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v1::utf8_to_utf32(str);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v1_ascii);

static void UTFConv_v2_ascii(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32(str);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_v2_ascii);

static void UTFConv_OtherImpl_ascii(benchmark::State& state) {
    for ([[maybe_unused]] auto _ : state) {
        auto res = v2::utf8_to_utf32(str);
        benchmark::DoNotOptimize(res);
    }
}

BENCHMARK(UTFConv_OtherImpl_ascii);

// NOLINTEND(*-magic-numbers)
