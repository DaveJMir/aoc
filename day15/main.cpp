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
#include <unordered_set>

using namespace aoc::util;

using Map = Grid;

Map::offset instruction2dir(char instruction)
{
  switch(instruction)
  {
    case '<' : return Map::West;
    case '>' : return Map::East;
    case 'v' : return Map::South;
    case '^' : return Map::North;
    default:
    std::cout << "wtf: instruction: '" << instruction << "'\n";
  }
  assert(!"unknown instruction");
}

struct Move
{
  Map::Iterator a, b;
};


bool swap(std::vector<Move>& moves, Map::Iterator a, Map::Iterator b)
{
  moves.emplace_back(a, b);
  return true;
}

bool moveIfPossible(Map::Iterator item, Map::offset direction)
{
  auto target = item + direction;
  if(*target == '#') return false;
  if(*target == '.')
  {
    std::swap(*target, *item);
    return true;
  }
  if(*target == 'O'){
    if (moveIfPossible(target, direction)) {
      assert(*target == '.');
      std::swap(*target, *item);
      return true;
    }
    return false;
  }
  assert(!"Well then what");
}

void applyMoves(std::vector<Move>& moves)
{
  for(auto& m : moves)
  {
    std::swap(*m.a, *m.b);
  }
}

struct Box
{
  Map::coord left, right;
  constexpr bool operator<(const Box &other) const {
    return (left < other.left) || (left == other.left && right < other.right);
  }
  constexpr bool operator==(const Box &other) const {
    return (left == other.left) && (right == other.right);
  }
};
struct FatWarehouse
{
  std::set<Map::coord> blockages{};
  std::vector<Box> boxes{};
  using BoxIter = std::vector<Box>::iterator;
  Map::coord robot{};

  size_t width, height;

  explicit FatWarehouse(Map &map) {
    width = map.width*2;
    height = map.height;
    for(auto it = map.begin(); it != map.end(); ++it)
    {
      switch(*it){
        case '#':
          blockages.emplace(it.coords().first * 2 + 0, it.coords().second);
          blockages.emplace(it.coords().first * 2 + 1, it.coords().second);
          break;
        case '@':
          robot = {it.coords().first*2, it.coords().second};
          break;
        case 'O':
          boxes.push_back({{it.coords().first * 2 + 0, it.coords().second},
                           {it.coords().first * 2 + 1, it.coords().second}});
          break;
        case '.':
          break;
        default:
          assert(!"WTF");
      }
    }
  }

  Grid grid() {
    Grid ret{width, height, '.'};
    ret.at(robot.first, robot.second) = '@';
    for(auto& b : blockages)
      ret.at(b.first, b.second) = '#';
    for(auto& b : boxes){
      ret.at(b.left.first, b.left.second) = '[';
      ret.at(b.right.first, b.right.second) = ']';
    }
    return ret;
  }

  BoxIter boxAt(Map::coord coord)
  {
    return std::find_if(boxes.begin(), boxes.end(), [&coord](const Box &box) {
      return (box.left == coord || box.right == coord);
    });
  }

  bool canMoveBox(BoxIter box, Map::coord direction, std::set<BoxIter>& visited)
  {
    if(direction == Map::West)
    {
      auto target = box->left + Map::West;
      if(blockages.contains(target)) return false;
      if( auto nextBox = boxAt(target); nextBox != boxes.end())
      {
        if(!canMoveBox(nextBox, direction, visited)) return false;
      }
      visited.insert(box);
      return true;
    }

    if(direction == Map::East)
    {
      auto target = box->right + Map::East;
      if(blockages.contains(target)) return false;
      if( auto nextBox = boxAt(target); nextBox != boxes.end())
      {
        if(!canMoveBox(nextBox, direction, visited)) return false;
      }
      visited.insert(box);
      return true;
    }

    {
      auto targetLeft = box->left + direction;
      auto targetRight = box->right + direction;
      if(blockages.contains(targetLeft)) return false;
      if(blockages.contains(targetRight)) return false;

      auto nextBoxLeft = boxAt(targetLeft);
      auto nextBoxRight = boxAt(targetRight);

      if(nextBoxLeft != boxes.end()) {
        if(!canMoveBox(nextBoxLeft, direction, visited)) return false;
      }

      if(*nextBoxLeft != *nextBoxRight){
        if (nextBoxRight != boxes.end()) {
          if (!canMoveBox(nextBoxRight, direction, visited))
            return false;
        }
      }
      visited.insert(box);
      return true;
    }
  }

  bool step(char c)
  {
    auto dir = instruction2dir(c);

    auto target = robot + dir;
    if(blockages.contains(target)) return false;

    std::set<BoxIter> visited{};
    if(auto box = boxAt(target); box != boxes.end())
    {
      if(!canMoveBox(box, dir, visited)) return false;
      for(BoxIter it : visited)
      {
        it->left = it->left + dir;
        it->right = it->right + dir;
      }
    }

    robot = target;
    return true;
  }

  uint64_t gpsTotal() const
  {
    uint64_t total = 0;
    for(const auto& b : boxes)
    {
      total += b.left.second*100 + b.left.first;
    }
    return total;
  }
};

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  std::pair<std::uint64_t, std::uint64_t> ret{};

  std::string instructions{};
  auto map = Map{input};
  aoc::util::foreach_line(input, [&](std::string_view line) {
    instructions.append(line);
  });

  auto warehouse = FatWarehouse(map);



  auto robot = map.find('@');
  assert(robot != map.end());

  for (char c : instructions) {
    auto dir = instruction2dir(c);
    if (moveIfPossible(robot, dir)) {
      robot = robot + dir;
    }
  }

  for (auto pos = map.begin(); pos != map.end(); ++pos) {
    if (*pos == 'O') {
      auto coord = pos.coords();
      ret.first += (100 * coord.second + coord.first);
    }
  }

  for (char c : instructions) {
    warehouse.step(c);
  }
  ret.second = warehouse.gpsTotal();

  return ret;
}
