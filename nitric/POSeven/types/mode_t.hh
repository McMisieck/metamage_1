// mode_t.hh
// ---------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2008 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef POSEVEN_TYPES_MODE_T_HH
#define POSEVEN_TYPES_MODE_T_HH

// POSIX
#include <sys/stat.h>

// Nucleus
#include "Nucleus/Enumeration.h"


namespace poseven
{
	
	enum mode_t
	{
		s_ifmt  = S_IFMT,
		s_ifblk = S_IFBLK,
		s_ifchr = S_IFCHR,
		s_ififo = S_IFIFO,
		s_ifdir = S_IFDIR,
		s_iflnk = S_IFLNK,
		
		s_irwxu = S_IRWXU,
		s_irusr = S_IRUSR,
		s_iwusr = S_IWUSR,
		s_ixusr = S_IXUSR,
		s_irwxg = S_IRWXG,
		s_irgrp = S_IRGRP,
		s_iwgrp = S_IWGRP,
		s_ixgrp = S_IXGRP,
		s_irwxo = S_IRWXO,
		s_iroth = S_IROTH,
		s_iwoth = S_IWOTH,
		s_ixoth = S_IXOTH,
		s_isuid = S_ISUID,
		s_isgid = S_ISGID,
		
	#ifdef S_ISVTX
		
		s_isvtx = S_ISVTX,
		
	#endif
		
		mode_t_max = Nucleus::Enumeration_Traits< ::mode_t >::max
	};
	
	inline bool s_isblk ( mode_t mode )  { return S_ISBLK ( mode ); }
	inline bool s_ischr ( mode_t mode )  { return S_ISCHR ( mode ); }
	inline bool s_isdir ( mode_t mode )  { return S_ISDIR ( mode ); }
	inline bool s_isfifo( mode_t mode )  { return S_ISFIFO( mode ); }
	inline bool s_isreg ( mode_t mode )  { return S_ISREG ( mode ); }
	inline bool s_islnk ( mode_t mode )  { return S_ISLNK ( mode ); }
	
}

#endif

