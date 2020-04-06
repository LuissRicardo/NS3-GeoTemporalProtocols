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

#include "geotemporal-installer.h"
#include "geotemporal-application.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>

#include <ns3/abort.h>
#include <ns3/assert.h>
#include <ns3/command-line.h>
#include <ns3/log.h>
#include <ns3/names.h>
#include <ns3/rng-seed-manager.h>
#include <ns3/simulator.h>

#include <ns3/geotemporal-helper.h>
#include <ns3/geotemporal-packets.h>
#include <ns3/geotemporal-routing-protocol.h>
#include <ns3/geotemporal-utils.h>

// Wifi
#include <ns3/wifi-helper.h>
#include <ns3/yans-wifi-helper.h>
#include <ns3/wave-mac-helper.h>
#include <ns3/wifi-80211p-helper.h>

// Modules
#include <ns3/core-module.h>
#include <ns3/internet-module.h>
#include <ns3/mobility-module.h>

namespace ns3
{

// =============================================================================
//                             GeoTemporalInstaller
// =============================================================================

NS_LOG_COMPONENT_DEFINE ("GeoTemporalInstaller");

GeoTemporalInstaller::GeoTemporalInstaller () :
m_ipv4_interfaces_container (), m_net_devices_container (), m_nodes_container (),
m_gps_system (0), m_random_destination_gtas (0), m_node_id_to_ip (),
m_simulation_number (1u), m_simulation_duration (600u),
m_data_packet_source_vehicles_count (8u), m_data_packets_per_source (2u),
m_emergency_data_packets_ratio (0.0), m_emergency_data_packets_per_source (0u),
m_normal_data_packets_per_source (0u), m_data_packet_message_size (128u),
m_data_packets_data_rate (5u), m_mobility_scenario_id ("60"),
m_vehicles_count (2u), m_fixed_nodes_distance (200u),
m_use_80211p_mac_protocol (false), m_progress_report_time_interval (25u),
m_hello_packets_interval (1000u), m_packets_queue_capacity (128u),
m_neighbor_expiration_time (10u), m_data_packet_replicas (3u),
m_neighbor_min_valid_distance_diff (20.0), m_exponential_average_time_slot_size (30u),
m_streets_graph_input_filename (""), m_street_junctions_input_filename (""),
m_vehicles_routes_input_filename (""), m_vehicles_mobility_trace_filename (""),
m_random_destination_gta_input_filename (""), m_gta_visitor_vehicles_input_filename (""),
m_statistics_output_filename ("/simulations-output/simulation_statistics.xml")
{
  NS_LOG_FUNCTION (this);
}

GeoTemporalInstaller::GeoTemporalInstaller (const GeoTemporalInstaller& copy) :
m_ipv4_interfaces_container (copy.m_ipv4_interfaces_container),
m_net_devices_container (copy.m_net_devices_container),
m_nodes_container (copy.m_nodes_container),
m_gps_system (copy.m_gps_system),
m_random_destination_gtas (copy.m_random_destination_gtas),
m_node_id_to_ip (copy.m_node_id_to_ip),
m_simulation_number (copy.m_simulation_number),
m_simulation_duration (copy.m_simulation_duration),
m_data_packet_source_vehicles_count (copy.m_data_packet_source_vehicles_count),
m_data_packets_per_source (copy.m_data_packets_per_source),
m_emergency_data_packets_ratio (copy.m_emergency_data_packets_ratio),
m_emergency_data_packets_per_source (copy.m_emergency_data_packets_per_source),
m_normal_data_packets_per_source (copy.m_normal_data_packets_per_source),
m_data_packet_message_size (copy.m_data_packet_message_size),
m_data_packets_data_rate (copy.m_data_packets_data_rate),
m_mobility_scenario_id (copy.m_mobility_scenario_id),
m_vehicles_count (copy.m_vehicles_count),
m_fixed_nodes_distance (copy.m_fixed_nodes_distance),
m_use_80211p_mac_protocol (copy.m_use_80211p_mac_protocol),
m_progress_report_time_interval (copy.m_progress_report_time_interval),
m_hello_packets_interval (copy.m_hello_packets_interval),
m_packets_queue_capacity (copy.m_packets_queue_capacity),
m_neighbor_expiration_time (copy.m_neighbor_expiration_time),
m_data_packet_replicas (copy.m_data_packet_replicas),
m_neighbor_min_valid_distance_diff (copy.m_neighbor_min_valid_distance_diff),
m_exponential_average_time_slot_size (copy.m_exponential_average_time_slot_size),
m_streets_graph_input_filename (copy.m_streets_graph_input_filename),
m_street_junctions_input_filename (copy.m_street_junctions_input_filename),
m_vehicles_routes_input_filename (copy.m_vehicles_routes_input_filename),
m_vehicles_mobility_trace_filename (copy.m_vehicles_mobility_trace_filename),
m_random_destination_gta_input_filename (copy.m_random_destination_gta_input_filename),
m_gta_visitor_vehicles_input_filename (copy.m_gta_visitor_vehicles_input_filename),
m_statistics_output_filename (copy.m_statistics_output_filename)
{
  NS_LOG_FUNCTION (this);
}

bool
GeoTemporalInstaller::Configure (int argc, char** argv)
{
  NS_LOG_FUNCTION (this);

  CommandLine cmd;

  // Simulation parameters

  cmd.AddValue ("simulationId",
                "Identifier number of the current simulation. [Default value: 1]",
                m_simulation_number);

  cmd.AddValue ("simulationDuration",
                "The total duration of the simulation (in seconds). "
                "[Default value: 600]",
                m_simulation_duration);

  cmd.AddValue ("dataPacketSourceVehiclesCount",
                "The number of vehicles that will generate DATA packets. "
                "[Default value: 8]",
                m_data_packet_source_vehicles_count);

  cmd.AddValue ("dataPacketsPerSource",
                "The number of DATA packets generated at each source node. "
                "[Default value: 2]",
                m_data_packets_per_source);

  cmd.AddValue ("emergencyDataPacketsRatio",
                "The ratio of emergency DATA packets generated at each source "
                "node. [Default value: 0.0]",
                m_emergency_data_packets_ratio);

  cmd.AddValue ("dataPacketMessageSize",
                "The size (in bytes) of the message to disseminate. "
                "[Default value: 128]",
                m_data_packet_message_size);

  cmd.AddValue ("dataPacketsDataRate",
                "Interval (in milliseconds) between DATA packets transmissions. "
                "[Default value: 5]",
                m_data_packets_data_rate);

  cmd.AddValue ("mobilityScenarioId",
                "The identifier of the mobility scenario used in the simulation."
                "Valid values are: fixed, 20, 30, 60, 90, 120, 30-60, 30-90, "
                "30-120, 60-90, 60-120, 90-120, low, medium, high. "
                "[Default value: 60]",
                m_mobility_scenario_id);

  cmd.AddValue ("vehiclesCount",
                "The number of vehicle nodes to create. Only actively used when "
                "the fixed mobility scenario is selected."
                "[Default value: 2]",
                m_vehicles_count);

  cmd.AddValue ("fixedNodesDistance",
                "Distance in (meters) between adjacent nodes in the grid of "
                "nodes. Only used when the fixed mobility scenario is selected. "
                "[Default value: 200]",
                m_fixed_nodes_distance);

  cmd.AddValue ("use80211pMacProtocol",
                "Flag that indicates if the simulation uses the 802.11p MAC "
                "protocol (with true) or the 802.11b MAC protocol (with false). "
                "[Default value: false]",
                m_use_80211p_mac_protocol);

  cmd.AddValue ("progressReportInterval",
                "Time interval (in seconds) between progress reports. If set to "
                "zero then progress reports are disabled. "
                "[Default value: 25]",
                m_progress_report_time_interval);

  // Routing protocol parameters

  cmd.AddValue ("helloPacketsInterval",
                "Time interval (in milliseconds) between HELLO packets transmissions. "
                "[Default value: 1,000]",
                m_hello_packets_interval);

  cmd.AddValue ("packetQueueCapacity",
                "The maximum number of packets that we allow the routing "
                "protocol to buffer. "
                "[Default value: 128]",
                m_packets_queue_capacity);

  cmd.AddValue ("neighborExpirationTime",
                "Time (in seconds) interval in which a recently contacted "
                "neighbor node is not contacted again. "
                "[Default value: 10]",
                m_neighbor_expiration_time);

  cmd.AddValue ("dataPacketReplicas",
                "The number of packet replicas of each DATA packet that we "
                "allow the routing protocol to transmit. "
                "[Default value: 3]",
                m_data_packet_replicas);

  cmd.AddValue ("neighborMinimumValidDistanceDifference",
                "Minimum distance (in meters) difference between nodes to be "
                "valid packet carriers. "
                "[Default value: 20]",
                m_neighbor_min_valid_distance_diff);

  cmd.AddValue ("exponentialAverageTimeSlotSize",
                "Size (in seconds) of the time-slot to calculate exponential "
                "average. "
                "[Default value: 30]",
                m_exponential_average_time_slot_size);

  // Output files

  cmd.AddValue ("outputStatisticsFile",
                "The name of the file where the statistics will be saved in a "
                "XML format. [Default value: /simulations-output/simulation_statistics.xml]",
                m_statistics_output_filename);

  cmd.Parse (argc, argv);

  // Validations
  if (m_simulation_number == 0u)
    NS_ABORT_MSG ("Invalid simulation number. The minimum valid value is 1.");

  if (m_emergency_data_packets_ratio < 0.0 || m_emergency_data_packets_ratio > 1.0)
    NS_ABORT_MSG ("The ratio of emergency DATA packets must be between 0.0 and "
                  "1.0, including both limits.");

  // Configure the name of the mobility related files.
  if (m_mobility_scenario_id == "20")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 020.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 020.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 020.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 020.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "30")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 030.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 030.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 030.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 030.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "60")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 060.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 060.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 060.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 060.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "90")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 090.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 090.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 090.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 090.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "120")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 120.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 120.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 120.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Homogeneous mobilities)/Homogeneous 120.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "30-60")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-060.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-060.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-060.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-060.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "30-90")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-090.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-090.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-090.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-090.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "30-120")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-120.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-120.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-120.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 030-120.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "60-90")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-090.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-090.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-090.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-090.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "60-120")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-120.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-120.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-120.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 060-120.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "90-120")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Murcia.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Murcia.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 090-120.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 090-120.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 090-120.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Murcia (Heterogeneous mobilities)/Heterogeneous 090-120.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "low")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Luxembourg.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Luxembourg.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Luxembourg/Low.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Luxembourg/Low.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Luxembourg/Low.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Luxembourg/Low.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "medium")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Luxembourg.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Luxembourg.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Luxembourg/Medium.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Luxembourg/Medium.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Luxembourg/Medium.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Luxembourg/Medium.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "high")
    {
      // GPS initialization files
      m_streets_graph_input_filename = "simulations-input/Luxembourg.graph.txt";
      m_street_junctions_input_filename = "simulations-input/Luxembourg.junctions.txt";
      m_vehicles_routes_input_filename = "simulations-input/Luxembourg/High.routes.txt";

      // Vehicles' mobility trace file
      m_vehicles_mobility_trace_filename = "simulations-input/Luxembourg/High.ns2.mob.tcl";

      // Random destination areas.
      m_random_destination_gta_input_filename = "simulations-input/Luxembourg/High.random_geo_temporal_areas.txt";

      // Geo-temporal areas visitor vehicles
      m_gta_visitor_vehicles_input_filename = "simulations-input/Luxembourg/High.geo_temporal_areas_visitor_nodes.txt";
    }
  else if (m_mobility_scenario_id == "fixed")
    {
      // Do something when fixed mobility.
    }
  else
    {
      NS_ABORT_MSG ("Invalid mobility scenario identifier. Valid values are: "
                    "fixed, 20, 30, 60, 90, 120, 30-60, 30-90, 30-120, 60-90, "
                    "60-120, 90-120, low, medium, high.");
    }

  // When the nodes have mobility create GPS and configurate GPS system and 
  // vehicles count from the input files from the selected mobility scenario.
  if (m_mobility_scenario_id != "fixed")
    {
      // Create GPS system
      NS_ASSERT (m_gps_system == 0);
      m_gps_system = Create<GpsSystem> (m_streets_graph_input_filename,
                                        m_vehicles_routes_input_filename,
                                        m_street_junctions_input_filename);

      // Set number of vehicles in the simulation
      m_vehicles_count = m_gps_system->GetVehiclesRoutesData ().GetNodesCount ();

      // Random destination geo-temporal areas object
      NS_ASSERT (m_random_destination_gtas == 0);
      m_random_destination_gtas =
              Create<RandomDestinationGeoTemporalAreasLists> (m_random_destination_gta_input_filename);
    }

  // Compute the number of emergency packets
  if (m_emergency_data_packets_ratio > 0.0)
    {
      m_emergency_data_packets_per_source =
              (uint32_t) std::round ((double) m_data_packets_per_source * m_emergency_data_packets_ratio);
      m_normal_data_packets_per_source = m_data_packets_per_source - m_emergency_data_packets_per_source;
    }
  else
    {
      m_emergency_data_packets_per_source = 0u;
      m_normal_data_packets_per_source = m_data_packets_per_source;
    }

  NS_ASSERT (m_normal_data_packets_per_source + m_emergency_data_packets_per_source
             == m_data_packets_per_source);

  return true;
}

