/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_PACKET_UTILS_H
#define UTILS_PACKET_UTILS_H

#include <ns3/ipv4-address.h>

#include <ostream>
#include <string>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

// =============================================================================
//                                  Constants
// =============================================================================

/**
 * Factor used in encoding/decoding from double to ints, and viceversa: 
 * 1,000,000,000.
 */
static const int DOUBLE_ENCODING_FACTOR = 1000000000;

/**
 * Factor used in encoding/decoding from float to ints, and viceversa: 10,000.
 */
static const int FLOAT_ENCODING_FACTOR = 10000;



// =============================================================================
//                                 Free functions
// =============================================================================

/**
 * Returns the string representation of the given IPv4 address.
 */
std::string
ToString (const ns3::Ipv4Address & ip);


// -----------------------------------
// Bit operations
// -----------------------------------

/**
 * Sets the specified bit.
 * 
 * @param bit_flags Data that contains the bit to set.
 * @param bit_position Zero-based position of the bit to set.
 */
void
SetBitFlag (uint8_t & bit_flags, const uint8_t bit_position);

/**
 * Clears the specified bit.
 * 
 * @param bit_flags Data that contains the bit to clear.
 * @param bit_position Zero-based position of the bit to clear.
 */
void
ClearBitFlag (uint8_t & bit_flags, const uint8_t bit_position);

/**
 * Toggles/flips the specified bit.
 * 
 * @param bit_flags Data that contains the bit to toggle/flip.
 * @param bit_position Zero-based position of the bit to toggle/flip.
 */
void
ToggleBitFlag (uint8_t & bit_flags, const uint8_t bit_position);

/**
 * Returns <code>true</code> if the specified bit is set, otherwise returns
 * <code>false</code>.
 * 
 * @param bit_flags Data that contains the bit to check.
 * @param bit_position Zero-based position of the bit to check.
 */
bool
CheckBitFlag (const uint8_t & bit_flags, const uint8_t bit_position);


// -----------------------------------
// Floating point numbers encoding & decoding
// -----------------------------------

/**
 * Encodes the number in <code>to_encode</code> into two <code>uint32_t</code> 
 * integers, one integer for the integer part of the double value and other
 * integer for the fractional part.
 * 
 * The sign of the number is encoded in a single bit in the 
 * <code>negative_sign_bit_flag</code> reference parameter, in the specified
 * position (from 0 to 7): <code>true</code> indicates that the number is 
 * negative, and <code>false</code> indicates that the number is positive.
 * 
 * <h3>Example</h3>
 * <code>
 * uint32_t integer_part = 0, fractional_part = 0;
 * 
 * uint8_t negative_sign_bit_flags = 0;
 * 
 * EncodeDoubleToIntegers (789.00456, integer_part, fractional_part, 
 *                         negative_sign_bit_flags, 3);
 * </code>
 * 
 * Now the values of the variables are:
 * 
 * - <code>integer_part = 789</code>
 * 
 * - <code>fractional_part = 4559999</code>
 * 
 * - <code>negative_sign_bit_flags = 0b00001000</code>
 * 
 * @param to_encode [IN] Floating point number to encode.
 * @param integer_part [OUT] Integer part of the floating point number.
 * @param fractional_part [OUT] Fractional part of the floating point number.
 * @param negative_sign_bit_flags [OUT] Sign of the floating point number
 * encoded in a single bit.
 * @param negative_sign_bit_flag_position [IN] Position (from 0 to 7) of the bit
 * where the sign is stored.
 */
void
EncodeDoubleToIntegers (const double & to_encode, uint32_t & integer_part,
                        uint32_t & fractional_part, uint8_t & negative_sign_bit_flags,
                        uint8_t negative_sign_bit_flag_position);

/**
 * Decodes a floating point number (<code>double</code>) from two 
 * <code>uint32_t</code> integers.
 * 
 * The sign of the number is decoded from a single bit in the
 * <code>negative_sign_bit_flag</code> reference parameter, in the specified
 * position (from 0 to 7): <code>true</code> indicates that the number is 
 * negative, and <code>false</code> indicates that the number is positive.
 * 
 * <h3>Example</h3>
 * <code>
 * double decoded = DecodeDoubleFromIntegers (789, 4559999, 0b00001000, 3);
 * </code>
 * 
 * Now the value of <code>decoded</code> is <code>-789.004560</code>.
 * 
 * @param integer_part [IN] Integer part of the floating point number.
 * @param fractional_part [IN] Fractional part of the floating point number.
 * @param negative_sign_bit_flag [IN] Sign of the floating point number
 * encoded in a single bit.
 * @param negative_sign_bit_flag_position [IN] Position (from 0 to 7) of the bit
 * where the sign is stored.
 * @return The decoded floating point number.
 */
double
DecodeDoubleFromIntegers (uint32_t integer_part, uint32_t fractional_part,
                          uint8_t negative_sign_bit_flag,
                          uint8_t negative_sign_bit_flag_position);

