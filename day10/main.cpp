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

struct Cell
{
  int height;

};

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
  std::set<Grid::coord> seen;
  int ret = countTrails(cell, seen);
  // std::cout << cell << "::Maybe: " << ret << " == " << seen.size() << "?\n";
  // for(auto s : seen)
  //   std::cout << s << "\n";
  return {seen.size(), ret};
}

 // 5, 6, 5, 3, 1, 3, 5, 3, and 5

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Grid map{std::move(input)};

  // auto iterRange = std::ranges::subrange(map.begin(), map.end());
  // auto isTrailHead = iterRange | std::ranges::views::filter([](const auto& it) {
  //       return *it == '0'; // Filter based on dereferenced iterator
  //   });

  // auto makeMapIter = [&map](Grid::Iterator::value_type cell) { return map.iterAt(cell);};
  // auto countTrailsFromCoords = [&map](

  // auto filtered = map | std::ranges::views::filter(isTrailHead);

  // auto transformed =
  //     filtered | std::ranges::views::transform([](const Grid::Iterator &cell) {
  //       return countTrails(cell); // Returns std::pair<int, int>
  //     });

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

int main() {
  auto [exPart1, exPart2] = process(std::ifstream{"example.txt"});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  if(exPart1 != 36) return -1;

  auto [part1, part2] = process(std::ifstream{"input.txt"});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}