void
GeoTemporalInstaller::Run ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "Running simulation...\n\n";

  std::cout << "Parameters:\n";

  const double emergency_packets_ptg = m_emergency_data_packets_ratio * 100.0;
  const double real_emergency_packets_ptg
          = (double) m_emergency_data_packets_per_source / (double) m_data_packets_per_source * 100.0;

  std::cout << " - Simulation Number               :  " << m_simulation_number << "\n";
  std::cout << " - Simulation duration             :  " << m_simulation_duration << " seconds\n";
  std::cout << " - Number of source vehicle nodes  :  " << m_data_packet_source_vehicles_count << " vehicles\n";
  std::cout << " - Data packets per source node    :  " << m_data_packets_per_source
          << " packets (" << m_normal_data_packets_per_source << " normal, " << m_emergency_data_packets_per_source << " emergency)\n";
  std::cout << " - Emergency packets percentage    :  " << emergency_packets_ptg << " % (" << real_emergency_packets_ptg << " % real)\n";
  std::cout << " - Data packet message size        :  " << m_data_packet_message_size << " bytes\n";
  std::cout << " - Data packets data rate          :  " << m_data_packets_data_rate << " milliseconds\n";
  std::cout << " - Mobility scenario ID            :  " << m_mobility_scenario_id << "\n";
  std::cout << " - Vehicles count                  :  " << m_vehicles_count << "\n";
  std::cout << " - Distance between fixed nodes    :  " << m_fixed_nodes_distance << " meters";
  if (m_mobility_scenario_id != "fixed")
    std::cout << " (Using mobile nodes, parameter ignored)\n";
  else
    std::cout << " (Using fixed position nodes, parameter used)\n";
  std::cout << " - Use 802.11p MAC protocol        :  " << (m_use_80211p_mac_protocol ? "Enabled" : "Disabled") << "\n";
  std::cout << " - Progress reports time interval  :  ";
  if (m_progress_report_time_interval == 0u)
    std::cout << "Disabled\n";
  else
    std::cout << m_progress_report_time_interval << " seconds\n";
  std::cout << "\n";

  std::cout << " - Hello packets time interval     :  " << m_hello_packets_interval << " milliseconds\n";
  std::cout << " - Packets queue capacity          :  " << m_packets_queue_capacity << " packets\n";
  std::cout << " - Neighbor expiration time        :  " << m_neighbor_expiration_time << " seconds\n";
  std::cout << " - Data packet replicas            :  " << m_data_packet_replicas << " packet replicas\n";
  std::cout << " - Min valid distance between nodes:  " << m_neighbor_min_valid_distance_diff << " meters\n";
  std::cout << " - Exp adap avg time-slot size     :  " << m_exponential_average_time_slot_size << " seconds\n";
  std::cout << "\n";

  if (m_mobility_scenario_id != "fixed")
    std::cout << " - Output statistics XML file      :  " << m_statistics_output_filename << "\n";
  else
    std::cout << " - Output statistics XML file      :  Disabled (Using fixed position nodes)\n";
  std::cout << "\n\n";

  // Configures the seed number of the random number generator used in the 
  // simulation. For this we use de simulation ID.
  RngSeedManager::SetSeed (m_simulation_number);

  CreateNodes ();
  CreateDevices ();
  InstallInternetStack ();
  ConfigureNodesMobility ();
  InstallAplications ();
  FreeUnnecessaryResources ();

  std::cout << "Running simulation with a duration of " << m_simulation_duration
          << " second(s)...\n";

  ScheduleNextProgressReport ();
  Simulator::Stop (Seconds (m_simulation_duration));
  Simulator::Run ();
  Simulator::Destroy ();

  std::cout << "Simulation finished.\n";

  Report (m_statistics_output_filename);
}

