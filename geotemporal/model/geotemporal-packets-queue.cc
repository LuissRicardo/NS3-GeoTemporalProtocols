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
 * Author: Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>
 * 
 * Data packets queue of the Geo-Temporal protocol.
 */

#include "geotemporal-packets-queue.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iterator>
#include <utility>

#include <ns3/log.h>
#include <ns3/simulator.h>


NS_LOG_COMPONENT_DEFINE ("GeoTemporalPacketsQueue");

namespace ns3
{
namespace geotemporal
{

// =============================================================================
//                                PacketQueueEntry
// =============================================================================

PacketQueueEntry::PacketQueueEntry ()
: m_data_packet (), m_expiration_time (), m_replicas_counter (0u),
m_known_carriers () { }

PacketQueueEntry::PacketQueueEntry (const DataHeader& data_packet,
                                    const uint16_t replicas_counter)
: m_data_packet (data_packet), m_expiration_time (), m_replicas_counter (replicas_counter),
m_known_carriers ()
{
  SetExpirationTime (m_data_packet.GetDestinationGeoTemporalArea ().GetTimePeriod ());
}

PacketQueueEntry::PacketQueueEntry (const PacketQueueEntry& copy)
: m_data_packet (copy.m_data_packet),
m_expiration_time (copy.m_expiration_time),
m_replicas_counter (copy.m_replicas_counter),
m_known_carriers (copy.m_known_carriers) { }

void
PacketQueueEntry::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
PacketQueueEntry::ToString () const
{
  char buffer[15];
  std::sprintf (buffer, "%.2f", m_expiration_time.ToDouble (Time::S));

  std::string str = "Packet queue entry " + m_data_packet.GetDataIdentifier ().ToString ()
          + " will expire at second " + std::string (buffer) + ", is known by ";

  std::sprintf (buffer, "%d", (uint32_t) m_known_carriers.size ());
  str += std::string (buffer) + " nodes and has ";

  std::sprintf (buffer, "%d", (uint32_t) m_replicas_counter);
  str += std::string (buffer) + " replicas available.";
  return str;
}


// =============================================================================
//                                 PacketsQueue
// =============================================================================

PacketsQueue::PacketsQueue () : PacketsQueue (0, 128u, 3u) { }

PacketsQueue::PacketsQueue (Ptr<GpsSystem> gps_system, uint32_t maximum_queue_length,
                            uint32_t default_replicas_counter)
: m_gps (gps_system), m_max_queue_length (maximum_queue_length),
m_max_replicas_counter (default_replicas_counter), m_packets_dropped_counter (0u),
m_min_vehicles_distance_diff (20), m_packets_table (),
m_data_packet_reception_stats () { }

PacketsQueue::PacketsQueue (const PacketsQueue& copy)
: m_gps (copy.m_gps),
m_max_queue_length (copy.m_max_queue_length),
m_max_replicas_counter (copy.m_max_replicas_counter),
m_packets_dropped_counter (copy.m_packets_dropped_counter),
m_min_vehicles_distance_diff (copy.m_min_vehicles_distance_diff),
m_packets_table (copy.m_packets_table),
m_data_packet_reception_stats (copy.m_data_packet_reception_stats) { }


// --------------------------
// Getters & Setters
// --------------------------

uint32_t
PacketsQueue::Size () const
{
  return m_packets_table.size ();
}

void
PacketsQueue::GetSummaryVector (std::set<DataIdentifier>& summary_vector) const
{
  summary_vector.clear ();

  for (ConstIterator_t entry_it = m_packets_table.begin ();
          entry_it != m_packets_table.end (); ++entry_it)
    {
      summary_vector.insert (entry_it->first);
    }
}



// --------------------------
// Lookup
// --------------------------

bool
PacketsQueue::Find (const DataIdentifier& data_packet_id, PacketQueueEntry& entry) const
{
  NS_LOG_FUNCTION (this << data_packet_id);

  ConstIterator_t entry_it = m_packets_table.find (data_packet_id);

  if (entry_it == m_packets_table.end ())
    {
      NS_LOG_DEBUG ("Packet entry " << data_packet_id << " NOT found.");
      return false;
    }

  entry = entry_it->second;
  NS_LOG_DEBUG ("Packet entry " << data_packet_id << " found: " << entry);
  return true;
}

bool
PacketsQueue::Find (const DataIdentifier& data_packet_id) const
{
  PacketQueueEntry entry;
  return Find (data_packet_id, entry);
}

bool
PacketsQueue::Find (const PacketQueueEntry& packet_entry) const
{
  PacketQueueEntry e;
  return Find (packet_entry.GetDataPacketId (), e);
}


// --------------------------
// Modifiers
// --------------------------

void
PacketsQueue::ProcessDisjointVectors (const std::set<DataIdentifier>& summary_vector,
                                      const Ipv4Address& local_ip,
                                      const Ipv4Address& neighbor_ip,
                                      std::set<DataIdentifier>& local_unknown_packets,
                                      std::set<DataIdentifier>& neighbor_unknown_packets) const
{
  NS_LOG_FUNCTION (this << local_ip << neighbor_ip);

  local_unknown_packets.clear ();
  neighbor_unknown_packets.clear ();

  /* For each packet in the received summary vector check if it is contained in 
   * this packets queue. If it doesn't it means that the local node does NOT
   * know the packet, so it's added to the local_unknown_packets set (the set of
   * packets that the local node doesn't know).
   * 
   * It also checks that the local node is not the source of the packet. */

  for (std::set<DataIdentifier>::const_iterator it = summary_vector.begin ();
          it != summary_vector.end (); ++it)
    {
      // If the local node doesn't know the packet and the local node is not the
      // source node, then add it to the local_unknown_packets set.
      if (m_packets_table.find (*it) == m_packets_table.end ()
          && it->GetSourceIp () != local_ip)
        {
          local_unknown_packets.insert (*it);
        }
    }

  /* For each packet contained in this packets queue check if it exists in the
   * received summary vector. If it doesn't it means that the neighbor node 
   * doesn't know it, so it's added to the neighbor_unknown_packets (the set of
   * packets that the neighbor node doesn't know).
   * 
   * Also checks that the neighbor node is not the source of the packet. */
  for (ConstIterator_t it = m_packets_table.begin ();
          it != m_packets_table.end (); ++it)
    {
      // If the neighbor node doesn't know the packet and the neighbor node is not
      // the source node, then add it to the neighbor_unknown_packets set.
      if (summary_vector.find (it->first) == summary_vector.end ()
          && it->first.GetSourceIp () != neighbor_ip)
        {
          neighbor_unknown_packets.insert (it->first);
        }
    }
}

bool
PacketsQueue::ComparePacketTransmissionPriority (const std::map<DataIdentifier, PacketQueueEntry>::value_type& entry_1,
                                                 const std::map<DataIdentifier, PacketQueueEntry>::value_type& entry_2)
{
  const DataHeader & packet_1 = entry_1.second.GetDataPacket ();
  const DataHeader & packet_2 = entry_2.second.GetDataPacket ();

  /* The first criteria to select the packet with higher priority to be dequeued
   * is the hops count.
   * 
   * This way, the packet that has traveled less is transmitted and given the 
   * opportunity to travel.
   */
  if (packet_1.GetHopsCount () != packet_2.GetHopsCount ())
    {
      // If packet 1 has traveled MORE hops than packet 2 then it has LOWER 
      // priority, return true. Otherwise return false because packet 1 has 
      // traveled LESS hops than packet 2.
      return packet_1.GetHopsCount () > packet_2.GetHopsCount ();
    }

  /* Both have traveled an equal number of hops, the criteria to break the tie
   * is the number of nodes that know the packets.
   * 
   * The packet that is known by MORE number of nodes has LOWER priority to be dequeued.
   * The packet that is known by LESS or EQUAL number of nodes has HIGHER priority to be dequeued.
   */
  return entry_1.second.GetKnownCarrierNodesCount () > entry_2.second.GetKnownCarrierNodesCount ();
}

bool
PacketsQueue::FindHighestDropPriorityPacket (const Vector2D& current_node_position,
                                             DataIdentifier& selected_packet) const
{
  NS_LOG_FUNCTION (this << current_node_position);

  const uint32_t packets_queue_size = Size ();

  if (packets_queue_size == 0u)
    {
      NS_LOG_DEBUG ("Queue is empty, no packets to drop.");
      return false;
    }

  // Trivial case, only one packet in the queue
  if (packets_queue_size == 1u)
    {
      selected_packet = m_packets_table.begin ()->first;
      NS_LOG_DEBUG ("Trivial case: the only packet in queue is selected: "
                    << selected_packet);
      return true;
    }

  /* We drop the packet with the lowest transmission priority. So the packet
   * with the lowest transmission priority also has the highest drop priority.
   * 
   * We use ComparePacketTransmissionPriority (A, B) that returns true if A has
   * lower transmission priority than B. Because another way to look at it is
   * that A has higher drop priority than B.
   * 
   * If possible, try to find a regular packet that is outside its destination
   * GTA and has the lowest possible transmission priority.
   */

  const Time current_time = Simulator::Now ();

  ConstIterator_t lowest_priority_emergency_pkt_it = m_packets_table.end ();
  ConstIterator_t lowest_priority_emergency_pkt_not_in_gta_it = m_packets_table.end ();

  ConstIterator_t lowest_priority_regular_pkt_it = m_packets_table.end ();
  ConstIterator_t lowest_priority_regular_pkt_not_in_gta_it = m_packets_table.end ();

  for (ConstIterator_t packet_entry_it = m_packets_table.begin ();
          packet_entry_it != m_packets_table.end (); ++packet_entry_it)
    {
      const DataHeader & current_data_packet = packet_entry_it->second.GetDataPacket ();
      const bool outside_gta = !current_data_packet.GetDestinationGeoTemporalArea ()
              .IsInsideGeoTemporalArea (current_node_position, current_time);

      if (current_data_packet.IsEmergencyPacket ())
        {
          // If the current emergency data packet is the one with the lowest 
          // transmission priority then select it.
          if (lowest_priority_emergency_pkt_it == m_packets_table.end ()
              || ComparePacketTransmissionPriority (*packet_entry_it, *lowest_priority_emergency_pkt_it))
            {
              lowest_priority_emergency_pkt_it = packet_entry_it;
            }

          // If the current emergency data packet is outside its destination GTA
          // and has lower transmission priority than the currently selected
          // packet then select it.
          if (outside_gta
              && (lowest_priority_emergency_pkt_not_in_gta_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*packet_entry_it, *lowest_priority_emergency_pkt_not_in_gta_it)))
            {
              lowest_priority_emergency_pkt_not_in_gta_it = packet_entry_it;
            }
        }
      else
        {
          // If the current regular data packet is the one with the lowest 
          // transmission priority then select it.
          if (lowest_priority_regular_pkt_it == m_packets_table.end ()
              || ComparePacketTransmissionPriority (*packet_entry_it, *lowest_priority_regular_pkt_it))
            {
              lowest_priority_regular_pkt_it = packet_entry_it;
            }

          // If the current regular data packet is outside its destination GTA
          // and has lower transmission priority than the currently selected
          // packet then select it.
          if (outside_gta
              && (lowest_priority_regular_pkt_not_in_gta_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*packet_entry_it, *lowest_priority_regular_pkt_not_in_gta_it)))
            {
              lowest_priority_regular_pkt_not_in_gta_it = packet_entry_it;
            }
        }
    }

