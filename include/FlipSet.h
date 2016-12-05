// ======================================================================
/*!
 *
 * Class FlipSet<T> is similar to set<T>, but instead of ignoring
 * a second insert of the same value the first instance will also
 * be removed. Thus any value which is inserted an even number of
 * times will not remain in the set.
 *
 * Full set functionality is not provided, since the idea is to
 * simply keep on inserting new objects until in the end the user
 * will simply iterate through the set. Note that we do not even
 * provide non-const iterators to make sure the user cannot
 * fudge with the intended functionality.
 */
// ======================================================================

#pragma once

#include <boost/foreach.hpp>
#include <algorithm>
#include <utility>

#define USE_STD_HASH_SET 0

#if USE_STD_HASH_SET
#include <unordered_set>
#else
#include <google/dense_hash_set>
#endif

#include <limits>

#include <vector>

namespace Tron
{
template <typename T>
class FlipSet
{
 public:
  typedef T value_type;

  typedef typename std::vector<value_type> storage_type;
  typedef typename storage_type::size_type size_type;
  typedef typename storage_type::const_iterator const_iterator;

  typedef typename google::dense_hash_set<value_type, std::hash<value_type> > internal_type;

  FlipSet()
  {
    typedef typename value_type::coord_type coord_type;
    // The values must differ, but Edge.h requires the values
    // to be symmetric so that operator== works
    itsFlipValues.set_empty_key(value_type(std::numeric_limits<coord_type>::infinity(),
                                           0,
                                           std::numeric_limits<coord_type>::infinity(),
                                           0));
    itsFlipValues.set_deleted_key(value_type(-std::numeric_limits<coord_type>::infinity(),
                                             0,
                                             -std::numeric_limits<coord_type>::infinity(),
                                             0));
  }

  const storage_type& edges() const { return itsValues; }
  void flip(const value_type& theValue)
  {
    std::pair<typename internal_type::iterator, bool> ret = itsFlipValues.insert(theValue);
    if (!ret.second) itsFlipValues.erase(ret.first);
  }

  void eflip(const value_type& theValue)
  {
    if (theValue.x1() != theValue.x2() || theValue.y1() != theValue.y2())
    {
      std::pair<typename internal_type::iterator, bool> ret = itsFlipValues.insert(theValue);
      if (!ret.second) itsFlipValues.erase(ret.first);
    }
  }

  void prepare()
  {
    itsValues.reserve(itsFlipValues.size());
    BOOST_FOREACH (const value_type& value, itsFlipValues)
      itsValues.push_back(value);
    sort(itsValues.begin(), itsValues.end());
  }

 private:
  storage_type itsValues;
  internal_type itsFlipValues;

};  // class FlipSet

}  // namespace Tron

// ======================================================================
