/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
#include "string-utils.h"

#include <sstream>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

std::vector<std::string>
Split (const std::string & text, char separator)
{
  std::istringstream tokens_stream (text);
  std::vector<std::string> tokens;
  std::string token;

  while (std::getline (tokens_stream, token, separator))
    {
      tokens.push_back (token);
    }

  if (!text.empty () && text.back () == separator) tokens.push_back ("");

  return tokens;
}

void
ReplaceAll (std::string & str_source, const std::string & old_substr,
            const std::string & new_substr)
{
  std::string new_str;

  // Check four cases:
  //   i) If both old substring (old_substr) and new substring (new_substr) are
  //      equal (trivial case).
  //  ii) Source string (str_source) and old substring (old_substr) to be
  //      replaced are equal (trivial case).
  // iii) Old substring (old_substr) to be replaced is empty (trivial case).
  //  iv) Find occurrences of old substring (old_substr) in the source string
  //      (str_source) and replace each of them with the new string (new_substr).

  if (old_substr == new_substr) // Case (i)
    {
      // Old substring (old_substr) and new substring (new_substr) are equal,
      // then really there's nothing to do.
      return;
    }
  else if (str_source == old_substr) // Case (ii)
    {
      // The substring to be replaced is equal to the source string, the expected
      // output is just the new substring (new_substr).
      new_str = new_substr;
    }
  else if (old_substr.empty ()) // Case (iii)
    {
      // Old substring to be replaced is the empty string.
      // Then at the beginning, between each character and at the end of the
      // original str_source string insert the old_substr string.
      new_str.reserve (str_source.size ()
                       + ((str_source.size () + 1u) * new_substr.size ())); // Avoids a few memory allocations

      for (std::string::const_iterator character_it = str_source.begin ();
              character_it != str_source.end (); ++character_it)
        {
          new_str += new_substr + *character_it;
        }

      new_str += new_substr;
    }
  else // Case (iv)
    {
      // Non-empty old substring to be replaced given. Then find occurrences of
      // old_substr and replace them for new_substr.
      new_str.reserve (str_source.size ()); // Avoids a few memory allocations

      std::string::size_type last_position = 0;
      std::string::size_type find_position;

      while (std::string::npos != (find_position = str_source.find (old_substr, last_position)))
        {
          new_str.append (str_source, last_position, find_position - last_position);
          new_str += new_substr;
          last_position = find_position + old_substr.size ();
        }

      // Care for the rest after last occurrence
      new_str += str_source.substr (last_position);
    }

  str_source.swap (new_str);
}

bool
GetInputStreamNextLine (std::istream & istream, std::string & read_text_line)
{
  if (!std::getline (istream, read_text_line)) return false;
  Trim (read_text_line);
  return true;
}

}
}
