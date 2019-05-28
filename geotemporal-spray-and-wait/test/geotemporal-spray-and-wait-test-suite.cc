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

#include <ns3/geotemporal-utils.h>
#include <ns3/math-utils.h>
#include <ns3/packet.h>

#include <ns3/geotemporal-spray-and-wait-module.h>


using namespace ns3;
using namespace ns3::geotemporal_spray_and_wait;

/**
 * With the following line all references to Vector2D will be to 
 * GeoTemporalLibrary::LibraryUtils::Vector2D class instead of ns3::Vector2D.
 */
using GeoTemporalLibrary::LibraryUtils::Vector2D;

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
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 19u, "Must be 19u");

    // Parameters constructor
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    Vector2D position (3, 4);

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
    Vector2D position (5, 14);

    ReplyBackHeader header (summary_vector, position);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 3u, "Summary vector size must be 3");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);

    position = Vector2D (99, 88);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 3u, "Summary vector size must be 3");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
  }

  void
  TestOverloadedOperators ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    Vector2D position (3, 4);

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

    position = Vector2D (99, 299);

    different = ReplyBackHeader (summary_vector, position);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    Vector2D position (3, 4);

    ReplyBackHeader h1;
    ReplyBackHeader h2 (summary_vector, position);
    ReplyBackHeader h3 (h2);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});
    position = Vector2D (-54, 23);

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

    Vector2D position (3, 4);

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
    GeoTemporalArea gta (TimePeriod (10, 20), Area (30, 40, 50, 60));
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
    GeoTemporalArea gta (TimePeriod (10, 20), Area (30, 40, 50, 60));
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

    gta = GeoTemporalArea (TimePeriod (50, 60), Area (30, 40, 50, 60));
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
    GeoTemporalArea gta (TimePeriod (10, 20), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader equal_1 (data_id, gta, message, replicas);
    DataHeader equal_2 (data_id, gta, message, replicas);
    DataHeader different (DataIdentifier (Ipv4Address ("1.2.3.4"), 99u), gta, message, replicas);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    different = DataHeader (data_id, GeoTemporalArea (TimePeriod (10, 20), Area (0, 0, 100, 100)),
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
    GeoTemporalArea gta (TimePeriod (10, 20), Area (30, 40, 50, 60));
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
    GeoTemporalArea gta (TimePeriod (10, 20), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16
    uint32_t replicas = 5u;

    DataHeader h (data_id, gta, message, replicas);
    std::string expected_str = "DATA 1.2.3.4:5 (5 replicas) destined to area "
            "{(30.00, 40.00), (50.00, 60.00)} to start at second "
            "10 with a duration of 10 seconds has a message of 16 byte(s)";
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
  }
};

static GeoTemporalSprayAndWaitTestSuite g_geotemporal_spray_and_wait_test_suite;

