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

#include "geotemporal-routing-protocol.h"

#include <ns3/math-utils.h>

#include <ns3/address.h>
#include <ns3/boolean.h>
#include <ns3/double.h>
#include <ns3/ipv4-l3-protocol.h>
#include <ns3/log.h>
#include <ns3/mobility-model.h>
#include <ns3/node.h>
#include <ns3/pointer.h>
#include <ns3/string.h>
#include <ns3/udp-socket-factory.h>
#include <ns3/uinteger.h>


namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("GeoTemporalRoutingProtocol");

namespace geotemporal
{

// =============================================================================
//                                RoutingProtocol
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (RoutingProtocol);

RoutingProtocol::RoutingProtocol () :
m_hello_timeout (MilliSeconds (1000)),
m_hello_timer (Timer::CANCEL_ON_DESTROY),
m_packets_queue_capacity (128u),
m_neighbor_expiration_time (Seconds (10)),
m_default_data_packet_replicas (3u),
m_neighbor_min_valid_distance_diff (20),
m_exponential_average_time_slot_size (Seconds (30)),
m_exponential_average_timer (Timer::CANCEL_ON_DESTROY),
m_node_id (-1),
m_ipv4 (0),
m_gps (0),
m_selected_interface_index (-1),
m_last_known_selected_interface_index (-1),
m_selected_interface_address (),
m_unicast_socket (0),
m_broadcast_socket (0),
m_data_sequential_id (0u),
m_packets_queue (0u, m_packets_queue_capacity, m_default_data_packet_replicas),
m_neighbors_table (m_neighbor_expiration_time),
m_duplicate_detector (MilliSeconds (5600)),
m_tx_packets_counter (),
m_created_data_packets (),
m_exponential_average (0.0),
m_exp_avg_time_slot_neighbors ()
{
  NS_LOG_FUNCTION (this);
}

RoutingProtocol::~RoutingProtocol ()
{
  NS_LOG_FUNCTION (this);
}

TypeId
RoutingProtocol::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::geotemporal::RoutingProtocol")
          .SetParent<Ipv4RoutingProtocol> ()
          .SetGroupName ("Geotemporal")
          .AddConstructor<RoutingProtocol> ()
          .AddAttribute ("HelloInterval", "Time interval between HELLO transmissions.",
                         TimeValue (Seconds (1)),
                         MakeTimeAccessor (&RoutingProtocol::m_hello_timeout),
                         MakeTimeChecker (Seconds (0)))
          .AddAttribute ("PacketsQueueCapacity", "The maximum number of packets that we allow the routing protocol to buffer.",
                         UintegerValue (128),
                         MakeUintegerAccessor (&RoutingProtocol::m_packets_queue_capacity),
                         MakeUintegerChecker<uint32_t> (8))
          .AddAttribute ("NeighborsExpirationTime", "Time interval in which a recently contacted neighbor node is not contacted again.",
                         TimeValue (Seconds (10)),
                         MakeTimeAccessor (&RoutingProtocol::m_neighbor_expiration_time),
                         MakeTimeChecker ())
          .AddAttribute ("DataPacketReplicas", "Number of replicas of each data packet allowed to be transmitted.",
                         UintegerValue (3),
                         MakeUintegerAccessor (&RoutingProtocol::m_default_data_packet_replicas),
                         MakeUintegerChecker<uint32_t> (1))
          .AddAttribute ("NeighborMinimumValidDistanceDifference", "Minimum distance difference between nodes to be valid packet carriers.",
                         DoubleValue (20),
                         MakeDoubleAccessor (&RoutingProtocol::m_neighbor_min_valid_distance_diff),
                         MakeDoubleChecker<double> (0.0))
          .AddAttribute ("ExponentialAverageTimeSlotSize", "Size of the time-slot to calculate exponential average.",
                         TimeValue (Seconds (30)),
                         MakeTimeAccessor (&RoutingProtocol::m_exponential_average_time_slot_size),
                         MakeTimeChecker (Seconds (5)))
          .AddAttribute ("UniformRv", "Access to the underlying UniformRandomVariable",
                         StringValue ("ns3::UniformRandomVariable"),
                         MakePointerAccessor (&RoutingProtocol::m_uniform_random_variable),
                         MakePointerChecker<UniformRandomVariable> ());
  return tid;
}

int64_t
RoutingProtocol::AssignStreams (int64_t stream)
{
  NS_LOG_FUNCTION (this << stream);
  m_uniform_random_variable->SetStream (stream);
  return 1;
}

uint32_t
RoutingProtocol::CalculateCompletePacketSize (const Ptr<const Packet> packet)
{
  return packet->GetSize () // Entire Geo-Temporal header size (including Type header)
          + 24 + 8 // IEEE 802.11 header + LCC header
          + 20 + 8 + 4; // + IP header + UDP header + Frame end.
}

void
RoutingProtocol::SetGpsSystem (Ptr<GpsSystem> gps_system)
{
  NS_LOG_FUNCTION (this << gps_system);

  NS_ASSERT (gps_system != 0);
  NS_ASSERT (m_gps == 0);

  m_gps = gps_system;
}

void
RoutingProtocol::GetNodeMobility (Vector2D& position, Vector2D& velocity) const
{
  NS_LOG_FUNCTION (this);

  Ptr<const MobilityModel> mobility = m_ipv4->GetObject<Node> ()->GetObject<MobilityModel> ();

  ns3::Vector vector = mobility->GetPosition ();
  position = Vector2D (vector);

  vector = mobility->GetVelocity ();
  velocity = Vector2D (vector);
}

void
RoutingProtocol::DoInitialize ()
{
  NS_LOG_FUNCTION (this);

  Ipv4RoutingProtocol::DoInitialize ();
  Start ();
}

void
RoutingProtocol::DoDispose ()
{
  NS_LOG_FUNCTION (this);

  m_ipv4 = 0;

  if (m_unicast_socket)
    {
      m_unicast_socket->Close ();
      m_unicast_socket = 0;
    }

  if (m_broadcast_socket)
    {
      m_broadcast_socket->Close ();
      m_broadcast_socket = 0;
    }

  if (m_gps)
    {
      m_gps = 0;
    }

  // Stop routing protocol operation
  Stop ();

  Ipv4RoutingProtocol::DoDispose ();
}

