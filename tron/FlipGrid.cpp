#include "FlipGrid.h"
#include <stdexcept>

using namespace std;

namespace Tron
{
// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

FlipGrid::FlipGrid(size_t width, size_t height)
    : itsWidth(width + 1), itsHeight(height + 1), itsSize(0)
{
  if (width < 2)
    throw runtime_error("FlipGrid width must be atleast 2");
  if (height < 2)
    throw runtime_error("FlipGrid height must be atleast 2");

  const size_t n = itsWidth * itsHeight;
  itsHorizontalEdges.resize(n, None);
  itsVerticalEdges.resize(n, None);
}

}  // namespace Tron

// ======================================================================
