/*	===========
 *	Scroller.hh
 *	===========
 */

#ifndef PEDESTAL_SCROLLER_HH
#define PEDESTAL_SCROLLER_HH

// Mac OS
#include <ControlDefinitions.h>

// Nucleus
#include "Nucleus/NAssert.h"
#include "Nucleus/Saved.h"

// Nitrogen / Carbon support
#include "Nitrogen/Controls.h"

// ClassicToolbox
#include "ClassicToolbox/MacWindows.h"

// Nitrogen Extras / Utilities
#include "Utilities/RectangleMath.h"

// Pedestal
#include "Pedestal/View.hh"
#include "Pedestal/Scrollbar.hh"
#include "Pedestal/ScrollingView.hh"


namespace Pedestal
{
	
	short ActualScrollbarLength( short viewLength, bool shortened );
	
	Rect VerticalScrollbarBounds  ( short width, short height, bool shortened );
	Rect HorizontalScrollbarBounds( short width, short height, bool shortened );
	
	Point ScrollDimensions( short width, short height, bool vertical, bool horizontal );
	
	inline Rect ScrollBounds( short width, short height, bool vertical, bool horizontal )
	{
		Point dimensions = ScrollDimensions( width, height, vertical, horizontal );
		
		return Nitrogen::SetRect( 0, 0, dimensions.h, dimensions.v );
	}
	
	Point ScrollbarMaxima( Point scrollableRange, Point viewableRange, Point scrollPosition );
	
	short FigureScrollDistance( Nitrogen::ControlPartCode part, short pageDistance );
	
	short SetControlValueFromClippedDelta( ControlRef control, short delta );
	
	
	class BoundedView : public View
	{
		private:
			Rect bounds;
		
		public:
			BoundedView(const Rect& bounds) : bounds( bounds )  {}
			
			const Rect& Bounds() const  { return bounds; }
			
			void Resize( short width, short height )
			{
				bounds.right = bounds.left + width;
				bounds.bottom = bounds.top + height;
			}
	};
	
	template < bool present >  struct ScrollbarPresence_Traits;
	
	// present:  Is the scrollbar present?
	// Type:     The storage type of the scrollbar object.
	// profile:  The amount the scrollbar encroaches on the scroll view.
	
	template <>  struct ScrollbarPresence_Traits< true >
	{
		static const bool present = true;
		typedef Scrollbar Type;
		static const short profile = 15;
	};
	
	template <>  struct ScrollbarPresence_Traits< false >
	{
		static const bool present = false;
		
		struct NoProcID {};
		
		static NoProcID procID;
		
		struct Type
		{
			Type( const Rect&, NoProcID, Nitrogen::RefCon, ControlTracker )
			{
			}
			
			Type& Get()  { return *this; }
			
			void Activate( bool )  {}
		};
		
		static const short profile = 0;
	};
	
	enum ScrollbarConfig
	{
		kNoScrollbar            = 0,
		kOldSchoolVariant       = 1,
		
	#if TARGET_CPU_68K
		
		// Cheap hack until we figure out something better
		
		kAppearanceSavvyVariant = kOldSchoolVariant,
		kLiveFeedbackVariant    = kOldSchoolVariant
		
	#else
		
		kAppearanceSavvyVariant = 2,
		kLiveFeedbackVariant    = 3
		
	#endif
	};
	
	enum ScrollbarAxis
	{
		kVertical,
		kHorizontal
	};
	
	template < ScrollbarConfig config > struct ScrollbarLiveScrolling_Traits
	{
		static const bool scrollingIsLive = false;
	};
	
	template < ScrollbarConfig config > struct ScrollbarVariant_Traits {};
	
	template <> struct ScrollbarVariant_Traits< kOldSchoolVariant >
	{
		static const Nitrogen::ControlProcID procID = Nitrogen::scrollBarProc;
	};
	
#if !TARGET_CPU_68K
	
	template <> struct ScrollbarVariant_Traits< kAppearanceSavvyVariant >
	{
		static const Nitrogen::ControlProcID procID = Nitrogen::kControlScrollBarProc;
	};
	
	template <> struct ScrollbarVariant_Traits< kLiveFeedbackVariant >
	{
		static const Nitrogen::ControlProcID procID = Nitrogen::kControlScrollBarLiveProc;
	};
	
