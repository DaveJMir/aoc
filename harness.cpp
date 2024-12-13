#include <iostream>
#include <fstream>
#include <utility>
#include <cstdint>

using Result = std::pair<std::uint64_t, std::uint64_t>;

namespace 
{

std::pair<Result, Result> readExpected(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        throw std::runtime_error("Unable to open expected file: " + filename);
    }

    std::string line;
    Result example, input;

    while (std::getline(file, line)) {
        if (line.starts_with("Example Part1:")) {
            example.first = std::stoull(line.substr(15));
        } else if (line.starts_with("Example Part2:")) {
            example.second = std::stoull(line.substr(15));
        } else if (line.starts_with("Part1:")) {
            input.first = std::stoull(line.substr(7));
        } else if (line.starts_with("Part2:")) {
            input.second = std::stoull(line.substr(7));
        }
    }
    return {example, input};
}

}

std::pair<std::uint64_t, std::uint64_t> process(std::ifstream&& input);

int main() {
    auto expected = readExpected("expected.txt");

    std::ifstream exampleInput("example.txt");
    auto exampleResult = process(std::move(exampleInput));
    std::cout << "Example Part1: " << exampleResult.first << "\n";
    std::cout << "Example Part2: " << exampleResult.second << "\n";

    std::ifstream input("input.txt");
    auto result = process(std::move(input));
    std::cout << "\nPart1: " << result.first << "\n";
    std::cout << "Part2: " << result.second << "\n";

    int ret = 0;
    if (expected.first != exampleResult) {
        std::cout << "Example mismatch:\n";
        std::cout << "   Obtained " << exampleResult.first << ", "
                  << exampleResult.second << "\n";
        std::cout << "   Expected " << expected.first.first << ", "
                  << expected.first.second << "\n";
        ret++;
    }

    if (expected.second != result) {
        std::cout << "Puzzle mismatch:\n";
        std::cout << "   Obtained " << result.first << ", "
                  << result.second << "\n";
        std::cout << "   Expected " << expected.second.first << ", "
                  << expected.second.second << "\n";
        ret++;
    }

    return ret;
}
