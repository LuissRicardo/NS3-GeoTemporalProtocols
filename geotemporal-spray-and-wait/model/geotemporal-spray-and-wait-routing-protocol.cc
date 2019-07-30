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

#include "geotemporal-spray-and-wait-routing-protocol.h"

#include <ns3/math-utils.h>

#include <ns3/address.h>
#include <ns3/boolean.h>
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

NS_LOG_COMPONENT_DEFINE ("GeoTemporalSprayAndWaitRoutingProtocol");

namespace geotemporal_spray_and_wait
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
m_default_data_packet_replicas (32u),
m_binary_mode (false),
m_node_id (-1),
m_ipv4 (0),
m_selected_interface_index (-1),
m_last_known_selected_interface_index (-1),
m_selected_interface_address (),
m_unicast_socket (0),
m_broadcast_socket (0),
m_data_sequential_id (0u),
m_packets_queue (m_binary_mode, m_packets_queue_capacity),
m_neighbors_table (m_neighbor_expiration_time),
m_duplicate_detector (MilliSeconds (5600)),
m_tx_packets_counter (),
m_created_data_packets ()
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
  static TypeId tid = TypeId ("ns3::geotemporal_spray_and_wait::RoutingProtocol")
          .SetParent<Ipv4RoutingProtocol> ()
          .SetGroupName ("Geotemporal-spray-and-wait")
          .AddConstructor<RoutingProtocol> ()
          .AddAttribute ("HelloInterval", "Time interval between HELLO transmissions.",
                         TimeValue (Seconds (1)),
                         MakeTimeAccessor (&RoutingProtocol::m_hello_timeout),
                         MakeTimeChecker ())
          .AddAttribute ("PacketsQueueCapacity", "The maximum number of packets that we allow the routing protocol to buffer.",
                         UintegerValue (128),
                         MakeUintegerAccessor (&RoutingProtocol::m_packets_queue_capacity),
                         MakeUintegerChecker<uint32_t> (8))
          .AddAttribute ("NeighborsExpirationTime", "Time interval in which a recently contacted neighbor node is not contacted again.",
                         TimeValue (Seconds (10)),
                         MakeTimeAccessor (&RoutingProtocol::m_neighbor_expiration_time),
                         MakeTimeChecker ())
          .AddAttribute ("DataPacketReplicas", "Number of replicas of each data packet allowed to be transmitted.",
                         UintegerValue (32),
                         MakeUintegerAccessor (&RoutingProtocol::m_default_data_packet_replicas),
                         MakeUintegerChecker<uint32_t> (1))
          .AddAttribute ("BinaryMode", "Indicates if the Spray And Wait protocol works in Binary mode (true) or normal mode (false).",
                         BooleanValue (false),
                         MakeBooleanAccessor (&RoutingProtocol::m_binary_mode),
                         MakeBooleanChecker ())
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
  return packet->GetSize () // Entire Spray & Wait header size (including Type header)
          + 24 + 8 // IEEE 802.11 header + LCC header
          + 20 + 8 + 4; // + IP header + UDP header + Frame end.
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
                    "in the Geo-Temporal Spray & Wait operation.");
      return;
    }

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_DEBUG ("Added address ignored: Geo-Temporal Spray & Wait does NOT "
                    "work with more than one address per each interface.");
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
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal Spray & Wait does NOT "
                    "work with more than one interface. Already selected "
                    "interface " << m_selected_interface_index);
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
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
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
                    "in the Geo-Temporal Spray & Wait operation.");
      return;
    }

  // Check if the removed address was the operational interface address
  if (m_selected_interface_address != removed_interface_address)
    {
      NS_LOG_DEBUG ("Removed address is not participating in the "
                    "Geo-Temporal Spray & Wait operation.");
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
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetLocal (),
                                       SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_unicast_socket = socket;

      // Create also a socket to listen for broadcast directed packets
      socket = Socket::CreateSocket (GetObject <Node> (),
                                     UdpSocketFactory::GetTypeId ());
      NS_ASSERT (socket != 0);
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetBroadcast (),
                                       SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_broadcast_socket = socket;

      NS_LOG_DEBUG ("Sockets created successfully!");
    }

  // If there are no more sockets active left, stop routing protocol operation.
  if (!m_unicast_socket)
    {
      NS_LOG_LOGIC ("No Geo-Temporal Spray & Wait interfaces.");
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
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal Spray & Wait does NOT "
                    "work with more than one interface. Already selected "
                    "interface " << m_selected_interface_index);
      return;
    }

  Ptr<Ipv4L3Protocol> ipv4_l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_WARN ("Extra addresses ignored: Geo-Temporal Spray & Wait does NOT "
                   "work with more than one address per each interface.");
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
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvSprayAndWaitPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
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
                    "Spray & Wait operation.");
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

  NS_LOG_LOGIC ("Geo-Temporal Spray & Wait interface disabled.");
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
      NS_LOG_LOGIC ("No Geo-Temporal Spray & Wait interfaces.");
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
      // Geo-Temporal Spray And Wait does not support multicasting.
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
      NS_LOG_DEBUG ("No Geo-Temporal Spray & Wait interfaces.");
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
RoutingProtocol::RecvSprayAndWaitPacket (Ptr<Socket> socket)
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

  NS_LOG_DEBUG ("Spray & Wait " << type_header << " packet received from node "
                << sender_node_ip);

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
      RecvDataPacket (received_packet, sender_node_ip);
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

  NS_LOG_DEBUG ("Processing HELLO packet received from node " << sender_node_ip);
  NS_ASSERT (sender_node_ip == hello_header.GetSenderNodeIp ());

  // Check if the neighbor node has been contacted before recently, if it has 
  // been contacted then ignore the HELLO, and if it hasn't store its IP address
  // in the table of recently contacted neighbors.
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

  ReplyHeader reply_header;
  received_packet->RemoveHeader (reply_header);

  // Answer with a REPLY BACK packet
  NS_LOG_DEBUG ("Answer with a REPLY BACK packet to node " << sender_node_ip);
  SendReplyBackPacket (sender_node_ip, reply_header.GetSummaryVector ());
}