	template <> struct ScrollbarLiveScrolling_Traits< kLiveFeedbackVariant >
	{
		static const bool scrollingIsLive = true;
	};
	
#endif
	
	template < ScrollbarConfig config >
	struct Scrollbar_Traits : public ScrollbarPresence_Traits< config != kNoScrollbar >,
	                          public ScrollbarVariant_Traits< config >,
	                          public ScrollbarLiveScrolling_Traits< config >
	{
		
	};
	
	using Nitrogen::SetControlMaximum;
	
	inline void SetControlViewSize( ControlRef control, long size )
	{
	#if TARGET_CPU_PPC
		
		if ( TARGET_API_MAC_CARBON || ::SetControlViewSize != (void*)kUnresolvedCFragSymbolAddress )
		{
			::SetControlViewSize( control, size );
		}
		
	#endif
	}
	
	inline void InvalidateControl( ControlRef control )  { Nitrogen::InvalRect( Nitrogen::GetControlBounds( control ) ); }
	
	inline void SetBounds         ( ScrollbarPresence_Traits< false >::Type, Rect  )  {}
	inline void SetControlMaximum ( ScrollbarPresence_Traits< false >::Type, short )  {}
	inline void SetValueStretch   ( ScrollbarPresence_Traits< false >::Type, short )  {}
	inline void SetControlViewSize( ScrollbarPresence_Traits< false >::Type, long  )  {}
	inline void InvalidateControl ( ScrollbarPresence_Traits< false >::Type        )  {}
	
	
	template < ScrollbarAxis axis >
	inline short VHSelect( Point point )
	{
		return ( axis == kVertical ) ? point.v : point.h;
	}
	
	template < class ScrollViewType >
	inline Point ComputeScrollbarMaxima( const ScrollViewType&  scrolledView )
	{
		Point scrollRange = ScrollableRange( scrolledView );
		Point viewRange   = ViewableRange  ( scrolledView );
		
		Point scrollPos   = ScrollPosition ( scrolledView );
		
		Point maxima = ScrollbarMaxima( scrollRange, viewRange, scrollPos );
		
		return maxima;
	}
	
	template < class Vertical, class Horizontal >
	inline void SetScrollbarMaxima( const Vertical&    verticalScrollbar,
	                                const Horizontal&  horizontalScrollbar,
	                                Point              maxima )
	{
		SetControlMaximum( verticalScrollbar,   maxima.v );
		SetControlMaximum( horizontalScrollbar, maxima.h );
	}
	
	
	template < class ScrollViewType >
	inline ScrollViewType& RecoverScrolledViewFromScrollbar( ControlRef control )
	{
		Control_Hooks* controlHooks = Nitrogen::GetControlReference( control );
		
		ASSERT( controlHooks       != NULL );
		ASSERT( controlHooks->data != NULL );
		
		ScrollViewType& scrolledView = *static_cast< ScrollViewType* >( controlHooks->data );
		
		return scrolledView;
	}
	
	
	template < ScrollbarAxis axis, class ScrollViewType >
	inline void ScrollByDelta( ScrollViewType& scrolledView, ControlRef control, short delta, bool updateNow )
	{
		if ( delta != 0 )
		{
			ScrollView( scrolledView,
			            ( axis != kVertical ) ? delta : 0,
			            ( axis == kVertical ) ? delta : 0,
			            updateNow );
			
			SetControlMaximum( control, VHSelect< axis >( ComputeScrollbarMaxima( scrolledView ) ) );
		}
	}
	
	template < ScrollbarAxis axis, class ScrollViewType >
	inline void ScrollByDelta( ControlRef control, short delta, bool updateNow )
	{
		ScrollViewType& scrolledView = RecoverScrolledViewFromScrollbar< ScrollViewType >( control );
		
		ScrollByDelta< axis >( scrolledView, control, delta, updateNow );
	}
	
