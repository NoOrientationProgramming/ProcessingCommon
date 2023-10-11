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

#ifndef FILE_EXECUTING_H
#define FILE_EXECUTING_H

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <unistd.h>
#include <signal.h>

#include "Transfering.h"
#include "LibFilesys.h"

#define dTimeoutExecDefault	5000

typedef std::vector<const char *> VecConstChar;
typedef VecConstChar fec; // FileExecuting() Command

struct FeNodeBuffer
{
	FeNodeBuffer()
		: pSrc(NULL)
		, pDest(NULL)
		, len(0)
		, processed(0)
		, autoFree(false)
	{}
	FeNodeBuffer(const char *p1, char *p2, size_t l, bool af)
		: pSrc(p1)
		, pDest(p2)
		, len(l)
		, processed(0)
		, autoFree(af)
	{}
	const char *pSrc;
	char *pDest;
	size_t len;
	size_t processed;
	bool autoFree;
};

struct FeFileDescSetting
{
	FeFileDescSetting()
		: fd(-1)
		, autoClose(false)
	{}
	FeFileDescSetting(int f, bool ac)
		: fd(f)
		, autoClose(ac)
	{}
	int fd;
	bool autoClose;
};

struct FeNode
{
	PairFd pipe;
	std::list<FeNodeBuffer> lstBuffers;
	std::list<std::string *> lstStrings;
	std::list<FeFileDescSetting> lstFds;
	std::list<Transfering *> lstTransfers;
	bool manualEnabled;
	bool autoEnabled;
	bool autoDone;
	bool redirect;
};

struct FeResult
{
	bool childStopped;
	bool childTerminated;
	bool childTerminatedBySig;
	bool coreDumped;
	int idChild;
	int codeSig;
	int codeRet;
};

// 1. https://man7.org/linux/man-pages/man2/pipe.2.html
// 2. https://man7.org/linux/man-pages/man2/fork.2.html
// 3. https://man7.org/linux/man-pages/man2/dup.2.html
// 4. https://man7.org/linux/man-pages/man3/exec.3.html
class FileExecuting : public Transfering
{

public:

	static FileExecuting *create()
	{
		return new (std::nothrow) FileExecuting;
	}

	// routing & change container

	FileExecuting &msTimeoutSet(uint32_t msTimeout);
	FileExecuting &sourceEnable();
	FileExecuting &sourceSet(const char *pSrc, size_t len = 0, bool autoFree = false);
	FileExecuting &sourceSet(const std::string *pStr);
	FileExecuting &sourceSet(int fd, bool autoClose = false);
	FileExecuting &sourceSet(Transfering *pTrans);

	// add command

	FileExecuting &cmdAdd(const VecConstChar &argv);
	FileExecuting &cmdAdd(const std::string &cmd, const VecConstChar &argv)
	{ return intCmdAdd(cmd, argv); }
	FileExecuting &cmdAdd(const std::string &cmd, int argc, char *argv[])
	{ return intCmdAdd(cmd, VecConstChar(argv, argv + argc)); }

	// - https://en.cppreference.com/w/cpp/language/operators
	// - https://en.cppreference.com/w/cpp/language/operator_assignment
	// - https://en.cppreference.com/w/cpp/language/operator_precedence
	FileExecuting &operator|=(const VecConstChar &argv) { return cmdAdd(argv); }
	FileExecuting &operator|(const VecConstChar &argv) { return cmdAdd(argv); }

	// routing & change internal

	FileExecuting &envSet(const VecConstChar &envv, bool dropOld = true);
	FileExecuting &sinkEnable(int fdSel = STDOUT_FILENO);
	FileExecuting &sinkAdd(char *pDest, size_t len, int fdSel = STDOUT_FILENO);
	FileExecuting &sinkAdd(std::string *pStr, int fdSel = STDOUT_FILENO);
	FileExecuting &sinkAdd(int fd, bool autoClose = false, int fdSel = STDOUT_FILENO);
	FileExecuting &sinkAdd(Transfering *pTrans, int fdSel = STDOUT_FILENO);
	FileExecuting &errRedirect();

	// getters & child control

	size_t numCommands() const
	{ return mLstExec.size(); }

	int sigSend(int sig, ssize_t idx = -1) const;

	bool childStopped(ssize_t idx = -1, bool isAnd = false)
	{ return boolRet(idx, 0, isAnd); }
	bool childTerminated(ssize_t idx = -1, bool isAnd = true)
	{ return boolRet(idx, 1, isAnd); }
	bool childTerminatedBySig(ssize_t idx = -1, bool isAnd = false)
	{ return boolRet(idx, 2, isAnd); }
	bool coreDumped(ssize_t idx = -1, bool isAnd = false)
	{ return boolRet(idx, 3, isAnd); }

	int idChild(ssize_t idx = -1)
	{ return intRet(idx, 0); }
	int codeSig(ssize_t idx = -1)
	{ return intRet(idx, 1); }
	int codeRet(ssize_t idx = -1)
	{ return intRet(idx, 2); }

	// send / read

	ssize_t send(const void *pData, size_t lenReq);

	virtual ssize_t send(const std::string &str)
	{ return this->send(str.c_str(), str.size()); }

	ssize_t read(void *pBuf, size_t lenReq);
	ssize_t sinkRead(void *pBuf, size_t lenReq, int fdSel = STDOUT_FILENO);

protected:

	FileExecuting();
	virtual ~FileExecuting() {}

private:

	FileExecuting(const FileExecuting &) : Transfering("") {}
	FileExecuting &operator=(const FileExecuting &) { return *this; }

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */

	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);
	void containerInfo(char *pBuf, char *pBufEnd);
	void internalInfo(char *pBuf, char *pBufEnd);

	bool sinksReady(FeNode *pNode) const;
	Success childStateRecord();
	void autoSource(FeNode *pNode);
	void autoSourceDone();
	void autoSink(FeNode *pNode);

	char **vecConstCharToCharList(const VecConstChar &lst);
	void ptrListFree(int cnt, char ** &lst);

	FileExecuting &intCmdAdd(const std::string &cmd, const VecConstChar &argv);
	bool boolRet(ssize_t idx, size_t offs, bool isAnd = false);
	int intRet(ssize_t idx, size_t offs);

	ssize_t intSend(const void *pData, size_t lenReq);
	ssize_t intSinkRead(void *pBuf, size_t lenReq, FeNode *pNode);

	/* member variables */

	// container & internal

	bool mIsInternal;

	// container

	uint32_t mMsStart;
	uint32_t mMsTimeout;
	std::vector<FileExecuting *> mLstExec;
	std::vector<FeResult> mResults;
	FeNode mNodeIn;
	bool mInternalsStarted;
	bool mConfigClosed;
	std::mutex mMtxConfig;

	// internal

	std::string mCmd;
	std::string mCmdBase;
	bool mUserEnv;
	char **mpArgs;
	char **mpEnv;
	std::string mStrStateChild;
	FeResult mResult;
	FeResult *mpResult;
	uint32_t mBytesRead;
	uint32_t mBytesSent;
	std::mutex mMtxWrite;
	std::mutex mMtxRead;
	VecConstChar mArgs;
	VecConstChar mEnv;
	FeNode mNodeOut;
	FeNode mNodeErr;
	bool mDoneAck;

	/* static functions */

	/* static variables */

	/* constants */

};

#endif

