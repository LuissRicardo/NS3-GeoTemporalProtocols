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
 * This is an example application for the Geo-Temporal routing protocol.
 * 
 * Author: Luis Ricardo Gallego Tercero <luiss_121314@hotmail.com>
 */

#include "geotemporal-application.h"

#include <ns3/geotemporal-routing-protocol.h>
#include <ns3/log.h>


namespace ns3
{

// =============================================================================
//                             GeoTemporalApplication
// =============================================================================

NS_LOG_COMPONENT_DEFINE ("GeoTemporalApplication");

GeoTemporalApplication::GeoTemporalApplication ()
: m_node (0), m_message (), m_destination_gta (), m_running_flag (false),
m_event (), m_sent_packets_counter (0), m_data_rate (1000), m_packet_size (128),
m_packets_number (2), m_multiple_messages (true), m_characters_list ()
{
  char chars[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '/', '*', '-', '+', '!', '@', '#', '$', '%', '^', '&', '(', ')', '_', '='};
  m_characters_list = std::vector<char> (chars, chars + sizeof (chars) / sizeof (char));
}

GeoTemporalApplication::GeoTemporalApplication (const GeoTemporalApplication& copy)
: m_node (copy.m_node),
m_message (copy.m_message),
m_destination_gta (copy.m_destination_gta),
m_running_flag (copy.m_running_flag),
m_event (copy.m_event),
m_sent_packets_counter (copy.m_sent_packets_counter),
m_data_rate (copy.m_data_rate),
m_packet_size (copy.m_packet_size),
m_packets_number (copy.m_packets_number),
m_multiple_messages (copy.m_multiple_messages),
m_characters_list (copy.m_characters_list) { }

GeoTemporalApplication::~GeoTemporalApplication ()
{
  m_node = 0;
}

void
GeoTemporalApplication::ConfigureMessage (Ptr<Node> source_node,
                                          const GeoTemporalArea& destination_geo_temporal_area,
                                          const std::string& message)
{
  m_multiple_messages = false;
  m_node = source_node;
  m_destination_gta = destination_geo_temporal_area;
  m_message = message;
}

void
GeoTemporalApplication::ConfigureMultipleMessages (Ptr<Node> source_node,
                                                   const GeoTemporalArea& destination_geo_temporal_area,
                                                   const uint32_t data_rate,
                                                   const uint32_t packets_size,
                                                   const uint32_t packets_number)
{
  m_multiple_messages = true;
  m_node = source_node;
  m_destination_gta = destination_geo_temporal_area;

  m_data_rate = data_rate;
  m_packet_size = packets_size;
  m_packets_number = packets_number;
}

void
GeoTemporalApplication::StartApplication ()
{
  m_running_flag = true;
  SendPacket ();
}

void
GeoTemporalApplication::StopApplication ()
{
  m_running_flag = false;

  if (m_event.IsRunning ())
    {
      Simulator::Cancel (m_event);
    }
}

void
GeoTemporalApplication::SendPacket ()
{
  Ptr<geotemporal::RoutingProtocol> routing_protocol =
          m_node->GetObject<geotemporal::RoutingProtocol> ();

  // Single packet or multiple packets?
  if (!m_multiple_messages)
    {
      // Single packet
      routing_protocol->NewMessage (m_message, m_destination_gta);
    }
  else
    {
      // Multiple packets
      const uint32_t char_index = m_sent_packets_counter % m_characters_list.size ();

      m_message = "";
      m_message.append (m_packet_size, m_characters_list[char_index]);

      routing_protocol->NewMessage (m_message, m_destination_gta);

      ++m_sent_packets_counter;

      if (m_sent_packets_counter < m_packets_number)
        {
          ScheduleTransmission ();
        }
    }
}

void
GeoTemporalApplication::ScheduleTransmission ()
{
  if (m_running_flag)
    {
      m_event = Simulator::Schedule (MilliSeconds (m_data_rate),
                                     &GeoTemporalApplication::SendPacket,
                                     this);
    }
}

} // namespace ns3

