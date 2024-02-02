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
		gen(StTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

ThreadPooling::ThreadPooling()
	: Processing("ThreadPooling")
	, mStartMs(0)
	, mCntWorker(0)
	, mpFctDriverCreate(NULL)
	, mIsInternal(false)
{
	mState = StStart;
}

/* member functions */

void ThreadPooling::workerCntSet(uint16_t cnt)
{
	mCntWorker = cnt;
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

		if (!mCntWorker)
			return procErrLog(-1, "no workers configured");

		for (uint16_t i = 0; i < mCntWorker; ++i)
		{
			ThreadPooling *pWorker;

			pWorker = ThreadPooling::create();
			if (!pWorker)
				return procErrLog(-1, "could not create thread pool worker");

			pWorker->mIsInternal = true;

			if (mpFctDriverCreate)
			{
				start(pWorker, DrivenByExternalDriver);
				mpFctDriverCreate(pWorker, i);
			}
			else
				start(pWorker, DrivenByNewInternalDriver);
		}

		mState = StBrokerMain;

		break;
	case StBrokerMain:

		break;
	case StInternalStart:

		mState = StInternalMain;

		break;
	case StInternalMain:

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

void ThreadPooling::procAdd(Processing *pProc)
{
	infLog("adding proc %p to queue", pProc);
}

void ThreadPooling::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
}

/* static functions */

