
#include "util/util.hpp"

#include <algorithm>
#include <expected>
#include <iostream>
#include <iterator>
#include <map>
#include <numeric>
#include <ranges>
#include <set>
#include <string>
#include <vector>

using namespace aoc::util;
using Rules = std::map<int, std::set<int>>;
using Result = std::expected<int, int>;
using Update = std::vector<int>;

Result process_update(const Update& update, Rules rules)
{
  const int middleIndex = update.size() / 2;
  auto compareByRules = [&rules](int a, int b) -> bool {
    return rules[b].contains(a);
  };

  if (std::adjacent_find(update.begin(), update.end(), compareByRules) ==
      update.end()) {
    return update[middleIndex];
  }

  std::vector<int> sorted = update;
  std::sort(sorted.begin(), sorted.end(), compareByRules);
  return std::unexpected(sorted[middleIndex]);
}

int main() {
#if 1
  std::ifstream input{"input.txt"};
#else
  std::ifstream input{"example.txt"};
#endif

  Rules rules{};
  std::vector<Update> updates{};

  std::string line;
  bool readingRules = true;
  while (std::getline(input, line)) {
    auto payload = trimmed(line);
    if (payload.empty()) {
      readingRules = false;
      continue;
    }

    if (readingRules) {
      auto ruleVec = string2vec<int>(line, '|');
      rules[ruleVec[0]].insert(ruleVec[1]);
    } else {
      updates.emplace_back(string2vec<int>(line, ','));
    }
  }

  auto results =
      updates | std::ranges::views::transform([&](const auto &update) {
        return process_update(update, rules);
      });

  auto [part1, part2] = std::accumulate(
      results.begin(), results.end(), std::pair<int, int>{0, 0},
      [](std::pair<int, int> acc, const Result& result) {
        if (result) {
          acc.first += result.value();
        } else {
          acc.second += result.error();
        }
        return acc;
      });

  std::cout << "Part1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";
  return 0;
}