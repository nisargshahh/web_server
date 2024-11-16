#ifndef SimpleServer_hpp
#define SimpleServer_hpp

#include <stdio.h>
#include <unistd.h>
#include "../hdelibc-networking.hpp"

namespace HDE{
    class SimpleServer{
        private:
            ListeningSocket *socket;
            virtual void acceptor() = 0;
            virtual void handler() = 0;
            virtual void responder() = 0;
        public:
            SimpleServer(int domain, int service, int protocol, int port, u_long interface, int bcklg);
            virtual void launch() = 0;
            ListeningSocket * get_socket();
    };
}


#endif // !SimpleServer_hpp
