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

#if defined(__linux__)
#include <pthread.h>
#endif

#include "LibDriverPlatform.h"

using namespace std;

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
	DriverCustom *pDrv = (DriverCustom *)pData;
	pDrv->pFctDrive(pDrv->pProc);
	return NULL;
}

void *driverPlatformCreate(FuncInternalDrive pFctDrive, void *pProc, void *pConfigDriver)
{
	wrnLog("REMOVE_ME: creating custom driver");

	pthread_attr_t attrThread;
	bool initConfigDone = false;
	DriverCustomConfig *pConfig;
	DriverCustom *pDrv = NULL;
	int res;

	res = pthread_attr_init(&attrThread);
	if (res)
	{
		errLog(-1, "could not initialize thread attributes: %s (%d)", strerror(errno), errno);
		goto drvExit;
	}

	initConfigDone = true;

	pConfig = (DriverCustomConfig *)pConfigDriver;

	res = pthread_attr_setstacksize(&attrThread, pConfig->mSizeStack);
	if (res)
	{
		errLog(-1, "could not set stack size: %s (%d)", strerror(errno), errno);
		goto drvExit;
	}

	pDrv = new (nothrow) DriverCustom;
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
		delete pDrv;
		pDrv = NULL;

		errLog(-1, "could not create custom driver: %s (%d)", strerror(errno), errno);
		goto drvExit;
	}

drvExit:

	if (initConfigDone)
	{
		res = pthread_attr_destroy(&attrThread);
		if (res)
			errLog(-1, "could not destroy thread attributes: %s (%d)", strerror(errno), errno);
	}

	return pDrv;
}

void driverPlatformCleanUp(void *pDriver)
{
	wrnLog("REMOVE_ME: cleaning up custom driver");

	DriverCustom *pDrv = (DriverCustom *)pDriver;

	int res = pthread_join(pDrv->thread, NULL);
	if (res)
		errLog(-1, "could not cleanup custom driver: %s (%d)", strerror(errno), errno);

	delete pDrv;
}

size_t sizeStackGet()
{
	pthread_attr_t attrThread;
	bool initConfigDone = false;
	int res;
	size_t sizeStack = 0;

	res = pthread_attr_init(&attrThread);
	if (res)
	{
		errLog(-1, "could not initialize thread attributes: %s (%d)", strerror(errno), errno);
		goto drvExit;
	}

	initConfigDone = true;

	res = pthread_attr_getstacksize(&attrThread, &sizeStack);
	if (res)
	{
		errLog(-1, "could not get stack size: %s (%d)", strerror(errno), errno);
		goto drvExit;
	}

drvExit:

	if (initConfigDone)
	{
		res = pthread_attr_destroy(&attrThread);
		if (res)
			errLog(-1, "could not destroy thread attributes: %s (%d)", strerror(errno), errno);
	}

	return sizeStack;
}
#endif

/* Literature
 * - https://learn.microsoft.com/en-us/windows/win32/procthread/creating-threads
 */
#if defined(_WIN32)
#endif

