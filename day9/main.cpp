#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <numeric>
#include <ranges>
#include <vector>
#include <ranges>
#include <string_view>

using namespace aoc::util;

struct Block
{
  Block(int id) : fileId{id} {}
  int fileId; // or -1 for freespace

  bool isEmpty() const { return fileId == -1; }

  friend std::ostream &operator<<(std::ostream &os, const Block &b) {
    if(b.fileId == -1)
      os << '.';
    else
      os << b.fileId;
    return os;
  }
};

struct Hdd
{
  explicit Hdd(std::ifstream&& input)
  {

    int lineCount = foreach_line(input, [this](std::string_view line)
    {
      bool isFile{true};
      for(auto c : line)
      {
        int fileId = (isFile)? fileCount++ : -1;

        for( int i=0; i<c-'0'; i++)
        {
          blockList.emplace_back(fileId);
        }
        isFile = !isFile;
      }

    });
    assert(lineCount == 1);
  }

  void defrag()
  {

    auto forwardEmpty = std::find_if(blockList.begin(), blockList.end(),
                                     [](auto &b) { return b.isEmpty(); });
    auto reverseFile = std::find_if(blockList.rbegin(), blockList.rend(),
                                    [](auto &b) { return !b.isEmpty(); });

    // Perform swapping
    while (forwardEmpty != blockList.end() && reverseFile != blockList.rend()) {
      if (reverseFile.base() <= forwardEmpty) {
        break;
      }
        std::swap(*forwardEmpty, *reverseFile);
        for(++forwardEmpty; !forwardEmpty->isEmpty() && forwardEmpty!=blockList.end(); ++forwardEmpty);
        for(++reverseFile; reverseFile->isEmpty() && reverseFile!=blockList.rend(); ++reverseFile);
    }
  }

  std::uint64_t checksum() const
  {
    auto checksums =
        std::ranges::views::zip(std::ranges::views::iota(0), blockList) |
        std::ranges::views::transform([](const auto &pair) -> std::uint64_t {
          auto [index, block] = pair;
          return block.isEmpty() ? 0 : index * block.fileId;
        });

    std::uint64_t ret{0};
    for(auto checksum : checksums) ret += checksum;
    return ret;
  }

  std::vector<Block> blockList;
  int fileCount{0};

  friend std::ostream &operator<<(std::ostream &os, const Hdd &h) {
    os << "Hdd{";
    for(auto& b : h.blockList) os<<b;
    return os<<"}";
  }
};


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Hdd hdd{std::move(input)};
  hdd.defrag();
  std::pair<std::uint64_t, std::uint64_t>{};
  return {hdd.checksum(), 0};
}

int main() {
  auto [exPart1, exPart2] = process(std::ifstream{"example.txt"});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  auto [part1, part2] = process(std::ifstream{"input.txt"});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}