/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 25.03.2023

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

#include <iostream>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>

#include "FileExecuting.h"
#include "LibTime.h"

#define dForEach_ProcState(gen) \
		gen(StStart) \
		gen(StInternalStart) \
		gen(StPipesCreate) \
		gen(StFork) \
		gen(StParentStart) \
		gen(StSinksReadyWait) \
		gen(StChildSupervise) \
		gen(StContainerStart) \
		gen(StInternalsSupervise) \
		gen(StTmp) \
		gen(StSdStart) \
		gen(StSdContainer) \
		gen(StSdInternalsFree) \
		gen(StSdInternals) \
		gen(StSdPipesClose) \
		gen(StSdTmp) \

#define dGenProcStateEnum(s) s,
dProcessStateEnum(ProcState);

#if 0
#define dGenProcStateString(s) #s,
dProcessStateStr(ProcState);
#endif

using namespace std;

#define LOG_LVL	0

FileExecuting::FileExecuting()
	: Transfering("FileExecuting")
	// container & internal
	, mState(StStart)
	, mIsInternal(false)
	// container
	, mMsStart(0)
	, mMsTimeout(0)
	, mInternalsStarted(false)
	, mConfigClosed(false)
	// internal
	, mCmd("")
	, mCmdBase("")
	, mUserEnv(false)
	, mpArgs(NULL)
	, mpEnv(NULL)
	, mStrStateChild("Running")
	, mpResult(&mResult)
	, mBytesRead(0)
	, mBytesSent(0)
	, mDoneAck(false)
{
	mLstExec.reserve(4);
	mResult.idChild = -1;

	pipeInit(mNodeIn.pipe);
	pipeInit(mNodeOut.pipe);
	pipeInit(mNodeErr.pipe);
}

/* member functions */

Success FileExecuting::process()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mMsStart;
	Success success;
	bool ok, pipeUsed;
	int res;
	pid_t idProc;
	vector<FileExecuting *>::iterator iter;
	FileExecuting *pExec;
#if 0
	procWrnLog("mState = %s", ProcStateString[mState]);
