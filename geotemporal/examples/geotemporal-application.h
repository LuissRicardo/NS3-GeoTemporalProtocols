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

#ifndef GEOTEMPORAL_APPLICATION_H
#define GEOTEMPORAL_APPLICATION_H

#include <string>

#include <ns3/core-module.h>
#include <ns3/applications-module.h>

#include <ns3/geotemporal-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;

namespace ns3
{

// =============================================================================
//                             GeoTemporalApplication
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Application for the Geo-Temporal protocol. It is associated with individual 
 * nodes. Each node holds a list of references (smart pointers) to its 
 * applications.
 * 
 * The purpose to this application is so the nodes can send one or multiple 
 * Geo-Temporal messages.
 */
class GeoTemporalApplication : public Application
{
private:

  Ptr<Node> m_node;

  std::string m_message;
  GeoTemporalArea m_destination_gta;

  bool m_running_flag;
  EventId m_event;
  uint32_t m_sent_packets_counter;

  uint32_t m_data_rate;
  uint32_t m_packet_size;
  uint32_t m_normal_packets_number;
  uint32_t m_emergency_packets_number;
  uint32_t m_total_packets_number;
  bool m_multiple_messages;

  std::vector<char> m_characters_list;

public:

  GeoTemporalApplication ();
  GeoTemporalApplication (const GeoTemporalApplication & copy);

  virtual ~GeoTemporalApplication ();


  void ConfigureMessage (Ptr<Node> source_node,
                         const GeoTemporalArea & destination_geo_temporal_area,
                         const std::string & message);

  /**
   * This function configures the node to generate <code>packets_number</code>
   * packets with normal priority.
   */
  void ConfigureMultipleMessages (Ptr<Node> source_node,
                                  const GeoTemporalArea & destination_geo_temporal_area,
                                  const uint32_t data_rate,
                                  const uint32_t packets_size,
                                  const uint32_t packets_number);

  /**
   * This function configures the node to generate <code>normal_packets_number
   * </code> packets with normal priority and <code>normal_packets_number</code>
   * packets with emergency priority.
   */
  void ConfigureMultipleMessages (Ptr<Node> source_node,
                                  const GeoTemporalArea & destination_geo_temporal_area,
                                  const uint32_t data_rate,
                                  const uint32_t packets_size,
                                  const uint32_t normal_packets_number,
                                  const uint32_t emergency_packets_number);

private:

  void StartApplication () override;

  void StopApplication () override;

  void SendPacket (void);

  void ScheduleTransmission (void);

};

} // namespace ns3

#endif /* GEOTEMPORAL_APPLICATION_H */

