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

#ifndef GEOTEMPORAL_PACKETS_QUEUE_H
#define GEOTEMPORAL_PACKETS_QUEUE_H

#include "geotemporal-packets.h"

#include <map>
#include <set>
#include <string>

#include <ns3/assert.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>

#include <ns3/geotemporal-utils.h>
#include <ns3/packet-utils.h>
#include <ns3/statistics-utils.h>
#include <ns3/gps-system.h>

using namespace GeoTemporalLibrary::LibraryUtils;
using namespace GeoTemporalLibrary::NavigationSystem;

namespace ns3
{
namespace geotemporal
{

/* Forward declaration */
namespace test
{
class PacketsQueueTest;
}

// =============================================================================
//                                PacketQueueEntry
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Entry of the data packets queue of the Geo-Temporal protocol.
 */
class PacketQueueEntry
{
private:

  /** Geo-temporal data packet. */
  DataHeader m_data_packet;

  /** Expiration time of the data packet entry. */
  Time m_expiration_time;

  /**
   * Remaining number of packet replicas that the routing protocol can send to 
   * other nodes.
   * 
   * This number should be decreasing.
   */
  uint32_t m_replicas_counter;

  /** List of the IP addresses of the nodes that already know this packet. */
  std::set<Ipv4Address> m_known_carriers;


public:

  PacketQueueEntry ();

  PacketQueueEntry (const DataHeader & data_packet, const uint16_t replicas_counter);

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

  inline uint32_t
  GetReplicasCounter () const
  {
    return m_replicas_counter;
  }

  inline void
  SetReplicasCounter (uint32_t replicas_counter)
  {
    m_replicas_counter = replicas_counter;
  }

  inline void
  AddKnownCarrierNode (const Ipv4Address & carrier_node_ip)
  {
    m_known_carriers.insert (carrier_node_ip);
  }

  inline void
  ClearKnownCarriersNodes ()
  {
    m_known_carriers.clear ();
  }

  inline uint32_t
  GetKnownCarrierNodesCount () const
  {
    return m_known_carriers.size ();
  }

