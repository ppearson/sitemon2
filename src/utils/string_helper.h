#ifndef STRING_HELPER_H
#define STRING_HELPER_H

#include <string>
#include <vector>

void split(const std::string &str, std::vector<std::string> &lines, const std::string &sep = "\n");
void toLower(std::string &str);

#endif
