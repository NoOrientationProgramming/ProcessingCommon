/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 14.09.2023

  Copyright (C) 2023-now Authors and www.dsp-crowd.com

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

#ifndef EVENT_LISTENING_H
#define EVENT_LISTENING_H

#include <list>
#include <map>
#include <jsoncpp/json/json.h>

#include "Processing.h"
#include "TcpListening.h"
#include "TcpTransfering.h"

struct OpenEventConn
{
	TcpTransfering *pConn;
	uint32_t msStart;
};

class EventListening : public Processing
{

public:

	static EventListening *create()
	{
		return new (std::nothrow) EventListening;
	}

	static Success pop(
			const std::string &refMsg,
			Json::Value &msgEvent);

protected:

	EventListening();
	virtual ~EventListening() {}

private:

	EventListening(const EventListening &) : Processing("") {}
	EventListening &operator=(const EventListening &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	void processInfo(char *pBuf, char *pBufEnd);

	void connectionsAccept();
	void dataTimeoutsCheck();
	void dataReceive();
	Success msgEnqueue(TcpTransfering *pConn);
	void dequeueTimeoutsCheck();

	/* member variables */
	uint32_t mState;
	uint32_t mStartMs;
	TcpListening *mpLst;
	std::list<OpenEventConn> mConnsOpen;
	char mBuf[512];
	ssize_t mLenBuf;

	/* static functions */

	/* static variables */
	static std::mutex mMtxEvents;
	static std::map<std::string, Json::Value> mEvents;

	/* constants */

};

#endif