void
RoutingProtocol::PrintRoutingTable (Ptr<OutputStreamWrapper> stream, Time::Unit unit) const
{
  // There is no routing table.
  *stream->GetStream () << "No routing table.";
}

void
RoutingProtocol::SetIpv4 (Ptr<Ipv4> ipv4)
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT (ipv4 != 0);
  NS_ASSERT (m_ipv4 == 0);

  m_ipv4 = ipv4;

  // It is asserted that the only one interface up for now is loopback.
  NS_ASSERT (m_ipv4->GetNInterfaces () == 1 &&
             m_ipv4->GetAddress (0, 0).GetLocal () == Ipv4Address::GetLoopback ());
}

void
RoutingProtocol::NotifyAddAddress (uint32_t interface_index,
                                   Ipv4InterfaceAddress new_interface_address)
{
  NS_LOG_FUNCTION (this << " interface " << interface_index
                   << " address " << new_interface_address);
  NS_LOG_INFO ("New address " << new_interface_address.GetLocal ()
               << " added to interface with index " << interface_index);

  Ptr<Ipv4L3Protocol> ipv4_l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();

  // Check if the interface where the address was added is up
  if (!ipv4_l3->IsUp (interface_index))
    {
      NS_LOG_DEBUG ("Interface " << interface_index << " is DOWN.");
      return;
    }

  // Check if the added address belongs to the selected operational interface
  if (m_selected_interface_index != (int32_t) interface_index)
    {
      NS_LOG_DEBUG ("Added address doesn't belong to the interface participating "
                    "in the Geo-Temporal operation.");
      return;
    }

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_DEBUG ("Added address ignored: Geo-Temporal does NOT work with more "
                    "than one address per each interface.");
      return;
    }

  // We only use the first address of the interface. Extra addresses are ignored.
  Ipv4InterfaceAddress selected_interface_address = ipv4_l3->GetAddress (interface_index, 0);

  // Ignore loopback.
  if (selected_interface_address.GetLocal () == Ipv4Address::GetLoopback ())
    {
      NS_LOG_DEBUG ("Added address ignored: The address of the interface is "
                    "loopback.");
      return;
    }

  // Check if there already is a selected interface.
  if (m_selected_interface_index >= 0)
    {
      // There already is a selected interface
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal does NOT work with more "
                    "than one interface. Already selected interface "
                    << m_selected_interface_index);
      return;
    }

  // The interface:
  // - Is up,
  // - Has only 1 address, and
  // - Its first address is not loopback.
  // And there is no interface selected yet, so we mark the interface as selected
  // and create and bind sockets to its address.

  m_selected_interface_index = interface_index;
  m_last_known_selected_interface_index = interface_index;
  m_selected_interface_address = selected_interface_address;
  NS_LOG_DEBUG ("Selected interface " << m_selected_interface_index
                << " with address " << m_selected_interface_address);

  NS_LOG_DEBUG ("Creating sockets...");
  // Create a socket to listen for unicast directed packets
  Ptr<Socket> socket = Socket::CreateSocket (GetObject<Node> (),
                                             UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_broadcast_socket == 0);
  m_broadcast_socket = socket;

  NS_LOG_DEBUG ("Sockets created successfully!");
}

void
RoutingProtocol::NotifyRemoveAddress (uint32_t interface_index,
                                      Ipv4InterfaceAddress removed_interface_address)
{
  NS_LOG_FUNCTION (this << " interface " << interface_index
                   << " address " << removed_interface_address);
  NS_LOG_INFO ("Removed address " << removed_interface_address.GetLocal ()
               << " from interface with index " << interface_index);

  // Check if the removed address is one of the operational interface.
  if (m_selected_interface_index != (int32_t) interface_index)
    {
      NS_LOG_DEBUG ("Removed address doesn't belong to the interface participating "
                    "in the Geo-Temporal operation.");
      return;
    }

  // Check if the removed address was the operational interface address
  if (m_selected_interface_address != removed_interface_address)
    {
      NS_LOG_DEBUG ("Removed address is not participating in the "
                    "Geo-Temporal operation.");
      return;
    }

  // Removed address is from the operational interface and is the operational
  // address. Close sockets bound to the removed address.
  if (m_unicast_socket)
    {
      m_unicast_socket->Close ();
      m_unicast_socket = 0;
    }

  if (m_broadcast_socket)
    {
      m_broadcast_socket->Close ();
      m_broadcast_socket = 0;
    }

  // Now, if the interface has at least one address left, bind the sockets to the
  // first address.
  Ptr<Ipv4L3Protocol> ipv4_l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();

  if (ipv4_l3->GetNAddresses (interface_index) > 0)
    {
      NS_LOG_DEBUG ("Creating sockets...");
      Ipv4InterfaceAddress interface_address = ipv4_l3->GetAddress (interface_index, 0);

      // Create a socket to listen for unicast directed packets
      Ptr<Socket> socket = Socket::CreateSocket (GetObject<Node> (),
                                                 UdpSocketFactory::GetTypeId ());
      NS_ASSERT (socket != 0);
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetLocal (),
                                       GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_unicast_socket = socket;

      // Create also a socket to listen for broadcast directed packets
      socket = Socket::CreateSocket (GetObject <Node> (),
                                     UdpSocketFactory::GetTypeId ());
      NS_ASSERT (socket != 0);
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetBroadcast (),
                                       GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_broadcast_socket = socket;

      NS_LOG_DEBUG ("Sockets created successfully!");
    }

  // If there are no more sockets active left, stop routing protocol operation.
  if (!m_unicast_socket)
    {
      NS_LOG_LOGIC ("No Geo-Temporal interfaces.");
      Stop ();
    }
}

