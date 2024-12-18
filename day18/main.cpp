#include "util/util.hpp"
#include "util/grid.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <ranges>
#include <vector>

using namespace aoc::util;

std::vector<Grid::coord> loadByteFalls(std::ifstream& input)
{
  std::vector<Grid::coord> points;
  foreach_line(input, [&](std::string_view line)
  {
    auto point = string2vec<int>(line, ',');
    assert(point.size() == 2);
    points.emplace_back(point[0], point[1]);
  });
  return points;
}

Grid populateMap(std::vector<Grid::coord>& points)
{
  size_t gridSize = points.size() < 50? 7 : 71;
  size_t numFalls = points.size() < 50? 12 : 1024;

  Grid ret{gridSize, gridSize, '.'};
  for( auto c : points)
  {
    if(!numFalls--) break;
    ret.at(c.first, c.second) = '#';
  }
  
  // points.erase(points.begin() + numFalls);
  return ret;
}


struct Dijkstra
{
  struct Node
  {
    Grid::coord position;
    uint64_t cost;
    bool operator<(const Node &other) const {
      if (cost != other.cost) {
        return cost < other.cost; // Primary criterion
      }
      return position < other.position; // Secondary criterion
    }
  };

  bool hasSeen(const Node& node)
  {
    for (auto& n : seen)
    {
      if((n.position == node.position)) return true;
    }
    return false;
  }

  std::set<Node> frontier{};
  std::set<Node> seen{};

  Node popNode()
  {
    Node ret = *frontier.begin();
    frontier.erase(frontier.begin());
    seen.insert(ret);
    return ret;
  }

  void pushNode(Grid::coord pos, uint64_t cost)
  {
    Node newNode{pos, cost};
    if(hasSeen(newNode)) return;

    auto it = std::find_if(
        frontier.begin(), frontier.end(), [&newNode](const Node &n) {
          return (n.position == newNode.position);
        });
    if (it != frontier.end() && it->cost <= cost) {
      // A cheaper or equal path already exists; skip this node
      return;
    }

    if (it != frontier.end()) {
      frontier.erase(it); // Remove the higher-cost node
    }

    frontier.insert(newNode); // Insert the new or updated node
    return;
  }

  void printMap(Grid::coord pos, Grid grid) const {
    for (auto &n : frontier)
      grid.at(n.position.first, n.position.second) = '?';
    for (auto &n : seen)
      grid.at(n.position.first, n.position.second) = 'O';
    grid.at(pos.first, pos.second) = '@';
    std::cout << grid << "\n";
  }
};

std::optional<uint64_t> dijkstra(Grid& map, Grid::Iterator begin, Grid::Iterator goal)
{
  Dijkstra state{};

  state.pushNode(begin.coords(), 0);

  while(!state.frontier.empty())
  {
    auto current = state.popNode();
    if(current.position == goal.coords())
    {
     return current.cost;
    }

    auto it = map.iterAt(current.position);
    auto tryPush = [&](const Grid::coord &offset, uint64_t costIncrement) {
      auto nextIt = it + offset;
      if (*nextIt == '.') {
        state.pushNode(nextIt.coords(), current.cost + 1);
      }
    };

    tryPush(Grid::North, 1);
    tryPush(Grid::East, 1);
    tryPush(Grid::South, 1);
    tryPush(Grid::West, 1);
  }

  return std::nullopt;
}

uint64_t maxFalls(Grid& map, Grid::Iterator begin, Grid::Iterator goal, const std::vector<Grid::coord> falling)
{
  int upto =0;
  for( auto c : falling)
  {
    if (upto++ % 10 == 0)
     std::cout << "Byte: " << upto-1 << "/" << falling.size() << "\n";
    if(upto < 1024) continue;
    map.at(c.first, c.second) = '#';
    if( !dijkstra(map, begin, goal).has_value())
    {
      return c.first * 1000 + c.second;
    }
  }
  return 0;
}


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  auto points = loadByteFalls(input);
  Grid map = populateMap(points);
  std::cout << map << "\n";
  auto start = map.begin();
  auto goal = map.last();

  auto min = dijkstra(map, start, goal).value();
  auto firstFail =  maxFalls(map, start, goal, points);

  return { min, firstFail };
}
