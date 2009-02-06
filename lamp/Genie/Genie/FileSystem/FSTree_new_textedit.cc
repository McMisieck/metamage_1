/*	======================
 *	FSTree_new_textedit.cc
 *	======================
 */

#include "Genie/FileSystem/FSTree_new_textedit.hh"

// Genie
#include "Genie/FileSystem/FSTree_Directory.hh"
#include "Genie/FileSystem/FSTree_Property.hh"
#include "Genie/FileSystem/FSTree_sys_window_REF.hh"
#include "Genie/FileSystem/TextEdit.hh"
#include "Genie/FileSystem/TextEdit_text.hh"
#include "Genie/IO/VirtualFile.hh"


namespace Genie
{
	
	namespace Ped = Pedestal;
	
	
	boost::shared_ptr< Ped::View > TextEditFactory( const FSTree* delegate )
	{
		return boost::shared_ptr< Ped::View >( new TextEdit_Scroller( delegate ) );
	}
	
	
	void FSTree_new_textedit::DestroyDelegate( const FSTree* delegate )
	{
		RemoveScrollerParams( delegate );
		
		TextEditParameters::Erase( delegate );
	}
	
	
	class FSTree_TextEdit_interlock : public FSTree
	{
		public:
			FSTree_TextEdit_interlock( const FSTreePtr&    parent,
			                           const std::string&  name ) : FSTree( parent, name )
			{
			}
			
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void SetTimes() const;
	};
	
	void FSTree_TextEdit_interlock::SetTimes() const
	{
		const FSTree* view = ParentRef().get();
		
		TextEditParameters::Get( view ).itIsInterlocked = true;
	}
	
	
	namespace
	{
		
		bool& Wrapped( const FSTree* view )
		{
			return TextEditParameters::Get( view ).itIsWrapped;
		}
		
		int& Width( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientWidth;
		}
		
		int& Height( const FSTree* view )
		{
			return GetScrollerParams( view ).itsClientHeight;
		}
		
		int& HOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsHOffset;
		}
		
		int& VOffset( const FSTree* view )
		{
			return GetScrollerParams( view ).itsVOffset;
		}
		
	}
	
	template < class Scribe, typename Scribe::Value& (*Access)( const FSTree* ) >
	struct TE_View_Property : public View_Property< Scribe, Access >
	{
		static void Set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			const FSTree* view = GetViewKey( that );
			
			TextEditParameters::Get( view ).itHasChangedAttributes = true;
			
			View_Property::Set( that, begin, end, binary );
		}
	};
	
	template < class Property >
	static FSTreePtr Property_Factory( const FSTreePtr&    parent,
	                                   const std::string&  name )
	{
		return FSTreePtr( new FSTree_Property( parent,
		                                       name,
		                                       &Property::Get,
		                                       &Property::Set ) );
	}
	
	const FSTree_Premapped::Mapping TextEdit_view_Mappings[] =
	{
		{ "text", &Basic_Factory< FSTree_TextEdit_text > },
		
		{ "interlock", &Basic_Factory< FSTree_TextEdit_interlock > },
		
		{ "selection", &Property_Factory< Selection_Property > },
		
		//{ "wrapped", &Property_Factory< View_Property< Boolean_Scribe, Wrapped > > },
		
		// unlocked-text
		
		{ "width",  &Property_Factory< View_Property< Integer_Scribe< int >, Width  > > },
		{ "height", &Property_Factory< View_Property< Integer_Scribe< int >, Height > > },
		
		{ "x", &Property_Factory< TE_View_Property< Integer_Scribe< int >, HOffset > > },
		{ "y", &Property_Factory< TE_View_Property< Integer_Scribe< int >, VOffset > > },
		
		{ NULL, NULL }
	};
	
}

