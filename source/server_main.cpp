#include <compiler_portability/ignored_warnings.h>
// COMPILE TIME CHECKS
#include <compiler_portability/cpp_version_check.h> // C++11 and later supported
#include <compiler_portability/os_version_check.h>  // Linux and Windows supported

// RUNTIME CHECKS
#include <memory/cache_line.h>                      // To see if cache line we are running on
                                                    // matches the compiled one
#include <utility/os_utility.h>                     // To check whether we are root/admin or not

#include <exception>
#include <iostream>

#include <boost/format.hpp>

#include <utility/single_instance.h>
#include <utility/logger/logger.h>
#include <utility/file_utility.h>

#include <server/server.h>
#include <server/server_configuration.h>
#include <server/server_error.h>
#include <server/server_constants.h>

using namespace std;

int main ()
{
    // Initial checks
    if (memory::getCacheLineSize() != CACHE_LINE_SIZE)
    {
        auto message = boost::str(boost::format("This executable compiled for cache line size %d ,\
                                                 but you are running on a CPU with a cache line of %d")
                                                 % CACHE_LINE_SIZE % memory::getCacheLineSize()
                                                );
        Server::onError(message, ServerError::NON_SUPPORTED_EXECUTION);
    }

    if (utility::amIAdmin() == false)
    {
        // Mainly needed for ability to set thread priorities
        Server::onError("You can only run in root/administrator mode", ServerError::NON_SUPPORTED_EXECUTION);
    }

    // Set current working directory as current executable`s directory
    utility::setCurrentWorkingDirectory( utility::getCurrentExecutableDirectory() );

    // Load configuration file
    ServerConfiguration serverConfiguration;
    try
    {
        serverConfiguration.load(server_constants::CONFIGURATION_FILE);
    }
    catch (const std::invalid_argument & e)
    {
        Server::onError(e.what(), ServerError::INVALID_CONFIG_FILE);
    }
    catch (const std::runtime_error & e)
    {
        Server::onError(e.what(), ServerError::INVALID_CONFIG_FILE);
    }
    catch (const std::bad_alloc &)
    {
        Server::onError("Insufficient memory", ServerError::INSUFFICIENT_MEMORY);
    }
    catch (...)
    {
        Server::onError("Unknown exception occured", ServerError::UNKNOWN_PROBLEM);
    }

    // Single instance protection
    utility::SingleInstance singleton(serverConfiguration.getSingleInstancePortNumber());

    if ( !singleton() )
    {
        Server::onError("Ome process is running already.", ServerError::ALREADY_RUNNING);
    }

    // Backup FIX engine logs if exists
    utility::backupDirectory(server_constants::FIX_ENGINE_LOG_DIRECTORY, server_constants::FIX_ENGINE_LOG_DIRECTORY + string("_") + utility::getCurrentDateTime("%d_%m_%Y_%H_%M_%S"), server_constants::FIX_ENGINE_LOG_BACKUP_DIRECTORY);
    utility::createDirectory(server_constants::FIX_ENGINE_LOG_DIRECTORY);

    // Start and run the server
    try
    {
        // Start logger
        utility::Logger::getInstance().start();
        LOG_INFO("Main thread", "starting")

        Server application(server_constants::FIX_ENGINE_CONFIG_FILE, serverConfiguration);

        // Run the server
        application.run();
    }
    catch (const std::invalid_argument & e)
    {
        Server::onError(e.what(), ServerError::RUNTIME_ERROR);
    }
    catch (const std::runtime_error & e)
    {
        Server::onError(e.what(), ServerError::RUNTIME_ERROR);
    }
    catch (const std::bad_alloc & )
    {
        Server::onError("Insufficient memory", ServerError::INSUFFICIENT_MEMORY);
    }
    catch (...)
    {
        Server::onError("Unknown exception occured", ServerError::UNKNOWN_PROBLEM);
    }
    //////////////////////////////////////////
    // Application exit
    LOG_INFO("Main thread", "Ending")
    utility::Logger::getInstance().shutdown();
    return 0;
}