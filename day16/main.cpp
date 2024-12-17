#include "util/util.hpp"
#include "util/grid.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <ranges>
#include <set>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <utility>

using namespace aoc::util;

template<typename T>
std::set<T> append(const std::set<T>& orig, const T& next)
{
  std::set<T> ret{orig};
  ret.insert(next);
  return ret;
}

static inline Grid::coord TurnLeft(Grid::coord c)
{
  if(c == Grid::East) return Grid::North;
  if(c == Grid::North) return Grid::West;
  if(c == Grid::West) return Grid::South;
  if(c == Grid::South) return Grid::East;
  throw std::runtime_error("Can only turn from cardinal directions:");
}

static inline Grid::coord TurnRight(Grid::coord c)
{
  if(c == Grid::East) return Grid::South;
  if(c == Grid::South) return Grid::West;
  if(c == Grid::West) return Grid::North;
  if(c == Grid::North) return Grid::East;
  throw std::runtime_error("Can only turn from cardinal directions:");
}


[[maybe_unused]] static std::string_view dirStr(Grid::coord c)
{
  if(c == Grid::East) return "East";
  if(c == Grid::South) return "South";
  if(c == Grid::West) return "West";
  if(c == Grid::North) return "North";
  throw std::runtime_error("Can only toStr from cardinal directions:");
}

struct Dijkstra
{
  struct Node
  {
    Grid::coord position;
    Grid::coord direction;
    uint64_t cost;
    std::set<Grid::coord> seenPath;
    bool operator<(const Node &other) const {
      if (cost != other.cost) {
        return cost < other.cost; // Primary criterion
      }
      if (position != other.position) {
        return position < other.position; // Secondary criterion
      }
      return direction < other.direction; // Tertiary criterion
    }
  };

  bool hasSeen(const Node& node)
  {
    for (auto& n : seen)
    {
      if((n.position == node.position) && (n.direction == node.direction)) return true;
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

  void pushNode(Grid::coord pos, Grid::coord dir, uint64_t cost, std::set<Grid::coord>&& path)
  {
    Node newNode{pos, dir, cost, std::move(path)};
    if(hasSeen(newNode)) return;

    auto it = std::find_if(
        frontier.begin(), frontier.end(), [&newNode](const Node &n) {
          return (n.position == newNode.position) && (n.direction == newNode.direction);
        });
    if (it != frontier.end() && it->cost < cost) {
      // A cheaper path already exists; skip this node
      return;
    }

    if (it != frontier.end()) {
      if (it->cost == cost){ // equal cost: one inherits the path of the other
        newNode.seenPath.insert(it->seenPath.begin(), it->seenPath.end());
      }
      frontier.erase(it); // Remove the higher-cost node
    }

    frontier.insert(newNode); // Insert the new or updated node
    return;
  }

  void printMap(const Node& node, Grid grid) const {
    for (auto &n : frontier)
      grid.at(n.position.first, n.position.second) = '?';
    for( const auto& [x,y] : node.seenPath)
      grid.at(x, y) = 'O';
    grid.at(node.position.first, node.position.second) = '@';
    std::cout << grid << "\n";
  }
};

uint64_t dijkstra(Grid& map, Grid::Iterator pos, Grid::coord dir, std::set<Grid::coord>& vantages)
{
  Dijkstra state{};
  auto goal = map.find('E').coords();

  state.pushNode(pos.coords(), dir, 0, {});
  std::optional<uint64_t> minimumCost = {};

  while(!state.frontier.empty())
  {
    auto current = state.popNode();
    // If we've found a shortest route, and the current step is longer, then we
    // must have visited ALL the shortest paths already
    if (minimumCost.has_value() && current.cost > minimumCost)
    {
      break;
    }

    if(current.position == goal)
    {
      vantages.insert(current.seenPath.begin(), current.seenPath.end());
      if(minimumCost.has_value())
      {
        assert(*minimumCost == current.cost);
      } else {
        minimumCost = current.cost;
      }
      continue;
    }

    auto it = map.iterAt(current.position);
    auto tryPush = [&](const Grid::coord &offset, uint64_t costIncrement) {
      auto nextIt = it + offset;
      if (*nextIt != '#') {
        state.pushNode(nextIt.coords(), offset, current.cost + costIncrement,
                       append(current.seenPath, nextIt.coords()));
      }
    };

    auto leftDir = TurnLeft(current.direction);
    auto rightDir = TurnRight(current.direction);
    tryPush(current.direction, 1);
    tryPush(leftDir, 1001);
    tryPush(rightDir, 1001);
  }
  return minimumCost.value();
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Grid map{input};

  auto start = map.find('S');
  std::set<Grid::coord> vantages{};
  vantages.insert(start.coords());
  auto shortestPath = dijkstra(map, start, Grid::East, vantages);
  return {shortestPath, vantages.size()};
}
