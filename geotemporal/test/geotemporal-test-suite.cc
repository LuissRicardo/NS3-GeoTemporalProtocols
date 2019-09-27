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

#include <ns3/geotemporal-module.h>

#include <ns3/geotemporal-utils.h>
#include <ns3/math-utils.h>
#include <ns3/packet.h>
#include <ns3/packet-utils.h>

using namespace GeoTemporalLibrary::LibraryUtils;


namespace ns3
{
namespace geotemporal
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

static std::ostream &
operator<< (std::ostream & os, const std::set<Ipv4Address> & obj)
{
  os << "Set of Ipv4Address with " << obj.size () << " items";

  if (obj.size () == 0) return os;

  os << ":";

  for (std::set<Ipv4Address>::const_iterator it = obj.begin (); it != obj.end (); ++it)
    {
      os << " " << GeoTemporalLibrary::LibraryUtils::ToString (*it);
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
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_1, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_1, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_1, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_1, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_1, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_1, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_2, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_2, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (equal_2, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_2, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_2, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (equal_2, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (different, equal_1), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (different, equal_2), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator== (different, different), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (different, equal_1), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (different, equal_2), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (ns3::geotemporal::operator!= (different, different), false, "Must not be different");
  }
};


// =============================================================================
//                                PacketTypeTest
// =============================================================================

/**
 * PacketType enumeration test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
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

    pt = PacketType::Ack;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::Ack, "Must be Ack");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "Ack", "Must be Ack");

    pt = PacketType::Data;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::Data, "Must be Data");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "Data", "Must be Data");

    pt = PacketType::DataAck;
    NS_TEST_EXPECT_MSG_EQ (pt, PacketType::DataAck, "Must be DataAck");
    NS_TEST_EXPECT_MSG_EQ (ToString (pt), "DataAck", "Must be DataAck");
  }
};


// =============================================================================
//                                TypeHeaderTest
// =============================================================================

/**
 * TypeHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
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
 * \ingroup geotemporal-test
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
 * \ingroup geotemporal-test
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
    NS_TEST_EXPECT_MSG_EQ (h1.GetVelocity (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 27u, "Must be 27u");

    // Parameters constructor
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    SummaryVectorHeader h2 (summary_vector, position, velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h2.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 45u, "Must be 45u");

    // Copy constructor
    SummaryVectorHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVectorSize (), 3u, "Must be 3u");
    NS_TEST_EXPECT_MSG_EQ (h3.GetSummaryVector (), summary_vector, "Must be a set with 3 DataIdentifiers");
    NS_TEST_EXPECT_MSG_EQ (h3.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 45u, "Must be 45u");
  }

  void
  TestGettersSetters ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};
    GeoTemporalLibrary::LibraryUtils::Vector2D position (5, -14);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    SummaryVectorHeader header (summary_vector, position, velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 3u, "Summary vector size must be 3");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());

    summary_vector = {DataIdentifier (Ipv4Address ("9.9.9.9"), 9),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 19),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 29),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 39),
      DataIdentifier (Ipv4Address ("9.9.9.9"), 49)};
    header.SetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (-99, 88);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());

    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (99, -88);
    header.SetVelocity (velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVectorSize (), 5u, "Summary vector size must be 5");
    NS_TEST_EXPECT_MSG_EQ (header.GetSummaryVector (), summary_vector, "Summary vector must be equal to summary_vector");
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Summary vector size must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());
  }

  void
  TestOverloadedOperators ()
  {
    // Different summary vector
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (5, -14);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    SummaryVectorHeader equal_1 (summary_vector, position, velocity);
    SummaryVectorHeader equal_2 (summary_vector, position, velocity);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});

    SummaryVectorHeader different (summary_vector, position, velocity);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different position
    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
                                              DataIdentifier (Ipv4Address ("1.1.1.1"), 3)});

    equal_1 = SummaryVectorHeader (summary_vector, position, velocity);
    equal_2 = SummaryVectorHeader (summary_vector, position, velocity);

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (50, 1.4);

    different = SummaryVectorHeader (summary_vector, position, velocity);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different velocity
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (5, -14);

    equal_1 = SummaryVectorHeader (summary_vector, position, velocity);
    equal_2 = SummaryVectorHeader (summary_vector, position, velocity);

    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (8.86, -123.098);

    different = SummaryVectorHeader (summary_vector, position, velocity);

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (5, -14);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    SummaryVectorHeader h1;
    SummaryVectorHeader h2 (summary_vector, position, velocity);
    SummaryVectorHeader h3 (h2);

    summary_vector = std::set<DataIdentifier>({DataIdentifier (Ipv4Address ("1.1.1.1"), 1)});
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (981597.52, 719536.02);
    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (5.5559, -1.1111);

    h2 = SummaryVectorHeader (summary_vector, position, velocity);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    SummaryVectorHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 3u + 27u, "SummaryVectorHeader is 45 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 1u + 27u, "SummaryVectorHeader is 33 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 6u * 0u + 27u, "SummaryVectorHeader is 27 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");
  }

  void
  TestToString ()
  {
    std::set<DataIdentifier> summary_vector = {DataIdentifier (Ipv4Address ("1.1.1.1"), 1),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 2),
      DataIdentifier (Ipv4Address ("1.1.1.1"), 3)};

    GeoTemporalLibrary::LibraryUtils::Vector2D position (981597.52, 719536.02);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    SummaryVectorHeader h (summary_vector, position, velocity);
    std::string expected_str = "SUMMARY_VECTOR sent from position (981597.52, 719536.02) "
            "at velocity (-8.86, 123.10) "
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
//                                 AckHeaderTest
// =============================================================================

/**
 * AckHeaderTest packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
 */
class AckHeaderTest : public TestCasePlus
{
public:

  AckHeaderTest () : TestCasePlus ("AckHeaderTest") { }

  void
  TestConstructors ()
  {
    // Default constructor
    AckHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetDataIdentifier (), DataIdentifier (), "Must be a default DataIdentifier instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetVelocity (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be a default Vector2D instance");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 31u, "Must be 31u");

    // Parameters constructor
    DataIdentifier data_id ("1.1.1.1:3");
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    AckHeader h2 (data_id, position, velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 31u, "Must be 31u");

    // Copy constructor
    AckHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 31u, "Must be 31u");
  }

