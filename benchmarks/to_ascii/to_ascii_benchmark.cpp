#include "../../webpp/unicode/to_ascii.hpp"
#include "../benchmark.hpp"

#if __has_include("./ada_idna/singleheader/ada_idna.h")
#    include "./ada_idna/singleheader/ada_idna.h"
#endif

using namespace webpp;

auto const strs32 = str32_array_generator<1000>(100);
auto const strs8  = str8_array_generator<1000>(100);
auto const strs   = str_array_generator<1000>(100, "0123456789abcdefghijklmnopqrstuvwxyz-");

static void PunycodeEncode_webpp(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str = strs32.at(index++ % strs32.size());
        std::string out;
        auto        result = unicode::idna::to_punycode(std::u32string_view{str.data(), str.size()}, out);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(PunycodeEncode_webpp);

static void PunycodeEncode_webpp_utf32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const&    str = strs32.at(index++ % strs32.size());
        std::u32string out;
        auto           result = unicode::idna::to_punycode(std::u32string_view{str.data(), str.size()}, out);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(PunycodeEncode_webpp_utf32);

static void PunycodeEncode_webpp_utf8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const&   str = strs8.at(index++ % strs8.size());
        std::u8string out;
        auto          result = unicode::idna::to_punycode(std::u8string_view{str.data(), str.size()}, out);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(PunycodeEncode_webpp_utf8);


#ifdef HAS_ADA_IDNA
static void PunycodeEncode_Ada(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str = strs32.at(index++ % strs32.size());
        std::string out;
        auto        result = ada::idna::utf32_to_punycode(std::u32string_view{str.data(), str.size()}, out);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(out);
    }
}

BENCHMARK(PunycodeEncode_Ada);
#endif

static void ToASCII_UTF32_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF32_to_UTF32);

static void ToASCII_UTF8_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF8_to_UTF32);

static void ToASCII_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF8_to_UTF8);

static void ToASCII_UTF32_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_UTF32_to_UTF8);

static void ToASCII_ASCII_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_ASCII_to_UTF8);

static void ToASCII_ASCII_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_ASCII_to_UTF32);

/////////////////////////////////

static void ToASCII_Loose_UTF32_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_UTF32_to_UTF32);

static void ToASCII_Loose_UTF8_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_UTF8_to_UTF32);

static void ToASCII_Loose_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_UTF8_to_UTF8);

static void ToASCII_Loose_UTF32_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_UTF32_to_UTF8);

static void ToASCII_Loose_ASCII_to_utf8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_ASCII_to_utf8);

static void ToASCII_Loose_ASCII_to_utf32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::loose_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Loose_ASCII_to_utf32);

/////////////////////////////////

static void ToASCII_Strict_UTF32_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_UTF32_to_UTF32);

static void ToASCII_Strict_UTF8_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_UTF8_to_UTF32);

static void ToASCII_Strict_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs8.at(index++ % strs8.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_UTF8_to_UTF8);

static void ToASCII_Strict_UTF32_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs32.at(index++ % strs32.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_UTF32_to_UTF8);

static void ToASCII_Strict_ASCII_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u8string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_ASCII_to_UTF8);

static void ToASCII_Strict_ASCII_to_UTF32(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str   = strs.at(index++ % strs.size());
        auto        ascii = unicode::idna::to_ascii<std::u32string, unicode::idna::strict_idna_options>(str);
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Strict_ASCII_to_UTF32);


#ifdef HAS_ADA_IDNA
// Ada-url/idna benchmarks
static void ToASCII_Ada_UTF8_to_UTF8(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str = strs8.at(index++ % strs8.size());
        auto ascii = ada::idna::to_ascii(std::string_view(reinterpret_cast<char const*>(str.data()), str.length()));
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Ada_UTF8_to_UTF8);

static void ToASCII_Ada_ASCII(benchmark::State& state) {
    std::size_t index = 0;
    for (auto _ : state) {
        auto const& str = strs.at(index++ % strs.size());
        auto ascii = ada::idna::to_ascii(std::string_view(reinterpret_cast<char const*>(str.data()), str.length()));
        benchmark::DoNotOptimize(ascii);
    }
}

BENCHMARK(ToASCII_Ada_ASCII);
#endif
