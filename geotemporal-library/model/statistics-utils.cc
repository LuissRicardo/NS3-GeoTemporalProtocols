/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "statistics-utils.h"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <utility>

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                PacketsCounter
// =============================================================================

PacketsCounter::PacketsCounter ()
: m_data_packets_count (0), m_data_packets_size (0),
m_control_packets_count (0), m_control_packets_size (0) { }

PacketsCounter::PacketsCounter (const PacketsCounter & copy)
: m_data_packets_count (copy.m_data_packets_count), m_data_packets_size (copy.m_data_packets_size),
m_control_packets_count (copy.m_control_packets_count), m_control_packets_size (copy.m_control_packets_size) { }

void
PacketsCounter::CountPacket (PacketClass packet_class, uint32_t packet_size)
{
  switch (packet_class)
    {
    case PacketClass::Control:
      ++m_control_packets_count;
      m_control_packets_size += packet_size;
      break;
    case PacketClass::Data:
      ++m_data_packets_count;
      m_data_packets_size += packet_size;
      break;
    }
}

std::string
PacketsCounter::ToString () const
{
  char buffer[30];

  std::sprintf (buffer, "%u", GetCombinedPacketsCount ());
  std::string str = std::string (buffer) + " packet(s): ";

  std::sprintf (buffer, "%u", m_data_packets_count);
  str += "data " + std::string (buffer) + " | control ";

  std::sprintf (buffer, "%u", m_control_packets_count);
  str += std::string (buffer);

  return str;
}

void
PacketsCounter::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                            DataPacketReceptionStats
// =============================================================================

DataPacketReceptionStats::DataPacketReceptionStats ()
: m_received_packet_data_id (), m_transmitter_node_ip (), m_reception_time (),
m_packet_destined_to_receiver_node (false), m_unicast_transmitted_replicas_count (0u),
m_broadcast_transmitted_replicas_count (0u), m_received_duplicates_count (0u),
m_packet_dropped (false) { }

DataPacketReceptionStats::DataPacketReceptionStats (const DataIdentifier & data_id,
                                                    const ns3::Ipv4Address & transmitter_ip,
                                                    const ns3::Time & reception_time,
                                                    bool packet_destined_to_me)
: m_received_packet_data_id (data_id), m_transmitter_node_ip (transmitter_ip),
m_reception_time (reception_time), m_packet_destined_to_receiver_node (packet_destined_to_me),
m_unicast_transmitted_replicas_count (0u), m_broadcast_transmitted_replicas_count (0u),
m_received_duplicates_count (0u), m_packet_dropped (false) { }

DataPacketReceptionStats::DataPacketReceptionStats (const DataPacketReceptionStats & copy)
: m_received_packet_data_id (copy.m_received_packet_data_id),
m_transmitter_node_ip (copy.m_transmitter_node_ip),
m_reception_time (copy.m_reception_time),
m_packet_destined_to_receiver_node (copy.m_packet_destined_to_receiver_node),
m_unicast_transmitted_replicas_count (copy.m_unicast_transmitted_replicas_count),
m_broadcast_transmitted_replicas_count (copy.m_broadcast_transmitted_replicas_count),
m_received_duplicates_count (copy.m_received_duplicates_count),
m_packet_dropped (copy.m_packet_dropped) { }

void
DataPacketReceptionStats::IncrementTransmittedReplicasCount (const TransmissionType & transmission_type)
{
  switch (transmission_type)
    {
    case TransmissionType::Unicast:
      ++m_unicast_transmitted_replicas_count;
      break;
    case TransmissionType::Broadcast:
      ++m_broadcast_transmitted_replicas_count;
      break;
    }
}

std::string
DataPacketReceptionStats::ToString () const
{
  char buffer[30];

  std::string str = "<data-packet-reception data-id=\"" + m_received_packet_data_id.ToString () + "\" ";

  std::sprintf (buffer, "%f", m_reception_time.ToDouble (ns3::Time::S));
  str += "time=\"" + std::string (buffer) + "\" ";

  str += "from-node=\"" + LibraryUtils::ToString (m_transmitter_node_ip) + "\" ";

  if (m_packet_destined_to_receiver_node)
    str += "for-me=\"True\" ";
  else
    str += "for-me=\"False\" ";

  std::sprintf (buffer, "%u", m_received_duplicates_count);
  str += "received-duplicates=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", m_unicast_transmitted_replicas_count);
  str += "unicast-transmitted-replicas=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", m_broadcast_transmitted_replicas_count);
  str += "broadcast-transmitted-replicas=\"" + std::string (buffer) + "\" ";

  if (m_packet_dropped)
    str += "dropped=\"True\" ";
  else
    str += "dropped=\"False\" ";

  str += "/>";
  return str;
}

void
DataPacketReceptionStats::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                                   DataPacketStatistics
// =============================================================================

