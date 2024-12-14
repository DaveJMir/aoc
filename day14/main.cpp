#include "util/grid.hpp"
#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <complex>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <string_view>
#include <vector>

using namespace aoc::util;

struct Coord {
  int x, y;

  Coord(int x, int y) : x(x), y(y) {}
  Coord(std::pair<int, int> p) : x(p.first), y(p.second) {}

  Coord operator+(const Coord &other) const {
    return Coord(x + other.x, y + other.y);
  }

  Coord operator-(const Coord &other) const {
    return Coord(x - other.x, y - other.y);
  }

  Coord operator*(int scalar) const { return Coord(x * scalar, y * scalar); }

  Coord operator%(const Coord &mod) const {
    auto candidate = Coord(x % mod.x, y % mod.y);
    if (candidate.x < 0)
      candidate.x += mod.x;
    if (candidate.y < 0)
      candidate.y += mod.y;
    return candidate;
  }

  bool operator<(const Coord &other) const {
    return (x < other.x) || (x == other.x && y < other.y);
  }

  friend std::ostream &operator<<(std::ostream &os, const Coord &c) {
    return os << "(" << c.x << ", " << c.y << ")";
  }
};

struct Robot {
  Coord pos;
  Coord velocity;
  void step(Coord size) { pos = (pos + velocity) % size; }
  friend std::ostream &operator<<(std::ostream &os, const Robot &c) {
    return os << "R(" << c.pos << "@ " << c.velocity << ")";
  }
};

Coord parseCoords(std::string_view str) {
  auto vals = string2vec<int>(str, ',');
  return {vals.at(0), vals.at(1)};
}

std::vector<Robot> parseRobots(std::ifstream &input) {
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

int position2quadrant(Coord pos, Coord size) {
  const int halfX = size.x / 2;
  const int halfY = size.y / 2;

  if (pos.x < halfX) {
    if (pos.y < halfY)
      return 0;
    if (pos.y > halfY)
      return 3;
  }
  if (pos.x > halfX) {
    if (pos.y < halfY)
      return 1;
    if (pos.y > halfY)
      return 2;
  }
  return 4;
}

int countRowsFrom(Grid::Iterator iter, int width) {
  auto tmp = iter;
  for (int i = 0; i < width; i++, ++tmp) {
    if (*tmp != '#')
      return 0;
  }
  return 1 + countRowsFrom(iter + Grid::SouthWest, width + 2);
}

int triangleHeightAt(Grid::Iterator iter) {
  if (*iter != '#')
    return 0;
  return countRowsFrom(iter + Grid::SouthWest, 3);
}

int whenLookLikeTree(std::vector<Robot> robots, Coord size) {
  for (int i = 0; i < 100000000; i++) {
    auto map = Grid(size.x, size.y, '.');
    for (auto &r : robots) {
      map.at(r.pos.x, r.pos.y) = '#';
    }

    for (auto iter = map.begin(); iter != map.end(); ++iter) {
      int height = triangleHeightAt(iter);
      if (height >= 4) {
        return i;
      }
    }

    for (auto &r : robots) {
      r.step(size);
    }
  }
  return 0;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto robots = parseRobots(input);

  const auto steps = 100;
  bool isExample = robots.size() < 20;
  auto roomSize = isExample ? Coord{11, 7} : Coord{101, 103};
  std::array quadrants = {0, 0, 0, 0, 0};
  for (auto &robot : robots) {
    auto pos = (robot.pos + robot.velocity * steps) % roomSize;
    int quad = position2quadrant(pos, roomSize);
    quadrants[quad]++;
  }

  return {quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3],
          isExample ? 0 : whenLookLikeTree(robots, roomSize)};
}
