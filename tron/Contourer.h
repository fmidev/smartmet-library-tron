// ======================================================================
/*
 * The main contouring interface for data in rectangular grids.
 *
 * The grid is expected to have the following minimum interface:
 *
 * class Grid
 * {
 *  public:
 *    typedef X value_type;		// usually float or double
 *    typedef Y size_type;		// usually std::size_t
 *    typedef Z coord_type;		// usually float or double
 *
 *    const value_type & operator()(size_type i, size_type j) const;
 *    coord_type x()(size_type i, size_type j) const;
 *    coord_type y()(size_type i, size_type j) const;
 *
 *    size_type width() const;
 *    size_type height() const;
 * }
 *
 * Array indices are expected to run from 0...width-1 and 0...height-1.
 *
 * The path adapter is expected to have the following minimum interface:
 *
 * class PathAdapter
 * {
 *   public:
 *     void moveto(x,y);
 *     void lineto(x,y);
 *     void closepath();
 * }
 *
 * Note: We assume grid coordinates are upright so that increasing
 *       j in grid(i,j) implies increasing Y-coordinate.
 *
 * Note: Grid cell coordinates are using the above assumption always
 *       passed on to rectangle/triangle calculations using clockwise (CW)
 *       ordering:
 *
 *         2    3        2
 *          +--+        /\
 *          |  |       /  \
 *          +--+      +----+
 *         1    4     1    3
 */
// ======================================================================

#pragma once

#include "CoordinateHints.h"
#include "Edge.h"
#include "FlipGrid.h"
#include "FlipSet.h"
#include "Hints.h"
#include "Missing.h"

#include <boost/foreach.hpp>

namespace Tron
{
template <typename Grid,
          typename PathAdapter,
          typename Traits,
          template <typename> class Interpolation>
class Contourer : public Interpolation<Traits>
{
 private:
  typedef FlipSet<Edge<Traits> > MyFlipSet;

 public:
  typedef typename Traits::coord_type coord_type;
  typedef typename Traits::value_type value_type;
  typedef Hints<Grid, Traits> hints_type;
  typedef Hints<Grid, Traits> coordinate_hints_type;

  /*
   * Calculate polygon surrounding the given value range.
   */

  static void fill(
      PathAdapter& path, const Grid& grid, value_type lolimit, value_type hilimit, bool worlddata)
  {
    MyFlipSet flipset;
    FlipGrid flipgrid(grid.width(), grid.height(), worlddata);

    typename Grid::size_type width = (worlddata ? grid.width() + 1 : grid.width());

    for (typename Grid::size_type j = 0; j < grid.height() - 1; j++)
      for (typename Grid::size_type i = 0; i < width - 1; i++)
      {
        Contourer::rectangle(grid.x(i, j),
                             grid.y(i, j),
                             grid(i, j),
                             grid.x(i, j + 1),
                             grid.y(i, j + 1),
                             grid(i, j + 1),
                             grid.x(i + 1, j + 1),
                             grid.y(i + 1, j + 1),
                             grid(i + 1, j + 1),
                             grid.x(i + 1, j),
                             grid.y(i + 1, j),
                             grid(i + 1, j),
                             i,
                             j,
                             lolimit,
                             hilimit,
                             flipset,
                             flipgrid);
      }

    flipgrid.copy(grid, flipset);
    flipset.prepare();
    Builder::fill<Traits>(flipset.edges(), path);
  }

  /*
   * Calculate polygon surrounding the given value range. Use the given hints
   * on data values to contour only areas of interest.
   */

  static void fill(PathAdapter& path,
                   const Grid& grid,
                   value_type lolimit,
                   value_type hilimit,
                   bool worlddata,
                   const hints_type& hints)
  {
    typename hints_type::rectangles rects = hints.get_rectangles(lolimit, hilimit);

    MyFlipSet flipset;
    FlipGrid flipgrid(grid.width(), grid.height(), worlddata);

    for (typename hints_type::rectangles::const_iterator it = rects.begin(), end = rects.end();
         it != end;
         ++it)
    {
      for (typename Grid::size_type j = it->y1; j < it->y2; j++)
        for (typename Grid::size_type i = it->x1; i < it->x2; i++)
        {
          Contourer::rectangle(grid.x(i, j),
                               grid.y(i, j),
                               grid(i, j),
                               grid.x(i, j + 1),
                               grid.y(i, j + 1),
                               grid(i, j + 1),
                               grid.x(i + 1, j + 1),
                               grid.y(i + 1, j + 1),
                               grid(i + 1, j + 1),
                               grid.x(i + 1, j),
                               grid.y(i + 1, j),
                               grid(i + 1, j),
                               static_cast<int>(i),
                               static_cast<int>(j),
                               lolimit,
                               hilimit,
                               flipset,
                               flipgrid);
        }
    }

    if (worlddata)
    {
      typename Grid::size_type i = grid.width() - 1;

      for (typename Grid::size_type j = 0; j < grid.height() - 1; j++)
        Contourer::rectangle(grid.x(i, j),
                             grid.y(i, j),
                             grid(i, j),
                             grid.x(i, j + 1),
                             grid.y(i, j + 1),
                             grid(i, j + 1),
                             grid.x(i + 1, j + 1),
                             grid.y(i + 1, j + 1),
                             grid(i + 1, j + 1),
                             grid.x(i + 1, j),
                             grid.y(i + 1, j),
                             grid(i + 1, j),
                             i,
                             j,
                             lolimit,
                             hilimit,
                             flipset,
                             flipgrid);
    }

    flipgrid.copy(grid, flipset);
    flipset.prepare();
    Builder::fill<Traits>(flipset.edges(), path);
  }

