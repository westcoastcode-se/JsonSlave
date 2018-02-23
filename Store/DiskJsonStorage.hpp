//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_DISKJSONSTORAGE_HPP
#define JSONSLAVE_DISKJSONSTORAGE_HPP

#include "IJsonStorage.hpp"

/**
 * A disk-based json storage
 */
class DiskJsonStorage : public IJsonStorage
{
public:
	/**
	 * @param rootPath The path where the JSON data will be put into
	 */
	explicit DiskJsonStorage(const string& rootPath);

	~DiskJsonStorage() override;

	string Get(const string& key, const string& defaultVal) override;

	string Add(const string& key, const string& content) override;

	string Remove(const string& key) override;

private:
	void _Get(string* result, const string& key, const string& fullPath, const string& defaultValue);

	/**
	 * Secure the supplied path and convert it into an absolute path
	 *
	 * @param path
	 * @return
	 */
	string SecurePath(const string& path);

	/**
	 * Normalizes the path into something that only contains the pattern [a-zA-Z0-9_]
	 * @param path
	 * @return
	 */
	static string NormalizePath(const string& path);

private:
	string mRootPath;
	mutex mMutex;
};

#endif //JSONSLAVE_DISKJSONSTORAGE_HPP
