#include "../benchmark.hpp"
#include "../simd_pch.hpp" // std::experimental as of writing this

#include <algorithm>
#include <string>
#include <string_view>

// clang-format off
#include webpp_include(strings/to_case)
// clang-format on


using namespace std;



static void TLR_STD(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            benchmark::DoNotOptimize(tolower(c));
        }
    }
}
BENCHMARK(TLR_STD);

void to_lower_std_string(auto& str) noexcept {
    char*      it   = str.data();
    const auto size = str.size();
    const auto end  = it + size;
    for (; it != end; ++it) {
        *it = static_cast<char>(std::tolower(*it));
    }
}

static void TLR_STDString(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_std_string(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_STDString);

constexpr auto to_lower(auto&& c) noexcept {
    return c >= 'A' && c <= 'Z' ? c - ('A' - 'a') : c;
}

constexpr auto to_lower2(auto&& c) noexcept {
    using char_type = std::remove_cvref_t<decltype(c)>;
    switch (c) {
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return static_cast<char_type>(static_cast<char_type>(c) - static_cast<char_type>('A' - 'a'));
        default: return static_cast<char_type>(c);
    }
}



constexpr auto to_lower3(auto&& c) noexcept {
    switch (c) {
        case 'A': return 'a';
        case 'B': return 'b';
        case 'C': return 'c';
        case 'D': return 'd';
        case 'E': return 'e';
        case 'F': return 'f';
        case 'G': return 'g';
        case 'H': return 'h';
        case 'I': return 'i';
        case 'J': return 'j';
        case 'K': return 'k';
        case 'L': return 'l';
        case 'M': return 'm';
        case 'N': return 'n';
        case 'O': return 'o';
        case 'P': return 'p';
        case 'Q': return 'q';
        case 'R': return 'r';
        case 'S': return 's';
        case 'T': return 't';
        case 'U': return 'u';
        case 'V': return 'v';
        case 'W': return 'w';
        case 'X': return 'x';
        case 'Y': return 'y';
        case 'Z': return 'z';
        default: return c;
    }
}
static void TLR_Mine(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            benchmark::DoNotOptimize(to_lower(c));
        }
    }
}
BENCHMARK(TLR_Mine);


static void TLR_Switch1(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            benchmark::DoNotOptimize(to_lower2(c));
        }
    }
}
BENCHMARK(TLR_Switch1);


inline void to_lower_switch1(string& str) noexcept {
    char*      it   = str.data();
    const auto size = str.size();
    const auto end  = it + size;
    for (; it != end; ++it) {
        *it = to_lower2(*it);
    }
}

static void TLR_Switch1String(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_switch1(str);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_Switch1String);


static void TLR_Switch2(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            benchmark::DoNotOptimize(to_lower3(c));
        }
    }
}
BENCHMARK(TLR_Switch2);


inline void to_lower_switch2(string& str) noexcept {
    char*      it   = str.data();
    const auto size = str.size();
    const auto end  = it + size;
    for (; it != end; ++it) {
        *it = to_lower3(*it);
    }
}

static void TLR_Switch2String(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_switch2(str);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_Switch2String);



static void TLR_Webpp(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            // assert(webpp::ascii::to_lower_copy(c) == std::tolower(c));
            benchmark::DoNotOptimize(webpp::ascii::to_lower_copy(c));
        }
    }
}
BENCHMARK(TLR_Webpp);


auto to_lower_john(auto&& c) noexcept {
    return c + (32 * (c >= 'A' && c <= 'Z'));
}

static void TLR_John(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        for (auto c : str) {
            benchmark::DoNotOptimize(to_lower_john(c));
        }
    }
}
BENCHMARK(TLR_John);


inline void to_lower_string_john(auto& str) noexcept {
    auto*      it   = str.data();
    const auto size = str.size();
    const auto end  = it + size;
    for (; it != end; ++it)
        *it = to_lower_john(*it);
}