  // Select the packet with the lowest priority.
  if (lowest_priority_regular_pkt_not_in_gta_it != m_packets_table.end ())
    {
      selected_packet = lowest_priority_regular_pkt_not_in_gta_it->first;
      NS_LOG_DEBUG ("Regular packet outside its destination geo-temporal area selected: " << selected_packet);
    }
  else if (lowest_priority_regular_pkt_it != m_packets_table.end ())
    {
      selected_packet = lowest_priority_regular_pkt_it->first;
      NS_LOG_DEBUG ("Regular packet inside its destination geo-temporal area selected: " << selected_packet);
    }
  else if (lowest_priority_emergency_pkt_not_in_gta_it != m_packets_table.end ())
    {
      selected_packet = lowest_priority_emergency_pkt_not_in_gta_it->first;
      NS_LOG_DEBUG ("Emergency packet outside its destination geo-temporal area selected: " << selected_packet);
    }
  else if (lowest_priority_emergency_pkt_it != m_packets_table.end ())
    {
      selected_packet = lowest_priority_emergency_pkt_it->first;
      NS_LOG_DEBUG ("Emergency packet inside its destination geo-temporal area selected: " << selected_packet);
    }
  else
    {
      NS_ABORT_MSG ("A packet should have been selected.");
    }

  return true;
}

