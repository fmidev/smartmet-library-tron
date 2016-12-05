// ======================================================================
/*!
 * \file
 * \brief Regression tests for Tron::MirrorMatrix
 */
// ======================================================================

#include "MirrorMatrix.h"
#include <regression/tframe.h>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

template <typename T>
string tostr(const T& value)
{
  ostringstream out;
  out << value;
  return out.str();
}

//! Protection against conflicts with global functions
namespace MirrorMatrixTest
{
// ----------------------------------------------------------------------
/*
 * A customized grid for testing purposes
 */
// ----------------------------------------------------------------------

class Matrix
{
 public:
  typedef int value_type;
  typedef int size_type;
  size_type width() const { return itsWidth; }
  size_type height() const { return itsHeight; }
  const value_type& operator()(size_type i, size_type j) const
  {
    // cout << "Accessing " << i << "," << j << endl;
    return itsData[i + itsWidth * j];
  }
  value_type& operator()(size_type i, size_type j) { return itsData[i + itsWidth * j]; }
  Matrix(size_type i, size_type j)
      : itsWidth(i), itsHeight(j), itsData(itsWidth * itsHeight, value_type())
  {
  }

 private:
  Matrix();
  size_type itsWidth;
  size_type itsHeight;
  std::vector<value_type> itsData;
};

// ----------------------------------------------------------------------
/*!
 * \brief Test accessors
 */
// ----------------------------------------------------------------------

void accessors()
{
  // Create an i+j grid of size 10x5

  Matrix grid(10, 5);

  for (Matrix::size_type j = 0; j < grid.height(); j++)
    for (Matrix::size_type i = 0; i < grid.width(); i++)
      grid(i, j) = i + j;

  // Mirror it

  Tron::MirrorMatrix<Matrix> g(grid);

  // Test inner area

  int value;

  if ((value = g(3, 4)) != 3 + 4) TEST_FAILED("Value at 3,4 should be 3+4, not " + tostr(value));

  if ((value = g(1, 3)) != 1 + 3) TEST_FAILED("Value at 1,3 should be 1+3, not " + tostr(value));

  if ((value = g(5, 1)) != 5 + 1) TEST_FAILED("Value at 5,1 should be 5+1, not " + tostr(value));

  if ((value = g(5, 0)) != 5 + 0) TEST_FAILED("Value at 5,0 should be 5+0, not " + tostr(value));

  // Test south, west, east, north

  if ((value = g(5, -2)) != 5 - 2) TEST_FAILED("Value at 5,-2 should be 5-2, not " + tostr(value));

  if ((value = g(-2, 3)) != -2 + 3)
    TEST_FAILED("Value at -2,3 should be -2+3, not " + tostr(value));

  if ((value = g(10, 4)) != 10 + 4)
    TEST_FAILED("Value at 10,4 should be 10+4, not " + tostr(value));

  if ((value = g(4, 7)) != 4 + 7) TEST_FAILED("Value at 4,7 should be 4+7, not " + tostr(value));

  // NW

  if ((value = g(-2, 8)) != -2 + 8)
    TEST_FAILED("Value at -2,8 should be -2+8, not " + tostr(value));

  // SW

  if ((value = g(-2, -4)) != -2 - 4)
    TEST_FAILED("Value at -2,-4 should be -2-4, not " + tostr(value));

  // SE

  if ((value = g(13, -4)) != 13 - 4)
    TEST_FAILED("Value at 13,-4 should be 13-4, not " + tostr(value));

  // NE

  if ((value = g(13, 8)) != 13 + 8)
    TEST_FAILED("Value at 13,8 should be 13+8, not " + tostr(value));

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
  void test(void) { TEST(accessors); }
};

}  // namespace MirrorMatrixTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "MirrorMatrix" << endl << "============" << endl;
  MirrorMatrixTest::tests t;
  return t.run();
}

// ======================================================================
