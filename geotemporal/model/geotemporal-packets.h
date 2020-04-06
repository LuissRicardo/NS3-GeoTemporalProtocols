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

#ifndef GEOTEMPORAL_PACKETS_H
#define GEOTEMPORAL_PACKETS_H

#include <set>
#include <string>

#include <ns3/geotemporal-utils.h>
#include <ns3/math-utils.h>
#include <ns3/packet-utils.h>

#include <ns3/header.h>
#include <ns3/ipv4-address.h>
#include <ns3/type-id.h>

using namespace GeoTemporalLibrary::LibraryUtils;


namespace ns3
{
namespace geotemporal
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;

// =============================================================================
//                                  PacketType
// =============================================================================

/**
 * \ingroup geotemporal
 * \brief Geo-Temporal protocol packet types.
 * 
 * Contains all the possible types of packets in the Geo-Temporal protocol:
 * 
 * 1. HELLO packet: It is used to advertise the presence of a node in the network.
 * 
 * 2. REPLY packet: Once a HELLO is received, it starts the anti-entropy session.
 *    The node with smaller network ID (i.e. IP address) will send a REPLY packet
 *    that contains the summary vector of the data packets it has in its packets
 *    queue. So "it offers" the data packets contained in the summary vector.
 * 
 * 3. REPLY_BACK packet: Once a REPLY packet is received from node A, the receiver
 *    node B uses the summary vector SV_A included in the REPLY packet and the 
 *    summary vector SV_B of the content of the packets queue of node B to compute 
 *    the difference set operation of these two sets:
 *    
 *                     |SV_A - SV_B|  and  |SV_B - SV_A|
 * 
 *    The next step to execute is decided depending on the result of both previous
 *    operations:
 * 
 *    - If |SV_A - SV_B| > 0: then compute the disjoint vector and send it in a
 *      REPLY_BACK packet to node A.
 * 
 *    - If |SV_A - SV_B| == 0 && |SV_B - SV_A| > 0: then proceed to send DATA
 *      packets.
 * 
 *    - If |SV_A - SV_B| == 0 && |SV_B - SV_A| == 0: then does nothing. The 
 *      conversation between nodes A and B is finished.
 * 
 * 4. ACK packet: acknowledges a DATA packet as successfully received.
 * 
 * 5. DATA packet: a packet that contains actual data from the user.
 * 
 * 6. DATA_ACK packet: the combination of a DATA packet and an ACK packet. It
 *    carries data while acknowledging that another DATA packet as successfully
 *    received.
 */
enum class PacketType : uint8_t
{
  Hello = 0, //< Advertises the presence of a node.
  Reply = 1, //< Reply to a HELLO, contains the summary vector of the known packets. Also called OFFER.
  ReplyBack = 2, //< Response to a REPLY packet. Also called REQUEST.
  Ack = 3, //< Acknowledges that a DATA packet was successfully received.
  Data = 4, //< Actual data packet.
  DataAck = 5 //< Contains DATA and indicates that a DATA packet was successfully received.
};

/**
 * Returns a <code>string</code> object containing the representation of the
 * specified <code>PacketType</code> value.
 */
std::string
ToString (const PacketType & obj);

inline std::ostream &
operator<< (std::ostream & os, const PacketType & obj)
{
  os << ToString (obj);
  return os;
}


// =============================================================================
//                                   TypeHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Header that contains the type of the Geo-Temporal protocol packets.
 */
class TypeHeader : public Header
{
private:

  PacketType m_packet_type;

  bool m_valid;


public:

  TypeHeader ();

  TypeHeader (PacketType packet_type);

