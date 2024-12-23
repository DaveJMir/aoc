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
  Grid keypad{3, 4, '.'};
public:
  KeyPad(Grid keys) : keypad(keys) {}

  std::vector<std::string> pressPaths(std::string_view code) {
    auto pos = keypad.find('A');

    std::vector<std::string> ret{};

    for (char k : code) {
     auto dest = keypad.find(k);
     auto paths = findAllShortest(keypad, pos, dest);
     pos = dest;

     if (ret.size() == 0) {
        std::swap(ret, paths);
     } else {
        std::vector<std::string> product{};
        product.reserve(ret.size() * paths.size());
        for (const auto &a : ret)
          for (const auto &b : paths)
            product.emplace_back(a + b);
        std::swap(ret, product);
     }
    }
    return ret;
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

std::uint64_t minimumOuterPress(std::string_view code)
{
  int64_t min = std::numeric_limits<int64_t>::max();
  int64_t count =0;
  std::string sh = "";
  NumericPad pad{};
  auto presses = pad.pressPaths(code);
  DirectionPad p1{};
  for (const auto &p : presses) {
    auto p1presses = p1.pressPaths(p);
     DirectionPad p2{};
    for (const auto &q : p1presses) {
     auto p2presses = p2.pressPaths(q);
     for (const auto &x : p2presses) {
      count++;
        if (x.size() < min) {
          min = x.size();
          sh = x;
        }
     }
    }
  }
  std::cout << "Code: " << code << ":" << "Len: " << min << ": " << sh << "\n";
  std::cout << "Considered " << count << " options!\n";
  return min;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto codes = loadCodes(input);


  uint64_t total = 0;
  for (const auto &c : codes) {
    auto complexity = minimumOuterPress(c);
    uint64_t codeNum;
    auto [ptr, ec] = std::from_chars(c.data(), c.data() + c.size(), codeNum);

    if (ec != std::errc()) {
     throw std::runtime_error("Conversion failed for token: " + std::string(c));
    }
    total +=  codeNum * complexity;
  }

  return {total, 0};
}
