// ======================================================================
/*
 * Discrete interpolation inside grid cells to
 * find contour line intersection coordinates.
 * This is identical to nearest neighbour
 * interpolation except that we handle the
 * special case of rectangles with two values
 * only just like linear interpolation so that
 * we'd get nicer looking rounded corners
 * in the contours.
 *
 */
// ======================================================================

#pragma once

#include "Edge.h"
#include "FlipGrid.h"
#include "FlipSet.h"
#include "Missing.h"

namespace Tron
{
template <typename Traits>
class DiscreteInterpolation : public Traits
{
 public:
  // Convenience typedefs

  typedef typename Traits::coord_type coord_type;
  typedef typename Traits::value_type value_type;
  typedef Edge<Traits> MyEdge;
  typedef FlipSet<MyEdge> MyFlipSet;

 private:
  enum place_type
  {
    Below,
    Inside,
    Above
  };

  // Placement definitions. Note that the function works
  // correctly also for the special case of -inf...+inf
  // contouring when all valid values are inside.

  static place_type placement(value_type value, value_type lo, value_type hi)
  {
    if (!DiscreteInterpolation::missing(lo) && value < lo) return Below;
    if (!DiscreteInterpolation::missing(hi) && value > hi) return Above;
    return Inside;
  }

 public:
  // ** Fill-mode **

  // This is identical to the nearest neigbour method

  static void triangle(coord_type x1,
                       coord_type y1,
                       value_type z1,
                       coord_type x2,
                       coord_type y2,
                       value_type z2,
                       coord_type x3,
                       coord_type y3,
                       value_type z3,
                       value_type lo,
                       value_type hi,
                       MyFlipSet& flipset)
  {
    if (DiscreteInterpolation::missing(z1) || DiscreteInterpolation::missing(z2) ||
        DiscreteInterpolation::missing(z3))
      return;

    place_type c1 = placement(z1, lo, hi);
    place_type c2 = placement(z2, lo, hi);
    place_type c3 = placement(z3, lo, hi);

    // Handle the rectangular areas nearest to each corner separately

    // Edge center coordinates

    coord_type x12 = (x1 + x2) / 2;
    coord_type x23 = (x2 + x3) / 2;
    coord_type x31 = (x3 + x1) / 2;

    coord_type y12 = (y1 + y2) / 2;
    coord_type y23 = (y2 + y3) / 2;
    coord_type y31 = (y3 + y1) / 2;

    // Triangle center coordinates

    coord_type x0 = (x1 + x2 + x3) / 3;
    coord_type y0 = (y1 + y2 + y3) / 3;

    if (c1 == Inside)
    {
      flipset.eflip(MyEdge(x31, y31, x1, y1));
      flipset.eflip(MyEdge(x1, y1, x12, y12));
    }
    if (c2 == Inside)
    {
      flipset.eflip(MyEdge(x12, y12, x2, y2));
      flipset.eflip(MyEdge(x2, y2, x23, y23));
    }
    if (c3 == Inside)
    {
      flipset.eflip(MyEdge(x23, y23, x3, y3));
      flipset.eflip(MyEdge(x3, y3, x31, y31));
    }

    if ((c1 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x12, y12, x0, y0));
    if ((c2 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x0, y0, x12, y12));

    if ((c2 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x23, y23, x0, y0));
    if ((c3 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x0, y0, x23, y23));

    if ((c3 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x31, y31, x0, y0));
    if ((c1 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x0, y0, x31, y31));
  }

