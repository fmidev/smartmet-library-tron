// ======================================================================
/*
 * Linear interpolation inside grid cells to
 * find contour line intersection coordinates.
 *
 */
// ======================================================================

#pragma once

#include "Edge.h"
#include "FlipGrid.h"
#include "FlipSet.h"
#include "Missing.h"
#include "SmallVector.h"
#include <cassert>
#include <vector>

namespace Tron
{
// Push only if the last coordinate is not already there
template <typename VectorType, typename C>
static void unique_push(VectorType& x, VectorType& y, C xnext, C ynext)
{
  std::size_t n = x.size() - 1;
  if (x.empty() || x[n] != xnext || y[n] != ynext)
  {
    x.push_back(xnext);
    y.push_back(ynext);
  }
}

template <typename Traits>
class LinearInterpolation : public Traits
{
 public:
  // Convenience typedefs

  typedef typename Traits::coord_type coord_type;
  typedef typename Traits::value_type value_type;
  typedef Edge<Traits> MyEdge;
  typedef FlipSet<MyEdge> MyFlipSet;

 private:
  // Interpolate intersection coordinate. Note that we perform
  // the arithmetic with sorted coordinates to guarantee the
  // same results for adjacent triangles (this prevents mismatches
  // due to different rounding).

  static void intersect(coord_type x1,
                        coord_type y1,
                        value_type z1,
                        coord_type x2,
                        coord_type y2,
                        value_type z2,
                        value_type value,
                        coord_type& x,
                        coord_type& y)
  {
    // These equality tests are absolutely necessary for handling value==lolimit cases
    // without any rounding errors!
    if (z1 == value)
    {
      x = x1;
      y = y1;
    }
    else if (z2 == value)
    {
      x = x2;
      y = y2;
    }
    else if (x1 < x2 || (x1 == x2 && y1 < y2))  // lexicographic sorting
    {
      value_type s = (value - z2) / (z1 - z2);
      x = x2 + s * (x1 - x2);
      y = y2 + s * (y1 - y2);
    }
    else
    {
      value_type s = (value - z1) / (z2 - z1);
      x = x1 + s * (x2 - x1);
      y = y1 + s * (y2 - y1);
    }
  }

  // Placement definitions. Note that the function works
  // correctly also for the special case of -inf...+inf
  // contouring when all valid values are inside.

  enum place_type
  {
    Below,
    Inside,
    Above
  };

  static place_type placement(value_type value, value_type lo, value_type hi)
  {
    if (!LinearInterpolation::missing(lo) && value < lo)
      return Below;
    if (!LinearInterpolation::missing(hi) && value >= hi)
      return Above;
    return Inside;
  }

  static place_type placement(value_type value, value_type limit)
  {
    if (!LinearInterpolation::missing(limit) && value <= limit)
      return Below;
    return Above;
  }

  // Produce a line

  template <typename VectorType>
  static void flush_line(VectorType& x, VectorType& y, MyFlipSet& flipset)
  {
    if (x.size() == 2)
    {
      flipset.eflip(MyEdge(x[0], y[0], x[1], y[1]));
      x.clear();
      y.clear();
    }
    else
      throw std::runtime_error(
          "Invalid polyline, expecting 2 coordinates for a line segment inside a grid cell");
  }

  // NOTE: Always produce clockwise areas, assuming input is clockwise!!
  // That makes the polygons valid for OGC and eases building the
  // polygons from the results

  // Connect intersections found from a triangle or rectangle

  template <typename VectorType>
  static void flush_polygon(VectorType& x, VectorType& y, MyFlipSet& flipset)
  {
    const auto n = x.size();
    if (n > 2)
    {
      for (unsigned int i = 0; i < n - 1; i++)
        flipset.eflip(MyEdge(x[i], y[i], x[i + 1], y[i + 1]));

      flipset.eflip(MyEdge(x[n - 1], y[n - 1], x[0], y[0]));
    }
    x.clear();
    y.clear();
  }

