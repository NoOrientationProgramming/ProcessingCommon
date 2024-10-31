/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 07.02.2023

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

#ifndef KEY_USER_H
#define KEY_USER_H

#define KEY_USER_DEBUG 0

#if KEY_USER_DEBUG
#include <iostream>
#endif
#include <cstdint>
#include <locale>
#include <codecvt>

class KeyUser
{

public:

	KeyUser()
		: mVal(0)
		, mModShift(false)
		, mModAlt(false)
		, mModCtrl(false)
		, mIsValid(false)
	{
	}

	KeyUser(uint16_t key)
		: mVal(key)
		, mModShift(false)
		, mModAlt(false)
		, mModCtrl(false)
		, mIsValid(true)
	{
#if KEY_USER_DEBUG
		std::cout << "KeyUser(uint16_t key): key  = " << key << std::endl;
		std::cout << "KeyUser(uint16_t key): mVal = " << mVal << std::endl;
#endif
	}

	bool operator==(const char &other) const
	{
		if (!mIsValid) return false;
#if KEY_USER_DEBUG
		std::cout << "operator==(char):      mVal = " << mVal << ", other = " << other << std::endl;
#endif
		return mVal == other;
	}

	bool operator!=(const char &other) const
	{
		if (!mIsValid) return false;
#if KEY_USER_DEBUG
		std::cout << "operator!=(char):      mVal = " << mVal << ", other = " << other << std::endl;
#endif
		return mVal != other;
	}

	bool operator==(const uint16_t &other) const
	{
		if (!mIsValid) return false;
#if KEY_USER_DEBUG
		std::cout << "operator==(uint16_t):  mVal = " << mVal << ", other = " << other << std::endl;
#endif
		return mVal == other;
	}

	bool operator!=(const uint16_t &other) const
	{
		if (!mIsValid) return false;
#if KEY_USER_DEBUG
		std::cout << "operator!=(uint16_t):  mVal = " << mVal << ", other = " << other << std::endl;
#endif
		return mVal != other;
	}

	KeyUser &operator=(uint16_t key)
	{
		mVal = key;
		mIsValid = true;
#if KEY_USER_DEBUG
		std::cout << "operator=(uint16_t): mVal = " << mVal << std::endl;
#endif
		return *this;
	}

	operator uint16_t() const
	{
#if KEY_USER_DEBUG
		std::cout << "operator uint16_t(): mVal = " << mVal << std::endl;
#endif
		return mVal;
	}

	uint16_t mVal;
	bool mModShift;
	bool mModAlt;
	bool mModCtrl;

private:

	/* member variables */
	bool mIsValid;

};

const uint16_t keyBackspace    = 0x7F;
const uint16_t keyBackspaceWin = 0x08;
const uint16_t keyEnter        = 0x0D;
const uint16_t keyEsc          = 0x1B;
const uint16_t keyCtrlV        = 0x16;
const uint16_t keyCtrlX        = 0x18;
const uint16_t keyCtrlC        = 0x03;
const uint16_t keyCtrlD        = 0x04;
const uint16_t keyTab          = 0x09;
const uint16_t keyHelp         = '?';
const uint16_t keyOpt          = '=';
const uint16_t keySpace        = ' ';

enum KeyExtensions : uint16_t
{
	keyUp = 1000,
	keyDown,
	keyLeft,
	keyRight,
	keyHome,
	keyInsert,
	keyDelete,
	keyEnd,
	keyPgUp,
	keyPgDn,
	keyF0,	keyF1,	keyF2,	keyF3,
	keyF4,	keyF5,	keyF6,	keyF7,
	keyF8,	keyF9,	keyF10,	keyF11,
	keyF12,	keyF13,	keyF14,	keyF15,
	keyF16,	keyF17,	keyF18,	keyF19,
	keyF20,
};

bool keyIsAlphaNum(const KeyUser &key);
bool keyIsNum(const KeyUser &key);
bool keyIsCommon(const KeyUser &key);
bool keyIsCtrl(const KeyUser &key);
bool keyIsAccept(const KeyUser &key);
bool keyIsAbort(const KeyUser &key);
bool keyIsBackspace(const KeyUser &key);
bool keyIsUserDisconnect(const KeyUser &key);
bool keyIsHome(const KeyUser &key);
bool keyIsEnd(const KeyUser &key);
bool keyIsUp(const KeyUser &key);
bool keyIsDown(const KeyUser &key);
bool keyIsUpJump(const KeyUser &key);
bool keyIsDownJump(const KeyUser &key);

// Unicode

void strToUtf(const std::string &str, std::u32string &ustr);
void utfToStr(const std::u32string &ustr, std::string &str);
void utfStrAdd(std::u32string &ustr, const std::string &str);

#endif

