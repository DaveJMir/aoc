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

using RegionId = int;
using PlotSizes = std::pair<std::uint64_t, std::uint64_t>;
using Perimeters = std::map<char, std::set<Grid::coord>>;
using Visited = std::map<Grid::coord, RegionId>;

PlotSizes plotSize(Grid::Iterator plot, RegionId region, Visited &visited) {
  if (visited.contains(plot.coords()))
    return {0, 0};
  visited[plot.coords()] = region;

  uint64_t area = 1;
  uint64_t perimeter = 0;

  static const std::array<Grid::offset, 4> directions = {
      Grid::North, Grid::South, Grid::East, Grid::West};

  for (auto direction : directions) {
    auto neighbour = plot + direction;

    if (*plot == *neighbour) {
      auto next = plotSize(neighbour, region, visited);
      area += next.first;
      perimeter += next.second;
    } else {
      perimeter += 1;
    }
  }
  return {area, perimeter};
}

bool isAbove(Grid::Iterator it) { return *it != *(it+Grid::North);}
bool isBelow(Grid::Iterator it) { return *it != *(it+Grid::South);}
bool isLeft(Grid::Iterator it) { return *it != *(it+Grid::West);}
bool isRight(Grid::Iterator it) { return *it != *(it+Grid::East);}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());

  auto garden = Grid{input};

  RegionId nextRegion = 1;
  std::pair<std::uint64_t, std::uint64_t> ret{};

  std::map<RegionId, uint64_t> areas{};
  std::map<RegionId, uint64_t> edges{};
  std::map<RegionId, char> plants{};
  Visited regions{};

  // Part1: Flood fill each new region, recursively accumulating area and
  // perimeter. Also, populate the coord->region mapping, which we'll use in part2
  for (auto plot = garden.begin(); plot != garden.end(); ++plot) {
    if (regions.contains(plot.coords()))
      continue;
    auto thisRegion = nextRegion++;
    auto [area, perimeter] = plotSize(plot, thisRegion, regions);
    ret.first += (area * perimeter);
    areas[thisRegion] = area;
    plants[thisRegion] = *plot; // debug only
    regions[plot.coords()] = thisRegion;
  }

  // Part2: Walk along each row in the garden, noting when a new fence begins
  // above or below the current plot, and charge it to that region.
  // Then, do the exactly the same thing vertically walking the columns,
  // tracking when a new fence appears on our left or right.
  for (int y = 0; y < garden.height; y++) {
    struct Last {
      bool above = false;
      bool below = false;
    };
    std::map<RegionId, Last> regionLast{};
    RegionId lastRegion = 0;
    for (auto it = garden.iterAt({0, y}); (*it != ' '); it = it + Grid::East) {
      RegionId thisRegion = regions.at(it.coords());
      auto &last = regionLast[thisRegion];

      bool nextAbove = isAbove(it);
      bool newRegion = thisRegion != lastRegion;
      bool nextBelow = isBelow(it);

      if (nextAbove && !last.above)
        edges[thisRegion]++;
      else if (nextAbove && newRegion)
        edges[thisRegion]++;

      if (nextBelow && !last.below)
        edges[thisRegion]++;
      else if (nextBelow && newRegion)
        edges[thisRegion]++;

      last.above = nextAbove;
      last.below = nextBelow;
      lastRegion = thisRegion;
    }
  }

  for (int x = 0; x < garden.width; x++) {
    struct Last {
      bool left = false;
      bool right = false;
    };
    std::map<RegionId, Last> regionLast{};
    RegionId lastRegion = 0;
    for (auto it = garden.iterAt({x, 0}); (*it != ' '); it = it + Grid::South) {
      RegionId thisRegion = regions.at(it.coords());
      auto &last = regionLast[thisRegion];

      bool newRegion = thisRegion != lastRegion;
      bool nextLeft = isLeft(it);
      bool nextRight = isRight(it);

      if (nextLeft && !last.left)
        edges[thisRegion]++;
      else if (nextLeft && newRegion)
        edges[thisRegion]++;

      if (nextRight && !last.right)
        edges[thisRegion]++;
      else if (nextRight && newRegion)
        edges[thisRegion]++;

      last.left = nextLeft;
      last.right = nextRight;
      lastRegion = thisRegion;
    }
  }

  for (auto [region, edge] : edges) {
    ret.second += edges[region] * areas[region];
  }
  return ret;
}
