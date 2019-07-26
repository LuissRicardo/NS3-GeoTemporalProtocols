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

#include "geotemporal-spray-and-wait-duplicate-detector.h"

#include <algorithm>


namespace ns3
{
namespace geotemporal_spray_and_wait
{

// =============================================================================
//                                CachedPacketId
// =============================================================================

CachedPacketId::CachedPacketId ()
: m_source_ip (), m_packet_id (0u), m_expiration_time () { }

CachedPacketId::CachedPacketId (const Ipv4Address& source_ip,
                                const uint32_t packet_id,
                                const Time& expiration_time)
: m_source_ip (source_ip), m_packet_id (packet_id),
m_expiration_time (expiration_time) { }

CachedPacketId::CachedPacketId (const CachedPacketId& copy)
: m_source_ip (copy.m_source_ip), m_packet_id (copy.m_packet_id),
m_expiration_time (copy.m_expiration_time) { }


// =============================================================================
//                           DuplicatedPacketDetector
// =============================================================================

DuplicatedPacketDetector::DuplicatedPacketDetector ()
: DuplicatedPacketDetector (Seconds (5)) { }

DuplicatedPacketDetector::DuplicatedPacketDetector (const Time& expiration_time)
: m_cache (), m_expiration_time (expiration_time) { }

DuplicatedPacketDetector::DuplicatedPacketDetector (const DuplicatedPacketDetector& copy)
: m_cache (copy.m_cache), m_expiration_time (copy.m_expiration_time) { }

uint32_t
DuplicatedPacketDetector::Size ()
{
  Purge ();
  return m_cache.size ();
}

void
DuplicatedPacketDetector::Purge ()
{
  m_cache.erase (std::remove_if (m_cache.begin (), m_cache.end (),
                                 IsExpired ()), m_cache.end ());
}

bool
DuplicatedPacketDetector::IsDuplicate (const Ipv4Address& source_ip,
                                       const uint32_t packet_id)
{
  Purge ();

  for (std::vector<CachedPacketId>::const_iterator cached_id_it = m_cache.begin ();
          cached_id_it != m_cache.end (); ++cached_id_it)
    {
      if (cached_id_it->GetSourceIp () == source_ip &&
          cached_id_it->GetPacketId () == packet_id)
        {
          return true;
        }
    }

  m_cache.push_back (CachedPacketId (source_ip, packet_id,
                                     m_expiration_time + Simulator::Now ()));

  return false;
}

bool
DuplicatedPacketDetector::IsDuplicate (Ptr<const Packet> packet_ptr,
                                       const Ipv4Header& ipv4_header)
{
  return IsDuplicate (ipv4_header.GetSource (), packet_ptr->GetUid ());
}


} // namespace geotemporal_spray_and_wait
} // namespace ns3

