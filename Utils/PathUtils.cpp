//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include "PathUtils.hpp"

#if defined(_WIN32)

const string PathUtils::Delimiter("\\");

#else

#include <sys/stat.h>

const string PathUtils::Delimiter("/");

#endif

bool PathUtils::CreateDirectory(string path) {
	bool success;
	int result = ::mkdir(path.c_str(), 0775);
	if (result == -1) {
		if (errno != EEXIST)
			return false;
	}
	return true;
}
