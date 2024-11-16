#ifndef ConnectingSocket_hpp
#define ConnectingSocket_hpp

#include <iostream>
#include "SimpleSocket.hpp"

namespace HDE{
    class ConnectingSocket: public SimpleSocket{
        public:
            ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface){};
            int connect_to_nw(int sock, sockaddr_in address);
    };
}

#endif