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
	, mUstrCurrent()
	, mUstrWork()
	, mIdxFront()
	, mIdxBack()
	, mPasswordMode(false)
	, mNumbersOnly(false)
	, mDirty(false)
{
	mIdxFront.winSet(mWidth);
	mIdxBack.winSet(mIdxFront.win());

	currentSet("");
}

void TextBox::widthSet(uint16_t width)
{
	if (width < dSizeWidthMin)
		return;

	mWidth = width;

	mIdxFront.winSet(width - dSizeWidthMin);
	mIdxBack.winSet(mIdxFront.win());
}

void TextBox::lenMaxSet(uint16_t lenMax)
{
	mLenMax = lenMax;
}

void TextBox::cursorBoundSet(uint32_t bnd)
{
	mIdxFront.cursorBoundSet(bnd);
	mIdxBack.cursorBoundSet(bnd);
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
		mUstrWork = mUstrCurrent;

		// 2
		mIdxFront = mUstrWork.size() + 1;

		// 3
		mIdxFront.cursorEndSet();
		mIdxBack = mIdxFront;
		return;
	}

	if (accept)
	{
		// 1
		mUstrCurrent = mUstrWork;
		utfToStr(mUstrCurrent, mCurrent);
	}

	// 2
	mIdxFront = mUstrCurrent.size() + 1;

	// 3
	mIdxFront.reset();
	mIdxBack = mIdxFront;
}

TextBox::operator string() const
{
	return current();
}

void TextBox::currentSet(const string &str)
{
	mCurrent = str;
	strToUtf(mCurrent, mUstrCurrent);

	if (mFocus)
		return;

	mIdxFront = mUstrCurrent.size() + 1;
	mIdxBack = mIdxFront.size();
}

TextBox &TextBox::operator=(const string &str)
{
	currentSet(str);
	return *this;
}

bool TextBox::keyProcess(const KeyUser &key, const char *pListKeysDisabled)
{
	(void)pListKeysDisabled;
	mDirty = false;

	// TODO: Cut/Copy

	// Indexes can be changed during focus mode only
	if (!mFocus)
		return false;

	// Navigation

	// mIdxFront may change here
	if (navigate(key))
		return dirtySet();

	// Removal

	if (keyIsBackspace(key) and mIdxFront.prevErase())
	{
		mUstrWork.erase(mIdxFront.cursorAbs(), 1);
		mIdxBack = mIdxFront;
		return dirtySet();
	}

	if (key == keyDelete)
	{
		// TextBox behavior
		if (mIdxFront.cursorAbs() == mIdxFront.size() - 1)
			return false;

		// 1. Backup cursorAbs
		uint32_t cursorAbs = mIdxFront.cursorAbs();

		// 2. Change list
		if (!mIdxFront.currErase())
			return false;
		mIdxBack = mIdxFront;

		// 3. Change text
		mUstrWork.erase(cursorAbs, 1);

		return dirtySet();
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
		if (mUstrWork.size() >= mLenMax)
			return false;

		// 1. Change text
		mUstrWork.insert(mIdxFront.cursorAbs(), 1, key);

		// 2. Change list
		mIdxFront.insert();
		mIdxBack = mIdxFront;

		return dirtySet();
	}

	return false;
}

bool TextBox::dirty()
{
	bool bkupDirty = mDirty;
	mDirty = false;
	return bkupDirty;
}

