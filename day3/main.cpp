
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

int main() {
  std::ifstream input{"input.txt"};
  std::string inputStr((std::istreambuf_iterator<char>(input)),
                       std::istreambuf_iterator<char>());

  auto example_p1 = std::string("xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]"
                          "then(mul(11,8)mul(8,5))");

  auto example_p2 = std::string("xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,"
                                "64](mul(11,8)undo()?mul(8,5))");


  std::cout << "Example Part1: " << multiply(example_p1, false) << "\n";
  std::cout << "Example Part2: " << multiply(example_p2, true) << "\n";
  std::cout << "\n";
  std::cout << "Input Part1: " << multiply(inputStr, false) << "\n";
  std::cout << "Input Part2: " << multiply(inputStr, true) << "\n";

  return 0;
}