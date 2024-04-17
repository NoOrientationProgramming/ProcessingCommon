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

// Platform includes

#if defined(__linux__)
#include <pthread.h>
#else
#error "No specific drivers implemented for this platform"
#endif

// Common

#include "LibDriverPlatform.h"

using namespace std;

// Platform types

#if defined(__linux__)
struct DriverLinux
{
	pthread_t thread;
	ConfigDriver config;
	FuncInternalDrive pFctDrive;
	void *pProc;
};
#endif

// class ConfigDriver

size_t ConfigDriver::sizeStackDefault = 16384;

void ConfigDriver::sizeStackDefaultSet(size_t sizeStack)
{
	sizeStackDefault = sizeStack;
}

// Platform code

/*
 * Literature
 * - https://man7.org/linux/man-pages/man3/pthread_create.3.html
 * - https://man7.org/linux/man-pages/man3/pthread_join.3.html
 * - https://man7.org/linux/man-pages/man3/pthread_attr_init.3.html
 * - https://man7.org/linux/man-pages/man3/pthread_attr_setstacksize.3.html
 */
#if defined(__linux__)
void *threadExecute(void *pData)
{
	DriverLinux *pDrv = (DriverLinux *)pData;
	pDrv->pFctDrive(pDrv->pProc);
	return NULL;
}

void *driverPlatformCreate(FuncInternalDrive pFctDrive, void *pProc, void *pConfigDriver)
{
	pthread_attr_t attrThread;
	ConfigDriver configDefault;
	ConfigDriver *pConfig = &configDefault;
	DriverLinux *pDrv = NULL;
	int res;

	res = pthread_attr_init(&attrThread);
	if (res)
	{
		errLog(-1, "could not initialize thread attributes: %s (%d)", strerror(res), res);
		return NULL;
	}

	if (pConfigDriver)
		pConfig = (ConfigDriver *)pConfigDriver;

	res = pthread_attr_setstacksize(&attrThread, pConfig->mSizeStack);
	if (res)
	{
		errLog(-1, "could not set stack size: %s (%d)", strerror(res), res);
		goto drvExit;
	}

	pDrv = new (nothrow) DriverLinux;
	if (!pDrv)
	{
		errLog(-1, "could not allocate custom driver");
		goto drvExit;
	}

	pDrv->config = *pConfig;
	pDrv->pFctDrive = pFctDrive;
	pDrv->pProc = pProc;

	res = pthread_create(&pDrv->thread, &attrThread, threadExecute, pDrv);
	if (res)
	{
		errLog(-1, "could not create custom driver: %s (%d)", strerror(res), res);
		delete pDrv;
		pDrv = NULL;
	}

drvExit:

	res = pthread_attr_destroy(&attrThread);
	if (res)
		errLog(-1, "could not destroy thread attributes: %s (%d)", strerror(res), res);

	return pDrv;
}

void driverPlatformCleanUp(void *pDriver)
{
	DriverLinux *pDrv = (DriverLinux *)pDriver;
	pDriver = NULL;

	int res = pthread_join(pDrv->thread, NULL);
	if (res)
		errLog(-1, "could not cleanup custom driver: %s (%d)", strerror(res), res);

	delete pDrv;
	pDrv = NULL;
}

/*
 * Literature
 * - https://man7.org/linux/man-pages/man3/pthread_getattr_np.3.html
 */
size_t sizeStackGet()
{
	pthread_attr_t attrThread;
	size_t sizeStack;
	int res;

	res = pthread_getattr_np(pthread_self(), &attrThread); // non-standard
	if (res)
	{
		errLog(-1, "could not get thread attributes: %s (%d)", strerror(res), res);
		return 0;
	}

	res = pthread_attr_getstacksize(&attrThread, &sizeStack);
	if (res)
	{
		errLog(-1, "could not get stack size: %s (%d)", strerror(res), res);
		sizeStack = 0;
	}

	res = pthread_attr_destroy(&attrThread);
	if (res)
		errLog(-1, "could not destroy thread attributes: %s (%d)", strerror(res), res);

	return sizeStack;
}
#endif

/* Literature
 * - https://learn.microsoft.com/en-us/windows/win32/procthread/creating-threads
 */
#if defined(_WIN32)
#endif