void
RoutingProtocol::NotifyInterfaceUp (uint32_t interface_index)
{
  NS_LOG_FUNCTION (this << " interface " << interface_index
                   << " address " << m_ipv4->GetAddress (interface_index, 0).GetLocal ());
  NS_LOG_DEBUG ("Interface " << interface_index << " is up.");

  // Check if there already is a selected interface.
  if (m_selected_interface_index >= 0)
    {
      // There already is a selected interface
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal does NOT work with more "
                    "than one interface. Already selected interface "
                    << m_selected_interface_index);
      return;
    }

  Ptr<Ipv4L3Protocol> ipv4_l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_WARN ("Extra addresses ignored: Geo-Temporal does NOT work with "
                   "more than one address per each interface.");
    }

  // Get the first address of the interface
  Ipv4InterfaceAddress selected_interface_address = ipv4_l3->GetAddress (interface_index, 0);

  // Ignore loopback (127.0.0.1)
  if (selected_interface_address.GetLocal () == Ipv4Address::GetLoopback ())
    {
      NS_LOG_DEBUG ("Interface address ignored: The first address of the "
                    "interface is loopback.");
      return;
    }

  // The interface:
  // - Is up,
  // - We're using its first address, and
  // - said address is not loopback (127.0.0.1).
  // And there is no interface selected yet, so we mark the interface as selected
  // and create and bind sockets to its address.

  m_selected_interface_index = interface_index;
  m_last_known_selected_interface_index = interface_index;
  m_selected_interface_address = selected_interface_address;
  NS_LOG_DEBUG ("Selected interface " << m_selected_interface_index
                << " with address " << m_selected_interface_address);

  NS_LOG_DEBUG ("Creating sockets...");

  // Create a socket to listen for unicast directed packets
  Ptr<Socket> socket = Socket::CreateSocket (GetObject<Node> (),
                                             UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvGeoTemporalPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_broadcast_socket == 0);
  m_broadcast_socket = socket;

  NS_LOG_DEBUG ("Sockets created successfully!");
}

void
RoutingProtocol::NotifyInterfaceDown (uint32_t interface_index)
{
  NS_LOG_FUNCTION (this << " interface " << interface_index
                   << " address " << m_ipv4->GetAddress (interface_index, 0).GetLocal ());
  NS_LOG_DEBUG ("Interface " << interface_index << " is down.");

  // Check if the down interface is the operational interface.
  if (m_selected_interface_index != (int32_t) interface_index)
    {
      NS_LOG_DEBUG ("Down interface is not participating in the Geo-Temporal "
                    "operation.");
      return;
    }

  // Close sockets bound to the interface to disable
  if (m_unicast_socket)
    {
      m_unicast_socket->Close ();
      m_unicast_socket = 0;
    }

  if (m_broadcast_socket)
    {
      m_broadcast_socket->Close ();
      m_broadcast_socket = 0;
    }

  // Unselect interface.
  m_selected_interface_index = -1;
  m_selected_interface_address = Ipv4InterfaceAddress ();

  // Stop routing protocol operation
  Stop ();

  NS_LOG_LOGIC ("Geo-Temporal interface disabled.");
}

bool
RoutingProtocol::RouteInput (Ptr<const Packet> received_packet, const Ipv4Header& ipv4_header,
                             Ptr<const NetDevice> input_device, UnicastForwardCallback unicast_cb,
                             MulticastForwardCallback multicast_cb,
                             LocalDeliverCallback local_cb,
                             ErrorCallback error_cb)
{
  NS_LOG_FUNCTION (this << received_packet << ipv4_header.GetDestination ()
                   << input_device->GetAddress ());

  if (m_selected_interface_index == -1)
    {
      NS_LOG_LOGIC ("No Geo-Temporal interfaces.");
      return false;
    }

  NS_ASSERT (m_ipv4 != 0);
  NS_ASSERT (received_packet != 0);

  // Check if input device supports IP
  NS_ASSERT (m_ipv4->GetInterfaceForDevice (input_device) >= 0);

  const Ipv4Address source_ip = ipv4_header.GetSource ();
  const Ipv4Address destination_ip = ipv4_header.GetDestination ();
  NS_LOG_DEBUG ("Received packet transmitted from " << source_ip
                << " destined for " << destination_ip);

  // Get the interface index of the receptor NetDevice
  const int32_t receptor_interface_index = m_ipv4->GetInterfaceForDevice (input_device);

  // Destination is a multicast address?
  if (destination_ip.IsMulticast ())
    {
      // Geo-Temporal does not support multicasting.
      return false;
    }

  // Duplicate of my own packet?
  if (IsMyOwnAddress (source_ip))
    {
      NS_LOG_DEBUG ("Duplicate of own packet. Drop.");
      return true;
    }

  // Duplicate of an already received packet?
  if (m_duplicate_detector.IsDuplicate (received_packet, ipv4_header))
    {
      NS_LOG_DEBUG ("Duplicated packet " << received_packet->GetUid () << " from "
                    << source_ip << ". Drop.");
      return true;
    }

  // Broadcast local delivery/forwarding
  if (m_ipv4->GetInterfaceForAddress (m_selected_interface_address.GetLocal ()) == receptor_interface_index)
    {
      if (destination_ip == m_selected_interface_address.GetBroadcast () || destination_ip.IsBroadcast ())
        {
          if (local_cb.IsNull () == false)
            {
              NS_LOG_DEBUG ("Broadcast local delivery to " << m_selected_interface_address.GetLocal ());
              local_cb (received_packet, ipv4_header, receptor_interface_index);
            }
          else
            {
              NS_LOG_ERROR ("Unable to deliver packet locally due to null callback. "
                            "Packet " << received_packet->GetUid () << " from "
                            << source_ip << " is discarded.");
              error_cb (received_packet, ipv4_header, Socket::ERROR_NOROUTETOHOST);
            }
          return true;
        }
    }

  // Unicast local delivery
  if (m_ipv4->IsDestinationAddress (destination_ip, receptor_interface_index))
    {
      if (local_cb.IsNull () == false)
        {
          NS_LOG_DEBUG ("Unicast local delivery to " << destination_ip);
          local_cb (received_packet, ipv4_header, receptor_interface_index);
        }
      else
        {
          NS_LOG_ERROR ("Unable to deliver packet locally due to null callback. "
                        "Packet " << received_packet->GetUid () << " from "
                        << source_ip << " is discarded.");
          error_cb (received_packet, ipv4_header, Socket::ERROR_NOROUTETOHOST);
        }
      return true;
    }

  // The routing protocol can't figure out what to do.
  return false;
}

