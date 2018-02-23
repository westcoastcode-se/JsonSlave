//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_HTTPHEADER_HPP
#define JSONSLAVE_HTTPHEADER_HPP

#include "../Config.hpp"

/**
 * Object representing an Http Header
 */
struct HttpHeader
{
	HttpHeader(const string& name, const string& value)
			: mName(name), mValue(value) {}

	explicit HttpHeader(const HttpHeader& rhs)
			: mName(rhs.mName), mValue(rhs.mValue) {}

	/**
	 * @return The name of the header
	 */
	inline const string& GetName() const {
		return mName;
	}

	/**
	 * @return The headers value
	 */
	inline const string& GetValue() const {
		return mValue;
	}

	int GetIntValue() const;

private:
	string mName;
	string mValue;
};

#endif //JSONSLAVE_HTTPHEADER_HPP
