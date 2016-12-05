// ======================================================================
/*
 * LogLinear interpolation inside grid cells to
 * find contour line intersection coordinates.
 *
 */
// ======================================================================

#pragma once

#include "Edge.h"
#include "FlipSet.h"
#include "FlipGrid.h"
#include "Missing.h"
#include <cassert>
#include <iostream>
#include <vector>

namespace Tron
{
template <typename Traits>
class LogLinearInterpolation : public Traits
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

  // Note: log1p(a)-log1p(b) <> log1p(a/b) when b=0
  // Hence the mathematical "simplification" cannot be used here

  // Note: the base of the logarithmic interpolation is meaningless
  // since in calculating s the division cancels out the base

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
    // This test is absolutely necessary for handling value==lolimit cases
    if (z1 == z2)
    {
      x = x1;
      y = y1;
    }
    else if (z1 < 0 || z2 < 0 || value < 0)
    {
      // This is a user error, data must be nonnegative
    }
    else if (x1 < x2 || (x1 == x2 && y1 < y2))  // lexicographic sorting
    {
      value_type s = (log1p(value) - log1p(z2)) / (log1p(z1) - log1p(z2));
      x = x2 + s * (x1 - x2);
      y = y2 + s * (y1 - y2);
    }
    else
    {
      value_type s = (log1p(value) - log1p(z1)) / (log1p(z2) - log1p(z1));
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
    if (value < lo && !LogLinearInterpolation::missing(lo)) return Below;
    if (value >= hi && !LogLinearInterpolation::missing(hi)) return Above;
    return Inside;
  }

  static place_type placement(value_type value, value_type limit)
  {
    if (value < limit && !LogLinearInterpolation::missing(limit)) return Below;
    if (value > limit && !LogLinearInterpolation::missing(limit)) return Above;
    return Inside;
  }

  // Connect intersections found in a triangle.

  static void addedges(const std::vector<coord_type>& x,
                       const std::vector<coord_type>& y,
                       MyFlipSet& flipset)
  {
    for (unsigned int i = 0; i < x.size(); i++)
    {
      unsigned int j = (i + 1) % x.size();
      flipset.eflip(MyEdge(x[i], y[i], x[j], y[j]));
    }
  }