  template <typename VectorType>
  static void intersect(VectorType& x,
                        VectorType& y,
                        coord_type x1,
                        coord_type y1,
                        value_type z1,
                        place_type c1,
                        coord_type x2,
                        coord_type y2,
                        value_type z2,
                        place_type c2,
                        value_type lo,
                        value_type hi)
  {
    coord_type X, Y;
    if (c1 == Below)
    {
      if (c2 == Below)
      {
        // Below Below
      }
      else if (c2 == Inside)
      {
        // Below Inside
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        unique_push(x, y, X, Y);
        unique_push(x, y, x2, y2);
      }
      else
      {
        // Below Above
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        unique_push(x, y, X, Y);
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        unique_push(x, y, X, Y);
      }
    }
    else if (c1 == Inside)
    {
      if (c2 == Below)
      {
        // Inside Below
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        unique_push(x, y, x1, y1);
        unique_push(x, y, X, Y);
      }
      else if (c2 == Inside)
      {
        // Inside Inside
        unique_push(x, y, x1, y1);
        unique_push(x, y, x2, y2);
      }
      else
      {
        // Inside Above
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        unique_push(x, y, x1, y1);
        unique_push(x, y, X, Y);
      }
    }
    else
    {
      if (c2 == Below)
      {
        // Above Below
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        unique_push(x, y, X, Y);
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        unique_push(x, y, X, Y);
      }
      else if (c2 == Inside)
      {
        // Above Inside
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        unique_push(x, y, X, Y);
        unique_push(x, y, x2, y2);
      }
      else
      {
        // Above Above
      }
    }
  }

  // Intersect triangle edges without calculating fill areas
  // The logic for the algorithm is explained in the
  // calling method.

  template <typename VectorType>
  static void intersect(VectorType& x,
                        VectorType& y,
                        coord_type x1,
                        coord_type y1,
                        value_type z1,
                        place_type c1,
                        coord_type x2,
                        coord_type y2,
                        value_type z2,
                        place_type c2,
                        value_type value)
  {
    // Here we require exactly one Below value, hence we require xor to be true

    if (!((c1 == Below) ^ (c2 == Below)))
      return;

    // Note that we do not need to worry about an Inside-Inside
    // edge here - the case is handled by joining the end points
    // of the adjacent Below-Inside edges.
    //
    // Perform math using sorted coordinates to guarantee
    // ordering has no effect in results via rounding errors.
    // Otherwise adjacent cells might produce slightly different
    // intersection coordinates. Note that we choose the exact
    // same ordering as in fill mode, just in case the results
    // for some reason should match to the last decimal.

    if (x1 < x2 || (x1 == x2 && y1 < y2))  // lexicographic sorting
    {
      value_type s = (value - z2) / (z1 - z2);
      x.push_back(x2 + s * (x1 - x2));
      y.push_back(y2 + s * (y1 - y2));
    }
    else
    {
      value_type s = (value - z1) / (z2 - z1);
      x.push_back(x1 + s * (x2 - x1));
      y.push_back(y1 + s * (y2 - y1));
    }
  }

