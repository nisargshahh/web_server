#include "ConnectingSocket.hpp"

//Constructor
HDE::ConnectingSocket::ConnectingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface){
    //Network Connection
    set_connection(connect_to_nw(get_sock(), get_address()));
    test_connection(get_connection());
}

//Definition of connect to nw virtual function
int HDE::ConnectingSocket::connect_to_nw(int sock, struct sockaddr_in address){
    return connect(sock, (struct sockaddr *)&address ,sizeof(address));
}