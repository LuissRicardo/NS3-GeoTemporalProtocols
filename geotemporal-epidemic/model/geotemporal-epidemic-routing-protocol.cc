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

#include "geotemporal-epidemic-routing-protocol.h"

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

NS_LOG_COMPONENT_DEFINE ("GeoTemporalEpidemicRoutingProtocol");

namespace geotemporal_epidemic
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
m_default_packets_hops_count (8u),
m_node_id (-1),
m_ipv4 (0),
m_selected_interface_index (-1),
m_last_known_selected_interface_index (-1),
m_selected_interface_address (),
m_unicast_socket (0),
m_broadcast_socket (0),
m_data_sequential_id (0u),
m_packets_queue (m_packets_queue_capacity),
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
  static TypeId tid = TypeId ("ns3::geotemporal_epidemic::RoutingProtocol")
          .SetParent<Ipv4RoutingProtocol> ()
          .SetGroupName ("Geotemporal-epidemic")
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
          .AddAttribute ("DataPacketHopsCount", "Number of hops that the DATA packets are flooded before being discarded.",
                         UintegerValue (8),
                         MakeUintegerAccessor (&RoutingProtocol::m_default_packets_hops_count),
                         MakeUintegerChecker<uint32_t> (1))
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
  return packet->GetSize () // Entire Epidemic header size (including Type header)
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
                    "in the Geo-Temporal Epidemic operation.");
      return;
    }

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_DEBUG ("Added address ignored: Geo-Temporal Epidemic does NOT "
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
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal Epidemic does NOT "
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
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   EPIDEMIC_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   EPIDEMIC_ROUTING_PROTOCOL_PORT));
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
                    "in the Geo-Temporal Epidemic operation.");
      return;
    }

  // Check if the removed address was the operational interface address
  if (m_selected_interface_address != removed_interface_address)
    {
      NS_LOG_DEBUG ("Removed address is not participating in the "
                    "Geo-Temporal Epidemic operation.");
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
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetLocal (),
                                       EPIDEMIC_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_unicast_socket = socket;

      // Create also a socket to listen for broadcast directed packets
      socket = Socket::CreateSocket (GetObject <Node> (),
                                     UdpSocketFactory::GetTypeId ());
      NS_ASSERT (socket != 0);
      socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
      socket->BindToNetDevice (ipv4_l3->GetNetDevice (interface_index));
      socket->Bind (InetSocketAddress (interface_address.GetBroadcast (),
                                       EPIDEMIC_ROUTING_PROTOCOL_PORT));
      socket->SetAllowBroadcast (true);
      socket->SetAttribute ("IpTtl", UintegerValue (1));
      m_broadcast_socket = socket;

      NS_LOG_DEBUG ("Sockets created successfully!");
    }

  // If there are no more sockets active left, stop routing protocol operation.
  if (!m_unicast_socket)
    {
      NS_LOG_LOGIC ("No Geo-Temporal Epidemic interfaces.");
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
      NS_LOG_DEBUG ("Interface ignored: Geo-Temporal Epidemic does NOT "
                    "work with more than one interface. Already selected "
                    "interface " << m_selected_interface_index);
      return;
    }

  Ptr<Ipv4L3Protocol> ipv4_l3 = m_ipv4->GetObject<Ipv4L3Protocol> ();

  // Check that the interface has only 1 address
  if (ipv4_l3->GetNAddresses (interface_index) > 1)
    {
      NS_LOG_WARN ("Extra addresses ignored: Geo-Temporal Epidemic does NOT "
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
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetLocal (),
                                   EPIDEMIC_ROUTING_PROTOCOL_PORT));
  socket->SetAllowBroadcast (true);
  socket->SetAttribute ("IpTtl", UintegerValue (1));
  NS_ASSERT (m_unicast_socket == 0);
  m_unicast_socket = socket;

  // Create also a socket to listen for broadcast directed packets
  socket = Socket::CreateSocket (GetObject <Node> (),
                                 UdpSocketFactory::GetTypeId ());
  NS_ASSERT (socket != 0);
  socket->SetRecvCallback (MakeCallback (&RoutingProtocol::RecvEpidemicPacket, this));
  socket->BindToNetDevice (ipv4_l3->GetNetDevice (m_selected_interface_index));
  socket->Bind (InetSocketAddress (m_selected_interface_address.GetBroadcast (),
                                   EPIDEMIC_ROUTING_PROTOCOL_PORT));
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
                    "Epidemic operation.");
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

  NS_LOG_LOGIC ("Geo-Temporal Epidemic interface disabled.");
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
      NS_LOG_LOGIC ("No Geo-Temporal Epidemic interfaces.");
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
      // Geo-Temporal Epidemic does not support multicasting.
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
      NS_LOG_DEBUG ("No Geo-Temporal Epidemic interfaces.");
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
RoutingProtocol::RecvEpidemicPacket (Ptr<Socket> socket)
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

  NS_LOG_DEBUG ("Epidemic " << type_header << " packet received from node "
                << sender_node_ip);

  switch (type_header.GetPacketType ())
    {
    case PacketType::Hello:
      RecvHelloPacket (received_packet, sender_node_ip);
      return;
    case PacketType::Reply:
      RecvSummaryVectorPacket (received_packet, sender_node_ip, PacketType::Reply);
      return;
    case PacketType::ReplyBack:
      RecvSummaryVectorPacket (received_packet, sender_node_ip, PacketType::ReplyBack);
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

  NS_LOG_DEBUG ("Processing HELLO packet received from node " << sender_node_ip
                << " in node " << m_selected_interface_address.GetLocal ());
  NS_ASSERT (sender_node_ip == hello_header.GetSenderNodeIp ());

  // Anti-entropy session: if the current node has the smaller identifier then
  // it initiates the session, if not then ignore the HELLO.
  if (m_selected_interface_address.GetLocal ().Get () > sender_node_ip.Get ())
    {
      // Current node has a greater identifier, don't initiate session.
      NS_LOG_DEBUG ("HELLO ignored due to being from a node with smaller identifier ("
                    << sender_node_ip << ").");
      return;
    }

  // Current node has the smaller identifier, check if the neighbor node has 
  // been contacted before recently, if it has then ignore the HELLO.
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
  SendSummaryVectorPacket (sender_node_ip, PacketType::Reply);
}

