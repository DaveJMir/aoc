#include "util/util.hpp"

#include <span>
#include <string_view>
#include <vector>

using namespace aoc::util;

std::uint64_t concatenate(std::uint64_t a, std::uint64_t b) {
  std::uint64_t scale = [](std::uint64_t term) {
    if (term < 10)
      return 10;
    if (term < 100)
      return 100;
    if (term < 1000)
      return 1000;
    throw std::runtime_error("Bad term");
  }(b);
  return a * scale + b;
}

struct Equation {
  std::uint64_t answer;
  std::vector<std::uint64_t> terms;

  bool satisfyable(uint64_t val, std::span<std::uint64_t> terms,
                   bool maybeConcat) {
    if (terms.empty())
      return answer == val;
    if (val > answer)
      return false;

    const auto first = terms[0];
    const auto rest = terms.subspan(1);
    return satisfyable(val + first, rest, maybeConcat) ||
           satisfyable(val * first, rest, maybeConcat) ||
           (satisfyable(concatenate(val, first), rest, maybeConcat) &&
            maybeConcat);
  }

  bool satisfyable(bool maybeConcat) {
    const auto first = terms[0];
    auto rest = std::span{terms}.subspan(1);
    return satisfyable(first, rest, maybeConcat) ||
           satisfyable(first, rest, maybeConcat) ||
           (satisfyable(first, rest, maybeConcat) && maybeConcat);
  }
};

Equation parseEquation(std::string_view line) {
  Equation ret{};
  if (size_t colon = line.find(':'); colon != std::string_view::npos) {
    ret.answer = std::stoull(std::string{line.substr(0, colon)});
    ret.terms = string2vec<std::uint64_t>(line.substr(colon + 2), ' ');
    return ret;
  }
  throw std::runtime_error("Bad parse");
}

std::pair<uint64_t, uint64_t> processInput(std::ifstream &&input) {

  std::uint64_t part1{0};
  std::uint64_t part2{0};
  foreach_line(input, [&](std::string_view line) {
    auto eq = parseEquation(line);
    if (eq.satisfyable(false)) {
      part1 += eq.answer;
    }
    if (eq.satisfyable(true)) {
      part2 += eq.answer;
    }
  });
  return {part1, part2};
}

int main() {
  auto [exPart1, exPart2] = processInput(std::ifstream{"example.txt"});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  auto [part1, part2] = processInput(std::ifstream{"input.txt"});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}