Ptr<Ipv4Route>
RoutingProtocol::RouteOutput (Ptr<Packet> output_packet, const Ipv4Header& ipv4_header,
                              Ptr<NetDevice> output_device, Socket::SocketErrno& sockerr)
{
  NS_LOG_FUNCTION (this << ipv4_header << (output_device ? output_device->GetIfIndex () : 0));

  Ptr<Ipv4Route> route;

  if (m_selected_interface_index == -1)
    {
      NS_LOG_DEBUG ("No Geo-Temporal interfaces.");
      sockerr = Socket::ERROR_NOROUTETOHOST;
      return route; // No route
    }

  route = Create<Ipv4Route> ();

  route->SetDestination (ipv4_header.GetDestination ());
  route->SetGateway (ipv4_header.GetDestination ());
  route->SetSource (m_selected_interface_address.GetLocal ());
  route->SetOutputDevice (m_ipv4->GetNetDevice (m_selected_interface_index));

  NS_LOG_DEBUG ("Route created: destination " << route->GetDestination ()
                << ", gateway " << route->GetDestination ()
                << ", source " << route->GetSource ()
                << ", output device (" << m_selected_interface_index << ", "
                << route->GetOutputDevice () << ").");

  return route;
}

void
RoutingProtocol::RecvGeoTemporalPacket (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);

  Address sender_address;
  Ptr<Packet> received_packet = socket->RecvFrom (sender_address);
  NS_ASSERT (received_packet);

  const InetSocketAddress sender_socket_address = InetSocketAddress::ConvertFrom (sender_address);

  const Ipv4Address sender_node_ip = sender_socket_address.GetIpv4 ();

  if (socket == m_unicast_socket)
    {
      NS_LOG_DEBUG ("Unicast packet received.");
    }
  else if (socket == m_broadcast_socket)
    {
      NS_LOG_DEBUG ("Broadcast packet received.");
    }
  else
    {
      NS_ASSERT_MSG (false, "Received a packet from an unknown socket.");
    }

  TypeHeader type_header;
  received_packet->RemoveHeader (type_header);

  NS_ASSERT (type_header.IsValid ());

  NS_LOG_DEBUG ("Geo-Temporal " << type_header << " packet received from node "
                << sender_node_ip);

  m_packets_queue.Purge ();
  m_neighbors_table.Purge ();

  switch (type_header.GetPacketType ())
    {
    case PacketType::Hello:
      RecvHelloPacket (received_packet, sender_node_ip);
      return;
    case PacketType::Reply:
      RecvReplyPacket (received_packet, sender_node_ip);
      return;
    case PacketType::ReplyBack:
      RecvReplyBackPacket (received_packet, sender_node_ip);
      return;
    case PacketType::Data:
      RecvDataPacket (received_packet, sender_node_ip, false);
      return;
    case PacketType::DataAck:
      RecvDataPacket (received_packet, sender_node_ip, true);
      return;
    case PacketType::Ack:
      RecvAckPacket (received_packet, sender_node_ip);
      return;
    default:
      NS_ABORT_MSG ("ERROR: Unknown packet type.");
    }
}

void
RoutingProtocol::RecvHelloPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  HelloHeader hello_header;
  received_packet->RemoveHeader (hello_header);
  NS_ASSERT (sender_node_ip == hello_header.GetSenderNodeIp ());

  NS_LOG_DEBUG ("Processing HELLO packet received from node " << sender_node_ip);

  // Count the encountered neighbor node for the exponential adapatative average
  // calculation.
  m_exp_avg_time_slot_neighbors.insert (sender_node_ip);

  // Anti-entropy session: if the current node has the smaller identifier then
  // it initiates the session, if not then ignore the HELLO.
  if (m_selected_interface_address.GetLocal ().Get () > sender_node_ip.Get ())
    {
      // Current node has a greater identifier, don't initiate session.
      NS_LOG_DEBUG ("HELLO ignored due to being from a node with smaller identifier ("
                    << sender_node_ip << ").");
      return;
    }

  // Check if the neighbor node has been contacted before recently, if it has 
  // been contacted then ignore the HELLO.
  if (m_neighbors_table.Find (sender_node_ip))
    {
      // Recently contacted neighbor, ignore HELLO.
      NS_LOG_DEBUG ("HELLO ignored due to being from recently contacted node ("
                    << sender_node_ip << ").");
      return;
    }

  // Neighbor hasn't been contacted recently, add it to the table of recently
  // contacted neighbors.
  m_neighbors_table.Insert (sender_node_ip);

  // Answer with a REPLY packet
  NS_LOG_DEBUG ("Answer with a REPLY packet to node " << sender_node_ip);
  SendReplyPacket (sender_node_ip);
}

void
RoutingProtocol::RecvReplyPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  NS_LOG_DEBUG ("Processing REPLY packet received from " << sender_node_ip);

  SummaryVectorHeader reply_header;
  received_packet->RemoveHeader (reply_header);

  // The disjoint vector of the local node. P_A - P_B
  std::set<DataIdentifier> locally_unknown_packets;

  // The disjoint vector of the neighbor node. P_B - P_A
  std::set<DataIdentifier> neighbor_unknown_packets;

  m_packets_queue.ProcessDisjointVectors (/*Received summary vector*/ reply_header.GetSummaryVector (),
                                          /*Local IP*/ m_selected_interface_address.GetLocal (),
                                          /*Neighbor IP*/ sender_node_ip,
                                          /*Local unknown packets*/ locally_unknown_packets,
                                          /*Neighbor unknown packets*/ neighbor_unknown_packets);

  // Store the set of packets requested by the neighbor node
  m_neighbors_table.Insert (/*Neighbor node*/ sender_node_ip,
                            /*Neighbor unknown packets*/ neighbor_unknown_packets);

  // Store the neighbor node as a known packet carrier of each packet contained
  // in the summary vector
  m_packets_queue.AddKnownPacketCarriers (reply_header.GetSummaryVector (),
                                          sender_node_ip);

  // If the neighbor node has at least 1 packet that I don't know then I send a
  // REPLY BACK packet to request the unknown packets.
  if (!locally_unknown_packets.empty ())
    {
      NS_LOG_DEBUG ("Answer with a REPLY BACK packet to node " << sender_node_ip);
      SendReplyBackPacket (sender_node_ip, locally_unknown_packets);
    }
  else
    {
      // I have all the packets that the neighbor node has. Check if I have packets
      // that the neighbor node doesn't have.

      // If I have at least one packet that the neighbor node doesn't have, start
      // sending them.
      if (!neighbor_unknown_packets.empty ())
        {
          NS_LOG_DEBUG ("Proceed to send DATA packets unknown by the neighbor node.");
          SendDataPacket (sender_node_ip,
                          reply_header.GetPosition (),
                          reply_header.GetVelocity ());
        }
      else
        {
          // Both nodes have the same packets in memory, do nothing.
          NS_LOG_DEBUG ("Both nodes have the same packets in memory. Do nothing.");
        }
    }
}

