#include "util/util.hpp"

#include <regex>
#include <cassert>

const std::regex regex(R"(do\(\)|don't\(\)|(mul)\((\d+),(\d+)\))");

int multiply(std::string line, bool obeyDoDont) {
  std::smatch match;

  int total{0};
  bool enabled = true;
  while (std::regex_search(line, match, regex)) {
    if (match[0].str().starts_with("don't")) {
      enabled = false;
    } else if (match[0].str().starts_with("do")) {
      enabled = true;
    } else if (enabled || !obeyDoDont) {
      int num1 = std::stoi(match[2]);
      int num2 = std::stoi(match[3]);
      total += num1 * num2;
    }
    line = match.suffix();
  }
  return total;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {

  // example has two lines, one for part1 and one for part2
  // input has one line, to be used for both parts.
  auto inputs = aoc::util::splitExampleByParts(input);

  // either way, part1 is the first string and part2 is the last one
  return {multiply(inputs.front(), false), multiply(inputs.back(), true)};
}
