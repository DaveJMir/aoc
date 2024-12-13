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
#include <ranges>

using namespace aoc::util;

bool isIntegral(double value) {
  uint64_t v = value;
  return v == value;
    // double intPart;
    // return std::modf(value, &intPart) != 0.0;
}

bool isIntegral(std::complex<double> c) {
  return isIntegral(c.real()) && isIntegral(c.imag());
}

struct Claw
{
  explicit Claw(std::uint64_t aScale) : scale{aScale} {}
  void setA(double x, double y)
  {
    A = { x, y};
  }

  void setB(double x, double y)
  {
    B = { x, y};
  }

  void setPrize(double x, double y)
  {
    P = { x*scale, y*scale};
  }

  void reset() { A = B = P = {}; }
  std::optional<std::complex<double>>solve()
  {
    double bPress = (A.real()*P.imag() - A.imag()*P.real()) / (A.real()*B.imag() - A.imag()*B.real());
    double aPress = (P.real() - bPress*B.real()) / A.real();
    if (isIntegral(aPress) && isIntegral(bPress))
      return {{aPress, bPress}};
    else
      return std::nullopt;
  }

    friend std::ostream &operator<<(std::ostream &os, const Claw &c) {
      return os << "(A" << c.A << ", B" << c.B << ", P" << c.P << ")";
    }
  private:
    std::complex<double> A{};
    std::complex<double> B{};
    std::complex<double> P{};
    std::uint64_t scale;

};

int valAfter(std::string_view str, std::string_view after)
{
  size_t startpos = str.find(after);
  assert(startpos != std::string_view::npos);
  startpos += after.length();

  auto num =  str.substr(startpos);

  int ret;
  auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), ret);
  if (ec != std::errc()) {
    throw std::runtime_error("Conversion failed for token: " +
                             std::string(num));
  }
  return ret;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());

  std::uint64_t scale = 10000000000000;

  std::pair<std::uint64_t, std::uint64_t> ret{};

  Claw claw1{1};
  Claw claw2{scale};

  foreach_line(input, [&](std::string_view line)
  {
    if( line.starts_with("Button A")){
      int x = valAfter(line, "X+");
      int y = valAfter(line, "Y+");
      claw1.setA(x,y);
      claw2.setA(x,y);
      return;
    }
    if( line.starts_with("Button B")){
      int x = valAfter(line, "X+");
      int y = valAfter(line, "Y+");
      claw1.setB(x,y);
      claw2.setB(x,y);
      return;
    }
    assert (line.starts_with("Prize"));
    int x = valAfter(line, "X=");
    int y = valAfter(line, "Y=");
    claw1.setPrize(x,y);
    claw2.setPrize(x,y);

    if (1) {
      if (auto presses = claw1.solve(); presses.has_value()) {
        std::cout << claw1 << " == " << *presses << "\tYES\n";
        ret.first += int(presses->real()) * 3 + int(presses->imag());
      } else
        std::cout << "nope\n";
    }

    if(1){
      if (auto presses = claw2.solve(); presses.has_value()) {
        std::cout << claw2 << " == " << *presses << "\tYES\n";
        ret.second += int(presses->real()) * 3 + int(presses->imag());
      } else
        std::cout << "nope\n";
    }

    claw1.reset();
    claw2.reset();
  });
  return ret;
}
