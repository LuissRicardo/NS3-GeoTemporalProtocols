/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "vehicle-routes.h"

#include <fstream>
#include <iostream>
#include <set>
#include <utility>

#include "path-utils.h"
#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace NavigationSystem
{

// =============================================================================
//                                   RouteStep
// =============================================================================

RouteStep::RouteStep ()
: m_time (), m_position_coordinates (), m_street_name (), m_distance_to_initial_junction (),
m_distance_to_ending_junction () { }

RouteStep::RouteStep (const uint32_t & time, const LibraryUtils::Vector2D & position_coordinates,
                      const std::string & street_name, const double & distance_to_initial_junction,
                      const double & distance_to_ending_junction)
: m_time (time), m_position_coordinates (position_coordinates), m_street_name (street_name),
m_distance_to_initial_junction (distance_to_initial_junction),
m_distance_to_ending_junction (distance_to_ending_junction) { }

RouteStep::RouteStep (const RouteStep & copy)
: m_time (copy.m_time), m_position_coordinates (copy.m_position_coordinates),
m_street_name (copy.m_street_name),
m_distance_to_initial_junction (copy.m_distance_to_initial_junction),
m_distance_to_ending_junction (copy.m_distance_to_ending_junction) { }

std::string
RouteStep::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%u", m_time);
  return "At second " + std::string (buffer) + " the route is in street "
          + m_street_name + " (at " + m_position_coordinates.ToString () + ").";
}

void
RouteStep::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                                 NodeRouteData
// =============================================================================

NodeRouteData::NodeRouteData ()
: m_node_id (), m_node_route () { }

NodeRouteData::NodeRouteData (uint32_t node_id)
: m_node_id (node_id), m_node_route () { }

NodeRouteData::NodeRouteData (const NodeRouteData & copy)
: m_node_id (copy.m_node_id), m_node_route (copy.m_node_route) { }

uint32_t
NodeRouteData::GetRouteInitialTime () const
{
  // Check if the route is empty
  if (m_node_route.empty ())
    throw std::runtime_error ("Empty route. If the route is empty, then there cannot be an initial "
                              "route step.");

  // Not empty, return the time of the first element.
  return m_node_route.front ().GetTime ();
}

uint32_t
NodeRouteData::GetRouteLastTime () const
{
  // Check if the route is empty
  if (m_node_route.empty ())
    throw std::runtime_error ("Empty route. If the route is empty, then there cannot be a last "
                              "route step.");

  // Not empty, return the time of the last element.
  return m_node_route.back ().GetTime ();
}

uint32_t
NodeRouteData::GetRouteDuration () const
{
  // Check if the route is empty
  if (m_node_route.empty ())
    return 0u;

  return m_node_route.back ().GetTime () - m_node_route.front ().GetTime () + 1u;
}

void
NodeRouteData::AddRouteStep (const RouteStep & new_route_step)
{
  // Check if the route is empty
  if (m_node_route.empty ())
    {
      m_node_route.push_back (new_route_step);
      return;
    }

  // If this is the first route step just add it to the list.
  if (new_route_step.GetTime () != m_node_route.back ().GetTime () + 1u)
    throw std::invalid_argument ("Invalid new route step: the time of the new route step must "
                                 "be the immediate following second after the time of the last "
                                 "route step.");

  // There are at least one other route step, validate further.
  if (GetRouteLastTime () - GetRouteInitialTime () + 1u != m_node_route.size ())
    {
      char buffer[25];
      std::sprintf (buffer, "%u", (uint32_t) m_node_route.size ());
      throw std::runtime_error ("Unexpected error: there should be exactly "
                                + std::string (buffer) + " items in the list of route steps.");
    }

  m_node_route.push_back (new_route_step);
}

const RouteStep &
NodeRouteData::GetRouteStep (uint32_t time) const
{
  // Check if the route is empty
  if (m_node_route.empty ())
    throw std::runtime_error ("Empty route. If the route is empty, then there is nothing to "
                              "retrieve.");

  if (time < GetRouteInitialTime () || time > GetRouteLastTime ())
    throw std::out_of_range ("Invalid time: there isn't any route step at the given time.");

  return m_node_route.at (time - GetRouteInitialTime ());
}

std::string
NodeRouteData::ToString () const
{
  std::string str;
  char buffer[25];

  if (m_node_route.empty ())
    {
      str = "an empty route";
    }
  else
    {
      std::sprintf (buffer, "%u", GetRouteInitialTime ());
      str = "a route from second " + std::string (buffer) + " to second ";

      std::sprintf (buffer, "%u", GetRouteLastTime ());
      str += std::string (buffer);
    }

  std::sprintf (buffer, "%u", GetNodeId ());
  return "Node with ID " + std::string (buffer) + " has " + str + ".";
}

void
NodeRouteData::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                                NodesRoutesData
// =============================================================================

NodesRoutesData::NodesRoutesData ()
: m_nodes_routes () { }

