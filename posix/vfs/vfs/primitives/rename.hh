/*
	rename.hh
	---------
*/

#ifndef VFS_PRIMITIVES_RENAME
#define VFS_PRIMITIVES_RENAME

// vfs
#include "vfs/node_fwd.hh"


namespace vfs
{
	
	void rename( const node& that, const node& target );
	
}

#endif
