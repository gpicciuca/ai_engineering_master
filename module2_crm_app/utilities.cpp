#include "utilities.h"

#include <iomanip>
#include <iostream>
#include <sstream>

namespace utilities {

// Source: https://www.geeksforgeeks.org/how-to-convert-string-to-date-in-cpp/
bool to_timestamp(const std::string& date, const char* format,
                  std::time_t& output) {
  std::stringstream strstream{date};

  std::tm date_time{};
  strstream >> std::get_time(&date_time, format);

  if (strstream.fail()) {
    return false;
  }

  output = std::mktime(&date_time);
  return true;
}

bool is_valid_date(const std::string& date, const char* format) {
  std::time_t timestamp{};
  return to_timestamp(date, format, timestamp);
}

void remove_chars_from_str(std::string& str, const std::string& pattern,
                           const char replace) {
  const auto cb = [pattern](const char c) {
    return std::any_of(pattern.begin(), pattern.end(),
                       [c](const char p) { return p == c; });
  };

  std::replace_if(str.begin(), str.end(), cb, replace);
}

}  // namespace utilities