#endif
	switch (mState)
	{
	case StStart:

		{
			Guard lock(mMtxConfig);
			mConfigClosed = true;
		}

		if (mIsInternal)
		{
			mState = StInternalStart;
			break;
		}

		// container

		if (!mLstExec.size())
		{
			procWrnLog("command list empty. nothing to do");
			return Positive;
		}

		mLstExec.front()->mNodeIn = mNodeIn;

		mResults.reserve(mLstExec.size());

		iter = mLstExec.begin();
		for (; iter != mLstExec.end(); ++iter)
		{
			pExec = *iter;

			mResults.push_back(FeResult());

			pExec->mpResult = &mResults.back();
			pExec->mpResult->idChild = -1;

			start(pExec);
		}

		mInternalsStarted = true;

		mState = StContainerStart;

		break;
	case StInternalStart:

		mState = StPipesCreate;

		break;
	case StPipesCreate:

		res = pipe(&mNodeIn.pipe.fdRead);
		if (res < 0)
			return procErrLog(-1, "could not create pipe (stdin): %s", strerror(errno));

		pipeUsed = mNodeOut.manualEnabled or mNodeOut.autoEnabled;
		if (pipeUsed)
			res = pipe(&mNodeOut.pipe.fdRead);
		else
			res = mNodeOut.pipe.fdWrite = open("/dev/null", O_WRONLY);

		if (res < 0)
			return procErrLog(-1, "could not create %s (stdout): %s",
					pipeUsed ? "pipe" : "write fd",
					strerror(errno));

		pipeUsed = mNodeErr.manualEnabled or mNodeErr.autoEnabled;
		if (pipeUsed)
			res = pipe(&mNodeErr.pipe.fdRead);
		else
			res = mNodeErr.pipe.fdWrite = open("/dev/null", O_WRONLY);

		if (res < 0)
			return procErrLog(-1, "could not create %s (stderr): %s",
					pipeUsed ? "pipe" : "write fd",
					strerror(errno));

		mState = StFork;

		break;
	case StFork:
#if 0
		procWrnLog("Forking");
		procWrnLog("Using file: %s", mCmdBase.c_str());
#endif
		mpArgs = vecConstCharToCharList(mArgs);
		if (!mpArgs)
			return procErrLog(-1, "could not create arguments");

		mpEnv = vecConstCharToCharList(mEnv);
		if (!mpEnv)
		{
			ptrListFree(mArgs.size(), mpArgs);
			return procErrLog(-1, "could not create environment");
		}

		idProc = vfork();
		if (idProc)
		{
			ptrListFree(mArgs.size(), mpArgs);
			ptrListFree(mEnv.size(), mpEnv);
		}

		if (idProc < 0)
			return procErrLog(-1, "could not fork process: %s", strerror(errno));

		if (idProc > 0)
		{
			mpResult->idChild = idProc;
			mState = StParentStart;
			break;
		}

		// Child

		// Setup Pipes
#if 1
		res = dup2(mNodeErr.pipe.fdWrite, STDERR_FILENO);
		if (res < 0)
			_exit(EXIT_FAILURE); // only valid situation in every system to use _exit()!
#endif
		res = dup2(mNodeIn.pipe.fdRead, STDIN_FILENO);
		if (res < 0)
		{
			cerr << "could not duplicate fd (stdin): " << strerror(errno) << endl;
			_exit(EXIT_FAILURE);
		}

		res = dup2(mNodeOut.pipe.fdWrite, STDOUT_FILENO);
		if (res < 0)
		{
			cerr << "could not duplicate fd (stdout): " << strerror(errno) << endl;
			_exit(EXIT_FAILURE);
		}

		// Close all open files
		closefrom(3); // non standard ..

		// Execute
		res = execvpe(mpArgs[0], mpArgs, mUserEnv ? mpEnv : environ);
		if (res < 0)
		{
			cerr << "could not execute file '" << mpArgs[0] << "': " << strerror(errno) << endl;
			_exit(EXIT_FAILURE);
		}

		cerr << "reached invalid position" << endl;
		_exit(EXIT_FAILURE);

		break;
	case StParentStart:

		//procWrnLog("ID child: %d", mpResult->idChild);

		/* IMPORTANT:
		 * Do not close read end of stdin pipe in parent
		 * Reason:
		 * Child process may be terminated before internal send().
		 * Keeping the read end of stdin open avoids SIGPIPE
		 */
		//fdClose(mNodeIn.pipe.fdRead);

		fdClose(mNodeOut.pipe.fdWrite);
		fdClose(mNodeErr.pipe.fdWrite);

		pipeUsed = mNodeIn.manualEnabled or mNodeIn.autoEnabled;
		if (!pipeUsed)
			fdClose(mNodeIn.pipe.fdWrite);

		ok = mNodeIn.pipe.fdWrite < 0 ? true : fileNonBlockingSet(mNodeIn.pipe.fdWrite);
		if (!ok)
			return procErrLog(-3, "could not set non blocking mode: %s", strerror(errno));

		ok = mNodeOut.pipe.fdRead < 0 ? true : fileNonBlockingSet(mNodeOut.pipe.fdRead);
		if (!ok)
			return procErrLog(-3, "could not set non blocking mode: %s", strerror(errno));

		ok = mNodeErr.pipe.fdRead < 0 ? true : fileNonBlockingSet(mNodeErr.pipe.fdRead);
		if (!ok)
			return procErrLog(-3, "could not set non blocking mode: %s", strerror(errno));

		{
			Guard lock(mMtxWrite);
			mSendReady = true;
		}

		{
			Guard lock(mMtxRead);
			mReadReady = true;
		}

		mState = StSinksReadyWait;

		break;
	case StSinksReadyWait:

		ok = sinksReady(&mNodeOut) and sinksReady(&mNodeErr);
		if (!ok)
			break;

		mState = StChildSupervise;

		break;
	case StChildSupervise:

		if (mDone and not mDoneAck)
		{
			autoSourceDone();
			mDoneAck = true;
		}

		autoSource(&mNodeIn);

		autoSink(&mNodeOut);
		autoSink(&mNodeErr);

		success = childStateRecord();
		if (success != Pending and success != Positive)
			return procErrLog(-1, "chould not record child state");

		if (!mpResult->childTerminated)
			return Pending;

		if (mNodeOut.pipe.fdRead != -1)
			return Pending;

		if (mNodeErr.pipe.fdRead != -1)
			return Pending;
#if 1
		return Positive;
#endif
		break;
	case StContainerStart:

		mState = StInternalsSupervise;

		break;
	case StInternalsSupervise:

		if (!mSendReady and mLstExec.front()->mSendReady)
			mSendReady = true;

		if (!mReadReady and mLstExec.back()->mReadReady)
			mReadReady = true;

		if (mDone)
			mLstExec.front()->doneSet();

		// TODO: Timeout

		return childrenSuccess();

		break;
	case StTmp:

		break;
	default:
		break;
	}

	return Pending;
}

