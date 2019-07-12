/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "geotemporal-utils.h"

#include <fstream>
#include <iostream>

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                   TimePeriod
// =============================================================================

TimePeriod::TimePeriod ()
: m_start_time (ns3::Seconds (0)), m_end_time (ns3::Seconds (0)) { }

TimePeriod::TimePeriod (const ns3::Time & start_time, const ns3::Time & end_time)
: TimePeriod ()
{
  if (end_time < start_time)
    throw std::invalid_argument ("Invalid end time: it must be greater or equal "
                                 "than the start time.");

  m_start_time = start_time;
  m_end_time = end_time;
}

TimePeriod::TimePeriod (const TimePeriod & copy)
: m_start_time (copy.m_start_time), m_end_time (copy.m_end_time) { }

bool
TimePeriod::IsDuringTimePeriod (const ns3::Time & time_instant) const
{
  return (m_start_time <= time_instant && time_instant <= m_end_time);
}

std::string
TimePeriod::ToString () const
{
  char buffer[25];

  std::sprintf (buffer, "%04.2f", m_start_time.GetSeconds ());
  std::string ret_string = "Period of time starts at " + std::string (buffer);

  std::sprintf (buffer, "%04.2f", m_end_time.GetSeconds ());
  ret_string += " sec. and ends at " + std::string (buffer) + " sec. ";

  std::sprintf (buffer, "%04.2f", GetDuration ().GetSeconds ());
  ret_string += "(lasts " + std::string (buffer) + " seconds)";

  return ret_string;
}

void
TimePeriod::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                                GeoTemporalArea
// =============================================================================

GeoTemporalArea::GeoTemporalArea ()
: m_time_period (), m_area () { }

GeoTemporalArea::GeoTemporalArea (const TimePeriod & time_period, const Area & area)
: m_time_period (time_period), m_area (area) { }

GeoTemporalArea::GeoTemporalArea (const GeoTemporalArea & copy)
: m_time_period (copy.m_time_period), m_area (copy.m_area) { }

std::string
GeoTemporalArea::ToString () const
{
  char buffer[25];

  std::sprintf (buffer, "%04.2f", m_time_period.GetStartTime ().GetSeconds ());
  std::string ret_string = "Geo-temporal area " + m_area.ToString () + " active from "
          + std::string (buffer) + " to ";

  std::sprintf (buffer, "%04.2f", m_time_period.GetEndTime ().GetSeconds ());
  ret_string += std::string (buffer) + " seconds.";

  return ret_string;
}

void
GeoTemporalArea::Print (std::ostream & os) const
{
  os << ToString ();
}

// =============================================================================
//                           DestinationGeoTemporalArea
// =============================================================================

DestinationGeoTemporalArea::DestinationGeoTemporalArea ()
: GeoTemporalArea (), m_node_id (0), m_creation_time (ns3::Seconds (0)) { }

DestinationGeoTemporalArea::DestinationGeoTemporalArea (uint32_t node_id,
                                                        const TimePeriod & time_period,
                                                        const Area & area)
: GeoTemporalArea (time_period, area), m_node_id (node_id),
m_creation_time (time_period.GetStartTime ()) { }

DestinationGeoTemporalArea::DestinationGeoTemporalArea (const DestinationGeoTemporalArea & copy)
: GeoTemporalArea (copy), m_node_id (copy.m_node_id),
m_creation_time (copy.m_creation_time) { }

std::string
DestinationGeoTemporalArea::ToString () const
{
  char buffer[25];

  std::sprintf (buffer, "%u", m_node_id);
  std::string ret_string = "Node with ID " + std::string (buffer)
          + " has destination geo-temporal area " + m_area.ToString ();

  std::sprintf (buffer, "%04.2f", m_time_period.GetStartTime ().GetSeconds ());
  ret_string += " active from " + std::string (buffer) + " to ";

  std::sprintf (buffer, "%04.2f", m_time_period.GetEndTime ().GetSeconds ());
  ret_string += std::string (buffer) + " seconds created at ";

  std::sprintf (buffer, "%04.2f", m_creation_time.GetSeconds ());
  ret_string += std::string (buffer);

  return ret_string;
}

