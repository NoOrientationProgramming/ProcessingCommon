/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 15.09.2018

  Copyright (C) 2018, Johannes Natter

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

#ifndef HTTP_REQUESTING_H
#define HTTP_REQUESTING_H

#include <string>
#include <list>
#include <vector>

#include "Processing.h"
#if CONFIG_LIB_DSPC_HAVE_C_ARES
#include "DnsResolving.h"
#endif
#include "LibDspc.h"

#define numSharedDataTypes		4
#define dHttpDefaultTimeoutMs		2700

#define dHttpResponseCodeOk		200

struct HttpSession
{
	size_t numReferences;
	size_t maxReferences;
	std::string address;
	uint16_t port;
	CURL *pCurlShare;
	std::vector<std::mutex *> sharedDataMtxList;
	std::vector<std::mutex *> sslMtxList;
};

class HttpRequesting : public Processing
{

public:

	static HttpRequesting *create()
	{
		return new dNoThrow HttpRequesting;
	}
	static HttpRequesting *create(const std::string &url)
	{
		return new dNoThrow HttpRequesting(url);
	}

	// input
	void urlSet(const std::string &url);
	void methodSet(const std::string &type);
	void userPwSet(const std::string &userPw);
	void hdrAdd(const std::string &hdr);
	void dataSet(const std::string &data);
	void dataSet(const uint8_t *pData, size_t len);
	void authMethodSet(const std::string &authMethod);
	void versionTlsSet(const std::string &versionTls);
	void versionHttpSet(const std::string &versionHttp);
	void modeDebugSet(bool en);

	CURL *easyHandleCurl();

	// output
	uint16_t respCode() const;
	std::string &respHdr();
	std::string &respData();

protected:

	HttpRequesting();
	HttpRequesting(const std::string &url);
	virtual ~HttpRequesting();

private:

	HttpRequesting(const HttpRequesting &) = delete;
	HttpRequesting &operator=(const HttpRequesting &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);

	Success easyHandleCurlConfigure();
	Success easyHandleCurlBind();
	CURLM *multiHandleCurlInit();
	void easyHandleCurlUnbind();
	Success sessionCreate(const std::string &address, const uint16_t port);
	void sessionTerminate();
	void sharedDataMtxListDelete();

	/* member variables */
	uint32_t mStateSd;

	std::string mUrl;
	std::string mProtocol;
	std::string mNameHost;
	std::string mAddrHost;
	int mTypeIp;
	uint16_t mPort;
	std::string mPath;
	std::string mQueries;
	std::string mMethod;
	std::string mUserPw;
	std::list<std::string> mLstHdrs;
	std::vector<uint8_t> mData;
	std::string mAuthMethod;
	std::string mVersionTls;
	std::string mVersionHttp;
	bool mModeDebug;
#if CONFIG_LIB_DSPC_HAVE_C_ARES
	DnsResolving *mpResolv;
#endif
	CURL *mpCurl;
	bool mCurlBound;
	struct curl_slist *mpHeaderList;

	CURLcode mCurlRes;
	long mRespCode;
	std::string mRespHdr;
	std::string mRespData;

	std::list<HttpSession>::iterator mSession;
#if 0 // TODO: Implement
	uint8_t mRetries;
#endif
	Success mDoneCurl;

	/* static functions */
	static void multiProcess();
	static void curlMultiDeInit();
	static void sharedDataLock(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr);
	static void sharedDataUnLock(CURL *handle, curl_lock_data data, void *userptr);
	static size_t curlDataToStringWrite(void *ptr, size_t size, size_t nmemb, std::string *pData);
	static int curlTrace(CURL *pCurl, curl_infotype type, char *pData, size_t size, void *pUser);

	/* static variables */
	static std::mutex mtxCurlMulti;
	static CURLM *pCurlMulti;

	static std::mutex sessionMtx;
	static std::list<HttpSession> sessions;

	/* constants */

};

#endif

