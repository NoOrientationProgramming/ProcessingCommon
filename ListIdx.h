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

#ifndef LIST_IDX_H
#define LIST_IDX_H

#include <cstdint>
#include <string>

#include "KeyUser.h"

class ListIdx
{

public:
	ListIdx();
	ListIdx(uint32_t win);
	virtual ~ListIdx();

	void winSet(uint32_t win);
	void sizeSet(uint32_t size);
	void sizeMaxSet(uint32_t size);
	ListIdx &operator=(uint32_t size);
	void sizeJumpSet(uint16_t size);

	void cursorLboundSet(uint32_t bnd);
	void cursorUboundSet(uint32_t bnd);
	void cursorBoundSet(uint32_t bnd);
	void textModeSet();

	uint32_t win() const;
	uint32_t size() const;

	bool keyProcess(const KeyUser &key, const char *pListKeysDisabled = NULL);
	uint32_t cursor() const;
	uint32_t offset() const;

	uint32_t cursorAbs() const;
	bool endReached() const;

	bool winEndPrint(std::string &msg,
			size_t idxRel, size_t idxAbs,
			const std::string &prefix = "  ",
			const std::string &postfix = "",
			size_t width = 0) const;
	bool cursorPrint(std::string &msg,
			size_t idxRel,
			size_t padding = 2,
			const std::string &prefix = "") const;

	bool reset();
	bool cursorEndSet();

	// Offset

	bool incOffset();
	bool decOffset();

	// Cursor

	bool incCursor();
	bool decCursor();

	// Absolute cursor

	bool inc();
	bool dec();

	bool incJump();
	bool decJump();

	// Change items

	void insert();
	ListIdx &operator++();
	ListIdx &operator+=(uint32_t num);

	bool currErase();
	bool prevErase();
	ListIdx &operator--();
	ListIdx &operator-=(uint32_t num);

private:

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */

	void sanityCheck();

	/* member variables */
	uint32_t mWin;
	uint32_t mSize;
	uint32_t mSizeMax;
	uint32_t mSizeJump;
	uint32_t mCursor;
	uint32_t mOffset;
	uint32_t mCursorLbound;
	uint32_t mCursorUbound;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

