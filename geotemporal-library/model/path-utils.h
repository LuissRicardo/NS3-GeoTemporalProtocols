/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Common path name manipulations.
 */
#ifndef UTILS_PATH_UTILS_H
#define UTILS_PATH_UTILS_H

#include <initializer_list>
#include <string>

namespace GeoTemporalLibrary
{
namespace LibraryUtils
{

/**
 * Directory separator character used in the current system.
 */
inline const char &
DIRECTORY_SEPARATOR ()
{
  static char ret = '/';

#ifdef __CYGWIN__
  ret = '\\';
#endif

  return ret;
}

/**
 * Directory separator character used in different systems.
 */
inline const char &
ALTERNATE_DIRECTORY_SEPARATOR ()
{
  static char ret = '\\';

#ifdef __CYGWIN__
  ret = '/';
#endif

  return ret;
}

/**
 * Splits a pathname into drive/UNC sharepoint and relative path specifiers.
 *
 * Returns the result to the two output parameters <code>drive_or_unc</code> and
 * <code>path</code>; either part may be empty.
 *
 * If you call:
 *
 *      <code>LibraryUtils::SplitDrive(ipath, drive, opath);</code>
 *
 * It is always true that:
 *
 *      <code>drive + opath == ipath</code>
 *
 * If the <code>ipath</code> path contained a drive letter,
 * <code>drive_ou_unc</code> will contain everything up to and including the
 * colon. E.g.
 * <code>LibraryUtils::SplitDrive("c:&#92;dir", drive, opath);</code>
 * will assign <code>drive = "c:"</code> and <code>opath = "&#92;dir"</code>.
 *
 * If the path contained a UNC path, the <code>drive_ou_unc</code> will contain
 * the host name and share up to but not including the fourth directory
 * separator character. E.g.
 * <code>LibraryUtils::SplitDrive("//host/computer/dir", drive, opath, '/');</code>
 * will assign <code>drive = "//host/computer"</code> and
 * <code>opath = "/dir"</code>.
 *
 * Paths cannot contain both a drive letter and a UNC path.
 *
 * @param ipath [IN] Input path to split into drive or UNC, and path specifications.
 * @param drive_or_unc [OUT] Drive or UNC specification.
 * @param opath [OUT] Path specification.
 */
void
SplitDrive (const std::string & ipath, std::string & drive_or_unc, std::string & opath);

/**
 * Joins two or more pathname components, inserting the appropriate directory
 * separator character (<code>'&#92;'</code> or <code>'/'</code>) as needed.
 *
 * The return value is the concatenation of any members of <code>paths</code>
 * with exactly one directory separator <code>DIRECTORY_SEPARATOR ()</code>
 * following each non-empty part except the last, meaning that the result will
 * only end in a separator if the last part is empty. If a component is an
 * absolute path, all previous components are thrown away and joining continues
 * from the absolute path component.
 *
 * For Windows' paths, the drive letter is not reset when an absolute path
 * component (e.g. <code>"&#92;foo"</code>) is encountered. If a component
 * contains a drive letter, all previous components are thrown away and the
 * drive letter is reset. Note that since there is a current directory for each
 * drive, <code>JoinPath("c:", "foo")</code> represents a path relative to the
 * current directory on drive <code>C:</code> (<code>c:foo</code>), not
 * <code>c:&#92;foo</code>.
 *
 * @param paths Paths to join.
 * @return The joined paths.
 */
std::string
JoinPath (std::initializer_list<std::string> paths);

}
}

#endif //UTILS_PATH_UTILS_H
