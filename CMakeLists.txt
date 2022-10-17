cmake_minimum_required(VERSION 3.10)

project(myHttpServer
    LANGUAGES CXX)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(src)

add_executable(myHttpServer
    src/main.cpp
    src/server.cpp
    src/http_request.cpp
    src/http_response.cpp
    src/utils.cpp
)

target_link_libraries(myHttpServer PRIVATE Threads::Threads)
target_compile_features(myHttpServer PRIVATE cxx_std_17)