//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_HTTPHEADERS_HPP
#define JSONSLAVE_HTTPHEADERS_HPP

#include "../Network/TcpSocketStream.hpp"
#include "HttpHeader.hpp"

/**
 * Available http methods we can use
 */
struct HttpMethods
{
	static const string Get;
	static const string Put;
	static const string Delete;
};

/**
 * Http statuses that can be returned
 */
struct HttpStatus
{
	static const string Ok;
	static const string BadRequest;
	static const string NotFound;
	static const string InternalServerError;
	static const string Unknown;

	/**
	 * Convert a status code in integer format into a string
	 *
	 * @param code The status code
	 * @return A string version of the status code
	 */
	static const string& CodeToString(int code);
};

/**
 * Container for http headers
 */
class HttpHeaders
{
public:
	static const string ContentLength;

	~HttpHeaders();

	/**
	 * @return The incoming request's method
	 */
	inline const string& GetMethod() const {
		return mMethod;
	}

	/**
	 * @return The requested path
	 */
	inline const string& GetPath() const {
		return mPath;
	}

	/**
	 * Try to get the header with the supplied name
	 *
	 * @param name The name of the header
	 * @return The header if found; <code>nullptr</code> otherwise
	 */
	const HttpHeader* FindHeader(const string& name) const;

	/**
	 * Read the content of the supplied stream and convert it into a header
	 *
	 * @param stream The stream
	 * @return The headers found in the incoming stream
	 */
	static shared_ptr<HttpHeaders> FromStream(shared_ptr<TcpSocketStream> stream);

private:
	HttpHeaders(const string& method, const string& path);

	void AddHeader(const string& key, const string& value);

private:
	string mMethod;
	string mPath;
	unordered_map<string, HttpHeader> mHeaders;
};

#endif //JSONSLAVE_HTTPHEADERS_HPP
