/*	==============
 *	RegularFile.cc
 *	==============
 */

#include "Genie/IO/RegularFile.hh"

// POSIX
#include <fcntl.h>

// POSeven
#include "POSeven/Errno.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	
	
	RegularFileHandle::RegularFileHandle( OpenFlags flags ) : FileHandle( flags ), itsMark()
	{
	}
	
	RegularFileHandle::~RegularFileHandle()
	{
	}
	
	ssize_t RegularFileHandle::Positioned_Read( char* buffer, size_t n_bytes, off_t offset )
	{
		p7::throw_errno( EPERM );
		
		return 0;
	}
	
	ssize_t RegularFileHandle::Positioned_Write( const char* buffer, size_t n_bytes, off_t offset )
	{
		p7::throw_errno( EPERM );
		
		return 0;
	}
	
	ssize_t RegularFileHandle::Append( const char* buffer, size_t n_bytes )
	{
		itsMark = GetEOF();
		
		return Positioned_Write( buffer, n_bytes, itsMark );
	}
	
	ssize_t RegularFileHandle::SysRead( char* buffer, size_t n_bytes )
	{
		ssize_t read = Positioned_Read( buffer,
		                                n_bytes,
		                                GetFileMark() );
		
		return Advance( read );
	}
	
	ssize_t RegularFileHandle::SysWrite( const char* buffer, size_t n_bytes )
	{
		const bool appending = GetFlags() & O_APPEND;
		
		ssize_t written = appending ? Append( buffer, n_bytes )
		                            : Positioned_Write( buffer,
		                                                n_bytes,
		                                                GetFileMark() );
		
		return Advance( written );
	}
	
	off_t RegularFileHandle::Seek( off_t offset, int whence )
	{
		off_t base = 0;
		
		switch ( whence )
		{
			case SEEK_SET:
				base = 0;
				break;
			
			case SEEK_CUR:
				base = GetFileMark();
				break;
			
			case SEEK_END:
				base = GetEOF();
				break;
			
			default:
				p7::throw_errno( EINVAL );
		}
		
		itsMark = base + offset;
		
		return itsMark;
	}
	
	off_t RegularFileHandle::GetEOF()
	{
		return GetFile()->GetEOF();
	}
	
	void RegularFileHandle::SetEOF( off_t length )
	{
		GetFile()->SetEOF( length );
	}
	
	ssize_t RegularFileHandle::Write( const char* buffer, std::size_t byteCount )
	{
		OpenFlags flags = GetFlags();
		
		if ( flags & O_TRUNC_LAZY )
		{
			SetEOF( GetFileMark() );
			
			SetFlags( flags & ~O_LAZY );
		}
		
		return SysWrite( buffer, byteCount );
	}
	
}

