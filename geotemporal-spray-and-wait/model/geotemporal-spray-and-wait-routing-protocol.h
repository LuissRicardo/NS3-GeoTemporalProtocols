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
 * Geo-Temporal Spray And Wait routing protocol.
 */

#ifndef GEOTEMPORAL_SPRAY_AND_WAIT_ROUTING_PROTOCOL_H
#define GEOTEMPORAL_SPRAY_AND_WAIT_ROUTING_PROTOCOL_H

#include <map>
#include <utility>
#include <vector>

#include <ns3/geotemporal-spray-and-wait-duplicate-detector.h>
#include <ns3/geotemporal-spray-and-wait-neighbors-table.h>
#include <ns3/geotemporal-spray-and-wait-packets-queue.h>
#include <ns3/geotemporal-spray-and-wait-packets.h>
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

namespace ns3
{
namespace geotemporal_spray_and_wait
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;

// =============================================================================
//                                RoutingProtocol
// =============================================================================

class RoutingProtocol : public Ipv4RoutingProtocol
{
private:

  /**
   * UDP port used for the Geo-Temporal Spray & Wait routing protocol.
   */
  static const uint32_t SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT = 49765;


  // ---------------------
  // Protocol parameters
  // ---------------------

  /** Time interval between HELLO transmissions. */
  Time m_hello_timeout;

  /** Hello timer. Controls when the HELLOs are transmitted. */
  Timer m_hello_timer;

  /** The maximum number of packets that we allow a routing protocol to buffer. */
  uint32_t m_max_length_packet_queue;

  /** Time interval in which a recently contacted neighbor node is not contacted
   * again. */
  Time m_neighbor_expiration_time;

  /** Maximum number of replicas of each data packet allowed to be trasmitted. */
  uint32_t m_default_max_replicas;

  /** Flag that indicates if the Spray And Wait protocol works in Binary or normal mode. 
   * \c true for Binary mode,
   * \c false for Normal mode.
   */
  bool m_binary_mode;


  // ---------------------
  // Internal variables
  // ---------------------

  /** ID of the node. */
  int32_t m_node_id;

  /** IP protocol. */
  Ptr<Ipv4> m_ipv4;

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

  /** Receives a packet that must contain a Geo-temporal Spray & Wait header 
   * (HELLO, REPLY, REPLY_BACK, DATA) and the Type header, and returns the size
   * of the complete packet when transmitted (Geo-temporal Spray & Wait header,
   * UDP header, IP header, IEEE 802.11 header, LCC header and frame end).
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
   * Receives Spray And Wait packets and forwards the processing to the 
   * appropriate function.
   */
  void
  RecvSprayAndWait (Ptr<Socket> socket);

  /**
   * Processes the received HELLO packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the HELLO header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvHello (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received REPLY packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the REPLY header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvReply (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received REPLY_BACK packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the REPLY_BACK header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvReplyBack (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);

  /**
   * Processes the received DATA packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the DATA header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvData (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);



  // ---------------------
  // Send packets functions
  // ---------------------

public:

  /**
   * Receives a message from upper layers and starts its dissemination.
   * 
   * @param message [IN] Message to transmit.
   * @param destination_gta [IN] Destination geo-temporal area of the message.
   */
  void
  NewMessage (const std::string & message,
              const GeoTemporalArea & destination_gta);

private:

  /** Transmits via unicast the given packet to the specified destination node. */
  void
  SendUnicastPacket (const Ptr<Packet> packet_to_send,
                     const Ipv4Address & destination_node) const;

  /** Broadcasts the given packet. */
  void
  SendBroadcastPacket (const Ptr<Packet> packet_to_send) const;

  /** Broadcasts a HELLO packet. */
  void
  SendHelloPacket ();

  /** Sends a REPLY packet to the specified destination node. */
  void
  SendReplyPacket (const Ipv4Address & destination_node);

  /** 
   * Sends a REPLY_BACK packet to the specified destination node. It uses the
   * received summary vector to construct the disjoint vector included in the
   * REPLY_BACK packet.
   */
  void
  SendReplyBackPacket (const Ipv4Address & destination_node,
                       const std::set<DataIdentifier> & received_summary_vector);

  /**
   * Schedules the transmission of all the available DATA packets in the received 
   * request to the specified destination IP address of the requesting node.
   * 
   * It uses the reported position of the requesting node to determine if the 
   * packet's replicas counter is decreased or not.
   * 
   * @param requested_packets_set [IN] Set of requested data packets IDs.
   * @param destination_node_ip [IN] IP address of the requesting node.
   * @param destination_node_position [IN] Last reported position of the 
   * requesting node.
   */
  void
  SendRequestedDataPackets (const std::set<DataIdentifier> & requested_packets_set,
                            const Ipv4Address & destination_ip,
                            const Vector2D & destination_node_position);

  /** 
   * Transmits via unicast the given DATA packet to the node with the specified
   * IP address.
   */
  void
  SendDataPacket (const Ipv4Address & destination_ip, const DataHeader & data_to_send);



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



public:
  // ---------------------
  // Node status functions
  // ---------------------

  /** Enables the node. When enabled, it can send and receive packets. */
  void Enable ();

  /** Disables the node. When disabled, it can not send nor receive packets. */
  void Disable ();

};

} // namespace geotemporal_spray_and_wait
} // namespace ns3

#endif /* GEOTEMPORAL_SPRAY_AND_WAIT_ROUTING_PROTOCOL_H */