bool
PacketsQueue::Enqueue (const DataHeader& data_header, const Vector2D& current_node_position,
                       const Ipv4Address& transmitter_ip)
{
  NS_LOG_FUNCTION (this << data_header << current_node_position);

  // Check if the packet entry already exists in the queue.
  if (Find (data_header.GetDataIdentifier ()))
    {
      // Packet already exists in queue, cancel.
      NS_LOG_DEBUG ("Packet already exists in queue, insertion canceled.");

      // Log statistics about received duplicated packet
      LogDuplicatedPacketReceived (data_header.GetDataIdentifier ());

      return false;
    }

  NS_LOG_DEBUG (m_packets_table.size () << " / " << m_max_queue_length
                << " packets before insertion.");

  // Check if the queue is full.
  if (m_packets_table.size () == m_max_queue_length)
    {
      ++m_packets_dropped_counter;

      // Find the packet with highest priority to be dropped (the one with the
      // highest hops count).
      DataIdentifier entry_to_delete;

      const bool to_delete_found = FindHighestDropPriorityPacket (current_node_position,
                                                                  entry_to_delete);
      NS_ASSERT (to_delete_found);

      NS_LOG_DEBUG ("Drops the packet with highest priority to be dropped due "
                    "to a full queue : " << entry_to_delete);

      // Log statistics about the packet drop
      LogPacketDropped (entry_to_delete);

      const bool deleted = m_packets_table.erase (entry_to_delete) > 0u;
      NS_ABORT_MSG_UNLESS (deleted == true, "Packet entry should have been deleted.");
    }

  std::pair<Iterator_t, bool> inserted_result;
  inserted_result = m_packets_table.insert (std::make_pair (data_header.GetDataIdentifier (),
                                                            PacketQueueEntry (data_header, m_max_replicas_counter)));

  NS_LOG_DEBUG ("Packet successfully inserted : " << inserted_result.first->second);
  NS_LOG_DEBUG (m_packets_table.size () << " / " << m_max_queue_length
                << " packets after insertion.");

  // Save statistics about the packet reception.
  LogNewPacketReceived (/* Data packet ID */ data_header.GetDataIdentifier (),
                        /* Transmitter node IP */ transmitter_ip);

  return true;
}