DataPacketStatistics::DataPacketStatistics ()
: m_data_id (), m_source_node_id (0u), m_packet_creation_time (ns3::Seconds (0.0)),
m_packet_data_size (0u), m_packet_geo_temporal_area (),
m_expected_receiver_nodes_set_flag (false), m_expected_receiver_nodes_gta_arrival_time_mapping (),
m_stats_processed_receiver_nodes_ids_set (), m_stats_confirmed_receiver_nodes_ids_set (),
m_stats_delivery_delay_list () { }

DataPacketStatistics::DataPacketStatistics (const DataIdentifier & data_id, uint32_t source_node_id,
                                            const ns3::Time & creation_time, uint32_t data_size,
                                            const LibraryUtils::GeoTemporalArea & geo_temporal_area)
: m_data_id (data_id), m_source_node_id (source_node_id), m_packet_creation_time (creation_time),
m_packet_data_size (data_size), m_packet_geo_temporal_area (geo_temporal_area),
m_expected_receiver_nodes_set_flag (false), m_expected_receiver_nodes_gta_arrival_time_mapping (),
m_stats_processed_receiver_nodes_ids_set (), m_stats_confirmed_receiver_nodes_ids_set (),
m_stats_delivery_delay_list () { }

DataPacketStatistics::DataPacketStatistics (const DataPacketStatistics & copy)
: m_data_id (copy.m_data_id), m_source_node_id (copy.m_source_node_id),
m_packet_creation_time (copy.m_packet_creation_time),
m_packet_data_size (copy.m_packet_data_size),
m_packet_geo_temporal_area (copy.m_packet_geo_temporal_area),
m_expected_receiver_nodes_set_flag (copy.m_expected_receiver_nodes_set_flag),
m_expected_receiver_nodes_gta_arrival_time_mapping (copy.m_expected_receiver_nodes_gta_arrival_time_mapping),
m_stats_processed_receiver_nodes_ids_set (copy.m_stats_processed_receiver_nodes_ids_set),
m_stats_confirmed_receiver_nodes_ids_set (copy.m_stats_confirmed_receiver_nodes_ids_set),
m_stats_delivery_delay_list (copy.m_stats_delivery_delay_list) { }

void
DataPacketStatistics::ClearExpectedReceiverNodes ()
{
  m_expected_receiver_nodes_set_flag = false;
  m_expected_receiver_nodes_gta_arrival_time_mapping.clear ();

  // Given that the set of expected nodes is being modified, clear previously computed statistics.
  m_stats_processed_receiver_nodes_ids_set.clear ();
  m_stats_confirmed_receiver_nodes_ids_set.clear ();
  m_stats_delivery_delay_list.clear ();
}

void
DataPacketStatistics::SetExpectedReceiverNodes (const std::set<NavigationSystem::VisitorNode> & visitor_nodes_set)
{
  ClearExpectedReceiverNodes ();
  m_expected_receiver_nodes_set_flag = true;

  for (std::set<NavigationSystem::VisitorNode>::const_iterator visitor_node_it = visitor_nodes_set.begin ();
          visitor_node_it != visitor_nodes_set.end (); ++visitor_node_it)
    {
      if (!m_packet_geo_temporal_area.GetTimePeriod ().IsTimeInstantInTimePeriod (visitor_node_it->GetArrivalTime ()))
        {
          // Clear the list of visitor nodes
          ClearExpectedReceiverNodes ();
          throw std::runtime_error ("The node's arrival time to the geo-temporal area is invalid.");
        }

      m_expected_receiver_nodes_gta_arrival_time_mapping.insert (std::make_pair (visitor_node_it->GetNodeId (),
                                                                                 visitor_node_it->GetArrivalTime ()));
    }
}

bool
DataPacketStatistics::CountReception (const uint32_t & receiver_node_id, const DataPacketReceptionStats & reception_stats)
{
  if (!IsExpectedReceiverNodesSet ())
    throw std::runtime_error ("The nodes that visited the packet's geo-temporal area haven't been set.");

  if (m_data_id != reception_stats.GetReceivedPacketDataId ())
    throw std::runtime_error ("The packet's DATA ID doesn't match.");

  // If the given node is not an expected receiver node (i.e., didn't visited the geo-temporal area) then stop.
  std::map<uint32_t, uint32_t>::const_iterator node_arrival_time_it
          = m_expected_receiver_nodes_gta_arrival_time_mapping.find (receiver_node_id);

  if (node_arrival_time_it == m_expected_receiver_nodes_gta_arrival_time_mapping.end ())
    return false;

  // If the given node was processed already then stop.
  if (m_stats_processed_receiver_nodes_ids_set.count (receiver_node_id) == 1u)
    throw std::runtime_error ("Node ID already processed before.");

  // Mark node as processed.
  m_stats_processed_receiver_nodes_ids_set.insert (receiver_node_id);

  // If the packet was dropped then don't count it as a confirmed receiver.
  if (reception_stats.GetPacketDropped ())
    return false;

  // The node is an expected receiver and hasn't been processed yet,
  // add it to the list of confirmed receiver nodes.
  m_stats_confirmed_receiver_nodes_ids_set.insert (receiver_node_id);

  // Calculate delivery delay
  const double start_of_area = (double) std::max (node_arrival_time_it->second,
                                                  m_packet_geo_temporal_area.GetTimePeriod ().GetStartTime ());
  const double reception_time = reception_stats.GetReceptionTime ().ToDouble (ns3::Time::S);

  // If the packet was received before the node's area arrival time then there was no delay
  if (reception_time <= start_of_area)
    m_stats_delivery_delay_list.push_back (0.0);
  else
    m_stats_delivery_delay_list.push_back (reception_time - start_of_area);

  return true;
}

