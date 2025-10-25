cmake_minimum_required(VERSION 3.10)
project(dsa_project)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(ASIO_INCLUDE_DIR NAMES asio.hpp PATHS "${CMAKE_PREFIX_PATH}")
find_path(CROW_INCLUDE_DIR NAMES crow.h PATHS "${CMAKE_PREFIX_PATH}")

add_executable(dsa_project main.cpp src/helper.cpp src/farmer_routes.cpp src/buyer_routes.cpp) 

target_include_directories(dsa_project PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include 
    ${ASIO_INCLUDE_DIR}
    ${CROW_INCLUDE_DIR}
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(dsa_project PRIVATE stdc++fs)
endif()