bool
PacketsQueue::FindHighestTransmitPriorityPacket (const Ipv4Address & local_node_ip,
                                                 const Vector2D& local_position,
                                                 const Vector2D& local_velocity,
                                                 const Ipv4Address & neighbor_node_ip,
                                                 const Vector2D& neighbor_position,
                                                 const Vector2D& neighbor_velocity,
                                                 const std::set<DataIdentifier>& disjoint_vector,
                                                 PacketQueueEntry& selected_packet,
                                                 bool & inside_area_flag) const
{
  NS_LOG_FUNCTION (this << " disjoint vector size " << disjoint_vector.size ()
                   << " local node IP " << local_node_ip
                   << " neighbor node IP " << neighbor_node_ip
                   << " gps " << m_gps);

  NS_ABORT_MSG_IF (m_gps == 0,
                   "A pointer to a valid GPS System object must be set in this "
                   "PacketsQueue object.");

  if (Size () == 0u)
    {
      NS_LOG_DEBUG ("Packets queue is empty, nothing to send.");
      return false;
    }

  if (disjoint_vector.empty ())
    {
      NS_LOG_DEBUG ("Disjoint vector is empty, no requested packets.");
      return false;
    }

  const Time current_time = Simulator::Now ();
  const uint32_t current_second = static_cast<uint32_t> (current_time.GetSeconds ());

  // Packet requested by another node in a SummaryVectorHeader that is currently
  // being processed.
  ConstIterator_t requested_packet_it;

  // Emergency packet that is inside its destination geo-temporal area.
  ConstIterator_t emergency_high_priority_it = m_packets_table.end ();

  // Emergency packet that is outside its destination geo-temporal area.
  ConstIterator_t emergency_low_priority_it = m_packets_table.end ();

  // Normal packet that is inside its destination geo-temporal area.
  ConstIterator_t normal_high_priority_it = m_packets_table.end ();

  // Normal packet that is outside its destination geo-temporal area.
  ConstIterator_t normal_low_priority_it = m_packets_table.end ();

  try
    {
      // Iterate through all the requested packets.
      for (std::set<DataIdentifier>::const_iterator data_id_it = disjoint_vector.begin ();
              data_id_it != disjoint_vector.end (); ++data_id_it)
        {
          requested_packet_it = m_packets_table.find (*data_id_it);

          // If the requested packet doesn't exist in the queue continue to the next one.
          if (requested_packet_it == m_packets_table.end ())
            {
              NS_LOG_DEBUG ("Requested packet " << *data_id_it << " doesn't exist in queue.");
              continue;
            }

          const DataHeader & data_packet = requested_packet_it->second.GetDataPacket ();
          const GeoTemporalArea & destination_gta = data_packet.GetDestinationGeoTemporalArea ();
          const bool emergency_packet = data_packet.IsEmergencyPacket ();

          /* Determine if there's is a packet inside its destination geo-temporal area,
           * if so, it has higher priority to be dequeued because it needs to be 
           * broadcasted before other packets that aren't inside its destination 
           * geo-temporal area are transmitted.
           */
          const bool local_node_inside_gta
                  = destination_gta.IsInsideGeoTemporalArea (local_position, current_time);
          const bool neighbor_node_inside_gta
                  = destination_gta.IsInsideGeoTemporalArea (neighbor_position, current_time);
          const bool inside_gta = local_node_inside_gta || neighbor_node_inside_gta;

          // If the current packet:
          // - Is an emergency packet, and
          // - Is inside its destination geo-temporal area, and
          // - Has better priority than the currently selected emergency high-priority packet.
          // Then the current packet is emergency high-priority packet.
          if (emergency_packet
              && inside_gta
              && (emergency_high_priority_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*emergency_high_priority_it, *requested_packet_it)))
            {
              NS_LOG_DEBUG ("Emergency high priority packet " << *data_id_it << " selected.");
              emergency_high_priority_it = requested_packet_it;
              continue;
            }

          // Emergency high-priority packet already found, stop looking for lower priorities.
          if (emergency_high_priority_it != m_packets_table.end ())
            continue;

          const bool replicas_remaining = requested_packet_it->second.GetReplicasCounter () > 0u;
          const bool valid_carrier = m_gps->IsVehicleValidPacketCarrier (/*Neighbor node IP*/ neighbor_node_ip,
                                                                         /*Carrier node IP*/ local_node_ip,
                                                                         /*Destination area*/ destination_gta.GetArea (),
                                                                         /*Current time (second)*/ current_second,
                                                                         /*Min. dist. diff.*/ m_min_vehicles_distance_diff);

          // If the current packet:
          // - Is an emergency packet, and
          // - Has remaining replicas to forward, and
          // - The neighbor node is a valid data-carrier, and
          // - Has better priority than the currently selected emergency low-priority packet.
          // Then the current packet is emergency low-priority packet.
          if (emergency_packet
              && replicas_remaining
              && valid_carrier
              && (emergency_low_priority_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*emergency_low_priority_it, *requested_packet_it)))
            {
              NS_LOG_DEBUG ("Emergency low priority packet " << *data_id_it << " selected.");
              emergency_low_priority_it = requested_packet_it;
              continue;
            }

          // Emergency low-priority packet already found, stop looking for lower priorities.
          if (emergency_low_priority_it != m_packets_table.end ())
            continue;

          // If the current packet:
          // - Is a normal packet (not an emergency), and
          // - Is inside its destination geo-temporal area, and
          // - Has better priority than the currently selected normal high-priority packet.
          // Then the current packet is normal high-priority packet.
          if (!emergency_packet
              && inside_gta
              && (normal_high_priority_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*normal_high_priority_it, *requested_packet_it)))
            {
              NS_LOG_DEBUG ("Normal high priority packet " << *data_id_it << " selected.");
              normal_high_priority_it = requested_packet_it;
              continue;
            }

          // Normal high-priority packet already found, stop looking for lower priorities.
          if (normal_high_priority_it != m_packets_table.end ())
            continue;

          // If the current packet:
          // - Is a normal packet (not an emergency), and
          // - Has remaining replicas to forward, and
          // - The neighbor node is a valid data-carrier, and
          // - Has better priority than the currently selected normal low-priority packet.
          // Then the current packet is normal low-priority packet.
          if (!emergency_packet
              && replicas_remaining
              && valid_carrier
              && (normal_low_priority_it == m_packets_table.end ()
                  || ComparePacketTransmissionPriority (*normal_low_priority_it, *requested_packet_it)))
            {
              NS_LOG_DEBUG ("Normal low priority packet " << *data_id_it << " selected.");
              normal_low_priority_it = requested_packet_it;
              continue;
            }
        }
    }
  catch (const std::exception & ex)
    {
      NS_LOG_WARN ("No packet was selected: Unexpected exception (" << ex.what () << ").");
      return false;
    }

  // If no packet was selected return false
  if (emergency_high_priority_it == m_packets_table.end ()
      && emergency_low_priority_it == m_packets_table.end ()
      && normal_high_priority_it == m_packets_table.end ()
      && normal_low_priority_it == m_packets_table.end ())
    {
      NS_LOG_DEBUG ("No requested packet was selected.");
      return false;
    }

  // Return the selected packet with the highest priority
  if (emergency_high_priority_it != m_packets_table.end ())
    {
      selected_packet = emergency_high_priority_it->second;
      inside_area_flag = true;
      NS_LOG_DEBUG ("Emergency high priority packet " << selected_packet.GetDataPacketId () << " selected.");
    }
  else if (emergency_low_priority_it != m_packets_table.end ())
    {
      selected_packet = emergency_low_priority_it->second;
      inside_area_flag = false;
      NS_LOG_DEBUG ("Emergency low priority packet " << selected_packet.GetDataPacketId () << " selected.");
    }
  else if (normal_high_priority_it != m_packets_table.end ())
    {
      selected_packet = normal_high_priority_it->second;
      inside_area_flag = true;
      NS_LOG_DEBUG ("Normal high priority packet " << selected_packet.GetDataPacketId () << " selected.");
    }
  else if (normal_low_priority_it != m_packets_table.end ())
    {
      selected_packet = normal_low_priority_it->second;
      inside_area_flag = false;
      NS_LOG_DEBUG ("Normal low priority packet " << selected_packet.GetDataPacketId () << " selected.");
    }
  else
    {
      NS_ABORT_MSG ("A packet should have been selected.");
    }

  return true;
}

