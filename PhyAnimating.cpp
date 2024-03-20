/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 19.03.2024

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

#include "PhyAnimating.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

using namespace std;

#define LOG_LVL	0

PhyAnimating::PhyAnimating(const char *name)
	: Processing(name)
	, mpWindow(NULL)
	, mpOpt(NULL)
	, mArgc(0)
	, mArgv(NULL)
	, mAppQt(mArgc, mArgv)
	, mpGrid(NULL)
	, mWinVisibleOld(false)
{
	mStateBase = StStart;
}

/* member functions */

Success PhyAnimating::process()
{
	Success success;
	bool ok;

	switch (mStateBase)
	{
	case StStart:

		mpWindow = new QWidget();
		if (!mpWindow)
			return procErrLog(-1, "could not create window");

		mpWindow->resize(1280, 800);
		mpWindow->setMinimumSize(400, 300);

		mpGrid = new QGridLayout();
		if (!mpGrid)
			return procErrLog(-1, "could not create grid");

		mpWindow->setLayout(mpGrid);

		mpOpt = new QVBoxLayout();
		if (!mpOpt)
			return procErrLog(-1, "could not create vertical box");

		mpGrid->addLayout(mpOpt, 0, 0, 1, 1);

		mpGrid->setColumnStretch(0, 1);
		mpGrid->setColumnStretch(1, 3);

		mStateBase = StMain;

		break;
	case StMain:

		QApplication::processEvents();

		success = animate();
		if (success != Pending)
			return success;

		ok = mpWindow->isVisible();
		if (mWinVisibleOld && !ok)
		{
			procDbgLog(LOG_LVL, "Qt window isn't visible anymore. Shutting down");
			return Positive;
		}
		mWinVisibleOld = ok;

		break;
	default:
		break;
	}

	return Pending;
}

Success PhyAnimating::shutdown()
{
	procDbgLog(LOG_LVL, "Deleting Qt window");
	if (mpWindow)
	{
		delete mpWindow;
		mpWindow = NULL;
	}

	procDbgLog(LOG_LVL, "Quitting Qt application");
	QApplication::quit();

	return Positive;
}

/* static functions */

