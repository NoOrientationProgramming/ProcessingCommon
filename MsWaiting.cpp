/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 02.10.2024

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

#include "MsWaiting.h"
#include "LibTime.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 0
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

MsWaiting::MsWaiting(uint32_t durationMs)
	: Processing("MsWaiting")
	, mStartMs(0)
	, mDurationMs(durationMs)
	, mDiffMs(0)
{
	mState = StStart;
}

/* member functions */

Success MsWaiting::process()
{
	uint32_t curTimeMs = millis();
	uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mStartMs = curTimeMs;
		mState = StMain;

		break;
	case StMain:

		mDiffMs = diffMs;

		if (diffMs < mDurationMs)
			break;

		return Positive;

		break;
	default:
		break;
	}

	return Pending;
}

void MsWaiting::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	//dInfo("State\t\t\t%s\n", ProcStateString[mState]);
	pBuf += progressStr(pBuf, pBufEnd, mDiffMs, mDurationMs);
#endif
}

/* static functions */

