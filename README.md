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
A static file server represents one of the fundamental applications of web server technology. At its core, it maps incoming HTTP requests to a file system hierarchy and serves the requested files back to clients. The implementation requires careful consideration of several critical aspects:
Security is paramount - the server must implement proper path sanitization to prevent directory traversal attacks where malicious clients might attempt to access files outside the intended directory structure using patterns like "../". Additionally, the server needs to handle concurrent file access efficiently, potentially implementing file caching mechanisms to reduce disk I/O overhead. MIME type detection and proper content-type headers are crucial for browsers to correctly interpret and render the served content. For large files, the server should implement chunked transfer encoding or range requests to support partial content delivery and resume capabilities. Error handling must account for scenarios like missing files, permission issues, and disk I/O failures.

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
A RESTful API server implements the principles of Representational State Transfer (REST) architecture, providing a stateless, uniform interface for client-server communication. This architectural style is built around resources, which are uniquely identifiable through URIs and can be manipulated through standard HTTP methods (GET, POST, PUT, DELETE, etc.).
The implementation requires careful consideration of resource modeling, where each endpoint represents a noun rather than a verb (e.g., /api/users instead of /api/getUsers). State management must be handled entirely through the request/response cycle, with no session state maintained on the server. The server must implement proper content negotiation to support different representation formats (JSON, XML, etc.) based on client preferences. Authentication and authorization layers are typically implemented through middleware that processes requests before they reach the resource handlers. Rate limiting and request validation become critical at scale. The server should also implement proper HTTP status code usage (200 for success, 201 for creation, 404 for not found, etc.) and maintain API versioning to ensure backward compatibility as the API evolves.

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
A WebSocket server maintains persistent, full-duplex connections with clients, enabling real-time bidirectional communication - a paradigm shift from the traditional request-response model of HTTP. In a chat application, this technology enables immediate message delivery without polling.
The implementation begins with the WebSocket handshake, where an HTTP connection is upgraded to a WebSocket connection through a specific protocol exchange. The server must maintain a registry of connected clients and their associated WebSocket connections. Message broadcasting requires careful handling of connection state and potential network issues. The server needs to implement ping/pong frame exchanges to detect stale connections and handle connection cleanup appropriately. For scalability, the server might implement pub/sub patterns using message brokers like Redis or RabbitMQ. Security considerations include origin validation, message sanitization, and potential rate limiting of messages. The server should also handle different types of messages (text, binary) and implement proper frame masking as per the WebSocket protocol specification.

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
A load balancer serves as a traffic cop for distributed systems, distributing incoming requests across multiple backend servers to ensure optimal resource utilization and high availability. This critical piece of infrastructure requires sophisticated decision-making algorithms and health monitoring capabilities.
The implementation must consider various load balancing algorithms beyond simple round-robin, such as least connections, weighted round-robin, or even advanced algorithms based on server response times or custom health metrics. Health checking mechanisms must be implemented to detect and remove failing backend servers from the rotation. Session persistence might be required for applications that maintain state, implemented through techniques like IP hashing or cookie-based stickiness. The load balancer should handle connection pooling to minimize the overhead of creating new connections to backend servers. Error handling must account for backend server failures, timeout scenarios, and proper error propagation to clients. Advanced features might include SSL termination, request routing based on URL patterns, and traffic shaping capabilities.

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

## Performance Tuning

### Buffer Size Optimization
Buffer management in network programming involves careful balancing of memory usage versus performance. The choice of buffer size has significant implications:
Small buffers (4-8KB) minimize memory usage per connection but may require more system calls for large transfers. They're ideal for request-response patterns with small payloads. Large buffers (64KB-1MB) reduce system call overhead but consume more memory per connection. They're optimal for file transfers or streaming applications. Dynamic buffer sizing might be implemented based on connection patterns or server load. Memory pooling strategies can be employed to reduce allocation overhead and memory fragmentation.

```cpp
// For large file transfers
char buffer[65536] = {0};  // 64KB buffer

// For small HTTP requests
char buffer[8192] = {0};   // 8KB buffer
```

### Connection Pooling
Connection pooling optimizes resource usage by maintaining a set of reusable connections, particularly important for database or backend service communication:
The pool size must be carefully tuned based on available system resources and expected concurrency. Connection validation and refresh strategies prevent the use of stale connections. Timeout policies must be implemented for both idle connections and connection acquisition attempts. The implementation must handle connection leaks through proper tracking and cleanup mechanisms. Advanced implementations might implement priority queuing for critical requests or connection affinity for specific use cases.

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
Multi-threading in server applications enables concurrent request processing but introduces complexity in resource management and synchronization:
Thread pool sizing must balance resource usage against request latency. Work distribution strategies must consider CPU affinity and cache coherency. Synchronization mechanisms must be carefully implemented to prevent race conditions while minimizing contention. Resource sharing between threads (like connection pools or caches) must be implemented thread-safely. Advanced implementations might employ work-stealing algorithms or adaptive thread pool sizing based on server load.

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
A comprehensive logging system is crucial for debugging, audit trails, and performance analysis:
Structured logging formats enable efficient parsing and analysis. Log levels must be carefully chosen to balance information value against storage costs. Rotation and retention policies prevent disk space issues while maintaining necessary history. Sensitive information must be properly redacted or encrypted. Distributed tracing integration enables request flow visualization across services.

```cpp
void log_request(const char* buffer, int bytes_read) {
    std::ofstream log_file("server.log", std::ios::app);
    log_file << "=== " << get_timestamp() << " ===\n"
             << "Bytes Read: " << bytes_read << "\n"
             << "Request:\n" << buffer << "\n\n";
}
```

### Performance Metrics
Metrics provide quantitative insights into server behavior and performance:
Counter metrics track discrete events like request counts or error rates. Gauge metrics represent point-in-time values like active connections or memory usage. Histogram metrics capture distributions of values like response times. Metric aggregation and storage must consider both precision and resource usage. Alert thresholds should be carefully defined based on business requirements and system capabilities.

```cpp
struct ServerMetrics {
    std::atomic<uint64_t> total_requests{0};
    std::atomic<uint64_t> active_connections{0};
    std::atomic<uint64_t> bytes_transferred{0};
    std::atomic<uint64_t> error_count{0};
};
```
