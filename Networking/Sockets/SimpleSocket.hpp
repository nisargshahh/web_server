#ifndef SimpleSocket_hpp
#define SimpleSocket_hpp

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>


namespace HDE{
    class SimpleSocket{
        private:
            struct sockaddr_in address;
            int sock;
            int connection;
        public:
            SimpleSocket(int domain, int service, int protocol, int port, u_long interface);
            virtual int connect_to_nw(int sock, struct sockaddr_in address) = 0;
            void test_connection(int item_to_test);
            struct sockaddr_in get_address();
            int get_sock();
            void set_connection(int con);
    };
}

#endif