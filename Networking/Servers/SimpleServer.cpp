#include "SimpleServer.hpp"

HDE::SimpleServer::SimpleServer(int domain, int service, int protocol, int port, u_long interface, int bcklg){
    socket = new ListeningSocket(domain, service, protocol, port, interface, bcklg);
}

HDE::ListeningSocket * HDE::SimpleServer::get_socket(){
    return socket;
}