#include "util/util.hpp"
#include <algorithm>
#include <numeric>
#include <ranges>
#include <cstdlib>
#include <tuple>
#include <span>

int isSafe(std::span<int> values)
{
  auto deltas =
      std::ranges::views::zip(values, values | std::ranges::views::drop(1)) |
      std::ranges::views::transform([](const auto &pair) {
        return std::get<1>(pair) - std::get<0>(pair);
      });

    auto [allPositive, allNegative, allInRange] = std::accumulate(
        deltas.begin(), deltas.end(), std::make_tuple(true, true, true),
        [](std::tuple<bool, bool, bool> acc, int x) {
            auto& [pos, neg, range] = acc;
            bool inRange = (std::abs(x) <= 3)  && (std::abs(x) >= 1);
            return std::make_tuple(pos && x > 0,
                                   neg && x < 0,
                                   range && inRange);
        });

    return (allPositive || allNegative) && allInRange;
}

int isDampenedSafe(std::span<int> values)
{
    if(isSafe(values)) return 1;

    for(auto removedPos = 0U; removedPos < values.size(); removedPos++)
    {
        std::vector<int> vec{values.begin(), values.end()};
        vec.erase(vec.begin() + removedPos);
        if(isSafe(vec)) return 1;
    }
    return 0;
}


int main()
{
  std::ifstream input{"input.txt"};

  int safeCount = 0;
  int dampCount = 0;
  aoc::util::foreach_line(input, [&](std::string_view line) {
    // std::cout << "Processed line: " << line << std::endl;
    auto report = aoc::util::string2vec<int>(line);
    safeCount += isSafe(report);
    dampCount += isDampenedSafe(report);
  });

  std::cout << "Safe count: " << safeCount << "\n";
  std::cout << "Damp count: " << dampCount << "\n";

  return 0;
}