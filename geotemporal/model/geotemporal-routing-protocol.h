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
 * Geo-Temporal routing protocol.
 */

#ifndef GEOTEMPORAL_ROUTING_PROTOCOL_H
#define GEOTEMPORAL_ROUTING_PROTOCOL_H

#include <map>
#include <set>
#include <utility>
#include <vector>

#include "geotemporal-duplicate-detector.h"
#include "geotemporal-neighbors-table.h"
#include "geotemporal-packets-queue.h"
#include "geotemporal-packets.h"

#include <ns3/gps-system.h>
#include <ns3/packet-utils.h>
#include <ns3/statistics-utils.h>

#include <ns3/ipv4-address.h>
#include <ns3/ipv4-interface-address.h>
#include <ns3/ipv4-routing-protocol.h>
#include <ns3/net-device.h>
#include <ns3/nstime.h>
#include <ns3/ptr.h>
#include <ns3/random-variable-stream.h>
#include <ns3/socket.h>
#include <ns3/timer.h>
#include <ns3/type-id.h>


using namespace GeoTemporalLibrary::LibraryUtils;
using namespace GeoTemporalLibrary::NavigationSystem;

namespace ns3
{
namespace geotemporal
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;

// =============================================================================
//                                RoutingProtocol
// =============================================================================

class RoutingProtocol : public Ipv4RoutingProtocol
{
private:

  /**
   * UDP port used for the Geo-Temporal routing protocol.
   */
  static const uint32_t GEO_TEMPORAL_ROUTING_PROTOCOL_PORT = 49765;


  // ---------------------
  // Protocol parameters
  // ---------------------

  /** Time interval between HELLO transmissions. */
  Time m_hello_timeout;

  /** Hello timer. Controls when the HELLOs are transmitted. */
  Timer m_hello_timer;

  /** The maximum number of packets that we allow a routing protocol to buffer. */
  uint32_t m_packets_queue_capacity;

  /** Time interval in which a recently contacted neighbor node is not contacted
   * again. */
  Time m_neighbor_expiration_time;

  /** Number of replicas of each data packet allowed to be transmitted. */
  uint32_t m_default_data_packet_replicas;

  /** Minimum distance (in meters) difference between nodes to be valid packet carriers. */
  double m_neighbor_min_valid_distance_diff;

  /** Size of the time-slot to calculate exponential average. */
  Time m_exponential_average_time_slot_size;

  /** Exponential average timer. Controls when the exponential average is calculated. */
  Timer m_exponential_average_timer;


  // ---------------------
  // Internal variables
  // ---------------------

  /** ID of the node. */
  int32_t m_node_id;

  /** IP protocol. */
  Ptr<Ipv4> m_ipv4;

  /** Navigation system and GPS used by the vehicle node. */
  Ptr<GpsSystem> m_gps;

  /**
   * The index of the interface used by the protocol (to receive incoming 
   * and transmit outgoing packets).
   * 
   * \c -1 (or any negative integer) indicates that no interface is selected, and
   * \c >= 0 indicates the index of the interface
   */
  int32_t m_selected_interface_index;

  /**
   * Stores the last known interface index used by the protocol. This is used 
   * by <code>RoutingProtocol::Enable ()</code> to try to enable the last known
   * interface.
   */
  int32_t m_last_known_selected_interface_index;

  /** The interface address of the interface used by the protocol. */
  Ipv4InterfaceAddress m_selected_interface_address;

  /** Socket to receive unicast packets and transmit all outgoing packets. */
  Ptr<Socket> m_unicast_socket;

  /**
   * Socket to receive broadcast packets.
   * 
   * Info on changes from ns-3.26 to ns-3.27:
   * 
   * Sockets do not receive anymore broadcast packets, unless they are bound to 
   * an "Any" address (0.0.0.0) or to a subnet-directed broadcast packet (e.g., 
   * x.y.z.0 for a /24 noterok). As in Linux, the following rules are now enforced:
   * 
   *   -  A socket bound to 0.0.0.0 will receive everything.
   * 
   *   -  A socket bound to x.y.z.0/24 will receive subnet-directed broadcast 
   *      (x.y.z.255) and unicast packets.
   * 
   *   -  A socket bound to x.y.z.w will only receive unicast packets.
   * 
   * Previously, a socket bound to an unicast address received also subnet-directed
   * broadcast packets. This is not anymore possible.
   */
  Ptr<Socket> m_broadcast_socket;

