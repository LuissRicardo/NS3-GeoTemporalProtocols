/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef NAVIGATION_SYSTEM_GPS_SYSTEM_H
#define NAVIGATION_SYSTEM_GPS_SYSTEM_H

#include <map>
#include <set>
#include <string>

#include "vehicle-routes.h"
#include "geotemporal-utils.h"
#include "graph-utils.h"
#include "math-utils.h"

#include <ns3/simple-ref-count.h>

namespace GeoTemporalLibrary
{
namespace NavigationSystem
{

class GpsSystem;

// =============================================================================
//                                 StreetJunction
// =============================================================================

/**
 * Street junction data.
 *
 * Provides methods that overload the ==, !=, &#60;, &#60;=, &#62;, and &#62;=
 * operators.
 */
class StreetJunction
{
private:

  // Name of the street junction.
  std::string m_name;
  // Location coordinate of the street junction.
  LibraryUtils::Vector2D m_location;

public:

  StreetJunction ();

  StreetJunction (const std::string & name, const LibraryUtils::Vector2D & location);

  StreetJunction (const StreetJunction & copy);

  inline const std::string &
  GetName () const
  {
    return m_name;
  }

  inline const LibraryUtils::Vector2D &
  GetLocation () const
  {
    return m_location;
  }

  /**
   * Imports the street junctions contained in the given text file.
   * @param filename Name of the input file.
   * @return A <code>map</code> that contains all the street junctions in the file.
   */
  static std::map<std::string, StreetJunction>
  ImportStreetJunctionsFile (const std::string & filename);