void
DestinationGeoTemporalArea::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                     RandomDestinationGeoTemporalAreasLists
// =============================================================================

RandomDestinationGeoTemporalAreasLists::RandomDestinationGeoTemporalAreasLists ()
: m_simulation_total_time (), m_lists_sets_number (), m_destination_areas_list (),
m_list_lengths_in_set (), m_lists_sets () { }

RandomDestinationGeoTemporalAreasLists::RandomDestinationGeoTemporalAreasLists (const std::string & input_filename)
: RandomDestinationGeoTemporalAreasLists ()
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (input_filename);
  std::ifstream input_file (filename_trimmed, std::ios::in);
  std::string text_line;
  std::vector<std::string> tokens;

  if (!input_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Importing lists of random destination geo-temporal areas file \"" << filename_trimmed << "\"...";

  // Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Expected 3 integers separated by a comma.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ())
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  tokens = LibraryUtils::Split (text_line, ',');

  if (tokens.size () != 3u)
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  for (uint32_t i = 0; i < tokens.size (); ++i)
    LibraryUtils::Trim (tokens.at (i));

  const uint32_t areas_number = (uint32_t) std::stoi (tokens.at (0u));
  m_simulation_total_time = (uint32_t) std::stoi (tokens.at (1u));
  m_lists_sets_number = (uint32_t) std::stoi (tokens.at (2u));

  // Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || !text_line.empty ())
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Expected comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Expected lengths of lists in each set
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ())
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  tokens = LibraryUtils::Split (text_line, ',');

  if (tokens.size () < 1u)
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  for (uint32_t i = 0; i < tokens.size (); ++i)
    {
      LibraryUtils::Trim (tokens.at (i));
      m_list_lengths_in_set.insert ((uint32_t) std::stoi (tokens.at (i)));
    }

  // Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || !text_line.empty ())
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Expected comment.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // List of areas
  for (uint32_t expected_area_id = 0; expected_area_id < areas_number; ++expected_area_id)
    {
      // Expected 5 numbers: 1 int and 4 doubles
      if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ())
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 5u)
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (uint32_t i = 0; i < tokens.size (); ++i)
        LibraryUtils::Trim (tokens.at (i));

      if (expected_area_id != (uint32_t) std::stoi (tokens.at (0u)))
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format. All"
                                    " area IDs must be consecutive.");
        }

      m_destination_areas_list.emplace_back (std::stod (tokens.at (1u)),
                                             std::stod (tokens.at (2u)),
                                             std::stod (tokens.at (3u)),
                                             std::stod (tokens.at (4u)));
    }

  // Expected emtpy line.
  if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || !text_line.empty ())
    {
      input_file.close ();
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Sets of lists
  std::vector<uint32_t> int_tokens;
  DestinationGeoTemporalArea destination_gta;
  std::vector<DestinationGeoTemporalArea> destinations_vector;
  std::map<uint32_t, std::vector<DestinationGeoTemporalArea> > lists_set;

  for (uint32_t set_index = 0; set_index < m_lists_sets_number; ++set_index)
    {
      // Expected comment.
      if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
          || text_line.at (0) != '#')
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      // Expected comment.
      if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ()
          || text_line.at (0) != '#')
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      // One list per each list length
      for (std::set<uint32_t>::const_iterator list_length_it = m_list_lengths_in_set.begin ();
              list_length_it != m_list_lengths_in_set.end (); ++list_length_it)
        {
          // Expected at least 2 integers
          if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || text_line.empty ())
            {
              input_file.close ();
              std::cout << " Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }

          tokens = LibraryUtils::Split (text_line, ',');

          if (tokens.size () < 2u || (tokens.size () - 2) % 4 != 0)
            {
              input_file.close ();
              std::cout << " Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }

          int_tokens.clear ();

          for (uint32_t i = 0; i < tokens.size (); ++i)
            {
              LibraryUtils::Trim (tokens.at (i));
              int_tokens.push_back ((uint32_t) std::stoi (tokens.at (i)));
            }

          if (set_index + 1u != int_tokens.at (0u))
            {
              input_file.close ();
              std::cout << " Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format."
                                        "Invalid set number.");
            }

          if (*list_length_it != int_tokens.at (1u))
            {
              input_file.close ();
              std::cout << " Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format."
                                        " Invalid list length.");
            }

          // Discard validated information we don't need anymore
          destinations_vector.clear ();
          int_tokens.erase (int_tokens.begin (), int_tokens.begin () + 2);

          for (uint32_t list_index = 0u; list_index < *list_length_it; ++list_index)
            {
              destination_gta = DestinationGeoTemporalArea (/*Node ID*/ int_tokens.at (0u),
                                                            /*Time period*/ TimePeriod (ns3::Seconds (int_tokens.at (2u)),
                                                                                        ns3::Seconds (int_tokens.at (3u))),
                                                            /*Area*/ m_destination_areas_list.at (int_tokens.at (1u)));
              destination_gta.SetCreationTime (ns3::Seconds (int_tokens.at (4u)));

              destinations_vector.push_back (destination_gta);

              int_tokens.erase (int_tokens.begin (), int_tokens.begin () + 5);
            }

          if (destinations_vector.size () != *list_length_it)
            {
              input_file.close ();
              std::cout << " Error!\n";
              throw std::runtime_error ("Corrupt file. The file does not match the correct format."
                                        " Invalid list length.");
            }

          // Add single list to set
          lists_set.insert (std::make_pair (*list_length_it, destinations_vector));
        }

      // Add set of lists to final list.
      m_lists_sets.push_back (lists_set);
      lists_set.clear ();

      // Expected empty line.
      if (!LibraryUtils::GetInputStreamNextLine (input_file, text_line) || !text_line.empty ())
        {
          input_file.close ();
          std::cout << " Error!\n";
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }
    }

  input_file.close ();

  if (m_lists_sets.size () != m_lists_sets_number)
    {
      std::cout << " Error!\n";
      throw std::runtime_error ("Corrupt file. The file does not match the correct format. "
                                "Invalid number of sets.");
    }

  std::cout << " Done.\n";
}