void
RoutingProtocol::RecvReplyBackPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  NS_LOG_DEBUG ("Processing REPLY_BACK packet received from " << sender_node_ip);

  SummaryVectorHeader reply_back_header;
  received_packet->RemoveHeader (reply_back_header);

  NS_ASSERT_MSG (!reply_back_header.GetSummaryVector ().empty (),
                 "The REPLY BACK packet's disjoint vector shouldn't be empty.");

  // Store the set of data packets requested by the neighbor node.
  m_neighbors_table.UpdateNeighborRequestedPackets (sender_node_ip,
                                                    reply_back_header.GetSummaryVector ());

  NS_LOG_DEBUG ("Proceed to send requested DATA packets to neighbor node " << sender_node_ip);
  SendDataPacket (sender_node_ip,
                  reply_back_header.GetPosition (),
                  reply_back_header.GetVelocity ());
}

void
RoutingProtocol::RecvDataPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip,
                                 const bool ack_flag)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  DataHeader data_header;

  if (!ack_flag)
    {
      // Received a DATA packet
      received_packet->RemoveHeader (data_header);
      NS_LOG_DEBUG ("Received DATA packet " << data_header.GetDataIdentifier ()
                    << " from node " << sender_node_ip);
    }
  else
    {
      // Received a DATA_ACK packet
      DataAckHeader data_ack_header;
      received_packet->RemoveHeader (data_ack_header);

      data_header = (DataHeader) data_ack_header;

      NS_LOG_DEBUG ("Received DATA_ACK packet " << data_ack_header.GetDataIdentifier ()
                    << " that acknowledges reception of packet " << data_ack_header.GetDataIdentifierToAck ()
                    << " from " << sender_node_ip);

      // Store neighbor as known packet carrier
      m_packets_queue.AddKnownPacketCarrier (data_ack_header.GetDataIdentifierToAck (),
                                             sender_node_ip);
    }

  // Increment hops count
  data_header.SetHopsCount (data_header.GetHopsCount () + 1u);
  NS_LOG_DEBUG ("Received DATA packet with incremented hops count: " << data_header);

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  if (m_packets_queue.Enqueue (data_header, my_position, sender_node_ip))
    {
      NS_LOG_DEBUG ("Packet successfully stored in packets queue.");
    }
  else
    {
      NS_LOG_DEBUG ("Packet not stored in packets queue.");
    }

  // Reply to the neighbor node with an ACK or DATA_ACK packet.
  NeighborEntry neighbor;

  // - If I have at least one packet that the neighbor node doesn't, use a 
  //   DATA_ACK packet. Otherwise use an ACK packet.
  if (m_neighbors_table.Find (sender_node_ip, neighbor)
      && !neighbor.GetRequestedPacketsSet ().empty ())
    {
      // Reply with a DATA_ACK packet
      NS_LOG_DEBUG ("Answer with a DATA_ACK packet to node " << sender_node_ip);
      SendDataPacket (sender_node_ip,
                      data_header.GetPosition (),
                      data_header.GetVelocity (),
                      true,
                      data_header.GetDataIdentifier ());
    }
  else
    {
      // Reply with an ACK packet
      NS_LOG_DEBUG ("Answer with an ACK packet to node " << sender_node_ip);
      SendAckPacket (sender_node_ip, data_header.GetDataIdentifier ());
    }
}

void
RoutingProtocol::RecvAckPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  NS_LOG_DEBUG ("Processing ACK packet received from " << sender_node_ip);

  AckHeader ack_header;
  received_packet->RemoveHeader (ack_header);
  NS_LOG_DEBUG ("Received ACK for packet " << ack_header.GetDataIdentifier ()
                << " from " << sender_node_ip);

  // Store neighbor as known packet carrier
  m_packets_queue.AddKnownPacketCarrier (ack_header.GetDataIdentifier (),
                                         sender_node_ip);

  NS_LOG_DEBUG ("Proceed to send requested DATA packets to neighbor node " << sender_node_ip);
  SendDataPacket (sender_node_ip,
                  ack_header.GetPosition (),
                  ack_header.GetVelocity ());
}

void
RoutingProtocol::NewMessage (const std::string& message,
                             const GeoTemporalArea& destination_gta)
{
  NS_LOG_FUNCTION (this << destination_gta << " message size " << message.size ());

  NewMessage (message, destination_gta, false);
}

void
RoutingProtocol::NewMessage (const std::string& message,
                             const GeoTemporalArea& destination_gta,
                             const bool emergency_flag)
{
  NS_LOG_FUNCTION (this << destination_gta << " message size " << message.size ()
                   << (emergency_flag ? " EMERGENCY PACKET " : " NORMAL PACKET "));

  const Ipv4Address local_ip = m_selected_interface_address.GetLocal ();

  const DataIdentifier data_id (local_ip, m_data_sequential_id++);

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  const DataHeader data_packet (/*Data ID*/ data_id,
                                /*Emergency flag*/ emergency_flag,
                                /*Hops count*/ 0u,
                                /*Node position*/ my_position,
                                /*Node velocity*/ my_velocity,
                                /*Dest. geo-temporal area*/ destination_gta,
                                /*Message*/ message);

  const bool enqueued = m_packets_queue.Enqueue (data_packet, my_position, local_ip);
  NS_ABORT_MSG_UNLESS (enqueued, "Message must be enqueued successfully.");
  NS_LOG_DEBUG ("DATA packet " << data_id << " constructed and stored in packets "
                "queue successfully: " << data_packet);

  // Store the data packet in a list to keep a log of all created data packets
  // in the node and its creation time.
  m_created_data_packets.push_back (std::make_pair (data_packet, Simulator::Now ()));
}

