// ======================================================================
/*!
 * An edge is a container for x1,y1 x2,y2 coordinate pairs.
 * Edge objects are used by the contourer while building
 * the contour paths. Notable features are:
 *
 *  - empty() to test for a 0-length edge
 *  - operator== for comparisons
 *  - operator< so edges can be placed in sets
 */
// ======================================================================

#pragma once

#include <boost/functional/hash.hpp>
#include <boost/math/constants/constants.hpp>

#include <algorithm>

namespace Tron
{
template <typename Traits>
class Edge
{
 public:
  typedef typename Traits::coord_type coord_type;
  typedef typename std::pair<coord_type, coord_type> coordinate_type;

  // Needed for containers
  Edge() {}
  Edge(coord_type theX1, coord_type theY1, coord_type theX2, coord_type theY2)
      : itsX1(theX1), itsY1(theY1), itsX2(theX2), itsY2(theY2)
  {
  }

  coord_type x1() const { return itsX1; }
  coord_type y1() const { return itsY1; }
  coord_type x2() const { return itsX2; }
  coord_type y2() const { return itsY2; }
  // We're intentionally altering the meaning of equivalence here,
  // since in contouring a match can only occur when the other
  // edge is reversed: it is going the other way round in the
  // adjacent cell.
  // Note: undordered_set requires operator== to work when
  //       the hashes match. However, since contouring will
  //       only ever produce the same edge twice if they
  //       are in opposite directions, we do not need to
  //       test for absolute equality. However, the hasher
  //       DOES need to guarantee the returned value
  //       is the same regardless of the order, hence the
  //       min & max functions.

  bool operator==(const Edge<Traits>& theEdge) const
  {
    return (itsX1 == theEdge.itsX2 && itsY1 == theEdge.itsY2 && itsX2 == theEdge.itsX1 &&
            itsY2 == theEdge.itsY1);
  }

  // We want lexicographic ordering to guarantee edges which start
  // from the same point are consecutive when sorted
  bool operator<(const Edge<Traits>& theEdge) const
  {
    if (itsX1 != theEdge.itsX1) return itsX1 < theEdge.itsX1;
    if (itsY1 != theEdge.itsY1) return itsY1 < theEdge.itsY1;
    if (itsX2 != theEdge.itsX2) return itsX2 < theEdge.itsX2;
    return itsY2 < theEdge.itsY2;
  }

  bool operator==(const coordinate_type& theCoord) const
  {
    return (itsX1 == theCoord.first && itsY1 == theCoord.second);
  }

  bool operator<(const coordinate_type& theCoord) const
  {
    if (itsX1 != theCoord.first) return itsX1 < theCoord.first;
    return itsY1 < theCoord.second;
  }

  double angle() const
  {
    return atan2(itsY2 - itsY1, itsX2 - itsX1) * boost::math::double_constants::radian;
  }

 private:
  coord_type itsX1;
  coord_type itsY1;
  coord_type itsX2;
  coord_type itsY2;

};  // class Edge

}  // namespace Tron

namespace std
{
template <typename Traits>
struct hash<Tron::Edge<Traits> >
{
  // We want to find matches fast ignoring the orientation
  std::size_t operator()(const Tron::Edge<Traits>& theEdge) const
  {
    std::size_t hash = 0;
    boost::hash_combine(hash, std::min(theEdge.x1(), theEdge.x2()));
    boost::hash_combine(hash, std::min(theEdge.y1(), theEdge.y2()));
    boost::hash_combine(hash, std::max(theEdge.x1(), theEdge.x2()));
    boost::hash_combine(hash, std::max(theEdge.y1(), theEdge.y2()));
    return hash;
  }
};
}  // namespace std

// ======================================================================
