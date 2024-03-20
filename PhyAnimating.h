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

#ifndef PHY_ANIMATING_H
#define PHY_ANIMATING_H

/*
 * Dependencies
 * - Qt6
 *   - Install: apt install qt6-base-dev
 *   - License: https://doc.qt.io/qt-6/licensing.html
 * - Matplotlib => QtCharts?
 *   - License: https://matplotlib.org/stable/users/project/license.html
 */

#include <Python.h>
#include <QApplication>
#include <QGridLayout>
#include <QPushButton>

#include "Processing.h"

class PhyAnimating : public Processing
{

public:

protected:

	PhyAnimating(const char *name);
	virtual ~PhyAnimating() {}

	QWidget *mpWindow;
	QVBoxLayout *mpOpt;

private:

	PhyAnimating()
		: Processing("")
		, mArgc(0)
		, mArgv(NULL)
		, mAppQt(mArgc, mArgv)
	{}
	PhyAnimating(const PhyAnimating &)
		: Processing("")
		, mArgc(0)
		, mArgv(NULL)
		, mAppQt(mArgc, mArgv)
	{}
	PhyAnimating &operator=(const PhyAnimating &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	virtual Success animate() = 0;

	/* member variables */
	uint32_t mStateBase;
	int mArgc;
	char **mArgv;
	QApplication mAppQt;
	QGridLayout *mpGrid;
	bool mWinVisibleOld;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