bool FileExecuting::sinksReady(FeNode *pNode) const
{
	list<Transfering *>::iterator iTrans;
	Transfering *pTrans;

	iTrans = pNode->lstTransfers.begin();
	for (; iTrans != pNode->lstTransfers.end(); ++iTrans)
	{
		pTrans = *iTrans;

		if (!pTrans->mSendReady)
			return false;
	}

	return true;
}

Success FileExecuting::childStateRecord()
{
	if (mpResult->childTerminated)
		return Pending;

	pid_t idProc;
	int res;

	idProc = waitpid(mpResult->idChild, &res, WNOHANG | WUNTRACED | WCONTINUED);

	if (idProc < 0 and errno != EAGAIN)
		return procErrLog(-1, "could not wait for child: %s", strerror(errno));

	if (!idProc or (idProc < 0 and errno == EAGAIN))
		return Pending;

	//procWrnLog("state of child changed");

	if (WIFEXITED(res))
	{
		mStrStateChild = "Terminated by exit()";

		mpResult->idChild = -1;
		mpResult->childTerminated = true;
		mpResult->childTerminatedBySig = false;
		mpResult->codeRet = WEXITSTATUS(res);
	}
	else
	if (WIFSIGNALED(res))
	{
		mStrStateChild = "Terminated by signal";

		mpResult->idChild = -1;
		mpResult->childTerminated = true;
		mpResult->childTerminatedBySig = true;
		mpResult->codeSig = WTERMSIG(res);
#if WCOREDUMP
		mpResult->coreDumped = (bool)WCOREDUMP(res);
#endif
	}
	else
	if (WIFSTOPPED(res))
	{
		mStrStateChild = "Stopped";
		mpResult->childStopped = true;
		mpResult->codeSig = WSTOPSIG(res);
	}
	else
	if (WIFCONTINUED(res))
	{
		mStrStateChild = "Running (Continued)";
		mpResult->childStopped = false;
		mpResult->codeSig = 0;
	}
	else
		return procErrLog(-1, "unknown child state recorded: %d", res);

	return Positive;
}

