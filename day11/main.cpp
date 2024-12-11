#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <string_view>

using namespace aoc::util;

using Val = uint64_t;
using MemKey = std::pair<Val, int>;
using Memoize = std::map<MemKey, Val>;

void pv(auto &t) {
  return;
  std::cout << "{";
  for (auto x : t)
    std::cout << " " << x;
  std::cout << " }\n";
}

int numDigits(Val n) {
  int ret = 0;
  while (n) {
    ret++;
    n /= 10;
  }
  return ret;
}

std::pair<uint64_t, uint64_t> split(Val s, int num) {
  int scale = std::pow(10, num / 2);
  return {s / scale, s % scale};
}

Val stoneBlink(const Val s, int generation, Memoize &memoize) {
  if (generation == 0)
    return 1;
  auto key = MemKey{s, generation};
  if (auto found = memoize.find(key); found != memoize.end()) {
    return found->second;
  }

  Val ret;
  if (s == 0)
    ret = stoneBlink(1, generation - 1, memoize);
  else if (int numdigits = numDigits(s); (numdigits & 1) == 0) {
    auto vals = split(s, numdigits);
    ret = stoneBlink(vals.first, generation - 1, memoize) +
          stoneBlink(vals.second, generation - 1, memoize);
  } else
    ret = stoneBlink(s * 2024, generation - 1, memoize);

  memoize[key] = ret;
  return ret;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());

  Memoize memoize_p1{};
  Memoize memoize_p2{};

  std::pair<std::uint64_t, std::uint64_t> result{};
  int numLines = foreach_line(
      input, [&result, &memoize_p1, &memoize_p2](std::string_view line) {
        auto vals = string2vec<Val>(line);
        for (auto v : vals) {
          result.first += stoneBlink(v, 25, memoize_p1);
          result.second += stoneBlink(v, 75, memoize_p2);
        }
      });
  assert(numLines == 1);

  return result;
}