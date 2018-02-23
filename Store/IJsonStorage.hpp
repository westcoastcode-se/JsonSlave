//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_IJSONSTORAGE_HPP
#define JSONSLAVE_IJSONSTORAGE_HPP

#include "../Config.hpp"

/**
 * Exception thrown from the storage engine if an error occurs
 */
struct storage_error : runtime_error
{
	explicit storage_error(const string& s)
			: runtime_error(s) {
	}
};

/**
 * Where the json data can be found
 */
class IJsonStorage
{
public:
	virtual ~IJsonStorage() {}

	/**
	 * Read json data from the store
	 *
	 * @param key
	 * @param defaultVal
	 * @return
	 */
	virtual string Get(const string& key, const string& defaultVal = string()) = 0;

	/**
	 * Add or Update json data in the store
	 *
	 * @param key
	 * @param content
	 * @return The old value if one is found
	 */
	virtual string Add(const string& key, const string& content) = 0;

	/**
	 * Remove the json data with the supplied key
	 *
	 * @param key
	 * @return The removed value if json data is found
	 */
	virtual string Remove(const string& key) = 0;
};

#endif //JSONSLAVE_IJSONSTORAGE_HPP