void FileExecuting::autoSource(FeNode *pNode)
{
	if (!pNode or !pNode->autoEnabled)
		return;

	if (pNode->autoDone)
		return;

	ssize_t lenReq = 512;
	ssize_t lenLeft, lenPlanned, lenDone;
	const char *pSrc;
	bool isErr, wouldBlock, isErrFinal, isDone;

	if (pNode->lstBuffers.size())
	{
		FeNodeBuffer *pBuff = &pNode->lstBuffers.front();

		pSrc = pBuff->pSrc + pBuff->processed;

		lenLeft = pBuff->len - pBuff->processed;
		lenPlanned = MIN(lenReq, lenLeft);
		lenDone = intSend(pSrc, lenPlanned);

		if (lenDone < 0)
		{
			autoSourceDone();
			return;
		}

		pBuff->processed += lenDone;

		if (pBuff->processed < pBuff->len)
			return;

		procDbgLog(LOG_LVL, "end-of-buffer");

		autoSourceDone();
		return;
	}
	else
	if (pNode->lstFds.size())
	{
		FeFileDescSetting *pFd = &pNode->lstFds.front();
		char buf[15];
		lenLeft = lenReq;

		while (lenLeft)
		{
			lenReq = sizeof(buf) - 1;
			lenPlanned = MIN(lenReq, lenLeft);
			lenDone = ::read(pFd->fd, buf, lenPlanned);

			isErr = lenDone < 0;
			wouldBlock = isErr and ((errno == EAGAIN) or (errno == EWOULDBLOCK));
			isErrFinal = isErr and not wouldBlock;
			isDone = !lenDone or wouldBlock or isErrFinal;

			if (isDone)
			{
				if (pFd->autoClose)
					fdClose(pFd->fd);

				autoSourceDone();
			}

			if (isErrFinal)
			{
				procErrLog(-1, "read() failed: %s", strerror(errno));
				return;
			}

			if (isDone)
			{
				procDbgLog(LOG_LVL, "end-of-file");
				return;
			}

			buf[lenDone] = 0;

			lenReq = lenDone;
			lenDone = intSend(buf, lenReq);

			if (lenDone != lenReq)
			{
				procErrLog(-1, "internal send() failed");
				autoSourceDone();
				return;
			}

			lenLeft -= lenReq;
		}

		return;
	}
	else
	if (pNode->lstTransfers.size())
	{
		Transfering *pTrans = pNode->lstTransfers.front();
		char buf[15];
		lenLeft = lenReq;

		while (lenLeft)
		{
			lenReq = sizeof(buf) - 1;
			lenPlanned = MIN(lenReq, lenLeft);
			lenDone = pTrans->read(buf, lenPlanned);

			if (!lenDone)
				return;

			if (lenDone < 0)
			{
				procDbgLog(LOG_LVL, "end-of-Transfering()");
				autoSourceDone();
				return;
			}

			buf[lenDone] = 0;

			lenReq = lenDone;
			lenDone = intSend(buf, lenReq);

			if (lenDone != lenReq)
			{
				procErrLog(-1, "internal send() failed");
				autoSourceDone();
				return;
			}

			lenLeft -= lenReq;
		}

		return;
	}
}

void FileExecuting::autoSourceDone()
{
	FeNode *pNode = &mNodeIn;

	if (pNode->autoDone)
		return;

	fdClose(pNode->pipe.fdWrite);
	pNode->autoDone = true;

	mDone = true;

	procDbgLog(LOG_LVL, "autoSourceDone()");
}

void FileExecuting::autoSink(FeNode *pNode)
{
	if (!pNode or !pNode->autoEnabled)
		return;

	if (pNode->autoDone)
		return;

	char buf[23];
	size_t numBurst = 23;

	// buffer
	list<FeNodeBuffer>::iterator iBuff;
	FeNodeBuffer *pBuff;
	ssize_t lenReq;
	ssize_t lenLeft, lenPlanned, lenDone;
	char *pDest;
	// string
	list<std::string *>::iterator iStr;
	// fd
	list<FeFileDescSetting>::iterator iFd;
	// Transfering
	list<Transfering *>::iterator iTrans;

	buf[0] = 0;

	while (numBurst--)
	{
		lenReq = sizeof(buf) - 1;
		lenPlanned = lenReq;
		lenDone = intSinkRead(buf, lenPlanned, pNode);

		if (!lenDone)
			return;

		if (lenDone < 0)
		{
			procDbgLog(LOG_LVL, "autoSink() finished");

			iFd = pNode->lstFds.begin();
			for (; iFd != pNode->lstFds.end(); ++iFd)
			{
				if (!iFd->autoClose)
					continue;

				procDbgLog(LOG_LVL, "closing auto fd: %d", iFd->fd);
				fdClose(iFd->fd);
			}

			iTrans = pNode->lstTransfers.begin();
			for (; iTrans != pNode->lstTransfers.end(); ++iTrans)
				(*iTrans)->doneSet();

			pNode->autoDone = true;
			return;
		}

		if (lenDone < 0)
			return;

		buf[lenDone] = 0;

		// buffer
		iBuff = pNode->lstBuffers.begin();
		for (; iBuff != pNode->lstBuffers.end(); ++iBuff)
		{
			pBuff = &(*iBuff);

			if (!pBuff->pDest)
				continue;

			lenLeft = pBuff->len - pBuff->processed;
			if (!lenLeft)
				continue;

			lenReq = lenDone;
			lenPlanned = MIN(lenDone, lenLeft);

			pDest = pBuff->pDest + pBuff->processed;
			memcpy(pDest, buf, lenPlanned);

			pBuff->processed += lenPlanned;

			if (pBuff->processed < pBuff->len)
				return;

			procDbgLog(LOG_LVL, "end-of-buffer");
		}

		// string
		iStr = pNode->lstStrings.begin();
		for (; iStr != pNode->lstStrings.end(); ++iStr)
			*(*iStr) += string(buf, lenDone);

		// fd
		iFd = pNode->lstFds.begin();
		for (; iFd != pNode->lstFds.end(); ++iFd)
			::write(iFd->fd, buf, lenDone);

		// Transfering
		iTrans = pNode->lstTransfers.begin();
		for (; iTrans != pNode->lstTransfers.end(); ++iTrans)
			(*iTrans)->send(buf, lenDone);
	}
}

