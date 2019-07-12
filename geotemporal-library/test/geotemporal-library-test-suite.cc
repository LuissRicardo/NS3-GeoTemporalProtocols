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

#include <ns3/geotemporal-library-module.h>

#include <ns3/ipv4-address.h>
#include <ns3/nstime.h>

#include <fstream>
#include <vector>
#include <map>
#include <utility>


using namespace GeoTemporalLibrary::LibraryUtils;
using namespace GeoTemporalLibrary::NavigationSystem;
using namespace ns3;

namespace TestUtils
{

/**
 * Deletes the file specified in <code>filename</code>.
 * @param filename Full name of the file to delete. Path, name and extension.
 */
static void
DeleteFile (const std::string & filename)
{
  std::cout << "Deleting generated temporal test file \"" << filename << "\"... ";
  if (remove (filename.c_str ()) != 0)
    {
      // Error deleting file
      std::cout << "Error!\n";
      NS_ABORT_MSG ("Fail!");
    }
  else
    {
      // File successfully deleted
      std::cout << "Successfully deleted.\n";
    }
}

/**
 * Reads the file with the path and name specified in <code>filename</code> and
 * returns in the <i>output</i> (non-const) reference parameter
 * <code>file_content_text</code>.
 *
 * If the file can not be opened it calls <code>FAIL ()</code>.
 *
 * The content of the reference (non-const) parameter <code>file_content_text</code>
 * is cleared before storing the content of the text file.
 *
 * @param filename [IN] Path and name (with extension) of the file.
 * @param file_content_text [OUT] Whole text content of the file.
 */
static void
ReadFile (const std::string & filename, std::string & file_content_text)
{
  std::cout << "Reading file \"" << filename << "\"... ";

  std::ifstream input_file (filename, std::ios::in);

  if (!input_file.is_open ())
    {
      std::cout << "Error: couldn't open file.\n";
      NS_ABORT_MSG ("Fail!");
    }

  // Clear previous content of the output reference
  file_content_text.clear ();

  // Read file
  input_file.seekg (0, std::ios::end);
  file_content_text.reserve (input_file.tellg ());
  input_file.seekg (0, std::ios::beg);

  file_content_text.assign ((std::istreambuf_iterator<char> (input_file)),
                            std::istreambuf_iterator<char> ());

  input_file.close ();
  std::cout << "Done.\n";
}

} // namespace TestUtils

// =============================================================================
//                             LibraryUtilsTestCase
// =============================================================================

class LibraryUtilsTestCase : public TestCase
{
public:

  LibraryUtilsTestCase (std::string name) : TestCase (name) { }

  template<typename T>
  void
  DoTestEqualityRelationalOperators (const T & equal_1, const T & equal_2, const T & different)
  {
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_1, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_1, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_1, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_1, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_1, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_1, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_2, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_2, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (equal_2, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_2, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_2, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (equal_2, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (different, equal_1), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (different, equal_2), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator== (different, different), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (different, equal_1), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (different, equal_2), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator!= (different, different), false, "Must not be different");
  }

  template<typename T>
  void
  DoTestOrderingRelationalOperators (const T & smaller, const T & greater)
  {
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator< (smaller, smaller), false, "Must not be smaller");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator< (smaller, greater), true, "Must be smaller");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator<= (smaller, smaller), true, "Must be smaller or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator<= (smaller, greater), true, "Must be smaller or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator> (smaller, smaller), false, "Must not be greater");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator> (smaller, greater), false, "Must not be greater");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator>= (smaller, smaller), true, "Must be greater or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator>= (smaller, greater), false, "Must not be greater or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator< (greater, smaller), false, "Must not be smaller");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator< (greater, greater), false, "Must not be smaller");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator<= (greater, smaller), false, "Must not be smaller or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator<= (greater, greater), true, "Must be smaller or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator> (greater, smaller), true, "Must be greater");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator> (greater, greater), false, "Must not be greater");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator>= (greater, smaller), true, "Must be greater or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::LibraryUtils::operator>= (greater, greater), true, "Must be greater or equal");
  }
};


// =============================================================================
//                           NavigationSystemTestCase
// =============================================================================

class NavigationSystemTestCase : public TestCase
{
public:

  NavigationSystemTestCase (std::string name) : TestCase (name) { }

  template<typename T>
  void
  DoTestEqualityRelationalOperators (const T & equal_1, const T & equal_2, const T & different)
  {
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_1, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_1, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_1, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_1, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_1, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_1, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_2, equal_1), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_2, equal_2), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (equal_2, different), false, "Must not be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_2, equal_1), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_2, equal_2), false, "Must not be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (equal_2, different), true, "Must be different");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (different, equal_1), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (different, equal_2), false, "Must not be equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator== (different, different), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (different, equal_1), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (different, equal_2), true, "Must be different");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator!= (different, different), false, "Must not be different");
  }

  template<typename T>
  void
  DoTestOrderingRelationalOperators (const T & smaller, const T & greater)
  {
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator< (smaller, smaller), false, "Must not be smaller");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator< (smaller, greater), true, "Must be smaller");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator<= (smaller, smaller), true, "Must be smaller or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator<= (smaller, greater), true, "Must be smaller or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator> (smaller, smaller), false, "Must not be greater");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator> (smaller, greater), false, "Must not be greater");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator>= (smaller, smaller), true, "Must be greater or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator>= (smaller, greater), false, "Must not be greater or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator< (greater, smaller), false, "Must not be smaller");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator< (greater, greater), false, "Must not be smaller");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator<= (greater, smaller), false, "Must not be smaller or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator<= (greater, greater), true, "Must be smaller or equal");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator> (greater, smaller), true, "Must be greater");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator> (greater, greater), false, "Must not be greater");

    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator>= (greater, smaller), true, "Must be greater or equal");
    NS_TEST_EXPECT_MSG_EQ (GeoTemporalLibrary::NavigationSystem::operator>= (greater, greater), true, "Must be greater or equal");
  }
};


/******************************************************************************/
/*                           geotemporal-utils.h/cc                           */
/******************************************************************************/

// =============================================================================
//                                TimePeriodTest
// =============================================================================

/**
 * TypeHeader packet header test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class TimePeriodTest : public LibraryUtilsTestCase
{
public:

  TimePeriodTest () : LibraryUtilsTestCase ("TimePeriod") { }

  void
  TestConstructors ()
  {
    // Default constructor
    TimePeriod t1;
    NS_TEST_EXPECT_MSG_EQ (t1.GetStartTime (), Seconds (0), "Must be 0 seconds");
    NS_TEST_EXPECT_MSG_EQ (t1.GetEndTime (), Seconds (0), "Must be 0 seconds");

    // Parameters constructor
    TimePeriod t2 (Days (2), Days (3));
    NS_TEST_EXPECT_MSG_EQ (t2.GetStartTime (), Days (2), "Must be 2 days");
    NS_TEST_EXPECT_MSG_EQ (t2.GetEndTime (), Days (3), "Must be 3 days");

    bool throwed_exception = false;

    try
      {
        TimePeriod will_fail (Days (3), Minutes (1));
      }
    catch (std::invalid_argument & ex)
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

    // Copy constructor
    TimePeriod t3 (t2);
    NS_TEST_EXPECT_MSG_EQ (t3.GetStartTime (), Days (2), "Must be 2 days");
    NS_TEST_EXPECT_MSG_EQ (t3.GetEndTime (), Days (3), "Must be 3 days");
  }

  void
  TestGettersSetters ()
  {
    TimePeriod t (Minutes (23), Hours (3));

    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetStartTime (), Minutes (23), MicroSeconds (1), "Must be 23 minutes");
    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetEndTime (), Hours (3), MicroSeconds (1), "Must be 3 hours");
    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetDuration (), Minutes (157), MicroSeconds (1), "Must be 157 minutes");

    t = TimePeriod (Hours (17), Days (32));

    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetStartTime (), Hours (17), MicroSeconds (1), "Must be 17 hours");
    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetEndTime (), Days (32), MicroSeconds (1), "Must be 32 days");
    NS_TEST_EXPECT_MSG_EQ_TOL (t.GetDuration (), Hours (751), MicroSeconds (1), "Must be 751 hours");
  }

  void
  TestCalculateEndTime ()
  {
    NS_TEST_EXPECT_MSG_EQ_TOL (TimePeriod::CalculateEndTime (Days (0), Days (0)), Time (0), MicroSeconds (0), "Must be 0");
    NS_TEST_EXPECT_MSG_EQ_TOL (TimePeriod::CalculateEndTime (Hours (20), Hours (0)), Hours (20), MicroSeconds (0), "Must be 20 hours");
    NS_TEST_EXPECT_MSG_EQ_TOL (TimePeriod::CalculateEndTime (Seconds (0), Minutes (15)), Minutes (15), MicroSeconds (0), "Must be 15 minutes");
    NS_TEST_EXPECT_MSG_EQ_TOL (TimePeriod::CalculateEndTime (Minutes (50), Seconds (100)), Seconds (3100), MicroSeconds (0), "Must be 3,100 seconds");
  }

  void
  IsDuringTimePeriod ()
  {
    TimePeriod t (Minutes (10), Minutes (20));

    NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Seconds (10 * 60)), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Seconds (20 * 60)), true, "Must be true");

    // Test with minutes
    for (uint32_t i = 0; i < 10; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Minutes (i)), false, "Must be false");
      }

    for (uint32_t i = 10; i < 21; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Minutes (i)), true, "Must be true");
      }

    for (uint32_t i = 21; i < 30; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Minutes (i)), false, "Must be false");
      }

    // Test with seconds
    for (uint32_t i = 0; i < 10 * 60; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Seconds (i)), false, "Must be false");
      }

    for (uint32_t i = 10 * 60; i <= 20 * 60; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Seconds (i)), true, "Must be true");
      }

    for (uint32_t i = 20 * 60 + 1; i < 30 * 60; ++i)
      {
        NS_TEST_EXPECT_MSG_EQ (t.IsDuringTimePeriod (Seconds (i)), false, "Must be false");
      }
  }

  void
  TestToString ()
  {
    TimePeriod t;
    std::string expected_str = "Period of time starts at 0.00 sec. and ends at 0.00 sec. "
            "(lasts 0.00 seconds)";

    NS_TEST_EXPECT_MSG_EQ (t.ToString (), expected_str, "Expected string: " + expected_str);

    t = TimePeriod (Seconds (0), Seconds (1900));
    expected_str = "Period of time starts at 0.00 sec. and ends at 1900.00 sec. "
            "(lasts 1900.00 seconds)";

    NS_TEST_EXPECT_MSG_EQ (t.ToString (), expected_str, "Expected string: " + expected_str);

    t = TimePeriod (Seconds (1900), Seconds (1905));
    expected_str = "Period of time starts at 1900.00 sec. and ends at 1905.00 sec. "
            "(lasts 5.00 seconds)";

    NS_TEST_EXPECT_MSG_EQ (t.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestEqualityRelationalOperators ()
  {
    // Test with a small time unit
    TimePeriod equal_1;
    TimePeriod equal_2;
    TimePeriod different (Time (0), NanoSeconds (1));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = TimePeriod (NanoSeconds (50), NanoSeconds (60));
    equal_2 = TimePeriod (NanoSeconds (50), NanoSeconds (60));
    different = TimePeriod (NanoSeconds (51), NanoSeconds (60));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = TimePeriod (NanoSeconds (50), NanoSeconds (60));
    equal_2 = TimePeriod (NanoSeconds (50), NanoSeconds (60));
    different = TimePeriod (NanoSeconds (50), NanoSeconds (61));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Test with a big time unit
    equal_1 = TimePeriod ();
    equal_2 = TimePeriod ();
    different = TimePeriod (Time (0), Years (1));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = TimePeriod (Years (50), Years (60));
    equal_2 = TimePeriod (Years (50), Years (60));
    different = TimePeriod (Years (51), Years (60));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = TimePeriod (Years (50), Years (60));
    equal_2 = TimePeriod (Years (50), Years (60));
    different = TimePeriod (Years (50), Years (61));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestOrderingRelationalOperators ()
  {
    // Test with a small time unit
    // Different duration
    TimePeriod smaller (NanoSeconds (43), NanoSeconds (60));
    TimePeriod greater (NanoSeconds (42), NanoSeconds (60));

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different start time
    smaller = TimePeriod (NanoSeconds (13), NanoSeconds (30));
    greater = TimePeriod (NanoSeconds (43), NanoSeconds (60));

    DoTestOrderingRelationalOperators (smaller, greater);

    smaller = TimePeriod ();
    greater = TimePeriod (NanoSeconds (10), NanoSeconds (10));

    DoTestOrderingRelationalOperators (smaller, greater);

    // Test with a big time unit
    // Different duration
    smaller = TimePeriod (Years (43), Years (60));
    greater = TimePeriod (Years (42), Years (60));

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different start time
    smaller = TimePeriod (Years (13), Years (30));
    greater = TimePeriod (Years (43), Years (60));

    DoTestOrderingRelationalOperators (smaller, greater);

    smaller = TimePeriod ();
    greater = TimePeriod (Years (10), Years (10));

    DoTestOrderingRelationalOperators (smaller, greater);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestGettersSetters ();
    TestCalculateEndTime ();
    IsDuringTimePeriod ();
    TestToString ();
    TestEqualityRelationalOperators ();
    TestOrderingRelationalOperators ();
  }
};


/******************************************************************************/
/*                            statistics-utils.h/cc                           */
/******************************************************************************/

