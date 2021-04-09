// ======================================================================
/*!
 * \file
 * \brief Regression tests for Tron::SmallVector
 */
// ======================================================================

#include "SmallVector.h"
#include <regression/tframe.h>

using namespace std;

//! Protection against conflicts with global functions
namespace SmallVectorTest
{
// ----------------------------------------------------------------------

void empty()
{
  Tron::SmallVector<int, 10U> data;
  if (!data.empty())
    TEST_FAILED("Default constructed vector should be empty");

  TEST_PASSED();
}

// ----------------------------------------------------------------------

void size()
{
  Tron::SmallVector<int, 10U> data;
  if (data.size() != 0)
    TEST_FAILED("Default constructed vector size should be zero");

  data.push_back(1);
  if (data.size() != 1)
    TEST_FAILED("Vector size should be one after one push_back");

  data.push_back(2);
  if (data.size() != 2)
    TEST_FAILED("Vector size should be two after two push_back calls");

  TEST_PASSED();
}

// ----------------------------------------------------------------------

void clear()
{
  Tron::SmallVector<int, 10U> data;
  data.push_back(1);
  data.clear();
  if (!data.empty())
    TEST_FAILED("Vector should be empty after a clear() call");

  TEST_PASSED();
}

// ----------------------------------------------------------------------

void array_operator()
{
  Tron::SmallVector<int, 10U> data;
  data.push_back(1);
  data.push_back(2);
  if (data[0] != 1)
    TEST_FAILED("First array element should be 1");
  if (data[1] != 2)
    TEST_FAILED("Second array element should be 2");
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
    TEST(empty);
    TEST(size);
    TEST(clear);
    TEST(array_operator);
  }
};

}  // namespace SmallVectorTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "SmallVector" << endl << "===========" << endl;
  SmallVectorTest::tests t;
  return t.run();
}

// ======================================================================
