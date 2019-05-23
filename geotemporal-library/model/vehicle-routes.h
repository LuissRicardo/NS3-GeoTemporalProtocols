/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NAVIGATION_SYSTEM_VEHICLE_ROUTES_H
#define NAVIGATION_SYSTEM_VEHICLE_ROUTES_H

#include <map>
#include <string>
#include <vector>

#include "graph-utils.h"
#include "math-utils.h"

namespace GeoTemporalLibrary
{
namespace NavigationSystem
{

// =============================================================================
//                                   RouteStep
// =============================================================================

/**
 * Represents a single step of a whole route of a mobile node.
 */
class RouteStep
{
private:

  // Time (in seconds) when the node is at the specified street location.
  uint32_t m_time;

  // Position coordinates of a point in the street.
  LibraryUtils::Vector2D m_position_coordinates;

  // Name of the street where the position coordinates are located.
  std::string m_street_name;

  // Distance (in meters) from the position coordinates to the initial junction of the street.
  double m_distance_to_initial_junction;

  // Distance (in meters) from the position coordinates to the ending junction of the street.
  double m_distance_to_ending_junction;

public:

  RouteStep ();

  RouteStep (const uint32_t & time, const LibraryUtils::Vector2D & position_coordinates,
             const std::string & street_name, const double & distance_to_initial_junction,
             const double & distance_to_ending_junction);

  RouteStep (const RouteStep & copy);

  inline uint32_t
  GetTime () const
  {
    return m_time;
  }

  /**
   * Returns the position coordinates of the point on the street where the node is.
   */
  inline const LibraryUtils::Vector2D &
  GetPositionCoordinate () const
  {
    return m_position_coordinates;
  }

  /**
   * Returns the name of the street where the node is.
   */
  inline const std::string &
  GetStreetName () const
  {
    return m_street_name;
  }

  /**
   * Returns the distance (in meters) from the position of the node to the initial junction of
   * the street.
   */
  inline double
  GetDistanceToInitialJunction () const
  {
    return m_distance_to_initial_junction;
  }

  /**
   * Returns the distance (in meters) from the position of the node to the ending junction of
   * the street.
   */
  inline double
  GetDistanceToEndingJunction () const
  {
    return m_distance_to_ending_junction;
  }

  /**
   * Returns the distance to the street junction that the vehicle is closer to.
   */
  inline double
  GetDistanceToCloserJunction () const
  {
    if (m_distance_to_initial_junction <= m_distance_to_ending_junction)
      return m_distance_to_initial_junction;
    return m_distance_to_ending_junction;
  }

