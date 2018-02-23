//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_SOCKET_HPP
#define JSONSLAVE_SOCKET_HPP

#include "../Config.hpp"

#if defined(_WIN32)

#	define _WINSOCK_DEPRECATED_NO_WARNINGS
#   include <winsock2.h>
#   include <inaddr.h>

typedef u_long from_address;

typedef int socklen_t;

#else

#   include <arpa/inet.h>
#   include <netinet/in.h>
#   include <sys/types.h>
#   include <sys/fcntl.h>
#   include <unistd.h>
#   include <sys/socket.h>
#   include <netdb.h>

typedef in_addr_t from_address;

#endif

#if !defined(_WIN32)

#   define SOCKET int
#   define INVALID_SOCKET (-1)
#   define SOCKET_ERROR (-1)

#endif

using namespace std;

#endif //JSONSLAVE_SOCKET_HPP
