#ifndef _STRINGUTIL_HPP
#define _STRINGUTIL_HPP

#include <vector>
#include <string>

std::vector<std::string> split(const std::string& s, char delimiter=' ', bool skip_empty=false);
std::string trim(std::string const& input);

#endif /* _STRINGUTIL_HPP */
