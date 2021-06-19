// ======================================================================
/*
 * Ring is a temporary container for a simple connected polyline, which
 * might be closed, in which case the winding of the ring indicates
 * whether the ring is an exterior ring (CW) or a hole (CCW).
 * If the ring is closed, the first and last coordinates are equal.
 *
 * The ring is assumed to contain distinct points only.
 */
// ======================================================================

#pragma once

// Experimentally (global EC world, all times, all t2m contours) lists are faster than vectors
#define USE_RING_LIST 1

#include <boost/math/constants/constants.hpp>

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <list>
#include <sstream>
#include <string>

namespace Tron
{
template <typename Traits>
class Ring
{
 public:
  using coord_type = typename Traits::coord_type;
  using value_type = std::pair<coord_type, coord_type>;
  using storage_type = std::list<value_type>;
  using size_type = typename storage_type::size_type;
  using iterator = typename storage_type::iterator;
  using const_iterator = typename storage_type::const_iterator;

  Ring() = default;
  Ring(coord_type x1, coord_type y1, coord_type x2, coord_type y2) : itsData(), itsAreaOK(false)
  {
    itsData.push_back(value_type(x1, y1));
    itsData.push_back(value_type(x2, y2));
  }

  bool empty() const { return itsData.empty(); }
  size_type size() const { return itsData.size(); }
  const_iterator begin() const { return itsData.begin(); }
  const_iterator end() const { return itsData.end(); }
  const value_type& front() const { return itsData.front(); }
  const value_type& back() const { return itsData.back(); }
  bool closed() const
  {
    if (empty())
      return false;
    return (itsData.back() == itsData.front());
  }

  // Note: This is intentionally not thread safe. You're not
  // supposed to share Rings between threads.
  coord_type signedArea() const
  {
    if (itsAreaOK)
      return itsArea;

    if (itsData.size() < 2)
      return 0;

    coord_type area = 0;
    const_iterator next = itsData.begin();
    const_iterator prev = next;
    for (const_iterator end = itsData.end(); ++next != end;)
    {
      area += (next->first - prev->first) * (prev->second + next->second);
      prev = next;
    }
    itsArea = area / 2;
    itsAreaOK = true;
    return itsArea;
  }

  // Treating zero-size holes as anti-clockwise leads to trouble, hence equality is included
  bool isClockWise() const { return (signedArea() >= 0); }
  // Try to close the path with a (critical) edge, return true if succesful
  bool close(coord_type x1, coord_type y1, coord_type x2, coord_type y2)
  {
    if (itsData.back().first == x1 && itsData.back().second == y1 && itsData.front().first == x2 &&
        itsData.front().second == y2)
    {
      itsData.push_back(value_type(x2, y2));
      itsAreaOK = false;
      return true;
    }
    return false;
  }

  // Try to extend the end of the polyline
  bool extendEnd(coord_type x1, coord_type y1, coord_type x2, coord_type y2)
  {
    if (itsData.back().first != x1 || itsData.back().second != y1)
      return false;
    itsData.push_back(value_type(x2, y2));
    itsAreaOK = false;
    return true;
  }

  // Try to extend the start of the polyline with another
  bool extendStart(Ring& other, coord_type x1, coord_type y1, coord_type x2, coord_type y2)
  {
    if (itsData.front().first != x1 || itsData.front().second != y1)
      return false;
    if (other.itsData.back().first != x2 || other.itsData.back().second != y2)
      return false;
    itsData.pop_front();                             // drop the old x1,y1
    itsData.splice(itsData.begin(), other.itsData);  // this will reintroduce it
    return true;
  }

  // Try to extend the start of the polyline with another
  bool extendStart(Ring& other)
  {
    if (itsData.front().first != other.itsData.back().first ||
        itsData.front().second != other.itsData.back().second)
      return false;
    itsData.pop_front();                             // drop the old x1,y1
    itsData.splice(itsData.begin(), other.itsData);  // this will reintroduce it
    return true;
  }

  // Mostly for debugging purposes

  std::string asText(int precision = 3) const
  {
    std::ostringstream out;
    out << std::setprecision(precision) << std::fixed;
    for (typename storage_type::const_iterator it = begin(); it != end();)
    {
      out << it->first << " " << it->second;
      if (++it != end())
        out << ',';
    }
    return out.str();
  }

  // Remove self-touching ring from the end
  Ring removeSelfTouch()
  {
    Ring ring;
    coord_type x = itsData.back().first;
    coord_type y = itsData.back().second;

    iterator pos = --itsData.end();
    while (--pos != itsData.begin())
    {
      if (pos->first == x && pos->second == y)
      {
        std::copy(pos, itsData.end(), std::back_inserter(ring.itsData));
        itsData.erase(++pos, itsData.end());
        return ring;
      }
    }
    throw std::runtime_error("Failed to extract self-touching ring from polyline");
  }

  double endAngle() const
  {
    const_iterator pos = --end();
    double x2 = pos->first;
    double y2 = pos->second;
    --pos;
    double x1 = pos->first;
    double y1 = pos->second;
    return atan2(y2 - y1, x2 - x1) * boost::math::double_constants::radian;
  }

  // For speed
  void swap(Ring& other) { std::swap(itsData, other.itsData); }

 private:
  storage_type itsData;
  // The user won't see these changing
  mutable coord_type itsArea;
  mutable bool itsAreaOK = false;

};  // class Ring

// std::swap will see this
template <typename Traits>
void swap(Ring<Traits>& lhs, Ring<Traits>& rhs)
{
  lhs.swap(rhs);
}

}  // namespace Tron

// ======================================================================
