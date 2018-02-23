//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_TCPSOCKET_HPP
#define JSONSLAVE_TCPSOCKET_HPP

#include "TcpSocketStream.hpp"

using namespace std;

class TcpSocket
{
public:
	TcpSocket(SOCKET socket, const string& address);

	~TcpSocket();

	/**
	 * Accept a new client connection
	 *
	 * @return A new client if found; <code>nullptr</code> if no client was accepted
	 */
	TcpSocket* Accept();

	/**
	 * @return A new stream that we can send and receive data over
	 */
	shared_ptr<TcpSocketStream> OpenStream();

	/**
	 * @return Fetch the address of this connection
	 */
	inline const string& GetAddress() const {
		return mAddress;
	}

	/**
	 * Listen for incoming connections on the supplied port
	 *
	 * @param port The port we are listening for traffic on
	 * @return A new socket
	 */
	static TcpSocket* Listen(uint16_t port);

private:
	SOCKET mSocket;
	string mAddress;
};

#endif //JSONSLAVE_TCPSOCKET_HPP
