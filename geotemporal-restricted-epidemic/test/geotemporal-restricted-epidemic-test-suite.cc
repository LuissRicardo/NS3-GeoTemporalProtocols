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
 */

#include "ns3/test.h"

#include <cmath>
#include <cstdio>
#include <map>
#include <set>

#include <ns3/geotemporal-restricted-epidemic-module.h>

#include <ns3/geotemporal-utils.h>
#include <ns3/math-utils.h>
#include <ns3/packet.h>
#include <ns3/packet-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;


namespace ns3
{
namespace geotemporal_restricted_epidemic
{
namespace test
{

// =============================================================================
//                             Needed free functions
// =============================================================================

static std::ostream &
operator<< (std::ostream & os, const std::set<DataIdentifier> & obj)
{
  os << "Set of DataIdentifiers with " << obj.size () << " items";

  if (obj.size () == 0) return os;

  os << ":";

  for (std::set<DataIdentifier>::const_iterator it = obj.begin (); it != obj.end (); ++it)
    {
      os << " " << it->ToString ();
    }

  return os;
}


// =============================================================================
//                                 TestCasePlus
// =============================================================================

class TestCasePlus : public TestCase
{
public:

  TestCasePlus (std::string name) : TestCase (name) { }

  template<typename T>
  void
  TestEqualityRelationalOperators (const T & equal_1, const T & equal_2, const T & different)
  {
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_1, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_1, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_1, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_1, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_1, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_1, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_2, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_2, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (equal_2, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_2, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_2, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (equal_2, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (different, equal_1), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (different, equal_2), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator== (different, different), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (different, equal_1), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (different, equal_2), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_restricted_epidemic::operator!= (different, different), false, "Must not be different");
  }
};


// =============================================================================
//                                PacketTypeTest
// =============================================================================

/**
 * PacketType enumeration test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class PacketTypeTest : public TestCasePlus
{
public:

  PacketTypeTest () : TestCasePlus ("PacketType") { }

  void
  DoRun () override
  {
    PacketType pt = PacketType::Hello;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::Hello, "Must be Hello");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "Hello", "Must be Hello");

    pt = PacketType::Reply;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::Reply, "Must be Reply");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "Reply", "Must be Reply");

    pt = PacketType::ReplyBack;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::ReplyBack, "Must be ReplyBack");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "ReplyBack", "Must be ReplyBack");

    pt = PacketType::Data;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::Data, "Must be Data");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "Data", "Must be Data");
  }
};


// =============================================================================
//                                TypeHeaderTest
// =============================================================================

/**
 * TypeHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class TypeHeaderTest : public TestCasePlus
{
public:

  TypeHeaderTest () : TestCasePlus ("TypeHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    TypeHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetPacketType (), PacketType::Hello, "Must be Hello");
    NS_TEST_EXPECT_MSG_EQ (h1.IsValid (), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 1u, "Must be 1u");

    // Parameters constructor
    TypeHeader h2 (PacketType::ReplyBack);
    NS_TEST_EXPECT_MSG_EQ (h2.GetPacketType (), PacketType::ReplyBack, "Must be ReplyBack");
    NS_TEST_EXPECT_MSG_EQ (h2.IsValid (), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 1u, "Must be 1u");

    // Copy constructor
    TypeHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetPacketType (), PacketType::ReplyBack, "Must be ReplyBack");
    NS_TEST_EXPECT_MSG_EQ (h3.IsValid (), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 1u, "Must be 1u");
  }

  void
  TestGettersSetters ()
  {
    TypeHeader header;

    NS_TEST_EXPECT_MSG_EQ (header.GetPacketType (), PacketType::Hello, "Must be Hello");

    header.SetPacketType (PacketType::Data);

    NS_TEST_EXPECT_MSG_EQ (header.GetPacketType (), PacketType::Data, "Must be Data");
  }

  void
  TestOverloadedOperators ()
  {
    TypeHeader equal_1 (PacketType::Data);
    TypeHeader equal_2 (PacketType::Data);
    TypeHeader different (PacketType::Hello);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    TypeHeader h1;
    TypeHeader h2 (PacketType::ReplyBack);
    TypeHeader h3 (h2);
    h2 = TypeHeader (PacketType::Data);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    TypeHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 1u, "TypeHeader is 1 byte long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 1u, "TypeHeader is 1 byte long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 1u, "TypeHeader is 1 byte long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestOverloadedOperators ();
    TestSerializationDeserialization ();
  }
};


// =============================================================================
//                                HelloHeaderTest
// =============================================================================

/**
 * HelloHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class HelloHeaderTest : public TestCasePlus
{
public:

  HelloHeaderTest () : TestCasePlus ("HelloHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    HelloHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVectorSize (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSenderNodeIp (), Ipv4Address (), "Must be an empty IPv4 address");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 7u, "Must be 7u");

    // Parameters constructor
    HelloHeader h2 (Ipv4Address ("1.30.2.44"), 17253u);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVectorSize (), 17253u, "Must be 17253u");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSenderNodeIp (), Ipv4Address ("1.30.2.44"), "Must be 1.30.2.44 IPv4 address");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 7u, "Must be 7u");

    // Copy constructor
    HelloHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVectorSize (), 17253u, "Must be 17253u");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSenderNodeIp (), Ipv4Address ("1.30.2.44"), "Must be 1.30.2.44 IPv4 address");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 7u, "Must be 7u");
  }

  void
  TestGettersSetters ()
  {
    HelloHeader header (Ipv4Address ("1.2.3.4"), 5u);

    NS_TEST_EXPECT_MSG_EQ (header.GetSenderNodeIp (), Ipv4Address ("1.2.3.4"), "IP must be 1.2.3.4");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");

    header.SetSenderNodeIp (Ipv4Address ("9.8.7.6"));

    NS_TEST_EXPECT_MSG_EQ (header.GetSenderNodeIp (), Ipv4Address ("9.8.7.6"), "IP must be 9.8.7.6");

    header.SetSummaryVectorSize (17u);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 17u, "Summary vector size must be 17");
  }

  void
  TestOverloadedOperators ()
  {
    HelloHeader equal_1 (Ipv4Address ("1.2.3.4"), 5u);
    HelloHeader equal_2 (Ipv4Address ("1.2.3.4"), 5u);
    HelloHeader different (Ipv4Address ("1.2.3.99"), 5u);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = HelloHeader (Ipv4Address ("1.2.3.4"), 5u);
    equal_2 = HelloHeader (Ipv4Address ("1.2.3.4"), 5u);
    different = HelloHeader (Ipv4Address ("1.2.3.4"), 99u);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    HelloHeader h1;
    HelloHeader h2 (Ipv4Address ("1.30.2.44"), 17253u);
    HelloHeader h3 (h2);
    h2 = HelloHeader (Ipv4Address ("1.2.3.4"), 5u);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    HelloHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 7u, "HelloHeader is 7 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 7u, "HelloHeader is 7 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 7u, "HelloHeader is 7 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    HelloHeader h (Ipv4Address ("1.2.3.4"), 54u);
    std::string expected_str = "HELLO from 1.2.3.4 who knows 54 packets";
    NS_TEST_EXPECT_MSG_EQ (h.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestOverloadedOperators ();
    TestSerializationDeserialization ();
    TestToString ();
  }
};


// =============================================================================
//                            SummaryVectorHeaderTest
// =============================================================================

/**
 * SummaryVectorHeaderTest packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class SummaryVectorHeaderTest : public TestCasePlus
{
public:

  SummaryVectorHeaderTest () : TestCasePlus ("SummaryVectorHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    SummaryVectorHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVectorSize (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVector (), std::set<DataIdentifier> (), "Must be an empty set");
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 19u, "Must be 19u");

    // Parameters constructor
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    SummaryVectorHeader h2 (summary_vector, position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 37u, "Must be 37u");

    // Copy constructor
    SummaryVectorHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h3.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 37u, "Must be 37u");
  }

  void
  TestGettersSetters ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};
    GeoTemporalLibrary::LibraryUtils::Vector2D position (5, 14);

    SummaryVectorHeader header (summary_vector, position);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 3u, "Summary vector size must be 3");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);

    summary_vector = {DataIdentifier (Ipv4Address ("9.9.9.9"), 9),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 19),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 29),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 39),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 49)};
    header.SetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (99, 88);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
  }

  void
  TestOverloadedOperators ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    SummaryVectorHeader equal_1 (summary_vector, position);
    SummaryVectorHeader equal_2 (summary_vector, position);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});

    SummaryVectorHeader different (summary_vector, position);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 3)});

    equal_1 = SummaryVectorHeader (summary_vector, position);
    equal_2 = SummaryVectorHeader (summary_vector, position);

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (99, 299);

    different = SummaryVectorHeader (summary_vector, position);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    SummaryVectorHeader h1;
    SummaryVectorHeader h2 (summary_vector, position);
    SummaryVectorHeader h3 (h2);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (-54, 23);

    h2 = SummaryVectorHeader (summary_vector, position);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    SummaryVectorHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 3u + 19u, "ReplyBackHeader is 37 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 1u + 19u, "ReplyBackHeader is 25 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 0u + 19u, "ReplyBackHeader is 19 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    SummaryVectorHeader h (summary_vector, position);
    std::string expected_str = "SUMMARY_VECTOR sent from position (3.00, 4.00) "
            "with 3 entries: 1.1.1.1:1 1.1.1.1:2 1.1.1.1:3 ";
    NS_TEST_EXPECT_MSG_EQ (h.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestOverloadedOperators ();
    TestSerializationDeserialization ();
    TestToString ();
  }
};


// =============================================================================
//                                DataHeaderTest
// =============================================================================

/**
 * DataHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class DataHeaderTest : public TestCasePlus
{
public:

  DataHeaderTest () : TestCasePlus ("DataHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    DataHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetDataIdentifier (), DataIdentifier (), "Must be default DataIdentifier");
    NS_TEST_EXPECT_MSG_EQ (h1.GetCreationCoordinates (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be default Vector2D");
    NS_TEST_EXPECT_MSG_EQ (h1.GetDestinationGeoTemporalArea (), GeoTemporalArea (), "Must be default GeoTemporalArea");
    NS_TEST_EXPECT_MSG_EQ (h1.GetMessage (), "", "Must be empty string");
    NS_TEST_EXPECT_MSG_EQ (h1.GetHopsCount (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 64u, "Must be 64u");

    // Parameters constructor
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (-75.62, 85.62);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader h2 (data_id, creation_coordinates, gta, message, hops);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetHopsCount (), hops, "Must be " << hops);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 64u + 16u, "Must be 80u");

    // Copy constructor
    DataHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h3.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h3.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h3.GetHopsCount (), hops, "Must be " << hops);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 64u + 16u, "Must be 80u");
  }

  void
  TestGettersSetters ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (-75.62, 85.62);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader header (data_id, creation_coordinates, gta, message, hops);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);

    data_id = DataIdentifier (Ipv4Address ("9.8.7.6"), 5u);
    header.SetDataIdentifier (data_id);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);

    creation_coordinates = GeoTemporalLibrary::LibraryUtils::Vector2D (3.25, -7456.325);
    header.SetCreationCoordinates (creation_coordinates);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);

    gta = GeoTemporalArea (TimePeriod (MilliSeconds (50), MilliSeconds (60)), Area (30, 40, 50, 60));
    header.SetDestinationGeoTemporalArea (gta);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);

    message = "new packet's message :)";
    header.SetMessage (message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);

    hops = 91u;
    header.SetHopsCount (hops);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetCreationCoordinates (), creation_coordinates, "Must be " << creation_coordinates);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops, "Must be " << hops);
  }

  void
  TestGetAllowedArea ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (100.0, 100.0);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)),
                         Area (535.0, 1063.0, 732.0, 1237.0));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader header (data_id, creation_coordinates, gta, message, hops);

    Area allowed_area, expected_area;
    expected_area = Area (100.0, 100.0, 732.0, 1237.0);
    allowed_area = header.GetAllowedArea ();

    NS_TEST_EXPECT_MSG_EQ (allowed_area, expected_area, "Must be " << expected_area);

    creation_coordinates = GeoTemporalLibrary::LibraryUtils::Vector2D (681.0, 765.0);
    header.SetCreationCoordinates (creation_coordinates);

    expected_area = Area (535.0, 765.0, 732.0, 1237.0);
    allowed_area = header.GetAllowedArea ();

    NS_TEST_EXPECT_MSG_EQ (allowed_area, expected_area, "Must be " << expected_area);

    creation_coordinates = GeoTemporalLibrary::LibraryUtils::Vector2D (679.0, 1107.0);
    header.SetCreationCoordinates (creation_coordinates);

    expected_area = Area (535.0, 1063.0, 732.0, 1237.0);
    allowed_area = header.GetAllowedArea ();

    NS_TEST_EXPECT_MSG_EQ (allowed_area, expected_area, "Must be " << expected_area);

    creation_coordinates = GeoTemporalLibrary::LibraryUtils::Vector2D (931.0, 1499.0);
    header.SetCreationCoordinates (creation_coordinates);

    expected_area = Area (535.0, 1063.0, 931.0, 1499.0);
    allowed_area = header.GetAllowedArea ();

    NS_TEST_EXPECT_MSG_EQ (allowed_area, expected_area, "Must be " << expected_area);
  }

  void
  TestOverloadedOperators ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (-75.62, 85.62);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader equal_1 (data_id, creation_coordinates, gta, message, hops);
    DataHeader equal_2 (data_id, creation_coordinates, gta, message, hops);
    DataHeader different (DataIdentifier (Ipv4Address ("1.2.3.4"), 99u), creation_coordinates, gta, message, hops);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, GeoTemporalLibrary::LibraryUtils::Vector2D (3.25, -7456.325),
                            gta, message, hops);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, creation_coordinates,
                            GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (0, 0, 100, 100)),
                            message, hops);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, creation_coordinates, gta, "Different message", hops);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, creation_coordinates, gta, message, 1u);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (-75.62, 85.62);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader h1;
    DataHeader h2 (data_id, creation_coordinates, gta, message, hops);
    DataHeader h3 (h2);

    h2 = DataHeader (data_id, creation_coordinates, gta, "Different message", 2u); // Message length = 17

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    DataHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 80u, "DataHeader is 80 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 81u, "DataHeader is 81 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 64u, "DataHeader is 64 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalLibrary::LibraryUtils::Vector2D creation_coordinates (-75.62, 85.62);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t hops = 5u;

    DataHeader h (data_id, creation_coordinates, gta, message, hops);
    std::string expected_str = "DATA 1.2.3.4:5 (5 hops left) created at "
            "(-75.62, 85.62) destined to area "
            "{(30.00, 40.00), (50.00, 60.00)} to start at second "
            "10.00 with a duration of 10.00 seconds has a message of 16 byte(s)";
    NS_TEST_EXPECT_MSG_EQ (h.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestGetAllowedArea ();
    TestOverloadedOperators ();
    TestSerializationDeserialization ();
    TestToString ();
  }
};


// =============================================================================
//                               NeighborEntryTest
// =============================================================================

/**
 * NeighborEntry for the neighbors table test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class NeighborEntryTest : public TestCasePlus
{
public:

  NeighborEntry m_neighbor_entry;

  NeighborEntryTest () : TestCasePlus ("NeighborEntry"), m_neighbor_entry () { }

  void
  TestConstructors ()
  {
    // Default constructor
    NeighborEntry e1;
    NS_TEST_EXPECT_MSG_EQ (e1.GetNeighborIpAddress (), Ipv4Address (), "Must be default IP");
    NS_TEST_EXPECT_MSG_EQ (e1.GetExpirationTime (), Time (), "Must be default time (0)");

    // Parameters constructor
    NeighborEntry e2 (Ipv4Address ("1.2.3.4"), Seconds (10));
    NS_TEST_EXPECT_MSG_EQ (e2.GetNeighborIpAddress (), Ipv4Address ("1.2.3.4"), "Must be default IP");
    NS_TEST_EXPECT_MSG_EQ (e2.GetExpirationTime (), Seconds (10), "Must be default time (0)");

    // Copy constructor
    NeighborEntry e3 (e2);
    NS_TEST_EXPECT_MSG_EQ (e3.GetNeighborIpAddress (), Ipv4Address ("1.2.3.4"), "Must be default IP");
    NS_TEST_EXPECT_MSG_EQ (e3.GetExpirationTime (), Seconds (10), "Must be default time (0)");
  }

  void
  TestOverloadedOperators ()
  {
    NeighborEntry equal_1 (Ipv4Address ("1.2.3.4"), Seconds (10));
    NeighborEntry equal_2 (Ipv4Address ("1.2.3.4"), Seconds (10));
    NeighborEntry different (Ipv4Address ("10.20.30.40"), Seconds (10));

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Note that the equality operator of NeighborEntry object only considers
    // the IP of the neighbor node.
    equal_1 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (10));
    equal_2 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (5));
    different = NeighborEntry (Ipv4Address ("10.20.30.40"), Seconds (10));

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestToString_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 2.5
    NeighborEntry entry (Ipv4Address ("1.2.3.4"), Seconds (10));
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 12.50";
    NS_TEST_EXPECT_MSG_EQ (entry.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestToString_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 15.25
    NeighborEntry entry (Ipv4Address ("1.2.3.4"), Seconds (17));
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 32.25";
    NS_TEST_EXPECT_MSG_EQ (entry.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestToString_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 21.9
    NeighborEntry entry (Ipv4Address ("1.2.3.4"), Seconds (3));
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 24.90";
    NS_TEST_EXPECT_MSG_EQ (entry.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestGetSetExpirationTime_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 2.5
    Time expected_expiration_time = Seconds (47.5);
    NS_TEST_EXPECT_MSG_EQ_TOL (m_neighbor_entry.GetExpirationTime (), expected_expiration_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_expiration_time.ToDouble (Time::S)
                               << " seconds, got: " << m_neighbor_entry.GetExpirationTime ().ToDouble (Time::S)
                               << " seconds.");

    // Reset the expiration time
    m_neighbor_entry.SetExpirationTime (Seconds (180));
  }

  void
  TestGetSetExpirationTime_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 15.25
    Time expected_expiration_time = Seconds (167.25);
    NS_TEST_EXPECT_MSG_EQ_TOL (m_neighbor_entry.GetExpirationTime (), expected_expiration_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_expiration_time.ToDouble (Time::S)
                               << " seconds, got: " << m_neighbor_entry.GetExpirationTime ().ToDouble (Time::S)
                               << " seconds.");

    // Reset the expiration time
    m_neighbor_entry.SetExpirationTime (Seconds (10));
  }

  void
  TestGetSetExpirationTime_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 21.9
    Time expected_expiration_time = Seconds (3.35);
    NS_TEST_EXPECT_MSG_EQ_TOL (m_neighbor_entry.GetExpirationTime (), expected_expiration_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_expiration_time.ToDouble (Time::S)
                               << " seconds, got: " << m_neighbor_entry.GetExpirationTime ().ToDouble (Time::S)
                               << " seconds.");
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestOverloadedOperators ();

    // -- Test ToString --
    Simulator::Schedule (Seconds (2.5), &NeighborEntryTest::TestToString_Scheduled_1, this);
    Simulator::Schedule (Seconds (15.25), &NeighborEntryTest::TestToString_Scheduled_2, this);
    Simulator::Schedule (Seconds (21.9), &NeighborEntryTest::TestToString_Scheduled_3, this);

    Simulator::Run ();
    Simulator::Destroy ();

    // -- Test Get/Set Expiration time
    m_neighbor_entry = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (50));

    Simulator::Schedule (Seconds (2.5), &NeighborEntryTest::TestGetSetExpirationTime_Scheduled_1, this);
    Simulator::Schedule (Seconds (15.25), &NeighborEntryTest::TestGetSetExpirationTime_Scheduled_2, this);
    Simulator::Schedule (Seconds (21.9), &NeighborEntryTest::TestGetSetExpirationTime_Scheduled_3, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }
};


// =============================================================================
//                               NeighborsTableTest
// =============================================================================

/**
 * NeighborsTable for the neighbors table test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class NeighborsTableTest : public TestCasePlus
{
public:

  NeighborsTable m_neighbors_table;

  NeighborsTableTest () : TestCasePlus ("NeighborsTable"), m_neighbors_table () { }

  void
  TestConstructors ()
  {
    // Default constructor
    NeighborsTable e1;
    NS_TEST_EXPECT_MSG_EQ (e1.GetEntriesExpirationTime (), Seconds (15), "Must be 15 seconds");
    NS_TEST_EXPECT_MSG_EQ (e1.Size (), 0u, "Must be empty (0 entries)");

    // Parameters constructor
    NeighborsTable e2 (Seconds (10));
    NS_TEST_EXPECT_MSG_EQ (e2.GetEntriesExpirationTime (), Seconds (10), "Must be 10 seconds");
    NS_TEST_EXPECT_MSG_EQ (e2.Size (), 0u, "Must be empty (0 entries)");

    // Copy constructor
    NeighborsTable e3 (e2);
    NS_TEST_EXPECT_MSG_EQ (e3.GetEntriesExpirationTime (), Seconds (10), "Must be 10 seconds");
    NS_TEST_EXPECT_MSG_EQ (e3.Size (), 0u, "Must be empty (0 entries)");
  }

  void
  TestGetSetEntriesExpirationTime ()
  {
    m_neighbors_table = NeighborsTable (Seconds (5));

    m_neighbors_table.SetEntriesExpirationTime (Minutes (13));
    NS_TEST_EXPECT_MSG_EQ_TOL (m_neighbors_table.GetEntriesExpirationTime (), Minutes (13), MicroSeconds (1),
                               "Expiration time must be 13 minutes.");

    m_neighbors_table.SetEntriesExpirationTime (Days (7));
    NS_TEST_EXPECT_MSG_EQ_TOL (m_neighbors_table.GetEntriesExpirationTime (), Days (7), MicroSeconds (1),
                               "Expiration time must be 7 days.");

  }

  void
  TestFindFunctions_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 6.00
    bool found = false;

    // Some packet entries already expired at second 5.0.
    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 10
    //      1.1.1.2       -      second 10

    // The entry to be found expired at second 5, so it must not be found.
    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.3"));
    NS_TEST_EXPECT_MSG_EQ (found, false, "Neighbor entry 1.1.1.3 must not be found.");

    // There should be 2 entries in the table
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 2u, "Size of the neighbors table must be 2.");

    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.1"));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.1 must be found.");

    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.2"));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.2 must be found.");
  }

  void
  TestFindFunctions ()
  {
    NeighborEntry entry;
    bool found = false;

    m_neighbors_table = NeighborsTable (Seconds (10));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));

    m_neighbors_table.SetEntriesExpirationTime (Seconds (5));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.4"));

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 10
    //      1.1.1.2       -      second 10
    //      1.1.1.3       -      second 5
    //      1.1.1.4       -      second 5

    // Test bool Find (const Ipv4Address &, NeighborEntry &);
    // - Successfully found
    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.1 must be found.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetNeighborIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Entry must have IP address 1.1.1.1.");
    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Entry must have 10 seconds of expiration time.");

    // - Expected not found
    found = m_neighbors_table.Find (Ipv4Address ("2.2.2.2"), entry);
    NS_TEST_EXPECT_MSG_EQ (found, false, "Neighbor entry 2.2.2.2 must not be found.");

    // Test bool Find (const Ipv4Address & neighbor_ip);
    // - Successfully found
    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.2"));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.2 must be found.");

    // - Expected not found
    found = m_neighbors_table.Find (Ipv4Address ("2.2.2.2"));
    NS_TEST_EXPECT_MSG_EQ (found, false, "Neighbor entry 2.2.2.2 must not be found.");

    // Test bool Find (const NeighborEntry & neighbor_entry);
    // - Successfully found
    found = m_neighbors_table.Find (NeighborEntry (Ipv4Address ("1.1.1.2"), Days (2)));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.2 must be found.");

    // - Expected not found
    found = m_neighbors_table.Find (NeighborEntry (Ipv4Address ("2.2.2.2"), Days (2)));
    NS_TEST_EXPECT_MSG_EQ (found, false, "Neighbor entry 2.2.2.2 must not be found.");

    // The following scheduled calls test:
    //  - That Find calls Purge()
    Simulator::Schedule (Seconds (6), &NeighborsTableTest::TestFindFunctions_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestInsertFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 7.00
    bool inserted;
    NeighborEntry entry;

    // The following entry to be inserted used to exist but was purged by the call
    // to Insert before it was detected as already existing and failing, so it 
    // is inserted as newly inserted.
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.3 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3u, "Size of the neighbors table must be 3.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Entry 1.1.1.3 expiration time must be 5 seconds.");
  }

  void
  TestInsertFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 13.00
    bool inserted;

    // The following entry to be inserted still exits and expires at second 15,
    // so the insertion will fail. Other 2 entries expired at 10 and 12 seconds
    // so they will be purged by the call to Purge.

    // Expected failure to insert an already existing neighbor.
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));

    NS_TEST_EXPECT_MSG_EQ (inserted, false, "Neighbor entry 1.1.1.2 must have not been inserted.");

    // Only the entry that expires at 15 must exist in the table.
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1u, "Size of the neighbors table must be 1.");
  }

  void
  TestInsertFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0u, "Neighbors table must be empty.");

    bool inserted;
    NeighborEntry entry;

    // Successful insertion of new entry
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.1 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1u, "Size of the neighbors table must be 1.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Entry 1.1.1.1 expiration time must be 10 seconds.");

    // Expected failure of already inserted entry
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (inserted, false, "Neighbor entry 1.1.1.1 must have not been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1u, "Size of the neighbors table must be 1.");

    // Test that changing the expiration time of the entries is applied to new entries.
    m_neighbors_table.SetEntriesExpirationTime (Seconds (15));

    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.2 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 2u, "Size of the neighbors table must be 2.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.2"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (15), MicroSeconds (1),
                               "Entry 1.1.1.2 expiration time must be 15 seconds.");

    // Now the expiration time is set to 5 seconds
    m_neighbors_table.SetEntriesExpirationTime (Seconds (5));

    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.3 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3u, "Size of the neighbors table must be 3.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Entry 1.1.1.3 expiration time must be 5 seconds.");

    // The following scheduled calls test:
    //  - That Insert calls Purge()
    Simulator::Schedule (Seconds (7), &NeighborsTableTest::TestInsertFunction_Scheduled_1, this);
    Simulator::Schedule (Seconds (13), &NeighborsTableTest::TestInsertFunction_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestRemoveFunction_Scheduled_Now ()
  {
    // This function is launched by the scheduler at second 0.00
    bool removed;

    // Successful removal of existing entry
    removed = m_neighbors_table.Remove (Ipv4Address ("1.1.1.3"));

    NS_TEST_EXPECT_MSG_EQ (removed, true, "Neighbor entry 1.1.1.3 must have been removed.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 4u, "Size of the neighbors table must be 4.");

    // Expected failure to remove a non-existing entry
    removed = m_neighbors_table.Remove (Ipv4Address ("10.20.30.40"));

    NS_TEST_EXPECT_MSG_EQ (removed, false, "Neighbor entry 10.20.30.40 must have not been removed.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 4u, "Size of the neighbors table must be 4.");
  }

  void
  TestRemoveFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 7.50
    bool removed;
    NeighborEntry entry_to_remove;

    // Expected failure to remove a non-existing entry
    entry_to_remove = NeighborEntry (Ipv4Address ("100.100.100.100"), Days (2));

    removed = m_neighbors_table.Remove (entry_to_remove);

    NS_TEST_EXPECT_MSG_EQ (removed, false, "Neighbor entry 100.100.100.100 must have not been removed.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 4u, "Size of the neighbors table must be 4.");

    // Successful removal of existing entry
    entry_to_remove = NeighborEntry (Ipv4Address ("1.1.1.2"), Days (2));

    removed = m_neighbors_table.Remove (entry_to_remove);

    NS_TEST_EXPECT_MSG_EQ (removed, true, "Neighbor entry 1.1.1.2 must have been removed.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3u, "Size of the neighbors table must be 3.");
  }

  void
  TestRemoveFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 15.00
    bool removed;

    // The following entry to be removed used to exist but was purged by the call
    // to Remove before it was removed explicitly, so it must return a false.
    removed = m_neighbors_table.Remove (NeighborEntry (Ipv4Address ("1.1.1.1"), Days (2)));

    NS_TEST_EXPECT_MSG_EQ (removed, false, "Neighbor entry 1.1.1.1 must have not been removed.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0u, "Size of the neighbors table must be 0.");
  }

  void
  TestRemoveFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.4"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.5"));

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 5u, "Size of the neighbors table must be 5.");

    // The following scheduled call tests:
    //  - bool Remove (const Ipv4Address &);
    //  - Remove an existing entry.
    //  - Remove a non-existing entry.
    Simulator::ScheduleNow (&NeighborsTableTest::TestRemoveFunction_Scheduled_Now, this);

    // The following scheduled call tests:
    //  - bool Remove (const NeighborEntry &)
    //  - Remove an existing entry.
    //  - Remove a non-existing entry.
    Simulator::Schedule (Seconds (7.5), &NeighborsTableTest::TestRemoveFunction_Scheduled_1, this);

    // The following scheduled call tests:
    //  - That Remove calls Purge()
    Simulator::Schedule (Seconds (15), &NeighborsTableTest::TestRemoveFunction_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestRestartNeighborEntryExpirationTimeFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 9.00

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 10

    bool restarted;
    NeighborEntry entry;

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (1), MicroSeconds (1),
                               "Entry 1.1.1.1 expiration time must be 1 seconds.");

    restarted = m_neighbors_table.RestartNeighborEntryExpirationTime (Ipv4Address ("1.1.1.1"));
    NS_TEST_EXPECT_MSG_EQ (restarted, true, "The expiration time of entry 1.1.1.1 must have been restarted.");

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 19

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Entry 1.1.1.1 expiration time must be 10 seconds.");
  }

  void
  TestRestartNeighborEntryExpirationTimeFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 21.00

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 19

    bool restarted, found;
    NeighborEntry entry;

    // Before calling RestartNeighborEntryExpirationTime (const Ipv4Address&)
    // we don't use other functions that call Purge () to avoid purging the 
    // expired entry.

    restarted = m_neighbors_table.RestartNeighborEntryExpirationTime (Ipv4Address ("1.1.1.1"));
    NS_TEST_EXPECT_MSG_EQ (restarted, true, "The expiration time of entry 1.1.1.1 must have been restarted.");

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 31

    found = m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Neighbor entry 1.1.1.1 must be found.");
    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Entry 1.1.1.1 expiration time must be 10 seconds.");
  }

  void
  TestRestartNeighborEntryExpirationTimeFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 10

    // Test that the expiration time of the valid entry (with 1 second left 
    // before expiring) can be restarted.
    Simulator::Schedule (Seconds (9), &NeighborsTableTest::TestRestartNeighborEntryExpirationTimeFunction_Scheduled_1, this);

    // The neighbors table now looks like this:
    //      Neighbor IP   -   Expiration time
    //      1.1.1.1       -      second 19

    // Test that the expiration time of the expired entry (with 2 seconds past 
    // its expiration time) can be restarted.
    Simulator::Schedule (Seconds (21), &NeighborsTableTest::TestRestartNeighborEntryExpirationTimeFunction_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestToStringFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    std::string expected_str = "Neighbors table with 0 entries";
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.ToString (), expected_str, "Expected string: " + expected_str);

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));

    expected_str = "Neighbors table with 2 entries: 1.1.1.1 1.1.1.2";
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.ToString (), expected_str, "Expected string: " + expected_str);

    m_neighbors_table.SetEntriesExpirationTime (Seconds (5));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.4"));
    m_neighbors_table.Insert (Ipv4Address ("1.1.1.5"));

    expected_str = "Neighbors table with 5 entries: 1.1.1.1 1.1.1.2 1.1.1.3 1.1.1.4 1.1.1.5";
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGetSetEntriesExpirationTime ();
    TestFindFunctions ();
    TestInsertFunction ();
    TestRemoveFunction ();
    TestRestartNeighborEntryExpirationTimeFunction ();
    TestToStringFunction ();
  }
};


// =============================================================================
//                               PacketQueueEntryTest
// =============================================================================

/**
 * PacketQueueEntry for the data packets queue test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class PacketQueueEntryTest : public TestCasePlus
{
public:

  const DataIdentifier m_data_id;
  const GeoTemporalLibrary::LibraryUtils::Vector2D m_creation_coordinates;
  const GeoTemporalArea m_geo_temporal_area;
  const std::string m_message;
  const uint32_t m_hops_count;
  const DataHeader m_header;

  PacketQueueEntry m_queue_entry;

  PacketQueueEntryTest ()
  : TestCasePlus ("PacketQueueEntry"),
  m_data_id (Ipv4Address ("1.1.1.1"), 1u),
  m_creation_coordinates (-75.62, 85.62),
  m_geo_temporal_area (TimePeriod (Seconds (34), Seconds (74)), Area (10, 10, 90, 90)),
  m_message ("0123456789"),
  m_hops_count (15u),
  m_header (m_data_id, m_creation_coordinates, m_geo_temporal_area, m_message, m_hops_count),
  m_queue_entry () { }

  void
  TestConstructors ()
  {
    // Default constructor
    PacketQueueEntry e1;

    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacketId (), DataIdentifier (), "Must be the default DataIdentifier.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacket (), DataHeader (), "Must be the default DataHeader.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetHopsCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetExpirationTime (), Time (), "Must be the default Time.");

    // Parameters constructor
    PacketQueueEntry e2 (m_header);

    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ (e2.GetHopsCount (), m_hops_count, "Must be " << m_hops_count);
    NS_TEST_EXPECT_MSG_EQ_TOL (e2.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");

    // Copy constructor
    PacketQueueEntry e3 (e2);

    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ (e3.GetHopsCount (), m_hops_count, "Must be " << m_hops_count);
    NS_TEST_EXPECT_MSG_EQ_TOL (e3.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");
  }

  void
  TestGetHopsCount ()
  {
    PacketQueueEntry entry (m_header);

    NS_TEST_EXPECT_MSG_EQ (entry.GetHopsCount (), m_hops_count, "Must be " << m_hops_count);
  }

  void
  TestExpirationTime_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 13.82
    Time expected_time = Seconds (69.18); // 83 - 13.82 = 69.18
    NS_TEST_EXPECT_MSG_EQ_TOL (m_queue_entry.GetExpirationTime (), expected_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_time.ToDouble (Time::S)
                               << " seconds.");

    m_queue_entry.SetExpirationTime (Seconds (28.6)); // Will expire at second 42.42 (13.82 + 28.6)
  }

  void
  TestExpirationTime_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 25.25
    Time expected_time = Seconds (17.17); // 42.42 − 25.25 = 17.17
    NS_TEST_EXPECT_MSG_EQ_TOL (m_queue_entry.GetExpirationTime (), expected_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_time.ToDouble (Time::S)
                               << " seconds.");

    m_queue_entry.SetExpirationTime (30u, 43u); // Will expire at second 73 (30 + 43)
  }

  void
  TestExpirationTime_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 61.9
    Time expected_time = Seconds (11.1); // 73 − 61.9 = 11.1
    NS_TEST_EXPECT_MSG_EQ_TOL (m_queue_entry.GetExpirationTime (), expected_time, MicroSeconds (1),
                               "Expected expiration time: " << expected_time.ToDouble (Time::S)
                               << " seconds.");
  }

  void
  TestExpirationTime ()
  {
    DataHeader header (m_header);
    header.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (37), Seconds (83)),
                                                           Area (10, 10, 90, 90)));

    m_queue_entry = PacketQueueEntry (header);

    Simulator::Schedule (Seconds (13.82), &PacketQueueEntryTest::TestExpirationTime_Scheduled_1, this);
    Simulator::Schedule (Seconds (25.25), &PacketQueueEntryTest::TestExpirationTime_Scheduled_2, this);
    Simulator::Schedule (Seconds (61.9), &PacketQueueEntryTest::TestExpirationTime_Scheduled_3, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestToStringFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 13.82
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 83";

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);

    m_queue_entry.SetExpirationTime (Seconds (28.6)); // Will expire at second 42.42 (13.82 + 28.6)
  }

  void
  TestToStringFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 42.42
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 83";

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);

    m_queue_entry.SetExpirationTime (30u, 43u); // Will expire at second 73 (30 + 43)
  }

  void
  TestToStringFunction_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 61.9
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 73";

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);
  }

  void
  TestToStringFunction ()
  {
    DataHeader header (m_header);
    header.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (37), Seconds (83)),
                                                           Area (10, 10, 90, 90)));

    m_queue_entry = PacketQueueEntry (header);

    Simulator::Schedule (Seconds (13.82), &PacketQueueEntryTest::TestExpirationTime_Scheduled_1, this);
    Simulator::Schedule (Seconds (25.25), &PacketQueueEntryTest::TestExpirationTime_Scheduled_2, this);
    Simulator::Schedule (Seconds (61.9), &PacketQueueEntryTest::TestExpirationTime_Scheduled_3, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestOverloadedOperators ()
  {
    // Different DataHeader
    PacketQueueEntry equal_1 (m_header);
    PacketQueueEntry equal_2 (m_header);

    DataHeader diff_header (m_header);
    diff_header.SetHopsCount (8u);
    PacketQueueEntry different (diff_header);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different expiration time
    equal_1 = PacketQueueEntry (m_header);
    equal_2 = PacketQueueEntry (m_header);
    different = PacketQueueEntry (m_header);

    different.SetExpirationTime (Seconds (400));

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGetHopsCount ();
    TestExpirationTime ();
    TestToStringFunction ();
    TestOverloadedOperators ();
  }
};


// =============================================================================
//                               PacketsQueueTest
// =============================================================================

/**
 * PacketsQueue for the data packets queue test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class PacketsQueueTest : public TestCasePlus
{
public:

  PacketsQueue m_packets_queue;

  PacketsQueueTest () : TestCasePlus ("PacketsQueue"), m_packets_queue () { }

  void
  TestConstructors ()
  {
    std::set<DataIdentifier> summary_vector;

    // Default constructor
    PacketsQueue q1;
    q1.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q1.GetMaxLength (), 128u, "Must be 128.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q1.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q1.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Parameters constructor
    PacketsQueue q2 (17u);
    q2.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q2.GetMaxLength (), 17u, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q2.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q2.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Copy constructor
    PacketsQueue q3 (q2);
    q3.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q3.GetMaxLength (), 17u, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q3.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q3.GetPacketReceptionStats ().empty (), true, "Must be empty.");
  }

  void
  TestGettersSetters ()
  {
    m_packets_queue = PacketsQueue (67);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67u, "Must be 67.");

    m_packets_queue.SetMaxLength (11u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 11u, "Must be 11.");

    m_packets_queue.SetMaxLength (1u);

    Ipv4Address source_ip = Ipv4Address ("1.1.1.1");

    DataHeader data_packet (/* Data ID */ DataIdentifier (source_ip, 0u),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);
    m_packets_queue.Enqueue (data_packet, source_ip);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");

