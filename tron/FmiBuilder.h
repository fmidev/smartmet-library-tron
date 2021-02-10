// ======================================================================
/*!
l* Building utility for FMI.
 *
 * GeosBuilder is just too slow.
 *
 * Algorithm notes:
 *
 * The contourer produces automatically all edges directed so that
 * the "inside" is to the right of the edge. Hence all shells built
 * from the edges shall automatically be clock-wise and holes
 * counter clock-wise.
 *
 * Once all edges have been collected into a lexicographically sorted
 * vector, all polygons (and polylines) are built as follows:
 *
 * 1. Create a vector indices as long as the vector of edges.
 *    Initialize the values to -1 to indicate the edge has not
 *    been assigned yet to any polygon.
 * 2. Set an edgeindex to zero to indicate from where we'll
 *    start looking for a free edge to start a new polyline.
 * 3. Repeat:
 *    A) Pick the next edge to start a polygon, if one is available.
 *    B) Increment the polygon index to be assigned to the matching edges.
 *    C) Update edgeindex to the next edge for the next iteration.
 *    D) Start a polygon with the coordinates of the edge.
 *    Repeat:
 *      a) Find from the sorted edges the possible match to the polygon
 *         being built.
 *      b) If there are none, the polygon is done. Save it and go back to step 3.
 *      c) If there is a used match to the polygon itself, the polygon is now
 *         touching itself. Extract the self touching ring, save it, and remove it
 *         from the polyline being built. Increment the index for the remaining part.
 *      d) If there is only one and it has not been assigned yet, continue
 *         the polygon with the edge and mark it used.
 *      e) If there are multiple free edges, pick the one that turns most clockwise..
 *
 */
// ======================================================================

#pragma once

#include "Ring.h"

#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/utility.hpp>
#include <geos/algorithm/CGAlgorithmsDD.h>
#include <geos/geom/CoordinateArraySequence.h>
#include <geos/geom/CoordinateSequenceFactory.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/LineString.h>
#include <geos/geom/LinearRing.h>
#include <geos/geom/Polygon.h>
#include <geos/io/WKTWriter.h>
#include <geos/operation/valid/IsValidOp.h>
#include <cmath>
#include <map>
#include <memory>
#include <vector>

namespace Tron
{
class FmiBuilder : private boost::noncopyable
{
 public:
  ~FmiBuilder() = default;
  FmiBuilder(const FmiBuilder &other) = delete;
  FmiBuilder &operator=(const FmiBuilder &other) = delete;

  FmiBuilder(const geos::geom::GeometryFactory &theFactory);

  std::unique_ptr<geos::geom::Geometry> result();

  template <typename Traits, typename Edges>
  void build(const Edges &theEdges, bool fillmode);

 private:
  // The final result
  std::unique_ptr<geos::geom::Geometry> itsResult;

