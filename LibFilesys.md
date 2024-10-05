
# LIB_FILESYS Manual Page

## NAME
**LibFilesys** – A library providing file and directory management, pipe operations, and system resource control.

## SYNOPSIS
```cpp
#include "LibFilesys.h"

// File Operations
int fdCreate(const std::string &path, const std::string &mode, bool closeOnExec = true);
void fdClose(int &fd, bool deInit = true);
bool fileExists(const std::string &path);
bool dirExists(const std::string &path);
bool dirCreate(const std::string &path, mode_t mode = (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));

// Pipe Operations
void pipeInit(PairFd &pair);
void pipeClose(PairFd &pair, bool deInit = true);

// Resource Control
bool maxFdsSet(rlim_t val);
```

## DESCRIPTION
**LibFilesys** provides a set of functions for managing files, directories, file descriptors, and system resources. It simplifies common operations, such as checking for file existence, creating files and directories, working with pipes, and handling system-level file descriptor limits and core dumps. This library is designed for developers who need to interact with the filesystem and system resources in a cross-platform manner, particularly in Linux environments.

## FILE OPERATIONS

- **int fdCreate(const std::string &path, const std::string &mode, bool closeOnExec = true)**  
  Creates a file descriptor for the specified path.
  - **path**: Path to the file to create.
  - **mode**: File mode (e.g., `"r"`, `"w"`, `"rw"`).
  - **closeOnExec**: If `true`, the file descriptor will be automatically closed when `exec` is called (default: `true`).

- **void fdClose(int &fd, bool deInit = true)**  
  Closes an open file descriptor and optionally deinitializes it.
  - **fd**: The file descriptor to close.
  - **deInit**: If `true`, the file descriptor will be set to `-1` after closing (default: `true`).

- **bool fileExists(const std::string &path)**  
  Checks if a file exists at the specified path.
  - **path**: The file path to check.

  **Returns**: `true` if the file exists, otherwise `false`.

- **bool fileCreate(const std::string &path)**  
  Creates a file at the specified path.
  - **path**: The path to the file to be created.

  **Returns**: `true` if the file was successfully created, otherwise `false`.

- **bool dirExists(const std::string &path)**  
  Checks if a directory exists at the specified path.
  - **path**: The directory path to check.

  **Returns**: `true` if the directory exists, otherwise `false`.

- **bool dirCreate(const std::string &path, mode_t mode = (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH))**  
  Creates a directory with the specified permissions.
  - **path**: The path of the directory to be created.
  - **mode**: Permissions for the new directory (default: `S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH`).

  **Returns**: `true` if the directory was created successfully, otherwise `false`.

- **bool strToFile(const std::string &str, const std::string &path)**  
  Writes a string to a file at the specified path.
  - **str**: The content to write to the file.
  - **path**: The file path where the content will be written.

  **Returns**: `true` on success, otherwise `false`.

## PIPE OPERATIONS

- **void pipeInit(PairFd &pair)**  
  Initializes a pair of file descriptors for a pipe.
  - **pair**: A `PairFd` structure that will hold the read and write file descriptors for the pipe.

- **void pipeClose(PairFd &pair, bool deInit = true)**  
  Closes both ends of a pipe and optionally deinitializes the `PairFd` structure.
  - **pair**: The `PairFd` structure containing the file descriptors to close.
  - **deInit**: If `true`, the file descriptors will be set to `-1` after closing (default: `true`).

## RESOURCE CONTROL

- **bool maxFdsSet(rlim_t val)**  
  Sets the maximum number of file descriptors the process can open.
  - **val**: The desired maximum file descriptor limit.

  **Returns**: `true` if successful, otherwise `false`.

- **bool coreDumpsEnable(void (\*pFctReq)(int signum) = NULL)**  
  (Linux only) Enables core dumps and sets an optional signal handler.
  - **pFctReq**: Pointer to a signal handling function for the core dump. If `NULL`, no custom handler is used.

  **Returns**: `true` if core dumps were enabled, otherwise `false`.

## LOCK MANAGEMENT

- **bool lockDirDefaultOpen(const std::string &dirBase)**  
  Opens a lock directory for system-wide locking.
  - **dirBase**: The base directory to be used for locking.

  **Returns**: `true` if the lock directory was successfully opened, otherwise `false`.

- **void lockDirDefaultClose()**  
  Closes the lock directory and releases any system-wide locks.

- **Success sysFlagsIntLock(void \*pRequester, const char \*filename, const char \*function, const int line, UserLocks \*pLocks, ...)**  
  Internal function to acquire a system-wide lock for a given set of flags. Typically used with the `sysFlagsLock()` macro.
  - **pRequester**: Pointer to the requesting object.
  - **filename**: The name of the file where the lock is requested.
  - **function**: The function name where the lock is requested.
  - **line**: Line number in the code where the lock is requested.
  - **pLocks**: Pointer to the user locks structure.

- **void sysFlagsIntUnlock(void \*pRequester, const char \*filename, const char \*function, const int line, UserLocks \*pLocks)**  
  Internal function to release a system-wide lock. Typically used with the `sysFlagsUnlock()` macro.
  - **pRequester**: Pointer to the requesting object.
  - **filename**: The name of the file where the lock is being released.
  - **function**: The function name where the lock is being released.
  - **line**: Line number in the code where the lock is being released.
  - **pLocks**: Pointer to the user locks structure.

## EXAMPLES
```cpp
// Example of creating a file and writing to it
std::string path = "/tmp/testfile.txt";
if (fileCreate(path)) {
    std::cout << "File created successfully: " << path << std::endl;
} else {
    std::cerr << "Failed to create file: " << path << std::endl;
}

// Example of working with pipes
PairFd pipe;
pipeInit(pipe);
// Use pipe.fdRead and pipe.fdWrite for reading/writing
pipeClose(pipe);
```

## SEE ALSO
- `Processing()`, `LibTime`, `LibDspc`

## AUTHORS
Written by Johannes Natter.

