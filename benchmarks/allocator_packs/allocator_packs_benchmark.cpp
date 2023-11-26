#include "../../webpp/memory/object.hpp"
#include "../../webpp/memory/std_pmr_allocator_pack.hpp"
#include "../benchmark.hpp"

using namespace webpp;
using namespace webpp::stl;
using namespace webpp::object;
using namespace webpp::alloc;

auto rand_gen = std::mt19937(std::random_device()());

static void custom_str_generator(auto& str) {
    static constexpr std::size_t      size = 100;
    static constexpr std::string_view chars =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    //    str.reserve(size);
    for (std::size_t i = 0; i < size; i++) {
        str.append(chars);
    }
    std::shuffle(str.begin(), str.end(), rand_gen);
    // str = str.substr(0, size);
}

static void ALLOC_STD(benchmark::State& state) {
    for (auto _ : state) {
        std::string str1;
        custom_str_generator(str1);
        benchmark::DoNotOptimize(str1.data());
    }
}

BENCHMARK(ALLOC_STD);

static void ALLOC_Stack_PMR_Pack(benchmark::State& state) {
    allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    for (auto _ : state) {
        auto str1 = make_local<string>(alloc_pack);
        custom_str_generator(str1);
        benchmark::DoNotOptimize(str1.data());
    }
}

BENCHMARK(ALLOC_Stack_PMR_Pack);

static void ALLOC_Heap_PMR_Pack(benchmark::State& state) {
    allocator_pack<stl::pmr::allocator_descriptors> alloc_pack;
    for (auto _ : state) {
        auto str1 = make_general<string>(alloc_pack);
        custom_str_generator(str1);
        benchmark::DoNotOptimize(str1.data());
    }
}

BENCHMARK(ALLOC_Heap_PMR_Pack);
