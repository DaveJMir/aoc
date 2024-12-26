#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <charconv>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <random>
#include <ranges>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

using namespace aoc::util;

using Patch = std::pair<std::string, const std::string>;

enum class OP {
  AND = 1,
  XOR = 2,
  OR = 3,
};

std::string_view op2str(OP op) {
  switch (op) {
  case OP::AND:
    return "AND";
  case OP::XOR:
    return "XOR";
  case OP::OR:
    return "OR";
  default:
    assert(!"WTF");
  }
}

struct Binop {
  std::string a, b, result;
  OP op;

  Binop(const std::vector<std::string> &tokens) {
    a = tokens[0];
    b = tokens[2];
    result = tokens[4];
    if (tokens[1] == "AND")
      op = OP::AND;
    else if (tokens[1] == "XOR")
      op = OP::XOR;
    else if (tokens[1] == "OR")
      op = OP::OR;
    else
      assert(!"wtf");
  }

  uint64_t execute(uint64_t a, uint64_t b) const {
    switch (op) {
    case OP::AND:
      return a & b;
    case OP::XOR:
      return a ^ b;
    case OP::OR:
      return a | b;
    default:
      assert(!"WTF");
    }
  }

  friend std::ostream &operator<<(std::ostream &os, const Binop &op) {
    return os << "Op{" << op.a << " " << op2str(op.op) << " " << op.b
              << "} == " << op.result;
  }
};

struct Device {
  std::bitset<45> _x{}, _y{}, _z{};
  std::vector<Binop> ops;

  Binop &getOp(const std::string &name) {
    auto ret = std::find_if(ops.begin(), ops.end(), [&name](const Binop &bo) {
      return name == bo.result;
    });
    assert(ret != ops.end());
    return *ret;
  }

  std::uint64_t x() const { return _x.to_ullong(); }
  std::uint64_t y() const { return _y.to_ullong(); }
  std::uint64_t z() const { return _z.to_ullong(); }
  void x(uint64_t v) { _x = v; }
  void y(uint64_t v) { _y = v; }
  void z(uint64_t v) { _z = v; }

  std::bitset<45>::reference decode(std::string_view name) {
    char reg = name[0];
    uint64_t bitNum;
    auto [ptr, ec] = std::from_chars(name.data() + 1, name.data() + 3, bitNum);
    if (ec != std::errc()) {
      throw std::runtime_error("Conversion failed for: " + std::string(name));
    }
    if (reg == 'x')
      return _x[bitNum];
    else if (reg == 'y')
      return _y[bitNum];
    else if (reg == 'z')
      return _z[bitNum];
    else
      assert(!"wtf");
  }

  bool isInputReg(std::string_view name) {
    switch (name[0]) {
    case 'x':
    case 'y':
      return true;
    default:
      return false;
    }
  }

  void set(std::string_view name, int val) { decode(name) = val; }

  int execute(const std::string &name) {
    if (isInputReg(name)) {
      return decode(name);
    }

    auto &op = getOp(name);
    return op.execute(execute(op.a), execute(op.b));
  }

  uint64_t run() {
    for (auto &op : ops) {
      const auto &name = op.result;
      if (!name.starts_with('z'))
        continue;
      decode(name) = execute(name);
    }
    return z();
  }

  void toFile(std::ofstream &&of) {
    for (int i = 0; i < 45; i++) {
      of << std::format("x{:02}", i) << ": " << _x[i] << "\n";
      of << std::format("y{:02}", i) << ": " << _y[i] << "\n";
      of << std::format("z{:02}", i) << ": " << _z[i] << "\n";
    }
    of << "\n";
    for (const auto &op : ops) {
      of << op.a << " " << op2str(op.op) << " " << op.b << " -> " << op.result
         << "\n";
    }
    of << "\n";
  }

  void rename(const std::string old, const std::string replacement) {
    if (old == replacement)
      return;
    for (auto &op : ops) {
      if (op.a == old)
        op.a = replacement;
      if (op.b == old)
        op.b = replacement;
      if (op.result == old)
        op.result = replacement;
    }
  }

