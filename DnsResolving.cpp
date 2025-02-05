/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 11.10.2024

  Copyright (C) 2024, Johannes Natter

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

#include "DnsResolving.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StGlobalInit) \
		gen(StAresStart) \
		gen(StAresDoneWait) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#define dForEach_SdState(gen) \
		gen(StSdStart) \

#define dGenSdStateEnum(s) s,
dProcessStateEnum(SdState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

DnsResolving::DnsResolving()
	: Processing("DnsResolving")
	//, mStartMs(0)
	, mStateSd(StSdStart)
	, mHostname("")
#if CONFIG_LIB_DSPC_HAVE_C_ARES
	, mOptionsAres()
	, mChannelAres()
	, mChannelAresInitDone(false)
	, mDoneAres(Pending)
	, mErrAres("")
#endif
{
	mState = StStart;
}

/* member functions */

Success DnsResolving::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if CONFIG_LIB_DSPC_HAVE_C_ARES
	bool ok;
#endif
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		if (!mHostname.size())
			return procErrLog(-1, "hostname not set");

		mState = StGlobalInit;

		break;
	case StGlobalInit:

#if CONFIG_LIB_DSPC_HAVE_C_ARES
		procDbgLog("using libc-ares");
		caresGlobalInit();
#else
		procDbgLog("libc-ares required");
		return -1;
#endif
		mState = StAresStart;

		break;
	case StAresStart:

#if CONFIG_LIB_DSPC_HAVE_C_ARES
		ok = aresStart();
		if (!ok)
			return procErrLog(-1, "could not start async address resolution");
#endif
		mState = StAresDoneWait;

		break;
	case StAresDoneWait:

#if CONFIG_LIB_DSPC_HAVE_C_ARES
		aresProcess();

		if (mDoneAres == Pending)
			break;

		if (mDoneAres != Positive)
			return procErrLog(-1, "could not finish async address resolution: %s",
									mErrAres.c_str());

		ares_destroy(mChannelAres);
		mChannelAresInitDone = false;
#endif
		return Positive;

		break;
	default:
		break;
	}

	return Pending;
}

Success DnsResolving::shutdown()
{
	switch (mStateSd)
	{
	case StSdStart:

#if CONFIG_LIB_DSPC_HAVE_C_ARES
		if (mChannelAresInitDone)
		{
			ares_destroy(mChannelAres);
			mChannelAresInitDone = false;
		}
#endif
		return Positive;

		break;
	default:
		break;
	}

	return Pending;
}

#if CONFIG_LIB_DSPC_HAVE_C_ARES
/*
 * Literature
 * - https://c-ares.org/docs.html
 * - https://c-ares.org/docs/ares_init_options.html
 * - https://c-ares.org/docs/ares_getaddrinfo.html
 * - https://man7.org/linux/man-pages/man3/getaddrinfo.3.html
 * - https://c-ares.org/docs/ares_freeaddrinfo.html
 */
bool DnsResolving::aresStart()
{
	int res;

	memset(&mOptionsAres, 0, sizeof(mOptionsAres));

	mOptionsAres.flags = ARES_FLAG_NORECURSE;
	mOptionsAres.timeout = 400;
	mOptionsAres.tries = 2;

	res = ares_init_options(&mChannelAres, &mOptionsAres, ARES_OPT_TIMEOUTMS | ARES_OPT_TRIES);
	if (res != ARES_SUCCESS)
	{
		procErrLog(-1, "could not set ares options");
		return false;
	}

	ares_addrinfo_hints hints;

	memset(&hints, 0, sizeof(hints));

	hints.ai_flags = 0;
	hints.ai_family = AF_UNSPEC; /* IPv4 / IPv6 */
	hints.ai_socktype = 0; /* Any */
	hints.ai_protocol = 0; /* Any */

	//procWrnLog("Getting address of: %s", mHostname.c_str());

	ares_getaddrinfo(mChannelAres,
					mHostname.c_str(), NULL, &hints,
					aresRequestDone, this);

	mChannelAresInitDone = true;

	return true;
}

/*
 * Literature
 * - https://c-ares.org/docs.html
 * - https://c-ares.org/docs/ares_fds.html
 * - https://c-ares.org/docs/ares_process.html
 * - https://c-ares.org/docs/ares_timeout.html
 * - https://man7.org/linux/man-pages/man2/select.2.html
 * - https://linux.die.net/man/3/ares_process
 */
void DnsResolving::aresProcess()
{
	if (!mChannelAresInitDone)
	{
		mDoneAres = -1;
		return;
	}

	fd_set fdsRead, fdsWrite;
	int fdsMax, res;

	FD_ZERO(&fdsRead);
	FD_ZERO(&fdsWrite);

	fdsMax = ares_fds(mChannelAres, &fdsRead, &fdsWrite);
	if (!fdsMax)
	{
		mDoneAres = procErrLog(-1, "no file descriptors to be processed");
		return;
	}

	if (fdsMax > 1000)
	{
		mDoneAres = procErrLog(-1, "socket numbers above 1000 not supported at the moment");
		return;
	}

	struct timeval tmoSelect, *pTmo;

	tmoSelect.tv_sec = 0;
	tmoSelect.tv_usec = 5000;

	pTmo = ares_timeout(mChannelAres, &tmoSelect, &tmoSelect);

	res = select(fdsMax, &fdsRead, &fdsWrite, NULL, pTmo);
	if (res < 0)
	{
		mDoneAres = procErrLog(-1, "select returned error: %s (%d)", strerror(errno), errno);
		return;
	}

	ares_process(mChannelAres, &fdsRead, &fdsWrite);
}
#endif

void DnsResolving::hostnameSet(const string &hostname)
{
	mHostname = hostname;
}

const list<string> &DnsResolving::lstIPv4()
{
	return mLstIPv4;
}

const list<string> &DnsResolving::lstIPv6()
{
	return mLstIPv6;
}

void DnsResolving::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

#if CONFIG_LIB_DSPC_HAVE_C_ARES
/*
 * Literature
 * - https://c-ares.org/docs.html
 * - https://c-ares.org/docs/ares_freeaddrinfo.html
 */
void DnsResolving::aresRequestDone(void *arg, int status, int timeouts, struct ares_addrinfo *result)
{
	DnsResolving *pReq = (DnsResolving *)arg;

	if (status)
	{
		pReq->mErrAres = ares_strerror(status);
		pReq->mDoneAres = -1;

		ares_freeaddrinfo(result);
		return;
	}

	(void)timeouts;

	struct ares_addrinfo_node *pNode;
	const void *pAddr;
	char bAddr[64];
	string addr;
	list<string> *pList;

	pNode = result->nodes;
	for (; pNode; pNode = pNode->ai_next)
	{
		pList = NULL;

		if (pNode->ai_family == AF_INET)
		{
			const struct sockaddr_in *in_addr =
						(const struct sockaddr_in *)((void *)pNode->ai_addr);
			pAddr = &in_addr->sin_addr;
			pList = &pReq->mLstIPv4;
		}
		else
		if (pNode->ai_family == AF_INET6)
		{
			const struct sockaddr_in6 *in_addr =
						(const struct sockaddr_in6 *)((void *)pNode->ai_addr);
			pAddr = &in_addr->sin6_addr;
			pList = &pReq->mLstIPv6;
		} else
			continue;

		ares_inet_ntop(pNode->ai_family, pAddr, bAddr, sizeof(bAddr));

		if (pList)
			pList->push_back(bAddr);

		//wrnLog("Addr: %s", bAddr);
	}

	pReq->mDoneAres = Positive;

	ares_freeaddrinfo(result);
}
#endif

