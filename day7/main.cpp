#include "util/util.hpp"
#include "util/permutation.hpp"

#include <vector>
#include <string_view>
#include <ranges>
#include <set>
#include <span>

using namespace aoc::util;

struct Equation
{
  std::uint64_t answer;
  std::vector<std::uint64_t> terms;

  bool isSatisfied(std::string_view operators) const
  {
    auto op = operators.begin();

    auto result = terms[0];
    for (auto term : terms | std::views::drop(1)) {
      switch (*op) {
      case '+':
        result += term;
        break;
      case '*':
        result *= term;
        break;
      default:
        throw std::runtime_error("Bad operator");
      }
      op++;
    }

    if(result == answer)
    {
      return true;
    }
      // std::cout << "FALSe!\n";
    return false;
  }

  bool satisfyable() const {
    int numSlots = terms.size() - 1;
    return generatePermutations("*+", numSlots, [this](std::string_view perm) {
      return isSatisfied(perm);
    });
  }
};

Equation parseEquation(std::string_view line)
{
  Equation ret{};
  if(size_t colon = line.find(':'); colon != std::string_view::npos){
    ret.answer = std::stoull(std::string{line.substr(0, colon)});
    ret.terms = string2vec<std::uint64_t>(line.substr(colon+2), ' ');
    return ret;
  }
  throw std::runtime_error("Bad parse");
}

void ps(const auto& set)
{
  std::cout <<"{";
  for(auto i : set) std::cout << " " << i;
  std::cout << " }\n";
}

bool cheat(const Equation &eq) {
  std::set<std::uint64_t> candidates{};
    candidates.insert(eq.terms[0]);

  for(auto term : eq.terms|std::views::drop(1))
  {
    ps(candidates);
    std::cout << "+*" << term << "\n";
    std::set<std::uint64_t> newCandidates{};
    for(auto candidate : candidates)
    {
      if(candidate*term <= eq.answer) newCandidates.insert(candidate * term);
      if(candidate+term <= eq.answer) newCandidates.insert(candidate + term);
    }
    candidates = newCandidates;
  }
  ps(candidates);
  return candidates.contains(eq.answer);
}

bool redux(std::uint64_t answer, uint64_t val, std::span<std::uint64_t> terms) {
  std::cout << "Consider: "<< answer << " == " << val << "?"; ps(terms);
  if (terms.empty())
    return answer == val;


  if (val > answer)
    return false;
  return redux(answer, val + terms[0], terms.subspan(1)) ||
         redux(answer, val * terms[0], terms.subspan(1));
}

std::pair<uint64_t, uint64_t> processInput(std::ifstream&& input){

  std::uint64_t part1{0};
  foreach_line(input, [&](std::string_view line) {
    auto eq = parseEquation(line);

    auto og = eq.satisfyable();
    if(og)
    {
      part1 += eq.answer;
    }
    else{
    }
  });
  return {part1,0};
}



int main() {
  auto [exPart1, exPart2] = processInput(std::ifstream{"example.txt"});
  std::cout << "Example Part1: " << exPart1 << "\n";
  std::cout << "Example Part2: " << exPart2 << "\n";

  auto [part1, part2] = processInput(std::ifstream{"input.txt"});
  std::cout << "\nPart1: " << part1 << "\n";
  std::cout << "Part2: " << part2 << "\n";

  return 0;
}