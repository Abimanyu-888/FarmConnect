cmake_minimum_required(VERSION 4.0.3)
project(dsa_project)

# Require C++17
set(CMAKE_CXX_STANDARD        17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS      OFF)


add_executable(dsa_project main.cpp src/helper.cpp src/farmer_routes.cpp src/buyer_routes.cpp)

target_include_directories(dsa_project 
                        PUBLIC 
                        ${INCLUDE_PATHS} 
                        include
                        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/asio/asio/include
                        ${CMAKE_CURRENT_SOURCE_DIR}/vendor/Crow/include)


# On some toolchains you must link the filesystem library manually
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(dsa_project PRIVATE stdc++fs)
endif()