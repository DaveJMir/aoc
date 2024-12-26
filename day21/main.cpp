#include "util/util.hpp"
#include "util/grid.hpp"

#include <algorithm>
#include <cassert>
#include <complex>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <ranges>
#include <set>
#include <ranges>

using namespace aoc::util;

static char offset2dir(Grid::offset o)
{
  if(o == Grid::North) return '^';
  if(o == Grid::East) return '>';
  if(o == Grid::South) return 'v';
  if(o == Grid::West) return '<';
  assert(!"Bad dir");
}

struct State
{
  std::optional<int> minPath;
  std::vector<std::string> paths;
};

int findAllShortest(Grid &keypad, Grid::Iterator pos, Grid::Iterator target,
                    size_t cost, const std::string &path, State &state) {
  if(pos == target)
  {
    if(!state.minPath.has_value())
    {
      state.minPath = cost;
      state.paths.emplace_back(path+'A');
    }
    else if (cost == state.minPath.value())
    {
      state.paths.emplace_back(path+'A');
    }
  }

    if(state.minPath.has_value() && cost > state.minPath.value())
    {
      return 0;
    }

    auto tryNext = [&](Grid::offset offs)
    {
      auto candidate = pos + offs;
      if (candidate.isOutOfBounds())
      {
        return;
      }
      if(*candidate == 'X')
      {
        return;
      }
      findAllShortest(keypad, candidate, target, cost + 1,
                      path + offset2dir(offs), state);
    };

    [[maybe_unused]]auto delta = target.coords() - pos.coords();

    if(delta.first < 0)
      tryNext(Grid::West);
    if(delta.first > 0)
      tryNext(Grid::East);

    if(delta.second < 0)
      tryNext(Grid::North);
    if (delta.second > 0)
      tryNext(Grid::South);
    return 0;
}

std::vector<std::string> findAllShortest(Grid &keypad, Grid::Iterator start,
                                         Grid::Iterator target) {
  State state;
  findAllShortest(keypad, start, target, 0, "", state);
  return state.paths;
}

class KeyPad {
  Grid keypad;
  Grid::Iterator pos;

  std::map<std::pair<char,char>, uint64_t> cache{};

public:
  KeyPad *subKeys = nullptr;
  KeyPad(Grid keys) : keypad(keys), pos{keypad.find('A')} {}

  uint64_t calcPath(Grid::Iterator from, Grid::Iterator to) {
      uint64_t shortest = ~0;
      if (subKeys == nullptr) {
      for (auto &s : findAllShortest(keypad, from, to)) {
        if (s.size() < shortest)
          shortest = s.size();
      }
      } else {
      for (auto &s : findAllShortest(keypad, from, to)) {
        auto length = subKeys->pathLen(s);
        if (length < shortest)
          shortest = length;
      }
      }
      return shortest;
  }

  uint64_t pathLen(std::string_view code) {
    uint64_t sum = 0;
    for (char nextPress : code) {
     auto goal = keypad.find(nextPress);
      auto lookupKey = std::pair<char,char>{*pos, *goal};
      if(!cache.contains(lookupKey))
      {
        cache[lookupKey] = calcPath(pos, goal);
      }
      sum += cache.at(lookupKey);
      pos = goal;
    }

  return sum;
  }
};

class NumericPad : public KeyPad {
public:
  NumericPad()
      : KeyPad{std::vector<std::string>{
            "789",
            "456",
            "123",
            "X0A",
        }} {}
};

class DirectionPad : public KeyPad {
public:
  DirectionPad()
      : KeyPad{std::vector<std::string>{
            "X^A",
            "<v>",
        }} {}
};

std::vector<std::string> loadCodes(std::ifstream& input)
{
  std::vector<std::string> ret{};
  foreach_line(input,
               [&ret](std::string_view line) { ret.emplace_back(line); });
  return ret;
}

std::uint64_t minimumOuterPress(std::string_view code, int inners)
{
  int64_t min = std::numeric_limits<int64_t>::max();
  std::string sh = "";
  NumericPad pad{};

  KeyPad* prevPad = &pad;
  while(inners--)
  {
    prevPad->subKeys = new DirectionPad{};
    prevPad = prevPad->subKeys;
  }
  auto presses = pad.pathLen(code);
  return presses;
  return min;
}

std::uint64_t complexity(const std::vector<std::string> &codes, int inners) {
  uint64_t total = 0;
  for (const auto &c : codes) {
    auto complexity = minimumOuterPress(c, inners);
    uint64_t codeNum;
    auto [ptr, ec] = std::from_chars(c.data(), c.data() + c.size(), codeNum);

    if (ec != std::errc()) {
     throw std::runtime_error("Conversion failed for token: " + std::string(c));
    }
    total += codeNum * complexity;
  }
  return total;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto codes = loadCodes(input);
  return {complexity(codes,2), complexity(codes,25)};
}
