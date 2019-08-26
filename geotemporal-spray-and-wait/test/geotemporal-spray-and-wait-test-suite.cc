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

#include <ns3/geotemporal-spray-and-wait-module.h>

#include <ns3/geotemporal-utils.h>
#include <ns3/math-utils.h>
#include <ns3/packet.h>
#include <ns3/packet-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;


namespace ns3
{
namespace geotemporal_spray_and_wait
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
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_1, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_1, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_1, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_1, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_1, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_1, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_2, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_2, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (equal_2, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_2, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_2, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (equal_2, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (different, equal_1), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (different, equal_2), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator== (different, different), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (different, equal_1), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (different, equal_2), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal_spray_and_wait::operator!= (different, different), false, "Must not be different");
  }
};


// =============================================================================
//                                PacketTypeTest
// =============================================================================

/**
 * PacketType enumeration test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-spray-and-wait-test
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
 * \ingroup geotemporal-spray-and-wait-test
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
 * \ingroup geotemporal-spray-and-wait-test
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
//                                ReplyHeaderTest
// =============================================================================

/**
 * ReplyHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-spray-and-wait-test
 */
class ReplyHeaderTest : public TestCasePlus
{
public:

  ReplyHeaderTest () : TestCasePlus ("ReplyHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    ReplyHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVectorSize (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVector (), std::set<DataIdentifier> (), "Must be an empty set");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 3u, "Must be 3u");

    // Parameters constructor
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    ReplyHeader h2 (summary_vector);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 21u, "Must be 21u");

    // Copy constructor
    ReplyHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 21u, "Must be 21u");
  }

  void
  TestGettersSetters ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    ReplyHeader header (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 3u, "Summary vector size must be 3");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");

    summary_vector.insert ({DataIdentifier (Ipv4Address ("1.1.1.1"), 4),
                           DataIdentifier (Ipv4Address ("1.1.1.1"), 5)});
    header.SetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
  }

  void
  TestOverloadedOperators ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    ReplyHeader equal_1 (summary_vector);
    ReplyHeader equal_2 (summary_vector);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});

    ReplyHeader different (summary_vector);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    ReplyHeader h1;
    ReplyHeader h2 (summary_vector);
    ReplyHeader h3 (h2);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});

    h2 = ReplyHeader (summary_vector);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    ReplyHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 3u + 3u, "ReplyHeader is 21 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 1u + 3u, "ReplyHeader is 9 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 0u + 3u, "ReplyHeader is 3 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    ReplyHeader h (summary_vector);
    std::string expected_str = "REPLY with 3 entries: 1.1.1.1:1 1.1.1.1:2 1.1.1.1:3 ";
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
//                                ReplyBackHeaderTest
// =============================================================================

/**
 * ReplyBackHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-spray-and-wait-test
 */
class ReplyBackHeaderTest : public TestCasePlus
{
public:

  ReplyBackHeaderTest () : TestCasePlus ("ReplyBackHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    ReplyBackHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVectorSize (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSummaryVector (), std::set<DataIdentifier> (), "Must be an empty set");
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 19u, "Must be 19u");

    // Parameters constructor
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    ReplyBackHeader h2 (summary_vector, position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 37u, "Must be 37u");

    // Copy constructor
    ReplyBackHeader h3 (h2);
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

    ReplyBackHeader header (summary_vector, position);

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

