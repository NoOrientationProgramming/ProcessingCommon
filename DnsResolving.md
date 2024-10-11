
# DNS_RESOLVING() Manual Page

## NAME

**DnsResolving()** - Class for resolving hostnames to IP addresses.

## SYNOPSIS

```cpp
#include "DnsResolving.h"

// creation
static DnsResolving *create()

// configuration
void nameHostSet(const std::string &nameHost);

// result
const std::list<std::string> &lstIPv4();
const std::list<std::string> &lstIPv6();
```

## DESCRIPTION

The **DnsResolving()** class allows for the resolution of hostnames into IPv4 and IPv6 addresses. It uses the c-ares library (when available) to perform asynchronous DNS queries and process the results.

## CREATION

### `static DnsResolving *create()`

Creates a new instance of the **DnsResolving()** class. Memory is allocated using `new` with the `std::nothrow` modifier to ensure safe handling of failed allocations.

## CONFIGURATION

### `void nameHostSet(const std::string &nameHost)`

Sets the hostname to be resolved. This hostname is used to query both IPv4 and IPv6 addresses.

- **nameHost**: The domain name to resolve (e.g., "example.com").

## RESULT

### `const std::list<std::string> &lstIPv4()`

Returns a list of resolved IPv4 addresses for the set hostname.

### `const std::list<std::string> &lstIPv6()`

Returns a list of resolved IPv6 addresses for the set hostname.

## EXAMPLES

### Example: Simple DNS Resolution

In header file of **Trying()**
```cpp
  /* member variables */
  DnsResolving *mpResolv;
```

In source file of **Trying()**
```cpp
Trying::Trying()
  : Processing("Trying")
  , mpResolv(NULL)
{
  mState = StStart;
}

Success HttpRequesting::process()
{
  Success success;

  switch (mState)
  {
  case StStart:

    // create AND CHECK
    mpResolv = DnsResolving::create()
    if (!mpResolv)
      return procErrLog(-1, "could not create process");

    // configure
    mpResolv->nameHostSet("example.com");

    // start
    start(mpResolv);

    mState = StDnsResolvDoneWait;

    break;
  case StDnsResolvDoneWait:

    // wait
    success = mpResolv->success();
    if (success == Pending)
      break;

    // FIRST: check for errors
    if (success != Positive)
      return procErrLog(-1, "could not finish DNS request");

    // consume result
    for (list<string>::iterator iter = mpResolv->lstIPv4().begin();
                              iter != mpResolv->lstIPv4().end(); ++iter)
      procInfLog("IPv4 address: %s", iter->c_str());

    // ALWAYS: repel
    repel(mpResolv);
    mpResolv = NULL;

    mState = StNext;

    break;
  case StNext:

    ...

    break;
  default:
    break;
  }

  return Pending;
}
```

## SCOPE OF APPLICATION

- Linux
- Windows
- FreeBSD
- MacOSX

## DEPENDENCIES

### libc-ares

This library provides asynchronous DNS request handling in C, enabling programs to perform DNS lookups without blocking. It allows querying for various DNS records like A, AAAA, and CNAME, and is ideal for networking applications by allowing DNS operations to be handled in a non-blocking fashion.

```
License               MIT
Required              No
Project Page          https://c-ares.org
Documentation         https://c-ares.org/docs.html
Sources               https://github.com/c-ares/c-ares
```

## RECURSION

```
Order                 1
Depth                 -
```

## SEE ALSO

**Processing()**, **c-ares**, **ares_getaddrinfo()**, **getaddrinfo()**

## COPYRIGHT

Copyright (C) 2024, Johannes Natter

## LICENSE

This program is distributed under the terms of the GNU General Public License v3 or later. See <http://www.gnu.org/licenses/> for more information.

