/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#ifndef UTILS_STRING_UTILS_H
#define UTILS_STRING_UTILS_H

#include <algorithm>
#include <cctype>
#include <functional>
#include <string>
#include <vector>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

/**
 * End line string used in the current system.
 */
inline const std::string &
SYSTEM_NEW_LINE_STRING ()
{
  static std::string ret = "\n";

#ifdef __CYGWIN__
  ret = "\r\n";
#endif

  return ret;
}

/**
 * Splits a string object into a vector of strings by separating the string into
 * substrings, using a specified <code>separator</code> string to determine where
 * to make each split.
 * @param text [IN] The string to split.
 * @param separator The string which denotes the points at which each split
 * should occur.
 * @return A <code>vector</code> of strings split at each point where the
 * <code>separator</code> occurs in the given string.
 */
std::vector<std::string>
Split (const std::string & text, char separator);

/**
 * Replaces <b>in place</b> all the occurrences of <code>old_substr</code>
 * substring with <code>new_substr</code> substring in the given
 * <code>str_source</code> string.
 *
 * @param str_source [IN/OUT] String where the replacement will be performed
 * <b>in place</b>.
 * @param old_substr [IN] This is the old substring to be replaced.
 * @param new_substr [IN] This is the new substring, which would replace old
 * substring.
 */
void
ReplaceAll (std::string & str_source, const std::string & old_substr,
            const std::string & new_substr);

/**
 * Replaces <b>in place</b> all the occurrences of <code>old_substr</code>
 * substring with <code>new_substr</code> substring in the given
 * <code>str_source</code> string.
 *
 * @param str_source [IN/OUT] String where the replacement will be performed
 * <b>in place</b>.
 * @param old_substr [IN] This is the old substring to be replaced.
 * @param new_substr [IN] This is the new substring, which would replace old
 * substring.
 */
inline void
ReplaceAll (std::string & str_source, const char & old_substr,
            const char & new_substr)
{
  ReplaceAll (str_source, std::string (1u, old_substr), std::string (1u, new_substr));
}

/**
 * Returns <b>a copy</b> of <code>str_source</code> string in which all the
 * occurrences of <code>old_substr</code> have been replaced with
 * <code>new_substr</code>.
 *
 * @param str_source String where the replacement will be performed.
 * @param old_substr [IN] This is the old substring to be replaced.
 * @param new_substr [IN] This is the new substring, which would replace old
 * substring.
 * @return A copy of the <code>str_source</code> string with all occurrences of
 * substring <code>old_substr</code> replaced by <code>new_substr</code>.
 */
inline std::string
ReplaceAll_Copy (std::string str_source, const std::string & old_substr,
                 const std::string & new_substr)
{
  ReplaceAll (str_source, old_substr, new_substr);
  return str_source;
}

/**
 * Returns <b>a copy</b> of <code>str_source</code> string in which all the
 * occurrences of <code>old_substr</code> have been replaced with
 * <code>new_substr</code>.
 *
 * @param str_source String where the replacement will be performed.
 * @param old_substr [IN] This is the old substring to be replaced.
 * @param new_substr [IN] This is the new substring, which would replace old
 * substring.
 * @return A copy of the <code>str_source</code> string with all occurrences of
 * substring <code>old_substr</code> replaced by <code>new_substr</code>.
 */
inline std::string
ReplaceAll_Copy (std::string str_source, const char & old_substr,
                 const char & new_substr)
{
  ReplaceAll (str_source, std::string (1u, old_substr), std::string (1u, new_substr));
  return str_source;
}

/**
 * Returns <code>true</code> if the <code>text</code> string starts with the
 * specified <code>prefix</code> string, otherwise returns <code>false</code>.
 * @param text [IN] String to test.
 * @param prefix [IN] String to look for at the start of the <code>text</code>
 * string.
 * @return <code>true</code> if the string starts with the specified prefix,
 * otherwise <code>false</code>.
 */
inline bool
StartsWith (const std::string & text, const std::string & prefix)
{
  return text.size () >= prefix.size ()
          && 0 == text.compare (0u, prefix.size (), prefix);
}

/**
 * Returns <code>true</code> if the <code>text</code> string ends with the
 * specified <code>suffix</code> string, otherwise returns <code>false</code>.
 * @param text [IN] String to test.
 * @param suffix [IN] String to look for at the end of the <code>text</code>
 * string.
 * @return <code>true</code> if the string ends with the specified suffix,
 * otherwise <code>false</code>.
 */
