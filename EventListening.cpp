/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 14.09.2023

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

#include "EventListening.h"
#include "LibDspc.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StMain) \
		gen(StTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 1
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;
using namespace Json;

#if CONFIG_PROC_HAVE_DRIVERS
mutex EventListening::mMtxEvents;
#endif
map<string, Value> EventListening::mEvents;

const uint32_t cMsDelayDataMax = 300;
const uint32_t cNumOpenConnsMax = 10;
const uint32_t cMsDelayDequeueMax = 600;

EventListening::EventListening()
	: Processing("EventListening")
	//, mStartMs(0)
	, mpLst(NULL)
	, mLenBuf(0)
{
	mState = StStart;
}

/* member functions */

Success EventListening::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mStartMs;
	//Success success;
#if 0
	dStateTrace;
#endif
	switch (mState)
	{
	case StStart:

		mpLst = TcpListening::create();
		if (!mpLst)
			procErrLog(-1, "could not create process");

		mpLst->portSet(4050, true);
		//mpLst->procTreeDisplaySet(true);

		start(mpLst);

		mState = StMain;

		break;
	case StMain:

		connectionsAccept();
		dataTimeoutsCheck();
		dataReceive();
		dequeueTimeoutsCheck();

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

void EventListening::connectionsAccept()
{
	PipeEntry<int> peerFdEntry;
	int peerFd;
	TcpTransfering *pConn;
	OpenEventConn openConn;

	if (mpLst->ppPeerFd.get(peerFdEntry) < 1)
		return;
	peerFd = peerFdEntry.particle;

	if (mConnsOpen.size() >= cNumOpenConnsMax)
	{
		procErrLog(-1, "reached max open connections. Conn dropped");
		::close(peerFd);
		return;
	}

	pConn = TcpTransfering::create(peerFd);
	if (!pConn)
	{
		procErrLog(-1, "could not create process");
		::close(peerFd);
		return;
	}

	pConn->procTreeDisplaySet(false);

	start(pConn);

	openConn.msStart = millis();
	openConn.pConn = pConn;

	mConnsOpen.push_back(openConn);
}

void EventListening::dataTimeoutsCheck()
{
	uint32_t curTimeMs = millis();
	list<OpenEventConn>::iterator iter;
	uint32_t diffMs;

	iter = mConnsOpen.begin();
	while (iter != mConnsOpen.end())
	{
		diffMs = curTimeMs - iter->msStart;
		if (diffMs < cMsDelayDataMax)
		{
			++iter;
			continue;
		}

		procErrLog(-1, "data rcv timeout reached. Conn dropped");

		repel(iter->pConn);

		iter = mConnsOpen.erase(iter);
	}
}

void EventListening::dataReceive()
{
	list<OpenEventConn>::iterator iter;
	Success success;

	iter = mConnsOpen.begin();
	while (iter != mConnsOpen.end())
	{
		success = msgEnqueue(iter->pConn);

		if (success == Pending)
		{
			++iter;
			continue;
		}

		repel(iter->pConn);

		iter = mConnsOpen.erase(iter);
	}
}

Success EventListening::msgEnqueue(TcpTransfering *pConn)
{
	ssize_t lenReq;
	Reader jReader;
	bool ok;
	Value msgEvent;
	string refMsg;

	lenReq = sizeof(mBuf) - 1;
	mLenBuf = pConn->read(mBuf, lenReq);

	if (!mLenBuf)
		return Pending;

	if (mLenBuf < 0)
		return procErrLog(-1, "connection lost");

	mBuf[mLenBuf] = 0;
#if 0
	procWrnLog("received data. len = %d", mLenBuf);
	hexDump(mBuf, mLenBuf);
#endif
	ok = jReader.parse(mBuf, mBuf + mLenBuf, msgEvent);
	if (!ok)
		return procErrLog(-1, "could not parse event message");

	if (!jKeyFind(msgEvent, "refMsg"))
		return procErrLog(-1, "could not find message reference");

	if (!msgEvent["refMsg"].isString())
		return procErrLog(-1, "message reference has wrong type");

	refMsg = msgEvent["refMsg"].asString();
	msgEvent.removeMember("refMsg");

	{
#if CONFIG_PROC_HAVE_DRIVERS
		Guard lock(mMtxEvents);
#endif
		msgEvent["msEnqueued"] = millis();
		mEvents[refMsg] = msgEvent;
	}
#if 0
	{
		StyledWriter jWriter;
		string str = jWriter.write(msgEvent);
		procInfLog("\nReference: %s\n%s",
				refMsg.c_str(), str.c_str());
	}
#endif
	return Positive;
}

// msgDequeue
ssize_t EventListening::pop(const string &refMsg, Value &msgEvent)
{
#if CONFIG_PROC_HAVE_DRIVERS
	Guard lock(mMtxEvents);
#endif
	map<string, Value>::iterator iter;

	iter = mEvents.find(refMsg);
	if (iter == mEvents.end())
		return 0;

	msgEvent = iter->second;
	msgEvent.removeMember("msEnqueued");

	mEvents.erase(iter);

	return 1;
}

void EventListening::dequeueTimeoutsCheck()
{
	uint32_t curTimeMs = millis();
	map<string, Value>::iterator iter;
	uint32_t msStart;
	uint32_t diffMs;

	iter = mEvents.begin();
	while (iter != mEvents.end())
	{
		const Value &msgEvent = iter->second;
		msStart = msgEvent["msEnqueued"].asUInt();

		diffMs = curTimeMs - msStart;
		if (diffMs < cMsDelayDequeueMax)
		{
			++iter;
			continue;
		}

		procErrLog(-1, "dequeue timeout reached. Message dropped");

		iter = mEvents.erase(iter);
	}
}

void EventListening::processInfo(char *pBuf, char *pBufEnd)
{
#if 1
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#endif
	size_t numEvents;
	{
#if CONFIG_PROC_HAVE_DRIVERS
		Guard lock(mMtxEvents);
#endif
		numEvents = mEvents.size();
	}
	dInfo("Messages\t\t\t%zu\n", numEvents);
}

/* static functions */

