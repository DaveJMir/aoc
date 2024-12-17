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

  void pushNode(Grid::coord pos, Grid::coord dir, uint64_t cost)
  {
    Node newNode{pos, dir, cost};
    if(hasSeen(newNode)) return;

    auto it = std::find_if(
        frontier.begin(), frontier.end(), [&newNode](const Node &n) {
          return (n.position == newNode.position) && (n.direction == newNode.direction);
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
    return;
    for (auto &n : frontier)
      grid.at(n.position.first, n.position.second) = '?';
    grid.at(pos.first, pos.second) = '@';
  }
};

uint64_t dijkstra(Grid& map, Grid::Iterator pos, Grid::coord dir)
{
  Dijkstra state{};
  auto goal = map.find('E').coords();

  state.pushNode(pos.coords(), dir, 0);

  while(!state.frontier.empty())
  {
    auto current = state.popNode();
    if(current.position == goal) return current.cost;

    auto it = map.iterAt(current.position);
    auto tryPush = [&](const Grid::coord &offset, uint64_t costIncrement) {
      auto nextIt = it + offset;
      if (*nextIt != '#') {
        state.pushNode(nextIt.coords(), offset, current.cost + costIncrement);
      }
    };

    auto leftDir = TurnLeft(current.direction);
    auto rightDir = TurnRight(current.direction);
    tryPush(current.direction, 1);
    tryPush(leftDir, 1001);
    tryPush(rightDir, 1001);
  }
  throw std::runtime_error("No path found");
}


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Grid map{input};

  auto start = map.find('S');
  auto end = map.find('E');
  std::cout << "FROM: " << start << "TO " << end << "\n";

  return {// shortestPath(start, end, map),
          dijkstra(map, start, Grid::East), 0};
}
