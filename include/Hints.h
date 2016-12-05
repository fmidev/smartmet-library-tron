// ======================================================================
/*
 * Hints builds information on the ranges of values inside
 * a grid, and upon request will list all subgrids containing
 * values the user is searching for.
 *
 * The grid is expected to have the following minimum interface:
 *
 * class Traits
 * {
 *    typedef X value_type;		// usually float or double
 * }
 *
 * class Grid
 * {
 *  public:
 *    typedef Y size_type;		// usually unsigned long
 *    const Traits::value_type & operator(size_type i, size_type j) const;
 *    size_type width() const;
 *    size_type height() const;
 * }
 *
 * Array indices are expected to run from 0...width-1 and 0...height-1.
 *
 *
 */
// ======================================================================

#pragma once

#include "Missing.h"
#include <boost/shared_ptr.hpp>
#include <list>
#include <stdexcept>

namespace Tron
{
template <typename Grid, typename Traits>
class Hints : public Traits
{
 public:
  typedef typename Grid::size_type size_type;
  typedef typename Traits::value_type value_type;

  struct Rectangle
  {
    size_type x1;
    size_type y1;
    size_type x2;
    size_type y2;
    value_type minimum;
    value_type maximum;
    bool hasmissing;
  };

  typedef std::list<Rectangle> return_type;

  Hints(const Grid& theGrid, size_type theMaxSize = 10)
      : itsMaxSize(theMaxSize), itsRoot(new RecursiveInfo())
  {
    if (theGrid.width() == 0 || theGrid.height() == 0)
      throw std::runtime_error("Cannot contour an empty grid");

    recurse(itsRoot, theGrid, 0, 0, theGrid.width() - 1, theGrid.height() - 1);
  }

  return_type rectangles(value_type theValue) const
  {
    return_type ret;
    if (find(ret, itsRoot, theValue)) ret.push_back(itsRoot->itsRectangle);
    return ret;
  }

  return_type rectangles(value_type theLoLimit, value_type theHiLimit) const
  {
    return_type ret;
    if (find(ret, itsRoot, theLoLimit, theHiLimit)) ret.push_back(itsRoot->itsRectangle);
    return ret;
  }

 private:
  Hints();

  struct RecursiveInfo
  {
    Rectangle itsRectangle;
    boost::shared_ptr<RecursiveInfo> itsLeft;
    boost::shared_ptr<RecursiveInfo> itsRight;
  };

  size_type itsMaxSize;

  typedef boost::shared_ptr<RecursiveInfo> node_type;
  node_type itsRoot;

  void recurse(node_type& theNode,
               const Grid& theGrid,
               size_type x1,
               size_type y1,
               size_type x2,
               size_type y2)
  {
    theNode->itsRectangle.x1 = x1;
    theNode->itsRectangle.y1 = y1;
    theNode->itsRectangle.x2 = x2;
    theNode->itsRectangle.y2 = y2;

    size_type gwidth = x2 - x1;
    size_type gheight = y2 - y1;

    if ((gwidth <= itsMaxSize && gheight <= itsMaxSize) || (gwidth <= 1 || gheight <= 1))
    {
      // The rectangle is small enough now, find the extrema from it

      bool hasmissing = false;
      value_type minimum = theGrid(x1, y1);
      value_type maximum = theGrid(x1, y1);

      for (size_type j = y1; j <= y2; j++)
        for (size_type i = x1; i <= x2; i++)
        {
          value_type value = theGrid(i, j);
          if (this->missing(value))
            hasmissing = true;
          else
          {
            minimum = std::min(value, minimum);
            maximum = std::max(value, maximum);
          }
        }

      theNode->itsRectangle.hasmissing = hasmissing;
      theNode->itsRectangle.minimum = minimum;
      theNode->itsRectangle.maximum = maximum;
    }
    else
    {
      theNode->itsLeft.reset(new RecursiveInfo());
      theNode->itsRight.reset(new RecursiveInfo());

      // Recurse the longer edge first
      if (gwidth > gheight)
      {
        size_type x = (x1 + x2) / 2;
        recurse(theNode->itsLeft, theGrid, x1, y1, x, y2);
        recurse(theNode->itsRight, theGrid, x, y1, x2, y2);
      }
      else
      {
        size_type y = (y1 + y2) / 2;
        recurse(theNode->itsLeft, theGrid, x1, y1, x2, y);
        recurse(theNode->itsRight, theGrid, x1, y, x2, y2);
      }

      // Update node from children

      theNode->itsRectangle.hasmissing =
          (theNode->itsLeft->itsRectangle.hasmissing | theNode->itsRight->itsRectangle.hasmissing);

      const value_type& min1 = theNode->itsLeft->itsRectangle.minimum;
      const value_type& max1 = theNode->itsLeft->itsRectangle.maximum;

      const value_type& min2 = theNode->itsRight->itsRectangle.minimum;
      const value_type& max2 = theNode->itsRight->itsRectangle.maximum;

      if (this->missing(min1))
      {
        theNode->itsRectangle.minimum = min2;
        theNode->itsRectangle.maximum = max2;
      }
      else if (this->missing(min2))
      {
        theNode->itsRectangle.minimum = min1;
        theNode->itsRectangle.maximum = max1;
      }
      else
      {
        theNode->itsRectangle.minimum = std::min(min1, min2);
        theNode->itsRectangle.maximum = std::max(max1, max2);
      }
    }
  }

