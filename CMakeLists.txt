cmake_minimum_required(VERSION 3.16)
project(simpleChat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PRODUCTION_BUILD "Make this a production build!" OFF)

# common
file(GLOB_RECURSE COMMON_SOURCES src/common/*.cpp)
file(GLOB_RECURSE COMMON_HEADERS include/common/*.hpp)

add_library(common ${COMMON_SOURCES} ${COMMON_HEADERS})
target_include_directories(common PUBLIC include)

# server
file(GLOB_RECURSE SERVER_SOURCES src/server/*.cpp include/server/*.hpp)
add_executable(server ${SERVER_SOURCES})
target_link_libraries(server PRIVATE common)

if(PRODUCTION_BUILD)
    target_compile_definitions(server PRIVATE
            RESOURCES_PATH="./resources/"
            PRODUCTION_BUILD=1
    )
else()
    target_compile_definitions(server PRIVATE
            RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
            PRODUCTION_BUILD=0
    )
endif()

# client
file(GLOB_RECURSE CLIENT_SOURCES src/client/*.cpp include/client/*.hpp)
add_executable(client ${CLIENT_SOURCES})
target_link_libraries(client PRIVATE common)

if(PRODUCTION_BUILD)
    target_compile_definitions(client PRIVATE
            RESOURCES_PATH="./resources/"
            PRODUCTION_BUILD=1
    )
else()
    target_compile_definitions(client PRIVATE
            RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/"
            PRODUCTION_BUILD=0
    )
endif()
