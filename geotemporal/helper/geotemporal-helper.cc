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
 */

#include "geotemporal-helper.h"

#include <utility>

#include <ns3/assert.h>
#include <ns3/geotemporal-routing-protocol.h>


namespace ns3
{

GeoTemporalHelper::GeoTemporalHelper () : Ipv4RoutingHelper ()
{
  m_object_factory.SetTypeId ("ns3::geotemporal::RoutingProtocol");
}

GeoTemporalHelper::GeoTemporalHelper (const GeoTemporalHelper& copy)
: m_object_factory (copy.m_object_factory) { }

GeoTemporalHelper*
GeoTemporalHelper::Copy (void) const
{
  return new GeoTemporalHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
GeoTemporalHelper::Create (Ptr<Node> node) const
{
  Ptr<geotemporal::RoutingProtocol> agent = m_object_factory.Create<geotemporal::RoutingProtocol> ();
  node->AggregateObject (agent);
  return agent;
}

void
GeoTemporalHelper::Set (const std::string& name, const AttributeValue& value)
{
  m_object_factory.Set (name, value);
}

int64_t
GeoTemporalHelper::AssignStreams (NodeContainer nodes, int64_t stream)
{
  int64_t current_stream = stream;
  Ptr<Node> node;

  for (NodeContainer::Iterator node_it = nodes.Begin ();
          node_it != nodes.End (); ++node_it)
    {
      node = (*node_it);

      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      NS_ASSERT_MSG (ipv4, "Ipv4 not installed on node");

      Ptr<Ipv4RoutingProtocol> routing_protocol = ipv4->GetRoutingProtocol ();
      NS_ASSERT_MSG (routing_protocol, "Ipv4 routing protocol not installed on node");

      Ptr<geotemporal::RoutingProtocol> gt_saw
              = DynamicCast<geotemporal::RoutingProtocol> (routing_protocol);

      if (gt_saw)
        {
          current_stream += gt_saw->AssignStreams (current_stream);
          continue;
        }

      // Geo-temporal may also be in a list
      Ptr<Ipv4ListRouting> list = DynamicCast<Ipv4ListRouting> (gt_saw);

      if (list)
        {
          int16_t priority;
          Ptr<Ipv4RoutingProtocol> protocol_item;
          Ptr<geotemporal::RoutingProtocol> saw_item;

          for (uint32_t i = 0; i < list->GetNRoutingProtocols (); i++)
            {
              protocol_item = list->GetRoutingProtocol (i, priority);
              saw_item = DynamicCast<geotemporal::RoutingProtocol> (protocol_item);

              if (saw_item)
                {
                  current_stream += saw_item->AssignStreams (current_stream);
                  break;
                }
            }
        }
    }

  return (current_stream - stream);
}

} // namespace ns3

