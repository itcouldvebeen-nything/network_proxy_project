# Network Proxy Server

A lightweight, multi-threaded HTTP proxy server written in C++ for Windows. This proxy server forwards HTTP requests from clients to remote servers while providing domain filtering capabilities and comprehensive request logging.

## Features

- **HTTP Proxy Server**: Forwards HTTP requests between clients and remote servers
- **Domain Filtering**: Blocks requests to specified domains using a configurable blocklist
- **Request Logging**: Logs all proxy activity to console and file (`proxy.log`)
- **Multi-threaded**: Handles multiple concurrent client connections efficiently
- **Thread-safe**: Uses mutexes to ensure safe concurrent access to shared resources
- **Graceful Shutdown**: Supports Ctrl+C for clean server termination

## Why This Project is Useful

This proxy server is useful for:

- **Network Monitoring**: Track and log all HTTP traffic passing through the proxy
- **Content Filtering**: Block access to specific domains or websites
- **Educational Purposes**: Learn about network programming, socket programming, and HTTP protocol handling
- **Development Testing**: Test applications with proxy configurations
- **Traffic Analysis**: Analyze HTTP request patterns and behaviors

## Prerequisites

- **Windows** (uses WinSock2 API)
- **CMake** 3.10 or higher
- **C++ Compiler** with C++11 support (Visual Studio recommended)
- **Build Tools**: Visual Studio or MinGW with CMake support

## Installation

### Building from Source

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd network-proxy
   ```

2. **Create a build directory**:
   ```bash
   mkdir build
   cd build
   ```

3. **Generate build files with CMake**:
   ```bash
   cmake ..
   ```

4. **Build the project**:
   ```bash
   cmake --build . --config Release
   ```

   Or use Visual Studio:
   ```bash
   cmake --build . --config Release
   ```

5. **The executable** will be located at `build/Release/proxy_exe.exe`

## Getting Started

### Basic Usage

1. **Create a blocklist file** (optional):
   Create a file named `blocked.txt` in the same directory as the executable with one domain per line:
   ```
   example.com
   www.example.com
   badsite.org
   doubleclick.net
   ```

2. **Run the proxy server**:
   ```bash
   ./build/Release/proxy_exe.exe
   ```

   The server will start listening on port **8888**:
   ```
   ============================================
     PROXY SERVER STARTING ON PORT 8888        
     Press Ctrl+C to stop                      
   ============================================
   ```

3. **Configure your application** to use the proxy:
   - Proxy address: `localhost` or `127.0.0.1`
   - Proxy port: `8888`

### Example: Using curl with the Proxy

```bash
curl -x http://localhost:8888 http://www.google.com
```

### Example: Testing Concurrent Requests

A test script is provided (`test_proxy.bat`) that sends 20 concurrent requests:
```bash
test_proxy.bat
```

### Configuration

- **Port**: The proxy listens on port `8888` by default. To change this, modify `src/main.cpp` line 39.
- **Blocklist**: The proxy loads domains from `blocked.txt` in the current working directory. Create this file to block specific domains.
- **Timeouts**: 
  - Client socket timeout: 7 seconds (line 47 in `src/ProxyCore.cpp`)
  - Remote socket timeout: 10 seconds (line 96 in `src/ProxyCore.cpp`)

## Project Structure

```
network-proxy/
├── src/              # Source files
│   ├── main.cpp      # Entry point and server initialization
│   ├── ProxyCore.cpp # Core proxy logic and client handling
│   ├── Parser.cpp    # HTTP request parsing
│   ├── Filter.cpp    # Domain filtering logic
│   └── Logger.cpp    # Request logging
├── include/          # Header files
│   ├── Common.h      # Common definitions and structures
│   ├── ProxyCore.h   # Proxy core declarations
│   ├── Parser.h      # Parser declarations
│   ├── Filter.h      # Filter declarations
│   └── Logger.h      # Logger declarations
├── build/            # Build output directory
├── config/           # Configuration files directory
├── docs/             # Documentation directory
├── CMakeLists.txt    # CMake build configuration
└── test_proxy.bat    # Test script for concurrent requests
```

## How It Works

1. **Server Initialization**: The server creates a listening socket on port 8888 and loads the domain blocklist from `blocked.txt`.

2. **Client Connection**: When a client connects, a new thread is spawned to handle the request.

3. **Request Parsing**: The proxy parses the HTTP request to extract:
   - HTTP method (GET, POST, etc.)
   - Target host and port
   - Request path
   - HTTP version

4. **Domain Filtering**: The proxy checks if the requested host is in the blocklist. If blocked, it returns a 403 Forbidden response.

5. **Request Forwarding**: If allowed, the proxy:
   - Establishes a connection to the remote server
   - Modifies the request (adds `Connection: close` header)
   - Forwards the request to the remote server
   - Relays the response back to the client

6. **Logging**: All requests are logged to both console and `proxy.log` file with details including:
   - Client IP address
   - Requested host and port
   - HTTP method and path
   - Status (ALLOWED/BLOCKED)
   - Bytes transferred

## Logging

The proxy logs all requests in two places:

1. **Console Output**: Real-time logging with status messages:
   - `[FORWARD]` - Request being forwarded
   - `[BLOCKED]` - Request blocked by filter
   - `[DONE]` - Request completed

2. **File Logging**: Detailed logs written to `proxy.log` in CSV format:
   ```
   IP,Host,Port,Method,Status,Bytes
   127.0.0.1,www.google.com,80,GET,ALLOWED,12345
   ```

## Limitations

- **Windows Only**: Uses WinSock2 API, not cross-platform
- **HTTP Only**: Does not support HTTPS/SSL tunneling
- **No Authentication**: No proxy authentication support
- **No Caching**: Does not cache responses
- **Connection: close**: Forces connection closure (no keep-alive)

## Troubleshooting

### Port Already in Use
If you see "Bind failed. Is port 8888 already in use?", either:
- Stop the process using port 8888
- Change the port in `src/main.cpp`

### Blocked Domains Not Working
- Ensure `blocked.txt` exists in the same directory as the executable
- Check that domains are listed one per line
- Verify the file is readable (no permission issues)

### No Logs Appearing
- Check that the executable has write permissions in the current directory
- Verify `proxy.log` is not locked by another process

## Getting Help

- **Issues**: Open an issue on the repository for bug reports or feature requests
- **Documentation**: Check the `docs/` directory for additional documentation (if available)
- **Code Review**: Review the source code in `src/` and `include/` directories for implementation details

## Contributing

Contributions are welcome! When contributing:

1. Follow the existing code style and structure
2. Ensure thread-safety when modifying shared resources
3. Test your changes with multiple concurrent connections
4. Update relevant documentation

For detailed contribution guidelines, see [CONTRIBUTING.md](CONTRIBUTING.md) (if available).

---

**Note**: This proxy server is designed for educational and development purposes. Use responsibly and in accordance with applicable laws and regulations.

