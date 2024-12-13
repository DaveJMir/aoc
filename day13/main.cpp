#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <complex>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <optional>
#include <ranges>
#include <set>
#include <stdexcept>

using namespace aoc::util;

bool isIntegral(double value) {
  double intPart;
  return std::modf(value, &intPart) == 0.0;
}

bool isIntegral(std::complex<double> c) {
  return isIntegral(c.real()) && isIntegral(c.imag());
}

struct Claw {
  void setA(std::pair<double,double> x    ) { A = {x.first, x.second}; }
  void setB(std::pair<double,double> x    ) { B = {x.first, x.second}; }
  void setPrize(std::pair<double,double> x) { P = {x.first, x.second}; }

  std::complex<double> pressesRequired(double offset = 0) const
  {
    auto Prize = P + std::complex<double>{offset, offset};
    double bPress = (A.real() * Prize.imag() - A.imag() * Prize.real()) /
                    (A.real() * B.imag() - A.imag() * B.real());
    double aPress = (Prize.real() - bPress * B.real()) / A.real();
    if (isIntegral(aPress) && isIntegral(bPress))
      return {aPress, bPress};
    else
      return {0, 0};
  }

  friend std::ostream &operator<<(std::ostream &os, const Claw &c) {
    return os << "(A" << c.A << ", B" << c.B << ", P" << c.P << ")";
  }

private:
  std::complex<double> A{};
  std::complex<double> B{};
  std::complex<double> P{};
};

int valAfter(std::string_view str, std::string_view after) {
  size_t startpos = str.find(after);
  assert(startpos != std::string_view::npos);
  startpos += after.length();

  int ret;
  auto num = str.substr(startpos);
  auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), ret);
  if (ec != std::errc()) {
    throw std::runtime_error("Conversion failed for token: " +
                             std::string(num));
  }
  return ret;
}

std::vector<Claw> readClaws(std::ifstream &input) {
  auto parseCoordinates = [](std::string_view line,
                             const std::string &prefix) -> std::pair<int, int> {
    int x = valAfter(line, "X" + std::string{prefix});
    int y = valAfter(line, "Y" + std::string{prefix});
    return {x, y};
  };

  std::vector<Claw> claws;
  Claw currentClaw;
  foreach_line(input, [&](std::string_view line) {
    if (line.starts_with("Button A")) {
      currentClaw.setA(parseCoordinates(line, "+"));
    } else if (line.starts_with("Button B")) {
      currentClaw.setB(parseCoordinates(line, "+"));
    } else if (line.starts_with("Prize")) {
      currentClaw.setPrize(parseCoordinates(line, "="));
      claws.push_back(currentClaw);
    } else {
      throw std::runtime_error("Unexpected line");
    }
  });
  return claws;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());

  auto totalPrice = [](const auto &claw, std::uint64_t offset = 0) {
    auto presses = claw.pressesRequired(offset);
    return std::uint64_t(presses.real()) * 3 + std::uint64_t(presses.imag());
  };

  const uint64_t offset = 10000000000000;
  auto claws = readClaws(input);
  return {std::accumulate(claws.begin(), claws.end(), 0ull,
                          [&](std::uint64_t acc, const auto &claw) {
                            return acc + totalPrice(claw);
                          }),
          std::accumulate(claws.begin(), claws.end(), 0ull,
                          [&](std::uint64_t acc, const auto &claw) {
                            return acc + totalPrice(claw, offset);
                          })};
}