void
DataPacketStatistics::GetStatistics (uint32_t & confirmed_packet_receivers_count, uint32_t & expected_packet_receivers_count,
                                     double & packet_delivery_ratio, double & packet_average_delivery_delay,
                                     uint32_t & delivered_data_bytes) const
{
  if (m_stats_confirmed_receiver_nodes_ids_set.size () != m_stats_delivery_delay_list.size ())
    throw std::runtime_error ("Unexpected error: list of confirmed receivers and list of delivery delays must have "
                              "the same number of items.");

  if (m_stats_confirmed_receiver_nodes_ids_set.size () > m_expected_receiver_nodes_gta_arrival_time_mapping.size ())
    throw std::runtime_error ("Unexpected error: the length of the list of confirmed receivers shouldn't be greater "
                              "than the number of expected receivers.");

  // Number of nodes expected to receive the packet.
  expected_packet_receivers_count = GetExpectedReceiverNodesCount ();

  // Number of nodes that actually received the packet.
  confirmed_packet_receivers_count = GetConfirmedReceiverNodesCount ();

  // Sum of the size (in bytes) of all DATA packets successfully delivered to destination nodes.
  delivered_data_bytes = confirmed_packet_receivers_count * (m_packet_data_size
                                                             + 72 // Other fields of geo-temporal protocol's DATA packet's header
                                                             + 24 + 8 // IEEE 802.11 header + LCC header
                                                             + 20 + 8 // + IP header + UDP header
                                                             + 4 + 1); // Frame end + Terminator.

  if (expected_packet_receivers_count == 0u)
    {
      // No node should receive the packet.
      packet_delivery_ratio = -1.0;
      packet_average_delivery_delay = -1.0;
      return;
    }

  // At least one node should have received the packet.
  if (confirmed_packet_receivers_count == 0u)
    {
      // At least one node should have received the packet but no node received it.
      packet_delivery_ratio = 0.0;
      packet_average_delivery_delay = -1.0;
      return;
    }

  // At least one node should have received the packet and at least one node actually received it.

  // Packet delivery ratio: confirmed receivers / expected receivers
  packet_delivery_ratio = ((double) confirmed_packet_receivers_count) / ((double) expected_packet_receivers_count);

  // Average delivery delay of all input receptions
  double sum = 0.0;

  for (std::vector<double>::const_iterator delivery_delay_it = m_stats_delivery_delay_list.begin ();
          delivery_delay_it != m_stats_delivery_delay_list.end (); ++delivery_delay_it)
    {
      sum += *delivery_delay_it;
    }

  packet_average_delivery_delay = sum / ((double) confirmed_packet_receivers_count);
}

std::string
DataPacketStatistics::ToString () const
{
  char buffer[30];

  std::string str = "<data-packet data-id=\"" + m_data_id.ToString () + "\" ";

  str += "source-node-ip=\"" + LibraryUtils::ToString (m_data_id.GetSourceIp ()) + "\" ";

  std::sprintf (buffer, "%u", m_source_node_id);
  str += "source-node-id=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%f", m_packet_creation_time.ToDouble (ns3::Time::S));
  str += "creation-time=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%f", m_packet_geo_temporal_area.GetArea ().GetX1 ());
  str += "destination-area=\"" + std::string (buffer) + ",";

  std::sprintf (buffer, "%f", m_packet_geo_temporal_area.GetArea ().GetY1 ());
  str += std::string (buffer) + ", ";

  std::sprintf (buffer, "%f", m_packet_geo_temporal_area.GetArea ().GetX2 ());
  str += std::string (buffer) + ",";

  std::sprintf (buffer, "%f", m_packet_geo_temporal_area.GetArea ().GetY2 ());
  str += std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", m_packet_geo_temporal_area.GetTimePeriod ().GetStartTime ());
  str += "initial-time=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", m_packet_geo_temporal_area.GetTimePeriod ().GetDuration ());
  str += "duration=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", m_packet_data_size);
  str += "data-size=\"" + std::string (buffer) + "\" ";

  str += "/>";
  return str;
}

