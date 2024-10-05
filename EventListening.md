
# EVENT_LISTENING() Manual Page

## NAME
**EventListening** – Event-driven TCP connection listener with message queue for asynchronous event processing.

## SYNOPSIS
```cpp
#include "EventListening.h"

EventListening *create();
ssize_t pop(const std::string &refMsg, Json::Value &msgEvent);
```

## DESCRIPTION
**EventListening** is a C++ class designed to handle TCP-based event connections and facilitate the asynchronous processing of JSON-encoded messages. It provides a way to listen for incoming TCP connections, manage active connections, and enqueue or dequeue events. The class is useful for building services that need to handle multiple simultaneous connections while processing events in real-time.

### Features:
- **TCP Connection Management**: Accepts new connections and manages existing ones using `TcpListening` and `TcpTransfering`.
- **Event Queue**: Provides mechanisms for enqueuing and dequeuing JSON-encoded messages.
- **Timeout Handling**: Monitors open connections and automatically removes timed-out or inactive connections.
- **Thread-Safe Event Management**: Uses mutexes to protect the event queue in multi-threaded environments.

## METHODS

### TCP Connection Handling
- **create()**  
  Allocates and initializes a new instance of the **EventListening** class.

### Event Queue
- **pop(const std::string &refMsg, Json::Value &msgEvent)**  
  Retrieves the next event message from the queue. If a message matching the reference `refMsg` is found, it is copied to `msgEvent` and removed from the queue. The function returns the size of the message on success or `-1` on failure.

### Protected Methods
These methods are used internally by the class to manage the listening process and handle incoming connections:

- **EventListening()**  
  Constructor that initializes the event listener object.

- **~EventListening()**  
  Destructor that cleans up resources used by the event listener.

- **process()**  
  Starts the main event processing loop, accepting connections and managing the event queue.

- **processInfo(char *pBuf, char *pBufEnd)**  
  Provides status or debugging information about the event processing.

- **connectionsAccept()**  
  Accepts incoming TCP connections and adds them to the list of open connections.

- **dataTimeoutsCheck()**  
  Checks for timed-out connections and removes them from the list.

- **dataReceive()**  
  Reads data from active connections and enqueues events.

- **msgEnqueue(TcpTransfering *pConn)**  
  Queues an event message from a TCP connection.

- **dequeueTimeoutsCheck()**  
  Scans the event queue for timed-out messages and removes them.

## MEMBER VARIABLES
The class maintains several internal variables to track connections, events, and buffer states:

- **mStartMs**  
  Stores the start time of the listener process.

- **mpLst**  
  Pointer to a **TcpListening** object that manages incoming TCP connections.

- **mConnsOpen**  
  A list of open connections, each represented by an **OpenEventConn** struct, which tracks the connection and its start time.

- **mBuf[512]**  
  A buffer for receiving data from connections.

- **mLenBuf**  
  Tracks the current length of data in the buffer.

### Static Variables
- **mMtxEvents**  
  Mutex that guards access to the event queue, ensuring thread-safe operations.

- **mEvents**  
  A map of event messages, with each event referenced by a string key and containing a `Json::Value` object for the event data.

## STRUCTURES

### OpenEventConn
```cpp
struct OpenEventConn
{
    TcpTransfering *pConn;
    uint32_t msStart;
};
```
- **pConn**: Pointer to a **TcpTransfering** object representing the open TCP connection.
- **msStart**: The time (in milliseconds) when the connection was established.

## EXAMPLES
```cpp
EventListening *eventListener = EventListening::create();
Json::Value eventData;
if (eventListener->pop("event1", eventData) > 0) {
    // Process the eventData
}
```
In this example, an event listener is created, and an event with the reference "event1" is dequeued from the event queue and processed.

## RETURN VALUES
- **pop()**: Returns the size of the event message if successful, or `-1` if no matching message is found.

## THREAD SAFETY
The class employs mutexes to ensure thread-safe access to the event queue. This allows for the safe use of the `pop()` method in multi-threaded environments.

## NOTES
- The class is designed for use in event-driven systems where multiple TCP connections are accepted and managed simultaneously.
- It is not copyable or assignable to prevent issues related to resource management and connection states.

## SEE ALSO
- `Processing()`, `TcpListening()`, `TcpTransfering()`, `jsoncpp`

## AUTHORS
Written by Johannes Natter.

