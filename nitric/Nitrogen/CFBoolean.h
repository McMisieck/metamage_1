// CFBoolean.h

#ifndef NITROGEN_CFBOOLEAN_H
#define NITROGEN_CFBOOLEAN_H

#ifndef __CFNUMBER__
#include <CFNumber.h>
#endif
#ifndef NITROGEN_CFBASE_H
#include "Nitrogen/CFBase.h"
#endif
#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

namespace Nitrogen
  {
   using ::CFBooleanRef;
   
   template <> struct CFType_Traits< CFBooleanRef >: Basic_CFType_Traits< CFBooleanRef, ::CFBooleanGetTypeID > {};
   template <> struct OwnedDefaults< CFBooleanRef >: OwnedDefaults<CFTypeRef>  {};
   
   inline bool CFBooleanGetValue( CFBooleanRef b )       { return ::CFBooleanGetValue( b ); }

   template <>
   struct Converter< bool, CFBooleanRef >: public std::unary_function< CFBooleanRef, bool >
     {
      bool operator()( const CFBooleanRef& in ) const
        {
         return Nitrogen::CFBooleanGetValue( in );
        }
     };

   template <>
   struct Converter< CFBooleanRef, bool >: public std::unary_function< bool, CFBooleanRef >
     {
      CFBooleanRef operator()( const bool& in ) const
        {
         return in ? kCFBooleanTrue : kCFBooleanFalse;
        }
     };
   
   template <>
   struct Converter< Owned<CFBooleanRef>, bool >: public std::unary_function< bool, Owned<CFBooleanRef> >
     {
      Owned<CFBooleanRef> operator()( const bool& in ) const
        {
         return CFRetain( Convert<CFBooleanRef>( in ) );
        }
     };
  }

#endif