  void
  TestGettersSetters ()
  {
    DataIdentifier data_id ("1.1.1.1:3");
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    AckHeader header (data_id, position, velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 31u, "Must be 31u");

    data_id = DataIdentifier ("8.7.6.5:43210");
    header.SetDataIdentifier (data_id);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 31u, "Must be 31u");

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (75.3, -951.25);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 31u, "Must be 31u");

    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (-156.2, -75.5);
    header.SetVelocity (velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 31u, "Must be 31u");
  }

  void
  TestOverloadedOperators ()
  {
    // Different data ID
    DataIdentifier data_id ("1.1.1.1:3");
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    AckHeader equal_1 (data_id, position, velocity);
    AckHeader equal_2 (data_id, position, velocity);

    AckHeader different (DataIdentifier ("1.1.1.1:45000"), position, velocity);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different position
    different = AckHeader (data_id, GeoTemporalLibrary::LibraryUtils::Vector2D (30, -0.456), velocity);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different velocity
    different = AckHeader (data_id, position, GeoTemporalLibrary::LibraryUtils::Vector2D (30, -0.456));

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    DataIdentifier data_id ("1.1.1.1:3");
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    AckHeader h1;
    AckHeader h2 (data_id, position, velocity);
    AckHeader h3 (h2);

    data_id = DataIdentifier ("1.1.1.1:62302");
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (981597.52, 719536.02);
    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (5.5559, -1.1111);

    h2 = AckHeader (data_id, position, velocity);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    AckHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 31u, "SummaryVectorHeader is 31 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 31u, "SummaryVectorHeader is 31 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 31u, "SummaryVectorHeader is 31 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");

    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " + data_id.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " + position.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " + velocity.ToString ());
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 31u, "Must be 31u");
  }

  void
  TestToString ()
  {
    DataIdentifier data_id ("1.1.1.1:3");
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);

    AckHeader h (data_id, position, velocity);

    std::string expected_str = "ACK packet 1.1.1.1:3 from position (3.00, 4.00) "
            "at velocity (-8.86, 123.10)";
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
 * \ingroup geotemporal-test
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
    NS_TEST_EXPECT_MSG_EQ (h1.GetHopsCount (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be default GeoTemporalLibrary::LibraryUtils::Vector2D");
    NS_TEST_EXPECT_MSG_EQ (h1.GetVelocity (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be default GeoTemporalLibrary::LibraryUtils::Vector2D");
    NS_TEST_EXPECT_MSG_EQ (h1.GetDestinationGeoTemporalArea (), GeoTemporalArea (), "Must be default GeoTemporalArea");
    NS_TEST_EXPECT_MSG_EQ (h1.GetMessage (), "", "Must be empty string");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 72u, "Must be 72u");

    // Parameters constructor
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataHeader h2 (data_id, hops_count, position, velocity, gta, message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 72u + 16u, "Must be 88u");

    // Copy constructor
    DataHeader h3 (h2);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h3.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h3.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h3.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h3.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 72u + 16u, "Must be 88u");
  }

  void
  TestGettersSetters ()
  {
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataHeader header (data_id, hops_count, position, velocity, gta, message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    data_id = DataIdentifier ("10.11.12.13:14");
    header.SetDataIdentifier (data_id);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    hops_count = 65123u;
    header.SetHopsCount (hops_count);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (12.36, -985.2);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (-741.369, -987.123);
    header.SetVelocity (velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    gta = GeoTemporalArea (TimePeriod (Seconds (50), Seconds (80)), Area (0, 0, 150, 160));
    header.SetDestinationGeoTemporalArea (gta);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 16u, "Must be 88u");

    message = "012345678901234567890123456789";
    header.SetMessage (message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 72u + 30u, "Must be 102u");
  }

  void
  TestOverloadedOperators ()
  {
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataHeader equal_1 (data_id, hops_count, position, velocity, gta, message);
    DataHeader equal_2 (data_id, hops_count, position, velocity, gta, message);

    // Different data ID
    DataHeader different (DataIdentifier ("10.9.8.7:64321"), hops_count, position, velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different hops count
    different = DataHeader (data_id, 30, position, velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different position
    different = DataHeader (data_id, hops_count, GeoTemporalLibrary::LibraryUtils::Vector2D (100, 100),
                            velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different velocity
    different = DataHeader (data_id, hops_count, position,
                            GeoTemporalLibrary::LibraryUtils::Vector2D (100, 951.37), gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different geo-temporal area
    different = DataHeader (data_id, hops_count, position, velocity,
                            GeoTemporalArea (TimePeriod (Seconds (100), Seconds (200)), Area (30, 40, 50, 60)),
                            message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different message
    different = DataHeader (data_id, hops_count, position, velocity, gta,
                            "012345678901234567890123456789");

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataHeader h1;
    DataHeader h2 (data_id, hops_count, position, velocity, gta, message);
    DataHeader h3 (h2);

    data_id = DataIdentifier ("10.9.8.7:64321");
    hops_count = 32147u;
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (-50.63, 159);
    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (5.5559, -1.1111);
    gta = GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (-100, 40, 50, -90));
    message = "012345678901234567890123456789"; // Message length = 30

    h2 = DataHeader (data_id, hops_count, position, velocity, gta, message);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    DataHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 72u + 16u, "DataHeader is 88 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 72u + 30u, "DataHeader is 102 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 72u, "DataHeader is 72 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");

    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 72u + 30u, "Must be 102u");
  }

  void
  TestToString ()
  {
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (300, 40.89);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataHeader h (data_id, hops_count, position, velocity, gta, message);

    std::string expected_str = "DATA 1.2.3.4:5 (65 hops) sent from position "
            "(300.00, 40.89) at velocity (-8.86, 123.10) destined to area "
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
//                               DataAckHeaderTest
// =============================================================================

/**
 * DataAckHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
 */
class DataAckHeaderTest : public TestCasePlus
{
public:

  DataAckHeaderTest () : TestCasePlus ("DataAckHeader") { }

  void
  TestConstructors ()
  {
    // Default constructor
    DataAckHeader h1;
    NS_TEST_EXPECT_MSG_EQ (h1.GetDataIdentifierToAck (), DataIdentifier (), "Must be default DataIdentifier");
    NS_TEST_EXPECT_MSG_EQ (h1.GetDataIdentifier (), DataIdentifier (), "Must be default DataIdentifier");
    NS_TEST_EXPECT_MSG_EQ (h1.GetHopsCount (), 0u, "Must be 0u");
    NS_TEST_EXPECT_MSG_EQ (h1.GetPosition (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be default GeoTemporalLibrary::LibraryUtils::Vector2D");
    NS_TEST_EXPECT_MSG_EQ (h1.GetVelocity (), GeoTemporalLibrary::LibraryUtils::Vector2D (), "Must be default GeoTemporalLibrary::LibraryUtils::Vector2D");
    NS_TEST_EXPECT_MSG_EQ (h1.GetDestinationGeoTemporalArea (), GeoTemporalArea (), "Must be default GeoTemporalArea");
    NS_TEST_EXPECT_MSG_EQ (h1.GetMessage (), "", "Must be empty string");
    NS_TEST_EXPECT_MSG_EQ (h1.GetSerializedSize (), 80u, "Must be 80u");

    // Parameters constructor
    DataIdentifier to_ack ("10.20.30.40:50");
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataAckHeader h2 (to_ack, data_id, hops_count, position, velocity, gta, message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 80u + 16u, "Must be 96u");

    data_id = DataIdentifier ("9.8.7.6:5");
    hops_count = 193u;
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (98.65, -12.68);
    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (-742.3, 82.1);
    gta = GeoTemporalArea (TimePeriod (Seconds (50), Seconds (100)), Area (100, 100, 200, 200));
    message = "new packet's message"; // Length 20

    DataHeader h3_base (data_id, hops_count, position, velocity, gta, message);

    to_ack = DataIdentifier ("90.88.70.66:505");

    DataAckHeader h3 (h3_base, to_ack);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h3.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h3.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h3.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h3.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h3.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h3.GetSerializedSize (), 80u + 20u, "Must be 100u");

    // Copy constructor
    DataAckHeader h4 (h3);
    NS_TEST_EXPECT_MSG_EQ (h4.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (h4.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h4.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h4.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h4.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h4.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h4.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h4.GetSerializedSize (), 80u + 20u, "Must be 100u");
  }

  void
  TestGettersSetters ()
  {
    DataIdentifier to_ack ("10.20.30.40:50");
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataAckHeader header (to_ack, data_id, hops_count, position, velocity, gta, message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    to_ack = DataIdentifier ("99.98.97.96:95");
    header.SetDataIdentifierToAck (to_ack);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    data_id = DataIdentifier ("10.11.12.13:14");
    header.SetDataIdentifier (data_id);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    hops_count = 65123u;
    header.SetHopsCount (hops_count);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    position = GeoTemporalLibrary::LibraryUtils::Vector2D (12.36, -985.2);
    header.SetPosition (position);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (-741.369, -987.123);
    header.SetVelocity (velocity);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    gta = GeoTemporalArea (TimePeriod (Seconds (50), Seconds (80)), Area (0, 0, 150, 160));
    header.SetDestinationGeoTemporalArea (gta);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 16u, "Must be 96u");

    message = "012345678901234567890123456789";
    header.SetMessage (message);

    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (header.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (header.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (header.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (header.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (header.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (header.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (header.GetSerializedSize (), 80u + 30u, "Must be 110u");
  }

  void
  TestOverloadedOperators ()
  {
    DataIdentifier to_ack ("10.20.30.40:50");
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataAckHeader equal_1 (to_ack, data_id, hops_count, position, velocity, gta, message);
    DataAckHeader equal_2 (to_ack, data_id, hops_count, position, velocity, gta, message);

    // Different data ID to ack
    DataAckHeader different (DataIdentifier ("10.9.8.7:64321"), data_id, hops_count, position, velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different data ID
    different = DataAckHeader (to_ack, DataIdentifier ("10.9.8.7:64321"), hops_count, position, velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different hops count
    different = DataAckHeader (to_ack, data_id, 30, position, velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different position
    different = DataAckHeader (to_ack, data_id, hops_count, GeoTemporalLibrary::LibraryUtils::Vector2D (100, 100),
                               velocity, gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different velocity
    different = DataAckHeader (to_ack, data_id, hops_count, position,
                               GeoTemporalLibrary::LibraryUtils::Vector2D (100, 951.37), gta, message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different geo-temporal area
    different = DataAckHeader (to_ack, data_id, hops_count, position, velocity,
                               GeoTemporalArea (TimePeriod (Seconds (100), Seconds (200)), Area (30, 40, 50, 60)),
                               message);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different message
    different = DataAckHeader (to_ack, data_id, hops_count, position, velocity, gta,
                               "012345678901234567890123456789");

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestSerializationDeserialization ()
  {
    DataIdentifier to_ack ("10.20.30.40:50");
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (3, 4);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataAckHeader h1;
    DataAckHeader h2 (to_ack, data_id, hops_count, position, velocity, gta, message);
    DataAckHeader h3 (h2);

    to_ack = DataIdentifier ("99.98.97.96:95");
    data_id = DataIdentifier ("10.9.8.7:64321");
    hops_count = 32147u;
    position = GeoTemporalLibrary::LibraryUtils::Vector2D (-50.63, 159);
    velocity = GeoTemporalLibrary::LibraryUtils::Vector2D (5.5559, -1.1111);
    gta = GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (-100, 40, 50, -90));
    message = "012345678901234567890123456789"; // Message length = 30

    h2 = DataAckHeader (to_ack, data_id, hops_count, position, velocity, gta, message);

    Ptr<Packet> packet = Create<Packet> ();
    packet->AddHeader (h1);
    packet->AddHeader (h2);
    packet->AddHeader (h3);

    DataAckHeader deserialized;

    uint32_t read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 80u + 16u, "DataAckHeader is 96 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h3, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 80u + 30u, "DataAckHeader is 110 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h2, deserialized, "Serialization and deserialization works");

    read_bytes = packet->RemoveHeader (deserialized);
    NS_TEST_EXPECT_MSG_EQ (read_bytes, 80u, "DataAckHeader is 80 bytes long");
    NS_TEST_EXPECT_MSG_EQ (h1, deserialized, "Serialization and deserialization works");

    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifierToAck (), to_ack, "Must be " << to_ack);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDataIdentifier (), data_id, "Must be " << data_id);
    NS_TEST_EXPECT_MSG_EQ (h2.GetHopsCount (), hops_count, "Must be " << hops_count);
    NS_TEST_EXPECT_MSG_EQ (h2.GetPosition (), position, "Must be " << position);
    NS_TEST_EXPECT_MSG_EQ (h2.GetVelocity (), velocity, "Must be " << velocity);
    NS_TEST_EXPECT_MSG_EQ (h2.GetDestinationGeoTemporalArea (), gta, "Must be " << gta);
    NS_TEST_EXPECT_MSG_EQ (h2.GetMessage (), message, "Must be " << message);
    NS_TEST_EXPECT_MSG_EQ (h2.GetSerializedSize (), 80u + 30u, "Must be 110");
  }

  void
  TestToString ()
  {
    DataIdentifier to_ack ("10.20.30.40:50");
    DataIdentifier data_id ("1.2.3.4:5");
    uint32_t hops_count = 65u;
    GeoTemporalLibrary::LibraryUtils::Vector2D position (300, 40.89);
    GeoTemporalLibrary::LibraryUtils::Vector2D velocity (-8.86, 123.098);
    GeoTemporalArea gta (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60));
    std::string message = "packet's message"; // Length 16

    DataAckHeader h (to_ack, data_id, hops_count, position, velocity, gta, message);

    std::string expected_str = "ACK 10.20.30.40:50 / "
            "DATA 1.2.3.4:5 (65 hops) sent from position "
            "(300.00, 40.89) at velocity (-8.86, 123.10) destined to area "
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
 * \ingroup geotemporal-test
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
    NS_TEST_EXPECT_MSG_EQ (e1.GetRequestedPacketsSet ().empty (), true, "Must be an empty set");

    // Parameters constructor 1
    NeighborEntry e2 (Ipv4Address ("1.2.3.4"), Seconds (10));
    NS_TEST_EXPECT_MSG_EQ (e2.GetNeighborIpAddress (), Ipv4Address ("1.2.3.4"), "Must be 1.2.3.4");
    NS_TEST_EXPECT_MSG_EQ (e2.GetExpirationTime (), Seconds (10), "Must be 10 seconds");
    NS_TEST_EXPECT_MSG_EQ (e2.GetRequestedPacketsSet ().empty (), true, "Must be an empty set");

    // Parameters constructor 2
    std::set<DataIdentifier> requested_packets = {DataIdentifier ("1.1.1.1:1"),
      DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:3")};

    NeighborEntry e3 (Ipv4Address ("1.2.3.6"), requested_packets);
    NS_TEST_EXPECT_MSG_EQ (e3.GetNeighborIpAddress (), Ipv4Address ("1.2.3.6"), "Must be 1.2.3.6");
    NS_TEST_EXPECT_MSG_EQ (e3.GetExpirationTime (), Time (), "Must be default time (0)");
    NS_TEST_EXPECT_MSG_EQ (e3.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    // Parameters constructor 3
    requested_packets = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};

    NeighborEntry e4 (Ipv4Address ("1.2.3.8"), Seconds (36), requested_packets);
    NS_TEST_EXPECT_MSG_EQ (e4.GetNeighborIpAddress (), Ipv4Address ("1.2.3.8"), "Must be 1.2.3.8");
    NS_TEST_EXPECT_MSG_EQ (e4.GetExpirationTime (), Seconds (36), "Must be 36 seconds");
    NS_TEST_EXPECT_MSG_EQ (e4.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    // Copy constructor
    NeighborEntry e5 (e4);
    NS_TEST_EXPECT_MSG_EQ (e5.GetNeighborIpAddress (), Ipv4Address ("1.2.3.8"), "Must be 1.2.3.8");
    NS_TEST_EXPECT_MSG_EQ (e5.GetExpirationTime (), Seconds (36), "Must be 36 seconds");
    NS_TEST_EXPECT_MSG_EQ (e5.GetRequestedPacketsSet (), requested_packets, "Must be equal");
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

    std::set<DataIdentifier> requested_packets = {DataIdentifier ("1.1.1.1:1"),
      DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:3")};

    equal_1 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (10), requested_packets);
    equal_2 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (5), requested_packets);
    different = NeighborEntry (Ipv4Address ("10.20.30.40"), Seconds (17), requested_packets);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (10), requested_packets);

    requested_packets = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};

    equal_2 = NeighborEntry (Ipv4Address ("1.2.3.4"), Seconds (10), requested_packets);

    requested_packets = {DataIdentifier ("3.3.3.3:1"), DataIdentifier ("3.3.3.3:2")};

    different = NeighborEntry (Ipv4Address ("10.20.30.40"), Seconds (10), requested_packets);
  }

  void
  TestToString_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 2.5
    NeighborEntry entry (Ipv4Address ("1.2.3.4"), Seconds (10));
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 12.50 requests 0 packets";
    NS_TEST_EXPECT_MSG_EQ (entry.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestToString_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 15.25
    std::set<DataIdentifier> requested_packets = {DataIdentifier ("1.1.1.1:1"),
      DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:3")};

    NeighborEntry entry (Ipv4Address ("1.2.3.4"), Seconds (17), requested_packets);
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 32.25 requests 3 packets: "
            "1.1.1.1:1 1.1.1.1:2 1.1.1.1:3";
    NS_TEST_EXPECT_MSG_EQ (entry.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestToString_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 21.9
    std::set<DataIdentifier> requested_packets = {DataIdentifier ("1.1.1.1:1")};

    NeighborEntry entry (Ipv4Address ("1.2.3.4"), requested_packets);
    std::string expected_str = "Neighbor entry 1.2.3.4 will expire at second 0.00 requests 1 packets: 1.1.1.1:1";
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
  TestRemoveRequestedPacketsSet ()
  {
    NeighborEntry neighbor (Ipv4Address ("1.2.3.4"), Seconds (10));

    std::set<DataIdentifier> requested_packets;

    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4")};

    neighbor.SetRequestedPacketsSet (requested_packets);

    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:4")};

    bool answer = neighbor.RemoveRequestedPacket (DataIdentifier ("1.1.1.1:3"));

    NS_TEST_EXPECT_MSG_EQ (answer, true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:4")};

    answer = neighbor.RemoveRequestedPacket (DataIdentifier ("1.1.1.1:5"));

    NS_TEST_EXPECT_MSG_EQ (answer, false, "Must be false");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets, "Must be equal");

    requested_packets = {DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:4")};

    answer = neighbor.RemoveRequestedPacket (DataIdentifier ("1.1.1.1:1"));

    NS_TEST_EXPECT_MSG_EQ (answer, true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets, "Must be equal");
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

    TestRemoveRequestedPacketsSet ();
  }
};


// =============================================================================
//                               NeighborsTableTest
// =============================================================================

/**
 * NeighborsTable for the neighbors table test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
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
  TestGetSize ()
  {
    m_neighbors_table = NeighborsTable (Seconds (5));

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), m_neighbors_table.m_table.size (),
                           "Must have the same size.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must have size 0");

    char buffer [25];

    for (uint32_t i = 1u; i <= 75u; ++i)
      {
        std::sprintf (buffer, "1.1.1.%d", i);

        m_neighbors_table.Insert (Ipv4Address (buffer));

        NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), m_neighbors_table.m_table.size (),
                               "Must have the same size.");
      }

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    for (uint32_t i = 30u; i <= 45u; ++i)
      {
        std::sprintf (buffer, "1.1.1.%d", i);

        m_neighbors_table.Remove (Ipv4Address (buffer));

        NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), m_neighbors_table.m_table.size (),
                               "Must have the same size.");
      }

    m_neighbors_table.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), m_neighbors_table.m_table.size (),
                           "Must have the same size.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must have size 0");
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
  }

  void
  TestClearFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (5));

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must have size 0");

    m_neighbors_table.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must have size 0");

    char buffer [25];

    for (uint32_t i = 1u; i <= 75u; ++i)
      {
        std::sprintf (buffer, "1.1.1.%d", i);

        m_neighbors_table.Insert (Ipv4Address (buffer));

        NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), m_neighbors_table.m_table.size (),
                               "Must have the same size.");
      }

    m_neighbors_table.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must have size 0");
  }

  void
  TestInsertFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 7.00
    bool inserted;
    NeighborEntry entry;

    m_neighbors_table.Purge ();

    // The following entry to be inserted used to exist but was purged by the call
    // to Purge, so it is inserted as newly inserted.
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

    m_neighbors_table.Purge ();

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
    std::set<DataIdentifier> requested_packets;

    // Successful insertion of new entry
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.1 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1u, "Size of the neighbors table must be 1.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Entry 1.1.1.1 expiration time must be 10 seconds.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet ().empty (), true,
                           "Entry 1.1.1.1 requested packets set must be empty");

    // Expected failure of already inserted entry
    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (inserted, false, "Neighbor entry 1.1.1.1 must have not been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1u, "Size of the neighbors table must be 1.");

    // Test that changing the expiration time of the entries is applied to new entries.
    m_neighbors_table.SetEntriesExpirationTime (Seconds (15));

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3")};

    inserted = m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"), requested_packets);

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.2 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 2u, "Size of the neighbors table must be 2.");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.2"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (15), MicroSeconds (1),
                               "Entry 1.1.1.2 expiration time must be 15 seconds.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.2 requested packets set must be the expected");

    // Now the expiration time is set to 5 seconds
    m_neighbors_table.SetEntriesExpirationTime (Seconds (5));

    requested_packets = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};

    entry = NeighborEntry (Ipv4Address ("1.1.1.3"), requested_packets);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (0), MicroSeconds (1),
                               "Must be 0 seconds");

    inserted = m_neighbors_table.Insert (entry);

    NS_TEST_EXPECT_MSG_EQ (inserted, true, "Neighbor entry 1.1.1.3 must have been inserted.");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3u, "Size of the neighbors table must be 3.");

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Must be 5 seconds");

    m_neighbors_table.Find (Ipv4Address ("1.1.1.3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Entry 1.1.1.3 expiration time must be 5 seconds.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.3 requested packets set must be the expected");

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

    m_neighbors_table.Purge ();

    // The following entry to be removed used to exist but was purged by the call
    // to Purge, so it must return a false.
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
    //  - Try to remove purged entries.
    Simulator::Schedule (Seconds (15), &NeighborsTableTest::TestRemoveFunction_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();
  }

  void
  TestUpdateNeighborRequestedPacketsFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    NeighborEntry entry;
    std::set<DataIdentifier> requested_packets;

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet ().empty (), true,
                           "Entry 1.1.1.1 requested packets set must be empty");

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3")};

    m_neighbors_table.UpdateNeighborRequestedPackets (Ipv4Address ("1.1.1.1"),
                                                      requested_packets);

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.2 requested packets set must be the expected");

    requested_packets = {DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2"),
      DataIdentifier ("2.2.2.2:3"), DataIdentifier ("2.2.2.2:4")};

    m_neighbors_table.UpdateNeighborRequestedPackets (Ipv4Address ("1.1.1.1"),
                                                      requested_packets);

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), entry);

    NS_TEST_EXPECT_MSG_EQ (entry.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.2 requested packets set must be the expected");
  }

  void
  TestRemoveNeighborRequestedPacketFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    NeighborEntry neighbor;
    bool answer = false;
    std::set<DataIdentifier> requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:3"), DataIdentifier ("1.1.1.1:4")};

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"), requested_packets);

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), neighbor);

    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.1 requested packets set must be the expected");

    // Test remove existing packet from existing neighbor
    answer = m_neighbors_table.RemoveNeighborRequestedPacket (Ipv4Address ("1.1.1.1"),
                                                              DataIdentifier ("1.1.1.1:3"));

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:4")};

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), neighbor);

    NS_TEST_EXPECT_MSG_EQ (answer, true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.1 requested packets set must be the expected");

    // Test remove packet from non-existing neighbor
    answer = m_neighbors_table.RemoveNeighborRequestedPacket (Ipv4Address ("2.2.2.2"),
                                                              DataIdentifier ("1.1.1.1:2"));

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), neighbor);

    NS_TEST_EXPECT_MSG_EQ (answer, false, "Must be false");

    // Test remove non-existing packet from existing neighbor
    answer = true;
    answer = m_neighbors_table.RemoveNeighborRequestedPacket (Ipv4Address ("1.1.1.1"),
                                                              DataIdentifier ("1.1.1.1:55"));

    requested_packets = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("1.1.1.1:4")};

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), neighbor);

    NS_TEST_EXPECT_MSG_EQ (answer, false, "Must be false");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.1 requested packets set must be the expected");

    // Test remove existing packet from existing neighbor
    answer = m_neighbors_table.RemoveNeighborRequestedPacket (Ipv4Address ("1.1.1.1"),
                                                              DataIdentifier ("1.1.1.1:1"));

    requested_packets = {DataIdentifier ("1.1.1.1:2"), DataIdentifier ("1.1.1.1:4")};

    m_neighbors_table.Find (Ipv4Address ("1.1.1.1"), neighbor);

    NS_TEST_EXPECT_MSG_EQ (answer, true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (neighbor.GetRequestedPacketsSet (), requested_packets,
                           "Entry 1.1.1.1 requested packets set must be the expected");
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
  TestPurgeFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 3.55

    // The neighbors table now looks like this:
    //     Neighbor IP   -   Expiration time
    //       1.1.1.1     -      second 10
    //       1.1.1.2     -      second 5
    //       1.1.1.3     -      second 7.5

    m_neighbors_table.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3, "Must be 3");
  }

  void
  TestPurgeFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 6.2

    // The neighbors table now looks like this:
    //     Neighbor IP   -   Expiration time
    //       1.1.1.1     -      second 10
    //       1.1.1.2     -      second 5    <EXPIRED>
    //       1.1.1.3     -      second 7.5

    m_neighbors_table.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 2, "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.1")),
                           true, "Neighbor 1.1.1.1 must be found");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.2")),
                           false, "Neighbor 1.1.1.2 must NOT be found");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.3")),
                           true, "Neighbor 1.1.1.3 must be found");
  }

  void
  TestPurgeFunction_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 8.1

    // The neighbors table now looks like this:
    //     Neighbor IP   -   Expiration time
    //       1.1.1.1     -      second 10
    //       1.1.1.3     -      second 7.5   <EXPIRED>

    m_neighbors_table.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 1, "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.1")),
                           true, "Neighbor 1.1.1.1 must be found");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.3")),
                           false, "Neighbor 1.1.1.3 must NOT be found");
  }

  void
  TestPurgeFunction_Scheduled_4 ()
  {
    // This function is launched by the scheduler at second 12

    // The neighbors table now looks like this:
    //     Neighbor IP   -   Expiration time
    //       1.1.1.1     -      second 10   <EXPIRED>

    m_neighbors_table.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Find (Ipv4Address ("1.1.1.1")),
                           false, "Neighbor 1.1.1.1 must NOT be found");
  }

  void
  TestPurgeFunction ()
  {
    m_neighbors_table = NeighborsTable (Seconds (10));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.1"));

    m_neighbors_table.SetEntriesExpirationTime (Seconds (5));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.2"));

    m_neighbors_table.SetEntriesExpirationTime (Seconds (7.5));

    m_neighbors_table.Insert (Ipv4Address ("1.1.1.3"));

    // The neighbors table now looks like this:
    //     Neighbor IP   -   Expiration time
    //       1.1.1.1     -      second 10
    //       1.1.1.2     -      second 5
    //       1.1.1.3     -      second 7.5

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3, "Must be 3");

    m_neighbors_table.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_neighbors_table.Size (), 3, "Must be 3");

    Simulator::Schedule (Seconds (3.55), &NeighborsTableTest::TestPurgeFunction_Scheduled_1, this);
    Simulator::Schedule (Seconds (6.2), &NeighborsTableTest::TestPurgeFunction_Scheduled_2, this);
    Simulator::Schedule (Seconds (8.1), &NeighborsTableTest::TestPurgeFunction_Scheduled_3, this);
    Simulator::Schedule (Seconds (12), &NeighborsTableTest::TestPurgeFunction_Scheduled_4, this);

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
    TestGetSize ();
    TestFindFunctions ();
    TestClearFunction ();
    TestInsertFunction ();
    TestRemoveFunction ();
    TestUpdateNeighborRequestedPacketsFunction ();
    TestRemoveNeighborRequestedPacketFunction ();
    TestRestartNeighborEntryExpirationTimeFunction ();
    TestPurgeFunction ();
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
 * \ingroup geotemporal-test
 */
class PacketQueueEntryTest : public TestCasePlus
{
public:

  const DataIdentifier m_data_id;
  const uint32_t m_hops_count;
  const GeoTemporalLibrary::LibraryUtils::Vector2D m_position;
  const GeoTemporalLibrary::LibraryUtils::Vector2D m_velocity;
  const GeoTemporalArea m_geo_temporal_area;
  const std::string m_message;
  const DataHeader m_header;

  PacketQueueEntry m_queue_entry;
  uint32_t m_replicas;

  PacketQueueEntryTest ()
  : TestCasePlus ("PacketQueueEntry"),
  m_data_id (Ipv4Address ("1.1.1.1"), 1u),
  m_hops_count (7u),
  m_position (300, 40.89),
  m_velocity (-8.86, 123.098),
  m_geo_temporal_area (TimePeriod (Seconds (34), Seconds (74)), Area (10, 10, 90, 90)),
  m_message ("0123456789"),
  m_header (m_data_id, m_hops_count, m_position, m_velocity, m_geo_temporal_area,
            m_message),
  m_queue_entry (),
  m_replicas (3u) { }

  void
  TestConstructors ()
  {
    // Default constructor
    PacketQueueEntry e1;

    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacketId (), DataIdentifier (), "Must be the default DataIdentifier.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetDataPacket (), DataHeader (), "Must be the default DataHeader.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetExpirationTime (), Time (), "Must be the default Time.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetReplicasCounter (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetKnownCarrierNodesCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (e1.GetKnownCarrierNodesSet ().empty (), true, "Must be true.");

    // Parameters constructor
    PacketQueueEntry e2 (m_header, m_replicas);

    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e2.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ_TOL (e2.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");
    NS_TEST_EXPECT_MSG_EQ (e2.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);
    NS_TEST_EXPECT_MSG_EQ (e2.GetKnownCarrierNodesCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (e2.GetKnownCarrierNodesSet ().empty (), true, "Must be true.");

    // Copy constructor
    PacketQueueEntry e3 (e2);

    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacketId (), m_data_id, "Must be " << m_data_id);
    NS_TEST_EXPECT_MSG_EQ (e3.GetDataPacket (), m_header, "Must be " << m_header);
    NS_TEST_EXPECT_MSG_EQ_TOL (e3.GetExpirationTime (), Seconds (74), MicroSeconds (1),
                               "Must be the second 74.");
    NS_TEST_EXPECT_MSG_EQ (e3.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);
    NS_TEST_EXPECT_MSG_EQ (e3.GetKnownCarrierNodesCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (e3.GetKnownCarrierNodesSet ().empty (), true, "Must be true.");
  }

  void
  TestGetSetReplicasCounter ()
  {
    PacketQueueEntry entry (m_header, m_replicas);

    NS_TEST_EXPECT_MSG_EQ (entry.GetReplicasCounter (), m_replicas, "Must be " << m_replicas);

    entry.SetReplicasCounter (497u);

    NS_TEST_EXPECT_MSG_EQ (entry.GetReplicasCounter (), 497u, "Must be 497");
  }

  void
  TestKnownCarrierNodes ()
  {
    PacketQueueEntry entry (m_header, m_replicas);
    std::set<Ipv4Address> expected_set;

    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesSet (), expected_set, "Must be equal.");

    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.1"));
    expected_set.insert (Ipv4Address ("10.0.0.1"));

    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesCount (), 1u, "Must be 1.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesSet (), expected_set, "Must be equal.");

    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.2"));
    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.2"));
    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.2"));
    expected_set.insert (Ipv4Address ("10.0.0.2"));

    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesCount (), 2u, "Must be 2.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesSet (), expected_set, "Must be equal.");

    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.3"));
    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.4"));
    entry.AddKnownCarrierNode (Ipv4Address ("10.0.0.5"));
    expected_set.insert (Ipv4Address ("10.0.0.3"));
    expected_set.insert (Ipv4Address ("10.0.0.4"));
    expected_set.insert (Ipv4Address ("10.0.0.5"));

    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesCount (), 5u, "Must be 5.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesSet (), expected_set, "Must be equal.");

    entry.ClearKnownCarriersNodes ();
    expected_set.clear ();

    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesCount (), 0u, "Must be zero.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetKnownCarrierNodesSet (), expected_set, "Must be equal.");
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

    m_queue_entry = PacketQueueEntry (header, m_replicas);

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
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 83, "
            "is known by 0 nodes and has 3 replicas available.";

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);

    m_queue_entry.SetExpirationTime (Seconds (28.6)); // Will expire at second 42.42 (13.82 + 28.6)
  }

  void
  TestToStringFunction_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 42.42
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 83, "
            "is known by 7 nodes and has 0 replicas available.";

    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.4"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.5"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.6"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.7"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.7"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.7"));

    m_queue_entry.SetReplicasCounter (0);

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);

    m_queue_entry.SetExpirationTime (30u, 43u); // Will expire at second 73 (30 + 43)
    m_queue_entry.SetReplicasCounter (11);

    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.10"));
    m_queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.11"));
  }

  void
  TestToStringFunction_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 61.9
    std::string expected_str = "Packet queue entry 1.1.1.1:1 will expire at second 73, "
            "is known by 9 nodes and has 11 replicas available.";

    NS_TEST_EXPECT_MSG_EQ (m_queue_entry.ToString (), expected_str, "Expected string: " << expected_str);
  }

  void
  TestToStringFunction ()
  {
    DataHeader header (m_header);
    header.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (37), Seconds (83)),
                                                           Area (10, 10, 90, 90)));

    m_queue_entry = PacketQueueEntry (header, m_replicas);

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
    PacketQueueEntry equal_1 (m_header, m_replicas);
    PacketQueueEntry equal_2 (m_header, m_replicas);

    equal_1.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    equal_1.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    equal_1.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    equal_2.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    equal_2.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    equal_2.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    DataHeader diff_header (m_header);
    diff_header.SetHopsCount (0u);
    PacketQueueEntry different (diff_header, m_replicas);

    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different expiration time
    different = PacketQueueEntry (m_header, m_replicas);

    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    different.SetExpirationTime (Seconds (400));

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different replicas counter
    different = PacketQueueEntry (m_header, m_replicas);

    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    different.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    different.SetReplicasCounter (999u);

    TestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different known carrier nodes
    different = PacketQueueEntry (m_header, m_replicas);

    different.AddKnownCarrierNode (Ipv4Address ("3.3.3.1"));
    different.AddKnownCarrierNode (Ipv4Address ("3.3.3.2"));
    different.AddKnownCarrierNode (Ipv4Address ("3.3.3.3"));

    TestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGetSetReplicasCounter ();
    TestKnownCarrierNodes ();
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
 * \ingroup geotemporal-test
 */
class PacketsQueueTest : public TestCasePlus
{
public:

  // Murcia's GPS
  std::string m_murcia_streets_graph_filename;
  std::string m_murcia_vehicles_routes_filename;
  std::string m_murcia_street_junctions_filename;

  // Luxembourg's GPS
  std::string m_lux_streets_graph_filename;
  std::string m_lux_vehicles_routes_filename;
  std::string m_lux_street_junctions_filename;

  Ptr<GpsSystem> m_gps;
  PacketsQueue m_packets_queue;

  PacketsQueueTest ()
  : TestCasePlus ("PacketsQueue"),
  m_murcia_streets_graph_filename ("src/geotemporal/test/Murcia.graph.txt"),
  m_murcia_vehicles_routes_filename ("src/geotemporal/test/Murcia.routes.txt"),
  m_murcia_street_junctions_filename ("src/geotemporal/test/Murcia.junctions.txt"),
  m_lux_streets_graph_filename ("src/geotemporal/test/Luxembourg.graph.txt"),
  m_lux_vehicles_routes_filename ("src/geotemporal/test/Luxembourg.routes.txt"),
  m_lux_street_junctions_filename ("src/geotemporal/test/Luxembourg.junctions.txt"),
  m_gps (0u),
  m_packets_queue ()
  {
    m_gps = Create<GpsSystem> (m_murcia_streets_graph_filename,
                               m_murcia_vehicles_routes_filename,
                               m_murcia_street_junctions_filename);
  }

  void
  TestConstructors ()
  {
    std::set<DataIdentifier> summary_vector;

    // Default constructor
    PacketsQueue q1;
    q1.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q1.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetMaxLength (), 128u, "Must be 128.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetMaxReplicasCounter (), 3u, "Must be 3.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q1.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");
    NS_TEST_EXPECT_MSG_EQ (q1.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q1.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Parameters constructor
    PacketsQueue q2 (m_gps, 17u, 8u);
    q2.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q2.GetGpsSystem (), m_gps, "Must be equal.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetMaxLength (), 17, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q2.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");
    NS_TEST_EXPECT_MSG_EQ (q2.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
    NS_TEST_EXPECT_MSG_EQ (q2.GetPacketReceptionStats ().empty (), true, "Must be empty.");

    // Copy constructor
    PacketsQueue q3 (q2);
    q3.GetSummaryVector (summary_vector);

    NS_TEST_EXPECT_MSG_EQ (q3.GetGpsSystem (), m_gps, "Must be equal.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetMaxLength (), 17, "Must be 17.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");
    NS_TEST_EXPECT_MSG_EQ (q3.Size (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (q3.GetPacketReceptionStats ().empty (), true, "Must be empty.");
    NS_TEST_EXPECT_MSG_EQ (summary_vector.empty (), true, "Must be empty");
  }

  void
  TestGettersSetters ()
  {
    m_packets_queue = PacketsQueue (m_gps, 67, 8u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), m_gps, "Must be equal.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67, "Must be 67.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    Ptr<GpsSystem> lux_gps = Create<GpsSystem> (m_lux_streets_graph_filename,
                                                m_lux_vehicles_routes_filename,
                                                m_lux_street_junctions_filename);
    m_packets_queue.SetGpsSystem (lux_gps);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), lux_gps, "Must be equal.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67, "Must be 67.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    m_packets_queue.ClearGpsSystem ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 67, "Must be 67.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    m_packets_queue.SetMaxLength (11u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 11, "Must be 11.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 8, "Must be 8.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    m_packets_queue.SetMaxReplicasCounter (6);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 11, "Must be 11.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 6, "Must be 6.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    m_packets_queue.SetMaxLength (1u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 1, "Must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 6, "Must be 6.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.2.3.4:5"),
                            /*Hops count*/ 65u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 123.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60)),
                            /*Message*/ "packet's message");

    Ipv4Address source_ip ("1.1.1.1");
    m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                             /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                             /*Transmitter node*/ source_ip);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 0u, "Must be 0.");

    for (uint32_t i = 1u; i <= 100; ++i)
      {
        data_packet.SetDataIdentifier (DataIdentifier (source_ip, i));

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ source_ip);

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), i, "Must be " << i);
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 1, "Must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 6, "Must be 6.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 100, "Must be 100.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 20u, "Must be 20.");

    m_packets_queue.SetMinVehiclesDistanceDifference (36);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetGpsSystem (), 0, "Must be 0.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxLength (), 1, "Must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMaxReplicasCounter (), 6, "Must be 6.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetDroppedPacketsCounter (), 100, "Must be 100.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetMinVehiclesDistanceDifference (), 36, "Must be 36.");
  }

  void
  TestGetSize ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Size of the packets queue must be 0.");

    Ipv4Address source_ip = Ipv4Address ("1.1.1.1");

    DataHeader data_packet (/*Data ID*/ DataIdentifier (source_ip, 0u),
                            /*Hops count*/ 65u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 123.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (0, 0, 100, 100)),
                            /*Message*/ "packet's message");

    for (uint32_t i = 0u; i < 75u; ++i)
      {
        data_packet.SetDataIdentifier (DataIdentifier (source_ip, i));

        m_packets_queue.m_packets_table.insert (std::make_pair (data_packet.GetDataIdentifier (),
                                                                PacketQueueEntry (data_packet, 3)));

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u + i,
                               "Size of the packets queue must be " << (1u + i));
      }

    for (uint32_t i = 0u; i < 75u; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.erase (DataIdentifier (source_ip, i)),
                               1u, "One element must have been erased");

        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 75 - (i + 1),
                               "Size of the packets queue must be " << (1u + i));
      }
  }

  void
  TestGetSummaryVector ()
  {
    std::set<DataIdentifier> summary_vector, expected_summary_vector;
    m_packets_queue = PacketsQueue (m_gps, 5u, 3u);

    // Test when packets queue is empty
    m_packets_queue.GetSummaryVector (summary_vector);
    NS_TEST_EXPECT_MSG_EQ (summary_vector.size (), 0u, "Summary vector must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 65u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 123.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (0, 0, 100, 100)),
                            /*Message*/ "packet's message");

    m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                             /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                             /*Transmitter node*/ Ipv4Address ("1.1.1.1"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 10
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (2), Seconds (10)),
                                                                Area (0, 0, 100, 100)));

    m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                             /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                             /*Transmitter node*/ Ipv4Address ("1.1.1.2"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));

    m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                             /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                             /*Transmitter node*/ Ipv4Address ("1.1.1.3"));

    m_packets_queue.GetSummaryVector (summary_vector);
    expected_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.2:2"),
      DataIdentifier ("1.1.1.3:3")};
    NS_TEST_EXPECT_MSG_EQ (summary_vector, expected_summary_vector, "Summary vector must be the expected.");

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                             /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                             /*Transmitter node*/ Ipv4Address ("1.1.1.4"));

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
  }

  void
  TestFindFunctions ()
  {
    bool found = false;
    PacketQueueEntry entry;
    DataIdentifier data_id;
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    m_packets_queue = PacketsQueue (m_gps, 5, 3u);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");

    // New entry expires at second 10
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)), Area (-15, 0, 30, 30)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.1"));

    // New entry expires at second 10
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (2), Seconds (10)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.2"));

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.3"));

    // New entry expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.4"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -           second 10
    //    1.1.1.2:2  -           second 10
    //    1.1.1.3:3  -           second 5
    //    1.1.1.4:4  -           second 5

    // Test bool Find (const DataIdentifier &, PacketQueueEntry &)
    // - Successfully found
    data_id = DataIdentifier ("1.1.1.1:1");
    found = m_packets_queue.Find (data_id, entry);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.1:1 must be found.");
    NS_TEST_EXPECT_MSG_EQ (entry.GetDataPacketId (), data_id, "Entry must have data ID " << data_id);
    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Packet queue entry must have 10 seconds of expiration time.");

    // - Expected not found
    data_id = DataIdentifier ("1.1.1.1:2");
    found = m_packets_queue.Find (data_id, entry);

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.1:2 must not be found.");

    // Test bool Find (const DataIdentifier &);
    // - Successfully found
    data_id = DataIdentifier ("1.1.1.2:2");
    found = m_packets_queue.Find (data_id);

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.2:2 must be found.");

    // - Expected not found
    data_id = DataIdentifier ("1.1.1.2:1");
    found = m_packets_queue.Find (data_id);

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.2:1 must not be found.");

    // Test bool Find (const PacketQueueEntry &);
    // - Successfully found
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    found = m_packets_queue.Find (PacketQueueEntry (data_packet, 3));

    NS_TEST_EXPECT_MSG_EQ (found, true, "Packet queue entry 1.1.1.3:3 must be found.");

    // - Expected not found
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:1"));
    found = m_packets_queue.Find (PacketQueueEntry (data_packet, 3));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.4:1 must not be found.");
  }

  void
  TestClear ()
  {
    m_packets_queue = PacketsQueue (m_gps, 15, 3);

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 65u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 123.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (0, 0, 100, 100)),
                            /*Message*/ "packet's message");

    Ipv4Address ip ("1.1.1.1");

    for (uint32_t id = 0; id < 10; ++id)
      {
        data_packet.SetDataIdentifier (DataIdentifier (ip, id));

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ Ipv4Address ("1.1.1.1"));
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

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ Ipv4Address ("1.1.1.2"));
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

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ Ipv4Address ("1.1.1.3"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 15, "Must be 15");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 50, "Must be 50");

    m_packets_queue.Clear ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 50, "Must be 50");
  }

  void
  TestProcessDisjointVector ()
  {
    std::set<DataIdentifier> received_summary_vector, local_unknown_packets, expected_local_unknown_packets,
            neighbor_unknown_packets, expected_neighbor_unknown_packets;
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

    // Test with an empty packets queue and an empty summary vector

    m_packets_queue.ProcessDisjointVectors (/*Recv SV*/ received_summary_vector,
                                            /*Local IP*/ Ipv4Address ("1.1.1.1"),
                                            /*Neighbor IP*/ Ipv4Address ("2.2.2.2"),
                                            /*Locally unknown packets*/ local_unknown_packets,
                                            /*Neighbor unknown packets*/ neighbor_unknown_packets);

    NS_TEST_EXPECT_MSG_EQ (local_unknown_packets, expected_local_unknown_packets, "Must be the expected.");
    NS_TEST_EXPECT_MSG_EQ (neighbor_unknown_packets, expected_neighbor_unknown_packets, "Must be the expected.");

    // Test with an empty packets queue and a non-empty summary vector.

    received_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"), DataIdentifier ("9.9.9.9:3")};

    m_packets_queue.ProcessDisjointVectors (/*Recv SV*/ received_summary_vector,
                                            /*Local IP*/ Ipv4Address ("1.1.1.1"),
                                            /*Neighbor IP*/ Ipv4Address ("2.2.2.2"),
                                            /*Locally unknown packets*/ local_unknown_packets,
                                            /*Neighbor unknown packets*/ neighbor_unknown_packets);

    expected_local_unknown_packets = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};
    expected_neighbor_unknown_packets.clear ();

    NS_TEST_EXPECT_MSG_EQ (local_unknown_packets, expected_local_unknown_packets, "Must be the expected.");
    NS_TEST_EXPECT_MSG_EQ (neighbor_unknown_packets, expected_neighbor_unknown_packets, "Must be the expected.");

    // Test with a non-empty packets queue and an empty summary vector.

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 65u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 123.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (0, 0, 100, 100)),
                            /*Message*/ "packet's message");

    std::set<DataIdentifier> to_enqueue = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3"), DataIdentifier ("2.2.2.2:1"), DataIdentifier ("2.2.2.2:2")};

    for (std::set<DataIdentifier>::const_iterator it = to_enqueue.begin ();
            it != to_enqueue.end (); ++it)
      {
        data_packet.SetDataIdentifier (*it);

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ Ipv4Address ("8.8.8.8"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 5u, "Size of the packets queue must be 5.");

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     2. 2. 2. 2: 1  -           second 10
    //     2. 2. 2. 2: 2  -           second 10
    //     9. 9. 9. 9: 1  -           second 10
    //     9. 9. 9. 9: 2  -           second 10
    //     9. 9. 9. 9: 3  -           second 10

    received_summary_vector.clear ();

    m_packets_queue.ProcessDisjointVectors (/*Recv SV*/ received_summary_vector,
                                            /*Local IP*/ Ipv4Address ("1.1.1.1"),
                                            /*Neighbor IP*/ Ipv4Address ("2.2.2.2"),
                                            /*Locally unknown packets*/ local_unknown_packets,
                                            /*Neighbor unknown packets*/ neighbor_unknown_packets);

    expected_local_unknown_packets.clear ();
    expected_neighbor_unknown_packets = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};

    NS_TEST_EXPECT_MSG_EQ (local_unknown_packets, expected_local_unknown_packets, "Must be the expected.");
    NS_TEST_EXPECT_MSG_EQ (neighbor_unknown_packets, expected_neighbor_unknown_packets, "Must be the expected.");

    // Test with a non-empty packets queue and a non-empty summary vector.

    to_enqueue = {DataIdentifier ("7.7.7.7:1"), DataIdentifier ("7.7.7.7:2"),
      DataIdentifier ("7.7.7.7:3")};

    for (std::set<DataIdentifier>::const_iterator it = to_enqueue.begin ();
            it != to_enqueue.end (); ++it)
      {
        data_packet.SetDataIdentifier (*it);

        m_packets_queue.Enqueue (/*Data packet*/ data_packet,
                                 /*Current position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                                 /*Transmitter node*/ Ipv4Address ("8.8.8.8"));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 8u, "Size of the packets queue must be 5.");

    // The packets queue now looks like this:
    //        Data ID     -   Packet entry expiration time
    //     2. 2. 2. 2: 1  -           second 10
    //     2. 2. 2. 2: 2  -           second 10
    //     9. 9. 9. 9: 1  -           second 10
    //     9. 9. 9. 9: 2  -           second 10
    //     9. 9. 9. 9: 3  -           second 10
    //     7. 7. 7. 7: 1  -           second 10
    //     7. 7. 7. 7: 2  -           second 10
    //     7. 7. 7. 7: 3  -           second 10

    received_summary_vector = {DataIdentifier ("1.1.1.1:1"), DataIdentifier ("1.1.1.1:2"),
      DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"), DataIdentifier ("9.9.9.9:3"),
      DataIdentifier ("8.8.8.8:1"), DataIdentifier ("8.8.8.8:2"), DataIdentifier ("8.8.8.8:3")};

    m_packets_queue.ProcessDisjointVectors (/*Recv SV*/ received_summary_vector,
                                            /*Local IP*/ Ipv4Address ("1.1.1.1"),
                                            /*Neighbor IP*/ Ipv4Address ("2.2.2.2"),
                                            /*Locally unknown packets*/ local_unknown_packets,
                                            /*Neighbor unknown packets*/ neighbor_unknown_packets);

    expected_local_unknown_packets = {DataIdentifier ("8.8.8.8:1"), DataIdentifier ("8.8.8.8:2"),
      DataIdentifier ("8.8.8.8:3")};
    expected_neighbor_unknown_packets = {DataIdentifier ("7.7.7.7:1"), DataIdentifier ("7.7.7.7:2"),
      DataIdentifier ("7.7.7.7:3")};

    NS_TEST_EXPECT_MSG_EQ (local_unknown_packets, expected_local_unknown_packets, "Must be the expected.");
    NS_TEST_EXPECT_MSG_EQ (neighbor_unknown_packets, expected_neighbor_unknown_packets, "Must be the expected.");
  }

  void
  TestComparePacketTransmissionPriority ()
  {
    std::pair<DataIdentifier, PacketQueueEntry> lower_priority, higher_priority;

    // Test with different hops count: lower = 23 hops vs higher = 22 hops

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.2.3.4:5"),
                            /*Hops count*/ 23u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (30, 40, 50, 60)),
                            /*Message*/ "packet's message");
    PacketQueueEntry queue_entry (data_packet, 3u);

    lower_priority = std::make_pair (DataIdentifier ("1.1.1.1:1"), queue_entry);

    data_packet.SetHopsCount (22u);
    queue_entry = PacketQueueEntry (data_packet, 3u);

    higher_priority = std::make_pair (DataIdentifier ("1.1.1.1:1"), queue_entry);

    NS_TEST_EXPECT_MSG_EQ (lower_priority.second.GetDataPacket ().GetHopsCount (), 23u, "Must be 23");
    NS_TEST_EXPECT_MSG_EQ (higher_priority.second.GetDataPacket ().GetHopsCount (), 22u, "Must be 22");
    NS_TEST_EXPECT_MSG_EQ (lower_priority.second.GetKnownCarrierNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (higher_priority.second.GetKnownCarrierNodesCount (), 0u, "Must be 0");

    NS_TEST_EXPECT_MSG_EQ (PacketsQueue::ComparePacketTransmissionPriority (lower_priority, higher_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (PacketsQueue::ComparePacketTransmissionPriority (higher_priority, lower_priority),
                           false, "Must be false");

    // Test with different known carriers count: lower = 4 carriers vs higher = 3 carriers

    data_packet.SetHopsCount (10u);
    queue_entry = PacketQueueEntry (data_packet, 3u);

    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));
    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.4"));

    lower_priority = std::make_pair (DataIdentifier ("1.1.1.1:1"), queue_entry);

    queue_entry.ClearKnownCarriersNodes ();

    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.1"));
    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.2"));
    queue_entry.AddKnownCarrierNode (Ipv4Address ("1.1.1.3"));

    higher_priority = std::make_pair (DataIdentifier ("1.1.1.1:1"), queue_entry);

    NS_TEST_EXPECT_MSG_EQ (lower_priority.second.GetDataPacket ().GetHopsCount (), 10u, "Must be 10");
    NS_TEST_EXPECT_MSG_EQ (higher_priority.second.GetDataPacket ().GetHopsCount (), 10u, "Must be 10");
    NS_TEST_EXPECT_MSG_EQ (lower_priority.second.GetKnownCarrierNodesCount (), 4u, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (higher_priority.second.GetKnownCarrierNodesCount (), 3u, "Must be 3");

    NS_TEST_EXPECT_MSG_EQ (PacketsQueue::ComparePacketTransmissionPriority (lower_priority, higher_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (PacketsQueue::ComparePacketTransmissionPriority (higher_priority, lower_priority),
                           false, "Must be false");

    // Test with equal hops count and known carriers count

    NS_TEST_EXPECT_MSG_EQ (PacketsQueue::ComparePacketTransmissionPriority (higher_priority, higher_priority),
                           false, "Must be false");
  }

  void
  TestFindHighestDropPriorityPacket ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

    DataIdentifier selected_packet;
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    // With an empty queue must return false

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           false, "Must be false");

    // With only one item in the queue must return it

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)), Area (-15, 0, 30, 30)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("0.0.0.0"));

    // - Outside of the geo-temporal area
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (40, 40);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, m_packets_queue.m_packets_table.begin ()->first,
                           "Must be the same data ID");

    // - Inside of the geo-temporal area
    selected_packet = DataIdentifier ();
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 15);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, m_packets_queue.m_packets_table.begin ()->first,
                           "Must be the same data ID");

    // With multiple messages it must select the one with highest priority

    // - 2 queue entries
    data_packet.SetDataIdentifier (DataIdentifier ("2.2.2.2:2"));
    data_packet.SetHopsCount (5);
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)), Area (-20, 10, 25, 40)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("0.0.0.0"));

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");

    //   > Outside both areas
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (-15, -15);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("1.1.1.1:1"), "Must be the expected");

    //   > Inside both areas
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 15);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("1.1.1.1:1"), "Must be the expected");

    //   > Inside area 1
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 5);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("2.2.2.2:2"), "Must be the expected");

    //   > Inside area 2
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 35);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("1.1.1.1:1"), "Must be the expected");

    // - 3 queue entries

    data_packet.SetDataIdentifier (DataIdentifier ("3.3.3.3:3"));
    data_packet.SetHopsCount (18);
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)), Area (-10, 20, 30, 50)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("0.0.0.0"));

    char buffer [25];
    for (uint32_t i = 1; i <= 10; ++i)
      {
        std::sprintf (buffer, "1.1.1.%u", i);
        m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("3.3.3.3:3"), Ipv4Address (buffer));
      }

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("3.3.3.3:3")).GetKnownCarrierNodesCount (), 10, "Must be 10");

    //   > Outside all areas
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (-15, -15);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("3.3.3.3:3"), "Must be the expected");

    //   > Inside all areas
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 25);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("3.3.3.3:3"), "Must be the expected");

    //   > Inside area 1
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 5);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("3.3.3.3:3"), "Must be the expected");

    //   > Inside area 2
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (-15, 35);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("3.3.3.3:3"), "Must be the expected");

    //   > Inside area 3
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 45);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("1.1.1.1:1"), "Must be the expected");

    //   > Inside area 1 & 2
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (-5, 15);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("3.3.3.3:3"), "Must be the expected");

    //   > Inside area 1 & 3
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (28, 25);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("2.2.2.2:2"), "Must be the expected");

    //   > Inside area 2 & 3
    node_position = GeoTemporalLibrary::LibraryUtils::Vector2D (15, 35);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestDropPriorityPacket (node_position, selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet, DataIdentifier ("1.1.1.1:1"), "Must be the expected");
  }

  void
  TestEnqueueFunction_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 16.00
    bool enqueued;
    PacketQueueEntry entry;
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    1.1.1.1:1  -            Expired
    //    1.1.1.2:2  -            Expired
    //    1.1.1.5:5  -           second 17

    // Purge expired entries.
    m_packets_queue.Purge ();

    // The following entry to be inserted used to exist but was previously 
    // dropped, so it is inserted as newly inserted.
    // Successful insertion of new entry that expires at second 20
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.3:3"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (10), Seconds (20)), Area (-15, 0, 30, 30)),
                            /*Message*/ "packet's message");
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.3"));

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
    NS_TEST_EXPECT_MSG_EQ (stats_it->second.GetPacketDropped (), true,
                           "Must be true");
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
    m_packets_queue = PacketsQueue (m_gps, 3u, 3u);
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 0u, "Packets queue must be empty.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().empty (), true,
                           "Size of the received packets statistics must be empty.");

    bool enqueued, found;
    PacketQueueEntry entry;
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    // Successful insertion of new entry that expires at second 10
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)), Area (-15, 0, 30, 30)),
                            /*Message*/ "packet's message");
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.1"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Hops count  -  Known carriers count
    //    1.1.1.1:1  -     second 10     -      15      -           0

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.1:1 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u, "Size of the packets queue must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 1u,
                           "Size of the received packets statistics must be 1.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.1:1"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (10), MicroSeconds (1),
                               "Packet queue entry 1.1.1.1:1 expiration time must be 10 seconds.");

    // Expected failure of already inserted entry
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.1"));

    NS_TEST_EXPECT_MSG_EQ (enqueued, false, "Packet queue entry 1.1.1.1:1 must have not been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 1u, "Size of the packets queue must be 1.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 1u,
                           "Size of the received packets statistics must be 1.");

    // Successful insertion of new entry that expires at second 15
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.2:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (3), Seconds (15)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.2"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Hops count  -  Known carriers count
    //    1.1.1.1:1  -     second 10     -      15      -           0
    //    1.1.1.2:2  -     second 15     -      15      -           0

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.2:2 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 2u,
                           "Size of the received packets statistics must be 2.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.2:2"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (15), MicroSeconds (1),
                               "Packet queue entry 1.1.1.2:2 expiration time must be 15 seconds.");

    // Expected failure of already inserted entry
    // We execute twice the Enqueue call to count 2 received duplicates.
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.2"));
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.2")); // Not an accidentally duplicated line

    NS_TEST_EXPECT_MSG_EQ (enqueued, false, "Packet queue entry 1.1.1.2:2 must have not been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 2u, "Size of the packets queue must be 2.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 2u,
                           "Size of the received packets statistics must be 2.");

    // Successful insertion of new entry that expires at second 5
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.3:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (5)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.3"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Hops count  -  Known carriers count
    //    1.1.1.1:1  -     second 10     -      15      -           0
    //    1.1.1.2:2  -     second 15     -      15      -           0
    //    1.1.1.3:3  -     second  5     -      15      -           0

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.3:3 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 3u,
                           "Size of the received packets statistics must be 3.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (5), MicroSeconds (1),
                               "Packet queue entry 1.1.1.3:3 expiration time must be 5 seconds.");

    // Test that when the queue is full the packet with maximum drop priority 
    // is dropped and the new entry is successfully inserted.
    // This new packet expires at second 9.
    // Packet 1.1.1.3:3 is dropped.
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.4:4"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (9)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.4"));

    // The packets queue now looks like this:
    //     Data ID   -  Expiration time  -  Hops count  -  Known carriers count
    //    1.1.1.1:1  -     second 10     -      15      -           0
    //    1.1.1.3:3  -     second  5     -      15      -           0
    //    1.1.1.4:4  -     second  9     -      15      -           0

    NS_TEST_EXPECT_MSG_EQ (enqueued, true, "Packet queue entry 1.1.1.4:4 must have been enqueued.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Size (), 3u, "Size of the packets queue must be 3.");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.GetPacketReceptionStats ().size (), 4u,
                           "Size of the received packets statistics must be 4.");

    m_packets_queue.Find (DataIdentifier ("1.1.1.4:4"), entry);

    NS_TEST_EXPECT_MSG_EQ_TOL (entry.GetExpirationTime (), Seconds (9), MicroSeconds (1),
                               "Packet queue entry 1.1.1.4:4 expiration time must be 9 seconds.");

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.2:2"));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.2:2 must not be found.");

    // Insert another packet entry into the already full queue.
    // This new packet expires at second 17.
    // Packet 1.1.1.4:4 is dropped.
    data_packet.SetDataIdentifier (DataIdentifier ("1.1.1.5:5"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (17)),
                                                                Area (0, 0, 100, 100)));
    enqueued = m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.5"));

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

    found = m_packets_queue.Find (DataIdentifier ("1.1.1.3:3"));

    NS_TEST_EXPECT_MSG_EQ (found, false, "Packet queue entry 1.1.1.3:3 must not be found.");

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
  TestFindHighestTransmitPriorityPacket_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 2.89

    const Time current_time = Simulator::Now ();
    const uint32_t current_second = static_cast<uint32_t> (current_time.GetSeconds ());
    NS_TEST_ASSERT_MSG_EQ (current_second, 2u, "Must be 2");

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 10
    //    9.9.9.9:3  -           second 10

    Ipv4Address local_node_ip ("1.1.1.1");
    Ipv4Address neighbor_node_ip ("2.2.2.2");

    GeoTemporalLibrary::LibraryUtils::Vector2D local_position, local_velocity,
            neighbor_position, neighbor_velocity;

    std::set<DataIdentifier> disjoint_vector;

    PacketQueueEntry selected_packet;
    bool selected_packet_high_priority = false;

    disjoint_vector = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};

    // ------------------
    // Destination node inside & Local node outside
    // ------------------

    // Destination node inside area 1
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2164.35, 2044.71);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside area 2
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2197.98, 2037.60);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside area 3
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.21, 2010.56);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1 & 2
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.48, 2040.18);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1 & 3
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.85, 2029.84);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 2 & 3
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2180.25, 2020.01);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1, 2 & 3
    selected_packet_high_priority = false;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.87, 2029.58);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // ------------------
    // Local node inside & Destination node outside
    // ------------------

    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D ();

    // Local node inside area 1
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2164.35, 2044.71);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside area 2
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2197.98, 2037.60);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside area 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.21, 2010.56);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1 & 2
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.48, 2040.18);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.85, 2029.84);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 2 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2180.25, 2020.01);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside areas 1, 2 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.87, 2029.58);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // ------------------
    // Both nodes (local & destination) inside
    // ------------------

    // Both nodes inside area 1
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2164.35, 2044.71);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2164.35, 2044.71);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside area 2
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2197.98, 2037.60);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2197.98, 2037.60);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside area 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.21, 2010.56);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.21, 2010.56);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside areas 1 & 2
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.48, 2040.18);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.48, 2040.18);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside areas 1 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.85, 2029.84);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.85, 2029.84);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside areas 2 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2180.25, 2020.01);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2180.25, 2020.01);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Both nodes inside areas 1, 2 & 3
    selected_packet_high_priority = false;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.87, 2029.58);
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2179.87, 2029.58);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // ------------------
    // Both nodes (local & destination) outside
    // ------------------

    // Both nodes outside all areas with valid receiver node
    local_node_ip = Ipv4Address ("2.2.2.2");
    neighbor_node_ip = Ipv4Address ("1.1.1.1"); // Valid receiver node

    selected_packet_high_priority = true;
    local_position = GeoTemporalLibrary::LibraryUtils::Vector2D ();
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, false, "Must be false");

    // Both nodes outside all areas with invalid receiver node
    local_node_ip = Ipv4Address ("1.1.1.1");
    neighbor_node_ip = Ipv4Address ("2.2.2.2"); // Invalid receiver node

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           false, "Must be false");

    // Set replicas counter of the packet with highest priority to zero
    m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:2")).SetReplicasCounter (0);

    // Both nodes outside all areas with valid receiver node
    local_node_ip = Ipv4Address ("2.2.2.2");
    neighbor_node_ip = Ipv4Address ("1.1.1.1"); // Valid receiver node

    selected_packet_high_priority = true;
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:1"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, false, "Must be false");

    // Both nodes outside all areas with invalid receiver node
    local_node_ip = Ipv4Address ("1.1.1.1");
    neighbor_node_ip = Ipv4Address ("2.2.2.2"); // Invalid receiver node

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           false, "Must be false");

    // Set replicas counter of the packet with highest priority to 10
    m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:2")).SetReplicasCounter (10);

    // Destination node inside area 3 with valid receiver node
    local_node_ip = Ipv4Address ("2.2.2.2");
    neighbor_node_ip = Ipv4Address ("1.1.1.1"); // Valid receiver node
    selected_packet_high_priority = false;

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (2180.76, 2008.28),
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Destination node inside area 3 with invalid receiver node
    local_node_ip = Ipv4Address ("1.1.1.1");
    neighbor_node_ip = Ipv4Address ("2.2.2.2"); // Invalid receiver node
    selected_packet_high_priority = false;

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (2180.76, 2008.28),
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Set replicas counter of packet 3 to 0
    m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:3")).SetReplicasCounter (0);

    // Local node inside area 3 with valid receiver node
    local_node_ip = Ipv4Address ("2.2.2.2");
    neighbor_node_ip = Ipv4Address ("1.1.1.1"); // Valid receiver node
    selected_packet_high_priority = false;

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (2180.76, 2008.28),
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");

    // Local node inside area 3 with invalid receiver node
    local_node_ip = Ipv4Address ("1.1.1.1");
    neighbor_node_ip = Ipv4Address ("2.2.2.2"); // Invalid receiver node
    selected_packet_high_priority = false;

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (2180.76, 2008.28),
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (selected_packet_high_priority, true, "Must be true");
  }

  void
  TestFindHighestTransmitPriorityPacket ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

    Ipv4Address local_node_ip ("1.1.1.1");
    Ipv4Address neighbor_node_ip ("2.2.2.2");

    GeoTemporalLibrary::LibraryUtils::Vector2D local_position, local_velocity,
            neighbor_position, neighbor_velocity, node_position;

    std::set<DataIdentifier> disjoint_vector;

    PacketQueueEntry selected_packet;
    bool selected_packet_high_priority;

    m_gps->SetNodeIpAddressToIdMapping ({
      { local_node_ip, 20},
      { neighbor_node_ip, 21},
    });

    // Test with empty disjoint vector & empty packets queue

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           false, "Must be false");

    // Test with non-empty disjoint vector & empty packets queue

    disjoint_vector = {DataIdentifier ("0.0.0.0:0"), DataIdentifier ("0.0.0.0:1"), DataIdentifier ("0.0.0.0:2")};

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           false, "Must be false");

    // Insert 3 packets to the packets queue

    // Geo-temporal area 1
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Geo-temporal area 2
    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (10)),
                                                                Area (2175.00, 2045.00, 2205.00, 2015.00)));
    data_packet.SetHopsCount (5);
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Geo-temporal area 3
    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (10)),
                                                                Area (2155.00, 2035.00, 2185.00, 2005.00)));
    data_packet.SetHopsCount (18);
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 10
    //    9.9.9.9:3  -           second 10

    // Test with non-empty disjoint vector full of non-existent packets & non-empty packets queue

    disjoint_vector = {DataIdentifier ("0.0.0.0:0"), DataIdentifier ("0.0.0.0:1"), DataIdentifier ("0.0.0.0:2")};

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.FindHighestTransmitPriorityPacket (/*Local node IP*/ local_node_ip,
                                                                              /*Local node position*/ local_position,
                                                                              /*Local node velocity*/ local_velocity,
                                                                              /*Neighbor node IP*/ neighbor_node_ip,
                                                                              /*Neighbor node position*/ neighbor_position,
                                                                              /*Neighbor node velocity*/ neighbor_velocity,
                                                                              /*Disjoint vector*/ disjoint_vector,
                                                                              /*Selected packet*/ selected_packet,
                                                                              /*Selected packet high priority*/ selected_packet_high_priority),
                           false, "Must be false");

    Simulator::Schedule (Seconds (2.89), &PacketsQueueTest::TestFindHighestTransmitPriorityPacket_Scheduled_1, this);

    Simulator::Run ();
    Simulator::Destroy ();

    m_gps->ClearNodeIpAddressToIdMapping ();
  }

  void
  TestDequeue_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 2.99

    const Time current_time = Simulator::Now ();
    const uint32_t current_second = static_cast<uint32_t> (current_time.GetSeconds ());
    NS_TEST_ASSERT_MSG_EQ (current_second, 2u, "Must be 2");

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time  -  Replicas count
    //    9.9.9.9:1  -           second 10                      3
    //    9.9.9.9:2  -           second 10                      3
    //    9.9.9.9:3  -           second 10                      3

    Ipv4Address local_node_ip ("1.1.1.1");
    Ipv4Address neighbor_node_ip ("2.2.2.2");

    GeoTemporalLibrary::LibraryUtils::Vector2D local_position, local_velocity,
            neighbor_position, neighbor_velocity;

    std::set<DataIdentifier> disjoint_vector;

    PacketQueueEntry selected_packet;

    disjoint_vector = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};

    // Test when no packet to transmit must be found
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           false, "Must be false");

    // Test when a packet inside its destination geo-temporal area (area 3) has 
    // the higher priority
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2160.21, 2010.56);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:3")).GetReplicasCounter (),
                           3u, "Must be 3");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:3"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:3")).GetReplicasCounter (),
                           3u, "Must be 3");

    // Test when a packet not inside its destination geo-temporal area (area 2) has 
    // the higher priority
    disjoint_vector = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};

    local_node_ip = Ipv4Address ("2.2.2.2");
    neighbor_node_ip = Ipv4Address ("1.1.1.1"); // Valid receiver node
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:2")).GetReplicasCounter (),
                           3u, "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (selected_packet.GetDataPacketId (), DataIdentifier ("9.9.9.9:2"), "Must be the expected");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:2")).GetReplicasCounter (),
                           2u, "Must be 2");

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time  -  Replicas count
    //    9.9.9.9:1  -           second 10                      3
    //    9.9.9.9:2  -           second 10                      2
    //    9.9.9.9:3  -           second 10                      3
  }

  void
  TestDequeue_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 50.756

    const Time current_time = Simulator::Now ();
    const uint32_t current_second = static_cast<uint32_t> (current_time.GetSeconds ());
    NS_TEST_ASSERT_MSG_EQ (current_second, 50u, "Must be 50");

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time  -  Replicas count
    //    9.9.9.9:1  -           second 10                      3       <EXPIRED>
    //    9.9.9.9:2  -           second 10                      2       <EXPIRED>
    //    9.9.9.9:3  -           second 10                      3       <EXPIRED>

    // Purge expired entries.
    m_packets_queue.Purge ();

    Ipv4Address local_node_ip ("1.1.1.1");
    Ipv4Address neighbor_node_ip ("2.2.2.2");

    GeoTemporalLibrary::LibraryUtils::Vector2D local_position, local_velocity,
            neighbor_position, neighbor_velocity;

    std::set<DataIdentifier> disjoint_vector;

    PacketQueueEntry selected_packet;

    disjoint_vector = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("9.9.9.9:2"),
      DataIdentifier ("9.9.9.9:3")};

    // Test when a packet inside its destination geo-temporal area (area 1) has 
    // the higher priority
    neighbor_position = GeoTemporalLibrary::LibraryUtils::Vector2D (2164.35, 2044.71);

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           false, "Must be false");
  }

  void
  TestDequeue ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

    Ipv4Address local_node_ip ("1.1.1.1");
    Ipv4Address neighbor_node_ip ("2.2.2.2");

    GeoTemporalLibrary::LibraryUtils::Vector2D local_position, local_velocity,
            neighbor_position, neighbor_velocity, node_position;

    std::set<DataIdentifier> disjoint_vector;

    PacketQueueEntry selected_packet;

    m_gps->SetNodeIpAddressToIdMapping ({
      { local_node_ip, 20},
      { neighbor_node_ip, 21},
    });

    // Test with empty queue and empty disjoint vector

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           false, "Must be false");

    // Test with empty queue and non-empty disjoint vector

    disjoint_vector = {DataIdentifier ("0.0.0.0:0"), DataIdentifier ("0.0.0.0:1"), DataIdentifier ("0.0.0.0:2")};

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           false, "Must be false");

    // Insert 3 packets to the packets queue

    // Geo-temporal area 1
    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Geo-temporal area 2
    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (9)),
                                                                Area (2175.00, 2045.00, 2205.00, 2015.00)));
    data_packet.SetHopsCount (5);
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Geo-temporal area 3
    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (1), Seconds (9)),
                                                                Area (2155.00, 2035.00, 2185.00, 2005.00)));
    data_packet.SetHopsCount (18);
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 10
    //    9.9.9.9:3  -           second 10

    // Test with non-empty queue and empty disjoint vector

    disjoint_vector.clear ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Dequeue (/*Local node IP*/ local_node_ip,
                                                    /*Local node position*/ local_position,
                                                    /*Local node velocity*/ local_velocity,
                                                    /*Neighbor node IP*/ neighbor_node_ip,
                                                    /*Neighbor node position*/ neighbor_position,
                                                    /*Neighbor node velocity*/ neighbor_velocity,
                                                    /*Disjoint vector*/ disjoint_vector,
                                                    /*Selected packet*/ selected_packet),
                           false, "Must be false");

    Simulator::Schedule (Seconds (2.99), &PacketsQueueTest::TestDequeue_Scheduled_1, this);
    Simulator::Schedule (Seconds (50.756), &PacketsQueueTest::TestDequeue_Scheduled_2, this);

    Simulator::Run ();
    Simulator::Destroy ();

    m_gps->ClearNodeIpAddressToIdMapping ();
  }

  void
  TestDiscountPacketReplica ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 15u);

    PacketQueueEntry packet_entry;
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    // Test that it returns false for a non-existent data packet entry
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.DiscountPacketReplica (DataIdentifier ("9.9.9.9:1")),
                           false, "Replicas must not be discounted.");

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    m_packets_queue.Find (DataIdentifier ("9.9.9.9:1"), packet_entry);
    NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (), 15u, "Packet entry must have 15 replica(s).");

    // Test that it returns true when successfully discounted a replica
    for (uint32_t i = 0u, expected_replicas; i < 15u; ++i)
      {
        expected_replicas = 15u - (i + 1u);
        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.DiscountPacketReplica (DataIdentifier ("9.9.9.9:1")),
                               true, "Replicas must be discounted.");

        m_packets_queue.Find (DataIdentifier ("9.9.9.9:1"), packet_entry);
        NS_TEST_EXPECT_MSG_EQ (packet_entry.GetReplicasCounter (),
                               expected_replicas, "Packet entry must have " << expected_replicas << " replica(s).");
      }

    bool throwed_exception = false;

    try
      {
        m_packets_queue.DiscountPacketReplica (DataIdentifier ("9.9.9.9:1"));
      }
    catch (std::runtime_error & ex)
      {
        throwed_exception = true;
        NS_TEST_EXPECT_MSG_EQ (1, 1, "Must happen.");
      }
    catch (...)
      {
        throwed_exception = false;
        NS_TEST_EXPECT_MSG_EQ (1, 0, "Must never happen.");
      }

    NS_TEST_EXPECT_MSG_EQ (throwed_exception, true, "Right exception must have been thrown.");
  }

  void
  TestAddKnownPacketCarrier ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 15u);

    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;
    std::set<Ipv4Address> expected_known_carriers;

    // Test with empty packets queue

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.1")),
                           false, "Must be false");

    // Add packet to queue

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Test with existing packet

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet ().empty (),
                           true, "Must be empty");

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.1")),
                           true, "Must be true");

    expected_known_carriers = {Ipv4Address ("1.1.1.1")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");

    // - Add duplicated known carrier

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.1")),
                           true, "Must be true");

    expected_known_carriers = {Ipv4Address ("1.1.1.1")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");

    // - Add new known carriers

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.2")),
                           true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.3")),
                           true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.AddKnownPacketCarrier (DataIdentifier ("9.9.9.9:1"),
                                                                  Ipv4Address ("1.1.1.4")),
                           true, "Must be true");

    expected_known_carriers = {Ipv4Address ("1.1.1.1"), Ipv4Address ("1.1.1.2"),
      Ipv4Address ("1.1.1.3"), Ipv4Address ("1.1.1.4")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");
  }

  void
  TestAddKnownPacketCarriers ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 15u);

    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;
    std::set<Ipv4Address> expected_known_carriers;

    std::set<DataIdentifier> carried_packets = {DataIdentifier ("9.9.9.9:1"), DataIdentifier ("7.7.7.7:1")};

    // Add packet to queue

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // Test with existing packet

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet ().empty (),
                           true, "Must be empty");

    m_packets_queue.AddKnownPacketCarriers (carried_packets, Ipv4Address ("1.1.1.1"));

    expected_known_carriers = {Ipv4Address ("1.1.1.1")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");

    // - Add duplicated known carrier

    m_packets_queue.AddKnownPacketCarriers (carried_packets, Ipv4Address ("1.1.1.1"));

    expected_known_carriers = {Ipv4Address ("1.1.1.1")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");

    // - Add new known carriers

    m_packets_queue.AddKnownPacketCarriers (carried_packets, Ipv4Address ("1.1.1.2"));
    m_packets_queue.AddKnownPacketCarriers (carried_packets, Ipv4Address ("1.1.1.3"));
    m_packets_queue.AddKnownPacketCarriers (carried_packets, Ipv4Address ("1.1.1.4"));

    expected_known_carriers = {Ipv4Address ("1.1.1.1"), Ipv4Address ("1.1.1.2"),
      Ipv4Address ("1.1.1.3"), Ipv4Address ("1.1.1.4")};
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.at (DataIdentifier ("9.9.9.9:1")).GetKnownCarrierNodesSet (),
                           expected_known_carriers, "Must be the expected");
  }

  void
  TestPurge_Scheduled_1 ()
  {
    // This function is launched by the scheduler at second 3.55

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 5
    //    9.9.9.9:3  -           second 7.5

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 3, "Must be 3");
  }

  void
  TestPurge_Scheduled_2 ()
  {
    // This function is launched by the scheduler at second 6.2

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 5    <EXPIRED>
    //    9.9.9.9:3  -           second 7.5

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 2, "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:1")),
                           true, "Packet 9.9.9.9:1 must be found");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:2")),
                           false, "Packet 9.9.9.9:2 must NOT be found");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:3")),
                           true, "Packet 9.9.9.9:3 must be found");
  }

  void
  TestPurge_Scheduled_3 ()
  {
    // This function is launched by the scheduler at second 8.1

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:3  -           second 7.5  <EXPIRED>

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 1, "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:1")),
                           true, "Packet 9.9.9.9:1 must be found");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:3")),
                           false, "Packet 9.9.9.9:3 must NOT be found");
  }

  void
  TestPurge_Scheduled_4 ()
  {
    // This function is launched by the scheduler at second 12

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10   <EXPIRED>

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.Find (DataIdentifier ("9.9.9.9:1")),
                           false, "Packet 9.9.9.9:1 must NOT be found");
  }

  void
  TestPurge ()
  {
    m_packets_queue = PacketsQueue (m_gps, 100u, 15u);

    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    // Test with empty queue

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.empty (), true, "Must be empty");

    // Add some packets

    // - Packet 1

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("9.9.9.9:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // - Packet 2

    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:2"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (5)),
                                                                Area (2175.00, 2045.00, 2205.00, 2015.00)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // - Packet 3

    data_packet.SetDataIdentifier (DataIdentifier ("9.9.9.9:3"));
    data_packet.SetDestinationGeoTemporalArea (GeoTemporalArea (TimePeriod (Seconds (0), Seconds (7.5)),
                                                                Area (2155.00, 2035.00, 2185.00, 2005.00)));
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("9.9.9.9"));

    // The packets queue now looks like this:
    //     Data ID   -   Packet entry expiration time
    //    9.9.9.9:1  -           second 10
    //    9.9.9.9:2  -           second 5
    //    9.9.9.9:3  -           second 7.5

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 3, "Must be 3");

    m_packets_queue.Purge ();

    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.m_packets_table.size (), 3, "Must be 3");

    Simulator::Schedule (Seconds (3.55), &PacketsQueueTest::TestPurge_Scheduled_1, this);
    Simulator::Schedule (Seconds (6.2), &PacketsQueueTest::TestPurge_Scheduled_2, this);
    Simulator::Schedule (Seconds (8.1), &PacketsQueueTest::TestPurge_Scheduled_3, this);
    Simulator::Schedule (Seconds (12), &PacketsQueueTest::TestPurge_Scheduled_4, this);

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
    m_packets_queue = PacketsQueue (m_gps, 100u, 3u);

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
    GeoTemporalLibrary::LibraryUtils::Vector2D node_position;

    m_packets_queue = PacketsQueue (m_gps, 1u, 3u);

    std::string expected_str = "Packets queue has 0 / 1 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    m_packets_queue.SetMaxLength (17u);

    expected_str = "Packets queue has 0 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    DataHeader data_packet (/*Data ID*/ DataIdentifier ("1.1.1.1:1"),
                            /*Hops count*/ 15u,
                            /*Position*/ GeoTemporalLibrary::LibraryUtils::Vector2D (3, 4),
                            /*Velocity*/ GeoTemporalLibrary::LibraryUtils::Vector2D (-8.86, 12.098),
                            /*GeoTemporal Area*/ GeoTemporalArea (TimePeriod (Seconds (0), Seconds (10)),
                                                                  Area (2155.00, 2055.00, 2185.00, 2025.00)),
                            /*Message*/ "packet's message");
    m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.1"));

    expected_str = "Packets queue has 1 / 17 packet entries";
    NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                           "Expected string: " + expected_str);

    for (uint32_t i = 0u; i < 16u; ++i)
      {
        std::sprintf (buffer, "1.1.2.%u:%u", i, i);
        data_packet.SetDataIdentifier (DataIdentifier (std::string (buffer)));

        m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.1"));

        std::sprintf (buffer, "Packets queue has %u / 17 packet entries", (i + 2));
        expected_str = std::string (buffer);
        NS_TEST_EXPECT_MSG_EQ (m_packets_queue.ToString (), expected_str,
                               "Expected string: " + expected_str);
      }

    for (uint32_t i = 0u; i < 100u; ++i)
      {
        std::sprintf (buffer, "1.1.3.%u:%u", i, i);
        data_packet.SetDataIdentifier (DataIdentifier (std::string (buffer)));

        m_packets_queue.Enqueue (data_packet, node_position, Ipv4Address ("1.1.1.3"));
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
    TestFindFunctions ();
    TestClear ();
    TestProcessDisjointVector ();
    TestComparePacketTransmissionPriority ();
    TestFindHighestDropPriorityPacket ();
    TestEnqueueFunction ();
    TestFindHighestTransmitPriorityPacket ();
    TestDequeue ();
    TestDiscountPacketReplica ();
    TestAddKnownPacketCarrier ();
    TestAddKnownPacketCarriers ();
    TestPurge ();
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
 * \ingroup geotemporal-test
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
//                              GeoTemporalTestSuite
// =============================================================================

/**
 * Geo-Temporal test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-test
 */
class GeoTemporalTestSuite : public TestSuite
{
public:

  GeoTemporalTestSuite () : TestSuite ("geotemporal", UNIT)
  {
    AddTestCase (new PacketTypeTest, TestCase::QUICK);
    AddTestCase (new TypeHeaderTest, TestCase::QUICK);
    AddTestCase (new HelloHeaderTest, TestCase::QUICK);
    AddTestCase (new SummaryVectorHeaderTest, TestCase::QUICK);
    AddTestCase (new AckHeaderTest, TestCase::QUICK);
    AddTestCase (new DataHeaderTest, TestCase::QUICK);
    AddTestCase (new DataAckHeaderTest, TestCase::QUICK);
    AddTestCase (new NeighborEntryTest, TestCase::QUICK);
    AddTestCase (new NeighborsTableTest, TestCase::QUICK);
    AddTestCase (new PacketQueueEntryTest, TestCase::QUICK);
    AddTestCase (new PacketsQueueTest, TestCase::QUICK);
    AddTestCase (new DuplicatedPacketDetectorTest, TestCase::QUICK);
  }
};

static GeoTemporalTestSuite g_geotemporal_test_suite;


} // namespace test
} // namespace geotemporal
} // namespace ns3