  static void intersect(std::vector<coord_type>& x,
                        std::vector<coord_type>& y,
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
        x.push_back(X);
        y.push_back(Y);
        x.push_back(x2);
        y.push_back(y2);
      }
      else
      {
        // Below Above
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        x.push_back(X);
        y.push_back(Y);
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        x.push_back(X);
        y.push_back(Y);
      }
    }
    else if (c1 == Inside)
    {
      if (c2 == Below)
      {
        // Inside Below
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        x.push_back(x1);
        y.push_back(y1);
        x.push_back(X);
        y.push_back(Y);
      }
      else if (c2 == Inside)
      {
        // Inside Inside
        x.push_back(x1);
        y.push_back(y1);
        x.push_back(x2);
        y.push_back(y2);
      }
      else
      {
        // Inside Above
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        x.push_back(x1);
        y.push_back(y1);
        x.push_back(X);
        y.push_back(Y);
      }
    }
    else
    {
      if (c2 == Below)
      {
        // Above Below
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        x.push_back(X);
        y.push_back(Y);
        intersect(x1, y1, z1, x2, y2, z2, lo, X, Y);
        x.push_back(X);
        y.push_back(Y);
      }
      else if (c2 == Inside)
      {
        // Above Inside
        intersect(x1, y1, z1, x2, y2, z2, hi, X, Y);
        x.push_back(X);
        y.push_back(Y);
        x.push_back(x2);
        y.push_back(y2);
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

  static void intersect(std::vector<coord_type>& x,
                        std::vector<coord_type>& y,
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

    if (!((c1 == Below) ^ (c2 == Below))) return;

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
    flipset.eflip(MyEdge(X1, Y1, x3, y3));
    flipset.eflip(MyEdge(x3, y3, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X1, Y1));
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
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, X3, Y3));
    flipset.eflip(MyEdge(X3, Y3, X4, Y4));
    flipset.eflip(MyEdge(X4, Y4, X1, Y1));
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
    flipset.eflip(MyEdge(x1, y1, X1, Y1));
    flipset.eflip(MyEdge(X1, Y1, X2, Y2));
    flipset.eflip(MyEdge(X2, Y2, x2, y2));
    flipset.eflip(MyEdge(x2, y2, x1, y1));
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
          triangle_BBA(x1, y1, z1, x3, y3, z3, x2, y2, z2, lo, hi, flipset);  // BAB
        else if (c3 == Inside)
          triangle_BIA(x1, y1, z1, x3, y3, z3, x2, y2, z2, lo, hi, flipset);  // BAI
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
          triangle_BII(x2, y2, z2, x1, y1, z1, x3, y3, z3, lo, hi, flipset);  // IBI
        else
          triangle_BIA(x2, y2, z2, x1, y1, z1, x3, y3, z3, lo, hi, flipset);  // IBA
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
          triangle_IIA(x1, y1, z1, x3, y3, z3, x2, y2, z2, lo, hi, flipset);  // IAI
        else                                                                  // c3=Above
          triangle_IAA(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);  // IAA
      }
    }
    else
    {
      if (c2 == Below)
      {
        if (c3 == Below)
          triangle_BBA(x3, y3, z3, x2, y2, z2, x1, y1, z1, lo, hi, flipset);  // ABB
        else if (c3 == Inside)
          triangle_BIA(x2, y2, z2, x3, y3, z3, x1, y1, z1, lo, hi, flipset);  // ABI
        else
          triangle_BAA(x2, y2, z2, x1, y1, z1, x3, y3, z3, lo, hi, flipset);  // ABA
      }
      else if (c2 == Inside)
      {
        if (c3 == Below)
          triangle_BIA(x3, y3, z3, x2, y2, z2, x1, y1, z1, lo, hi, flipset);  // AIB
        else if (c3 == Inside)
          triangle_IIA(x3, y3, z3, x2, y2, z2, x1, y1, z1, lo, hi, flipset);  // AII
        else                                                                  // c3=Above
          triangle_IAA(x2, y2, z2, x1, y1, z1, x3, y3, z3, lo, hi, flipset);  // AIA
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
    if (LogLinearInterpolation::missing(z1) || LogLinearInterpolation::missing(z2) ||
        LogLinearInterpolation::missing(z3))
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
    if (LogLinearInterpolation::missing(z1) || LogLinearInterpolation::missing(z2) ||
        LogLinearInterpolation::missing(z3))
      return;

    place_type c1 = placement(z1, value);
    place_type c2 = placement(z2, value);
    place_type c3 = placement(z3, value);

    // There are no intersections to be calculated if everything
    // is in the same region
    if (c1 == c2 && c2 == c3) return;

    // Handling Inside values is easiest, if we simply require
    // that the triangle contains atleast one Below value.
    // This is just a matter of defining what the contour line
    // really represents, and we really cannot make a sensible
    // definition in areas of constant value. Hence we need to
    // loosen the definition somewhat, and use either a supremum
    // or an infinum type definition. We require Below for
    // backward compatibility with the old Imagine library.

    if (c1 != Below && c2 != Below && c3 != Below) return;

    // After this, the only case which does not produce
    // a line segment is an Above+Above+Inside combination.
    // In all other cases the joining edges are either Above+Below
    // or Above+Inside, a Below+Inside intersection can be
    // disregarded. The special case triangle Above+Inside+Inside
    // is handled automatically. The case of Above+Above+Inside
    // reduces to a point, which can be easily tested for afterwards
    // as an unique case.

    // OK, now we can calculate the intersections.
    // In all edges we require an Above and something else,
    // that's it.

    std::vector<coord_type> x, y;
    intersect(x, y, x1, y1, z1, c1, x2, y2, z2, c2, value);
    intersect(x, y, x2, y2, z2, c2, x3, y3, z3, c3, value);
    intersect(x, y, x3, y3, z3, c3, x1, y1, z1, c1, value);

    // flush the buffer, whose size should always be 2

    assert(x.size() == 2);

    // And this test is used to omit the Above+Above+Inside
    // case as redundant

    flipset.eflip(MyEdge(x[0], y[0], x[1], y[1]));
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
    // We assume it is redundant to check the Y-coordinates too
    if (LogLinearInterpolation::missing(x1) || LogLinearInterpolation::missing(x2) ||
        LogLinearInterpolation::missing(x3) || LogLinearInterpolation::missing(x4))
    {
      return;
    }

    if (LogLinearInterpolation::missing(z1))
    {
      triangle(x2, y2, z2, x3, y3, z3, x4, y4, z4, value, flipset);
    }
    else if (LogLinearInterpolation::missing(z2))
    {
      triangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, value, flipset);
    }
    else if (LogLinearInterpolation::missing(z3))
    {
      triangle(x1, y1, z1, x2, y2, z2, x4, y4, z4, value, flipset);
    }
    else if (LogLinearInterpolation::missing(z4))
    {
      triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, value, flipset);
    }
    else
    {
      coord_type x0 = (x1 + x2 + x3 + x4) / 4;
      coord_type y0 = (y1 + y2 + y3 + y4) / 4;
      value_type z0 = expm1((log1p(z1) + log1p(z2) + log1p(z3) + log1p(z4)) / 4);
      triangle(x1, y1, z1, x2, y2, z2, x0, y0, z0, value, flipset);
      triangle(x2, y2, z2, x3, y3, z3, x0, y0, z0, value, flipset);
      triangle(x3, y3, z3, x4, y4, z4, x0, y0, z0, value, flipset);
      triangle(x4, y4, z4, x1, y1, z1, x0, y0, z0, value, flipset);
    }
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
    if (LogLinearInterpolation::missing(x1) || LogLinearInterpolation::missing(x2) ||
        LogLinearInterpolation::missing(x3) || LogLinearInterpolation::missing(x4))
    {
      return;
    }

    // If only one corner is missing, we can contour the remaining
    // triangle. If two or more are missing, we cannot do anything.

    if (LogLinearInterpolation::missing(z1))
    {
      triangle(x2, y2, z2, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
    }
    else if (LogLinearInterpolation::missing(z2))
    {
      triangle(x1, y1, z1, x3, y3, z3, x4, y4, z4, lo, hi, flipset);
    }
    else if (LogLinearInterpolation::missing(z3))
    {
      triangle(x1, y1, z1, x2, y2, z2, x4, y4, z4, lo, hi, flipset);
    }
    else if (LogLinearInterpolation::missing(z4))
    {
      triangle(x1, y1, z1, x2, y2, z2, x3, y3, z3, lo, hi, flipset);
    }
    else
    {
// If both limits are missing, all valid values are desired
// and hence we can simply cover the entire rectangle. However,
// -inf..+inf contouring is a special case which will be handled
// correctly by the if-tests coming up next. For normal cases
// we thus gain a tiny bit of speed by nothing bothering
// with these two if's

#if 0
		if(LogLinearInterpolation::missing(lo) && LogLinearInterpolation::missing(hi))
		{
		  flipgrid.flipTop(gridx,gridy);
		  flipgrid.flipRight(gridx,gridy);
		  flipgrid.flipBottom(gridx,gridy);
		  flipgrid.flipLeft(gridx,gridy);
		  return;
		}
#endif

      place_type c1 = placement(z1, lo, hi);
      place_type c2 = placement(z2, lo, hi);
      place_type c3 = placement(z3, lo, hi);
      place_type c4 = placement(z4, lo, hi);

      // Quick code if c1=c2=c3=c4

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

#if 0
		std::vector<coord_type> X, Y;
		intersect(X,Y,x1,y1,z1,c1,x2,y2,z2,c2,lo,hi);
		intersect(X,Y,x2,y2,z2,c2,x3,y3,z3,c3,lo,hi);
		intersect(X,Y,x3,y3,z3,c3,x4,y4,z4,c4,lo,hi);
		intersect(X,Y,x4,y4,z4,c4,x1,y1,z1,c1,lo,hi);
		addedges(X,Y,flipset);
#else
      // Subdivide into triangles to avoid problems with saddle points.
      // Note that we cannot simply skip subdivision if there is
      // no saddle point since an *adjacent* interval being contoured
      // might see the saddle point and do the subdivision. This means
      // adjacent contour fills would not have a common border, but
      // there would be gaps. Hence we always subdivide.
      //
      // One could test for a saddle point like this:
      //
      // bool saddlepoint = ((c1==c3 || c2==c4) && (c1!=c2 && c3!=c4));

      coord_type x0 = (x1 + x2 + x3 + x4) / 4;
      coord_type y0 = (y1 + y2 + y3 + y4) / 4;
      value_type z0 = expm1((log1p(z1) + log1p(z2) + log1p(z3) + log1p(z4)) / 4);
      place_type c0 = placement(z0, lo, hi);
      triangle(x1, y1, z1, c1, x2, y2, z2, c2, x0, y0, z0, c0, lo, hi, flipset);
      triangle(x2, y2, z2, c2, x3, y3, z3, c3, x0, y0, z0, c0, lo, hi, flipset);
      triangle(x3, y3, z3, c3, x4, y4, z4, c4, x0, y0, z0, c0, lo, hi, flipset);
      triangle(x4, y4, z4, c4, x1, y1, z1, c1, x0, y0, z0, c0, lo, hi, flipset);
#endif
    }
  }

};  // class LinearInterpolation

}  // namespace Tron

// ======================================================================