RandomDestinationGeoTemporalAreasLists::RandomDestinationGeoTemporalAreasLists (const RandomDestinationGeoTemporalAreasLists & copy)
: m_simulation_total_time (copy.m_simulation_total_time),
m_lists_sets_number (copy.m_lists_sets_number),
m_destination_areas_list (copy.m_destination_areas_list),
m_list_lengths_in_set (copy.m_list_lengths_in_set),
m_lists_sets (copy.m_lists_sets) { }

const std::vector<DestinationGeoTemporalArea> &
RandomDestinationGeoTemporalAreasLists::GetDestinationGeoTemporalAreasList (uint32_t set_number, uint32_t list_length) const
{
  if (set_number < 1 && set_number > m_lists_sets.size ())
    throw std::invalid_argument ("Invalid set number: it must be a positive integer between 1 and the number of sets, "
                                 "including both limits.");

  if (m_list_lengths_in_set.count (list_length) == 0u)
    throw std::invalid_argument ("Invalid list length: there aren't lists with the given length.");

  return m_lists_sets.at (set_number - 1u).at (list_length);
}

void
RandomDestinationGeoTemporalAreasLists::ExportToFile (const std::string & filename) const
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (filename);
  const std::string end_line = "\n"; // LibraryUtils::SYSTEM_NEW_LINE_STRING ();
  char buffer[25];

  if (filename_trimmed.empty ())
    throw std::runtime_error ("Invalid filename: the filename cannot be empty.");

  std::ofstream output_file (filename_trimmed, std::ios::out);

  if (!output_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  std::cout << "Exporting lists of random destination geo-temporal areas to file \"" << filename_trimmed << "\"...";

  // 1 - Setup data
  output_file << "# Number of areas, Simulation total time, Number of lists sets" << end_line;
  output_file << m_destination_areas_list.size () << ", " << m_simulation_total_time
          << ", " << m_lists_sets_number << end_line << end_line;

  output_file << "# Lengths of lists in set" << end_line;

  {
    uint32_t i;
    std::set<uint32_t>::const_iterator length_it;

    for (i = 0u, length_it = m_list_lengths_in_set.begin ();
            length_it != m_list_lengths_in_set.end (); ++i, ++length_it)
      {
        output_file << *length_it;

        if (i < m_list_lengths_in_set.size () - 1u)
          output_file << ", ";
      }
  }

  output_file << end_line << end_line;

  // 2 - List of areas
  std::map<Area, uint32_t> areas_ids_mapping;
  uint32_t area_id = 0u;

  output_file << "# Area ID, Area X1, Area Y1, Area X2, Area Y2" << end_line;

  for (std::vector<Area>::const_iterator area_it = m_destination_areas_list.begin ();
          area_it != m_destination_areas_list.end (); ++area_it)
    {
      areas_ids_mapping.insert (std::make_pair (*area_it, area_id));
      output_file << area_id << ", ";
      ++area_id;

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate1 ().m_x);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate1 ().m_y);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate2 ().m_x);
      output_file << buffer << ", ";

      std::sprintf (buffer, "%.6f", area_it->GetCoordinate2 ().m_y);
      output_file << buffer << end_line;
    }

  output_file << end_line;

  // 3 - Sets of lists
  uint32_t set_number;
  for (uint32_t set_index = 0u; set_index < m_lists_sets_number; ++set_index)
    {
      set_number = set_index + 1u;

      output_file << "# -- Set " << set_number << " --" << end_line;
      output_file << "# Set Number, List length[, Source node ID, Area ID, Start time, End time, Creation time]*" << end_line;

      for (std::set<uint32_t>::const_iterator list_length_it = m_list_lengths_in_set.begin ();
              list_length_it != m_list_lengths_in_set.end (); ++list_length_it)
        {
          output_file << set_number << ", " << *list_length_it;

          if (*list_length_it < 1u)
            {
              output_file << end_line;
              continue;
            }

          const std::vector<DestinationGeoTemporalArea> & destination_temporal_areas
                  = GetDestinationGeoTemporalAreasList (set_number, *list_length_it);

          for (std::vector<DestinationGeoTemporalArea>::const_iterator temporal_area_it = destination_temporal_areas
                  .begin (); temporal_area_it != destination_temporal_areas.end (); ++temporal_area_it)
            {
              output_file << ", " << temporal_area_it->GetNodeId ()
                      << ", " << areas_ids_mapping.at (temporal_area_it->GetArea ())
                      << ", " << ((uint32_t) temporal_area_it->GetTimePeriod ().GetStartTime ().GetSeconds ())
                      << ", " << ((uint32_t) temporal_area_it->GetTimePeriod ().GetEndTime ().GetSeconds ())
                      << ", " << ((uint32_t) temporal_area_it->GetCreationTime ().GetSeconds ());
            }

          output_file << end_line;
        }

      output_file << end_line;
    }

  output_file.close ();
  std::cout << " Done.\n";
}

std::string
RandomDestinationGeoTemporalAreasLists::ToString () const
{
  char buffer[25];

  std::sprintf (buffer, "%u", m_lists_sets_number);

  return std::string (buffer) + " sets of lists of destination geo-temporal areas.";
}

void
RandomDestinationGeoTemporalAreasLists::Print (std::ostream & os) const
{
  os << ToString ();
}

}
}
