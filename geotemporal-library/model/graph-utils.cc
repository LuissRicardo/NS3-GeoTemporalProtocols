/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "graph-utils.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <queue>

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                 DirectedEdge
// =============================================================================

DirectedEdge::DirectedEdge ()
: m_name (), m_from_node (), m_to_node (), m_weight (0.0) { }

DirectedEdge::DirectedEdge (const std::string & from_node, const std::string & to_node,
                            const double & weight, const std::string & name)
: m_name (LibraryUtils::Trim_Copy (name)), m_from_node (LibraryUtils::Trim_Copy (from_node)),
m_to_node (LibraryUtils::Trim_Copy (to_node)), m_weight (weight)
{
  if (name.empty ())
    throw std::invalid_argument ("The parameter 'name' can not be empty.");
  if (from_node.empty ())
    throw std::invalid_argument ("The parameter 'from_node' can not be empty.");
  if (to_node.empty ())
    throw std::invalid_argument ("The parameter 'to_node' can not be empty.");
}

DirectedEdge::DirectedEdge (const DirectedEdge & copy)
: m_name (copy.m_name), m_from_node (copy.m_from_node), m_to_node (copy.m_to_node),
m_weight (copy.m_weight) { }

std::string
DirectedEdge::ToString () const
{
  char buffer[15];
  std::sprintf (buffer, "%4.2f", m_weight);
  return m_from_node + "->" + m_to_node + " " + std::string (buffer)
          + " \"" + m_name + "\"";
}

void
DirectedEdge::SetName (const std::string & name)
{
  m_name = LibraryUtils::Trim_Copy (name);
}

void
DirectedEdge::SetFromNode (const std::string & from_node)
{
  m_from_node = LibraryUtils::Trim_Copy (from_node);
}

void
DirectedEdge::SetToNode (const std::string & to_node)
{
  m_to_node = LibraryUtils::Trim_Copy (to_node);
}

void
DirectedEdge::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                                   Multigraph
// =============================================================================

Multigraph::Multigraph ()
: m_adjacency_list (), m_edges_directory () { }

Multigraph::Multigraph (const Multigraph & copy)
: m_adjacency_list (copy.m_adjacency_list),
m_edges_directory (copy.m_edges_directory) { }

Multigraph::Multigraph (const std::string & filename)
: Multigraph ()
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  std::ifstream graph_file (filename_trimmed, std::ios::in);
  std::string text_line;

  if (!graph_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Importing graph from file \"" << filename_trimmed << "\"...";

  // First part: Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Second part: Expected 2 integers separated by a comma.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ())
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  std::vector<std::string> tokens = LibraryUtils::Split (text_line, ',');

  if (tokens.size () != 2u)
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  LibraryUtils::Trim (tokens.at (0u));
  const uint32_t nodes_count = (uint32_t) std::stoi (tokens.at (0u));
  LibraryUtils::Trim (tokens.at (1u));
  const uint32_t edges_count = (uint32_t) std::stoi (tokens.at (1u));

  // Third part: Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || !text_line.empty ())
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Fourth part: Expected comment.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Fifth part: Expected 'nodes_count' number of nodes.
  for (uint32_t i = 0; i < nodes_count; ++i)
    {
      if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ())
        {
          graph_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      if (!AddNode (text_line))
        {
          graph_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. Invalid (duplicated) node name found.");
        }
    }

  if (nodes_count != GetNodesCount ())
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format (invalid "
                                "number of nodes).");
    }

  // Sixth part: Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || !text_line.empty ())
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Seventh part: Expected comment.
  if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      graph_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Eighth part: Expected 'edges_count' number of edges.
  for (uint32_t i = 0; i < edges_count; ++i)
    {
      if (!LibraryUtils::GetInputStreamNextLine (graph_file, text_line) || text_line.empty ())
        {
          graph_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 4u)
        {
          graph_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (std::vector<std::string>::iterator it = tokens.begin (); it != tokens.end (); ++it)
        LibraryUtils::Trim (*it);

      try
        {
          if (!AddDirectedEdge (DirectedEdge (tokens.at (0u), tokens.at (1u),
                                              std::stod (tokens.at (3u)), tokens.at (2u))))
            throw std::runtime_error ("Corrupt edge data.");
        }
      catch (const std::exception & ex)
        {
          graph_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }
    }

  graph_file.close ();

  if (edges_count != GetEdgesCount ())
    {
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format (invalid "
                                "number of edges).");
    }

  std::cout << " Done.\n";
}

