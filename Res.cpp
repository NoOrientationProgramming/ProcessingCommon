/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  Copyright (C) 2017 Authors and www.dsp-crowd.com

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

#include "Res.h"

static const Resource *pTableStart = NULL;
static const Resource *pTableEnd = NULL;

bool resourceTableAdd(const Resource *pStart, const Resource *pEnd)
{
	if (!pStart || !pEnd)
		return false;

	if (pStart >= pEnd)
		return false;

	if (pTableStart || pTableEnd)
		return false;

	pTableStart = pStart;
	pTableEnd = pEnd;

	return true;
}

const Resource *resourceFind(const char *pName)
{
	if (!pTableStart || !pTableEnd)
		return NULL;

	const Resource *pRes = pTableStart;

	for (; pRes != pTableEnd; ++pRes)
	{
		if (!strcmp(pRes->name, pName))
			return pRes;
	}

	return NULL;
}

