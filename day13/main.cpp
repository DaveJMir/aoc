#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <complex>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <ranges>
#include <set>

using namespace aoc::util;

bool isIntegral(double value) {
  double intPart;
  return std::modf(value, &intPart) == 0.0;
}

bool isIntegral(std::complex<double> c) {
  return isIntegral(c.real()) && isIntegral(c.imag());
}

struct Claw {
  void setA(double x, double y) { A = {x, y}; }
  void setB(double x, double y) { B = {x, y}; }
  void setPrize(double x, double y) { P = {x, y}; }


  std::complex<double> winPrice(double offset = 0) {
    auto Prize = P + std::complex<double>{offset, offset};
     double bPress = (A.real() * Prize.imag() - A.imag() * Prize.real()) /
                         (A.real() * B.imag() - A.imag() * B.real());
    double aPress = (Prize.real() - bPress * B.real()) / A.real();
    if (isIntegral(aPress) && isIntegral(bPress))
      return {aPress, bPress};
    else
      return {0,0};
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
  std::vector<Claw> claws;
  Claw currentClaw;
  foreach_line(input, [&claws, &currentClaw](std::string_view line) {
    if (line.starts_with("Button A")) {
      int x = valAfter(line, "X+");
      int y = valAfter(line, "Y+");
      currentClaw.setA(x, y);
      return;
    }
    if (line.starts_with("Button B")) {
      int x = valAfter(line, "X+");
      int y = valAfter(line, "Y+");
      currentClaw.setB(x, y);
      return;
    }
    assert(line.starts_with("Prize"));
    int x = valAfter(line, "X=");
    int y = valAfter(line, "Y=");
    currentClaw.setPrize(x, y);
    claws.push_back(currentClaw);
  });
  return claws;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());

  std::pair<std::uint64_t, std::uint64_t> ret{};

  auto claws = readClaws(input);

  for(auto& claw : claws)
  {
    auto presses = claw.winPrice();
    ret.first += uint64_t(presses.real()) * 3 + uint64_t(presses.imag());
  }

  std::uint64_t offset = 10000000000000;
  for(auto& claw : claws)
  {
    auto presses = claw.winPrice(offset);
    ret.second += uint64_t(presses.real()) * 3 + uint64_t(presses.imag());
  }

  return ret;
}
