/*
	decimal.cc
	----------
*/

#include "plus/decimal.hh"

// plus
#include "plus/nth_power_of_ten.hh"


namespace plus
{
	
	static inline
	bool abs_lte( const integer& a, const integer& b )
	{
		return abs_compare( a, b ) <= 0;
	}
	
	static inline
	bool abs_gte( const integer& a, const integer& b )
	{
		return abs_compare( a, b ) >= 0;
	}
	
	
	integer decode_decimal( const char* p, unsigned n )
	{
		integer result;
		
		while ( n-- )
		{
			if ( char c = *p++ - '0' )
			{
				integer digit = c;
			
				result += nth_power_of_ten( n ) * digit;
			}
		}
		
		return result;
	}
	
	static string::size_type count_decimal_digits( const integer& x )
	{
		string::size_type power = 0;
		
		while ( abs_lte( nth_power_of_ten( power ), x ) )
		{
			++power;
		}
		
		return power;
	}
	
	
	string encode_decimal( const integer& x )
	{
		if ( x.is_zero() )
		{
			return "0";
		}
		
		integer remains = x;
		
		const bool negative = x.is_negative();
		
		if ( negative )
		{
			remains.invert();
		}
		
		string::size_type n = count_decimal_digits( remains );
		
		plus::string result;
		
		char* r = result.reset( negative + n );
		
		if ( negative )
		{
			*r++ = '-';
		}
		
		while ( n-- > 0 )
		{
			const integer& x10 = nth_power_of_ten( n );
			
			int i = 0;
			
			while ( abs_gte( remains, x10 ) )
			{
				remains -= x10;
				++i;
			}
			
			*r++ = '0' + i;
		}
		
		return result;
	}
	
}