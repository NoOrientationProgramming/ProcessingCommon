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

#include "PhyAnimating.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StQtInit) \
		gen(StMainStart) \
		gen(StMain) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

using namespace std;
using namespace Qt;

#define LOG_LVL	0

#if CONFIG_PROC_HAVE_DRIVERS
mutex PhyAnimating::mtxGlobalInit;
#endif
int PhyAnimating::qtArgc = 0;
char **PhyAnimating::pQtArgv = NULL;
QApplication *PhyAnimating::pAppQt = NULL;
bool PhyAnimating::globalInitDone = false;

PhyAnimating::PhyAnimating(const char *name)
	: Processing(name)
	, QObject()
	, mpWindow(NULL)
	, mpOpt(NULL)
	, mStateBase(0)
	, mpGrid(NULL)
	, mWinVisibleOld(false)
	, mBaseInitDone(false)
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

		if (levelDriver())
			return procErrLog(-1, "Qt process cannot be driven in non-parent driver");

		mStateBase = StQtInit;

		break;
	case StQtInit:

		ok = qtGlobalInit();
		if (!ok)
			return procErrLog(-1, "could not initialize Qt");

		mStateBase = StMainStart;

		break;
	case StMainStart:

		mpWindow = new (nothrow) QWidget();
		if (!mpWindow)
			return procErrLog(-1, "could not create window");

		mpWindow->resize(1280, 800);
		mpWindow->setMinimumSize(400, 300);

		mpGrid = new (nothrow) QGridLayout();
		if (!mpGrid)
			return procErrLog(-1, "could not create grid");

		mpWindow->setLayout(mpGrid);

		mpGrid->setColumnStretch(0, 1);
		mpGrid->setColumnStretch(1, 3);

		mpOpt = new (nothrow) QVBoxLayout();
		if (!mpOpt)
			return procErrLog(-1, "could not create vertical box");

		mpGrid->addLayout(mpOpt, 0, 0, 1, 1);

		mBaseInitDone = true;

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
	Success success = Positive;

	if (mBaseInitDone)
		success = animShutdown();

	if (success == Pending)
		return Pending;

	mMapLabels.clear();

	if (!mpWindow)
		return Positive;

	procDbgLog(LOG_LVL, "deleting Qt window");
	delete mpWindow;
	mpWindow = NULL;
	procDbgLog(LOG_LVL, "deleting Qt window. Done");

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
		pLabel1 = NULL;

		return;
	}

	pLine->setFrameShape(QFrame::HLine);
	pLine->setFrameShadow(QFrame::Sunken);

	if (strLabel.size())
		pLabel2 = new (nothrow) QLabel();

	if (strLabel.size() && !pLabel2)
	{
		delete pLine;
		pLine = NULL;

		delete pLabel1;
		pLabel1 = NULL;

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

QLabel *PhyAnimating::uiLabelAdd(const string &strPrefix, bool alignRight)
{
	QHBoxLayout *pLayout = NULL;
	QLabel *pLabelValue = NULL;
	QLabel *pLabelPrefix = NULL;
	bool stretchNeeded = false;

	pLayout = new (nothrow) QHBoxLayout();
	if (!pLayout)
		return NULL;

	pLabelValue = new (nothrow) QLabel();
	if (!pLabelValue)
	{
		delete pLayout;
		pLayout = NULL;

		return NULL;
	}

	if (strPrefix.size())
		pLabelPrefix = new (nothrow) QLabel();

	if (strPrefix.size() && !pLabelPrefix)
	{
		delete pLabelValue;
		pLabelValue = NULL;

		delete pLayout;
		pLayout = NULL;

		return NULL;
	}

	if (pLabelPrefix)
	{
		pLabelPrefix->setText(strPrefix.c_str());
		pLayout->addWidget(pLabelPrefix);
		stretchNeeded = true;
	}

	if (alignRight)
		stretchNeeded = true;

	if (stretchNeeded)
		pLayout->addStretch(1);

	pLayout->addWidget(pLabelValue);
	mpOpt->addLayout(pLayout);

	return pLabelValue;
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
		pLayout = NULL;

		return NULL;
	}

	pLabel->setText(strLabel.c_str());

	pSwitch = new (nothrow) QCheckBox();
	if (!pSwitch)
	{
		delete pLabel;
		pLabel = NULL;

		delete pLayout;
		pLayout = NULL;

		return NULL;
	}

	pLayout->addWidget(pLabel);
	pLayout->addStretch(1);
	pLayout->addWidget(pSwitch);

	mpOpt->addLayout(pLayout);

	return pSwitch;
}