  /** Provides uniform random variables. */
  Ptr<UniformRandomVariable> m_uniform_random_variable;

  /** Data packets sequential number. Each time it's used it must be incremented. */
  uint16_t m_data_sequential_id;

  /** Packets queue where the data packets are stored. */
  PacketsQueue m_packets_queue;

  /** Stores the recently contacted neighbor nodes. */
  NeighborsTable m_neighbors_table;

  /** Handles duplicated packets. */
  DuplicatedPacketDetector m_duplicate_detector;

  /** Counts the number and size of transmitted packets. */
  PacketsCounter m_tx_packets_counter;

  /** Stores a list of the data packets created in this node and its creation time. */
  std::vector<std::pair<DataHeader, Time> > m_created_data_packets;

  /** The current value of the adaptative exponential average. */
  double m_exponential_average;

  /**
   * The set of neighbors nodes that this node has met in the current exponential
   * average time-slot.
   * 
   * It must be cleared at the end of each time-slot.
   */
  std::set<Ipv4Address> m_exp_avg_time_slot_neighbors;


public:

  RoutingProtocol ();

  virtual ~RoutingProtocol ();


  static TypeId
  GetTypeId (void);

  /**
   * Assign a fixed random variable stream number to the random variables
   * used by this model.  Return the number of streams (possibly zero) that
   * have been assigned.
   *
   * \param stream first stream index to use
   * \return the number of stream indices assigned by this model
   */
  int64_t AssignStreams (int64_t stream);

  /** Receives a packet that must contain a Geo-temporal header 
   * (HELLO, REPLY, REPLY_BACK, DATA, DATA_ACK, ACK) and the Type header, and 
   * returns the size of the complete packet when transmitted (Geo-temporal 
   * header, UDP header, IP header, IEEE 802.11 header, LCC header and frame end).
   */
  static uint32_t
  CalculateCompletePacketSize (const Ptr<const Packet> packet);


  // ---------------------
  // Getters & setters
  // ---------------------

  inline int32_t
  GetNodeId () const
  {
    return m_node_id;
  }

  inline void
  SetNodeId (int32_t node_id)
  {
    m_node_id = node_id;
  }

  /** Sets a smart pointer to the GPS system to be used. */
  void
  SetGpsSystem (Ptr<GpsSystem> gps_system);

  /** Returns the statistics of all received data packets by the node. */
  inline const std::map<DataIdentifier, DataPacketReceptionStats> &
  GetNodePacketReceptionStats () const
  {
    return m_packets_queue.GetPacketReceptionStats ();
  }

  /** Returns the counter of transmitted packets. */
  inline const PacketsCounter &
  GetTransmittedPacketsCounter () const
  {
    return m_tx_packets_counter;
  }

  /** Returns a list of the data packets created in this node and its creation time. */
  inline const std::vector<std::pair<DataHeader, Time> > &
  GetCreatedDataPackets () const
  {
    return m_created_data_packets;
  }

private:
  /**
   * Sets the reference parameters to the current geographical position and
   * velocity vector of the node.
   * 
   * @param position [OUT] Current position coordinates of the node.
   * @param velocity [OUT] Current velocity vector of the node.
   */
  void
  GetNodeMobility (Vector2D & position, Vector2D & velocity) const;


  // ---------------------
  // Object virtual functions
  // ---------------------
protected:
  void
  DoInitialize () override;

public:
  void
  DoDispose () override;


  // ---------------------
  // Ipv4RoutingProtocol virtual functions
  // ---------------------

  void
  PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit = Time::S) const override;

  void
  SetIpv4 (Ptr<Ipv4> ipv4) override;

  void
  NotifyAddAddress (uint32_t interface_index,
                    Ipv4InterfaceAddress new_interface_address) override;

  void
  NotifyRemoveAddress (uint32_t interface_index,
                       Ipv4InterfaceAddress removed_interface_address) override;

  void
  NotifyInterfaceUp (uint32_t interface_index) override;

  void
  NotifyInterfaceDown (uint32_t interface_index) override;

  bool
  RouteInput (Ptr<const Packet> received_packet, const Ipv4Header &ipv4_header,
              Ptr<const NetDevice> input_device, UnicastForwardCallback unicast_cb,
              MulticastForwardCallback multicast_cb, LocalDeliverCallback local_cb,
              ErrorCallback error_cb) override;

