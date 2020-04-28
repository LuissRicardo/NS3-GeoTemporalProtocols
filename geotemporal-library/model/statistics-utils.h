/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_STATISTICS_UTILS_H
#define UTILS_STATISTICS_UTILS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include <ns3/ipv4-address.h>
#include <ns3/nstime.h>

#include "geotemporal-utils.h"
#include "gps-system.h"
#include "math-utils.h"
#include "packet-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                  PacketClass
// =============================================================================

/**
 * Indicates the class of a packet: data or control.
 */
enum class PacketClass : uint8_t
{
  Data = 0, //!< The packet is a data packet.
  Control = 1 //!< The packet is a control packet.
};


// =============================================================================
//                                PacketsCounter
// =============================================================================

/**
 * Keeps the count of the number of control and data packets, and its size in
 * bytes as well.
 */
class PacketsCounter
{
private:

  /**
   * Counter of data packets.
   */
  uint32_t m_data_packets_count;

  /**
   * Counter of data packets size (in bytes).
   */
  uint32_t m_data_packets_size;

  /**
   * Counter of control packets.
   */
  uint32_t m_control_packets_count;

  /**
   * Counter of control packets size (in bytes).
   */
  uint32_t m_control_packets_size;

public:

  PacketsCounter ();

  PacketsCounter (const PacketsCounter & copy);

  /**
   * Returns the count of data packets.
   */
  inline uint32_t
  GetDataPacketsCount () const
  {
    return m_data_packets_count;
  }

  /**
   * Returns the sum of the size (in bytes) of all counted data packets.
   */
  inline uint32_t
  GetDataPacketsSize () const
  {
    return m_data_packets_size;
  }

  /**
   * Returns the count of control packets.
   */
  inline uint32_t
  GetControlPacketsCount () const
  {
    return m_control_packets_count;
  }

  /**
   * Returns the sum of the size (in bytes) of all counted control packets.
   */
  inline uint32_t
  GetControlPacketsSize () const
  {
    return m_control_packets_size;
  }

  /**
   * Returns the count of both data and control packets.
   */
  inline uint32_t
  GetCombinedPacketsCount () const
  {
    return m_data_packets_count + m_control_packets_count;
  }

  /**
   * Returns the sum of the size (in bytes) of all counted packets (both data
   * and control packets).
   */
  inline uint32_t
  GetCombinedPacketsSize () const
  {
    return m_data_packets_size + m_control_packets_size;
  }

  void
  CountPacket (PacketClass packet_class, uint32_t packet_size);

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const PacketsCounter & lhs, const PacketsCounter & rhs);
  friend bool operator< (const PacketsCounter & lhs, const PacketsCounter & rhs);
};

// PacketsCounter relational operators

inline bool
operator== (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  return lhs.m_data_packets_count == rhs.m_data_packets_count
          && lhs.m_data_packets_size == rhs.m_data_packets_size
          && lhs.m_control_packets_count == rhs.m_control_packets_count
          && lhs.m_control_packets_size == rhs.m_control_packets_size;
}

inline bool
operator!= (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  uint32_t lhs_packets_count = lhs.GetCombinedPacketsCount ();
  uint32_t rhs_packets_count = rhs.GetCombinedPacketsCount ();

  if (lhs_packets_count != rhs_packets_count)
    return lhs_packets_count < rhs_packets_count;

  uint32_t lhs_packets_size = lhs.GetCombinedPacketsSize ();
  uint32_t rhs_packets_size = rhs.GetCombinedPacketsSize ();

  if (lhs_packets_size != rhs_packets_size)
    return lhs_packets_size < rhs_packets_size;

  if (lhs.m_data_packets_count != rhs.m_data_packets_count)
    return lhs.m_data_packets_count < rhs.m_data_packets_count;

  return lhs.m_data_packets_size < rhs.m_data_packets_size;
}

inline bool
operator> (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const PacketsCounter & lhs, const PacketsCounter & rhs)
{
  return !operator< (lhs, rhs);
}

// PacketsCounter stream operators