NodesRoutesData::NodesRoutesData (const std::string & input_filename)
: NodesRoutesData ()
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (input_filename);
  std::ifstream input_file (filename_trimmed, std::ios::in);
  std::string text_line;
  std::vector<std::string> tokens;

  if (!input_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Importing the routes of some nodes to file \"" << filename_trimmed << "\"... ";

  // Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Expected 8 numbers
  uint32_t node_id, time;
  double coord_x, coord_y, initial_distance, ending_distance;
  std::string street_name;

  std::set<uint32_t> known_nodes;

  while (LibraryUtils::GetInputStreamNextLine (input_file, text_line))
    {
      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 8)
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

      // route_index = (uint32_t) std::stoi (tokens.at (0));
      node_id = (uint32_t) std::stoi (tokens.at (1));
      time = (uint32_t) std::stoi (tokens.at (2));

      coord_x = std::stod (tokens.at (3));
      coord_y = std::stod (tokens.at (4));

      street_name = tokens.at (5);

      initial_distance = std::stod (tokens.at (6));
      ending_distance = std::stod (tokens.at (7));

      if (!known_nodes.count (node_id))
        {
          known_nodes.insert (node_id);
          AddNode (node_id);
        }

      AddNodeRouteStep (node_id, RouteStep (time, LibraryUtils::Vector2D (coord_x, coord_y),
                                            street_name, initial_distance, ending_distance));
    }

  input_file.close ();
  std::cout << "Done.\n";
}

NodesRoutesData::NodesRoutesData (const NodesRoutesData & copy)
: m_nodes_routes (copy.m_nodes_routes) { }

bool
NodesRoutesData::ContainsNode (uint32_t node_id) const
{
  return m_nodes_routes.count (node_id) > 0;
}

bool
NodesRoutesData::AddNode (uint32_t node_id)
{
  if (ContainsNode (node_id))
    return false;

  m_nodes_routes.insert (std::make_pair (node_id, NodeRouteData (node_id)));
  return true;
}

void
NodesRoutesData::AddNodeRouteStep (uint32_t node_id, const RouteStep & new_route_step)
{
  if (!ContainsNode (node_id))
    throw std::out_of_range ("Invalid node ID: the given node ID doesn't exist.");

  m_nodes_routes.at (node_id).AddRouteStep (new_route_step);
}

const NodeRouteData &
NodesRoutesData::GetNodeRouteData (uint32_t node_id) const
{
  if (!ContainsNode (node_id))
    throw std::out_of_range ("Invalid node ID: the given node ID doesn't exist.");

  return m_nodes_routes.at (node_id);
}

uint32_t
NodesRoutesData::GetNodeRouteInitialTime (uint32_t node_id) const
{
  return m_nodes_routes.at (node_id).GetRouteInitialTime ();
}

uint32_t
NodesRoutesData::GetNodeRouteLastTime (uint32_t node_id) const
{
  return m_nodes_routes.at (node_id).GetRouteLastTime ();
}

uint32_t
NodesRoutesData::GetNodeRouteDuration (uint32_t node_id) const
{
  return m_nodes_routes.at (node_id).GetRouteDuration ();
}

void
NodesRoutesData::ExportToFile (const std::string & filename) const
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();
  char buffer[25];

  if (filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream output_file (filename_trimmed, std::ios::out);

  if (!output_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Exporting the routes of the nodes to file \"" << filename_trimmed << "\"... ";

  output_file << "# Route Step Index, Node ID, Time, Coordinate X, Coordinate Y, Street Name, "
          "Distance to Initial Junction, Distance to Ending Junction" << end_line;

  uint32_t route_index;
  for (std::map<uint32_t, NodeRouteData>::const_iterator node_routes_it = m_nodes_routes.begin ();
          node_routes_it != m_nodes_routes.end (); ++node_routes_it)
    {
      route_index = 0u;
      const std::vector<RouteStep> & node_route = node_routes_it->second.GetCompleteRoute ();

      for (std::vector<RouteStep>::const_iterator route_step_it = node_route.begin ();
              route_step_it != node_route.end (); ++route_step_it)
        {
          std::sprintf (buffer, "%u", route_index++);
          output_file << std::string (buffer) << ", ";

          std::sprintf (buffer, "%u", node_routes_it->first);
          output_file << std::string (buffer) << ", ";

          std::sprintf (buffer, "%u", route_step_it->GetTime ());
          output_file << std::string (buffer) << ", ";

          std::sprintf (buffer, "%.6f", route_step_it->GetPositionCoordinate ().m_x);
          output_file << std::string (buffer) << ", ";

          std::sprintf (buffer, "%.6f", route_step_it->GetPositionCoordinate ().m_y);
          output_file << std::string (buffer) << ", ";

          output_file << route_step_it->GetStreetName () << ", ";

          std::sprintf (buffer, "%.6f", route_step_it->GetDistanceToInitialJunction ());
          output_file << std::string (buffer) << ", ";

          std::sprintf (buffer, "%.6f", route_step_it->GetDistanceToEndingJunction ());
          output_file << std::string (buffer) << end_line;
        }
    }

  output_file.close ();
  std::cout << "Done.\n";
}

std::string
NodesRoutesData::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%u", (uint32_t) m_nodes_routes.size ());
  return "Routes of " + std::string (buffer) + " node(s) stored.";
}

void
NodesRoutesData::Print (std::ostream & os) const
{
  os << ToString ();
}

}
}