// =============================================================================
//                                PacketClassTest
// =============================================================================

/**
 * PacketClass test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class PacketClassTest : public LibraryUtilsTestCase
{
public:

  PacketClassTest () : LibraryUtilsTestCase ("PacketClass") { }

  void
  DoRun () override
  {
    PacketClass pc = PacketClass::Data;

    NS_TEST_EXPECT_MSG_EQ ((pc == PacketClass::Data), true, "Must be data");

    pc = PacketClass::Control;
    NS_TEST_EXPECT_MSG_EQ ((pc == PacketClass::Control), true, "Must be control");

    pc = PacketClass::Data;
    NS_TEST_EXPECT_MSG_EQ ((pc == PacketClass::Data), true, "Must be data");
  }
};


// =============================================================================
//                                PacketsCounterTest
// =============================================================================

/**
 * PacketsCounter test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class PacketsCounterTest : public LibraryUtilsTestCase
{
public:

  PacketsCounterTest () : LibraryUtilsTestCase ("PacketsCounter") { }

  void
  TestConstructors ()
  {
    // Default constructor
    PacketsCounter p1;

    NS_TEST_EXPECT_MSG_EQ (p1.GetDataPacketsCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (p1.GetDataPacketsSize (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (p1.GetControlPacketsCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (p1.GetControlPacketsSize (), 0u, "Must be 0");

    // Copy constructor
    PacketsCounter to_copy;

    to_copy.CountPacket (PacketClass::Control, 321);
    to_copy.CountPacket (PacketClass::Control, 279);
    to_copy.CountPacket (PacketClass::Data, 400);

    PacketsCounter copy (to_copy);

    NS_TEST_EXPECT_MSG_EQ (to_copy.GetDataPacketsCount (), copy.GetDataPacketsCount (), "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (to_copy.GetDataPacketsSize (), copy.GetDataPacketsSize (), "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (to_copy.GetControlPacketsCount (), copy.GetControlPacketsCount (), "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (to_copy.GetControlPacketsSize (), copy.GetControlPacketsSize (), "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (to_copy, copy, "Must be equal");
  }

  void
  TestCountPacket ()
  {
    PacketsCounter packets_counter;

    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsCount (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsSize (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsCount (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsSize (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsCount (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsSize (), 0, "Must be equal");

    packets_counter.CountPacket (PacketClass::Control, 321);

    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsCount (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsSize (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsCount (), 1, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsSize (), 321, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsCount (), 1, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsSize (), 321, "Must be equal");

    packets_counter.CountPacket (PacketClass::Control, 279);

    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsCount (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsSize (), 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsCount (), 2, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsSize (), 600, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsCount (), 2, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsSize (), 600, "Must be equal");

    packets_counter.CountPacket (PacketClass::Data, 400);

    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsCount (), 1, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsSize (), 400, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsCount (), 2, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsSize (), 600, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsCount (), 3, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsSize (), 1000, "Must be equal");

    packets_counter.CountPacket (PacketClass::Data, 250);
    packets_counter.CountPacket (PacketClass::Data, 600);

    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsCount (), 3, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetDataPacketsSize (), 1250, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsCount (), 2, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetControlPacketsSize (), 600, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsCount (), 5, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packets_counter.GetCombinedPacketsSize (), 1850, "Must be equal");
  }

  void
  TestToString ()
  {
    PacketsCounter packets_counter;

    std::string expected_str = "0 packet(s): data 0 | control 0";
    NS_TEST_EXPECT_MSG_EQ (packets_counter.ToString (), expected_str, "Expected string: " + expected_str);

    packets_counter.CountPacket (PacketClass::Control, 321);

    expected_str = "1 packet(s): data 0 | control 1";
    NS_TEST_EXPECT_MSG_EQ (packets_counter.ToString (), expected_str, "Expected string: " + expected_str);

    packets_counter.CountPacket (PacketClass::Control, 279);

    expected_str = "2 packet(s): data 0 | control 2";
    NS_TEST_EXPECT_MSG_EQ (packets_counter.ToString (), expected_str, "Expected string: " + expected_str);

    packets_counter.CountPacket (PacketClass::Data, 400);

    expected_str = "3 packet(s): data 1 | control 2";
    NS_TEST_EXPECT_MSG_EQ (packets_counter.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestEqualityRelationalOperators ()
  {
    // Different data packets count
    PacketsCounter equal_1;
    PacketsCounter equal_2;
    PacketsCounter different;

    //     data 2 (200 bytes) | control 0 (0  bytes)
    equal_1.CountPacket (PacketClass::Data, 100);
    equal_1.CountPacket (PacketClass::Data, 100);

    //     data 2 (200 bytes) | control 0 (0  bytes)
    equal_2.CountPacket (PacketClass::Data, 100);
    equal_2.CountPacket (PacketClass::Data, 100);

    //     data 1 (200 bytes) | control 0 (0  bytes)
    different.CountPacket (PacketClass::Data, 200);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different data packets size
    //     data 2 (300 bytes) | control 0 (0  bytes)
    different.CountPacket (PacketClass::Data, 100);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different control packets count
    equal_1 = PacketsCounter ();
    equal_2 = PacketsCounter ();
    different = PacketsCounter ();

    //     data 0 (0 bytes) | control 2 (200  bytes)
    equal_1.CountPacket (PacketClass::Control, 100);
    equal_1.CountPacket (PacketClass::Control, 100);

    //     data 0 (0 bytes) | control 2 (200  bytes)
    equal_2.CountPacket (PacketClass::Control, 100);
    equal_2.CountPacket (PacketClass::Control, 100);

    //     data 0 (0 bytes) | control 1 (200  bytes)
    different.CountPacket (PacketClass::Control, 200);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different control packets size
    //     data 0 (0 bytes) | control 2 (300  bytes)
    different.CountPacket (PacketClass::Control, 100);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestOrderingRelationalOperators ()
  {
    // Different combined packets count
    PacketsCounter smaller;
    PacketsCounter greater;

    greater.CountPacket (PacketClass::Data, 600);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different combined packets size
    smaller.CountPacket (PacketClass::Control, 400);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different data packets count
    smaller.CountPacket (PacketClass::Control, 600);
    smaller.CountPacket (PacketClass::Data, 500);

    greater.CountPacket (PacketClass::Data, 400);
    greater.CountPacket (PacketClass::Control, 500);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different data packets size
    smaller = PacketsCounter ();

    smaller.CountPacket (PacketClass::Data, 400);
    smaller.CountPacket (PacketClass::Data, 400);
    smaller.CountPacket (PacketClass::Control, 600);
    smaller.CountPacket (PacketClass::Control, 600);

    greater = PacketsCounter ();

    greater.CountPacket (PacketClass::Data, 600);
    greater.CountPacket (PacketClass::Data, 600);
    greater.CountPacket (PacketClass::Control, 400);
    greater.CountPacket (PacketClass::Control, 400);

    DoTestOrderingRelationalOperators (smaller, greater);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestCountPacket ();
    TestToString ();
    TestEqualityRelationalOperators ();
    TestOrderingRelationalOperators ();
  }
};


// =============================================================================
//                              TransmissionTypeTest
// =============================================================================

/**
 * TransmissionType test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class TransmissionTypeTest : public LibraryUtilsTestCase
{
public:

  TransmissionTypeTest () : LibraryUtilsTestCase ("TransmissionType") { }

  void
  DoRun () override
  {
    TransmissionType tt = TransmissionType::Unicast;

    NS_TEST_EXPECT_MSG_EQ ((tt == TransmissionType::Unicast), true, "Must be unicast");

    tt = TransmissionType::Broadcast;
    NS_TEST_EXPECT_MSG_EQ ((tt == TransmissionType::Broadcast), true, "Must be broadcast");

    tt = TransmissionType::Unicast;
    NS_TEST_EXPECT_MSG_EQ ((tt == TransmissionType::Unicast), true, "Must be unicast");
  }
};



// =============================================================================
//                         DataPacketReceptionStatsTest
// =============================================================================

/**
 * DataPacketReceptionStats test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class DataPacketReceptionStatsTest : public LibraryUtilsTestCase
{
public:

  DataPacketReceptionStatsTest () : LibraryUtilsTestCase ("DataPacketReceptionStats") { }

  void
  TestConstructors ()
  {
    // Default constructor
    DataPacketReceptionStats r1;

    NS_TEST_EXPECT_MSG_EQ (r1.GetPacketDataIdentifier (), DataIdentifier (), "Must be default DataIdentifier");
    NS_TEST_EXPECT_MSG_EQ (r1.GetTransmitterIpAddress (), Ipv4Address (), "Must be default Ipv4Address");
    NS_TEST_EXPECT_MSG_EQ (r1.GetReceptionTime (), Seconds (0), "Must be 0 seconds");
    NS_TEST_EXPECT_MSG_EQ (r1.GetPacketDestinedToReceiverNode (), false, "Must be false");
    NS_TEST_EXPECT_MSG_EQ (r1.GetTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r1.GetUnicastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r1.GetBroadcastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r1.GetReceivedDuplicatesCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r1.GetPacketDropped (), false, "Must be false");

    // Parameters constructor
    DataPacketReceptionStats r2 (DataIdentifier ("10.0.0.1:37"),
                                 Ipv4Address ("10.0.0.9"),
                                 Seconds (27.7), true);

    NS_TEST_EXPECT_MSG_EQ (r2.GetPacketDataIdentifier (), DataIdentifier ("10.0.0.1:37"), "Must be 10.0.0.1:37");
    NS_TEST_EXPECT_MSG_EQ (r2.GetTransmitterIpAddress (), Ipv4Address ("10.0.0.9"), "Must be 10.0.0.9");
    NS_TEST_EXPECT_MSG_EQ (r2.GetReceptionTime (), Seconds (27.7), "Must be 27.7 seconds");
    NS_TEST_EXPECT_MSG_EQ (r2.GetPacketDestinedToReceiverNode (), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (r2.GetTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r2.GetUnicastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r2.GetBroadcastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r2.GetReceivedDuplicatesCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r2.GetPacketDropped (), false, "Must be false");

    // Copy constructor
    DataPacketReceptionStats r3 (r2);

    NS_TEST_EXPECT_MSG_EQ (r3.GetPacketDataIdentifier (), DataIdentifier ("10.0.0.1:37"), "Must be 10.0.0.1:37");
    NS_TEST_EXPECT_MSG_EQ (r3.GetTransmitterIpAddress (), Ipv4Address ("10.0.0.9"), "Must be 10.0.0.9");
    NS_TEST_EXPECT_MSG_EQ (r3.GetReceptionTime (), Seconds (27.7), "Must be 27.7 seconds");
    NS_TEST_EXPECT_MSG_EQ (r3.GetPacketDestinedToReceiverNode (), true, "Must be true");
    NS_TEST_EXPECT_MSG_EQ (r3.GetTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r3.GetUnicastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r3.GetBroadcastTransmittedReplicasCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r3.GetReceivedDuplicatesCount (), 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (r3.GetPacketDropped (), false, "Must be false");
  }

  void
  TestIncrementTransmittedReplicasCount ()
  {
    DataPacketReceptionStats reception_stats (DataIdentifier ("10.0.0.1:37"),
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (8), true);

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetTransmittedReplicasCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetUnicastTransmittedReplicasCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetBroadcastTransmittedReplicasCount (), 0u, "Must be 0");

    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Unicast);

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetTransmittedReplicasCount (), 1u, "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetUnicastTransmittedReplicasCount (), 1u, "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetBroadcastTransmittedReplicasCount (), 0u, "Must be 0");

    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);
    reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetTransmittedReplicasCount (), 8u, "Must be 8");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetUnicastTransmittedReplicasCount (), 3u, "Must be 3");
    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetBroadcastTransmittedReplicasCount (), 5u, "Must be 5");
  }

  void
  TestIncrementReceivedDuplicatesCount ()
  {
    DataPacketReceptionStats reception_stats (DataIdentifier ("10.0.0.1:37"),
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (8), true);

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetReceivedDuplicatesCount (), 0u, "Must be 0");

    reception_stats.IncrementReceivedDuplicatesCount ();

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetReceivedDuplicatesCount (), 1u, "Must be 1");

    reception_stats.IncrementReceivedDuplicatesCount ();
    reception_stats.IncrementReceivedDuplicatesCount ();
    reception_stats.IncrementReceivedDuplicatesCount ();
    reception_stats.IncrementReceivedDuplicatesCount ();
    reception_stats.IncrementReceivedDuplicatesCount ();
    reception_stats.IncrementReceivedDuplicatesCount ();

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetReceivedDuplicatesCount (), 7u, "Must be 7");
  }

  void
  TestSetPacketDropped ()
  {
    DataPacketReceptionStats reception_stats (DataIdentifier ("10.0.0.1:37"),
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (8), true);

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetPacketDropped (), false, "Must be false");

    reception_stats.SetPacketDropped ();

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetPacketDropped (), true, "Must be true");

    reception_stats.SetPacketDropped ();

    NS_TEST_EXPECT_MSG_EQ (reception_stats.GetPacketDropped (), true, "Must be true");
  }

  void
  TestToString ()
  {
    DataPacketReceptionStats reception_stats (DataIdentifier ("10.0.0.1:37"),
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (8), true);

    std::string expected_str = "<data-packet-reception data-id=\"10.0.0.1:37\" time=\"8.000000\" "
            "from-node=\"10.0.0.25\" for-me=\"True\" received-duplicates=\"0\" "
            "unicast-transmitted-replicas=\"0\" "
            "broadcast-transmitted-replicas=\"0\" dropped=\"False\" />";

    NS_TEST_EXPECT_MSG_EQ (reception_stats.ToString (), expected_str, "Expected string: " + expected_str);

    for (uint32_t i = 0u; i < 23; ++i)
      reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Unicast);

    expected_str = "<data-packet-reception data-id=\"10.0.0.1:37\" time=\"8.000000\" "
            "from-node=\"10.0.0.25\" for-me=\"True\" received-duplicates=\"0\" "
            "unicast-transmitted-replicas=\"23\" "
            "broadcast-transmitted-replicas=\"0\" dropped=\"False\" />";

    NS_TEST_EXPECT_MSG_EQ (reception_stats.ToString (), expected_str, "Expected string: " + expected_str);

    for (uint32_t i = 0u; i < 11; ++i)
      reception_stats.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);

    expected_str = "<data-packet-reception data-id=\"10.0.0.1:37\" time=\"8.000000\" "
            "from-node=\"10.0.0.25\" for-me=\"True\" received-duplicates=\"0\" "
            "unicast-transmitted-replicas=\"23\" "
            "broadcast-transmitted-replicas=\"11\" dropped=\"False\" />";

    NS_TEST_EXPECT_MSG_EQ (reception_stats.ToString (), expected_str, "Expected string: " + expected_str);

    for (uint32_t i = 0u; i < 7; ++i)
      reception_stats.IncrementReceivedDuplicatesCount ();

    expected_str = "<data-packet-reception data-id=\"10.0.0.1:37\" time=\"8.000000\" "
            "from-node=\"10.0.0.25\" for-me=\"True\" received-duplicates=\"7\" "
            "unicast-transmitted-replicas=\"23\" "
            "broadcast-transmitted-replicas=\"11\" dropped=\"False\" />";

    NS_TEST_EXPECT_MSG_EQ (reception_stats.ToString (), expected_str, "Expected string: " + expected_str);

    reception_stats.SetPacketDropped ();

    expected_str = "<data-packet-reception data-id=\"10.0.0.1:37\" time=\"8.000000\" "
            "from-node=\"10.0.0.25\" for-me=\"True\" received-duplicates=\"7\" "
            "unicast-transmitted-replicas=\"23\" "
            "broadcast-transmitted-replicas=\"11\" dropped=\"True\" />";

    NS_TEST_EXPECT_MSG_EQ (reception_stats.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestEqualityRelationalOperators ()
  {
    DataPacketReceptionStats equal_1;
    DataPacketReceptionStats equal_2;
    DataPacketReceptionStats different (DataIdentifier (), Ipv4Address (),
                                        Seconds (0), true);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:3"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.62"), Seconds (8), true);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (18), true);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), false);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    equal_1.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
    equal_2.IncrementTransmittedReplicasCount (TransmissionType::Unicast);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    equal_1.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
    equal_2.IncrementTransmittedReplicasCount (TransmissionType::Unicast);
    different.IncrementTransmittedReplicasCount (TransmissionType::Broadcast);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    equal_1.IncrementReceivedDuplicatesCount ();
    equal_2.IncrementReceivedDuplicatesCount ();

    different.IncrementReceivedDuplicatesCount ();
    different.IncrementReceivedDuplicatesCount ();

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    equal_1 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    equal_2 = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    different = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    different.SetPacketDropped ();

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestOrderingRelationalOperators ()
  {
    // Different "data ID"
    DataPacketReceptionStats smaller (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    DataPacketReceptionStats greater (DataIdentifier ("10.0.0.9:73"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different "reception time"
    smaller = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    greater = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (80), true);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different "transmitter node IP"
    smaller = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);
    greater = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.251"), Seconds (8), true);

    DoTestOrderingRelationalOperators (smaller, greater);

    // Different "destined to me (receiver node)"
    smaller = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), false);
    greater = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:37"), Ipv4Address ("10.0.0.25"), Seconds (8), true);

    DoTestOrderingRelationalOperators (smaller, greater);
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestIncrementTransmittedReplicasCount ();
    TestIncrementReceivedDuplicatesCount ();
    TestSetPacketDropped ();
    TestToString ();
    TestEqualityRelationalOperators ();
    TestOrderingRelationalOperators ();
  }
};


// =============================================================================
//                              DataPacketStatisticsTest
// =============================================================================

/**
 * DataPacketStatistics test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class DataPacketStatisticsTest : public LibraryUtilsTestCase
{
public:

  DataPacketStatisticsTest () : LibraryUtilsTestCase ("DataPacketStatistics") { }

  void
  TestConstructors ()
  {
    // Default constructor
    DataPacketStatistics dps1;

    NS_TEST_EXPECT_MSG_EQ (dps1.GetDataIdentifier (), DataIdentifier (), "Must be default data ID");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetSourceNodeIp (), Ipv4Address (), "Must be default IP address");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetSourceNodeId (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetPacketCreationTime (), Seconds (0), "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetPacketMessageSize (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetPacketSize (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetPacketDestinationGeoTemporalArea (), GeoTemporalArea (), "Must be default geo-temporal area");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetExpectedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.GetConfirmedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps1.IsExpectedReceiverNodesSet (), false, "Must be false");

    // Parameters constructor
    DataPacketStatistics dps2 (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                               /*Source ID*/ 0,
                               /*Creation time*/ Seconds (10),
                               /*Message size*/ 256,
                               /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                               /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                      Area (10, 20, 30, 40)));

    NS_TEST_EXPECT_MSG_EQ (dps2.GetDataIdentifier (), DataIdentifier ("10.0.0.1:32"), "Must be 10.0.0.1:32");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetSourceNodeIp (), Ipv4Address ("10.0.0.1"), "Must be 10.0.0.1");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetSourceNodeId (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetPacketCreationTime (), Seconds (10), "Must be second 10");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetPacketMessageSize (), 256u, "Must be 256");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetPacketSize (), 305u + 64u, "Must be 369 (305 + 64)");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetPacketDestinationGeoTemporalArea (),
                           GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)), Area (10, 20, 30, 40)),
                           "Must be the given area");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetExpectedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps2.GetConfirmedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps2.IsExpectedReceiverNodesSet (), false, "Must be false");

    // Copy constructor
    DataPacketStatistics dps3 (dps2);

    NS_TEST_EXPECT_MSG_EQ (dps3.GetDataIdentifier (), DataIdentifier ("10.0.0.1:32"), "Must be 10.0.0.1:32");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetSourceNodeIp (), Ipv4Address ("10.0.0.1"), "Must be 10.0.0.1");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetSourceNodeId (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetPacketCreationTime (), Seconds (10), "Must be second 10");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetPacketMessageSize (), 256u, "Must be 256");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetPacketSize (), 305u + 64u, "Must be 369 (305 + 64)");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetPacketDestinationGeoTemporalArea (),
                           GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)), Area (10, 20, 30, 40)),
                           "Must be the given area");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetExpectedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps3.GetConfirmedReceiverNodesCount (), 0u, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (dps3.IsExpectedReceiverNodesSet (), false, "Must be false");
    NS_TEST_EXPECT_MSG_EQ (dps2, dps3, "Must be equal");
  }

  void
  TestCalculateRealPacketSize ()
  {
    NS_TEST_EXPECT_MSG_EQ (DataPacketStatistics::CalculateRealPacketSize (0), 64, "Must be 64");
    NS_TEST_EXPECT_MSG_EQ (DataPacketStatistics::CalculateRealPacketSize (958), 1022, "Must be 1022");
  }

  void
  TestIsExpectedReceiverNodesSet ()
  {
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));

    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), false, "Must be false");

    std::map<Ipv4Address, Time> expected_receiver_nodes{
      {Ipv4Address ("10.0.0.1"), Seconds (30)}, // Source node, will be discarded.
      {Ipv4Address ("10.0.0.2"), Seconds (40)},
      {Ipv4Address ("10.0.0.3"), Seconds (50)},
      {Ipv4Address ("10.0.0.5"), Seconds (60)},
      {Ipv4Address ("10.0.0.6"), Seconds (70)},
      {Ipv4Address ("10.0.0.10"), Seconds (131)} // Didn't arrive to the area on time, will be discarded.
    };

    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), true, "Must be true");
  }

  void
  TestClearExpectedReceiverNodes ()
  {
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));

    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), false, "Must be false");

    std::map<Ipv4Address, Time> expected_receiver_nodes{
      {Ipv4Address ("10.0.0.1"), Seconds (30)}, // Source node, will be discarded.
      {Ipv4Address ("10.0.0.2"), Seconds (40)},
      {Ipv4Address ("10.0.0.3"), Seconds (50)},
      {Ipv4Address ("10.0.0.5"), Seconds (60)},
      {Ipv4Address ("10.0.0.6"), Seconds (70)},
      {Ipv4Address ("10.0.0.10"), Seconds (131)} // Didn't arrive to the area on time, will be discarded.
    };

    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), true, "Must be true");

    packet_stats.ClearExpectedReceiverNodes ();

    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), false, "Must be false");
  }

  void
  TestSetExpectedReceiverNodes ()
  {
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));

    std::map<Ipv4Address, Time> expected_receiver_nodes = {
      {Ipv4Address ("10.0.0.1"), Seconds (30)}, // Source node, will be discarded.
      {Ipv4Address ("10.0.0.2"), Seconds (30)},
      {Ipv4Address ("10.0.0.3"), Seconds (50)},
      {Ipv4Address ("10.0.0.5"), Seconds (60)},
      {Ipv4Address ("10.0.0.6"), Seconds (70)},
      {Ipv4Address ("10.0.0.10"), Seconds (131)} // Didn't arrive to the area on time, will be discarded.
    };

    uint32_t result = packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    NS_TEST_EXPECT_MSG_EQ (result, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), true, "Must be true");

    expected_receiver_nodes = {
      {Ipv4Address ("11.0.0.1"), Seconds (30)},
      {Ipv4Address ("11.0.0.2"), Seconds (40)},
      {Ipv4Address ("11.0.0.3"), Seconds (50)},
      {Ipv4Address ("11.0.0.5"), Seconds (60)},
      {Ipv4Address ("11.0.0.6"), Seconds (70)},
      {Ipv4Address ("11.0.0.10"), Seconds (80)}
    };

    result = packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    NS_TEST_EXPECT_MSG_EQ (result, 6, "Must be 6");
    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), true, "Must be true");

    expected_receiver_nodes = {}; // No expected receiver at all

    result = packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    NS_TEST_EXPECT_MSG_EQ (result, 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (packet_stats.IsExpectedReceiverNodesSet (), true, "Must be true");
  }

  void
  TestCountReception ()
  {
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));
    bool result = false;
    bool throwed_exception = false;

    // Packet doesn't have its expected receiver nodes configured.
    DataPacketReceptionStats reception (DataIdentifier ("10.0.0.1:32"),
                                        Ipv4Address ("10.0.0.2"),
                                        Seconds (20),
                                        true);
    try
      {
        result = packet_stats.CountReception (Ipv4Address ("10.0.0.10"), reception);
      }
    catch (std::runtime_error & e)
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
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false"); // Result must have not been changed since its last assignment.

    // Data ID doesn't match
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:200"),
                                          Ipv4Address ("10.0.0.2"),
                                          Seconds (20),
                                          true);
    try
      {
        throwed_exception = false;
        result = packet_stats.CountReception (Ipv4Address ("10.0.0.10"), reception);
      }
    catch (std::runtime_error & e)
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
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false"); // Result must have not been changed since its last assignment.

    std::map<Ipv4Address, Time> expected_receiver_nodes = {
      {Ipv4Address ("10.0.0.1"), Seconds (30)}, // Source node, will be discarded.
      {Ipv4Address ("10.0.0.2"), Seconds (30)},
      {Ipv4Address ("10.0.0.3"), Seconds (40)},
      {Ipv4Address ("10.0.0.4"), Seconds (50)},
      {Ipv4Address ("10.0.0.5"), Seconds (60)},
      {Ipv4Address ("10.0.0.6"), Seconds (131)} // Didn't arrive to the area on time, will be discarded.
    };
    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    // Node is not an expected receiver node
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.2"),
                                          Seconds (20),
                                          true);
    result = packet_stats.CountReception (Ipv4Address ("10.0.0.6"), reception);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    // Valid reception
    result = packet_stats.CountReception (Ipv4Address ("10.0.0.2"), reception);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    // Node already processed
    try
      {
        throwed_exception = false;
        result = packet_stats.CountReception (Ipv4Address ("10.0.0.2"), reception);
      }
    catch (std::runtime_error & e)
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
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true"); // Result must have not been changed since its last assignment.

    // Dropped packet
    reception.SetPacketDropped ();

    result = packet_stats.CountReception (Ipv4Address ("10.0.0.3"), reception);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");
  }

  void
  TestGetStatistics ()
  {
    // Used as output references by the function
    uint32_t output_confirmed_receivers_count, output_expected_receivers_count;
    double output_delivery_ratio, output_average_delivery_delay;
    uint32_t output_delivered_data_bytes;

    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));

    // No expected receiver nodes at all
    std::map<Ipv4Address, Time> expected_receiver_nodes;
    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, -1.0), true, "Must be -1.0");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, -1.0), true, "Must be -1.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 0, "Must be 0");

    // 4 expected receivers
    expected_receiver_nodes = {
      {Ipv4Address ("10.0.0.1"), Seconds (30)}, // Source node, will be discarded.
      {Ipv4Address ("10.0.0.2"), Seconds (30)},
      {Ipv4Address ("10.0.0.3"), Seconds (40)},
      {Ipv4Address ("10.0.0.4"), Seconds (50)},
      {Ipv4Address ("10.0.0.5"), Seconds (60)},
      {Ipv4Address ("10.0.0.6"), Seconds (131)} // Didn't arrive to the area on time, will be discarded.
    };
    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    // 4 Expected receivers, no confirmed receivers.
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 0, "Must be 0");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 0.0), true, "Must be 0.0");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, -1.0), true, "Must be -1.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 0, "Must be 0");

    // 4 Expected receivers, 1 confirmed receivers.
    DataPacketReceptionStats reception (DataIdentifier ("10.0.0.1:32"),
                                        Ipv4Address ("10.0.0.1"),
                                        Seconds (20),
                                        true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.2"), reception);

    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 1, "Must be 1");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 1.0 / 4.0), true, "Must be 1/4 (0.25)");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, 0.0), true, "Must be 0.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 369 * 1, "Must be 369 (305 + 64)");

    // 4 Expected receivers, 2 confirmed receivers.
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.2"),
                                          Seconds (50),
                                          true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.3"), reception);

    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 2, "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 2.0 / 4.0), true, "Must be 2/4 (0.5)");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, (0.0 + 10.0) / 2.0), true, "Must be 5.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 369 * 2, "Must be 738");

    // 4 Expected receivers, third expected receiver dropped the packet
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.3"),
                                          Seconds (20),
                                          true);
    reception.SetPacketDropped ();
    packet_stats.CountReception (Ipv4Address ("10.0.0.5"), reception);

    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 2, "Must be 2");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 2.0 / 4.0), true, "Must be 2/4 (0.5)");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, (0.0 + 10.0) / 2.0), true, "Must be 5.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 369 * 2, "Must be 738");

    // 4 Expected receivers, 3 confirmed receivers.
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.3"),
                                          Seconds (130),
                                          true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.4"), reception);

    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 3, "Must be 3");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 4, "Must be 4");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 3.0 / 4.0), true, "Must be 3/4 (0.75)");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, (0.0 + 10.0 + 80.0) / 3.0), true, "Must be 30.0");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 369 * 3, "Must be 1107");
  }

  void
  TestToString ()
  {
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 256,
                                       /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));

    std::string expected_str = "<data-packet data-id=\"10.0.0.1:32\" "
            "source-node-ip=\"10.0.0.1\" "
            "source-node-id=\"0\" "
            "creation-time=\"10.000000\" "
            "destination-area=\"10.000000,20.000000, 30.000000,40.000000\" "
            "initial-time=\"30.00\" "
            "duration=\"100.00\" "
            "data-message-size=\"256\" "
            "packet-size=\"369\" />";

    NS_TEST_EXPECT_MSG_EQ (packet_stats.ToString (), expected_str, "Expected string: " + expected_str);
  }

  void
  TestEqualityRelationalOperators ()
  {
    // Different DATA ID
    DataPacketStatistics equal_1 (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                  /*Source ID*/ 0,
                                  /*Creation time*/ Seconds (10),
                                  /*Message size*/ 256,
                                  /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                  /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                         Area (10, 20, 30, 40)));
    DataPacketStatistics equal_2 (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                  /*Source ID*/ 0,
                                  /*Creation time*/ Seconds (10),
                                  /*Message size*/ 256,
                                  /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                  /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                         Area (10, 20, 30, 40)));
    DataPacketStatistics different (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                    /*Source ID*/ 0,
                                    /*Creation time*/ Seconds (10),
                                    /*Message size*/ 256,
                                    /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                    /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                           Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different SOURCE NODE ID
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                      /*Source ID*/ 2,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different CREATION TIME
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ MicroSeconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different DATA MESSAGE SIZE
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 64,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different PACKET SIZE
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 150,
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different GEO-TEMPORAL AREA
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:33"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305,
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (20), Seconds (120)),
                                                                             Area (10, 20, 30, 40)));

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different RECEIVER NODES SET FLAG
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));
    different.SetExpectedReceiverNodes ({
      {Ipv4Address ("2.2.2.2"), Seconds (10)}
    });

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different RECEIVER NODES ARRIVAL TIME MAPPING
    equal_1.SetExpectedReceiverNodes ({
      {Ipv4Address ("1.1.1.1"), Seconds (30)},
      {Ipv4Address ("1.1.1.2"), Seconds (40)}
    });
    equal_2.SetExpectedReceiverNodes ({
      {Ipv4Address ("1.1.1.1"), Seconds (30)},
      {Ipv4Address ("1.1.1.2"), Seconds (40)}
    });

    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));
    different.SetExpectedReceiverNodes ({
      {Ipv4Address ("1.1.1.1"), Seconds (50)},
      {Ipv4Address ("1.1.1.2"), Seconds (60)}
    });

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different CONFIRMED RECEIVER NODES
    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));
    different.SetExpectedReceiverNodes ({
      {Ipv4Address ("1.1.1.1"), Seconds (30)},
      {Ipv4Address ("1.1.1.2"), Seconds (40)}
    });

    DataPacketReceptionStats reception (DataIdentifier ("10.0.0.1:32"),
                                        Ipv4Address ("10.0.0.1"),
                                        Seconds (70),
                                        true);
    different.CountReception (Ipv4Address ("1.1.1.2"), reception);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);

    // Different DELIVERY DELAY LIST
    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.1"),
                                          Seconds (50),
                                          true);

    equal_1.CountReception (Ipv4Address ("1.1.1.2"), reception);
    equal_2.CountReception (Ipv4Address ("1.1.1.2"), reception);

    different = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                      /*Source ID*/ 0,
                                      /*Creation time*/ Seconds (10),
                                      /*Message size*/ 256,
                                      /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                      /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                             Area (10, 20, 30, 40)));
    different.SetExpectedReceiverNodes ({
      {Ipv4Address ("1.1.1.1"), Seconds (30)},
      {Ipv4Address ("1.1.1.2"), Seconds (40)}
    });

    reception = DataPacketReceptionStats (DataIdentifier ("10.0.0.1:32"),
                                          Ipv4Address ("10.0.0.1"),
                                          Seconds (40),
                                          true);
    different.CountReception (Ipv4Address ("1.1.1.2"), reception);

    DoTestEqualityRelationalOperators (equal_1, equal_2, different);
  }

  void
  TestOrderingRelationalOperators ()
  {
    // Different "data ID"
    DataPacketStatistics smaller (/*Data ID*/ DataIdentifier ("10.0.0.1:30"),
                                  /*Source ID*/ 0,
                                  /*Creation time*/ Seconds (10),
                                  /*Message size*/ 256,
                                  /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                  /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                         Area (10, 20, 30, 40)));
    DataPacketStatistics greater (/*Data ID*/ DataIdentifier ("10.0.0.1:32"),
                                  /*Source ID*/ 0,
                                  /*Creation time*/ Seconds (10),
                                  /*Message size*/ 256,
                                  /*DataHeader size*/ 305, // 256 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                  /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                         Area (10, 20, 30, 40)));

    DoTestOrderingRelationalOperators (smaller, greater);
  }

  void
  SimulateScenario ()
  {
    // Used to calculate expected statistics
    uint32_t expected_receivers_count = 12u, expected_confirmed_receivers_count = 0u;
    double expected_delay_sum = 0.0;
    double expected_delay_average, expected_delivery_ratio;

    // Used as output references by the function
    uint32_t output_confirmed_receivers_count, output_expected_receivers_count;
    double output_delivery_ratio, output_average_delivery_delay;
    uint32_t output_delivered_data_bytes;

    // Set up
    DataIdentifier data_id ("10.0.0.99:32");

    std::map<Ipv4Address, Time> expected_receiver_nodes = {
      {Ipv4Address ("10.0.0.1"), Seconds (30)},
      {Ipv4Address ("10.0.0.2"), Seconds (30)},
      {Ipv4Address ("10.0.0.3"), Seconds (30)},
      {Ipv4Address ("10.0.0.4"), Seconds (30)},
      {Ipv4Address ("10.0.0.5"), Seconds (40)},
      {Ipv4Address ("10.0.0.6"), Seconds (50)},
      {Ipv4Address ("10.0.0.7"), Seconds (60)},
      {Ipv4Address ("10.0.0.8"), Seconds (70)},
      {Ipv4Address ("10.0.0.9"), Seconds (80)},
      {Ipv4Address ("10.0.0.10"), Seconds (90)},
      {Ipv4Address ("10.0.0.11"), Seconds (30)}, // This node drops the packet
      {Ipv4Address ("10.0.0.12"), Seconds (40)} // This node drops the packet
    };

    DataPacketStatistics packet_stats (/*Data ID*/ data_id,
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (10),
                                       /*Message size*/ 512,
                                       /*DataHeader size*/ 561, // 512 bytes (message) + 1 byte (terminator) + 48 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ GeoTemporalArea (TimePeriod (Seconds (30), Seconds (130)),
                                                                              Area (10, 20, 30, 40)));
    packet_stats.SetExpectedReceiverNodes (expected_receiver_nodes);

    // ----- Non-Visitor node 1 (should be ignored) -----
    DataPacketReceptionStats reception_stats (data_id,
                                              Ipv4Address ("10.0.0.99"),
                                              Seconds (50),
                                              true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.91"), reception_stats);

    // Get statistics (should get double values as -1.0)
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 0.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 1
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (40),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.1"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 10.0;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 2
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (20),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.2"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 0.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 3
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (20),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.3"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 0.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 4
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.4"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 0.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 11 (DROPS the packet)
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    reception_stats.SetPacketDropped ();
    packet_stats.CountReception (Ipv4Address ("10.0.0.11"), reception_stats);

    // The expected values remain unchanged, use the last computed values.

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 5
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.5"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 0.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // ----- Non-Visitor node 2 (should be ignored) -----
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (50),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.92"), reception_stats);

    // Get statistics (should be the same as the last computed expected statistics)
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 6
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (70),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.6"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 20.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 7
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (90),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.7"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 30.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 8
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (80),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.8"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 10.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 12 (DROPS the packet)
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    reception_stats.SetPacketDropped ();
    packet_stats.CountReception (Ipv4Address ("10.0.0.12"), reception_stats);

    // The expected values remain unchanged, use the last computed values.

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 9
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (10),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.9"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 0.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // *** Visitor node 10
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (120),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.10"), reception_stats);

    // Calculate expected values so far
    ++expected_confirmed_receivers_count;
    expected_delay_sum += 30.;
    expected_delay_average = expected_delay_sum / (double) expected_confirmed_receivers_count;
    expected_delivery_ratio = (double) expected_confirmed_receivers_count / (double) expected_receivers_count;

    // Get statistics
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // ----- Non-Visitor node 3 (should be ignored) -----
    reception_stats = DataPacketReceptionStats (data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (45),
                                                true);
    packet_stats.CountReception (Ipv4Address ("10.0.0.93"), reception_stats);

    // Get statistics (should be the same as the last computed expected statistics)
    packet_stats.GetStatistics (output_confirmed_receivers_count, output_expected_receivers_count,
                                output_delivery_ratio, output_average_delivery_delay,
                                output_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, expected_confirmed_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, expected_receivers_count, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, expected_delivery_ratio), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, expected_delay_average), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 625 * output_confirmed_receivers_count, "Must be equal");

    // Compare with manually calculated expected values
    NS_TEST_EXPECT_MSG_EQ (output_confirmed_receivers_count, 10u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_expected_receivers_count, 12u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_delivery_ratio, 10.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (output_average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (output_delivered_data_bytes, 6250u, "Must be equal");
  }

  void
  DoRun () override
  {
    TestConstructors ();
    TestCalculateRealPacketSize ();
    TestIsExpectedReceiverNodesSet ();
    TestClearExpectedReceiverNodes ();
    TestSetExpectedReceiverNodes ();
    TestCountReception ();
    TestGetStatistics ();
    TestToString ();
    TestEqualityRelationalOperators ();
    TestOrderingRelationalOperators ();
    SimulateScenario ();
  }
};


