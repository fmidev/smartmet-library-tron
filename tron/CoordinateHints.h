// ======================================================================
/*
 * CoordinateHints builds information on the ranges of coordinates
 * in a grid, and on request will list all subgrids containing
 * the given bounding box of coordinates.
 *
 * The grid is expected to have the following minimum interface:
 *
 * class Traits
 * {
 *    typedef X coord_type;		// usually float or double
 * }
 *
 * class Grid
 * {
 *  public:
 *    typedef Y size_type;		// usually unsigned long
 *    const Traits::coord_type & x(size_type i, size_type j) const;
 *    const Traits::coord_type & y(size_type i, size_type j) const;
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
#include <stdexcept>
#include <vector>

namespace Tron
{
template <typename Grid, typename Traits>
class CoordinateHints : public Traits
{
 public:
  typedef typename Grid::size_type size_type;
  typedef typename Traits::coord_type coord_type;

  struct Rectangle
  {
    size_type x1 = 0;
    size_type y1 = 0;
    size_type x2 = 0;
    size_type y2 = 0;
    coord_type min_x = 0;
    coord_type min_y = 0;
    coord_type max_x = 0;
    coord_type max_y = 0;
    bool isvalid = false;
  };

  typedef std::vector<Rectangle> rectangles;

  CoordinateHints(const Grid& theGrid, size_type theMaxSize = 10)
      : itsMaxSize(theMaxSize), itsRoot(new RecursiveInfo())
  {
    if (theGrid.width() == 0 || theGrid.height() == 0)
      throw std::runtime_error("Cannot contour an empty grid");

    recurse(itsRoot, theGrid, 0, 0, theGrid.width() - 1, theGrid.height() - 1);
  }

  rectangles get_rectangles(coord_type theMinX,
                            coord_type theMinY,
                            coord_type theMaxX,
                            coord_type theMaxY) const
  {
#ifdef MYDEBUG
    std::cout << "Searching rect: " << theMinX << "," << theMinY << " - " << theMaxX << ","
              << theMaxY << std::endl;
#endif
    rectangles ret;
    if (find(ret, itsRoot, theMinX, theMinY, theMaxX, theMaxY))
      ret.push_back(itsRoot->itsRectangle);
    return ret;
  }

 private:
  CoordinateHints();

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

    bool isvalid = false;
#ifdef MYDEBUG
    bool issmall = false;
#endif
    coord_type min_x = 0;
    coord_type min_y = 0;
    coord_type max_x = 0;
    coord_type max_y = 0;

    if ((gwidth <= itsMaxSize && gheight <= itsMaxSize) || (gwidth <= 1 || gheight <= 1))
    {
// The rectangle is small enough now, find the extrema from it
#ifdef MYDEBUG
      issmall = true;
#endif

      for (size_type j = y1; j <= y2; j++)
        for (size_type i = x1; i <= x2; i++)
        {
          coord_type x = theGrid.x(i, j);
          coord_type y = theGrid.y(i, j);
          if (!this->missing(x) && !this->missing(y))
          {
            if (!isvalid)
            {
              isvalid = true;
              min_x = x;
              min_y = y;
              max_x = x;
              max_y = y;
            }
            else
            {
              min_x = std::min(min_x, x);
              min_y = std::min(min_y, y);
              max_x = std::max(max_x, x);
              max_y = std::max(max_y, y);
            }
          }
        }
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

      if (!theNode->itsLeft->itsRectangle.isvalid)
      {
        if (!theNode->itsRight->itsRectangle.isvalid)
          isvalid = false;
        else
        {
          isvalid = true;
          min_x = theNode->itsRight->itsRectangle.min_x;
          max_x = theNode->itsRight->itsRectangle.max_x;
          min_y = theNode->itsRight->itsRectangle.min_y;
          max_y = theNode->itsRight->itsRectangle.max_y;
        }
      }
      else
      {
        isvalid = true;
        min_x = theNode->itsLeft->itsRectangle.min_x;
        max_x = theNode->itsLeft->itsRectangle.max_x;
        min_y = theNode->itsLeft->itsRectangle.min_y;
        max_y = theNode->itsLeft->itsRectangle.max_y;

        if (theNode->itsRight->itsRectangle.isvalid)
        {
          min_x = std::min(min_x, theNode->itsRight->itsRectangle.min_x);
          max_x = std::max(max_x, theNode->itsRight->itsRectangle.max_x);
          min_y = std::min(min_y, theNode->itsRight->itsRectangle.min_y);
          max_y = std::max(max_y, theNode->itsRight->itsRectangle.max_y);
        }
      }
    }

    theNode->itsRectangle.isvalid = isvalid;
    theNode->itsRectangle.min_x = min_x;
    theNode->itsRectangle.min_y = min_y;
    theNode->itsRectangle.max_x = max_x;
    theNode->itsRectangle.max_y = max_y;

#ifdef MYDEBUG
    if (isvalid)
      std::cout << "Valid ";
    else
      std::cout << "      ";

    if (issmall)
      std::cout << "Small ";
    else
      std::cout << "      ";
    std::cout << "Rect result: " << x1 << "," << y1 << " - " << x2 << "," << y2 << "\t= " << min_x
              << "," << min_y << " - " << max_x << "," << max_y << std::endl;
#endif
  }

  bool rectangle_intersects(const Rectangle& theRectangle,
                            coord_type theXMin,
                            coord_type theYMin,
                            coord_type theXMax,
                            coord_type theYMax) const
  {
    if (!theRectangle.isvalid) return false;

    // No overlap if some mimimum is greater than the other maximum

    bool no_overlap = (theXMin > theRectangle.max_x || theYMin > theRectangle.max_y ||
                       theRectangle.min_x > theXMax || theRectangle.min_y > theYMax);

    return !no_overlap;
  }

  bool find(rectangles& theRectangles,
            const node_type& theNode,
            coord_type theXMin,
            coord_type theYMin,
            coord_type theXMax,
            coord_type theYMax) const
  {
    // Quick exit if the rectangle does not intersect at all

    bool ok = rectangle_intersects(theNode->itsRectangle, theXMin, theYMin, theXMax, theYMax);

    if (!ok) return false;

    bool haschildren = (theNode->itsLeft.get() != 0 && theNode->itsRight.get() != 0);

    if (!haschildren)
    {
      return true;
    }
    else
    {
      bool leftok = find(theRectangles, theNode->itsLeft, theXMin, theYMin, theXMax, theYMax);
      bool rightok = find(theRectangles, theNode->itsRight, theXMin, theYMin, theXMax, theYMax);
      if (leftok && rightok) return true;
      if (leftok) theRectangles.push_back(theNode->itsLeft->itsRectangle);
      if (rightok) theRectangles.push_back(theNode->itsRight->itsRectangle);
      return false;
    }
  }
};

}  // namespace Tron

// ======================================================================