QSlider *PhyAnimating::uiSliderAdd(int valMax, int valStart,
			const string &strPrefix,
			const string &strUnit,
			bool isTwoSided)
{
	QSlider *pSlider = NULL;
	QHBoxLayout *pLayout = NULL;
	QLabel *pLabelPrefix = NULL;
	QLabel *pLabelValue = NULL;
	QLabel *pLabelUnit = NULL;
	LabelInfo inf;

	pSlider = new (nothrow) QSlider(Orientation::Horizontal);
	if (!pSlider)
		goto exitErr;

	pLayout = new (nothrow) QHBoxLayout();
	if (!pLayout)
		goto exitErr;

	pLabelPrefix = new (nothrow) QLabel();
	if (!pLabelPrefix)
		goto exitErr;

	pLabelPrefix->setText(strPrefix.c_str());

	pLabelValue = new (nothrow) QLabel();
	if (!pLabelValue)
		goto exitErr;

	snprintf(mBufLabel, sizeof(mBufLabel), "%d", valStart);
	pLabelValue->setText(mBufLabel);

	if (strUnit.size())
		pLabelUnit = new (nothrow) QLabel();

	if (strUnit.size() && !pLabelUnit)
		goto exitErr;

	if (pLabelUnit)
	{
		string str;

		str.append("[");
		str.append(strUnit);
		str.append("]");

		pLabelUnit->setText(str.c_str());
	}

	// Slider config
	if (isTwoSided)
		pSlider->setRange(-valMax, valMax);
	else
		pSlider->setRange(0, valMax);

	pSlider->setSingleStep(valMax / 4);
	pSlider->setValue(valStart);

	// Connect
	inf.pLabel = pLabelValue;
	inf.prefix = strPrefix;
	inf.unit = strUnit;
	inf.valMax = valMax;

	mMapLabels[pSlider] = inf;
	QObject::connect(pSlider, &QSlider::valueChanged,
				this, &PhyAnimating::sliderUpdated);

	// Show
	pLayout->addWidget(pLabelPrefix);
	pLayout->addStretch(1);
	pLayout->addWidget(pLabelValue);

	if (pLabelUnit)
		pLayout->addWidget(pLabelUnit);

	mpOpt->addWidget(pSlider);
	mpOpt->addLayout(pLayout);

	return pSlider;

exitErr:
	if (pLabelValue)
	{
		delete pLabelValue;
		pLabelValue = NULL;
	}

	if (pLabelPrefix)
	{
		delete pLabelPrefix;
		pLabelPrefix = NULL;
	}

	if (pLayout)
	{
		delete pLayout;
		pLayout = NULL;
	}

	if (pSlider)
	{
		delete pSlider;
		pSlider = NULL;
	}

	return NULL;
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
		pLine = NULL;

		return NULL;
	}

	// Show
	mpOpt->addWidget(pLine);

	if (pLabel)
		mpOpt->addWidget(pLabel);

	return pLine;
}

QProgressBar *PhyAnimating::uiProgressAdd(const string &strLabel)
{
	QProgressBar *pProgress;
	QLabel *pLabel;

	pProgress = new (nothrow) QProgressBar();
	if (!pProgress)
		return NULL;

	pLabel = new (nothrow) QLabel();
	if (!pLabel)
	{
		delete pProgress;
		pProgress = NULL;

		return NULL;
	}

	pLabel->setText(strLabel.c_str());

	// Show
	mpOpt->addWidget(pProgress);
	mpOpt->addWidget(pLabel);

	return pProgress;
}

QChart *PhyAnimating::uiChartAdd()
{
	QChartView *pView;
	QChart *pChart;

	pView = new (nothrow) QChartView();
	if (!pView)
		return NULL;

	pView->setFocusPolicy(FocusPolicy::ClickFocus);

	pChart = new (nothrow) QChart();
	if (!pChart)
	{
		delete pView;
		pView = NULL;

		return NULL;
	}

	pView->setChart(pChart);

	// Show
	mpGrid->addWidget(pView, 0, 1, 1, 1);

	return pChart;
}

void PhyAnimating::sliderUpdated(int value)
{
	QSlider *pSlider = (QSlider *)sender();
	map<QWidget *, LabelInfo>::iterator iter;

	iter = mMapLabels.find(pSlider);
	if (iter == mMapLabels.end())
		return;

	LabelInfo inf = iter->second;

	snprintf(mBufLabel, sizeof(mBufLabel), "%d", value);
	inf.pLabel->setText(mBufLabel);
}

bool PhyAnimating::qtGlobalInit()
{
#if CONFIG_PROC_HAVE_DRIVERS
	Guard lock(mtxGlobalInit);
#endif
	if (globalInitDone)
	{
		//procErrLog(-1, "multiple instances of PhyAnimating() not supported");
		return true;
	}

	procDbgLog(LOG_LVL, "global Qt init");

	pAppQt = new (nothrow) QApplication(qtArgc, pQtArgv);
	if (!pAppQt)
	{
		procErrLog(-1, "could not create Qt application");
		return false;
	}

	Processing::globalDestructorRegister(qtGlobalDestruct);

	globalInitDone = true;

	return true;
}

/* static functions */

void PhyAnimating::qtGlobalDestruct()
{
	dbgLog(LOG_LVL, "global Qt deinit");
#if CONFIG_PROC_HAVE_DRIVERS
	Guard lock(mtxGlobalInit);
#endif
	dbgLog(LOG_LVL, "quitting Qt application");
	QApplication::quit();
	dbgLog(LOG_LVL, "quitting Qt application. Done");

	dbgLog(LOG_LVL, "deleting Qt application");
	delete pAppQt;
	pAppQt = NULL;
	dbgLog(LOG_LVL, "deleting Qt application. Done");
}

