#include "util/util.hpp"
#include "util/grid.hpp"

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

using namespace aoc::util;

int countTrails(Grid::Iterator cell, std::set<Grid::coord> &seen) {
  int height = *cell - '0';
  if (height == 9) {
    seen.insert(cell.coords());
    return 1;
  }

  std::array dirs{cell + Grid::North, cell + Grid::East, cell + Grid::South,
                  cell + Grid::West};

  int ret = 0;
  for (auto adjacent : dirs) {
    if (*adjacent == (*cell) + 1) {
      ret += countTrails(adjacent, seen);
    }
  }
  return ret;
}

std::pair<int,int> countTrails(Grid::Iterator cell)
{
  std::set<Grid::coord> uniquePeaks;
  int possiblePaths = countTrails(cell, uniquePeaks);
  return {uniquePeaks.size(), possiblePaths};
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Grid map{std::move(input)};

  std::pair<std::uint64_t, std::uint64_t> ret{};
  for( auto cell = map.begin(); cell != map.end(); ++cell)
  {
    if(*cell == '0') {
      std::pair<int,int> score  = countTrails(cell);
      ret.first += score.first;
      ret.second += score.second;
    }
  }
  return ret;
}