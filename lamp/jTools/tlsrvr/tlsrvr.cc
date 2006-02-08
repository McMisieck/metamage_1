/*	=========
 *	tlsrvr.cc
 *	=========
 */

// Standard C++
#include <algorithm>
#include <functional>
#include <memory>
#include <numeric>
#include <string>

// Standard C/C++
#include <cctype>

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen Carbon
#include "Nitrogen/Events.h"

// Nitrogen Extras / Templates
#include "Templates/FunctionalExtensions.h"

// Nitrogen Extras / Utilities
#include "Utilities/Processes.h"

// Orion
#include "Orion/GetOptions.hh"
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"

// tlsrvr
#include "ToolServer.hh"
#include "RunToolServer.hh"


namespace N = Nitrogen;
namespace NX = NitrogenExtras;
namespace O = Orion;

namespace ext = N::STLExtensions;

using std::string;
using std::vector;
using RunToolServer::sigToolServer;
using RunToolServer::sEscapedQuote;
using RunToolServer::RunCommandInToolServer;


template < class F >
class Concat
{
	private:
		F f;
	
	public:
		Concat() : f( F() )  {}
		Concat( const F& f ) : f( f )  {}
		string operator()( const string& one, const char* other ) const
		{
			return one + " " + f( other );
		}
};

template < class F >
Concat< F > MakeConcat( const F& f )
{
	return Concat< F >( f );
}


static string QuoteForMPW( const string& str )
{
	string::const_iterator p = str.begin(), q = p, end = str.end();
	
	bool needsQuoting = false;
	
	string result = "'";
	
	while ( p < end )
	{
		while ( q < end  &&  *q != '\'' )
		{
			needsQuoting = needsQuoting || !std::isalnum( *q );
			++q;
		}
		
		result += string( p, q );
		
		if ( q < end )
		{
			needsQuoting = true;
			result += sEscapedQuote;
			++q;
		}
		
		p = q;
	}
	
	if ( !needsQuoting )
	{
		return str;
	}
	
	result += "'";
	
	return result;
}


static string MakeCommand( const vector<const char*>& args, bool needToEscape )
{
	string command;
	
	if ( args.size() > 0 )
	{
		if ( needToEscape )
		{
			command = std::accumulate
			(
				args.begin() + 1, 
				args.end(), 
				QuoteForMPW( args[ 0 ] ), 
				MakeConcat( std::ptr_fun( QuoteForMPW ) )
			);
		}
		else
		{
			command = std::accumulate
			(
				args.begin() + 1, 
				args.end(), 
				string( args[ 0 ] ), 
				Concat< ext::identity< const char* > >()
			);
		}
	}
	return command;
}

enum
{
	optEscapeForMPW, 
	optSwitchLayers
};

static O::Options DefineOptions()
{
	O::Options options;
	
	options.DefineSetFlag( "--escape", optEscapeForMPW );
	options.DefineSetFlag( "--switch", optSwitchLayers );
	
	return options;
}

static void MyOSStatusLogger( N::OSStatus error, const char *file, int line )
{
	static int level = 0;
	
	if ( error == -43 )  return;
	
	++level;
	
	ASSERT( level < 5 );
	
	try
	{
		Io::Err << "# LOG: OSStatus " << error << ".  \n"
		           "File '" << file << "'; Line " << line << "\n";
	}
	catch ( ... )
	{
		::SysBeep( 30 );
	}
	
	--level;
}

int O::Main( int argc, const char *const argv[] )
{
	//N::SetOSStatusLoggingProc( MyOSStatusLogger );
	
	O::Options options = DefineOptions();
	options.GetOptions( argc, argv );
	
	const vector< const char* >& params = options.GetFreeParams();
	
	string command = MakeCommand( params, options.GetFlag( optEscapeForMPW ) );
	
	bool gSwitchLayers = options.GetFlag( optSwitchLayers );
	
	// This is a bit of a hack.  It really ought to happen just after we send the event,
	// but gSwitchLayers is local to this file and I'm not dealing with that now.
	if ( gSwitchLayers && N::SameProcess( N::CurrentProcess(),
	                                      N::GetFrontProcess() ) )
	{
		N::SetFrontProcess( NX::LaunchApplication( sigToolServer ) );
	}
	
	int result = RunCommandInToolServer( command );
	
	if ( gSwitchLayers && N::SameProcess( NX::LaunchApplication( sigToolServer ),
	                                      N::GetFrontProcess() ) )
	{
		N::SetFrontProcess( N::CurrentProcess() );
	}
	
	return result;
}