  /**
   * Returns the distance to the street junction that the vehicle is farther from.
   */
  inline double
  GetDistanceToFartherJunction () const
  {
    if (m_distance_to_initial_junction > m_distance_to_ending_junction)
      return m_distance_to_initial_junction;
    return m_distance_to_ending_junction;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const RouteStep & lhs, const RouteStep & rhs);
  friend bool operator< (const RouteStep & lhs, const RouteStep & rhs);
};

inline bool
operator== (const RouteStep & lhs, const RouteStep & rhs)
{
  return lhs.m_time == rhs.m_time
          && lhs.m_position_coordinates == rhs.m_position_coordinates
          && lhs.m_street_name == rhs.m_street_name
          && lhs.m_distance_to_initial_junction == rhs.m_distance_to_initial_junction
          && lhs.m_distance_to_ending_junction == rhs.m_distance_to_ending_junction;
}

inline bool
operator!= (const RouteStep & lhs, const RouteStep & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const RouteStep & lhs, const RouteStep & rhs)
{
  if (lhs.m_time != rhs.m_time)
    return lhs.m_time < rhs.m_time;

  if (lhs.m_street_name != rhs.m_street_name)
    return lhs.m_street_name < rhs.m_street_name;

  if (lhs.m_distance_to_initial_junction != rhs.m_distance_to_initial_junction)
    return lhs.m_distance_to_initial_junction < rhs.m_distance_to_initial_junction;

  return lhs.m_position_coordinates < rhs.m_position_coordinates;
}

inline bool
operator> (const RouteStep & lhs, const RouteStep & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const RouteStep & lhs, const RouteStep & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const RouteStep & lhs, const RouteStep & rhs)
{
  return !operator< (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const RouteStep & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                                 NodeRouteData
// =============================================================================

/**
 * Represents the route that a mobile node follows.
 */
class NodeRouteData
{
private:

  // Identifier of the node.
  uint32_t m_node_id;

  // Route that the node follows.
  std::vector<RouteStep> m_node_route;

public:

  NodeRouteData ();

  NodeRouteData (uint32_t node_id);

  NodeRouteData (const NodeRouteData & copy);

  /**
   * Returns the identifier of the node.
   */
  inline uint32_t
  GetNodeId () const
  {
    return m_node_id;
  }

  /**
   * Returns <code>true</code> if the route of the node is empty (i.e. it doesn't have any
   * route steps). On the contrary, if it has at least one route step it returns
   * <code>false</code>.
   */
  inline bool
  EmptyRoute () const
  {
    return m_node_route.empty ();
  }

  /**
   * Returns the time (in seconds) at which the route of the node begins.
   *
   * If the route is empty it throws an <code>std::runtime_error</code> exception.
   */
  uint32_t
  GetRouteInitialTime () const;

  /**
   * Returns the time (in seconds) at which the route of the node ends.
   *
   * If the route is empty it throws an <code>std::runtime_error</code> exception.
   */
  uint32_t
  GetRouteLastTime () const;

  /**
   * Returns the duration (in seconds) of the complete route.
   *
   * If the route is empty it returns 0.
   */
  uint32_t
  GetRouteDuration () const;

  /**
   * Adds a new route step to the route of the node.
   * @param new_route_step New route step to add.
   */
  void
  AddRouteStep (const RouteStep & new_route_step);

  /**
   * Returns a const reference to the route step at the specified time (in seconds).
   *
   * It throws an <code>std::out_of_range</code> if the route is empty or there is not a
   * route step at the specified time.
   * @param time Time (in seconds) of the desired route step.
   */
  const RouteStep &
  GetRouteStep (uint32_t time) const;

  /**
   * Returns a const reference to the vector that contains all the route steps that
   * form the complete route.
   */
  inline const std::vector<RouteStep> &
  GetCompleteRoute () const
  {
    return m_node_route;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const NodeRouteData & lhs, const NodeRouteData & rhs);
};

inline bool
operator== (const NodeRouteData & lhs, const NodeRouteData & rhs)
{
  return lhs.m_node_id == rhs.m_node_id && lhs.m_node_route == rhs.m_node_route;
}

inline bool
operator!= (const NodeRouteData & lhs, const NodeRouteData & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const NodeRouteData & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                                NodesRoutesData
// =============================================================================

/**
 * Contains the route of one or many nodes, each its own distinct route.
 */
class NodesRoutesData
{
private:

  std::map<uint32_t, NodeRouteData> m_nodes_routes;

public:

  NodesRoutesData ();

  NodesRoutesData (const std::string & input_filename);

  NodesRoutesData (const NodesRoutesData & copy);

  /**
   * Returns the number of nodes.
   */
  inline uint32_t
  GetNodesCount () const
  {
    return m_nodes_routes.size ();
  }

  /**
   * Returns <code>true</code> if the object contains a node with the given identifier.
   * Otherwise returns <code>false</code>.
   * @param node_id Identifier of the node.
   */
  bool
  ContainsNode (uint32_t node_id) const;

  /**
   * Adds a new node with an empty route.
   *
   * If the given node doesn't exist yet then it adds it and returns <code>true</code>.
   * On the contrary, if the node already exists it does nothing and returns
   * <code>false</code>.
   * @param node_id Identifier of the node to add.
   */
  bool
  AddNode (uint32_t node_id);

  /**
   * Adds a new route step to the specified node.
   * @param node_id Identifier of the node.
   * @param new_route_step New route step to add to the route of the node.
   */
  void
  AddNodeRouteStep (uint32_t node_id, const RouteStep & new_route_step);

  /**
   * Returns the route data of the desired node.
   * @param node_id Identifier of the node.
   */
  const NodeRouteData &
  GetNodeRouteData (uint32_t node_id) const;

  /**
   * Returns the time (in seconds) at which the route of the specified node begins.
   *
   * If the route is empty it throws an <code>std::runtime_error</code> exception.
   * @param node_id Identifier of the node.
   */
  uint32_t
  GetNodeRouteInitialTime (uint32_t node_id) const;

  /**
   * Returns the time (in seconds) at which the route of the specified node ends.
   *
   * If the route is empty it throws an <code>std::runtime_error</code> exception.
   * @param node_id Identifier of the node.
   */
  uint32_t
  GetNodeRouteLastTime (uint32_t node_id) const;

  /**
   * Returns the duration (in seconds) of the complete route of the specified node.
   *
   * If the route is empty it throws an <code>std::runtime_error</code> exception.
   * @param node_id Identifier of the node.
   */
  uint32_t
  GetNodeRouteDuration (uint32_t node_id) const;

  /**
   * Exports the routes of the nodes to a text file.
   * @param filename Name of the output file.
   */
  void ExportToFile (const std::string & filename) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const NodesRoutesData & lhs, const NodesRoutesData & rhs);
};

inline bool
operator== (const NodesRoutesData & lhs, const NodesRoutesData & rhs)
{
  return lhs.m_nodes_routes == rhs.m_nodes_routes;
}

inline bool
operator!= (const NodesRoutesData & lhs, const NodesRoutesData & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const NodesRoutesData & obj)
{
  obj.Print (os);
  return os;
}

}
}

#endif //NAVIGATION_SYSTEM_VEHICLE_ROUTES_H