char **FileExecuting::vecConstCharToCharList(const VecConstChar &lst)
{
	size_t cnt = lst.size(); // +1 = NULL
	char **pElem;

	pElem = (char **)calloc(cnt + 1, sizeof(char *));
	if (!pElem)
	{
		cerr << "could not allocate memory for list" << endl;
		return NULL;
	}

	for (size_t i = 0; i < cnt; ++i)
	{
		pElem[i] = strdup(lst[i]);

		if (!pElem[i])
		{
			cerr << "could not allocate memory for list element" << endl;
			ptrListFree(lst.size(), pElem);
			return NULL;
		}
	}

	return pElem;
}

void FileExecuting::ptrListFree(int cnt, char ** &lst)
{
	if (!lst)
		return;

	for (int i = 0; i < cnt; ++i)
	{
		if (!lst[i])
			continue;

		free(lst[i]);
		lst[i] = NULL;
	}

	free(lst);
	lst = NULL;
}

Success FileExecuting::shutdown()
{
	//uint32_t curTimeMs = millis();
	//uint32_t diffMs = curTimeMs - mMsStart;
	//Success success;
	//int res;
	vector<FileExecuting *>::iterator iter;
#if 0
	procWrnLog("mState = %s", ProcStateString[mState]);
#endif
	switch (mState)
	{
	case StSdStart:

		if (mIsInternal)
		{
			mState = StSdInternals;
			break;
		}

		mState = StSdContainer;

		break;
	case StSdContainer:

		mState = StSdInternalsFree;

		break;
	case StSdInternalsFree:

		if (mInternalsStarted)
			return Positive;

		iter = mLstExec.begin();
		for (; iter != mLstExec.end(); ++iter)
			delete *iter;

		return Positive;

		break;
	case StSdInternals:

		mState = StSdPipesClose;

		break;
	case StSdPipesClose:

		pipeClose(mNodeIn.pipe);
		pipeClose(mNodeOut.pipe);
		pipeClose(mNodeErr.pipe);

		return Positive;

		break;
	case StSdTmp:

		break;
	default:
		mState = StSdStart;
		break;
	}

	return Pending;
}

FileExecuting &FileExecuting::msTimeoutSet(uint32_t msTimeout)
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	mMsTimeout = msTimeout;

	return *this;
}

