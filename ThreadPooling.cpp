/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 01.02.2024

  Copyright (C) 2024-now Authors and www.dsp-crowd.com

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

#include "ThreadPooling.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StBrokerStart) \
		gen(StBrokerMain) \
		gen(StInternalStart) \
		gen(StInternalMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#define dForEach_SdState(gen) \
		gen(StSdStart) \
		gen(StBrokerSdStart) \
		gen(StInternalSdStart) \
		gen(StInternalSdMain) \

#define dGenSdStateEnum(s) s,
dProcessStateEnum(SdState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#define dGenSdStateString(s) #s,
dProcessStateStr(SdState);
#endif

using namespace std;

#define LOG_LVL	1

Pipe<PoolRequest> ThreadPooling::ppPoolRequests;

ThreadPooling::ThreadPooling()
	: Processing("ThreadPooling")
	, mStateSd(StSdStart)
	, mStartMs(0)
	, mCntInternals(0)
	, mpFctDriverCreate(NULL)
	, mIsInternal(false)
	, mNumFinished(0)
{
	mState = StStart;
}

/* member functions */

void ThreadPooling::workerCntSet(uint16_t cnt)
{
	mCntInternals = cnt;
}

void ThreadPooling::driverCreateFctSet(FctDriverCreate pFctDriverCreate)
{
	mpFctDriverCreate = pFctDriverCreate;
}

Success ThreadPooling::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
	int32_t idDriver = 0;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		if (mIsInternal)
		{
			mState = StInternalStart;
			break;
		}

		mState = StBrokerStart;

		break;
	case StBrokerStart:

		if (!mCntInternals)
			return procErrLog(-1, "no workers configured");

		mVecInternals.reserve(mCntInternals);

		for (uint16_t i = 0; i < mCntInternals; ++i)
		{
			ThreadPooling *pInternal;

			pInternal = ThreadPooling::create();
			if (!pInternal)
				return procErrLog(-1, "could not create thread pool worker");

			pInternal->mIsInternal = true;
			mVecInternals.push_back(pInternal);

			if (mpFctDriverCreate)
			{
				start(pInternal, DrivenByExternalDriver);
				mpFctDriverCreate(pInternal, i);
			}
			else
				start(pInternal, DrivenByNewInternalDriver);
		}

		mState = StBrokerMain;

		break;
	case StBrokerMain:

		PipeEntry<PoolRequest> entryReq;
		PoolRequest req;

		if (!ppPoolRequests.get(entryReq))
			break;
		req = entryReq.particle;

		procDbgLog(LOG_LVL, "pool request received");

		if (req.idDriverDesired >= 0 and req.idDriverDesired < mCntInternals)
			idDriver = req.idDriverDesired;
		else
			idDriver = idDriverNextGet();

		mVecInternals[idDriver]->mListProcs.push_back(req.pProc);

		break;
	case StInternalStart:

		mState = StInternalMain;

		break;
	case StInternalMain:

		procsDrive();

		break;
	default:
		break;
	}

	return Pending;
}

Success ThreadPooling::shutdown()
{
	switch (mStateSd)
	{
	case StSdStart:

		if (mIsInternal)
		{
			mStateSd = StInternalSdStart;
			break;
		}

		mStateSd = StBrokerSdStart;

		break;
	case StBrokerSdStart:

		return Positive;

		break;
	case StInternalSdStart:

		if (mListProcs.size())
		{
			procWrnLog("driving not finished");

			mStateSd = StInternalSdMain;
			break;
		}

		return Positive;

		break;
	case StInternalSdMain:

		procsDrive();

		if (mListProcs.size())
			break;

		return Positive;

		break;
	default:
		break;
	}

	return Pending;
}

void ThreadPooling::procsDrive()
{
	list<Processing *>::iterator iter;
	Processing *pProc;

	iter = mListProcs.begin();
	while (iter != mListProcs.end())
	{
		pProc = *iter;

		pProc->treeTick();

		if (pProc->progress())
		{
			++iter;
			continue;
		}

		procDbgLog(LOG_LVL, "finished driving process %p", pProc);
		++mNumFinished;

		undrivenSet(pProc);
		iter = mListProcs.erase(iter);
	}
}

int32_t ThreadPooling::idDriverNextGet()
{
	int32_t idSelected = 0;

	for (size_t i = 1; i < mVecInternals.size(); ++i)
	{
		if (mVecInternals[i]->mListProcs.size() <
			mVecInternals[idSelected]->mListProcs.size())
			idSelected = i;
	}

	return idSelected;
}

void ThreadPooling::procAdd(Processing *pProc, int32_t idDriver)
{
	PoolRequest req;

	req.pProc = pProc;
	req.idDriverDesired = idDriver;

	dbgLog(LOG_LVL, "adding proc %p to queue", pProc);
	ppPoolRequests.commit(req);
}

void ThreadPooling::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
	dInfo("State shutdown\t\t%s\n", SdStateString[mStateSd]);
#endif
	if (!mIsInternal)
		return;

	dInfo("Processing\t\t%zu\n", mListProcs.size());
	//dInfo("Finished\t\t%lu\n", mNumFinished);
}

/* static functions */

