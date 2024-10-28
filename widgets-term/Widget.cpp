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

#include "Widget.h"

using namespace std;

Widget::Widget()
	: mName("")
	, mLabel("")
	, mTag("")
	, mFocus(false)
	, mCurrent("")
	, mModifierFrame("")
	, mModifierContent("")
{
}

bool Widget::focus() const
{
	return mFocus;
}

void Widget::focusSet(bool foc, bool accept)
{
	if (foc)
		mWork = mCurrent;

	if (!foc and accept)
		mCurrent = mWork;

	mFocus = foc;
}

const string &Widget::current() const
{
	return mCurrent;
}

// https://en.cppreference.com/w/cpp/language/cast_operator
Widget::operator string() const
{
	return current();
}

void Widget::currentSet(const string &str)
{
	mCurrent = str;
}

Widget &Widget::operator=(const string &str)
{
	mCurrent = str;
	return *this;
}

void Widget::modifierFrameSet(const string &str)
{
	mModifierFrame = str;
}

void Widget::modifierContentSet(const string &str)
{
	mModifierContent = str;
}

/* static functions */

void Widget::strToUtf(const string &str, u32string &ustr)
{
	wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
	ustr = converter.from_bytes(str);
}

void Widget::utfToStr(const u32string &ustr, string &str)
{
	wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
	str = converter.to_bytes(ustr);
}

void Widget::utfStrAdd(u32string &ustr, const string &str)
{
	u32string ustrTmp;
	strToUtf(str, ustrTmp);
	ustr += ustrTmp;
}

void Widget::strPadCutTo(string &str, size_t width, bool dots, bool padLeft)
{
	wstring_convert<codecvt_utf8<char32_t>, char32_t> converter;
	u32string ustr = converter.from_bytes(str);
	size_t sz = ustr.size();
	size_t szOld = sz;

	if (sz > width)
		ustr.erase(width);
	else
	if (width > sz)
	{
		if (padLeft)
			ustr.insert(0, width - sz, ' ');
		else
			ustr.append(width - sz, ' ');
	}

	if (szOld > width and width >= 2 and dots)
	{
		ustr.pop_back();
		ustr.pop_back();
		ustr.push_back('.');
		ustr.push_back('.');
	}

	str = converter.to_bytes(ustr);
}