// =============================================================================
//                              SimulationStatisticsTest
// =============================================================================

/**
 * SimulationStatistics test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class SimulationStatisticsTest : public LibraryUtilsTestCase
{
protected:

  std::vector<Area> m_destination_areas;
  std::vector<DataPacketStatistics> m_data_packets_list;
  GeoTemporalAreasVisitorNodes m_gta_visitor_nodes;
  std::vector<PacketsCounter> m_nodes_packets_counter;
  std::map<uint32_t, Ipv4Address> m_node_id_to_ip;
  std::map<Ipv4Address, uint32_t> m_node_ip_to_id;

public:

  SimulationStatisticsTest () : SimulationStatisticsTest ("SimulationStatistics") { }

  SimulationStatisticsTest (std::string name) : LibraryUtilsTestCase (name),
  m_destination_areas (), m_data_packets_list (), m_gta_visitor_nodes (),
  m_nodes_packets_counter (), m_node_id_to_ip (), m_node_ip_to_id ()
  {
    SetUpInternalData ();
  }

private:

  void
  SetUpInternalData ()
  {
    m_destination_areas = {
      Area (0, 0, 10, 10),
      Area (10, 10, 20, 20),
      Area (20, 20, 30, 30),
      Area (30, 30, 40, 40),
      Area (40, 40, 50, 50)
    };

    // Create data packets and populate geo temporal areas visitor nodes object

    // - First geo-temporal area
    GeoTemporalArea geo_temporal_area (TimePeriod (Seconds (30), Seconds (130)), m_destination_areas.at (0));
    m_gta_visitor_nodes.AddGeoTemporalArea (geo_temporal_area);

    // Create and store first data packet
    DataPacketStatistics packet_stats (/*Data ID*/ DataIdentifier ("10.0.0.0:1"),
                                       /*Source ID*/ 0,
                                       /*Creation time*/ Seconds (0),
                                       /*Message size*/ 512,
                                       /*DataHeader size*/ 585, // 512 bytes (message) + 1 byte (terminator) + 72 bytes (other fields in DATA header)
                                       /*Geo-temporal area*/ geo_temporal_area);
    m_data_packets_list.push_back (packet_stats);

    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (0, 30));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (1, 30));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (2, 30));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (3, 30));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (4, 30));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (5, 40));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (6, 50));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (7, 60));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (8, 70));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (9, 80));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (10, 90));

    // Other 2 expected receiver nodes
    for (uint32_t i = 0u; i < 2; ++i)
      m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (11 + i, 50 + (i * 20)));

    // - Second geo-temporal area
    geo_temporal_area = GeoTemporalArea (TimePeriod (Seconds (120), Seconds (220)), m_destination_areas.at (1));
    m_gta_visitor_nodes.AddGeoTemporalArea (geo_temporal_area);

    // Create and store second data packet
    packet_stats = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.0:2"),
                                         /*Source ID*/ 0,
                                         /*Creation time*/ Seconds (70),
                                         /*Message size*/ 1024,
                                         /*DataHeader size*/ 1097, // 1024 bytes (message) + 1 byte (terminator) + 72 bytes (other fields in DATA header)
                                         /*Geo-temporal area*/ geo_temporal_area);
    m_data_packets_list.push_back (packet_stats);

    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (0, 120));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (1, 120));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (2, 120));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (3, 190));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (4, 150));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (5, 220));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (6, 165));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (7, 135));

    // Other 11 expected receiver nodes
    for (uint32_t i = 0u; i < 11; ++i)
      m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (8 + i, 120 + (i * 10)));

    // - Third geo-temporal area
    geo_temporal_area = GeoTemporalArea (TimePeriod (Seconds (350), Seconds (450)), m_destination_areas.at (2));
    m_gta_visitor_nodes.AddGeoTemporalArea (geo_temporal_area);

    // Create and store third data packet
    packet_stats = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.0:3"),
                                         /*Source ID*/ 0,
                                         /*Creation time*/ Seconds (200),
                                         /*Message size*/ 512,
                                         /*DataHeader size*/ 585, // 512 bytes (message) + 1 byte (terminator) + 72 bytes (other fields in DATA header)
                                         /*Geo-temporal area*/ geo_temporal_area);
    m_data_packets_list.push_back (packet_stats);

    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (0, 370));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (1, 370));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (2, 405));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (3, 385));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (4, 425));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (5, 350));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (6, 350));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (7, 350));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (8, 435));

    // Other 20 expected receiver nodes
    for (uint32_t i = 0u; i < 20; ++i)
      m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (9 + i, 350 + (i * 5)));

    // - Fourth geo-temporal area
    geo_temporal_area = GeoTemporalArea (TimePeriod (Seconds (210), Seconds (310)), m_destination_areas.at (3));
    m_gta_visitor_nodes.AddGeoTemporalArea (geo_temporal_area);

    // Create and store fourth data packet
    packet_stats = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.0:4"),
                                         /*Source ID*/ 0,
                                         /*Creation time*/ Seconds (130),
                                         /*Message size*/ 1024,
                                         /*DataHeader size*/ 1097, // 1024 bytes (message) + 1 byte (terminator) + 72 bytes (other fields in DATA header)
                                         /*Geo-temporal area*/ geo_temporal_area);
    m_data_packets_list.push_back (packet_stats);

    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (0, 210));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (1, 210));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (2, 220));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (3, 215));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (4, 240));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (5, 275));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (6, 220));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (7, 210));
    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (8, 240));

    // - Fifth geo-temporal area
    geo_temporal_area = GeoTemporalArea (TimePeriod (Seconds (350), Seconds (450)), m_destination_areas.at (4));
    m_gta_visitor_nodes.AddGeoTemporalArea (geo_temporal_area);

    // Create and store fifth data packet
    packet_stats = DataPacketStatistics (/*Data ID*/ DataIdentifier ("10.0.0.0:5"),
                                         /*Source ID*/ 0,
                                         /*Creation time*/ Seconds (200),
                                         /*Message size*/ 256,
                                         /*DataHeader size*/ 329, // 256 bytes (message) + 1 byte (terminator) + 72 bytes (other fields in DATA header)
                                         /*Geo-temporal area*/ geo_temporal_area);
    m_data_packets_list.push_back (packet_stats);

    m_gta_visitor_nodes.AddVisitorNode (geo_temporal_area, VisitorNode (0, 350));

    // Create the transmitted packets of the nodes and stablish the mapping of
    // node IP to ID and viceversa.
    PacketsCounter packets_counter;
    char ip_str [30];
    Ipv4Address node_ip;

    for (uint32_t node_id = 0u; node_id < 40u; ++node_id)
      {
        packets_counter = PacketsCounter ();
        packets_counter.CountPacket (PacketClass::Control, 1050); // 40 * 1,050 = 42,000
        packets_counter.CountPacket (PacketClass::Data, 950); // 38 * 950 = 38,000

        m_nodes_packets_counter.push_back (packets_counter);

        std::sprintf (ip_str, "10.0.0.%u", node_id);
        node_ip = Ipv4Address (ip_str);

        m_node_id_to_ip.insert (std::make_pair (node_id, node_ip));
        m_node_ip_to_id.insert (std::make_pair (node_ip, node_id));
      }
  }

