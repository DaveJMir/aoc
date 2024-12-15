
#include "util/grid.hpp"
#include "util/util.hpp"

#include <algorithm>
#include <expected>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <exception>

using namespace aoc::util;

enum class Facing
{
  UP = 'A',
  RIGHT = '>',
  DOWN = 'V',
  LEFT = '<',
};

Facing turnRight(Facing f) {
  switch (f) {
  case Facing::DOWN:
    return Facing::LEFT;
  case Facing::UP:
    return Facing::RIGHT;
  case Facing::RIGHT:
    return Facing::DOWN;
  case Facing::LEFT:
    return Facing::UP;
  }
  assert(!"WTF");
}

Grid::offset facingToDir(Facing f) {
  switch (f) {
  case Facing::DOWN:
    return Grid::South;
  case Facing::UP:
    return Grid::North;
  case Facing::RIGHT:
    return Grid::East;
  case Facing::LEFT:
    return Grid::West;
  }
  assert(!"WTF");
}


class Map
{
  public:
    using Pos = Grid::Iterator;

    explicit Map(std::ifstream& input)
        : map{input}, guard{map.find('^')}, initialGuard{guard},
          facing{Facing::UP} {
    assert(guard != map.end());
    }

    Map(const Map &other)
        : map{other.map}, guard{map.iterAt(other.guard.coords())}, initialGuard{map.iterAt(other.initialGuard.coords())},
          facing{other.facing} {}

    bool isWalkable(Grid::Iterator it) const {
      char c = *it;
      switch (c) {
      case '.':
      case 'X':
      case '^':
        return true;
      case ' ':
      case '#':
      case 'O':
        return false;
      default:
        throw std::runtime_error("Unexpected char: " + std::string{&c,1});
      }
    };

    std::expected<Pos, Pos> step()
    {
      auto direction = facingToDir(facing);
      auto next = guard + direction;
      // std::cout << guard << "\n" << map << "\n";

      if(isWalkable(next))
      {
        guard = next;
        return guard;
      }

      return std::unexpected{next};
    }

    void turn()
    {
      facing = turnRight(facing);
    }

    void reset()
    {
      guard = initialGuard;
      facing = Facing::UP;
    }

    Grid map;
    Grid::Iterator guard, initialGuard;
    Facing facing;
};


std::pair<uint64_t,uint64_t> run(Map map)
{
  std::map<std::pair<int,int>, std::set<Facing>> seenP1;

  auto initialPos = map.guard.coords();
  seenP1[initialPos].insert(map.facing);

  for (;;) {
      auto result = map.step();
      if (result) {
        seenP1[result.value().coords()].insert(map.facing);
        continue;
      }
      if (*result.error() == ' ') {
        break;
      }
      map.turn();
  };

  int loops = 0;
  for(auto [candidate, facings] : seenP1)
  {
    if(candidate== initialPos) continue;
    auto it = map.map.iterAt(candidate);
    map.reset();

    auto original = *it;
    *it = 'O';

    auto isLoop = [&]() {
      std::map<std::pair<int, int>, std::set<Facing>> seen;
      for (;;) {
        auto result = map.step();
        if (result) {
          auto location = result->coords();
          if (seen[location].contains(map.facing)) {
            return true;
          }
          seen[result.value().coords()].insert(map.facing);
          continue;
        }
        if (*result.error() == ' ') {
          return false;
        }
        map.turn();
      };
    };

    if(isLoop()) loops++;

    *it = original;
  }

  return { seenP1.size(), loops};
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  return run(Map{input});
}
