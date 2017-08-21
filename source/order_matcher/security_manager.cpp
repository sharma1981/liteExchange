#include "security_manager.h"
#include <algorithm>
#include <core/string_utility.h>
using namespace std;

namespace order_matcher
{

size_t SecurityManager::addSecurity(const string& symbol)
{
    m_securities.push_back(symbol);
    auto ret = m_securities.size();
    return ret;
}

size_t SecurityManager::getSecurityId(const string& symbol)const
{
    size_t index{ 0 };
    for (auto security : m_securities)
    {
        index++;
        if (core::compare(symbol, security))
        {
            return index;
        }
    }
    return -1;
}

const string SecurityManager::getSecurityName(size_t securityId) const
{
    return m_securities[securityId - 1];
}

bool SecurityManager::isSecuritySupported(const string& symbol) const
{
    if (std::find(m_securities.begin(), m_securities.end(), symbol) != m_securities.end())
    {
        return true;
    }
    return false;
}

bool SecurityManager::isSecuritySupported(size_t securityId) const
{
    if (securityId > 0 && securityId <= m_securities.size())
    {
        return true;
    }
    return false;
}


} // namespace