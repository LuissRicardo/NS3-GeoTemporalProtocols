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
 * Geo-Temporal Epidemic routing protocol.
 */

#ifndef GEOTEMPORAL_EPIDEMIC_ROUTING_PROTOCOL_H
#define GEOTEMPORAL_EPIDEMIC_ROUTING_PROTOCOL_H

#include <map>
#include <utility>
#include <vector>

#include "geotemporal-epidemic-duplicate-detector.h"
#include "geotemporal-epidemic-neighbors-table.h"
#include "geotemporal-epidemic-packets-queue.h"
#include "geotemporal-epidemic-packets.h"

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
namespace geotemporal_epidemic
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;

// =============================================================================
//                                RoutingProtocol
// =============================================================================

class RoutingProtocol : public Ipv4RoutingProtocol
{
private:

  /**
   * UDP port used for the Geo-Temporal Epidemic routing protocol.
   */
  static const uint32_t EPIDEMIC_ROUTING_PROTOCOL_PORT = 49765;


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

  /** Maximum number of hops that the data packets are flooded before being discarded. */
  uint32_t m_default_packets_hops_count;


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

  /** Receives a packet that must contain a Geo-temporal Epidemic header 
   * (HELLO, SUMMARY VECTOR, DATA) and the Type header, and returns the size
   * of the complete packet when transmitted (Geo-temporal Epidemic header,
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
   * Receives Epidemic packets and forwards the processing to the appropriate
   * function.
   */
  void
  RecvEpidemicPacket (Ptr<Socket> socket);

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
   * Processes the received SUMMARY VECTOR packets: REPLY or REPLY_BACK.
   * 
   * @param received_packet [IN/OUT] The received packet with the SUMMARY VECTOR
   * header on top. The packet is modified, so if you want to conserve the original
   * make sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   * @param packet_type [IN] To indicate if the received packet is a REPLY packet
   * or a REPLY_BACK packet. With other packet types it throws a fatal error.
   */
  void
  RecvSummaryVectorPacket (Ptr<Packet> received_packet,
                           const Ipv4Address & sender_node_ip,
                           const PacketType & reply_type);

  /**
   * Processes the received DATA packets.
   * 
   * @param received_packet [IN/OUT] The received packet with the DATA header on 
   * top. The packet is modified, so if you want to conserve the original make
   * sure to use a copy of the received packet.
   * @param sender_node_ip [IN] IP address of the node that sent the packet.
   */
  void
  RecvDataPacket (Ptr<Packet> received_packet, const Ipv4Address & sender_node_ip);



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

  /**
   * Sends a SUMMARY VECTOR packet (REPLY or REPLY_BACK) to the specified 
   * destination node.
   * 
   * @param destination_node [IN] Address of the destination node.
   * @param reply_type [IN] To indicate the type of SUMMARY VECTOR to send, 
   * REPLY or REPLY_BACK. With other packet types it throws a fatal error.
   */
  void
  SendSummaryVectorPacket (const Ipv4Address & destination_node,
                           const PacketType & reply_type);

  /**
   * Schedules the transmission of all the available DATA packets that are not
   * contained in the given Summary Vector header to the node with the specified
   * IP address.
   * 
   * It uses the reported geographical position of the node contained in the 
   * Summary Vector header to determine if it can send packets with only 1 hops
   * count remaining.
   * 
   * @param summary_vector_header [IN] Received summary vector header.
   * @param destination_ip [IN] IP address of the destination node.
   */
  void
  SendDisjointVectorPackets (const SummaryVectorHeader & summary_vector_header,
                             const Ipv4Address & destination_ip);

  /** 
   * Transmits via unicast the given DATA packet to the node with the specified
   * IP address.
   */
  void
  SendDataPacket (const Ipv4Address destination_ip, const DataHeader data_to_send);



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

} // namespace geotemporal_epidemic
} // namespace ns3

#endif /* GEOTEMPORAL_EPIDEMIC_ROUTING_PROTOCOL_H */

