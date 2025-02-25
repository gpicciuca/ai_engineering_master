#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <algorithm>
#include <ctime>
#include <string>
#include <vector>

namespace utilities {

/// @brief Safely convert a string to an integer
/// @tparam ToType Output type
/// @param input String to attempt to convert
/// @param output Where to store the converted result
/// @return True if conversion succeeds, false otherwise
template <typename ToType>
bool try_convert(const std::string& input, ToType& output) {
  try {
    output = static_cast<ToType>(std::stoull(input));
    return true;
  } catch (const std::exception& e) {
  }
  return false;
}

/// @brief Checks if needle is present within haystack
/// @tparam T Output type
/// @param haystack Vector of values of type T
/// @param needle Value to look for
/// @return True if value found, false otherwise
template <typename T>
bool is_in_vector(const std::vector<T>& haystack, const T& needle) {
  return std::find(haystack.cbegin(), haystack.cend(), needle) !=
         haystack.cend();
}

/// @brief Converts a date string into the corresponding timestamp
/// @param date A string containing a date
/// @param format Date format expected in the input string
/// @param output Where to store the timestamp
/// @return True if conversion succeeds, false otherwise
bool to_timestamp(const std::string& date, const char* format,
                  std::time_t& output);

/// @brief Checks if the date string can be converted to a timestamp
/// following a given format
/// @param date String containing a date
/// @param format Date format
/// @return True if string is a valid date, false otherwise
bool is_valid_date(const std::string& date, const char* format);

/// @brief Replace all occurrences of pattern (individual chars) with 'replace'
/// in str
/// @param str Input string
/// @param pattern A string containing one or more chars to use as pattern
/// @param replace Single-char to replace all chars from pattern with
void remove_chars_from_str(std::string& str, const std::string& pattern,
                           const char replace);

}  // namespace utilities

#endif  // __UTILITIES_H__
