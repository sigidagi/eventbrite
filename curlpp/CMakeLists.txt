cmake_minimum_required(VERSION 2.8)

project(curlpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(${PROJECT_NAME}
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/cURLpp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Easy.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Exception.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Form.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Info.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Multi.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/OptionBase.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/Options.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/internal/CurlHandle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/internal/OptionList.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/internal/OptionSetter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/curlpp/internal/SList.cpp
)

target_link_libraries(${PROJECT_NAME}
    ${OPENSSL_LIBRARIES}
    ${CURL_LIBRARIES}
    pthread
)