bool
PacketsQueue::Dequeue (const Ipv4Address& local_node_ip,
                       const Vector2D& local_position,
                       const Vector2D& local_velocity,
                       const Ipv4Address& neighbor_node_ip,
                       const Vector2D& neighbor_position,
                       const Vector2D& neighbor_velocity,
                       const std::set<DataIdentifier>& disjoint_vector,
                       PacketQueueEntry& selected_packet)
{
  NS_LOG_FUNCTION (this << " disjoint vector size " << disjoint_vector.size ()
                   << " local node IP " << local_node_ip
                   << " neighbor node IP " << neighbor_node_ip
                   << " gps " << m_gps);

  if (Size () == 0u || disjoint_vector.empty ())
    {
      NS_LOG_DEBUG ("No packets to send.");
      return false;
    }

  // Flag that indicates if the selected packet is inside its destination area.
  bool inside_area_flag = false;

  // Find the packet with highest priority to transmit it
  if (!FindHighestTransmitPriorityPacket (local_node_ip,
                                          local_position,
                                          local_velocity,
                                          neighbor_node_ip,
                                          neighbor_position,
                                          neighbor_velocity,
                                          disjoint_vector,
                                          selected_packet,
                                          inside_area_flag))
    {
      // No packet was selected to be transmitted.
      NS_LOG_DEBUG ("No packet selected to be transmitted.");
      return false;
    }

  // A packet was selected to be transmitted.

  // If the selected packet is OUTSIDE the geo-temporal area then decrement a replica
  if (!inside_area_flag)
    {
      NS_LOG_DEBUG ("Selected packet with normal priority, discount 1 packet replica.");
      DiscountPacketReplica (selected_packet.GetDataPacketId ());
    }

  return true;
}

