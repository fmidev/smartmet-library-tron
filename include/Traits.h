// ======================================================================
/*
 * Type traits for contouring
 */
// ======================================================================

#pragma once

#include "Missing.h"

namespace Tron
{
template <typename Value, typename Coordinate, template <typename> class Missing = NotMissing>
struct Traits : public Missing<Value>
{
  typedef Value value_type;
  typedef Coordinate coord_type;
};
}

// ======================================================================
