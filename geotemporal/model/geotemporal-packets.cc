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
 * Geo-Temporal protocol packets.
 */

#include "geotemporal-packets.h"

#include <cstdio>

#include <ns3/abort.h>
#include <ns3/address-utils.h>
#include <ns3/assert.h>
#include <ns3/packet.h>

#define COORDINATE_X 7
#define COORDINATE_Y 6
#define VELOCITY_X 5
#define VELOCITY_Y 4
#define AREA_X1 3
#define AREA_Y1 2
#define AREA_X2 1
#define AREA_Y2 0

#define EMERGENCY 7


namespace ns3
{
namespace geotemporal
{


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
    case PacketType::Ack:
      return "Ack";
    case PacketType::Data:
      return "Data";
    case PacketType::DataAck:
      return "DataAck";
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
  static TypeId type_id = TypeId ("ns3::geotemporal::TypeHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
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
    case PacketType::Ack:
    case PacketType::Data:
    case PacketType::DataAck:
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

  return geotemporal::ToString (m_packet_type);
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
  static TypeId type_id = TypeId ("ns3::geotemporal::HelloHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
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
: m_summary_vector (), m_position (), m_velocity () { }

SummaryVectorHeader::SummaryVectorHeader (const std::set<DataIdentifier>& summary_vector,
                                          const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                                          const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity)
: m_summary_vector (summary_vector), m_position (position), m_velocity (velocity) { }

SummaryVectorHeader::SummaryVectorHeader (const SummaryVectorHeader& copy)
: m_summary_vector (copy.m_summary_vector), m_position (copy.m_position),
m_velocity (copy.m_velocity) { }


// --------------------------
// Type ID
// --------------------------

TypeId
SummaryVectorHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal::SummaryVectorHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
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
  return 27u + (m_summary_vector.size () * 6u);
}

void
SummaryVectorHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t position_x_int, position_x_float;
  EncodeDoubleToIntegers (m_position.m_x, position_x_int, position_x_float,
                          sign_flags, COORDINATE_X);

  uint32_t position_y_int, position_y_float;
  EncodeDoubleToIntegers (m_position.m_y, position_y_int, position_y_float,
                          sign_flags, COORDINATE_Y);

  uint32_t velocity_x_int;
  EncodeFloatToInteger (m_velocity.m_x, velocity_x_int, sign_flags, VELOCITY_X);

  uint32_t velocity_y_int;
  EncodeFloatToInteger (m_velocity.m_y, velocity_y_int, sign_flags, VELOCITY_Y);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_summary_vector.size ());

  start.WriteHtonU32 (position_x_int);
  start.WriteHtonU32 (position_x_float);
  start.WriteHtonU32 (position_y_int);
  start.WriteHtonU32 (position_y_float);

  start.WriteHtonU32 (velocity_x_int);
  start.WriteHtonU32 (velocity_y_int);

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

  int_part = it.ReadNtohU32 ();
  m_velocity.m_x = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_X);

  int_part = it.ReadNtohU32 ();
  m_velocity.m_y = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_Y);

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
          + " at velocity " + m_velocity.ToString ()
          + " with " + std::string (buffer) + " entries: ";

  for (std::set<DataIdentifier>::const_iterator it = m_summary_vector.begin ();
          it != m_summary_vector.end (); ++it)
    {
      str += it->ToString () + " ";
    }

  return str;
}


// =============================================================================
//                                   AckHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (AckHeader);

AckHeader::AckHeader ()
: m_data_id (), m_position (), m_velocity () { }

AckHeader::AckHeader (const DataIdentifier& data_identifier,
                      const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                      const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity)
: m_data_id (data_identifier), m_position (position), m_velocity (velocity) { }

AckHeader::AckHeader (const AckHeader& copy)
: m_data_id (copy.m_data_id), m_position (copy.m_position),
m_velocity (copy.m_velocity) { }


// --------------------------
// Type ID
// --------------------------

TypeId
AckHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal::AckHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
          .AddConstructor<AckHeader> ();
  return type_id;
}

TypeId
AckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
AckHeader::GetSerializedSize () const
{
  return 31u;
}