  /*
   * Calculate polygon surrounding the given value range. Process only the given
   * list of coordinate rectangles using the prebuilt hints for them.
   */

  static void fill(PathAdapter& path,
                   const Grid& grid,
                   value_type lolimit,
                   value_type hilimit,
                   bool worlddata,
                   const typename hints_type::rectangles& hints,
                   const typename coordinate_hints_type::rectangles& coordinate_hints)
  {
    typename hints_type::rectangles rects = hints.get_rectangles(lolimit, hilimit);

    MyFlipSet flipset;
    FlipGrid flipgrid(grid.width(), grid.height(), worlddata);

    for (typename hints_type::rectangles::const_iterator it = rects.begin(), end = rects.end();
         it != end;
         ++it)
    {
      for (typename Grid::size_type j = it->y1; j < it->y2; j++)
        for (typename Grid::size_type i = it->x1; i < it->x2; i++)
          Contourer::rectangle(grid.x(i, j),
                               grid.y(i, j),
                               grid(i, j),
                               grid.x(i, j + 1),
                               grid.y(i, j + 1),
                               grid(i, j + 1),
                               grid.x(i + 1, j + 1),
                               grid.y(i + 1, j + 1),
                               grid(i + 1, j + 1),
                               grid.x(i + 1, j),
                               grid.y(i + 1, j),
                               grid(i + 1, j),
                               static_cast<int>(i),
                               static_cast<int>(j),
                               lolimit,
                               hilimit,
                               flipset,
                               flipgrid);
    }

    if (worlddata)
    {
      typename Grid::size_type i = grid.width() - 1;

      for (typename Grid::size_type j = 0; j < grid.height() - 1; j++)
        Contourer::rectangle(grid.x(i, j),
                             grid.y(i, j),
                             grid(i, j),
                             grid.x(i, j + 1),
                             grid.y(i, j + 1),
                             grid(i, j + 1),
                             grid.x(i + 1, j + 1),
                             grid.y(i + 1, j + 1),
                             grid(i + 1, j + 1),
                             grid.x(i + 1, j),
                             grid.y(i + 1, j),
                             grid(i + 1, j),
                             i,
                             j,
                             lolimit,
                             hilimit,
                             flipset,
                             flipgrid);
    }

    flipgrid.copy(grid, flipset);
    flipset.prepare();
    Builder::fill<Traits>(flipset.edges(), path);
  }

  /*
   * Calculate isoline for the given value
   */

  static void line(PathAdapter& path, const Grid& grid, value_type value, bool worlddata)
  {
    MyFlipSet flipset;

    typename Grid::size_type width = (worlddata ? grid.width() + 1 : grid.width());

    for (typename Grid::size_type j = 0; j < grid.height() - 1; j++)
      for (typename Grid::size_type i = 0; i < width - 1; i++)
        Contourer::rectangle(grid.x(i, j),
                             grid.y(i, j),
                             grid(i, j),
                             grid.x(i, j + 1),
                             grid.y(i, j + 1),
                             grid(i, j + 1),
                             grid.x(i + 1, j + 1),
                             grid.y(i + 1, j + 1),
                             grid(i + 1, j + 1),
                             grid.x(i + 1, j),
                             grid.y(i + 1, j),
                             grid(i + 1, j),
                             value,
                             flipset);

    flipset.prepare();
    Builder::line<Traits>(flipset.edges(), path);
  }

  /*
   * Calculate isoline for the given value. Use the given hints on
   * data values to contour only areas of interest.
   */

  static void line(PathAdapter& path,
                   const Grid& grid,
                   value_type value,
                   bool worlddata,
                   const hints_type& hints)
  {
    typename hints_type::rectangles rects = hints.get_rectangles(value);

    MyFlipSet flipset;

    for (typename hints_type::rectangles::const_iterator it = rects.begin(), end = rects.end();
         it != end;
         ++it)
    {
      for (typename Grid::size_type j = it->y1; j < it->y2; j++)
        for (typename Grid::size_type i = it->x1; i < it->x2; i++)
          Contourer::rectangle(grid.x(i, j),
                               grid.y(i, j),
                               grid(i, j),
                               grid.x(i, j + 1),
                               grid.y(i, j + 1),
                               grid(i, j + 1),
                               grid.x(i + 1, j + 1),
                               grid.y(i + 1, j + 1),
                               grid(i + 1, j + 1),
                               grid.x(i + 1, j),
                               grid.y(i + 1, j),
                               grid(i + 1, j),
                               value,
                               flipset);
    }

    if (worlddata)
    {
      typename Grid::size_type i = grid.width() - 1;
      for (typename Grid::size_type j = 0; j < grid.height() - 1; j++)
        Contourer::rectangle(grid.x(i, j),
                             grid.y(i, j),
                             grid(i, j),
                             grid.x(i, j + 1),
                             grid.y(i, j + 1),
                             grid(i, j + 1),
                             grid.x(i + 1, j + 1),
                             grid.y(i + 1, j + 1),
                             grid(i + 1, j + 1),
                             grid.x(i + 1, j),
                             grid.y(i + 1, j),
                             grid(i + 1, j),
                             value,
                             flipset);
    }

    flipset.prepare();
    Builder::line<Traits>(flipset.edges(), path);
  }

};  // class Contourer

}  // namespace Tron

// ======================================================================