  Ptr<Ipv4Route>
  RouteOutput (Ptr<Packet> output_packet, const Ipv4Header &ipv4_header,
               Ptr<NetDevice> output_device, Socket::SocketErrno &sockerr) override;


private:

  // ---------------------
  // Receive packets functions
  // ---------------------

  /** 
   * Receives Geo-Temporal packets and forwards the processing to the 
   * appropriate function.
   */
  void
  RecvGeoTemporalPacket (Ptr<Socket> socket);

  /**
   * Processes the received HELLO packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the HELLO header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvHelloPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received REPLY packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the REPLY header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvReplyPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received REPLY_BACK packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the REPLY_BACK header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvReplyBackPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received DATA and DATA_ACK packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the DATA or 
   * DATA_ACK header on top. The packet is modified, so if you want to conserve
   * the original make sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   * @param ack_flag [IN] With <code>true</code> indicates that the received
   * packet is a DATA_ACK packet, otherwise the received packet is a DATA packet.
   */
  void
  RecvDataPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip,
                  const bool ack_flag);

  /**
   * Processes the received ACK packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the REPLY_BACK header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvAckPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);



  // ---------------------
  // Send packets functions
  // ---------------------

public:

  /**
   * Receives a normal priority message from upper layers and starts its
   * dissemination.
   * 
   * @param message [IN] Message to transmit.
   * @param destination_gta [IN] Destination geo-temporal area of the message.
   */
  void
  NewMessage (const std::string & message,
              const GeoTemporalArea & destination_gta);

  /**
   * Receives a message from upper layers and starts its dissemination. Use
   * <code>emergency_flag</code> to specify if it is an emergency packet or not.
   * 
   * @param message [IN] Message to transmit.
   * @param destination_gta [IN] Destination geo-temporal area of the message.
   * @param emergency_flag [IN] If enabled, it indicates that the packet is an 
   * emergency packet. Otherwise, disabled indicates that is a normal packet.
   */
  void
  NewMessage (const std::string & message,
              const GeoTemporalArea & destination_gta,
              const bool emergency_flag);

