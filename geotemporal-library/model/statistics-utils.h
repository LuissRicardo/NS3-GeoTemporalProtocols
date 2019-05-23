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
#include "packet-utils.h"
#include "gps-system.h"

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
  GetReceivedPacketDataId () const
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
private:

  /**
   * Identifier of the data packet (source node IP + packet SEQ number).
   */
  DataIdentifier m_data_id;

  /**
   * Numeric ID of the source node.
   */
  uint32_t m_source_node_id;

  /**
   * Time when the packet was created.
   */
  ns3::Time m_packet_creation_time;

  /**
   * Size (in bytes) of the packet's data message.
   */
  uint32_t m_packet_data_size;

  /**
   * The destination geo-temporal area of the data packet.
   */
  LibraryUtils::GeoTemporalArea m_packet_geo_temporal_area;

  /**
   * Indicates if the set of nodes that should receive the packet is set (<code>true</code>),
   * otherwise <code>false</code>.
   */
  bool m_expected_receiver_nodes_set_flag;

  /**
   * This maps all the nodes that visited the packet's geo-temporal area to their
   * respective area arrival time.
   *
   * The mapping is: <code>Visitor node ID (key) --> node arrival time (value)</code>.
   */
  std::map<uint32_t, uint32_t> m_expected_receiver_nodes_gta_arrival_time_mapping;

  /**
   * The set of IDs of all the expected receiver nodes that have been processed.
   */
  std::set<uint32_t> m_stats_processed_receiver_nodes_ids_set;

  /**
   * The set of IDs of all the nodes that are confirmed receivers of the packet.
   */
  std::set<uint32_t> m_stats_confirmed_receiver_nodes_ids_set;

  /**
   * The list of all delivery delays.
   */
  std::vector<double> m_stats_delivery_delay_list;