FileExecuting &FileExecuting::sourceEnable()
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	mNodeIn.manualEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sourceSet(const char *pSrc, size_t len, bool autoFree)
{
	if (!pSrc)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	if (mNodeIn.lstBuffers.size())
		mNodeIn.lstBuffers.clear();

	if (!len)
		len = strlen(pSrc);

	mNodeIn.lstBuffers.emplace_back(pSrc, nullptr, len, autoFree);
	mNodeIn.autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sourceSet(const string *pStr)
{
	if (!pStr)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	if (mNodeIn.lstBuffers.size())
		mNodeIn.lstBuffers.clear();

	mNodeIn.lstBuffers.emplace_back(pStr->c_str(), nullptr, pStr->size(), false);
	mNodeIn.autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sourceSet(int fd, bool autoClose)
{
	if (fd < 0)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	if (mNodeIn.lstFds.size())
		mNodeIn.lstFds.clear();

	mNodeIn.lstFds.emplace_back(fd, autoClose);
	mNodeIn.autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sourceSet(Transfering *pTrans)
{
	if (!pTrans)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	if (mNodeIn.lstTransfers.size())
		mNodeIn.lstTransfers.clear();

	mNodeIn.lstTransfers.push_back(pTrans);
	mNodeIn.autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::cmdAdd(const VecConstChar &argv)
{
	size_t argc = argv.size();

	if (!argc)
		return *this;

	return intCmdAdd(argv[0], VecConstChar(argv.begin() + 1, argv.end()));
}

FileExecuting &FileExecuting::intCmdAdd(const string &cmd, const VecConstChar &argv)
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	if (!cmd.size())
		return *this;

	FileExecuting *pExec = FileExecuting::create();
	if (!pExec)
	{
		procErrLog(-1, "could not create process");
		return *this;
	}

	pExec->mIsInternal = true;

	pExec->mCmd = cmd;

	size_t pos;

	pos = cmd.find_last_of("/\\");
	if (pos == string::npos)
		pExec->mCmdBase = cmd;
	else
		pExec->mCmdBase = cmd.substr(pos + 1);

	pExec->mArgs.clear();
	pExec->mArgs.push_back(pExec->mCmd.c_str());
	pExec->mArgs.insert(pExec->mArgs.end(), argv.begin(), argv.end());

	pExec->mEnv.clear();
	pExec->mEnv.reserve(8);

	mLstExec.push_back(pExec);

	size_t numCmd = mLstExec.size();

	if (numCmd < 2)
		return *this;

	// Do not use reversed linking!
	// Reason: pPrev would be in manual read() mode otherwise

	pExec->sourceEnable();

	FileExecuting *pPrev = mLstExec[numCmd - 2];
	FeNode *pNode = &pPrev->mNodeOut;

	pNode->lstTransfers.push_back(pExec);
	pNode->autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::envSet(const VecConstChar &envv, bool dropOld)
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];

	for (char **pEnv = environ; (not dropOld) and *pEnv; ++pEnv)
		pExec->mEnv.push_back(*pEnv);

	pExec->mEnv.insert(pExec->mEnv.end(), envv.begin(), envv.end());
	pExec->mUserEnv = true;

	return *this;
}

FileExecuting &FileExecuting::sinkEnable(int fdSel)
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];
	FeNode *pNode = fdSel == STDOUT_FILENO ? &pExec->mNodeOut : &pExec->mNodeErr;

	pNode->manualEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sinkAdd(char *pDest, size_t len, int fdSel)
{
	if (!pDest)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];
	FeNode *pNode = fdSel == STDOUT_FILENO ? &pExec->mNodeOut : &pExec->mNodeErr;

	pNode->lstBuffers.emplace_back(nullptr, pDest, len, false);
	pNode->autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sinkAdd(string *pStr, int fdSel)
{
	if (!pStr)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];
	FeNode *pNode = fdSel == STDOUT_FILENO ? &pExec->mNodeOut : &pExec->mNodeErr;

	pNode->lstStrings.push_back(pStr);
	pNode->autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sinkAdd(int fd, bool autoClose, int fdSel)
{
	if (fd < 0)
	{
		procErrLog(-1, "could not add fd sink. Invalid fd");
		return *this;
	}

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];
	FeNode *pNode = fdSel == STDOUT_FILENO ? &pExec->mNodeOut : &pExec->mNodeErr;

	pNode->lstFds.emplace_back(fd, autoClose);
	pNode->autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::sinkAdd(Transfering *pTrans, int fdSel)
{
	if (!pTrans)
		return *this;

	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];
	FeNode *pNode = fdSel == STDOUT_FILENO ? &pExec->mNodeOut : &pExec->mNodeErr;

	pNode->lstTransfers.push_back(pTrans);
	pNode->autoEnabled = true;

	return *this;
}

