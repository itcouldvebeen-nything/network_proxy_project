cmake_minimum_required(VERSION 3.10)
project(ProxyServer)

# Tell CMake to look in the include directory for headers
include_directories(include)

# List all your new source files
add_executable(proxy_exe 
    src/main.cpp 
    src/Parser.cpp 
    src/Filter.cpp 
    src/Logger.cpp 
    src/ProxyCore.cpp
)

target_link_libraries(proxy_exe ws2_32)