protected:

  void
  SetUpTransmittedPacketsCounters (SimulationStatistics & simulation_statistics)
  {
    for (uint32_t node_id = 0u; node_id < m_nodes_packets_counter.size (); ++node_id)
      {
        simulation_statistics.SetNodeTransmittedPacketsCounter (m_node_id_to_ip.at (node_id),
                                                                m_nodes_packets_counter.at (node_id));
      }
  }

  /**
   * Sets up the receivers nodes of the first data packet.
   *
   * Returns the DATA ID of the data packet.
   */
  DataIdentifier
  SetUpFirstPacketReceivers (SimulationStatistics & simulation_statistics)
  {
    simulation_statistics.AddDataPacket (m_data_packets_list.at (0));
    const DataIdentifier & packet_1_data_id = m_data_packets_list.at (0).GetDataIdentifier ();

    // Non-Visitor node 1 (should be ignored)
    DataPacketReceptionStats reception_stats (packet_1_data_id,
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (50),
                                              true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.91"), reception_stats);

    // Visitor node 1
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (40),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.1"), reception_stats);

    // Visitor node 2
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (20),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.2"), reception_stats);

    // Visitor node 3
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (20),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.3"), reception_stats);

    // Visitor node 4
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.4"), reception_stats);

    // Visitor node 5
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.5"), reception_stats);

    // Non-Visitor node 2 (should be ignored)
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (50),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.92"), reception_stats);

    // Visitor node 6
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (70),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.6"), reception_stats);

    // Visitor node 7
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (90),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.7"), reception_stats);

    // Visitor node 8
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (80),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.8"), reception_stats);

    // Non-Visitor node 3 (should be ignored)
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (45),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.93"), reception_stats);

    // Visitor node 9
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (10),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.9"), reception_stats);

    // Visitor node 10
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (120),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.10"), reception_stats);

    // Visitor node 11 (DROPS the packet)
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.11"), reception_stats);

    // Visitor node 12 (DROPS the packet)
    reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (30),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.12"), reception_stats);

    // Some non-expected nodes receptions
    char ip_address_str[20];

    for (uint32_t i = 0; i < 20; ++i)
      {
        reception_stats = DataPacketReceptionStats (packet_1_data_id,
                                                    Ipv4Address ("10.0.0.25"),
                                                    Seconds (10.0 + (i * 5.0)),
                                                    true);

        // Create the string of some IP address
        std::sprintf (ip_address_str, "10.0.0.%u", 100 + i);

        simulation_statistics.CountDataPacketReceiverNode (Ipv4Address (ip_address_str), reception_stats);
      }

    return packet_1_data_id;
  }

  /**
   * Sets up the receiver nodes of the second data packet.
   *
   * Returns the DATA ID of the data packet.
   */
  DataIdentifier
  SetUpSecondPacketRececeivers (SimulationStatistics & simulation_statistics)
  {
    simulation_statistics.AddDataPacket (m_data_packets_list.at (1));
    const DataIdentifier & packet_2_data_id = m_data_packets_list.at (1).GetDataIdentifier ();

    // Visitor node 1
    DataPacketReceptionStats reception_stats (packet_2_data_id,
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (135),
                                              true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.1"), reception_stats);

    // Visitor node 2
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (100),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.2"), reception_stats);

    // Visitor node 3
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (155),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.3"), reception_stats);

    // Visitor node 4
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (175),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.4"), reception_stats);

    // Visitor node 5
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (140),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.5"), reception_stats);

    // Visitor node 6
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (200),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.6"), reception_stats);

    // Visitor node 7 (First non-receiver that was expected to receive it)
    reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (80),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.7"), reception_stats);

    // Some non-expected nodes receptions
    char ip_address_str[20];

    for (uint32_t i = 0; i < 20; ++i)
      {
        reception_stats = DataPacketReceptionStats (packet_2_data_id,
                                                    Ipv4Address ("10.0.0.25"),
                                                    Seconds (70.0 + (i * 5.0)),
                                                    true);

        // Create the string of some IP address
        std::sprintf (ip_address_str, "10.0.0.%u", 100 + i);

        simulation_statistics.CountDataPacketReceiverNode (Ipv4Address (ip_address_str), reception_stats);
      }

    return packet_2_data_id;
  }

  /**
   * Sets up the receiver nodes of the third data packet.
   *
   * Returns the DATA ID of the data packet.
   */
  DataIdentifier
  SetUpThirdPacketReceivers (SimulationStatistics & simulation_statistics)
  {
    simulation_statistics.AddDataPacket (m_data_packets_list.at (2));
    const DataIdentifier & packet_3_data_id = m_data_packets_list.at (2).GetDataIdentifier ();

    // Visitor node 1
    DataPacketReceptionStats reception_stats (packet_3_data_id,
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (230),
                                              true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.1"), reception_stats);

    // Visitor node 2 (First non-receiver that was expected to receive it)
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (210),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.2"), reception_stats);

    // Visitor node 3 (Second non-receiver that was expected to receive it)
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (225),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.3"), reception_stats);

    // Visitor node 4
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (445),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.4"), reception_stats);

    // Visitor node 5
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (405),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.5"), reception_stats);

    // Visitor node 6
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (420),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.6"), reception_stats);

    // Visitor node 7
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (380),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.7"), reception_stats);

    // Visitor node 8
    reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (395),
                                                true);
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.8"), reception_stats);

    // Some non-expected nodes receptions
    char ip_address_str[20];

    for (uint32_t i = 0; i < 20; ++i)
      {
        reception_stats = DataPacketReceptionStats (packet_3_data_id,
                                                    Ipv4Address ("10.0.0.25"),
                                                    Seconds (330.0 + (i * 5.0)),
                                                    true);

        // Create the string of some IP address
        std::sprintf (ip_address_str, "10.0.0.%u", 100 + i);

        simulation_statistics.CountDataPacketReceiverNode (Ipv4Address (ip_address_str), reception_stats);
      }

    return packet_3_data_id;
  }

  /**
   * Sets up the receiver nodes of the fourth data packet.
   *
   * Returns the DATA ID of the data packet.
   */
  DataIdentifier
  SetUpFourthPacketReceivers (SimulationStatistics & simulation_statistics)
  {
    simulation_statistics.AddDataPacket (m_data_packets_list.at (3));
    const DataIdentifier & packet_4_data_id = m_data_packets_list.at (3).GetDataIdentifier ();

    // Visitor node 1 (First non-receiver that was expected to receive it)
    DataPacketReceptionStats reception_stats (packet_4_data_id,
                                              Ipv4Address ("10.0.0.25"),
                                              Seconds (180),
                                              true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.1"), reception_stats);

    // Visitor node 2 (Second non-receiver that was expected to receive it)
    reception_stats = DataPacketReceptionStats (packet_4_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (170),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.2"), reception_stats);

    // Some non-expected nodes receptions
    char ip_address_str[20];

    for (uint32_t i = 0; i < 20; ++i)
      {
        reception_stats = DataPacketReceptionStats (packet_4_data_id,
                                                    Ipv4Address ("10.0.0.25"),
                                                    Seconds (180.0 + (i * 5.0)),
                                                    true);

        // Create the string of some IP address
        std::sprintf (ip_address_str, "10.0.0.%u", 100 + i);

        simulation_statistics.CountDataPacketReceiverNode (Ipv4Address (ip_address_str), reception_stats);
      }

    return packet_4_data_id;
  }

  /**
   * Sets up the receiver nodes of the fifth data packet.
   *
   * Returns the DATA ID of the data packet.
   */
  DataIdentifier
  SetUpFifthPacketReceivers (SimulationStatistics & simulation_statistics)
  {
    simulation_statistics.AddDataPacket (m_data_packets_list.at (4));
    const DataIdentifier & packet_5_data_id = m_data_packets_list.at (4).GetDataIdentifier ();

    DataPacketReceptionStats reception_stats;

    // Some non-expected nodes receptions
    char ip_address_str[20];

    for (uint32_t i = 0; i < 20; ++i)
      {
        reception_stats = DataPacketReceptionStats (packet_5_data_id,
                                                    Ipv4Address ("10.0.0.25"),
                                                    Seconds (300.0 + (i * 5.0)),
                                                    true);

        // Create the string of some IP address
        std::sprintf (ip_address_str, "10.0.0.%u", 100 + i);

        simulation_statistics.CountDataPacketReceiverNode (Ipv4Address (ip_address_str), reception_stats);
      }

    // Node 1 that dropped the packet
    reception_stats = DataPacketReceptionStats (packet_5_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (180),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.1"), reception_stats);

    // Node 2 that dropped the packet
    reception_stats = DataPacketReceptionStats (packet_5_data_id,
                                                Ipv4Address ("10.0.0.25"),
                                                Seconds (170),
                                                true);
    reception_stats.SetPacketDropped ();
    simulation_statistics.CountDataPacketReceiverNode (Ipv4Address ("10.0.0.2"), reception_stats);

    return packet_5_data_id;
  }

public:

  virtual void
  TestGetStatistics ()
  {
    SimulationStatistics simulation_stats (m_gta_visitor_nodes,
                                           m_node_id_to_ip);

    bool result;
    double total_average_delivery_delay, total_average_delivery_ratio, total_overhead,
            data_overhead, control_overhead;
    uint64_t total_transmitted_bytes, data_transmitted_bytes, control_transmitted_bytes,
            total_delivered_data_bytes;
    uint32_t total_expected_receivers, total_confirmed_receivers;

    // ======= No packets yet =======

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 0u, "Must be equal");

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats);

    // ======= First packet =======

    SetUpFirstPacketReceivers (simulation_stats);

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 10.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 6490.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 6490.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 6490.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6490u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 12, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 10, "Must be equal");

    // ======= Second packet =======

    SetUpSecondPacketRececeivers (simulation_stats);

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 11.25), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 7.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 13456.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 13456.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 13456.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 13456u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 30, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 16, "Must be equal");

    // ======= Third packet =======

    SetUpThirdPacketReceivers (simulation_stats);

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 63.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 58, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");

    // ======= Fourth packet =======

    SetUpFourthPacketReceivers (simulation_stats);

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 84.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 66, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");

    // ======= Fifth packet =======

    SetUpFifthPacketReceivers (simulation_stats);

    result = simulation_stats.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                             total_overhead, data_overhead, control_overhead,
                                             total_transmitted_bytes, data_transmitted_bytes,
                                             control_transmitted_bytes, total_delivered_data_bytes,
                                             total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 84.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 66, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");
  }

  virtual void
  TestGetDataPacketStatistics ()
  {
    SimulationStatistics simulation_stats (m_gta_visitor_nodes,
                                           m_node_id_to_ip);

    uint32_t packet_confirmed_receivers_count, packet_expected_receivers_count;
    double packet_delivery_ratio, packet_average_delivery_delay;
    uint32_t packet_delivered_data_bytes;

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats);

    // ======= First packet =======

    const DataIdentifier packet_1_data_id = SetUpFirstPacketReceivers (simulation_stats);

    simulation_stats.GetDataPacketStatistics (packet_1_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 10u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 12u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 10.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 6490u, "Must be equal");

    // ======= Second packet =======

    const DataIdentifier packet_2_data_id = SetUpSecondPacketRececeivers (simulation_stats);

    simulation_stats.GetDataPacketStatistics (packet_2_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 6u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 18u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 6.0 / 18.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 12.5), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 6966u, "Must be equal");

    // ======= Third packet =======

    const DataIdentifier packet_3_data_id = SetUpThirdPacketReceivers (simulation_stats);

    simulation_stats.GetDataPacketStatistics (packet_3_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 6u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 28u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 6.0 / 28.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 175.0 / 6.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 3894u, "Must be equal");

    // ======= Fourth packet =======

    const DataIdentifier packet_4_data_id = SetUpFourthPacketReceivers (simulation_stats);

    simulation_stats.GetDataPacketStatistics (packet_4_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 8u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 0.0 / 8.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 0u, "Must be equal");

    // ======= Fifth packet =======

    const DataIdentifier packet_5_data_id = SetUpFifthPacketReceivers (simulation_stats);

    simulation_stats.GetDataPacketStatistics (packet_5_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 0u, "Must be equal");
  }

  virtual void
  TestGetAreaStatistics ()
  {
    SimulationStatistics simulation_stats (m_gta_visitor_nodes,
                                           m_node_id_to_ip);

    bool result;
    Area destination_area;
    double average_delivery_delay, average_delivery_ratio;
    uint64_t total_delivered_data_bytes;
    uint32_t expected_receivers, confirmed_receivers;


    // ======= No packets yet =======

    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats);

    SetUpFirstPacketReceivers (simulation_stats);
    SetUpSecondPacketRececeivers (simulation_stats);
    SetUpThirdPacketReceivers (simulation_stats);
    SetUpFourthPacketReceivers (simulation_stats);
    SetUpFifthPacketReceivers (simulation_stats);

    // ======= First area =======

    destination_area = m_destination_areas.at (0);
    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 10.0 / 12.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6490, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 12, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 10, "Must be equal");

    // ======= Second area =======

    destination_area = m_destination_areas.at (1);
    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 12.5), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 6.0 / 18.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6966, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 18, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 6, "Must be equal");

    // ======= Third area =======

    destination_area = m_destination_areas.at (2);
    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 175.0 / 6.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 6. / 28.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 3894, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 28, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 6, "Must be equal");

    // ======= Unknown area =======

    result = simulation_stats.GetAreaStatistics (Area (-10, -10, -20, -20), average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // ======= Fourth area =======

    destination_area = m_destination_areas.at (3);
    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 0.0 / 8.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 8, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // ======= Fifth area =======

    destination_area = m_destination_areas.at (4);
    result = simulation_stats.GetAreaStatistics (destination_area, average_delivery_delay,
                                                 average_delivery_ratio, total_delivered_data_bytes,
                                                 expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");
  }

  void
  DoRun () override
  {
    TestGetStatistics ();
    TestGetDataPacketStatistics ();
    TestGetAreaStatistics ();
  }
};


