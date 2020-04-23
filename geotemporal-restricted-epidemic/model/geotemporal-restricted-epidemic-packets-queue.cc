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
 * Data packets queue of the Geo-Temporal Restricted Epidemic protocol.
 */

#include "geotemporal-restricted-epidemic-packets-queue.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <utility>

#include <ns3/assert.h>
#include <ns3/log.h>
#include <ns3/simulator.h>

#include <ns3/math-utils.h>


NS_LOG_COMPONENT_DEFINE ("GeoTemporalRestrictedEpidemicPacketsQueue");

namespace ns3
{
namespace geotemporal_restricted_epidemic
{

// =============================================================================
//                                PacketQueueEntry
// =============================================================================

PacketQueueEntry::PacketQueueEntry ()
: m_data_packet (), m_expiration_time () { }

PacketQueueEntry::PacketQueueEntry (const DataHeader& data_packet)
: m_data_packet (data_packet), m_expiration_time ()
{
  SetExpirationTime (m_data_packet.GetDestinationGeoTemporalArea ().GetTimePeriod ());
}

PacketQueueEntry::PacketQueueEntry (const PacketQueueEntry& copy)
: m_data_packet (copy.m_data_packet),
m_expiration_time (copy.m_expiration_time) { }

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
          + " will expire at second " + std::string (buffer);
  return str;
}


// =============================================================================
//                                 PacketsQueue
// =============================================================================

PacketsQueue::PacketsQueue () : PacketsQueue (128u) { }

PacketsQueue::PacketsQueue (uint32_t maximum_queue_length)
: m_max_queue_length (maximum_queue_length),
m_packets_dropped_counter (0u),
m_packets_table (),
m_data_packet_reception_stats () { }

PacketsQueue::PacketsQueue (const PacketsQueue& copy)
: m_max_queue_length (copy.m_max_queue_length),
m_packets_dropped_counter (copy.m_packets_dropped_counter),
m_packets_table (copy.m_packets_table),
m_data_packet_reception_stats (copy.m_data_packet_reception_stats) { }


// --------------------------
// Getters & Setters
// --------------------------

uint32_t
PacketsQueue::Size ()
{
  Purge ();
  return m_packets_table.size ();
}

void
PacketsQueue::GetSummaryVector (std::set<DataIdentifier>& summary_vector)
{
  Purge ();

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
PacketsQueue::Find (const DataIdentifier& data_packet_id, PacketQueueEntry& entry)
{
  NS_LOG_FUNCTION (this << data_packet_id);

  Purge ();

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
PacketsQueue::Find (const DataIdentifier& data_packet_id)
{
  PacketQueueEntry entry;
  return Find (data_packet_id, entry);
}

bool
PacketsQueue::Find (const PacketQueueEntry& packet_entry)
{
  PacketQueueEntry e;
  return Find (packet_entry.GetDataPacketId (), e);
}


// --------------------------
// Modifiers
// --------------------------

void
PacketsQueue::ProcessDisjointVector (const SummaryVectorHeader& summary_vector_header,
                                     const Ipv4Address& destination_node_ip,
                                     std::set<DataIdentifier>& disjoint_vector) const
{
  NS_LOG_FUNCTION (this << " summary vector entries count "
                   << summary_vector_header.GetSummaryVectorSize ());

  // Clean the disjoint vector.
  disjoint_vector.clear ();

  uint32_t hops_count = 0u, added_packets_counter = 0u;

  const Time current_time = Simulator::Now ();
  const std::set<DataIdentifier> & summary_vector = summary_vector_header.GetSummaryVector ();
  const GeoTemporalLibrary::LibraryUtils::Vector2D & receiver_node_position
          = summary_vector_header.GetPosition ();

  Area allowed_area;

  // Iterate through all the packets in queue
  for (ConstIterator_t entry_it = m_packets_table.begin ();
          entry_it != m_packets_table.end (); ++entry_it)
    {
      /* Check if the data identifier of the current packet exists in the 
       * received summary vector. If it doesn't exist then insert it into the 
       * disjoint vector.
       * 
       * Also check that:
       * 1. the other node is NOT the source node of the packet,
       * 2. the receiver node is inside of the allowed area for transmission, and
       * 3. the packet has a hop count greater than 1 or equal to 1 and the
       *    other node is inside the packet's destination geo-temporal area.
       */

      // If packet is contained in the summary vector, don't insert it into disjoint vector.
      if (summary_vector.find (entry_it->first) != summary_vector.end ())
        continue;

      // 1. If destination node is the source node, don't insert it into disjoint vector.
      if (entry_it->second.GetSourceNodeIp () == destination_node_ip)
        continue;

      hops_count = entry_it->second.GetHopsCount ();
      NS_ABORT_MSG_IF (hops_count == 0u,
                       "The hops count of the packets shouldn't be equal to 0.");

      const DataHeader & data_packet = entry_it->second.GetDataPacket ();

      allowed_area = data_packet.GetAllowedArea ();

      // 2. If the receiver node is not inside the allowed area then the packet 
      // can not be forwarded.
      if (!allowed_area.IsInside (receiver_node_position))
        {
          NS_LOG_DEBUG ("Receiver node " << destination_node_ip << " is outside of the"
                        << " allowed area for transmission.");
          continue;
        }

      // 3. If packet's hops count is 1 AND the destination node is not inside the
      // destination geo-temporal area, then don't insert it into disjoint vector.
      if (hops_count == 1u
          && !data_packet.GetDestinationGeoTemporalArea ()
          .IsInsideGeoTemporalArea (receiver_node_position, current_time))
        continue;

      /* At this point the packet fulfills all of the following:
       * - Is not contained in the summary vector.
       * - Destination node is not the source node.
       * - It is inside of the allowed area for transmission.
       * - It has at least 1 hop left.
       * 
       * Add it to the disjoint vector.
       */
      disjoint_vector.insert (entry_it->first);
      ++added_packets_counter;
    }

  NS_LOG_DEBUG (added_packets_counter << " / " << m_packets_table.size ()
                << " packets added to the disjoint vector.");
}

bool
PacketsQueue::ComparePacketEntriesExpirationTime (const std::map<DataIdentifier, PacketQueueEntry>::value_type& entry_1,
                                                  const std::map<DataIdentifier, PacketQueueEntry>::value_type& entry_2)
{
  return entry_1.second.GetExpirationTime () < entry_2.second.GetExpirationTime ();
}

bool
PacketsQueue::Enqueue (const DataHeader& data_header, const Ipv4Address& transmitter_ip)
{
  NS_LOG_FUNCTION (this << data_header << transmitter_ip);

  // Check if the packet entry already exists in the queue.
  // Find will call Purge () to purge expired entries before performing the search
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

      // Find oldest entry
      Iterator_t entry_to_delete_it = std::min_element (m_packets_table.begin (),
                                                        m_packets_table.end (),
                                                        ComparePacketEntriesExpirationTime);

      NS_LOG_DEBUG ("Drops the oldest packet due to full queue : "
                    << entry_to_delete_it->second);

      // Log statistics about the packet drop
      LogPacketDropped (entry_to_delete_it->first);

      m_packets_table.erase (entry_to_delete_it);
    }

  std::pair<Iterator_t, bool> inserted_result;
  inserted_result = m_packets_table.insert (std::make_pair (data_header.GetDataIdentifier (),
                                                            PacketQueueEntry (data_header)));

  NS_LOG_DEBUG ("Packet successfully inserted : " << inserted_result.first->second);
  NS_LOG_DEBUG (m_packets_table.size () << " / " << m_max_queue_length
                << " packets after insertion.");

  // Save statistics about the packet reception.
  LogNewPacketReceived (/* Data packet ID */ data_header.GetDataIdentifier (),
                        /* Transmitter node IP */ transmitter_ip);

  return true;
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

} // namespace geotemporal_restricted_epidemic
} // namespace ns3

