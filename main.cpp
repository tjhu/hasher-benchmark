#include <iostream>
#include <nmmintrin.h>
#include <x86intrin.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "city.h"
#include "xxhash.h"
#include "zipf.h"
#include "xorwow.h"
#include "wyhash.h"
extern "C" {
#include "fnv.h"
}

typedef void(*hashfn)(const void*);

void city(const void* buff) {
  CityHash64((char*)buff, 8);
}

void xxh(const void* buff) {
  XXH64(buff, 8, 0);
}

void xxh3(const void* buff) {
  XXH3_64bits_withSeed(buff, 8, 0);
}

void crc(const void* buff) {
  _mm_crc32_u64(0, *(uint64_t*)buff);
}

void wyhash(const void* buff) {
  wyhash(buff, 8, 0, _wyp);
}

void fnv32(const void* buff) {
  static uint32_t state_ = FNV1_32_INIT;
  state_ = fnv_32_buf((void*)buff, 8, state_);
}

void fnv64(const void* buff) {
  static uint64_t state_ = FNV1_64_INIT;
  state_ = fnv_64_buf((void*)buff, 8, state_);
}

void fnv64a(const void* buff) {
  static uint64_t state_ = FNV1A_64_INIT;
  state_ = fnv_64a_buf((void*)buff, 8, state_);
}

void run_hashfn(const std::vector<uint64_t>& dataset, hashfn fn) {
  for(const auto& data : dataset) {
    fn(&data);
  }
}

void benchmark(const std::string& name, const std::vector<uint64_t>& dataset, hashfn fn) {
    unsigned int tmp;
    const auto start = __rdtsc();
    run_hashfn(dataset, fn);
    const auto end = __rdtscp(&tmp);
    const auto duration = end - start;

    const auto test_size = dataset.size();
    std::cout << name << ": " << duration / test_size << " cycles"  << std::endl;
}

template <typename Generator>
std::vector<uint64_t> build_dataset(uint64_t size, Generator& gen) {
  std::vector<uint64_t> vec;
  vec.reserve(size);
  for (uint64_t i = 0; i < size; i++) {
    const auto val = gen();
    vec.push_back(val);
  }
  return vec;
}

void run_benchmarks(const std::string& name, const std::vector<uint64_t>& dataset) {
  benchmark("CITY-" + name, dataset, city);
  benchmark("XXH-" + name, dataset, xxh);
  benchmark("XXH3-" + name, dataset, xxh3);
  benchmark("CRC-" + name, dataset, crc);
  benchmark("WYHASH-" + name, dataset, wyhash);
  benchmark("FNV32-" + name, dataset, fnv32);
  benchmark("FNV64-" + name, dataset, fnv64);
  benchmark("FNV64a-" + name, dataset, fnv64a);
}

int main() {
  constexpr uint64_t test_size = 10'000'000;
  std::vector<uint64_t> dataset;
  
  XorwowDist dist;
  dataset = build_dataset<>(test_size, dist);
  run_benchmarks("xorwow", dataset);
  

  for (const double skew : {0.0, 0.1, 0.5, 0.9}) {
    zipf_distribution dist{skew, 192ull * (1ull << 20), 1};  // FIXME: magic numbers
    dataset = build_dataset<>(test_size, dist);

    std::stringstream ss;
    ss << std::fixed << std::setprecision(1) << skew;
    const auto skew_str = ss.str();
    run_benchmarks(skew_str, dataset);
  }
  return 0;
}