bool TextBox::print(string &msg)
{
	u32string strIn = mFocus ? mUstrWork : mUstrCurrent;
	u32string ustrPrint;
	string strPrint;
	size_t idxAbs = mIdxFront.offset();
	size_t idxRel = 0;
	bool cursorPrint;
	char32_t ch;

	size_t idxFront = mIdxFront.cursor();
	size_t idxBack = mIdxBack.cursor();
	bool frontIsHigh = idxFront > idxBack;
	size_t idxRelHigh = frontIsHigh ? idxFront : idxBack;
	size_t idxRelLow = frontIsHigh ? idxBack : idxFront;
	bool selectionPrint = mFocus && idxRelLow != idxRelHigh;

	// extend tmp string
	strIn.push_back(' ');

	// begin: Frame
	utfStrAdd(ustrPrint, mModifierFrame);
	if (mFocus)
		ustrPrint.push_back('>');
	else
		ustrPrint.push_back('|');
	ustrPrint.push_back(' ');
	utfStrAdd(ustrPrint, "\033[0m");
	// end: Frame

	// begin: Content
	utfStrAdd(ustrPrint, mModifierContent);

	for (; idxRel < mIdxFront.win(); ++idxRel, ++idxAbs)
	{
		if (!idxRel and mIdxFront.offset())
		{
			utfStrAdd(ustrPrint, "\u00AB");
			continue;
		}

		if (idxRel == mIdxFront.win() - 1 and !mIdxFront.endReached())
		{
			utfStrAdd(ustrPrint, "\u00BB");
			continue;
		}

		if (idxAbs >= strIn.size())
			ch = ' ';
		else
		if (mPasswordMode and idxAbs < strIn.size() - 1)
			ch = '*';
		else
			ch = strIn[idxAbs];

		if (selectionPrint && idxRel == idxRelLow)
			utfStrAdd(ustrPrint, mModifierSelection);

		if (selectionPrint && idxRel == idxRelHigh)
		{
			utfStrAdd(ustrPrint, "\033[0m");

			// restore after reset only
			utfStrAdd(ustrPrint, mModifierContent);
		}

		cursorPrint = mFocus && idxRel == mIdxFront.cursor();

		if (cursorPrint)
			utfStrAdd(ustrPrint, "\033[4m");

		ustrPrint.push_back(ch);

		if (cursorPrint)
		{
			utfStrAdd(ustrPrint, "\033[0m");

			// restore after reset only
			bool isSelection = selectionPrint && idxRel < idxRelHigh;

			utfStrAdd(ustrPrint,
				isSelection ? mModifierSelection : mModifierContent);
		}
	}

	utfStrAdd(ustrPrint, "\033[0m");
	// end: Content

	// begin: Frame
	utfStrAdd(ustrPrint, mModifierFrame);
	ustrPrint.push_back(' ');
	if (mFocus)
		ustrPrint.push_back('<');
	else
		ustrPrint.push_back('|');
	utfStrAdd(ustrPrint, "\033[0m");
	// end: Frame

	utfToStr(ustrPrint, strPrint);
	msg += strPrint;

	return true;
}

bool TextBox::navigate(const KeyUser &key)
{
	bool processed = false;
	bool changed = false;

	if (key == keyHome)
		processed = true, changed = mIdxFront.reset();
	else
	if (key == keyEnd)
		processed = true, changed = mIdxFront.cursorEndSet();
	else
	if (key.mModCtrl && (key == keyLeft || key == keyRight))
		processed = true, changed = cursorJump(key);
	else
	if (key == keyLeft || key == keyUp)
		processed = true, changed = mIdxFront.dec();
	else
	if (key == keyRight || key == keyDown)
		processed = true, changed = mIdxFront.inc();
	else
	if (key == keyPgUp || key == keyPgDn)
		processed = true, changed = mIdxFront.keyProcess(key);

	if (processed && !key.mModShift)
	{
		mIdxBack = mIdxFront;
		changed = true;
	}

	return changed;
}

bool TextBox::cursorJump(const KeyUser &key)
{
	if (!mUstrWork.size())
		return false;

	bool isRight = key == keyRight;
	int direction = isRight ? 1 : -1;
	bool statePrev = (direction + 1) >> 1;
	bool stateCursor = !statePrev;
	uint16_t idxAbs = mIdxFront.cursorAbs();
	uint16_t idxStop = statePrev ? mUstrWork.size() : 0;
	bool changed = false;

	const char32_t *pCursor = mUstrWork.c_str() + idxAbs;
	const char32_t *pPrev = NULL;

	while (true)
	{
		if (idxAbs == idxStop)
			break;

		changed = true;

		if (isRight)
			mIdxFront.inc();
		else
			mIdxFront.dec();

		pCursor += direction;
		idxAbs += direction;

		if (idxAbs)
			pPrev = pCursor - 1;

		if (!pPrev)
			continue;

		if (keyIsAlphaNum(*pPrev) == statePrev &&
			keyIsAlphaNum(*pCursor) == stateCursor)
			break;
	}

	return changed;
}

bool TextBox::dirtySet(bool dirty)
{
	mDirty = dirty;
	return mDirty;
}

