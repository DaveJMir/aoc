#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <ostream>
#include <ranges>
#include <set>
#include <ranges>
#include <iostream>
#include <vector>
#include <ranges>

using namespace aoc::util;

struct WinFour
{
  int a{0}, b{0}, c{0}, d{0};
  int push(int x)
  {
    int r = a;
    a = b;
    b = c;
    c = d;
    d = x;
    return r;
  }

  friend std::ostream& operator<<(std::ostream& os, const WinFour& w)
  {
    return std::cout << "W4{"<<w.a<<","<<w.b<<","<<w.c<<","<<w.d<<"}";
  }

  bool operator<(const WinFour &other) const {
    if (a != other.a)
      return a < other.a;
    if (b != other.b)
      return b < other.b;
    if (c != other.c)
      return c < other.c;
    return d < other.d;
  }
};

class Monkey {
public:
  Monkey(uint64_t initial) : initial{initial} {
    assert(mix(42, 15) == 37);
    assert(prune(100000000) == 16113920);
  }

  uint64_t step(int n) {
    auto secret = initial;
    for(int i=0; i<n; i++)
      secret = step(secret);
    return secret;
  }

  friend std::ostream& operator<<(std::ostream& os, const Monkey& m)
  {
    return os << "M{" << m.initial << "}";
  }

  void generate(int n) {
    auto lastSecret = initial;
    auto lastDigit = initial % 10;
    WinFour w4{};


    for(int i=0; i<n; i++)
    {
      auto secret = step(lastSecret);
      auto digit = secret % 10;

      int difference = digit - lastDigit;
      w4.push(difference);
      if(i>=3) {
        prices.emplace(w4, digit);
      }

      lastDigit = digit;
      lastSecret = secret;
    }
  }

  const std::map<WinFour, int>& getPrices() const { return prices; };

private:
  const uint64_t initial;
  std::map<WinFour, int> prices{};

  static inline uint64_t mix(uint64_t a, uint64_t b) { return a ^ b; }
  static inline uint64_t prune(uint64_t a) { return a % 16777216; }

  uint64_t step(uint64_t v) {
    const uint64_t a = prune(mix(v * 64, v));
    const uint64_t b = prune(mix(a / 32, a));
    return prune(mix(b * 2048, b));
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

  uint64_t part1 = std::accumulate(monkeys.begin(), monkeys.end(), uint64_t{0}, [](uint64_t v, Monkey& m)
  {
    auto n = m.step(2000);
    return n + v;
  });

  int numCommands = 0;
  std::set<WinFour> commands{};
  for(auto& m : monkeys)
  {
     m.generate(2000);
     const auto& prices = m.getPrices();
     numCommands += prices.size();

     for (const auto &key : prices | std::views::keys) {
      commands.insert(key);
     }

    //  commands.insert(prices.begin(), prices.end());
  }
  std::cout << "For " << monkeys.size() << ": Totalled " << numCommands << " but only " << commands.size() << " are unique\n";


  int64_t max = 0;
  double numRuns = commands.size();
  int run = 0;
  for (const auto &key : commands) {
     int64_t total = 0;
     if (run++ % 1000 == 0)
      std::cout << run-1 << "/" << numRuns << "\n";
     for (const auto &m2 : monkeys) {
      if (m2.getPrices().contains(key)) {
        auto val = m2.getPrices().at(key);
        total += val;
      }
     }
     if (total > max) {
      max = total;
     }
  }

  return {part1, max};
}
