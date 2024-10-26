/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 24.10.2024

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

#include "KeyUser.h"

using namespace std;

bool keyIsAlphaNum(const KeyUser &key)
{
	if (key.mVal >= 'a' and key.mVal <= 'z')
		return true;

	if (key.mVal >= 'A' and key.mVal <= 'Z')
		return true;

	if (keyIsNum(key))
		return true;

	return false;
}

bool keyIsNum(const KeyUser &key)
{
	if (key.mVal >= '0' and key.mVal <= '9')
		return true;

	return false;
}

bool keyIsCommon(const KeyUser &key)
{
	if (keyIsAlphaNum(key))
		return true;

	if (key.mVal == ' ' or key.mVal == '-' or key.mVal == '_')
		return true;

	if (key.mVal == keyHelp)
		return true;

	return false;
}

bool keyIsCtrl(const KeyUser &key)
{
	if (key.mVal == keyEnter or key.mVal == keyEsc)
		return true;

	if (key.mVal == keyUp or key.mVal == keyDown)
		return true;

	if (key.mVal == keyLeft or key.mVal == keyRight)
		return true;

	if (key.mVal == keyHome or key.mVal == keyEnd)
		return true;

	if (key.mVal == keyPgUp or key.mVal == keyPgDn)
		return true;

	if (key.mVal == keyBackspace or key.mVal == keyBackspaceWin)
		return true;

	if (key.mVal == keyDelete)
		return true;

	return false;
}

bool keyIsAccept(const KeyUser &key)
{
	if (key.mVal == keyEnter or key.mVal == 'l')
		return true;

	return false;
}

bool keyIsAbort(const KeyUser &key)
{
	if (key.mVal == keyEsc or key.mVal == 'q')
		return true;

	return false;
}

bool keyIsBackspace(const KeyUser &key)
{
	return (key.mVal == keyBackspace) or (key.mVal == keyBackspaceWin);
}

bool keyIsUserDisconnect(const KeyUser &key)
{
	if (key.mVal == keyCtrlC or key.mVal == keyCtrlD)
		return true;

	return false;
}

bool keyIsHome(const KeyUser &key)
{
	if (key.mVal == 'g' or key.mVal == keyHome)
		return true;

	return false;
}

bool keyIsEnd(const KeyUser &key)
{
	if (key.mVal == 'G' or key.mVal == keyEnd)
		return true;

	return false;
}

bool keyIsUp(const KeyUser &key)
{
	if (key.mVal == 'k' or key.mVal == keyUp)
		return true;

	return false;
}

bool keyIsDown(const KeyUser &key)
{
	if (key.mVal == 'j' or key.mVal == keyDown)
		return true;

	return false;
}

bool keyIsUpJump(const KeyUser &key)
{
	if (key.mVal == 'K' or key.mVal == keyPgUp)
		return true;

	return false;
}

bool keyIsDownJump(const KeyUser &key)
{
	if (key.mVal == 'J' or key.mVal == keyPgDn)
		return true;

	return false;
}

