/*
	Genie/FS/sys/mac/unit.cc
	------------------------
*/

#if defined( __MACOS__ )  &&  !TARGET_API_MAC_CARBON

#include "Genie/FS/sys/mac/unit.hh"

// Standard C
#include <ctype.h>

// gear
#include "gear/inscribe_decimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/contains.hh"
#include "plus/serialize.hh"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// vfs
#include "vfs/dir_contents.hh"
#include "vfs/dir_entry.hh"
#include "vfs/node/types/fixed_dir.hh"

// Genie
#include "Genie/FS/basic_directory.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/property.hh"
#include "Genie/FS/serialize_Str255.hh"
#include "Genie/Utilities/canonical_positive_integer.hh"


namespace Nitrogen
{
	
	UnitTableDrivers_Container_Specifics::key_type
	//
	UnitTableDrivers_Container_Specifics::get_next_key( key_type key )
	{
		const key_type end = end_key();
		
		while ( ++key < end  &&  *key == NULL )
		{
			continue;
		}
		
		return key;
	}
	
}

namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	struct decode_unit_number
	{
		static unsigned apply( const plus::string& name )
		{
			return gear::parse_unsigned_decimal( name.c_str() );
		}
	};
	
	static UnitNumber GetKey( const FSTree* that )
	{
		return UnitNumber( decode_unit_number::apply( that->name() ) );
	}
	
	
	static inline AuxDCEHandle* GetUTableBase()
	{
		return (AuxDCEHandle*) LMGetUTableBase();
	}
	
	
	static bool is_valid_unit_number( UInt32 i )
	{
		AuxDCEHandle* base = GetUTableBase();
		
		const UInt16 count = LMGetUnitTableEntryCount();
		
		return i < count  &&  base[ i ] != NULL;
	}
	
	
	struct GetDriverFlags : plus::serialize_hex< short >
	{
		static short Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			return dceHandle[0]->dCtlFlags;
		}
	};
	
	const unsigned char* GetDriverName_WithinHandle( AuxDCEHandle dceHandle )
	{
		ASSERT( dceHandle != NULL );
		
		if ( dceHandle[0]->dCtlDriver != NULL )
		{
			const bool ramBased = dceHandle[0]->dCtlFlags & dRAMBasedMask;
			
			const Ptr drvr = dceHandle[0]->dCtlDriver;
			
			// Dereferences a handle if ramBased
			const DRVRHeaderPtr header = ramBased ? *(DRVRHeader **) drvr
			                                      :  (DRVRHeader * ) drvr;
			
			if ( header != NULL )
			{
				return header->drvrName;
			}
		}
		
		return "\p";
	}
	
	static N::Str255 GetDriverName( AuxDCEHandle dceHandle )
	{
		const unsigned char* name = GetDriverName_WithinHandle( dceHandle );
		
		// Safely copy Pascal string onto stack
		return N::Str255( name );
	}
	
	struct DriverName : serialize_Str255_contents
	{
		static N::Str255 Get( AuxDCEHandle dceHandle )
		{
			return GetDriverName( dceHandle );
		}
	};
	
	struct GetDriverSlot : plus::serialize_int< char >
	{
		// dCtlSlot is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlot == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlSlot;
		}
	};
	
	struct GetDriverSlotId : plus::serialize_int< char >
	{
		// dCtlSlotId is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlSlotId == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlSlotId;
		}
	};
	
	struct GetDriverBase : plus::serialize_hex< UInt32 >
	{
		static UInt32 Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlDevBase == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlDevBase;
		}
	};
	
	struct GetDriverOwner : plus::serialize_pointer
	{
		static Ptr Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlOwner == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlOwner;
		}
	};
	
	struct GetDriverExternalDeviceID : plus::serialize_int< char >
	{
		// dCtlExtDev is defined as 'char', but int is more debugger-friendly
		typedef int result_type;
		
		static int Get( AuxDCEHandle dceHandle )
		{
			ASSERT( dceHandle != NULL );
			
			if ( dceHandle[0]->dCtlExtDev == 0 )
			{
				p7::throw_errno( ENOENT );
			}
			
			return dceHandle[0]->dCtlExtDev;
		}
	};
	
	template < class Accessor >
	struct sys_mac_unit_N_Property : readonly_property
	{
		static const int fixed_size = Accessor::fixed_size;
		
		static void get( plus::var_string& result, const FSTree* that, bool binary )
		{
			UnitNumber key = GetKey( that );
			
			if ( !is_valid_unit_number( key ) )
			{
				throw undefined_property();
			}
			
			AuxDCEHandle dceHandle = GetUTableBase()[ key ];
			
			const typename Accessor::result_type data = Accessor::Get( dceHandle );
			
			Accessor::deconstruct::apply( result, data, binary );
		}
	};
	
	
	struct valid_name_of_unit_number
	{
		typedef canonical_positive_integer well_formed_name;
		
		static bool applies( const plus::string& name )
		{
			return well_formed_name::applies( name )  &&  is_valid_unit_number( decode_unit_number::apply( name ) );
		}
	};
	
	extern const vfs::fixed_mapping sys_mac_unit_N_Mappings[];
	
	static FSTreePtr unit_lookup( const FSTree* parent, const plus::string& name )
	{
		if ( !valid_name_of_unit_number::applies( name ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		return fixed_dir( parent, name, sys_mac_unit_N_Mappings );
	}
	
	class unit_IteratorConverter
	{
		public:
			vfs::dir_entry operator()( N::UnitTableDrivers_Container::const_reference ref ) const
			{
				const int i = &ref - GetUTableBase();
				
				const ino_t inode = i;
				
				plus::string name = gear::inscribe_decimal( i );
				
				return vfs::dir_entry( inode, name );
			}
	};
	
	static void unit_iterate( const FSTree* parent, vfs::dir_contents& cache )
	{
		unit_IteratorConverter converter;
		
		N::UnitTableDrivers_Container sequence = N::UnitTableDrivers();
		
		std::transform( sequence.begin(),
		                sequence.end(),
		                std::back_inserter( cache ),
		                converter );
	}
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< sys_mac_unit_N_Property< prop > >::value
	
	const vfs::fixed_mapping sys_mac_unit_N_Mappings[] =
	{
		{ "flags",  PROPERTY( GetDriverFlags            ) },
		{ "name",   PROPERTY( DriverName                ) },
		{ "slot",   PROPERTY( GetDriverSlot             ) },
		{ "id",     PROPERTY( GetDriverSlotId           ) },
		{ "base",   PROPERTY( GetDriverBase             ) },
		{ "owner",  PROPERTY( GetDriverOwner            ) },
		{ "extdev", PROPERTY( GetDriverExternalDeviceID ) },
		
		{ ".~flags",  PROPERTY( GetDriverFlags            ) },
		{ ".~name",   PROPERTY( DriverName                ) },
		{ ".~slot",   PROPERTY( GetDriverSlot             ) },
		{ ".~id",     PROPERTY( GetDriverSlotId           ) },
		{ ".~base",   PROPERTY( GetDriverBase             ) },
		{ ".~owner",  PROPERTY( GetDriverOwner            ) },
		{ ".~extdev", PROPERTY( GetDriverExternalDeviceID ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_sys_mac_unit( const FSTree*        parent,
	                                   const plus::string&  name,
	                                   const void*          args )
	{
		return new_basic_directory( parent, name, unit_lookup, unit_iterate );
	}
	
}

#endif

