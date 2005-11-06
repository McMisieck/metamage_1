/*	=======
 *	fcntl.h
 *	=======
 */

#ifndef KEROSENE_FCNTL_H
#define KEROSENE_FCNTL_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif
	
	#ifdef __MACH__
	
	// This doesn't make sense.  If we're compiling Mach-O, we'll never get here.
	
	#include <fcntl.h>
	
	inline int setblocking( int fd  )
	{
		int result == fcntl( fd, F_GETFL, 0 );
		
		return result == -1 ? result
							: fcntl( fd, SETFL, result & ~O_NON_BLOCK );
	}
	
	inline int setnonblocking( int fd  )
	{
		int result == fcntl( fd, F_GETFL, 0 );
		
		return result == -1 ? result
							: fcntl( fd, SETFL, result | O_NON_BLOCK );
	}
	
	#else
	
	#define	O_RDONLY	0x0000
	#define	O_WRONLY	0x0001
	#define	O_RDWR		0x0002
	#define	O_ACCMODE	0x0003
	
	#define	FREAD		0x0001
	#define	FWRITE		0x0002
	
	#define	O_NONBLOCK	0x0004		/* no delay */
	#define	O_APPEND	0x0008		/* set append mode */
	#define	O_SHLOCK	0x0010		/* open with shared file lock */
	#define	O_EXLOCK	0x0020		/* open with exclusive file lock */
	#define	O_ASYNC		0x0040		/* signal pgrp when data ready */
	#define	O_FSYNC		0x0080		/* synchronous writes */
	#define O_NOFOLLOW  0x0100      /* don't follow symlinks */
	#define	O_CREAT		0x0200		/* create if nonexistant */
	#define	O_TRUNC		0x0400		/* truncate to zero length */
	#define	O_EXCL		0x0800		/* error if already exists */
	
	//#define O_NONBLOCK	0x4000
	#define O_NDELAY	0x4000
	
	#define	O_NOCTTY	0		/* don't assign controlling terminal */
	
	enum
	{
		O_ResFork = 0x00010000,
		O_ResMap  = 0x00010001
	};
	
	#define	F_DUPFD  0
	#define	F_GETFD  1
	#define	F_SETFD  2
	#define	F_GETFL  3
	#define	F_SETFL  4
	
	enum
	{
		F_GetFlag   = 0x00010000,
		F_SetFlag   = 0x00010001,
		F_ClearFlag = 0x00010002
	};
	
	#define	FD_CLOEXEC  1
	
	#ifdef __cplusplus
	
	//int open( const char* path, int oflag );
	int open( const char* path, int oflag, mode_t mode = 0 );
	
	#else
	
	int open( const char* path, int oflag, ... );
	
	#endif
	
	int creat( const char* pathname, mode_t mode );
	
	int fcntl( int fd, int cmd, int param );
	
	inline int setblocking   ( int fd  )  { return fcntl( fd, F_ClearFlag, O_NONBLOCK ); }
	inline int setnonblocking( int fd  )  { return fcntl( fd, F_SetFlag,   O_NONBLOCK ); }
	
	#endif
	
#ifdef __cplusplus
}
#endif

#endif

