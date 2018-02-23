//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include <iostream>
#include "DiskJsonStorage.hpp"
#include "../Utils/StringUtils.hpp"
#include "../Utils/PathUtils.hpp"

DiskJsonStorage::DiskJsonStorage(const string& rootPath)
		: mRootPath(PathUtils::GetAbsolutePath(rootPath)) {
	if (!PathUtils::CreateDirectory(mRootPath)) {
		throw storage_error(StringUtils::Format("Failed to create storage at '%s'", mRootPath.c_str()));
	}
	cout << "Start serving data from: '" << mRootPath << "'" << endl;
}

DiskJsonStorage::~DiskJsonStorage() {

}

string DiskJsonStorage::Get(const string& key, const string& defaultVal) {
	lock_guard<mutex> l(mMutex);
	const auto path = SecurePath(key);
	string result;
	_Get(&result, key, path, defaultVal);
	return result;
}

string DiskJsonStorage::Add(const string& key, const string& content) {
	lock_guard<mutex> l(mMutex);
	const auto path = SecurePath(key);

	// Read the old value if one exists
	string oldValue;
	_Get(&oldValue, key, path, StringUtils::Empty);

	// Try to write the new content
	FILE* fp = fopen(path.c_str(), "wb");
	if (fp == nullptr) {
		throw storage_error(StringUtils::Format("Failed to open resource: '%s'", key.c_str()));
	}

	fwrite(content.c_str(), content.length(), 1, fp);
	fclose(fp);
	return oldValue;
}

string DiskJsonStorage::Remove(const string& key) {
	lock_guard<mutex> l(mMutex);
	const auto path = SecurePath(key);

	// Read the old value if one exists. If no value can be found then assume that we cannot remove it
	string oldValue;
	_Get(&oldValue, key, path, StringUtils::Empty);
	if (oldValue.empty())
		return StringUtils::Empty;

	// Remove the old "removed" file and mark the current file as "removed"
	const string removedFile = path + ".removed";
	remove(removedFile.c_str());
	rename(path.c_str(), removedFile.c_str());

	return oldValue;
}

string DiskJsonStorage::SecurePath(const string& key) {
	if (key.empty() || key[0] != '/')
		throw storage_error(StringUtils::Format("Resource '%s' is not valid", key.c_str()));
	return mRootPath + NormalizePath(key);
}

void DiskJsonStorage::_Get(string* result, const string& key, const string& fullPath, const string& defaultValue) {
	FILE* fp = fopen(fullPath.c_str(), "r");
	if (fp == nullptr) {
		*result = defaultValue;
		return;
	}

	// Figure out the file size
	fseek(fp, 0, SEEK_END);
	const auto length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// Larger than 4 mb?
	if (length > 4096 * 1024) {
		throw storage_error(StringUtils::Format("Resource '%s' is to large", key.c_str()));
	}

	// Read the data from the HDD
	result->resize(length);
	fread((char*) result->c_str(), length, 1, fp);
	fclose(fp);
}

string DiskJsonStorage::NormalizePath(const string& path) {
	//
	// Assume that we've verified the path to the resource (i.e. length > 0 and first character is "/")
	//
	stringstream result;
	result << "/";
	const auto size = path.length();
	for (int i = 1; i < size; ++i) {
		const auto c = path[i];
		if (c >= 'a' && c <= 'z')
			result << c;
		else if (c >= 'A' && c <= 'Z')
			result << c;
		else if (c >= '0' && c <= '9')
			result << c;
		else if (c == '_')
			result << c;
		else
			result << "+";
	}

	result << ".json";
	return result.str();
}