bool
Multigraph::ContainsNode (const std::string & node_name) const
{
  const std::string node_name_trimmed = LibraryUtils::Trim_Copy (node_name);
  // If the node_name string is empty return false.
  if (node_name_trimmed.empty ()) return false;
  return m_adjacency_list.count (node_name_trimmed) == 1;
}

bool
Multigraph::ContainsEdge (const std::string & edge_name) const
{
  const std::string edge_name_trimmed = LibraryUtils::Trim_Copy (edge_name);
  // If the edge_name string is empty return false.
  if (edge_name_trimmed.empty ()) return false;
  return m_edges_directory.count (edge_name_trimmed) == 1;
}

uint32_t
Multigraph::GetNodesCount () const
{
  return m_adjacency_list.size ();
}

uint32_t
Multigraph::GetEdgesCount () const
{
  return m_edges_directory.size ();
}

bool
Multigraph::Empty () const
{
  return GetNodesCount () == 0u && GetEdgesCount () == 0u;
}

bool
Multigraph::AddNode (const std::string & node_name)
{
  const std::string node_name_trimmed = LibraryUtils::Trim_Copy (node_name);

  // If the node_name string is empty return false.
  if (node_name_trimmed.empty ()) return false;

  // Verify that the node doesn't exist. If it does exist, return false.
  if (ContainsNode (node_name_trimmed)) return false;

  m_adjacency_list.insert (std::make_pair (node_name_trimmed,
                                           std::map<std::string, std::set<DirectedEdge> > ()));
  return true;
}

void
Multigraph::AddNodes (std::initializer_list<std::string> nodes_names)
{
  for (std::initializer_list<std::string>::const_iterator node_name_it = nodes_names.begin ();
          node_name_it != nodes_names.end (); ++node_name_it)
    {
      AddNode (*node_name_it);
    }
}

bool
Multigraph::DeleteNode (const std::string & node_name)
{
  const std::string node_name_trimmed = LibraryUtils::Trim_Copy (node_name);

  // If the node_name string is empty return false.
  if (node_name_trimmed.empty ()) return false;

  AdjacencyMapIterator_t node_adj_list_it = m_adjacency_list.find (node_name_trimmed);

  // If the node doesn't exist return false.
  if (node_adj_list_it == m_adjacency_list.end ()) return false;

  // It exist, delete from the adjacency list.
  // First, the node and its outgoing edges. But before doing this, delete the
  // outgoing edges from the edges directory.
  for (NeighborsMapIterator_t neighbor_edges_list_it = node_adj_list_it->second.begin ();
          neighbor_edges_list_it != node_adj_list_it->second.end (); ++neighbor_edges_list_it)
    {
      for (EdgesSetConstIterator_t outgoing_edge_it = neighbor_edges_list_it->second.begin ();
              outgoing_edge_it != neighbor_edges_list_it->second.end (); ++outgoing_edge_it)
        {
          m_edges_directory.erase (outgoing_edge_it->GetName ());
        }
    }

  // Actual deletion of the node from the adjacency list.
  m_adjacency_list.erase (node_adj_list_it);

  // Second, delete any incoming edge towards the node.
  // FROM -> (TO -> [LIST])
  for (AdjacencyMapIterator_t node_neighborhood_it = m_adjacency_list.begin ();
          node_neighborhood_it != m_adjacency_list.end (); ++node_neighborhood_it)
    {
      // TO -> [LIST]
      for (NeighborsMapIterator_t neighbor_edges_list_it = node_neighborhood_it->second.begin ();
              neighbor_edges_list_it != node_neighborhood_it->second.end ();
              /* Increment is handled inside the loop, for deletion purposes. */)
        {
          if (node_name_trimmed == neighbor_edges_list_it->first)
            {
              // i_x in [LIST]
              for (EdgesSetConstIterator_t outgoing_edge_it = neighbor_edges_list_it->second.begin ();
                      outgoing_edge_it != neighbor_edges_list_it->second.end (); ++outgoing_edge_it)
                {
                  m_edges_directory.erase (outgoing_edge_it->GetName ());
                }

              // Delete it. Post increment used to keep a valid iterator.
              node_neighborhood_it->second.erase (neighbor_edges_list_it++);
            }
          else
            {
              ++neighbor_edges_list_it;
            }
        }
    }

  return true;
}

