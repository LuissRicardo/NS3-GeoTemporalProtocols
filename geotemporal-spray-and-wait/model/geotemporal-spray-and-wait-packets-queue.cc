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
 * Data packets queue of the Geo-Temporal Spray And Wait protocol.
 */

#include "geotemporal-spray-and-wait-packets-queue.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <utility>

#include <ns3/assert.h>
#include <ns3/log.h>
#include <ns3/simulator.h>


NS_LOG_COMPONENT_DEFINE ("GeoTemporalSprayAndWaitPacketsQueue");

namespace ns3
{
namespace geotemporal_spray_and_wait
{

// =============================================================================
//                                PacketQueueEntry
// =============================================================================

PacketQueueEntry::PacketQueueEntry ()
: m_data_packet (), m_expiration_time (), m_replicas_counter (0u) { }

PacketQueueEntry::PacketQueueEntry (const DataHeader& data_packet)
: m_data_packet (data_packet), m_expiration_time (),
m_replicas_counter (data_packet.GetReplicasToForward ())
{
  SetExpirationTime (m_data_packet.GetDestinationGeoTemporalArea ().GetTimePeriod ());
}

PacketQueueEntry::PacketQueueEntry (const PacketQueueEntry& copy)
: m_data_packet (copy.m_data_packet),
m_expiration_time (copy.m_expiration_time),
m_replicas_counter (copy.m_replicas_counter) { }

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

PacketsQueue::PacketsQueue () : PacketsQueue (false, 128u) { }

PacketsQueue::PacketsQueue (bool binary_mode_flag, uint32_t maximum_queue_length)
: m_binary_spray_and_wait_mode_flag (binary_mode_flag),
m_max_queue_length (maximum_queue_length),
m_packets_dropped_counter (0u),
m_packets_table (),
m_data_packet_reception_stats () { }

PacketsQueue::PacketsQueue (const PacketsQueue& copy)
: m_binary_spray_and_wait_mode_flag (copy.m_binary_spray_and_wait_mode_flag),
m_max_queue_length (copy.m_max_queue_length),
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
PacketsQueue::ProcessDisjointVector (const std::set<DataIdentifier>& summary_vector,
                                     std::set<DataIdentifier>& disjoint_vector) const
{
  NS_LOG_FUNCTION (this << " summary vector entries count " << summary_vector.size ());

  // Clean the disjoint vector.
  disjoint_vector.clear ();

  // Iterate through all data identifiers in the summary vector
  for (std::set<DataIdentifier>::const_iterator it = summary_vector.begin ();
          it != summary_vector.end (); ++it)
    {
      // Check if the packet exists in this packets queue: if it doesn't exist
      // in this packets queue then insert it into the disjoint vector.
      if (m_packets_table.find (*it) == m_packets_table.end ())
        {
          // The current packet doesn't exist in this packets queue, insert it
          // into the disjoint vector.
          disjoint_vector.insert (*it);
        }
    }
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

bool
PacketsQueue::DiscountPacketReplicasToForward (const DataIdentifier& data_packet_id,
                                               uint32_t& replicas_to_forward)
{
  NS_LOG_FUNCTION (this << data_packet_id << "Binary mode" << m_binary_spray_and_wait_mode_flag);

  Iterator_t packet_entry_it = m_packets_table.find (data_packet_id);

  // Check if the specified data packet exists in the queue
  if (packet_entry_it == m_packets_table.end ())
    return false; // The data packet it doesn't exists, cancel.

  // If there's one or less packet replicas left, it must NOT discount any replicas
  // because the packet is in direct-transmission-only mode, that is, transmit 
  // only to destination.
  if (packet_entry_it->second.GetReplicasCounter () <= 1u)
    return false;

  uint32_t replicas_to_keep;
  const uint32_t current_replicas = packet_entry_it->second.GetReplicasCounter ();

  if (!m_binary_spray_and_wait_mode_flag)
    {
      // Normal mode: discount 1 replica
      replicas_to_forward = 1u;
      replicas_to_keep = current_replicas - 1u;
    }
  else
    {
      // Binary mode: transmit floor(current replicas / 2) replicas and 
      //              keep ceil(current replicas / 2) replicas
      const double modified_replicas = ((double) current_replicas) / 2.0;

      replicas_to_forward = std::floor (modified_replicas);
      replicas_to_keep = std::ceil (modified_replicas);
    }

  NS_ASSERT (current_replicas == replicas_to_forward + replicas_to_keep);

  packet_entry_it->second.SetReplicasCounter (replicas_to_keep);

  NS_LOG_DEBUG ("Replicas from data packet " << data_packet_id << " discounted: "
                << "Initial " << current_replicas
                << " | To keep " << replicas_to_keep
                << " | To forward " << replicas_to_forward);

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

} // namespace geotemporal_spray_and_wait
} // namespace ns3

