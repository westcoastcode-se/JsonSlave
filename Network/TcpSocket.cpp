//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include <iostream>
#include "TcpSocket.hpp"

#if defined(_WIN32)
#   ifdef _WIN32_WINNT
#       undef _WIN32_WINNT
#       define _WIN32_WINNT 0x0600
#   endif

#   include <Ws2tcpip.h>

#endif

namespace
{
#ifdef _WIN32
	atomic_int gInitCounter(0);
#endif

	bool IncreaseAndInit() {
#if defined(_WIN32)
		if (++gInitCounter == 1) {
			WSADATA wsaData;

			if (WSAStartup(MAKEWORD (2, 2), &wsaData)) {
				gInitCounter--;
				return false;
			}

			if (LOBYTE (wsaData.wVersion) != 2 || HIBYTE (wsaData.wVersion) != 2) {
				WSACleanup();
				gInitCounter--;
				return false;
			}
		}
#endif
		return true;
	}

	void DecreaseAndRelease() {
#if defined(_WIN32)
		if (--gInitCounter == 0) {
			WSACleanup();
		}
#endif
	}

	void SocketClose(SOCKET s) {
#if defined(_WIN32)
		closesocket(s);
#else
		close(s);
#endif
	}

	string GetSocketAddress(SOCKET socket) {
		socklen_t len;
		struct sockaddr_storage addr;
		char ipstr[256];

		len = sizeof addr;
		getpeername(socket, (struct sockaddr*) &addr, &len);

		if (addr.ss_family == AF_INET) {
			auto s = (struct sockaddr_in*) &addr;
			inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
		} else if (addr.ss_family == AF_INET6) {
			auto s = (struct sockaddr_in6*) &addr;
			inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
		}

		return string(ipstr);
	}

	void SocketSetBlocking(SOCKET socket, bool blocking) {
#if defined(_WIN32)
		u_long block = blocking ? 0 : 1;
		ioctlsocket(socket, FIONBIO, &block);
#else
		const auto ctl = fcntl(socket, F_GETFL);
		int command = ctl;
		if (!blocking) {
			command = command | O_NONBLOCK;
		} else {
			command = command & ~O_NONBLOCK;
		}
		fcntl(socket, F_SETFL, command);
#endif
	}
}

TcpSocket::TcpSocket(SOCKET socket, const string& address)
		: mSocket(socket), mAddress(address) {
	const int reuse = 0;
#ifdef SO_REUSEADDR
	setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, (const char*) &reuse, sizeof(reuse));
#endif
#ifdef SO_REUSEPORT
	setsockopt(socket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse));
#endif

#if !defined(_WIN32)
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &tv, sizeof(struct timeval));
	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*) &tv, sizeof(struct timeval));
#else
	DWORD timeout = 5 * 1000;
	setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, (const char*) &timeout, sizeof(timeout));
	setsockopt(socket, SOL_SOCKET, SO_SNDTIMEO, (const char*) &timeout, sizeof(timeout));
#endif
}

TcpSocket::~TcpSocket() {
	if (mSocket != INVALID_SOCKET) {
		SocketClose(mSocket);
		mSocket = INVALID_SOCKET;
	}

	DecreaseAndRelease();
	cout << "Shutting down client " << mAddress.c_str() << endl;
}

TcpSocket* TcpSocket::Accept() {
	timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(mSocket, &read_set);

	if (select(mSocket + 1, &read_set, NULL, NULL, &tv) > 0) {
		sockaddr_in addrin;
		memset((void*) &addrin, 0, sizeof(addrin));
		socklen_t addrinLen = sizeof(addrin);
		SOCKET s = ::accept(mSocket, (sockaddr*) &addrin, &addrinLen);
		if (s < 0) {
			return nullptr;
		}

		IncreaseAndInit();
		// Make the client socket blocking
		SocketSetBlocking(s, true);

		return new TcpSocket(s, GetSocketAddress(s));
	}

	return nullptr;
}

shared_ptr<TcpSocketStream> TcpSocket::OpenStream() {
	return shared_ptr<TcpSocketStream>(new TcpSocketStream(mSocket));
}

TcpSocket* TcpSocket::Listen(uint16_t port) {
	if (!IncreaseAndInit()) {
		throw runtime_error("Failed to initialize Socket library");
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		DecreaseAndRelease();
		throw runtime_error("Could not create TCP socket");
	}

	sockaddr_in addrin;
	memset((void*) &addrin, 0, sizeof(addrin));
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(port);
	addrin.sin_addr.s_addr = htonl(INADDR_ANY);
	const socklen_t addrinLen = sizeof(addrin);
	if (::bind(serverSocket, (const sockaddr*) &addrin, addrinLen) < 0) {
		SocketClose(serverSocket);
		DecreaseAndRelease();

		stringstream ss;
		ss << "Failed to bind socket to port " << port;
		throw runtime_error(ss.str());
	}

	if (::listen(serverSocket, 16) < 0) {
		SocketClose(serverSocket);
		DecreaseAndRelease();

		stringstream ss;
		ss << "Failed to listen for incoming connections on port " << port;
		throw runtime_error(ss.str());
	}

	// Make the server-accept socket non-blocking
	SocketSetBlocking(serverSocket, false);

	return new TcpSocket(serverSocket, string("0.0.0.0"));
}
