// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace FlipGrid
 */
// ======================================================================

#include "FlipGrid.h"
#include "Edge.h"
#include "FlipSet.h"
#include "Traits.h"
#include <regression/tframe.h>
#include <sstream>

using namespace Tron;
using namespace std;

//! Protection against conflicts with global functions
namespace FlipGridTest
{
template <typename T>
std::string tostring(const T& theValue)
{
  std::ostringstream output;
  output << theValue;
  return output.str();
}

template <typename T>
std::string flipset_tostring(T& theFlipSet)
{
  theFlipSet.prepare();
  const typename T::storage_type& edges = theFlipSet.edges();
  std::ostringstream out;
  for (typename T::const_iterator iter = edges.begin(); iter != edges.end(); ++iter)
  {
    out << iter->x1() << "," << iter->y1() << "--" << iter->x2() << "," << iter->y2() << " : ";
  }
  return out.str();
}

// ----------------------------------------------------------------------
/*
 * A customized grid for testing purposes
 */
// ----------------------------------------------------------------------

class Grid
{
 public:
  typedef int size_type;
  typedef int coord_type;

  size_type width() const { return itsWidth; }
  size_type height() const { return itsHeight; }
  int operator()(size_type i, size_type j) const { return itsData[i + itsWidth * j]; }
  int operator()(size_type i, size_type j) { return itsData[i + itsWidth * j]; }
  coord_type x(size_type i, size_type j) const { return i; }
  coord_type y(size_type i, size_type j) const { return j; }
  Grid(size_type i, size_type j) : itsWidth(i), itsHeight(j), itsData(itsWidth * itsHeight, 0) {}
 private:
  Grid();
  size_type itsWidth;
  size_type itsHeight;
  std::vector<int> itsData;
};

// ----------------------------------------------------------------------
/*!
 * Porting grid edges to a FlipSet
 */
// ----------------------------------------------------------------------

void copy()
{
  typedef Tron::Traits<double, double> MyTraits;
  typedef Tron::Edge<MyTraits> MyEdge;
  typedef Tron::FlipSet<MyEdge> MyFlipSet;

  Grid grid(10, 10);

  {
    FlipGrid flip(10, 10);
    flip.flipLeft(0, 0);
    flip.flipRight(0, 0);
    flip.flipTop(0, 0);
    flip.flipBottom(0, 0);

    MyFlipSet flipset;
    flip.copy(grid, flipset);
    string result = flipset_tostring(flipset);
    string ok = "0,0--0,1 : 0,1--1,1 : 1,0--0,0 : 1,1--1,0 : ";
    if (result != ok) TEST_FAILED("Expected " + ok + " got this instead: " + result);
  }

  {
    FlipGrid flip(10, 10);
    flip.flipLeft(0, 0);
    flip.flipRight(0, 0);
    flip.flipTop(0, 0);
    flip.flipBottom(0, 0);

    flip.flipLeft(1, 0);
    flip.flipRight(1, 0);
    flip.flipTop(1, 0);
    flip.flipBottom(1, 0);

    MyFlipSet flipset;
    flip.copy(grid, flipset);
    string result = flipset_tostring(flipset);
    string ok = "0,0--0,1 : 0,1--1,1 : 1,0--0,0 : 1,1--2,1 : 2,0--1,0 : 2,1--2,0 : ";
    if (result != ok) TEST_FAILED("Expected " + ok + " got this instead: " + result);
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
  void test(void) { TEST(copy); }
};

}  // namespace FlipGridTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "FlipGrid" << endl << "========" << endl;
  FlipGridTest::tests t;
  return t.run();
}

// ======================================================================