void
GeoTemporalInstaller::CreateNodes ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "Creating " << m_vehicles_count << " vehicle nodes(s)... ";

  m_nodes_container.Create (m_vehicles_count);

  // Naming nodes
  char buffer[10];

  for (uint32_t node_index = 0u; node_index < m_vehicles_count; ++node_index)
    {
      std::sprintf (buffer, "%u", node_index);
      Names::Add ("Node-" + std::string (buffer), m_nodes_container.Get (node_index));
    }

  std::cout << "Done.\n";
}

void
GeoTemporalInstaller::CreateDevices ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "Creating " << m_vehicles_count << " hardware device(s)... ";

  // Set up WiFi
  WifiHelper wifi_helper;

  YansWifiPhyHelper wifi_phy_helper = YansWifiPhyHelper::Default ();
  wifi_phy_helper.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11);

  // Install the selected 802.11 protocol: 802.11p or 802.11b
  if (m_use_80211p_mac_protocol)
    {
      YansWifiChannelHelper wifi_channel_helper = YansWifiChannelHelper::Default ();
      wifi_phy_helper.SetChannel (wifi_channel_helper.Create ());

      NqosWaveMacHelper wifi_80211p_mac_helper = NqosWaveMacHelper::Default ();
      Wifi80211pHelper wifi_80211p_helper = Wifi80211pHelper::Default ();
      wifi_80211p_helper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                                  "DataMode", StringValue ("OfdmRate6MbpsBW10MHz"),
                                                  "ControlMode", StringValue ("OfdmRate6MbpsBW10MHz"));

      m_net_devices_container = wifi_80211p_helper.Install (wifi_phy_helper,
                                                            wifi_80211p_mac_helper,
                                                            m_nodes_container);
    }
  else
    {
      YansWifiChannelHelper wifi_channel_helper;
      wifi_channel_helper.SetPropagationDelay ("ns3::ConstantSpeedPropagationDelayModel");
      wifi_channel_helper.AddPropagationLoss ("ns3::FriisPropagationLossModel");

      // For a range of aprox. 250 meters
      wifi_phy_helper.Set ("TxPowerStart", DoubleValue (33));
      wifi_phy_helper.Set ("TxPowerEnd", DoubleValue (33));
      wifi_phy_helper.Set ("TxPowerLevels", UintegerValue (1));
      wifi_phy_helper.Set ("TxGain", DoubleValue (0));
      wifi_phy_helper.Set ("RxGain", DoubleValue (0));
      wifi_phy_helper.Set ("EnergyDetectionThreshold", DoubleValue (-61.8));
      wifi_phy_helper.Set ("CcaMode1Threshold", DoubleValue (-64.8));

      wifi_phy_helper.SetChannel (wifi_channel_helper.Create ());

      // Add a non-QoS upper MAC
      WifiMacHelper wifi_mac_helper;
      wifi_mac_helper.SetType ("ns3::AdhocWifiMac",
                               "QosSupported", BooleanValue (false));

      // Set 802.11b standard
      wifi_helper.SetStandard (WIFI_PHY_STANDARD_80211b);
      wifi_helper.SetRemoteStationManager ("ns3::ConstantRateWifiManager",
                                           "DataMode", StringValue ("DsssRate1Mbps"),
                                           "ControlMode", StringValue ("DsssRate1Mbps"));

      m_net_devices_container = wifi_helper.Install (wifi_phy_helper,
                                                     wifi_mac_helper,
                                                     m_nodes_container);
    }

  // if (m_pcap_output)
  //   {
  //     wifi_phy_helper.EnablePcapAll (std::string ("geotemporal"), true);
  //   }

  std::cout << "Done.\n";
}

