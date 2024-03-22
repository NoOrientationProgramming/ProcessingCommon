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
using namespace Qt;

#define LOG_LVL	0

PhyAnimating::PhyAnimating(const char *name)
	: Processing(name)
	, QObject()
	, mpWindow(NULL)
	, mpOpt(NULL)
	, mpChart(NULL)
	, mArgc(0)
	, mArgv(NULL)
	, mAppQt(mArgc, mArgv)
	, mpGrid(NULL)
	, mpView(NULL)
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

		mpWindow = new (nothrow) QWidget();
		if (!mpWindow)
			return procErrLog(-1, "could not create window");

		mpWindow->resize(1280, 800);
		mpWindow->setMinimumSize(400, 300);

		mpGrid = new (nothrow) QGridLayout();
		if (!mpGrid)
			return procErrLog(-1, "could not create grid");

		mpWindow->setLayout(mpGrid);

		mpOpt = new (nothrow) QVBoxLayout();
		if (!mpOpt)
			return procErrLog(-1, "could not create vertical box");

		mpGrid->addLayout(mpOpt, 0, 0, 1, 1);

		mpChart = new (nothrow) QChart();
		if (!mpChart)
			return procErrLog(-1, "could not create chart");

		mpView = new (nothrow) QChartView(mpChart);
		if (!mpView)
		{
			delete mpChart;
			return procErrLog(-1, "could not create chart view");
		}

		mpGrid->addWidget(mpView, 0, 1, 1, 1);

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
	Success success;

	success = animShutdown();
	if (success == Pending)
		return Pending;

	mMapLabels.clear();

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

Success PhyAnimating::animShutdown()
{
	return Positive;
}

void PhyAnimating::uiLineAdd(const string &strLabel)
{
	QFrame *pLine;
	QLabel *pLabel1 = NULL;
	QLabel *pLabel2 = NULL;

	pLabel1 = new (nothrow) QLabel();
	if (!pLabel1)
		return;

	pLine = new (nothrow) QFrame();
	if (!pLine)
	{
		delete pLabel1;
		return;
	}

	pLine->setFrameShape(QFrame::HLine);
	pLine->setFrameShadow(QFrame::Sunken);

	if (strLabel.size())
		pLabel2 = new (nothrow) QLabel();

	if (strLabel.size() && !pLabel2)
	{
		delete pLine;
		delete pLabel1;
		return;
	}

	// Show
	mpOpt->addWidget(pLabel1);
	mpOpt->addWidget(pLine);

	if (pLabel2)
	{
		string str;

		str.append("<b>");
		str.append(strLabel);
		str.append("</b>");

		pLabel2->setText(str.c_str());

		mpOpt->addWidget(pLabel2);
	}
}

QPushButton *PhyAnimating::uiButtonAdd(const string &strText)
{
	QPushButton *pButton;

	pButton = new (nothrow) QPushButton(strText.c_str());
	if (!pButton)
		return NULL;

	mpOpt->addWidget(pButton);

	return pButton;
}

QCheckBox *PhyAnimating::uiSwitchAdd(const string &strLabel)
{
	QHBoxLayout *pLayout;
	QLabel *pLabel;
	QCheckBox *pSwitch;

	pLayout = new (nothrow) QHBoxLayout();
	if (!pLayout)
		return NULL;

	pLabel = new (nothrow) QLabel();
	if (!pLabel)
	{
		delete pLayout;
		return NULL;
	}

	pLabel->setText(strLabel.c_str());

	pSwitch = new (nothrow) QCheckBox();
	if (!pSwitch)
	{
		delete pLabel;
		delete pLayout;
		return NULL;
	}

	pLayout->addWidget(pLabel);
	pLayout->addStretch(1);
	pLayout->addWidget(pSwitch);

	mpOpt->addLayout(pLayout);

	return pSwitch;
}

QSlider *PhyAnimating::uiSliderAdd(float valMax, float valStart,
			const string &strPrefix,
			const string &strUnit,
			bool isTwoSided)
{
	QSlider *pSlider;
	QLabel *pLabel;

	pSlider = new (nothrow) QSlider(Orientation::Horizontal);
	if (!pSlider)
		return NULL;

	pLabel = new (nothrow) QLabel();
	if (!pLabel)
	{
		delete pSlider;
		return NULL;
	}

	// Slider config
	if (isTwoSided)
		pSlider->setRange(-100, 100);
	else
		pSlider->setRange(0, 100);

	pSlider->setSingleStep(25);
	pSlider->setValue((int)((100 * valStart) / valMax));

	// Label config
	snprintf(mBufLabel, sizeof(mBufLabel),
			"%-40s %.2f [%s]",
			strPrefix.c_str(),
			valStart,
			strUnit.c_str());

	pLabel->setText(mBufLabel);

	// Connect
	LabelInfo inf;

	inf.pLabel = pLabel;
	inf.prefix = strPrefix;
	inf.unit = strUnit;
	inf.valMax = valMax;

	mMapLabels[pSlider] = inf;
	QObject::connect(pSlider, &QSlider::valueChanged,
				this, &PhyAnimating::sliderUpdated);

	// Show
	mpOpt->addWidget(pSlider);
	mpOpt->addWidget(pLabel);

	return pSlider;
}

QLineEdit *PhyAnimating::uiLineEditAdd(const string &strLabel)
{
	QLineEdit *pLine;
	QLabel *pLabel = NULL;

	pLine = new (nothrow) QLineEdit();
	if (!pLine)
		return NULL;

	if (strLabel.size())
		pLabel = new (nothrow) QLabel(strLabel.c_str());

	if (strLabel.size() && !pLabel)
	{
		delete pLine;
		return NULL;
	}

	// Show
	mpOpt->addWidget(pLine);

	if (pLabel)
		mpOpt->addWidget(pLabel);

	return pLine;
}

void PhyAnimating::sliderUpdated(int value)
{
	QSlider *pSlider = (QSlider *)sender();
	map<QWidget *, LabelInfo>::iterator iter;

	iter = mMapLabels.find(pSlider);
	if (iter == mMapLabels.end())
		return;

	LabelInfo inf = iter->second;

	snprintf(mBufLabel, sizeof(mBufLabel),
			"%-40s %.2f [%s]",
			inf.prefix.c_str(),
			inf.valMax * value / 100,
			inf.unit.c_str());

	inf.pLabel->setText(mBufLabel);
}

/* static functions */

