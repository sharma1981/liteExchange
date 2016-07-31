#ifndef _SERVER_INTERFACE_H_
#define _SERVER_INTERFACE_H_

// Forward declarations
class Server;
enum class ServerError;

class ServerInterface
{
    public :

        explicit ServerInterface(Server& server) : m_parentServer(server)
        {}

        virtual void run() = 0;
        virtual void displayUsage() = 0;

    protected:
        Server& m_parentServer;
};

#endif