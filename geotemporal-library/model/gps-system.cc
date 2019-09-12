/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "gps-system.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
#include <utility>
#include <vector>

#include "string-utils.h"
#include "packet-utils.h"

namespace GeoTemporalLibrary
{
namespace NavigationSystem
{

// =============================================================================
//                                 StreetJunction
// =============================================================================

StreetJunction::StreetJunction ()
: m_name (), m_location () { }

StreetJunction::StreetJunction (const std::string & name, const LibraryUtils::Vector2D & location)
: m_name (name), m_location (location) { }

StreetJunction::StreetJunction (const StreetJunction & copy)
: m_name (copy.m_name), m_location (copy.m_location) { }

std::map<std::string, StreetJunction>
StreetJunction::ImportStreetJunctionsFile (const std::string & filename)
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  std::ifstream junctions_file (filename_trimmed, std::ios::in);
  std::string text_line;

  if (!junctions_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Importing street junctions data from file \"" << filename_trimmed << "\"...";

  // First part: Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (junctions_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      junctions_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Second part: Expected 1 integer.
  if (!LibraryUtils::GetInputStreamNextLine (junctions_file, text_line) || text_line.empty ())
    {
      junctions_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  const uint32_t juntions_count = (uint32_t) std::stoi (LibraryUtils::Trim_Copy (text_line));

  // Third part: Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (junctions_file, text_line) || !text_line.empty ())
    {
      junctions_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Fourth part: Expected comment.
  if (!LibraryUtils::GetInputStreamNextLine (junctions_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      junctions_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Fifth part: expected list of comma-separated values: 1 string and 2 doubles
  std::map<std::string, StreetJunction> street_junctions_map;
  StreetJunction street_junction;

  std::vector<std::string> tokens;
  while (LibraryUtils::GetInputStreamNextLine (junctions_file, text_line))
    {
      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 3u)
        {
          junctions_file.close ();
          std::cout << "Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (std::vector<std::string>::iterator it = tokens.begin (); it != tokens.end (); ++it)
        {
          LibraryUtils::Trim (*it);

          if (it->empty ())
            {
              junctions_file.close ();
              std::cout << "Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }
        }

      if (street_junctions_map.count (tokens.at (0u)) == 1u)
        throw std::runtime_error ("Corrupt file. Duplicated street junction name.");

      street_junction = StreetJunction (tokens.at (0u),
                                        LibraryUtils::Vector2D (std::stod (tokens.at (1u)), std::stod (tokens.at (2u))));

      street_junctions_map.insert (std::make_pair (tokens.at (0u), street_junction));
    }

  junctions_file.close ();

  if (juntions_count != street_junctions_map.size ())
    {
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format (invalid "
                                "number of edges).");
    }

  std::cout << " Done.\n";

  return street_junctions_map;
}

void
StreetJunction::ExportStreetJunctionsFile (const std::string & filename,
                                           const std::map<std::string, StreetJunction> & street_junctions_map)
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();

  if (filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream junctions_file (filename_trimmed, std::ios::out);

  if (!junctions_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Exporting street junctions data to file \"" << filename_trimmed << "\"...";

  junctions_file << "# Junctions_Number" << end_line;
  junctions_file << street_junctions_map.size () << end_line << end_line;

  junctions_file << "# Junction_Name, Junction_Coordinate_X, Junction_Coordinate_Y" << end_line;
  char buffer[25];

  for (std::map<std::string, StreetJunction>::const_iterator street_junction_it = street_junctions_map.begin ();
          street_junction_it != street_junctions_map.end (); ++street_junction_it)
    {
      junctions_file << street_junction_it->second.m_name << ", ";

      std::sprintf (buffer, "%.6f", street_junction_it->second.m_location.m_x);
      junctions_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", street_junction_it->second.m_location.m_y);
      junctions_file << buffer << end_line;
    }

  junctions_file.close ();
  std::cout << " Done." << end_line;
}

std::string
StreetJunction::ToString () const
{
  return "Junction '" + m_name + "' located at " + m_location.ToString ();
}

void
StreetJunction::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                                  VisitorNode
// =============================================================================

VisitorNode::VisitorNode ()
: m_node_id (0u), m_arrival_time (0u) { }

VisitorNode::VisitorNode (uint32_t node_id, uint32_t arrival_time)
: m_node_id (node_id), m_arrival_time (arrival_time) { }

VisitorNode::VisitorNode (const VisitorNode & copy)
: m_node_id (copy.m_node_id), m_arrival_time (copy.m_arrival_time) { }

std::string
VisitorNode::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%u", m_node_id);
  std::string str = "Node with ID " + std::string (buffer) + " arrived to the area at ";

  std::sprintf (buffer, "%u", m_arrival_time);
  return str + std::string (buffer) + " seconds.";
}

void
VisitorNode::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                          GeoTemporalAreasVisitorNodes
// =============================================================================

GeoTemporalAreasVisitorNodes::GeoTemporalAreasVisitorNodes ()
: m_geo_temporal_areas_visitors () { }

GeoTemporalAreasVisitorNodes::GeoTemporalAreasVisitorNodes (const std::string & input_filename)
: GeoTemporalAreasVisitorNodes ()
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (input_filename);
  std::ifstream input_file (filename_trimmed, std::ios::in);
  std::string text_line;
  std::vector<std::string> tokens;

  if (!input_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Importing geo-temporal area visitor nodes from file \"" << filename_trimmed << "\"... ";

  // Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // List of areas
  uint32_t area_counter = 0u, area_id;
  std::vector<LibraryUtils::Area> areas_list;

  while (LibraryUtils::GetInputStreamNextLine (input_file, text_line) && !text_line.empty ())
    {
      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 5u)
        {
          input_file.close ();
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (std::vector<std::string>::iterator it = tokens.begin (); it != tokens.end (); ++it)
        {
          LibraryUtils::Trim (*it);

          if (it->empty ())
            {
              input_file.close ();
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }
        }

      area_id = (uint32_t) std::stoi (tokens[0]);

      if (area_id != area_counter)
        {
          input_file.close ();
          throw std::runtime_error ("Corrupt file. The file does not match the correct format. Area identifiers "
                                    "must be sequential.");
        }

      areas_list.emplace_back (std::stod (tokens[1]), std::stod (tokens[2]),
                               std::stod (tokens[3]), std::stod (tokens[4]));
      ++area_counter;
    }

  // Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // List of visitor nodes
  LibraryUtils::GeoTemporalArea geo_temporal_area;
  std::set<VisitorNode> visitor_nodes;
  std::vector<uint32_t> int_tokens;

  while (LibraryUtils::GetInputStreamNextLine (input_file, text_line))
    {
      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () < 3u)
        {
          input_file.close ();
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (std::vector<std::string>::iterator it = tokens.begin (); it != tokens.end (); ++it)
        {
          LibraryUtils::Trim (*it);

          if (it->empty ())
            {
              input_file.close ();
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }
        }

      geo_temporal_area = LibraryUtils::GeoTemporalArea (LibraryUtils::TimePeriod (ns3::Seconds (std::stoi (tokens.at (1u))),
                                                                                   ns3::Seconds (std::stoi (tokens.at (2u)))),
                                                         areas_list.at ((uint32_t) std::stoi (tokens.at (0u))));
      visitor_nodes.clear ();

      if (tokens.size () > 3)
        {
          int_tokens.clear ();

          for (uint32_t i = 3u; i < tokens.size (); ++i)
            int_tokens.push_back ((uint32_t) std::stoi (tokens.at (i)));

          while (!int_tokens.empty ())
            {
              visitor_nodes.insert (VisitorNode (int_tokens.at (0u), int_tokens.at (1u)));
              int_tokens.erase (int_tokens.begin (), int_tokens.begin () + 2);
            }
        }

      m_geo_temporal_areas_visitors.insert (std::make_pair (geo_temporal_area, visitor_nodes));
    }

  input_file.close ();
  std::cout << "Done.\n";
}

GeoTemporalAreasVisitorNodes::GeoTemporalAreasVisitorNodes (const GeoTemporalAreasVisitorNodes & copy)
: m_geo_temporal_areas_visitors (copy.m_geo_temporal_areas_visitors) { }

bool
GeoTemporalAreasVisitorNodes::AddGeoTemporalArea (const LibraryUtils::GeoTemporalArea & geo_temporal_area)
{
  if (m_geo_temporal_areas_visitors.count (geo_temporal_area) == 1u) return false;

  m_geo_temporal_areas_visitors.insert (std::make_pair (geo_temporal_area, std::set<VisitorNode> ()));
  return true;
}

void
GeoTemporalAreasVisitorNodes::AddVisitorNode (const LibraryUtils::GeoTemporalArea & geo_temporal_area,
                                              const VisitorNode & visitor_node)
{
  std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> >::iterator gta_entry_it
          = m_geo_temporal_areas_visitors.find (geo_temporal_area);

  // If there's no entry then add it with a a new set.
  if (gta_entry_it == m_geo_temporal_areas_visitors.end ())
    {
      std::set<VisitorNode> new_set {visitor_node};
      m_geo_temporal_areas_visitors.insert (std::make_pair (geo_temporal_area, new_set));
    }
  else // There's an entry with the specified geo-temporal area
    {
      gta_entry_it->second.insert (visitor_node);
    }
}

bool
GeoTemporalAreasVisitorNodes::ContainsGeoTemporalArea (const LibraryUtils::GeoTemporalArea & geo_temporal_area) const
{
  return m_geo_temporal_areas_visitors.count (geo_temporal_area) == 1u;
}

const std::set<VisitorNode> &
GeoTemporalAreasVisitorNodes::GetGeoTemporalAreaVisitorNodes (const LibraryUtils::GeoTemporalArea & geo_temporal_area) const
{
  std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> >::const_iterator geo_temporal_area_visitors_it =
          m_geo_temporal_areas_visitors.find (geo_temporal_area);

  if (geo_temporal_area_visitors_it == m_geo_temporal_areas_visitors.end ())
    throw std::out_of_range ("Error: the specified geo-temporal area \"" + geo_temporal_area.ToString ()
                             + "\" is not known.");

  return geo_temporal_area_visitors_it->second;
}

bool
GeoTemporalAreasVisitorNodes::NodeVisitedGeoTemporalArea (uint32_t node_id,
                                                          const LibraryUtils::GeoTemporalArea & geo_temporal_area) const
{
  try
    {
      GetVisitorNode (node_id, geo_temporal_area);
      return true;
    }
  catch (std::exception & exception)
    {
      return false;
    }
}

const VisitorNode &
GeoTemporalAreasVisitorNodes::GetVisitorNode (uint32_t node_id,
                                              const LibraryUtils::GeoTemporalArea & geo_temporal_area) const
{
  std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> >::const_iterator geo_temporal_area_visitors_it =
          m_geo_temporal_areas_visitors.find (geo_temporal_area);

  if (geo_temporal_area_visitors_it == m_geo_temporal_areas_visitors.end ())
    throw std::out_of_range ("Error: the specified geo-temporal area \"" + geo_temporal_area.ToString ()
                             + "\" is not known.");

  for (std::set<VisitorNode>::const_iterator visitor_node_it = geo_temporal_area_visitors_it->second.begin ();
          visitor_node_it != geo_temporal_area_visitors_it->second.end (); ++visitor_node_it)
    {
      if (node_id == visitor_node_it->GetNodeId ())
        return *visitor_node_it;
    }

  char buffer[25];
  std::sprintf (buffer, "%u", node_id);
  throw std::out_of_range ("Node " + std::string (buffer) + " didn't visited geo-temporal area "
                           + geo_temporal_area.ToString ());
}

void
GeoTemporalAreasVisitorNodes::ExportToFile (const std::string & filename) const
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();
  char buffer[25];

  if (filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream output_file (filename_trimmed, std::ios::out);

  if (!output_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Exporting geo-temporal area visitor nodes to file \"" << filename_trimmed << "\"... ";

  // List of areas

  // - Process areas
  std::set<LibraryUtils::Area> used_areas_set;

  for (std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> >::const_iterator
    area_visitor_nodes_it = m_geo_temporal_areas_visitors.begin ();
          area_visitor_nodes_it != m_geo_temporal_areas_visitors.end ();
          ++area_visitor_nodes_it)
    {
      used_areas_set.insert (area_visitor_nodes_it->first.GetArea ());
    }

  // - Print areas
  std::map<LibraryUtils::Area, uint32_t> areas_ids_mapping;
  uint32_t area_id = 0u;

  output_file << "# Area ID, Area X1, Area Y1, Area X2, Area Y2" << end_line;

  for (std::set<LibraryUtils::Area>::const_iterator area_it = used_areas_set.begin ();
          area_it != used_areas_set.end (); ++area_it)
    {
      areas_ids_mapping.insert (std::make_pair (*area_it, area_id));
      output_file << area_id << ", ";
      ++area_id;

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate1 ().m_x);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate1 ().m_y);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate2 ().m_x);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate2 ().m_y);
      output_file << buffer << end_line;
    }

  // List of visitor nodes for each geo-temporal area
  output_file << end_line << "# Area ID, Time Start, Time End[, Visitor Node ID, Arrival Time]*" << end_line;

  for (std::map<LibraryUtils::GeoTemporalArea, std::set<VisitorNode> >::const_iterator
    area_visitor_nodes_it = m_geo_temporal_areas_visitors.begin ();
          area_visitor_nodes_it != m_geo_temporal_areas_visitors.end ();
          ++area_visitor_nodes_it)
    {
      output_file << areas_ids_mapping.at (area_visitor_nodes_it->first.GetArea ()) << ", "
              << ((uint32_t) area_visitor_nodes_it->first.GetTimePeriod ().GetStartTime ().GetSeconds ()) << ", "
              << ((uint32_t) area_visitor_nodes_it->first.GetTimePeriod ().GetEndTime ().GetSeconds ());

      if (!area_visitor_nodes_it->second.empty ())
        {
          for (std::set<VisitorNode>::const_iterator visitor_node_it = area_visitor_nodes_it->second.begin ();
                  visitor_node_it != area_visitor_nodes_it->second.end (); ++visitor_node_it)
            {
              output_file << ", " << visitor_node_it->GetNodeId () << ", " << visitor_node_it->GetArrivalTime ();
            }
        }

      output_file << end_line;
    }

  output_file.close ();
  std::cout << "Done.\n";
}

std::string
GeoTemporalAreasVisitorNodes::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%u", (uint32_t) m_geo_temporal_areas_visitors.size ());
  return "List(s) of visitor nodes of " + std::string (buffer) + " geo-temporal area(s).";
}

void
GeoTemporalAreasVisitorNodes::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                              SuperNodeStreetGraph
// =============================================================================

/**
 * ID of the super-node node.
 */
const std::string SuperNodeStreetGraph::SUPER_NODE_ID = "<- Super Node ->";

SuperNodeStreetGraph::SuperNodeStreetGraph ()
: m_super_node_graph (), m_super_node_shortest_paths (), m_super_node_nodes_set (),
m_original_destination_area (), m_modified_destination_area () { }

SuperNodeStreetGraph::SuperNodeStreetGraph (const LibraryUtils::Area & destination_area, const GpsSystem & gps)
: SuperNodeStreetGraph ()
{
  m_original_destination_area = destination_area;

  // 1.- Find all nodes inside the area.
  const std::map<std::string, StreetJunction> & street_junctions_data = gps.GetAllStreetJunctionsData ();
  std::set<std::string> nodes_inside_set;

  for (std::map<std::string, StreetJunction>::const_iterator junction_it = street_junctions_data.begin ();
          junction_it != street_junctions_data.end (); ++junction_it)
    {
      if (destination_area.IsInside (junction_it->second.GetLocation ()))
        nodes_inside_set.insert (junction_it->second.GetName ());
    }

  if (nodes_inside_set.empty ())
    {
      std::cout << " Error!\n";
      throw std::runtime_error ("The given destination area " + destination_area.ToString ()
                                + " must contain at least one (1) street junction inside the destination area.");
    }

  // 2.- Locate the node closest to the area's center. Use it as root to compute shortest-paths
  // Dijkstra's alg. to all other nodes in nodes_inside.
  const LibraryUtils::Vector2D area_center_coord ((destination_area.GetCoordinate1 ().m_x + destination_area.GetCoordinate2 ().m_x) / 2.0,
                                                  (destination_area.GetCoordinate1 ().m_y + destination_area.GetCoordinate2 ().m_y) / 2.0);

  double minimum_distance_found = std::numeric_limits<double>::max ();
  double junction_distance;
  std::string junction_closest_to_area_center;
  std::map<std::string, StreetJunction>::const_iterator junction_data_it;

  for (std::set<std::string>::const_iterator junction_inside_it = nodes_inside_set.begin ();
          junction_inside_it != nodes_inside_set.end (); ++junction_inside_it)
    {
      junction_data_it = street_junctions_data.find (*junction_inside_it);
      junction_distance = area_center_coord.DistanceTo (junction_data_it->second.GetLocation ());

      if (junction_distance < minimum_distance_found)
        {
          minimum_distance_found = junction_distance;
          junction_closest_to_area_center = *junction_inside_it;
        }
    }

  // 3.- Super node will contain all nodes inside destination area and between the shortest paths
  // between root and all nodes inside destination area.
  LibraryUtils::ShortestPathsTree shortest_paths (gps.GetStreetsGraph (), junction_closest_to_area_center);

  m_super_node_nodes_set = std::set<std::string> (nodes_inside_set.begin (), nodes_inside_set.end ());

  std::vector<std::string> spt_nodes_path;

  for (std::set<std::string>::const_iterator junction_inside_it = nodes_inside_set.begin ();
          junction_inside_it != nodes_inside_set.end (); ++junction_inside_it)
    {
      if (*junction_inside_it == junction_closest_to_area_center)
        continue; // Skip the root node

      if (!shortest_paths.GetNodesPathToNode (*junction_inside_it, spt_nodes_path))
        {
          std::cout << " Error!\n";
          throw std::runtime_error ("Path from node " + junction_closest_to_area_center + " to "
                                    + "node " + *junction_inside_it + " not found.");
        }

      m_super_node_nodes_set.insert (spt_nodes_path.begin (), spt_nodes_path.end ());
    }

  // 4.- Calculate new area size
  double x_min, x_max, y_min, y_max;

  if (destination_area.GetCoordinate1 ().m_x <= destination_area.GetCoordinate2 ().m_x)
    {
      x_min = destination_area.GetCoordinate1 ().m_x;
      x_max = destination_area.GetCoordinate2 ().m_x;
    }
  else
    {
      x_min = destination_area.GetCoordinate2 ().m_x;
      x_max = destination_area.GetCoordinate1 ().m_x;
    }

  if (destination_area.GetCoordinate1 ().m_y <= destination_area.GetCoordinate2 ().m_y)
    {
      y_min = destination_area.GetCoordinate1 ().m_y;
      y_max = destination_area.GetCoordinate2 ().m_y;
    }
  else
    {
      y_min = destination_area.GetCoordinate2 ().m_y;
      y_max = destination_area.GetCoordinate1 ().m_y;
    }

  // Compute the set of nodes that are in the super_node_set but are not in the
  // nodes_inside_set. That is, the nodes that aren't inside the area but are
  // needed to have a connected sub-graph.
  std::set<std::string> extra_needed_nodes_set;

  std::set_difference (m_super_node_nodes_set.begin (), m_super_node_nodes_set.end (),
                       nodes_inside_set.begin (), nodes_inside_set.end (),
                       std::inserter (extra_needed_nodes_set, extra_needed_nodes_set.begin ()));

  // If there are extra nodes then the area needs to be modified. Search for the
  // new min and max coordinates.
  if (!extra_needed_nodes_set.empty ())
    {
      for (std::set<std::string>::const_iterator extra_node_it = extra_needed_nodes_set.begin ();
              extra_node_it != extra_needed_nodes_set.end (); ++extra_node_it)
        {
          junction_data_it = street_junctions_data.find (*extra_node_it);
          const LibraryUtils::Vector2D & junction_location = junction_data_it->second.GetLocation ();

          x_min = junction_location.m_x <= x_min ? junction_location.m_x : x_min;
          x_max = junction_location.m_x >= x_max ? junction_location.m_x : x_max;

          y_min = junction_location.m_y <= y_min ? junction_location.m_y : y_min;
          y_max = junction_location.m_y >= y_max ? junction_location.m_y : y_max;
        }
    }

  // If there aren't any extra needed nodes (extra_needed_nodes_set == 0 -> True) then it assigns
  // the same original specified area coordinates, just rearranged into min and max.
  // If there is at least one extra needed node (extra_needed_nodes_set == 0 -> False) then it
  // assigns the computed value.
  m_modified_destination_area = LibraryUtils::Area (x_min, y_min, x_max, y_max);

  // 5.- Compute new street graph with super node
  const std::string super_node_id = "<- Super Node ->";
  m_super_node_graph.AddNode (super_node_id);

  // Add nodes outside the super node area
  const std::set<std::string> graph_nodes = gps.GetStreetsGraph ().GetAllNodes ();

  for (std::set<std::string>::const_iterator node_it = graph_nodes.begin ();
          node_it != graph_nodes.end (); ++node_it)
    {
      if (m_super_node_nodes_set.count (*node_it) == 0u)
        m_super_node_graph.AddNode (*node_it);
    }

  // Add edges that cross the super node area. That is, edges from the area to the area are ignored.
  const std::set<LibraryUtils::DirectedEdge> graph_edges = gps.GetStreetsGraph ().GetAllEdges ();
  std::string from_node, to_node;

  for (std::set<LibraryUtils::DirectedEdge>::const_iterator edge_it = graph_edges.begin ();
          edge_it != graph_edges.end (); ++edge_it)
    {
      from_node = m_super_node_nodes_set.count (edge_it->GetFromNode ()) == 1u ?
              super_node_id : edge_it->GetFromNode ();
      to_node = m_super_node_nodes_set.count (edge_it->GetToNode ()) == 1u ?
              super_node_id : edge_it->GetToNode ();

      // If the edge goes from the super node to the super node then ignore it.
      if (from_node == super_node_id && to_node == super_node_id)
        continue;

      m_super_node_graph.AddDirectedEdge (LibraryUtils::DirectedEdge (from_node, to_node,
                                                                      edge_it->GetWeight (),
                                                                      edge_it->GetName ()));
    }

  // 6.- Compute shortest-paths to the super node graph using the super node as root
  m_super_node_shortest_paths = LibraryUtils::ShortestPathsTree (m_super_node_graph, super_node_id);
}

SuperNodeStreetGraph::SuperNodeStreetGraph (const SuperNodeStreetGraph & copy)
: m_super_node_graph (copy.m_super_node_graph),
m_super_node_shortest_paths (copy.m_super_node_shortest_paths),
m_super_node_nodes_set (copy.m_super_node_nodes_set),
m_original_destination_area (copy.m_original_destination_area),
m_modified_destination_area (copy.m_modified_destination_area) { }

std::string
SuperNodeStreetGraph::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%d", (uint32_t) m_super_node_nodes_set.size ());
  std::string to_string = "Super node formed by " + std::string (buffer) + " node(s) '";

  uint32_t i = 0;
  for (std::set<std::string>::const_iterator node_it = m_super_node_nodes_set.begin ();
          node_it != m_super_node_nodes_set.end (); ++node_it)
    {
      to_string += *node_it;
      if (i++ < m_super_node_nodes_set.size () - 1u)
        to_string += ", ";
    }

  to_string += "' and has destination area " + m_original_destination_area.ToString () + ".";

  return to_string;
}

void
SuperNodeStreetGraph::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                                   GpsSystem
// =============================================================================

GpsSystem::GpsSystem ()
: m_streets_graph (), m_vehicles_routes_data (), m_street_junctions_data (),
m_super_node_graphs_cache (), m_node_ip_to_id () { }

GpsSystem::GpsSystem (const std::string & street_graph_filename,
                      const std::string & vehicles_routes_filename,
                      const std::string & street_junctions_data_filename)
: m_streets_graph (street_graph_filename), m_vehicles_routes_data (vehicles_routes_filename),
m_street_junctions_data (StreetJunction::ImportStreetJunctionsFile (street_junctions_data_filename)),
m_super_node_graphs_cache (), m_node_ip_to_id ()
{
  // If the number of street junctions and nodes in the graph doesn't match throw exception
  if (m_street_junctions_data.size () != m_streets_graph.GetNodesCount ())
    throw std::runtime_error ("The number of street junctions must match with the number of graph's nodes.");
}

GpsSystem::GpsSystem (const GpsSystem & copy)
: m_streets_graph (copy.m_streets_graph), m_vehicles_routes_data (copy.m_vehicles_routes_data),
m_street_junctions_data (copy.m_street_junctions_data),
m_super_node_graphs_cache (copy.m_super_node_graphs_cache),
m_node_ip_to_id (copy.m_node_ip_to_id) { }

const StreetJunction &
GpsSystem::GetStreetJunctionData (const std::string & junction_name) const
{
  std::map<std::string, StreetJunction>::const_iterator junction_it =
          m_street_junctions_data.find (junction_name);

  if (junction_it == m_street_junctions_data.end ())
    throw std::out_of_range ("Error: Junction '" + junction_name + "' NOT found.");

  return junction_it->second;
}

std::string
GpsSystem::GetCloserJunctionName (const RouteStep & route_step) const
{
  LibraryUtils::DirectedEdge street_edge;

  // The street must exist in the streets graph, otherwise throw an exception.
  if (!m_streets_graph.GetEdge (route_step.GetStreetName (), street_edge))
    throw std::runtime_error ("Invalid street: street '" + route_step.GetStreetName ()
                              + "' doesn't exist in the streets graph.");

  if (route_step.GetDistanceToInitialJunction () <= route_step.GetDistanceToEndingJunction ())
    return street_edge.GetFromNode ();

  return street_edge.GetToNode ();
}

std::string
GpsSystem::GetFartherJunctionName (const RouteStep & route_step) const
{
  LibraryUtils::DirectedEdge street_edge;

  // The street must exist in the streets graph, otherwise throw an exception.
  if (!m_streets_graph.GetEdge (route_step.GetStreetName (), street_edge))
    throw std::runtime_error ("Invalid street: street '" + route_step.GetStreetName ()
                              + "' doesn't exist in the streets graph.");

  if (route_step.GetDistanceToInitialJunction () > route_step.GetDistanceToEndingJunction ())
    return street_edge.GetFromNode ();

  return street_edge.GetToNode ();
}

const SuperNodeStreetGraph &
GpsSystem::GetSuperNodeStreetGraph (const LibraryUtils::Area & destination_area)
{
  // std::cout << "Processing destination area " << destination_area << "... ";

  // Check if the given area has already been processed before. If so, retrieve it.
  std::map<LibraryUtils::Area, SuperNodeStreetGraph>::const_iterator super_node_cache_it =
          m_super_node_graphs_cache.find (destination_area);

  if (super_node_cache_it != m_super_node_graphs_cache.end ())
    {
      // std::cout << "Retrieved from cache.\n";
      return super_node_cache_it->second;
    }

  // Not processed before, process it and store result in cache, to avoid repetitive computation.
  std::pair < std::map<LibraryUtils::Area, SuperNodeStreetGraph>::const_iterator, bool> inserted_it;
  std::pair<LibraryUtils::Area, SuperNodeStreetGraph> pair_to_insert;

  pair_to_insert = std::make_pair (destination_area, SuperNodeStreetGraph (destination_area, *this));
  inserted_it = m_super_node_graphs_cache.insert (pair_to_insert);
  // std::cout << "Done.\n";

  // Return the just inserted object into the map to return the right reference.
  return inserted_it.first->second;
}

double
GpsSystem::CalculateDistanceToArea (const RouteStep & vehicle_location,
                                    const LibraryUtils::Area & destination_area)
{
  const SuperNodeStreetGraph & super_node_data = GetSuperNodeStreetGraph (destination_area);

  if (!m_streets_graph.ContainsEdge (vehicle_location.GetStreetName ()))
    throw std::runtime_error ("Error: the streets graph doesn't contain a street named '"
                              + vehicle_location.GetStreetName () + "'.");

  const std::string closer_junction_name = GetCloserJunctionName (vehicle_location);
  const std::string farther_junction_name = GetFartherJunctionName (vehicle_location);

  const double closer_junction_distance = vehicle_location.GetDistanceToCloserJunction ();
  const double farther_junction_distance = vehicle_location.GetDistanceToFartherJunction ();

  const bool closer_junction_in_super_node_flag =
          super_node_data.GetSuperNodeNodesSet ().count (closer_junction_name) > 0u;
  const bool farther_junction_in_super_node_flag =
          super_node_data.GetSuperNodeNodesSet ().count (farther_junction_name) > 0u;

  // 1. Check if the vehicle is inside the super node or in the 1-neighborhood

  // If both street junctions are inside the super node return 0
  if (closer_junction_in_super_node_flag && farther_junction_in_super_node_flag)
    return 0.0; // Vehicle is already inside the super node
  else
    {
      // If only one of the junctions is inside the super node then return the distance towards
      // that junction.
      if (closer_junction_in_super_node_flag)
        return closer_junction_distance;
      else if (farther_junction_in_super_node_flag)
        return farther_junction_distance;
    }

  // 2. Is not inside super node or in the 1-neighborhood, calculate distance to super node
  //    using the 2 sides of the street
  const LibraryUtils::ShortestPathsTree & super_node_spt = super_node_data.GetSuperNodeShortestPaths ();

  const double closer_junction_path_distance = super_node_spt.GetDistanceToNode (closer_junction_name)
          + closer_junction_distance;
  const double farther_junction_path_distance = super_node_spt.GetDistanceToNode (farther_junction_name)
          + farther_junction_distance;

  if (closer_junction_path_distance <= farther_junction_path_distance)
    return closer_junction_path_distance;

  return farther_junction_path_distance;
}

bool
GpsSystem::VehicleGettingCloserToArea (const uint32_t vehicle_id,
                                       const LibraryUtils::Area & destination_area,
                                       const uint32_t current_time)
{
  // If vehicle only active for 1 second or it's its first second active, then
  // it has no previous location, we can't know if it's getting closer or not.
  if (m_vehicles_routes_data.GetNodeRouteDuration (vehicle_id) == 1u
      || current_time == m_vehicles_routes_data.GetNodeRouteInitialTime (vehicle_id))
    {
      return false;
    }

  // Use last location to deduce if moving closer or not.
  const RouteStep & previous_location = m_vehicles_routes_data.GetNodeRouteData (vehicle_id)
          .GetRouteStep (current_time - 1u);
  const double previous_distance_to_area = CalculateDistanceToArea (previous_location, destination_area);

  // Get current location
  const RouteStep & current_location = m_vehicles_routes_data.GetNodeRouteData (vehicle_id)
          .GetRouteStep (current_time);
  const double current_distance_to_area = CalculateDistanceToArea (current_location, destination_area);

  return current_distance_to_area <= previous_distance_to_area;
}

bool
GpsSystem::VehicleGettingCloserToArea (const ns3::Ipv4Address& vehicle_ip,
                                       const LibraryUtils::Area& destination_area,
                                       const uint32_t current_time)
{
  return VehicleGettingCloserToArea (m_node_ip_to_id.at (vehicle_ip),
                                     destination_area,
                                     current_time);
}

bool
GpsSystem::VehicleGoingAwayFromArea (const uint32_t vehicle_id,
                                     const LibraryUtils::Area & destination_area,
                                     const uint32_t current_time)
{
  return !VehicleGettingCloserToArea (vehicle_id, destination_area, current_time);
}

bool
GpsSystem::VehicleGoingAwayFromArea (const ns3::Ipv4Address& vehicle_ip,
                                     const LibraryUtils::Area& destination_area,
                                     const uint32_t current_time)
{
  return !VehicleGettingCloserToArea (vehicle_ip, destination_area, current_time);
}

bool
GpsSystem::IsVehicleCloserToArea (const RouteStep& evaluated_vehicle_location,
                                  const RouteStep& base_vehicle_location,
                                  const LibraryUtils::Area& destination_area,
                                  const uint32_t current_time,
                                  double& distance_difference)
{
  // Calculate distance from evaluated vehicle towards the area.
  const double evaluated_vehicle_dist_to_area =
          CalculateDistanceToArea (evaluated_vehicle_location, destination_area);

  // Calculate distance from the comparison base vehicle towards the area.
  const double base_vehicle_dist_to_area =
          CalculateDistanceToArea (base_vehicle_location, destination_area);

  // Calculate the differences of distances between base vehicle and evaluated vehicle.
  // If difference is:
  //   * Positive: evaluated vehicle is closer & base vehicle is farther away.
  //   * Zero: At equal distance from the area.
  //   * Negative: evaluated vehicle is farther away & base vehicle is closer.
  distance_difference = base_vehicle_dist_to_area - evaluated_vehicle_dist_to_area;

  // Returns true if evaluated vehicle is CLOSER (i.e. at LESS distance from area)
  // or both at equal distance.
  return evaluated_vehicle_dist_to_area <= base_vehicle_dist_to_area;
}

bool
GpsSystem::IsVehicleCloserToArea (const uint32_t evaluated_vehicle_id,
                                  const uint32_t base_vehicle_id,
                                  const LibraryUtils::Area & destination_area,
                                  const uint32_t current_time,
                                  double & distance_difference)
{
  // Get the location of the vehicle being evaluated in the streets graph.
  const RouteStep & evaluated_vehicle_location = m_vehicles_routes_data.GetNodeRouteData (evaluated_vehicle_id)
          .GetRouteStep (current_time);

  // Get the location of the vehicle being used as base for comparison in the streets graph.
  const RouteStep & base_vehicle_location = m_vehicles_routes_data.GetNodeRouteData (base_vehicle_id)
          .GetRouteStep (current_time);

  return IsVehicleCloserToArea (evaluated_vehicle_location,
                                base_vehicle_location,
                                destination_area,
                                current_time,
                                distance_difference);
}

bool
GpsSystem::IsVehicleCloserToArea (const ns3::Ipv4Address& evaluated_vehicle_ip,
                                  const ns3::Ipv4Address& base_vehicle_ip,
                                  const LibraryUtils::Area& destination_area,
                                  const uint32_t current_time,
                                  double& distance_difference)
{
  return IsVehicleCloserToArea (m_node_ip_to_id.at (evaluated_vehicle_ip),
                                m_node_ip_to_id.at (base_vehicle_ip),
                                destination_area,
                                current_time,
                                distance_difference);
}

bool
GpsSystem::IsVehicleValidPacketCarrier (const uint32_t candidate_vehicle_id,
                                        const uint32_t current_carrier_vehicle_id,
                                        const LibraryUtils::Area & destination_area,
                                        const uint32_t current_time,
                                        const double & minimum_valid_distance_difference)
{
  const RouteStep & candidate_vehicle_location =
          m_vehicles_routes_data.GetNodeRouteData (candidate_vehicle_id).GetRouteStep (current_time);
  const RouteStep & carrier_vehicle_location =
          m_vehicles_routes_data.GetNodeRouteData (current_carrier_vehicle_id).GetRouteStep (current_time);

  // If any of the two vehicles is in the area then return true, because both are very
  // close to the area.
  if (destination_area.IsInside (candidate_vehicle_location.GetPositionCoordinate ())
      || destination_area.IsInside (carrier_vehicle_location.GetPositionCoordinate ()))
    return true;

  // Compute if candidate vehicle is closer to the area.
  double distance_difference = 0;
  const bool candidate_closer_to_area =
          IsVehicleCloserToArea (candidate_vehicle_location, carrier_vehicle_location,
                                 destination_area, current_time, distance_difference);

  const bool candidate_getting_closer = VehicleGettingCloserToArea (candidate_vehicle_id,
                                                                    destination_area,
                                                                    current_time);
  const bool candidate_moving_away = !candidate_getting_closer;
  const bool carrier_getting_closer = VehicleGettingCloserToArea (current_carrier_vehicle_id,
                                                                  destination_area,
                                                                  current_time);
  const bool carrier_moving_away = !carrier_getting_closer;

  // If candidate vehicle is closer & current carrier vehicle is farther away.
  if (candidate_closer_to_area)
    {
      // If
      //    distance difference between current carrier & candidate vehicles is less than
      //    the minimum valid, that is, the distance difference is not valid,
      // OR
      //    carrier is getting closer to area and candidate is moving away from area (don't
      //    send packet to a vehicle that's going away from the area)
      // then
      //    candidate is not a valid carrier, return false.
      if (distance_difference < minimum_valid_distance_difference
          || (carrier_getting_closer && candidate_moving_away))
        return false;
    }
  else // else, then candidate is farther away & carrier is closer.
    {
      // Then, we want carrier send the packet to candidate vehicle ONLY if
      // carrier is moving away from the area (i.e. getting the packet away from
      // its destination area) AND candidate is getting closer to the area.
      if (candidate_getting_closer && carrier_moving_away)
        return true;

      // In any other case (of this else-body) we don't want to send the packet.
      return false;
    }

  // Candidate is a valid carrier.
  return true;
}

bool
GpsSystem::IsVehicleValidPacketCarrier (const ns3::Ipv4Address& candidate_vehicle_ip,
                                        const ns3::Ipv4Address& current_carrier_vehicle_ip,
                                        const LibraryUtils::Area& destination_area,
                                        const uint32_t current_time,
                                        const double& minimum_valid_distance_difference)
{
  return IsVehicleValidPacketCarrier (m_node_ip_to_id.at (candidate_vehicle_ip),
                                      m_node_ip_to_id.at (current_carrier_vehicle_ip),
                                      destination_area,
                                      current_time,
                                      minimum_valid_distance_difference);
}

}
}

