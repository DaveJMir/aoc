#include <cstdlib>
#include <fstream>
#include <set>
#include <iostream>
#include <ranges>

int main()
{
  std::ifstream input{"input.txt"};
  if(!input.good())
  {
    std::cerr << "Unable to open: intput.txt: " << std::strerror(errno) << "\n";
    return 1;
  }

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

  std::cout << "Distance  : " << distance << "\n";
  std::cout << "Similarity: " << similarity << "\n";
}