void
RoutingProtocol::RecvSummaryVectorPacket (Ptr<Packet> received_packet,
                                          const Ipv4Address& sender_node_ip,
                                          const PacketType& reply_type)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip << reply_type);

  if (reply_type != PacketType::Reply && reply_type != PacketType::ReplyBack)
    NS_ABORT_MSG ("Packet type must be Reply or ReplyBack.");

  NS_LOG_DEBUG ("Processing " << reply_type << " packet received from " << sender_node_ip);

  SummaryVectorHeader summary_vector_header;
  received_packet->RemoveHeader (summary_vector_header);

  NS_LOG_DEBUG ("Process summary vector with " << summary_vector_header.GetSummaryVectorSize ()
                << " entries from node " << sender_node_ip);
  SendDisjointVectorPackets (/*Summary vector header*/ summary_vector_header,
                             /*Destination node IP*/ sender_node_ip);

  // If received a REPLY packet then answer with a REPLY_BACK packet.
  if (reply_type == PacketType::Reply)
    {
      NS_LOG_DEBUG ("Answer with a REPLY BACK packet to node " << sender_node_ip);
      SendSummaryVectorPacket (sender_node_ip, PacketType::ReplyBack);
    }
}

void
RoutingProtocol::RecvDataPacket (Ptr<Packet> received_packet, const Ipv4Address& sender_node_ip)
{
  NS_LOG_FUNCTION (this << received_packet << sender_node_ip);

  DataHeader data_header;
  received_packet->RemoveHeader (data_header);
  NS_LOG_DEBUG ("Received DATA packet <" << data_header << "> from node " << sender_node_ip);

  uint32_t hops_count = data_header.GetHopsCount ();
  NS_ABORT_MSG_IF (hops_count == 0u, "Hops count shouldn't get to 0.");

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  // Evaluate the value of the hops count field in the received DATA packet.
  if (hops_count == 1u)
    {
      // If the hops count is equal to 1 then the packet can only be received
      // by nodes that are inside the destination geo-temporal area.
      if (!data_header.GetDestinationGeoTemporalArea ()
          .IsInsideGeoTemporalArea (my_position, Simulator::Now ()))
        {
          // The node is not inside the destination geo-temporal area, drop packet.
          NS_LOG_DEBUG ("DATA packet " << data_header.GetDataIdentifier ()
                        << " dropped because the hops limit has been reached.");
          return;
        }

      // Packet received by node inside the destination geo-temporal area, keep
      // the hops count unchanged (1).
      NS_LOG_DEBUG ("Packet with hops count 1 received by DESTINATION node. Store "
                    "packet with hops count 1.");
    }
  else
    {
      // The hops count is greater than 1, then just decrease by 1 the hops count.
      NS_LOG_DEBUG ("Packet with hops count " << hops_count << ". Store packet "
                    "with hops count " << hops_count - 1 << ".");
      --hops_count;
    }

  data_header.SetHopsCount (hops_count);

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
                                /*Hops count*/ m_default_packets_hops_count);

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
                                               EPIDEMIC_ROUTING_PROTOCOL_PORT));
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
                                               EPIDEMIC_ROUTING_PROTOCOL_PORT));
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
RoutingProtocol::SendSummaryVectorPacket (const Ipv4Address& destination_node,
                                          const PacketType& reply_type)
{
  NS_LOG_FUNCTION (this << destination_node << reply_type);

  if (reply_type != PacketType::Reply && reply_type != PacketType::ReplyBack)
    NS_ABORT_MSG ("Packet type must be Reply or ReplyBack.");

  // Get the summary vector (the vector of known packets)
  std::set<DataIdentifier> known_packets_vector;
  m_packets_queue.GetSummaryVector (known_packets_vector);

  Vector2D my_position, my_velocity;
  GetNodeMobility (my_position, my_velocity);

  SummaryVectorHeader summary_vector_header (/*Summary vector*/ known_packets_vector,
                                             /*Position*/ my_position);
  NS_LOG_DEBUG ("Constructed SUMMARY VECTOR (" << reply_type << ") packet: "
                << summary_vector_header);

  Ptr<Packet> packet_to_send = Create<Packet> ();
  packet_to_send->AddHeader (summary_vector_header);
  packet_to_send->AddHeader (TypeHeader (reply_type));

  // Transmit the packet
  if (SendUnicastPacket (packet_to_send, destination_node))
    {
      // Count the transmitted packet
      uint32_t packet_size = RoutingProtocol::CalculateCompletePacketSize (packet_to_send);
      m_tx_packets_counter.CountPacket (PacketClass::Control, packet_size);

      NS_LOG_DEBUG ("SUMMARY VECTOR (" << reply_type << ") packet sent from "
                    << m_selected_interface_address.GetLocal ()
                    << " to " << destination_node);
    }
  else
    {
      NS_LOG_DEBUG ("SUMMARY VECTOR (" << reply_type << ") packet could not be sent to "
                    << destination_node << " (maybe node is disabled).");
    }
}

