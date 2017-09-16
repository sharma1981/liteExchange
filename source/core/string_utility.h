#ifndef _STRING_UTILITY_H_
#define _STRING_UTILITY_H_

#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <sstream>
#include <algorithm>

namespace core
{

inline bool replaceInString(std::string& target, const std::string& from, const std::string& to)
{
    auto startPos = target.find(from);

    if (startPos == std::string::npos)
    {
        return false;
    }

    target.replace(startPos, from.length(), to);
    return true;
}

inline void replaceCharacterInString(std::string& target, char oldChar, char newChar)
{
  std::replace(target.begin(), target.end(), oldChar, newChar);
}

inline bool contains(const std::string& haystack, const std::string& needle)
{
    if (haystack.find(needle) != std::string::npos)
    {
        return true;
    }

    return false;
}

inline std::vector<std::string> split(const std::string& input, char seperator)
{
  std::vector<std::string> ret;
  if (input.length() > 0)
  {
    std::istringstream stream(input);
    std::string token;
    while (std::getline(stream, token, seperator))
    {
      ret.push_back(token);
    }
  }
  return ret;
}

template <typename T>
inline T toLowercase(const T& input)
{
    T ret = input;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
    return ret;
}

template <typename T>
inline T toUppercase(const T& input)
{
    T ret = input;
    std::transform(ret.begin(), ret.end(), ret.begin(), ::toupper);
    return ret;
}

inline bool compare(const std::string& first, const std::string& second)
{
  bool same = false;

  auto firstLowerCase = toLowercase(first);
  auto secondLowerCase = toLowercase(second);

  if (!strcmp(firstLowerCase.c_str(), secondLowerCase.c_str()))
  {
    same = true;
  }

  return same;
}

inline bool startsWith(const std::string& inputString, char c)
{
  if (inputString.length() > 0)
  {
    if (inputString[0] == c)
    {
      return true;
    }
  }
  return false;
}

inline void narrowStringToWideString(const std::string& source, std::wstring& destination)
{
  std::wstringstream wideStringStream;
  wideStringStream << source.c_str();
  destination = wideStringStream.str();
}

inline void wideStringToNarrowString(const std::wstring& source, std::string& destination)
{
  std::string narrowString(source.begin(), source.end());
  destination = narrowString;
}

}// namespace

#endif