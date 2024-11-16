#include "BindingSocket.hpp"

//Constructor
HDE::BindingSocket::BindingSocket(int domain, int service, int protocol, int port, u_long interface) : SimpleSocket(domain, service, protocol, port, interface){
    //Network Connection
    set_connection(connect_to_nw(get_sock(), get_address()));
    test_connection(binding);
}

//Definition of connect to nw virtual function
int HDE::BindingSocket::connect_to_nw(int sock, struct sockaddr_in address){
    return bind(sock, (struct sockaddr *)&address ,sizeof(address));
}

int HDE::BindingSocket::get_binding(){
    return binding;
}