    ReplyBackHeader equal_1 (summary_vector, position);
    ReplyBackHeader equal_2 (summary_vector, position);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});

    ReplyBackHeader different (summary_vector, position);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 3)});

    equal_1 = ReplyBackHeader (summary_vector, position);
    equal_2 = ReplyBackHeader (summary_vector, position);

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (99, 299);

    different = ReplyBackHeader (summary_vector, position);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);

    ReplyBackHeader h1;
    ReplyBackHeader h2 (summary_vector, position);
    ReplyBackHeader h3 (h2);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (-54, 23);

    h2 = ReplyBackHeader (summary_vector, position);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    ReplyBackHeader deserialized;

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

    ReplyBackHeader h (summary_vector, position);
    std::string expected_str = "REPLY_BACK sent from position (3.00, 4.00) "
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
 * \ingroup geotemporal-spray-and-wait-test
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
    NS_TEST_EXPECT_MSG_EQ (h1.GetDestinationGeoTemporalArea (), GeoTemporalArea (), "Must be default GeoTemporalArea");
    NS_TEST_EXPECT_MSG_EQ (h1.GetMessage (), "", "Must be empty string");
    NS_TEST_EXPECT_MSG_EQ (h1.GetReplicasToForward (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 48u, "Must be 48u");

    // Parameters constructor
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader h2 (data_id, gta, message, replicas);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetReplicasToForward (), replicas, "Must be " << replicas);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 48u + 16u, "Must be 64u");

    // Copy constructor
    DataHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h3.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h3.GetReplicasToForward (), replicas, "Must be " << replicas);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 48u + 16u, "Must be 64u");
  }

  void
  TestGettersSetters ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader header (data_id, gta, message, replicas);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetReplicasToForward (), replicas, "Must be " << replicas);

    data_id = DataIdentifier (Ipv4Address ("9.8.7.6"), 5u);
    header.SetDataIdentifier (data_id);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetReplicasToForward (), replicas, "Must be " << replicas);

    gta = GeoTemporalArea (TimePeriod (MilliSeconds (50), MilliSeconds (60)), Area (30, 40, 50, 60));
    header.SetDestinationGeoTemporalArea (gta);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetReplicasToForward (), replicas, "Must be " << replicas);

    message = "new packet's message :)";
    header.SetMessage (message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetReplicasToForward (), replicas, "Must be " << replicas);

    replicas = 91u;
    header.SetReplicasToForward (replicas);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetReplicasToForward (), replicas, "Must be " << replicas);
  }

  void
  TestOverloadedOperators ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader equal_1 (data_id, gta, message, replicas);
    DataHeader equal_2 (data_id, gta, message, replicas);
    DataHeader different (DataIdentifier (Ipv4Address ("1.2.3.4"), 99u), gta, message, replicas);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (0, 0, 100, 100)),
                            message, replicas);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, gta, "Different message", replicas);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, gta, message, 1);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader h1;
    DataHeader h2 (data_id, gta, message, replicas);
    DataHeader h3 (h2);

    h2 = DataHeader (data_id, gta, "Different message", 2u); // Message length = 17

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    DataHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 64u, "DataHeader is 64 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 65u, "DataHeader is 65 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 48u, "DataHeader is 48 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    DataIdentifier data_id (Ipv4Address ("1.2.3.4"), 5u);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader h (data_id, gta, message, replicas);
    std::string expected_str = "DATA 1.2.3.4:5 (5 replicas) destined to area "
            "{(30.00, 40.00), (50.00, 60.00)} to start at second "
            "10.00 with a duration of 10.00 seconds has a message of 16 byte(s)";
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
//                               NeighborEntryTest
// =============================================================================

/**
 * NeighborEntry for the neighbors table test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-spray-and-wait-test
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
 * \ingroup geotemporal-spray-and-wait-test
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
 * \ingroup geotemporal-spray-and-wait-test
 */
class PacketQueueEntryTest : public TestCasePlus
{
public:

  const DataIdentifier m_data_id;
  const GeoTemporalArea m_geo_temporal_area;
  const std::string m_message;
  const uint32_t m_replicas;
  const DataHeader m_header;

  PacketQueueEntry m_queue_entry;

  PacketQueueEntryTest ()
  : TestCasePlus ("PacketQueueEntry"),
  m_data_id (Ipv4Address ("1.1.1.1"), 1u),
  m_geo_temporal_area (TimePeriod (Seconds (34), Seconds (74)), Area (10, 10, 90, 90)),
  m_message ("0123456789"),
  m_replicas (15u),
  m_header (m_data_id, m_geo_temporal_area, m_message, m_replicas),
  m_queue_entry () { }

