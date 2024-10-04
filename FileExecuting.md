
# FILE_EXECUTING(3) Manual Page

## NAME
**FileExecuting** – Execute system commands, manage input/output streams, and handle child processes in C++.

## SYNOPSIS
```cpp
#include "FileExecuting.h"

FileExecuting *create();

FileExecuting &msTimeoutSet(uint32_t msTimeout);
FileExecuting &sourceEnable();
FileExecuting &sourceSet(const char *pSrc, size_t len = 0, bool autoFree = false);
FileExecuting &sourceSet(const std::string *pStr);
FileExecuting &sourceSet(int fd, bool autoClose = false);
FileExecuting &sourceSet(Transfering *pTrans);

FileExecuting &cmdAdd(const VecConstChar &argv);
FileExecuting &cmdAdd(const std::string &cmd, const VecConstChar &argv);
FileExecuting &cmdAdd(const std::string &cmd, int argc, char *argv[]);

FileExecuting &envSet(const VecConstChar &envv, bool dropOld = true);
FileExecuting &sinkEnable(int fdSel = STDOUT_FILENO);
FileExecuting &sinkAdd(char *pDest, size_t len, int fdSel = STDOUT_FILENO);
FileExecuting &sinkAdd(std::string *pStr, int fdSel = STDOUT_FILENO);
FileExecuting &sinkAdd(int fd, bool autoClose = false, int fdSel = STDOUT_FILENO);
FileExecuting &sinkAdd(Transfering *pTrans, int fdSel = STDOUT_FILENO);

int sigSend(int sig, ssize_t idx = -1) const;
ssize_t send(const void *pData, size_t lenReq);
ssize_t read(void *pBuf, size_t lenReq);
```

## DESCRIPTION
**FileExecuting()** is a C++ class used to execute system commands and manage interactions with processes via pipes and file descriptors. It facilitates complex inter-process communication (IPC) by managing command execution, redirecting standard input/output, and handling child process states.

### Features:
- **Command Execution**: Add system commands to be executed using `cmdAdd()`. Commands can be passed as string vectors or arrays.
- **Environment Variable Management**: Customize the execution environment with `envSet()`.
- **Timeout Handling**: Use `msTimeoutSet()` to set a custom timeout for the execution process.
- **Input/Output Stream Handling**: Configure sources (`sourceSet()`) and sinks (`sinkAdd()`) for reading from and writing to processes.
- **Process State Management**: Query the status of child processes, including termination by signals, core dumps, or other state transitions.

### Structs:
- **FeNodeBuffer**: Manages data buffers for communication between source and destination nodes. 
- **FeFileDescSetting**: Represents file descriptors for use in file redirection, with options for automatic closure.
- **FeNode**: Represents a node in the execution pipeline, managing buffers, file descriptors, and transfers.
- **FeResult**: Holds the result of process execution, including child process states and exit codes.

## METHODS

### Process Management
- **create()**  
  Allocates a new instance of the **FileExecuting()** class.

- **msTimeoutSet(uint32_t msTimeout)**  
  Sets the timeout in milliseconds for command execution.

- **cmdAdd(const VecConstChar &argv)**  
  Adds a command to the execution pipeline using a vector of argument strings.

- **operator|= (const VecConstChar &argv)**  
  Adds a command to the execution pipeline using the bitwise OR assignment operator for convenience.

### Input Source Management
- **sourceEnable()**  
  Enables the source stream for input to the child process.

- **sourceSet(const char *pSrc, size_t len = 0, bool autoFree = false)**  
  Sets a source buffer for input.

- **sourceSet(const std::string *pStr)**  
  Sets a source string for input.

- **sourceSet(int fd, bool autoClose = false)**  
  Sets a file descriptor as the input source.

- **sourceSet(Transfering *pTrans)**  
  Links an existing **Transfering** object as the input source.

### Output Sink Management
- **sinkEnable(int fdSel = STDOUT_FILENO)**  
  Enables the sink stream for reading from the child process output.

- **sinkAdd(char *pDest, size_t len, int fdSel = STDOUT_FILENO)**  
  Adds a buffer as a destination for process output.

- **sinkAdd(std::string *pStr, int fdSel = STDOUT_FILENO)**  
  Redirects output to a string.

- **sinkAdd(int fd, bool autoClose = false, int fdSel = STDOUT_FILENO)**  
  Redirects output to a file descriptor.

- **sinkAdd(Transfering *pTrans, int fdSel = STDOUT_FILENO)**  
  Links an existing **Transfering** object to receive process output.

- **errRedirect()**  
  Redirects error output (stderr) to the same destination as stdout.

### Process State Queries
- **childStopped(ssize_t idx = -1, bool isAnd = false)**  
  Returns whether the child process has stopped.

- **childTerminated(ssize_t idx = -1, bool isAnd = true)**  
  Checks if the child process has terminated normally.

- **childTerminatedBySig(ssize_t idx = -1, bool isAnd = false)**  
  Checks if the child was terminated by a signal.

- **coreDumped(ssize_t idx = -1, bool isAnd = false)**  
  Checks if the child process generated a core dump.

- **idChild(ssize_t idx = -1)**  
  Returns the process ID of the child.

- **codeSig(ssize_t idx = -1)**  
  Returns the signal number that terminated the child, if applicable.

- **codeRet(ssize_t idx = -1)**  
  Returns the exit code of the child process.

## RETURN VALUES
Methods that modify state or configuration typically return a reference to the **FileExecuting()** object itself, enabling method chaining. Query methods return values specific to the process or child state (e.g., boolean flags or integer status codes).

## NOTES
- This class provides an abstraction for interacting with UNIX-like systems' process control features such as pipes, forks, and execs.
- The class is not copyable or assignable to avoid inadvertent resource sharing or duplication.

## SEE ALSO
- `pipe(2)`, `fork(2)`, `exec(3)`, `dup(2)`

## AUTHORS
Written by [Your Name].

This man page-style document is in markdown and explains the `FileExecuting()` class, its purpose, and usage, akin to a Unix `man` page.