void
RoutingProtocol::RecvReplyBackPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  NS_LOG_DEBUG ("Processing REPLY_BACK packet received from " << sender_node_ip);

  ReplyBackHeader reply_back_header;
  received_packet->RemoveHeader (reply_back_header);

  if (reply_back_header.GetSummaryVectorSize () == 0)
    {
      NS_LOG_DEBUG ("REPLY_BACK (REQUEST) packet with empty disjoint vector.");
      return;
    }

  NS_LOG_DEBUG ("Send all " << reply_back_header.GetSummaryVectorSize ()
                << " packets to node " << sender_node_ip);
  SendRequestedDataPackets (/*Requested packets*/ reply_back_header.GetSummaryVector (),
                            /*Destination node IP*/ sender_node_ip,
                            /*Destination node position*/ reply_back_header.GetPosition ());
}

void
RoutingProtocol::RecvDataPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  DataHeader data_header;
  received_packet->RemoveHeader (data_header);
  NS_LOG_DEBUG ("Received DATA packet " << data_header.GetDataIdentifier ()
                << " from " << sender_node_ip);

  if (m_packets_queue.Enqueue (data_header, sender_node_ip))
    {
      NS_LOG_DEBUG ("Packet successfully stored in packets queue.");
    }
  else
    {
      NS_LOG_DEBUG ("Packet not stored in packets queue.");
    }
}

