/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 03.01.2023

  Copyright (C) 2023-now Authors and www.dsp-crowd.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIB_DSPC_H
#define LIB_DSPC_H

#include <string>
#include <mutex>
#include <vector>

#if CONFIG_LIB_DSPC_HAVE_JSONCPP
#include <jsoncpp/json/json.h>
#endif

#if CONFIG_LIB_DSPC_HAVE_CRYPTOPP
#include <cryptopp/secblock.h>
#include <cryptopp/base64.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/hmac.h>
#endif

#if CONFIG_LIB_DSPC_HAVE_CURL
#include <curl/curl.h>
#endif

#if CONFIG_PROC_HAVE_C_ARES
#include <ares.h>
#endif

#include "Processing.h"
#include "Res.h"
#include "LibTime.h"
#include "LibFilesys.h"

typedef std::vector<std::string> VecStr;
typedef VecStr::iterator VecStrIter;
typedef VecStr::const_iterator VecStrConstIter;

typedef std::list<std::string> ListStr;
typedef ListStr::iterator ListStrIter;
typedef ListStr::const_iterator ListStrConstIter;

// Debugging
std::string appVersion();
void hexDump(const void *pData, size_t len, const char *pName = NULL, size_t colWidth = 0x10);
size_t hexDumpPrint(char *pBuf, char *pBufEnd,
				const void *pData, size_t len,
				const char *pName = NULL, size_t colWidth = 0x10);
std::string toHexStr(const std::string &strIn);
std::vector<char> toHex(const std::string &strIn);
size_t strReplace(std::string &strIn, const std::string &strFind, const std::string &strReplacement);

// Json
#if CONFIG_LIB_DSPC_HAVE_JSONCPP
bool jKeyFind(const Json::Value &val, const std::string &nameKey);
void jsonPrint(const Json::Value &val);
#endif

// Cryptography
#if CONFIG_LIB_DSPC_HAVE_CRYPTOPP
std::string base64enc(const std::string &msg);
std::string md5(const std::string &msg, const std::string &prefix = "");
std::string sha256(const std::string &msg, const std::string &prefix = "");
std::string sha256(const CryptoPP::SecByteBlock &msg, const std::string &prefix = "");
bool isValidSha256(const std::string &digest);
std::string hmacSha256(const std::string &msg, const CryptoPP::SecByteBlock &key);
#endif

// curl
#if CONFIG_LIB_DSPC_HAVE_CURL
void curlGlobalInit();
void curlGlobalDeInit();
std::string versionCurl();
void versionCurlInfoPrint();
#endif

// c-ares
#if CONFIG_PROC_HAVE_C_ARES
void caresGlobalInit();
void caresGlobalDeInit();
#endif

// Internet
bool isValidEmail(const std::string &mail);
bool isValidIp4(const std::string &ip);
std::string remoteAddr(int socketFd);
std::string urlToHost(const std::string &url);
void urlToTriple(const std::string &url,
				std::string &protocol,
				std::string &host,
				std::string &path);

// Strings
void strToVecStr(const std::string &str, VecStr &vStr, char delim = '\n');

#endif

