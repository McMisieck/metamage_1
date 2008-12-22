/*	===================
 *	Vertice/Document.hh
 *	===================
 */

#ifndef VERTICE_DOCUMENT_HH
#define VERTICE_DOCUMENT_HH

// Pedestal
#include "Pedestal/Window.hh"

// Vertice
#include "Vertice/PortView.hh"


namespace Vertice
{
	
	class Window : public Pedestal::Window
	{
		public:
			Window( ConstStr255Param title );
			
			Scene& ItsScene()  { return SubView< PortView >().ItsScene(); }
			
			void Load( const FSSpec& file );
			void Store();
	};
	
}

#endif

