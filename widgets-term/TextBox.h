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

#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "ListIdx.h"
#include "Widget.h"

class TextBox : public Widget
{

public:
	TextBox();
	virtual ~TextBox() {}

	void widthSet(uint16_t width);
	void lenMaxSet(uint16_t lenMax);
	void cursorBoundSet(uint32_t bnd);

	void passwordModeSet(bool mode = true);
	void numbersOnlySet(bool only = true);

	void focusSet(bool foc, bool accept = true);

	operator std::string() const;
	void currentSet(const std::string &str);
	TextBox &operator=(const std::string &str);

	// Return
	// true  .. Key processed
	// false .. Key ignored
	bool keyProcess(const KeyUser &key, const char *pListKeysDisabled = NULL);

	// Return
	// true  .. Widget needs to be redrawn
	// false .. Widget stays the same
	bool dirty() const { return mDirty; }

	// Return
	// true  .. Printing finished
	// false .. Printing not finished
	bool print(std::string &msg);

private:

	bool navigate(const KeyUser &key);
	bool cursorJump(const KeyUser &key);
	void cursorActivate(std::u32string &ustr);
	bool dirtySet(bool dirty = true);

	/* member variables */
	size_t mWidth;
	size_t mLenMax;
	std::u32string mUstrCurrent;
	std::u32string mUstrWork;
	ListIdx mIdxChars;
	bool mCursorPrinted;
	bool mPasswordMode;
	bool mNumbersOnly;
	bool mDirty;

	/* static functions */

	/* static variables */

};

#endif

