/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_MATH_UTILS_H
#define UTILS_MATH_UTILS_H

#include <ostream>
#include <string>
#include <vector>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// Prototypes

class Vector2D;


// =============================================================================
//                                 Free functions
// =============================================================================

/**
 * Returns <code>true</code> if the absolute value of the difference between two
 * numbers is less or equal than the specified allowed <code>absolute_error</code>.
 * @param first First number to compare.
 * @param second Second number to compare.
 * @param absolute_error Allowed absolute error.
 */
bool
AlmostEqual (double first, double second, double absolute_error = 0.000001);

/**
 * Calculates the Euclidean distance between two points.
 * @return Distance between point 1 and point 2.
 */
double
CalculateDistance (const Vector2D & point_1, const Vector2D & point_2);

/**
 * Calculates the Euclidean distance between two points.
 * @return Distance between point 1 and point 2.
 */
double
CalculateDistance (double point_1_x, double point_1_y, double point_2_x, double point_2_y);



// =============================================================================
//                                    Vector2D
// =============================================================================

/**
 * Two dimensions coordinate.
 */
class Vector2D
{
public:

  double m_x, m_y;

  Vector2D ();

  Vector2D (const double & x, const double & y);

  Vector2D (const Vector2D & copy);

  double DistanceTo (const Vector2D & b) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void
  Print (std::ostream & os) const;

  inline Vector2D &
          operator+= (const Vector2D & rhs)
  {
    // Check for self-assignment!
    if (this == &rhs) // Same object?
      {
        // Yes, but no worries for this class
      }

    m_x += rhs.m_x;
    m_y += rhs.m_y;
    return *this;
  }

  inline Vector2D &
          operator-= (const Vector2D & rhs)
  {
    // Check for self-assignment!
    if (this == &rhs) // Same object?
      {
        // Yes, but no worries for this class
      }

    m_x -= rhs.m_x;
    m_y -= rhs.m_y;
    return *this;
  }
};

// Vector2D relational operators

inline bool
operator== (const Vector2D & lhs, const Vector2D & rhs)
{
  // Accurate to micro-meters (1 micro-meter (µm) = 0.000 001 meter)
  return AlmostEqual (lhs.m_x, rhs.m_x, 0.000001)
          && AlmostEqual (lhs.m_y, rhs.m_y, 0.000001);
}

inline bool
operator!= (const Vector2D & lhs, const Vector2D & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const Vector2D & lhs, const Vector2D & rhs)
{
  // Accurate to micro-meters (1 micro-meter (µm) = 0.000 001 meter)
  if (!AlmostEqual (lhs.m_x, rhs.m_x, 0.000001)) return lhs.m_x < rhs.m_x;
  return lhs.m_y < rhs.m_y;
}

inline bool
operator> (const Vector2D & lhs, const Vector2D & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const Vector2D & lhs, const Vector2D & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const Vector2D & lhs, const Vector2D & rhs)
{
  return !operator< (lhs, rhs);
}

// Vector2D arithmetic operators

inline const Vector2D
operator+ (const Vector2D & lhs, const Vector2D & rhs)
{
  // Returns const to avoid something like this:
  //      Vector2D a, b, c;
  //      ...
  //      (a + b) = c;
  return Vector2D (lhs) += rhs;
}

inline const Vector2D
operator- (const Vector2D & lhs, const Vector2D & rhs)
{
  // Returns const to avoid something like this:
  //      Vector2D a, b, c;
  //      ...
  //      (a - b) = c;
  return Vector2D (lhs) -= rhs;
}

// Vector2D stream operators

inline std::ostream &
operator<< (std::ostream & os, const Vector2D & obj)
{
  obj.Print (os);
  return os;
}


// =============================================================================
//                                      Area
// =============================================================================

/**
 * Rectangular area delimited by 2 coordinates (x1, y1) and (x2, y2).
 */
class Area
{
protected:

  /**
   * Coordinate 1. This coordinate is initialized with the lesser X and Y values:
   * <code>( min (x1, x2), min (y1, y2) )</code>.
   */
  Vector2D m_coord_1;

  /**
   * Coordinate 2. This coordinate is initialized with the greater X and Y values:
   * <code>( max (x1, x2), max (y1, y2) )</code>.
   */
  Vector2D m_coord_2;

public:

  Area ();

