//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include "TcpSocketStream.hpp"

TcpSocketStream::TcpSocketStream(SOCKET s)
		: mSocket(s) {
}

TcpSocketStream::~TcpSocketStream() {

}

string TcpSocketStream::ReadLine() {
	stringstream line;

	char buffer = 0;
	auto result = recv(mSocket, &buffer, 1, 0);
	while (result > 0 && buffer != '\n') {
		if (buffer != '\r') {
			line << buffer;
		}
		result = recv(mSocket, &buffer, 1, 0);
	}

	return line.str();
}

void TcpSocketStream::WriteLineFormatted(const char* s, ...) {
	va_list arglist;
	va_start(arglist, s);
	string tmp;
	tmp.resize(1024);
	vsnprintf((char*) tmp.c_str(), 1024, s, arglist);
	va_end(arglist);
	WriteLine(tmp);
}

void TcpSocketStream::WriteLine(const string& str) {
	Write(str);
	WriteLine();
}

void TcpSocketStream::WriteLine() {
	static const char NL = '\n';
	send(mSocket, &NL, 1, 0);
}

void TcpSocketStream::Write(const string& str) {
	send(mSocket, str.c_str(), str.length(), 0);
}

string TcpSocketStream::Read(int length) {
	string s;
	s.resize(length);

	int bytesRead = 0;
	int totalBytesRead = 0;
	auto dest = (char*) s.c_str();
	while (totalBytesRead < length &&
	       (bytesRead = (int) recv(mSocket, dest + totalBytesRead, length - totalBytesRead, 0)) > 0) {
		totalBytesRead += bytesRead;
	}
	return s;
}