FileExecuting &FileExecuting::errRedirect()
{
	Guard lock(mMtxConfig);

	if (mConfigClosed)
		return *this;

	int idx = mLstExec.size();
	if (!idx)
		return *this;
	--idx;

	FileExecuting *pExec = mLstExec[idx];

	pExec->mNodeErr.redirect = true;

	return *this;
}

int FileExecuting::sigSend(int sig, ssize_t idx) const
{
	// internal

	if (mIsInternal)
	{
		if (mpResult->idChild < 0)
			return procErrLog(-2, "cannot send signal. Child PID not set");

		return kill(mpResult->idChild, sig);
	}

	// container - specific internal

	if (idx >= (ssize_t)mLstExec.size())
		return procErrLog(-3, "cannot send signal. Index out of bounds");

	if (idx >= 0)
		return mLstExec[idx]->sigSend(sig);

	// container - all internals

	if (!mLstExec.size())
		return procErrLog(-4, "cannot send signal. Empty command list");

	vector<FileExecuting *>::const_iterator iter;
	int res;

	idx = 0;
	iter = mLstExec.begin();
	for (; iter != mLstExec.end(); ++iter, ++idx)
	{
		res = (*iter)->sigSend(sig);
		if (res < 0)
			return procErrLog(-4, "error sending signal to process. Index %d, PID %d: %s",
							idx, (*iter)->mpResult->idChild, strerror(errno));
	}

	return 0;
}

bool FileExecuting::boolRet(ssize_t idx, size_t offs, bool isAnd)
{
	{
		Guard lock(mMtxConfig);
		if (!mConfigClosed)
			return false;
	}

	// container - specific internal

	if (idx >= (ssize_t)mLstExec.size())
		return false;

	if (idx >= 0)
	{
		bool *pBool = &mResults[idx].childStopped;
		pBool += offs;
		return *pBool;
	}

	// container - all internals

	bool checkRes;

	for (size_t i = 0; i < mResults.size(); ++i)
	{
		checkRes = boolRet(i, offs, false);

		if (isAnd xor checkRes)
			return checkRes;
	}

	return isAnd;
}

int FileExecuting::intRet(ssize_t idx, size_t offs)
{
	{
		Guard lock(mMtxConfig);
		if (!mConfigClosed)
			return false;
	}

	// container - specific internal

	if (idx >= (ssize_t)mLstExec.size())
		return procErrLog(-1, "index out of bounds");

	if (idx >= 0)
	{
		int *pInt = &mResults[idx].idChild;
		pInt += offs;
		return *pInt;
	}

	// container - last internal

	size_t i = mResults.size();

	if (!i)
		return procErrLog(-1, "empty result list");

	return intRet(i - 1, offs);
}

ssize_t FileExecuting::send(const void *pData, size_t lenReq)
{
	if (!mIsInternal and mLstExec.size())
		return mLstExec.front()->send(pData, lenReq);

	if (mNodeIn.autoEnabled)
		return procErrLog(-1, "unable to send data. Auto sourcing");

	if (!mNodeIn.manualEnabled)
		return procErrLog(-1, "unable to send data. Source not enabled");

	return intSend(pData, lenReq);
}

ssize_t FileExecuting::intSend(const void *pData, size_t lenReq)
{
	if (!pData or !lenReq)
		return procErrLog(-1, "could not send data. Buffer not set");

	Guard lock(mMtxWrite);

	if (!mSendReady)
		return procErrLog(-1, "could not send data. Not ready");

	int &fdWrite = mNodeIn.pipe.fdWrite;

	if (fdWrite < 0)
		return procErrLog(-1, "could not send data. File descriptor not set");

	ssize_t res;
	size_t lenBkup = lenReq;
	size_t bytesSent = 0;

	while (lenReq)
	{
		res = ::write(fdWrite, (const char *)pData, lenReq);
		if (res < 0)
		{
			fdClose(fdWrite);
			return procErrLog(-1, "write() failed: %s", strerror(errno));
		}

		if (!res)
			break;

		pData = ((const uint8_t *)pData) + res;
		lenReq -= res;

		bytesSent += res;
	}

	if (bytesSent != lenBkup)
		procWrnLog("not all data has been sent");

	mBytesSent += bytesSent;

	return bytesSent;
}