  void
  TestConstructors ()
  {
    // Default constructor
    PacketQueueEntry e1;

    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacketId (), DataIdentifier (), "Must be the default DataIdentifier.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacket (), DataHeader (), "Must be the default DataHeader.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetExpirationTime (), Time (), "Must be the default Time.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetReplicasCounter (), 0u, "Must be zero.");

    // Parameters constructor
    PacketQueueEntry e2 (m_header);

    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ_TOL (e2.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");
    NS_TEST_EXPECT_MSG_EQ (e2.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);

    // Copy constructor
    PacketQueueEntry e3 (e2);

    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ_TOL (e3.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");
    NS_TEST_EXPECT_MSG_EQ (e3.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);
  }

  void
  TestGetSetReplicasCounter ()
  {
    PacketQueueEntry entry (m_header);

    NS_TEST_EXPECT_MSG_EQ (entry.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);

    entry.SetReplicasCounter (497u);

    NS_TEST_EXPECT_MSG_EQ (entry.GetReplicasCounter (), 497u, "Must be 497");
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
    diff_header.SetReplicasToForward (8u);
    PacketQueueEntry different (diff_header);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different expiration time
    equal_1 = PacketQueueEntry (m_header);
    equal_2 = PacketQueueEntry (m_header);
    different = PacketQueueEntry (m_header);

    different.SetExpirationTime (Seconds (400));

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different replicas counter
    equal_1 = PacketQueueEntry (m_header);
    equal_2 = PacketQueueEntry (m_header);
    different = PacketQueueEntry (m_header);

    different.SetReplicasCounter (999u);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGetSetReplicasCounter ();
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
 * \ingroup geotemporal-spray-and-wait-test
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

    NS_TEST_EXPECT_MSG_EQ (q1.GetBinaryMode (), false, "Must be false.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetMaxLength (), 128u, "Must be 128.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q1.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q1.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Parameters constructor
    PacketsQueue q2 (true, 17u);
    q2.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q2.GetBinaryMode (), true, "Must be true.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetMaxLength (), 17u, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q2.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q2.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Copy constructor
    PacketsQueue q3 (q2);
    q3.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q3.GetBinaryMode (), true, "Must be true.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetMaxLength (), 17u, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q3.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q3.GetPacketReceptionStats ().empty (), true, "Must be empty.");
  }

  void
  TestGettersSetters ()
  {
    m_packets_queue = PacketsQueue (false, 67);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetBinaryMode (), false, "Must be false.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67u, "Must be 67.");

    m_packets_queue.SetBinaryMode (true);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetBinaryMode (), true, "Must be true.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67u, "Must be 67.");

    m_packets_queue.SetMaxLength (11u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetBinaryMode (), true, "Must be true.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 11u, "Must be 11.");

    m_packets_queue.SetMaxLength (1u);

    Ipv4Address source_ip = Ipv4Address ("1.1.1.1");

    DataHeader data_packet (/* Data ID */ DataIdentifier (source_ip, 0u),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
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
    m_packets_queue = PacketsQueue (false, 5u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Size of the packets queue must be 0.");

    Ipv4Address source_ip = Ipv4Address ("1.1.1.1");
    DataHeader data_packet (/* Data ID */ DataIdentifier (source_ip, 0u),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);

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
  TestGetSummaryVector ()
  {
    std::set<DataIdentifier> summary_vector, expected_summary_vector;
    m_packets_queue = PacketsQueue (false, 5);

    // Test when packets queue is empty
    m_packets_queue.GetSummaryVector (summary_vector);
    NS_TEST_EXPECT_MSG_EQ (summary_vector.size (), 0u, "Summary vector must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
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
    // This function is launched by the scheduler at second 20.00
    std::set<DataIdentifier> received_summary_vector, disjoint_vector, expected_disjoint_vector;

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     1. 1. 1. 1: 1  -           second 10
    //     9. 9. 9. 9: 9  -           second 5
    //    10.10.10.10:10  -           second 7
    //    11.11.11.11:11  -           second 9
    //     1. 1. 1. 1: 3  -           second 3
    //     1. 1. 1. 1: 2  -           second 8
    //     1. 1. 1. 1: 4  -           second 11
    //     1. 1. 1. 1: 5  -           second 10

    // All entries are expired and a call to Purge () would remove them from the queue.

    // If we call ProcessDisjointVector () when the packets queue contains all packets
    // contained in the summary vector, even when this packets are expired, it should
    // return an empty disjoint vector.

    received_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"), DataIdentifier ("1.1.1.1:5")};
    expected_disjoint_vector.clear ();

    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");
  }

  void
  TestProcessDisjointVector ()
  {
    std::set<DataIdentifier> received_summary_vector, disjoint_vector, expected_disjoint_vector;
    m_packets_queue = PacketsQueue (false, 15);

    received_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4"), DataIdentifier ("1.1.1.1:5")};

    // Test when packets queue is empty
    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    expected_disjoint_vector = std::set<DataIdentifier> (received_summary_vector);
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Test when 1 packet from the summary vector is present in the queue
    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10

    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:3"),
      DataIdentifier ("1.1.1.1:4"), DataIdentifier ("1.1.1.1:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // Add packets that won't be contained in the summary vector to the packets queue
    {
      // This new packet expires at second 5
      DataHeader data_packet_1 (/* Data ID */ DataIdentifier ("9.9.9.9:9"),
                                /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                         Area (0, 0, 100, 100)),
                                /* Message */ "Message",
                                /* Replicas */ 5);
      m_packets_queue.Enqueue (data_packet_1, Ipv4Address ("9.9.9.9"));

      // This new packet expires at second 7
      DataHeader data_packet_2 (/* Data ID */ DataIdentifier ("10.10.10.10:10"),
                                /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (7)),
                                                                         Area (0, 0, 100, 100)),
                                /* Message */ "Message",
                                /* Replicas */ 5);
      m_packets_queue.Enqueue (data_packet_2, Ipv4Address ("10.10.10.10"));

      // This new packet expires at second 9
      DataHeader data_packet_3 (/* Data ID */ DataIdentifier ("11.11.11.11:11"),
                                /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (9)),
                                                                         Area (0, 0, 100, 100)),
                                /* Message */ "Message",
                                /* Replicas */ 5);
      m_packets_queue.Enqueue (data_packet_3, Ipv4Address ("11.11.11.11"));
    }

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     1. 1. 1. 1: 1  -           second 10
    //     9. 9. 9. 9: 9  -           second 5
    //    10.10.10.10:10  -           second 7
    //    11.11.11.11:11  -           second 9

    // Test when 1 packet from the summary vector is present in the queue among 
    // other packets not included in the summary vector
    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:3"),
      DataIdentifier ("1.1.1.1:4"), DataIdentifier ("1.1.1.1:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // New entry expires at second 3
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.1:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (3)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     1. 1. 1. 1: 1  -           second 10
    //     9. 9. 9. 9: 9  -           second 5
    //    10.10.10.10:10  -           second 7
    //    11.11.11.11:11  -           second 9
    //     1. 1. 1. 1: 3  -           second 3

    // Test when 2 packets from the summary vector are present in the queue among 
    // other packets not included in the summary vector
    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    expected_disjoint_vector = {DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:4"),
      DataIdentifier ("1.1.1.1:5")};
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // New entry expires at second 8
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.1:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (8)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // New entry expires at second 11
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.1:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (11)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // New entry expires at second 10
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.1:5"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (10)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     1. 1. 1. 1: 1  -           second 10
    //     9. 9. 9. 9: 9  -           second 5
    //    10.10.10.10:10  -           second 7
    //    11.11.11.11:11  -           second 9
    //     1. 1. 1. 1: 3  -           second 3
    //     1. 1. 1. 1: 2  -           second 8
    //     1. 1. 1. 1: 4  -           second 11
    //     1. 1. 1. 1: 5  -           second 10

    // Test when all packets from the summary vector are present in the queue among 
    // other packets not included in the summary vector
    m_packets_queue.ProcessDisjointVector (received_summary_vector, disjoint_vector);

    expected_disjoint_vector.clear ();
    NS_TEST_EXPECT_MSG_EQ (disjoint_vector, expected_disjoint_vector, "Disjoint vector must be the expected.");

    // The following scheduled calls test:
    //  - That ProcessDisjointVector does not call Purge()
    Simulator::Schedule (Seconds (20), &PacketsQueueTest::TestProcessDisjointVector_Scheduled_1, this);

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

    m_packets_queue = PacketsQueue (false, 5);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
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
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
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
    m_packets_queue = PacketsQueue (false, 3);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().empty (), true,
                           "Size of the received packets statistics must be empty.");

    bool enqueued, found;
    PacketQueueEntry entry;

    // Successful insertion of new entry that expires at second 10
    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 5);
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
    enqueued = m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.2")); // Not an accidentally duplicated line

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
  TestDiscountPacketReplicasToForward_NormalMode ()
  {
    bool discounted = true; // This value must be modified
    uint32_t replicas = 98761u; // We test that this value is not modified.
    PacketQueueEntry packet_entry;

    // Test that it returns false for a non-existent data packet entry
    m_packets_queue = PacketsQueue (false, 1u);
    discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

    NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
    NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");

    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 1u);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
    NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u, "Packet entry must have 1 replica(s).");

    // Test that packets with only 1 replica remaining don't get its replicas discounted
    replicas = 98761u; // We test that this value is not modified.
    discounted = true; // This value must be modified

    discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

    NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
    NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
    NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u, "Packet entry must have 1 replica(s).");

    // Test that only 1 replica is discounted at a time, until there are no more
    // replicas available
    for (uint32_t starting_replicas = 2u; starting_replicas < 20u; ++starting_replicas)
      {
        m_packets_queue = PacketsQueue (false, 1u);

        data_packet.SetReplicasToForward (starting_replicas);
        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

        m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
        NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), starting_replicas,
                               "Packet entry must have " << starting_replicas << " replica(s).");

        // 'starting_replicas' - 1 replicas must be discounted
        for (uint8_t i = 0u; i < starting_replicas - 1u; ++i)
          {
            replicas = 98761u; // This value must be modified.
            discounted = false; // This value must be modified

            discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

            NS_TEST_EXPECT_MSG_EQ (discounted, true, "Replicas must be discounted.");
            NS_TEST_EXPECT_MSG_EQ (replicas, 1u, "Replicas to forward must be 1.");

            m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
            NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), starting_replicas - 1u - i,
                                   "Packet entry must have " << starting_replicas - 1u - i << " replica(s).");
          }

        // No more replicas can be discounted
        for (uint8_t i = 0u; i < 5u; ++i)
          {
            replicas = 98761u; // This value must not be modified.
            discounted = true; // This value must be modified

            discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

            NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
            NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");
          }

        m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
        NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u,
                               "Packet entry must have 1 replica(s).");
      }
  }

  void
  TestDiscountPacketReplicasToForward_BinaryMode ()
  {
    bool discounted = true; // This value must be modified
    uint32_t replicas = 98761u; // We test that this value is not modified.
    PacketQueueEntry packet_entry;

    // Test that it returns false for a non-existent data packet entry
    m_packets_queue = PacketsQueue (false, 1u);
    discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

    NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
    NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");

    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 1u);
    m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
    NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u, "Packet entry must have 1 replica(s).");

    // Test that packets with only 1 replica remaining don't get its replicas discounted
    replicas = 98761u; // We test that this value is not modified.
    discounted = true; // This value must be modified

    discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

    NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
    NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
    NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u, "Packet entry must have 1 replica(s).");

    // Test that only 1 replica is discounted at a time, until there are no more
    // replicas available
    uint8_t discounts_counter;
    uint32_t expected_replicas_to_keep, expected_replicas_to_forward;

    for (uint32_t starting_replicas = 2u; starting_replicas <= 128u; ++starting_replicas)
      {
        m_packets_queue = PacketsQueue (true, 1u);

        data_packet.SetReplicasToForward (starting_replicas);
        m_packets_queue.Enqueue (data_packet, Ipv4Address ("1.1.1.1"));

        m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
        NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), starting_replicas,
                               "Packet entry must have " << starting_replicas << " replica(s).");

        discounts_counter = (uint8_t) std::ceil (std::log2 (starting_replicas));
        expected_replicas_to_keep = starting_replicas;
        expected_replicas_to_forward = starting_replicas;

        // The packet must be discounted maximum 'discounts_counter' times
        for (uint8_t i = 0u; i < discounts_counter; ++i)
          {
            m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);

            expected_replicas_to_keep = (uint32_t) std::ceil ((double) packet_entry.GetReplicasCounter () / 2.0);
            expected_replicas_to_forward = (uint32_t) std::floor ((double) packet_entry.GetReplicasCounter () / 2.0);

            NS_TEST_EXPECT_MSG_EQ (expected_replicas_to_keep + expected_replicas_to_forward, packet_entry.GetReplicasCounter (),
                                   "Must be equal.");

            replicas = 98761u; // This value must be modified.
            discounted = false; // This value must be modified

            discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

            NS_TEST_EXPECT_MSG_EQ (discounted, true, "Replicas must be discounted.");
            NS_TEST_EXPECT_MSG_EQ (replicas, expected_replicas_to_forward,
                                   "Replicas to forward must be " << expected_replicas_to_forward);

            m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
            NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), expected_replicas_to_keep,
                                   "Packet entry must have " << expected_replicas_to_keep << " replica(s).");
          }

        // No more replicas can be discounted
        for (uint8_t i = 0u; i < 5u; ++i)
          {
            replicas = 98761u; // This value must not be modified.
            discounted = true; // This value must be modified

            discounted = m_packets_queue.DiscountPacketReplicasToForward (DataIdentifier ("1.1.1.1:1"), replicas);

            NS_TEST_EXPECT_MSG_EQ (discounted, false, "Replicas must not be discounted.");
            NS_TEST_EXPECT_MSG_EQ (replicas, 98761u, "Replicas to forward must not be modified.");
          }

        m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), packet_entry);
        NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 1u,
                               "Packet entry must have 1 replica(s).");
      }
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
    m_packets_queue = PacketsQueue (true, 1u);

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

    m_packets_queue = PacketsQueue (false, 1u);

    std::string expected_str = "Packets queue has 0 / 1 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    m_packets_queue.SetMaxLength (17u);

    expected_str = "Packets queue has 0 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    DataHeader data_packet (/* Data ID */ DataIdentifier ("1.1.1.1:1"),
                            /* Geo-temporal area */ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                     Area (0, 0, 100, 100)),
                            /* Message */ "Message",
                            /* Replicas */ 1u);
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
    TestProcessDisjointVector ();
    TestFindFunctions ();
    TestEnqueueFunction ();
    TestDiscountPacketReplicasToForward_NormalMode ();
    TestDiscountPacketReplicasToForward_BinaryMode ();
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
 * \ingroup geotemporal-spray-and-wait-test
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
//                       GeoTemporalSprayAndWaitTestSuite
// =============================================================================

