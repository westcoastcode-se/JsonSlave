//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#ifndef JSONSLAVE_STRINGUTILS_HPP
#define JSONSLAVE_STRINGUTILS_HPP

#include "../Config.hpp"

struct StringUtils
{
	static const string Empty;

	/**
	 * Split the supplied string into it's smaller components
	 *
	 * @param s The string we want to split
	 * @param c The delimiter we are splitting the string with
	 * @return A vector with all the components
	 */
	inline static vector<string> Split(const string& s, const char c) {
		vector<string> result;
		stringstream ss;
		for (int i = 0; i < s.length(); ++i) {
			if (s[i] == c) {
				result.push_back(ss.str());
				ss.str(string());
			} else
				ss << s[i];
		}

		auto last = ss.str();
		if (last.length() > 0)
			result.push_back(last);
		return result;
	}

	/**
	 * Split the supplied string into maximum 2 components
	 *
	 * @param s The string we want to split
	 * @param c The delimiter we are splitting the string with
	 * @return A vector with all the components
	 */
	inline static vector<string> SplitFirst(const string& s, const char c) {
		vector<string> result;
		stringstream ss;
		for (int i = 0; i < s.length(); ++i) {
			if (!result.empty()) {
				ss << s[i];
			} else {
				if (s[i] == c) {
					result.push_back(ss.str());
					ss.str(string());
				} else {
					ss << s[i];
				}
			}
		}

		auto last = ss.str();
		if (last.length() > 0)
			result.push_back(last);
		return result;
	}

	/**
	 * Trim the supplied string
	 *
	 * @param s The string we want to trim
	 * @return
	 */
	static inline string Trim(const string& s) {
		string copy = s;
		_TrimFromLeft(copy);
		_TrimFromRight(copy);
		return copy;
	}

	/**
	 * Convert the supplied string into a lower-case version of it
	 *
	 * @param s
	 * @return
	 */
	inline static string ToLower(const string& s) {
		string data = s;
		transform(data.begin(), data.end(), data.begin(), ::tolower);
		return data;
	}

	/**
	 * Format the supplied format string
	 *
	 * @param s
	 * @param ...
	 * @return
	 */
	inline static string Format(const char* s, ...) {
		va_list arglist;
		va_start(arglist, s);
		string tmp;
		tmp.resize(1024);
		vsnprintf((char*) tmp.c_str(), 1024, s, arglist);
		va_end(arglist);
		return tmp;
	}

private:
	static inline void _TrimFromLeft(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(),
		                                std::not1(std::ptr_fun<int, int>(std::isspace))));
	}

	static inline void _TrimFromRight(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(),
		                     std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}
};

#endif //JSONSLAVE_STRINGUTILS_HPP
