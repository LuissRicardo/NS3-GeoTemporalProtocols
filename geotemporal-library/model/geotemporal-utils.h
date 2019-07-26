/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_GEOTEMPORAL_UTILS_H
#define UTILS_GEOTEMPORAL_UTILS_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "math-utils.h"

#include <ns3/nstime.h>
#include <ns3/simple-ref-count.h>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                   TimePeriod
// =============================================================================

/**
 * \ingroup geotemporal-library
 * 
 * Represents a period of time. It has start time, end time, and duration.
 */
class TimePeriod
{
private:

  ns3::Time m_start_time;
  ns3::Time m_end_time;

public:

  TimePeriod ();

  TimePeriod (const ns3::Time & start_time, const ns3::Time & end_time);

  TimePeriod (const TimePeriod & copy);

public:

  /**
   * Returns the start time of the period of time.
   */
  inline const ns3::Time &
  GetStartTime () const
  {
    return m_start_time;
  }

  /**
   * Returns the end time of the period of time.
   */
  inline const ns3::Time &
  GetEndTime () const
  {
    return m_end_time;
  }

  /**
   * Returns the duration of the period of time.
   */
  inline ns3::Time
  GetDuration () const
  {
    return m_end_time - m_start_time;
  }

  /**
   * Using the start time and duration it calculates the end time of a time period.
   */
  inline static ns3::Time
  CalculateEndTime (const ns3::Time & start_time, const ns3::Time & duration)
  {
    return start_time + duration;
  }

  /**
   * Returns <code>true</code> if the specified time instant occurs during the 
   * time period, otherwise returns <code>false</code>.
   */
  bool
  IsDuringTimePeriod (const ns3::Time & time_instant) const;

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
  const ns3::Time lhs_duration = lhs.GetDuration ();
  const ns3::Time rhs_duration = rhs.GetDuration ();

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
   * Sets the temporal scope of the geo-temporal area.
   */
  inline void
  SetTimePeriod (const TimePeriod & new_time_period)
  {
    m_time_period = new_time_period;
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
   * Sets the geographical area of the geo-temporal area.
   */
  inline void
  SetArea (const Area & new_area)
  {
    m_area = new_area;
  }

  /**
   * Returns the duration of the geo-temporal area.
   */
  inline ns3::Time
  GetDuration () const
  {
    return m_time_period.GetDuration ();
  }

  /**
   * Returns <code>true</code> if the specified time instant occurs during the 
   * time period, otherwise returns <code>false</code>.
   */
  inline bool
  IsDuringTimePeriod (const ns3::Time & time_instant) const
  {
    return m_time_period.IsDuringTimePeriod (time_instant);
  }

  /**
   * Returns <code>true</code> if the given <code>point</code> is inside the
   * area, otherwise returns <code>false</code>.
   */
  inline bool
  IsInsideArea (const Vector2D & point) const
  {
    return m_area.IsInside (point);
  }

  /**
   * Returns <code>true</code> if the given <code>point</code> is inside the
   * area and the specified <code>time_instant</code> occurs during the time
   * period, otherwise returns <code>false</code>.
   * 
   * This is equivalent to calling 
   * <code>geo_temporal_area.IsDuringTimePeriod (time_instant) &&
   * geo_temporal_area.IsInsideArea (point)</code>.
   */
  inline bool
  IsInsideGeoTemporalArea (const Vector2D & point, const ns3::Time & time_instant) const
  {
    return m_area.IsInside (point) && m_time_period.IsDuringTimePeriod (time_instant);
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

  /**
   * Identifier of the node that has as destination the current geo-temporal
   * area.
   */
  uint32_t m_node_id;

  /**
   * The time when the packet to send to the destination geo-temporal area must
   * be created.
   */
  ns3::Time m_creation_time;

public:

  DestinationGeoTemporalArea ();

  /**
   * Initializes the object with the geographical area and temporal scope linked
   * with the given node ID.
   * 
   * By default, the packet's creation time is set to the initial time of the 
   * given time period. Use <code>DestinationGeoTemporalArea::SetCreationTime 
   * (const ns3::Time &)</code> setter to change this value.
   * 
   * @param node_id Identifier of the node that has as destination the specified
   * geo-temporal area.
   * @param time_period Temporal scope of the destination geo-temporal area.
   * @param area The geographical area of the destination geo-temporal area.
   */
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
   * Sets the identifier of the node that has as destination the current geo-temporal area.
   */
  inline void
  SetNodeId (uint32_t node_id)
  {
    m_node_id = node_id;
  }

  /**
   * Returns the time when the packet to send to the destination geo-temporal 
   * area must be created.
   */
  inline const ns3::Time &
  GetCreationTime () const
  {
    return m_creation_time;
  }

  /**
   * Sets the time when the packet to send to the destination geo-temporal 
   * area must be created.
   */
  inline void
  SetCreationTime (const ns3::Time & creation_time)
  {
    m_creation_time = creation_time;
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
  return lhs.m_node_id == rhs.m_node_id
          && lhs.m_creation_time == rhs.m_creation_time
          && ((GeoTemporalArea) lhs) == ((GeoTemporalArea) rhs);
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

  if (operator!= ((GeoTemporalArea) lhs, (GeoTemporalArea) rhs))
    return operator< ((GeoTemporalArea) lhs, (GeoTemporalArea) rhs);

  return lhs.m_creation_time < rhs.m_creation_time;
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
   * @param set_number Number of the set in which the desired list is located. This is 
   * the set index + 1.
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
