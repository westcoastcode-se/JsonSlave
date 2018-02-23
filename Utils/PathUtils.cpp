//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include "PathUtils.hpp"

#if defined(_WIN32)
#include <io.h>
const string PathUtils::Delimiter("\\");

#else

#include <sys/stat.h>

const string PathUtils::Delimiter("/");

#endif

bool PathUtils::CreateDirectory(string path) {
#if defined(_WIN32)
	int result = ::mkdir(path.c_str());
#else
	int result = ::mkdir(path.c_str(), 0775);
#endif
	if (result == -1) {
		if (errno != EEXIST)
			return false;
	}
	return true;
}
