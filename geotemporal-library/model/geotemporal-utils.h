/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_GEOTEMPORAL_UTILS_H
#define UTILS_GEOTEMPORAL_UTILS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "math-utils.h"

#include <ns3/simple-ref-count.h>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                  AreaStatus
// =============================================================================

/**
 * Rectangular area delimited by 2 coordinates (x1, y1) and (x2, y2), and a flag
 * to indicate if the node is located inside or outside the area.
 *
 * \extends Area
 */
class AreaStatus : public Area
{
public:

  bool m_node_inside_area; /// Indicates if a node is inside the area.

  AreaStatus ();

  AreaStatus (const bool inside_area);

  explicit AreaStatus (const Area & area);

  AreaStatus (const Vector2D & vector1, const Vector2D & vector2);

  AreaStatus (const Vector2D & vector1, const Vector2D & vector2,
              const bool inside_area);

  AreaStatus (const double & x1, const double & y1, const double & x2,
              const double & y2);

  AreaStatus (const double & x1, const double & y1, const double & x2,
              const double & y2, const bool inside_area);

  AreaStatus (const AreaStatus & copy);

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const override;

  void Print (std::ostream & os) const override;

};

// AreaStatus relational operators

inline bool
operator== (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return ((Area) lhs) == ((Area) rhs)
          && lhs.m_node_inside_area == rhs.m_node_inside_area;
}

inline bool
operator== (const AreaStatus & lhs, const Area & rhs)
{
  // Note that when comparing an AreaStatus vs an Area we ignore the m_node_inside_area.
  return ((Area) lhs) == rhs;
}

inline bool
operator== (const Area & lhs, const AreaStatus & rhs)
{
  return operator== (rhs, lhs);
}

