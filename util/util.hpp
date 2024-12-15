#pragma once

#include <charconv>
#include <fstream>
#include <ranges>
#include <iostream>
#include <vector>
#include <string_view>
#include <sstream>

namespace aoc::util
{

inline std::string_view trimmed(std::string_view str)
{
  size_t endpos = str.find_last_not_of(" \t\n");
  if(endpos == std::string_view::npos) return {};
  size_t startpos = str.find_first_not_of(" \t\n");
  return str.substr(startpos, endpos-startpos+1);
}

template<typename T>
inline std::vector<T> string2vec(std::string_view s, char c = ' ')
{
    std::vector<T> result{};
    for(const auto part : std::ranges::views::split(s, c))
    {
        std::string_view token(part.begin(), part.end());
        T value;
        auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), value);

        if (ec != std::errc()) {
            throw std::runtime_error("Conversion failed for token: " + std::string(token));
        }

        result.push_back(value);
    }
    return result;
}

enum class OnEmptyLine{
    Ignore = 0,
    PassThrough,
    Halt,
};

template <typename CB>
inline int foreach_line(std::ifstream &stream, CB &&callback,
                        OnEmptyLine onEmpty = OnEmptyLine::Ignore) {
    std::string line;
    int count = 0;
    while (std::getline(stream, line)) {
        auto payload = trimmed(line);
        if (payload.empty())
        {
            if(onEmpty == OnEmptyLine::Halt) return count;
            if(onEmpty == OnEmptyLine::Ignore) continue;
        }
        count++;
        callback(payload);
    }
    return count;
}

inline std::vector<std::string> splitExampleByParts(std::ifstream& input) {
    std::vector<std::ostringstream> parts(1); // Start with one part
    foreach_line(input, [&parts](std::string_view line)
    {
        if (line == "##PART2##") {
            parts.emplace_back();
            return;
        }
        parts.back() << line << "\n";
    });

    std::vector<std::string> result;
    for (auto& part : parts) {
        result.push_back(part.str());
    }
    return result;
}

}