void
DataPacketStatistics::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                              SimulationStatistics
// =============================================================================

SimulationStatistics::SimulationStatistics ()
: m_nodes_transmitted_packets_counters (), m_data_packets_statistics (),
m_gta_visitor_nodes (), m_known_destination_areas () { }

SimulationStatistics::SimulationStatistics (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes)
: m_nodes_transmitted_packets_counters (), m_data_packets_statistics (),
m_gta_visitor_nodes (gta_visitor_nodes), m_known_destination_areas () { }

SimulationStatistics::SimulationStatistics (const SimulationStatistics & copy)
: m_nodes_transmitted_packets_counters (copy.m_nodes_transmitted_packets_counters),
m_data_packets_statistics (copy.m_data_packets_statistics),
m_gta_visitor_nodes (copy.m_gta_visitor_nodes),
m_known_destination_areas (copy.m_known_destination_areas) { }

const DataPacketStatistics &
SimulationStatistics::GetDataPacketStatistics (const DataIdentifier & packet_data_id) const
{
  std::map<DataIdentifier, DataPacketStatistics>::const_iterator packet_stats_it
          = m_data_packets_statistics.find (packet_data_id);

  if (packet_stats_it == m_data_packets_statistics.end ())
    throw std::out_of_range ("Error: the packet with ID '" + packet_data_id.ToString ()
                             + "' is unknown.");

  return packet_stats_it->second;
}

void
SimulationStatistics::AddDataPacket (DataPacketStatistics & packet_statistics)
{
  const LibraryUtils::GeoTemporalArea & destination_geo_temporal_area
          = packet_statistics.GetPacketDestinationGeoTemporalArea ();

  if (!m_gta_visitor_nodes.ContainsGeoTemporalArea (destination_geo_temporal_area))
    throw std::runtime_error ("The given data packet has a destination geo-temporal area that "
                              "is unknown to the GeoTemporalAreasVisitorNodes object.");

  if (m_data_packets_statistics.count (packet_statistics.GetDataIdentifier ()) > 0u)
    throw std::runtime_error ("The given DataPacketStatistics already exists.");

  packet_statistics.SetExpectedReceiverNodes (m_gta_visitor_nodes.GetGeoTemporalAreaVisitorNodes (destination_geo_temporal_area));

  m_data_packets_statistics.insert (std::make_pair (packet_statistics.GetDataIdentifier (),
                                                    packet_statistics));

  m_known_destination_areas.insert (packet_statistics.GetPacketDestinationGeoTemporalArea ().GetArea ());
}

bool
SimulationStatistics::CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                                                   const uint32_t receiver_node_id,
                                                   const DataPacketReceptionStats & reception_stats)
{
  std::map<DataIdentifier, DataPacketStatistics>::iterator packet_statistics_it
          = m_data_packets_statistics.find (reception_stats.GetReceivedPacketDataId ());

  if (packet_statistics_it == m_data_packets_statistics.end ())
    throw std::runtime_error ("The specified packet doesn't exist. Add it using AddDataPacket "
                              "(const DataPacketStatistics &) function.");

  return packet_statistics_it->second.CountReception (receiver_node_id, reception_stats);
}

void
SimulationStatistics::SetNodeTransmittedPacketsCounter (const ns3::Ipv4Address & node_ip,
                                                        const uint32_t node_id,
                                                        const PacketsCounter & node_tx_packets_counter)
{
  m_nodes_transmitted_packets_counters[node_id] = node_tx_packets_counter;
}

