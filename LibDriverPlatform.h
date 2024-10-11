/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 16.04.2024

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

#ifndef LIB_DRIVER_PLATFORM_H
#define LIB_DRIVER_PLATFORM_H

#include "Processing.h"

class ConfigDriver
{
public:
	ConfigDriver()
		: mSizeStack(sizeStackDefault)
	{}

	static void sizeStackDefaultSet(size_t sizeStack);

	size_t mSizeStack;

private:
	static size_t sizeStackDefault;
};

void *driverPlatformCreate(FuncInternalDrive pFctDrive, void *pProc, void *pConfigDriver);
void driverPlatformCleanUp(void *pDriver);
size_t sizeStackGet();

#endif

