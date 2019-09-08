// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once
#ifndef CEF_TESTS_UTILS_H_
#define CEF_TESTS_UTILS_H_
#include "include/internal/cef_types.h"
#include "include/cef_browser.h"

std::string GetDataURI(const std::string& data, const std::string& mime_type);

std::string GetErrorString(cef_errorcode_t code);

std::string StringReplace(const std::string& str, const std::string& from, const std::string& to);

void Alert(CefRefPtr<CefBrowser> browser, const std::string& message);


#endif