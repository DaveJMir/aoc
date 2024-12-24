#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <random>
#include <ranges>
#include <set>
#include <ranges>
#include <string_view>

using namespace aoc::util;

enum class OP
{
  AND = 1,
  XOR =2,
  OR = 3,
};


struct Binop
{
  std::string a, b;
  OP op;

  Binop(const std::vector<std::string>& tokens)
  {
    a = tokens[0];
    b = tokens[2];
    if(tokens[1] == "AND") op = OP::AND;
    else if(tokens[1] == "XOR") op = OP::XOR;
    else if(tokens[1] == "OR") op = OP::OR;
    else assert(!"wtf");
  }

  uint64_t execute(uint64_t a, uint64_t b) const
  {
    switch(op)
    {
      case OP::AND: return a & b;
      case OP::XOR: return a ^ b;
      case OP::OR: return a | b;
      default:
      assert(!"WTF");
    }
  }
};

struct Device
{
  std::map<std::string, uint64_t> wires;
  std::map<std::string, Binop> ops;

  void settle()
  {
    while (!ops.empty()) {
      for (auto it = ops.begin(); it != ops.end();) {

        const auto &result = it->first;
        const auto &op = it->second;

        if (contains(op.a, op.b)) {
          wires[result] = op.execute(wires[op.a], wires[op.b]);
          it = ops.erase(it);
        }
        else it++;
      }
    }
  }

  uint64_t result() const
  {
    uint64_t result =0;
    uint64_t c = 0;
    for(auto& x : wires)
    {
      if(!x.first.starts_with('z')) continue;
      result = result | (x.second << c++);
    }

    return result;
  }

  private:
  bool contains(const std::string& s)
  {
    return wires.contains(s);
  }
  bool contains(const std::string& a, const std::string& b)
  {
    return contains(a) && contains(b);
  }
};

Device load(std::ifstream& input)
{
  Device ret;
  foreach_line(
      input,
      [&ret](std::string_view line) {
        auto colon = line.find(':');
        assert(colon != line.npos);
        auto name = line.substr(0, colon);
        auto valstr = line.substr(colon + 1);
        uint64_t value;
        auto [ptr, ec] = std::from_chars(valstr.data() + 1,
                                         valstr.data() + valstr.size(), value);
        if (ec != std::errc()) {
          throw std::runtime_error("Conversion failed for: " +
                                   std::string(valstr));
        }

        ret.wires[std::string{name}] = value;
      },
      OnEmptyLine::Halt);

  foreach_line(input, [&ret](std::string_view line) {
    std::vector<std::string> parts{};
    for(const auto part : std::ranges::views::split(line, ' '))
    {
      std::string_view token(part.begin(), part.end());
      parts.emplace_back(token);
    }

    ret.ops.emplace(parts[4], Binop{parts});
  });

  return ret;
}


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  auto device = load(input);

  device.settle();
  return { device.result(), 0};
}
