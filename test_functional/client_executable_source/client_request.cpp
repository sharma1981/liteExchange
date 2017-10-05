#include <exception>
#include <iterator>
#include <core/pretty_exception.h>
#include <core/string_utility.h>
#include "client_request.h"
using namespace std;

ClientRequest::ClientRequest(const string& csvRequest)
{
    m_requestSent.store(false);

    auto tokens = core::split(csvRequest, ',');
    auto numTokens = tokens.size();

    if (numTokens < 5)
    {
        auto exceptionMessage = core::format("Invalid number of tokens in line : %s", csvRequest);
        THROW_PRETTY_INVALID_ARG_EXCEPTION(exceptionMessage)
    }

    // ORDER TYPE COLUMN
    string orderType = tokens[0];

    if (orderType.compare("NEW_ORDER") == 0)
    {
        m_type = ClientRequestType::NEW_ORDER;

        if (numTokens != 6)
        {
            auto exceptionMessage = core::format("Invalid number of tokens for a new order in line : %s", csvRequest);
            THROW_PRETTY_INVALID_ARG_EXCEPTION(exceptionMessage)
        }
    }
    else if (orderType.compare("CANCEL_ORDER") == 0)
    {
        m_type = ClientRequestType::CANCEL_ORDER;

        if (numTokens != 5)
        {
            auto exceptionMessage = core::format("Invalid number of tokens for a cancel order in line : %s", csvRequest);
            THROW_PRETTY_INVALID_ARG_EXCEPTION(exceptionMessage)
        }
    }
    else
    {
        THROW_PRETTY_INVALID_ARG_EXCEPTION("Invalid order type")
    }

    // SYMBOL COLUMN
    string temp(tokens[1]);
    m_symbol = temp;

    // SIDE COLUMN
    string orderSide = tokens[2];

    if (orderSide.compare("BUY") == 0)
    {
        m_side = FIX::Side_BUY;
    }
    else if (orderSide.compare("SELL") == 0)
    {
        m_side = FIX::Side_SELL;
    }
    else
    {
        THROW_PRETTY_INVALID_ARG_EXCEPTION("Invalid side")
    }

    // TARGET ID
    m_targetID = tokens[3];

    // ORIG ORDER ID , IF CANCEL ORDER
    if (m_type == ClientRequestType::CANCEL_ORDER)
    {
        m_origOrderID = tokens[4];
    }
    else // PRICE AND QUANTITY , IF NEW ORDER
    {

        m_price = std::stod(tokens[4]);
        m_quantity = std::stol(tokens[5]);
    }
}

ClientRequest::ClientRequest(const ClientRequest& other)
{
    this->m_requestSent.store( other.requestSent() );
    this->m_orderID = other.getOrderID();
    this->m_origOrderID = other.getOrigOrderID();
    this->m_side = other.getSide();
    this->m_type = other.getType();
    this->m_price = other.getPrice();
    this->m_quantity = other.getQuantity();
    this->m_senderID = other.getSenderID();
    this->m_targetID = other.getTargetID();
}