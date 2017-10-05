#ifndef _CONFIG_FILE_H_
#define _CONFIG_FILE_H_

#include<string>
#include<vector>
#include<unordered_map>

namespace core
{

class ConfigFile
{
    public :

        void loadFromFile(const std::string& fileName);
        bool doesAttributeExist(const std::string& attribute) const;

        const std::string getStringValue(const std::string& attribute, std::string defaultVal="") const;
        bool getBoolValue(const std::string& attribute, bool defaultVal=false) const;
        int getIntValue(const std::string& attribute, int defaultVal=0) const;
        std::vector<std::string> getArray(const std::string& attribute);

    private:

        mutable std::unordered_multimap<std::string, std::string> m_dictionary;
};

}//namespace
#endif