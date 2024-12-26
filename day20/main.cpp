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

using Distance = std::pair<Grid::coord, size_t>;
using Distances = std::vector<Distance>;

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
      [&c](const Distance& p) { return p.first == c; });
}

Shortcuts findShortcuts(Grid& map, const Distances& path, const Distances& constellation)
{
  Shortcuts ret{};
  auto total = path.size();
  int upto = 0;
  for(auto loc : path)// [pos, count] : path)
  {
    if (upto % 100 == 0)
      std::cout << "UPTO: " << upto << " / " << total << "\n";
    upto++;
    auto pos = loc.first;
    auto count= loc.second;
    auto it = map.iterAt(pos);
    for(auto& d : constellation)
    {
      auto offset = d.first;
      auto cost = d.second;
      auto candidate = it + offset;
      auto history = contains(path, candidate.coords());
      if(history != path.end())
      {
        auto newCost = history->second;
        if(newCost > count + cost)
        {
          ret[newCost - (count + cost)]++;
        }
      }

    }
  }
  return ret;
}

Distances makeConstellation(int max)
{
  Distances ret{};

  for(int x=0; x<=max; x++)
  {
    for(int y=0; y<=max-x; y++)
    {
      if(x==0 && y==0) continue;
      if(x == 0)
      {
        ret.emplace_back(Grid::coord{0, y}, x+y);
        ret.emplace_back(Grid::coord{0, -y}, x + y);
      }
      else if(y==0)
      {
        ret.emplace_back(Grid::coord{x, 0}, x+y);
        ret.emplace_back(Grid::coord{-x, 0}, x+y);
      }
      else
      {
        ret.emplace_back(Grid::coord{x, y}, x+y);
        ret.emplace_back(Grid::coord{-x, y}, x+y);
        ret.emplace_back(Grid::coord{x, -y}, x+y);
        ret.emplace_back(Grid::coord{-x, -y}, x+y);
      }
    }
  }

  return ret;
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  auto track = Grid{input};
  assert(input.is_open());

  auto threshold = track.height < 20? 50 : 100;
  auto p1Constellation = makeConstellation(2);
  auto p2Constellation = makeConstellation(20);

  Distances path = walk(track);
  auto p1Shortcuts = findShortcuts(track, path, p1Constellation);
  auto p2Shortcuts = findShortcuts(track, path, p2Constellation);

  size_t p1count = 0;
  for(auto [saving, count] : p1Shortcuts)
    if(saving >= threshold) p1count += count;

  size_t p2count = 0;
  for(auto [saving, count] : p2Shortcuts)
    if(saving >= threshold) p2count += count;

  return {p1count, p2count};
}
