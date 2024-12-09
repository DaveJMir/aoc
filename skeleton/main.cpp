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
#include <ranges>

using namespace aoc::util;


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  std::pair<std::uint64_t, std::uint64_t>{};
  return {};
}

int main() {
  auto [exPart1, exPart2] = process(std::ifstream{"example.txt"});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  auto [part1, part2] = process(std::ifstream{"input.txt"});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}