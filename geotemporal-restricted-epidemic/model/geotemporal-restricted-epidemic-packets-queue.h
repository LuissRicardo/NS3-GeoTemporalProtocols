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

#ifndef GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_QUEUE_H
#define GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_QUEUE_H

#include "geotemporal-restricted-epidemic-packets.h"

#include <map>
#include <set>
#include <string>

#include <ns3/nstime.h>
#include <ns3/simulator.h>

#include <ns3/geotemporal-utils.h>
#include <ns3/packet-utils.h>
#include <ns3/statistics-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;


namespace ns3
{
namespace geotemporal_restricted_epidemic
{

// =============================================================================
//                                PacketQueueEntry
// =============================================================================

/**
 * \ingroup geotemporal-restricted-epidemic
 * 
 * Entry of the data packets queue of the Geo-Temporal Restricted Epidemic protocol.
 */
class PacketQueueEntry
{
private:

  /** Epidemic data packet. */
  DataHeader m_data_packet;

  /** Expiration time of the data packet entry. */
  Time m_expiration_time;


public:

  PacketQueueEntry ();

  PacketQueueEntry (const DataHeader & data_packet);

  PacketQueueEntry (const PacketQueueEntry & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const DataIdentifier &
  GetDataPacketId () const
  {
    return m_data_packet.GetDataIdentifier ();
  }

  inline const DataHeader &
  GetDataPacket () const
  {
    return m_data_packet;
  }

  inline const uint32_t
  GetHopsCount () const
  {
    return m_data_packet.GetHopsCount ();
  }

  inline const Ipv4Address &
  GetSourceNodeIp () const
  {
    return m_data_packet.GetDataIdentifier ().GetSourceIp ();
  }

  inline const Time
  GetExpirationTime () const
  {
    return m_expiration_time - Simulator::Now ();
  }

  inline void
  SetExpirationTime (const Time & expiration_time)
  {
    m_expiration_time = expiration_time + Simulator::Now ();
  }

  inline void
  SetExpirationTime (uint32_t packet_initial_time, uint32_t packet_duration)
  {
    m_expiration_time = Seconds (packet_initial_time + packet_duration);
  }

  inline void
  SetExpirationTime (const TimePeriod & packet_time_period)
  {
    m_expiration_time = packet_time_period.GetEndTime ();
  }


  void
  Print (std::ostream &os) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const PacketQueueEntry & lhs, const PacketQueueEntry & rhs);

};

// PacketQueueEntry relational operators

inline bool
operator== (const PacketQueueEntry & lhs, const PacketQueueEntry & rhs)
{
  return lhs.m_data_packet == rhs.m_data_packet
          && lhs.m_expiration_time == rhs.m_expiration_time;
}

inline bool
operator!= (const PacketQueueEntry & lhs, const PacketQueueEntry & rhs)
{
  return !operator== (lhs, rhs);
}

// PacketQueueEntry stream operators

inline std::ostream &
operator<< (std::ostream & os, const PacketQueueEntry & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                 PacketsQueue
// =============================================================================

/**
 * \ingroup geotemporal-restricted-epidemic
 * 
 * Packets queue of the Geo-Temporal Restricted Epidemic protocol.
 */
class PacketsQueue
{
private:

  /** 
   * Maximum number of packet entries that the queue can store at once.
   * 
   * If the queue reaches this maximum number of packet entries and a new packet
   * is stored, then the oldest packet will be dropped from the queue before 
   * inserting the new one.
   */
  uint32_t m_max_queue_length;

  /**
   * The counter of all packets dropped due to a full queue.
   */
  uint32_t m_packets_dropped_counter;

  /** Data packets table. */
  std::map<DataIdentifier, PacketQueueEntry> m_packets_table;

  /** Data packets stats. */
  std::map<DataIdentifier, DataPacketReceptionStats> m_data_packet_reception_stats;

  /** Packets table iterator. */
  typedef std::map<DataIdentifier, PacketQueueEntry>::iterator Iterator_t;

  /** Packets table constant iterator. */
  typedef std::map<DataIdentifier, PacketQueueEntry>::const_iterator ConstIterator_t;

  /** Packet statistics iterator. */
  typedef std::map<DataIdentifier, DataPacketReceptionStats>::iterator StatsIterator_t;


public:

  /**
   * Initializes a new packets queue with default values; 128 as the maximum 
   * queue length.
   */
  PacketsQueue ();

  /**
   * Initializes a new packets queue with the specified maximum queue length.
   * 
   * @param maximum_queue_length Maximum number of packet entries that the queue
   * can store at once.
   */
  PacketsQueue (uint32_t maximum_queue_length);

  PacketsQueue (const PacketsQueue & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  /**
   * Returns the maximum number of packet entries that the queue can store at 
   * once.
   * 
   * If the queue reaches this maximum number of packet entries and a new packet
   * is stored, then the oldest packet will be dropped from the queue before 
   * inserting the new one.
   */
  inline uint32_t
  GetMaxLength () const
  {
    return m_max_queue_length;
  }

  /**
   * Sets the maximum number of packet entries that the queue can store at 
   * once.
   * 
   * If the queue reaches this maximum number of packet entries and a new packet
   * is stored, then the oldest packet will be dropped from the queue before 
   * inserting the new one.
   * 
   * @param maximum_queue_length [IN] The maximum number of entries.
   */
  inline void
  SetMaxLength (uint32_t maximum_queue_length)
  {
    m_max_queue_length = maximum_queue_length;
  }

  /**
   * Returns the counter of all packets dropped due to a full queue.
   */
  inline uint32_t
  GetDroppedPacketsCounter () const
  {
    return m_packets_dropped_counter;
  }

  /**
   * Returns the size of the packets queue.
   * 
   * This function purges expired packet entries before counting the packet
   * entries, so if there exist expired packet entries first they will be
   * purged and then the number of remaining (non expired) entries will be 
   * returned.
   */
  uint32_t
  Size ();

  /**
   * Returns (in the reference parameter) the summary vector of the packets queue.
   * 
   * The <code>summary_vector</code> parameter is always emptied. And then the
   * data is stored into it. The <code>summary_vector</code> may remain empty if
   * there's no data to insert into it.
   * 
   * This function purges expired packet entries before storing the summary 
   * vector in the <code>summary_vector</code> parameter, so the summary vector
   * will be composed by valid (non expired) entries only.
   * 
   * @param summary_vector [OUT] The summary vector is stored in this parameter.
   */
  void
  GetSummaryVector (std::set<DataIdentifier> & summary_vector);

  /** Returns the statistics of all received data packets. */
  inline const std::map<DataIdentifier, DataPacketReceptionStats> &
  GetPacketReceptionStats () const
  {
    return m_data_packet_reception_stats;
  }


  // --------------------------
  // Lookup
  // --------------------------

  /**
   * Finds a packet queue entry with the specified data packet identifier.
   * 
   * This function purges expired packet entries before searching the desired
   * packet entry, so if the packet entry exists but it has expired then it will
   * be purged and it won't be found.
   * 
   * @param data_packet_id [IN] Data packet identifier of the packet to search for.
   * @param entry [OUT] The packet entry with the specified data packet 
   * identifier, if it exists. If an entry with the given data packet identifier
   * is not found this out parameter is not modified.
   * 
   * @return <code>true</code> if a packet entry with the specified data packet
   * identifier exists. If no such packet entry is found then returns 
   * <code>false</code>.
   */
  bool
  Find (const DataIdentifier & data_packet_id, PacketQueueEntry & entry);

  /**
   * Finds a packet queue entry with the specified data packet identifier.
   * 
   * This function purges expired packet entries before searching the desired
   * packet entry, so if the packet entry exists but it has expired then it will
   * be purged and it won't be found.
   * 
   * @param data_packet_id [IN] Data packet identifier of the packet to search for.
   * 
   * @return <code>true</code> if a packet entry with the specified data packet
   * identifier exists. If no such packet entry is found then returns 
   * <code>false</code>.
   */
  bool
  Find (const DataIdentifier & data_packet_id);

  /**
   * Finds a packet queue entry with the same data packet identifier of the 
   * given packet queue entry.
   * 
   * This function purges expired packet entries before searching the desired
   * packet entry, so if the packet entry exists but it has expired then it will
   * be purged and it won't be found.
   * 
   * @param packet_entry [IN] Packet queue entry of the packet to search for.
   * 
   * @return <code>true</code> if a packet entry with the specified data packet
   * identifier exists. If no such packet entry is found then returns 
   * <code>false</code>.
   */
  bool
  Find (const PacketQueueEntry & packet_entry);


  // --------------------------
  // Modifiers
  // --------------------------

  /**
   * Removes all packet entries from the packets queue.
   */
  inline void
  Clear ()
  {
    m_packets_table.clear ();
  }

  /**
   * Given a Summary Vector header that contains a summary vector (a set of data
   * packet identifiers) and the geographical position of the receiver node 
   * processes the disjoint vector, that is, the set of data packet identifiers 
   * that aren't contained in the summary vector.
   * 
   * The <code>disjoint_vector</code> parameter is always emptied. And then the
   * processed data is stored into it. The <code>disjoint_vector</code> may 
   * remain empty if there's no data to insert into it.
   * 
   * This function doesn't purge expired packet entries.
   * 
   * @param summary_vector_header [IN] Summary vector header received from other
   * node.
   * @param destination_node_ip [IN] The IP address of the node that sent the
   * summary vector.
   * @param disjoint_vector [OUT] The disjoint vector is stored in this parameter.
   */
  void
  ProcessDisjointVector (const SummaryVectorHeader & summary_vector_header,
                         const Ipv4Address & destination_node_ip,
                         std::set<DataIdentifier> & disjoint_vector) const;


private:

  /**
   * Compares the expiration time of 2 packet queue entries and returns 
   * <code>true</code> if <code>entry_1</code> is <i>less</i> than
   * <code>entry_2</code>.
   * 
   * This comparison function object satisfies the requirements of Compare.
   * 
   * @return <code>true</code> if <code>entry_1</code> is <i>less</i> than
   * <code>entry_2</code>
   */
  static bool
  ComparePacketEntriesExpirationTime (const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_1,
                                      const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_2);


public:

  /**
   * Inserts a new packet queue entry to the packets queue.
   * 
   * The new packet queue entry is set with the expiration time contained in the
   * given <code>DataHeader</code> object, so this object must be properly 
   * configured. The expiration time is obtained from the end of the time scope 
   * of the destination geo-temporal area.
   * 
   * This function purges expired packet entries before enqueueing the new 
   * packet queue entry, so if the packet entry to be inserted does exists but 
   * it has expired then it will be purged and inserted again.
   * 
   * @param data_header [IN] Data header of the data packet to insert.
   * @param transmitter_ip [IN] IP address of the node that transmitted the 
   * packet.
   * 
   * @return <code>true</code> if the data packet was successfully inserted, 
   * <code>false</code> otherwise.
   */
  bool
  Enqueue (const DataHeader & data_header, const Ipv4Address & transmitter_ip);


private:

  /**
   * Removes all expired data packet entries from the data packets queue.
   */
  void
  Purge ();


public:

  // --------------------------
  // Packet statistics
  // --------------------------

  /**
   * Logs that a data packet was received for the first time.
   * 
   * It is also used to overwrite that a packet was received again after it was
   * dropped before.
   * 
   * @param data_packet_id Data packet identifier of the received data packet.
   * @param transmitter_ip IP address of the node that transmitted the packet.
   */
  void
  LogNewPacketReceived (const DataIdentifier & data_packet_id,
                        const Ipv4Address & transmitter_ip);

  /**
   * Logs that a data packet in the queue was transmitted.
   * 
   * @param data_packet_id Data packet identifier of the transmitted packet.
   */
  void
  LogPacketTransmitted (const DataIdentifier & data_packet_id);

  /**
   * Logs that a duplicate of an already known data packet is received.
   * 
   * @param data_packet_id Data packet identifier of the received packet.
   */
  void
  LogDuplicatedPacketReceived (const DataIdentifier & data_packet_id);

  /**
   * Logs that a data packet was dropped from storage because the queue is full
   * and a new data packet was received.
   * 
   * @param data_packet_id Data packet identifier of the dropped packet.
   */
  void
  LogPacketDropped (const DataIdentifier & data_packet_id);


  void
  Print (std::ostream &os) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const PacketsQueue & lhs, const PacketsQueue & rhs);
};

// PacketsQueue relational operators

inline bool
operator== (const PacketsQueue & lhs, const PacketsQueue & rhs)
{
  return lhs.m_max_queue_length == rhs.m_max_queue_length
          && lhs.m_packets_dropped_counter == rhs.m_packets_dropped_counter
          && lhs.m_packets_table == rhs.m_packets_table
          && lhs.m_data_packet_reception_stats == rhs.m_data_packet_reception_stats;
}

inline bool
operator!= (const PacketsQueue & lhs, const PacketsQueue & rhs)
{
  return !operator== (lhs, rhs);
}

// PacketsQueue stream operators

inline std::ostream &
operator<< (std::ostream & os, const PacketsQueue & obj)
{
  obj.Print (os);
  return os;
}

} // namespace geotemporal_restricted_epidemic
} // namespace ns3

#endif /* GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_QUEUE_H */