bool
PacketsQueue::DiscountPacketReplica (const DataIdentifier& packet_data_id)
{
  NS_LOG_FUNCTION (this << packet_data_id);

  Iterator_t packet_entry_it = m_packets_table.find (packet_data_id);

  if (packet_entry_it == m_packets_table.end ()) return false; // Packet not found

  // Retrieve current count
  const uint32_t original_replicas_count = packet_entry_it->second.GetReplicasCounter ();

  if (original_replicas_count == 0u)
    {
      throw std::runtime_error ("Replicas must never be less than zero.");
    }

  // Decrease count
  uint32_t new_replicas_count = original_replicas_count - 1u;

  // Save new count
  packet_entry_it->second.SetReplicasCounter (new_replicas_count);

  NS_LOG_DEBUG ("Replicas counter of packet " << packet_data_id << " decreased from "
                << original_replicas_count << " to " << new_replicas_count);
  return true;
}

bool
PacketsQueue::AddKnownPacketCarrier (const DataIdentifier& packet_data_id,
                                     const Ipv4Address& carrier_node_ip)
{
  NS_LOG_FUNCTION (this << packet_data_id << carrier_node_ip);

  Iterator_t packet_entry_it = m_packets_table.find (packet_data_id);

  if (packet_entry_it == m_packets_table.end ()) return false; // Packet not found

  packet_entry_it->second.AddKnownCarrierNode (carrier_node_ip);

  NS_LOG_DEBUG ("Added confirmed carrier node " << carrier_node_ip
                << " to packet " << packet_data_id);
  return true;
}