inline bool
operator!= (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator!= (const AreaStatus & lhs, const Area & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator!= (const Area & lhs, const AreaStatus & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return operator< ((Area) lhs, (Area) rhs);
}

inline bool
operator> (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const AreaStatus & lhs, const AreaStatus & rhs)
{
  return !operator< (lhs, rhs);
}

// AreaStatus stream operators

inline std::ostream &
operator<< (std::ostream & os, const AreaStatus & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                                   TimePeriod
// =============================================================================

/**
 * Represents a period of time. It has start time, end time, and duration.
 */
class TimePeriod
{
private:

  uint32_t m_start_time;
  uint32_t m_end_time;

public:

  TimePeriod ();

  TimePeriod (uint32_t start_time, uint32_t end_time);

  TimePeriod (const TimePeriod & copy);

public:

  /**
   * Returns the start time (in seconds) of the period of time.
   */
  inline uint32_t
  GetStartTime () const
  {
    return m_start_time;
  }

  /**
   * Returns the end time (in seconds) of the period of time.
   */
  inline uint32_t
  GetEndTime () const
  {
    return m_end_time;
  }

  /**
   * Returns the duration (in seconds) of the period of time.
   */
  inline uint32_t
  GetDuration () const
  {
    return m_end_time - m_start_time;
  }

  /**
   * Using the start time and duration it calculates the end time of a time period.
   */
  inline static uint32_t
  CalculateEndTime (uint32_t start_time, uint32_t duration)
  {
    return start_time + duration;
  }

  /**
   * Returns <code>true</code> if the specified time instant is in the period
   * of time. Otherwise returns <code>false</code>.
   * @param time_instant Time instant (in seconds) to validate if it is inside the
   * period of time.
   */
  bool
  IsTimeInstantInTimePeriod (double time_instant) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  virtual std::string ToString () const;

  virtual void Print (std::ostream & os) const;

  friend bool operator== (const TimePeriod & lhs, const TimePeriod & rhs);
  friend bool operator< (const TimePeriod & lhs, const TimePeriod & rhs);

};

// TimePeriod relational operators

inline bool
operator== (const TimePeriod & lhs, const TimePeriod & rhs)
{
  return lhs.m_start_time == rhs.m_start_time && lhs.m_end_time == rhs.m_end_time;
}

inline bool
operator!= (const TimePeriod & lhs, const TimePeriod & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const TimePeriod & lhs, const TimePeriod & rhs)
{
  const uint32_t lhs_duration = lhs.GetDuration ();
  const uint32_t rhs_duration = rhs.GetDuration ();

  if (lhs_duration != rhs_duration)
    return lhs_duration < rhs_duration;

  return lhs.m_start_time < rhs.m_start_time;
}

inline bool
operator> (const TimePeriod & lhs, const TimePeriod & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const TimePeriod & lhs, const TimePeriod & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const TimePeriod & lhs, const TimePeriod & rhs)
{
  return !operator< (lhs, rhs);
}

// TimePeriod stream operators

inline std::ostream &
operator<< (std::ostream & os, const TimePeriod & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                                GeoTemporalArea
// =============================================================================

/**
 * Represents a geographical area with a temporal scope.
 */
class GeoTemporalArea
{
protected:

  TimePeriod m_time_period;
  Area m_area;

public:

  GeoTemporalArea ();

  GeoTemporalArea (const TimePeriod & time_period, const Area & area);

  GeoTemporalArea (const GeoTemporalArea & copy);

  /**
   * Returns the temporal scope of the geo-temporal area.
   */
  inline const TimePeriod &
  GetTimePeriod () const
  {
    return m_time_period;
  }

  /**
   * Returns the geographical area of the geo-temporal area.
   */
  inline const Area &
  GetArea () const
  {
    return m_area;
  }

  /**
   * Returns the duration (in seconds) of the geo-temporal area.
   */
  inline uint32_t
  GetDuration () const
  {
    return m_time_period.GetDuration ();
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  virtual std::string ToString () const;

  virtual void Print (std::ostream & os) const;

  friend bool operator== (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs);
  friend bool operator< (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs);
};

// GeoTemporalArea relational operators

inline bool
operator== (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  return lhs.m_area == rhs.m_area && lhs.m_time_period == rhs.m_time_period;
}

inline bool
operator!= (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  if (lhs.m_time_period != rhs.m_time_period)
    return lhs.m_time_period < rhs.m_time_period;

  return lhs.m_area < rhs.m_area;
}

inline bool
operator> (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const GeoTemporalArea & lhs, const GeoTemporalArea & rhs)
{
  return !operator< (lhs, rhs);
}

// GeoTemporalArea stream operators

inline std::ostream &
operator<< (std::ostream & os, const GeoTemporalArea & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                           DestinationGeoTemporalArea
// =============================================================================

/**
 * Represents the destination geo-temporal area of the specified node.
 *
 * \extends GeoTemporalArea
 */
class DestinationGeoTemporalArea : public GeoTemporalArea
{
private:

  uint32_t m_node_id;

public:

  DestinationGeoTemporalArea ();

  DestinationGeoTemporalArea (uint32_t node_id, const TimePeriod & time_period,
                              const Area & area);

  DestinationGeoTemporalArea (const DestinationGeoTemporalArea & copy);

  /**
   * Returns the identifier of the node that has as destination the current geo-temporal area.
   */
  inline uint32_t
  GetNodeId () const
  {
    return m_node_id;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const override;

  void Print (std::ostream & os) const override;

  friend bool operator== (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs);
  friend bool operator< (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs);
};

// DestinationGeoTemporalArea relational operators

inline bool
operator== (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  return lhs.m_node_id == rhs.m_node_id && ((GeoTemporalArea) lhs) == ((GeoTemporalArea) rhs);
}

inline bool
operator!= (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  if (lhs.m_node_id != rhs.m_node_id)
    return lhs.m_node_id < rhs.m_node_id;

  return operator< ((GeoTemporalArea) lhs, (GeoTemporalArea) rhs);
}

inline bool
operator> (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const DestinationGeoTemporalArea & lhs, const DestinationGeoTemporalArea & rhs)
{
  return !operator< (lhs, rhs);
}

// DestinationGeoTemporalArea stream operators

inline std::ostream &
operator<< (std::ostream & os, const DestinationGeoTemporalArea & obj)
{
  obj.Print (os);
  return os;
}

// =============================================================================
//                     RandomDestinationGeoTemporalAreasLists
// =============================================================================

/**
 * Contains sets of lists of destination geo-temporal areas.
 */
class RandomDestinationGeoTemporalAreasLists : public ns3::SimpleRefCount<RandomDestinationGeoTemporalAreasLists>
{
private:

  uint32_t m_simulation_total_time;
  uint32_t m_lists_sets_number;
  std::vector<Area> m_destination_areas_list;
  std::set<uint32_t> m_list_lengths_in_set;
  std::vector<std::map<uint32_t, std::vector<DestinationGeoTemporalArea> > > m_lists_sets;

public:

  RandomDestinationGeoTemporalAreasLists ();

  RandomDestinationGeoTemporalAreasLists (const std::string & input_filename);

  RandomDestinationGeoTemporalAreasLists (const RandomDestinationGeoTemporalAreasLists & copy);

  /**
   * Returns a vector of destination geo-temporal areas from the desired set and length in a
   * const reference.
   *
   * If the specified set number or list length doesn't exists then it throws an
   * <code>invalid_argument</code> exception.
   *
   * @param set_number Number of the set in which the desired list is located.
   * @param list_length Length of the desired list.
   */
  const std::vector<DestinationGeoTemporalArea> &
  GetDestinationGeoTemporalAreasList (uint32_t set_number, uint32_t list_length) const;

  /**
   * Exports the list of sets to a file.
   * @param filename Name of the output file.
   */
  void ExportToFile (const std::string & filename) const;

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool
  operator== (const RandomDestinationGeoTemporalAreasLists & lhs, const RandomDestinationGeoTemporalAreasLists & rhs);
};

// RandomDestinationGeoTemporalAreasLists relational operators

inline bool
operator== (const RandomDestinationGeoTemporalAreasLists & lhs, const RandomDestinationGeoTemporalAreasLists & rhs)
{
  return lhs.m_simulation_total_time == rhs.m_simulation_total_time
          && lhs.m_lists_sets_number == rhs.m_lists_sets_number
          && lhs.m_destination_areas_list == rhs.m_destination_areas_list
          && lhs.m_list_lengths_in_set == rhs.m_list_lengths_in_set
          && lhs.m_lists_sets == rhs.m_lists_sets;
}

inline bool
operator!= (const RandomDestinationGeoTemporalAreasLists & lhs, const RandomDestinationGeoTemporalAreasLists & rhs)
{
  return !operator== (lhs, rhs);
}

// RandomDestinationGeoTemporalAreasLists stream operators

inline std::ostream &
operator<< (std::ostream & os, const RandomDestinationGeoTemporalAreasLists & obj)
{
  obj.Print (os);
  return os;
}

}
}

#endif //UTILS_GEOTEMPORAL_UTILS_H