void
GeoTemporalInstaller::InstallInternetStack ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "Installing Internet stack... ";

  GeoTemporalHelper gt_helper;

  gt_helper.Set ("HelloInterval", TimeValue (MilliSeconds (m_hello_packets_interval)));
  gt_helper.Set ("PacketsQueueCapacity", UintegerValue (m_packets_queue_capacity));
  gt_helper.Set ("NeighborsExpirationTime", TimeValue (Seconds (m_neighbor_expiration_time)));
  gt_helper.Set ("DataPacketReplicas", UintegerValue (m_data_packet_replicas));
  gt_helper.Set ("NeighborMinimumValidDistanceDifference", DoubleValue (m_neighbor_min_valid_distance_diff));
  gt_helper.Set ("ExponentialAverageTimeSlotSize", TimeValue (Seconds (m_exponential_average_time_slot_size)));

  InternetStackHelper internet_stack;
  internet_stack.SetRoutingHelper (gt_helper); // has effect on the next Install ()
  internet_stack.Install (m_nodes_container);

  // Set the corresponding node ID to each routing protocol instance.
  for (uint32_t node_id = 0; node_id < m_nodes_container.GetN (); ++node_id)
    {
      m_nodes_container.Get (node_id)
              ->GetObject<geotemporal::RoutingProtocol> ()
              ->SetNodeId (node_id);
    }

  // Assign the IP addresses to the interface of each node.
  Ipv4AddressHelper ip_addresses_helper;
  ip_addresses_helper.SetBase ("10.0.0.0", "255.0.0.0");
  m_ipv4_interfaces_container = ip_addresses_helper.Assign (m_net_devices_container);

  // Fill the map that links the node ID to the IP address of the node, and the 
  // inverse map (IP address to node ID).
  std::map<Ipv4Address, uint32_t> node_ip_to_id;

  for (uint32_t node_id = 0; node_id < m_ipv4_interfaces_container.GetN (); ++node_id)
    {
      m_node_id_to_ip.insert (std::make_pair (node_id,
                                              m_ipv4_interfaces_container.GetAddress (node_id)));

      node_ip_to_id.insert (std::make_pair (m_ipv4_interfaces_container.GetAddress (node_id),
                                            node_id));
    }

  if (m_mobility_scenario_id != "fixed")
    m_gps_system->SetNodeIpAddressToIdMapping (node_ip_to_id);

  std::cout << "Done.\n";
}