static void TLR_JohnString(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_string_john(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_JohnString);


//////////////////////////////////////////////////////////

static void TLR_WebppToLowerString(benchmark::State& state) {
    std::string str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        webpp::ascii::to_lower(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_WebppToLowerString);



#define TEST_SUITE(name, algorithm, str_count)             \
    static void TLR_##name##str_count(benchmark::State& state) { \
        auto str2 = str_generator(str_count);              \
        for (auto _ : state) {                             \
            std::string istr = str2;                       \
            algorithm(istr);                               \
            benchmark::DoNotOptimize(istr);                \
        }                                                  \
    }                                                      \
    BENCHMARK(TLR_##name##str_count);

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 1)
TEST_SUITE(NJohnString, to_lower_string_john, 1)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 16)
TEST_SUITE(NJohnString, to_lower_string_john, 16)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 32)
TEST_SUITE(NJohnString, to_lower_string_john, 32)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 64)
TEST_SUITE(NJohnString, to_lower_string_john, 64)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 128)
TEST_SUITE(NJohnString, to_lower_string_john, 128)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 256)
TEST_SUITE(NJohnString, to_lower_string_john, 256)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 512)
TEST_SUITE(NJohnString, to_lower_string_john, 512)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 1024)
TEST_SUITE(NJohnString, to_lower_string_john, 1024)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 2001)
TEST_SUITE(NJohnString, to_lower_string_john, 2001)

TEST_SUITE(NWebppEveString, webpp::ascii::algo::eve_to_lower, 10000)
TEST_SUITE(NJohnString, to_lower_string_john, 10000)





static void TLR_WebppToLowerStringData(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        webpp::ascii::to_lower(istr.data());
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_WebppToLowerStringData);


