# C++ Web Server Implementation Documentation

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [Class Hierarchy](#class-hierarchy)
4. [Core Components](#core-components)
5. [Networking Concepts](#networking-concepts)
6. [Implementation Details](#implementation-details)
7. [Usage Guide](#usage-guide)
8. [Technical Reference](#technical-reference)

## Overview

This project implements a modular TCP/IP server in C++ using object-oriented principles. The server is built using a hierarchical class structure that separates concerns between socket management, network binding, connection listening, and request handling.

## Architecture

The server implementation follows a layered architecture:

```
SimpleSocket (Base Socket Operations)
    ↓
BindingSocket (Network Binding)
    ↓
ListeningSocket (Connection Listening)
    ↓
SimpleServer (Server Framework)
    ↓
TestServer (Concrete Implementation)
```

## Class Hierarchy

### 1. SimpleSocket (Base Class)
```cpp
class SimpleSocket {
    private:
        struct sockaddr_in address;  // Socket address structure
        int sock;                    // Socket file descriptor
        int connection;              // Connection status
    // ... methods
}
```

#### Key Responsibilities:
- Basic socket creation and management
- Address structure initialization
- Connection testing
- Socket descriptor management

#### Important Methods:
- `SimpleSocket(int domain, int service, int protocol, int port, u_long interface)`: Constructor that initializes the socket with network parameters
  - `domain`: Usually AF_INET for IPv4
  - `service`: SOCK_STREAM for TCP
  - `protocol`: Usually 0 for default
  - `port`: Port number for the server
  - `interface`: Network interface address

### 2. BindingSocket (Inherits from SimpleSocket)
```cpp
class BindingSocket: public SimpleSocket {
    private:
        int binding;  // Binding status
    // ... methods
}
```

#### Key Responsibilities:
- Binds the socket to a specific network address and port
- Implements the network connection logic

#### Important Methods:
- `connect_to_nw()`: Implements the binding operation using `bind()`
- `get_binding()`: Returns binding status

### 3. ListeningSocket (Inherits from BindingSocket)
```cpp
class ListeningSocket : public BindingSocket {
    private:
        int backlog;    // Maximum connection queue length
        int listening;  // Listening status
    // ... methods
}
```

#### Key Responsibilities:
- Configures socket to listen for incoming connections
- Manages connection backlog

#### Important Methods:
- `start_listening()`: Initiates listening on the socket using `listen()`
- `get_backlog()`: Returns maximum queue length
- `get_listening()`: Returns listening status

### 4. SimpleServer (Abstract Base Class)
```cpp
class SimpleServer {
    private:
        ListeningSocket *socket;
        virtual void acceptor() = 0;
        virtual void handler() = 0;
        virtual void responder() = 0;
    // ... methods
}
```

#### Key Responsibilities:
- Defines server framework
- Manages server lifecycle
- Provides interface for concrete implementations

#### Important Methods:
- Pure virtual methods that must be implemented by derived classes:
  - `acceptor()`: Accepts new connections
  - `handler()`: Processes requests
  - `responder()`: Sends responses
  - `launch()`: Main server loop

### 5. TestServer (Concrete Implementation)
```cpp
class TestServer : public SimpleServer {
    private:
        char buffer[30000];  // Request buffer
        int new_socket;      // Accepted connection socket
    // ... methods
}
```

#### Key Responsibilities:
- Implements concrete server behavior
- Handles HTTP requests and responses
- Manages connection lifecycle

## Networking Concepts

### Socket Programming Fundamentals

1. **Socket Creation**
   ```cpp
   sock = socket(domain, service, protocol);
   ```
   - `domain`: Address family (AF_INET for IPv4)
   - `service`: Socket type (SOCK_STREAM for TCP)
   - `protocol`: Protocol (0 for default)

2. **Network Byte Order**
   ```cpp
   address.sin_port = htons(port);  // Host to Network Short
   address.sin_addr.s_addr = htonl(interface);  // Host to Network Long
   ```
   - Ensures consistent byte ordering across different architectures

3. **Address Structure**
   ```cpp
   struct sockaddr_in {
       sa_family_t sin_family;     // Address family
       in_port_t sin_port;         // Port number
       struct in_addr sin_addr;    // IPv4 address
   };
   ```

### Connection Lifecycle

1. **Binding**
   - Associates socket with specific network interface and port
   - Implemented in `BindingSocket::connect_to_nw()`

2. **Listening**
   - Marks socket as passive and ready to accept connections
   - Configures connection backlog
   - Implemented in `ListeningSocket::start_listening()`

3. **Accepting Connections**
   ```cpp
   new_socket = accept(get_socket()->get_sock(), 
                      (struct sockaddr *)&address, 
                      (socklen_t *)&addr_len);
   ```
   - Creates new socket for each accepted connection

4. **Data Exchange**
   - Reading requests using `recv()`
   - Sending responses using `send()`
   - Proper error handling and buffer management

## Implementation Details

### Error Handling
```cpp
void SimpleSocket::test_connection(int item_to_test) {
    if (item_to_test < 0) {
        perror("Failed to connect!");
        exit(EXIT_FAILURE);
    }
}
```
- Systematic error checking throughout the codebase
- Use of `errno` and `strerror()` for detailed error messages

### Buffer Management
```cpp
char buffer[30000] = {0};  // Zero-initialized buffer
memset(buffer, 0, sizeof(buffer));  // Clear buffer before reuse
```
- Fixed-size buffer with proper initialization
- Clearing before each use to prevent data leakage

### Timeout Handling
```cpp
struct timeval tv;
tv.tv_sec = 5;  // 5 seconds timeout
tv.tv_usec = 0;
setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, 
           (const char*)&tv, sizeof tv);
```
- Prevents hanging on slow or dead connections

## Usage Guide

### Basic Server Setup
```cpp
// Create instance of TestServer
HDE::TestServer server;  // Automatically starts on port 3000
```

### HTTP Response Format
```cpp
const char* response = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/plain\r\n"
                      "Connection: close\r\n"
                      "Content-Length: 19\r\n"
                      "\r\n"
                      "Hello from Server!\r\n";
```

### Compilation
```bash
g++ Servers/test.cpp Servers/TestServer.cpp Servers/SimpleServer.cpp \
    Sockets/BindingSocket.cpp Sockets/SimpleSocket.cpp \
    Sockets/ListeningSocket.cpp hdelibc-networking.hpp \
    -o web_server.exe
```

## Technical Reference

### Important Header Files
- `<sys/socket.h>`: Core socket functionality
- `<netinet/in.h>`: Internet address family
- `<unistd.h>`: POSIX operating system API
- `<errno.h>`: System error numbers

### Key Socket Options
- `SO_RCVTIMEO`: Receive timeout
- `SHUT_RDWR`: Full-duplex shutdown

### Common Error Scenarios
1. Address already in use
2. Permission denied (ports < 1024)
3. Connection reset by peer
4. Broken pipe

This server implementation provides a foundation for building more complex networking applications. The modular design allows for easy extension and modification of behavior by creating new classes that inherit from SimpleServer.

[Previous documentation remains the same...]

# Detailed TestServer Implementation Guide

## TestServer.cpp Breakdown

### Constructor Initialization
```cpp
HDE::TestServer::TestServer() : SimpleServer(AF_INET, SOCK_STREAM, 0, 3000, INADDR_ANY, 10) {
    memset(buffer, 0, sizeof(buffer));
    launch();
}
```

#### Parameters Explained:
- `AF_INET`: IPv4 Internet protocols
- `SOCK_STREAM`: TCP connection-based stream
- `0`: Default protocol
- `3000`: Port number
- `INADDR_ANY`: Bind to all available network interfaces
- `10`: Connection backlog size (max pending connections)

### Acceptor Method
```cpp
void HDE::TestServer::acceptor() {
    struct sockaddr_in address = get_socket()->get_address();
    int addr_len = sizeof(address);
    
    std::cout << "Attempting to accept connection..." << std::endl;
    new_socket = accept(get_socket()->get_sock(), (struct sockaddr *)&address, 
                       (socklen_t *)&addr_len);
```

The acceptor method handles incoming connections:
1. Gets server address structure
2. Accepts new connection
3. Sets up timeout
4. Reads client data

#### Timeout Configuration
```cpp
struct timeval tv;
tv.tv_sec = 5;  // 5 seconds timeout
tv.tv_usec = 0;
setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
```
- Prevents server from hanging on slow connections
- Critical for handling dropped connections
- Can be adjusted based on needs (e.g., longer for file uploads)

#### Data Reading
```cpp
ssize_t bytes_read = recv(new_socket, buffer, 30000, 0);
```
- Uses `recv()` for TCP-specific features
- Buffer size of 30KB (suitable for basic HTTP requests)
- Returns number of bytes read or -1 on error

### Handler Method
```cpp
void HDE::TestServer::handler() {
    if (strlen(buffer) > 0) {
        std::cout << "\nReceived Request (" << strlen(buffer) << " bytes):" << std::endl;
        std::cout << "--- Begin Request ---" << std::endl;
        std::cout << buffer << std::endl;
        std::cout << "--- End Request ---" << std::endl;
    }
}
```
- Processes received HTTP request
- Currently implements logging functionality
- Extensible for request parsing and routing

### Responder Method
```cpp
void HDE::TestServer::responder() {
    const char* response = "HTTP/1.1 200 OK\r\n"
                          "Content-Type: text/plain\r\n"
                          "Connection: close\r\n"
                          "Content-Length: 19\r\n"
                          "\r\n"
                          "Hello from Server!\r\n";
```
- Constructs and sends HTTP response
- Implements proper HTTP headers
- Handles connection cleanup

## Real-World Usage Examples

### 1. Basic Static File Server
```cpp
class FileServer : public HDE::SimpleServer {
private:
    std::string root_dir;
    
    void handler() override {
        // Parse HTTP request to get file path
        std::string request(buffer);
        std::string path = parse_request_path(request);
        
        // Read file content
        std::string file_path = root_dir + path;
        std::string content = read_file(file_path);
        
        // Send file content as response
        std::string response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: " + get_mime_type(file_path) + "\r\n"
                             "Content-Length: " + std::to_string(content.length()) + "\r\n"
                             "\r\n" + content;
        send(new_socket, response.c_str(), response.length(), 0);
    }
};

// Usage:
FileServer server("/var/www/html");
```

### 2. RESTful API Server
```cpp
class APIServer : public HDE::SimpleServer {
private:
    std::map<std::string, std::function<void(Request&, Response&)>> routes;
    
    void handler() override {
        Request req(buffer);
        Response res;
        
        // Route handling
        std::string path = req.get_path();
        if (routes.find(path) != routes.end()) {
            routes[path](req, res);
        } else {
            res.set_status(404);
            res.set_body("Not Found");
        }
        
        send(new_socket, res.to_string().c_str(), res.to_string().length(), 0);
    }
public:
    void add_route(std::string path, std::function<void(Request&, Response&)> handler) {
        routes[path] = handler;
    }
};

// Usage:
APIServer api_server;
api_server.add_route("/api/users", [](Request& req, Response& res) {
    res.set_content_type("application/json");
    res.set_body("{\"users\": [\"john\", \"jane\"]}");
});
```

### 3. WebSocket Chat Server
```cpp
class ChatServer : public HDE::SimpleServer {
private:
    std::vector<int> clients;
    
    void handler() override {
        // WebSocket handshake
        if (is_new_connection(buffer)) {
            handle_websocket_handshake();
            clients.push_back(new_socket);
        } else {
            // Broadcast message to all clients
            std::string message = decode_websocket_frame(buffer);
            broadcast_message(message);
        }
    }
    
    void broadcast_message(const std::string& message) {
        for (int client : clients) {
            send_websocket_frame(client, message);
        }
    }
};

// Usage:
ChatServer chat;
// Connect using WebSocket client:
// ws://localhost:3000
```

### 4. Load Balancer
```cpp
class LoadBalancer : public HDE::SimpleServer {
private:
    std::vector<std::string> backend_servers;
    size_t current_server;
    
    void handler() override {
        // Round-robin selection of backend server
        std::string target_server = backend_servers[current_server];
        current_server = (current_server + 1) % backend_servers.size();
        
        // Forward request to selected server
        forward_request(target_server);
    }
};

// Usage:
LoadBalancer lb;
lb.add_backend("localhost:8081");
lb.add_backend("localhost:8082");
```

### 5. Rate-Limited API Gateway
```cpp
class RateLimitedServer : public HDE::SimpleServer {
private:
    struct ClientState {
        int requests;
        time_t window_start;
    };
    std::map<std::string, ClientState> clients;
    
    void handler() override {
        std::string client_ip = get_client_ip();
        
        if (is_rate_limited(client_ip)) {
            send_response(429, "Too Many Requests");
            return;
        }
        
        // Process request normally
        process_request();
    }
    
    bool is_rate_limited(const std::string& ip) {
        auto& state = clients[ip];
        time_t now = time(nullptr);
        
        if (now - state.window_start >= 60) {
            // Reset window
            state.requests = 1;
            state.window_start = now;
            return false;
        }
        
        return ++state.requests > 100; // 100 requests per minute
    }
};

// Usage:
RateLimitedServer api_gateway;
```

## Performance Tuning

### Buffer Size Optimization
```cpp
// For large file transfers
char buffer[65536] = {0};  // 64KB buffer

// For small HTTP requests
char buffer[8192] = {0};   // 8KB buffer
```

### Connection Pooling
```cpp
class PooledServer : public HDE::SimpleServer {
private:
    std::queue<int> connection_pool;
    static const int POOL_SIZE = 10;
    
    void initialize_pool() {
        for (int i = 0; i < POOL_SIZE; i++) {
            connection_pool.push(create_connection());
        }
    }
    
    int get_connection() {
        if (connection_pool.empty()) {
            return create_connection();
        }
        int conn = connection_pool.front();
        connection_pool.pop();
        return conn;
    }
};
```

### Multi-threading Support
```cpp
class ThreadedServer : public HDE::SimpleServer {
private:
    std::vector<std::thread> worker_threads;
    
    void launch() override {
        while (true) {
            int client_socket = accept_connection();
            worker_threads.emplace_back([this, client_socket]() {
                handle_client(client_socket);
            });
        }
    }
};
```

## Monitoring and Debugging

### Request Logging
```cpp
void log_request(const char* buffer, int bytes_read) {
    std::ofstream log_file("server.log", std::ios::app);
    log_file << "=== " << get_timestamp() << " ===\n"
             << "Bytes Read: " << bytes_read << "\n"
             << "Request:\n" << buffer << "\n\n";
}
```

### Performance Metrics
```cpp
struct ServerMetrics {
    std::atomic<uint64_t> total_requests{0};
    std::atomic<uint64_t> active_connections{0};
    std::atomic<uint64_t> bytes_transferred{0};
    std::atomic<uint64_t> error_count{0};
};
```
