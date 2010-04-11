#include "string_helper.h"

void split(const std::string &str, std::vector<std::string> &tokens, const std::string &sep)
{
    int lastPos = str.find_first_not_of(sep, 0);
    int pos = str.find_first_of(sep, lastPos);
	
    while (lastPos != -1 || pos != -1)
    {
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(sep, pos);
        pos = str.find_first_of(sep, lastPos);
    }
}

void toLower(std::string &str)
{
	for (int i = 0; i < str.size(); i++)
		str[i] = tolower(str[i]);
}