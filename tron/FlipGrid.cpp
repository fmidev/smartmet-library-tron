// ======================================================================
/*
 * Indexing of edges based on corner indices is:
 *
 *           (i,j)   (2w-1)j+i
 *               +-----------------+
 *               |                 |
 * (2w-1)j+w+i-1 |                 |  (2w-1)j+w+i
 *               |                 |
 *               +-----------------+
 *                  (2w-1)(j+1)+i
 *
 * where w = width, h = height.
 *
 * Inverse calculation for horizontal segments:
 *
 *    i = idx % (2w-1)
 *    if i >= w-1 then
 *       i = i-w-1
 *    j = (idx-i)/(2w-1) = idx/(2w-1) in integer arithmetic
 *
 */
// ----------------------------------------------------------------------

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

FlipGrid::FlipGrid(size_t width, size_t height) : itsWidth(width), itsHeight(height), itsSize(0)
{
  if (width < 2) throw runtime_error("FlipGrid width must be atleast 2");
  if (height < 2) throw runtime_error("FlipGrid height must be atleast 2");

  const size_t n = (2 * width - 1) * height + width - 1;
  itsEdges.resize(n, None);
}

}  // namespace Tron

// ======================================================================