  TypeHeader (const TypeHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline PacketType
  GetPacketType () const
  {
    return m_packet_type;
  }

  inline void
  SetPacketType (const PacketType & type)
  {
    m_packet_type = type;
  }

  inline bool
  IsValid () const
  {
    return m_valid;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const TypeHeader & lhs, const TypeHeader & rhs);
};

// TypeHeader relational operators

inline bool
operator== (const TypeHeader & lhs, const TypeHeader & rhs)
{
  return lhs.m_packet_type == rhs.m_packet_type && lhs.m_valid == rhs.m_valid;
}

inline bool
operator!= (const TypeHeader & lhs, const TypeHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// TypeHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const TypeHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                  HelloHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * HELLO packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |    Reserved   | Number of cached DATA packets |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               HELLO sender's Local IPv4 address               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
 */
class HelloHeader : public Header
{
private:

  uint8_t m_reserved; // Not used

  /** Size of the summary vector. */
  uint16_t m_summary_vector_size;

  /** IPv4 address of the node that sent the HELLO packet. */
  Ipv4Address m_sender_node_ip;


public:

  HelloHeader ();

  HelloHeader (const Ipv4Address & node_ip, uint16_t summary_vector_size);

  HelloHeader (const HelloHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline uint16_t
  GetSummaryVectorSize () const
  {
    return m_summary_vector_size;
  }

  inline void
  SetSummaryVectorSize (uint16_t summary_vector_size)
  {
    m_summary_vector_size = summary_vector_size;
  }

  inline const Ipv4Address &
  GetSenderNodeIp () const
  {
    return m_sender_node_ip;
  }

  inline void
  SetSenderNodeIp (const Ipv4Address & sender_node_ip)
  {
    m_sender_node_ip = sender_node_ip;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const HelloHeader & lhs, const HelloHeader & rhs);
};

// HelloHeader relational operators

inline bool
operator== (const HelloHeader & lhs, const HelloHeader & rhs)
{
  return lhs.m_sender_node_ip == rhs.m_sender_node_ip
          && lhs.m_summary_vector_size == rhs.m_summary_vector_size
          && lhs.m_reserved == rhs.m_reserved;
}

inline bool
operator!= (const HelloHeader & lhs, const HelloHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// HelloHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const HelloHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                              SummaryVectorHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * SUMMARY VECTOR packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |     Flags     |  16 bit Summary Vector Length |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    SUMMARY VECTOR sender's                    |
  +                     x-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    SUMMARY VECTOR sender's                    |
  +                     y-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     SUMMARY VECTOR sender's x-coordinate velocity vector      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     SUMMARY VECTOR sender's y-coordinate velocity vector      |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               DATA ID Source IP Address # 1 [1]               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   DATA ID SEQ number # 1 [1]  |  DATA ID SEQ number # 2 [2]   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               DATA ID Source IP Address # 2 [2]               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                              ...                              |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |              ...              |  DATA ID SEQ number # n [2]   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |               DATA ID Source IP Address # n [2]               |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

  [1] = If the DATA ID index is odd.
  [2] = If the DATA ID index is even.
  \endverbatim
 */
class SummaryVectorHeader : public Header
{
private:

  /** The set of identifiers of known DATA packets. */
  std::set<DataIdentifier> m_summary_vector;

  /** Geographic position of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_position;

  /** Velocity vector of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_velocity;


public:

  SummaryVectorHeader ();

  SummaryVectorHeader (const std::set<DataIdentifier> & summary_vector,
                       const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
                       const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity);

  SummaryVectorHeader (const SummaryVectorHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline uint32_t
  GetSummaryVectorSize () const
  {
    return m_summary_vector.size ();
  }

  inline const std::set<DataIdentifier> &
  GetSummaryVector () const
  {
    return m_summary_vector;
  }

  inline void
  SetSummaryVector (const std::set<DataIdentifier> & summary_vector)
  {
    m_summary_vector = summary_vector;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetPosition () const
  {
    return m_position;
  }

  inline void
  SetPosition (const GeoTemporalLibrary::LibraryUtils::Vector2D & position)
  {
    m_position = position;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetVelocity () const
  {
    return m_velocity;
  }

  inline void
  SetVelocity (const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity)
  {
    m_velocity = velocity;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const SummaryVectorHeader & lhs, const SummaryVectorHeader & rhs);
};

// SummaryVectorHeader relational operators

inline bool
operator== (const SummaryVectorHeader & lhs, const SummaryVectorHeader & rhs)
{
  return lhs.m_summary_vector == rhs.m_summary_vector
          && lhs.m_position == rhs.m_position
          && lhs.m_velocity == rhs.m_velocity;
}

inline bool
operator!= (const SummaryVectorHeader & lhs, const SummaryVectorHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// SummaryVectorHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const SummaryVectorHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                   AckHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * ACK packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |     Flags     |Source SEQ # of the pkt to ACK |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    Source IP address of the received packet to acknowledge    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         ACK sender's                          |
  +                     x-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         ACK sender's                          |
  +                     y-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           ACK sender's x-coordinate velocity vector           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           ACK sender's y-coordinate velocity vector           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
 */
class AckHeader : public Header
{
private:

  /** Data identifier of the DATA packet to acknowledge. */
  DataIdentifier m_data_id;

  /** Geographic position of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_position;

  /** Velocity vector of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_velocity;


public:

  AckHeader ();

  AckHeader (const DataIdentifier & data_identifier,
             const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
             const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity);

  AckHeader (const AckHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const DataIdentifier &
  GetDataIdentifier () const
  {
    return m_data_id;
  }

  void
  SetDataIdentifier (const DataIdentifier & data_id)
  {
    m_data_id = data_id;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetPosition () const
  {
    return m_position;
  }

  inline void
  SetPosition (const GeoTemporalLibrary::LibraryUtils::Vector2D & position)
  {
    m_position = position;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetVelocity () const
  {
    return m_velocity;
  }

  inline void
  SetVelocity (const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity)
  {
    m_velocity = velocity;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const AckHeader & lhs, const AckHeader & rhs);
};

// AckHeader relational operators

inline bool
operator== (const AckHeader & lhs, const AckHeader & rhs)
{
  return lhs.m_data_id == rhs.m_data_id
          && lhs.m_position == rhs.m_position
          && lhs.m_velocity == rhs.m_velocity;
}

inline bool
operator!= (const AckHeader & lhs, const AckHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// AckHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const AckHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                  DataHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * DATA packet header
 * 
  \verbatim
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |   Sign Flags  |      DATA ID SEQ number       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                   DATA ID Source IP Address                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                      Current hops count                       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         DATA sender's                         |
  +                     x-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         DATA sender's                         |
  +                     y-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           DATA sender's x-coordinate velocity vector          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           DATA sender's y-coordinate velocity vector          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         x1 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         y1 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         x2 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         y2 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Initial time (seconds)    |      Duration (seconds)       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |E|   Unused    |   Message...  |   Terminator  |  E = Emergency flag
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   0                   1                   2                   3
  \endverbatim
 */
class DataHeader : public Header
{
protected:

  /** The identifier of the DATA packet. */
  DataIdentifier m_data_id;

  /**
   * If enabled, it indicates that the packet has high priority, is an emergency
   * packet. Otherwise, when disabled, it indicates that the packet doesn't have
   * high priority.
   */
  bool m_emergency_flag;

  /**
   * Indicates how far the packet has traveled away from the source node, the 
   * number of hops that it has been forwarded.
   * 
   * If its value is equal to 0 then the packet is at the source node, and if 
   * the value is greater than 0 then it has been forwarded that many times.
   */
  uint32_t m_hops_count;

  /** Geographic position of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_position;

  /** Velocity vector of the packet sender node. */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_velocity;

  /** Destination geo-temporal area. */
  GeoTemporalArea m_dest_geo_temporal_area;

  /** Message to transmit (in bytes). Recommended maximum 1024 bytes. */
  std::string m_message;


public:

  DataHeader ();

  DataHeader (const DataIdentifier & data_id,
              const uint32_t hops_count,
              const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
              const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity,
              const GeoTemporalArea & destination_geo_temporal_area,
              const std::string message);

  DataHeader (const DataIdentifier & data_id,
              const bool emergency_flag,
              const uint32_t hops_count,
              const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
              const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity,
              const GeoTemporalArea & destination_geo_temporal_area,
              const std::string message);

  DataHeader (const DataHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const DataIdentifier &
  GetDataIdentifier () const
  {
    return m_data_id;
  }

  inline void
  SetDataIdentifier (const DataIdentifier & data_id)
  {
    m_data_id = data_id;
  }

  inline const bool
  IsEmergencyPacket () const
  {
    return m_emergency_flag;
  }

  inline void
  SetEmergencyPacket (const bool emergency_flag)
  {
    m_emergency_flag = emergency_flag;
  }

  uint32_t
  GetHopsCount () const
  {
    return m_hops_count;
  }

  void
  SetHopsCount (uint32_t hops_count)
  {
    m_hops_count = hops_count;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetPosition () const
  {
    return m_position;
  }

  inline void
  SetPosition (const GeoTemporalLibrary::LibraryUtils::Vector2D & position)
  {
    m_position = position;
  }

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetVelocity () const
  {
    return m_velocity;
  }

  inline void
  SetVelocity (const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity)
  {
    m_velocity = velocity;
  }

  inline const GeoTemporalArea &
  GetDestinationGeoTemporalArea () const
  {
    return m_dest_geo_temporal_area;
  }

  inline void
  SetDestinationGeoTemporalArea (const GeoTemporalArea & dest_geo_temporal_area)
  {
    m_dest_geo_temporal_area = dest_geo_temporal_area;
  }

  inline const std::string &
  GetMessage () const
  {
    return m_message;
  }

  inline void
  SetMessage (const std::string & message)
  {
    m_message = message;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const DataHeader & lhs, const DataHeader & rhs);
};

// DataHeader relational operators

inline bool
operator== (const DataHeader & lhs, const DataHeader & rhs)
{
  return lhs.m_data_id == rhs.m_data_id
          && lhs.m_emergency_flag == rhs.m_emergency_flag
          && lhs.m_hops_count == rhs.m_hops_count
          && lhs.m_position == rhs.m_position
          && lhs.m_velocity == rhs.m_velocity
          && lhs.m_dest_geo_temporal_area == rhs.m_dest_geo_temporal_area
          && lhs.m_message == rhs.m_message;
}

inline bool
operator!= (const DataHeader & lhs, const DataHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// DataHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const DataHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                 DataAckHeader
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * DATA packet header
 * 
  \verbatim
   0                   1                   2                   3
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |   Sign Flags  |Source SEQ # of the pkt to ACK |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |    Source IP address of the received packet to acknowledge    |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |E|           Unused            |      DATA ID SEQ number       |  E = Emergency flag
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                   DATA ID Source IP Address                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                      Current hops count                       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         DATA sender's                         |
  +                     x-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                         DATA sender's                         |
  +                     y-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           DATA sender's x-coordinate velocity vector          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |           DATA sender's y-coordinate velocity vector          |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         x1 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         y1 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         x2 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                       Destination area's                      |
  +                         y2 coordinate                         +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |     Initial time (seconds)    |      Duration (seconds)       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   Message...  |   Terminator  |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
   0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
   0                   1                   2                   3
  \endverbatim
 */
class DataAckHeader : public DataHeader
{
private:

  /** Data identifier of the DATA packet to acknowledge. */
  DataIdentifier m_data_id_to_ack;

  uint8_t m_reserved; // Not used


public:

  DataAckHeader ();

  DataAckHeader (const DataIdentifier & data_id_to_ack,
                 const DataIdentifier & data_id,
                 const uint32_t hops_count,
                 const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
                 const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity,
                 const GeoTemporalArea & destination_geo_temporal_area,
                 const std::string message);

  DataAckHeader (const DataIdentifier & data_id_to_ack,
                 const DataIdentifier & data_id,
                 const bool emergency_flag,
                 const uint32_t hops_count,
                 const GeoTemporalLibrary::LibraryUtils::Vector2D & position,
                 const GeoTemporalLibrary::LibraryUtils::Vector2D & velocity,
                 const GeoTemporalArea & destination_geo_temporal_area,
                 const std::string message);

  DataAckHeader (const DataHeader & data_header,
                 const DataIdentifier & data_id_to_ack);

  DataAckHeader (const DataAckHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const DataIdentifier &
  GetDataIdentifierToAck () const
  {
    return m_data_id_to_ack;
  }

  inline void
  SetDataIdentifierToAck (const DataIdentifier & data_id_to_ack)
  {
    m_data_id_to_ack = data_id_to_ack;
  }


  // --------------------------
  // Type ID
  // --------------------------

  static TypeId
  GetTypeId ();

  TypeId
  GetInstanceTypeId () const;


  // --------------------------
  // Header serialization/deserialization
  // --------------------------

  uint32_t
  GetSerializedSize () const override;

  void
  Serialize (Buffer::Iterator start) const override;

  uint32_t
  Deserialize (Buffer::Iterator start) override;

  void
  Print (std::ostream &os) const override;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const DataAckHeader & lhs, const DataAckHeader & rhs);
};

// DataAckHeader relational operators

inline bool
operator== (const DataAckHeader & lhs, const DataAckHeader & rhs)
{
  return ((DataHeader) lhs) == ((DataHeader) rhs)
          && lhs.m_data_id_to_ack == rhs.m_data_id_to_ack
          && lhs.m_reserved == rhs.m_reserved;
}

inline bool
operator!= (const DataAckHeader & lhs, const DataAckHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// DataAckHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const DataAckHeader & obj)
{
  obj.Print (os);
  return os;
}


} // namespace geotemporal
} // namespace ns3

#endif /* GEOTEMPORAL_PACKETS_H */

