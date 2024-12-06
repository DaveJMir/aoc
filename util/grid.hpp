#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <cassert>

#include "util/util.hpp"


namespace aoc::util
{

inline std::vector<std::string> gridFromFile(std::ifstream &&input) {
  std::vector<std::string> gridStrings{};
  int width = 0;

  aoc::util::foreach_line(input, [&](std::string_view line) {
    if (width && line.size() != width) {
      assert(!"Why not square?");
    }
    width = line.size();
    gridStrings.push_back(std::string{line});
  });
  return gridStrings;
}

class Grid {
  std::vector<std::string> gridData;

public:
  using offset = std::pair<int, int>;

    static constexpr offset North = {0, -1};
    static constexpr offset South = {0, 1};
    static constexpr offset East = {1, 0};
    static constexpr offset West = {-1, 0};

    static constexpr offset NorthEast = {1, -1};
    static constexpr offset NorthWest = {-1, -1};
    static constexpr offset SouthEast = {1, 1};
    static constexpr offset SouthWest = {-1, 1};

  Grid(std::vector<std::string> &&data)
      : gridData(std::move(data)), width(gridData[0].size()),
        height(gridData.size()) {}

  Grid(std::ifstream &&instream)
      : gridData(gridFromFile(std::move(instream))), width(gridData[0].size()),
        height(gridData.size()) {}

  const size_t width;
  const size_t height;

  class Iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char;
    using pointer = const char *;
    using reference = const char &;

    Iterator(int x, int y, const Grid *grid) : x(x), y(y), grid(grid) {}

    value_type operator*() const { return grid->at(x, y); }

    Iterator &operator++() {
      if (++x == grid->width) {
        x = 0;
        ++y;
      }
      if (y == grid->height) {
        x = y = -1;
      }
      return *this;
    }

    Iterator &operator--() {
      if (--x == -1) {
        x = grid->width - 1;
        --y;
      }
      if (y == -1) {
        x = y = -1;
      }
      return *this;
    }

    bool isOutOfBounds() const {
      return x < 0 || y < 0 || x >= grid->width || y >= grid->height;
    }

    Iterator operator+(const offset &offs) const {
      return Iterator(x + offs.first, y + offs.second, grid);
    }

    bool operator==(const Iterator &other) const {
      return x == other.x && y == other.y && grid == other.grid;
    }

    bool operator!=(const Iterator &other) const { return !(*this == other); }

    friend std::ostream &operator<<(std::ostream &os, const Iterator &it) {
      char value = (it.x >= 0 && it.x < it.grid->width && it.y >= 0 &&
                    it.y < it.grid->height)
                       ? it.grid->at(it.x, it.y)
                       : ' ';
      return os << "(" << it.x << ", " << it.y << ", '" << value << "')";
    }

  protected:
    friend class Grid;
    int x, y;
    const Grid *grid;
  };

  Iterator begin() const { return Iterator(0, 0, this); }
  Iterator end() const { return Iterator(-1, -1, this); }

  char operator[](const Iterator &it) const {
    assert(it.grid == this);
    return *it;
  }

  char at(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      return gridData[y][x];
    }
    return ' '; // Out-of-bounds returns space
  }
};

}