  // Below Below Inside
  static void triangle_BBI(coord_type x1,
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
    coord_type X1, Y1, X2, Y2;

    intersect(x1, y1, z1, x3, y3, z3, lo, X1, Y1);
    intersect(x2, y2, z2, x3, y3, z3, lo, X2, Y2);
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, x3, y3));
    flipset.eflip(MyEdge(x3, y3, X1, Y1));
  }

  // Below Below Above
  static void triangle_BBA(coord_type x1,
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
    coord_type X1, Y1, X2, Y2, X3, Y3, X4, Y4;

    intersect(x1, y1, z1, x3, y3, z3, lo, X1, Y1);
    intersect(x1, y1, z1, x3, y3, z3, hi, X2, Y2);
    intersect(x2, y2, z2, x3, y3, z3, hi, X3, Y3);
    intersect(x2, y2, z2, x3, y3, z3, lo, X4, Y4);
    flipset.eflip(MyEdge(X1, Y1, X4, Y4));
    flipset.eflip(MyEdge(X4, Y4, X3, Y3));
    flipset.eflip(MyEdge(X3, Y3, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X1, Y1));
  }

  // Below Inside Inside
  static void triangle_BII(coord_type x1,
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
    coord_type X1, Y1, X2, Y2;

    intersect(x1, y1, z1, x2, y2, z2, lo, X1, Y1);
    intersect(x1, y1, z1, x3, y3, z3, lo, X2, Y2);
    flipset.eflip(MyEdge(X1, Y1, x2, y2));
    flipset.eflip(MyEdge(x2, y2, x3, y3));
    flipset.eflip(MyEdge(x3, y3, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X1, Y1));
  }

  // Below Inside Above
  static void triangle_BIA(coord_type x1,
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
    coord_type X1, Y1, X2, Y2, X3, Y3, X4, Y4;

    intersect(x1, y1, z1, x2, y2, z2, lo, X1, Y1);
    intersect(x2, y2, z2, x3, y3, z3, hi, X2, Y2);
    intersect(x1, y1, z1, x3, y3, z3, hi, X3, Y3);
    intersect(x1, y1, z1, x3, y3, z3, lo, X4, Y4);
    flipset.eflip(MyEdge(X1, Y1, x2, y2));
    flipset.eflip(MyEdge(x2, y2, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X3, Y3));
    flipset.eflip(MyEdge(X3, Y3, X4, Y4));
    flipset.eflip(MyEdge(X4, Y4, X1, Y1));
  }

  // Below Above Inside
  static void triangle_BAI(coord_type x1,
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
    coord_type X1, Y1, X2, Y2, X3, Y3, X4, Y4;

    intersect(x1, y1, z1, x2, y2, z2, lo, X1, Y1);
    intersect(x1, y1, z1, x2, y2, z2, hi, X2, Y2);
    intersect(x2, y2, z2, x3, y3, z3, hi, X3, Y3);
    intersect(x1, y1, z1, x3, y3, z3, lo, X4, Y4);
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X3, Y3));
    flipset.eflip(MyEdge(X3, Y3, x3, y3));
    flipset.eflip(MyEdge(x3, y3, X4, Y4));
    flipset.eflip(MyEdge(X4, Y4, X1, Y1));
  }

  // Below Above Above
  static void triangle_BAA(coord_type x1,
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
    coord_type X1, Y1, X2, Y2, X3, Y3, X4, Y4;

    intersect(x1, y1, z1, x2, y2, z2, lo, X1, Y1);
    intersect(x1, y1, z1, x2, y2, z2, hi, X2, Y2);
    intersect(x1, y1, z1, x3, y3, z3, hi, X3, Y3);
    intersect(x1, y1, z1, x3, y3, z3, lo, X4, Y4);
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X3, Y3));
    flipset.eflip(MyEdge(X3, Y3, X4, Y4));
    flipset.eflip(MyEdge(X4, Y4, X1, Y1));
  }

  // Inside Inside Above
  static void triangle_IIA(coord_type x1,
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
    coord_type X1, Y1, X2, Y2;

    intersect(x1, y1, z1, x3, y3, z3, hi, X1, Y1);
    intersect(x2, y2, z2, x3, y3, z3, hi, X2, Y2);
    flipset.eflip(MyEdge(x1, y1, x2, y2));
    flipset.eflip(MyEdge(x2, y2, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X1, Y1));
    flipset.eflip(MyEdge(X1, Y1, x1, y1));
  }

  // Inside Above Above
  static void triangle_IAA(coord_type x1,
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
    coord_type X1, Y1, X2, Y2;

    intersect(x1, y1, z1, x2, y2, z2, hi, X1, Y1);
    intersect(x1, y1, z1, x3, y3, z3, hi, X2, Y2);
    flipset.eflip(MyEdge(x1, y1, X1, Y1));
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, x1, y1));
  }

 public:
  // ** Fill-mode **

  static void triangle(coord_type x1,
                       coord_type y1,
                       value_type z1,
                       place_type c1,
                       coord_type x2,
                       coord_type y2,
                       value_type z2,
                       place_type c2,
                       coord_type x3,
                       coord_type y3,
                       value_type z3,
                       place_type c3,
                       value_type lo,
                       value_type hi,
                       MyFlipSet& flipset)
  {
    if (c1 == Below)
    {
      if (c2 == Below)
      {
        if (c3 == Below)
          ;  // BBB = nothing
        else if (c3 == Inside)
          triangle_BBI(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BBI
        else
          triangle_BBA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BBA
      }
      else if (c2 == Inside)
      {
        if (c3 == Below)
          triangle_BBI(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // BIB
        else if (c3 == Inside)
          triangle_BII(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BII
        else
          triangle_BIA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BIA
      }
      else  // c2=Above
      {
        if (c3 == Below)
          triangle_BBA(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // BAB
        else if (c3 == Inside)
          triangle_BAI(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BAI
        else
          triangle_BAA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // BAA
      }
    }
    else if (c1 == Inside)
    {
      if (c2 == Below)
      {
        if (c3 == Below)
          triangle_BBI(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // IBB
        else if (c3 == Inside)
          triangle_BII(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // IBI
        else
          triangle_BAI(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // IBA
      }
      else if (c2 == Inside)
      {
        if (c3 == Below)
          triangle_BII(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // IIB
        else if (c3 == Inside)
        {
          // Inside Inside Inside
          flipset.eflip(MyEdge(x1, y1, x2, y2));
          flipset.eflip(MyEdge(x2, y2, x3, y3));
          flipset.eflip(MyEdge(x3, y3, x1, y1));
        }
        else
          triangle_IIA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // IIA
      }
      else  // c2=Above
      {
        if (c3 == Below)
          triangle_BIA(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // IAB
        else if (c3 == Inside)
          triangle_IIA(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // IAI
        else                                                                  // c3=Above
          triangle_IAA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // IAA
      }
    }
    else
    {
      if (c2 == Below)
      {
        if (c3 == Below)
          triangle_BBA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // ABB
        else if (c3 == Inside)
          triangle_BIA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // ABI
        else
          triangle_BAA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // ABA
      }
      else if (c2 == Inside)
      {
        if (c3 == Below)
          triangle_BAI(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // AIB
        else if (c3 == Inside)
          triangle_IIA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // AII
        else                                                                  // c3=Above
          triangle_IAA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // AIA
      }
      else
      {
        if (c3 == Below)
          triangle_BAA(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // AAB
        else if (c3 == Inside)
          triangle_IAA(x3, y3, z3, x1, y1, z1, x2, y2, z2, lo, hi, flipset);  // AAI
        // AAA is OK
      }
    }
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
                       value_type lo,
                       value_type hi,
                       MyFlipSet& flipset)
  {
    if (LinearInterpolation::missing(z1) || LinearInterpolation::missing(z2) ||
        LinearInterpolation::missing(z3))
      return;

    place_type c1 = placement(z1, lo, hi);
    place_type c2 = placement(z2, lo, hi);
    place_type c3 = placement(z3, lo, hi);
    triangle(x1, y1, z1, c1, x2, y2, z2, c2, x3, y3, z3, c3, lo, hi, flipset);
  }

  // ** line-mode **

  static void triangle(coord_type x1,
                       coord_type y1,
                       value_type z1,
                       coord_type x2,
                       coord_type y2,
                       value_type z2,
                       coord_type x3,
                       coord_type y3,
                       value_type z3,
                       value_type value,
                       MyFlipSet& flipset)
  {
    if (LinearInterpolation::missing(z1) || LinearInterpolation::missing(z2) ||
        LinearInterpolation::missing(z3))
      return;

    place_type c1 = placement(z1, value);
    place_type c2 = placement(z2, value);
    place_type c3 = placement(z3, value);

    // There are no intersections to be calculated if everything
    // is in the same region
    if (c1 == c2 && c2 == c3)
      return;

    // Handling Inside values is easiest, if we simply require
    // that the triangle contains atleast one Below value.
    // This is just a matter of defining what the contour line
    // really represents, and we really cannot make a sensible
    // definition in areas of constant value. Hence we need to
    // loosen the definition somewhat, and use either a supremum
    // or an infinum type definition. We require Below for
    // backward compatibility with the old Imagine library.

    if (c1 != Below && c2 != Below && c3 != Below)
      return;

    // After this, the only case which does not produce
    // a line segment is an Above+Above+Inside combination.
    // In all other cases the joining edges are either Above+Below
    // or Above+Inside, a Below+Inside intersection can be
    // disregarded. The special case triangle Above+Inside+Inside
    // is handled automatically. The case of Above+Above+Inside
    // reduces to a point, which can be easily tested for afterwards
    // as an unique case.

    // Note that we choose to select the orientation of the lines
    // so that the edge going towards the Above vertex is always the
    // source, this guarantees a consistent orientation. It is helpful
    // to imagine contouring from -inf to isovalue, and think what
    // the resulting orientation would then be.

    SmallVector<coord_type, 10U> x, y;
    intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
    intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
    place_type final_place = c3;
    if (x.size() != 2)
    {
      intersect(x, y, x3, y3, z3, c3, x1, y1, z1, c1, value);
      final_place = c1;
    }

    // flush the buffer, whose size should always be 2

    assert(x.size() == 2);

    // The extra test done by eflip is used to omit the Above+Above+Inside
    // case as redundant.

    if (final_place == Below)
      flipset.eflip(MyEdge(x[0], y[0], x[1], y[1]));
    else
      flipset.eflip(MyEdge(x[1], y[1], x[0], y[0]));
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
                        value_type value,
                        MyFlipSet& flipset)
  {
    if (LinearInterpolation::missing(z1))
    {
      triangle(x2, y2, z2, x3, y3, z3, x4, y4, z4, value, flipset);
    }
    else if (LinearInterpolation::missing(z2))
    {
      triangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, value, flipset);
    }
    else if (LinearInterpolation::missing(z3))
    {
      triangle(x1, y1, z1, x2, y2, z2, x4, y4, z4, value, flipset);
    }
    else if (LinearInterpolation::missing(z4))
    {
      triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, value, flipset);
    }
    else
    {
      place_type c1 = placement(z1, value);
      place_type c2 = placement(z2, value);
      place_type c3 = placement(z3, value);
      place_type c4 = placement(z4, value);

      // Quick code if c1=c2=c3=c4

      if (c1 == c2 && c2 == c3 && c3 == c4)
        return;

      // Note: We must determine whether there is a saddle in the data
      // without using the contour limits so that the same decision
      // will be made for all contours. We also want the isolines to match
      // isobands.

      bool saddlepoint = is_saddle(z1, z2, z3, z4);

      if (saddlepoint)
      {
        coord_type x0 = (x1 + x2 + x3 + x4) / 4;
        coord_type y0 = (y1 + y2 + y3 + y4) / 4;
        value_type z0 = (z1 + z2 + z3 + z4) / 4;

        triangle(x1, y1, z1, x2, y2, z2, x0, y0, z0, value, flipset);
        triangle(x2, y2, z2, x3, y3, z3, x0, y0, z0, value, flipset);
        triangle(x3, y3, z3, x4, y4, z4, x0, y0, z0, value, flipset);
        triangle(x4, y4, z4, x1, y1, z1, x0, y0, z0, value, flipset);
      }
      else
      {
        SmallVector<coord_type, 10U> x, y;
        if (c1 == Below)
        {
          if (c2 == Below)
          {
            if (c3 == Below)
            {
              if (c4 == Below)
              {
              }
              else  // BBBA
              {
                intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
              }
            }
            else
            {
              if (c4 == Below)  // BBAB
              {
                intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
              }
              else  // BBAA
              {
                intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
              }
            }
          }
          else
          {
            if (c3 == Below)
            {
              if (c4 == Below)  // BABB
              {
                intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
                intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              }
              else  // BABA
              {
                value_type z0 = (z1 + z2 + z3 + z4) / 4;
                place_type c0 = placement(z0, value);
                if (c0 == c1)  // Below
                {
                  intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
                  intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
                  flush_line(x, y, flipset);
                  intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                  intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                }
                else
                {
                  intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                  intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
                  flush_line(x, y, flipset);
                  intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
                  intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                }
              }
            }
            else
            {
              if (c4 == Below)  // BAAB
              {
                intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              }
              else  // BAAA
              {
                intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              }
            }
          }
        }
        else if (c2 == Below)
        {
          if (c3 == Below)
          {
            if (c4 == Below)  // ABBB
            {
              intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
            }
            else  // ABBA
            {
              intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
            }
          }
          else
          {
            if (c4 == Below)  // ABAB
            {
              value_type z0 = (z1 + z2 + z3 + z4) / 4;
              place_type c0 = placement(z0, value);
              if (c0 == c1)  // Above
              {
                intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
                intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
                flush_line(x, y, flipset);
                intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
              }
              else
              {
                intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
                intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
                flush_line(x, y, flipset);
                intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
                intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
              }
            }
            else  // ABAA
            {
              intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
              intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
            }
          }
        }
        else
        {
          if (c3 == Below)
          {
            if (c4 == Below)  // AABB
            {
              intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
              intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
            }
            else  // AABA
            {
              intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
              intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
            }
          }
          else
          {
            if (c4 == Below)  // AAAB
            {
              intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, value);
              intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, value);
            }
            else  // AAAA
            {
            }
          }
        }
        if (!x.empty())
          flush_line(x, y, flipset);
      }
    }
  }

  // A grid cell looks like a saddle point for some value z if that value would intersect
  // all the edges. Hence if the intersection of all the intervals represented by the
  // edges is not empty, there is a potential saddle point.

  static bool is_saddle(value_type z1, value_type z2, value_type z3, value_type z4)
  {
    auto lo = std::min(z1, z2);
    auto hi = std::max(z1, z2);
    lo = std::max(lo, std::min(z2, z3));
    hi = std::min(hi, std::max(z2, z3));
    if (lo >= hi)
      return false;
    lo = std::max(lo, std::min(z3, z4));
    hi = std::min(hi, std::max(z3, z4));
    if (lo >= hi)
      return false;
    lo = std::max(lo, std::min(z4, z1));
    hi = std::min(hi, std::max(z4, z1));
    return (hi > lo);
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
    // We assume the cell is ok and do not validate it

    // If only one corner is missing, we can contour the remaining
    // triangle. If two or more are missing, we cannot do anything.
    // Chosen CW-order is: 1,2,3,4

    if (LinearInterpolation::missing(z1))
    {
      triangle(x2, y2, z2, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
    }
    else if (LinearInterpolation::missing(z2))
    {
      triangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
    }
    else if (LinearInterpolation::missing(z3))
    {
      triangle(x1, y1, z1, x2, y2, z2, x4, y4, z4, lo, hi, flipset);
    }
    else if (LinearInterpolation::missing(z4))
    {
      triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);
    }
    else
    {
      place_type c1 = placement(z1, lo, hi);
      place_type c2 = placement(z2, lo, hi);
      place_type c3 = placement(z3, lo, hi);
      place_type c4 = placement(z4, lo, hi);

      // Quick code if c1=c2=c3=c4

      // This is a good speedup (30% at one point in time)
      if (c1 == c2 && c2 == c3 && c3 == c4)
      {
        // If all above or below there is nothing to do,
        // otherwise cover the entire rectangle

        if (c1 == Inside)
        {
          flipgrid.flipTop(gridx, gridy);
          flipgrid.flipRight(gridx, gridy);
          flipgrid.flipBottom(gridx, gridy);
          flipgrid.flipLeft(gridx, gridy);
        }
        return;
      }

      // Note: We must determine whether there is a saddle in the data
      // without using the contour limits so that the same decision
      // will be made for all contours.

      bool saddlepoint = is_saddle(z1, z2, z3, z4);

      if (!saddlepoint)
      {
        SmallVector<coord_type, 10U> x, y;
        intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, lo, hi);
        intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, lo, hi);
        intersect(x, y, x3, y3, z3, c3, x4, y4, z4, c4, lo, hi);
        intersect(x, y, x4, y4, z4, c4, x1, y1, z1, c1, lo, hi);
        flush_polygon(x, y, flipset);
      }
      else
      {
        coord_type x0 = (x1 + x2 + x3 + x4) / 4;
        coord_type y0 = (y1 + y2 + y3 + y4) / 4;
        value_type z0 = (z1 + z2 + z3 + z4) / 4;

        triangle(x1, y1, z1, x2, y2, z2, x0, y0, z0, lo, hi, flipset);
        triangle(x2, y2, z2, x3, y3, z3, x0, y0, z0, lo, hi, flipset);
        triangle(x3, y3, z3, x4, y4, z4, x0, y0, z0, lo, hi, flipset);
        triangle(x4, y4, z4, x1, y1, z1, x0, y0, z0, lo, hi, flipset);
      }
    }
  }

};  // class LinearInterpolation

}  // namespace Tron

// ======================================================================
