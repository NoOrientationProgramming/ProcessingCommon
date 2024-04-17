/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 16.04.2024

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

#ifndef LIB_DRIVER_CUSTOM_H
#define LIB_DRIVER_CUSTOM_H

#include <pthread.h>

#include "Processing.h"

class DriverCustomConfig
{
public:
	DriverCustomConfig()
		: mSizeStack(7)
	{}

	size_t mSizeStack;
};

struct DriverCustom
{
	pthread_t thread;
	DriverCustomConfig config;
	FuncInternalDrive pFctDrive;
	void *pProc;
};

void *platformDriverCreate(FuncInternalDrive pFctDrive, void *pProc, void *pConfigDriver);
void platformDriverCleanUp(void *pDriver);
size_t sizeStackGet();

#endif