void
AckHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t position_x_int, position_x_float;
  EncodeDoubleToIntegers (m_position.m_x, position_x_int, position_x_float,
                          sign_flags, COORDINATE_X);

  uint32_t position_y_int, position_y_float;
  EncodeDoubleToIntegers (m_position.m_y, position_y_int, position_y_float,
                          sign_flags, COORDINATE_Y);

  uint32_t velocity_x_int;
  EncodeFloatToInteger (m_velocity.m_x, velocity_x_int, sign_flags, VELOCITY_X);

  uint32_t velocity_y_int;
  EncodeFloatToInteger (m_velocity.m_y, velocity_y_int, sign_flags, VELOCITY_Y);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_data_id.GetSourceId ());

  WriteTo (start, m_data_id.GetSourceIp ());

  start.WriteHtonU32 (position_x_int);
  start.WriteHtonU32 (position_x_float);
  start.WriteHtonU32 (position_y_int);
  start.WriteHtonU32 (position_y_float);

  start.WriteHtonU32 (velocity_x_int);
  start.WriteHtonU32 (velocity_y_int);
}

uint32_t
AckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  const uint8_t sign_flags = it.ReadU8 ();

  // Read DATA packet ID
  const uint16_t data_source_id = it.ReadNtohU16 ();
  Ipv4Address data_source_ip;
  ReadFrom (it, data_source_ip);

  m_data_id = DataIdentifier (data_source_ip, data_source_id);

  uint32_t int_part = it.ReadNtohU32 ();
  uint32_t float_part = it.ReadNtohU32 ();
  m_position.m_x = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_X);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  m_position.m_y = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_Y);

  int_part = it.ReadNtohU32 ();
  m_velocity.m_x = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_X);

  int_part = it.ReadNtohU32 ();
  m_velocity.m_y = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_Y);

  uint32_t distance = it.GetDistanceFrom (start);
  NS_ASSERT (distance == GetSerializedSize ());
  return distance;
}

void
AckHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
AckHeader::ToString () const
{
  return "ACK packet " + m_data_id.ToString () + " from position " + m_position.ToString ()
          + " at velocity " + m_velocity.ToString ();
}


// =============================================================================
//                                  DataHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (DataHeader);

DataHeader::DataHeader ()
: m_data_id (), m_emergency_flag (false), m_hops_count (0u), m_position (),
m_velocity (), m_dest_geo_temporal_area (), m_message () { }

DataHeader::DataHeader (const DataIdentifier& data_id,
                        const uint32_t hops_count,
                        const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                        const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity,
                        const GeoTemporalArea& destination_geo_temporal_area,
                        const std::string message)
: m_data_id (data_id), m_emergency_flag (false), m_hops_count (hops_count),
m_position (position), m_velocity (velocity),
m_dest_geo_temporal_area (destination_geo_temporal_area), m_message (message) { }

DataHeader::DataHeader (const DataIdentifier& data_id,
                        const bool emergency_flag,
                        const uint32_t hops_count,
                        const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                        const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity,
                        const GeoTemporalArea& destination_geo_temporal_area,
                        const std::string message)
: m_data_id (data_id), m_emergency_flag (emergency_flag), m_hops_count (hops_count),
m_position (position), m_velocity (velocity),
m_dest_geo_temporal_area (destination_geo_temporal_area), m_message (message) { }

DataHeader::DataHeader (const DataHeader& copy)
: m_data_id (copy.m_data_id),
m_emergency_flag (copy.m_emergency_flag),
m_hops_count (copy.m_hops_count),
m_position (copy.m_position),
m_velocity (copy.m_velocity),
m_dest_geo_temporal_area (copy.m_dest_geo_temporal_area),
m_message (copy.m_message) { }


// --------------------------
// Type ID
// --------------------------

TypeId
DataHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal::DataHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
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
  return 73u + m_message.size (); // 1 byte is because of the null terminator of the message.
}

