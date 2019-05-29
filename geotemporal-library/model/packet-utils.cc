/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "packet-utils.h"

#include <cstdio>
#include <vector>

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                 Free functions
// =============================================================================

std::string
ToString (const ns3::Ipv4Address& ip)
{
  uint8_t ip_bytes[4];
  ip.Serialize (ip_bytes);

  char buffer[5];
  std::string str;

  for (uint8_t i = 0u; i < 4u; ++i)
    {
      std::sprintf (buffer, "%u", ip_bytes[i]);
      str += std::string (buffer);

      if (i < 3u) str += ".";
    }

  return str;
}


// -----------------------------------
// Bit operations
// -----------------------------------

void
SetBitFlag (uint8_t & bit_flags, const uint8_t bit_position)
{
  bit_flags |= (1 << bit_position);
}

void
ClearBitFlag (uint8_t & bit_flags, const uint8_t bit_position)
{
  bit_flags &= ~(1 << bit_position);
}

void
ToggleBitFlag (uint8_t & bit_flags, const uint8_t bit_position)
{
  bit_flags ^= (1 << bit_position);
}

bool
CheckBitFlag (const uint8_t & bit_flags, const uint8_t bit_position)
{
  return bit_flags & (1 << bit_position);
}


// -----------------------------------
// Floating point numbers encoding & decoding
// -----------------------------------

void
EncodeDoubleToIntegers (const double & to_encode, uint32_t & integer_part,
                        uint32_t & fractional_part, uint8_t & negative_sign_bit_flags,
                        uint8_t negative_sign_bit_flag_position)
{
  double value;

  if (to_encode >= 0.0)
    {
      value = to_encode;
      ClearBitFlag (negative_sign_bit_flags, negative_sign_bit_flag_position);
    }
  else
    {
      value = -to_encode; // Make positive
      SetBitFlag (negative_sign_bit_flags, negative_sign_bit_flag_position);
    }

  integer_part = static_cast<uint32_t> (value);
  fractional_part = (value - integer_part) * LibraryUtils::DOUBLE_ENCODING_FACTOR;
}

double
DecodeDoubleFromIntegers (uint32_t integer_part, uint32_t fractional_part,
                          uint8_t negative_sign_bit_flag,
                          uint8_t negative_sign_bit_flag_position)
{
  double value = (double) integer_part;
  value += (double) fractional_part / (double) LibraryUtils::DOUBLE_ENCODING_FACTOR;

  if (CheckBitFlag (negative_sign_bit_flag, negative_sign_bit_flag_position))
    value = -value;

  return value;
}

void
EncodeFloatToInteger (const float & to_encode, uint32_t & encoded,
                      uint8_t & negative_sign_bit_flag,
                      uint8_t negative_sign_bit_flag_position)
{
  float value;

  if (to_encode >= 0.0)
    {
      value = to_encode;
      ClearBitFlag (negative_sign_bit_flag, negative_sign_bit_flag_position);
    }
  else
    {
      value = -to_encode; // Make positive
      SetBitFlag (negative_sign_bit_flag, negative_sign_bit_flag_position);
    }

  encoded = static_cast<uint32_t> (value * LibraryUtils::FLOAT_ENCODING_FACTOR);
}

float
DecodeFloatFromInteger (uint32_t encoded, uint8_t negative_sign_bit_flag,
                        uint8_t negative_sign_bit_flag_position)
{
  float value = (float) encoded / (float) LibraryUtils::FLOAT_ENCODING_FACTOR;

  if (CheckBitFlag (negative_sign_bit_flag, negative_sign_bit_flag_position))
    value = -value;

  return value;
}



// =============================================================================
//                                DataIdentifier
// =============================================================================

DataIdentifier::DataIdentifier () : m_source_ip (), m_source_id (0) { }

DataIdentifier::DataIdentifier (const ns3::Ipv4Address & source_ip,
                                const uint16_t source_id) :
m_source_ip (source_ip), m_source_id (source_id) { }

DataIdentifier::DataIdentifier (const std::string& data_id) : DataIdentifier ()
{
  std::vector<std::string> tokens = Split (Trim_Copy (data_id), ':');

  if (tokens.size () != 2u) throw std::runtime_error ("Invalid input string.");

  m_source_ip = ns3::Ipv4Address (tokens.at (0u).c_str ());
  m_source_id = (uint16_t) std::stoi (tokens.at (1u));
}

DataIdentifier::DataIdentifier (const DataIdentifier& copy) :
m_source_ip (copy.m_source_ip), m_source_id (copy.m_source_id) { }

std::string
DataIdentifier::ToString () const
{
  char buffer[10];
  std::sprintf (buffer, "%u", m_source_id);

  return LibraryUtils::ToString (m_source_ip) + ":" + std::string (buffer);
}

void
DataIdentifier::Print (std::ostream & os) const
{
  os << ToString ();
}

}
}

