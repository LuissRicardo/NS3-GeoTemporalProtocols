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
 * Geo-Temporal Restricted Epidemic protocol packets.
 */

#include "geotemporal-restricted-epidemic-packets.h"

#include <algorithm>
#include <cstdio>

#include <ns3/abort.h>
#include <ns3/address-utils.h>
#include <ns3/assert.h>
#include <ns3/packet.h>

#define COORDINATE_X 7
#define COORDINATE_Y 6
#define AREA_X1 3
#define AREA_Y1 2
#define AREA_X2 1
#define AREA_Y2 0


namespace ns3
{
namespace geotemporal_restricted_epidemic
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;


// =============================================================================
//                                  PacketType
// =============================================================================

std::string
ToString (const PacketType& obj)
{
  switch (obj)
    {
    case PacketType::Hello:
      return "Hello";
    case PacketType::Reply:
      return "Reply";
    case PacketType::ReplyBack:
      return "ReplyBack";
    case PacketType::Data:
      return "Data";
    default:
      return "Unknown";
      // NS_ABORT_MSG ("Unknown packet type.");
    }
}


// =============================================================================
//                                   TypeHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (TypeHeader);

TypeHeader::TypeHeader ()
: m_packet_type (PacketType::Hello), m_valid (true) { }

TypeHeader::TypeHeader (PacketType packet_type)
: m_packet_type (packet_type), m_valid (true) { }

TypeHeader::TypeHeader (const TypeHeader& copy)
: m_packet_type (copy.m_packet_type), m_valid (copy.m_valid) { }


// --------------------------
// Type ID
// --------------------------

TypeId
TypeHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal_restricted_epidemic::TypeHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal-restricted-epidemic")
          .AddConstructor<TypeHeader> ();
  return type_id;
}

TypeId
TypeHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
TypeHeader::GetSerializedSize () const
{
  return 1u;
}

void
TypeHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 ((uint8_t) m_packet_type);
}

uint32_t
TypeHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  uint8_t packet_type = it.ReadU8 ();

  m_valid = true;

  switch (static_cast<PacketType> (packet_type))
    {
    case PacketType::Hello:
    case PacketType::Reply:
    case PacketType::ReplyBack:
    case PacketType::Data:
      m_packet_type = static_cast<PacketType> (packet_type);
      break;
    default:
      m_valid = false;
    }

  uint32_t distance = it.GetDistanceFrom (start);
  NS_ASSERT (distance == GetSerializedSize ());

  return distance;
}

void
TypeHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
TypeHeader::ToString () const
{
  if (m_valid == false) return "Unknown";

  return geotemporal_restricted_epidemic::ToString (m_packet_type);
}


// =============================================================================
//                                  HelloHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (HelloHeader);

HelloHeader::HelloHeader ()
: m_reserved (0u), m_summary_vector_size (0u), m_sender_node_ip () { }

HelloHeader::HelloHeader (const Ipv4Address& node_ip, uint16_t summary_vector_size)
: m_reserved (0u), m_summary_vector_size (summary_vector_size),
m_sender_node_ip (node_ip) { }

HelloHeader::HelloHeader (const HelloHeader& copy)
: m_reserved (copy.m_reserved), m_summary_vector_size (copy.m_summary_vector_size),
m_sender_node_ip (copy.m_sender_node_ip) { }


// --------------------------
// Type ID
// --------------------------

TypeId
HelloHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal_restricted_epidemic::HelloHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal-restricted-epidemic")
          .AddConstructor<HelloHeader> ();
  return type_id;
}

TypeId
HelloHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
HelloHeader::GetSerializedSize () const
{
  return 7u;
}

void
HelloHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (m_reserved);
  start.WriteHtonU16 (m_summary_vector_size);
  WriteTo (start, m_sender_node_ip);
}

