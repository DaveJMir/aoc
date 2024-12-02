#include <charconv>
#include <fstream>
#include <ranges>
#include <iostream>
#include <vector>
#include <string_view>

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
inline std::vector<T> string2vec(std::string_view s)
{
    std::vector<T> result{};
    for(const auto part : std::ranges::views::split(s, ' '))
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

template<typename CB>
inline int foreach_line(std::ifstream& stream, CB&& callback)
{
  std::string line;
  int count = 0;
  while(std::getline(stream, line)) {
    auto payload = trimmed(line);
    if(payload.empty()) break;
    count++;
    callback(payload);
  }
  return count;

}

}