void
GeoTemporalInstaller::ConfigureNodesMobility ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "Configuring nodes mobility... \n";

  // Set vehicles mobility scenario
  if (m_mobility_scenario_id != "fixed")
    {
      // Import mobility from selected mobility scenario
      std::cout << "\tImporting vehicles mobility from file \""
              << m_vehicles_mobility_trace_filename << "\"... ";

      // Load the mobility trace of the vehicles generated by SUMO simulator.
      Ns2MobilityHelper mobility_helper (m_vehicles_mobility_trace_filename);

      // Read the mobility trace file in a format compatible for ns-2 and ns-3,
      // and install the mobility patterns of all nodes contained in the global
      // ns3::NodeList whose node ID matches the node ID in the trace file.
      mobility_helper.Install ();

      std::cout << "Done.\n";

      /* Now, some vehicles in the mobility trace start their mobility after the
       * simulation's initial second and some vehicles end their mobility before
       * the simulation's last second.
       * 
       * NS-3 does not handle this appropriately because all nodes are activated
       * at the start of the simulation. This causes that the vehicle nodes that 
       * initiate their mobility after the start of the simulation are activated
       * and stay in their initial location until they start moving. Likewise,
       * the vehicle nodes that end their mobility before the last second of the
       * simulation stop moving and stay in their last location until the end of
       * the simulation. This is not acceptable because they participate in the
       * simulation when they should not to.
       * 
       * To fix this, we disable vehicle nodes that initiate their mobility 
       * after the start of the simulation and enable them when they start 
       * moving. And disable vehicle nodes when they end their mobility.
       */

      std::cout << "\tConfiguring initial and ending time of vehicle node(s)... \n";

      Ptr<Node> node;
      Ptr<geotemporal::RoutingProtocol> routing_protocol;
      uint32_t node_initial_time, node_end_time;

      for (uint32_t node_id = 0u; node_id < m_nodes_container.GetN (); ++node_id)
        {
          node = m_nodes_container.Get (node_id);
          routing_protocol = node->GetObject<geotemporal::RoutingProtocol> ();

          // Get the node's route initial and ending time.
          node_initial_time = m_gps_system->GetVehiclesRoutesData ().GetNodeRouteInitialTime (node_id);
          node_end_time = m_gps_system->GetVehiclesRoutesData ().GetNodeRouteLastTime (node_id);

          // std::cout << "\t\tNode " << node_id << " enabled at second " << node_initial_time
          //         << " and disabled at second " << node_end_time << ".\n";

          // If the node's route initial time is 1 then it starts enabled.
          // If the node's route initial time is greater than 1 then it is 
          // disabled and later enabled via a scheduled event.
          if (node_initial_time > 1)
            {
              routing_protocol->Disable ();

              if (node_initial_time < m_simulation_duration)
                {
                  Simulator::ScheduleWithContext (node_id, Seconds (node_initial_time),
                                                  &geotemporal::RoutingProtocol::Enable,
                                                  routing_protocol);
                }
            }

          // If the node's route last time is less than the simulation total duration 
          // schedule a disable.
          if (node_end_time < m_simulation_duration)
            {
              Simulator::ScheduleWithContext (node_id, Seconds (node_end_time),
                                              &geotemporal::RoutingProtocol::Disable,
                                              routing_protocol);
            }
        }

      std::cout << "\tFinished configuring initial and ending times.\n";

      std::cout << "\tInstalling GPS system in all vehicle node(s)... ";

      for (uint32_t node_id = 0u; node_id < m_nodes_container.GetN (); ++node_id)
        {
          node = m_nodes_container.Get (node_id);
          routing_protocol = node->GetObject<geotemporal::RoutingProtocol> ();

          routing_protocol->SetGpsSystem (m_gps_system);
        }

      std::cout << "Done.\n";
    }
  else
    {
      // Create a static grid of (vehicle) nodes
      std::cout << "\tCreating a grid of static vehicle nodes (adjacent nodes are "
              << m_fixed_nodes_distance << " meters apart from each other)... ";
      MobilityHelper mobility_helper;

      // Define grid's width
      uint32_t grid_width = std::sqrt (m_vehicles_count);

      if (grid_width < 3u) grid_width = 3u;

      mobility_helper.SetPositionAllocator ("ns3::GridPositionAllocator",
                                            "MinX", DoubleValue (0.0),
                                            "MinY", DoubleValue (0.0),
                                            "DeltaX", DoubleValue (m_fixed_nodes_distance),
                                            "DeltaY", DoubleValue (m_fixed_nodes_distance),
                                            "GridWidth", UintegerValue (grid_width),
                                            "LayoutType", StringValue ("RowFirst"));
      mobility_helper.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
      mobility_helper.Install (m_nodes_container);
      std::cout << "Done.\n";
    }

  std::cout << "Finished configuring nodes mobility.\n";
}

