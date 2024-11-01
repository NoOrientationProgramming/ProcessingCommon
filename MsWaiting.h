/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 02.10.2024

  Copyright (C) 2024, Johannes Natter

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

#ifndef MS_WAITING_H
#define MS_WAITING_H

#include "Processing.h"

class MsWaiting : public Processing
{

public:

	static MsWaiting *create(uint32_t duration)
	{
		return new dNoThrow MsWaiting(duration);
	}

protected:

	MsWaiting(uint32_t duration);
	virtual ~MsWaiting() {}

private:

	MsWaiting() = delete;
	MsWaiting(const MsWaiting &) = delete;
	MsWaiting &operator=(const MsWaiting &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	/* member variables */
	uint32_t mStartMs;
	uint32_t mDurationMs;
	uint32_t mDiffMs;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

