#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <cassert>

#include "util/util.hpp"


namespace aoc::util
{

inline std::vector<std::string> gridFromFile(std::ifstream &input) {
  std::vector<std::string> gridStrings{};
  int width = 0;

  aoc::util::foreach_line(input, [&](std::string_view line) {
    if (width && line.size() != width) {
      assert(!"Why not square?");
    }
    width = line.size();
    gridStrings.push_back(std::string{line});
  }, OnEmptyLine::Halt);
  return gridStrings;
}

class Grid {
  std::vector<std::string> gridData;

public:
  using offset = std::pair<int, int>;
  using coord = std::pair<int, int>;

    static constexpr offset North = {0, -1};
    static constexpr offset South = {0, 1};
    static constexpr offset East = {1, 0};
    static constexpr offset West = {-1, 0};

    static constexpr offset NorthEast = {1, -1};
    static constexpr offset NorthWest = {-1, -1};
    static constexpr offset SouthEast = {1, 1};
    static constexpr offset SouthWest = {-1, 1};

    static constexpr std::array<Grid::offset, 4> directions4 = {
        Grid::North, Grid::South, Grid::East, Grid::West};
    static constexpr std::array<Grid::offset, 8> directions8 = {
        Grid::North,     Grid::South,     Grid::East,      Grid::West,
        Grid::NorthEast, Grid::NorthWest, Grid::SouthEast, Grid::SouthWest};

    Grid(size_t width, size_t height, char c) : width{width}, height{height} {
      for (int y = 0; y < height; y++) {
        gridData.emplace_back(width, c);
      }
  }

  Grid(std::vector<std::string> &&data)
      : gridData(std::move(data)), width(gridData[0].size()),
        height(gridData.size()) {}

  Grid(std::ifstream &instream)
      : gridData(gridFromFile(instream)), width(gridData[0].size()),
        height(gridData.size()) {}

  const size_t width;
  const size_t height;

  class Iterator {
  public:
    using iterator_category = std::bidirectional_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = char;
    using pointer = char *;
    using reference = char&;

    Iterator(int x, int y, Grid *grid) : x(x), y(y), grid(grid) {}

    reference operator*() { return grid->at(x, y); }

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

    std::pair<int, int> coords() const
    {
      return {x,y};
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
    Grid *grid;
  };

  Iterator begin() { return Iterator(0, 0, this); }
  Iterator last() { return Iterator(width - 1, height - 1, this); }
  Iterator end() { return Iterator(-1, -1, this); }

  char& operator[](Iterator &it) {
    assert(it.grid == this);
    return *it;
  }

  Iterator find(char c) 
  {
    for(auto it = begin(); it != end(); ++it)
    {
      if (*it == c)
        return it;
    }
    return end();
  }

  Iterator iterAt(std::pair<int,int> pos)
  {
    return Iterator(pos.first, pos.second, this);
  }

  // char at(int x, int y) const {
  //   if (x >= 0 && x < width && y >= 0 && y < height) {
  //     return gridData[y][x];
  //   }
  //   return ' '; // Out-of-bounds returns space
  // }

  offset bounds() const { return {width, height}; }
  bool inBound(coord c) const { return c.first >= 0 && c.first < width && c.second >= 0 && c.second < height; }

  char &at(int x, int y) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      return gridData[y][x];
    }
    static char outOfBounds = ' '; // Fallback for out-of-bounds
    return outOfBounds;
  }

  const char &at(int x, int y) const {
    if (x >= 0 && x < width && y >= 0 && y < height) {
      return gridData[y][x];
    }
    static char outOfBounds = ' '; // Fallback for out-of-bounds
    return outOfBounds;
  }

  friend std::ostream &operator<<(std::ostream &os, const Grid &g) {
    os << "Grid{";
    for (int y = 0; y < g.height; y++) {
      os << "\n  ";
      for (int x = 0; x < g.width; x++) {
        os << g.at(x, y);
      }
    }
    return os << " }";
  }
};

Grid::coord operator-(const Grid::coord& a, const Grid::coord& b) {
    return {a.first - b.first, a.second - b.second};
}
std::ostream& operator<<(std::ostream& os, const Grid::coord& c) {
    os << "(" << c.first << ", " << c.second << ")";
    return os;
}
Grid::coord operator+(const Grid::coord& a, const Grid::coord& b) {
    return {a.first + b.first, a.second + b.second};
}
Grid::coord operator-(const Grid::coord& c) {
    return {-c.first, -c.second};
}

Grid::coord operator*(const Grid::coord &c, int scalar) {
  return {c.first * scalar, c.second * scalar};
}

Grid::coord operator*(int scalar, const Grid::coord& c) {
    return c * scalar;
}

}