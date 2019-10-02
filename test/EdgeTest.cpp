// ======================================================================
/*!
 * \file
 * \brief Regression tests for Tron::Edge
 */
// ======================================================================

#include "Edge.h"
#include "Traits.h"
#include <regression/tframe.h>
#include <string>

using namespace std;

//! Protection against conflicts with global functions
namespace EdgeTest
{
// ----------------------------------------------------------------------
/*!
 * \brief Test comparisons
 */
// ----------------------------------------------------------------------

void comparisons()
{
  typedef Tron::Traits<int, int> MyTraits;

  Tron::Edge<MyTraits> e1(0, 0, 0, 1);
  Tron::Edge<MyTraits> e2(0, 0, 1, 0);
  Tron::Edge<MyTraits> e3(0, 1, 1, 1);
  Tron::Edge<MyTraits> e4(1, 0, 1, 1);

  Tron::Edge<MyTraits> g1(0, 1, 0, 0);
  Tron::Edge<MyTraits> g2(1, 0, 0, 0);
  Tron::Edge<MyTraits> g3(1, 1, 0, 1);
  Tron::Edge<MyTraits> g4(1, 1, 1, 0);

  if (!(e1 == g1)) TEST_FAILED("e1 == g1 failed");
  if (!(e2 == g2)) TEST_FAILED("e2 == g2 failed");
  if (!(e3 == g3)) TEST_FAILED("e3 == g3 failed");
  if (!(e4 == g4)) TEST_FAILED("e4 == g4 failed");

  if (!(e1 < e2)) TEST_FAILED("e1 < e2 failed");
  if (!(e1 < e3)) TEST_FAILED("e1 < e3 failed");
  if (!(e1 < e4)) TEST_FAILED("e1 < e4 failed");
  if (!(e2 < e3)) TEST_FAILED("e2 < e3 failed");
  if (!(e2 < e4)) TEST_FAILED("e2 < e4 failed");
  if (!(e3 < e4)) TEST_FAILED("e3 < e4 failed");

  if (e2 < e1) TEST_FAILED("e2 < e1 failed");
  if (e3 < e1) TEST_FAILED("e3 < e1 failed");
  if (e4 < e1) TEST_FAILED("e4 < e1 failed");
  if (e3 < e2) TEST_FAILED("e3 < e2 failed");
  if (e4 < e2) TEST_FAILED("e4 < e2 failed");
  if (e4 < e3) TEST_FAILED("e4 < e3 failed");

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
  void test(void) { TEST(comparisons); }
};

}  // namespace EdgeTest

//! The main program
int main(void)
{
  using namespace std;
  cout << endl << "Edge" << endl << "====" << endl;
  EdgeTest::tests t;
  return t.run();
}

// ======================================================================
