/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "math-utils.h"

#include <cmath>
#include <cstdio>
#include <fstream>

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

bool
AlmostEqual (double first, double second, double absolute_error)
{
  if (first == second) return true;

  return std::abs (first - second) <= absolute_error;
}

double
CalculateDistance (const Vector2D & point_1, const Vector2D & point_2)
{
  return CalculateDistance (point_1.m_x, point_1.m_y, point_2.m_x, point_2.m_y);
}

double
CalculateDistance (double point_1_x, double point_1_y, double point_2_x, double point_2_y)
{
  const double dx = point_1_x - point_2_x;
  const double dy = point_1_y - point_2_y;
  return std::sqrt (dx * dx + dy * dy);
}


// =============================================================================
//                                    Vector2D
// =============================================================================

Vector2D::Vector2D ()
: m_x (0.0), m_y (0.0) { }

Vector2D::Vector2D (const double & x, const double & y)
: m_x (x), m_y (y) { }

Vector2D::Vector2D (const Vector2D & copy)
: m_x (copy.m_x), m_y (copy.m_y) { }

double
Vector2D::DistanceTo (const Vector2D & b) const
{
  return CalculateDistance (*this, b);
}

std::string
Vector2D::ToString () const
{
  char buffer[25];
  std::sprintf (buffer, "%.2f", m_x);
  std::string to_string = "(" + std::string (buffer) + ", ";

  std::sprintf (buffer, "%.2f", m_y);
  to_string += std::string (buffer) + ")";

  return to_string;
}

void
Vector2D::Print (std::ostream & os) const
{
  os << ToString ();
}


// =============================================================================
//                                      Area
// =============================================================================

Area::Area ()
: m_coord_1 (), m_coord_2 () { }

Area::Area (const double & x1, const double & y1, const double & x2, const double & y2)
: Area ()
{
  SetUp (x1, y1, x2, y2);
}

Area::Area (const Vector2D & vector1, const Vector2D & vector2)
: Area ()
{
  SetUp (vector1.m_x, vector1.m_y, vector2.m_x, vector2.m_y);
}

Area::Area (const Area & copy)
: m_coord_1 (copy.m_coord_1), m_coord_2 (copy.m_coord_2) { }

void
Area::SetUp (const double & x1, const double & y1, const double & x2, const double & y2)
{
  if (x1 <= x2)
    {
      m_coord_1.m_x = x1;
      m_coord_2.m_x = x2;
    }
  else
    {
      m_coord_1.m_x = x2;
      m_coord_2.m_x = x1;
    }

  if (y1 <= y2)
    {
      m_coord_1.m_y = y1;
      m_coord_2.m_y = y2;
    }
  else
    {
      m_coord_1.m_y = y2;
      m_coord_2.m_y = y1;
    }
}

double
Area::CalculateArea () const
{
  // Base * height
  return (m_coord_2.m_x - m_coord_1.m_x) * (m_coord_2.m_y - m_coord_1.m_y);
}

bool
Area::IsInside (const Vector2D & point) const
{
  return m_coord_1.m_x <= point.m_x && point.m_x <= m_coord_2.m_x
          && m_coord_1.m_y <= point.m_y && point.m_y <= m_coord_2.m_y;
}

std::vector<Area>
Area::ImportAreasFromFile (const std::string & input_filename)
{
  const std::string filename_trimmed = LibraryUtils::Trim_Copy (input_filename);
  std::ifstream areas_file (filename_trimmed, std::ios::in);
  std::string text_line;

  if (!areas_file.is_open ())
    throw std::runtime_error ("Unable to open file \"" + filename_trimmed + "\".");

  // First part: Expected a comment.
  if (!LibraryUtils::GetInputStreamNextLine (areas_file, text_line) || text_line.empty ()
      || text_line.at (0) != '#')
    {
      areas_file.close ();
      throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
    }

  // Second part: expected list of Areas.
  std::vector<Area> areas_list;
  Area read_area;
  std::vector<std::string> tokens;
  uint32_t area_counter = 0, area_id;

  while (LibraryUtils::GetInputStreamNextLine (areas_file, text_line))
    {
      tokens = LibraryUtils::Split (text_line, ',');

      if (tokens.size () != 5)
        {
          areas_file.close ();
          throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
        }

      for (std::vector<std::string>::iterator it = tokens.begin (); it != tokens.end (); ++it)
        {
          LibraryUtils::Trim (*it);

          if (it->empty ())
            {
              areas_file.close ();
              throw std::runtime_error ("Corrupt file. The file does not match the correct format.");
            }
        }

      area_id = (uint32_t) std::stoi (tokens[0]);

      if (area_id != area_counter)
        {
          areas_file.close ();
          throw std::runtime_error ("Corrupt file. The file does not match the correct format. Area identifiers "
                                    "must be sequential.");
        }

      read_area = Area (std::stod (tokens[1]), std::stod (tokens[2]),
                        std::stod (tokens[3]), std::stod (tokens[4]));
      areas_list.push_back (read_area);
      ++area_counter;
    }

  areas_file.close ();

  return areas_list;
}

std::string
Area::ToString () const
{
  return "{" + m_coord_1.ToString () + ", " + m_coord_2.ToString () + "}";
}

void
Area::Print (std::ostream & os) const
{
  os << ToString ();
}

}
}