bool
Multigraph::AddDirectedEdge (const DirectedEdge & directed_edge)
{
  EdgesDirectoryConstIterator_t edge_directory_entry_it = m_edges_directory.find (directed_edge.GetName ());

  // If the edge already exists return false
  if (edge_directory_entry_it != m_edges_directory.end ()) return false;

  const std::string from_node_name = directed_edge.GetFromNode ();
  const std::string to_node_name = directed_edge.GetToNode ();

  // If any of the nodes doesn't exist return false
  if (m_adjacency_list.count (from_node_name) == 0
      || m_adjacency_list.count (to_node_name) == 0)
    {
      return false;
    }

  // Add the edge to adjacency list. There are two cases:
  // (i)  If it is the first edge between FROM and TO nodes.
  // (ii) If there is at least one other edge between FROM and TO nodes. So
  //      we're adding a new parallel edge.
  AdjacencyMapIterator_t from_node_adj_it = m_adjacency_list.find (from_node_name);
  NeighborsMapIterator_t neighbors_edges_list_it = from_node_adj_it->second.find (to_node_name);
  if (neighbors_edges_list_it == from_node_adj_it->second.end ())
    {
      // Case (i) Single edge. The first edge.
      std::set<DirectedEdge> edges_list = {directed_edge};
      from_node_adj_it->second.insert (std::make_pair (to_node_name, edges_list));
    }
  else
    {
      // Case (ii) Parallel edges. At least one edge already exist.
      neighbors_edges_list_it->second.insert (directed_edge);
    }

  // Create the edge in the edges directory.
  m_edges_directory.insert (std::make_pair (directed_edge.GetName (),
                                            std::make_pair (from_node_name, to_node_name)));
  return true;
}

bool
Multigraph::DeleteDirectedEdge (const std::string & edge_name)
{
  DirectedEdge deleted_edge;
  return DeleteDirectedEdge (edge_name, deleted_edge);
}

bool
Multigraph::DeleteDirectedEdge (const std::string & edge_name, DirectedEdge & deleted_edge)
{
  const std::string edge_name_trimmed = LibraryUtils::Trim_Copy (edge_name);

  // Check that the node exists
  EdgesDirectoryIterator_t edge_location_it = m_edges_directory.find (edge_name_trimmed);
  if (edge_location_it == m_edges_directory.end ()) return false; // Doesn't exist

  // Use edge directory to know the location of the edge.
  const std::string & from_node_name = edge_location_it->second.first;
  const std::string & to_node_name = edge_location_it->second.second;

  // Get the DirectedEdge instance.
  AdjacencyMapIterator_t node_neighbors_it = m_adjacency_list.find (from_node_name);
  NeighborsMapIterator_t neighbor_edges_it = node_neighbors_it->second.find (to_node_name);
  EdgesSetIterator_t edge_it;
  bool edge_found_flag = false;

  for (edge_it = neighbor_edges_it->second.begin ();
          edge_it != neighbor_edges_it->second.end (); ++edge_it)
    {
      if (edge_it->GetName () == edge_name_trimmed)
        {
          deleted_edge = *edge_it;
          edge_found_flag = true;
          break;
        }
    }

  // An edge must have been found. If it wasn't return false
  if (!edge_found_flag) return false;

  // Delete the edge from the adjacency list.
  // - Remember!: If the edge is the only node between the two nodes then delete
  //   the TO NODE from the map of neighbor nodes of FROM NODE.
  if (neighbor_edges_it->second.size () == 1)
    {
      node_neighbors_it->second.erase (neighbor_edges_it);
    }
  else
    {
      neighbor_edges_it->second.erase (edge_it);
    }

  // Delete the edge from the edges directory
  m_edges_directory.erase (edge_location_it);

  // The specified edge was successfully deleted, return true.
  return true;
}