  static void rectangle(coord_type x1,
                        coord_type y1,
                        value_type z1,
                        coord_type x2,
                        coord_type y2,
                        value_type z2,
                        coord_type x3,
                        coord_type y3,
                        value_type z3,
                        coord_type x4,
                        coord_type y4,
                        value_type z4,
                        int gridx,
                        int gridy,
                        value_type lo,
                        value_type hi,
                        MyFlipSet& flipset,
                        FlipGrid& flipgrid)
  {
    // We assume it is redundant to check the Y-coordinates too
    if (DiscreteInterpolation::missing(x1) || DiscreteInterpolation::missing(x2) ||
        DiscreteInterpolation::missing(x3) || DiscreteInterpolation::missing(x4))
    {
      return;
    }

    // If only one corner is missing, we can contour the remaining
    // triangle. If two or more are missing, we cannot do anything.

    if (DiscreteInterpolation::missing(z1))
    {
      if (!DiscreteInterpolation::missing(z2) && !DiscreteInterpolation::missing(z3) &&
          !DiscreteInterpolation::missing(z4))
        triangle(x2, y2, z2, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
      return;
    }
    if (DiscreteInterpolation::missing(z2))
    {
      if (!DiscreteInterpolation::missing(z3) && !DiscreteInterpolation::missing(z4))
        triangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
      return;
    }
    if (DiscreteInterpolation::missing(z3))
    {
      if (!DiscreteInterpolation::missing(z4))
        triangle(x1, y1, z1, x2, y2, z2, x4, y4, z4, lo, hi, flipset);
      return;
    }
    if (DiscreteInterpolation::missing(z4))
    {
      triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);
      return;
    }

    place_type c1 = placement(z1, lo, hi);
    place_type c2 = placement(z2, lo, hi);
    place_type c3 = placement(z3, lo, hi);
    place_type c4 = placement(z4, lo, hi);

    // Edge center coordinates

    coord_type x12 = (x1 + x2) / 2;
    coord_type x23 = (x2 + x3) / 2;
    coord_type x34 = (x3 + x4) / 2;
    coord_type x41 = (x4 + x1) / 2;

    coord_type y12 = (y1 + y2) / 2;
    coord_type y23 = (y2 + y3) / 2;
    coord_type y34 = (y3 + y4) / 2;
    coord_type y41 = (y4 + y1) / 2;

    // Rectangle center coordinates

    coord_type x0 = (x12 + x34) / 2;
    coord_type y0 = (y12 + y34) / 2;

    // Now see how many unique values there are in the rectangle

    std::set<value_type> values;
    values.insert(z1);
    values.insert(z2);
    values.insert(z3);
    values.insert(z4);

    // The rectangle edges can be handled by
    // common code

    if (c1 == Inside)
    {
      flipset.eflip(MyEdge(x41, y41, x1, y1));
      flipset.eflip(MyEdge(x1, y1, x12, y12));
    }
    if (c2 == Inside)
    {
      flipset.eflip(MyEdge(x12, y12, x2, y2));
      flipset.eflip(MyEdge(x2, y2, x23, y23));
    }
    if (c3 == Inside)
    {
      flipset.eflip(MyEdge(x23, y23, x3, y3));
      flipset.eflip(MyEdge(x3, y3, x34, y34));
    }
    if (c4 == Inside)
    {
      flipset.eflip(MyEdge(x34, y34, x4, y4));
      flipset.eflip(MyEdge(x4, y4, x41, y41));
    }

    // If there are only 2 values and one of them occurs only
    // once, we can round that corner. Otherwise we use
    // regular nearest neighbour interpolation

    if (values.size() == 2 && ((z1 == z2 && z2 == z3) || (z1 == z2 && z1 == z4) ||
                               (z1 == z3 && z3 == z4) || (z2 == z3 && z3 == z4)))
    {
      if (z2 == z3 && z3 == z4)
      {
        if ((c1 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x12, y12, x41, y41));
        if ((c2 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x41, y41, x12, y12));
      }
      else if (z1 == z3 && z3 == z4)
      {
        if ((c2 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x23, y23, x12, y12));
        if ((c3 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x12, y12, x23, y23));
      }
      else if (z1 == z2 && z2 == z4)
      {
        if ((c3 == Inside) && (c4 != Inside)) flipset.eflip(MyEdge(x34, y34, x23, y23));
        if ((c4 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x23, y23, x34, y34));
      }
      else
      {
        if ((c4 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x41, y41, x34, y34));
        if ((c1 == Inside) && (c4 != Inside)) flipset.eflip(MyEdge(x34, y34, x41, y41));
      }
    }
    else
    {
      // We handle all 4 corners of the rectangle separately.

      if ((c1 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x12, y12, x0, y0));
      if ((c2 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x0, y0, x12, y12));

      if ((c2 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x23, y23, x0, y0));
      if ((c3 == Inside) && (c2 != Inside)) flipset.eflip(MyEdge(x0, y0, x23, y23));

      if ((c3 == Inside) && (c4 != Inside)) flipset.eflip(MyEdge(x34, y34, x0, y0));
      if ((c4 == Inside) && (c3 != Inside)) flipset.eflip(MyEdge(x0, y0, x34, y34));

      if ((c4 == Inside) && (c1 != Inside)) flipset.eflip(MyEdge(x41, y41, x0, y0));
      if ((c1 == Inside) && (c4 != Inside)) flipset.eflip(MyEdge(x0, y0, x41, y41));
    }
  }

  // ** line-mode **

  static void rectangle(coord_type x1,
                        coord_type y1,
                        value_type z1,
                        coord_type x2,
                        coord_type y2,
                        value_type z2,
                        coord_type x3,
                        coord_type y3,
                        value_type z3,
                        coord_type x4,
                        coord_type y4,
                        value_type z4,
                        value_type value,
                        MyFlipSet& flipset,
                        FlipGrid& flipgrid)
  {
  }

  static void triangle(coord_type x1,
                       coord_type y1,
                       value_type z1,
                       coord_type x2,
                       coord_type y2,
                       value_type z2,
                       coord_type x3,
                       coord_type y3,
                       value_type z3,
                       int gridx,
                       int gridy,
                       value_type value,
                       MyFlipSet& flipset)
  {
  }

};  // class LinearInterpolation

}  // namespace Tron

// ======================================================================