    for (uint32_t i = 1u; i <= 100u; ++i)
      {
        data_packet.SetDataIdentifier (DataIdentifier (source_ip, i));
        m_packets_queue.Enqueue (data_packet, source_ip);

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), i, "Must be " << i);
      }
  }

  void
  TestGetSize_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 15.00
    // Al entries must be purged by Size.
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Size of the packets queue must be 0.");
  }

  void
  TestGetSize ()
  {
    m_packets_queue = PacketsQueue (5u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Size of the packets queue must be 0.");

    Ipv4Address source_ip = Ipv4Address ("1.1.1.1");
    DataHeader data_packet (/* Data ID */ DataIdentifier (source_ip, 0u),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);

    for (uint32_t i = 0u; i < 5u; ++i)
      {
        data_packet.SetDataIdentifier (DataIdentifier (source_ip, i));

        m_packets_queue.Enqueue (data_packet, source_ip);

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u + i,
                               "Size of the packets queue must be " << (1u + i));
      }

    for (uint32_t i = 0u; i < 15u; ++i)
      {
        data_packet.SetDataIdentifier (DataIdentifier (source_ip, 10u + i));

        m_packets_queue.Enqueue (data_packet, source_ip);

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 5u, "Size of the packets queue must be 5.");
      }

    // The following scheduled calls test:
    //  - That Size () calls Purge()
    Simulator::Schedule (Seconds (15), &PacketsQueueTest::TestGetSize_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestGetSummaryVector_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 6.00

    // Some packet entries already expired at second 5.0.
    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 10

    std::set<DataIdentifier> summary_vector, expected_summary_vector;

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector size must be the expected.");
  }

  void
  TestClear ()
  {
    m_packets_queue = PacketsQueue (15);

    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);

    Ipv4Address ip ("1.1.1.1");

    for (uint32_t id = 0; id < 10; ++id)
      {
        data_packet.SetDataIdentifier (DataIdentifier (ip, id));

        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 10, "Must be 10");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 10, "Must be 10");

    m_packets_queue.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 10, "Must be 10");

    ip = Ipv4Address ("1.1.1.2");

    for (uint32_t id = 0; id < 15; ++id)
      {
        data_packet.SetDataIdentifier (DataIdentifier (ip, id));

        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 15, "Must be 15");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 25, "Must be 25");

    m_packets_queue.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 25, "Must be 25");

    ip = Ipv4Address ("1.1.1.3");

    for (uint32_t id = 0; id < 25; ++id)
      {
        data_packet.SetDataIdentifier (DataIdentifier (ip, id));

        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 15, "Must be 15");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 50, "Must be 50");

    m_packets_queue.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 50, "Must be 50");
  }

  void
  TestGetSummaryVector ()
  {
    std::set<DataIdentifier> summary_vector, expected_summary_vector;
    m_packets_queue = PacketsQueue (15);

    // Test when packets queue is empty
    m_packets_queue.GetSummaryVector (summary_vector);
    NS_TEST_EXPECT_MSG_EQ (summary_vector.size (), 0u, "Summary vector must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 10
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (2), Seconds (10)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2"),
      DataIdentifier ("1.1.1.3:3")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.4"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2"),
      DataIdentifier ("1.1.1.3:3"), DataIdentifier ("1.1.1.4:4")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 10
    //    1.1.1.3:3  -           second 5
    //    1.1.1.4:4  -           second 5

    // The following scheduled calls test:
    //  - That GetSummaryVector () calls Purge()
    Simulator::Schedule (Seconds (6), &PacketsQueueTest::TestGetSummaryVector_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestProcessDisjointVector_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 8.00
    SummaryVectorHeader received_sv_header;
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));

    std::set<DataIdentifier> disjoint_vector, expected_disjoint_vector;

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      1
    //    2.2.2.2:5  -     second 15     -      5 to 15     -   0, 0, 100, 100   -      1
    //    2.2.2.2:6  -     second 10     -      0 to 10     -   0, 0,-100,-100   -      1
    //    2.2.2.2:7  -     second 15     -      5 to 15     -   0, 0,-100,-100   -      1

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
      DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4"),
      DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4"),
      DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with another summary vector
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));
    received_sv_header.SetSummaryVector ({DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
                                         DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
                                         DataIdentifier ("1.1.1.1:5")});

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4"),
      DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4"),
      DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");
  }

  void
  TestProcessDisjointVector_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 12.00
    SummaryVectorHeader received_sv_header;
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));

    std::set<DataIdentifier> disjoint_vector, expected_disjoint_vector;

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      1      (EXPIRED)
    //    2.2.2.2:5  -     second 15     -      5 to 15     -   0, 0, 100, 100   -      1
    //    2.2.2.2:6  -     second 10     -      0 to 10     -   0, 0,-100,-100   -      1      (EXPIRED)
    //    2.2.2.2:7  -     second 15     -      5 to 15     -   0, 0,-100,-100   -      1

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
      DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with another summary vector
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));
    received_sv_header.SetSummaryVector ({DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
                                         DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
                                         DataIdentifier ("1.1.1.1:5")});

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");
  }

  void
  TestProcessDisjointVector_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 20.00
    SummaryVectorHeader received_sv_header;
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));

    std::set<DataIdentifier> disjoint_vector, expected_disjoint_vector;

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5      (EXPIRED)
    //    2.2.2.2:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      1      (EXPIRED)
    //    2.2.2.2:5  -     second 15     -      5 to 15     -   0, 0, 100, 100   -      1      (EXPIRED)
    //    2.2.2.2:6  -     second 10     -      0 to 10     -   0, 0,-100,-100   -      1      (EXPIRED)
    //    2.2.2.2:7  -     second 15     -      5 to 15     -   0, 0,-100,-100   -      1      (EXPIRED)

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
      DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with another summary vector
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));
    received_sv_header.SetSummaryVector ({DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
                                         DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
                                         DataIdentifier ("1.1.1.1:5")});

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");
  }

  void
  TestProcessDisjointVector ()
  {
    std::set<DataIdentifier> disjoint_vector, expected_disjoint_vector;
    SummaryVectorHeader received_sv_header;
    m_packets_queue = PacketsQueue (15);

    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));
    received_sv_header.SetSummaryVector ({DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
                                         DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"),
                                         DataIdentifier ("1.1.1.1:5")});

    // Test when packets queue is empty
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test when 1 packet from the summary vector is present in the queue
    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5

    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test when 4 packet from the summary vector is present in the queue
    // New entries expires at second 10
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("1.1.1.1:2"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    data_packet = DataHeader (/* Data ID */ DataIdentifier ("1.1.1.1:3"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    data_packet = DataHeader (/* Data ID */ DataIdentifier ("1.1.1.1:4"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count  -  Allowed area
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 

    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test when the packets queue has some packets not contained in the summary vector
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:1"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (110.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:2"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (125.0, 125.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:3"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 110.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count  -  Allowed area
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,150 
    //    2.2.2.2:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 110,150 
    //    2.2.2.2:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 125,125 
    //    2.2.2.2:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5       -   0,0 150,110 

    // Test with receiver node inside the allowed area of the 3 packets
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with receiver node inside allowed area of packet 2.2.2.2:1
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (105, 130));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with receiver node inside allowed area of packet 2.2.2.2:2
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (120, 120));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:2")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with receiver node inside allowed area of packet 2.2.2.2:3
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (140, 105));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with receiver node inside allowed area of packet 2.2.2.2:1 and 2.2.2.2:2
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (105, 120));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test with receiver node inside allowed area of packet 2.2.2.2:2 and 2.2.2.2:3
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (120, 105));
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:2"), DataIdentifier ("2.2.2.2:3")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Now test packets with only one hop left
    received_sv_header.SetPosition (GeoTemporalLibrary::LibraryUtils::Vector2D (50, 50));

    // Destination node is inside area and temporal scope
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:4"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 1);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    // Destination node is inside area but not temporal scope
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:5"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (5), Seconds (15)),
                                                                       Area (0, 0, 100, 100)),
                              /* Message */ "Message",
                              /* Hops count */ 1);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    // Destination node is inside temporal scope but not area
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:6"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                       Area (0, 0, -100, -100)),
                              /* Message */ "Message",
                              /* Hops count */ 1);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    // Destination node is OUTSIDE area and temporal scope
    data_packet = DataHeader (/* Data ID */ DataIdentifier ("2.2.2.2:7"),
                              /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (150.0, 150.0),
                              /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (5), Seconds (15)),
                                                                       Area (0, 0, -100, -100)),
                              /* Message */ "Message",
                              /* Hops count */ 1);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("2.2.2.2"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Temporal scope  -  Destination area  -  Hops count
    //    1.1.1.1:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    1.1.1.1:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:1  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:2  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:3  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      5
    //    2.2.2.2:4  -     second 10     -      0 to 10     -   0, 0, 100, 100   -      1
    //    2.2.2.2:5  -     second 15     -      5 to 15     -   0, 0, 100, 100   -      1
    //    2.2.2.2:6  -     second 10     -      0 to 10     -   0, 0,-100,-100   -      1
    //    2.2.2.2:7  -     second 15     -      5 to 15     -   0, 0,-100,-100   -      1

    // Destination node 9.9.9.9
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("9.9.9.9"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 1.1.1.1
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("1.1.1.1"),
                                           disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Destination node 2.2.2.2
    m_packets_queue.ProcessDisjointVector (received_sv_header, Ipv4Address ("2.2.2.2"),
                                           disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // The following scheduled calls test:
    //  - That ProcessDisjointVector evaluates current time to decide if deliver or not
    //    packets with 1 hop left.
    //  - That ProcessDisjointVector does not call Purge()
    Simulator::Schedule (Seconds (8), &PacketsQueueTest::TestProcessDisjointVector_Scheduled_1, this);
    Simulator::Schedule (Seconds (12), &PacketsQueueTest::TestProcessDisjointVector_Scheduled_2, this);
    Simulator::Schedule (Seconds (20), &PacketsQueueTest::TestProcessDisjointVector_Scheduled_3, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestFindFunctions_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 6.00
    bool found = false;

    // Some packet entries already expired at second 5.0.
    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 10

    // The entry to be found expired at second 5, so it must not be found.
    found = m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"));
    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.3:3 must not be found.");

    // There should be 2 entries in the queue
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.1:1 must be found.");

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.2:2"));
    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.2:2 must be found.");
  }

  void
  TestFindFunctions ()
  {
    bool found = false;
    PacketQueueEntry entry;
    DataIdentifier data_id;

    m_packets_queue = PacketsQueue (5);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // New entry expires at second 10
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (2), Seconds (10)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2"));

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.4"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 10
    //    1.1.1.3:3  -           second 5
    //    1.1.1.4:4  -           second 5

    // Test bool Find (const DataIdentifier &, PacketQueueEntry &)
    // - Successfully found
    data_id = DataIdentifier (Ipv4Address ("1.1.1.1"), 1);
    found = m_packets_queue.Find (data_id, entry);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.1:1 must be found.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetDataPacketId (), data_id, "Entry must have data ID " << data_id);
    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Packet queue entry must have 10 seconds of expiration time.");

    // - Expected not found
    data_id = DataIdentifier (Ipv4Address ("1.1.1.1"), 2);
    found = m_packets_queue.Find (data_id, entry);

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.1:2 must not be found.");

    // Test bool Find (const DataIdentifier &);
    // - Successfully found
    data_id = DataIdentifier (Ipv4Address ("1.1.1.2"), 2);
    found = m_packets_queue.Find (data_id);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.2:2 must be found.");

    // - Expected not found
    data_id = DataIdentifier (Ipv4Address ("1.1.1.2"), 1);
    found = m_packets_queue.Find (data_id);

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.2:1 must not be found.");

    // Test bool Find (const PacketQueueEntry &);
    // - Successfully found
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    found = m_packets_queue.Find (PacketQueueEntry (data_packet));

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.3:3 must be found.");

    // - Expected not found
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:1"));
    found = m_packets_queue.Find (PacketQueueEntry (data_packet));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.4:1 must not be found.");

    // The following scheduled calls test:
    //  - That Find calls Purge()
    Simulator::Schedule (Seconds (6), &PacketsQueueTest::TestFindFunctions_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestEnqueueFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 16.00
    bool enqueued;
    PacketQueueEntry entry;

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -            Expired
    //    1.1.1.2:2  -            Expired
    //    1.1.1.5:5  -           second 17

    // The following entry to be inserted used to exist but was previously 
    // dropped, so it is inserted as newly inserted.
    // Successful insertion of new entry that expires at second 20
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.3:3"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.5:5  -           second 17
    //    1.1.1.3:3  -           second 20

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.3:3 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 5u,
                           "Size of the received packets statistics must be 5.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (4), MicroSeconds (1),
                               "Packet queue entry 1.1.1.3:3 expiration time must be 4 seconds.");
  }

  void
  TestEnqueueFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 25.00
    // At this point in time all entries must have expired.

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 5u,
                           "Size of the received packets statistics must be 5.");

    const std::map<DataIdentifier, DataPacketReceptionStats> & queue_stats
            = m_packets_queue.GetPacketReceptionStats ();

    std::map<DataIdentifier, DataPacketReceptionStats>::const_iterator stats_it;

    // Check that Enqueue () logs the expected statistics (when: a new packet arrives,
    // a duplicated packet arrives, a packet is dropped).

    // Check statistics about packet 1.1.1.1:1
    stats_it = queue_stats.find (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.1:1 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 1u,
                           "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Check statistics about packet 1.1.1.2:2
    stats_it = queue_stats.find (DataIdentifier ("1.1.1.2:2"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.2:2 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.2:2"),
                           "Must be 1.1.1.2:2");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.2"),
                           "Must be 1.1.1.2");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 2u,
                           "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Check statistics about packet 1.1.1.3:3
    stats_it = queue_stats.find (DataIdentifier ("1.1.1.3:3"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.3:3 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.3:3"),
                           "Must be 1.1.1.3:3");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (16), MicroSeconds (1),
                               "Must be second 16");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.3"),
                           "Must be 1.1.1.3");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Check statistics about packet 1.1.1.4:4
    stats_it = queue_stats.find (DataIdentifier ("1.1.1.4:4"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.4:4 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.4:4"),
                           "Must be 1.1.1.4:4");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.4"),
                           "Must be 1.1.1.4");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Check statistics about packet 1.1.1.5:5
    stats_it = queue_stats.find (DataIdentifier ("1.1.1.5:5"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.5:5 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.5:5"),
                           "Must be 1.1.1.5:5");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.5"),
                           "Must be 1.1.1.5");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");
  }

  void
  TestEnqueueFunction ()
  {
    m_packets_queue = PacketsQueue (3);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().empty (), true,
                           "Size of the received packets statistics must be empty.");

    bool enqueued, found;
    PacketQueueEntry entry;

    // Successful insertion of new entry that expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 5);
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.1:1 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u, "Size of the packets queue must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 1u,
                           "Size of the received packets statistics must be 1.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Packet queue entry 1.1.1.1:1 expiration time must be 10 seconds.");

    // Expected failure of already inserted entry
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (enqueued, false, "Packet queue entry 1.1.1.1:1 must have not been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u, "Size of the packets queue must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 1u,
                           "Size of the received packets statistics must be 1.");

    // Successful insertion of new entry that expires at second 15
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (3), Seconds (15)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 15

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.2:2 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 2u,
                           "Size of the received packets statistics must be 2.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.2:2"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (15), MicroSeconds (1),
                               "Packet queue entry 1.1.1.2:2 expiration time must be 15 seconds.");

    // Expected failure of already inserted entry
    // We execute twice the Enqueue call to count 2 received duplicates.
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2"));
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2")); // Not a accidentally duplicated line

    NS_TEST_EXPECT_MSG_EQ (enqueued, false, "Packet queue entry 1.1.1.2:2 must have not been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 2u,
                           "Size of the received packets statistics must be 2.");

    // Successful insertion of new entry that expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 15
    //    1.1.1.3:3  -           second 5

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.3:3 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 3u,
                           "Size of the received packets statistics must be 3.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Packet queue entry 1.1.1.3:3 expiration time must be 5 seconds.");

    // Test that when the queue is full the packet with minimum expiration time 
    // is dropped and the new entry is successfully inserted.
    // This new packet expires at second 9.
    // Packet 1.1.1.3:3 is dropped.
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (9)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.4"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 15
    //    1.1.1.4:4  -           second 09

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.4:4 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 4u,
                           "Size of the received packets statistics must be 4.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.4:4"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (9), MicroSeconds (1),
                               "Packet queue entry 1.1.1.4:4 expiration time must be 9 seconds.");

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.3:3 must not be found.");

    // Insert another packet entry into the already full queue.
    // This new packet expires at second 17.
    // Packet 1.1.1.4:4 is dropped.
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.5:5"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (17)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.5"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 15
    //    1.1.1.5:5  -           second 17

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.5:5 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 5u,
                           "Size of the received packets statistics must be 5.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.5:5"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (17), MicroSeconds (1),
                               "Packet queue entry 1.1.1.5:5 expiration time must be 17 seconds.");

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.4:4"));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.4:4 must not be found.");

    // The following scheduled call test:
    //  - That Enqueue calls Purge()
    Simulator::Schedule (Seconds (16), &PacketsQueueTest::TestEnqueueFunction_Scheduled_1, this);

    // The following scheduled call test:
    //  - That Enqueue logs the statistics
    Simulator::Schedule (Seconds (20), &PacketsQueueTest::TestEnqueueFunction_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestStatistics_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 25.00
    m_packets_queue.LogNewPacketReceived (DataIdentifier ("1.1.1.2:2"), Ipv4Address ("1.1.1.2"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 2u,
                           "Size of the received packets statistics must be 2.");

    const std::map<DataIdentifier, DataPacketReceptionStats> & queue_stats
            = m_packets_queue.GetPacketReceptionStats ();

    std::map<DataIdentifier, DataPacketReceptionStats>::const_iterator stats_it
            = queue_stats.find (DataIdentifier ("1.1.1.2:2"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.2:2 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.2:2"),
                           "Must be 1.1.1.2:2");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (25), MicroSeconds (1),
                               "Must be second 25");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.2"),
                           "Must be 1.1.1.2");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");
  }

  void
  TestStatistics ()
  {
    m_packets_queue = PacketsQueue (1u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 0u,
                           "Size of the received packets statistics must be 0.");

    // Test the logging of non-existing data packets does nothing
    m_packets_queue.LogPacketTransmitted (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 0u,
                           "Size of the received packets statistics must be 0.");

    m_packets_queue.LogDuplicatedPacketReceived (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 0u,
                           "Size of the received packets statistics must be 0.");

    m_packets_queue.LogPacketDropped (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 0u,
                           "Size of the received packets statistics must be 0.");

    // Test log new packet received
    m_packets_queue.LogNewPacketReceived (DataIdentifier ("1.1.1.1:1"), Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 1u,
                           "Size of the received packets statistics must be 1.");

    const std::map<DataIdentifier, DataPacketReceptionStats> & queue_stats
            = m_packets_queue.GetPacketReceptionStats ();

    std::map<DataIdentifier, DataPacketReceptionStats>::const_iterator stats_it
            = queue_stats.find (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ ((stats_it != queue_stats.end ()), true,
                           "Stats of packet 1.1.1.1:1 must be found.");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Test log duplicated packet received
    for (uint32_t i = 0u; i < 13u; ++i)
      {
        m_packets_queue.LogDuplicatedPacketReceived (DataIdentifier ("1.1.1.1:1"));
      }

    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 13u,
                           "Must be 13");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    // Test log packet transmitted

    for (uint32_t i = 0u; i < 17u; ++i)
      {
        m_packets_queue.LogPacketTransmitted (DataIdentifier ("1.1.1.1:1"));
      }

    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 13u,
                           "Must be 13");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 17u,
                           "Must be 17");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 17u,
                           "Must be 17");

    // Test log packets dropped
    m_packets_queue.LogPacketDropped (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 13u,
                           "Must be 13");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 17u,
                           "Must be 17");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 17u,
                           "Must be 17");

    for (uint32_t i = 0u; i < 7u; ++i)
      {
        m_packets_queue.LogPacketDropped (DataIdentifier ("1.1.1.1:1"));
      }

    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.1"),
                           "Must be 1.1.1.1");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 13u,
                           "Must be 13");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 17u,
                           "Must be 17");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 17u,
                           "Must be 17");

    // Test that data packet reception stats object is overwritten correctly
    {
      char buffer[25];
      for (uint32_t i = 0u; i < 23u; ++i)
        {
          std::sprintf (buffer, "1.1.1.%u", (i + 100u));
          m_packets_queue.LogNewPacketReceived (DataIdentifier ("1.1.1.1:1"), Ipv4Address (buffer));
        }
    }

    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDataIdentifier (), DataIdentifier ("1.1.1.1:1"),
                           "Must be 1.1.1.1:1");
    NS_TEST_EXPECT_MSG_EQ_TOL (stats_it->second.GetReceptionTime (), Seconds (0), MicroSeconds (1),
                               "Must be second 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmitterIpAddress (), Ipv4Address ("1.1.1.122"),
                           "Must be 1.1.1.23");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDestinedToReceiverNode (), true,
                           "Must be true");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), false,
                           "Must be false");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetReceivedDuplicatesCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetBroadcastTransmittedReplicasCount (), 0u,
                           "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetUnicastTransmittedReplicasCount (), 0u,
                           "Must be 0");

    Simulator::Schedule (Seconds (25), &PacketsQueueTest::TestStatistics_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestToStringFunction ()
  {
    char buffer[100];

    m_packets_queue = PacketsQueue (1u);

    std::string expected_str = "Packets queue has 0 / 1 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    m_packets_queue.SetMaxLength (17u);

    expected_str = "Packets queue has 0 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Creation coordinates */ GeoTemporalLibrary::LibraryUtils::Vector2D (-75.62, 85.62),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Hops */ 1u);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    expected_str = "Packets queue has 1 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    for (uint32_t i = 0u; i < 16u; ++i)
      {
        std::sprintf (buffer, "1.1.2.%u:%u", i, i);
        data_packet.SetDataIdentifier (DataIdentifier (std::string (buffer)));

        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

        std::sprintf (buffer, "Packets queue has %u / 17 packet entries", (i + 2));
        expected_str = std::string (buffer);
        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                               "Expected string: " + expected_str);
      }

    for (uint32_t i = 0u; i < 100u; ++i)
      {
        std::sprintf (buffer, "1.1.3.%u:%u", i, i);
        data_packet.SetDataIdentifier (DataIdentifier (std::string (buffer)));

        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.3"));
      }

    expected_str = "Packets queue has 17 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestGetSize ();
    TestGetSummaryVector ();
    TestClear ();
    TestProcessDisjointVector ();
    TestFindFunctions ();
    TestEnqueueFunction ();
    TestStatistics ();
    TestToStringFunction ();
  }
};