bool
RoutingProtocol::SendUnicastPacket (const Ptr<Packet> packet_to_send,
                                    const Ipv4Address& destination_node) const
{
  NS_LOG_FUNCTION (this << packet_to_send << destination_node
                   << (m_unicast_socket ? "Node enabled" : "Node disabled"));

  if (m_unicast_socket == 0)
    {
      NS_LOG_DEBUG ("Node is disabled, packet [" << packet_to_send << "] was not sent.");
      return false;
    }

  m_unicast_socket->SendTo (packet_to_send, 0,
                            InetSocketAddress (destination_node,
                                               GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  NS_LOG_DEBUG ("Packet [" << packet_to_send << "] send to node "
                << destination_node << " (unicast).");
  return true;
}

bool
RoutingProtocol::SendBroadcastPacket (const Ptr<Packet> packet_to_send) const
{
  NS_LOG_FUNCTION (this << packet_to_send << (m_unicast_socket ? "Node enabled" : "Node disabled"));

  if (m_unicast_socket == 0)
    {
      NS_LOG_DEBUG ("Node is disabled, packet [" << packet_to_send << "] was not sent.");
      return false;
    }

  Ipv4Address destination_broadcast;

  // If the operational address is on a 32 bits address then send to all hosts, 
  // otherwise send to subnet.
  if (m_selected_interface_address.GetMask () == Ipv4Mask::GetOnes ())
    destination_broadcast = Ipv4Address::GetBroadcast (); // The 255.255.255.255 address, ALL hosts
  else
    destination_broadcast = m_selected_interface_address.GetBroadcast (); // Subnet broadcast

  m_unicast_socket->SendTo (packet_to_send, 0,
                            InetSocketAddress (destination_broadcast,
                                               GEO_TEMPORAL_ROUTING_PROTOCOL_PORT));
  NS_LOG_DEBUG ("Packet [" << packet_to_send << "] broadcasted to "
                << destination_broadcast << ".");
  return true;
}

void
RoutingProtocol::SendHelloPacket ()
{
  NS_LOG_FUNCTION (this << m_selected_interface_address.GetLocal ());

  HelloHeader hello_header (/*Node IP*/ m_selected_interface_address.GetLocal (),
                            /*Known packets count*/ m_packets_queue.Size ());
  NS_LOG_DEBUG ("Constructed HELLO packet: " << hello_header);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (hello_header);
  packet_to_send->AddHeader (TypeHeader (PacketType::Hello));

  // Broadcast the hello
  if (SendBroadcastPacket (packet_to_send))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Control, packet_size);

      NS_LOG_DEBUG ("HELLO packet broadcasted from " << m_selected_interface_address.GetLocal ());
    }
  else
    {
      NS_LOG_DEBUG ("HELLO packet could not be broadcasted (maybe node is disabled).");
    }
}