void
DataHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t position_x_int, position_x_float;
  EncodeDoubleToIntegers (m_position.m_x, position_x_int, position_x_float,
                          sign_flags, COORDINATE_X);

  uint32_t position_y_int, position_y_float;
  EncodeDoubleToIntegers (m_position.m_y, position_y_int, position_y_float,
                          sign_flags, COORDINATE_Y);

  uint32_t velocity_x_int;
  EncodeFloatToInteger (m_velocity.m_x, velocity_x_int, sign_flags, VELOCITY_X);

  uint32_t velocity_y_int;
  EncodeFloatToInteger (m_velocity.m_y, velocity_y_int, sign_flags, VELOCITY_Y);

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

  uint8_t other_flags = 0u;
  if (m_emergency_flag) SetBitFlag (other_flags, EMERGENCY);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_data_id.GetSourceId ());

  WriteTo (start, m_data_id.GetSourceIp ());

  start.WriteHtonU32 (m_hops_count);

  start.WriteHtonU32 (position_x_int);
  start.WriteHtonU32 (position_x_float);
  start.WriteHtonU32 (position_y_int);
  start.WriteHtonU32 (position_y_float);

  start.WriteHtonU32 (velocity_x_int);
  start.WriteHtonU32 (velocity_y_int);

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

  start.WriteU8 (other_flags);

  const uint32_t message_length = m_message.length () + 1u;
  char chars_buffer [message_length];
  std::strcpy (chars_buffer, m_message.c_str ());
  start.Write ((uint8_t *) chars_buffer, message_length);
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

  m_hops_count = it.ReadNtohU32 ();

  // Read position
  uint32_t int_part = it.ReadNtohU32 ();
  uint32_t float_part = it.ReadNtohU32 ();
  m_position.m_x = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_X);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  m_position.m_y = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_Y);

  // Read velocity
  int_part = it.ReadNtohU32 ();
  m_velocity.m_x = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_X);

  int_part = it.ReadNtohU32 ();
  m_velocity.m_y = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_Y);

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

  // Read emergency flag
  const uint8_t other_flags = it.ReadU8 ();
  m_emergency_flag = CheckBitFlag (other_flags, EMERGENCY);

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
  std::sprintf (buffer, "%u", m_hops_count);

  std::string str = "";

  if (m_emergency_flag) str += "EMERGENCY ";

  str += "DATA " + m_data_id.ToString () + " (" + std::string (buffer)
          + " hops) sent from position " + m_position.ToString () + " at velocity "
          + m_velocity.ToString () + " destined to area "
          + m_dest_geo_temporal_area.GetArea ().ToString ();

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetTimePeriod ().GetStartTime ().GetSeconds ());
  str += " to start at second " + std::string (buffer);

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetDuration ().GetSeconds ());
  str += " with a duration of " + std::string (buffer) + " seconds";

  std::sprintf (buffer, "%u", (uint32_t) m_message.size ());
  str += " has a message of " + std::string (buffer) + " byte(s)";
  return str;
}


// =============================================================================
//                                 DataAckHeader
// =============================================================================

NS_OBJECT_ENSURE_REGISTERED (DataAckHeader);

DataAckHeader::DataAckHeader ()
: DataHeader (), m_data_id_to_ack (), m_reserved (0u) { }

DataAckHeader::DataAckHeader (const DataIdentifier& data_id_to_ack,
                              const DataIdentifier& data_id,
                              const uint32_t hops_count,
                              const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                              const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity,
                              const GeoTemporalArea& destination_geo_temporal_area,
                              const std::string message)
: DataHeader (data_id, hops_count, position, velocity,
              destination_geo_temporal_area, message),
m_data_id_to_ack (data_id_to_ack), m_reserved (0u) { }

DataAckHeader::DataAckHeader (const DataIdentifier& data_id_to_ack,
                              const DataIdentifier& data_id,
                              const bool emergency_flag,
                              const uint32_t hops_count,
                              const GeoTemporalLibrary::LibraryUtils::Vector2D& position,
                              const GeoTemporalLibrary::LibraryUtils::Vector2D& velocity,
                              const GeoTemporalArea& destination_geo_temporal_area,
                              const std::string message)
: DataHeader (data_id, emergency_flag, hops_count, position, velocity,
              destination_geo_temporal_area, message),
m_data_id_to_ack (data_id_to_ack), m_reserved (0u) { }

DataAckHeader::DataAckHeader (const DataHeader& data_header,
                              const DataIdentifier& data_id_to_ack)
: DataHeader (data_header), m_data_id_to_ack (data_id_to_ack), m_reserved (0u) { }

DataAckHeader::DataAckHeader (const DataAckHeader& copy)
: DataHeader (copy), m_data_id_to_ack (copy.m_data_id_to_ack),
m_reserved (copy.m_reserved) { }


// --------------------------
// Type ID
// --------------------------

TypeId
DataAckHeader::GetTypeId ()
{
  static TypeId type_id = TypeId ("ns3::geotemporal::DataAckHeader")
          .SetParent<Header> ()
          .SetGroupName ("Geotemporal")
          .AddConstructor<DataAckHeader> ();
  return type_id;
}

TypeId
DataAckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


// --------------------------
// Header serialization/deserialization
// --------------------------

uint32_t
DataAckHeader::GetSerializedSize () const
{
  return 80u + m_message.size (); // 1 byte is because of the null terminator of the message.
}