// =============================================================================
//                         DuplicatedPacketDetectorTest
// =============================================================================

/**
 * DuplicatedPacketDetector for the detector of duplicated data packets test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class DuplicatedPacketDetectorTest : public TestCasePlus
{
public:

  DuplicatedPacketDetector m_detector;

  DuplicatedPacketDetectorTest () : TestCasePlus ("DuplicatedPacketDetector"), m_detector () { }

  void
  TestConstructors ()
  {
    // Default constructor

    DuplicatedPacketDetector d1;

    NS_TEST_EXPECT_MSG_EQ_TOL (d1.GetExpirationTime (), Seconds (5), MicroSeconds (1), "Must be 5 seconds");
    NS_TEST_EXPECT_MSG_EQ (d1.Size (), 0u, "Must be 0.");

    DuplicatedPacketDetector d2 (Days (7));

    NS_TEST_EXPECT_MSG_EQ_TOL (d2.GetExpirationTime (), Days (7), MicroSeconds (1), "Must be 7 days");
    NS_TEST_EXPECT_MSG_EQ (d2.Size (), 0u, "Must be 0.");

    DuplicatedPacketDetector d3 (d2);

    NS_TEST_EXPECT_MSG_EQ_TOL (d3.GetExpirationTime (), Days (7), MicroSeconds (1), "Must be 7 days");
    NS_TEST_EXPECT_MSG_EQ (d3.Size (), 0u, "Must be 0.");
  }

  void
  TestGettersSetters ()
  {
    m_detector = DuplicatedPacketDetector (Seconds (13.6));

    NS_TEST_EXPECT_MSG_EQ_TOL (m_detector.GetExpirationTime (), Seconds (13.6), MicroSeconds (1), "Must be 13.6 seconds");

    m_detector.SetExpirationTime (Days (97.8));

    NS_TEST_EXPECT_MSG_EQ_TOL (m_detector.GetExpirationTime (), Days (97.8), MicroSeconds (1), "Must be 97.8 days");
  }

  void
  Scheduled_check_1 ()
  {
    // This function is launched by the scheduler at second 8.00

    // Cache looks like this:
    //   <IP - ID> tuple        Expiration time (in seconds)
    //   1.1.1.1 - 1                       10
    //   1.1.1.1 - 2                       10
    //   2.2.2.2 - 1                       10
    //   3.3.3.3 - 3                       15
    //   4.4.4.4 - 4                       15
    //   5.5.5.5 - 5                       15
    //   6.6.6.6 - 6                       15

    NS_TEST_EXPECT_MSG_EQ (m_detector.Size (), 7, "Expected 7 entries");

    m_detector.SetExpirationTime (Seconds (5));

    m_detector.IsDuplicate (Ipv4Address ("7.7.7.7"), 7u);

    // Cache looks like this:
    //   <IP - ID> tuple        Expiration time (in seconds)
    //   1.1.1.1 - 1                       10
    //   1.1.1.1 - 2                       10
    //   2.2.2.2 - 1                       10
    //   3.3.3.3 - 3                       15
    //   4.4.4.4 - 4                       15
    //   5.5.5.5 - 5                       15
    //   6.6.6.6 - 6                       15
    //   7.7.7.7 - 7                       13
  }

  void
  Scheduled_check_2 ()
  {
    // This function is launched by the scheduler at second 12.00

    // Cache looks like this:
    //   <IP - ID> tuple        Expiration time (in seconds)
    //   3.3.3.3 - 3                       15
    //   4.4.4.4 - 4                       15
    //   5.5.5.5 - 5                       15
    //   6.6.6.6 - 6                       15
    //   7.7.7.7 - 7                       13

    NS_TEST_EXPECT_MSG_EQ (m_detector.Size (), 5, "Expected 5 entries");
  }

  void
  Scheduled_check_3 ()
  {
    // This function is launched by the scheduler at second 20.00

    // Cache looks like this:
    //   <IP - ID> tuple        Expiration time (in seconds)

    NS_TEST_EXPECT_MSG_EQ (m_detector.Size (), 0, "Expected 0 entries");
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();

    m_detector = DuplicatedPacketDetector (Seconds (10));

    // Test IsDuplicate ()
    NS_TEST_EXPECT_MSG_EQ (m_detector.IsDuplicate (Ipv4Address ("1.1.1.1"), 1u),
                           false, "Unknown IP and ID");
    NS_TEST_EXPECT_MSG_EQ (m_detector.IsDuplicate (Ipv4Address ("1.1.1.1"), 2u),
                           false, "Unknown ID");
    NS_TEST_EXPECT_MSG_EQ (m_detector.IsDuplicate (Ipv4Address ("2.2.2.2"), 1u),
                           false, "Unknown IP");

    NS_TEST_EXPECT_MSG_EQ (m_detector.IsDuplicate (Ipv4Address ("1.1.1.1"), 1u),
                           true, "Known IP and ID");

    m_detector.SetExpirationTime (Seconds (15));

    m_detector.IsDuplicate (Ipv4Address ("3.3.3.3"), 3u);
    m_detector.IsDuplicate (Ipv4Address ("4.4.4.4"), 4u);
    m_detector.IsDuplicate (Ipv4Address ("5.5.5.5"), 5u);
    m_detector.IsDuplicate (Ipv4Address ("6.6.6.6"), 6u);

    NS_TEST_EXPECT_MSG_EQ (m_detector.Size (), 7, "Expected 7 entries");

    // Cache looks like this:
    //   <IP - ID> tuple        Expiration time (in seconds)
    //   1.1.1.1 - 1                       10
    //   1.1.1.1 - 2                       10
    //   2.2.2.2 - 1                       10
    //   3.3.3.3 - 3                       15
    //   4.4.4.4 - 4                       15
    //   5.5.5.5 - 5                       15
    //   6.6.6.6 - 6                       15

    // The following scheduled calls test:
    //  - That Size () calls Purge()
    Simulator::Schedule (Seconds (8), &DuplicatedPacketDetectorTest::Scheduled_check_1, this);
    Simulator::Schedule (Seconds (12), &DuplicatedPacketDetectorTest::Scheduled_check_2, this);
    Simulator::Schedule (Seconds (20), &DuplicatedPacketDetectorTest::Scheduled_check_3, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }
};


// =============================================================================
//                    GeoTemporalRestrictedEpidemicTestSuite
// =============================================================================

/**
 * Geo-Temporal Restricted Epidemic test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-restricted-epidemic-test
 */
class GeoTemporalRestrictedEpidemicTestSuite : public TestSuite
{
public:

  GeoTemporalRestrictedEpidemicTestSuite () : TestSuite ("geotemporal-restricted-epidemic", UNIT)
  {
    AddTestCase (new PacketTypeTest, TestCase::QUICK);
    AddTestCase (new TypeHeaderTest, TestCase::QUICK);
    AddTestCase (new HelloHeaderTest, TestCase::QUICK);
    AddTestCase (new SummaryVectorHeaderTest, TestCase::QUICK);
    AddTestCase (new DataHeaderTest, TestCase::QUICK);
    AddTestCase (new NeighborEntryTest, TestCase::QUICK);
    AddTestCase (new NeighborsTableTest, TestCase::QUICK);
    AddTestCase (new PacketQueueEntryTest, TestCase::QUICK);
    AddTestCase (new PacketsQueueTest, TestCase::QUICK);
    AddTestCase (new DuplicatedPacketDetectorTest, TestCase::QUICK);
  }
};

static GeoTemporalRestrictedEpidemicTestSuite g_geotemporal_restricted_epidemic_test_suite;


} // namespace test
} // namespace geotemporal_restricted_epidemic
} // namespace ns3