void
GeoTemporalInstaller::InstallAplications ()
{
  NS_LOG_FUNCTION (this);

  DestinationGeoTemporalArea dest_gta;
  Ipv4Address node_ip;
  Ptr<GeoTemporalApplication> app;
  Ptr<Node> node;

  if (m_mobility_scenario_id == "fixed")
    {
      std::cout << "Installing Geo-Temporal application on "
              << m_data_packet_source_vehicles_count << " static nodes...\n";

      for (uint32_t source_node_index = 0u; source_node_index < m_data_packet_source_vehicles_count;
              ++source_node_index)
        {
          node_ip = m_ipv4_interfaces_container.GetAddress (source_node_index);
          node = m_nodes_container.Get (source_node_index);

          std::cout << "\tInstalling application in node #" << source_node_index << " with IP "
                  << node_ip << "... ";

          dest_gta = DestinationGeoTemporalArea (source_node_index,
                                                 TimePeriod (Seconds (11), Seconds (21)),
                                                 Area (0, 0, 150, 150));

          app = CreateObject<GeoTemporalApplication> ();

          app->SetStartTime (dest_gta.GetTimePeriod ().GetStartTime ());
          app->SetStopTime (Seconds (m_simulation_duration));
          app->ConfigureMultipleMessages (/*Source node ptr*/ node,
                                          /*Destination geo-temporal area*/ dest_gta,
                                          /*Data rate*/ m_data_packets_data_rate,
                                          /*Packets size*/ m_data_packet_message_size,
                                          /*Normal packets number*/ m_normal_data_packets_per_source,
                                          /*Emergency packets number*/ m_emergency_data_packets_per_source);

          node->AddApplication (app);

          std::cout << "Done.\n";
        }

      std::cout << "Finished installing application in static source nodes.\n";
    }
  else
    {
      std::cout << "Pre-processing geographical data...\n";

      const std::vector<Area> & destination_areas_list =
              m_random_destination_gtas->GetDestinationAreasList ();

      for (std::vector<Area>::const_iterator area_it = destination_areas_list.begin ();
              area_it != destination_areas_list.end (); ++area_it)
        {
          m_gps_system->GetSuperNodeStreetGraph (*area_it);
        }

      std::cout << "Finished pre-processing geographical data.\n";

      const std::vector<DestinationGeoTemporalArea> & destination_gtas_list =
              m_random_destination_gtas->GetDestinationGeoTemporalAreasList (m_simulation_number,
                                                                             m_data_packet_source_vehicles_count);

      std::cout << "Installing Geo-Temporal application on "
              << destination_gtas_list.size () << " mobile nodes...\n";

      uint32_t node_id;

      for (std::vector<DestinationGeoTemporalArea>::const_iterator dest_gta_it = destination_gtas_list.begin ();
              dest_gta_it != destination_gtas_list.end (); ++dest_gta_it)
        {
          dest_gta = *dest_gta_it;
          node_id = dest_gta.GetNodeId ();
          node_ip = m_ipv4_interfaces_container.GetAddress (node_id);
          node = m_nodes_container.Get (node_id);

          // std::cout << "\tInstalling application in node #" << node_id << " with IP "
          //         << node_ip << "... ";

          app = CreateObject<GeoTemporalApplication> ();

          app->SetStartTime (dest_gta.GetCreationTime ());
          app->SetStopTime (Seconds (m_simulation_duration));
          app->ConfigureMultipleMessages (/*Source node ptr*/ node,
                                          /*Destination geo-temporal area*/ (GeoTemporalArea) dest_gta,
                                          /*Data rate*/ m_data_packets_data_rate,
                                          /*Packets size*/ m_data_packet_message_size,
                                          /*Normal packets number*/ m_normal_data_packets_per_source,
                                          /*Emergency packets number*/ m_emergency_data_packets_per_source);

          node->AddApplication (app);

          // std::cout << "Done.\n";
        }

      std::cout << "Finished installing application in mobile source nodes.\n";
    }
}

