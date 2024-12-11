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
#include <string_view>

using namespace aoc::util;

using Val = uint32_t;

void pv(auto& t)
{
  return;
  std::cout << "{";
  for(auto x : t) std::cout << " " << x;
  std::cout << " }\n";
}


int numDigits(Val n)
{
  int ret=0;
  while (n) {
    ret++;
    n /= 10;
  }
  return ret;
}

std::pair<uint64_t, uint64_t> split(Val s, int num)
{
  int scale = std::pow(10, num/2);
  return {s / scale, s%scale };
}

std::uint64_t stoneBlink(std::vector<Val>&& stones, int generation)
{
  std::cout << "generation: " << generation << "[" << stones.size() <<  "]\n";
  if(generation==0) return stones.size();

  std::vector<Val> next{};

  for(auto s : stones)
  {
    if(s==0) next.push_back(1);
    else if(int numdigits = numDigits(s); (numdigits & 1) == 0)
    {
       auto vals = split(s, numdigits);
       next.push_back(vals.first);
       next.push_back(vals.second);
    }
    else next.push_back(s*2024);
  }
  pv(next);


  return stoneBlink(std::move(next), generation-1);
}


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());

  std::pair<std::uint64_t, std::uint64_t> result{};
  int numLines = foreach_line(input, [&result](std::string_view line)
  {
    auto vals = string2vec<Val>(line);
    pv(vals);
    result.first += stoneBlink(std::move(vals), 75);
  });
  assert(numLines == 1);

  return result;
}