bool
Multigraph::GetEdge (const std::string & edge_name, DirectedEdge & edge) const
{
  // If the edge doesn't exist return false.
  if (!ContainsEdge (edge_name)) return false;

  const std::string edge_name_trimmed = LibraryUtils::Trim_Copy (edge_name);

  // Use edge directory to know the location of the edge.
  EdgesDirectoryConstIterator_t edge_location_it = m_edges_directory.find (edge_name_trimmed);
  const std::string & from_node_name = edge_location_it->second.first;
  const std::string & to_node_name = edge_location_it->second.second;

  // Get the DirectedEdge instance.
  AdjacencyMapConstIterator_t node_neighbors_it = m_adjacency_list.find (from_node_name);
  NeighborsMapConstIterator_t neighbor_edges_it = node_neighbors_it->second.find (to_node_name);
  EdgesSetConstIterator_t edge_it;

  for (edge_it = neighbor_edges_it->second.begin ();
          edge_it != neighbor_edges_it->second.end (); ++edge_it)
    {
      if (edge_it->GetName () == edge_name_trimmed)
        {
          edge = *edge_it;
          return true;
        }
    }

  throw std::runtime_error ("The specified 'edge_name' (" + edge_name_trimmed + ") should "
                            + "exist in the graph but is missing.");
}

bool
Multigraph::HasEdgeBetweenNodes (const std::string & from_node,
                                 const std::string & to_node) const
{
  // Check that both nodes exist in the graph
  if (!ContainsNode (from_node))
    throw std::out_of_range ("The specified 'from_node' \"" + from_node + "\" doesn't exist.");
  if (!ContainsNode (to_node))
    throw std::out_of_range ("The specified 'to_node' \"" + to_node + "\" doesn't exist.");

  const std::string from_node_trimmed = LibraryUtils::Trim_Copy (from_node);
  const std::string to_node_trimmed = LibraryUtils::Trim_Copy (to_node);

  // Both nodes exist, proceed to search.
  AdjacencyMapConstIterator_t adj_iterator = m_adjacency_list.find (from_node_trimmed);

  if (adj_iterator == m_adjacency_list.end ())
    {
      // Both nodes should exist, but the 'from_node' is unexpectedly missing.
      // If this ever occurs then indicates a bad handling of m_adjacency_list.
      throw std::logic_error ("'From node' \"" + from_node_trimmed + "\" should exist, but "
                              + "it's missing. This indicates a bad handling of "
                              + "'m_adjacency_list'.");
    }

  return adj_iterator->second.count (to_node_trimmed) == 1;
}

void
Multigraph::GetNodeNeighborNodes (const std::string & from_node_name,
                                  std::set<std::string> & node_neighbors_set) const
{
  // Check that the node exist in the graph
  if (!ContainsNode (from_node_name))
    throw std::out_of_range ("The specified 'from_node_name' \"" + from_node_name + "\" doesn't exist.");

  node_neighbors_set.clear ();

  AdjacencyMapConstIterator_t node_neighbors_it =
          m_adjacency_list.find (LibraryUtils::Trim_Copy (from_node_name));

  for (NeighborsMapConstIterator_t neighbor_edges_it = node_neighbors_it->second.begin ();
          neighbor_edges_it != node_neighbors_it->second.end (); ++neighbor_edges_it)
    {
      node_neighbors_set.insert (neighbor_edges_it->first);
    }
}

void
Multigraph::GetNodeOutgoingEdges (const std::string & from_node_name,
                                  std::set<DirectedEdge> & outgoing_edges_set) const
{
  // If the given node doesn't exist throw exception
  if (!ContainsNode (from_node_name))
    throw std::out_of_range ("The specified 'from_node_name' \"" + from_node_name + "\" doesn't exist.");

  outgoing_edges_set.clear ();

  AdjacencyMapConstIterator_t node_neighbors_it =
          m_adjacency_list.find (LibraryUtils::Trim_Copy (from_node_name));

  for (NeighborsMapConstIterator_t neighbor_edges_it = node_neighbors_it->second.begin ();
          neighbor_edges_it != node_neighbors_it->second.end (); ++neighbor_edges_it)
    {
      for (EdgesSetConstIterator_t edge_it = neighbor_edges_it->second.begin ();
              edge_it != neighbor_edges_it->second.end (); ++edge_it)
        {
          outgoing_edges_set.insert (*edge_it);
        }
    }
}

