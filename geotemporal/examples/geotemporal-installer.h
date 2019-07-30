/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2019 Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>, 
 *                    Networks and Data Science Laboratory (NDS-Lab) at the 
 *                    Computing Research Center (CIC-IPN) <www.prime.cic.ipn.mx>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This is an example script for the Geo-Temporal routing protocol.
 * 
 * Author: Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>
 */

#ifndef GEOTEMPORAL_INSTALLER_H
#define GEOTEMPORAL_INSTALLER_H

#include <map>
#include <string>

#include <ns3/geotemporal-library-module.h>
#include <ns3/ipv4-interface-container.h>
#include <ns3/net-device-container.h>
#include <ns3/node-container.h>
#include <ns3/ptr.h>

using namespace GeoTemporalLibrary::NavigationSystem;
using namespace GeoTemporalLibrary::LibraryUtils;

namespace ns3
{

// =============================================================================
//                             GeoTemporalInstaller
// =============================================================================

class GeoTemporalInstaller
{
private:

  // --------------------------
  // Containers
  // --------------------------

  Ipv4InterfaceContainer m_ipv4_interfaces_container;

  NetDeviceContainer m_net_devices_container;

  NodeContainer m_nodes_container;

  /** Emulates a GPS system. */
  Ptr<GpsSystem> m_gps_system;

  /** Contains sets of lists of destination geo-temporal areas. */
  Ptr<RandomDestinationGeoTemporalAreasLists> m_random_destination_gtas;

  /** Contains the equivalences of node ID to node IP address. */
  std::map<uint32_t, Ipv4Address> m_node_id_to_ip;


  // --------------------------
  // Simulation parameters
  // --------------------------

  /** Number of the current simulation (it should start with 1). */
  uint32_t m_simulation_number;

  /** The total duration of the simulation (in seconds). */
  uint32_t m_simulation_duration;

  /** The number of vehicles that will generate DATA packets. */
  uint32_t m_data_packet_source_vehicles_count;

  /** The number of DATA packets generated at each source node. */
  uint32_t m_data_packets_per_source;

  /** The size (in bytes) of the message to disseminate. */
  uint32_t m_data_packet_message_size;

  /** Interval (in milliseconds) between DATA packets transmissions. */
  uint32_t m_data_packets_data_rate;

  /**
   * The (unique) identifier of the mobility scenario used in the simulation.
   * 
   * This parameter defines the streets-map used, the number, mobility and 
   * routes of the vehicles, and the source nodes and destination geo-temporal 
   * areas of the generated geo-temporal packets.
   * 
   * The valid identifiers are:
   * 
   * - fixed: The nodes are NOT mobile and its position is determined in a grid
   *   position allocator.
   * 
   * - 20, 30, 60, 90, & 120: Uses a simple streets map inspired in Murcia,
   *   Spain. The mobility and routes of the vehicles are artificial.
   *   18 vehicles enter the simulation each X seconds, where X is the 
   *   identifier.
   * 
   * - 30-60, 30-90, 30-120, 60-90, 60-120, 90-120: Uses a simple streets map 
   *   inspired in Murcia, Spain. The mobility and routes of the vehicles are 
   *   artificial.
   *   10 vehicles enter the simulation in the north part each X seconds, and 
   *   8 vehicles enter the simulation in the south part each Y seconds, where
   *   X-Y is the identifier.
   * 
   * - low, medium, & high: Uses a realistic streets map based in Luxembourg 
   *   city. The mobility and routes of the vehicles are realistic as well.
   *   The identifier indicates the density of vehicles. There are 3,127 
   *   vehicles during the entire 'low' density simulation, 6,370 vehicles
   *   for the 'medium' density simulation, and 9,080 vehicles for the
   *   'high' density simulation.
   */
  std::string m_mobility_scenario_id;

  /** 
   * The number of vehicle nodes to create. Only actively used when the fixed 
   * mobility scenario is selected.
   * 
   * When the fixed mobility scenario is selected this parameter indicates the
   * number of vehicles to create in the simulation.
   * 
   * When a non-fixed mobility scenario is selected this parameter is ignored
   * and overridden by the number of vehicles in the selected mobility scenario.
   */
  uint32_t m_vehicles_count;

  /** 
   * Distance in (meters) between adjacent nodes in the grid of nodes.
   * 
   * Only used when the fixed mobility scenario is selected.
   */
  uint32_t m_fixed_nodes_distance;

  /**
   * Flag that indicates if the simulation uses the 802.11p MAC protocol or
   * the 802.11b MAC protocol.
   * \c true for 802.11p.
   * \c false for 802.11b.
   */
  bool m_use_80211p_mac_protocol;

  /**
   * Time interval (in seconds) between progress reports. If set to zero then
   * progress report are disabled.
   */
  uint32_t m_progress_report_time_interval;


  // --------------------------
  // Routing protocol parameters
  // --------------------------

  /** Time interval (in milliseconds) between HELLO packets transmissions. */
  uint32_t m_hello_packets_interval;

  /** 
   * The maximum number of packets that we allow the routing protocol to buffer.
   */
  uint32_t m_packets_queue_capacity;

  /**
   * Time (in seconds) interval in which a recently contacted neighbor node is 
   * not contacted again.
   */
  uint32_t m_neighbor_expiration_time;

  /**
   * The number of packet replicas of each DATA packet that we allow the 
   * routing protocol to transmit.
   */
  uint32_t m_data_packet_replicas;

  /** Minimum distance (in meters) difference between nodes to be valid packet carriers. */
  double m_neighbor_min_valid_distance_diff;

  /** Size (in seconds) of the time-slot to calculate exponential average. */
  uint32_t m_exponential_average_time_slot_size;


  // --------------------------
  // Input files
  // --------------------------

  /** The file that contains the streets-map graph. */
  std::string m_streets_graph_input_filename;

  /** The file that contains the street junctions coordinates. */
  std::string m_street_junctions_input_filename;

  /** The file that contains the routes of the vehicles. */
  std::string m_vehicles_routes_input_filename;

  /** The mobility trace file of the vehicles. */
  std::string m_vehicles_mobility_trace_filename;

  /** The file that contains the destination geo-temporal areas generated 
   * at random. */
  std::string m_random_destination_gta_input_filename;

  /** 
   * The file that contains the list of vehicles that visited each 
   * geo-temporal area. 
   */
  std::string m_gta_visitor_vehicles_input_filename;


  // --------------------------
  // Output files
  // --------------------------

  /** The file where the statistics will be saved in a XML format. */
  std::string m_statistics_output_filename;


public:

  GeoTemporalInstaller ();

  GeoTemporalInstaller (const GeoTemporalInstaller & copy);

  /** Returns a map that contains the equivalence from node ID to IP address. */
  inline const std::map<uint32_t, Ipv4Address> &
  GetNodeIdToIpMap () const
  {
    return m_node_id_to_ip;
  }


  /**
   * Receives user input and configures the simulation, mobility and routing 
   * protocol parameters.
   * 
   * @return Returns <code>true</code> if the configuration was set successfully, 
   * otherwise returns <code>false</code>.
   */
  bool
  Configure (int argc, char **argv);

  void
  Run ();

private:

  void
  CreateNodes ();

  void
  CreateDevices ();

  void
  InstallInternetStack ();

  void
  ConfigureNodesMobility ();

  void
  InstallAplications ();

  void
  FreeUnnecessaryResources ();

  void
  ScheduleNextProgressReport ();

  void
  DoProgressReport ();

public:

  void
  Report (const std::string & output_xml_filename);
};

} // namespace ns3


#endif /* GEOTEMPORAL_INSTALLER_H */

