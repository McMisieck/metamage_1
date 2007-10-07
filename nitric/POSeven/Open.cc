// Open.cc
// -------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2006-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "POSeven/Open.hh"


namespace poseven
{
	
	Nucleus::Owned< fd_t > open( const char* name, int oflag, int mode )
	{
		// register errnos
		
		int fd;
		
		throw_posix_result( fd = ::open( name, oflag, mode ) );
		
		return Nucleus::Owned< fd_t >::Seize( fd_t( fd ) );
	}
	
	
	namespace Testing
	{
		
		static void Foo()
		{
			fd_t foo = open( "/etc/motd", o_rdonly, 0 );
			
			//Read( foo );
			
			int fd = foo;
			
			// This is a compile error with enums!  YES!!
			//Write( fd );
		}
		
	}
	
}