uint32_t
HelloHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  m_reserved = it.ReadU8 ();
  m_summary_vector_size = it.ReadNtohU16 ();
  ReadFrom (it, m_sender_node_ip);

  uint32_t distance = it.GetDistanceFrom (start);
  NS_ASSERT (distance == GetSerializedSize ());
  return distance;
}

void
HelloHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
HelloHeader::ToString () const
{
  char buffer [6];
  std::sprintf (buffer, "%u", m_summary_vector_size);
  std::string str = "HELLO from "
          + GeoTemporalLibrary::LibraryUtils::ToString (m_sender_node_ip)
          + " who knows " + std::string (buffer) + " packets";
  return str;
}


// =============================================================================
//                              SummaryVectorHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (SummaryVectorHeader);

SummaryVectorHeader::SummaryVectorHeader ()
: m_summary_vector (), m_position () { }

SummaryVectorHeader::SummaryVectorHeader (const std::set<DataIdentifier>& summary_vector,
                                          const GeoTemporalLibrary::LibraryUtils::Vector2D& position)
: m_summary_vector (summary_vector), m_position (position) { }

SummaryVectorHeader::SummaryVectorHeader (const SummaryVectorHeader& copy)
: m_summary_vector (copy.m_summary_vector), m_position (copy.m_position) { }


// --------------------------
// Type ID
// --------------------------

TypeId
SummaryVectorHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal_restricted_epidemic::SummaryVectorHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal-restricted-epidemic")
          .AddConstructor<SummaryVectorHeader> ();
  return type_id;
}

TypeId
SummaryVectorHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
SummaryVectorHeader::GetSerializedSize () const
{
  return 19u + (m_summary_vector.size () * 6u);
}

void
SummaryVectorHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t x_int, x_float;
  EncodeDoubleToIntegers (m_position.m_x, x_int, x_float,
                          sign_flags, COORDINATE_X);

  uint32_t y_int, y_float;
  EncodeDoubleToIntegers (m_position.m_y, y_int, y_float,
                          sign_flags, COORDINATE_Y);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_summary_vector.size ());

  start.WriteHtonU32 (x_int);
  start.WriteHtonU32 (x_float);
  start.WriteHtonU32 (y_int);
  start.WriteHtonU32 (y_float);

  bool odd = true;
  for (std::set<DataIdentifier>::const_iterator it = m_summary_vector.begin ();
          it != m_summary_vector.end (); ++it)
    {
      if (odd)
        {
          WriteTo (start, it->GetSourceIp ());
          start.WriteHtonU16 (it->GetSourceId ());
        }
      else
        {
          start.WriteHtonU16 (it->GetSourceId ());
          WriteTo (start, it->GetSourceIp ());
        }
      odd = !odd;
    }
}

uint32_t
SummaryVectorHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  const uint8_t sign_flags = it.ReadU8 ();
  const uint16_t summary_vector_size = it.ReadNtohU16 ();

  uint32_t int_part = it.ReadNtohU32 ();
  uint32_t float_part = it.ReadNtohU32 ();
  m_position.m_x = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_X);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  m_position.m_y = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_Y);

  m_summary_vector = std::set<DataIdentifier> ();
  Ipv4Address ip;
  uint16_t id;
  bool odd = true;

  for (uint32_t k = 0; k < summary_vector_size; ++k)
    {
      if (odd)
        {
          ReadFrom (it, ip);
          id = it.ReadNtohU16 ();
        }
      else
        {
          id = it.ReadNtohU16 ();
          ReadFrom (it, ip);
        }

      odd = !odd;
      m_summary_vector.insert (DataIdentifier (ip, id));
    }

  uint32_t distance = it.GetDistanceFrom (start);
  NS_ASSERT (distance == GetSerializedSize ());
  return distance;
}

