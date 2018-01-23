#ifndef _FIX_MESSAGE_
#define _FIX_MESSAGE_

#include <unordered_map>
#include <vector>
#include <string>

namespace fix
{

class FixMessage
{
    public:

        char getMessageType() const;
        bool isAdminMessage() const;
        int calculateBodyLength() const;
        void getTagValue(int tag, std::string& value) const;

        bool hasTag(int tag)const;
        void setTag(int tag, const std::string& value);
        void setTag(int tag, int value);
        void setTag(int tag, char value);
        void setFixVersion(const std::string& version);

        void loadFromString(const std::string& input);
        void toString(bool sendingAsMessage, bool updateTransactionTime, std::string& stringOutput);

        static void loadFromFile(const std::string& input, std::vector<FixMessage>& messages);
        static void calculateCheckSum(const std::string& message, std::string& checksum);

    private:
        mutable std::unordered_map<int, std::string> m_tagValuePairs;
};

}

#endif