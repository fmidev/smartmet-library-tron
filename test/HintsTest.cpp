// ======================================================================
/*!
 * \file
 * \brief Regression tests for namespace Hints
 */
// ======================================================================

#include "Hints.h"
#include "Missing.h"
#include "Traits.h"
#include <regression/tframe.h>
#include <iostream>
#include <vector>

using namespace std;

//! Protection against conflicts with global functions
namespace HintsTest
{
template <typename T>
class Grid
{
 public:
  typedef T value_type;
  typedef int size_type;
  size_type width() const { return itsWidth; }
  size_type height() const { return itsHeight; }
  const value_type& operator()(size_type i, size_type j) const
  {
    return itsData[i + itsHeight * j];
  }
  value_type& operator()(size_type i, size_type j) { return itsData[i + itsHeight * j]; }
  Grid(size_type i, size_type j)
      : itsWidth(i), itsHeight(j), itsData(itsWidth * itsHeight, value_type())
  {
  }

 private:
  Grid();
  size_type itsWidth;
  size_type itsHeight;
  std::vector<value_type> itsData;
};

// ----------------------------------------------------------------------
/*!
 * \brief Test Tron::Hints::rectangles
 */
// ----------------------------------------------------------------------

void rectangles()
{
  typedef Tron::Traits<int, int> MyTraits;
  typedef Grid<MyTraits::value_type> MyGrid;
  typedef Tron::Hints<MyGrid, MyTraits> MyHints;

  MyGrid data(100, 100);
  for (int j = 0; j < data.height(); j++)
    for (int i = 0; i < data.width(); i++)
      data(i, j) = i + j;

  {
    MyHints hints(data, 10);

    auto r = hints.get_rectangles(-99, -98);

    if (r.size() != 0) TEST_FAILED("i+j interval -99..-98 should be empty");

    r = hints.get_rectangles(998, 999);
    if (r.size() != 0) TEST_FAILED("i+j interval 998..999 should be empty");

    r = hints.get_rectangles(0, 5);
    if (r.size() != 1) TEST_FAILED("Failed to extract i+j interval 0...5");
    if (r.front().x1 != 0) TEST_FAILED("i+j interval 0...5 1st x1<>0");
    if (r.front().y1 != 0) TEST_FAILED("i+j interval 0...5 1st y1<>0");
    if (r.front().x2 != 6) TEST_FAILED("i+j interval 0...5 1st x2<>6");
    if (r.front().y2 != 6) TEST_FAILED("i+j interval 0...5 1st y2<>6");
    if (r.front().minimum != 0) TEST_FAILED("i+j interval 0...5 1st minimum<>0");
    if (r.front().maximum != 12) TEST_FAILED("i+j interval 0...5 1st maximum<>12");

    r = hints.get_rectangles(0, 10);
    if (r.size() != 2) TEST_FAILED("Failed to extract i+j interval 0...10");

    if (r.front().x1 != 0) TEST_FAILED("i+j interval 0...10 1st x1<>0");
    if (r.front().y1 != 6) TEST_FAILED("i+j interval 0...10 1st y1<>6");
    if (r.front().x2 != 6) TEST_FAILED("i+j interval 0...10 1st x2<>6");
    if (r.front().y2 != 12) TEST_FAILED("i+j interval 0...10 1st y2<>12");
    if (r.front().minimum != 6) TEST_FAILED("i+j interval 0...10 1st minimum<>6");
    if (r.front().maximum != 18) TEST_FAILED("i+j interval 0...10 1st maximum<>18");

    if (r.back().x1 != 0) TEST_FAILED("i+j interval 0...10 2nd x1<>0");
    if (r.back().y1 != 0) TEST_FAILED("i+j interval 0...10 2nd y1<>0");
    if (r.back().x2 != 12) TEST_FAILED("i+j interval 0...10 2nd x2<>12");
    if (r.back().y2 != 6) TEST_FAILED("i+j interval 0...10 2nd y2<>6");
    if (r.back().minimum != 0) TEST_FAILED("i+j interval 0...10 2nd minimum<>0");
    if (r.back().maximum != 18) TEST_FAILED("i+j interval 0...10 2nd maximum<>18");

    r = hints.get_rectangles(0);
    if (r.size() != 1) TEST_FAILED("i+j value 0 failed");
    if (r.front().x1 != 0) TEST_FAILED("i+j value 0 1st x1<>0");
    if (r.front().y1 != 0) TEST_FAILED("i+j value 0 1st y1<>0");
    if (r.front().x2 != 6) TEST_FAILED("i+j value 0 1st x2<>6");
    if (r.front().y2 != 6) TEST_FAILED("i+j value 0 1st y2<>6");

    r = hints.get_rectangles(10);
    if (r.size() != 2) TEST_FAILED("i+j value 10 failed");
    if (r.front().x1 != 0) TEST_FAILED("i+j value 0 1st x1<>0");
    if (r.front().y1 != 6) TEST_FAILED("i+j value 0 1st y1<>6");
    if (r.front().x2 != 6) TEST_FAILED("i+j value 0 1st x2<>6");
    if (r.front().y2 != 12) TEST_FAILED("i+j value 0 1st y2<>12");
    if (r.back().x1 != 0) TEST_FAILED("i+j value 0 1st x1<>0");
    if (r.back().y1 != 0) TEST_FAILED("i+j value 0 1st y1<>0");
    if (r.back().x2 != 12) TEST_FAILED("i+j value 0 1st x2<>12");
    if (r.back().y2 != 6) TEST_FAILED("i+j value 0 1st y2<>6");

#if 0
	  for(Tron::Hints<Grid<int> >::return_type::const_iterator it = r.begin();
		  it != r.end();
		  ++it)
		{
		  cout << "Rectangle "
			   << it->x1
			   << ","
			   << it->y1
			   << "..."
			   << it->x2
			   << ","
			   << it->y2
			   << " values "
			   << it->minimum
			   << "..."
			   << it->maximum
			   << endl;
		}
#endif
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
  cout << endl << "Hints" << endl << "=====" << endl;
  HintsTest::tests t;
  return t.run();
}

// ======================================================================