void
GeoTemporalInstaller::FreeUnnecessaryResources ()
{
  std::cout << "Freeing up unnecessary resources... ";

  // After being used to set the source nodes we don't need this anymore
  m_random_destination_gtas = 0;

  std::cout << "Done.\n";
}

void
GeoTemporalInstaller::ScheduleNextProgressReport ()
{
  NS_LOG_FUNCTION (this);

  // It set to zero then progress reports are disabled.
  if (m_progress_report_time_interval == 0u) return;

  // Progress reports are enabled, schedule the next one.
  Simulator::Schedule (Seconds (m_progress_report_time_interval),
                       &GeoTemporalInstaller::DoProgressReport,
                       this);
}

void
GeoTemporalInstaller::DoProgressReport ()
{
  NS_LOG_FUNCTION (this);

  std::cout << "\t" << Simulator::Now ().GetSeconds () << " seconds simulated.\n";
  ScheduleNextProgressReport ();
}

void
GeoTemporalInstaller::Report (const std::string& output_xml_filename)
{
  NS_LOG_FUNCTION (this);

  // The fixed mobility scenario lacks the information to calculate statistics,
  // so don't calculate them.
  if (m_mobility_scenario_id == "fixed")
    return;

  std::cout << "Computing statistics... ";

  SimulationStatisticsFile simulation_stats (m_gta_visitor_vehicles_input_filename,
                                             m_node_id_to_ip);

  Ptr<Node> node;
  Ptr<geotemporal::RoutingProtocol> routing_protocol;
  DataPacketStatistics packet_stats;

  // Initialize SimulationStatisticsFile object with the counter of transmitted
  // packets and the created DATA packets of each node.
  for (uint32_t node_id = 0u; node_id < m_nodes_container.GetN (); ++node_id)
    {
      node = m_nodes_container.Get (node_id);
      routing_protocol = node->GetObject<geotemporal::RoutingProtocol> ();
      NS_ASSERT (node_id == (uint32_t) routing_protocol->GetNodeId ());

      simulation_stats.SetNodeTransmittedPacketsCounter (m_node_id_to_ip.at (node_id),
                                                         routing_protocol->GetTransmittedPacketsCounter ());

      const std::vector<std::pair<geotemporal::DataHeader, Time> > & created_packets
              = routing_protocol->GetCreatedDataPackets ();

      for (std::vector<std::pair<geotemporal::DataHeader, Time> >::const_iterator
        packet_it = created_packets.begin ();
              packet_it != created_packets.end (); ++packet_it)
        {
          packet_stats = DataPacketStatistics (/*Data ID*/ packet_it->first.GetDataIdentifier (),
                                               /*Source node ID*/ node_id,
                                               /*Creation time*/ packet_it->second,
                                               /*Message size*/ packet_it->first.GetMessage ().size (),
                                               /*Data header size*/ packet_it->first.GetSerializedSize () + 1u, // DATA header size + Type header size
                                               /*Destination geo-temporal area*/ packet_it->first.GetDestinationGeoTemporalArea ());

          simulation_stats.AddDataPacket (packet_stats);
        }
    }

  // Load the received packets of each node into the SimulationStatisticsFile object
  for (uint32_t node_id = 0u; node_id < m_nodes_container.GetN (); ++node_id)
    {
      node = m_nodes_container.Get (node_id);
      routing_protocol = node->GetObject<geotemporal::RoutingProtocol> ();
      NS_ASSERT (node_id == (uint32_t) routing_protocol->GetNodeId ());

      const Ipv4Address & receiver_node_ip = m_node_id_to_ip.at (node_id);

      const std::map<DataIdentifier, DataPacketReceptionStats> & received_packets
              = routing_protocol->GetNodePacketReceptionStats ();

      for (std::map<DataIdentifier, DataPacketReceptionStats>::const_iterator
        received_packet_it = received_packets.begin ();
              received_packet_it != received_packets.end (); ++received_packet_it)
        {
          simulation_stats.CountDataPacketReceiverNode (receiver_node_ip,
                                                        received_packet_it->second);
        }
    }

  std::cout << "Done.\n";

  simulation_stats.SaveToXmlFile (output_xml_filename);

  std::cout << "Statistics saved to file \"" << output_xml_filename << "\".\n";
}

} // namespace ns3