bool
SimulationStatistics::GetStatistics (double & average_delivery_delay, double & average_delivery_ratio,
                                     double & total_overhead, double & data_overhead,
                                     double & control_overhead, uint64_t & total_transmitted_bytes,
                                     uint64_t & data_transmitted_bytes, uint64_t & control_transmitted_bytes,
                                     uint64_t & total_delivered_data_bytes, uint32_t & expected_receivers,
                                     uint32_t & confirmed_receivers) const
{
  if (m_data_packets_statistics.empty ())
    {
      average_delivery_delay = -1.0;
      average_delivery_ratio = -1.0;
      total_overhead = -1.0;
      data_overhead = -1.0;
      control_overhead = -1.0;
      total_transmitted_bytes = 0u;
      data_transmitted_bytes = 0u;
      control_transmitted_bytes = 0u;
      total_delivered_data_bytes = 0u;
      expected_receivers = 0u;
      confirmed_receivers = 0u;
      return false;
    }

  // Current packet output statistics
  uint32_t current_packet_confirmed_packet_receivers_count, current_packet_expected_packet_receivers_count,
          current_packet_delivered_data_bytes;
  double current_packet_packet_delivery_ratio, current_packet_packet_average_delivery_delay;

  // Initialize counters
  total_delivered_data_bytes = 0u;
  expected_receivers = 0u;
  confirmed_receivers = 0u;

  std::vector<double> average_delivery_delay_list;
  std::vector<double> delivery_ratio_list;

  for (std::map<DataIdentifier, DataPacketStatistics>::const_iterator packet_statistics_it =
          m_data_packets_statistics.begin ();
          packet_statistics_it != m_data_packets_statistics.end (); ++packet_statistics_it)
    {
      packet_statistics_it->second.GetStatistics (current_packet_confirmed_packet_receivers_count,
                                                  current_packet_expected_packet_receivers_count,
                                                  current_packet_packet_delivery_ratio,
                                                  current_packet_packet_average_delivery_delay,
                                                  current_packet_delivered_data_bytes);

      total_delivered_data_bytes += current_packet_delivered_data_bytes;
      expected_receivers += current_packet_expected_packet_receivers_count;
      confirmed_receivers += current_packet_confirmed_packet_receivers_count;

      if (current_packet_packet_average_delivery_delay >= 0.0)
        average_delivery_delay_list.push_back (current_packet_packet_average_delivery_delay);

      if (current_packet_packet_delivery_ratio >= 0.0)
        delivery_ratio_list.push_back (current_packet_packet_delivery_ratio);
    }

  // Compute transmitted bytes
  total_transmitted_bytes = 0u;
  data_transmitted_bytes = 0u;
  control_transmitted_bytes = 0u;

  for (std::map<uint32_t, PacketsCounter>::const_iterator node_transmitted_packets_it =
          m_nodes_transmitted_packets_counters.begin ();
          node_transmitted_packets_it != m_nodes_transmitted_packets_counters.end ();
          ++node_transmitted_packets_it)
    {
      total_transmitted_bytes += node_transmitted_packets_it->second.GetCombinedPacketsSize ();
      data_transmitted_bytes += node_transmitted_packets_it->second.GetDataPacketsSize ();
      control_transmitted_bytes += node_transmitted_packets_it->second.GetControlPacketsSize ();
    }

  // Compute overheads
  total_overhead = (double) total_transmitted_bytes / (double) total_delivered_data_bytes;
  data_overhead = (double) data_transmitted_bytes / (double) total_delivered_data_bytes;
  control_overhead = (double) control_transmitted_bytes / (double) total_delivered_data_bytes;

  // Compute average delivery delay
  double sum = 0.0;

  if (average_delivery_delay_list.empty ())
    {
      average_delivery_delay = -1.0;
    }
  else
    {
      for (std::vector<double>::const_iterator delivery_delay_it = average_delivery_delay_list.begin ();
              delivery_delay_it != average_delivery_delay_list.end (); ++delivery_delay_it)
        {
          sum += *delivery_delay_it;
        }

      average_delivery_delay = sum / (double) average_delivery_delay_list.size ();
    }

  // Compute average delivery ratio
  sum = 0.0;

  if (delivery_ratio_list.empty ())
    {
      average_delivery_ratio = -1.0;
    }
  else
    {
      for (std::vector<double>::const_iterator delivery_ratio_it = delivery_ratio_list.begin ();
              delivery_ratio_it != delivery_ratio_list.end (); ++delivery_ratio_it)
        {
          sum += *delivery_ratio_it;
        }

      average_delivery_ratio = sum / (double) delivery_ratio_list.size ();
    }

  return true;
}