void
DataAckHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t sign_flags = 0u;

  uint32_t position_x_int, position_x_float;
  EncodeDoubleToIntegers (m_position.m_x, position_x_int, position_x_float,
                          sign_flags, COORDINATE_X);

  uint32_t position_y_int, position_y_float;
  EncodeDoubleToIntegers (m_position.m_y, position_y_int, position_y_float,
                          sign_flags, COORDINATE_Y);

  uint32_t velocity_x_int;
  EncodeFloatToInteger (m_velocity.m_x, velocity_x_int, sign_flags, VELOCITY_X);

  uint32_t velocity_y_int;
  EncodeFloatToInteger (m_velocity.m_y, velocity_y_int, sign_flags, VELOCITY_Y);

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

  uint8_t other_flags = 0u;
  if (m_emergency_flag) SetBitFlag (other_flags, EMERGENCY);

  start.WriteU8 (sign_flags);
  start.WriteHtonU16 (m_data_id_to_ack.GetSourceId ());

  WriteTo (start, m_data_id_to_ack.GetSourceIp ());

  start.WriteU8 (other_flags);
  start.WriteU8 (m_reserved);
  start.WriteHtonU16 (m_data_id.GetSourceId ());

  WriteTo (start, m_data_id.GetSourceIp ());

  start.WriteHtonU32 (m_hops_count);

  start.WriteHtonU32 (position_x_int);
  start.WriteHtonU32 (position_x_float);
  start.WriteHtonU32 (position_y_int);
  start.WriteHtonU32 (position_y_float);

  start.WriteHtonU32 (velocity_x_int);
  start.WriteHtonU32 (velocity_y_int);

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

  const uint32_t message_length = m_message.length () + 1u;
  char chars_buffer [message_length];
  std::strcpy (chars_buffer, m_message.c_str ());
  start.Write ((uint8_t *) chars_buffer, message_length);
}

uint32_t
DataAckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator it = start;

  const uint8_t sign_flags = it.ReadU8 ();

  // Read acknowledged DATA packet ID
  uint16_t data_source_id = it.ReadNtohU16 ();
  Ipv4Address data_source_ip;
  ReadFrom (it, data_source_ip);

  m_data_id_to_ack = DataIdentifier (data_source_ip, data_source_id);

  const uint8_t other_flags = it.ReadU8 ();
  m_emergency_flag = CheckBitFlag (other_flags, EMERGENCY);
  m_reserved = it.ReadU8 ();

  // Read DATA packet ID
  data_source_id = it.ReadNtohU16 ();
  ReadFrom (it, data_source_ip);

  m_data_id = DataIdentifier (data_source_ip, data_source_id);

  m_hops_count = it.ReadNtohU32 ();

  // Read position
  uint32_t int_part = it.ReadNtohU32 ();
  uint32_t float_part = it.ReadNtohU32 ();
  m_position.m_x = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_X);

  int_part = it.ReadNtohU32 ();
  float_part = it.ReadNtohU32 ();
  m_position.m_y = DecodeDoubleFromIntegers (int_part, float_part,
                                             sign_flags, COORDINATE_Y);

  // Read velocity
  int_part = it.ReadNtohU32 ();
  m_velocity.m_x = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_X);

  int_part = it.ReadNtohU32 ();
  m_velocity.m_y = DecodeFloatFromInteger (int_part, sign_flags, VELOCITY_Y);

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
DataAckHeader::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
DataAckHeader::ToString () const
{
  char buffer[10];
  std::sprintf (buffer, "%u", m_hops_count);

  std::string str = "ACK " + m_data_id_to_ack.ToString () + " / ";

  if (m_emergency_flag) str += "EMERGENCY ";

  str += "DATA " + m_data_id.ToString () + " (" + std::string (buffer)
          + " hops) sent from position " + m_position.ToString () + " at velocity "
          + m_velocity.ToString () + " destined to area "
          + m_dest_geo_temporal_area.GetArea ().ToString ();

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetTimePeriod ().GetStartTime ().GetSeconds ());
  str += " to start at second " + std::string (buffer);

  std::sprintf (buffer, "%04.2f", m_dest_geo_temporal_area.GetDuration ().GetSeconds ());
  str += " with a duration of " + std::string (buffer) + " seconds";

  std::sprintf (buffer, "%u", (uint32_t) m_message.size ());
  str += " has a message of " + std::string (buffer) + " byte(s)";
  return str;
}


} // namespace geotemporal
} // namespace ns3

