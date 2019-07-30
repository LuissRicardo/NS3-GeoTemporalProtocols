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
 * Detector of received duplicated packets.
 */

#ifndef GEOTEMPORAL_DUPLICATE_DETECTOR_H
#define GEOTEMPORAL_DUPLICATE_DETECTOR_H

#include <vector>

#include <ns3/ipv4-address.h>
#include <ns3/ipv4-header.h>
#include <ns3/nstime.h>
#include <ns3/packet.h>
#include <ns3/ptr.h>
#include <ns3/simulator.h>


namespace ns3
{
namespace geotemporal
{

// =============================================================================
//                                CachedPacketId
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Identifies a packet and contains its expiration time.
 */
class CachedPacketId
{
private:

  /** IPv4 address of the packet's source node. */
  Ipv4Address m_source_ip;

  /** Sequential number of the packet. */
  uint32_t m_packet_id;

  /** Expiration time of the packet in the cache. */
  Time m_expiration_time;


public:

  CachedPacketId ();

  CachedPacketId (const Ipv4Address & source_ip, uint32_t packet_id,
                  const Time & expiration_time);

  CachedPacketId (const CachedPacketId & copy);

  inline const Ipv4Address &
  GetSourceIp () const
  {
    return m_source_ip;
  }

  inline uint32_t
  GetPacketId () const
  {
    return m_packet_id;
  }

  inline const Time &
  GetExpirationTime () const
  {
    return m_expiration_time;
  }

  inline bool
  operator== (const CachedPacketId & rhs) const
  {
    return m_source_ip == rhs.m_source_ip && m_packet_id == rhs.m_packet_id
            && m_expiration_time == rhs.m_expiration_time;
  }

  inline bool
  operator< (const CachedPacketId & rhs) const
  {
    if (m_source_ip != rhs.m_source_ip)
      return m_source_ip < rhs.m_source_ip;

    if (m_packet_id != rhs.m_packet_id)
      return m_packet_id < rhs.m_packet_id;

    return m_expiration_time < rhs.m_expiration_time;
  }

};


// =============================================================================
//                           DuplicatedPacketDetector
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Helper class used to remember already seen packets and detect duplicates.
 * 
 * Currently, duplicate detection is based on unique packet ID given by
 * Packet::GetUid (). This approach is known to be weak (ns3::Packet UID is an 
 * internal identifier and not intended for logical uniqueness in models) and
 * should be changed.
 */
class DuplicatedPacketDetector
{
private:

  /** Cache to hold the packet identifiers and expiration time. */
  std::vector<CachedPacketId> m_cache;

  /** Default expiration time of each individual entry in the cache. */
  Time m_expiration_time;

public:

  DuplicatedPacketDetector ();

  DuplicatedPacketDetector (const Time & expiration_time);

  DuplicatedPacketDetector (const DuplicatedPacketDetector & copy);

  inline void
  SetExpirationTime (const Time & expiration_time)
  {
    m_expiration_time = expiration_time;
  }

  inline const Time &
  GetExpirationTime () const
  {
    return m_expiration_time;
  }

  /**
   * Returns the number of entries in the cache.
   */
  uint32_t
  Size ();

  /**
   * Removes all expired entries.
   */
  void
  Purge ();

  /**
   * Checks if a packet with the given pair of source IP address and ID exists.
   * If it does exist then the packet is a duplicate and should be ignored, if 
   * it doesn't exist then adds the data to the cache of received packets.
   * 
   * @param source_ip Source IP address of the received packet.
   * @param packet_id ID of the received packet.
   * 
   * @return <code>true</code> if the packet is duplicated, otherwise 
   * <code>false</code>.
   */
  bool
  IsDuplicate (const Ipv4Address & source_ip, const uint32_t packet_id);

  /**
   * Checks if a packet with ID equal to the ID of the packet pointed by 
   * <code>packet_ptr</code> and source IP address equal to the source IP in 
   * <code>ipv4_header</code> exists.
   * If it does exist then the packet is a duplicate and should be ignored, if 
   * it doesn't exist then adds the data to the cache of received packets.
   * 
   * @param packet_ptr Constant pointer of the packet to check.
   * @param ipv4_header The IP header to check.
   * 
   * @return <code>true</code> if the packet is duplicated, otherwise 
   * <code>false</code>.
   */
  bool
  IsDuplicate (Ptr<const Packet> packet_ptr, const Ipv4Header & ipv4_header);

private:

  // --------------------------
  // IsExpired
  // --------------------------

  struct IsExpired
  {

    /**
     * Checks if an entry is expired.
     * 
     * @param cached_id Cache identifier to check.
     * @return <code>true</code> if the cache entry is expired, <code>false</code>
     * otherwise.
     */
    inline bool
    operator() (const CachedPacketId & cached_id) const
    {
      return cached_id.GetExpirationTime () < Simulator::Now ();
    }
  };

};


} // namespace geotemporal
} // namespace ns3

#endif /* GEOTEMPORAL_DUPLICATE_DETECTOR_H */

