
# DNS_RESOLVING() Manual Page

## NAME

**DnsResolving()** - Class for resolving hostnames to IP addresses.

## SYNOPSIS

```cpp
#include "DnsResolving.h"

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
```cpp
  break;
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
    return procErrLog(-1, "could not finish HTTP request");

  // consume results
  for (list<string>::iterator iter = mpResolv->lstIPv4().begin();
                              iter != mpResolv->lstIPv4().end(); ++iter)
    procInfLog("IPv4 address: %s", iter->c_str());

  // ALWAYS: repel
  repel(mpResolv);
  mpResolv = NULL;

  mState = StNext;

  break;
case StNext:
```

## SCOPE OF APPLICATION

- Linux
- Windows
- FreeBSD
- MacOSX

## DEPENDENCIES

The class is compatible with systems that support the **c-ares** library (if configured).

## RECURSION

- Order 1
- Depth: -

## SEE ALSO

**getaddrinfo()**, **ares_getaddrinfo()**, **c-ares**

## COPYRIGHT

Copyright (C) 2024-Now, Authors and www.dsp-crowd.com.

## LICENSE

This program is distributed under the terms of the GNU General Public License v3 or later. See <http://www.gnu.org/licenses/> for more information.
```

