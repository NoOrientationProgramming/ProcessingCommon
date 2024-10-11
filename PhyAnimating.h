/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 19.03.2024

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

#ifndef PHY_ANIMATING_H
#define PHY_ANIMATING_H

/*
 * Dependencies
 * - Qt6
 *   - Install: apt install qt6-base-dev libqt6charts6-dev
 *   - License: https://doc.qt.io/qt-6/licensing.html
 * - OpenCV?
 *   - Install: apt install libopencv-dev
 */

#include <map>

#include <QApplication>
#include <QGridLayout>
#include <QtCharts>
#include <QLabel>
#include <QFrame>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QLineEdit>
#include <QProgressBar>

#include "Processing.h"

struct LabelInfo
{
	QLabel *pLabel;
	std::string prefix;
	std::string unit;
	float valMax;
};

class PhyAnimating : public Processing, public QObject
{

public:

protected:

	PhyAnimating(const char *name);
	virtual ~PhyAnimating() {}

	void uiLineAdd(const std::string &strLabel = "");
	QLabel *uiLabelAdd(const std::string &strPrefix = "", bool alignRight = true);
	QPushButton *uiButtonAdd(const std::string &strText);
	QCheckBox *uiSwitchAdd(const std::string &strLabel);
	QSlider *uiSliderAdd(int valMax, int valStart,
				const std::string &strPrefix,
				const std::string &strUnit = "",
				bool isTwoSided = false);
	QLineEdit *uiLineEditAdd(const std::string &strLabel = "");
	QProgressBar *uiProgressAdd(const std::string &strLabel);

	QChart *uiChartAdd();

	QWidget *mpWindow;
	QVBoxLayout *mpOpt;

private:

	PhyAnimating() = delete;
	PhyAnimating(const PhyAnimating &) = delete;
	PhyAnimating &operator=(const PhyAnimating &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	virtual Success animate() = 0;
	virtual Success animShutdown();

	void sliderUpdated(int value);
	bool qtGlobalInit();

	/* member variables */
	uint32_t mStateBase;
	QGridLayout *mpGrid;
	QChartView *mpView;
	bool mWinVisibleOld;
	bool mBaseInitDone;
	std::map<QWidget * /* src widget */, LabelInfo> mMapLabels;
	char mBufLabel[19];

	/* static functions */
	static void qtGlobalDestruct();

	/* static variables */
#if CONFIG_PROC_HAVE_DRIVERS
	static std::mutex mtxGlobalInit;
#endif
	static int qtArgc;
	static char **pQtArgv;
	static QApplication *pAppQt;
	static bool globalInitDone;

	/* constants */

};

#endif