void
Multigraph::GetNodeOutgoingEdges (const std::string & from_node_name,
                                  const std::string & to_node_name,
                                  std::set<DirectedEdge> & outgoing_edges_set) const
{
  // If any of given nodes do not exist throw exception
  if (!ContainsNode (from_node_name))
    throw std::out_of_range ("The specified 'from_node_name' \"" + from_node_name + "\" doesn't exist.");
  if (!ContainsNode (to_node_name))
    throw std::out_of_range ("The specified 'to_node_name' \"" + to_node_name + "\" doesn't exist.");

  outgoing_edges_set.clear ();

  AdjacencyMapConstIterator_t node_neighbors_it =
          m_adjacency_list.find (LibraryUtils::Trim_Copy (from_node_name));
  NeighborsMapConstIterator_t neighbor_edges_it =
          node_neighbors_it->second.find (LibraryUtils::Trim_Copy (to_node_name));

  if (neighbor_edges_it != node_neighbors_it->second.end ())
    {
      for (EdgesSetConstIterator_t edge_it = neighbor_edges_it->second.begin ();
              edge_it != neighbor_edges_it->second.end (); ++edge_it)
        {
          outgoing_edges_set.insert (*edge_it);
        }
    }
}

std::set<std::string>
Multigraph::GetAllNodes () const
{
  std::set<std::string> nodes_set;

  for (AdjacencyMapConstIterator_t node_neighbors_it = m_adjacency_list.begin ();
          node_neighbors_it != m_adjacency_list.end (); ++node_neighbors_it)
    {
      nodes_set.insert (node_neighbors_it->first);
    }

  return nodes_set;
}

std::set<DirectedEdge>
Multigraph::GetAllEdges () const
{
  std::set<DirectedEdge> edges_set;

  for (AdjacencyMapConstIterator_t node_neighbors_it = m_adjacency_list.begin ();
          node_neighbors_it != m_adjacency_list.end (); ++node_neighbors_it)
    {
      for (NeighborsMapConstIterator_t neighbor_edges_it = node_neighbors_it->second.begin ();
              neighbor_edges_it != node_neighbors_it->second.end (); ++neighbor_edges_it)
        {
          for (EdgesSetConstIterator_t edge_it = neighbor_edges_it->second.begin ();
                  edge_it != neighbor_edges_it->second.end (); ++edge_it)
            {
              edges_set.insert (*edge_it);
            }
        }
    }

  return edges_set;
}

