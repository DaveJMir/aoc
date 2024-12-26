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

using Lock = std::array<int,5>;
using Key = std::array<int,5>;

struct Puzzle
{
  std::vector<Lock> locks;
  std::vector<Key> keys;
};

Puzzle load(std::ifstream &input) {
  Puzzle ret;

  bool isFirst = true;
  bool isKey;

  std::array<int, 5> hashCount;
  auto flush = [&]() {
    if (isKey) {
      // dont count the base row of a key
      for (auto &hc : hashCount)
        hc--;
      ret.keys.push_back(hashCount);
    } else
      ret.locks.push_back(hashCount);
    isFirst = true;
  };

  foreach_line(
      input,
      [&](std::string_view line) {
        if (isFirst) {
          isKey = line[0] == '.';
          isFirst = false;
          hashCount.fill(0);
          return;
        }
        if (line.empty()) {
          flush();
          return;
        }
        for (int i = 0; i < 5; i++) {
          hashCount[i] += line[i] == '#' ? 1 : 0;
        }
      },
      OnEmptyLine::PassThrough);
  flush();

  std::cout << "Loaded " << ret.keys.size() << " keys and " << ret.locks.size()
            << " locks\n";
  return ret;
}

bool doesFit2(const Key& key, const Lock& lock)
{
  for(int i=0; i<5; i++)
  {
    for(int j=0; j<=i; j++)
    {
      int l = lock[5 - i+j];
      int k = key[i+j];
      if(l+k > 5) return false;
    }
  }

  return true;
}
bool doesFit(const Key& key, const Lock& lock)
{
  for(int i=0; i<5; i++)
  {
      int l = lock[i];
      int k = key[i];
      if(l+k > 5) return false;
  }

  return true;
}

std::ostream& operator<<(std::ostream& os, Lock l)
{
  os << "{ ";
  os << l[0] << " ";
  os << l[2] << " ";
  os << l[3] << " ";
  os << l[4] << " ";
  os << l[5] << " ";
  return os << "}";
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto puzzle = load(input);

  int part1 = 0;
  for (auto &lock : puzzle.locks) {
    for (auto &key : puzzle.keys) {
      if (doesFit(key, lock)) {
        part1++;
      }
    }
  }
  return {part1, 0};
}
