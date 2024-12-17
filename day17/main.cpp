#include "util/util.hpp"

#include <algorithm>
#include <cassert>
#include <expected>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <ostream>
#include <ranges>
#include <set>
#include <ranges>
#include <stdexcept>

using namespace aoc::util;

/*
The adv instruction (opcode 0) performs division.
The numerator is the value in the A register.
The denominator is found by raising 2 to the power of the instruction's combo operand.
(So, an operand of 2 would divide A by 4 (2^2); an operand of 5 would divide A by 2^B.)
The result of the division operation is truncated to an integer and then written to the A register.

The bxl instruction (opcode 1) calculates the bitwise XOR of register B and the instruction's literal operand, then stores the result in register B.

The bst instruction (opcode 2) calculates the value of its combo operand modulo 8 (thereby keeping only its lowest 3 bits), then writes that value to the B register.

The jnz instruction (opcode 3) does nothing if the A register is 0. However, if the A register is not zero, it jumps by setting the instruction pointer to the value of its literal operand; if this instruction jumps, the instruction pointer is not increased by 2 after this instruction.

The bxc instruction (opcode 4) calculates the bitwise XOR of register B and register C, then stores the result in register B. (For legacy reasons, this instruction reads an operand but ignores it.)

The out instruction (opcode 5) calculates the value of its combo operand modulo 8, then outputs that value. (If a program outputs multiple values, they are separated by commas.)

The bdv instruction (opcode 6) works exactly like the adv instruction except that the result is stored in the B register. (The numerator is still read from the A register.)

The cdv instruction (opcode 7) works exactly like the adv instruction except that the result is stored in the C register. (The numerator is still read from the A register.)


If register C contains 9, the program 2,6 would set register B to 1.
If register A contains 10, the program 5,0,5,1,5,4 would output 0,1,2.
If register A contains 2024, the program 0,1,5,4,3,0 would output 4,2,5,6,7,7,7,7,3,1,0 and leave 0 in register A.
If register B contains 29, the program 1,7 would set register B to 26.
If register B contains 2024 and register C contains 43690, the program 4,0 would set register B to 44354.
*/

enum class INST
{
  ADV = 0,
  BXL = 1,
  BST = 2,
  JNZ = 3,
  BXC = 4,
  OUT = 5,
  BDV = 6,
  CDV = 7,

  HALT = 100,
};

struct Computer
{
    int64_t A, B, C;
    int64_t PC{0};

    std::vector<int64_t> outputs;
    std::vector<int> program;
    bool halted = false;

    friend std::ostream& operator<<(std::ostream& os, const Computer& pc)
    {
      os << "Computer{ PC="<<pc.PC<<", A=" << pc.A << ", B=" << pc.B << ", C= " << pc.C << ",";
      os << " Program=";
      for(auto v : pc.program ) os << " " << v;
      os << ",    Output=";
      for(auto v : pc.outputs ) os << " " << v;
      return os << " }";
    }

    int64_t COMBO()
    {
      auto op = OPERAND();
      switch(op){
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return A;
        case 5: return B;
        case 6: return C;
      }
      throw std::runtime_error("Bad comboval");
    }

    int64_t OPERAND() const
    {
      return program.at(PC+1);
    }

    INST OPCODE()
    {
      if(PC >= program.size())
        return INST::HALT;

      return INST{program.at(PC)};
    }

    void run()
    {
      while(!halted)
      {
        std::cout << *this << "\n";
        auto inst = OPCODE();
        switch(inst)
        {
          case INST::ADV:
          std::cout << "ADV\n";
            A = A / pow(2,COMBO());
            PC += 2;
            break;

          case INST::BXL:
          std::cout << "BXL\n";
            B = B ^ OPERAND();
            PC += 2;
            break;

          case INST::BST:
          std::cout << "BST: B=" << COMBO() << "&7\n";
            B = COMBO() % 8;
            PC += 2;
            break;

          case INST::JNZ:
          std::cout << "JNZ\n";
            if (A == 0) PC+=2;
            else PC = OPERAND();
            break;

          case INST::BXC:
          std::cout << "BXC\n";
            B = B^C;
            PC += 2;
            break;

          case INST::OUT:
          std::cout << "OUT\n";
            outputs.push_back( COMBO() % 8);
            PC += 2;
            break;

          case INST::BDV:
          std::cout << "BDV\n";
            B = A / pow(2,COMBO());
            PC += 2;
            break;

          case INST::CDV:
          std::cout << "CDV\n";
            C = A / pow(2,COMBO());
            PC += 2;
            break;

          case INST::HALT:
            halted = true;
            return;
        }
      }
    }

    uint64_t checksum() const {
      int pos = 1;
      uint64_t sum =0;
      for(auto v : outputs) sum += v*pos++;
      return sum;
    }
};

Computer loadState(std::ifstream& input)
{
    std::string line;

    auto getInt = [](const std::string &s) {
      auto portion = s.substr(s.find(":") + 2);
      int value = 0;
      auto [ptr, ec] = std::from_chars(portion.data(),
                                       portion.data() + portion.size(), value);

      if (ec != std::errc()) {
        throw std::runtime_error("Conversion failed for token: " +
                                 std::string(s));
      }

      return value;
    };

    Computer ret;
    std::getline(input, line);
    ret.A = getInt(line);

    std::getline(input, line);
    ret.B = getInt(line);

    std::getline(input, line);
    ret.C = getInt(line);

    std::getline(input, line); // blank

    std::getline(input, line); // program
    auto progStr = line.substr(line.find(':')+2);
    ret.program = string2vec<int>( progStr, ',' );
    return ret;
}




  


std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input)
{
  assert(input.is_open());
  auto computer = loadState(input);
  computer.run();

  return { computer.checksum(), 0};
}