inline void webpp_to_lower2(auto& str) noexcept {
    using namespace webpp;
    using str_t           = stl::remove_cvref_t<decltype(str)>;
    using char_type       = istl::char_type_of<str_t>;
    char_type*       it   = str.data();
    const auto       size = str.size();
    const char_type* end  = it + size;
    for (; it != end; ++it)
        *it = webpp::ascii::to_lower_copy(*it);
}
static void TLR_ToLowerString2(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        webpp_to_lower2(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_ToLowerString2);

inline void webpp_to_lower3(auto& str) noexcept {
    using namespace webpp;
    using namespace webpp::ascii;
    using str_t                 = stl::remove_cvref_t<decltype(str)>;
    using char_type             = istl::char_type_of<str_t>;
    char_type*       it         = str.data();
    const auto       size       = str.size();
    const char_type* end        = it + size;
    const char_type* almost_end = end - (size % 4);
    for (; it != almost_end; it += 4) {
        *it       = to_lower_copy(*it);
        *(it + 1) = to_lower_copy(*(it + 1));
        *(it + 2) = to_lower_copy(*(it + 2));
        *(it + 3) = to_lower_copy(*(it + 3));
    }
    // doing the rest
    for (it -= 4; it != end; ++it)
        *it = to_lower_copy(*it);
}
static void TLR_ToLowerString3(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        webpp_to_lower3(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_ToLowerString3);


static void TLR_STDToLowerString(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        std::transform(istr.begin(), istr.end(), istr.begin(), [](auto c) {
            return std::tolower(c);
        });
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_STDToLowerString);



constexpr auto to_lower_simd(char c) noexcept {
    using char_type = std::remove_cvref_t<decltype(c)>;
    constexpr char_type diff('a' - 'A');
    return (c >= 'A' && c <= 'Z') ? c + diff : c;
}

void to_lower_simd(auto& str) noexcept {
    using namespace std::experimental;
    using char_type          = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type          = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type*       it         = str.data();
    const auto       size       = str.size();
    const char_type* end        = it + size;
    const char_type* almost_end = end - (size % simd_size);
    for (; it != almost_end; it += simd_size) {
        simd_type diff{'a' - 'A'};
        simd_type simd_it(it, element_aligned);
        auto      ifs = simd_it >= 'A' && simd_it <= 'Z';
        for (std::size_t idx = 0; idx != simd_size; ++idx) {
            if (!ifs[idx])
                diff[idx] = 0;
        }
        simd_it += diff;
        simd_it.copy_to(it, element_aligned);
    }
    // doing the rest
    for (it -= 4; it != end; ++it)
        *it = to_lower(*it);
}



static void TLR_SIMDToLowerString(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_simd(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_SIMDToLowerString);




void to_lower_simd2(auto& str) noexcept {
    using namespace std::experimental;
    using char_type          = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type          = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type*       it         = str.data();
    const auto       size       = str.size();
    const char_type* end        = it + size;
    const char_type* almost_end = end - (size % simd_size);
    simd_type        diff, simd_it;
    for (; it != almost_end; it += simd_size) {
        diff = 'a' - 'A';
        simd_it.copy_from(it, element_aligned);
        auto ifs = simd_it >= 'A' && simd_it <= 'Z';
        for (std::size_t idx = 0; idx != simd_size; ++idx) {
            if (!ifs[idx])
                diff[idx] = 0;
        }
        simd_it += diff;
        simd_it.copy_to(it, element_aligned);
    }
    // doing the rest
    for (it -= 4; it != end; ++it)
        *it = to_lower(*it);
}


static void TLR_SIMDToLowerString2(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_simd2(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_SIMDToLowerString2);




////////////////////////////////


#include <eve/eve.hpp>
#include <eve/function/store.hpp>
#include <eve/function/add.hpp>
// #include <eve/function/load.hpp>

//
// inline void eve_to_lower(auto &str) noexcept {
//   using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
//   constexpr auto simd_size = 64;
//   using simd_type = eve::wide<char, eve::fixed<simd_size>>;
//
//   char_type* it = str.data();
//   const auto size = str.size();
//   const char_type* end = it + size;
//   const char_type* almost_end = end - (size % simd_size);
//   simd_type diff, simd_it;
//   for (; it != almost_end; it += simd_size) {
//     diff = 'a' - 'A';
//     simd_it = simd_type{it};
//     auto data = eve::if_else(simd_it >= 'A' && simd_it <= 'Z', simd_it + diff, simd_it);
//     eve::store(data, it);
//   }
//   // doing the rest
//   for (it -= simd_size; it != end; ++it)
//     *it = to_lower(*it);
// }
//
//
// static void EveToLowerString(benchmark::State& state) {
//   for (auto _ : state) {
//     std::string istr = str;
//     eve_to_lower(istr);
//     benchmark::DoNotOptimize(istr);
//   }
// }
// BENCHMARK(TLR_EveToLowerString);
//
//
//
//
// inline void eve_to_lower2(auto &str) noexcept {
//   using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
//   constexpr auto simd_size = 64;
//   using simd_type = eve::wide<char, eve::fixed<simd_size>>;
//
//   char_type* it = str.data();
//   const auto size = str.size();
//   const char_type* end = it + size;
//   const char_type* almost_end = end - (size % simd_size);
//   simd_type simd_it;
//   const simd_type diff {'a' - 'A'};
//   for (; it != almost_end; it += simd_size) {
//     simd_it = simd_type{it};
//     const auto data = eve::add[ simd_it >= 'A' && simd_it <= 'Z']( simd_it, diff);
//     eve::store(data, it);
//   }
//   // doing the rest
//   // for (it -= simd_size; it != end; ++it)
//   //   *it = to_lower(*it);
//   it -= simd_size;
//   simd_it = simd_type{it};
//   const auto data = eve::add[ simd_it >= 'A' && simd_it <= 'Z']( simd_it, diff);
//   eve::store(data, it);
// }
//
//
// static void EveToLowerString2(benchmark::State& state) {
//   for (auto _ : state) {
//     std::string istr = str;
//     eve_to_lower2(istr);
//     benchmark::DoNotOptimize(istr);
//   }
// }
// BENCHMARK(TLR_EveToLowerString2);
//
//
//
// void eve_to_lower3(std::string &str) noexcept
// {
//   using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
//   constexpr auto simd_size = 64;
//   using simd_type = eve::wide<char, eve::fixed<simd_size>>;
//
//   char_type* it = str.data();
//   const auto size = str.size();
//   const char_type* end = it + size;
//   const char_type* almost_end = end - (size % simd_size);
//
//   for (; it != almost_end; it += simd_size)
//     {
//       auto values = simd_type{it};
//       auto data = eve::if_else( eve::saturated_(eve::sub)(values - 65,char_type(25)) == 0
//                                 , values+32
//                                 , char_type(32)
//                                 );
//       eve::store(data, it);
//     }
// }
//
//
// static void EveToLowerString3(benchmark::State& state) {
//   for (auto _ : state) {
//     std::string istr = str;
//     eve_to_lower3(istr);
//     benchmark::DoNotOptimize(istr);
//   }
// }
// BENCHMARK(TLR_EveToLowerString3);
//
//
//
//
//
//
// void eve_to_lower4(std::string &str) noexcept
// {
//   using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
//   constexpr auto simd_size = 64;
//   using simd_type = eve::wide<char, eve::fixed<simd_size>>;
//
//   char_type* it = str.data();
//   const auto size = str.size();
//   const char_type* end = it + size;
//   const char_type* almost_end = end - (size % simd_size);
//   const simd_type shift(32);
//   for (; it != almost_end; it += simd_size)
//     {
//       const auto values  = simd_type{it};
//       const auto shifted = values+shift;
//       const auto data = eve::if_else( eve::saturated_(eve::sub)(values - 65,char_type(25)) == 0
//                                 , shifted
//                                 , shift
//                                 );
//       eve::store(data, it);
//     }
// }
//
//
// static void EveToLowerString4(benchmark::State& state) {
//   for (auto _ : state) {
//     std::string istr = str;
//     eve_to_lower4(istr);
//     benchmark::DoNotOptimize(istr);
//   }
// }
// BENCHMARK(TLR_EveToLowerString4);
//
//
//
//
// void eve_to_lower5(std::string &str) noexcept
// {
//   using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
//   constexpr auto simd_size = 64;
//   using simd_type = eve::wide<char, eve::fixed<simd_size>>;
//
//   char_type* it = str.data();
//   const auto size = str.size();
//   const char_type* end = it + size;
//   const char_type* almost_end = end - (size % simd_size);
//   const simd_type shift(32);
//   const auto tick = [&] (auto&& it) noexcept {
//     const auto values  = simd_type{it};
//     const auto shifted = values+shift;
//     const auto data = eve::if_else( eve::saturated_(eve::sub)(values - 65,char_type(25)) == 0
//                                     , shifted
//                                     , shift
//                                     );
//     eve::store(data, it);
//   };
//   const auto almost_end_2 = end - (size % (simd_size * 4));
//     for (; it != almost_end_2; it += simd_size * 4)
//       {
//         tick(it);
//         tick(it + simd_size * 1);
//         tick(it + simd_size * 2);
//         tick(it + simd_size * 3);
//       }
//     for (; it != almost_end; it += simd_size)
//       {
//         tick(it);
//       }
// }
//
//
// static void EveToLowerString5(benchmark::State& state) {
//   for (auto _ : state) {
//     std::string istr = str;
//     eve_to_lower5(istr);
//     benchmark::DoNotOptimize(istr);
//   }
// }
// BENCHMARK(TLR_EveToLowerString5);

static void TLR_EveToLowerStringFixed(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        webpp::ascii::algo::eve_to_lower(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_EveToLowerStringFixed);





constexpr auto to_lower_utopia(auto c) noexcept {
    __asm volatile("# LLVM-MCA-BEGIN tolower_utopia");
    using char_type = std::remove_cvref_t<decltype(c)>;
    constexpr char_type diff('a' - 'A');
    return (c >= 'A' && c <= 'Z') ? c + diff : c;
    __asm volatile("# LLVM-MCA-END");
}





/*

void to_lower_simd_utopia(auto &str) noexcept  {
  using namespace std::experimental;
    using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type* it = str.data();
    const auto size = str.size();
    const char_type* end = it + size;

    const char_type diff{'a' - 'A'};

    const simd_type lower_bound{'A' - 1};
    const simd_type upper_bound{'Z' + 1};
    const simd_type one{1};

    simd_type simd_it;
    for (; it < end; it += simd_size) {
        simd_it.copy_from(it, element_aligned);
        const auto simd_it_lower_mask = min(max(simd_it, lower_bound) - lower_bound, one);
        const auto simd_it_upper_mask = min(upper_bound - min(simd_it, upper_bound), one);

        simd_it += (simd_it_lower_mask & simd_it_upper_mask) * diff;
        simd_it.copy_to(it, element_aligned);
    }
    // doing the rest
    for (; it != end; ++it)
        *it = to_lower(*it);

}


static void TLR_UtopiaToLowerString(benchmark::State& state) {
  for (auto _ : state) {
    std::string istr = str.data();
    to_lower_simd_utopia(istr);
    benchmark::DoNotOptimize(istr);
  }
}
BENCHMARK(TLR_UtopiaToLowerString);





void to_lower_simd_utopia_vector_alligned(auto &str) noexcept  {
  using namespace std::experimental;
    using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type* it = str.data();
    const auto size = str.size();
    const char_type* end = it + size;

    const char_type diff{'a' - 'A'};

    const simd_type lower_bound{'A' - 1};
    const simd_type upper_bound{'Z' + 1};
    const simd_type one{1};

    simd_type simd_it;
    for (; it < end; it += simd_size) {
        simd_it.copy_from(it, vector_aligned);
        const auto simd_it_lower_mask = min(max(simd_it, lower_bound) - lower_bound, one);
        const auto simd_it_upper_mask = min(upper_bound - min(simd_it, upper_bound), one);

        simd_it += (simd_it_lower_mask & simd_it_upper_mask) * diff;
        simd_it.copy_to(it, vector_aligned);
    }
    // doing the rest
    for (; it != end; ++it)
        *it = to_lower(*it);

}


static void TLR_UtopiaToLowerStringVectorAligned(benchmark::State& state) {
  for (auto _ : state) {
    std::string istr = str.data();
    to_lower_simd_utopia_vector_alligned(istr);
    benchmark::DoNotOptimize(istr);
  }
}
BENCHMARK(TLR_UtopiaToLowerStringVectorAligned);






void to_lower_simd_utopia_over_alligned(auto &str) noexcept  {
  using namespace std::experimental;
    using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type* it = str.data();
    const auto size = str.size();
    const char_type* end = it + size;

    const char_type diff{'a' - 'A'};

    const simd_type lower_bound{'A' - 1};
    const simd_type upper_bound{'Z' + 1};
    const simd_type one{1};

    simd_type simd_it;
    for (; it < end; it += simd_size) {
        simd_it.copy_from(it, overaligned<simd_size>);
        const auto simd_it_lower_mask = min(max(simd_it, lower_bound) - lower_bound, one);
        const auto simd_it_upper_mask = min(upper_bound - min(simd_it, upper_bound), one);

        simd_it += (simd_it_lower_mask & simd_it_upper_mask) * diff;
        simd_it.copy_to(it, overaligned<simd_size>);
    }
    // doing the rest
    for (; it != end; ++it)
        *it = to_lower(*it);

}


static void TLR_UtopiaToLowerStringOverAligned(benchmark::State& state) {
  for (auto _ : state) {
    std::string istr = str.data();
    to_lower_simd_utopia_over_alligned(istr);
    benchmark::DoNotOptimize(istr);
  }
}
BENCHMARK(TLR_UtopiaToLowerStringOverAligned);
*/





void to_lower_simd_utopia_optimized(auto& str) noexcept {
    using namespace std::experimental;
    using char_type          = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type          = native_simd<char_type>;
    constexpr auto simd_size = simd_type().size();

    char_type*       it       = str.data();
    const auto       size     = str.size();
    const char_type* end      = it + size;
    const char_type* simd_end = end - simd_size;

    const char_type diff{'a' - 'A'};

    const simd_type lower_bound{'A' - 1};
    const simd_type upper_bound{'Z' + 1};
    const simd_type one{1};

    simd_type simd_it;
    for (; it < simd_end; it += simd_size) {
        simd_it.copy_from(it, element_aligned);
        const auto simd_it_mask =
          min(max(simd_it, lower_bound) - lower_bound, upper_bound - min(simd_it, upper_bound));

        simd_it += min(simd_it_mask, one) * diff;
        simd_it.copy_to(it, element_aligned);
    }
    // doing the rest
    for (; it < end; ++it)
        *it = to_lower(*it);
}


static void TLR_UtopiaToLowerStringOptimized(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_simd_utopia_optimized(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_UtopiaToLowerStringOptimized);





void to_lower_simd_utopia3(auto& str) noexcept {
    using namespace std::experimental;
    using char_type = typename std::remove_cvref_t<decltype(str)>::value_type;
    using simd_type = native_simd<char_type>;

    auto       it   = str.data();
    const auto size = str.size();
    const auto end  = it + size;

    constexpr auto               simd_size = simd_type().size();
    const auto                   remainder = size % simd_size;
    const auto                   simd_end  = end - remainder;
    static constexpr char_type   diff{'a' - 'A'};
    static constexpr std::size_t diff_bit_width =
      std::bit_width(std::size_t(diff)); // Can put in 6 if bit_width is not available

    const simd_type lower_bound{'A' - 1};
    const simd_type upper_bound{'Z' + 1};
    const simd_type one_vector{1};
    const simd_type diff_vector{diff};

    simd_type simd_it;
    for (; it < simd_end; it += simd_size) {
        simd_it.copy_from(it, element_aligned);
        const auto simd_it_mask =
          min(max(simd_it, lower_bound) - lower_bound, upper_bound - min(simd_it, upper_bound));

        simd_it += min(min(simd_it_mask, one_vector) << diff_bit_width, diff_vector);
        simd_it.copy_to(it, element_aligned);
    }

    // doing the rest
    for (; it < end; ++it)
        *it = to_lower(*it);
}

static void TLR_UtopiaToLowerString3(benchmark::State& state) {
    auto str = str_generator();
    for (auto _ : state) {
        std::string istr = str;
        to_lower_simd_utopia3(istr);
        benchmark::DoNotOptimize(istr);
    }
}
BENCHMARK(TLR_UtopiaToLowerString3);