void
RoutingProtocol::NewMessage (const std::string& message,
                             const GeoTemporalArea& destination_gta)
{
  NS_LOG_FUNCTION (this << destination_gta << " message size " << message.size ());

  const Ipv4Address local_ip = m_selected_interface_address.GetLocal ();

  const DataIdentifier data_id (local_ip, m_data_sequential_id++);

  const DataHeader data_packet (/*Data ID*/ data_id,
                                /*Dest. geo-temporal area*/ destination_gta,
                                /*Message*/ message,
                                /*Replicas to forward*/ m_default_data_packet_replicas);

  const bool enqueued = m_packets_queue.Enqueue (data_packet, local_ip);
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
                                               SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
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
                                               SPRAY_AND_WAIT_ROUTING_PROTOCOL_PORT));
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
  std::set<DataIdentifier> known_packets_vector;
  m_packets_queue.GetSummaryVector (known_packets_vector);

  ReplyHeader reply_header (known_packets_vector);
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
                                      const std::set<DataIdentifier>& received_summary_vector)
{
  NS_LOG_FUNCTION (this << destination_node << " summary vector size "
                   << received_summary_vector.size ());

  // I received a REPLY, construct a REPLY BACK (REQUEST)
  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  // Get the disjoint vector (the vector of unknown packets)
  NS_LOG_DEBUG ("Processing disjoint vector.");
  std::set<DataIdentifier> requested_packets;
  m_packets_queue.ProcessDisjointVector (received_summary_vector, requested_packets);
  NS_LOG_DEBUG ("Processed disjoint vector has " << requested_packets.size ()
                << " packets.");

  ReplyBackHeader reply_back_header (/*Disjoint vector*/ requested_packets,
                                     /*Node position*/ my_position);
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
RoutingProtocol::SendRequestedDataPackets (const std::set<DataIdentifier>& requested_packets_set,
                                           const Ipv4Address& destination_ip,
                                           const Vector2D& destination_node_position)
{
  NS_LOG_FUNCTION (this << " requested packets count " << requested_packets_set.size ()
                   << " destination address " << destination_ip
                   << " destination node position " << destination_node_position);

  // To avoid collisions, packets are sent X milliseconds apart of each other,
  // where X is the value of incremental_time_slot.
  uint16_t incremental_time_slot = 0u;

  PacketQueueEntry packet_entry;
  DataHeader data_to_send;
  const Time current_time = Simulator::Now ();
  uint32_t replicas_to_forward = 0u;

  for (std::set<DataIdentifier>::const_iterator requested_packet_it = requested_packets_set.begin ();
          requested_packet_it != requested_packets_set.end (); ++requested_packet_it)
    {
      NS_LOG_DEBUG ("Checking for packet " << *requested_packet_it);

      if (!m_packets_queue.Find (*requested_packet_it, packet_entry))
        {
          NS_LOG_DEBUG ("Packet " << *requested_packet_it << " not in packets queue.");
          continue;
        }

      const DataHeader & data_packet = packet_entry.GetDataPacket ();
      const GeoTemporalArea & destination_gta = data_packet.GetDestinationGeoTemporalArea ();

      // Check if the requesting node is inside the destination area AND the 
      // current time is the active time of the packet.
      if (destination_gta.IsInsideGeoTemporalArea (destination_node_position,
                                                   current_time))
        {
          // Given that the requesting node actually is inside the destination 
          // area during the active time of the data packet then the requesting
          // node is a DESTINATION node, send one (1) packet replica via unicast
          // without decreasing the replicas counter. 
          //
          // This is because when the packet is inside its destination area during
          // its active time it should be disseminated epidemically.
          NS_LOG_DEBUG ("Requesting node is inside destination geo-temporal area, "
                        "transmit node without decreasing replicas to keep.");
          replicas_to_forward = 1u;
        }
      else
        {
          // Check if the packet is in direct-transmission-only mode.
          if (packet_entry.GetReplicasCounter () <= 1u)
            {
              // It IS in direct transmission only, and the requesting node is 
              // not the destination (given that it is not inside the area during
              // the time period). Do not send packet.
              NS_LOG_DEBUG ("Only 1 replica left and requesting node is not "
                            "inside destination geo-temporal area, DON'T SEND.");
              continue;
            }

          // There are replicas to transmit, and the node is just a data mule, 
          // send some replicas (determined by the Spray & Wait mode).
          m_packets_queue.DiscountPacketReplicasToForward (/*Packet ID*/ data_packet.GetDataIdentifier (),
                                                           /*Replicas to forward*/ replicas_to_forward);
        }

      // Construct the packet to send
      data_to_send = DataHeader (data_packet);
      data_to_send.SetReplicasToForward (replicas_to_forward);
      NS_LOG_DEBUG ("Constructed DATA packet: " << data_to_send);

      Simulator::Schedule (MilliSeconds (incremental_time_slot),
                           &RoutingProtocol::SendDataPacket,
                           this,
                           destination_ip,
                           data_to_send);
      NS_LOG_DEBUG ("Scheduled DATA packet " << data_to_send.GetDataIdentifier ()
                    << " in " << incremental_time_slot << " milliseconds.");
      ++incremental_time_slot;
    }
}

void
RoutingProtocol::SendDataPacket (const Ipv4Address destination_ip,
                                 const DataHeader data_to_send)
{
  NS_LOG_FUNCTION (this << destination_ip << data_to_send);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (data_to_send);
  packet_to_send->AddHeader (TypeHeader (PacketType::Data));

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_ip))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Data, packet_size);

      // Log the transmission
      m_packets_queue.LogPacketTransmitted (data_to_send.GetDataIdentifier ());

      NS_LOG_DEBUG ("DATA packet sent from " << m_selected_interface_address.GetLocal ()
                    << " to " << destination_ip);
    }
  else
    {
      NS_LOG_DEBUG ("DATA packet could not be sent to " << destination_ip
                    << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::Start ()
{
  NS_LOG_FUNCTION (this);

  NS_LOG_DEBUG ("Initialize packets queue with binary mode "
                << (m_binary_mode ? "enabled" : "disabled") << " and "
                << "a capacity of " << m_packets_queue_capacity << " packets.");
  m_packets_queue = PacketsQueue (m_binary_mode, m_packets_queue_capacity);

  NS_LOG_DEBUG ("Initialize neighbors table with an expiration time of "
                << m_neighbor_expiration_time.GetSeconds () << " seconds.");
  m_neighbors_table = NeighborsTable (m_neighbor_expiration_time);

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
}

void
RoutingProtocol::Stop ()
{
  NS_LOG_FUNCTION (this);

  CancelHelloPacketTransmission ();
  m_neighbors_table.Clear ();
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

  // The call to SetUp (uint32_t) will call RoutingProtocol::Start ().
  m_ipv4->SetUp (m_last_known_selected_interface_index);
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

  // The call to SetDown (uint32_t) will call RoutingProtocol::Stop ().
  m_ipv4->SetDown (m_selected_interface_index);
  NS_LOG_DEBUG ("Node with ID " << m_node_id << " disabled at "
                << Simulator::Now ().GetSeconds () << " seconds.");
}

} // namespace geotemporal_spray_and_wait
} // namespace ns3