void
RoutingProtocol::SendReplyPacket (const Ipv4Address& destination_node)
{
  NS_LOG_FUNCTION (this << destination_node);

  // I received a HELLO, construct a REPLY.

  // Get the summary vector (the vector of known packets)
  std::set<DataIdentifier> summary_vector;
  m_packets_queue.GetSummaryVector (summary_vector);

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  SummaryVectorHeader reply_header (summary_vector, my_position, my_velocity);
  NS_LOG_DEBUG ("Constructed REPLY packet: " << reply_header);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (reply_header);
  packet_to_send->AddHeader (TypeHeader (PacketType::Reply));

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_node))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Control, packet_size);

      NS_LOG_DEBUG ("REPLY packet sent from " << m_selected_interface_address.GetLocal ()
                    << " to " << destination_node);
    }
  else
    {
      NS_LOG_DEBUG ("REPLY packet could not be sent to " << destination_node
                    << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::SendReplyBackPacket (const Ipv4Address& destination_node,
                                      const std::set<DataIdentifier>& disjoint_vector)
{
  NS_LOG_FUNCTION (this << destination_node << " disjoint vector size "
                   << disjoint_vector.size ());

  // I received a REPLY, construct a REPLY BACK (REQUEST)
  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  SummaryVectorHeader reply_back_header (disjoint_vector, my_position, my_velocity);
  NS_LOG_DEBUG ("Constructed REPLY_BACK packet: " << reply_back_header);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (reply_back_header);
  packet_to_send->AddHeader (TypeHeader (PacketType::ReplyBack));

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_node))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Control, packet_size);

      NS_LOG_DEBUG ("REPLY_BACK packet sent from " << m_selected_interface_address.GetLocal ()
                    << " to " << destination_node);
    }
  else
    {
      NS_LOG_DEBUG ("REPLY_BACK packet could not be sent to " << destination_node
                    << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::SendDataPacket (const Ipv4Address& destination_node_ip,
                                 const Vector2D& destination_node_position,
                                 const Vector2D& destination_node_velocity,
                                 const bool ack_flag,
                                 const DataIdentifier& data_id_to_ack)
{
  NS_LOG_FUNCTION (this << " Neighbor IP " << destination_node_ip
                   << " ack flag " << ack_flag);

  NS_LOG_DEBUG ("Sending DATA" << (ack_flag ? "_ACK" : "") << " packet to node "
                << destination_node_ip);

  // Get the set of requested packets by the neighbor
  NeighborEntry neighbor;

  if (!m_neighbors_table.Find (destination_node_ip, neighbor))
    {
      // The neighbor entry wasn't found, so there is no set of requested packets.
      NS_LOG_DEBUG ("Neighbor " << destination_node_ip << " not found in the "
                    "neighbors table (no set of requested packets). "
                    "Finish sending DATA packet.");

      if (ack_flag)
        {
          // When sending a DATA_ACK packet, send an ACK packet instead.
          NS_LOG_DEBUG ("Tried to send a DATA_ACK packet, so send an ACK packet instead.");
          SendAckPacket (destination_node_ip, data_id_to_ack);
        }

      return;
    }

  // The neighbor was found in the neighbors table, from its set of requested packets
  // select the one with the highest transmission priority.

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  PacketQueueEntry selected_packet; // The highest-priority packet selected to be transmitted.

  if (!m_packets_queue.Dequeue (/*Local node IP*/ m_selected_interface_address.GetLocal (),
                                /*Local node position*/ my_position,
                                /*Local node velocity*/ my_velocity,
                                /*Neighbor node IP*/ destination_node_ip,
                                /*Neighbor node position*/ destination_node_position,
                                /*Neighbor node velocity*/ destination_node_velocity,
                                /*Requested packets*/ neighbor.GetRequestedPacketsSet (),
                                /*Selected packet*/ selected_packet))
    {
      // No packet was selected to be transmitted to the neighbor node.
      NS_LOG_DEBUG ("No DATA packet was selected to be transmitted to the neighbor. "
                    "Finish sending DATA packet.");

      if (ack_flag)
        {
          // When sending a DATA_ACK packet, send an ACK packet instead.
          NS_LOG_DEBUG ("Tried to send a DATA_ACK packet, so send an ACK packet instead.");
          SendAckPacket (destination_node_ip, data_id_to_ack);
        }

      return;
    }

  // A packet was selected to be transmitted.

  // - Remove selected packet from the set of packets that the neighbor node doesn't know.
  m_neighbors_table.RemoveNeighborRequestedPacket (destination_node_ip,
                                                   selected_packet.GetDataPacketId ());

  // - Construct packet to be sent
  Ptr<Packet> packet_to_send = Create<Packet> ();

  DataHeader data_packet = selected_packet.GetDataPacket ();
  data_packet.SetPosition (my_position);
  data_packet.SetVelocity (my_velocity);

  if (!ack_flag)
    {
      // When sending a DATA packet
      NS_LOG_DEBUG ("Constructed DATA packet: " << data_packet);

      packet_to_send->AddHeader (data_packet);
      packet_to_send->AddHeader (TypeHeader (PacketType::Data));
    }
  else
    {
      // When sending a DATA_ACK packet
      DataAckHeader data_ack_packet (data_packet, data_id_to_ack);
      NS_LOG_DEBUG ("Constructed DATA_ACK packet: " << data_ack_packet);

      packet_to_send->AddHeader (data_ack_packet);
      packet_to_send->AddHeader (TypeHeader (PacketType::DataAck));
    }

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_node_ip))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Data, packet_size);

      // Log the transmission
      m_packets_queue.LogPacketTransmitted (data_packet.GetDataIdentifier ());

      NS_LOG_DEBUG ("DATA" << (ack_flag ? "_ACK" : "") << " packet sent from "
                    << m_selected_interface_address.GetLocal ()
                    << " to " << destination_node_ip);
    }
  else
    {
      NS_LOG_DEBUG ("DATA" << (ack_flag ? "_ACK" : "") << " packet could not be sent to "
                    << destination_node_ip << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::SendAckPacket (const Ipv4Address& destination_node,
                                const DataIdentifier& data_id_to_ack)
{
  NS_LOG_FUNCTION (this << destination_node << data_id_to_ack);

  // I received a DATA or DATA_ACK, construct an ACK
  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  AckHeader ack_header (data_id_to_ack, my_position, my_velocity);
  NS_LOG_DEBUG ("Constructed ACK packet: " << ack_header);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (ack_header);
  packet_to_send->AddHeader (TypeHeader (PacketType::Ack));

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_node))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Control, packet_size);

      NS_LOG_DEBUG ("ACK packet sent from " << m_selected_interface_address.GetLocal ()
                    << " to " << destination_node);
    }
  else
    {
      NS_LOG_DEBUG ("ACK packet could not be sent to " << destination_node
                    << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::Start ()
{
  NS_LOG_FUNCTION (this);

  NS_LOG_DEBUG ("Initialize packets queue a capacity of "
                << m_packets_queue_capacity << " packets, "
                << m_default_data_packet_replicas << " replicas for each data "
                "packet and a minimum valid distance of "
                << m_neighbor_min_valid_distance_diff << " meters.");
  m_packets_queue = PacketsQueue (m_gps, m_packets_queue_capacity, m_default_data_packet_replicas);
  m_packets_queue.SetMinVehiclesDistanceDifference (m_neighbor_min_valid_distance_diff);

  NS_LOG_DEBUG ("Initialize neighbors table with an expiration time of "
                << m_neighbor_expiration_time.GetSeconds () << " seconds.");
  m_neighbors_table = NeighborsTable (m_neighbor_expiration_time);

  m_exp_avg_time_slot_neighbors.clear ();

  // If the node is enabled (AKA there's an interface up), set the hello timer.
  NS_LOG_DEBUG ("Set up hello timer if node is enabled.");

  if (m_selected_interface_index == -1)
    {
      NS_LOG_DEBUG ("Node is disabled. Hello timer NOT set.");
      return;
    }

  NS_LOG_DEBUG ("Node enabled, setting hello timer.");
  m_hello_timer.SetFunction (&RoutingProtocol::HelloTimerExpire, this);
  ScheduleHelloPacketTransmission (MilliSeconds (0));

  NS_LOG_DEBUG ("Also setting exponential adaptative average timer.");
  m_exponential_average_timer.SetFunction (&RoutingProtocol::ExponentialAverageTimerExpire, this);
  ScheduleExponentialAverageCalculation ();
}

void
RoutingProtocol::Stop ()
{
  NS_LOG_FUNCTION (this);

  // Cancel timers
  CancelHelloPacketTransmission ();
  CancelExponentialAverageCalculation ();

  // Clear memory that won't be used
  m_neighbors_table.Clear ();
  m_packets_queue.Clear ();
  m_exp_avg_time_slot_neighbors.clear ();
}

bool
RoutingProtocol::IsMyOwnAddress (const Ipv4Address& ip_address)
{
  NS_LOG_FUNCTION (this << ip_address);
  return m_selected_interface_address.GetLocal () == ip_address;
}

void
RoutingProtocol::ScheduleHelloPacketTransmission ()
{
  NS_LOG_FUNCTION (this);
  ScheduleHelloPacketTransmission (m_hello_timeout);
}

void
RoutingProtocol::ScheduleHelloPacketTransmission (const Time& delay)
{
  NS_LOG_FUNCTION (this << delay.GetMilliSeconds () << " milliseconds ");

  if (m_hello_timer.IsRunning ())
    {
      NS_LOG_DEBUG ("Hello packet already scheduled.");
      return;
    }

  m_hello_timer.Cancel ();

  // Random delay = expected delay + random jitter
  const Time random_delay = delay // Expected delay
          + MilliSeconds (m_uniform_random_variable->GetInteger (0, 50)); // Random jitter
  m_hello_timer.Schedule (random_delay);

  NS_LOG_DEBUG ("Hello packet scheduled to be transmitted in " <<
                random_delay.GetMilliSeconds () << " milliseconds.");
}

void
RoutingProtocol::CancelHelloPacketTransmission ()
{
  NS_LOG_FUNCTION (this);

  if (!m_hello_timer.IsRunning ())
    {
      NS_LOG_DEBUG ("Hello packet not scheduled.");
      return;
    }

  m_hello_timer.Cancel ();
  NS_LOG_DEBUG ("Hello packet transmission cancelled.");
}

void
RoutingProtocol::HelloTimerExpire ()
{
  NS_LOG_FUNCTION (this);

  SendHelloPacket ();
  ScheduleHelloPacketTransmission ();
}

void
RoutingProtocol::ScheduleExponentialAverageCalculation ()
{
  NS_LOG_FUNCTION (this << m_exponential_average_time_slot_size.GetSeconds ()
                   << " seconds ");

  if (m_exponential_average_timer.IsRunning ())
    {
      NS_LOG_DEBUG ("Exponential average calculation already scheduled.");
      return;
    }

  m_exponential_average_timer.Cancel ();

  // Clears the registered neighbor nodes in the last time-slot to prepare for 
  // the new time-slot.
  m_exp_avg_time_slot_neighbors.clear ();

  m_exponential_average_timer.Schedule (m_exponential_average_time_slot_size);

  NS_LOG_DEBUG ("Exponential average to be calculated in "
                << m_exponential_average_time_slot_size.GetSeconds () << " seconds.");
}

void
RoutingProtocol::CancelExponentialAverageCalculation ()
{
  NS_LOG_FUNCTION (this);

  if (!m_exponential_average_timer.IsRunning ())
    {
      NS_LOG_DEBUG ("Exponential average calculation not scheduled.");
      return;
    }

  m_exponential_average_timer.Cancel ();
  NS_LOG_DEBUG ("Exponential average calculation cancelled.");
}

void
RoutingProtocol::ExponentialAverageTimerExpire ()
{
  NS_LOG_FUNCTION (this);

  CalculateExponentialAverage ();
  ScheduleExponentialAverageCalculation ();
}

void
RoutingProtocol::CalculateExponentialAverage ()
{
  const Time time_slot_end = Simulator::Now ();
  const Time time_slot_start = time_slot_end - m_exponential_average_time_slot_size;

  NS_LOG_FUNCTION (this << " start " << time_slot_start.GetSeconds ()
                   << " end " << time_slot_end.GetSeconds ());

  // (The following is in LaTeX format)
  // H_i = H_{i - 1} * \beta + h_i * (1 - \beta)
  // such that:
  //  - \beta \in \left[0, 1\right].
  //  - H_{i - 1} is the previously calculated average.
  //  - h_i represents the number of HELLO packets received from different 
  //    nodes in the current time slot.
  // We use \beta = 0.5

  const double beta = 0.5;

  // The previously calculated exponential adaptative average.
  const double H = m_exponential_average;

  // The number of different neighbor nodes in the current exponential adaptative
  // average time-slot.
  const uint32_t h = m_exp_avg_time_slot_neighbors.size ();

  // Calculation
  m_exponential_average = (H * beta) + (h * (1.0 - beta));

  NS_LOG_DEBUG ("Exponential adaptative average : " << m_exponential_average
                << " = (" << H << " * " << beta << ") + (" << h << " * (1 - "
                << beta << ") ) for time slot (" << time_slot_start.GetSeconds ()
                << ", " << time_slot_end.GetSeconds () << "].");
}

void
RoutingProtocol::Enable ()
{
  NS_LOG_FUNCTION (this);

  if (m_last_known_selected_interface_index < 0)
    {
      NS_LOG_DEBUG ("There isn't a last known interface to restore.");
      return;
    }

  if (m_ipv4->IsUp (m_last_known_selected_interface_index))
    {
      NS_LOG_DEBUG ("Last known selected interface already up.");
      return;
    }

  // The call to SetUp (uint32_t) will call RoutingProtocol::NotifyInterfaceUp (uint32_t).
  m_ipv4->SetUp (m_last_known_selected_interface_index);

  // RoutingProtocol::NotifyInterfaceUp (uint32_t) enables the device interface,
  // but to fully enable the operation of the node we call RoutingProtocol::Start ()
  // that starts the regular transmission of HELLO packets and initializes the
  // packets queue and neighbors table.
  Start ();

  NS_LOG_DEBUG ("Node " << m_selected_interface_address.GetLocal () << " (#"
                << m_node_id << ") enabled at " << Simulator::Now ().GetSeconds ()
                << " seconds.");
}

void
RoutingProtocol::Disable ()
{
  NS_LOG_FUNCTION (this);

  if (!m_ipv4->IsUp (m_selected_interface_index))
    {
      NS_LOG_DEBUG ("Selected interface already down.");
      return;
    }

  // The call to SetDown (uint32_t) will call RoutingProtocol::NotifyInterfaceDown (uint32_t),
  // and NotifyInterfaceDown will call RoutingProtocol::Stop ().
  m_ipv4->SetDown (m_selected_interface_index);
  NS_LOG_DEBUG ("Node with ID " << m_node_id << " disabled at "
                << Simulator::Now ().GetSeconds () << " seconds.");
}

} // namespace geotemporal
} // namespace ns3