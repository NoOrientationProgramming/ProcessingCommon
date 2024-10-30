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

#ifndef WIDGET_H
#define WIDGET_H

#include <string>
#include <locale>
#include <codecvt>

#include "KeyUser.h"

class Widget
{

public:
	Widget();
	virtual ~Widget() {}

	std::string mName;
	std::string mLabel;
	std::string mTag;

	bool focus() const;
	virtual void focusSet(bool foc, bool accept = true);

	virtual const std::string &current() const;
	virtual operator std::string() const;
	virtual void currentSet(const std::string &str);
	virtual Widget &operator=(const std::string &str);

	void modifierFrameSet(const std::string &str);
	void modifierContentSet(const std::string &str);
	void modifierSelectionSet(const std::string &str);

	// I/O
	virtual bool keyProcess(const KeyUser &key, const char *pListKeysDisabled = NULL) = 0;
	virtual bool print(std::string &str) = 0;

	// Helper
	static void strToUtf(const std::string &str, std::u32string &ustr);
	static void utfToStr(const std::u32string &ustr, std::string &str);
	static void utfStrAdd(std::u32string &ustr, const std::string &str);
	static void strPadCutTo(std::string &str, size_t width, bool dots = false, bool padLeft = false);

protected:

	/* member variables */
	bool mFocus;
	std::string mCurrent;
	std::string mWork;
	std::string mModifierFrame;
	std::string mModifierContent;
	std::string mModifierSelection;

	/* static functions */

	/* static variables */

private:

};

#endif

