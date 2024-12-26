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
#include <stdexcept>

using namespace aoc::util;

using Seen = std::set<Grid::coord>;
using Cheats = std::set<int>;

using Distances = std::vector<std::pair<Grid::coord, size_t>>;

void draw(Grid g, Distances& distances, Grid::Iterator at)
{
  for( auto d : distances)
  {
    g.at(d.first.first, d.first.second) = 'x';
  }
  g.at(at.coords().first, at.coords().second) = '@';
  std::cout << g << "\n";
}

Grid::Iterator nextPos(Grid::Iterator pos, Grid::Iterator forbidden) {
  for (auto dir : Grid::directions4) {
    auto candidate = pos + dir;
    if (candidate != forbidden && *candidate != '#')
      return candidate;
  }
  throw std::runtime_error("No valid dir");
}

Distances walk(Grid& grid, Grid::Iterator pos, Grid::Iterator goal)
{
  Distances ret{};

  size_t distance = 0;
  Grid::Iterator prev = pos;

  while(pos != goal)
  {
    auto next = nextPos(pos, prev);
    ret.push_back({pos.coords(), distance++});
    prev = pos;
    pos = next;
  }
  ret.push_back({pos.coords(), distance++});
  return ret;
}

Distances walk(Grid& map)
{
  auto start = map.find('S');
  auto goal = map.find('E');
  assert(start != map.end());
  assert(goal != map.end());
  return walk(map, start, goal);
}

using Shortcuts = std::map<size_t, size_t>;

auto contains(const Distances &d, Grid::coord c)
{
  return std::find_if(
      d.begin(), d.end(),
      [&c](const std::pair<Grid::coord, size_t> &p) { return p.first == c; });
}

Shortcuts findShortcuts(Grid& map, const Distances& path)
{
  Shortcuts ret{};
  for(auto loc : path)// [pos, count] : path)
  {
    auto pos = loc.first;
    auto count= loc.second;
    auto it = map.iterAt(pos);
    for(auto& d : Grid::directions4)
    {
      auto candidate = it + d + d;
      auto history = contains(path, candidate.coords());
      if(history != path.end())
      {
        auto newCost = history->second;
        if(newCost > count + 2)
        {
          ret[newCost - (count + 2)]++;
        }
      }

    }
  }
  return ret;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  auto track = Grid{input};
  assert(input.is_open());

  auto threshold = track.height < 20? 0 : 100;

  Distances path = walk(track);
  auto shortcuts = findShortcuts(track, path);

  size_t p1count = 0;
  for(auto [saving, count] : shortcuts)
    if(saving >= threshold) p1count += count;

  return {p1count, 0};
}
