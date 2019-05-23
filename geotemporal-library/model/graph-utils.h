/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_GRAPH_UTILS_H
#define UTILS_GRAPH_UTILS_H

#include <initializer_list>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                 DirectedEdge
// =============================================================================

/** Directed weighted edge.
 *
 * Provides methods that overload the ==, !=, \<, \<=, >, and >= operators. The
 * == and != operators evaluate all member attributes, while the \<, \<=, >, and
 * >= operators only evaluate the m_weight attribute.
 */
class DirectedEdge
{
private:

  // Name of the edge.
  std::string m_name;
  // Starting point of the edge.
  std::string m_from_node;
  // Ending point of the edge.
  std::string m_to_node;
  // Weight of the edge.
  double m_weight;

public:

  DirectedEdge ();

  DirectedEdge (const std::string & from_node, const std::string & to_node,
                const double & weight, const std::string & name);

  DirectedEdge (const DirectedEdge & copy);

  inline std::string
  GetName () const
  {
    return m_name;
  }

  void SetName (const std::string & name);

  inline std::string
  GetFromNode () const
  {
    return m_from_node;
  }

  void SetFromNode (const std::string & from_node);

  inline std::string
  GetToNode () const
  {
    return m_to_node;
  }

  void SetToNode (const std::string & to_node);

  inline double
  GetWeight () const
  {
    return m_weight;
  }

  inline void
  SetWeight (const double & weight)
  {
    m_weight = weight;
  };

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const DirectedEdge & lhs, const DirectedEdge & rhs);
  friend bool operator< (const DirectedEdge & lhs, const DirectedEdge & rhs);
};

inline bool
operator== (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  return lhs.m_name == rhs.m_name && lhs.m_from_node == rhs.m_from_node
          && lhs.m_to_node == rhs.m_to_node && lhs.m_weight == rhs.m_weight;
}

inline bool
operator!= (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  if (lhs.m_weight != rhs.m_weight) return lhs.m_weight < rhs.m_weight;
  if (lhs.m_name != rhs.m_name) return lhs.m_name < rhs.m_name;
  if (lhs.m_from_node != rhs.m_from_node) return lhs.m_from_node < rhs.m_from_node;
  return lhs.m_to_node < rhs.m_to_node;
}

