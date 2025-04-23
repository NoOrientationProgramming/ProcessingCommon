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

	/* configuration */

	void widthSet(uint16_t width);
	void lenMaxSet(uint16_t lenMax)		{ mLenMax = lenMax;		}
	void cursorBoundSet(uint32_t bnd);

	void passwordModeSet(bool mode = true)	{ mPasswordMode = mode;	}
	void numbersOnlySet(bool only = true)	{ mNumbersOnly = only;	}

	void focusSet(bool foc, bool accept = true);
	void selectionSet(uint16_t idxStart = 0, uint16_t len = 0);

	void frameEnabledSet(bool val = true);
	void paddingEnabledSet(bool val = true);
	void cursorShow(bool val = true);

	/* input */

	void currentSet(const std::string &str);
	TextBox &operator=(const std::string &str);

	void paste(const std::string &str);

	/* input / output */

	// Return
	// true  .. Key processed
	// false .. Key ignored
	bool keyProcess(const KeyUser &key, const char *pListKeysDisabled = NULL);

	/* output */

	operator std::string() const;

	// Return
	// true  .. Widget needs to be redrawn
	// false .. Widget stays the same
	bool dirty();

	// Return
	// true  .. Printing finished
	// false .. Printing not finished
	bool print(std::string &msg);

	std::string clipboard(bool clear = false);

	size_t sizeDisplayed() const;
	uint32_t cursorAbsFront() const;
	uint32_t cursorAbsBack() const;

private:

	void clipboardSet();
	void selectionReplace(const std::string &str = "");
	bool navigate(const KeyUser &key);
	bool cursorJump(const KeyUser &key);
	ListIdx &listIdxLow();
	ListIdx &listIdxHigh();
	bool dirtySet(bool dirty = true);

	/* member variables */
	size_t mWidth;
	size_t mLenMax;
	std::u32string mUstrCurrent;
	std::u32string mUstrWork;
	std::string mClipboard;
	ListIdx mIdxFront;
	ListIdx mIdxBack;
	bool mPasswordMode;
	bool mNumbersOnly;
	bool mDirty;
	bool mFrameEnabled;
	bool mPaddingEnabled;
	bool mCursorShow;

	/* static functions */

	/* static variables */

};

#endif

