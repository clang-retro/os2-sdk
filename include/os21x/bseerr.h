/* bseerr.h - OS/2 error codes, for clang-retro. */
#ifndef __BSEERR_H
#define __BSEERR_H

#define NO_ERROR                     0
#define ERROR_INVALID_FUNCTION       1
#define ERROR_FILE_NOT_FOUND         2
#define ERROR_PATH_NOT_FOUND         3
#define ERROR_TOO_MANY_OPEN_FILES    4
#define ERROR_ACCESS_DENIED          5
#define ERROR_INVALID_HANDLE         6
#define ERROR_ARENA_TRASHED          7
#define ERROR_NOT_ENOUGH_MEMORY      8
#define ERROR_INVALID_BLOCK          9
#define ERROR_BAD_ENVIRONMENT        10
#define ERROR_BAD_FORMAT             11
#define ERROR_INVALID_ACCESS         12
#define ERROR_INVALID_DATA           13
#define ERROR_INVALID_DRIVE          15
#define ERROR_CURRENT_DIRECTORY      16
#define ERROR_NOT_SAME_DEVICE        17
#define ERROR_NO_MORE_FILES          18
#define ERROR_WRITE_PROTECT          19
#define ERROR_NOT_READY              21
#define ERROR_SHARING_VIOLATION      32
#define ERROR_LOCK_VIOLATION         33
#define ERROR_HANDLE_EOF             38
#define ERROR_NOT_SUPPORTED          50
#define ERROR_FILE_EXISTS            80
#define ERROR_CANNOT_MAKE            82
#define ERROR_INVALID_PARAMETER      87
#define ERROR_INTERRUPT              95
#define ERROR_TOO_MANY_SEMAPHORES    100
#define ERROR_SEM_IS_SET             102
#define ERROR_DRIVE_LOCKED           108
#define ERROR_BROKEN_PIPE            109
#define ERROR_OPEN_FAILED            110
#define ERROR_BUFFER_OVERFLOW        111
#define ERROR_DISK_FULL              112
#define ERROR_NO_MORE_SEARCH_HANDLES 113
#define ERROR_INVALID_TARGET_HANDLE  114
#define ERROR_INVALID_NAME           123
#define ERROR_INVALID_LEVEL          124
#define ERROR_NEGATIVE_SEEK          131
#define ERROR_SEEK_ON_DEVICE         132
#define ERROR_DIR_NOT_EMPTY          145
#define ERROR_PATH_BUSY              148
#define ERROR_ALREADY_EXISTS         183
#define ERROR_INVALID_EXE_SIGNATURE  191
#define ERROR_EXE_MARKED_INVALID     192
#define ERROR_BAD_EXE_FORMAT         193
#define ERROR_FILENAME_EXCED_RANGE   206
#define ERROR_PIPE_BUSY              231
#define ERROR_NO_DATA                232
#define ERROR_MORE_DATA              234
#define ERROR_TIMEOUT                640

#endif /* __BSEERR_H */
