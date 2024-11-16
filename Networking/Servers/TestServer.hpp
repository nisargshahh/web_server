#ifndef TestServer_hpp
#define TestServer_hpp

#include <iostream>
#include <stdio.h>
#include <cstring>
#include "SimpleServer.hpp"

namespace HDE{
    class TestServer : public SimpleServer{
        private:
            char buffer[30000] = {0};
            int new_socket;
            void acceptor();
            void handler();
            void responder();
        public:
            TestServer();
            void launch();
    };
}

#endif
