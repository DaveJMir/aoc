
#include "util/util.hpp"

#include <regex>
#include <iostream>
#include <cassert>

int find_str(const std::vector<std::string>& grid, int x, int y, int stepX, int stepY, std::string_view target)
{

  if(target.empty()) return 1;
  if (x < 0)
    return 0;
  if (x >= grid[0].size())
    return 0;
  if (y < 0)
    return 0;
  if (y >= grid.size())
    return 0;

  if(grid[x][y] != target[0]) return 0;
  return find_str(grid, x+stepX, y+stepY, stepX, stepY, target.substr(1));
}

int find_str(const std::vector<std::string>& grid, int x, int y, std::string_view target)
{
  if(target.empty()) return 1;
  if (x < 0)
    return 0;
  if (x >= grid[0].size())
    return 0;
  if (y < 0)
    return 0;
  if (y >= grid.size())
    return 0;

  if(grid[x][y] != target[0]) return 0;

  int found =0;

  found += find_str(grid, x, y, -1, -1, target);
  found += find_str(grid, x, y,  0, -1, target);
  found += find_str(grid, x, y,  1, -1, target);

  found += find_str(grid, x, y, -1,  0, target);
  found += find_str(grid, x, y,  0,  0, target);
  found += find_str(grid, x, y,  1,  0, target);

  found += find_str(grid, x, y, -1, +1, target);
  found += find_str(grid, x, y,  0, +1, target);
  found += find_str(grid, x, y,  1, +1, target);
  return found;
}

int notit(char a)
{
  if (a == 'X') return 1;
  if (a == 'A') return 1;
  return 0;
}

int find_x_mas(const std::vector<std::string>& grid, int x, int y)
{
  if(grid[x][y] != 'A') return 0;
  if( (x==0) || (y==0)) return 0;
  if (x == grid[0].size()-1) return 0;
  if (y == grid.size()-1) return 0;

  char ul = grid[x-1][y+1];
  char ur = grid[x+1][y+1];

  char ll = grid[x-1][y-1];
  char lr = grid[x+1][y-1];

  if(notit(ul)) return 0;
  if(notit(ur)) return 0;
  if(notit(ll)) return 0;
  if(notit(lr)) return 0;

  if ( (ul != lr) && (ur != ll))
  {
     return 1;
  }
  return 0;



}

int main() {
  #if 1
  std::ifstream input{"input.txt"};
  #else
  std::ifstream input{"example.txt"};
  #endif

  std::vector<std::string> grid{};
  int width = 0;
  int height = 0;

  aoc::util::foreach_line(input, [&](std::string_view line) {
    if (width && line.size() != width) {
      assert(!"Why not square?");
    }
    width = line.size();
    grid.push_back(std::string{line});
  });
  height = grid.size();

  int total = 0;
  for(int x=0; x < width; x++)
  {
    for(int y=0; y<height; y++)
    {
      total += find_str(grid, x, y, "XMAS");
    }
  }
  std::cout << "Input Part1: " << total << "\n";

  total = 0;
  for(int x=0; x < width; x++)
  {
    for(int y=0; y<height; y++)
    {
      total += find_x_mas(grid, x, y);
    }
  }
  std::cout << "Input Part2: " << total << "\n";

  return 0;
}