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

#ifndef GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_H
#define GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_H

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
namespace geotemporal_restricted_epidemic
{

// =============================================================================
//                                  PacketType
// =============================================================================

/**
 * \ingroup geotemporal-restricted-epidemic
 * \brief Geo-Temporal Restricted Epidemic protocol packet types.
 * 
 * Contains all the possible types of packets in the Geo-Temporal Restricted 
 * Epidemic protocol:
 * 
 * 1. HELLO packet: It is used to advertise the presence of a node in the network.
 * 
 * 2. REPLY packet: Once a HELLO is received, it starts the anti-entropy session.
 *    The node with smaller network ID (i.e. IP address) will send a REPLY packet
 *    that contains the summary vector of the data packets it has in its packets
 *    queue.
 * 
 * 3. REPLY BACK (or REQUEST) packet: Once a REPLY packet is received, the 
 *    receiver node determines the disjoint packets between its buffer and the 
 *    received summary vector.
 *    Then, it sends the disjoint packets to the other node. After that it sends
 *    a REPLY BACK packet containing the summary vector of all the packet IDs 
 *    it has in its packets queue so the other node can send the disjoint 
 *    packets as well.
 * 
 * 4. DATA packet: Contains the actual data that must be delivered to its destination.
 */
enum class PacketType : uint8_t
{
  Hello = 0, //< Advertises the presence of a node.
  Reply = 5, //< Reply to a HELLO, with the summary vector of the known packets.
  ReplyBack = 7, //< Response to a REPLY packet.
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
 * \ingroup geotemporal-restricted-epidemic
 * 
 * Header that contains the type of the Geo-Temporal Restricted Epidemic 
 * protocol packets.
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
 * \ingroup geotemporal-restricted-epidemic
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
 * \ingroup geotemporal-restricted-epidemic
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


public:

  SummaryVectorHeader ();

  SummaryVectorHeader (const std::set<DataIdentifier> & summary_vector,
                       const GeoTemporalLibrary::LibraryUtils::Vector2D & position);

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
          && lhs.m_position == rhs.m_position;
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
//                                  DataHeader
// =============================================================================

/**
 * \ingroup geotemporal-restricted-epidemic
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
  |                          Hops Count                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Source node's creation                     |
  +                         X coordinate                          +
  |                           (64 bits)                           |
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
  |                    Source node's creation                     |
  +                         Y coordinate                          +
  |                           (64 bits)                           |
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

  /**
   * The geographic coordinates of the source node at the moment of creation of 
   * the packet.
   */
  GeoTemporalLibrary::LibraryUtils::Vector2D m_creation_coordinates;

  /** Destination geo-temporal area. */
  GeoTemporalArea m_dest_geo_temporal_area;

  /** Message to transmit (in bytes). Recommended maximum 1024 bytes. */
  std::string m_message;

  /**
   * Indicates the maximum number of times the packet is flooded before being
   * discarded.
   * 
   * It is similar to the IP TTL field but with with a higher size limit.
   * 
   * This field is decremented by one after each forwarding step.
   */
  uint32_t m_hops_counter;

public:

  DataHeader ();

  DataHeader (const DataIdentifier & data_id,
              const GeoTemporalLibrary::LibraryUtils::Vector2D & creation_coordinates,
              const GeoTemporalArea & destination_geo_temporal_area,
              const std::string message, const uint32_t & hops_count);

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

  inline const GeoTemporalLibrary::LibraryUtils::Vector2D &
  GetCreationCoordinates () const
  {
    return m_creation_coordinates;
  }

  inline void
  SetCreationCoordinates (const GeoTemporalLibrary::LibraryUtils::Vector2D & creation_coordinates)
  {
    m_creation_coordinates = creation_coordinates;
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
  GetHopsCount () const
  {
    return m_hops_counter;
  }

  inline void
  SetHopsCount (uint32_t hops_count)
  {
    m_hops_counter = hops_count;
  }


  // --------------------------
  // Processing functions
  // --------------------------

  /** Returns the geographic area where it is allowed to forward the packet. */
  Area
  GetAllowedArea () const;


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
          && lhs.m_creation_coordinates == rhs.m_creation_coordinates
          && lhs.m_dest_geo_temporal_area == rhs.m_dest_geo_temporal_area
          && lhs.m_message == rhs.m_message
          && lhs.m_hops_counter == rhs.m_hops_counter;
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


} // namespace geotemporal_restricted_epidemic
} // namespace ns3

#endif /* GEOTEMPORAL_RESTRICTED_EPIDEMIC_PACKETS_H */