void
RoutingProtocol::SendDisjointVectorPackets (const SummaryVectorHeader& summary_vector_header,
                                            const Ipv4Address& destination_ip)
{
  NS_LOG_FUNCTION (this << " summary vector entries count " << summary_vector_header.GetSummaryVectorSize ()
                   << " destination IP " << destination_ip
                   << " destination node position " << summary_vector_header.GetPosition ());

  // Compute disjoint vector from the received summary vector
  std::set<DataIdentifier> disjoint_vector;
  m_packets_queue.ProcessDisjointVector (summary_vector_header,
                                         destination_ip,
                                         disjoint_vector);

  // To avoid collisions, packets are sent X milliseconds apart of each other,
  // where X is the value of incremental_time_slot.
  uint16_t incremental_time_slot = 0u;

  PacketQueueEntry packet_entry;

  if (disjoint_vector.empty ())
    {
      NS_LOG_DEBUG ("Disjoint vector empty, no packets to send to node " << destination_ip);
      return;
    }

  NS_LOG_DEBUG ("Schedule to send " << disjoint_vector.size ()
                << " packets from the disjoint vector.");

  for (std::set<DataIdentifier>::const_iterator packet_it = disjoint_vector.begin ();
          packet_it != disjoint_vector.end (); ++packet_it)
    {
      if (!m_packets_queue.Find (*packet_it, packet_entry))
        {
          NS_LOG_DEBUG ("Packet " << *packet_it << " not in packets queue.");
          continue;
        }

      NS_LOG_DEBUG ("DATA packet to be sent: " << packet_entry.GetDataPacket ());

      Simulator::Schedule (MilliSeconds (incremental_time_slot),
                           &RoutingProtocol::SendDataPacket,
                           this,
                           destination_ip,
                           packet_entry.GetDataPacket ());
      NS_LOG_DEBUG ("Scheduled DATA packet " << packet_entry.GetDataPacket ().GetDataIdentifier ()
                    << " in " << incremental_time_slot << " milliseconds.");
      ++incremental_time_slot;
    }

  NS_LOG_DEBUG (disjoint_vector.size () << " were scheduled to be sent to node "
                << destination_ip);
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

  NS_LOG_DEBUG ("Initialize packets queue with a capacity of "
                << m_packets_queue_capacity << " packets.");
  m_packets_queue = PacketsQueue (m_packets_queue_capacity);

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

  // Clear memory that won't be used
  m_neighbors_table.Clear ();
  m_packets_queue.Clear ();
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

} // namespace geotemporal_epidemic
} // namespace ns3

