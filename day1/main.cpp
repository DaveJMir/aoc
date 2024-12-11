#include <cstdlib>
#include <fstream>
#include <set>
#include <iostream>
#include <ranges>

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  std::multiset<int> leftList, rightList;
  int left, right;
  while (input >> left >> right) {
    leftList.insert(left);
    rightList.insert(right);
  }

  int distance = 0;
  int similarity = 0;
  for (auto [left, right] : std::ranges::views::zip(leftList, rightList)) {
    distance += std::abs(left - right);
    similarity += left * rightList.count(left);
  }

  return { distance, similarity};
}
