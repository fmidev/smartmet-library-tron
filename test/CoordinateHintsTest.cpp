// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace CoordinateHints
 */
// ======================================================================

#include "CoordinateHints.h"
#include "Missing.h"
#include "Traits.h"
#include <regression/tframe.h>
#include <iostream>
#include <vector>

using namespace std;

//! Protection against conflicts with global functions
namespace CoordinateHintsTest
{
/* Dummy grid with coordinates 2*i+j, i+2*j
 *
 *  X-coordinates:            Y-coordinates:
 *
 *
 *  ...                       ...
 *  2  4  6  8 10 12 14       4  5  6  7  8  9 10
 *  1  3  5  7  9 11 13       2  3  4  5  6  7  8
 *  0  2  4  6  8 10 12       0  1  2  3  4  5  6
 *
 */

template <typename T>
class Grid
{
 public:
  typedef T coord_type;
  typedef int size_type;
  size_type width() const { return itsWidth; }
  size_type height() const { return itsHeight; }
  coord_type x(size_type i, size_type j) const { return 2 * i + j; }
  coord_type y(size_type i, size_type j) const { return i + 2 * j; }

  Grid(size_type i, size_type j) : itsWidth(i), itsHeight(j) {}

 private:
  Grid();
  size_type itsWidth;
  size_type itsHeight;
};

// ----------------------------------------------------------------------
/*!
 * \brief Test Tron::CoordinateHints::rectangles
 */
// ----------------------------------------------------------------------

void rectangles()
{
  typedef Tron::Traits<int, int> MyTraits;
  typedef Grid<int> MyGrid;
  typedef Tron::CoordinateHints<MyGrid, MyTraits> MyHints;

  typedef MyHints::return_type rectangles;

  MyGrid grid(1000, 1000);

  {
    MyHints hints(grid);

    // Low left corner for trivial test
    rectangles r = hints.rectangles(0, 0, 5, 5);
    if (r.size() != 1) TEST_FAILED("box 0,0 5,5 should return one rectangle");
    if (r.front().x1 != 0) TEST_FAILED("box 0,0 5,5 should be 0,0 7,7 : x1 is not 0");
    if (r.front().y1 != 0) TEST_FAILED("box 0,0 5,5 should be 0,0 7,7 : y1 is not 0");
    if (r.front().x2 != 7) TEST_FAILED("box 0,0 5,5 should be 0,0 7,7 : x2 is not 7");
    if (r.front().y2 != 7) TEST_FAILED("box 0,0 5,5 should be 0,0 7,7 : y2 is not 7");

    // Completely outside
    r = hints.rectangles(10000, 10000, 20000, 20000);
    if (r.size() != 0) TEST_FAILED("box 10000,10000 20000,20000 should be empty");

    // In the center we get 19 6x6 rectangles which cover about 68% of the grid
    r = hints.rectangles(100, 100, 150, 150);
    if (r.size() == 0) TEST_FAILED("box 100,100 150,150 should not be empty");

    for (auto it = r.begin(); it != r.end(); ++it)
    {
#if 0
      cout << "Rectangle " << it->x1 << "," << it->y1 << "..." << it->x2 << "," << it->y2
           << " coordinates " << it->min_x << "," << it->min_y << " - " << it->max_x << ","
           << it->max_y << endl;
#endif
      bool outside = (100 > it->max_x || 100 > it->max_y || it->min_x > 150 || it->min_y > 150);
      if (outside) TEST_FAILED("Found a rectangle not overlapping box 100,100 150,150");
    }
  }

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * The actual test suite
 */
// ----------------------------------------------------------------------

class tests : public tframe::tests
{
  virtual const char* error_message_prefix() const { return "\n\t"; }
  void test(void) { TEST(rectangles); }
};

}  // namespace HintsTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "CoordinateHints" << endl << "==============" << endl;
  CoordinateHintsTest::tests t;
  return t.run();
}

// ======================================================================
