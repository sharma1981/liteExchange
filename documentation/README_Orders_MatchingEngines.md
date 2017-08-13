For limit orders please see : https://en.wikipedia.org/wiki/Order_%28exchange%29#Limit_order

Basically a limit order is an order which you specify a maximum price for the security you want to buy. 
As for the terminology a limit order to buy is called a "bid" and a limit order to sell is called an "ask".

An order matching engine matches the ask orders and the bid orders. This implementation places the highest bid order on
top of bids table and places the lowest ask order on top of asks table. Then it checks the bids and asks and tries to 
match the orders. And then it sends status reports back to the owners of the orders. A status of an order can be :

- "accepted" : the server accepted the order and it will be processed
- "filled" : the order matched
- "partially filled" , meaning that some trading happened , but still more to process
- "rejected" , if order type is not supported.
- "canceled" , if a client wants to cancel an order ,and if the order is canceled , the server informs the client

For general information about the trading systems and the order types , please see :
http://www.investopedia.com/university/intro-to-order-types/