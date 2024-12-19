#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <ranges>
#include <set>
#include <string_view>
#include <type_traits>

using namespace aoc::util;

std::vector<std::string> readStock(std::ifstream &input) {
  std::string line;
  std::getline(input, line);
  std::vector<std::string> ret;

  for (const auto part : std::ranges::views::split(line, ',')) {
    std::string_view design{part.begin(), part.end()};
    design = trimmed(design);
    ret.emplace_back(design);
  }
  return ret;
}

std::vector<std::string> readDesigns(std::ifstream &input) {
  std::string line;
  std::getline(input, line); // burn the blank line

  std::vector<std::string> ret{};
  foreach_line(input, [&ret](std::string_view sv) { ret.emplace_back(sv); });
  return ret;
}

uint64_t isPossibleDesign(const std::vector<std::string> &stock,
                   std::string_view design) {
  uint64_t ret = 0;
  if (design.empty())
    return 1;

  for (const auto &s : stock) {
    if (design.starts_with(s)) {
      if (isPossibleDesign(stock, design.substr(s.size())))
        return 1;
    }
  }
  return ret;
}

uint64_t countPossibleDesigns(const std::vector<std::string> &stock,
                   std::vector<std::string> &designs) {
  uint64_t ret = 0;
  for (const auto &d : designs) {
    ret += isPossibleDesign(stock, d);
  }
  return ret;
}

using StringPair = std::pair<std::string, std::string>;
std::map<std::string_view, uint64_t> memoize{};

uint64_t totalWays(const std::vector<std::string> &stock,
                   std::string_view design) {
  if (memoize.contains(design))
    return memoize[design];

  uint64_t ret = 0;
  if (design.empty())
    return 1;

  for (const auto &s : stock) {
    if (design.starts_with(s)) {
      ret += totalWays(stock, design.substr(s.size()));
    }
  }
  memoize[design] = ret;
  return ret;
}

uint64_t totalWays(const std::vector<std::string> &stock,
                   std::vector<std::string> &designs) {
  uint64_t ret = 0;
  for (const auto &d : designs) {
    ret += totalWays(stock, d);
  }
  return ret;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto stock = readStock(input);
  auto designs = readDesigns(input);

  uint64_t achievableCount = countPossibleDesigns(stock, designs);
  uint64_t achievableWays = totalWays(stock, designs);

  return {achievableCount, achievableWays};
}