  // Used while building:
  const geos::geom::GeometryFactory &itsFactory;

};  // class FmiBuilder

// To which polyline is an edge assigned to
typedef std::vector<int> Targets;

// Representative non-vertical edge from a polyline

typedef std::vector<std::size_t> EdgeFromRing;

// ----------------------------------------------------------------------
/*!
 * \brief Validate a geometry
 */
// ----------------------------------------------------------------------

inline void validate(const std::unique_ptr<geos::geom::Geometry> &geom)
{
  if (!geom)
    return;
#if 0
  geos::operation::valid::IsValidOp validator(geom.get());
  // http://postgis.net/docs/using_postgis_dbmanagement.html#OGC_Validity
  if (!validator.isValid())
  {
    geos::io::WKTWriter writer;
    std::cerr << "\nWarning: Invalid geometry" << std::endl
              << "Message: " << validator.getValidationError()->getMessage() << std::endl
              << "toString: " << validator.getValidationError()->toString() << std::endl;
    std::cerr << writer.writeFormatted(geom.get()) << std::endl;
#if 0
		if(validator.getValidationError()->getErrorType() != geos::operation::valid::TopologyValidationError::eSelfIntersection)
		  throw std::runtime_error("Result does not validate");
#endif
  }
  else
  {
    std::cout << "Valid!" << std::endl;
  }
#endif
  return;
}

// ----------------------------------------------------------------------
/*!
 * \brief Find maximum edge width
 */
// ----------------------------------------------------------------------

template <typename Edges>
double find_maximum_edge_width(const Edges &edges)
{
  double maxwidth = -1;
  std::size_t nedges = edges.size();
  for (std::size_t i = 0; i < nedges; i++)
  {
    double width = std::abs(edges[i].x1() - edges[i].x2());
    maxwidth = std::max(maxwidth, width);
  }
  return maxwidth;
}

// ----------------------------------------------------------------------
/*!
 * \brief Pick the next free edge, or return -1 if none are available
 */
// ----------------------------------------------------------------------

template <typename Edges>
long pick_free_edge(const Edges &edges, const Targets &targets, long index)
{
  const std::size_t ntargets = targets.size();
  std::size_t i = boost::numeric_cast<std::size_t>(index);
  for (; i < ntargets; ++i)
  {
    if (targets[i] < 0)
      return i;
  }
  return -1;
}

// ----------------------------------------------------------------------
/*!
 * \brief Find the next edge for the polyline, or return -1 if there is none.
 */
// ----------------------------------------------------------------------

template <typename Polyline, typename Edges>
long find_first_match(
    const Polyline &polyline, const Edges &edges, const Targets &targets, long pos, long lastpos)
{
  const long nedges = boost::numeric_cast<long>(edges.size());

  // Last coordinate of the polyline
  const typename Polyline::value_type &endcoordinate = polyline.back();

  // A better guess than the last chosen edge is that we must skip roughly the same amount of edges
  // again

  pos += pos - lastpos;
  if (pos < 0)
    pos = 0;
  if (pos >= nedges)
    pos = nedges - 1;

  // Note: The last coordinate can never be the same as the start coordinate at the hint,
  // that would mean there was a 0-length edge.

  if (!(edges[pos] < endcoordinate))
  {
    // Search left for smaller coordinate, then test whether the next one was a match.

    for (--pos;; --pos)
    {
      if (pos < 0 || edges[pos] < endcoordinate)
      {
        if (edges[++pos] == endcoordinate)
          return pos;
        return -1;
      }
    }
  }
  else
  {
    // Search right for the first match or a greater coordinate
    for (++pos; pos < nedges; ++pos)
    {
      if (edges[pos] < endcoordinate)
      {
        // This is the most likely case and hence we test it first for speed
      }
      else if (edges[pos] == endcoordinate)
        return pos;
      else
        return -1;
    }
  }
  return -1;
}

// ----------------------------------------------------------------------
/*!
 * \brief Pick best edge to continue with, or return -1 if there is none.
 */
// ----------------------------------------------------------------------

template <typename Polylines, typename Polyline, typename Edges>
long pick_best_match(const Polylines &polylines,
                     const Polyline &polyline,
                     const Edges &edges,
                     const Targets &targets,
                     long pos,
                     long polylineindex,
                     bool *self_touch,
                     bool *isoline_extension)
{
  // Not touching itself
  *self_touch = false;

  // Now extending old isoline
  *isoline_extension = false;

  // Return no choice if there is nothing to choose from
  if (pos < 0)
    return pos;

  // Last coordinate of the polyline
  const typename Polyline::value_type &endcoordinate = polyline.back();

  // Handle quickly the most common case of exactly one match

  const long npolylines = boost::numeric_cast<long>(polylines.size());
  const long nedges = boost::numeric_cast<long>(edges.size());

#ifdef OPTIMIZE_ONE_CHOICE
  // This optimization is not robust for polylines

  if (pos + 1 == nedges || !(edges[pos + 1] == endcoordinate))
  {
    // No best pick if the edge is already taken. Could happen with polylines, not with polygons.

    if (targets[pos] < 0)
      return pos;

    if (targets[pos] == polylineindex)
      *self_touch = true;

    else if (targets[pos] >= npolylines)
      throw std::runtime_error(
          "Internal error trying to build a valid geometry, target polygon number overflow");

    else if (polylines[targets[pos]].closed())
      return -1;

    // There is a previous polyline we can extend.
    return pos;
  }
#endif

  // There must be multiple matches then, but perhaps not all are taken.
  // Also, if any of the matches has already been selected for the current polygon,
  // we have built a self-touching polyline, and need to extract the ring we have now
  // built. Note that there may be several edges leaving the same point, we actually
  // have to search the last node with the same coordinate to cut the ring.

  std::vector<long> available;

  for (long i = pos; i < nedges; i++)
  {
    if (!(edges[i] == endcoordinate))
      break;
    if (targets[i] < 0)
      available.push_back(i);
    else if (targets[i] == polylineindex)
      *self_touch = true;
    else if (!polylines[targets[i]].closed())
      // Non-closed polylines are viable candidates for continuation
      available.push_back(i);
  }

  // Nothing available?
  if (available.empty())
    return -1;

  // No need to calculate angles if there is only one choice remaining

  std::size_t bestpos = 0;

  if (available.size() == 1)
    bestpos = available[0];

  else
  {
    // Pick the edge that turns most clockwise with respect to the end of the polyline.
    // (most negative turn in range -180...+180)

    double bestangle = +999;
    double alpha1 = polyline.endAngle();

    for (std::size_t i = 0; i < available.size(); i++)
    {
      double alpha2 = edges[available[i]].angle();
      // The extra +360 makes sure the fmod argument is positive
      // The +180 and -180 make the result -180...+180
      double angle = fmod(alpha2 - alpha1 + 180 + 360, 360.0) - 180;  // symmetric modulo

      if (angle < bestangle)
      {
        bestangle = angle;
        bestpos = available[i];
      }
    }
  }

  // We're extending an old isoline if the best match is not closed. We also know it
  // will never be closed, otherwise the algorithm would have closed it already.

  if (targets[bestpos] >= 0 && targets[bestpos] < npolylines)
  {
    *isoline_extension = !polylines[targets[bestpos]].closed();
  }

  return bestpos;
}

// ----------------------------------------------------------------------
/*!
 * \brief Assign a new index for the given edges
 */
// ----------------------------------------------------------------------

template <typename EdgeIndexes>
void reindex_edges(Targets &targets, const EdgeIndexes &edgeindexes, long newindex)
{
  for (std::size_t i = 0; i < edgeindexes.size(); i++)
    targets[edgeindexes[i]] = newindex;
}

// ----------------------------------------------------------------------
/*!
 * \brief Find a non-vertical edge backwards from the given indexes
 */
// ----------------------------------------------------------------------

template <typename Edges, typename EdgeIndexes>
std::size_t representative_edge(const Edges &edges, const EdgeIndexes &edgeindexes)
{
  for (std::size_t i = edgeindexes.size() - 1; i > 0; i--)
  {
    std::size_t idx = edgeindexes[i];
    if (edges[idx].x1() != edges[idx].x2())
      return idx;
  }

  // Should never happen for polygons, just return zero to keep the compiler happy
  return 0;
}

// ----------------------------------------------------------------------
/*!
 * \brief Find the shell containing the hole which uses edges[edgeindex]
 *
 * holeindex is the polygon index assigned to the hole itself
 */
// ----------------------------------------------------------------------

template <typename Edges>
boost::optional<std::size_t> find_shell(const Targets &targets,
                                        const Edges &edges,
                                        std::size_t edgeindex,
                                        std::size_t holeindex,
                                        double maxedgewidth)
{
  // Pick the center of the edge chosen for the hole as the
  // start point of the vertical sweep. We choose the middle since
  // the polygons are guaranteed not to touch there, and hence a strict
  // ordering is guaranteed.

  const double x = (edges[edgeindex].x1() + edges[edgeindex].x2()) / 2;
  const double y = (edges[edgeindex].y1() + edges[edgeindex].y2()) / 2;

  // TODO: We actually know for a fact that due to lexicographic sorting of the
  // edges the shell is most likely to be at position edgeindex+1, unless there
  // is another adjacent hole or a shell inside the same shell we're looking for.
  // However, utilizing that guess seems to be quite hard.

  // Look for the last edge which cannot have an intersection.

  std::size_t pos = edgeindex + 1;
  while (pos < edges.size() && edges[pos].x1() - maxedgewidth <= x)
    ++pos;

  // Now we scan backwards until edge.x1 < x-maxwidth, looking for intersections
  // Note that we use x1+width <= x < x2, not twice with <=.

  // The number of intersections for a polygon index
  std::map<std::size_t, std::size_t> counts;

  // The intersection-coordinates
  std::multimap<double, std::size_t> intersections;

  while (pos > 0)
  {
    --pos;

    // Look for an intersection

    const typename Edges::value_type &edge = edges[pos];

    const double y1 = edge.y1();
    const double y2 = edge.y2();
    const double x1 = edge.x1();
    const double x2 = edge.x2();

    // We could also ignore holes below, might not give much of a speedup though.

    if (x1 + maxedgewidth < x)
    {
      // Done if the edges can no longer reach x. Note the positioning of the test
      // to speed up the more likely cases above. Cannot place it below the next
      // "to the left" case though, or the loop may continue until pos=0
      break;
    }
    else if (y1 < y && y2 < y)
    {
      // std::cout << "\tbelow\n";
    }
    else if (x1 >= x && x2 >= x)
    {
      // std::cout << "\tto the right\n";
    }
    else if (x1 < x && x2 < x)
    {
      // std::cout << "\tto the left\n";
    }
    else if (static_cast<std::size_t>(targets[pos]) == holeindex)
    {
      // std::cout << "\titself\n";
    }
    else if (x1 == x2)
    {
      // std::cout << "\tvertical\n";
    }
    else
    {
      // Standard line intersection formula with y = alpha * x + beta.
      // Vertical lines are disallowed above.
      const double alpha = (y2 - y1) / (x2 - x1);
      const double ysect = alpha * (x - x1) + y1;
      if (y < ysect)
      {
        const std::size_t polyline = targets[pos];
        counts[polyline]++;
        intersections.insert(std::make_pair(ysect, polyline));
      }
    }
  }

  // Now select the polygon with the smallest intersection coordinate and an odd number of
  // intersections

  for (std::multimap<double, std::size_t>::const_iterator iter = intersections.begin(),
                                                          end = intersections.end();
       iter != end;
       ++iter)
  {
    std::size_t polyline = iter->second;
    if (counts[polyline] % 2 != 0)
      return polyline;
  }

  return {};
  // throw std::runtime_error("Failed to assign hole to a shell");
}

// ----------------------------------------------------------------------
/*
 * \brief Build polygons or polylines from the given edges
 */
// ----------------------------------------------------------------------

// In fill-mode we build only polygons, otherwise only linestrings.

template <typename Traits, typename Edges>
inline void FmiBuilder::build(const Edges &edges, bool fillmode)
{
  namespace gg = geos::geom;

  typedef Ring<Traits> Polyline;
  typedef typename std::vector<Polyline> Polylines;

  // Objects to be created are closed rings and polylines,
  // but for now we do not separate them so we'll get the
  // indexing right.
  Polylines polylines;

  // Current polygon being created
  long polylineindex = -1;

  // The edge picked first for a new polyline/ring
  long edgeindex = -1;

  // Edge assignments to polygons, nothing assigned yet
  Targets targets(edges.size(), -1);

  // A non-vertical edge from each ring
  EdgeFromRing ringedge;

  // Build the polygons
  while (true)
  {
    // Find next free edge. Done if everything has been processed.
    edgeindex = pick_free_edge(edges, targets, ++edgeindex);
    if (edgeindex < 0)
      break;

    // Start a new polyline from the chosen edge
    const typename Edges::value_type &edge = edges[edgeindex];
    Polyline polyline(edge.x1(), edge.y1(), edge.x2(), edge.y2());
    targets[edgeindex] = ++polylineindex;

    // Keep a record of selected edges since we may have to reindex them
    // when a self-touch occurs.

    std::vector<long> edgeindexes = {edgeindex};

    // Edge index while we jump round the edges finding matches
    long index = edgeindex;

    // We remember the last index to make a better guess on how much to
    // move to the next guess (index-lastindex).
    long lastindex = index;

    // Find continuation, if there is one, using last edge position as hint
    while (true)
    {
      // Find the best match available
      bool self_touch = false;
      bool isoline_extension = false;

      std::size_t tmp = index;
      index = find_first_match(polyline, edges, targets, index, lastindex);
      index = pick_best_match(polylines,
                              polyline,
                              edges,
                              targets,
                              index,
                              polylineindex,
                              &self_touch,
                              &isoline_extension);
      lastindex = tmp;

      // End the polyline if there are no more matches
      if (index < 0)
      {
        ringedge.push_back(0);
        polylines.emplace_back();
        std::swap(polylines.back(), polyline);
        break;
      }

      // Extract ring from possible self-touch

      if (self_touch)
      {
        Polyline newring = polyline.removeSelfTouch();
        if (newring.signedArea() != 0)
        {
          polylines.emplace_back(newring);
          ringedge.push_back(representative_edge(edges, edgeindexes));
          edgeindexes.resize(polyline.size() - 1);  // nedges = nvertices-1
          reindex_edges(targets, edgeindexes, ++polylineindex);
        }
        else
          std::cout << "Warning: Discarding empty cut ring" << std::endl;
      }

      // The best edge found
      const typename Edges::value_type &best = edges[index];

      // Extend old polyline with current one if possible and begin a new one
      if (isoline_extension)
      {
        if (polylines[targets[index]].extendStart(polyline))
        {
          // No need for this, the edge was already assigned
          // edgeindexes.push_back(index);

          // Assign current edges to the old polyline at polylines[index]
          reindex_edges(targets, edgeindexes, targets[index]);

          // Start a new polyline with the same index as before
          --polylineindex;
          break;
        }
        else
        {
          // Now we're touching an old polyline somewhere else beside its start point.
          // Time to stop this polyline.
          // We could also slice the self-touching part from the older polyline,
          // this would make the algorithm always take the same right-turning choise
          // as the isoband algorithm does. Does not seem to be worth the trouble though.
          ringedge.push_back(representative_edge(edges, edgeindexes));
          polylines.emplace_back();
          std::swap(polylines.back(), polyline);
          break;
        }
      }

      if (targets[index] < 0)
      {
        if (!polyline.extendEnd(best.x1(), best.y1(), best.x2(), best.y2()))
          throw std::runtime_error("Internal error while contouring, failed to extend polygon");
        // Mark the found edge used
        targets[index] = polylines.size();
        edgeindexes.push_back(index);
      }
      else
        throw std::runtime_error("Internal error, self touching isoline not handled properly");

      // Terminate the polyline if it became closed
      if (polyline.closed())
      {
        // if (polyline.signedArea() != 0)
        if (true)
        {
          ringedge.push_back(representative_edge(edges, edgeindexes));
          polylines.emplace_back();
          std::swap(polylines.back(), polyline);
        }
        else
        {
          // Discard empty rings - should not happen unless coordinates are degenerate
          std::cout << "Warning: Discarding empty ring created by contouring" << std::endl;
          Polyline emptyline;
          std::swap(emptyline, polyline);
        }
        break;
      }
    }
  }

  // Now we convert everything to GEOS geometry objects.
  // If we're not in fill mode, we can just create the output linestrings now.

  if (polylines.empty())
  {
    itsResult = itsFactory.createEmptyGeometry();
    return;
  }

  if (!fillmode)
  {
    std::vector<gg::LineString *> lines;

    for (std::size_t i = 0; i < polylines.size(); i++)
    {
      const Polyline &polyline = polylines[i];
      std::vector<gg::Coordinate> points;
      points.reserve(points.size());
      for (typename Ring<Traits>::const_iterator it = polyline.begin(); it != polyline.end(); ++it)
        points.emplace_back(gg::Coordinate(it->first, it->second));

      gg::CoordinateSequence *cl = new gg::CoordinateArraySequence();
      cl->setPoints(points);

      gg::LineString *ls = itsFactory.createLineString(cl);

      geos::io::WKTWriter writer;
      lines.push_back(ls);
    }

    if (lines.size() == 1)
    {
      itsResult.reset(lines[0]);
      lines.clear();
    }
    else
    {
      std::vector<gg::Geometry *> *parts = new std::vector<geos::geom::Geometry *>;
      for (std::size_t i = 0; i < lines.size(); i++)
        parts->push_back(lines[i]);
      itsResult.reset(itsFactory.createMultiLineString(parts));
    }
    itsResult->normalize();
    validate(itsResult);

    return;
  }

  // Check that everything is closed

  for (std::size_t i = 0; i < polylines.size(); i++)
    if (!polylines[i].closed())
    {
      std::cerr << "Warning: polyline " << i << "/" << polylines.size() << " is not closed\n";
      std::cout << "POLY " << i << "\t" << polylines[i].signedArea() << "\t"
                << polylines[i].asText(9) << std::endl;
      // throw std::runtime_error("Failed to build a valid multipolygon, linestrings still remain");
    }

  // Find the maximum width of an edge to bound the search for the right shell for each hole.

  double maxedgewidth = find_maximum_edge_width(edges);

  // Find all the shells

  // A mapping from polyline index to shell index
  std::map<std::size_t, std::size_t> shellindexes;

  std::vector<gg::LinearRing *> shells;

  for (std::size_t i = 0; i < polylines.size(); i++)
  {
#if 1
    if (!polylines[i].closed())
      continue;
#endif
    const Polyline &polyline = polylines[i];

    // std::cout << "POLY " << i << "\t" << polyline.signedArea() << "\t" << polyline.asText(20) <<
    // std::endl;

    if (polyline.isClockWise())
    {
      std::vector<gg::Coordinate> points;
      points.reserve(points.size());
      for (typename Ring<Traits>::const_iterator it = polyline.begin(); it != polyline.end(); ++it)
        points.emplace_back(gg::Coordinate(it->first, it->second));

      gg::CoordinateSequence *cl = new gg::CoordinateArraySequence();
      cl->setPoints(points);

      gg::LinearRing *lr = itsFactory.createLinearRing(cl);

      shellindexes[i] = shells.size();
      shells.push_back(lr);
    }
  }

  // Assign holes to shells

  std::map<std::size_t, std::vector<std::size_t> > shellholes;
  std::vector<gg::LinearRing *> holes;

  for (std::size_t i = 0; i < polylines.size(); i++)
  {
#if 1
    if (!polylines[i].closed())
      continue;
#endif
    const Polyline &polyline = polylines[i];

    if (!polyline.isClockWise())
    {
      // Polyline index
      auto idx = find_shell(targets, edges, ringedge[i], i, maxedgewidth);

      if (!idx)
      {
        // This may happen if the grid coordinates are not topologically sound.
        // For example PROJ.4 may produce unexpected/duplicate coordinates for poles in some
        // projections std::cout << "Warning: unassigned hole found\n";
      }
      else
      {
        // std::cout << "HOLE " << i << " HAS SHELL " << idx << std::endl;

        // Append the hole index for the shell
        shellholes[shellindexes[*idx]].push_back(holes.size());

        std::vector<gg::Coordinate> points;
        points.reserve(points.size());
        for (typename Ring<Traits>::const_iterator it = polyline.begin(); it != polyline.end();
             ++it)
          points.emplace_back(gg::Coordinate(it->first, it->second));

        gg::CoordinateSequence *cl = new gg::CoordinateArraySequence();
        cl->setPoints(points);

        gg::LinearRing *lr = itsFactory.createLinearRing(cl);

        holes.push_back(lr);
      }
    }
  }

  // The built polygons

  std::vector<gg::Geometry *> *geom = new std::vector<gg::Geometry *>;

  if (holes.empty())
  {
    for (std::size_t i = 0; i < shells.size(); i++)
      geom->push_back(itsFactory.createPolygon(shells[i], NULL));
  }
  else
  {
    for (std::size_t i = 0; i < shells.size(); i++)
    {
      std::vector<gg::LinearRing *> *holetransfer = NULL;

      const std::vector<std::size_t> &holeindexes = shellholes[i];

      for (std::size_t j = 0; j < holeindexes.size(); j++)
      {
        if (!holetransfer)
          holetransfer = new std::vector<gg::LinearRing *>;
        // std::cout << "Shell " << i << " has hole " << holeindexes[j] << std::endl;
        holetransfer->push_back(holes[holeindexes[j]]);
      }
      auto &&foo = itsFactory.createPolygon(shells[i], holetransfer);
      geom->push_back(foo);
    }
  }

  // Create a MULTIPOLYGON if required
  gg::Geometry *multipolygon = NULL;
  if (geom->size() == 1)
  {
    multipolygon = (*geom)[0];
    delete geom;
  }
  else
    multipolygon = itsFactory.createMultiPolygon(geom);

  itsResult.reset(multipolygon);
  itsResult->normalize();
  validate(itsResult);
}

namespace Builder
{
// ----------------------------------------------------------------------
/*
 * \brief FMI geometry builder for polygons
 */
// ----------------------------------------------------------------------

template <typename Traits, typename Edges>
void fill(const Edges &theEdges, FmiBuilder &theAdapter)
{
  theAdapter.build<Traits>(theEdges, true);
}

// ----------------------------------------------------------------------
/*
 * \brief FMI geometry builder for lines
 */
// ----------------------------------------------------------------------

template <typename Traits, typename Edges>
void line(const Edges &theEdges, FmiBuilder &theAdapter)
{
  theAdapter.build<Traits>(theEdges, false);
}
}  // namespace Builder

}  // namespace Tron
