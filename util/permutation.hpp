#pragma once

#include <string>

namespace aoc::util {

// returning true will abort the search

template <typename CB>
bool generatePermutations(const std::string &symbols, int length, CB &&callback,
                          std::string soFar) {
  if (length == 0) {
    return std::invoke(std::forward<CB>(callback), soFar);
  }

  for (auto c : symbols) {
    auto result = generatePermutations(symbols, length - 1,
                                       std::forward<CB>(callback), soFar + c);
    if (result)
      return result;
  }
  return false;
}

template <typename CB>
bool generatePermutations(const std::string &symbols, int length,
                          CB &&callback) {
  return generatePermutations(symbols, length, std::forward<CB>(callback),
                              std::string{});
}

} // namespace aoc::util