inline bool
EndsWith (const std::string & text, const std::string & suffix)
{
  return text.size () >= suffix.size ()
          && 0 == text.compare (text.size () - suffix.size (), suffix.size (), suffix);
}

/**
 * Converts <b>in place</b> all uppercase characters to lowercase.
 *
 * @param text String to change to lowercase.
 */
inline void
ToLowerCase (std::string & text)
{
  std::transform (text.begin (), text.end (), text.begin (), ::tolower);
}

/**
 * Returns <b>a copy</b> of the string in which all case-based characters have
 * been lowercased.
 *
 * @param text String to change to lowercase.
 * @return A copy of the string in which all case-based characters have been
 * lowercased.
 */
inline std::string
ToLowerCase_Copy (std::string text)
{
  if (!text.empty ())
    ToLowerCase (text);
  return text;
}

/**
 * Converts <b>in place</b> all lowercase characters to uppercase.
 *
 * @param text String to change to lowercase.
 */
inline void
ToUpperCase (std::string & text)
{
  std::transform (text.begin (), text.end (), text.begin (), ::toupper);
}

/**
 * Returns <b>a copy</b> of the string in which all case-based characters have
 * been uppercased.
 *
 * @param text String to change to uppercase.
 * @return A copy of the string in which all case-based characters have been
 * uppercased.
 */
inline std::string
ToUpperCase_Copy (std::string text)
{
  if (!text.empty ())
    ToUpperCase (text);
  return text;
}

/**
 * Removes <b>in place</b> the leading whitespace characters of the string.
 * @param text [IN/OUT] The string to trim.
 */
inline void
TrimFront (std::string & text)
{
  if (text.empty ()) return;
  text.erase (text.begin (), std::find_if (text.begin (), text.end (),
                                           std::not1 (std::ptr_fun<int, int> (std::isspace))));
}

/**
 * Returns <b>a copy</b> of the string with leading whitespace characters removed.
 */
inline std::string
TrimFront_Copy (std::string text)
{
  if (!text.empty ())
    TrimFront (text);
  return text;
}

/**
 * Removes <b>in place</b> the trailing whitespace characters of the string.
 * @param text [IN/OUT] The string to trim.
 */
inline void
TrimBack (std::string & text)
{
  if (text.empty ()) return;
  text.erase (std::find_if (text.rbegin (), text.rend (),
                            std::not1 (std::ptr_fun<int, int> (std::isspace))).base (), text.end ());
}

/**
 * Returns <b>a copy</b> of the string with trailing emtpy characters removed.
 */
inline std::string
TrimBack_Copy (std::string text)
{
  if (!text.empty ())
    TrimBack (text);
  return text;
}

/**
 * Removes <b>in place</b> the leading and trailing whitespace characters of the string.
 * @param text [IN/OUT] The string to trim.
 */
inline void
Trim (std::string & text)
{
  if (text.empty ()) return;
  TrimFront (text);
  TrimBack (text);
}

/**
 * Returns <b>a copy</b> of the string with leading and trailing emtpy characters
 * removed.
 */
inline std::string
Trim_Copy (std::string text)
{
  if (!text.empty ())
    Trim (text);
  return text;
}

/**
 * Extracts characters from the <code>istream</code> input stream and stores
 * them into the <code>text_line</code> string, until a end-line character
 * (<code>'&#92;n'</code>) is found.
 *
 * If the end-line character (<code>'&#92;n'</code>) is found, it is extracted
 * and discarded (i.e. it is not stored in <code>text_line</code> and the next
 * input operation on <code>istream</code> will begin after it).
 *
 * The extraction also stops if the <i>end-of-file</i> is reached in
 * <code>istream</code> or if some other error occurs during the input
 * operation.
 *
 * Note that any content in <code>text_line</code> before the call is replaced
 * by the newly extracted sequence.
 *
 * Each extracted character is appended to the string as if its member
 * <code>push_back</code> was called.
 *
 * @param istream [IN/OUT] <code>std::istream</code> input stream from which
 * characters are extracted.
 * @param read_text_line [OUT] <code>std::string</code> object where the extracted
 * line is stored.
 * The contents in the string before the call (if any) are discarded and
 * replaced by the extracted line.
 * @return Returns <code>true</code> if it extracts at least one character,
 * <code>false</code> otherwise (i.e. if the function extracts no characters
 * or if it manages to extract <code>basic_string::max_size</code> characters
 * from the input stream).
 */
bool
GetInputStreamNextLine (std::istream & istream, std::string & read_text_line);

}
}

#endif //UTILS_STRING_UTILS_H
