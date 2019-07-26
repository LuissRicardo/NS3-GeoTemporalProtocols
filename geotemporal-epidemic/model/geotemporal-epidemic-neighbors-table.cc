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
 * Neighbors table of the Geo-Temporal Epidemic protocol.
 */

#include "geotemporal-epidemic-neighbors-table.h"

#include <algorithm>
#include <cstdio>
#include <utility>

#include <ns3/log.h>

#include <ns3/packet-utils.h>


NS_LOG_COMPONENT_DEFINE ("GeoTemporalEpidemicNeighborsTable");

namespace ns3
{
namespace geotemporal_epidemic
{


// =============================================================================
//                                 NeighborEntry
// =============================================================================

NeighborEntry::NeighborEntry () : m_neighbor_ip (), m_expiration_time () { }

NeighborEntry::NeighborEntry (const Ipv4Address& neighbor_ip,
                              const Time& expiration_time)
: m_neighbor_ip (neighbor_ip), m_expiration_time ()
{
  SetExpirationTime (expiration_time);
}

NeighborEntry::NeighborEntry (const NeighborEntry& copy)
: m_neighbor_ip (copy.m_neighbor_ip), m_expiration_time (copy.m_expiration_time) { }

void
NeighborEntry::Print (std::ostream& os) const
{
  os << ToString ();
}

std::string
NeighborEntry::ToString () const
{
  char buffer[15];
  std::sprintf (buffer, "%.2f", m_expiration_time.ToDouble (Time::S));

  std::string str = "Neighbor entry "
          + GeoTemporalLibrary::LibraryUtils::ToString (m_neighbor_ip)
          + " will expire at second " + std::string (buffer);
  return str;
}


// =============================================================================
//                                NeighborsTable
// =============================================================================

NeighborsTable::NeighborsTable ()
: NeighborsTable (Seconds (15))
{
  NS_LOG_FUNCTION (this);
}

NeighborsTable::NeighborsTable (const Time& entries_expiration_time)
: m_table (), m_entries_expiration_time (entries_expiration_time)
{
  NS_LOG_FUNCTION (this);
}

NeighborsTable::NeighborsTable (const NeighborsTable& copy)
: m_table (copy.m_table),
m_entries_expiration_time (copy.m_entries_expiration_time)
{
  NS_LOG_FUNCTION (this);
}


// --------------------------
// Getters & Setters
// --------------------------

uint32_t
NeighborsTable::Size ()
{
  NS_LOG_FUNCTION (this);

  Purge ();
  return m_table.size ();
}


// --------------------------
// Lookup
// --------------------------

bool
NeighborsTable::Find (const Ipv4Address& neighbor_ip, NeighborEntry& entry)
{
  NS_LOG_FUNCTION (this);

  Purge ();

  ConstIterator_t entry_it = m_table.find (neighbor_ip);

  if (entry_it == m_table.end ())
    {
      return false;
    }

  entry = entry_it->second;
  return true;
}

bool
NeighborsTable::Find (const Ipv4Address& neighbor_ip)
{
  NS_LOG_FUNCTION (this);

  NeighborEntry temp;
  return Find (neighbor_ip, temp);
}

bool
NeighborsTable::Find (const NeighborEntry& neighbor_entry)
{
  NS_LOG_FUNCTION (this);

  NeighborEntry temp;
  return Find (neighbor_entry.GetNeighborIpAddress (), temp);
}


// --------------------------
// Modifiers
// --------------------------

bool
NeighborsTable::Insert (const Ipv4Address& new_neighbor_ip)
{
  NS_LOG_FUNCTION (this << new_neighbor_ip);

  Purge ();
  NS_LOG_DEBUG (m_table.size () << " neighbors before insertion of new neighbor "
                << new_neighbor_ip);

  if (Find (new_neighbor_ip))
    {
      NS_LOG_DEBUG ("Neighbor " << new_neighbor_ip
                    << " already present in table. Insertion ignored.");
      return false;
    }

  const NeighborEntry new_neighbor_entry (new_neighbor_ip, m_entries_expiration_time);
  m_table.insert (std::make_pair (new_neighbor_ip, new_neighbor_entry));

  NS_LOG_DEBUG (m_table.size () << " neighbors after insertion of new neighbor: "
                << new_neighbor_entry);

  return true;
}

bool
NeighborsTable::Remove (const Ipv4Address& neighbor_ip_to_delete)
{
  NS_LOG_FUNCTION (this << neighbor_ip_to_delete);

  Purge ();

  NS_LOG_DEBUG ("Removing neighbor " << neighbor_ip_to_delete);
  return m_table.erase (neighbor_ip_to_delete) > 0u;
}

bool
NeighborsTable::Remove (const NeighborEntry& neighbor_entry_to_delete)
{
  NS_LOG_FUNCTION (this << neighbor_entry_to_delete);
  return Remove (neighbor_entry_to_delete.GetNeighborIpAddress ());
}

bool
NeighborsTable::RestartNeighborEntryExpirationTime (const Ipv4Address& neighbor_ip)
{
  NS_LOG_FUNCTION (this << neighbor_ip);

  Iterator_t entry_it = m_table.find (neighbor_ip);

  if (entry_it == m_table.end ())
    {
      NS_LOG_DEBUG ("Neighbor " << neighbor_ip
                    << " not present in table. Operation ignored.");
      return false;
    }

  NS_LOG_DEBUG ("Expiration time of neighbor entry " << neighbor_ip
                << " restarted.");

  entry_it->second.SetExpirationTime (m_entries_expiration_time);
  return true;
}

void
NeighborsTable::Purge ()
{
  NS_LOG_FUNCTION (this);

  for (ConstIterator_t entry_it = m_table.begin (); entry_it != m_table.end ();)
    {
      if (entry_it->second.GetExpirationTime () <= Seconds (0))
        {
          NS_LOG_LOGIC ("Drops expired neighbor entry : " << entry_it->second);
          // m_table.erase (entry_it++); // For C++03
          entry_it = m_table.erase (entry_it); // For C++11
        }
      else
        {
          ++entry_it;
        }
    }
}

void
NeighborsTable::Print (std::ostream& os) const
{
  NS_LOG_FUNCTION (this);

  os << ToString ();
}

std::string
NeighborsTable::ToString () const
{
  NS_LOG_FUNCTION (this);

  char buffer [10];
  std::sprintf (buffer, "%u", (uint32_t) m_table.size ());
  std::string str = "Neighbors table with " + std::string (buffer)
          + " entries";

  if (m_table.size () > 0)
    str += ":";

  for (ConstIterator_t it = m_table.begin (); it != m_table.end (); ++it)
    {
      str += " " + GeoTemporalLibrary::LibraryUtils::ToString (it->second.GetNeighborIpAddress ());
    }
  return str;
}

} // namespace geotemporal_epidemic
} // namespace ns3

