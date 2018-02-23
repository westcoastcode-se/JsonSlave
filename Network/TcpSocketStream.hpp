//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_TCPSOCKETSTREAM_HPP
#define JSONSLAVE_TCPSOCKETSTREAM_HPP

#include "Socket.hpp"

/**
 * A stream that we can use to send and receive data over a TCP socket
 */
class TcpSocketStream
{
public:
	explicit TcpSocketStream(SOCKET socket);

	~TcpSocketStream();

	/**
	 * Read a line from this stream
	 *
	 * @return A new line
	 */
	string ReadLine();

	/**
	 *
	 * @param length How many bytes we want to (try) read from the socket
	 * @return The data
	 */
	string Read(int length);

	/**
	 *
	 * @param str
	 * @param ...
	 */
	void WriteLineFormatted(const char* str, ...);

	void WriteLine(const string& str);

	void WriteLine();

	void Write(const string& str);

private:
	SOCKET mSocket;
};

#endif //JSONSLAVE_TCPSOCKETSTREAM_HPP
