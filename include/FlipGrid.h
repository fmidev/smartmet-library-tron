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
 *
 * The worlddata boolean indicates that the data wraps around
 * in the x-direction and that special care should be taken
 * to handle the extra cell created by the rightmost and
 * leftmost edges.
 */
// ======================================================================

#pragma once

#include <vector>

namespace Tron
{
class FlipGrid
{
 public:
  typedef std::vector<std::size_t> value_type;

  FlipGrid(std::size_t width, std::size_t height, bool worlddata = false);

  void flipTop(std::size_t i, std::size_t j);
  void flipRight(std::size_t i, std::size_t j);
  void flipBottom(std::size_t i, std::size_t j);
  void flipLeft(std::size_t i, std::size_t j);

  // Copy flipgrid edges to a flipset
  template <typename Grid, typename FlipSet>
  void copy(const Grid& grid, FlipSet& flipset) const;

 private:
  FlipGrid();

  // Calculate edge indices
  std::size_t top(std::size_t i, std::size_t j) const;
  std::size_t left(std::size_t i, std::size_t j) const;
  std::size_t right(std::size_t i, std::size_t j) const;
  std::size_t bottom(std::size_t i, std::size_t j) const;

  // Inverse calculations
  std::size_t i1pos(std::size_t index) const;
  std::size_t j1pos(std::size_t index) const;
  std::size_t i2pos(std::size_t index) const;
  std::size_t j2pos(std::size_t index) const;

  std::size_t itsWidth;
  std::size_t itsHeight;
  std::size_t itsSize;
  bool itsWorldData;

  // Flipping a None sets the enum value, otherwise the value is set to None
  enum Side
  {
    None,
    Top,
    Left,
    Right,
    Bottom
  };

  void flip(Side side, std::size_t index);

  // We save size here since standard does not speficy the size of enums (32-bit?)
  typedef std::vector<char> storage_type;
  storage_type itsEdges;

};  // class FlipGrid

// ----------------------------------------------------------------------
/*!
 * \brief Flip a side
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flip(Side side, std::size_t index)
{
  if (itsEdges[index] == None)
  {
    itsEdges[index] = side;
    ++itsSize;
  }
  else
  {
    itsEdges[index] = None;
    --itsSize;
  }
}

// ----------------------------------------------------------------------
/*!
 * \brief Flip a left edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipLeft(std::size_t i, std::size_t j) { flip(Left, left(i, j)); }
// ----------------------------------------------------------------------
/*!
 * \brief Flip a top edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipTop(std::size_t i, std::size_t j) { flip(Top, top(i, j)); }
// ----------------------------------------------------------------------
/*!
 * \brief Flip a right edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipRight(std::size_t i, std::size_t j) { flip(Right, right(i, j)); }
// ----------------------------------------------------------------------
/*!
 * \brief Flip a bottom edge
 */
// ----------------------------------------------------------------------

inline void FlipGrid::flipBottom(std::size_t i, std::size_t j) { flip(Bottom, bottom(i, j)); }
// ----------------------------------------------------------------------
/*!
 * \brief Convert (i,j) to edge index
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::top(std::size_t i, std::size_t j) const
{
  return (2 * itsWidth - 1) * j + i;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert (i,j) to edge index
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::left(std::size_t i, std::size_t j) const
{
  return (2 * itsWidth - 1) * j + itsWidth + i - 1;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert (i,j) to edge index
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::right(std::size_t i, std::size_t j) const
{
  return (2 * itsWidth - 1) * j + itsWidth + i;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert (i,j) to edge index
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::bottom(std::size_t i, std::size_t j) const
{
  return (2 * itsWidth - 1) * (j + 1) + i;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert edge index to i1
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::i1pos(std::size_t index) const
{
  std::size_t x = index % (2 * itsWidth - 1);
  if (x >= itsWidth - 1) x -= (itsWidth - 1);
  return x;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert edge index to j1 in (i,j)
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::j1pos(std::size_t index) const
{
  std::size_t y = index / (2 * itsWidth - 1);
  return y;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert edge index to i2
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::i2pos(std::size_t index) const
{
  std::size_t x = index % (2 * itsWidth - 1);
  if (x >= itsWidth - 1)
    x -= (itsWidth - 1);
  else
    ++x;
  return x;
}

// ----------------------------------------------------------------------
/*!
 * \brief Convert edge index to j2 in (i,j)
 */
// ----------------------------------------------------------------------

inline std::size_t FlipGrid::j2pos(std::size_t index) const
{
  std::size_t y = index / (2 * itsWidth - 1);
  if (index % (2 * itsWidth - 1) >= itsWidth - 1) ++y;
  return y;
}

// Copy flipgrid edges to a flipset
template <typename Grid, typename FlipSet>
void FlipGrid::copy(const Grid& grid, FlipSet& flipset) const
{
  if (itsSize == 0) return;

  storage_type::size_type n = itsEdges.size();
  for (storage_type::size_type i = 0; i < n; i++)
  {
    Side side = Side(itsEdges[i]);
    if (side != None)
    {
      std::size_t i1 = i1pos(i);
      std::size_t i2 = i2pos(i);
      std::size_t j1 = j1pos(i);
      std::size_t j2 = j2pos(i);
      typename Grid::coord_type x1 = grid.x(i1, j1);
      typename Grid::coord_type y1 = grid.y(i1, j1);
      typename Grid::coord_type x2 = grid.x(i2, j2);
      typename Grid::coord_type y2 = grid.y(i2, j2);
      if (side == Right || side == Top)
      {
        // Preserve original edge orientation
        std::swap(x1, x2);
        std::swap(y1, y2);
      }
      // eflip since projected coordinates may be identical at the poles
      flipset.eflip(typename FlipSet::value_type(x1, y1, x2, y2));
    }
  }
}

}  // namespace Tron

// ======================================================================
