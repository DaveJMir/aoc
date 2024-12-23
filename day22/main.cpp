#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <ranges>

using namespace aoc::util;

class Monkey {
public:
  Monkey(uint64_t initial) : secret{initial} {
    assert(mix(42, 15) == 37);
    assert(prune(100000000) == 16113920);
  }

  uint64_t step(int n) {
    for(int i=0; i<n; i++)step();
    return secret;
  }

private:
  uint64_t secret;

  static inline uint64_t mix(uint64_t a, uint64_t b) { return a ^ b; }
  static inline uint64_t prune(uint64_t a) { return a % 16777216; }

  uint64_t step() {
    uint64_t a = prune(mix(secret * 64, secret));
    a = prune(mix(a / 32, a));
    secret = prune(mix(a * 2048, a));
    return secret;
  }
};

std::pair<std::uint64_t, std::uint64_t>
process(std::ifstream &&input) {
  assert(input.is_open());
  assert(Monkey{123}.step(10) == 5908254);

  std::vector<Monkey> monkeys;
  foreach_line(input, [&monkeys](std::string_view line)
  {
    int64_t val;
    auto [ptr, ec] =
        std::from_chars(line.data(), line.data() + line.size(), val);
    if (ec != std::errc()) {
      throw std::runtime_error("Bad parse: " + std::string(line));
    }
    monkeys.emplace_back(val);
  });

  uint64_t total = std::accumulate(monkeys.begin(), monkeys.end(), uint64_t{0}, [](uint64_t v, Monkey& m)
  {
    auto n = m.step(2000);
    return n + v;
  });
  return {total, 0};
}
