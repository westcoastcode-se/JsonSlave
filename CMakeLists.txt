cmake_minimum_required(VERSION 3.6)
project(jsonslave)

# Use C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Network Libraries
if (WIN32)
    if (MINGW)
        set(SOCK_LIBRARIES "wsock32 -lws2_32 -static-libgcc -static-libstdc++ -static -lpthread")
    else ()
        set(SOCK_LIBRARIES "ws2_32")
    endif ()
elseif (APPLE)
    set(SOCK_LIBRARIES "")
elseif (UNIX)
    set(SOCK_LIBRARIES "")
endif ()

file(GLOB all_files *.cpp *.hpp
        Utils/*.cpp Utils/*.hpp
        Http/*.cpp Http/*.hpp
        Network/*.cpp Network/*.hpp
        Store/*.cpp Store/*.hpp)

add_executable(jsonslave ${all_files})
target_link_libraries(jsonslave ${SOCK_LIBRARIES})