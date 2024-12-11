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

    int direction = 0;
    for (auto x : deltas) {
      if ((std::abs(x) > 3) || (std::abs(x) < 1)) return 0;
      int sign = x / std::abs(x);
      if ((direction * sign) == -1) return 0;
      direction = sign;
    }

    return 1;
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


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  int safeCount = 0;
  int dampCount = 0;
  aoc::util::foreach_line(input, [&](std::string_view line) {
    auto report = aoc::util::string2vec<int>(line);
    safeCount += isSafe(report);
    dampCount += isDampenedSafe(report);
  });

  return { safeCount, dampCount };
}
