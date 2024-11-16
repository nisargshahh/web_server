#ifndef BindingSocket_hpp
#define BindingSocket_hpp

#include <iostream>
#include "SimpleSocket.hpp"

namespace HDE{
    class BindingSocket: public SimpleSocket{
        private:
            int binding;
            int connect_to_nw(int sock, sockaddr_in address);
        public:
            BindingSocket(int domain, int service, int protocol, int port, u_long interface);
            int get_binding();
    };
}

#endif