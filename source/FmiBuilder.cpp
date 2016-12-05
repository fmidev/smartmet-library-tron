#include "FmiBuilder.h"
#include <boost/unordered_map.hpp>
#include <boost/functional/hash.hpp>
#include <stack>

namespace Tron
{
// ----------------------------------------------------------------------
/*!
 * \brief A temporary struct for identifying identical coordinates
 */
// ----------------------------------------------------------------------

struct point
{
  point(double x, double y) : x(x), y(y) {}
  point(const geos::geom::Coordinate &c) : x(c.x), y(c.y) {}
  bool operator==(const point &other) const { return x == other.x && y == other.y; }
  bool operator<(const point &other) const { return (x != other.x ? x < other.x : y < other.y); }
  double x;
  double y;
};

std::size_t hash_value(const point &p)
{
  std::size_t hash = 0;
  boost::hash_combine(hash, boost::hash_value(p.x));
  boost::hash_combine(hash, boost::hash_value(p.y));
  return hash;
}

// ----------------------------------------------------------------------
/*!
 * \brief Constructor
 */
// ----------------------------------------------------------------------

FmiBuilder::FmiBuilder(boost::shared_ptr<geos::geom::GeometryFactory> theFactory)
    : itsResult(), itsFactory(theFactory)
{
}

// ----------------------------------------------------------------------
/*!
 * \brief Clean up in case the result() method is never called
 */
// ----------------------------------------------------------------------

FmiBuilder::~FmiBuilder() {}
// ----------------------------------------------------------------------
/*!
 * \brief Return the final result
 */
// ----------------------------------------------------------------------

boost::shared_ptr<geos::geom::Geometry> FmiBuilder::result() { return itsResult; }
}  // namespace Tron
