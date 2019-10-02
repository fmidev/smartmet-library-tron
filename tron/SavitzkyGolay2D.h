// ======================================================================
/*!
 * \brief 2D Savitzky-Golay smoothing
 *
 */
// ======================================================================

#pragma once

#include "MirrorMatrix.h"
#include "SavitzkyGolay2DCoefficients.h"
#include <cmath>
#include <stdexcept>

namespace Tron
{
namespace SavitzkyGolay2D
{
// ----------------------------------------------------------------------
/*!
 * \brief Smoothen a matrix using a mirror matrix for boundary conditions
 *
 * Length is limited to range 0..6, degree to 0...5
 */
// ----------------------------------------------------------------------

template <typename Grid>
void smooth(Grid& input, std::size_t length, std::size_t degree)
{
  if (length == 0 || degree == 0) return;

  if (length > 6) length = 6;
  if (degree > 5) degree = 5;

  // Work with a copy

  Grid grid = input;
  MirrorMatrix<Grid> mirror(input);

  int n = 2 * length + 1;

  int* factor = SavitzkyGolay2DCoefficients::coeffs[length - 1][degree - 1];
  if (factor == 0) return;

  int denom = SavitzkyGolay2DCoefficients::denoms[length - 1][degree - 1];

  for (typename Grid::size_type jj = 0; jj < grid.height(); ++jj)
    for (typename Grid::size_type ii = 0; ii < grid.width(); ++ii)
    {
      typename Grid::value_type sum = 0;
      int k = 0;
      for (int j = 0; j < n; j++)
        for (int i = 0; i < n; i++)
          sum += (factor[k++] * mirror(ii + i - length, jj + j - length));
      if (!std::isnan(sum)) grid(ii, jj) = sum / denom;
    }

  input.swap(grid);
}
}  // namespace SavitzkyGolay2D
}  // namespace Tron
