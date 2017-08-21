#ifndef _SECURITY_MANAGER_H_
#define _SECURITY_MANAGER_H_

#include <string>
#include <vector>
#include <cstddef>
#include <core/design_patterns/singleton_dclp.hpp>

namespace order_matcher
{

class SecurityManager : public core::SingletonDCLP<SecurityManager>
{
    public :
        std::size_t addSecurity(const std::string&);
        std::size_t getSecurityId(const std::string&)const;
        const std::string getSecurityName(std::size_t securityId) const;
        bool isSecuritySupported(const std::string&) const;
        bool isSecuritySupported(std::size_t) const;
    private :
        std::vector<std::string> m_securities;
};

} // namespace

#endif