private:

  /**
   * Transmits via unicast the given packet to the specified destination node.
   * 
   * Returns <code>true</code> if the packet was transmitted successfully, 
   * otherwise it returns <code>false</code>.
   */
  bool
  SendUnicastPacket (const Ptr<Packet> packet_to_send,
                     const Ipv4Address & destination_node) const;

  /**
   * Broadcasts the given packet.
   * 
   * Returns <code>true</code> if the packet was transmitted successfully, 
   * otherwise it returns <code>false</code>.
   */
  bool
  SendBroadcastPacket (const Ptr<Packet> packet_to_send) const;

  /** Broadcasts a HELLO packet. */
  void
  SendHelloPacket ();

  /** Sends a REPLY packet to the specified destination node. */
  void
  SendReplyPacket (const Ipv4Address & destination_node);

  /** Sends a REPLY_BACK packet to the specified destination node. */
  void
  SendReplyBackPacket (const Ipv4Address & destination_node,
                       const std::set<DataIdentifier> & disjoint_vector);

  /**
   * From the set of packets requested by the neighbor node it selects the packet
   * with the highest transmission priority for which the neighbor node is a 
   * valid packet carrier and it transmits it via unicast to the neighbor node.
   * 
   * It uses the geographical information of both nodes, local node and neighbor
   * node, to determine if the neighbor node is a valid packet carrier. Depending
   * on the validity of the neighbor node as a packet carrier it might be that
   * no packet is selected, and therefore, no packet is transmitted.
   * 
   * If the parameter <code>ack_flag</code> is specified and is set to <code>true,
   * </code> then instead of transmitting a DATA packet, it transmits a DATA_ACK
   * packet. This DATA_ACK packet contains the same information of the selected
   * requested packet along with the data ID of the DATA packet being 
   * acknowledged. This data ID is specified in the parameter <code>
   * data_id_to_ack</code>.
   * 
   * When transmitting a DATA_ACK packet and no data packet is selected to be 
   * transmitted to the neighbor node it sends instead an ACK packet. This is
   * because there are no data packets to send, but the ACK must be transmitted.
   * 
   * @param destination_node_ip [IN] IP address of the destination node.
   * @param destination_node_position [IN] Last reported position of the neighbor
   * node.
   * @param destination_node_velocity [IN] Last reported velocity vector of the
   * neighbor node.
   * @param ack_flag [IN] (Optional, default value: <code>false</code>) When
   * <code>true</code> it transmits a DATA_ACK packet, when <code>false</code>
   * it transmits a DATA packet.
   * @param data_id_to_ack [IN] (Optional, default value: default-constructed 
   * DataIdentifier object) The ID of the data packet to be acknowledged as 
   * successfully received. Only used when <code>ack_flag</code> is set to
   * <code>true</code>.
   */
  void
  SendDataPacket (const Ipv4Address & destination_node_ip,
                  const Vector2D & destination_node_position,
                  const Vector2D & destination_node_velocity,
                  const bool ack_flag = false,
                  const DataIdentifier & data_id_to_ack = DataIdentifier ());

  /**
   * Transmits via unicast an ACK packet to acknowledge the reception of the 
   * DATA packet with the given DATA ID to the node with the specified IP address.
   */
  void
  SendAckPacket (const Ipv4Address & destination_node,
                 const DataIdentifier & data_id_to_ack);



  // ---------------------
  // Other routing protocol functions
  // ---------------------

  /** Starts the protocol operation. */
  void Start ();

  /** Stops the protocol operation. */
  void Stop ();

  /** 
   * Returns <code>true</code> if the given IP address is the operational IP 
   * address of the node. Otherwise returns <code>false</code>.
   */
  bool IsMyOwnAddress (const Ipv4Address & ip_address);

  /** 
   * Schedules the next transmission of the HELLO packet after the default delay
   * time <code>HELLO timeout</code> has passed.
   * 
   * A randomized time (jitter) is added to the default delay time to lower the 
   * chances of different nodes "synchronizing" and transmitting HELLO packets 
   * at the same time.
   * 
   * If the transmission of the HELLO packet is already scheduled then it does 
   * nothing.
   */
  void
  ScheduleHelloPacketTransmission ();

  /**
   * Schedules the next transmission of the HELLO packet after the given time
   * delay has passed.
   * 
   * A randomized time (jitter) is added to the given delay time to lower the 
   * chances of different nodes "synchronizing" and transmitting HELLO packets 
   * at the same time.
   * 
   * If the transmission of the HELLO packet is already scheduled then it does 
   * nothing.
   * 
   * @param delay [IN] Period of time when the HELLO packet must be transmitted.
   */
  void
  ScheduleHelloPacketTransmission (const Time & delay);

  /**
   * Cancels the scheduled transmission of the HELLO packet.
   * 
   * If there is not a scheduled transmission then it does nothing.
   */
  void
  CancelHelloPacketTransmission ();

  /**
   * Broadcasts a HELLO packet and schedules the next HELLO packet transmission.
   */
  void
  HelloTimerExpire ();

  /**
   * Schedules the next calculation of the exponential adaptative average after
   * the default time interval has passed.
   * 
   * If the next calculation is already scheduled then it does nothing.
   * 
   * This function is called to start a new time-slot for the exponential 
   * adaptative average.
   * 
   * It clears the content of the member variable that stores the IP addresses
   * of the different neighbor nodes encountered during the last time-slot.
   */
  void
  ScheduleExponentialAverageCalculation ();

  /**
   * Cancels the scheduled calculation of the exponential adaptative average.
   * 
   * If there is not a scheduled calculation then it does nothing.
   */
  void
  CancelExponentialAverageCalculation ();

  /**
   * Calls the function to calculate the exponential adaptative average and 
   * afterwards it schedules the next calculation of the exp. adaptative avg.
   * 
   * This function is called at the end of the current time-slot of the exponential
   * adaptative to calculate the value of the current time-slot and starts the
   * next time-slot.
   */
  void
  ExponentialAverageTimerExpire ();

  /**
   * Calculates the value of the current time-slot of the exponential adaptative
   * average.
   */
  void
  CalculateExponentialAverage ();



public:
  // ---------------------
  // Node status functions
  // ---------------------

  /** Enables the node. When enabled, it can send and receive packets. */
  void Enable ();

  /** Disables the node. When disabled, it can not send nor receive packets. */
  void Disable ();

};

} // namespace geotemporal
} // namespace ns3

#endif /* GEOTEMPORAL_ROUTING_PROTOCOL_H */

