/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 06.02.2024

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

#include <cinttypes>

#include "EspLedPulsing.h"
#include "LibTime.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StLedHighSet) \
		gen(StLedHigh) \
		gen(StLedLow) \
		gen(StLedBurstWait) \
		gen(StTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 0
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

EspLedPulsing::EspLedPulsing()
	: Processing("EspLedPulsing")
	, mStartMs(0)
	, mPin(0)
	, mCount(1)
	, mCurCount(0)
	, mWidth(50)
	, mPeriod(1000)
	, mGap(0)
	, mNumBurstsDone(0)
{
	mState = StStart;
}

/* member functions */

void EspLedPulsing::pinSet(uint8_t id)
{
	mPin = id;
}

//      _____________ .. Count
//     /        /
//   __       __
// _|  |_____|  |_____ ___
//
//                    |   | .. Gap
//  |        | .. Period
//  |  | .. Width
//
void EspLedPulsing::paramSet(uint32_t width, uint32_t period, uint8_t count, uint32_t gap)
{
	mWidth = width;
	mPeriod = period;
	mCount = count;
	mGap = gap;
}

Success EspLedPulsing::process()
{
	uint32_t curTimeMs = millis();
	uint32_t diffMs = curTimeMs - mStartMs;
	Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		if (!mPin)
			return procErrLog(-1, "pin not set");

		success = gpioInit();
		if (success != Positive)
			return procErrLog(-1, "could not initialize gpio");

		mState = StLedHighSet;

		break;
	case StLedHighSet:

		if (mWidth)
			gpio_set_level((gpio_num_t)mPin, 1);
		else
			gpio_set_level((gpio_num_t)mPin, 0);

		if (!mCurCount)
			mStartMs = curTimeMs;

		mState = StLedHigh;

		break;
	case StLedHigh:

		if (diffMs < mCurCount * mPeriod + mWidth)
			break;

		gpio_set_level((gpio_num_t)mPin, 0);

		mState = StLedLow;

		break;
	case StLedLow:

		if (diffMs < mCurCount * mPeriod + mPeriod)
			break;

		++mCurCount;
		if (mCurCount >= mCount)
		{
			mState = StLedBurstWait;
			break;
		}

		mState = StLedHighSet;

		break;
	case StLedBurstWait:

		if (diffMs < mCurCount * mPeriod + mGap)
			break;

		mCurCount = 0;
		++mNumBurstsDone;

		mState = StLedHighSet;

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

Success EspLedPulsing::shutdown()
{
	if (mPin)
		gpio_set_level((gpio_num_t)mPin, 0);

	return Positive;
}

Success EspLedPulsing::gpioInit()
{
	gpio_config_t ioConf = {};
	esp_err_t res;

	ioConf.intr_type = GPIO_INTR_DISABLE;
	ioConf.mode = GPIO_MODE_OUTPUT;
	ioConf.pin_bit_mask = 1ULL << mPin;
	ioConf.pull_down_en = (gpio_pulldown_t)0;
	ioConf.pull_up_en = (gpio_pullup_t)0;

	res = gpio_config(&ioConf);
	if (res != ESP_OK)
		return procErrLog(-1, "could not configure gpio: %s (0x%04x)",
							esp_err_to_name(res), res);

	return Positive;
}

void EspLedPulsing::processInfo(char *pBuf, char *pBufEnd)
{
#if 0
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
	dInfo("Pin\t\t\t%u\n", mPin);
	dInfo("Pulse width\t\t%lu [ms]\n", mWidth);
	dInfo("Period width\t\t%lu [ms]\n", mPeriod);
	dInfo("Period count\t\t%u\n", mCount);
	dInfo("Gap betw. bursts\t\t%lu [ms]\n", mGap);
#endif
	dInfo("Bursts done\t\t%" PRIu32 "\n", mNumBurstsDone);
}

/* static functions */

