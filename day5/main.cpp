
#include "util/util.hpp"

#include <algorithm>
#include <list>
#include <numeric>
#include <iostream>
#include <ranges>

#include <utility>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <iostream>
#include <cassert>

using namespace aoc::util;

std::vector<int> splitStringBy(const std::string& str, char c)
{
  std::vector<int> ret{};

  std::stringstream ss{str};
  std::string tmp;

  while (getline(ss, tmp, c))
    ret.push_back(std::stoi(tmp));

  return ret;
}

bool obeys_constraint(const std::vector<int>& update, std::pair<int,int> rule)
{
  auto firstIter = std::find(update.begin(), update.end(), rule.first);
  auto secondIter = std::find(update.begin(), update.end(), rule.second);

  if(firstIter == update.end() || secondIter == update.end())
    return true;

  auto firstIdx = firstIter - update.begin();
  auto secondIdx = secondIter - update.begin();

  return firstIdx < secondIdx;
}

struct Ordering
{
  std::set<int> smaller{};
  std::set<int> larger{};
};

std::map<int, Ordering> relations{};

struct MapSorter {
  MapSorter(const std::map<int, Ordering> &relations)
      : theRelations{relations} {}

    bool operator()(int a, int b) const
    {
      const auto& ordering = theRelations.at(a);
      if(ordering.larger.contains(b))
      {
        return true;
      }

      if(ordering.smaller.contains(b))
      {
        return false;
      }

      assert(false && "wtf");
    }

private:
  const std::map<int, Ordering> &theRelations;
};

// first is middle when correct
// second is middle once fixed
// probably both wont be non-zero?
std::pair<int,int> validate_update(const std::vector<int>& update, const std::vector<std::pair<int,int>>& rules)
{
    for (const auto &rule : rules) {
    if(!obeys_constraint(update, rule))
    {
      MapSorter compare{relations};
      std::vector<int> sorted{};
      for(const auto& x : update)
      {
        auto pos =
            std::lower_bound(sorted.begin(), sorted.end(), x, compare);
        sorted.insert(pos, x);
      }
      return std::make_pair(0, sorted[update.size() / 2]);
    }
  }
  return std::make_pair(update[update.size()/2], 0);
}


int main() {
  #if 1
  std::ifstream input{"input.txt"};
  #else
  std::ifstream input{"example.txt"};
  #endif

  std::vector<std::pair<int,int>> rules{};
  std::vector<std::vector<int>> updates{};
  bool processingRules = true;

  std::string line;
  while(std::getline(input, line)) {
    auto payload = trimmed(line);
    if(payload.empty()) 
    {
      processingRules = false;
      continue;
    }

    if(processingRules){
       auto ruleVec = splitStringBy({line}, '|');
       int smaller = ruleVec[0];
       int larger = ruleVec[1];
       rules.emplace_back(smaller, larger);
       relations[smaller].larger.insert(larger);
       relations[larger].smaller.insert(smaller);

    } else {
      //  std::cout << "Update: " << line << "\n";
       updates.emplace_back(splitStringBy({line}, ','));
    }
  }

  // std::cout << "havbe " << rules.size() << "rules and " << updates.size() << "updates\n";

  int part1 = 0;
  int part2 = 0;
  for(const auto& update : updates)
  {
    auto result = validate_update(update, rules);
    part1 += result.first;
    part2 += result.second;
  }


  std::cout << "Part1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";



  return 0;
}