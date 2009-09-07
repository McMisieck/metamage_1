/*	================
 *	ProjectCommon.cc
 *	================
 */

#include "A-line/ProjectCommon.hh"

// C++
#include <algorithm>
#include <map>
#include <vector>

// A-line
#include "A-line/Exceptions.hh"
#include "A-line/Includes.hh"
#include "A-line/Project.hh"
#include "CompileDriver/ProjectCatalog.hh"


namespace tool
{
	
	// A map from project name to project data
	typedef std::map< std::string, boost::shared_ptr< Project > > ProjectMap;
	
	// A map from platform to project map
	typedef std::map< Platform, ProjectMap > ProjectPlatformMap;
	
	
	static ProjectPlatformMap gProjectPlatformMap;
	
	
	Project& GetProject( const std::string& project_name, Platform platform )
	{
		// Cache the platform lookup
		ProjectMap& map = gProjectPlatformMap[ platform ];
		
		// Check for the project
		ProjectMap::iterator it = map.find( project_name );
		
		const bool exists = it != map.end();
		
		// Take a reference (auto-vivifying)
		boost::shared_ptr< Project >& project_ptr = exists ? it->second
		                                                   : map[ project_name ];
		
		if ( !exists )
		{
			// First encounter with this project; load it
			
			const ProjectConfig& config = GetProjectConfig( project_name, platform );
			
			project_ptr.reset( new Project( project_name,
			                                platform,
			                                config.get_project_dir(),
			                                config.get_config_data() ) );
		}
		else if ( project_ptr.get() == NULL )
		{
			// Project entry exists but is NULL -- probably trying to load itself
			
			throw circular_dependency( project_name );
		}
		
		return *project_ptr;
	}
	
}

