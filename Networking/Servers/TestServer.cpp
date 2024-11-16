#include "TestServer.hpp"
#include <cstring>
#include <unistd.h>
#include <errno.h>

HDE::TestServer::TestServer() : SimpleServer(AF_INET, SOCK_STREAM, 0, 3000, INADDR_ANY, 10) {
    memset(buffer, 0, sizeof(buffer));
    launch();
}

void HDE::TestServer::acceptor() {
    struct sockaddr_in address = get_socket()->get_address();
    int addr_len = sizeof(address);
    
    std::cout << "Attempting to accept connection..." << std::endl;
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, (socklen_t *)&addr_len);
    
    if (new_socket < 0) {
        std::cerr << "Accept failed with error: " << strerror(errno) << std::endl;
        return;
    }
    
    std::cout << "Connection accepted on socket: " << new_socket << std::endl;
    
    // Clear buffer before each read
    memset(buffer, 0, sizeof(buffer));
    
    // Set a receive timeout
    struct timeval tv;
    tv.tv_sec = 5;  // 5 seconds timeout
    tv.tv_usec = 0;
    setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    
    // Read with detailed error checking
    std::cout << "Reading from socket..." << std::endl;
    ssize_t bytes_read = recv(new_socket, buffer, 30000, 0);  // Using recv instead of read
    
    if (bytes_read < 0) {
        std::cerr << "Read failed with error: " << strerror(errno) << std::endl;
        close(new_socket);
        return;
    } else if (bytes_read == 0) {
        std::cerr << "Client closed connection" << std::endl;
        close(new_socket);
        return;
    }
    
    std::cout << "Bytes read: " << bytes_read << std::endl;
    buffer[bytes_read] = '\0';
    
    // Print the raw bytes for debugging
    std::cout << "Raw received data (hex):" << std::endl;
    for(int i = 0; i < bytes_read && i < 50; i++) {  // Print first 50 bytes
        printf("%02x ", (unsigned char)buffer[i]);
    }
    std::cout << std::endl;
}

void HDE::TestServer::handler() {
    if (strlen(buffer) > 0) {
        std::cout << "\nReceived Request (" << strlen(buffer) << " bytes):" << std::endl;
        std::cout << "--- Begin Request ---" << std::endl;
        std::cout << buffer << std::endl;
        std::cout << "--- End Request ---" << std::endl;
    } else {
        std::cout << "Empty request received" << std::endl;
    }
}

void HDE::TestServer::responder() {
    const char* response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Connection: close\r\n"
                          "Content-Length: 19\r\n"
                          "\r\n"
                          "Hello from Server!\r\n";
    
    ssize_t bytes_sent = send(new_socket, response, strlen(response), 0);  // Using send instead of write
    if (bytes_sent < 0) {
        std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
    } else {
        std::cout << "Sent " << bytes_sent << " bytes as response" << std::endl;
    }
    
    // Properly shutdown the socket
    shutdown(new_socket, SHUT_RDWR);
    close(new_socket);
}

void HDE::TestServer::launch() {
    while(true) {
        std::cout << "\n=== Waiting for new connection ====" << std::endl;
        acceptor();
        handler();
        responder();
        std::cout << "=== Connection handled ====" << std::endl;
    }
}