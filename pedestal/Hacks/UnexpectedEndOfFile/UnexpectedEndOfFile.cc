/*
	
	Unexpected End Of File
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <MacWindows.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <Resources.h>
#include <Sound.h>

// Silver
#include "Silver/Patches.hh"

// UnexpectedEndOfFile
#include "UEOFUtils.hh"


namespace Ag = Silver;

using namespace Ag::Trap_ProcPtrs;


static Point gLastMouseLoc;

struct QuitInfo
{
	ProcessSerialNumber  psn;
	unsigned             menuCode;
};

enum { stashSize = 20 };

static unsigned stashCount;
static QuitInfo gStash[ stashSize ];


inline MenuHandle GetAppMenuHandle()
{
	const UInt16 appMenuID = 0xbf97;
	
	MenuHandle appMenu = GetMenuHandle( appMenuID );
	
	return appMenu;
}

inline bool StashIsFull()
{
	return stashCount >= stashSize;
}

inline QuitInfo* NextStashSlot()
{
	if ( StashIsFull() )
	{
		return NULL;
	}
	
	return &gStash[ stashCount++ ];
}

static QuitInfo* FindPSNInStash( const ProcessSerialNumber& psn )
{
	for ( unsigned i = 0; i < stashCount; ++i )
	{
		if ( gStash[i].psn == psn )
		{
			return &gStash[i];
		}
	}
	
	return NULL;
}

static void StoreQuitInfo( MenuHandle menu, short item )
{
	UInt32 code = menu[0]->menuID << 16  |  item;
	
	ProcessSerialNumber psn = CurrentPSN();
	
	QuitInfo* info = FindPSNInStash( psn );
	
	if ( info == NULL )
	{
		info = NextStashSlot();
	}
	
	if ( info != NULL )
	{
		info->psn      = psn;
		info->menuCode = code;
	}
}

static unsigned GetQuitInfo()
{
	ProcessSerialNumber psn = CurrentPSN();
	
	QuitInfo* info = FindPSNInStash( psn );
	
	if ( info == NULL )
	{
		return 0;
	}
	
	return info->menuCode;
}

namespace
{
	
	Boolean PatchedStillDown( StillDownProcPtr nextHandler )
	{
		bool stillDown = nextHandler();
		
		if ( !stillDown )
		{
			GetMouse( &gLastMouseLoc );
		}
		
		return stillDown;
	}
	
	
	long PatchedMenuKey( short c, MenuKeyProcPtr nextHandler )
	{
		if ( (c | ' ') == 'q' )
		{
			return GetQuitInfo();
		}
		
		return nextHandler( c );
	}
	
	long PatchedMenuSelect( Point startPt, MenuSelectProcPtr nextHandler )
	{
		if ( IsFinderInForeground() )
		{
			return nextHandler( startPt );
		}
		
		MenuHandle appMenu = GetAppMenuHandle();
		
		short count = CountMenuItems( appMenu );
		
		::AppendMenu( appMenu, "\p" "Quit/Q" );
		
		//TemporaryPatchApplied< StillDownPatch > stillDownPatch( PatchedStillDown );
		//gOriginalStillDown = stillDownPatch.Original();
		
		typedef Ag::TrapPatch< _StillDown, PatchedStillDown > StillDownPatch;
		
		StillDownPatch::Install();
		
		long result = nextHandler( startPt );
		
		StillDownPatch::Remove();
		
		Handle menuProcH = appMenu[0]->menuProc;
		short state = ::HGetState( menuProcH );
		::HLock( menuProcH );
		
		MenuDefProcPtr mdefProc = reinterpret_cast< MenuDefProcPtr >( *menuProcH );
		
		GrafPtr wMgrPort;
		GetWMgrPort( &wMgrPort );
		
		Rect rect;
		rect.top = 20;
		rect.right = wMgrPort->portRect.right - 11;
		rect.bottom = rect.top + appMenu[0]->menuHeight;
		rect.left = rect.right - appMenu[0]->menuWidth;
		
		short whichItem;
		
		{
			ThingWhichPreventsMenuItemDrawing thing;
			
			mdefProc( mChooseMsg, appMenu, &rect, gLastMouseLoc, &whichItem );
		}
		
		::HSetState( menuProcH, state );
		
		::DeleteMenuItem( appMenu, count + 1 );
		
		if ( whichItem > count )
		{
			return GetQuitInfo();
		}
		
		return result;
	}
	
	void PatchedInsertMenu( MenuHandle menu, short beforeID, InsertMenuProcPtr nextHandler )
	{
		if ( EqualPstrings( menu[0]->menuData, "\p" "File" ) )
		{
			short count = CountMenuItems( menu );
			
			Str255 itemName;
			::GetMenuItemText( menu, count, itemName );
			
			if ( EqualPstrings( itemName, "\p" "Quit" ) )
			{
				StoreQuitInfo( menu, count );
				
				::DeleteMenuItem( menu, count );
				--count;
				
				::GetMenuItemText( menu, count, itemName );
				
				if ( itemName[1] == '-' )
				{
					::DeleteMenuItem( menu, count );
				}
			}
		}
		
		nextHandler( menu, beforeID );
	}
	
}


static bool Install()
{
	bool locked = Ag::LoadAndLock();
	
	if ( !locked )
	{
		return false;
	}
	
	Ag::MyA4 a4;
	
	stashCount = 0;
	
	Ag::TrapPatch< _MenuSelect, PatchedMenuSelect >::Install();
	Ag::TrapPatch< _InsertMenu, PatchedInsertMenu >::Install();
	Ag::TrapPatch< _MenuKey,    PatchedMenuKey    >::Install();
	
	return true;
}

void main()
{
	bool installed = Install();
}

