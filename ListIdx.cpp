/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 15.09.2022

  Copyright (C) 2022, Johannes Natter

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

#include <cstring>

#include "ListIdx.h"

using namespace std;

#define dMinSizeJump		1
#define dDefaultSizeJump		3

ListIdx::ListIdx()
	: mWin(0)
	, mSize(0)
	, mSizeJump(dDefaultSizeJump)
	, mCursor(0)
	, mOffset(0)
	, mCursorLbound(0)
	, mCursorUbound(0)
{
}

ListIdx::ListIdx(uint32_t win)
	: mSize(0)
	, mSizeJump(dDefaultSizeJump)
	, mCursor(0)
	, mOffset(0)
	, mCursorLbound(0)
	, mCursorUbound(0)
{
	winSet(win);
}

ListIdx::~ListIdx()
{
}

/* member functions */

void ListIdx::winSet(uint32_t win)
{
	mWin = win;
	sanityCheck();

	uint32_t bnd = mWin >> 1;

	if (!(mWin & 1))
		--bnd;

	cursorLboundSet(bnd);
	cursorUboundSet(mWin >> 1);
}

void ListIdx::sizeSet(uint32_t size)
{
	mSize = size;
	sanityCheck();
}

void ListIdx::sizeMaxSet(uint32_t size)
{
	mSizeMax = size;
}

ListIdx &ListIdx::operator=(uint32_t size)
{
	sizeSet(size);
	return *this;
}

// Resize if necessary
void ListIdx::sanityCheck()
{
	uint32_t hardUbound = 0;

	if (mOffset && (mOffset + mWin) > mSize)
	{
		mCursor += mOffset + mWin - mSize;
		mOffset = mSize > mWin ? mSize - mWin : 0;
	}

	if (mWin && mSize)
		hardUbound = (mWin < mSize ? mWin : mSize) - 1;

	if (mCursor > hardUbound)
		mCursor = hardUbound;
}

void ListIdx::sizeJumpSet(uint16_t size)
{
	mSizeJump = size < dMinSizeJump ? dMinSizeJump : size;
}

void ListIdx::cursorLboundSet(uint32_t bnd)
{
	if (bnd < mWin)
		mCursorLbound = bnd;
	else
	if (mWin)
		mCursorLbound = mWin - 1;
	else
		mCursorLbound = 0;
}

void ListIdx::cursorUboundSet(uint32_t bnd)
{
	if (bnd < mWin)
		mCursorUbound = bnd;
	else
	if (mWin)
		mCursorUbound = mWin - 1;
	else
		mCursorUbound = 0;
}

void ListIdx::cursorBoundSet(uint32_t bnd)
{
	if (!mWin)
		return;

	if (mWin < (bnd + 1))
		bnd = mWin - 1;

	cursorLboundSet(bnd);
	cursorUboundSet(mWin - 1 - bnd);
}

void ListIdx::textModeSet()
{
	cursorLboundSet(mWin);
	cursorUboundSet(0);
}

uint32_t ListIdx::win() const
{
	return mWin;
}

uint32_t ListIdx::size() const
{
	return mSize;
}

bool ListIdx::keyProcess(const KeyUser &key, const char *pListKeysDisabled)
{
	KeyUser keyCmp = key;

	if (keyCmp == keyPgUp)
		keyCmp.mVal = 'K';

	if (keyCmp == keyPgDn)
		keyCmp.mVal = 'J';

	bool disabled = false;

	for (; pListKeysDisabled && *pListKeysDisabled; ++pListKeysDisabled)
	{
		if (keyCmp != *pListKeysDisabled)
			continue;

		disabled = true;
		break;
	}

	if (keyIsDown(key) && !disabled)
		return inc();

	if (keyIsUp(key) && !disabled)
		return dec();

	if (keyIsDownJump(key) && !disabled)
		return incJump();

	if (keyIsUpJump(key) && !disabled)
		return decJump();

	if (keyIsHome(key) && !disabled)
		return reset();

	if (keyIsEnd(key) && !disabled)
		return cursorEndSet();

	return false;
}

uint32_t ListIdx::cursor() const
{
	return mCursor;
}

uint32_t ListIdx::offset() const
{
	return mOffset;
}

uint32_t ListIdx::cursorAbs() const
{
	return mOffset + mCursor;
}

