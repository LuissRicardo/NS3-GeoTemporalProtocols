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
 * Neighbors table of the Geo-Temporal protocol.
 */

#ifndef GEOTEMPORAL_NEIGHBORS_TABLE_H
#define GEOTEMPORAL_NEIGHBORS_TABLE_H

#include <map>
#include <set>
#include <string>

#include <ns3/ipv4-address.h>
#include <ns3/nstime.h>
#include <ns3/simulator.h>

#include <ns3/packet-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;

namespace ns3
{
namespace geotemporal
{

/* Forward declaration */
namespace test
{
class NeighborsTableTest;
}

// =============================================================================
//                                 NeighborEntry
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Entry of the neighbors table of the Geo-Temporal protocol.
 */
class NeighborEntry
{
private:

  /** IPv4 address of the neighbor node. */
  Ipv4Address m_neighbor_ip;

  /** Expiration time of the neighbor entry. */
  Time m_expiration_time;

  /** The set of DATA packets that the neighbor node requested (the disjoint vector). */
  std::set<DataIdentifier> m_requested_packets;


public:

  NeighborEntry ();

  NeighborEntry (const Ipv4Address & neighbor_ip, const Time & expiration_time);

  NeighborEntry (const Ipv4Address & neighbor_ip,
                 const std::set<DataIdentifier> & requested_packets);

  NeighborEntry (const Ipv4Address & neighbor_ip, const Time & expiration_time,
                 const std::set<DataIdentifier> & requested_packets);