	template < ScrollbarAxis axis, class ScrollViewType >
	void ScrollbarAction( ControlRef control, Nitrogen::ControlPartCode part )
	{
		ScrollViewType& scrolledView = RecoverScrolledViewFromScrollbar< ScrollViewType >( control );
		
		short jump = VHSelect< axis >( ViewableRange( scrolledView ) ) - 1;
		short scrollDistance = FigureScrollDistance( part, jump );
		
		short delta = SetControlValueFromClippedDelta( control, scrollDistance );
		
		if ( part == Nitrogen::kControlIndicatorPart )
		{
			short oldValue = VHSelect< axis >( ScrollPosition( scrolledView ) );
			
			delta = Nitrogen::GetControlValue( control ) - oldValue;
		}
		
		ScrollByDelta< axis >( scrolledView, control, delta, true );
	}
	
	template < ScrollbarAxis axis, bool scrollingIsLive, class ScrollViewType >
	inline void Track( ControlRef control, Nitrogen::ControlPartCode part, Point point )
	{
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
		Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
		
		switch ( part )
		{
			case kControlIndicatorPart:
				// The user clicked on the indicator
				
				if ( !scrollingIsLive )
				{
					// Classic scrolling, handled specially.
					
					// Get the current scrollbar value.
					short oldValue = Nitrogen::GetControlValue( control );
					
					// Let the system track the drag...
					part = Nitrogen::TrackControl( control, point );
					
					if ( part == Nitrogen::kControlIndicatorPart )
					{
						// Drag was successful (i.e. within bounds).  Subtract to get distance.
						short scrollDistance = Nitrogen::GetControlValue( control ) - oldValue;
						
						// Scroll by that amount, but don't update just yet.
						ScrollByDelta< axis, ScrollViewType >( control, scrollDistance, false );
					}
					
					// Break here for classic thumb-scrolling (whether sucessful or not).
					break;
				}
				// else fall through for live feedback scrolling
			case kControlUpButtonPart:
			case kControlDownButtonPart:
			case kControlPageUpPart:
			case kControlPageDownPart:
				part = Nitrogen::TrackControl<
				                        #ifdef __MWERKS__
				                        (Nitrogen::ControlActionProcPtr)
				                        #endif
				                        ScrollbarAction< axis, ScrollViewType >  >( control, point );
				break;
			
			default:
				break;
		}
	}
	
	
	inline bool operator==( Point a, Point b )
	{
		return a.h == b.h  &&  a.v == b.v;
	}
	
	inline bool operator!=( Point a, Point b )
	{
		return !( a == b );
	}
	
	template < class Vertical, class Horizontal >
	inline void UpdateScrollbars( const Vertical&        verticalScrollbar,
	                              const Horizontal&      horizontalScrollbar,
	                              Point                  maxima,
	                              Point                  position )
	{
		Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
		
		Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
		
		SetValueStretch( horizontalScrollbar, position.h );
		SetValueStretch( verticalScrollbar,   position.v );
		
		// need to update scrollbar maxima
		SetScrollbarMaxima( verticalScrollbar,
		                    horizontalScrollbar,
		                    maxima );
	}
	
	template < class ScrollViewType, class Vertical, class Horizontal >
	inline void UpdateScrollbars( const ScrollViewType&  scrolledView,
	                              const Vertical&        verticalScrollbar,
	                              const Horizontal&      horizontalScrollbar,
	                              Point                  oldRange,
	                              Point                  oldPosition )
	{
		using namespace Nucleus::Operators;
		
		Point range = ScrollableRange( scrolledView );
		Point pos   = ScrollPosition ( scrolledView );
		
		if ( oldPosition != pos  ||  oldRange != range )
		{
			UpdateScrollbars( verticalScrollbar,
			                  horizontalScrollbar,
			                  ComputeScrollbarMaxima( scrolledView ),
			                  pos );
		}
	}
	
	class ClickableScroller
	{
		protected:
			static ClickableScroller* gCurrentScroller;
		
		public:
			//static ClickableScroller& Current();
			
			static void ClickLoop()  { if ( gCurrentScroller )  gCurrentScroller->ClickInLoop(); }
			
			virtual void ClickInLoop() = 0;
	};
	
	template < class            ScrollViewType,
	           ScrollbarConfig  vertical,
	           ScrollbarConfig  horizontal = kNoScrollbar >
	class Scroller : public BoundedView, public ClickableScroller
	{
		private:
			typedef Scrollbar_Traits< vertical   > VerticalTraits;
			typedef Scrollbar_Traits< horizontal > HorizontalTraits;
			
