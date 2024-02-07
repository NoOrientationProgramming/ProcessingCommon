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

#ifndef ESP_LED_PULSING_H
#define ESP_LED_PULSING_H

#include <driver/gpio.h>

#include "Processing.h"

class EspLedPulsing : public Processing
{

public:

	static EspLedPulsing *create()
	{
		return new (std::nothrow) EspLedPulsing;
	}

	void pinSet(uint8_t id);

	// width, period and gap in [ms]
	void paramSet(uint32_t width, uint32_t period, uint8_t count = 1, uint32_t gap = 0);

protected:

	EspLedPulsing();
	virtual ~EspLedPulsing() {}

private:

	EspLedPulsing(const EspLedPulsing &) : Processing("") {}
	EspLedPulsing &operator=(const EspLedPulsing &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);

	Success gpioInit();

	/* member variables */
	uint32_t mStartMs;

	uint8_t mPin;
	uint8_t mCount;
	uint8_t mCurCount;

	uint32_t mWidth;
	uint32_t mPeriod;
	uint32_t mGap;

	uint32_t mNumBurstsDone;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

