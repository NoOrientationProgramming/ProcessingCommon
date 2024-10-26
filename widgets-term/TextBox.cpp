/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 31.01.2023

  Copyright (C) 2023, Johannes Natter

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

#include "TextBox.h"

using namespace std;

#define dSizeWidthMin	4

TextBox::TextBox()
	: mWidth(10)
	, mLenMax(64)
	, mUtfCurrent()
	, mUtfWork()
	, mIdxChars()
	, mCursorPrinted(false)
	, mPasswordMode(false)
	, mNumbersOnly(false)
{
	mIdxChars.winSet(mWidth);
	currentSet("");
}

void TextBox::widthSet(uint16_t width)
{
	if (width < dSizeWidthMin)
		return;

	mWidth = width;
	mIdxChars.winSet(width - dSizeWidthMin);
}

void TextBox::lenMaxSet(uint16_t lenMax)
{
	mLenMax = lenMax;
}

void TextBox::cursorBoundSet(uint32_t bnd)
{
	mIdxChars.cursorBoundSet(bnd);
}

void TextBox::passwordModeSet(bool mode)
{
	mPasswordMode = mode;
}

void TextBox::numbersOnlySet(bool only)
{
	mNumbersOnly = only;
}

void TextBox::focusSet(bool foc, bool accept)
{
	mFocus = foc;

	if (foc)
	{
		// 1
		mUtfWork = mUtfCurrent;

		// 2
		mIdxChars = mUtfWork.size() + 1;

		// 3
		mIdxChars.cursorEndSet();
		return;
	}

	if (accept)
	{
		// 1
		mUtfCurrent = mUtfWork;
		utfToStr(mUtfCurrent, mCurrent);

		// 2
		mIdxChars = mUtfCurrent.size() + 1;
	}

	// 3
	mIdxChars.reset();
}

TextBox::operator string() const
{
	return current();
}

void TextBox::currentSet(const string &str)
{
	mCurrent = str;
	strToUtf(mCurrent, mUtfCurrent);

	if (mFocus)
		return;

	mIdxChars = mUtfCurrent.size() + 1;
}

TextBox &TextBox::operator=(const string &str)
{
	currentSet(str);
	return *this;
}

bool TextBox::keyProcess(const KeyUser &key, const char *pListKeysDisabled)
{
	(void)pListKeysDisabled;

	if (!mFocus)
		return false;

	// Navigation

	if (key == keyHome)
		return mIdxChars.reset();

	if (key == keyEnd)
		return mIdxChars.cursorEndSet();

	if (key == keyLeft)
		return mIdxChars.dec();

	if (key == keyRight)
		return mIdxChars.inc();

	if (key == keyPgUp || key == keyPgDn)
		return mIdxChars.keyProcess(key);

	// Removal

	if (keyIsBackspace(key) and mIdxChars.prevErase())
	{
		mUtfWork.erase(mIdxChars.cursorAbs(), 1);
		return true;
	}

	if (key == keyDelete)
	{
		// TextBox behavior
		if (mIdxChars.cursorAbs() == mIdxChars.size() - 1)
			return false;

		// 1. Backup cursorAbs
		uint32_t cursorAbs = mIdxChars.cursorAbs();

		// 2. Change list
		if (!mIdxChars.currErase())
			return false;

		// 3. Change text
		mUtfWork.erase(cursorAbs, 1);

		return true;
	}

	// Insertion

	if (mNumbersOnly and (key < '0' or key > '9'))
		return false;

	const char *pListKeysExt = "@.";
	bool extFound = false;

	for (; *pListKeysExt; ++pListKeysExt)
	{
		if (key != *pListKeysExt)
			continue;

		extFound = true;
		break;
	}

	if (keyIsCommon(key) or extFound)
	{
		// TextBox behavior
		if (mUtfWork.size() >= mLenMax)
			return false;

		// 1. Change text
		mUtfWork.insert(mIdxChars.cursorAbs(), 1, key);

		// 2. Change list
		mIdxChars.insert();

		return true;
	}

	return false;
}

bool TextBox::print(string &msg)
{
	u32string strIn = mFocus ? mUtfWork : mUtfCurrent;
	u32string strUtfPrint;
	u32string strUtfModFrame, strUtfModContent, strUtfTmp;
	string strPrint;
	size_t idxAbs = mIdxChars.offset();
	size_t idxRel = 0;

	strIn.push_back(' ');

	if (mModifierFrame.size())
	{
		strToUtf(mModifierFrame, strUtfModFrame);
		strUtfPrint += strUtfModFrame;
	}

	if (mModifierContent.size())
		strToUtf(mModifierContent, strUtfModContent);

	if (mFocus)
		strUtfPrint.push_back('>');
	else
		strUtfPrint.push_back('|');
	strUtfPrint.push_back(' ');
	utfStrAdd(strUtfPrint, "\033[0m");

	mCursorPrinted = false;

	for (; idxRel < mIdxChars.win(); ++idxRel, ++idxAbs)
	{
		if (strUtfModContent.size())
			strUtfPrint += strUtfModContent;

		if (!idxRel and mIdxChars.offset())
		{
			utfStrAdd(strUtfPrint, "\u00AB");
			continue;
		}

		if (idxRel == mIdxChars.win() - 1 and !mIdxChars.endReached())
		{
			utfStrAdd(strUtfPrint, "\u00BB");
			continue;
		}

		if (idxAbs >= strIn.size())
		{
			strUtfPrint.push_back(' ');
			utfStrAdd(strUtfPrint, "\033[0m");
			continue;
		}

		if (idxRel == mIdxChars.cursor())
			cursorActivate(strUtfPrint);

		if (mPasswordMode and idxAbs < strIn.size() - 1)
			strUtfPrint.push_back('*');
		else
			strUtfPrint.push_back(strIn[idxAbs]);

		utfStrAdd(strUtfPrint, "\033[0m");
	}

	if (strUtfModFrame.size())
		strUtfPrint += strUtfModFrame;

	strUtfPrint.push_back(' ');
	if (mFocus)
		strUtfPrint.push_back('<');
	else
		strUtfPrint.push_back('|');
	utfStrAdd(strUtfPrint, "\033[0m");

	utfToStr(strUtfPrint, strPrint);
	msg += strPrint;

	return true;
}

void TextBox::cursorActivate(u32string &strUtf)
{
	if (mCursorPrinted or !mFocus)
		return;

	utfStrAdd(strUtf, "\033[4m");
	mCursorPrinted = true;
}