			typedef typename VerticalTraits  ::Type VerticalScrollbarType;
			typedef typename HorizontalTraits::Type HorizontalScrollbarType;
			
			VerticalScrollbarType    myScrollV;
			HorizontalScrollbarType  myScrollH;
			
			ScrollViewType myScrollView;
		
		public:
			typedef typename ScrollViewType::Initializer Initializer;
			
			Scroller( const Rect& bounds, Initializer init = Initializer() );
			
			static bool ScrollsVertically()    { return VerticalTraits  ::present; }
			static bool ScrollsHorizontally()  { return HorizontalTraits::present; }
			
			ScrollViewType const&          ScrolledView() const        { return myScrollView; }
			ScrollViewType&                ScrolledView()              { return myScrollView; }
			VerticalScrollbarType&   VerticalScrollbar()    { return myScrollV; }
			HorizontalScrollbarType& HorizontalScrollbar()  { return myScrollH; }
			
			void SetControlViewSizes();
			
			void Calibrate()
			{
				SetScrollbarMaxima( myScrollV.Get(),
				                    myScrollH.Get(),
				                    ComputeScrollbarMaxima( myScrollView ) );
			}
			
			void Scroll(short dh, short dv, bool updateNow = 0)
			{
				ScrollView( myScrollView, dh, dv, updateNow );
				Calibrate();
			}
			
			void UpdateScrollbars( Point oldRange, Point oldPosition )
			{
				Pedestal::UpdateScrollbars( myScrollView, myScrollV.Get(), myScrollH.Get(), oldRange, oldPosition );
			}
			
			void ClickInLoop()
			{
				Pedestal::UpdateScrollbars( myScrollV.Get(),
				                            myScrollH.Get(),
				                            ComputeScrollbarMaxima( ScrolledView() ),
				                            ScrollPosition        ( ScrolledView() ) );
				
				return;
			}
			
			void Idle( const EventRecord& event )
			{
				// Intersect the clip region with the scrollview bounds,
				// so the scrollview doesn't overpaint the scroll bars.
				Nucleus::Saved< Nitrogen::Clip_Value > savedClip( Nitrogen::SectRgn( Nitrogen::GetClip(),
				                                                                     Nitrogen::RectRgn( ScrolledView().Bounds() ) ) );
				
				ScrolledView().Idle( event );
			}
			
			void MouseDown( const EventRecord& event )
			{
				if ( Nitrogen::PtInRect( Nitrogen::GlobalToLocal( event.where ), ScrolledView().Bounds() ) )
				{
					gCurrentScroller = this;
					
					ScrolledView().MouseDown( event );
					
					gCurrentScroller = NULL;
				}
			}
			
			bool KeyDown( const EventRecord& event )
			{
				Point scrollableRange = ScrollableRange( ScrolledView() );
				Point scrollPosition  = ScrollPosition ( ScrolledView() );
				
				char keyCode = (event.message & keyCodeMask) >> 8;
				
				// Only consider programmers' keys, not control characters
				if ( keyCode >= 0x70 )
				{
					char keyChar = event.message & charCodeMask;
					
					switch ( keyChar )
					{
						case kHomeCharCode:
							Scroll( 0, -scrollPosition.v );
							UpdateScrollbars( scrollableRange, scrollPosition );
							return true;
						
						case kEndCharCode:
							Scroll( 0, scrollableRange.v - scrollPosition.v );
							UpdateScrollbars( scrollableRange, scrollPosition );
							return true;
						
						case kPageUpCharCode:
							ScrollbarAction< kVertical, ScrollViewType >( myScrollV.Get(), Nitrogen::kControlPageUpPart );
							return true;
						
						case kPageDownCharCode:
							ScrollbarAction< kVertical, ScrollViewType >( myScrollV.Get(), Nitrogen::kControlPageDownPart );
							return true;
						
						default:
							break;
					}
				}
				
				if ( ScrolledView().KeyDown( event ) )
				{
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				}
				
				return false;
			}
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event )
			{
				return ScrolledView().EnterShiftSpaceQuasimode( event );
			}
			
			void Update()
			{
				// Intersect the clip region with the scrollview bounds,
				// so the scrollview doesn't overpaint the scroll bars.
				Nucleus::Saved< Nitrogen::Clip_Value > savedClip( Nitrogen::SectRgn( Nitrogen::GetClip(),
				                                                                     Nitrogen::RectRgn( ScrolledView().Bounds() ) ) );
				
				ScrolledView().Update();
			}
			