  bool rectangle_intersects(const Rectangle& theRectangle, value_type theValue) const
  {
    const value_type& nodemin = theRectangle.minimum;
    const value_type& nodemax = theRectangle.maximum;
    const bool nodemissing = this->missing(nodemin);  // no valid values at all?

    if (!this->missing(theValue))
    {
      if (nodemissing) return false;
      if (nodemin <= theValue && theValue <= nodemax) return true;
      return false;
    }
    else
    {
      return nodemissing;
    }
  }

  bool rectangle_intersects(const Rectangle& theRectangle,
                            value_type theLoLimit,
                            value_type theHiLimit) const
  {
    const value_type& nodemin = theRectangle.minimum;
    const value_type& nodemax = theRectangle.maximum;
    const bool nodemissing = this->missing(nodemin);  // no valid values at all?

    if (!this->missing(theLoLimit))
    {
      if (!this->missing(theHiLimit))  // searched range: x..y
      {
        if (nodemissing) return false;
        if (std::max(theLoLimit, nodemin) <= std::min(theHiLimit, nodemax)) return true;
        return false;
      }
      else  // searched range: x..inf
      {
        if (nodemissing) return false;
        if (nodemax >= theLoLimit) return true;
        return false;
      }
    }
    else
    {
      if (!this->missing(theHiLimit))  // searched range: -inf..y
      {
        if (nodemissing) return false;
        if (nodemin <= theHiLimit) return true;
        return false;
      }
      else  // searched range: -inf..inf
      {
        if (!nodemissing) return true;
        return false;
      }
    }
  }

  bool find(return_type& theRectangles, const node_type& theNode, value_type theValue) const
  {
    bool haschildren = (theNode->itsLeft.get() != 0 && theNode->itsRight.get() != 0);

    // Quick exit if the rectangle does not intersect at all

    bool ok = rectangle_intersects(theNode->itsRectangle, theValue);

    if (!ok) return false;

    if (!haschildren)
    {
      return true;
    }
    else
    {
      bool leftok = find(theRectangles, theNode->itsLeft, theValue);
      bool rightok = find(theRectangles, theNode->itsRight, theValue);
      if (leftok && rightok) return true;
      if (leftok) theRectangles.push_back(theNode->itsLeft->itsRectangle);
      if (rightok) theRectangles.push_back(theNode->itsRight->itsRectangle);
      return false;
    }
  }

  bool find(return_type& theRectangles,
            const node_type& theNode,
            value_type theLoLimit,
            value_type theHiLimit) const
  {
    bool haschildren = (theNode->itsLeft.get() != 0 && theNode->itsRight.get() != 0);

    // Quick exit if the rectangle does not intersect at all

    bool ok = rectangle_intersects(theNode->itsRectangle, theLoLimit, theHiLimit);

    if (!ok) return false;

    if (!haschildren)
    {
      return true;
    }
    else
    {
      bool leftok = find(theRectangles, theNode->itsLeft, theLoLimit, theHiLimit);
      bool rightok = find(theRectangles, theNode->itsRight, theLoLimit, theHiLimit);
      if (leftok && rightok) return true;
      if (leftok) theRectangles.push_back(theNode->itsLeft->itsRectangle);
      if (rightok) theRectangles.push_back(theNode->itsRight->itsRectangle);
      return false;
    }
  }
};

}  // namespace Tron

// ======================================================================
