cmake_minimum_required(VERSION 2.8)

project(eventbrite)

set(MAJOR_VERSION 0)
set(MINOR_VERSION 1)
set(PATCH_VERSION 0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++17 -O3 -Wall -Wno-deprecated-declarations")

message(STATUS "Configuring ${PROJECT_NAME} ${SH_MAJOR}.${SH_MINOR}.${SH_PATCH}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(EV REQUIRED)
find_package(CURL REQUIRED)

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/curlpp
        ${CMAKE_SOURCE_DIR}/include
        )

add_subdirectory(curlpp)

add_executable(${PROJECT_NAME}
        src/eventbrite.cpp
        src/configuration.cpp
        src/util.cpp
        src/main.cpp
        )

target_link_libraries(${PROJECT_NAME}
        ${LIBEV_LIBRARIES}
        pthread
        curlpp
        )