bool ListIdx::endReached() const
{
	return mOffset + mWin >= mSize;
}

// Return value = Entry has been processed
bool ListIdx::winEndPrint(string &msg,
			size_t idxRel, size_t idxAbs,
			const string &prefix,
			const string &postfix,
			size_t width) const
{
	if (!mWin)
		return true;

	bool indicatorNeeded = false;
	string str = "---";

	indicatorNeeded |= !idxRel && mOffset;
	indicatorNeeded |= idxRel == mWin - 1 && !endReached();

	if (idxAbs >= mSize)
		str = "";
	else
	if (!indicatorNeeded)
		return false;

	size_t lenPad = width - str.size();

	if (width && lenPad)
		str += string(lenPad, ' ');

	msg += prefix;
	msg += str;
	msg += postfix;
	msg += "\r\n";

	return true;
}

bool ListIdx::cursorPrint(string &msg,
			size_t idxRel,
			size_t padding,
			const string &prefix) const
{
	if (!padding)
		return false;

	bool printed = idxRel == mCursor;

	msg += prefix;
	msg.push_back(printed ? '>' : ' ');

	if (--padding)
		msg += string(padding, ' ');

	return printed;
}

bool ListIdx::reset()
{
	bool changed;

	changed = (mCursor != 0) || (mOffset != 0);

	mCursor = 0;
	mOffset = 0;

	return changed;
}

bool ListIdx::cursorEndSet()
{
	if (!mWin || !mSize)
		return false;

	uint32_t hardUbound;
	bool changed;

	hardUbound = (mWin < mSize ? mWin : mSize) - 1;
	changed = (mCursor != hardUbound) || !endReached();

	mCursor = hardUbound;
	mOffset = mSize > mWin ? mSize - mWin : 0;

	return changed;
}

bool ListIdx::incOffset()
{
	if (endReached())
		return false;

	++mOffset;

	return true;
}

bool ListIdx::decOffset()
{
	if (!mOffset)
		return false;

	--mOffset;

	return true;
}

bool ListIdx::incCursor()
{
	if (!mWin || !mSize)
		return false;

	uint32_t hardUbound = (mWin < mSize ? mWin : mSize) - 1;

	if (mCursor == hardUbound)
		return false;

	++mCursor;

	return true;
}

bool ListIdx::decCursor()
{
	if (!mCursor)
		return false;

	--mCursor;

	return true;
}

bool ListIdx::inc()
{
	if (!mWin || !mSize)
		return false;

	uint32_t hardUbound = (mWin < mSize ? mWin : mSize) - 1;

	if (mCursor >= mCursorUbound && !endReached())
		++mOffset;
	else
	if (mCursor < hardUbound)
		++mCursor;
	else
		return false;

	return true;
}

bool ListIdx::dec()
{
	if (mCursor <= mCursorLbound && mOffset)
		--mOffset;
	else
	if (mCursor)
		--mCursor;
	else
		return false;

	return true;
}

bool ListIdx::incJump()
{
	bool changed = false;

	for (uint16_t i = 0; i < mSizeJump; ++i)
		changed |= inc();

	return changed;
}

bool ListIdx::decJump()
{
	bool changed = false;

	for (uint16_t i = 0; i < mSizeJump; ++i)
		changed |= dec();

	return changed;
}

void ListIdx::insert()
{
	sizeSet(mSize + 1);
	inc();
}

ListIdx &ListIdx::operator++()
{
	sizeSet(mSize + 1);
	return *this;
}

ListIdx &ListIdx::operator+=(uint32_t num)
{
	sizeSet(mSize + num);
	return *this;
}

bool ListIdx::currErase()
{
	if (!mSize)
		return false;

	if (endReached() && mSize > mWin)
	{
		decOffset();
		incCursor();
	}

	sizeSet(mSize - 1);

	return true;
}

bool ListIdx::prevErase()
{
	if (!cursorAbs())
		return false;

	if (endReached() && mSize > mWin)
		decOffset();
	else
		dec();

	sizeSet(mSize - 1);

	return true;
}

ListIdx &ListIdx::operator--()
{
	if (!mSize)
		return *this;

	sizeSet(mSize - 1);
	return *this;
}

ListIdx &ListIdx::operator-=(uint32_t num)
{
	if (num > mSize)
		num = mSize;

	sizeSet(mSize - num);
	return *this;
}

/* static functions */