/**
 * Encodes the number in <code>to_encode</code> into an <code>uint32_t</code> 
 * integer.
 * 
 * The sign of the number is encoded in a single bit in the 
 * <code>negative_sign_bit_flag</code> reference parameter, in the specified
 * position (from 0 to 7): <code>true</code> indicates that the number is 
 * negative, and <code>false</code> indicates that the number is positive.
 * 
 * <h3>Example</h3>
 * <code>
 * uint32_t encoded = 0;
 * 
 * uint8_t negative_sign_bit_flags = 0;
 * 
 * EncodeFloatToInteger (-789.00456, encoded, negative_sign_bit_flags, 2);
 * </code>
 * 
 * Now the values of the variables are:
 * 
 * - <code>encoded = 7890046</code>
 * 
 * - <code>negative_sign_bit_flags = 0b00000100</code>
 * 
 * @param to_encode [IN] Floating point number to encode.
 * @param encoded [OUT] Encoded floating point number.
 * @param negative_sign_bit_flags [OUT] Sign of the floating point number
 * encoded in a single bit.
 * @param negative_sign_bit_flag_position [IN] Position (from 0 to 7) of the bit
 * where the sign is stored.
 */
void
EncodeFloatToInteger (const float & to_encode, uint32_t & encoded,
                      uint8_t & negative_sign_bit_flag,
                      uint8_t negative_sign_bit_flag_position);

/**
 * Decodes a floating point number (<code>flat</code>) from an 
 * <code>uint32_t</code> integer.
 * 
 * The sign of the number is decoded from a single bit in the
 * <code>negative_sign_bit_flag</code> reference parameter, in the specified
 * position (from 0 to 7): <code>true</code> indicates that the number is 
 * negative, and <code>false</code> indicates that the number is positive.
 * 
 * <h3>Example</h3>
 * <code>
 * double decoded = DecodeFloatFromInteger (7890046, 0b00000100, 2);
 * </code>
 * 
 * Now the value of <code>decoded</code> is <code>-789.004578</code>.
 * 
 * @param encoded [IN] Encoded floating point number.
 * @param negative_sign_bit_flag [IN] Sign of the floating point number
 * encoded in a single bit.
 * @param negative_sign_bit_flag_position [IN] Position (from 0 to 7) of the bit
 * where the sign is stored.
 * @return The decoded floating point number.
 */
float
DecodeFloatFromInteger (uint32_t encoded, uint8_t negative_sign_bit_flag,
                        uint8_t negative_sign_bit_flag_position);




// =============================================================================
//                                DataIdentifier
// =============================================================================

class DataIdentifier
{
private:

  /**
   * IPv4 address of the source node of the DATA packet.
   */
  ns3::Ipv4Address m_source_ip;

  /**
   * Sequence number of the transmitted packets in the source node.
   */
  uint16_t m_source_id;

public:

  DataIdentifier ();

  DataIdentifier (const ns3::Ipv4Address & source_ip, const uint16_t source_id);

  /**
   * Constructs a <code>DataIdentifier</code> object by parsing the input string.
   * 
   * The input string is in format: <pre>xxx.xxx.xxx.xxx:yyy</pre>
   * 
   * Where:
   * 
   * - <code>xxx.xxx.xxx.xxx</code> is a valid IP v4 address, and
   * 
   * - <code>yyy</code> is a 16-bit unsigned integer (from 0 up to 65,535).
   * 
   * @param data_id The string containing the data identifier as described above.
   */
  DataIdentifier (const std::string & data_id);

  DataIdentifier (const DataIdentifier & copy);

  /**
   * Returns the IPv4 address of the source node of the DATA packet.
   */
  inline const ns3::Ipv4Address &
  GetSourceIp () const
  {
    return m_source_ip;
  }

  /**
   * Returns the sequence number of the transmitted packets in the source node.
   */
  inline uint16_t
  GetSourceId () const
  {
    return m_source_id;
  }

  /**
   * Returns a <code>string</code> object containing the representation of this
   * instance as a sequence of characters.
   */
  std::string ToString () const;

  void Print (std::ostream & os) const;

  friend bool operator== (const DataIdentifier & lhs, const DataIdentifier & rhs);
  friend bool operator< (const DataIdentifier & lhs, const DataIdentifier & rhs);
};

// DataIdentifier relational operators

inline bool
operator== (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  return lhs.m_source_ip == rhs.m_source_ip && lhs.m_source_id == rhs.m_source_id;
}

inline bool
operator!= (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  return !operator== (lhs, rhs);
}

inline bool
operator< (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  if (lhs.m_source_ip != rhs.m_source_ip)
    return lhs.m_source_ip < rhs.m_source_ip;

  return lhs.m_source_id < rhs.m_source_id;
}

inline bool
operator> (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  return operator< (rhs, lhs);
}

inline bool
operator<= (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  return !operator> (lhs, rhs);
}

inline bool
operator>= (const DataIdentifier & lhs, const DataIdentifier & rhs)
{
  return !operator< (lhs, rhs);
}

// DataIdentifier stream operators

inline std::ostream &
operator<< (std::ostream & os, const DataIdentifier & obj)
{
  obj.Print (os);
  return os;
}

}
}

#endif /* UTILS_PACKET_UTILS_H */

