/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 01.02.2024

  Copyright (C) 2024-now Authors and www.dsp-crowd.com

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

#ifndef THREAD_POOLING_H
#define THREAD_POOLING_H

#include "Processing.h"
#include "Pipe.h"

typedef void (*FctDriverCreate)(Processing *pProc, uint16_t idProc);

struct PoolRequest
{
	Processing *pProc;
	int32_t idDriverDesired;
};

class ThreadPooling : public Processing
{

public:

	static ThreadPooling *create()
	{
		return new (std::nothrow) ThreadPooling;
	}

	void workerCntSet(uint16_t cnt);
	void driverCreateFctSet(FctDriverCreate pFctDriverCreate);

	static void procAdd(Processing *pProc, int32_t idDriver = -1);

protected:

	ThreadPooling();
	virtual ~ThreadPooling() {}

private:

	ThreadPooling(const ThreadPooling &) : Processing("") {}
	ThreadPooling &operator=(const ThreadPooling &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);

	int32_t idDriverNextGet();
	void procsDrive();

	/* member variables */
	uint32_t mStateSd;
	uint32_t mStartMs;

	// Broker
	uint16_t mCntInternals;
	std::vector<ThreadPooling *> mVecInternals;
	FctDriverCreate mpFctDriverCreate;

	// Internal
	bool mIsInternal;
	uint32_t mNumFinished;
	std::list<Processing *> mListProcs;

	/* static functions */

	/* static variables */
	static Pipe<PoolRequest> ppPoolRequests;

	/* constants */

};

#endif

