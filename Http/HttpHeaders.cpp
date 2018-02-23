//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include "HttpHeaders.hpp"
#include "../Utils/StringUtils.hpp"

const string HttpMethods::Get("GET");
const string HttpMethods::Put("PUT");
const string HttpMethods::Delete("DELETE");
const string HttpHeaders::ContentLength("content-length");

const string HttpStatus::Ok("OK");
const string HttpStatus::BadRequest("Bad Request");
const string HttpStatus::NotFound("Not Found");
const string HttpStatus::InternalServerError("Internal Server Error");
const string HttpStatus::Unknown("");

const string& HttpStatus::CodeToString(int code) {
	switch (code) {
		case 200:
			return Ok;
		case 400:
			return BadRequest;
		case 404:
			return NotFound;
		case 500:
			return InternalServerError;
		default:
			return Unknown;
	}
}

HttpHeaders::HttpHeaders(const string& method, const string& path)
		: mMethod(method), mPath(path) {
}

HttpHeaders::~HttpHeaders() {

}

const HttpHeader* HttpHeaders::FindHeader(const string& name) const {
	const HttpHeader* result = nullptr;
	const string lowerName = StringUtils::ToLower(name);
	const auto it = mHeaders.find(lowerName);
	if (it != mHeaders.end()) {
		result = &it->second;
	}
	return result;
}

shared_ptr<HttpHeaders> HttpHeaders::FromStream(shared_ptr<TcpSocketStream> stream) {
	// First line is always the same
	auto result = stream->ReadLine();
	const auto methodParts = StringUtils::Split(result, ' ');
	string method;
	string path;
	if (methodParts.size() > 0) method = methodParts[0];
	if (methodParts.size() > 1) path = methodParts[1];

	// Parse the header values
	auto const headers = new HttpHeaders(method, path);
	while (!(result = stream->ReadLine()).empty()) {
		const auto parts = StringUtils::SplitFirst(result, ':');
		if (parts.size() <= 0)
			continue;

		const string key(StringUtils::Trim(parts[0]));
		string value;
		if (parts.size() > 1) {
			value = StringUtils::Trim(parts[1]);
		}

		headers->AddHeader(key, value);
	}

	return shared_ptr<HttpHeaders>(headers);
}

void HttpHeaders::AddHeader(const string& key, const string& value) {
	const string lowerKey = StringUtils::ToLower(key);
	mHeaders.insert(make_pair(lowerKey, HttpHeader(key, value)));
}
