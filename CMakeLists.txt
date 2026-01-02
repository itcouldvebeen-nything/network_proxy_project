cmake_minimum_required(VERSION 3.10)
project(ProxyServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp 
    src/Parser.cpp 
    src/Filter.cpp 
    src/Logger.cpp 
    src/ProxyCore.cpp
    src/Config.cpp
)

add_executable(proxy_exe ${SOURCES})
target_include_directories(proxy_exe PRIVATE include)

if(WIN32)
    target_link_libraries(proxy_exe PRIVATE ws2_32)
endif()