/*
  This file is part of the DSP-Crowd project
  https://www.dsp-crowd.com

  Author(s):
      - Johannes Natter, office@dsp-crowd.com

  File created on 11.10.2024

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

#ifndef DNS_RESOLVING_H
#define DNS_RESOLVING_H

#include <string>
#include <list>

#include "Processing.h"
#include "LibDspc.h"

class DnsResolving : public Processing
{

public:

	static DnsResolving *create()
	{
		return new dNoThrow DnsResolving;
	}

	// input
	void hostnameSet(const std::string &hostname);

	// output
	const std::list<std::string> &lstIPv4();
	const std::list<std::string> &lstIPv6();

protected:

	virtual ~DnsResolving() {}

private:

	DnsResolving();
	DnsResolving(const DnsResolving &) = delete;
	DnsResolving &operator=(const DnsResolving &) = delete;

	/*
	 * Naming of functions:  objectVerb()
	 * Example:              peerAdd()
	 */

	/* member functions */
	Success process();
	Success shutdown();
	void processInfo(char *pBuf, char *pBufEnd);

#if CONFIG_LIB_DSPC_HAVE_C_ARES
	bool aresStart();
	void aresProcess();
#endif
	/* member variables */
	//uint32_t mStartMs;
	uint32_t mStateSd;
	std::string mHostname;
	std::list<std::string> mLstIPv4;
	std::list<std::string> mLstIPv6;

#if CONFIG_LIB_DSPC_HAVE_C_ARES
	ares_options mOptionsAres;
	ares_channel mChannelAres;
	bool mChannelAresInitDone;
	Success mDoneAres;
	std::string mErrAres;
#endif
	/* static functions */
#if CONFIG_LIB_DSPC_HAVE_C_ARES
	static void aresRequestDone(void *arg, int status, int timeouts, struct ares_addrinfo *result);
#endif

	/* static variables */

	/* constants */

};

#endif

