#include "fix_message.h"
#include "fix_constants.h"
#include <core/string_utility.h>
#include <core/datetime_utility.h>
#include <core/pretty_exception.h>
#include <fstream>
using namespace std;

namespace fix
{

char FixMessage::getMessageType() const
{
    string val;
    getTagValue(FixConstants::FIX_TAG_MESSAGE_TYPE, val);
    return val[0];
}

bool FixMessage::isAdminMessage() const
{
    auto type = getMessageType();

    if (type == FixConstants::FIX_MESSAGE_LOG_OFF || type == FixConstants::FIX_MESSAGE_LOG_ON || type == FixConstants::FIX_MESSAGE_HEARTBEAT)
    {
        return true;
    }

    return false;
}

int FixMessage::calculateBodyLength() const
{
    int bodyLength{ 0 };


    for(const auto& tagValue : m_tagValuePairs)
    {
        int tagValueKey = tagValue.first;

        //  We exclude header and checksum
        if (tagValueKey == FixConstants::FIX_TAG_VERSION)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_BODY_LENGTH)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_BODY_CHECKSUM)
        {
            continue;
        }

        auto value = tagValue.second;
        int keyLength = std::to_string(tagValueKey).length();

        bodyLength += keyLength + 2 + value.length(); // +2 is because of = and delimiter
    }

    return bodyLength;
}

bool FixMessage::hasTag(int tag) const
{
    auto element = m_tagValuePairs.find(tag);
    if (element == m_tagValuePairs.end())
    {
        return false;
    }
    return true;
}

void FixMessage::setTag(int tag, const string& value)
{
    m_tagValuePairs[tag] = value;
}

void FixMessage::setTag(int tag, int value)
{
    setTag(tag, std::to_string(value));
}

void FixMessage::setTag(int tag, char value)
{
    setTag(tag, std::to_string(value));
}

void FixMessage::setFixVersion(const string& version)
{
    setTag(FixConstants::FIX_TAG_VERSION, version);
}

void FixMessage::getTagValue(int tag, string& value) const
{
    value = m_tagValuePairs[tag];
}

void FixMessage::loadFromString(const string& input)
{
    auto tagValuePairs = core::split(input, FixConstants::FIX_DELIMITER);

    for(auto& tagValuePair : tagValuePairs)
    {
        if (tagValuePair.length() == 0)
        {
            continue;
        }

        auto tokens = core::split(tagValuePair, FixConstants::FIX_EQUALS);
        int tag = std::stoi(tokens[0]);
        string value = tokens[1];
        setTag(tag, value);
    }
}

void FixMessage::loadFromFile(const string& input, vector<FixMessage>& messages)
{
    ifstream file(input); // ifstream dtor also closes the file so no need for using a smart ptr to close the file

    if (!file.good())
    {
        THROW_PRETTY_RUNTIME_EXCEPTION(core::format("File %s could not be opened", input))
    }

    file.seekg(0, std::ios::beg);
    string line;

    while (std::getline(file, line))
    {
        if (core::startsWith(line, '#') == false)
        {
            FixMessage message;
            message.loadFromString(line);
            messages.emplace_back(message);
        }
    }

    file.close();
}

void FixMessage::toString(bool sendingAsMessage, bool updateTransactionTime, string& stringOutput)
{
    auto appendTag = [&stringOutput, this](int tag)
                            {
                                string value;
                                getTagValue(tag, value);
                                stringOutput += std::to_string(tag) + FixConstants::FIX_EQUALS + value + FixConstants::FIX_DELIMITER;
                            };

    auto appendTagValue = [&stringOutput, this](int tag, string value)
    {
        stringOutput += std::to_string(tag) + FixConstants::FIX_EQUALS + value + FixConstants::FIX_DELIMITER;
    };

    // FIX VERSION
    appendTag(FixConstants::FIX_TAG_VERSION);

    // FIX SENDING TIME AND TRANSACTION, have to be before body length calculation ,but not appended for the correct order
    if (sendingAsMessage)
    {
        auto currentUTCDateTime = core::getUtcDatetime(core::Subseconds::MICROSECONDS);

        setTag(FixConstants::FIX_TAG_SENDING_TIME, currentUTCDateTime);

        if (updateTransactionTime && isAdminMessage() == false)
        {
            setTag(FixConstants::FIX_TAG_TRANSACTION_TIME, currentUTCDateTime);
        }
    }

    // FIX BODY LENGTH
    if (sendingAsMessage)
    {
        appendTagValue(FixConstants::FIX_TAG_BODY_LENGTH, std::to_string(calculateBodyLength()));
    }

    // FIX MESSAGE TYPE
    appendTag(FixConstants::FIX_TAG_MESSAGE_TYPE);

    // FIX SEQUENCE NUMBER
    appendTag(FixConstants::FIX_TAG_SEQUENCE_NUMBER);

    // FIX SENDER COMPID
    appendTag(FixConstants::FIX_TAG_SENDER_COMPID);

    // FIX SENDING TIME
    appendTag(FixConstants::FIX_TAG_SENDING_TIME);

    // FIX TARGET COMPID
    appendTag(FixConstants::FIX_TAG_TARGET_COMPID);

    for(const auto& tagValue : m_tagValuePairs)
    {
        auto tagValueKey = tagValue.first;
        if (tagValueKey == FixConstants::FIX_TAG_VERSION)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_BODY_LENGTH)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_MESSAGE_TYPE)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_SEQUENCE_NUMBER)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_SENDING_TIME)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_SENDER_COMPID)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_TARGET_COMPID)
        {
            continue;
        }

        if (tagValueKey == FixConstants::FIX_TAG_BODY_CHECKSUM)
        {
            continue;
        }

        appendTag(tagValueKey);
    }

    // FIX CHECKSUM
    if (sendingAsMessage)
    {
        string checksum;
        FixMessage::calculateCheckSum(stringOutput, checksum);
        appendTagValue(FixConstants::FIX_TAG_BODY_CHECKSUM, checksum);
    }
}

void FixMessage::calculateCheckSum(const string& message, string& checksum)
{
    int sum{ 0 };

    for(char c : message)
    {
        sum += (int)c;
    }

    sum = sum % 256;

    checksum = std::to_string(sum);
    core::padLeft(checksum, 3, '0');
}

}