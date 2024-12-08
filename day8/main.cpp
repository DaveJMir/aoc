#include "util/grid.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <iostream>
#include <iterator>
#include <map>
#include <ranges>
#include <set>
#include <ranges>

using namespace aoc::util;

bool is_tower(Grid::Iterator it) {
  return *it != '.';
}

struct TowerInfo
{
  std::set<Grid::coord> locations;
};

struct Towers
{
  std::map<char, TowerInfo> towers;
  void insert(Grid::Iterator it)
  {
    assert(!towers[*it].locations.contains(it.coords()));
    towers[*it].locations.insert(it.coords());
  }
  std::size_t size() const { return towers.size(); }
};

template<typename CB>
void foreach_pairwise(const std::set<Grid::coord>& mySet,  CB&& callback)
{
    auto indices = std::views::iota(0) | std::views::take(mySet.size());
    for (auto i : indices) {
        for (auto j : indices | std::views::drop(i + 1)) {
            auto it1 = std::next(mySet.begin(), i);
            auto it2 = std::next(mySet.begin(), j);
            callback(*it1, *it2);
        }
    }
}

std::pair<std::set<Grid::coord>, std::set<Grid::coord>> processTower(char tower, TowerInfo& info, int maxSteps)
{
  std::set<Grid::coord> primary_antinodes{};
  std::set<Grid::coord> resonant_antinodes{};
  foreach_pairwise(info.locations, [&](Grid::coord a, Grid::coord b) {
    auto delta = b - a;
    primary_antinodes.insert(b+delta);
    primary_antinodes.insert(a-delta);

    for(int i=0; i<maxSteps; i++) {
      resonant_antinodes.insert(b + i*delta);
      resonant_antinodes.insert(a - i*delta);
    }
  });
  return {primary_antinodes, resonant_antinodes};
}

std::pair<int, int> processInput(Grid &&grid) {
  Towers towers{};
  for (auto it = grid.begin(); it != grid.end(); ++it) {
    if (is_tower(it)) {
      towers.insert(it);
    }
  }

  std::set<Grid::coord> part1{};
  std::set<Grid::coord> part2{};

  auto myInbound = [&grid](Grid::coord c) { return grid.inBound(c); };
  const int maxBound = std::max(grid.bounds().first, grid.bounds().second);

  for (auto &[tower, info] : towers.towers) {
    auto [primary, resonant] = processTower(tower, info, maxBound);
    std::ranges::copy_if(primary, std::inserter(part1, part1.end()), myInbound);
    std::ranges::copy_if(resonant, std::inserter(part2, part2.end()),
                         myInbound);
  }
  return {part1.size(), part2.size()};
}

int main() {
  auto [exPart1, exPart2] = processInput(Grid{std::ifstream{"example.txt"}});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  auto [part1, part2] = processInput(Grid{std::ifstream{"input.txt"}});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}