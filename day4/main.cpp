
#include "util/grid.hpp"

#include <algorithm>
#include <numeric>
#include <iostream>
#include <ranges>

#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <cassert>

using Grid = aoc::util::Grid;

int find_xmas(Grid::Iterator it, const Grid::offset direction, std::string_view target)
{
  if(target.empty()) return 1;
  if(it.isOutOfBounds()) return 0;
  if(*it != target.front()) return 0;
  return find_xmas(it + direction, direction, target.substr(1));
}

int find_xmas(Grid::Iterator it)
{
  if(*it != 'X') return 0;
  static const std::array<Grid::offset, 8> directions = {
      Grid::North,     Grid::South,     Grid::East,      Grid::West,
      Grid::NorthEast, Grid::NorthWest, Grid::SouthEast, Grid::SouthWest};

  auto partialSums = directions | std::ranges::views::transform(
                                      [&](const Grid::offset &direction) {
                                        return find_xmas(it+direction, direction, "MAS");
                                      });
  return std::accumulate(partialSums.begin(), partialSums.end(), 0);
}

int find_x_mas(Grid::Iterator it)
{
  static const std::set<char> validXLegs = {'M', 'S'};
  if(*it != 'A') return 0;

  char ne = *(it+Grid::NorthEast);
  char sw = *(it+Grid::SouthWest);
  char nw = *(it+Grid::NorthWest);
  char se = *(it+Grid::SouthEast);

  auto values = {ne, sw, nw, se};
  return std::all_of(values.begin(), values.end(),
                     [&](char c) { return validXLegs.contains(c); }) &&
         ne != sw && nw != se;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  int total_xmas{0};
  int total_x_mas{0};
  Grid grid{input};
  for(auto it = grid.begin(); it != grid.end(); ++it)
  {
    total_xmas += find_xmas(it);
    total_x_mas += find_x_mas(it);
  }
  return {total_xmas, total_x_mas};
}
