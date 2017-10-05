#ifndef _ORDER_H_
#define _ORDER_H_

#include <string>
#include <cstddef>

namespace order_matcher
{

enum class OrderSide { NON_SUPPORTED, BUY, SELL };
enum class OrderType { NON_SUPPORTED, LIMIT  };

class Order
{
    public:

        Order(){}
        Order(std::string clientOrderID, std::size_t securityId, std::string owner, std::string target, OrderSide side, OrderType type, double price, long quantity);
        void execute(double price, long quantity);
        void cancel() { m_cancelled = true; }
        std::string toString() const;

        bool isFilled() const { return m_executedQuantity == m_quantity; }
        bool isPartiallyFilled() const { return (m_executedQuantity > 0 && m_executedQuantity < m_quantity) ? true : false; }
        bool isCancelled() const { return m_cancelled; }

        long getQuantity() const { return m_quantity; }
        long getOpenQuantity() const { return m_openQuantity; }
        long getExecutedQuantity() const { return m_executedQuantity; }
        double getPrice() const { return m_price; }
        const std::string& getClientID() const { return m_clientOrderID; }
        std::size_t getSecurityId() const { return m_securityId; }
        const std::string& getOwner() const { return m_owner; }
        const std::string& getTarget() const { return m_target; }
        OrderSide getSide() const { return m_side; }
        OrderType getOrderType() const { return m_orderType; }
        double getAverageExecutedPrice() const { return m_averageExecutedPrice; }
        double getLastExecutedPrice() const { return m_lastExecutedPrice; }
        long getLastExecutedQuantity() const { return m_lastExecutedQuantity; }

        friend std::ostream& operator<<(std::ostream& os, Order& entry);

    private:

        std::string m_clientOrderID;
        std::size_t m_securityId;
        std::string m_owner;
        std::string m_target;
        OrderSide m_side;
        OrderType m_orderType;
        double m_price;
        long m_quantity;

        long m_openQuantity;
        long m_executedQuantity;
        bool m_cancelled;
        double m_averageExecutedPrice;
        double m_lastExecutedPrice;
        long m_lastExecutedQuantity;
};

} // namespace

#endif