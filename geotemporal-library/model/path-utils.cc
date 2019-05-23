/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Common path name manipulations.
 */
#include "path-utils.h"

#include "string-utils.h"

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

void
SplitDrive (const std::string & ipath, std::string & drive_or_unc, std::string & opath)
{
  // Based on the python 2.7's ntpath.splitdrive(p) function.
  if (ipath.size () > 1u)
    {
      const char & directory_separator = DIRECTORY_SEPARATOR ();

      // Normalize the path's directory separator character.
      const std::string norm_path = ReplaceAll_Copy (ipath, ALTERNATE_DIRECTORY_SEPARATOR (),
                                                     directory_separator);

      // If is a UNC path:
      if (norm_path.size () >= 3u && norm_path.at (0u) == directory_separator
          && norm_path.at (1u) == directory_separator
          && norm_path.at (2u) != directory_separator)
        {
          // Is a UNC path:
          // vvvvvvvvvvvvvvvvvvvv drive letter or UNC path
          // \\machine\mountpoint\directory\etc\...
          //           directory ^^^^^^^^^^^^^^^

          std::string::size_type index = norm_path.find (directory_separator, 2u);

          if (index == std::string::npos)
            {
              drive_or_unc = "";
              opath = ipath;
              return;
            }

          std::string::size_type index2 = norm_path.find (directory_separator, index + 1u);

          // A UNC path can't have two slashes in a row
          // (after the initial two)
          if (index2 == index + 1u)
            {
              drive_or_unc = "";
              opath = ipath;
              return;
            }

          if (index2 == std::string::npos) index2 = norm_path.size ();

          drive_or_unc = ipath.substr (0u, index2);
          opath = ipath.substr (index2);
          return;
        }

      // If is a drive path:
      if (norm_path.at (1u) == ':')
        {
          drive_or_unc = ipath.substr (0u, 2u);
          opath = ipath.substr (2u);
          return;
        }
    }

  drive_or_unc = "";
  opath = ipath;
}

std::string
JoinPath (std::initializer_list<std::string> paths)
{
  // Based on the python 2.7's ntpath.join(path, *paths) function.
  const char & directory_separator = DIRECTORY_SEPARATOR ();

  std::string result_drive, result_path;
  std::string current_path, p_drive, p_path;

  for (std::initializer_list<std::string>::const_iterator current_path_it = paths.begin ();
          current_path_it != paths.end (); ++current_path_it)
    {
      current_path = *current_path_it;

      SplitDrive (current_path, p_drive, p_path);

      // If this is the first path initialize the result drive and path.
      if (current_path_it == paths.begin ())
        {
          result_drive = p_drive;
          result_path = p_path;
          continue;
        }

      // If the current path is absolute
      if (!p_path.empty () && (p_path.at (0u) == '/' || p_path.at (0u) == '\\'))
        {
          // The current path is absolute, reset (star over) constructing the
          // result drive and result path.
          if (!p_drive.empty () || result_drive.empty ())
            result_drive = p_drive;

          result_path = p_path;
          continue;
        }
      else if (!p_drive.empty () && p_drive != result_drive)
        {
          if (ToLowerCase_Copy (result_drive) != ToLowerCase_Copy (p_drive))
            {
              // Different drives, then ignore the constructed path so far entirely.
              result_drive = p_drive;
              result_path = p_path;
              continue;
            }

          // Same drive just different case
          result_drive = p_drive;
        }

      // if current path is relative to the first
      if (!result_path.empty () && result_path.back () != '\\'
          && result_path.back () != '/')
        result_path += directory_separator;

      result_path += p_path;
    }

  // Add separator between UNC and non-absolute path
  if (!result_path.empty () && result_path.front () != '\\' && result_path.front () != '/'
      && !result_drive.empty () && result_drive.back () != ':')
    {
      return result_drive + directory_separator + result_path;
    }

  return result_drive + result_path;
}

}
}