  /**
   * Exports a <code>map</code> of <code>StreetJunction</code>s to a text file.
   * @param filename Name of the output file.
   * @param street_junctions_map Map with the street junctions to export.
   */
  static void
  ExportStreetJunctionsFile (const std::string & filename,
                             const std::map<std::string, StreetJunction> & street_junctions_map);

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const StreetJunction & lhs, const StreetJunction & rhs);
  friend bool operator< (const StreetJunction & lhs, const StreetJunction & rhs);
};

inline bool
operator== (const StreetJunction & lhs, const StreetJunction & rhs)
{
  return lhs.m_name == rhs.m_name && lhs.m_location == rhs.m_location;
}

inline bool
operator!= (const StreetJunction & lhs, const StreetJunction & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const StreetJunction & lhs, const StreetJunction & rhs)
{
  if (lhs.m_name != rhs.m_name) return lhs.m_name < rhs.m_name;
  return lhs.m_location < rhs.m_location;
}

inline bool
operator> (const StreetJunction & lhs, const StreetJunction & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const StreetJunction & lhs, const StreetJunction & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const StreetJunction & lhs, const StreetJunction & rhs)
{
  return !operator< (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const StreetJunction & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                  VisitorNode
// =============================================================================

/**
 * Represents a visitor node with its arrival time.
 */
class VisitorNode
{
private:

  /**
   * Identifier of the visitor node.
   */
  uint32_t m_node_id;

  /**
   * Time (in seconds) at which the node arrived to the area.
   */
  uint32_t m_arrival_time;

public:

  VisitorNode ();

  VisitorNode (uint32_t node_id, uint32_t arrival_time);

  VisitorNode (const VisitorNode & copy);

  /**
   * Returns the identifier of the visitor node.
   */
  inline uint32_t
  GetNodeId () const
  {
    return m_node_id;
  }

  /**
   * Returns the time (in seconds) at which the node arrived to the area.
   */
  inline uint32_t
  GetArrivalTime () const
  {
    return m_arrival_time;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const VisitorNode & lhs, const VisitorNode & rhs);
  friend bool operator< (const VisitorNode & lhs, const VisitorNode & rhs);
};

inline bool
operator== (const VisitorNode & lhs, const VisitorNode & rhs)
{
  return lhs.m_node_id == rhs.m_node_id && lhs.m_arrival_time == rhs.m_arrival_time;
}

inline bool
operator!= (const VisitorNode & lhs, const VisitorNode & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const VisitorNode & lhs, const VisitorNode & rhs)
{
  if (lhs.m_node_id != rhs.m_node_id) return lhs.m_node_id < rhs.m_node_id;

  return lhs.m_arrival_time < rhs.m_arrival_time;
}

inline bool
operator> (const VisitorNode & lhs, const VisitorNode & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const VisitorNode & lhs, const VisitorNode & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const VisitorNode & lhs, const VisitorNode & rhs)
{
  return !operator< (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const VisitorNode & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                          GeoTemporalAreasVisitorNodes
// =============================================================================

/**
 * Contains the list of identifiers of the nodes that visit each geo-temporal area.
 */
class GeoTemporalAreasVisitorNodes : public ns3::SimpleRefCount<GeoTemporalAreasVisitorNodes>
{
private:

  std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> > m_geo_temporal_areas_visitors;

public:

  GeoTemporalAreasVisitorNodes ();

  GeoTemporalAreasVisitorNodes (const std::string & input_filename);

  GeoTemporalAreasVisitorNodes (const GeoTemporalAreasVisitorNodes & copy);

  /**
   * Adds a new geo-temporal area, if the geo-temporal area already exist then it
   * does nothing.
   *
   * @param geo_temporal_area Geo-temporal area to add.
   * @return Returns <code>true</code> if the geo-temporal area was added, otherwise it returns
   * <code>false</code>.
   */
  bool
  AddGeoTemporalArea (const LibraryUtils::GeoTemporalArea & geo_temporal_area);

  /**
   * Adds the visitor node to the specified geo-temporal area.
   *
   * If the area doesn't exist it adds it and then it adds the visitor node to the
   * geo-temporal area.
   *
   * @param geo_temporal_area Visited geo-temporal area.
   * @param visitor_node Visitor node.
   */
  void
  AddVisitorNode (const LibraryUtils::GeoTemporalArea & geo_temporal_area, const VisitorNode & visitor_node);

  /**
   * Returns <code>true</code> if the object contains the given geo-temporal area.
   * Otherwise returns <code>false</code>.
   */
  bool
  ContainsGeoTemporalArea (const LibraryUtils::GeoTemporalArea & geo_temporal_area) const;

  /**
   * Returns a const reference to the set of nodes that visited the specified
   * geo-temporal area.
   * @param geo_temporal_area Visited geo-temporal area.
   */
  const std::set<VisitorNode> &
  GetGeoTemporalAreaVisitorNodes (const LibraryUtils::GeoTemporalArea & geo_temporal_area) const;

  /**
   * Returns <code>true</code> if a node with the given ID visited the specified
   * geo-temporal area. Returns <code>false</code> otherwise.
   * @param node_id ID of the visitor node.
   * @param geo_temporal_area Visited geo-temporal area.
   */
  bool
  NodeVisitedGeoTemporalArea (uint32_t node_id, const LibraryUtils::GeoTemporalArea & geo_temporal_area) const;

  /**
   * Returns a const reference to the <code>VisitorNode</code> object of the node if it visited the
   * specified geo-temporal area. Otherwise it throws a <code>std::out_of_range</code> exception.
   * @param node_id ID of the visitor node.
   * @param geo_temporal_area Visited geo-temporal area.
   */
  const VisitorNode &
  GetVisitorNode (uint32_t node_id, const LibraryUtils::GeoTemporalArea & geo_temporal_area) const;

  /**
   * Exports the list of sets to a file.
   * @param filename Name of the output file.
   */
  void ExportToFile (const std::string & filename) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool
  operator== (const GeoTemporalAreasVisitorNodes & lhs, const GeoTemporalAreasVisitorNodes & rhs);
};

inline bool
operator== (const GeoTemporalAreasVisitorNodes & lhs, const GeoTemporalAreasVisitorNodes & rhs)
{
  return lhs.m_geo_temporal_areas_visitors == rhs.m_geo_temporal_areas_visitors;
}

inline bool
operator!= (const GeoTemporalAreasVisitorNodes & lhs, const GeoTemporalAreasVisitorNodes & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const GeoTemporalAreasVisitorNodes & obj)
{
  obj.Print (os);
  return os;
}



// =============================================================================
//                              SuperNodeStreetGraph
// =============================================================================

class SuperNodeStreetGraph
{
public:

  /**
   * ID of the super-node node.
   */
  static const std::string SUPER_NODE_ID;

private:

  /**
   * The streets graph that contains the super node.
   */
  LibraryUtils::Multigraph m_super_node_graph;

  /**
   * The shortest-paths tree of the super node graph using the super node as
   * root.
   */
  LibraryUtils::ShortestPathsTree m_super_node_shortest_paths;

  /**
   * The set of all nodes (the ones inside the original destination area and the
   * needed extra ones) that form the super node.
   */
  std::set<std::string> m_super_node_nodes_set;

  /**
   * The original destination area.
   */
  LibraryUtils::Area m_original_destination_area;

  /**
   * The modified coordinates of the destination area. These coordinates may be
   * the same as the original area.
   */
  LibraryUtils::Area m_modified_destination_area;

public:

  SuperNodeStreetGraph ();

  /**
   * Computes the super-node streets graph of the given destination area.
   * @param destination_area
   * @param gps
   */
  SuperNodeStreetGraph (const LibraryUtils::Area & destination_area, const GpsSystem & gps);

  SuperNodeStreetGraph (const SuperNodeStreetGraph & copy);

  /**
   * Returns a <b>const</b> reference to the graph with the super node.
   */
  inline const LibraryUtils::Multigraph &
  GetSuperNodeGraph () const
  {
    return m_super_node_graph;
  }

  /**
   * Returns a <b>const</b> reference to the shortest paths tree of the super
   * node graph with the super node as root.
   */
  inline const LibraryUtils::ShortestPathsTree &
  GetSuperNodeShortestPaths () const
  {
    return m_super_node_shortest_paths;
  }

  /**
   * Returns a <b>const</b> reference to set of nodes that compose the super node.
   */
  inline const std::set<std::string> &
  GetSuperNodeNodesSet () const
  {
    return m_super_node_nodes_set;
  }

  /**
   * Returns a <b>const</b> reference to the original destination area.
   */
  inline const LibraryUtils::Area &
  GetOriginalDestinationArea () const
  {
    return m_original_destination_area;
  }

  /**
   * Returns a <b>const</b> reference to the modified destination area.
   */
  inline const LibraryUtils::Area &
  GetModifiedDestinationArea () const
  {
    return m_modified_destination_area;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const SuperNodeStreetGraph & lhs, const SuperNodeStreetGraph & rhs);
};

inline bool
operator== (const SuperNodeStreetGraph & lhs, const SuperNodeStreetGraph & rhs)
{
  return lhs.m_super_node_graph == rhs.m_super_node_graph
          && lhs.m_super_node_shortest_paths == rhs.m_super_node_shortest_paths
          && lhs.m_super_node_nodes_set == rhs.m_super_node_nodes_set
          && lhs.m_original_destination_area == rhs.m_original_destination_area
          && lhs.m_modified_destination_area == rhs.m_modified_destination_area;
}

inline bool
operator!= (const SuperNodeStreetGraph & lhs, const SuperNodeStreetGraph & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const SuperNodeStreetGraph & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                   GpsSystem
// =============================================================================

/**
 * Represents a GPS system.
 */
class GpsSystem : public ns3::SimpleRefCount<GpsSystem>
{
private:

  /**
   * Graph that represents the streets topology.
   */
  LibraryUtils::Multigraph m_streets_graph;

  /**
   * Contains the exact location in the streets topology of each vehicle during
   * the simulation.
   */
  NodesRoutesData m_vehicles_routes_data;

  /**
   * Map of street junctions data. Each junction name maps to its StreetJunction
   * instance.
   */
  std::map<std::string, StreetJunction> m_street_junctions_data;

  /**
   * Cache of computed super nodes and super node graphs. The destination area
   * maps to the computed SuperNodeStreetGraph.
   */
  std::map<LibraryUtils::Area, SuperNodeStreetGraph> m_super_node_graphs_cache;

public:

  GpsSystem ();

  /**
   * Initializes a GPS system instance with the streets graph and junctions data
   * contained in the text files.
   *
   * @param street_graph_filename Name of the text file that contains the
   * information of the graph that represents the streets topology.
   * @param vehicles_routes_filename Name of the text file that contains the
   * routes of the vehicles.
   * @param street_junctions_data_filename Name of the text file that contains
   * the streets junctions data.
   */
  GpsSystem (const std::string & street_graph_filename,
             const std::string & vehicles_routes_filename,
             const std::string & street_junctions_data_filename);

  GpsSystem (const GpsSystem & copy);

  /**
   * Returns a <b>constant reference</b> to the graph that represents the streets map.
   */
  inline const LibraryUtils::Multigraph &
  GetStreetsGraph () const
  {
    return m_streets_graph;
  }

  /**
   * Returns a <b>constant reference</b> to the data of the routes of the vehicles.
   */
  inline const NodesRoutesData &
  GetVehiclesRoutesData () const
  {
    return m_vehicles_routes_data;
  }

  /**
   * Returns a <b>constant reference</b> to the map that contains all the street
   * junctions data.
   */
  inline const std::map<std::string, StreetJunction> &
  GetAllStreetJunctionsData () const
  {
    return m_street_junctions_data;
  }

  /**
   * Returns a <b>constant reference</b> to the <code>StreetJunction</code> object
   * that contains the location information of the junction with the given name.
   * @param junction_name Name of the desired junction.
   */
  const StreetJunction &
  GetStreetJunctionData (const std::string & junction_name) const;

  /**
   * Returns the name of the street junction that is closer in the given route step.
   */
  std::string
  GetCloserJunctionName (const RouteStep & route_step) const;

  /**
   * Returns the name of the street junction that is farther in the given route step.
   */
  std::string
  GetFartherJunctionName (const RouteStep & route_step) const;

  /**
   * Computes and returns a <code>SuperNodeStreetGraph</code> object.
   *
   * Each <code>SuperNodeStreetGraph</code> object is computed only once, and
   * stored in a cache. When the same destination area is given a second time
   * the <code>SuperNodeStreetGraph</code> object is retrieved and returned
   * from the cache.
   *
   * The returned <code>SuperNodeStreetGraph</code> object is returned as a
   * <b>constant reference</b> to the object stored in the cache.
   *
   * @param destination_area Destination area to process its super node.
   * @return A <b>constant reference</b> to the <code>SuperNodeStreetGraph</code>
   * object in the cache.
   */
  const SuperNodeStreetGraph &
  GetSuperNodeStreetGraph (const LibraryUtils::Area & destination_area);

  /**
   * Calculates the distance from given vehicle location to the destination area.
   *
   * @param vehicle_location Location of the vehicle in a <code>RouteStep</code> object.
   * @param destination_area Destination area to measure the distance to.
   *
   * @return Distance from the given vehicle location to the destination area.
   */
  double
  CalculateDistanceToArea (const RouteStep & vehicle_location,
                           const LibraryUtils::Area & destination_area);

  /**
   * Computes if the given vehicle is getting closer to the area at the specified
   * time.
   *
   * To compute it, it uses the previous vehicle's location and compares it
   * to its current location. If the vehicle doesn't has a previous location at
   * the given time it returns <code>false</code> (because then we can't know if
   * it's getting closer).
   *
   * @param vehicle_id Identifier of the vehicle to evaluate.
   * @param destination_area Destination area.
   * @param current_time Current simulation time.
   *
   * @return <code>true</code> if the vehicle is getting closer to the area,
   * <code>false</code> otherwise.
   */
  bool
  VehicleGettingCloserToArea (const uint32_t vehicle_id,
                              const LibraryUtils::Area & destination_area,
                              const uint32_t current_time);

  /**
   * Computes if the given vehicle is going away from the area at the specified
   * time.
   *
   * To compute it, it uses the previous vehicle's location and compares it
   * to its current location. If the vehicle doesn't has a previous location at
   * the given time it returns <code>false</code> (because then we can't know if
   * it's going away).
   *
   * @param vehicle_id Identifier of the vehicle to evaluate.
   * @param destination_area Destination area.
   * @param current_time Current simulation time.
   *
   * @return <code>true</code> if the vehicle is going away from the area,
   * <code>false</code> otherwise.
   */
  bool
  VehicleGoingAwayFromArea (const uint32_t vehicle_id,
                            const LibraryUtils::Area & destination_area,
                            const uint32_t current_time);

  /**
   * Computes if a given vehicle to be evaluated is closer to the specified destination
   * area than a base vehicle to be compared with.
   *
   * It returns the distance difference between base vehicle and evaluated vehicle, that
   * is <code>base_vehicle_distance_to_area - evaluated_vehicle_distance_to_area</code>,
   * in the output parameter <code>distance_difference</code>. If difference is:
   * <ul>
   *  <li>Positive: evaluated vehicle is closer & base vehicle is farther away.</li>
   *  <li>Zero: At equal distance from the area.</li>
   *  <li>Negative: evaluated vehicle is farther away & base vehicle is closer.</li>
   * </ul>
   *
   * @param evaluated_vehicle_id [IN] ID of the vehicle being evaluated.
   * @param base_vehicle_id [IN] ID of the vehicle used as comparison.
   * @param destination_area [IN] Destination area.
   * @param current_time [IN] Current simulation time.
   * @param distance_difference [OUT] Distance difference between base vehicle and
   * evaluated vehicle.
   *
   * @return <code>true</code> if evaluated vehicle is closer (i.e. at LESS distance from
   * the destination area) than the base vehicle or both vehicles at equal distance from
   * area. Otherwise, returns <code>false</code>.
   */
  bool
  IsVehicleCloserToArea (const uint32_t evaluated_vehicle_id,
                         const uint32_t base_vehicle_id,
                         const LibraryUtils::Area & destination_area,
                         const uint32_t current_time,
                         double & distance_difference);

  /**
   * Computes if a candidate vehicle is a valid packet carrier comparing its current
   * position to the position of the current packet carrier vehicle.
   *
   * @param candidate_vehicle_id [IN] Vehicle being evaluated as candidate carrier.
   * @param current_carrier_vehicle_id [IN] Current packet carrier.
   * @param destination_area [IN] Packet's destination area.
   * @param current_time [IN] Current simulation time.
   * @param minimum_valid_distance_difference [IN] Minimum valid difference between
   * candidate and current carrier vehicles.
   *
   * @return <code>true</code> if the candidate vehicle is a valid packet carrier,
   * <code>false</code> otherwise.
   */
  bool
  IsVehicleValidPacketCarrier (const uint32_t candidate_vehicle_id,
                               const uint32_t current_carrier_vehicle_id,
                               const LibraryUtils::Area & destination_area,
                               const uint32_t current_time,
                               const double & minimum_valid_distance_difference);

  friend bool operator== (const GpsSystem & lhs, const GpsSystem & rhs);
};

inline bool
operator== (const GpsSystem & lhs, const GpsSystem & rhs)
{
  return lhs.m_streets_graph == rhs.m_streets_graph
          && lhs.m_vehicles_routes_data == rhs.m_vehicles_routes_data
          && lhs.m_street_junctions_data == rhs.m_street_junctions_data
          && lhs.m_super_node_graphs_cache == rhs.m_super_node_graphs_cache;
}

inline bool
operator!= (const GpsSystem & lhs, const GpsSystem & rhs)
{
  return !operator== (lhs, rhs);
}

}
}

#endif //NAVIGATION_SYSTEM_GPS_SYSTEM_H
