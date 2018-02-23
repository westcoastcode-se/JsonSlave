//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_PATHUTILS_HPP
#define JSONSLAVE_PATHUTILS_HPP

#include <string>
#include <sstream>

#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#else

#include <limits.h>
#include <stdlib.h>

#endif

using namespace std;

struct PathUtils
{
#if defined(_WIN32)
	static const int FullPathMax = MAX_PATH;
#else
	static const int FullPathMax = PATH_MAX;
#endif

	static const string Delimiter;

	/**
	 * Convert the supplied path into an absolute path
	 *
	 * @param relativePath
	 * @return
	 */
	inline static string GetAbsolutePath(const string& relativePath) {
		char fullPath[FullPathMax];
#if defined(_WIN32)
		GetFullPathName(relativePath.c_str(), FullPathMax, fullPath, NULL);
#else
		realpath(relativePath.c_str(), fullPath);
#endif
		return string(fullPath);
	}

	/**
	 * Create the supplied directory
	 *
	 * @param path The path we want to create
	 * @return <code>true</code> if the directory was successfully created
	 */
	static bool CreateDirectory(string path);
};

#endif //JSONSLAVE_PATHUTILS_HPP