void
SummaryVectorHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
SummaryVectorHeader::ToString () const
{
  char buffer [6];
  std::sprintf (buffer, "%u", (uint32_t) m_summary_vector.size ());

  std::string str = "SUMMARY_VECTOR sent from position " + m_position.ToString ()
          + " with " + std::string (buffer) + " entries: ";

  for (std::set<DataIdentifier>::const_iterator it = m_summary_vector.begin ();
          it != m_summary_vector.end (); ++it)
    {
      str += it->ToString () + " ";
    }

  return str;
}


// =============================================================================
//                                  DataHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (DataHeader);

DataHeader::DataHeader ()
: m_data_id (), m_creation_coordinates (), m_dest_geo_temporal_area (), m_message (),
m_hops_counter (0u) { }

DataHeader::DataHeader (const DataIdentifier& data_id,
                        const GeoTemporalLibrary::LibraryUtils::Vector2D& creation_coordinates,
                        const GeoTemporalArea& destination_geo_temporal_area,
                        const std::string message,
                        const uint32_t& hops_count)
: m_data_id (data_id), m_creation_coordinates (creation_coordinates),
m_dest_geo_temporal_area (destination_geo_temporal_area),
m_message (message), m_hops_counter (hops_count) { }

DataHeader::DataHeader (const DataHeader& copy)
: m_data_id (copy.m_data_id), m_creation_coordinates (copy.m_creation_coordinates),
m_dest_geo_temporal_area (copy.m_dest_geo_temporal_area),
m_message (copy.m_message), m_hops_counter (copy.m_hops_counter) { }


// --------------------------
// Processing functions
// --------------------------

Area
DataHeader::GetAllowedArea () const
{
  const Area & dest_area = m_dest_geo_temporal_area.GetArea ();

  return Area (std::min (m_creation_coordinates.m_x, dest_area.GetX1 ()),
               std::min (m_creation_coordinates.m_y, dest_area.GetY1 ()),
               std::max (m_creation_coordinates.m_x, dest_area.GetX2 ()),
               std::max (m_creation_coordinates.m_y, dest_area.GetY2 ()));
}


// --------------------------
// Type ID
// --------------------------

TypeId
DataHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal_restricted_epidemic::DataHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal-restricted-epidemic")
          .AddConstructor<DataHeader> ();
  return type_id;
}

TypeId
DataHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
DataHeader::GetSerializedSize () const
{
  return 64u + m_message.length (); // 1 byte is because of the null terminator of the message.
}

void
DataHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t x_int, x_float;
  EncodeDoubleToIntegers (m_creation_coordinates.m_x,
                          x_int, x_float, sign_flags, COORDINATE_X);

  uint32_t y_int, y_float;
  EncodeDoubleToIntegers (m_creation_coordinates.m_y,
                          y_int, y_float, sign_flags, COORDINATE_Y);

  uint32_t x1_int, x1_float;
  EncodeDoubleToIntegers (m_dest_geo_temporal_area.GetArea ().GetX1 (),
                          x1_int, x1_float, sign_flags, AREA_X1);

  uint32_t y1_int, y1_float;
  EncodeDoubleToIntegers (m_dest_geo_temporal_area.GetArea ().GetY1 (),
                          y1_int, y1_float, sign_flags, AREA_Y1);

  uint32_t x2_int, x2_float;
  EncodeDoubleToIntegers (m_dest_geo_temporal_area.GetArea ().GetX2 (),
                          x2_int, x2_float, sign_flags, AREA_X2);

  uint32_t y2_int, y2_float;
  EncodeDoubleToIntegers (m_dest_geo_temporal_area.GetArea ().GetY2 (),
                          y2_int, y2_float, sign_flags, AREA_Y2);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_data_id.GetSourceId ());

  WriteTo (start, m_data_id.GetSourceIp ());

  start.WriteHtonU32 (m_hops_counter);

  start.WriteHtonU32 (x_int);
  start.WriteHtonU32 (x_float);

  start.WriteHtonU32 (y_int);
  start.WriteHtonU32 (y_float);

  start.WriteHtonU32 (x1_int);
  start.WriteHtonU32 (x1_float);

  start.WriteHtonU32 (y1_int);
  start.WriteHtonU32 (y1_float);

  start.WriteHtonU32 (x2_int);
  start.WriteHtonU32 (x2_float);

  start.WriteHtonU32 (y2_int);
  start.WriteHtonU32 (y2_float);

  start.WriteHtonU16 ((uint32_t) m_dest_geo_temporal_area.GetTimePeriod ().GetStartTime ().GetSeconds ());
  start.WriteHtonU16 ((uint32_t) m_dest_geo_temporal_area.GetDuration ().GetSeconds ());

  char chars_buffer [m_message.length () + 1u];
  std::strcpy (chars_buffer, m_message.c_str ());
  start.Write ((uint8_t *) chars_buffer, std::strlen (chars_buffer) + 1u);
}