  /**
   * Initializes the area with the two given coordinates. The coordinates are sorted
   * from minimum to maximum, so coordinate 1 is the minimum and coordinate 2 is the
   * maximum.
   *
   * Example 1:
   *
   *        Area ( 5, 5,   0, 0 )
   *
   *    Assigns:
   *
   *        coordinate 1 = 0, 0
   *        coordinate 2 = 5, 5
   *
   * Example 2:
   *
   *        Area ( 5, -5,   0, 0 )
   *
   *    Assigns:
   *
   *        coordinate 1 = 0, -5
   *        coordinate 2 = 5, 0
   */
  Area (const double & x1, const double & y1, const double & x2, const double & y2);

  /**
   * Initializes the area with the two given coordinates. The coordinates are sorted
   * from minimum to maximum, so coordinate 1 is the minimum and coordinate 2 is the
   * maximum.
   *
   * Example 1:
   *
   *        Area ( Vector2D (5, 5),   Vector2D (0, 0) )
   *
   *    Assigns:
   *
   *        coordinate 1 = 0, 0
   *        coordinate 2 = 5, 5
   *
   * Example 2:
   *
   *        Area ( Vector2D (5, -5),   Vector2D (0, 0) )
   *
   *    Assigns:
   *
   *        coordinate 1 = 0, -5
   *        coordinate 2 = 5, 0
   */
  Area (const Vector2D & vector1, const Vector2D & vector2);

  Area (const Area & copy);

private:

  void
  SetUp (const double & x1, const double & y1, const double & x2, const double & y2);

public:

  inline const Vector2D &
  GetCoordinate1 () const
  {
    return m_coord_1;
  }

  inline const double &
  GetX1 () const
  {
    return m_coord_1.m_x;
  }

  inline const double &
  GetY1 () const
  {
    return m_coord_1.m_y;
  }

  inline const Vector2D &
  GetCoordinate2 () const
  {
    return m_coord_2;
  }

  inline const double &
  GetX2 () const
  {
    return m_coord_2.m_x;
  }

  inline const double &
  GetY2 () const
  {
    return m_coord_2.m_y;
  }

  /**
   * Calculates the quantity that expresses the extent of the two-dimensional area in the plane.
   * @return Computed area.
   */
  double CalculateArea () const;

  /**
   * Computes if the given <code>coordinate</code> is inside the area or not.
   * @param point [IN] Point to test.
   * @return <code>true</code> if <code>coordinate</code> is inside, <code>false</code>
   * otherwise.
   */
  bool IsInside (const Vector2D & point) const;

  /**
   * Imports the areas contained in the given text file.
   * @param input_filename Name of the input file.
   * @return A <code>vector</code> with all the areas contained in the file.
   */
  static std::vector<Area>
  ImportAreasFromFile (const std::string & input_filename);

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  virtual std::string ToString () const;

  virtual void Print (std::ostream & os) const;

  friend bool operator== (const Area & lhs, const Area & rhs);
  friend bool operator< (const Area & lhs, const Area & rhs);
};

// Area relational operators

inline bool
operator== (const Area & lhs, const Area & rhs)
{
  return lhs.m_coord_1 == rhs.m_coord_1 && lhs.m_coord_2 == rhs.m_coord_2;
}

inline bool
operator!= (const Area & lhs, const Area & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const Area & lhs, const Area & rhs)
{
  // Area's less-than operator is evaluated on these criteria (in order):
  //   i. Area size (base * height),
  //  ii. Lesser coordinate (coordinate 1).
  // iii. Greater coordinate (coordinate 2).

  //   i. If area sizes are different return comparison between them.
  double lhs_area = lhs.CalculateArea ();
  double rhs_area = rhs.CalculateArea ();

  // Accurate to micro-meters (1 micro-meter (µm) = 0.000 001 meter)
  if (!AlmostEqual (lhs_area, rhs_area, 0.000001))
    return lhs_area < rhs_area;

  //  ii. If lesser coordinates are different then return comparison between them.
  if (lhs.m_coord_1 != rhs.m_coord_1)
    return lhs.m_coord_1 < rhs.m_coord_1;

  // iii. Return greater coordinates comparison.
  return lhs.m_coord_2 < rhs.m_coord_2;
}

inline bool
operator> (const Area & lhs, const Area & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const Area & lhs, const Area & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const Area & lhs, const Area & rhs)
{
  return !operator< (lhs, rhs);
}

// Area stream operators

inline std::ostream &
operator<< (std::ostream & os, const Area & obj)
{
  obj.Print (os);
  return os;
}

}
}

#endif //UTILS_MATH_UTILS_H
