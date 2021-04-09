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
 *
 * Note that we have altered the meaning of equivalency when using
 * the container types. We consider an edge a duplicate if we find
 * a match in which the end points are reversed. The == and < operators
 * in Edge.h are deliberately screwed to get the desired effect in
 * various containers.
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
#include "robin_hood.h"
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

#if USE_STD_HASH_SET
  using internal_type = std::unordered_set<value_type>;
#else
  // Use the Robin Hood set which appears to be the fastest for Edges:

  // using internal_type = robin_hood::unordered_set<value_type>;
  using internal_type = robin_hood::unordered_flat_set<value_type>;
  // using internal_type = robin_hood::unordered_node_set<value_type>;
#endif

  FlipSet() {}

  const storage_type& edges() const { return itsValues; }
  const_iterator begin() const { return itsValues.begin(); }
  const_iterator end() const { return itsValues.end(); }
  size_type size() const { return itsValues.size(); }
  bool empty() const { return itsValues.empty(); }
  void clear() { itsValues.clear(); }
  void flip(const value_type& theValue)
  {
    std::pair<typename internal_type::iterator, bool> ret = itsFlipValues.insert(theValue);
    if (!ret.second)
      itsFlipValues.erase(ret.first);
  }

  void eflip(const value_type& theValue)
  {
    if (theValue.x1() != theValue.x2() || theValue.y1() != theValue.y2())
      flip(theValue);
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
