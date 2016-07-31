#ifndef _SERVER_INTERFACE_CLI_H_
#define _SERVER_INTERFACE_CLI_H_

#include <server/server_interface.h>
#include <server/server.h>
#include <server/server_error.h>

#include <utility/logger/logger.h>

#include <iostream>
#include <string>
#include <algorithm>

class ServerInterfaceCli : public ServerInterface
{
    public :

        explicit ServerInterfaceCli(Server& server) : ServerInterface{ server }
        {}

        virtual void run() override
        {
            displayUsage();

            while (true)
            {
                std::string value;
                std::cin >> value;

                std::transform(value.begin(), value.end(), value.begin(), ::tolower);

                if (value == "display")
                {
                    LOG_CONSOLE("FIX Engine", "All orders in the central order book :")
                    LOG_CONSOLE("FIX Engine", "")
                    LOG_CONSOLE("FIX Engine", m_parentServer.getAllOrderBookAsString())
                }
                else if (value == "quit")
                {
                    LOG_INFO("FIX Engine", "Quit message received")
                    break;
                }
                else
                {
                    LOG_CONSOLE("FIX Engine", "Invalid user command")
                    displayUsage();
                }

                std::cout << std::endl;
            }
        }

        virtual void displayUsage() override
        {
            LOG_CONSOLE("FIX Engine", "")
            LOG_CONSOLE("FIX Engine", "Available commands :")
            LOG_CONSOLE("FIX Engine", "")
            LOG_CONSOLE("FIX Engine", "\tdisplay : Shows all order books in the central order book")
            LOG_CONSOLE("FIX Engine", "\tquit : Shutdowns the server")
            LOG_CONSOLE("FIX Engine", "")
        }
};

#endif