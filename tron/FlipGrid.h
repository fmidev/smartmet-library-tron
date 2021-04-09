// ======================================================================
/*!
 * Class FlipSet is a set of objects such that an even number
 * of inserts causes the object to be discarded from the set.
 * Class FlipGrid is similar, but has been optimized for
 * handling the edges appearing in rectangular grids. Each
 * edge in a grid can be uniquely enumerated, and a vector
 * of booleans can be used to enumerate which edges belong
 * to the set and which do not. Finally a method is provided
 * for extracting the actual final set of edges out once all
 * edges have been inserted in (or "flipped").
 *
 * The principal idea in contouring would be to handle only
 * real contour lines using a FlipSet, and to use a FlipGrid
 * while extracting contour fill areas to manage the grid
 * cell edges. Since most cell edges are also in an adjacent
 * cell, using a FlipSet to manage all the flipping would
 * be inefficient since in typical contouring most cell
 * edges will be flipped twice or not at all, a single
 * flip is the exception.
 */
// ======================================================================

#pragma once

#include <cstdint>
#include <vector>

namespace Tron
{
class FlipGrid
{
 public:
  using value_type = std::vector<std::size_t>;

  FlipGrid(std::size_t width, std::size_t height);

  void flipTop(std::size_t i, std::size_t j);
  void flipRight(std::size_t i, std::size_t j);
  void flipBottom(std::size_t i, std::size_t j);
  void flipLeft(std::size_t i, std::size_t j);

  // Copy flipgrid edges to a flipset
  template <typename Grid, typename FlipSet>
  void copy(const Grid& grid, FlipSet& flipset) const;

 private:
  FlipGrid();

  std::size_t itsWidth;
  std::size_t itsHeight;
  std::size_t itsSize;

  // Flipping a None sets the enum value, otherwise the value is set to None
  enum class Side : std::uint8_t
  {
    None,
    Top,
    Left,
    Right,
    Bottom
  };

  using storage_type = std::vector<Side>;

  storage_type itsVerticalEdges;
  storage_type itsHorizontalEdges;

};  // class FlipGrid

// ----------------------------------------------------------------------
/*!
 * \brief Flip a left edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipLeft(std::size_t i, std::size_t j)
{
  const auto pos = j * itsWidth + i;
  if (itsVerticalEdges[pos] == Side::None)
  {
    itsVerticalEdges[pos] = Side::Left;
    ++itsSize;
  }
  else
  {
    itsVerticalEdges[pos] = Side::None;
    --itsSize;
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Flip a top edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipTop(std::size_t i, std::size_t j)
{
  const auto pos = (j + 1) * itsWidth + i;
  if (itsHorizontalEdges[pos] == Side::None)
  {
    itsHorizontalEdges[pos] = Side::Top;
    ++itsSize;
  }
  else
  {
    itsHorizontalEdges[pos] = Side::None;
    --itsSize;
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Flip a right edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipRight(std::size_t i, std::size_t j)
{
  const auto pos = j * itsWidth + i + 1;
  if (itsVerticalEdges[pos] == Side::None)
  {
    itsVerticalEdges[pos] = Side::Right;
    ++itsSize;
  }
  else
  {
    itsVerticalEdges[pos] = Side::None;
    --itsSize;
  }
}
// ----------------------------------------------------------------------
/*!
 * \brief Flip a bottom edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipBottom(std::size_t i, std::size_t j)
{
  const auto pos = j * itsWidth + i;
  if (itsHorizontalEdges[pos] == Side::None)
  {
    itsHorizontalEdges[pos] = Side::Bottom;
    ++itsSize;
  }
  else
  {
    itsHorizontalEdges[pos] = Side::None;
    --itsSize;
  }
}

// Copy flipgrid edges to a flipset
template <typename Grid, typename FlipSet>
void FlipGrid::copy(const Grid& grid, FlipSet& flipset) const
{
  if (itsSize == 0)
    return;

  for (std::size_t j = 0; j < itsHeight; j++)
  {
    const auto pos = j * itsWidth;
    for (std::size_t i = 0; i < itsWidth; i++)
    {
      const auto n = pos + i;
      const auto side = itsHorizontalEdges[n];
      if (side == Side::None)
      {
      }
      else if (side == Side::Bottom)
      {
        auto x1 = grid.x(i + 1, j);
        auto y1 = grid.y(i + 1, j);
        auto x2 = grid.x(i, j);
        auto y2 = grid.y(i, j);
        // eflip since projected coordinates may be identical at the poles
        flipset.eflip(typename FlipSet::value_type(x1, y1, x2, y2));
      }
      else if (side == Side::Top)
      {
        auto x1 = grid.x(i, j);
        auto y1 = grid.y(i, j);
        auto x2 = grid.x(i + 1, j);
        auto y2 = grid.y(i + 1, j);
        flipset.eflip(typename FlipSet::value_type(x1, y1, x2, y2));
      }
    }
  }

  for (std::size_t j = 0; j < itsHeight; j++)
  {
    const auto pos = j * itsWidth;
    for (std::size_t i = 0; i < itsWidth; i++)
    {
      const auto n = pos + i;
      const auto side = itsVerticalEdges[n];
      if (side == Side::None)
      {
      }
      else if (side == Side::Left)
      {
        auto x1 = grid.x(i, j);
        auto y1 = grid.y(i, j);
        auto x2 = grid.x(i, j + 1);
        auto y2 = grid.y(i, j + 1);
        flipset.eflip(typename FlipSet::value_type(x1, y1, x2, y2));
      }
      else if (side == Side::Right)
      {
        auto x1 = grid.x(i, j + 1);
        auto y1 = grid.y(i, j + 1);
        auto x2 = grid.x(i, j);
        auto y2 = grid.y(i, j);
        flipset.eflip(typename FlipSet::value_type(x1, y1, x2, y2));
      }
    }
  }
}

}  // namespace Tron

// ======================================================================