void
Multigraph::ExportToFile (const std::string & filename) const
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();

  if (filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream graph_file (filename_trimmed, std::ios::out);

  if (!graph_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Exporting graph to file \"" << filename_trimmed << "\"...";

  graph_file << "# Nodes_Number, Edges_Number" << end_line;
  graph_file << GetNodesCount () << ", " << GetEdgesCount () << end_line << end_line;

  graph_file << "# Node_Name" << end_line;

  const std::set<std::string> nodes_set = GetAllNodes ();
  for (std::set<std::string>::const_iterator node_it = nodes_set.begin ();
          node_it != nodes_set.end (); ++node_it)
    {
      graph_file << *node_it << end_line;
    }

  graph_file << end_line;
  graph_file << "# From_Node, To_Node, Name, Weight" << end_line;

  const std::set<DirectedEdge> edges_set = GetAllEdges ();
  char buffer[25];

  for (std::set<DirectedEdge>::const_iterator edge_it = edges_set.begin ();
          edge_it != edges_set.end (); ++edge_it)
    {
      std::sprintf (buffer, "%.6f", edge_it->GetWeight ());
      graph_file << edge_it->GetFromNode () << ", " << edge_it->GetToNode ()
              << ", " << edge_it->GetName () << ", " << buffer << end_line;
    }

  graph_file.close ();
  std::cout << " Done." << end_line;
}

std::string
Multigraph::ToString () const
{
  const uint32_t nodes_count = GetNodesCount ();
  const uint32_t edges_count = GetEdgesCount ();

  char buffer[15];

  std::sprintf (buffer, "%u", nodes_count);
  std::string to_string = "Edge-weighted multigraph with " + std::string (buffer);

  std::sprintf (buffer, "%u", edges_count);
  to_string += " node(s) & " + std::string (buffer) + " directed edge(s)"
          + (nodes_count > 0 ? ":\n" : ".");

  const std::set<std::string> nodes_set = GetAllNodes ();
  AdjacencyMapConstIterator_t node_neighbors_it;
  bool node_has_neighbors_flag;

  // Used later to decide if to print separator characters (comma and new line).
  uint32_t current_node_index = 0, current_neighbor_index, current_edge_index;
  unsigned long current_neighbors_count, current_edges_count;

  for (std::set<std::string>::const_iterator node_it = nodes_set.begin ();
          node_it != nodes_set.end (); ++node_it)
    {
      to_string += "   [ " + *node_it + " ]: ";
      node_neighbors_it = m_adjacency_list.find (*node_it);
      node_has_neighbors_flag = false;
      current_neighbors_count = node_neighbors_it->second.size ();
      current_neighbor_index = 0;

      for (NeighborsMapConstIterator_t neighbor_edges_it = node_neighbors_it->second.begin ();
              neighbor_edges_it != node_neighbors_it->second.end (); ++neighbor_edges_it)
        {
          current_edges_count = neighbor_edges_it->second.size ();
          current_edge_index = 0;

          // If the edges vector of the neighbor node is emtpy continue to the
          // next neighbor.
          if (current_edges_count == 0) continue;

          for (EdgesSetConstIterator_t edge_it = neighbor_edges_it->second.begin ();
                  edge_it != neighbor_edges_it->second.end (); ++edge_it)
            {
              node_has_neighbors_flag = true;
              to_string += edge_it->ToString ();
              if (current_edge_index++ < current_edges_count - 1) to_string += ", ";
            }

          if (current_neighbor_index++ < current_neighbors_count - 1) to_string += ", ";
        }

      if (!node_has_neighbors_flag) to_string += "(None)";
      if (current_node_index++ < nodes_count - 1) to_string += "\n";
    }

  return to_string;
}

void
Multigraph::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                               ShortestPathsTree
// =============================================================================

const double ShortestPathsTree::INFINITE_DISTANCE = std::numeric_limits<double>::max ();
const std::string ShortestPathsTree::UNDEFINED_PREDECESSOR = "";
const std::string ShortestPathsTree::UNDEFINED_EDGE = "";

ShortestPathsTree::ShortestPathsTree ()
: m_graph (), m_source_node (), m_distance_to (), m_predecessor_of (), m_edge_to () { }

ShortestPathsTree::ShortestPathsTree (const Multigraph & graph, const std::string & source_node)
: m_graph (graph), m_source_node (LibraryUtils::Trim_Copy (source_node)),
m_distance_to (), m_predecessor_of (), m_edge_to ()
{
  if (!m_graph.ContainsNode (m_source_node))
    throw std::invalid_argument ("The specified 'source_node' (" + m_source_node
                                 + ") must exist in the graph");

  // Dijkstra's algorithm from: https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm#Using_a_priority_queue

  // Initialization
  m_distance_to[m_source_node] = 0.0; // Set the distance from the source node to the source node.
  std::priority_queue<NodePriority_t, std::vector<NodePriority_t>,
          std::greater < NodePriority_t>> nodes_priority_queue;

  std::set<std::string> nodes_set = m_graph.GetAllNodes ();

  for (std::set<std::string>::const_iterator node_it = nodes_set.begin ();
          node_it != nodes_set.end (); ++node_it)
    {
      if (*node_it != m_source_node)
        {
          // Set unknown distance from source node.
          m_distance_to[*node_it] = INFINITE_DISTANCE;
        }

      // Set unknown predecessor of the current node.
      m_predecessor_of[*node_it] = UNDEFINED_PREDECESSOR;
      // Set unknown edge to the current node.
      m_edge_to[*node_it] = UNDEFINED_EDGE;

      // Add the node with its current distance as priority.
      nodes_priority_queue.push (std::make_pair (m_distance_to[*node_it], *node_it));
    }

  // We use the nodes_set to keep track of the not-visited nodes, to avoid repetitive computation.
  std::set<std::string> & not_visited_nodes_set = nodes_set; // Using reference as alias for readability.
  std::set<std::string>::const_iterator not_visited_node_it;

  std::string current_node_name, neighbor_node_name;
  double candidate_distance;
  std::set<DirectedEdge> node_outgoing_edges;

  while (!nodes_priority_queue.empty ())
    {
      current_node_name = nodes_priority_queue.top ().second; // Get best node.
      nodes_priority_queue.pop (); // Remove the best node from the priority queue.

      not_visited_node_it = not_visited_nodes_set.find (current_node_name);

      // If the iterator points to the end, then the node is not in the not_visited_nodes_set.
      // This means that the node was visited already, ignore it.
      if (not_visited_node_it == not_visited_nodes_set.end ()) continue;

      // Mark node as visited (delete it from the not_visited_nodes_set).
      not_visited_nodes_set.erase (not_visited_node_it);

      // The node's not been visited yet, explore all its outgoing edges.
      m_graph.GetNodeOutgoingEdges (current_node_name, node_outgoing_edges);

      for (std::set<DirectedEdge>::const_iterator out_edge_it = node_outgoing_edges.begin ();
              out_edge_it != node_outgoing_edges.end (); ++out_edge_it)
        {
          neighbor_node_name = out_edge_it->GetToNode ();
          candidate_distance = m_distance_to[current_node_name] + out_edge_it->GetWeight ();

          if (candidate_distance < m_distance_to[neighbor_node_name])
            {
              m_distance_to[neighbor_node_name] = candidate_distance;
              m_predecessor_of[neighbor_node_name] = current_node_name;
              m_edge_to[neighbor_node_name] = out_edge_it->GetName ();
              // Insert the best new distance priority into the priority queue.
              nodes_priority_queue.push (std::make_pair (candidate_distance, neighbor_node_name));
            }
        }
    }
}

ShortestPathsTree::ShortestPathsTree (const ShortestPathsTree & copy)
: m_graph (copy.m_graph), m_source_node (copy.m_source_node),
m_distance_to (copy.m_distance_to), m_predecessor_of (copy.m_predecessor_of),
m_edge_to (copy.m_edge_to) { }

double
ShortestPathsTree::GetDistanceToNode (const std::string & destination_node) const
{
  if (!m_graph.ContainsNode (destination_node))
    throw std::invalid_argument ("The specified 'destination_node' (" + destination_node
                                 + ") must exist in the graph");

  std::map<std::string, double>::const_iterator distance_it =
          m_distance_to.find (LibraryUtils::Trim_Copy (destination_node));
  if (distance_it == m_distance_to.end ()) return INFINITE_DISTANCE; // Shouldn't occur.
  return distance_it->second;
}

bool
ShortestPathsTree::HasPathToNode (const std::string & destination_node) const
{
  if (!m_graph.ContainsNode (destination_node))
    throw std::invalid_argument ("The specified 'destination_node' (" + destination_node
                                 + ") must exist in the graph");

  std::map<std::string, double>::const_iterator distance_it =
          m_distance_to.find (LibraryUtils::Trim_Copy (destination_node));
  if (distance_it == m_distance_to.end ()) return false; // Shouldn't occur.
  return distance_it->second != INFINITE_DISTANCE;
}

bool
ShortestPathsTree::GetNodesPathToNode (const std::string & destination_node,
                                       std::vector<std::string> & nodes_path) const
{
  if (!m_graph.ContainsNode (destination_node))
    throw std::invalid_argument ("The specified 'destination_node' (" + destination_node
                                 + ") must exist in the graph");

  // If there's no path to the destination node return false.
  if (!HasPathToNode (destination_node)) return false;

  nodes_path.clear ();

  // To compute the path do a backtracking starting at the destination node and
  // going backwards till the source node.
  std::string current_node = LibraryUtils::Trim_Copy (destination_node);
  std::map<std::string, std::string>::const_iterator predecessor_it
          = m_predecessor_of.find (current_node);

  while (predecessor_it->second != UNDEFINED_PREDECESSOR)
    {
      nodes_path.insert (nodes_path.begin (), current_node);
      current_node = predecessor_it->second;
      predecessor_it = m_predecessor_of.find (current_node);
    }

  nodes_path.insert (nodes_path.begin (), current_node);
  return true;
}

bool
ShortestPathsTree::GetEdgesPathToNode (const std::string & destination_node,
                                       std::vector<DirectedEdge> & edges_path) const
{
  if (!m_graph.ContainsNode (destination_node))
    throw std::invalid_argument ("The specified 'destination_node' (" + destination_node
                                 + ") must exist in the graph");

  // If there's no path to the destination node return false.
  if (!HasPathToNode (destination_node)) return false;

  edges_path.clear ();

  // To compute the path do a backtracking starting at the destination node and
  // going backwards till the source node.
  std::map<std::string, std::string>::const_iterator current_edge_it
          = m_edge_to.find (LibraryUtils::Trim_Copy (destination_node));
  DirectedEdge current_edge;

  while (current_edge_it->second != UNDEFINED_EDGE)
    {
      m_graph.GetEdge (current_edge_it->second, current_edge);
      edges_path.insert (edges_path.begin (), current_edge);

      current_edge_it = m_edge_to.find (current_edge.GetFromNode ());
    }

  return true;
}

std::set<std::string>
ShortestPathsTree::GetAllNodesInTree () const
{
  std::set<std::string> nodes_set;
  nodes_set.insert (m_source_node);

  for (std::map<std::string, double>::const_iterator node_distance_it = m_distance_to.begin ();
          node_distance_it != m_distance_to.end (); ++node_distance_it)
    {
      if (node_distance_it->second != INFINITE_DISTANCE)
        {
          nodes_set.insert (node_distance_it->first);
        }
    }

  return nodes_set;
}

std::set<DirectedEdge>
ShortestPathsTree::GetAllEdgesInTree () const
{
  std::set<DirectedEdge> edges_set;
  DirectedEdge edge;

  for (std::map<std::string, std::string>::const_iterator node_edge_it = m_edge_to.begin ();
          node_edge_it != m_edge_to.end (); ++node_edge_it)
    {
      if (node_edge_it->second != UNDEFINED_EDGE)
        {
          m_graph.GetEdge (node_edge_it->second, edge);
          edges_set.insert (edge);
        }
    }
  return edges_set;
}

std::string
ShortestPathsTree::ToString () const
{
  std::string to_string = "Shortest-paths tree rooted at node " + m_source_node
          + ":\n";

  std::set<std::string> nodes_set = m_graph.GetAllNodes ();

  // Given that the elements in a set are ordered in ascendant order, to make
  // the source node the first node in the list we remove it from the set,
  // transform the set in a vector, and insert the source node at the beginning
  // of the vector.
  nodes_set.erase (m_source_node);
  std::vector<std::string> nodes_vector (nodes_set.begin (), nodes_set.end ());
  nodes_vector.insert (nodes_vector.begin (), m_source_node);

  std::vector<DirectedEdge> edges_path;
  char buffer[25];

  const unsigned long nodes_count = nodes_vector.size ();
  uint32_t node_counter = 0;
  double distance_to_node;

  for (std::vector<std::string>::const_iterator destination_node_it = nodes_vector.begin ();
          destination_node_it != nodes_vector.end (); ++destination_node_it)
    {
      to_string += "   " + m_source_node + " to " + *destination_node_it + " (";

      distance_to_node = m_distance_to.find (*destination_node_it)->second;
      if (distance_to_node != INFINITE_DISTANCE)
        {
          std::sprintf (buffer, "%04.2f", distance_to_node);
          to_string += std::string (buffer) + "): ";
        }
      else
        {
          to_string += "INF): ";
        }

      if (*destination_node_it == m_source_node)
        {
          to_string += "(Already there)";
        }
      else
        {
          // If there's a path from source node to destination node print it
          if (GetEdgesPathToNode (*destination_node_it, edges_path))
            {
              // There's a path, iterate through it and add it to string
              const unsigned long edges_count = edges_path.size ();
              uint32_t edge_counter = 0;
              for (std::vector<DirectedEdge>::const_iterator edge_it = edges_path.begin ();
                      edge_it != edges_path.end (); ++edge_it)
                {
                  to_string += edge_it->ToString ();
                  if (edge_counter++ < edges_count - 1) to_string += ", ";
                }
            }
          else
            {
              // If there is not a path
              to_string += "(None)";
            }
        }

      if (node_counter++ < nodes_count - 1) to_string += "\n";
    }

  return to_string;
}

void
ShortestPathsTree::Print (std::ostream & os) const
{
  os << ToString ();
}

}
}