  Binop &find(const std::string &a, const std::string &b, OP theOp) {
    auto it = std::find_if(ops.begin(), ops.end(), [&](const Binop &op) {
      if (op.a == a && op.b == b && op.op == theOp)
        return true;
      if (op.a == b && op.b == a && op.op == theOp)
        return true;
      return false;
    });
    if (it == ops.end()) {
      std::cout << "Cannot find: " << a << " " << op2str(theOp) << " " << b
                << "\n";
      throw std::runtime_error("Unabelt or find\n");
    }

    return *it;
  }

  template <size_t N> void applyPatches(std::array<Patch, N> patches) {
    for (const auto &p : patches) {
      Binop &firstOp = getOp(p.first);
      Binop &secondOp = getOp(p.second);
      firstOp.result = p.second;
      secondOp.result = p.first;
    }
  }
};

Device load(std::ifstream &input) {
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

        ret.set(name, value == 1);
      },
      OnEmptyLine::Halt);

  foreach_line(input, [&ret](std::string_view line) {
    std::vector<std::string> parts{};
    for (const auto part : std::ranges::views::split(line, ' ')) {
      std::string_view token(part.begin(), part.end());
      parts.emplace_back(token);
    }

    ret.ops.emplace_back(parts);
  });

  return ret;
}

void annotate(Device &device) {
  auto &sum00 = device.find("x00", "y00", OP::XOR);
  auto &cry00 = device.find("x00", "y00", OP::AND);

  device.rename(sum00.result, "sum00");
  device.rename(cry00.result, "carry00");

  for (int i = 1; i < 45; i++) {
    auto carryInName = std::format("carry{:02}", i - 1);
    auto xName = std::format("x{:02}", i);
    auto yName = std::format("y{:02}", i);

    Binop &halfSum = device.find(xName, yName, OP::XOR);
    device.rename(halfSum.result, std::format("hsum{:02}", i));

    Binop &fullSum = device.find(halfSum.result, carryInName, OP::XOR);
    if (!fullSum.result.starts_with('z')) {
      std::cout << "ERROR! Should be a z output: " << fullSum << " !!!!\n";
      throw std::runtime_error("Bad Z");
    }

    Binop &ca = device.find(halfSum.result, carryInName, OP::AND);
    device.rename(ca.result, std::format("ca{:02}", i));
    Binop &cb = device.find(xName, yName, OP::AND);
    device.rename(cb.result, std::format("cb{:02}", i));

    Binop &carry = device.find(ca.result, cb.result, OP::OR);
    device.rename(carry.result, std::format("carry{:02}", i));
  }
}

std::string fixDevice(Device device) {
  try {
    annotate(device);
  } catch (std::runtime_error &er) {
    std::cout << "Failed: " << er.what() << "\n";
    device.toFile(std::ofstream{"annotated.txt"});
  }
  return "";
}

std::string sortPatchedWireNames(std::span<Patch> patches) {
  std::set<std::string_view> names{};
  for (const auto &patch : patches) {
    names.insert(patch.first);
    names.insert(patch.second);
  }
  std::stringstream ss{};
  for (auto &n : names) {
    if (n != *names.begin())
      ss << ",";
    ss << n;
  }
  return ss.str();
}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream &&input) {
  assert(input.is_open());
  auto device = load(input);
  auto part1 = device.run();

  if (device.ops.size() < 50) {
    // no adder in example
    return {part1, 0};
  }

  // else, full input can run Part2
  // These were obtained by running `device.annotate()`, seeing what asserted and swapping
  // net names it complained about. Not yet automated, but we can at least validate
  assert(device.x() + device.y() != device.z());
  std::array<Patch, 4> patches = {
      std::make_pair("cvp", "wjb"),
      std::make_pair("z34", "wcb"),
      std::make_pair("mkk", "z10"),
      std::make_pair("qbw", "z14"),
  };

  device.applyPatches(patches);
  assert(device.x() + device.y() == device.run());

  std::string answer = sortPatchedWireNames(patches);
  auto p2Hash = std::hash<std::string>{}(answer);
  return {part1, p2Hash};
}
