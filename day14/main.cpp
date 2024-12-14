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
#include <complex>
#include <string_view>

using namespace aoc::util;

struct Coord {
    int x, y;

    // Constructor
    Coord(int x = 0, int y = 0) : x(x), y(y) {}

    // Arithmetic Operators
    Coord operator+(const Coord& other) const {
        return Coord(x + other.x, y + other.y);
    }

    Coord operator-(const Coord& other) const {
        return Coord(x - other.x, y - other.y);
    }

    Coord operator*(int scalar) const {
        return Coord(x * scalar, y * scalar);
    }

    Coord operator%(const Coord& mod) const {
        auto candidate = Coord(x % mod.x, y % mod.y);
        if(candidate.x < 0) candidate.x += mod.x;
        if(candidate.y < 0) candidate.y += mod.y;
        return candidate;
    }

    // Stream output for convenience
    friend std::ostream& operator<<(std::ostream& os, const Coord& c) {
        return os << "(" << c.x << ", " << c.y << ")";
    }
};

struct Robot
{
  Coord pos;
  Coord velocity;
  friend std::ostream &operator<<(std::ostream &os, const Robot &c) {
        return os << "R(" << c.pos << "@ " << c.velocity << ")";
  }
};

Coord parseCoords(std::string_view str) {
  auto vals = string2vec<int>(str, ',');
  return {vals.at(0), vals.at(1)};
}

std::vector<Robot> parseRobots(std::ifstream& input)
{
  std::vector<Robot> ret{};
  foreach_line(input, [&](std::string_view line) {
    auto idx = line.find("p=");
    assert(idx != std::string::npos);

    auto pos = parseCoords(line.begin() + idx + 2);
    idx = line.find("v=");
    assert(idx != std::string::npos);

    auto vel = parseCoords(line.begin() + idx + 2);

    ret.emplace_back(pos, vel);
  });
  return ret;
}

int position2quadrant(Coord pos, Coord size)
{
  const int halfX = size.x/2;
  const int halfY = size.y/2;

  if(pos.x < halfX)
  {
    if(pos.y < halfY)
      return 0;
    if(pos.y > halfY)
      return 3;
  }
  if (pos.x > halfX)
  {
    if(pos.y < halfY)
      return 1;
    if(pos.y > halfY)
      return 2;
  }
  return 4;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto robots = parseRobots(input);

  const auto steps = 100;
  auto roomSize = robots.size() < 20 ? Coord{11, 7} : Coord{101, 103};
  std::array quadrants = { 0,0,0,0,0};
  for (auto &robot : robots) {
    auto pos = (robot.pos + robot.velocity * steps) % roomSize;
    int quad = position2quadrant(pos, roomSize);
    quadrants[quad]++;
  }

  return {
    quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3],
    0
  };
}