bool
SimulationStatistics::GetAreaStatistics (const LibraryUtils::Area & destination_area,
                                         double & average_delivery_delay, double & average_delivery_ratio,
                                         uint64_t & total_delivered_data_bytes, uint32_t & expected_receivers,
                                         uint32_t & confirmed_receivers) const
{
  if (m_data_packets_statistics.empty () || m_known_destination_areas.count (destination_area) == 0u)
    {
      average_delivery_delay = -1.0;
      average_delivery_ratio = -1.0;
      total_delivered_data_bytes = 0u;
      expected_receivers = 0u;
      confirmed_receivers = 0u;
      return false;
    }

  // Current packet output statistics
  uint32_t current_packet_confirmed_packet_receivers_count, current_packet_expected_packet_receivers_count,
          current_packet_delivered_data_bytes;
  double current_packet_packet_delivery_ratio, current_packet_packet_average_delivery_delay;

  // Initialize counters
  total_delivered_data_bytes = 0u;
  expected_receivers = 0u;
  confirmed_receivers = 0u;

  std::vector<double> average_delivery_delay_list;
  std::vector<double> delivery_ratio_list;

  for (std::map<DataIdentifier, DataPacketStatistics>::const_iterator packet_statistics_it =
          m_data_packets_statistics.begin ();
          packet_statistics_it != m_data_packets_statistics.end (); ++packet_statistics_it)
    {
      if (destination_area != packet_statistics_it->second.GetPacketDestinationGeoTemporalArea ().GetArea ())
        continue;

      packet_statistics_it->second.GetStatistics (current_packet_confirmed_packet_receivers_count,
                                                  current_packet_expected_packet_receivers_count,
                                                  current_packet_packet_delivery_ratio,
                                                  current_packet_packet_average_delivery_delay,
                                                  current_packet_delivered_data_bytes);

      total_delivered_data_bytes += current_packet_delivered_data_bytes;
      expected_receivers += current_packet_expected_packet_receivers_count;
      confirmed_receivers += current_packet_confirmed_packet_receivers_count;

      if (current_packet_packet_average_delivery_delay >= 0.0)
        average_delivery_delay_list.push_back (current_packet_packet_average_delivery_delay);

      if (current_packet_packet_delivery_ratio >= 0.0)
        delivery_ratio_list.push_back (current_packet_packet_delivery_ratio);
    }

  // Compute average delivery delay
  double sum = 0.0;

  if (average_delivery_delay_list.empty ())
    {
      average_delivery_delay = -1.0;
    }
  else
    {
      for (std::vector<double>::const_iterator delivery_delay_it = average_delivery_delay_list.begin ();
              delivery_delay_it != average_delivery_delay_list.end (); ++delivery_delay_it)
        {
          sum += *delivery_delay_it;
        }

      average_delivery_delay = sum / (double) average_delivery_delay_list.size ();
    }

  // Compute average delivery ratio
  sum = 0.0;

  if (delivery_ratio_list.empty ())
    {
      average_delivery_ratio = -1.0;
    }
  else
    {
      for (std::vector<double>::const_iterator delivery_ratio_it = delivery_ratio_list.begin ();
              delivery_ratio_it != delivery_ratio_list.end (); ++delivery_ratio_it)
        {
          sum += *delivery_ratio_it;
        }

      average_delivery_ratio = sum / (double) delivery_ratio_list.size ();
    }

  return true;
}


// =============================================================================
//                         SimulationStatisticsFile
// =============================================================================

SimulationStatisticsFile::SimulationStatisticsFile ()
: m_data_packets_str_section () { }

SimulationStatisticsFile::SimulationStatisticsFile (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes)
: SimulationStatistics (gta_visitor_nodes), m_data_packets_str_section () { }

SimulationStatisticsFile::SimulationStatisticsFile (const SimulationStatisticsFile & copy)
: SimulationStatistics (copy),
m_data_packets_str_section (copy.m_data_packets_str_section) { }

void
SimulationStatisticsFile::AddDataPacket (DataPacketStatistics & packet_statistics)
{
  SimulationStatistics::AddDataPacket (packet_statistics);

  m_data_packets_str_section.insert (std::make_pair (packet_statistics.GetDataIdentifier (),
                                                     std::string ()));
}

bool
SimulationStatisticsFile::CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                                                       const uint32_t receiver_node_id,
                                                       const DataPacketReceptionStats & reception_stats)
{
  const bool expected_receiver
          = SimulationStatistics::CountDataPacketReceiverNode (receiver_node_ip,
                                                               receiver_node_id,
                                                               reception_stats);

  if (expected_receiver == false)
    return expected_receiver; // Returns false.

  char buffer[30];
  const std::string indentation = "  ";
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();

  std::sprintf (buffer, "%u", receiver_node_id);
  std::string reception_str = indentation + indentation
          + "<receiver-node "
          + "node-ip=\"" + LibraryUtils::ToString (receiver_node_ip) + "\" "
          + "node-id=\"" + std::string (buffer) + "\" ";

  reception_str += "received-from=\""
          + LibraryUtils::ToString (reception_stats.GetTransmitterIpAddress ()) + "\" ";

  std::sprintf (buffer, "%f", reception_stats.GetReceptionTime ().ToDouble (ns3::Time::S));
  reception_str += "time=\"" + std::string (buffer) + "\" ";

  if (reception_stats.GetPacketDestinedToReceiverNode ())
    reception_str += "intended-receiver=\"True\" ";
  else
    reception_str += "intended-receiver=\"False\" ";

  std::sprintf (buffer, "%u", reception_stats.GetUnicastTransmittedReplicasCount ());
  reception_str += "unicast-transmitted-replicas=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", reception_stats.GetBroadcastTransmittedReplicasCount ());
  reception_str += "broadcast-transmitted-replicas=\"" + std::string (buffer) + "\" ";

  std::sprintf (buffer, "%u", reception_stats.GetReceivedDuplicatesCount ());
  reception_str += "received-duplicates=\"" + std::string (buffer) + "\" />" + end_line;

  m_data_packets_str_section[reception_stats.GetReceivedPacketDataId ()] += reception_str;

  return expected_receiver; // Returns true
}

