// ======================================================================
/*
 * Utilities for checking missing values
 */
// ======================================================================

#pragma once

#include <cmath>

// NFmiGlobals.h defines kFloatMissing to be 32700

namespace Tron
{
// There are no missing values

template <typename T>
struct NotMissing
{
  static bool missing(T value) { return false; }
};

// NaN is a missing value

template <typename T>
struct NanMissing
{
  static bool missing(T value) { return isnan(value); }
};

// NaN and +-Inf are considered missing
template <typename T>
struct InfMissing
{
  static bool missing(T value) { return isinf(value) || isnan(value); }
};

// FMI specific case is stateless to avoid creation costs

template <typename T>
struct FmiMissing
{
  static bool missing(T value) { return value == 32700; }
};

}  // namespace Tron

// ======================================================================