			void Activate( bool activating )
			{
				ScrolledView().Activate( activating );
				
				Nucleus::Saved< Nitrogen::Clip_Value > savedClip;
				
				Nitrogen::ClipRect( Nitrogen::GetPortBounds( Nitrogen::GetQDGlobalsThePort() ) );
				
				
				VerticalScrollbar  ().Activate( activating );
				HorizontalScrollbar().Activate( activating );
			}
			
			bool SetCursor( Point location, RgnHandle mouseRgn )
			{
				if ( Nitrogen::PtInRect( location, ScrolledView().Bounds() ) )
				{
					return ScrolledView().SetCursor( location, mouseRgn );
				}
				
				return false;
			}
			
			bool UserCommand( MenuItemCode code )
			{
				Point scrollableRange = ScrollableRange( ScrolledView() );
				Point scrollPosition  = ScrollPosition ( ScrolledView() );
				
				if ( ScrolledView().UserCommand( code ) )
				{
					UpdateScrollbars( scrollableRange, scrollPosition );
					return true;
				}
				
				return false;
			}
			
			void Resize( short width, short height )
			{
				BoundedView::Resize( width, height );
				
				Point dimensions = ScrollDimensions( width,
				                                     height,
				                                     VerticalTraits  ::profile,
				                                     HorizontalTraits::profile );
				
				// Invalidate old scrollbar regions
				InvalidateControl( VerticalScrollbar  ().Get() );
				InvalidateControl( HorizontalScrollbar().Get() );
				
				SetBounds( VerticalScrollbar  ().Get(),   VerticalScrollbarBounds( width, height, true ) );
				SetBounds( HorizontalScrollbar().Get(), HorizontalScrollbarBounds( width, height, true ) );
				
				// Invalidate new scrollbar regions
				InvalidateControl( VerticalScrollbar  ().Get() );
				InvalidateControl( HorizontalScrollbar().Get() );
				
				ScrolledView().Resize( dimensions.h, dimensions.v );
				
				Calibrate();
				
				SetControlViewSizes();
			}
	};
	
	
	template < class ScrollViewType, ScrollbarConfig vertical, ScrollbarConfig horizontal >
	inline Rect Bounds( const Scroller< ScrollViewType, vertical, horizontal >& scroller )
	{
		return scroller.Bounds();
	}
	
	template < class ScrollViewType, ScrollbarConfig vertical, ScrollbarConfig horizontal >
	Scroller< ScrollViewType, vertical, horizontal >::Scroller( const Rect& bounds, Initializer init )
	: 
		BoundedView( bounds ),
		myScrollV( VerticalScrollbarBounds  ( NitrogenExtras::RectWidth ( bounds ),
		                                      NitrogenExtras::RectHeight( bounds ),
		                                      true ),
		           VerticalTraits::procID,
		           &myScrollView,
		           Track< kVertical,
		                  Scrollbar_Traits< vertical >::scrollingIsLive,
		                  ScrollViewType > ),
		myScrollH( HorizontalScrollbarBounds( NitrogenExtras::RectWidth ( bounds ),
		                                      NitrogenExtras::RectHeight( bounds ),
		                                      true ),
		           HorizontalTraits::procID,
		           &myScrollView,
		           Track< kHorizontal,
		                  Scrollbar_Traits< horizontal >::scrollingIsLive,
		                  ScrollViewType > ),
		myScrollView( ScrollBounds( NitrogenExtras::RectWidth ( bounds ),
		                            NitrogenExtras::RectHeight( bounds ),
		                            VerticalTraits::profile,
		                            HorizontalTraits::profile ),
		              init )
	{
		SetControlViewSizes();
	}
	
	template < class ScrollViewType, ScrollbarConfig vertical, ScrollbarConfig horizontal >
	void Scroller< ScrollViewType, vertical, horizontal >::SetControlViewSizes()
	{
		Point range = ViewableRange( myScrollView );
		
		Pedestal::SetControlViewSize( VerticalScrollbar  ().Get(), range.v );
		Pedestal::SetControlViewSize( HorizontalScrollbar().Get(), range.h );
	}
	
}

#endif