uint32_t
DataHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  const uint8_t sign_flags = it.ReadU8 ();

  // Read DATA packet ID
  uint16_t data_source_id = it.ReadNtohU16 ();
  Ipv4Address data_source_ip;
  ReadFrom (it, data_source_ip);

  m_data_id = DataIdentifier (data_source_ip, data_source_id);

  m_hops_counter = it.ReadNtohU32 ();

  // Read creation coordinates
  uint32_t int_part = it.ReadNtohU32 ();
  uint32_t float_part = it.ReadNtohU32 ();
  double creation_x = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, COORDINATE_X);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  double creation_y = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, COORDINATE_Y);

  m_creation_coordinates = GeoTemporalLibrary::LibraryUtils::Vector2D (creation_x, creation_y);

  // Read destination area coordinates
  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  double area_x1 = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, AREA_X1);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  double area_y1 = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, AREA_Y1);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  double area_x2 = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, AREA_X2);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  double area_y2 = DecodeDoubleFromIntegers (int_part, float_part, sign_flags, AREA_Y2);

  // Read temporal scope's start time and duration. Then calculate end time.
  Time start_time = Seconds (it.ReadNtohU16 ());
  Time duration = Seconds (it.ReadNtohU16 ());
  Time end_time = TimePeriod::CalculateEndTime (start_time, duration);

  m_dest_geo_temporal_area = GeoTemporalArea (TimePeriod (start_time, end_time),
                                              Area (area_x1, area_y1, area_x2, area_y2));

  // Read the string message.
  uint8_t character_read = 0u;
  uint32_t message_size = 0u;
  Buffer::Iterator temporal_it = it;

  // Find the size/length of the message by counting the number of characters 
  // before the null terminator character is found. The null terminator character
  // is also counted.
  do
    {
      character_read = temporal_it.ReadU8 ();
      ++message_size;
    }
  while (character_read != 0u);

  // Read the message-size characters from the iterator-buffer.
  char message_buffer [message_size];
  it.Read ((uint8_t *) message_buffer, message_size);

  // Store the message in a C++ std::string.
  m_message = std::string (message_buffer);

  uint32_t distance = it.GetDistanceFrom (start);
  NS_ASSERT (distance == GetSerializedSize ());
  return distance;
}

void
DataHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
DataHeader::ToString () const
{
  char buffer[10];
  std::sprintf (buffer, "%u", m_hops_counter);

  std::string str = "DATA " + m_data_id.ToString () + " (" + std::string (buffer)
          + " hops left) created at " + m_creation_coordinates.ToString ()
          + " destined to area " + m_dest_geo_temporal_area.GetArea ().ToString ();

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetTimePeriod ().GetStartTime ().GetSeconds ());
  str += " to start at second " + std::string (buffer);

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetDuration ().GetSeconds ());
  str += " with a duration of " + std::string (buffer) + " seconds";

  std::sprintf (buffer, "%u", (uint32_t) m_message.size ());
  str += " has a message of " + std::string (buffer) + " byte(s)";
  return str;
}


} // namespace geotemporal_restricted_epidemic
} // namespace ns3

