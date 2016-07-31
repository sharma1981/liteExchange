#ifndef _FACTORY_H_
#define _FACTORY_H_

#include <string>

namespace utility
{

template<typename T>
class Factory
{
    public:
        virtual T create(const std::string& creationParameter) = 0;
};

}//namespace

#endif