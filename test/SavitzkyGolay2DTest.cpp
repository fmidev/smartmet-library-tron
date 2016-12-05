// ======================================================================
/*!
 * \file
 * \brief Regression tests for Tron::SavitzkyGolay2D
 */
// ======================================================================

#include <iostream>
#include "SavitzkyGolay2D.h"
#include <regression/tframe.h>
#include <string>
#include <sstream>
#include <vector>
#include <cmath>
#include <cstdlib>

using namespace std;

template <typename T>
string tostr(const T& value)
{
  ostringstream out;
  out << value;
  return out.str();
}

//! Protection against conflicts with global functions
namespace SavitzkyGolay2DTest
{
// ----------------------------------------------------------------------
/*
 * A customized grid for testing purposes
 */
// ----------------------------------------------------------------------

class Matrix
{
 public:
  typedef float value_type;
  typedef int size_type;
  size_type width() const { return itsWidth; }
  size_type height() const { return itsHeight; }
  const value_type& operator()(size_type i, size_type j) const { return itsData[i + itsWidth * j]; }
  value_type& operator()(size_type i, size_type j) { return itsData[i + itsWidth * j]; }
  Matrix(size_type i, size_type j)
      : itsWidth(i), itsHeight(j), itsData(itsWidth * itsHeight, value_type())
  {
  }

  void swap(Matrix& other) { itsData.swap(other.itsData); }
 private:
  Matrix();
  size_type itsWidth;
  size_type itsHeight;
  std::vector<value_type> itsData;
};

// ----------------------------------------------------------------------

Matrix make_matrix(Matrix::size_type w, Matrix::size_type h)
{
  Matrix grid(w, h);

  for (Matrix::size_type j = 0; j < grid.height(); j++)
    for (Matrix::size_type i = 0; i < grid.width(); i++)
      grid(i, j) = sin(2.0 * i / grid.width() + 3.0 * j / grid.height());

  return grid;
}

// ----------------------------------------------------------------------

Matrix add_noise(const Matrix& matrix)
{
  Matrix grid(matrix.width(), matrix.height());

  unsigned int seed = 123456;

  for (Matrix::size_type j = 0; j < grid.height(); j++)
    for (Matrix::size_type i = 0; i < grid.width(); i++)
    {
      unsigned int tmp = rand_r(&seed);
      grid(i, j) += 1.0 * tmp / RAND_MAX;
    }

  return grid;
}

// ----------------------------------------------------------------------

double difference(const Matrix& one, const Matrix& two)
{
  double sum = 0;
  for (Matrix::size_type j = 0; j < one.height(); ++j)
    for (Matrix::size_type i = 0; i < one.width(); ++i)
      sum += (one(i, j) - two(i, j)) * (one(i, j) - two(i, j));

  return sum / (one.height() * one.width());
}

// ----------------------------------------------------------------------
/*!
 * \brief Test length 0 smoother
 */
// ----------------------------------------------------------------------

void length_0()
{
  // Create an i+j grid of size 20x20

  Matrix smoothgrid = make_matrix(20, 20);

  Matrix grid = add_noise(smoothgrid);

  Tron::SavitzkyGolay2D::smooth(grid, 0, 0);

  if (difference(grid, smoothgrid) < 0.0001) TEST_FAILED("Should do no smoothing at length 0");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test degree 0 smoother
 */
// ----------------------------------------------------------------------

void degree_0()
{
  // Create an i+j grid of size 20x20

  Matrix smoothgrid = make_matrix(20, 20);

  Matrix grid = add_noise(smoothgrid);

  Tron::SavitzkyGolay2D::smooth(grid, 5, 0);

  if (difference(grid, smoothgrid) < 0.0001) TEST_FAILED("Should do no smoothing at length 0");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 3 degree 1 smoother
 */
// ----------------------------------------------------------------------

void smooth_1_1()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 1, 1);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.9 * eps1) TEST_FAILED("Should reduce error better with size 1, degree 1");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 3 degree 2 smoother
 */
// ----------------------------------------------------------------------

void smooth_1_2()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 1, 2);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.95 * eps1) TEST_FAILED("Should reduce error better with size 1, degree 2");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 5 degree 1 smoother
 */
// ----------------------------------------------------------------------

void smooth_2_1()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 2, 1);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.8 * eps1) TEST_FAILED("Should reduce error better with size 2, degree 1");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 5 degree 2 smoother
 */
// ----------------------------------------------------------------------

void smooth_2_2()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 2, 2);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 2, degree 2");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 5 degree 3 smoother
 */
// ----------------------------------------------------------------------

void smooth_2_3()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 2, 3);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 2, degree 3");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 5 degree 4 smoother
 */
// ----------------------------------------------------------------------

void smooth_2_4()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 2, 4);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.95 * eps1) TEST_FAILED("Should reduce error better with size 2, degree 4");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 5 degree 5 smoother
 */
// ----------------------------------------------------------------------

void smooth_2_5()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 2, 5);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.95 * eps1) TEST_FAILED("Should reduce error better with size 2, degree 5");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 7 degree 1 smoother
 */
// ----------------------------------------------------------------------

void smooth_3_1()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 3, 1);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.8 * eps1) TEST_FAILED("Should reduce error better with size 3, degree 1");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 7 degree 2 smoother
 */
// ----------------------------------------------------------------------

void smooth_3_2()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 3, 2);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 3, degree 2");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 7 degree 3 smoother
 */
// ----------------------------------------------------------------------

void smooth_3_3()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 3, 3);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 3, degree 3");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 7 degree 4 smoother
 */
// ----------------------------------------------------------------------

void smooth_3_4()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 3, 4);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 3, degree 4");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 7 degree 5 smoother
 */
// ----------------------------------------------------------------------

void smooth_3_5()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 3, 5);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 3, degree 5");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 13 degree 1 smoother
 */
// ----------------------------------------------------------------------

void smooth_6_1()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 6, 1);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 6, degree 1");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 13 degree 2 smoother
 */
// ----------------------------------------------------------------------

void smooth_6_2()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 6, 2);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.8 * eps1) TEST_FAILED("Should reduce error better with size 6, degree 2");

  TEST_PASSED();
}

// ----------------------------------------------------------------------
/*!
 * \brief Test size 13 degree 4 smoother
 */
// ----------------------------------------------------------------------

void smooth_6_4()
{
  Matrix smoothgrid = make_matrix(20, 20);
  Matrix grid = add_noise(smoothgrid);
  double eps1 = difference(grid, smoothgrid);
  Tron::SavitzkyGolay2D::smooth(grid, 6, 4);
  double eps2 = difference(grid, smoothgrid);

  if (eps2 > 0.85 * eps1) TEST_FAILED("Should reduce error better with size 6, degree 4");

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
  void test(void)
  {
    TEST(length_0);
    TEST(degree_0);
    TEST(smooth_1_1);
    TEST(smooth_1_2);
    TEST(smooth_2_1);
    TEST(smooth_2_2);
    TEST(smooth_2_3);
    TEST(smooth_2_4);
    TEST(smooth_2_5);
    TEST(smooth_3_1);
    TEST(smooth_3_2);
    TEST(smooth_3_3);
    TEST(smooth_3_4);
    TEST(smooth_3_5);
    TEST(smooth_6_1);
    TEST(smooth_6_2);
    TEST(smooth_6_4);
  }
};

}  // namespace SavitzkyGolay2DTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "SavitzkyGolay2D" << endl << "===============" << endl;
  SavitzkyGolay2DTest::tests t;
  return t.run();
}

// ======================================================================