  NeighborEntry (const NeighborEntry & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  inline const Ipv4Address &
  GetNeighborIpAddress () const
  {
    return m_neighbor_ip;
  }

  inline Time
  GetExpirationTime () const
  {
    return m_expiration_time - Simulator::Now ();
  }

  inline void
  SetExpirationTime (const Time & expiration_time)
  {
    m_expiration_time = expiration_time + Simulator::Now ();
  }

  /** Returns the entire current set of requested packets. */
  inline const std::set<DataIdentifier> &
  GetRequestedPacketsSet () const
  {
    return m_requested_packets;
  }

  /**
   * Updates the whole set of requested packets and replaces the current content
   * of the set.
   */
  inline void
  SetRequestedPacketsSet (const std::set<DataIdentifier> & requested_packets)
  {
    m_requested_packets = requested_packets;
  }

  /**
   * Removes from the set of requested packets the specified packet.
   * 
   * @param requested_packet [IN] Packet to be removed.
   * 
   * @return <code>true</code> if the packet was removed, otherwise returns
   * <code>false</code> (i.e. the given packet wasn't actually in the set).
   */
  inline bool
  RemoveRequestedPacket (const DataIdentifier & requested_packet)
  {
    return m_requested_packets.erase (requested_packet) > 0u;
  }

  void
  Print (std::ostream &os) const;


  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const NeighborEntry & lhs, const NeighborEntry & rhs);
};

// NeighborEntry relational operators

inline bool
operator== (const NeighborEntry & lhs, const NeighborEntry & rhs)
{
  return lhs.m_neighbor_ip == rhs.m_neighbor_ip;
}

inline bool
operator!= (const NeighborEntry & lhs, const NeighborEntry & rhs)
{
  return !operator== (lhs, rhs);
}

// NeighborEntry stream operators

inline std::ostream &
operator<< (std::ostream & os, const NeighborEntry & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                NeighborsTable
// =============================================================================

/**
 * \ingroup geotemporal
 * 
 * Neighbors table of the Geo-Temporal protocol.
 */
class NeighborsTable
{
private:

  /** Table of neighbors. */
  std::map<Ipv4Address, NeighborEntry> m_table;

  /** Expiration time of the neighbor entries. */
  Time m_entries_expiration_time;

  /** Neighbors table iterator. */
  typedef std::map<Ipv4Address, NeighborEntry>::iterator Iterator_t;

  /** Neighbors table constant iterator. */
  typedef std::map<Ipv4Address, NeighborEntry>::const_iterator ConstIterator_t;


public:

  NeighborsTable ();

  NeighborsTable (const Time & entries_expiration_time);

  NeighborsTable (const NeighborsTable & copy);


  // --------------------------
  // Getters & Setters
  // --------------------------

  /**
   * Returns the expiration time of the entries in the neighbors table.
   */
  inline const Time &
  GetEntriesExpirationTime () const
  {
    return m_entries_expiration_time;
  }

  /**
   * Sets the expiration time of the entries in the neighbors table.
   * 
   * @param expiration_time [IN] The expiration time of the entries.
   */
  inline void
  SetEntriesExpirationTime (const Time & expiration_time)
  {
    m_entries_expiration_time = expiration_time;
  }

  /**
   * Returns the size of the neighbors table.
   */
  uint32_t
  Size () const;


  // --------------------------
  // Lookup
  // --------------------------

  /**
   * Finds a neighbor entry with the specified IP address.
   * 
   * @param neighbor_ip [IN] IPv4 address of the neighbor entry to search for.
   * @param entry [OUT] The neighbor entry with the specified IP address, if it
   * exists. If an entry with the given IP address is not found this out 
   * parameter is not modified.
   * 
   * @return <code>true</code> if a neighbor entry with the specified IP address 
   * exists. If no such neighbor entry is found then returns <code>false</code>.
   */
  bool
  Find (const Ipv4Address & neighbor_ip, NeighborEntry & entry) const;

  /**
   * Finds a neighbor entry with the specified IP address.
   * 
   * @param neighbor_ip [IN] IPv4 address of the neighbor entry to search for.
   * 
   * @return <code>true</code> if a neighbor entry with the specified IP address 
   * exists. If no such neighbor entry is found then returns <code>false</code>.
   */
  bool
  Find (const Ipv4Address & neighbor_ip) const;

  /**
   * Finds a neighbor entry with the same IP address of the given neighbor entry.
   * 
   * @param neighbor_entry [IN] Neighbor entry of the neighbor to search for.
   * 
   * @return <code>true</code> if a neighbor entry with the specified IP address 
   * exists. If no such neighbor entry is found then returns <code>false</code>.
   */
  bool
  Find (const NeighborEntry & neighbor_entry) const;


  // --------------------------
  // Modifiers
  // --------------------------

  /**
   * Removes all neighbor entries from the table.
   */
  inline void
  Clear ()
  {
    m_table.clear ();
  }

  /**
   * Inserts a new neighbor entry with the specified IP address.
   * 
   * Sets the expiration time of the new neighbor entry to the value of the 
   * <b>entries expiration time</b> parameter. This parameter can be modified
   * with the <code>SetEntriesExpirationTime (const Time &)</code> function.
   * 
   * @param new_neighbor_ip [IN] IP address of the new neighbor entry to insert.
   * 
   * @return <code>true</code> if the insertion took place. If there was no 
   * insertion then <code>false</code>.
   */
  bool
  Insert (const Ipv4Address & new_neighbor_ip);

  /**
   * Inserts a new neighbor entry with the specified IP address and set of 
   * requested packets.
   * 
   * Sets the expiration time of the new neighbor entry to the value of the 
   * <b>entries expiration time</b> parameter. This parameter can be modified
   * with the <code>SetEntriesExpirationTime (const Time &)</code> function.
   * 
   * @param new_neighbor_ip [IN] IP address of the new neighbor entry to insert.
   * @param requested_packets [IN] Set that contains the identifiers of the data
   * packets that the neighbor node requested.
   * 
   * @return <code>true</code> if the insertion took place. If there was no 
   * insertion then <code>false</code>.
   */
  bool
  Insert (const Ipv4Address & new_neighbor_ip,
          const std::set<DataIdentifier> & requested_packets);

  /**
   * Inserts a new neighbor entry.
   * 
   * The expiration time value of the given entry is changed to the value of the 
   * <b>entries expiration time</b> parameter. This parameter can be modified
   * with the <code>SetEntriesExpirationTime (const Time &)</code> function.
   * 
   * @param new_neighbor_entry [IN/OUT] Neighbor entry to insert. The expiration
   * time in the given entry object may be changed.
   * 
   * @return <code>true</code> if the insertion took place. If there was no 
   * insertion then <code>false</code>.
   */
  bool
  Insert (NeighborEntry & new_neighbor_entry);

  /**
   * Removes neighbor entry with the specified IP address from the neighbors 
   * table.
   * 
   * @param neighbor_ip_to_delete [IN] IP address of the neighbor entry to remove.
   * 
   * @return <code>true</code> if the removal took place. If there was no 
   * removal then <code>false</code>.
   */
  bool
  Remove (const Ipv4Address & neighbor_ip_to_delete);

  /**
   * Removes neighbor entry with the same IP address of the given neighbor entry
   * from the neighbors table.
   * 
   * @param neighbor_entry_to_delete [IN] Neighbor entry to remove.
   * 
   * @return <code>true</code> if the removal took place. If there was no 
   * removal then <code>false</code>.
   */
  bool
  Remove (const NeighborEntry & neighbor_entry_to_delete);

  /**
   * Updates the set of requested packets of the neighbor node with the specified
   * IP address.
   * 
   * @param neighbor_ip [IN] IP address of the neighbor entry to update.
   * @param requested_packets [IN] Set of requested packets to update into the
   * neighbor entry.
   * 
   * @return <code>true</code> if the update took place. If there was no 
   * update then <code>false</code>.
   */
  bool
  UpdateNeighborRequestedPackets (const Ipv4Address & neighbor_ip,
                                  const std::set<DataIdentifier> & requested_packets);

  /**
   * Removes from the set of requested packets of the neighbor entry the specified
   * packet.
   * 
   * @param neighbor_ip [IN] IP address of the neighbor entry where the requested
   * packet is removed from.
   * @param requested_packet [IN] ID of the packet to be removed.
   * 
   * @return <code>true</code> if the removal took place. If there was no 
   * removal then <code>false</code>.
   */
  bool
  RemoveNeighborRequestedPacket (const Ipv4Address & neighbor_ip,
                                 const DataIdentifier & requested_packet);

  /**
   * Restarts the expiration time of the neighbor entry with the specified IP
   * address (if it exists). 
   * 
   * @param neighbor_ip [IN] IP address of the neighbor entry to restart its 
   * expiration time.
   * 
   * @return <code>true</code> if the expiration time restart took place (i.e.,
   * an entry with the given IP address exists). Otherwise returns 
   * <code>false</code>.
   */
  bool
  RestartNeighborEntryExpirationTime (const Ipv4Address & neighbor_ip);


public:

  /**
   * Removes all expired neighbor entries from the neighbors table.
   */
  void
  Purge ();



  void
  Print (std::ostream &os) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  friend bool operator== (const NeighborsTable & lhs, const NeighborsTable & rhs);
  friend test::NeighborsTableTest;
};

// NeighborsTable relational operators

inline bool
operator== (const NeighborsTable & lhs, const NeighborsTable & rhs)
{
  return lhs.m_table == rhs.m_table
          && lhs.m_entries_expiration_time == rhs.m_entries_expiration_time;
}

inline bool
operator!= (const NeighborsTable & lhs, const NeighborsTable & rhs)
{
  return !operator== (lhs, rhs);
}

// NeighborsTable stream operators

inline std::ostream &
operator<< (std::ostream & os, const NeighborsTable & obj)
{
  obj.Print (os);
  return os;
}

} // namespace geotemporal
} // namespace ns3

#endif /* GEOTEMPORAL_NEIGHBORS_TABLE_H */