inline bool
operator> (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const DirectedEdge & lhs, const DirectedEdge & rhs)
{
  return !operator< (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const DirectedEdge & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                                   Multigraph
// =============================================================================

/** Edge-weighted graph that allows to have named edges and nodes. */
class Multigraph
{
private:

  typedef std::set<DirectedEdge>::iterator EdgesSetIterator_t;
  typedef std::set<DirectedEdge>::const_iterator EdgesSetConstIterator_t;

  typedef std::map<std::string, std::set<DirectedEdge> >::iterator NeighborsMapIterator_t;
  typedef std::map<std::string, std::set<DirectedEdge> >::const_iterator NeighborsMapConstIterator_t;

  typedef std::map<std::string, std::map<std::string, std::set<DirectedEdge> > >::iterator AdjacencyMapIterator_t;
  typedef std::map<std::string,
  std::map<std::string, std::set<DirectedEdge> > >::const_iterator AdjacencyMapConstIterator_t;

  typedef std::map<std::string, std::pair<std::string, std::string> >::iterator EdgesDirectoryIterator_t;
  typedef std::map<std::string, std::pair<std::string, std::string> >::const_iterator EdgesDirectoryConstIterator_t;

private:

  // The adjacency list of the multi graph.
  std::map<std::string, std::map<std::string, std::set<DirectedEdge> > > m_adjacency_list;

  /** Map to maintain a log of the existing edges in the graph. This will avoid
   * traversing the whole graph to find an edge, at the cost of more memory. It
   * maps [edge_name] to ([from_node], [to_node]). */
  std::map<std::string, std::pair<std::string, std::string> > m_edges_directory;

public:

  Multigraph ();

  Multigraph (const Multigraph & copy);

  /**
   * Creates a <code>Multigraph</code> from the data contained in the given file.
   * @param filename The full path of the graph file to import.
   */
  Multigraph (const std::string & filename);

public:

  /**
   * Verifies if the graph contains a node with the given name.
   * @param node_name Name of the node.
   * @return <code>true</code> if the node exists, <code>false</code> otherwise.
   */
  bool ContainsNode (const std::string & node_name) const;

  /**
   * Verifies if the graph contains an edge with the given name.
   * @param edge_name Name of the edge.
   * @return <code>true</code> if the edge exists, <code>false</code> otherwise.
   */
  bool ContainsEdge (const std::string & edge_name) const;

  /**
   * If the specified edge name exists in the graph, stores the desired
   * <code>DirectedEdge</code> object in the out parameter <code>edge</code> and
   * returns <code>true</code>. If the edge doesn't exist the out parameter
   * <code>edge</code> is not modified at all and returns <code>false</code>.
   *
   * It might throw an <code>runtime_error</code> exception in the case that the
   * edge should exist but does not exist.
   *
   * @param edge_name [IN] Name of the desired edge.
   * @param edge [OUT] Desired edge object.
   * @return <code>true</code> if the desired edge was successfully located and
   * stored in the out parameter, <code>false</code> otherwise.
   */
  bool GetEdge (const std::string & edge_name, DirectedEdge & edge) const;

  /**
   * Returns <code>true</code> if the graph has at least one directed edge
   * starting in <b>from_node</b> and ending in <b>to_node</b>. Otherwise it
   * returns <code>false</code>.
   *
   * Throws:
   *
   * * std::invalid_argument if any of the given node names don't exist in the
   * graph.
   *
   * * std::logic_error : indicates that the <code>from_node</code> should exist
   * but doesn't actually exist. This should never occur.
   *
   * @param from_node Name of the starting node.
   * @param to_node Name of the ending node.
   * @return <code>true</code> if the directed edge exists, <code>false</code>
   * otherwise.
   */
  bool HasEdgeBetweenNodes (const std::string & from_node,
                            const std::string & to_node) const;

  /**
   * Returns the number of nodes in the graph.
   */
  uint32_t GetNodesCount () const;

  /**
   * Returns the number of edges in the graph.
   */
  uint32_t GetEdgesCount () const;

  /**
   * Returns <code>true</code> if the graph doesn't contain any node nor edge.
   * Otherwise it returns <code>false</code>.
   */
  bool Empty () const;

  /**
   * Adds a new node to the graph.
   * @param node_name Name of the new node.
   * @return <code>true</code> if the node was successfully added. <code>false
   * </code> if the node already exists in the graph.
   */
  bool AddNode (const std::string & node_name);

  /**
   * Adds a new node for each item in the given list.
   * @param nodes_names List of nodes.
   */
  void AddNodes (std::initializer_list<std::string> nodes_names);

  /**
   * Deletes the specified node from the graph.
   *
   * This deletes the node from the adjacency list as well as all its outgoing
   * edges. Furthermore, all incoming edges towards this node are deleted as
   * well.
   * @param node_name Name of the node to delete.
   * @return <code>true</code> if the node was successfully deleted. <code>false
   * </code> if the node doesn't exist.
   */
  bool DeleteNode (const std::string & node_name);

  /**
   * Adds the directed weighted-edge <code>directed_edge</code> to this graph.
   *
   * Returns <code>false</code> if:
   * <ul>
   *   <li> Already exists an edge with the given name. </li>
   *   <li> At least one of the origin or destination nodes doesn't exist in the
   *        graph. </li>
   * </ul>
   *
   * @param directed_edge Edge to add.
   * @return <code>true</code> if the edge was successfully added. <code>false
   * </code> if an error occurs.
   */
  bool AddDirectedEdge (const DirectedEdge & directed_edge);

  /**
   * Deletes the edge with the given name, if it exists, and returns <code>true
   * </code>. If the edge doesn't exist then it returns <code>false</code>.
   *
   * @param edge_name [IN] Name of the edge to delete.
   * @return <code>true</code> if the edge is successfully deleted, <code>false
   * </code> otherwise.
   */
  bool DeleteDirectedEdge (const std::string & edge_name);

  /**
   * Deletes the edge with the given name, if it exists, and returns <code>true
   * </code> and stores the deleted edge instance in the <b>out</b> parameter
   * <code>deleted_edge</code>. If the edge doesn't exist then it returns <code>
   * false</code> and the <code>deleted_edge</code> parameter isn't modified.
   *
   * @param edge_name [IN] Name of the edge to delete.
   * @param deleted_edge [OUT] Deleted edge.
   * @return <code>true</code> if the edge is successfully deleted, <code>false
   * </code> otherwise.
   */
  bool
  DeleteDirectedEdge (const std::string & edge_name, DirectedEdge & deleted_edge);

  /**
   * Returns a set with the neighbor nodes of the specified node in the out
   * parameter <code>node_neighbors_set</code>.
   *
   * Throws <code>std::out_of_range</code> if the given node doesn't exist in
   * the graph.
   *
   * @param from_node_name [IN] Name of the node from which to obtain its
   * neighbor nodes.
   * @param node_neighbors_set [OUT] A set with the neighbor nodes of the given
   * node. This out parameter is only modified if the given node exists.
   */
  void
  GetNodeNeighborNodes (const std::string & from_node_name,
                        std::set<std::string> & node_neighbors_set) const;

  /**
   * Returns a set with the outgoing edges from the specified node in the out
   * parameter <code>outgoing_edges_set</code>.
   *
   * Note that <code>outgoing_edges_set</code> might be empty if there's no
   * outgoing edges from the given node.
   *
   * Throws <code>std::out_of_range</code> if the given node doesn't exist in
   * the graph.
   *
   * @param from_node_name [IN] Name of the origin node of the outgoing edges.
   * @param outgoing_edges_set [OUT] A set with the outgoing edges from the
   * specified node. This out parameter is only modified if the function returns
   * <code>true</code> (when the given node exists).
   */
  void
  GetNodeOutgoingEdges (const std::string & from_node_name,
                        std::set<DirectedEdge> & outgoing_edges_set) const;

  /**
   * Returns a set with the outgoing edges from the <code>from_node_name</code>
   * node to the <code>to_node_name</code> node in the out parameter
   * <code>outgoing_edges_set</code>.
   *
   * Note that <code>outgoing_edges_set</code> might be empty if there's no
   * outgoing edges from the <code>from_node_name</code> node to the
   * <code>to_node_name</code>.
   *
   * Throws <code>std::out_of_range</code> if the given node doesn't exist in
   * the graph.
   *
   * @param from_node_name [IN] Name of the origin node of the outgoing edges.
   * @param to_node_name [IN] Name of the destination node of the outgoing edges.
   * @param outgoing_edges_set [OUT] A set with the outgoing edges from the
   * <code>from_node_name</code> to the <code>to_node_name</code>. This out
   * parameter is only modified if the function returns <code>true</code> (when
   * both given nodes exist).
   */
  void
  GetNodeOutgoingEdges (const std::string & from_node_name,
                        const std::string & to_node_name,
                        std::set<DirectedEdge> & outgoing_edges_set) const;

  /**
   * Returns a <b>newly constructed</b> set with the names of all the nodes in
   * the graph. Note that the set might be empty if there's no nodes in the graph.
   *
   * Note that a new <code>set</code> object is created each time this function
   * is called. So this constructed set guarantees that its contained nodes are
   * the ones in the graph at the moment of the function call, i.e., after the
   * call, nodes might have been added or deleted, and the previously constructed
   * set will NOT reflect those changes. To get an updated version of the
   * graph's nodes this function must be called again.
   */
  std::set<std::string> GetAllNodes () const;

  /**
   * Returns a <b>newly constructed</b> set with all the <code>DirectedEdge</code>s
   * in the graph. Note that the set might be empty if there's no edges in the graph.
   *
   * Note that a new <code>set</code> object is created each time this function
   * is called. So this constructed set guarantees that its contained edges are
   * the ones in the graph at the moment of the function call, i.e., after the
   * call, edges might have been added or deleted, and the previously constructed
   * set will NOT reflect those changes. To get an updated version of the
   * graph's edges this function must be called again.
   */
  std::set<DirectedEdge> GetAllEdges () const;

  /**
   * Exports the graph to a file.
   * @param filename Name of the output file.
   */
  void ExportToFile (const std::string & filename) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const Multigraph & lhs, const Multigraph & rhs);
};

inline bool
operator== (const Multigraph & lhs, const Multigraph & rhs)
{
  return lhs.m_adjacency_list == rhs.m_adjacency_list &&
          lhs.m_edges_directory == rhs.m_edges_directory;
}

inline bool
operator!= (const Multigraph & lhs, const Multigraph & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const Multigraph & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                               ShortestPathsTree
// =============================================================================

/**
 * Given a multigraph <code>G</code> and a designated source node <code>s</code>,
 * a shortest-path tree (SPT) is a subgraph from <code>G</code> that contains
 * <code>s</code> and all the nodes reachable from <code>s</code> that form a
 * directed tree rooted at <code>s</code> such that every tree path is a
 * shortest path in the graph, i.e., it gives a shortest path from <code>s</code>
 * to every node reachable from <code>s</code>.
 */
class ShortestPathsTree
{
private:

  /// Multi graph where the Dijkstra's algorithm is computed.
  Multigraph m_graph;

  /// Starting node (root) of the Dijkstra's algorithm.
  std::string m_source_node;

  /**
   * Maps the distance from the source node to each node in the graph:
   * <code>m_distance_to[destination node name] -> distance to destination
   * node</code>.
   *
   * When a node is not reachable from the source node the distance towards this
   * unreachable node is infinity, so it will have the value of the static
   * constant <code>INFINITE_DISTANCE</code>.
   */
  std::map<std::string, double> m_distance_to;

  /**
   * Maps the predecessor of a node for each node in the graph:
   * <code>m_predecessor_of[destination node name] -> name of the predecessor
   * node of the destination node</code>.
   *
   * When a node is not reachable from the source node its predecessor is
   * undefined, so it will have the value of the static constant
   * <code>UNDEFINED_PREDECESSOR</code>.
   */
  std::map<std::string, std::string> m_predecessor_of;

  /**
   * Maps the incoming edge towards a node for each node in the graph:
   * <code>m_edge_to[destination node name] -> name of the incoming edge
   * towards the destination node</code>.
   *
   * When a node is not reachable from the source node its incoming edge is
   * undefined, so it will have the value of the static constant
   * <code>UNDEFINED_EDGE</code>.
   */
  std::map<std::string, std::string> m_edge_to;

  /**
   * Used in the Dijkstra's algorithm computation. Contains the priority and the
   * name of a node, respectively (<code>(priority, node_name)</code>).
   */
  typedef std::pair<double, std::string> NodePriority_t;

public:

  /**
   * This value represents an infinite distance in the Dijkstra's SPT algorithm.
   *
   * Given that there are no negative distances, a negative value is used to
   * represent an infinite distance. In the source file (Multigraph.cc) this
   * constant variable is initialized to -1.0.
   */
  static const double INFINITE_DISTANCE;

  // This value represents an undefined predecessor.
  static const std::string UNDEFINED_PREDECESSOR;

  // This value represents an undefined edge.
  static const std::string UNDEFINED_EDGE;

  ShortestPathsTree ();

  /**
   * Computes the SPT rooted at C{source_node} using Dijkstra's algorithm using
   * a minimum priority queue.
   *
   * If the node <code>source_node</code> doesn't exist in the given graph then
   * it throws an <code>invalid_argument</code> exception.
   *
   * Throws <code>invalid_argument</code> exception if the given
   * <code>source_node</code> node does not exist in the graph.
   *
   * @param graph Graph to compute its shortest-path tree (SPT) to.
   * @param source_node Root of the SPT.
   */
  ShortestPathsTree (const Multigraph & graph, const std::string & source_node);

  ShortestPathsTree (const ShortestPathsTree & copy);

  /**
   * Returns the name of the source (root) node of the Shortest-paths Tree.
   */
  inline std::string
  GetSourceNode () const
  {
    return m_source_node;
  }

  /**
   * Returns the graph to which the shortest-paths tree was calculated from.
   */
  inline Multigraph
  GetGraph () const
  {
    return m_graph;
  }

  /**
   * Returns <code>true</code> if the Shortest Paths Tree instance is empty,
   * i.e., it doesn't contain a graph, a source node, nor a computed
   * shortest-path tree. Otherwise, it returns <code>false</code>.
   */
  inline bool
  Empty () const
  {
    return m_source_node.empty ();
  }

  /**
   * Distance from the source node of the SPT to the <code>destination_node</code>.
   *
   * Returns <code>-1</code> if there is no path from the source node to the
   * destination node.
   *
   * Throws <code>invalid_argument</code> exception if the given
   * <code>destination_node</code> node does not exist in the graph.
   *
   * @param destination_node Name of the destination node.
   * @return Distance from the source node to the destination node.
   */
  double
  GetDistanceToNode (const std::string & destination_node) const;

  /**
   * Indicates if there is a path towards the given <code>destination_node</code>
   * from the source node.
   *
   * Throws <code>invalid_argument</code> exception if the given
   * <code>destination_node</code> node does not exist in the graph.
   *
   * @param destination_node Name of the destination node.
   * @return <code>true</code> if the path exists, <code>false</code> otherwise.
   */
  bool
  HasPathToNode (const std::string & destination_node) const;

  /**
   * If there is a path from the source node to the given destination node returns
   * <code>true</code> and stores the nodes path in the out parameter
   * <code>nodes_path</code>. If there's no path it returns <code>false</code>
   * and the out parameter <code>nodes_path</code> is not modified at all.
   *
   * Throws <code>invalid_argument</code> exception if the given
   * <code>destination_node</code> node does not exist in the graph.
   *
   * Please notice that the out parameter <code>nodes_path</code> might not be
   * modified at all, and this parameter <b>DOES NOT</b> indicates if there is a
   * path towards the specified destination node or not. The returned boolean
   * indicates if there's a path towards the destination node.
   *
   * @param destination_node [IN] Name of the destination node.
   * @param nodes_path [OUT] Path of nodes from the source node to the destination
   * node.
   * @return <code>true</code> if there's a path from the source node to the
   * destination node, <code>false</code> otherwise.
   */
  bool
  GetNodesPathToNode (const std::string & destination_node,
                      std::vector<std::string> & nodes_path) const;

  /**
   * If there is a path from the source node to the given destination node returns
   * <code>true</code> and stores the edges path in the out parameter
   * <code>edges_path</code>. If there's no path it returns <code>false</code>
   * and the out parameter <code>edges_path</code> is not modified at all.
   *
   * Throws <code>invalid_argument</code> exception if the given
   * <code>destination_node</code> node does not exist in the graph.
   *
   * Please notice that the out parameter <code>edges_path</code> might not be
   * modified at all, and this parameter <b>DOES NOT</b> indicates if there is a
   * path towards the specified destination node or not. The returned boolean
   * indicates if there's a path towards the destination node.
   *
   * @param destination_node [IN] Name of the destination node.
   * @param nodes_path [OUT] Path of edges from the source node to the destination
   * node.
   * @return <code>true</code> if there's a path from the source node to the
   * destination node, <code>false</code> otherwise.
   */
  bool
  GetEdgesPathToNode (const std::string & destination_node,
                      std::vector<DirectedEdge> & edges_path) const;

  /**
   * Returns a set with all the nodes in the shortest-paths tree.
   *
   * Note that the smallest valid possible shortest-paths tree will only contain
   * a single node, the source node.
   */
  std::set<std::string> GetAllNodesInTree () const;

  /**
   * Returns a set with all the edges in the shortest-paths tree.
   *
   * If there's no edges in the SPT an empty set will be returned.
   */
  std::set<DirectedEdge> GetAllEdgesInTree () const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const ShortestPathsTree & lhs, const ShortestPathsTree & rhs);
};

inline bool
operator== (const ShortestPathsTree & lhs, const ShortestPathsTree & rhs)
{
  return lhs.m_graph == rhs.m_graph
          && lhs.m_source_node == rhs.m_source_node
          && lhs.m_distance_to == rhs.m_distance_to
          && lhs.m_predecessor_of == rhs.m_predecessor_of
          && lhs.m_edge_to == rhs.m_edge_to;
}

inline bool
operator!= (const ShortestPathsTree & lhs, const ShortestPathsTree & rhs)
{
  return !operator== (lhs, rhs);
}

inline std::ostream &
operator<< (std::ostream & os, const ShortestPathsTree & obj)
{
  obj.Print (os);
  return os;
}

}
}

#endif /* UTILS_GRAPH_UTILS_H */