// =============================================================================
//                         SimulationStatisticsFileTest
// =============================================================================

/**
 * SimulationStatisticsFile test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class SimulationStatisticsFileTest : public SimulationStatisticsTest
{
protected:

  std::string m_output_filename;

public:

  SimulationStatisticsFileTest ()
  : SimulationStatisticsTest ("SimulationStatisticsFile"), m_output_filename ()
  {
    m_output_filename = "test_SimulationStatisticsFile.xml";
  }

  void
  TestGetStatistics () override
  {
    SimulationStatisticsFile simulation_stats_file (m_gta_visitor_nodes,
                                                    m_node_id_to_ip);

    bool result;
    double total_average_delivery_delay, total_average_delivery_ratio, total_overhead,
            data_overhead, control_overhead;
    uint64_t total_transmitted_bytes, data_transmitted_bytes, control_transmitted_bytes,
            total_delivered_data_bytes;
    uint32_t total_expected_receivers, total_confirmed_receivers;

    // ======= No packets yet =======

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 0u, "Must be equal");

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats_file);

    // ======= First packet =======

    SetUpFirstPacketReceivers (simulation_stats_file);

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 10.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 6490.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 6490.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 6490.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6490u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 12, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 10, "Must be equal");

    // ======= Second packet =======

    SetUpSecondPacketRececeivers (simulation_stats_file);

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 11.25), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 7.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 13456.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 13456.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 13456.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 13456u, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 30, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 16, "Must be equal");

    // ======= Third packet =======

    SetUpThirdPacketReceivers (simulation_stats_file);

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 63.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 58, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");

    // ======= Fourth packet =======

    SetUpFourthPacketReceivers (simulation_stats_file);

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 84.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 66, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");

    // ======= Fifth packet =======

    SetUpFifthPacketReceivers (simulation_stats_file);

    result = simulation_stats_file.GetStatistics (total_average_delivery_delay, total_average_delivery_ratio,
                                                  total_overhead, data_overhead, control_overhead,
                                                  total_transmitted_bytes, data_transmitted_bytes,
                                                  control_transmitted_bytes, total_delivered_data_bytes,
                                                  total_expected_receivers, total_confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_delay, 155.0 / 9.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_average_delivery_ratio, 29.0 / 84.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (total_overhead, 80000.0 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (data_overhead, 38000 / 17350.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (control_overhead, 42000.0 / 17350.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_transmitted_bytes, 80000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (data_transmitted_bytes, 38000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (control_transmitted_bytes, 42000u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 17350, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_expected_receivers, 66, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (total_confirmed_receivers, 22, "Must be equal");
  }

  void
  TestGetDataPacketStatistics () override
  {
    SimulationStatisticsFile simulation_stats_file (m_gta_visitor_nodes,
                                                    m_node_id_to_ip);

    uint32_t packet_confirmed_receivers_count, packet_expected_receivers_count;
    double packet_delivery_ratio, packet_average_delivery_delay;
    uint32_t packet_delivered_data_bytes;

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats_file);

    // ======= First packet =======

    const DataIdentifier packet_1_data_id = SetUpFirstPacketReceivers (simulation_stats_file);

    simulation_stats_file.GetDataPacketStatistics (packet_1_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 10u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 12u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 10.0 / 12.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 6490u, "Must be equal");

    // ======= Second packet =======

    const DataIdentifier packet_2_data_id = SetUpSecondPacketRececeivers (simulation_stats_file);

    simulation_stats_file.GetDataPacketStatistics (packet_2_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 6u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 18u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 6.0 / 18.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 12.5), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 6966u, "Must be equal");

    // ======= Third packet =======

    const DataIdentifier packet_3_data_id = SetUpThirdPacketReceivers (simulation_stats_file);

    simulation_stats_file.GetDataPacketStatistics (packet_3_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 6u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 28u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 6.0 / 28.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, 175.0 / 6.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 3894u, "Must be equal");

    // ======= Fourth packet =======

    const DataIdentifier packet_4_data_id = SetUpFourthPacketReceivers (simulation_stats_file);

    simulation_stats_file.GetDataPacketStatistics (packet_4_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 8u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, 0.0 / 8.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 0u, "Must be equal");

    // ======= Fifth packet =======

    const DataIdentifier packet_5_data_id = SetUpFifthPacketReceivers (simulation_stats_file);

    simulation_stats_file.GetDataPacketStatistics (packet_5_data_id)
            .GetStatistics (packet_confirmed_receivers_count, packet_expected_receivers_count,
                            packet_delivery_ratio, packet_average_delivery_delay,
                            packet_delivered_data_bytes);

    NS_TEST_EXPECT_MSG_EQ (packet_confirmed_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_expected_receivers_count, 0u, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_delivery_ratio, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (packet_average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (packet_delivered_data_bytes, 0u, "Must be equal");
  }

  void
  TestGetAreaStatistics () override
  {
    SimulationStatisticsFile simulation_stats_file (m_gta_visitor_nodes,
                                                    m_node_id_to_ip);

    bool result;
    Area destination_area;
    double average_delivery_delay, average_delivery_ratio;
    uint64_t total_delivered_data_bytes;
    uint32_t expected_receivers, confirmed_receivers;


    // ======= No packets yet =======

    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // Set up transmitted packet counters
    SetUpTransmittedPacketsCounters (simulation_stats_file);

    SetUpFirstPacketReceivers (simulation_stats_file);
    SetUpSecondPacketRececeivers (simulation_stats_file);
    SetUpThirdPacketReceivers (simulation_stats_file);
    SetUpFourthPacketReceivers (simulation_stats_file);
    SetUpFifthPacketReceivers (simulation_stats_file);

    // ======= First area =======

    destination_area = m_destination_areas.at (0);
    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 10.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 10.0 / 12.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6490, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 12, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 10, "Must be equal");

    // ======= Second area =======

    destination_area = m_destination_areas.at (1);
    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 12.5), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 6.0 / 18.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 6966, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 18, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 6, "Must be equal");

    // ======= Third area =======

    destination_area = m_destination_areas.at (2);
    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, 175.0 / 6.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 6. / 28.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 3894, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 28, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 6, "Must be equal");

    // ======= Unknown area =======

    result = simulation_stats_file.GetAreaStatistics (Area (-10, -10, -20, -20), average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, false, "Must be false");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // ======= Fourth area =======

    destination_area = m_destination_areas.at (3);
    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, 0.0 / 8.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 8, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");

    // ======= Fifth area =======

    destination_area = m_destination_areas.at (4);
    result = simulation_stats_file.GetAreaStatistics (destination_area, average_delivery_delay,
                                                      average_delivery_ratio, total_delivered_data_bytes,
                                                      expected_receivers, confirmed_receivers);
    NS_TEST_EXPECT_MSG_EQ (result, true, "Must be true");

    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_delay, -1.0), true, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (AlmostEqual (average_delivery_ratio, -1.0), true, "Must be equal");

    NS_TEST_EXPECT_MSG_EQ (total_delivered_data_bytes, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (expected_receivers, 0, "Must be equal");
    NS_TEST_EXPECT_MSG_EQ (confirmed_receivers, 0, "Must be equal");
  }

  void
  TestSaveToXmlFile ()
  {
    SimulationStatisticsFile simulation_stats_file (m_gta_visitor_nodes,
                                                    m_node_id_to_ip);

    // ---- Empty SimulationStatisticsFile object -----

    simulation_stats_file.SaveToXmlFile (m_output_filename);

    std::string exported_file_content;
    TestUtils::ReadFile (m_output_filename, exported_file_content);

    std::string expected_str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<document>\n\n<statistics-results>\n  <result type=\"total\" average-delivery-ratio=\"-1.000000\" average-delivery-delay=\"-1.000000\" total-overhead=\"-1.000000\" data-overhead=\"-1.000000\" control-overhead=\"-1.000000\" total-transmitted-bytes=\"0\" data-transmitted-bytes=\"0\" control-transmitted-bytes=\"0\" total-delivered-data-bytes=\"0\" expected-receiver-nodes=\"0\" confirmed-receiver-nodes=\"0\" />\n</statistics-results>\n\n<nodes-transmitted-packets>\n</nodes-transmitted-packets>\n\n<data-packets>\n</data-packets>\n\n</document>\n";

    NS_TEST_EXPECT_MSG_EQ (exported_file_content, expected_str, "Must be equal");

    // ---- Set up transmitted packet counters -----

    SetUpTransmittedPacketsCounters (simulation_stats_file);

    simulation_stats_file.SaveToXmlFile (m_output_filename);
    TestUtils::ReadFile (m_output_filename, exported_file_content);

    expected_str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<document>\n\n<statistics-results>\n  <result type=\"total\" average-delivery-ratio=\"-1.000000\" average-delivery-delay=\"-1.000000\" total-overhead=\"-1.000000\" data-overhead=\"-1.000000\" control-overhead=\"-1.000000\" total-transmitted-bytes=\"0\" data-transmitted-bytes=\"0\" control-transmitted-bytes=\"0\" total-delivered-data-bytes=\"0\" expected-receiver-nodes=\"0\" confirmed-receiver-nodes=\"0\" />\n</statistics-results>\n\n<nodes-transmitted-packets>\n  <node-transmitted-packets node-ip=\"10.0.0.0\" node-id=\"0\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.1\" node-id=\"1\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.2\" node-id=\"2\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.3\" node-id=\"3\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.4\" node-id=\"4\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.5\" node-id=\"5\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.6\" node-id=\"6\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.7\" node-id=\"7\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.8\" node-id=\"8\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.9\" node-id=\"9\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.10\" node-id=\"10\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.11\" node-id=\"11\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.12\" node-id=\"12\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.13\" node-id=\"13\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.14\" node-id=\"14\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.15\" node-id=\"15\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.16\" node-id=\"16\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.17\" node-id=\"17\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.18\" node-id=\"18\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.19\" node-id=\"19\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.20\" node-id=\"20\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.21\" node-id=\"21\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.22\" node-id=\"22\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.23\" node-id=\"23\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.24\" node-id=\"24\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.25\" node-id=\"25\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.26\" node-id=\"26\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.27\" node-id=\"27\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.28\" node-id=\"28\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.29\" node-id=\"29\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.30\" node-id=\"30\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.31\" node-id=\"31\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.32\" node-id=\"32\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.33\" node-id=\"33\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.34\" node-id=\"34\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.35\" node-id=\"35\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.36\" node-id=\"36\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.37\" node-id=\"37\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.38\" node-id=\"38\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.39\" node-id=\"39\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n</nodes-transmitted-packets>\n\n<data-packets>\n</data-packets>\n\n</document>\n";

    NS_TEST_EXPECT_MSG_EQ (exported_file_content, expected_str, "Must be equal");

    // ---- Setup some packets ----
    SetUpFirstPacketReceivers (simulation_stats_file);
    SetUpSecondPacketRececeivers (simulation_stats_file);
    SetUpThirdPacketReceivers (simulation_stats_file);

    simulation_stats_file.SaveToXmlFile (m_output_filename);
    TestUtils::ReadFile (m_output_filename, exported_file_content);

    expected_str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<document>\n\n<statistics-results>\n  <result type=\"total\" average-delivery-ratio=\"0.460317\" average-delivery-delay=\"17.222222\" total-overhead=\"4.610951\" data-overhead=\"2.190202\" control-overhead=\"2.420749\" total-transmitted-bytes=\"80000\" data-transmitted-bytes=\"38000\" control-transmitted-bytes=\"42000\" total-delivered-data-bytes=\"17350\" expected-receiver-nodes=\"58\" confirmed-receiver-nodes=\"22\" />\n  <result type=\"area\" area=\"0.000000,0.000000, 10.000000,10.000000\" average-delivery-ratio=\"0.833333\" average-delivery-delay=\"10.000000\" total-delivered-data-bytes=\"6490\" expected-receiver-nodes=\"12\" confirmed-receiver-nodes=\"10\" />\n  <result type=\"area\" area=\"10.000000,10.000000, 20.000000,20.000000\" average-delivery-ratio=\"0.333333\" average-delivery-delay=\"12.500000\" total-delivered-data-bytes=\"6966\" expected-receiver-nodes=\"18\" confirmed-receiver-nodes=\"6\" />\n  <result type=\"area\" area=\"20.000000,20.000000, 30.000000,30.000000\" average-delivery-ratio=\"0.214286\" average-delivery-delay=\"29.166667\" total-delivered-data-bytes=\"3894\" expected-receiver-nodes=\"28\" confirmed-receiver-nodes=\"6\" />\n</statistics-results>\n\n<nodes-transmitted-packets>\n  <node-transmitted-packets node-ip=\"10.0.0.0\" node-id=\"0\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.1\" node-id=\"1\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.2\" node-id=\"2\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.3\" node-id=\"3\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.4\" node-id=\"4\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.5\" node-id=\"5\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.6\" node-id=\"6\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.7\" node-id=\"7\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.8\" node-id=\"8\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.9\" node-id=\"9\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.10\" node-id=\"10\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.11\" node-id=\"11\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.12\" node-id=\"12\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.13\" node-id=\"13\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.14\" node-id=\"14\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.15\" node-id=\"15\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.16\" node-id=\"16\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.17\" node-id=\"17\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.18\" node-id=\"18\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.19\" node-id=\"19\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.20\" node-id=\"20\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.21\" node-id=\"21\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.22\" node-id=\"22\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.23\" node-id=\"23\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.24\" node-id=\"24\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.25\" node-id=\"25\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.26\" node-id=\"26\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.27\" node-id=\"27\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.28\" node-id=\"28\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.29\" node-id=\"29\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.30\" node-id=\"30\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.31\" node-id=\"31\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.32\" node-id=\"32\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.33\" node-id=\"33\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.34\" node-id=\"34\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.35\" node-id=\"35\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.36\" node-id=\"36\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.37\" node-id=\"37\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.38\" node-id=\"38\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.39\" node-id=\"39\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n</nodes-transmitted-packets>\n\n<data-packets>\n  <data-packet data-id=\"10.0.0.0:1\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"0.000000,0.000000, 10.000000,10.000000\" temporal-scope=\"30.00,130.00\" creation-time=\"0.000000\" data-message-size=\"512\" packet-size=\"649\" confirmed-receiver-nodes=\"10\" expected-receiver-nodes=\"12\" delivery-ratio=\"0.833333\" delivery-delay=\"10.000000\" delivered-data-bytes=\"6490\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"40.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.2\" node-id=\"2\" received-from=\"10.0.0.25\" time=\"20.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.3\" node-id=\"3\" received-from=\"10.0.0.25\" time=\"20.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"30.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"30.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"70.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.7\" node-id=\"7\" received-from=\"10.0.0.25\" time=\"90.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.8\" node-id=\"8\" received-from=\"10.0.0.25\" time=\"80.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.9\" node-id=\"9\" received-from=\"10.0.0.25\" time=\"10.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.10\" node-id=\"10\" received-from=\"10.0.0.25\" time=\"120.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n  <data-packet data-id=\"10.0.0.0:2\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"10.000000,10.000000, 20.000000,20.000000\" temporal-scope=\"120.00,220.00\" creation-time=\"70.000000\" data-message-size=\"1024\" packet-size=\"1161\" confirmed-receiver-nodes=\"6\" expected-receiver-nodes=\"18\" delivery-ratio=\"0.333333\" delivery-delay=\"12.500000\" delivered-data-bytes=\"6966\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"135.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.2\" node-id=\"2\" received-from=\"10.0.0.25\" time=\"100.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.3\" node-id=\"3\" received-from=\"10.0.0.25\" time=\"155.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"175.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"140.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"200.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n  <data-packet data-id=\"10.0.0.0:3\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"20.000000,20.000000, 30.000000,30.000000\" temporal-scope=\"350.00,450.00\" creation-time=\"200.000000\" data-message-size=\"512\" packet-size=\"649\" confirmed-receiver-nodes=\"6\" expected-receiver-nodes=\"28\" delivery-ratio=\"0.214286\" delivery-delay=\"29.166667\" delivered-data-bytes=\"3894\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"230.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"445.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"405.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"420.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.7\" node-id=\"7\" received-from=\"10.0.0.25\" time=\"380.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.8\" node-id=\"8\" received-from=\"10.0.0.25\" time=\"395.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n</data-packets>\n\n</document>\n";

    NS_TEST_EXPECT_MSG_EQ (exported_file_content, expected_str, "Must be equal");

    // ---- Setup some packets more ----
    SetUpFourthPacketReceivers (simulation_stats_file);
    SetUpFifthPacketReceivers (simulation_stats_file);

    simulation_stats_file.SaveToXmlFile (m_output_filename);
    TestUtils::ReadFile (m_output_filename, exported_file_content);

    expected_str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<document>\n\n<statistics-results>\n  <result type=\"total\" average-delivery-ratio=\"0.345238\" average-delivery-delay=\"17.222222\" total-overhead=\"4.610951\" data-overhead=\"2.190202\" control-overhead=\"2.420749\" total-transmitted-bytes=\"80000\" data-transmitted-bytes=\"38000\" control-transmitted-bytes=\"42000\" total-delivered-data-bytes=\"17350\" expected-receiver-nodes=\"66\" confirmed-receiver-nodes=\"22\" />\n  <result type=\"area\" area=\"0.000000,0.000000, 10.000000,10.000000\" average-delivery-ratio=\"0.833333\" average-delivery-delay=\"10.000000\" total-delivered-data-bytes=\"6490\" expected-receiver-nodes=\"12\" confirmed-receiver-nodes=\"10\" />\n  <result type=\"area\" area=\"10.000000,10.000000, 20.000000,20.000000\" average-delivery-ratio=\"0.333333\" average-delivery-delay=\"12.500000\" total-delivered-data-bytes=\"6966\" expected-receiver-nodes=\"18\" confirmed-receiver-nodes=\"6\" />\n  <result type=\"area\" area=\"20.000000,20.000000, 30.000000,30.000000\" average-delivery-ratio=\"0.214286\" average-delivery-delay=\"29.166667\" total-delivered-data-bytes=\"3894\" expected-receiver-nodes=\"28\" confirmed-receiver-nodes=\"6\" />\n  <result type=\"area\" area=\"30.000000,30.000000, 40.000000,40.000000\" average-delivery-ratio=\"0.000000\" average-delivery-delay=\"-1.000000\" total-delivered-data-bytes=\"0\" expected-receiver-nodes=\"8\" confirmed-receiver-nodes=\"0\" />\n  <result type=\"area\" area=\"40.000000,40.000000, 50.000000,50.000000\" average-delivery-ratio=\"-1.000000\" average-delivery-delay=\"-1.000000\" total-delivered-data-bytes=\"0\" expected-receiver-nodes=\"0\" confirmed-receiver-nodes=\"0\" />\n</statistics-results>\n\n<nodes-transmitted-packets>\n  <node-transmitted-packets node-ip=\"10.0.0.0\" node-id=\"0\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.1\" node-id=\"1\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.2\" node-id=\"2\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.3\" node-id=\"3\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.4\" node-id=\"4\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.5\" node-id=\"5\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.6\" node-id=\"6\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.7\" node-id=\"7\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.8\" node-id=\"8\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.9\" node-id=\"9\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.10\" node-id=\"10\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.11\" node-id=\"11\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.12\" node-id=\"12\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.13\" node-id=\"13\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.14\" node-id=\"14\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.15\" node-id=\"15\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.16\" node-id=\"16\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.17\" node-id=\"17\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.18\" node-id=\"18\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.19\" node-id=\"19\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.20\" node-id=\"20\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.21\" node-id=\"21\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.22\" node-id=\"22\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.23\" node-id=\"23\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.24\" node-id=\"24\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.25\" node-id=\"25\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.26\" node-id=\"26\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.27\" node-id=\"27\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.28\" node-id=\"28\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.29\" node-id=\"29\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.30\" node-id=\"30\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.31\" node-id=\"31\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.32\" node-id=\"32\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.33\" node-id=\"33\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.34\" node-id=\"34\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.35\" node-id=\"35\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.36\" node-id=\"36\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.37\" node-id=\"37\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.38\" node-id=\"38\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n  <node-transmitted-packets node-ip=\"10.0.0.39\" node-id=\"39\" control-packets-count=\"1\" control-packets-size=\"1050\" data-packets-count=\"1\" data-packets-size=\"950\" />\n</nodes-transmitted-packets>\n\n<data-packets>\n  <data-packet data-id=\"10.0.0.0:1\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"0.000000,0.000000, 10.000000,10.000000\" temporal-scope=\"30.00,130.00\" creation-time=\"0.000000\" data-message-size=\"512\" packet-size=\"649\" confirmed-receiver-nodes=\"10\" expected-receiver-nodes=\"12\" delivery-ratio=\"0.833333\" delivery-delay=\"10.000000\" delivered-data-bytes=\"6490\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"40.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.2\" node-id=\"2\" received-from=\"10.0.0.25\" time=\"20.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.3\" node-id=\"3\" received-from=\"10.0.0.25\" time=\"20.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"30.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"30.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"70.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.7\" node-id=\"7\" received-from=\"10.0.0.25\" time=\"90.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.8\" node-id=\"8\" received-from=\"10.0.0.25\" time=\"80.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.9\" node-id=\"9\" received-from=\"10.0.0.25\" time=\"10.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.10\" node-id=\"10\" received-from=\"10.0.0.25\" time=\"120.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n  <data-packet data-id=\"10.0.0.0:2\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"10.000000,10.000000, 20.000000,20.000000\" temporal-scope=\"120.00,220.00\" creation-time=\"70.000000\" data-message-size=\"1024\" packet-size=\"1161\" confirmed-receiver-nodes=\"6\" expected-receiver-nodes=\"18\" delivery-ratio=\"0.333333\" delivery-delay=\"12.500000\" delivered-data-bytes=\"6966\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"135.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.2\" node-id=\"2\" received-from=\"10.0.0.25\" time=\"100.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.3\" node-id=\"3\" received-from=\"10.0.0.25\" time=\"155.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"175.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"140.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"200.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n  <data-packet data-id=\"10.0.0.0:3\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"20.000000,20.000000, 30.000000,30.000000\" temporal-scope=\"350.00,450.00\" creation-time=\"200.000000\" data-message-size=\"512\" packet-size=\"649\" confirmed-receiver-nodes=\"6\" expected-receiver-nodes=\"28\" delivery-ratio=\"0.214286\" delivery-delay=\"29.166667\" delivered-data-bytes=\"3894\">\n    <receiver-node node-ip=\"10.0.0.1\" node-id=\"1\" received-from=\"10.0.0.25\" time=\"230.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.4\" node-id=\"4\" received-from=\"10.0.0.25\" time=\"445.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.5\" node-id=\"5\" received-from=\"10.0.0.25\" time=\"405.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.6\" node-id=\"6\" received-from=\"10.0.0.25\" time=\"420.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.7\" node-id=\"7\" received-from=\"10.0.0.25\" time=\"380.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n    <receiver-node node-ip=\"10.0.0.8\" node-id=\"8\" received-from=\"10.0.0.25\" time=\"395.000000\" intended-receiver=\"True\" unicast-transmitted-replicas=\"0\" broadcast-transmitted-replicas=\"0\" received-duplicates=\"0\" />\n  </data-packet>\n\n  <data-packet data-id=\"10.0.0.0:4\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"30.000000,30.000000, 40.000000,40.000000\" temporal-scope=\"210.00,310.00\" creation-time=\"130.000000\" data-message-size=\"1024\" packet-size=\"1161\" confirmed-receiver-nodes=\"0\" expected-receiver-nodes=\"8\" delivery-ratio=\"0.000000\" delivery-delay=\"-1.000000\" delivered-data-bytes=\"0\" />\n\n  <data-packet data-id=\"10.0.0.0:5\" source-node-ip=\"10.0.0.0\" source-node-id=\"0\" destination-area=\"40.000000,40.000000, 50.000000,50.000000\" temporal-scope=\"350.00,450.00\" creation-time=\"200.000000\" data-message-size=\"256\" packet-size=\"393\" confirmed-receiver-nodes=\"0\" expected-receiver-nodes=\"0\" delivery-ratio=\"-1.000000\" delivery-delay=\"-1.000000\" delivered-data-bytes=\"0\" />\n\n</data-packets>\n\n</document>\n";

    NS_TEST_EXPECT_MSG_EQ (exported_file_content, expected_str, "Must be equal");

    // Remove the generated temporal testing files.
    TestUtils::DeleteFile (m_output_filename);
  }

  void
  DoRun () override
  {
    TestGetStatistics ();
    TestGetDataPacketStatistics ();
    TestGetAreaStatistics ();
    TestSaveToXmlFile ();
  }
};



/******************************************************************************/
/******************************************************************************/

// =============================================================================
//                          GeoTemporalLibraryTestSuite
// =============================================================================

/**
 * Geo-Temporal Library test suite.
 * 
 * \ingroup tests
 * \ingroup geotemporal-library-test
 */
class GeoTemporalLibraryTestSuite : public TestSuite
{
public:

  GeoTemporalLibraryTestSuite () : TestSuite ("geotemporal-library", UNIT)
  {
    AddTestCase (new TimePeriodTest, TestCase::QUICK);
    AddTestCase (new PacketClassTest, TestCase::QUICK);
    AddTestCase (new PacketsCounterTest, TestCase::QUICK);
    AddTestCase (new TransmissionTypeTest, TestCase::QUICK);
    AddTestCase (new DataPacketReceptionStatsTest, TestCase::QUICK);
    AddTestCase (new DataPacketStatisticsTest, TestCase::QUICK);
    AddTestCase (new SimulationStatisticsTest, TestCase::QUICK);
    AddTestCase (new SimulationStatisticsFileTest, TestCase::QUICK);
  }
};

static GeoTemporalLibraryTestSuite g_geotemporal_library_test_suite;


