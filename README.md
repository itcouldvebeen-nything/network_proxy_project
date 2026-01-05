# Network Proxy Server

[![Version](https://img.shields.io/badge/version-1.0-blue.svg)](CMakeLists.txt)
[![C++ Standard](https://img.shields.io/badge/C++-17-orange.svg)](CMakeLists.txt)
[![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)](CMakeLists.txt)
[![CMake](https://img.shields.io/badge/CMake-3.10+-green.svg)](CMakeLists.txt)

A lightweight, multi-threaded HTTP proxy server written in C++ for Windows. This proxy server forwards HTTP requests from clients to remote servers while providing domain filtering capabilities and comprehensive request logging.

## Table of Contents

- [What the Project Does](#what-the-project-does)
- [Why the Project is Useful](#why-the-project-is-useful)
- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Getting Started](#getting-started)
- [Configuration](#configuration)
- [Usage Examples](#usage-examples)
- [Project Structure](#project-structure)
- [How It Works](#how-it-works)
- [Documentation](#documentation)
- [Getting Help](#getting-help)
- [Contributing](#contributing)
- [Limitations](#limitations)

## What the Project Does

The Network Proxy Server is an HTTP proxy that:

- **Intercepts HTTP requests** from client applications
- **Parses and validates** incoming HTTP requests
- **Filters domains** based on a configurable blocklist
- **Forwards allowed requests** to remote servers
- **Streams responses** back to clients
- **Logs all activity** to console and file for monitoring and analysis

The proxy uses a **thread-per-connection** architecture, handling multiple concurrent client connections efficiently with thread-safe operations.

## Why the Project is Useful

This proxy server is useful for:

- **Network Monitoring**: Track and log all HTTP traffic passing through the proxy
- **Content Filtering**: Block access to specific domains or websites (e.g., ads, trackers, malicious sites)
- **Educational Purposes**: Learn about network programming, socket programming, HTTP protocol handling, and concurrent systems
- **Development Testing**: Test applications with proxy configurations and analyze request/response patterns
- **Traffic Analysis**: Analyze HTTP request patterns, methods, and behaviors in your network

## Features

- ✅ **HTTP Proxy Server**: Full HTTP request/response forwarding
- ✅ **HTTPS Tunneling**: CONNECT method support for HTTPS traffic tunneling
- ✅ **Domain Filtering**: Configurable blocklist with subdomain matching support
- ✅ **Request Logging**: Comprehensive logging to console and CSV file
- ✅ **Multi-threaded**: Thread-per-connection model for concurrent request handling
- ✅ **Thread-safe**: Mutex-based synchronization for shared resources
- ✅ **Configurable**: Port, blocklist path, and log path via configuration file
- ✅ **Graceful Shutdown**: Clean termination with Ctrl+C signal handling
- ✅ **Error Handling**: Proper error responses (403 Forbidden, 502 Bad Gateway)

## Prerequisites

- **Windows** (uses WinSock2 API)
- **CMake** 3.10 or higher
- **C++ Compiler** with C++17 support (Visual Studio 2017+ recommended)
- **Build Tools**: Visual Studio or MinGW with CMake support

## Installation

### Building from Source

1. **Clone the repository**:
   ```bash
   git clone https://github.com/itcouldvebeen-nything/network_proxy_project
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

   The executable will be located at `build/Release/proxy_exe.exe`

### Quick Build (Visual Studio)

If you have Visual Studio installed, you can also open the generated solution:

```bash
cmake ..
cmake --open .
```

Then build the `proxy_exe` project in Visual Studio.

## Getting Started

### 1. Prepare Configuration Files

Create a `config` directory in your build output directory (or where you'll run the executable):

```
config/
├── server.cfg
└── blocked.txt
```

**`config/server.cfg`** (optional - defaults will be used if missing):
```ini
PORT=8888
FILTER_PATH=config/blocked.txt
LOG_PATH=logs/proxy.log
MAX_HEADER_SIZE=8192
```

**`config/blocked.txt`** (one domain per line):
```
example.com
www.example.com
badsite.org
doubleclick.net
ads.tracker.com
```

### 2. Run the Proxy Server

```bash
./build/Release/proxy_exe.exe
```

You should see:
```
============================================================
         CUSTOM NETWORK PROXY SERVER v1.0
============================================================
 [SYSTEM] Winsock Initialized ... OK
 [FS]     Log directory found ... OK
 [CONFIG] Port: 8888
 [FILTER] Logic operational.
 [STATUS] Proxy is listening on 0.0.0.0:8888
------------------------------------------------------------
 [READY]  Waiting for client connections...
 [HINT]   Press Ctrl+C to terminate the server.
============================================================
```

### 3. Configure Your Application

Configure your HTTP client to use the proxy:
- **Proxy address**: `localhost` or `127.0.0.1`
- **Proxy port**: `8888`

## Configuration

The proxy server reads configuration from `config/server.cfg` (relative to the executable's working directory):

| Setting | Default | Description |
|---------|---------|-------------|
| `PORT` | `8888` | Port number the proxy listens on |
| `FILTER_PATH` | `config/blocked.txt` | Path to domain blocklist file |
| `LOG_PATH` | `logs/proxy.log` | Path to log file |
| `MAX_HEADER_SIZE` | `8192` | Maximum HTTP header size in bytes |

If the configuration file is missing, the proxy will use defaults and print a warning.

## Usage Examples

### Using curl

```bash
# Basic HTTP request through proxy
curl -x http://localhost:8888 http://www.google.com

# HTTPS request (uses CONNECT method for tunneling)
curl -x http://localhost:8888 https://www.google.com

# Check if a domain is blocked
curl -x http://localhost:8888 http://example.com
# Returns: HTTP/1.1 403 Forbidden
```

### Using PowerShell

```powershell
# Configure proxy for web requests
$proxy = "http://localhost:8888"
$env:HTTP_PROXY = $proxy
$env:HTTPS_PROXY = $proxy

# Make a request
Invoke-WebRequest -Uri "http://www.example.com"
```

### Testing with the Provided Script

A test script is included (`test_proxy.bat`) that performs multiple test scenarios:

```bash
test_proxy.bat
```

This script tests:
- HTTP GET forwarding
- Domain filtering
- Malformed request handling
- Concurrent request handling

## Project Structure

```
network-proxy/
├── src/                  # Source files
│   ├── main.cpp         # Entry point and server initialization
│   ├── ProxyCore.cpp    # Core proxy logic and client handling
│   ├── Parser.cpp       # HTTP request parsing
│   ├── Filter.cpp       # Domain filtering logic
│   ├── Logger.cpp       # Request logging
│   └── Config.cpp       # Configuration file parsing
├── include/             # Header files
│   ├── Common.h         # Common definitions and structures
│   ├── ProxyCore.h      # Proxy core declarations
│   ├── Parser.h         # Parser declarations
│   ├── Filter.h         # Filter declarations
│   ├── Logger.h         # Logger declarations
│   └── Config.h         # Configuration class
├── config/              # Configuration files (create this)
│   ├── server.cfg       # Server configuration
│   └── blocked.txt      # Domain blocklist
├── docs/                # Documentation
│   └── design.md        # System design and architecture
├── logs/                # Log files (auto-created)
├── build/               # Build output directory
├── CMakeLists.txt       # CMake build configuration
├── test_proxy.bat       # Test script
└── README.md            # This file
```

## How It Works

The proxy follows a **layered request-handling architecture**:

1. **Connection Acceptance**: The server listens on the configured port and accepts incoming client connections
2. **Thread Spawning**: Each connection spawns a new detached thread for concurrent handling
3. **Request Parsing**: HTTP headers are parsed to extract method, host, port, and path
4. **Domain Filtering**: The requested hostname is checked against the blocklist (exact and subdomain matching)
5. **Protocol Dispatch**: Based on the HTTP method:
   - **CONNECT method (HTTPS)**: Establishes a bidirectional tunnel between client and remote server
   - **Standard HTTP methods**: Forwards the request with modified headers
6. **Request Forwarding**: If allowed, the proxy:
   - Establishes a TCP connection to the remote server
   - For CONNECT: Creates bidirectional relay (two threads for client↔remote data flow)
   - For HTTP: Modifies the request (adds `Connection: close` header) and streams response
7. **Logging**: All requests are logged with metadata (IP, host, method, status, bytes)

For detailed architecture information, see [docs/design.md](docs/design.md).

### Logging

The proxy logs all requests in two places:

**Console Output** (real-time):
```
[FORWARD] 127.0.0.1 -> www.google.com:80 GET /
[DONE]    127.0.0.1 -> www.google.com:80 (12345 bytes)
[BLOCKED] 127.0.0.1 -> example.com:80 GET /
```

**File Logging** (`logs/proxy.log` - CSV format):
```csv
IP,Host,Port,Method,Path,Status,Bytes
127.0.0.1,www.google.com,80,GET,/,ALLOWED,12345
127.0.0.1,example.com,80,GET,/,BLOCKED,0
```

## Documentation

- **[System Design Document](docs/design.md)**: Comprehensive architecture documentation including:
  - Component responsibilities and interactions
  - Concurrency model and thread safety
  - Data flow and request lifecycle
  - Error handling strategies
  - Performance characteristics and limitations
  - Security considerations

## Getting Help

- **Issues**: Open an issue on the repository for bug reports or feature requests
- **Documentation**: Check [docs/design.md](docs/design.md) for detailed architecture and implementation details
- **Code Review**: Review the source code in `src/` and `include/` directories for implementation details

### Common Issues

**Port already in use:**
- Stop the process using port 8888, or change the port in `config/server.cfg`

**Blocked domains not working:**
- Ensure `config/blocked.txt` exists and is readable
- Verify domains are listed one per line
- Check that the `FILTER_PATH` in `server.cfg` is correct

**No logs appearing:**
- Check that the executable has write permissions in the current directory
- Verify `logs/` directory exists or can be created
- Ensure `proxy.log` is not locked by another process

## Contributing

Contributions are welcome! This project follows standard open source contribution practices.

### How to Contribute

1. **Fork the repository** and create a feature branch
2. **Follow the existing code style** and structure
3. **Ensure thread-safety** when modifying shared resources
4. **Test your changes** with multiple concurrent connections
5. **Update relevant documentation** (README, design docs)
6. **Submit a pull request** with a clear description of changes

### Development Guidelines

- Use C++17 standard features
- Maintain thread-safety with mutexes for shared resources
- Follow the existing naming conventions
- Add appropriate error handling
- Update `docs/design.md` if architecture changes
- Test with the provided `test_proxy.bat` script

## Limitations

- **Windows Only**: Uses WinSock2 API, not cross-platform
- **HTTPS Inspection**: While CONNECT method supports HTTPS tunneling, the proxy cannot inspect or modify encrypted traffic
- **No Authentication**: No proxy authentication support (suitable for trusted networks only)
- **No Caching**: Does not cache responses
- **Connection: close**: Forces connection closure (no HTTP keep-alive)
- **No Request Body Handling**: Parser only processes headers (POST/PUT bodies may be affected)
- **Scalability**: Thread-per-connection model limits concurrent connections (~500-1000 on typical hardware)

For a complete list of limitations and known issues, see [docs/design.md](docs/design.md#known-system-limitations).

---

**Note**: This proxy server is designed for educational and development purposes. Use responsibly and in accordance with applicable laws and regulations.
