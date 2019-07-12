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
 * Geo-Temporal Spray And Wait protocol packets.
 */

#ifndef GEOTEMPORAL_SPRAY_AND_WAIT_PACKETS_H
#define GEOTEMPORAL_SPRAY_AND_WAIT_PACKETS_H

#include <algorithm>
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
namespace geotemporal_spray_and_wait
{

using GeoTemporalLibrary::LibraryUtils::Vector2D;

// =============================================================================
//                                  PacketType
// =============================================================================

/**
 * \ingroup geotemporal-spray-and-wait
 * \brief Geo-Temporal Spray And Wait protocol packet types.
 * 
 * Contains all the possible types of packets in the Geo-Temporal Spray And Wait
 * protocol:
 * 
 * 1. HELLO packet: It is used to advertise the presence of a node in the network.
 * 
 * 2. REPLY packet: Once a HELLO is received, the receiver node responds with
 *    a REPLY packet that contains the list of packets that it has in memory, 
 *    this list is called the summary vector.
 * 
 * 3. REPLY BACK (or REQUEST) packet: Once a REPLY packet is received, the 
 *    receiver node determines the disjoint packets between its buffer and the 
 *    received summary vector and it sends the list of unknown packets  IDs 
 *    as a request to the other node.
 *    Then, the other node will check the packets in the request one bye one. If
 *    a packet has more than one replica (copy) or the requesting node is inside
 *    of the destination area of a packet, it sends back that packet. Discounting
 *    the number of replicas where it applies.
 * 
 * 4. DATA packet: Contains the actual data that must be delivered to its destination.
 */
enum class PacketType : uint8_t
{
  Hello = 0, //< Advertises the presence of a node.
  Reply = 5, //< Reply to a HELLO, with the summary vector of the known packets.
  ReplyBack = 7, //< Response to a REPLY packet. Also called REQUEST.
  Data = 9 //< Actual data packet.
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
 * \ingroup geotemporal-spray-and-wait
 * 
 * Header that contains the type of the Geo-Temporal Spray And Wait protocol 
 * packets.
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
 * \ingroup geotemporal-spray-and-wait
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
//                                  ReplyHeader
// =============================================================================

/**
 * \ingroup geotemporal-spray-and-wait
 * 
 * REPLY packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |    Reserved   |  16 bit Summary Vector Length |
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
class ReplyHeader : public Header
{
protected:

  /** The set of identifiers of known DATA packets. */
  std::set<DataIdentifier> m_summary_vector;

  uint8_t m_reserved; // Not used

public:

  ReplyHeader ();

  ReplyHeader (const std::set<DataIdentifier> & summary_vector);

  ReplyHeader (const ReplyHeader & copy);


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

  friend bool operator== (const ReplyHeader & lhs, const ReplyHeader & rhs);
};

// ReplyHeader relational operators

inline bool
operator== (const ReplyHeader & lhs, const ReplyHeader & rhs)
{
  return lhs.m_summary_vector == rhs.m_summary_vector
          && lhs.m_reserved == rhs.m_reserved;
}

inline bool
operator!= (const ReplyHeader & lhs, const ReplyHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// ReplyHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const ReplyHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                ReplyBackHeader
// =============================================================================

/**
 * \ingroup geotemporal-spray-and-wait
 * 
 * REPLY_BACK packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |     Flags     |  16 bit Summary Vector Length |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                      REPLY_BACK sender's                      |
  +                     x-coordinate position                     +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                      REPLY_BACK sender's                      |
  +                     y-coordinate position                     +
  |                           (64 bits)                           |
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
class ReplyBackHeader : public ReplyHeader
{
private:

  /** Geographic position of the packet sender node. */
  Vector2D m_position;

public:

  ReplyBackHeader ();

  ReplyBackHeader (const std::set<DataIdentifier> & summary_vector,
                   const Vector2D & position);

  ReplyBackHeader (const ReplyBackHeader & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const Vector2D &
  GetPosition () const
  {
    return m_position;
  }

  inline void
  SetPosition (const Vector2D & position)
  {
    m_position = position;
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

  friend bool operator== (const ReplyBackHeader & lhs, const ReplyBackHeader & rhs);
};

// ReplyBackHeader relational operators

inline bool
operator== (const ReplyBackHeader & lhs, const ReplyBackHeader & rhs)
{
  std::set<DataIdentifier> difference;
  std::set_symmetric_difference (lhs.m_summary_vector.begin (), lhs.m_summary_vector.end (),
                                 rhs.m_summary_vector.begin (), rhs.m_summary_vector.end (),
                                 std::inserter (difference, difference.begin ()));

  return lhs.m_position == rhs.m_position && difference.size () == 0;
}

inline bool
operator!= (const ReplyBackHeader & lhs, const ReplyBackHeader & rhs)
{
  return !operator== (lhs, rhs);
}

// ReplyBackHeader stream operators

inline std::ostream &
operator<< (std::ostream & os, const ReplyBackHeader & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                  DataHeader
// =============================================================================

/**
 * \ingroup geotemporal-spray-and-wait
 * 
 * DATA packet header
 * 
  \verbatim
  0                   1                   2                   3
  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |   TypeHeader  |     Flags     |      DATA ID SEQ number       |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                   DATA ID Source IP Address                   |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                 Number of replicas to forward                 |
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
  |                  Message (maximum 1024 bytes)                 |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  \endverbatim
 */
class DataHeader : public Header
{
private:

  /** The identifier of the DATA packet. */
  DataIdentifier m_data_id;

  /** Destination geo-temporal area. */
  GeoTemporalArea m_dest_geo_temporal_area;

  /** Message to transmit (in bytes). Recommended maximum 1024 bytes. */
  std::string m_message;

  /**
   * Indicates the number of packet replicas that the protocol can forward to
   * other nodes.
   * 
   * If it is 1 indicates that no replicas must be forwarded, only direct 
   * transmission. If it is > 1 indicates the number of replicas that can be
   * forwarded by the node.
   * 
   * Roughly, if it is 1 then the running protocol is Geo-Temporal Spray And 
   * Wait. If it is > 1 then it's Geo-Temporal Binary Spray And Wait.
   */
  uint32_t m_replicas_to_forward;

public:

  DataHeader ();

  DataHeader (const DataIdentifier & data_id,
              const GeoTemporalArea & destination_geo_temporal_area,
              const std::string message, const uint32_t & replicas_to_forward);

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

  inline uint32_t
  GetReplicasToForward () const
  {
    return m_replicas_to_forward;
  }

  inline void
  SetReplicasToForward (uint32_t replicas_to_forward)
  {
    m_replicas_to_forward = replicas_to_forward;
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
          && lhs.m_dest_geo_temporal_area == rhs.m_dest_geo_temporal_area
          && lhs.m_message == rhs.m_message
          && lhs.m_replicas_to_forward == rhs.m_replicas_to_forward;
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


} // namespace geotemporal_spray_and_wait
} // namespace ns3

#endif /* GEOTEMPORAL_SPRAY_AND_WAIT_PACKETS_H */