void
SimulationStatisticsFile::SetNodeTransmittedPacketsCounter (const ns3::Ipv4Address& node_ip,
                                                            const uint32_t node_id,
                                                            const PacketsCounter& node_tx_packets_counter)
{
  SimulationStatistics::SetNodeTransmittedPacketsCounter (node_ip, node_id,
                                                          node_tx_packets_counter);
  m_node_tx_packets_counter_ips.insert (std::make_pair (node_id, node_ip));
}

void
SimulationStatisticsFile::SaveToXmlFile (const std::string & output_filename) const
{
  const std::string output_filename_trimmed = LibraryUtils::Trim_Copy (output_filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();
  const std::string indentation = "  ";
  char buffer[40];
  uint32_t current_node_id;

  if (output_filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream output_file (output_filename_trimmed, std::ios::out);

  if (!output_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + output_filename_trimmed + "\".");

  // Print XML prolog
  output_file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << end_line << end_line;

  output_file << "<document>" << end_line << end_line;

  output_file << "<statistics-results>" << end_line;

  // Print general statistics
  double average_delivery_delay, average_delivery_ratio, total_overhead, data_overhead,
          control_overhead;
  uint64_t total_transmitted_bytes, data_transmitted_bytes, control_transmitted_bytes,
          total_delivered_data_bytes;
  uint32_t expected_receivers, confirmed_receivers;

  GetStatistics (average_delivery_delay, average_delivery_ratio, total_overhead, data_overhead,
                 control_overhead, total_transmitted_bytes, data_transmitted_bytes,
                 control_transmitted_bytes, total_delivered_data_bytes, expected_receivers,
                 confirmed_receivers);

  std::sprintf (buffer, "%f", average_delivery_ratio);
  output_file << indentation << "<result type=\"total\" "
          << "average-delivery-ratio=\"" << buffer << "\" ";

  std::sprintf (buffer, "%f", average_delivery_delay);
  output_file << "average-delivery-delay=\"" << buffer << "\" ";

  std::sprintf (buffer, "%f", total_overhead);
  output_file << "total-overhead=\"" << buffer << "\" ";

  std::sprintf (buffer, "%f", data_overhead);
  output_file << "data-overhead=\"" << buffer << "\" ";

  std::sprintf (buffer, "%f", control_overhead);
  output_file << "control-overhead=\"" << buffer << "\" ";

  std::sprintf (buffer, "%lu", total_transmitted_bytes);
  output_file << "total-transmitted-bytes=\"" << buffer << "\" ";

  std::sprintf (buffer, "%lu", data_transmitted_bytes);
  output_file << "data-transmitted-bytes=\"" << buffer << "\" ";

  std::sprintf (buffer, "%lu", control_transmitted_bytes);
  output_file << "control-transmitted-bytes=\"" << buffer << "\" ";

  std::sprintf (buffer, "%lu", total_delivered_data_bytes);
  output_file << "total-delivered-data-bytes=\"" << buffer << "\" ";

  std::sprintf (buffer, "%u", expected_receivers);
  output_file << "expected-receiver-nodes=\"" << buffer << "\" ";

  std::sprintf (buffer, "%u", confirmed_receivers);
  output_file << "confirmed-receiver-nodes=\"" << buffer << "\" ";
  output_file << "/>" << end_line;

  // Print area statistics
  for (std::set<LibraryUtils::Area>::const_iterator area_it = m_known_destination_areas.begin ();
          area_it != m_known_destination_areas.end (); ++area_it)
    {
      GetAreaStatistics (*area_it, average_delivery_delay, average_delivery_ratio,
                         total_delivered_data_bytes, expected_receivers, confirmed_receivers);

      std::sprintf (buffer, "%f", area_it->GetX1 ());
      output_file << indentation << "<result type=\"area\" "
              << "area=\"" << buffer << ",";

      std::sprintf (buffer, "%f", area_it->GetY1 ());
      output_file << buffer << ", ";

      std::sprintf (buffer, "%f", area_it->GetX2 ());
      output_file << buffer << ",";

      std::sprintf (buffer, "%f", area_it->GetY2 ());
      output_file << buffer << "\" ";

      std::sprintf (buffer, "%f", average_delivery_ratio);
      output_file << "average-delivery-ratio=\"" << buffer << "\" ";

      std::sprintf (buffer, "%f", average_delivery_delay);
      output_file << "average-delivery-delay=\"" << buffer << "\" ";

      std::sprintf (buffer, "%lu", total_delivered_data_bytes);
      output_file << "total-delivered-data-bytes=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", expected_receivers);
      output_file << "expected-receiver-nodes=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", confirmed_receivers);
      output_file << "confirmed-receiver-nodes=\"" << buffer << "\" ";
      output_file << "/>" << end_line;
    }

  output_file << "</statistics-results>" << end_line << end_line;

  // Print transmission counters
  output_file << "<nodes-transmitted-packets>" << end_line;

  for (std::map<uint32_t, PacketsCounter>::const_iterator node_packets_counter_it
          = m_nodes_transmitted_packets_counters.begin ();
          node_packets_counter_it != m_nodes_transmitted_packets_counters.end ();
          ++node_packets_counter_it)
    {
      output_file << indentation << "<node-transmitted-packets ";

      current_node_id = node_packets_counter_it->first;
      const ns3::Ipv4Address & current_node_ip = m_node_tx_packets_counter_ips.at (current_node_id);

      output_file << "node-ip=\"" << LibraryUtils::ToString (current_node_ip) << "\" ";

      std::sprintf (buffer, "%u", current_node_id);
      output_file << "node-id=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", node_packets_counter_it->second.GetControlPacketsCount ());
      output_file << "control-packets-count=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", node_packets_counter_it->second.GetControlPacketsSize ());
      output_file << "control-packets-size=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", node_packets_counter_it->second.GetDataPacketsCount ());
      output_file << "data-packets-count=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", node_packets_counter_it->second.GetDataPacketsSize ());
      output_file << "data-packets-size=\"" << buffer << "\" ";
      output_file << "/>" << end_line;
    }

  output_file << "</nodes-transmitted-packets>" << end_line << end_line;

  // Print data packets receiver nodes
  output_file << "<data-packets>" << end_line;

  uint32_t confirmed_packet_receivers_count, expected_packet_receivers_count;
  double packet_delivery_ratio, packet_average_delivery_delay;
  uint32_t delivered_data_bytes;

  for (std::map<DataIdentifier, DataPacketStatistics>::const_iterator data_packet_stats_it
          = m_data_packets_statistics.begin ();
          data_packet_stats_it != m_data_packets_statistics.end (); ++data_packet_stats_it)
    {
      output_file << indentation << "<data-packet data-id=\""
              << data_packet_stats_it->first.ToString () << "\" ";

      output_file << "source-node-ip=\""
              + LibraryUtils::ToString (data_packet_stats_it->second.GetSourceNodeIp ()) + "\" ";

      std::sprintf (buffer, "%u", data_packet_stats_it->second.GetSourceNodeId ());
      output_file << "source-node-id=\"" << buffer << "\" ";

      const LibraryUtils::GeoTemporalArea & geo_temporal_area = data_packet_stats_it->second
              .GetPacketDestinationGeoTemporalArea ();

      std::sprintf (buffer, "%f", geo_temporal_area.GetArea ().GetX1 ());
      output_file << "destination-area=\"" << buffer << ",";

      std::sprintf (buffer, "%f", geo_temporal_area.GetArea ().GetY1 ());
      output_file << buffer << ", ";

      std::sprintf (buffer, "%f", geo_temporal_area.GetArea ().GetX2 ());
      output_file << buffer << ",";

      std::sprintf (buffer, "%f", geo_temporal_area.GetArea ().GetY2 ());
      output_file << buffer << "\" ";

      std::sprintf (buffer, "%u", geo_temporal_area.GetTimePeriod ().GetStartTime ());
      output_file << "temporal-scope=\"" << buffer << ",";

      std::sprintf (buffer, "%u", geo_temporal_area.GetTimePeriod ().GetEndTime ());
      output_file << buffer << "\" ";

      std::sprintf (buffer, "%f", data_packet_stats_it->second.GetPacketCreationTime ().ToDouble (ns3::Time::S));
      output_file << "creation-time=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", data_packet_stats_it->second.GetPacketDataSize ());
      output_file << "data-size=\"" << buffer << "\" ";

      data_packet_stats_it->second.GetStatistics (confirmed_packet_receivers_count,
                                                  expected_packet_receivers_count,
                                                  packet_delivery_ratio,
                                                  packet_average_delivery_delay,
                                                  delivered_data_bytes);

      std::sprintf (buffer, "%u", confirmed_packet_receivers_count);
      output_file << "confirmed-receiver-nodes=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", expected_packet_receivers_count);
      output_file << "expected-receiver-nodes=\"" << buffer << "\" ";

      std::sprintf (buffer, "%f", packet_delivery_ratio);
      output_file << "delivery-ratio=\"" << buffer << "\" ";

      std::sprintf (buffer, "%f", packet_average_delivery_delay);
      output_file << "delivery-delay=\"" << buffer << "\" ";

      std::sprintf (buffer, "%u", delivered_data_bytes);
      output_file << "delivered-data-bytes=\"" << buffer << "\"";

      if (confirmed_packet_receivers_count == 0u)
        {
          output_file << " />" << end_line << end_line;
        }
      else
        {
          output_file << ">" << end_line;

          // Print confirmed receivers.
          output_file << m_data_packets_str_section.at (data_packet_stats_it->first);

          output_file << indentation << "</data-packet>" << end_line << end_line;
        }
    }

  output_file << "</data-packets>" << end_line << end_line;

  output_file << "</document>" << end_line;

  output_file.close ();
}

}
}
