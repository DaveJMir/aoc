#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <vector>
#include <string_view>

using namespace aoc::util;

struct Block
{
  Block(int id, int num, int total) : fileId{id}, blockNum{num}, totalBlocks{total} {}
  int fileId; // or -1 for freespace
  int blockNum;
  int totalBlocks;


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
  using BlockList = std::vector<Block>;
  explicit Hdd(std::ifstream&& input)
  {
    int lineCount = foreach_line(input, [this](std::string_view line)
    {
      bool isFile{true};
      for(auto c : line)
      {
        int fileId = (isFile)? fileMap.size() : -1;
        if(isFile)
          fileMap.push_back(blockList.size());
        int totalBlocks = c - '0';

        for( int i=0; i<totalBlocks; i++)
        {
          blockList.emplace_back(fileId, i, totalBlocks);
        }
        isFile = !isFile;
      }

    });
    assert(lineCount == 1 && "Only expect one line of input");
  }

  void refrag()
  {
    auto forwardEmpty = std::find_if(blockList.begin(), blockList.end(),
                                     [](auto &b) { return b.isEmpty(); });
    auto reverseFile = std::find_if(blockList.rbegin(), blockList.rend(),
                                    [](auto &b) { return !b.isEmpty(); });

    while (forwardEmpty != blockList.end() && reverseFile != blockList.rend()) {
      if (reverseFile.base() <= forwardEmpty) {
        break;
      }
      std::swap(*forwardEmpty, *reverseFile);
      for (++forwardEmpty;
           forwardEmpty != blockList.end() && !forwardEmpty->isEmpty();
           ++forwardEmpty)
        ;
      for (++reverseFile;
           forwardEmpty != blockList.end() && reverseFile->isEmpty();
           ++reverseFile)
        ;
    }
  }

  BlockList::iterator findFreeSpace(int blocksNeeded)
  {
    for(auto it = blockList.begin(); it != blockList.end(); ++it)
    {
      size_t count = 0;
      while(it->isEmpty() && it!=blockList.end())
      {
        count++;
        it++;
      }
      if(count >= blocksNeeded)
      {
        return it - count;
      }
      if(it == blockList.end())
        break;
    }
    return blockList.end();
  }

  void moveFile(BlockList::iterator source, BlockList::iterator dest)
  {
    int size = source->totalBlocks;
    for(int i=0; i<size; i++)
    {
      std::swap(*source, *dest);
      ++source;
      ++dest;
    }
  }

  void defrag() {
    for (auto blockIdx : std::ranges::views::reverse(fileMap)) {
      auto block = blockList.begin() + blockIdx;
      auto freeSpace = findFreeSpace(block->totalBlocks);
      if (freeSpace == blockList.end() || freeSpace > block) {
        continue;
      }
      moveFile(block, freeSpace);
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

  BlockList blockList;
  std::vector<std::size_t> fileMap;

  friend std::ostream &operator<<(std::ostream &os, const Hdd &h) {
    os << "Hdd{";
    for(auto& b : h.blockList) os<<b;
    return os<<"}";
  }
};

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  Hdd hdd_1{std::move(input)};
  Hdd hdd_2 = hdd_1;
  hdd_1.refrag();
  hdd_2.defrag();
  std::pair<std::uint64_t, std::uint64_t>{};
  return {hdd_1.checksum(), hdd_2.checksum()};
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