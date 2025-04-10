/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 20.12.2022

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

#ifndef KEY_FILTERING_H
#define KEY_FILTERING_H

#include "Processing.h"
#include "Transfering.h"
#include "Pipe.h"
#include "KeyUser.h"

class KeyFiltering : public Transfering
{

public:

	// output
	Pipe<KeyUser> ppKeys;

protected:

	KeyFiltering(const char *name)
		: Transfering(name)
		, mKeyLastRcvdMs(0)
	{}
	virtual ~KeyFiltering() {}

	uint32_t mKeyLastRcvdMs;

private:

	KeyFiltering() = delete;
	KeyFiltering(const KeyFiltering &) = delete;
	KeyFiltering &operator=(const KeyFiltering &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */

	/* member variables */

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

