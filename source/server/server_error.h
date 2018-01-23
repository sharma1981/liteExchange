#ifndef _SERVER_ERROR_H_
#define _SERVER_ERROR_H_

#include <cstdlib>
#include <string>
#include <iostream>
#include <type_traits>
#include <core/logger/logger.h>

enum class ServerError {
    INVALID_CONFIG_FILE,
    ALREADY_RUNNING,
    RUNTIME_ERROR,
    INSUFFICIENT_MEMORY,
    NON_SUPPORTED_EXECUTION,
    UNKNOWN_PROBLEM
};

inline void onError(const std::string& message, ServerError error)
{
    std::cerr << message << std::endl;
    auto exit_code = static_cast<std::underlying_type<ServerError>::type >(error);

    if (core::Logger::getInstance()->isAlive())
    {
        LOG_ERROR("Main thread", "Ending")
            core::Logger::getInstance()->shutdown();
    }
    std::exit(exit_code);
}


#endif