/**
 * Geo-Temporal Spray And Wait test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-spray-and-wait-test
 */
class GeoTemporalSprayAndWaitTestSuite : public TestSuite
{
public:

  GeoTemporalSprayAndWaitTestSuite () : TestSuite ("geotemporal-spray-and-wait", UNIT)
  {
    AddTestCase (new PacketTypeTest, TestCase::QUICK);
    AddTestCase (new TypeHeaderTest, TestCase::QUICK);
    AddTestCase (new HelloHeaderTest, TestCase::QUICK);
    AddTestCase (new ReplyHeaderTest, TestCase::QUICK);
    AddTestCase (new ReplyBackHeaderTest, TestCase::QUICK);
    AddTestCase (new DataHeaderTest, TestCase::QUICK);
    AddTestCase (new NeighborEntryTest, TestCase::QUICK);
    AddTestCase (new NeighborsTableTest, TestCase::QUICK);
    AddTestCase (new PacketQueueEntryTest, TestCase::QUICK);
    AddTestCase (new PacketsQueueTest, TestCase::QUICK);
    AddTestCase (new DuplicatedPacketDetectorTest, TestCase::QUICK);
  }
};

static GeoTemporalSprayAndWaitTestSuite g_geotemporal_spray_and_wait_test_suite;


} // namespace test
} // namespace geotemporal_spray_and_wait
} // namespace ns3