ssize_t FileExecuting::read(void *pBuf, size_t lenReq)
{
	return sinkRead(pBuf, lenReq);
}

ssize_t FileExecuting::sinkRead(void *pBuf, size_t lenReq, int fdSel)
{
	FeNode *pNode = fdSel == STDOUT_FILENO ? &mNodeOut : &mNodeErr;

	if (!mIsInternal and mLstExec.size())
		return mLstExec.back()->sinkRead(pBuf, lenReq, fdSel);

	if (pNode->autoEnabled)
		return procErrLog(-1, "unable to read data. Auto sinking");

	if (!pNode->manualEnabled)
		return procErrLog(-1, "unable to read data. Sink not enabled");

	return intSinkRead(pBuf, lenReq, pNode);
}

ssize_t FileExecuting::intSinkRead(void *pBuf, size_t lenReq, FeNode *pNode)
{
	if (!pBuf or !lenReq)
		return -1;

	Guard lock(mMtxRead);

	if (!pNode)
		return procErrLog(-1, "unable to read data. Node not set");

	if (!mReadReady)
		return 0;

	int &fdRead = pNode->pipe.fdRead;

	if (fdRead < 0)
		return -2;

	ssize_t lenRead = 0;

	lenRead = ::read(fdRead, (char *)pBuf, lenReq);

	bool isErr = lenRead < 0;
	bool wouldBlock = isErr and ((errno == EAGAIN) or (errno == EWOULDBLOCK));
	bool isErrFinal = isErr and not wouldBlock;
	bool isDone = !lenRead or (wouldBlock and mpResult->childTerminated) or isErrFinal;

	if (isDone)
		fdClose(fdRead);

	if (isErrFinal)
		return procErrLog(-3, "read() failed: %s", strerror(errno));

	if (isDone)
	{
		procDbgLog(LOG_LVL, "end-of-file");
		return -4;
	}

	if (wouldBlock)
		return 0;

	//procDbgLog(LOG_LVL, "received data. len: %d", lenRead);

	mBytesRead += lenRead;

	return lenRead;
}

void FileExecuting::processInfo(char *pBuf, char *pBufEnd)
{
	if (!mIsInternal)
		containerInfo(pBuf, pBufEnd);
	else
		internalInfo(pBuf, pBufEnd);
}

void FileExecuting::containerInfo(char *pBuf, char *pBufEnd)
{
#if 0
	dInfo("State\t\t\t%s\n", ProcStateString[mState]);
#else
	(void)pBuf;
	(void)pBufEnd;
#endif
}

void FileExecuting::internalInfo(char *pBuf, char *pBufEnd)
{
#if 1
	//dInfo("State\t\t\t%s\n", ProcStateString[mState]);
	dInfo("File\t\t\t%s [%d]\n", mCmdBase.c_str(), mpResult->idChild);
#if 0
	dInfo("Child\n");
	dInfo("  State\t\t\t%s%s\n", mStrStateChild.c_str(),
					mpResult->coreDumped ? " (core dumped)" : "");
	dInfo("  Signal\t\t\t%d\n", mpResult->codeSig);
	dInfo("  Return code\t\t%d\n", mpResult->codeRet);
	dInfo("  Terminated\t\t%s\n", mpResult->childTerminated ? "Yes" : "No");
#endif
#if 0
	dInfo("Done\t\t\t%d\n", mDone);
	dInfo("Pipes\t\t\t%d, %d | %d, %d | %d, %d\n",
			mNodeIn.pipe.fdRead,
			mNodeIn.pipe.fdWrite,
			mNodeOut.pipe.fdRead,
			mNodeOut.pipe.fdWrite,
			mNodeErr.pipe.fdRead,
			mNodeErr.pipe.fdWrite);
	dInfo("Auto\t\t\t%d, %d, %d\r\n",
			mNodeIn.autoDone,
			mNodeOut.autoDone,
			mNodeErr.autoDone);
#endif
	dInfo("Bytes read\t\t%d\r\n", mBytesRead);
	//dInfo("Bytes sent\t\t%d\n", mBytesSent);
#endif
}

/* static functions */