void
PacketsQueue::AddKnownPacketCarriers (const std::set<DataIdentifier>& packet_data_ids_set,
                                      const Ipv4Address& carrier_node_ip)
{
  NS_LOG_FUNCTION (this << packet_data_ids_set.size () << carrier_node_ip);

  if (packet_data_ids_set.empty ()) return; // When set is empty do nothing

  Iterator_t packet_entry_it;

  for (std::set<DataIdentifier>::const_iterator packet_data_id_it = packet_data_ids_set.begin ();
          packet_data_id_it != packet_data_ids_set.end (); ++packet_data_id_it)
    {
      packet_entry_it = m_packets_table.find (*packet_data_id_it);

      if (packet_entry_it == m_packets_table.end ()) continue; // Packet not found

      packet_entry_it->second.AddKnownCarrierNode (carrier_node_ip);

      NS_LOG_DEBUG ("Added confirmed carrier node " << carrier_node_ip
                    << " to packet " << *packet_data_id_it);
    }
}

void
PacketsQueue::Purge ()
{
  NS_LOG_FUNCTION (this);

  for (ConstIterator_t entry_it = m_packets_table.begin (); entry_it != m_packets_table.end ();)
    {
      if (entry_it->second.GetExpirationTime () <= Seconds (0))
        {
          NS_LOG_LOGIC ("Drops expired data packet entry : " << entry_it->second);
          // m_packets_table.erase (entry_it++); // For C++03
          entry_it = m_packets_table.erase (entry_it); // For C++11
        }
      else
        {
          ++entry_it;
        }
    }
}