public:

  DataPacketStatistics ();

  DataPacketStatistics (const DataIdentifier & data_id, uint32_t source_node_id,
                        const ns3::Time & creation_time, uint32_t data_size,
                        const LibraryUtils::GeoTemporalArea & geo_temporal_area);

  DataPacketStatistics (const DataPacketStatistics & copy);

  /**
   * Returns the identifier of the data packet.
   */
  inline const DataIdentifier &
  GetDataIdentifier () const
  {
    return m_data_id;
  }

  /**
   * Returns the IPv4 address of the source node.
   */
  inline const ns3::Ipv4Address &
  GetSourceNodeIp () const
  {
    return m_data_id.GetSourceIp ();
  }

  /**
   * Returns the numeric ID of the source node.
   */
  inline uint32_t
  GetSourceNodeId () const
  {
    return m_source_node_id;
  }

  /**
   * Returns the time (in seconds) when the packet was created.
   */
  inline const ns3::Time &
  GetPacketCreationTime () const
  {
    return m_packet_creation_time;
  }

  /**
   * Returns the size (in bytes) of the packet's data message.
   */
  inline uint32_t
  GetPacketDataSize () const
  {
    return m_packet_data_size;
  }

  /**
   * Returns the destination geo-temporal area of the data packet.
   */
  inline const LibraryUtils::GeoTemporalArea &
  GetPacketDestinationGeoTemporalArea () const
  {
    return m_packet_geo_temporal_area;
  }

  /**
   * Returns the number of nodes that are expected to receive the packet.
   */
  inline uint32_t
  GetExpectedReceiverNodesCount () const
  {
    return m_expected_receiver_nodes_gta_arrival_time_mapping.size ();
  }

  /**
   * Returns the number of nodes that are confirmed receivers of the packet.
   */
  inline uint32_t
  GetConfirmedReceiverNodesCount () const
  {
    return m_stats_confirmed_receiver_nodes_ids_set.size ();
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
   * Sets the set of nodes that visited the packet's geo-temporal area. Each item is
   * a <code>VisitorNode</object> object that contains the ID of the node and its
   * arrival time. This set can be viewed as the set of nodes that are expected to
   * receive the packet.
   *
   * If the arrival time of at least one visitor node occurs outside the period of
   * time of the geo-temporal area it will throw an exception and delete all elements
   * from the set of nodes that visited the packet's geo-temporal area. This is to
   * avoid undefined behavior. If this happens then the user can start over after
   * fixing the nodes' arrival time where needed.
   *
   * To avoid incongruities, it calls <code>ClearExpectedReceiverNodes ()</code> to
   * delete all data that may have been collected.
   */
  void
  SetExpectedReceiverNodes (const std::set<NavigationSystem::VisitorNode> & visitor_nodes_set);

  /**
   * Counts the information about the DATA packet reception.
   *
   * Returns <code>true</code> if the processed packet reception is from an expected receiver node,
   * and therefore it counts. If the packet reception is not from an expected receiver node then it
   * returns <code>false</code> and it is not counted.
   *
   * @param receiver_node_id Identifier of the node that received the DATA packet.
   * @param reception_stats Object that contains information about the packet reception (reception
   * time, transmitter node, etc).
   *
   * @return <code>true</code> if packet reception from a expected receiver node, otherwise
   * <code>false</code>.
   */
  bool
  CountReception (const uint32_t & receiver_node_id, const DataPacketReceptionStats & reception_stats);

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
          && lhs.m_packet_geo_temporal_area == rhs.m_packet_geo_temporal_area
          && lhs.m_packet_data_size == rhs.m_packet_data_size
          && lhs.m_expected_receiver_nodes_set_flag == rhs.m_expected_receiver_nodes_set_flag
          && lhs.m_expected_receiver_nodes_gta_arrival_time_mapping
          == rhs.m_expected_receiver_nodes_gta_arrival_time_mapping
          && lhs.m_stats_confirmed_receiver_nodes_ids_set == rhs.m_stats_confirmed_receiver_nodes_ids_set
          && lhs.m_stats_delivery_delay_list == rhs.m_stats_delivery_delay_list;
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
//                              SimulationStatistics
// =============================================================================

/**
 * Computes the statistics of the current simulation.
 */
class SimulationStatistics
{
protected:

  std::map<uint32_t, PacketsCounter> m_nodes_transmitted_packets_counters;

  std::map<DataIdentifier, DataPacketStatistics> m_data_packets_statistics;

  NavigationSystem::GeoTemporalAreasVisitorNodes m_gta_visitor_nodes;

  std::set<LibraryUtils::Area> m_known_destination_areas;

public:

  SimulationStatistics ();

  SimulationStatistics (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes);

  SimulationStatistics (const SimulationStatistics & copy);

  /**
   * Returns the set of known destination areas.
   */
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

  /**
   * Adds a data packet.
   *
   * The given <code>DataPacketStatistics</code> object is modified to set its
   * expected receiver nodes with the <code>GeoTemporalAreasVisitorNodes</code>
   * object given in the constructor
   * (<code>SimulationStatistics (const GeoTemporalAreasVisitorNodes & gta_visitor_nodes)</code>).
   */
  virtual void
  AddDataPacket (DataPacketStatistics & packet_statistics);

  /**
   * Counts the information about the DATA packet reception.
   *
   * Returns <code>true</code> if the processed packet reception is from an expected receiver node,
   * and therefore it counts. If the packet reception is not from an expected receiver node then it
   * returns <code>false</code> and it is not counted.
   *
   * @param receiver_node_ip IP address of the node that received the DATA packet.
   * @param receiver_node_id Identifier of the node that received the DATA packet.
   * @param reception_stats Object that contains information about the packet reception (reception
   * time, transmitter node, etc).
   *
   * @return <code>true</code> if packet reception from a expected receiver node, otherwise
   * <code>false</code>.
   */
  virtual bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const uint32_t receiver_node_id,
                               const DataPacketReceptionStats & reception_stats);

  /**
   * Sets the counter of transmitted packets of the specified node.
   */
  virtual void
  SetNodeTransmittedPacketsCounter (const ns3::Ipv4Address & node_ip,
                                    const uint32_t node_id,
                                    const PacketsCounter & node_tx_packets_counter);

  /**
   * Computes the final statistics using all the collected reception data of all
   * entered DATA packets.
   *
   * If there are no data packets in the object then it returns <code>false</code>,
   * because there's no statistics to calculate.
   */
  bool
  GetStatistics (double & average_delivery_delay, double & average_delivery_ratio,
                 double & total_overhead, double & data_overhead,
                 double & control_overhead, uint64_t & total_transmitted_bytes,
                 uint64_t & data_transmitted_bytes, uint64_t & control_transmitted_bytes,
                 uint64_t & total_delivered_data_bytes, uint32_t & expected_receivers,
                 uint32_t & confirmed_receivers) const;

  /**
   * Computes the final statistics using the collected reception data of the
   * DATA packets that have the specified area as destination area.
   *
   * If there are no data packets in the object or there are none data packets
   * with the specified destination area then it returns <code>false</code>,
   * because there's no statistics to calculate.
   */
  bool
  GetAreaStatistics (const LibraryUtils::Area & destination_area,
                     double & average_delivery_delay, double & average_delivery_ratio,
                     uint64_t & total_delivered_data_bytes, uint32_t & expected_receivers,
                     uint32_t & confirmed_receivers) const;
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

  /** Used to store the IP address of the node. */
  std::map<uint32_t, ns3::Ipv4Address> m_node_tx_packets_counter_ips;

public:

  SimulationStatisticsFile ();

  SimulationStatisticsFile (const NavigationSystem::GeoTemporalAreasVisitorNodes & gta_visitor_nodes);

  SimulationStatisticsFile (const SimulationStatisticsFile & copy);

  void AddDataPacket (DataPacketStatistics & packet_statistics) override;

  bool
  CountDataPacketReceiverNode (const ns3::Ipv4Address & receiver_node_ip,
                               const uint32_t receiver_node_id,
                               const DataPacketReceptionStats & reception_stats) override;

  void
  SetNodeTransmittedPacketsCounter (const ns3::Ipv4Address & node_ip,
                                    const uint32_t node_id,
                                    const PacketsCounter & node_tx_packets_counter) override;

  void
  SaveToXmlFile (const std::string & output_filename) const;

};

}
}

#endif //UTILS_STATISTICS_UTILS_H
