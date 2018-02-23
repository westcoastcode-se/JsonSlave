//
// Created by Per Andersson on 2018-02-23.
// Copyright (c) 2018 West Coast Code AB. All rights reserved.
//

#include "HttpHeader.hpp"

int HttpHeader::GetIntValue() const {
	return atoi(mValue.c_str());
}