  inline const std::set<Ipv4Address> &
  GetKnownCarrierNodesSet () const
  {
    return m_known_carriers;
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
          && lhs.m_expiration_time == rhs.m_expiration_time
          && lhs.m_replicas_counter == rhs.m_replicas_counter
          && lhs.m_known_carriers == rhs.m_known_carriers;
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
 * \ingroup geotemporal
 * 
 * Packets queue of the Geo-Temporal protocol.
 */
class PacketsQueue
{
private:

  /**
   * A pointer to the GPS System with information about the geographical area
   * and the mobility traces of the nodes being simulated.
   * 
   * The information about the geographical area includes a graph of the streets
   * map.
   * 
   * The information about the mobility traces is the routes that every node
   * follow through the entire simulation.
   */
  Ptr<GpsSystem> m_gps;

  /** 
   * Maximum number of packet entries that the queue can store at once.
   * 
   * If the queue reaches this maximum number of packet entries and a new packet
   * is stored, then the oldest packet will be dropped from the queue before 
   * inserting the new one.
   */
  uint32_t m_max_queue_length;

  /**
   * Maximum number of packet replicas of each data packet that the routing 
   * protocol can send.
   */
  uint16_t m_max_replicas_counter;

  /**
   * The counter of all packets dropped due to a full queue.
   */
  uint32_t m_packets_dropped_counter;

  /**
   * Minimum distance (in meters) difference between vehicles to consider a node
   * as a valid carrier.
   */
  uint32_t m_min_vehicles_distance_diff;

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
   * @param gps_system A pointer to the GPS System with information about the 
   * geographical area and the mobility traces of the nodes being simulated.
   * @param maximum_queue_length Maximum number of packet entries that the queue
   * can store at once.
   * @param default_replicas_counter The default number of replicas of each data
   * packet that the routing protocol can send.
   */
  PacketsQueue (Ptr<GpsSystem> gps_system, uint32_t maximum_queue_length,
                uint32_t default_replicas_counter);

  PacketsQueue (const PacketsQueue & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  /** Returns the GPS system. */
  inline const Ptr<const GpsSystem>
  GetGpsSystem () const
  {
    return m_gps;
  }

  /** Sets the GPS system to be used. */
  inline void
  SetGpsSystem (Ptr<GpsSystem> gps_system)
  {
    NS_ASSERT (gps_system != 0);

    m_gps = gps_system;
  }

  /** Clears the GPS system. */
  inline void
  ClearGpsSystem ()
  {
    m_gps = 0;

    NS_ASSERT (m_gps == 0);
  }

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
   * Returns the maximum number of packet replicas of each data packet that the 
   * routing protocol can send.
   */
  inline uint16_t
  GetMaxReplicasCounter () const
  {
    return m_max_replicas_counter;
  }

  /**
   * Sets the maximum number of packet replicas of each data packet that the 
   * routing protocol can send.
   * 
   * @param max_replicas_counter [IN] The maximum number of packet replicas.
   */
  inline void
  SetMaxReplicasCounter (uint16_t max_replicas_counter)
  {
    m_max_replicas_counter = max_replicas_counter;
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
   * Returns the minimum distance (in meters) difference between vehicles to 
   * consider a node as a valid carrier.
   */
  inline uint32_t
  GetMinVehiclesDistanceDifference () const
  {
    return m_min_vehicles_distance_diff;
  }

  /**
   * Sets the minimum distance (in meters) difference between vehicles to 
   * consider a node as a valid carrier.
   */
  inline void
  SetMinVehiclesDistanceDifference (uint32_t distance_difference)
  {
    m_min_vehicles_distance_diff = distance_difference;
  }

  /**
   * Returns the size of the packets queue.
   */
  uint32_t
  Size () const;

  /**
   * Returns (in the reference parameter) the summary vector of the packets queue.
   * 
   * The <code>summary_vector</code> parameter is always emptied. And then the
   * data is stored into it. The <code>summary_vector</code> may remain empty if
   * there's no data to insert into it.
   * 
   * @param summary_vector [OUT] The summary vector is stored in this parameter.
   */
  void
  GetSummaryVector (std::set<DataIdentifier> & summary_vector) const;

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
  Find (const DataIdentifier & data_packet_id, PacketQueueEntry & entry) const;

  /**
   * Finds a packet queue entry with the specified data packet identifier.
   * 
   * @param data_packet_id [IN] Data packet identifier of the packet to search for.
   * 
   * @return <code>true</code> if a packet entry with the specified data packet
   * identifier exists. If no such packet entry is found then returns 
   * <code>false</code>.
   */
  bool
  Find (const DataIdentifier & data_packet_id) const;

  /**
   * Finds a packet queue entry with the same data packet identifier of the 
   * given packet queue entry.
   * 
   * @param packet_entry [IN] Packet queue entry of the packet to search for.
   * 
   * @return <code>true</code> if a packet entry with the specified data packet
   * identifier exists. If no such packet entry is found then returns 
   * <code>false</code>.
   */
  bool
  Find (const PacketQueueEntry & packet_entry) const;


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
   * Calculates the mutual disjoint vectors using the given summary vector and
   * the contents of the packets queue to determine the set of locally unknown 
   * packets and the set of packets that the neighbor node does not know.
   * 
   * The <code>local_unknown_packets</code> and <code>neighbor_unknown_packets</code>
   * parameters are always emptied. And then the processed data is stored into
   * it. The <code>local_unknown_packets</code> and <code>neighbor_unknown_packets</code>
   * may remain empty if there's no data to insert into them.
   * 
   * @param summary_vector [IN] Received summary vector.
   * @param local_ip [IN] IP address of the local node.
   * @param neighbor_ip [IN] IP address of the neighbor node.
   * @param local_unknown_packets [OUT] The set of packets that the local node
   * does not know.
   * @param neighbor_unknown_packets [OUT] the set of packets that the neighbor
   * node does not know.
   */
  void
  ProcessDisjointVectors (const std::set<DataIdentifier> & summary_vector,
                          const Ipv4Address & local_ip,
                          const Ipv4Address & neighbor_ip,
                          std::set<DataIdentifier> & local_unknown_packets,
                          std::set<DataIdentifier> & neighbor_unknown_packets) const;


private:

  /**
   * Compares the drop priority of 2 packet queue entries and returns 
   * <code>true</code> if <code>entry_1</code> has <i>lower</i> drop priority
   * than <code>entry_2</code>.
   * 
   * This comparison function object satisfies the requirements of Compare.
   * 
   * @return <code>true</code> if <code>entry_1</code> has <i>lower</i> priority
   * than <code>entry_2</code>.
   */
  static bool
  ComparePacketDropPriority (const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_1,
                             const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_2);

  /**
   * Compares the transmission priority of 2 packet queue entries and returns
   * <code>true</code> if <code>entry_1</code> has <i>lower</i> transmission 
   * priority than <code>entry_2</code>.
   * 
   * This comparison function object satisfies the requirements of Compare.
   * 
   * @return <code>true</code> if <code>entry_1</code> has <i>lower</i> priority
   * than <code>entry_2</code>.
   */
  static bool
  ComparePacketTransmissionPriority (const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_1,
                                     const std::map<DataIdentifier, PacketQueueEntry>::value_type & entry_2);

  /**
   * Finds the packet with highest priority to be dropped from the queue.
   * 
   * A packet's priority to be dropped is determined by the hops count; the 
   * packet with the highest hops count has the highest priority. When a tie 
   * occurs, the tie is broken with the number of nodes that know the packets, 
   * the packet that is known by more nodes has higher priority.
   * 
   * When a packet entry is found it returns <code>true</code> and the identifier
   * of the packet with the highest priority is stored in the output parameter 
   * <code>selected_packet</code>. On the contrary, if it can't find a packet
   * entry to be dropped (when the packets queue is empty) it returns 
   * <code>false</code> and the output parameter <code>selected_packet</code> is
   * not modified at all.
   * 
   * @param current_node_position [IN] The current geographical position of the
   * local node.
   * @param selected_packet [OUT] The identifier of the selected packet to be 
   * dropped.
   * 
   * @return <code>true</code> if a packet is found, otherwise <code>false</code>.
   */
  bool
  FindHighestDropPriorityPacket (const Vector2D & current_node_position,
                                 DataIdentifier & selected_packet) const;

  /**
   * Finds the packet with highest priority to be transmitted.
   * 
   * When a packet is inside its destination geo-temporal area it has higher
   * priority than packets that aren't inside its destination geo-temporal
   * area. So, packets that are inside its destination geo-temporal area have
   * high priority and packets that are not inside its destination geo-temporal
   * area have normal priority.
   * 
   * Furthermore, when there are multiple packets with the same level of 
   * priority, normal or high, the one to transmit is determined by its hops 
   * count; the packet with the lowest hops count has the highest priority. And
   * if a tie occurs, the tie is broken with the number of nodes that know the 
   * packets, the packet that is known by less nodes has higher priority.
   * 
   * When a packet entry is found it returns <code>true</code>, the packet
   * queue entry object is stored in the output parameter <code>selected_packet
   * </code>, and the output parameter <code>high_priority</code> is set 
   * accordingly to the priority of the selected packet; <code>true</code> if it
   * has high priority and <code>false</code> if it has normal priority. On the 
   * contrary, if it can not find a packet queue entry to be transmitted it 
   * returns <code>false</code> and both output parameters, <code>selected_packet
   * </code> and <code>high_priority</code>, are not modified at all.
   * 
   * @Note
   * One important note is that even though the packets queue may not be empty
   * the function might not return a packet to be transmitted, depending on the
   * viability of the neighbor node as a data carrier or if it is inside the
   * destination geo-temporal area of a packet.
   * 
   * @Important
   * With the geographical position and velocity vector of a node and a streets
   * map it is possible to determine the exact location of the node in the 
   * streets map and the direction it is headed. Using this information
   * (geographical position and direction), of the local and neighbor nodes, it
   * can be determined if the neighbor node is a valid packet carrier or not.
   * 
   * But given that this process might take a long time to be computed depending
   * on the size of the streets map and the streets topology, in order to 
   * decrease the needed time to run the simulations, we pre-computed the
   * information where the nodes are in the streets map.
   * 
   * Using this pre-computed information,
   * <code>GpsSystem::IsVehicleValidPacketCarrier ()</code> computes (in a
   * seamless way) if a node is a valid packet carrier or not. This function 
   * only needs the identifiers (numeric ID or IP address) of the local node and
   * neighbor node, and the current simulation time to read from a file the
   * pre-computed information.
   * 
   * @param local_node_ip [IN] IP address of the local node.
   * @param local_position [IN] Geographical position of the local node.
   * @param local_velocity [IN] Velocity vector of the local node.
   * @param neighbor_node_ip [IN] IP address of the neighbor node.
   * @param neighbor_position [IN] Geographical position of the local node.
   * @param neighbor_velocity [IN] Velocity vector of the local node.
   * @param disjoint_vector [IN] Disjoint vector with the identifiers of the
   * requested packets.
   * @param selected_packet [OUT] The packet selected to be transmitted.
   * @param high_priority [OUT] With <code>true</code> indicates that the 
   * selected packet has high priority, otherwise it has normal priority.
   * 
   * @return <code>true</code> if a packet is found, otherwise <code>false</code>.
   */
  bool
  FindHighestTransmitPriorityPacket (const Ipv4Address & local_node_ip,
                                     const Vector2D & local_position,
                                     const Vector2D & local_velocity,
                                     const Ipv4Address & neighbor_node_ip,
                                     const Vector2D & neighbor_position,
                                     const Vector2D & neighbor_velocity,
                                     const std::set<DataIdentifier> & disjoint_vector,
                                     PacketQueueEntry & selected_packet,
                                     bool & high_priority) const;


public:

  /**
   * Inserts a new packet queue entry to the packets queue.
   * 
   * The new packet queue entry is set with the expiration time contained in the
   * given <code>DataHeader</code> object, so this object must be properly 
   * configured. The expiration time is obtained from the end of the time scope
   * of the destination geo-temporal area.
   * 
   * @param data_header [IN] Data header of the data packet to insert.
   * @param current_node_position [IN] The current geographical position of the
   * local node.
   * @param transmitter_ip [IN] IP address of the node that transmitted the 
   * packet.
   * 
   * @return <code>true</code> if the data packet was successfully inserted, 
   * <code>false</code> otherwise.
   */
  bool
  Enqueue (const DataHeader & data_header,
           const Vector2D & current_node_position,
           const Ipv4Address & transmitter_ip);

  /**
   * Dequeues for transmission the packet with the highest priority from the
   * given set of requested packets.
   * 
   * @Note
   * One important note is that even though the packets queue may not be empty
   * the function might not select a packet to be transmitted, depending on the
   * viability of the neighbor node as a data carrier or if it is inside the
   * destination geo-temporal area of a packet.
   * 
   * @Important
   * With the geographical position and velocity vector of a node and a streets
   * map it is possible to determine the exact location of the node in the 
   * streets map and the direction it is headed. Using this information
   * (geographical position and direction), of the local and neighbor nodes, it
   * can be determined if the neighbor node is a valid packet carrier or not.
   * 
   * But given that this process might take a long time to be computed depending
   * on the size of the streets map and the streets topology, in order to 
   * decrease the needed time to run the simulations, we pre-computed the
   * information where the nodes are in the streets map.
   * 
   * Using this pre-computed information,
   * <code>GpsSystem::IsVehicleValidPacketCarrier ()</code> computes (in a
   * seamless way) if a node is a valid packet carrier or not. This function 
   * only needs the identifiers (numeric ID or IP address) of the local node and
   * neighbor node, and the current simulation time to read from a file the
   * pre-computed information.
   * 
   * @param local_node_ip [IN] IP address of the local node.
   * @param local_position [IN] Geographical position of the local node.
   * @param local_velocity [IN] Velocity vector of the local node.
   * @param neighbor_node_ip [IN] IP address of the neighbor node.
   * @param neighbor_position [IN] Geographical position of the local node.
   * @param neighbor_velocity [IN] Velocity vector of the local node.
   * @param disjoint_vector [IN] Disjoint vector with the identifiers of the
   * requested packets.
   * @param selected_packet [OUT] The packet selected to be transmitted.
   * 
   * @return <code>true</code> if a packet is found, otherwise <code>false</code>.
   */
  bool
  Dequeue (const Ipv4Address & local_node_ip,
           const Vector2D & local_position,
           const Vector2D & local_velocity,
           const Ipv4Address & neighbor_node_ip,
           const Vector2D & neighbor_position,
           const Vector2D & neighbor_velocity,
           const std::set<DataIdentifier> & disjoint_vector,
           PacketQueueEntry & selected_packet);

private:

  /**
   * Discounts one packet replica from the specified data packet.
   * 
   * @Throws
   * If a packet queue entry with the specified data ID is found and its
   * replicas count is zero it throws an exception of type
   * <code>std::runtime_error</code>.
   * 
   * @param packet_data_id [IN] Data packet identifier of the packet to discount
   * the replica to.
   * 
   * @return <code>true</code> if the specified packet is found, otherwise
   * <code>false</code>.
   */
  bool
  DiscountPacketReplica (const DataIdentifier & packet_data_id);


public:

  /**
   * Adds the IP address of a node that we know that carries the data packet
   * specified by the given data packet identifier.
   * 
   * @param packet_data_id [IN] Data packet identifier of the packet to add a 
   * known carrier.
   * @param carrier_node_ip [IN] IP address of the known carrier.
   * 
   * @return <code>true</code> if the specified packet is found, otherwise
   * <code>false</code>.
   */
  bool
  AddKnownPacketCarrier (const DataIdentifier & packet_data_id,
                         const Ipv4Address & carrier_node_ip);


public:

  /**
   * Removes all expired data packet entries from the data packets queue.
   */
  void
  Purge ();


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
  friend test::PacketsQueueTest;
};

// PacketsQueue relational operators

inline bool
operator== (const PacketsQueue & lhs, const PacketsQueue & rhs)
{
  return lhs.m_gps == rhs.m_gps
          && lhs.m_max_queue_length == rhs.m_max_queue_length
          && lhs.m_max_replicas_counter == rhs.m_max_replicas_counter
          && lhs.m_packets_dropped_counter == rhs.m_packets_dropped_counter
          && lhs.m_min_vehicles_distance_diff == rhs.m_min_vehicles_distance_diff
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

} // namespace geotemporal
} // namespace ns3

#endif /* GEOTEMPORAL_PACKETS_QUEUE_H */