// --------------------------
// Packet statistics
// --------------------------

void
PacketsQueue::LogNewPacketReceived (const DataIdentifier & data_packet_id,
                                    const Ipv4Address & transmitter_ip)
{
  // If it's the first time the packet is received, then store the newly created
  // DataPacketReceptionStats object.
  // If this packet was previously received and subsequently dropped, and later
  // received again. So there must exist a DataPacketReceptionStats object about
  // this packet. This existing object must be deleted and replaced by a new one
  // (because the received packet will be treated as received for the first time).
  m_data_packet_reception_stats[data_packet_id]
          = DataPacketReceptionStats (/* Data packet ID */ data_packet_id,
                                      /* Transmitter node IP */ transmitter_ip,
                                      /* Reception time */ Simulator::Now (),
                                      /* packet destined for me */ true);
}

void
PacketsQueue::LogPacketTransmitted (const DataIdentifier& data_packet_id)
{
  StatsIterator_t packet_stats_it = m_data_packet_reception_stats.find (data_packet_id);

  // If it doesn't exist the specified data packet then do nothing
  if (packet_stats_it == m_data_packet_reception_stats.end ())
    return;

  packet_stats_it->second.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
}

void
PacketsQueue::LogDuplicatedPacketReceived (const DataIdentifier& data_packet_id)
{
  StatsIterator_t packet_stats_it = m_data_packet_reception_stats.find (data_packet_id);

  // If it doesn't exist the specified data packet then do nothing
  if (packet_stats_it == m_data_packet_reception_stats.end ())
    return;

  packet_stats_it->second.IncrementReceivedDuplicatesCount ();
}

void
PacketsQueue::LogPacketDropped (const DataIdentifier& data_packet_id)
{
  StatsIterator_t packet_stats_it = m_data_packet_reception_stats.find (data_packet_id);

  // If it doesn't exist the specified data packet then do nothing
  if (packet_stats_it == m_data_packet_reception_stats.end ())
    return;

  packet_stats_it->second.SetPacketDropped ();
}

void
PacketsQueue::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
PacketsQueue::ToString () const
{
  char buffer [10];

  std::sprintf (buffer, "%u", (uint32_t) m_packets_table.size ());
  std::string str = "Packets queue has " + std::string (buffer) + " / ";

  std::sprintf (buffer, "%u", m_max_queue_length);
  str += std::string (buffer) + " packet entries";

  return str;
}

} // namespace geotemporal
} // namespace ns3

