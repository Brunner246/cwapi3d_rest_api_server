cmake_minimum_required(VERSION 3.30)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(rest_api_server LANGUAGES CXX)

find_package(CwAPI3D CONFIG REQUIRED)
find_package(Boost 1.85.0 REQUIRED COMPONENTS system asio beast json)
include_directories(${Boost_INCLUDE_DIRS})

add_library(${PROJECT_NAME}
        SHARED
        main.cpp
)

add_subdirectory(src)

target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /WX
        /experimental:module
        /std:c++latest
        /Z7
)


target_link_libraries(rest_api_server
        PRIVATE
        ${Boost_LIBRARIES}
        CwAPI3D::CwAPI3D
)


target_compile_definitions(rest_api_server PRIVATE
        "API_EXPORT=__declspec(dllexport)"
        "_WIN32_WINNT=0x0601"
)
target_compile_definitions(rest_api_server INTERFACE
        "API_EXPORT=__declspec(dllimport)"
)

target_compile_definitions(${PROJECT_NAME} PRIVATE CWAPI3D_PLUGIN_NAME=L"Rest API Server")
target_compile_definitions(${PROJECT_NAME} PRIVATE CWAPI3D_AUTHOR_NAME=L"Michael Brunner")
target_compile_definitions(${PROJECT_NAME} PRIVATE CWAPI3D_AUTHOR_EMAIL=L"brunner@cadwork.swiss")
