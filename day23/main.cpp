#include "util/util.hpp"

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

struct Computer
{
  std::string name{};
  std::set<std::string> peers;
  bool visited = false;
};

struct LanParty{
  std::map<std::string, std::set<std::string>> nodes;
};

LanParty load(std::ifstream& input)
{
  LanParty ret{};
  foreach_line(input, [&ret](std::string_view line)
  {
    auto dash = line.find('-');
    assert(dash != line.npos);
    std::string c1Name = std::string{line.substr(0,dash)};
    std::string c2Name = std::string{line.substr(dash+1)};

    ret.nodes[{c1Name}].insert(c2Name);
    ret.nodes[{c2Name}].insert(c1Name);
  });
  return ret;
}

using StringSet = std::set<std::string>;
using StringSetSet = std::set<std::set<std::string>>;

StringSetSet trioDfs(LanParty &lp, const std::string& node, StringSet seen, StringSet items)
{
  StringSetSet ret{};
  if(seen.contains(node)) return ret;
  seen.insert(node);
  items.insert(node);

  for(const auto& i : items)
  {
    // if we're not connected to all our predecessors, not a clique
    if (i!=node && !lp.nodes[node].contains(i))
      return {};
  }

  if(items.size() == 3) {
    for(const auto& s : items)
    {
      if(s.starts_with('t'))
      return {items};
    }
    return {};
  }

  const auto adjacent = lp.nodes.at(node);
  for( const auto& a : adjacent)
  {
    auto next = trioDfs(lp, a, seen, items);
    ret.insert(next.begin(), next.end());
  }
  return ret;
}


StringSetSet findTrios(LanParty& lp)
{
  StringSetSet ret{};
  for(const auto& c : lp.nodes)
  {
    auto items = dfs(lp, c.first,{}, {});
    ret.insert(items.begin(), items.end());
  }
  return ret;
}


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  auto lp = load(input);

  auto trios = findTrios(lp);
  return {trios.size(), 0};
}