inline std::ostream &
operator<< (std::ostream & os, const PacketsCounter & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                TransmissionType
// =============================================================================

/**
 * Indicates the via that a packet was transmitted: unicast or broadcast.
 */
enum class TransmissionType : uint8_t
{
  Unicast = 0, //!< The packet was transmitted via unicast.
  Broadcast = 1 //!< The packet was transmitted via broadcast.
};


// =============================================================================
//                            DataPacketReceptionStats
// =============================================================================

/**
 * Contains the information of a received DATA packet.
 */
class DataPacketReceptionStats
{
private:

  /**
   * Data identifier of the received data packet.
   */

  DataIdentifier m_received_packet_data_id;

  /**
   * IP address of the node that transmitted the packet. Can be seen as "received from".
   */
  ns3::Ipv4Address m_transmitter_node_ip;

  /**
   * Time that the packet was received.
   */
  ns3::Time m_reception_time;

  /**
   * Indicates if the packet was transmitted with the receiver node as destination node.
   *
   * <code>true</code> indicates that the packet was transmitted specifically to me. Otherwise,
   * <code>false</code>.
   */
  bool m_packet_destined_to_receiver_node;

  /**
   * Number of packet replicas transmitted via unicast.
   */
  uint32_t m_unicast_transmitted_replicas_count;

  /**
   * Number of packet replicas transmitted via broadcast.
   */
  uint32_t m_broadcast_transmitted_replicas_count;

  /**
   * Number of packet duplicates received.
   */
  uint32_t m_received_duplicates_count;

  /**
   * Indicates if the packet was dropped from storage.
   */
  bool m_packet_dropped;

public:

  DataPacketReceptionStats ();

  DataPacketReceptionStats (const DataIdentifier & data_id, const ns3::Ipv4Address & transmitter_ip,
                            const ns3::Time & reception_time, bool packet_destined_to_me);

  DataPacketReceptionStats (const DataPacketReceptionStats & copy);

  /**
   * Returns the data identifier of the received data packet.
   */
  inline const DataIdentifier &
  GetPacketDataIdentifier () const
  {
    return m_received_packet_data_id;
  }

  /**
   * Returns the IP address of the node that transmitted the packet. Can be seen as "received from".
   */
  inline const ns3::Ipv4Address &
  GetTransmitterIpAddress () const
  {
    return m_transmitter_node_ip;
  }

  /**
   * Returns time that the packet was received.
   */
  inline const ns3::Time &
  GetReceptionTime () const
  {
    return m_reception_time;
  }

  /**
   * Returns <code>true</code> if the packet was transmitted with the receiver node as destination node.
   * Otherwise, <code>false</code>.
   */
  inline bool
  GetPacketDestinedToReceiverNode () const
  {
    return m_packet_destined_to_receiver_node;
  }

  /**
   * Returns the number of packet replicas transmitted.
   */
  inline uint32_t
  GetTransmittedReplicasCount () const
  {
    return m_unicast_transmitted_replicas_count + m_broadcast_transmitted_replicas_count;
  }

  /**
   * Returns the number of packet replicas transmitted via unicast.
   */
  inline uint32_t
  GetUnicastTransmittedReplicasCount () const
  {
    return m_unicast_transmitted_replicas_count;
  }

  /**
   * Returns the number of packet replicas transmitted via broadcast.
   */
  inline uint32_t
  GetBroadcastTransmittedReplicasCount () const
  {
    return m_broadcast_transmitted_replicas_count;
  }

  /**
   * Increments the number of transmitted packet replicas. It also increments the number of replicas
   * transmitted using the specified transmission type.
   * @param transmission_type The via in which the packet replica was transmitted.
   */
  void
  IncrementTransmittedReplicasCount (const TransmissionType & transmission_type);

  /**
   * Return the number of packet duplicates received.
   */
  inline uint32_t
  GetReceivedDuplicatesCount () const
  {
    return m_received_duplicates_count;
  }

  /**
   * Increments the number of packet duplicates received.
   */
  inline void
  IncrementReceivedDuplicatesCount ()
  {
    ++m_received_duplicates_count;
  }

  /**
   * Returns <code>true</code> if the packet was dropped from storage, otherwise <code>false</code>.
   */
  inline bool
  GetPacketDropped () const
  {
    return m_packet_dropped;
  }

  /**
   * Set that the packet was dropped.
   */
  inline void
  SetPacketDropped ()
  {
    m_packet_dropped = true;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs);
  friend bool operator< (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs);
};

// DataPacketReceptionStats relational operators

inline bool
operator== (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  return lhs.m_received_packet_data_id == rhs.m_received_packet_data_id
          && lhs.m_transmitter_node_ip == rhs.m_transmitter_node_ip
          && lhs.m_reception_time == rhs.m_reception_time
          && lhs.m_packet_destined_to_receiver_node == rhs.m_packet_destined_to_receiver_node
          && lhs.m_unicast_transmitted_replicas_count == rhs.m_unicast_transmitted_replicas_count
          && lhs.m_broadcast_transmitted_replicas_count == rhs.m_broadcast_transmitted_replicas_count
          && lhs.m_received_duplicates_count == rhs.m_received_duplicates_count
          && lhs.m_packet_dropped == rhs.m_packet_dropped;
}

inline bool
operator!= (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  if (lhs.m_received_packet_data_id != rhs.m_received_packet_data_id)
    return lhs.m_received_packet_data_id < rhs.m_received_packet_data_id;

  if (lhs.m_reception_time != rhs.m_reception_time)
    return lhs.m_reception_time < rhs.m_reception_time;

  if (lhs.m_transmitter_node_ip != rhs.m_transmitter_node_ip)
    return lhs.m_transmitter_node_ip < rhs.m_transmitter_node_ip;

  return lhs.m_packet_destined_to_receiver_node < rhs.m_packet_destined_to_receiver_node;
}

inline bool
operator> (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const DataPacketReceptionStats & lhs, const DataPacketReceptionStats & rhs)
{
  return !operator< (lhs, rhs);
}

// DataPacketReceptionStats stream operators

inline std::ostream &
operator<< (std::ostream & os, const DataPacketReceptionStats & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                           DataPacketStatistics
// =============================================================================

/**
 * This class calculates the statistics of a DATA packet.
 */
class DataPacketStatistics
{
protected:

  /** Identifier of the data packet (source node IP + packet SEQ number). */
  DataIdentifier m_data_id;

  /** Numeric ID of the source node. */
  uint32_t m_source_node_id;

  /** Time when the packet was created. */
  ns3::Time m_packet_creation_time;

  /** 
   * Size (in bytes) of ONLY the message field of the data packet (in other 
   * words, not counting other fields of the data packet).
   */
  uint32_t m_packet_message_size;

  /** 
   * Size (in bytes) of the real entire packet (including headers and trailers like 
   * IP, UDP, etc). 
   */
  uint32_t m_packet_size;

  /** The destination geo-temporal area of the data packet. */
  LibraryUtils::GeoTemporalArea m_destination_geo_temporal_area;

  /**
   * Indicates if the set of nodes that should receive the packet is set 
   * (<code>true</code>), otherwise <code>false</code>.
   */
  bool m_expected_receiver_nodes_set_flag;

  /**
   * This maps all the nodes that visited the packet's geo-temporal area to their
   * respective arrival time to the area.
   *
   * The mapping is: <code>Visitor node IP address --> node's area arrival time</code>.
   */
  std::map<ns3::Ipv4Address, ns3::Time> m_expected_receiver_nodes_map;

  /**
   * The set of IP addresses of all expected receiver nodes that have been 
   * processed.
   */
  std::set<ns3::Ipv4Address> m_processed_receiver_nodes_ips;

  /**
   * The set of IP addresses of all expected receiver nodes that are confirmed 
   * that received the packet.
   */
  std::set<ns3::Ipv4Address> m_confirmed_receiver_nodes_ips;

  /** The list of all delivery delays. */
  std::vector<double> m_processed_delivery_delay_list;


public:

  DataPacketStatistics ();

  /**
   * Initializes the object.
   * 
   * @param data_id Unique identifier of the data packet.
   * @param source_node_id Numeric identifier of the node that created the packet.
   * @param creation_time Simulation time when the packet was created.
   * @param message_size Size (in bytes) of the message of the packet.
   * @param data_packet_size Size (in bytes) of the entire DATA packet.
   * @param destination_geo_temporal_area Destination geo-temporal area of the 
   * packet.
   */
  DataPacketStatistics (const DataIdentifier & data_id,
                        uint32_t source_node_id,
                        const ns3::Time & creation_time,
                        uint32_t message_size,
                        uint32_t data_packet_size,
                        const LibraryUtils::GeoTemporalArea & destination_geo_temporal_area);

  DataPacketStatistics (const DataPacketStatistics & copy);

  /**
   * Calculates the size (in bytes) of the entire real packet (DATA header, UDP
   * header, IP header, IEEE 802.11 header, LCC header and frame end).
   * 
   * @param data_header_size Size (in bytes) of the entire DATA header.
   * 
   * @return The size (in bytes) of the entire real packet.
   */
  static inline uint32_t
  CalculateRealPacketSize (uint32_t data_header_size)
  {
    return data_header_size // Entire data packet size
            + 24 + 8 // IEEE 802.11 header + LCC header
            + 20 + 8 + 4; // + IP header + UDP header + Frame end.
  }

  /** Returns the identifier of the data packet. */
  inline const DataIdentifier &
  GetDataIdentifier () const
  {
    return m_data_id;
  }

  /** Returns the IPv4 address of the source node. */
  inline const ns3::Ipv4Address &
  GetSourceNodeIp () const
  {
    return m_data_id.GetSourceIp ();
  }

  /** Returns the numeric ID of the source node. */
  inline uint32_t
  GetSourceNodeId () const
  {
    return m_source_node_id;
  }

  /** Returns the time (in seconds) when the packet was created. */
  inline const ns3::Time &
  GetPacketCreationTime () const
  {
    return m_packet_creation_time;
  }

  /** Returns the size (in bytes) of the packet's data message. */
  inline uint32_t
  GetPacketMessageSize () const
  {
    return m_packet_message_size;
  }

  /** Returns the size (in bytes) of the whole packet (including headers and trailers). */
  inline uint32_t
  GetPacketSize () const
  {
    return m_packet_size;
  }

  /**
   * Returns the destination geo-temporal area of the data packet.
   */
  inline const LibraryUtils::GeoTemporalArea &
  GetPacketDestinationGeoTemporalArea () const
  {
    return m_destination_geo_temporal_area;
  }

  /**
   * Returns the number of nodes that are expected to receive the packet.
   */
  inline uint32_t
  GetExpectedReceiverNodesCount () const
  {
    return m_expected_receiver_nodes_map.size ();
  }

  /**
   * Returns the number of nodes that are confirmed receivers of the packet.
   */
  inline uint32_t
  GetConfirmedReceiverNodesCount () const
  {
    return m_confirmed_receiver_nodes_ips.size ();
  }

  /**
   * Indicates if the set of nodes that should receive the packet is set (<code>true</code>),
   * otherwise <code>false</code>.
   */
  inline bool
  IsExpectedReceiverNodesSet () const
  {
    return m_expected_receiver_nodes_set_flag;
  }

  /**
   * Erases all elements in the set of nodes that are expected to receive the packet.
   *
   * It also erases all the collected data that may have been added using
   * <code>CountReception (const uint32_t & receiver_node_id, const DataPacketReceptionStats & reception_stats)</code>.
   */
  void
  ClearExpectedReceiverNodes ();

  /**
   * Sets all the valid receiver nodes contained in the specified map of candidate
   * receiver nodes as the set of expected receiver nodes. 
   * 
   * Invalid nodes are those that didn't arrived to the destination geo-temporal
   * area during its active temporal scope and the packet's source node itself.
   * 
   * It returns the number of valid nodes found in the map of candidate receiver
   * nodes and set as expected receiver nodes.
   * 
   * To avoid incongruities, before doing anything else, it calls 
   * <code>DataPacketStatistics::ClearExpectedReceiverNodes ()</code>
   * to delete the current set of expected receiver nodes. After this call it 
   * sets the valid receiver nodes. So all the desired nodes to be set as expected
   * received nodes must be set in one call to this function.
   * 
   * @param candidate_receiver_nodes [IN] Map of candidate receiver nodes that 
   * contains the node's IP address and its arrival time to the destination 
   * geo-temporal area.
   * 
   * @return The number of valid expected receiver nodes set.
   */
  uint32_t
  SetExpectedReceiverNodes (const std::map<ns3::Ipv4Address, ns3::Time> & candidate_receiver_nodes);

  /**
   * Counts the information about the DATA packet reception.
   *
   * Returns <code>true</code> if the processed packet reception is from an expected receiver node,
   * and therefore it counts. If the packet reception is not from an expected receiver node then it
   * returns <code>false</code> and it is not counted.
   * 
   * @param receiver_node_ip IP address of the node that received the DATA packet.
   * @param reception_stats Object that contains information about the packet reception (reception
   * time, transmitter node, etc).
   *
   * @return <code>true</code> if packet reception from a expected receiver node, otherwise
   * <code>false</code>.
   */
  bool
  CountReception (const ns3::Ipv4Address & receiver_node_ip, const DataPacketReceptionStats & reception_stats);

  /**
   * Computes the final statistics using all the collected reception data that was
   * entered using the <code>CountReception(const uint32_t &, const DataPacketReceptionStats &)</code>
   * function.
   *
   * @param confirmed_packet_receivers_count [OUT] Number of nodes that actually received the packet.
   * @param expected_packet_receivers_count [OUT] Number of nodes expected to receive the packet.
   * @param packet_delivery_ratio [OUT] Computed delivery ratio.
   * @param packet_average_delivery_delay [OUT] Average delivery delay of all entered receptions.
   * @param delivered_data_bytes [OUT] Sum of the size (in bytes) of all the data packets that were
   * successfully delivered to the expected destination nodes.
   */
  void
  GetStatistics (uint32_t & confirmed_packet_receivers_count,
                 uint32_t & expected_packet_receivers_count,
                 double & packet_delivery_ratio,
                 double & packet_average_delivery_delay,
                 uint32_t & delivered_data_bytes) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  virtual std::string ToString () const;

  virtual void Print (std::ostream & os) const;

  friend bool operator== (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs);
  friend bool operator< (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs);
};

// DataPacketStatistics relational operators

inline bool
operator== (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return lhs.m_data_id == rhs.m_data_id
          && lhs.m_source_node_id == rhs.m_source_node_id
          && lhs.m_packet_creation_time == rhs.m_packet_creation_time
          && lhs.m_packet_message_size == rhs.m_packet_message_size
          && lhs.m_packet_size == rhs.m_packet_size
          && lhs.m_destination_geo_temporal_area == rhs.m_destination_geo_temporal_area
          && lhs.m_expected_receiver_nodes_set_flag == rhs.m_expected_receiver_nodes_set_flag
          && lhs.m_expected_receiver_nodes_map == rhs.m_expected_receiver_nodes_map
          && lhs.m_processed_receiver_nodes_ips == rhs.m_processed_receiver_nodes_ips
          && lhs.m_confirmed_receiver_nodes_ips == rhs.m_confirmed_receiver_nodes_ips
          && lhs.m_processed_delivery_delay_list == rhs.m_processed_delivery_delay_list;
}

inline bool
operator!= (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return lhs.m_data_id < rhs.m_data_id;
}

inline bool
operator> (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const DataPacketStatistics & lhs, const DataPacketStatistics & rhs)
{
  return !operator< (lhs, rhs);
}

// DataPacketStatistics stream operators

inline std::ostream &
operator<< (std::ostream & os, const DataPacketStatistics & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                          SimulationStatisticsValues
// =============================================================================

/**
 * Contains the statistic values computed by an SimulationStatistics object.
 */
class SimulationStatisticsValues
{
public:

  double m_average_delivery_delay;
  double m_average_delivery_ratio;
  double m_total_overhead;
  double m_data_overhead;
  double m_control_overhead;
  uint64_t m_total_transmitted_bytes;
  uint64_t m_data_transmitted_bytes;
  uint64_t m_control_transmitted_bytes;
  uint64_t m_total_delivered_data_bytes;
  uint32_t m_expected_receivers;
  uint32_t m_confirmed_receivers;


  SimulationStatisticsValues ();

  SimulationStatisticsValues (const SimulationStatisticsValues & copy);


  /**
   * Initializes the values of the object to an "empty (or clean) state".
   * 
   * Integer values to zero.
   * 
   * Double values to -1.0.
   */
  virtual void InitializeValues ();

  friend bool operator== (const SimulationStatisticsValues & lhs,
                          const SimulationStatisticsValues & rhs);
};

// SimulationStatisticsValues relational operators

inline bool
operator== (const SimulationStatisticsValues & lhs, const SimulationStatisticsValues & rhs)
{
  return AlmostEqual (lhs.m_average_delivery_delay, rhs.m_average_delivery_delay)
          && AlmostEqual (lhs.m_average_delivery_ratio, rhs.m_average_delivery_ratio)
          && AlmostEqual (lhs.m_total_overhead, rhs.m_total_overhead)
          && AlmostEqual (lhs.m_data_overhead, rhs.m_data_overhead)
          && AlmostEqual (lhs.m_control_overhead, rhs.m_control_overhead)
          && lhs.m_total_transmitted_bytes == rhs.m_total_transmitted_bytes
          && lhs.m_data_transmitted_bytes == rhs.m_data_transmitted_bytes
          && lhs.m_control_transmitted_bytes == rhs.m_control_transmitted_bytes
          && lhs.m_total_delivered_data_bytes == rhs.m_total_delivered_data_bytes
          && lhs.m_expected_receivers == rhs.m_expected_receivers
          && lhs.m_confirmed_receivers == rhs.m_confirmed_receivers;
}

inline bool
operator!= (const SimulationStatisticsValues & lhs, const SimulationStatisticsValues & rhs)
{
  return !operator== (lhs, rhs);
}


// =============================================================================
//                              SimulationStatistics
// =============================================================================

/**
 * Computes the statistics of the current simulation.
 */
class SimulationStatistics
{
protected:

  std::map<ns3::Ipv4Address, PacketsCounter> m_nodes_transmitted_packets_counters;

  std::map<DataIdentifier, DataPacketStatistics> m_data_packets_statistics;

  NavigationSystem::GeoTemporalAreasVisitorNodes m_gta_visitor_nodes;

  std::map<uint32_t, ns3::Ipv4Address> m_nodes_id_to_ip;

  std::map<ns3::Ipv4Address, uint32_t> m_nodes_ip_to_id;

  std::set<LibraryUtils::Area> m_known_destination_areas;


public:

  SimulationStatistics ();

  SimulationStatistics (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes,
                        const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  SimulationStatistics (const std::string & gta_visitor_nodes_input_filename,
                        const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  SimulationStatistics (const SimulationStatistics & copy);


protected:

  void
  SetUp ();


public:

  /** Returns the IP address of the node with the given node ID. */
  const ns3::Ipv4Address &
  GetNodeIpAddressFromId (const uint32_t node_id) const;

  /** Returns the node ID of the node with the given IP address. */
  uint32_t
  GetNodeIdFromIpAddress (const ns3::Ipv4Address & node_ip) const;

  /** Returns the set of known destination areas. */
  inline const std::set<LibraryUtils::Area> &
  GetDestinationAreas () const
  {
    return m_known_destination_areas;
  }

  /**
   * Returns a constant reference to the desired DATA packet.
   *
   * If a packet with the given DATA ID doesn't exist it throws an <code>std::out_of_range</code>
   * exception.
   *
   * @param packet_data_id ID of the packet to retrieve.
   */
  const DataPacketStatistics &
  GetDataPacketStatistics (const DataIdentifier & packet_data_id) const;



  /** Adds a data packet. */
  virtual void
  AddDataPacket (const DataPacketStatistics & packet_statistics);

  /**
   * Counts the information about the DATA packet reception.
   *
   * Returns <code>true</code> if the processed packet reception is from an 
   * expected receiver node, and therefore it counts. If the packet reception is
   * not from an expected receiver node then it returns <code>false</code> and 
   * it is not counted.
   *
   * @param receiver_node_ip IP address of the node that received the DATA packet.
   * @param reception_stats Object that contains information about the packet 
   * reception (reception time, transmitter node, etc).
   *
   * @return <code>true</code> if packet reception from a expected receiver node, 
   * otherwise <code>false</code>.
   */
  virtual bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const DataPacketReceptionStats & reception_stats);

  /** Sets the counter of transmitted packets of the specified node. */
  void
  SetNodeTransmittedPacketsCounter (const ns3::Ipv4Address & node_ip,
                                    const PacketsCounter & node_tx_packets_counter);

  /**
   * Computes the final statistics using the collected reception data of all
   * entered DATA packets.
   *
   * If there are no data packets in the object then it returns <code>false</code>,
   * because there's no statistics to calculate.
   * 
   * The <code>values</code> object is always initialized to its default values.
   */
  bool
  CalculateStatistics (SimulationStatisticsValues & values) const;

  /**
   * Computes the final statistics using the collected reception data of the
   * DATA packets that have the specified area as destination area.
   *
   * If there are no data packets in the object or there are none data packets
   * with the specified destination area then it returns <code>false</code>,
   * because there's no statistics to calculate.
   * 
   * The <code>values</code> object is always initialized to its default values.
   */
  bool
  CalculateAreaStatistics (const LibraryUtils::Area & destination_area,
                           SimulationStatisticsValues & values) const;
};


// =============================================================================
//                         SimulationStatisticsFile
// =============================================================================

/**
 * Computes the statistics of the current simulation and saves the data and
 * results to a text file.
 */
class SimulationStatisticsFile : public SimulationStatistics
{
private:

  /** Used to store the XML string of confirmed receivers of each data packet. */
  std::map<DataIdentifier, std::string> m_data_packets_str_section;


public:

  SimulationStatisticsFile ();

  SimulationStatisticsFile (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes,
                            const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  SimulationStatisticsFile (const std::string & gta_visitor_nodes_input_filename,
                            const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  SimulationStatisticsFile (const SimulationStatisticsFile & copy);


  void
  AddDataPacket (const DataPacketStatistics & packet_statistics) override;

  bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const DataPacketReceptionStats & reception_stats) override;

  /** Stores in the given file the resulting statistics in XML format. */
  void
  SaveToXmlFile (const std::string & output_filename) const;
};


// =============================================================================
//                         PriorityDataPacketStatistics
// =============================================================================

/**
 * This class calculates the statistics of a DATA packet with a emergency flag.
 */
class PriorityDataPacketStatistics : public DataPacketStatistics
{
private:

  /**
   * If enabled, it indicates that the packet has high priority, is an emergency
   * packet. Otherwise, when disabled, it indicates that the packet doesn't have
   * high priority.
   */
  bool m_emergency_flag;


public:

  PriorityDataPacketStatistics ();

  /**
   * Initializes the object.
   * 
   * @param data_id Unique identifier of the data packet.
   * @param emergency_flag Indicates if the packet is an emergency packet.
   * @param source_node_id Numeric identifier of the node that created the packet.
   * @param creation_time Simulation time when the packet was created.
   * @param message_size Size (in bytes) of the message of the packet.
   * @param data_packet_size Size (in bytes) of the entire DATA packet.
   * @param destination_geo_temporal_area Destination geo-temporal area of the 
   * packet.
   */
  PriorityDataPacketStatistics (const DataIdentifier & data_id,
                                const bool emergency_flag,
                                uint32_t source_node_id,
                                const ns3::Time & creation_time,
                                uint32_t message_size,
                                uint32_t data_packet_size,
                                const LibraryUtils::GeoTemporalArea & destination_geo_temporal_area);

  PriorityDataPacketStatistics (const PriorityDataPacketStatistics & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const bool
  IsEmergencyPacket () const
  {
    return m_emergency_flag;
  }


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const override;

  void Print (std::ostream & os) const override;

  friend bool operator== (const PriorityDataPacketStatistics & lhs,
                          const PriorityDataPacketStatistics & rhs);
  friend bool operator< (const PriorityDataPacketStatistics & lhs,
                         const PriorityDataPacketStatistics & rhs);
};

// PriorityDataPacketStatistics relational operators

inline bool
operator== (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return ((DataPacketStatistics) lhs) == ((DataPacketStatistics) rhs)
          && lhs.m_emergency_flag == rhs.m_emergency_flag;
}

inline bool
operator!= (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return ((DataPacketStatistics) lhs) < ((DataPacketStatistics) rhs);
}

inline bool
operator> (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const PriorityDataPacketStatistics & lhs, const PriorityDataPacketStatistics & rhs)
{
  return !operator< (lhs, rhs);
}

// PriorityDataPacketStatistics stream operators

inline std::ostream &
operator<< (std::ostream & os, const PriorityDataPacketStatistics & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                      PrioritySimulationStatisticsValues
// =============================================================================

/**
 * Contains the statistic values computed by an PrioritySimulationStatistics
 * object.
 */
class PrioritySimulationStatisticsValues : public SimulationStatisticsValues
{
public:

  double m_emergency_average_delivery_delay;
  double m_emergency_average_delivery_ratio;
  double m_normal_average_delivery_delay;
  double m_normal_average_delivery_ratio;


  PrioritySimulationStatisticsValues ();

  PrioritySimulationStatisticsValues (const PrioritySimulationStatisticsValues & copy);

  /**
   * Initializes the values of the object to an "empty (or clean) state".
   * 
   * Integer values to zero.
   * 
   * Double values to -1.0.
   */
  void InitializeValues () override;

  friend bool operator== (const PrioritySimulationStatisticsValues & lhs,
                          const PrioritySimulationStatisticsValues & rhs);
};

// PrioritySimulationStatisticsValues relational operators

inline bool
operator== (const PrioritySimulationStatisticsValues & lhs, const PrioritySimulationStatisticsValues & rhs)
{
  return ((SimulationStatisticsValues) lhs) == ((SimulationStatisticsValues) rhs)
          && AlmostEqual (lhs.m_emergency_average_delivery_delay, rhs.m_emergency_average_delivery_delay)
          && AlmostEqual (lhs.m_emergency_average_delivery_ratio, rhs.m_emergency_average_delivery_ratio)
          && AlmostEqual (lhs.m_normal_average_delivery_delay, rhs.m_normal_average_delivery_delay)
          && AlmostEqual (lhs.m_normal_average_delivery_ratio, rhs.m_normal_average_delivery_ratio);
}

inline bool
operator!= (const PrioritySimulationStatisticsValues & lhs, const PrioritySimulationStatisticsValues & rhs)
{
  return !operator== (lhs, rhs);
}


// =============================================================================
//                         PrioritySimulationStatistics
// =============================================================================

/**
 * Computes the statistics of the current simulation with priority packets.
 */
class PrioritySimulationStatistics : public SimulationStatistics
{
protected:

  std::map<DataIdentifier, PriorityDataPacketStatistics> m_priority_data_packet_statistics;


public:

  PrioritySimulationStatistics ();

  PrioritySimulationStatistics (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes,
                                const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  PrioritySimulationStatistics (const std::string & gta_visitor_nodes_input_filename,
                                const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  PrioritySimulationStatistics (const PrioritySimulationStatistics & copy);



  /**
   * Returns a constant reference to the desired DATA packet.
   *
   * If a packet with the given DATA ID doesn't exist it throws an <code>std::out_of_range</code>
   * exception.
   *
   * @param packet_data_id ID of the packet to retrieve.
   */
  const PriorityDataPacketStatistics &
  GetDataPacketStatistics (const DataIdentifier & packet_data_id) const;



  /** Adds a priority data packet. */
  virtual void
  AddDataPacket (const PriorityDataPacketStatistics & priority_packet_statistics);

  /**
   * Counts the information about the DATA packet reception.
   *
   * Returns <code>true</code> if the processed packet reception is from an 
   * expected receiver node, and therefore it counts. If the packet reception is
   * not from an expected receiver node then it returns <code>false</code> and 
   * it is not counted.
   *
   * @param receiver_node_ip IP address of the node that received the DATA packet.
   * @param reception_stats Object that contains information about the packet 
   * reception (reception time, transmitter node, etc).
   *
   * @return <code>true</code> if packet reception from a expected receiver node, 
   * otherwise <code>false</code>.
   */
  virtual bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const DataPacketReceptionStats & reception_stats);

  /**
   * Computes the final statistics using the collected reception data of all
   * entered DATA packets.
   *
   * If there are no data packets in the object then it returns <code>false</code>,
   * because there's no statistics to calculate.
   * 
   * The <code>values</code> object is always initialized to its default values.
   */
  bool
  CalculateStatistics (PrioritySimulationStatisticsValues & values) const;

  /**
   * Computes the final statistics using the collected reception data of the
   * DATA packets that have the specified area as destination area.
   *
   * If there are no data packets in the object or there are none data packets
   * with the specified destination area then it returns <code>false</code>,
   * because there's no statistics to calculate.
   * 
   * The <code>values</code> object is always initialized to its default values.
   */
  bool
  CalculateAreaStatistics (const LibraryUtils::Area & destination_area,
                           PrioritySimulationStatisticsValues & values) const;
};


// =============================================================================
//                       PrioritySimulationStatisticsFile
// =============================================================================

/**
 * Computes the statistics of the current simulation and saves the data and
 * results to a text file.
 */
class PrioritySimulationStatisticsFile : public PrioritySimulationStatistics
{
private:

  /** Used to store the XML string of confirmed receivers of each data packet. */
  std::map<DataIdentifier, std::string> m_data_packets_str_section;


public:

  PrioritySimulationStatisticsFile ();

  PrioritySimulationStatisticsFile (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes,
                                    const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  PrioritySimulationStatisticsFile (const std::string & gta_visitor_nodes_input_filename,
                                    const std::map<uint32_t, ns3::Ipv4Address> & nodes_id_to_ip);

  PrioritySimulationStatisticsFile (const PrioritySimulationStatisticsFile & copy);


  void
  AddDataPacket (const PriorityDataPacketStatistics & priority_packet_statistics) override;

  bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const DataPacketReceptionStats & reception_stats) override;

  /** Stores in the given file the resulting statistics in XML format. */
  void
  SaveToXmlFile (const std::string & output_filename) const;
};


}
}

#endif //UTILS_STATISTICS_UTILS_H
