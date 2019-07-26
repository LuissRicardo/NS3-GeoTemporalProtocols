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

#include "geotemporal-epidemic-helper.h"

#include <utility>

#include <ns3/assert.h>
#include <ns3/geotemporal-epidemic-routing-protocol.h>


namespace ns3
{

GeoTemporalEpidemicHelper::GeoTemporalEpidemicHelper () : Ipv4RoutingHelper ()
{
  m_object_factory.SetTypeId ("ns3::geotemporal_epidemic::RoutingProtocol");
}

GeoTemporalEpidemicHelper::GeoTemporalEpidemicHelper (const GeoTemporalEpidemicHelper& copy)
: m_object_factory (copy.m_object_factory) { }

GeoTemporalEpidemicHelper*
GeoTemporalEpidemicHelper::Copy (void) const
{
  return new GeoTemporalEpidemicHelper (*this);
}

Ptr<Ipv4RoutingProtocol>
GeoTemporalEpidemicHelper::Create (Ptr<Node> node) const
{
  Ptr<geotemporal_epidemic::RoutingProtocol> agent = m_object_factory.Create<geotemporal_epidemic::RoutingProtocol> ();
  node->AggregateObject (agent);
  return agent;
}

void
GeoTemporalEpidemicHelper::Set (const std::string& name, const AttributeValue& value)
{
  m_object_factory.Set (name, value);
}

int64_t
GeoTemporalEpidemicHelper::AssignStreams (NodeContainer nodes, int64_t stream)
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

      Ptr<geotemporal_epidemic::RoutingProtocol> gt_epi
              = DynamicCast<geotemporal_epidemic::RoutingProtocol> (routing_protocol);

      if (gt_epi)
        {
          current_stream += gt_epi->AssignStreams (current_stream);
          continue;
        }

      // Geo-temporal Epidemic may also be in a list
      Ptr<Ipv4ListRouting> list = DynamicCast<Ipv4ListRouting> (gt_epi);

      if (list)
        {
          int16_t priority;
          Ptr<Ipv4RoutingProtocol> protocol_item;
          Ptr<geotemporal_epidemic::RoutingProtocol> epi_item;

          for (uint32_t i = 0; i < list->GetNRoutingProtocols (); i++)
            {
              protocol_item = list->GetRoutingProtocol (i, priority);
              epi_item = DynamicCast<geotemporal_epidemic::RoutingProtocol> (protocol_item);

              if (epi_item)
                {
                  current_stream += epi_item->AssignStreams (current_stream);
                  break;
                }
            }
        }
    }

  return (current_stream - stream);
}

} // namespace ns3

