/*	==========
 *	Retrace.hh
 *	==========
 */

#ifndef CLASSICTOOLBOX_RETRACE_HH
#define CLASSICTOOLBOX_RETRACE_HH

// Mac OS
#ifndef __RETRACE__
#include <Retrace.h>
#endif

// nucleus
#ifndef NUCLEUS_ERRORSREGISTERED_HH
#include "nucleus/errors_registered.hh"
#endif

// Nitrogen
#ifndef NITROGEN_UPP_HH
#include "Nitrogen/UPP.hh"
#endif


namespace Nitrogen
{
	
	NUCLEUS_DECLARE_ERRORS_DEPENDENCY( VerticalRetraceManager );
	
	
	struct SlotVBLTask
	{
		VBLTask*  task;
		short     slot;
		
		SlotVBLTask() : task( NULL ), slot( 0 )  {}
		
		SlotVBLTask( VBLTask*  task,
		             short     slot )
		:
			task( task ),
			slot( slot )
		{}
	};
	
	inline bool operator==( const SlotVBLTask& a, const SlotVBLTask& b )
	{
		return a.task == b.task
		    && a.slot == b.slot;
	}
	
	inline bool operator!=( const SlotVBLTask& a, const SlotVBLTask& b )
	{
		return !( a == b );
	}
	
}

namespace nucleus
{
	
	template <>
	struct disposer< VBLTaskPtr >
	{
		typedef VBLTaskPtr  argument_type;
		typedef void        result_type;
		
		void operator()( VBLTaskPtr vblTaskPtr ) const
		{
			const QElemPtr qElem = (QElemPtr) vblTaskPtr;
			
			(void) ::VRemove( qElem );
		}
	};
	
	template <>
	struct disposer< Nitrogen::SlotVBLTask >
	{
		typedef Nitrogen::SlotVBLTask  argument_type;
		typedef void                   result_type;
		
		void operator()( Nitrogen::SlotVBLTask slotVBLTask ) const
		{
			const QElemPtr qElem = (QElemPtr) slotVBLTask.task;
			
			(void) ::SlotVRemove( qElem, slotVBLTask.slot );
		}
	};
	
}

namespace Nitrogen
{
	
	namespace Private
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		inline void InvokeVBLUPP( VBLTaskPtr vblTaskPtr, ::VBLUPP userUPP )
		{
			::InvokeVBLUPP( vblTaskPtr, userUPP );
		}
		
	#else
		
		using ::InvokeVBLUPP;
		
	#endif
	}
	
	struct VBLUPP_Details : Basic_UPP_Details< ::VBLUPP,
	                                           ::VBLProcPtr,
	                                           ::NewVBLUPP,
	                                           ::DisposeVBLUPP,
	                                           Private::InvokeVBLUPP >
	{};
	
	typedef UPP< VBLUPP_Details > VBLUPP;
	
	inline nucleus::owned< VBLUPP > NewVBLUPP( ::VBLProcPtr p )
	{
		return NewUPP< VBLUPP >( p );
	}
	
	inline void DisposeVBLUPP( nucleus::owned< VBLUPP > )  {}
	
#if !TARGET_CPU_68K || TARGET_RT_MAC_CFM
	
	inline void InvokeVBLUPP( VBLTaskPtr  vblTaskPtr,
	                          VBLUPP      userUPP )
	{
		userUPP( vblTaskPtr );
	}
	
#endif
	
	nucleus::owned< SlotVBLTask > SlotVInstall( VBLTask& vblTask, short slot );
	
	void SlotVRemove( nucleus::owned< SlotVBLTask > vblTask );
	
	// ...
	
	nucleus::owned< VBLTaskPtr > VInstall( VBLTask& vblTask );
	
	void VRemove( nucleus::owned< VBLTaskPtr > vblTask );
	
#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
	namespace Detail
	{
		
		typedef pascal void (*StackBased_VBLProcPtr)( VBLTaskPtr task );
		
		template < StackBased_VBLProcPtr proc >
		pascal void CallStackBasedVBLProcPtr()
		{
			asm
			{
				MOVE.L   A0, -(SP) ;  // taskPtr
				
				JSR      proc      ;
			}
		}
		
	}
	
	template < Detail::StackBased_VBLProcPtr proc >
	struct VBLProcPtr_Traits
	{
		static VBLProcPtr GetProcPtr()
		{
			return &Detail::CallStackBasedVBLProcPtr< proc >;
		}
	};
	
#else
	
	template < VBLProcPtr proc >
	struct VBLProcPtr_Traits
	{
		static VBLProcPtr GetProcPtr()
		{
			return proc;
		}
	};
	
#endif  // TARGET_CPU_68K && !TARGET_RT_MAC_CFM
	
}

#endif
