//fixed.cpp
//fixed point routines
//in 16.16 format
#include "..\h\gkit.h"
#include "..\h\fixed.h"

#define FloatScaler 65536.0
#define FixShift    16

fixed ConvertToFix( const double a )
{
   return ( fixed ) ( a * FloatScaler );
}

fixed ConvertToFix( const float a )
{
   return ( fixed ) ( a * FloatScaler );
}

fixed ConvertToFix( const long a )
{
   return ( fixed ) ( a << FixShift );
}

fixed ConvertToFix( const short int a )
{
   return ( fixed ) ( ( ( long ) a ) << FixShift );
}

float ConvertToFloat( const fixed a )
{
   return ( ( a ) / FloatScaler );
}

short ConvertToInt( const fixed a )
{
   return ( short ) ( ( a ) >> FixShift );
}


