#ifndef _CLI_H_
#define _CLI_H_

#include <string>
#include <iostream>

#include <core/string_utility.h>

#include <order_matcher/central_order_book.h>
#include <order_matcher/central_order_book_visitor.h>


class CommandLineInterface
{
    public :

        CommandLineInterface() : m_parentCentralOrderBook{ nullptr }
        {

        }

        void setParentCentralOrderbook(order_matcher::CentralOrderBook* parent)
        {
            m_parentCentralOrderBook = parent;
        }

        void run()
        {
            displayUsage();
            while (true)
            {
                std::string value;
                std::cin >> value;

                value = core::toLower(value);

                if (value == "display")
                {
                    outputMessage("All orders in the central order book :");
                    outputMessage("");
                    outputMessage(getAllOrderBookAsString());
                }
                else if (value == "quit")
                {
                    outputMessage("Quit message received");
                    break;
                }
                else
                {
                    outputMessage("Invalid user command");
                    displayUsage();
                }

                std::cout << std::endl;
            }
        }

    private :

        order_matcher::CentralOrderBook* m_parentCentralOrderBook;


        std::string getAllOrderBookAsString()
        {
            order_matcher::CentralOrderBookVisitor visitor;
            m_parentCentralOrderBook->accept(visitor);
            return visitor.toString();
        }

        void displayUsage()
        {
            outputMessage("");
            outputMessage("Available commands :");
            outputMessage("");
            outputMessage("\tdisplay : Shows all order books in the central order book");
            outputMessage("\tquit : Shutdowns the server");
            outputMessage("");
        }

        void outputMessage(const std::string& message)
        {
            std::cout << message << std::endl;
        }
};

#endif