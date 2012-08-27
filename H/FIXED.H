#ifndef FIXED_H
#define FIXED_H

#include "..\h\gkit.h"

typedef unsigned long fixed;

fixed ConvertToFix( const double a );
fixed ConvertToFix( const float a );
fixed ConvertToFix( const long a );
fixed ConvertToFix( const short int a );

float ConvertToFloat( const fixed a );
short ConvertToInt( const fixed a );
extern fixed FixMulFix( fixed a, fixed b );
extern fixed FixDivFix( fixed a, fixed b );

#  pragma aux FixDivFix parm [eax] [ebx] modify [edx] value [eax] = \
        "cdq" \
        "shld edx, eax, 16" \
        "sal eax,16" \
        "idiv ebx";

#  pragma aux FixMulFix parm [eax] [ebx] modify [edx] value [eax] = \
        "imul ebx" \
        "shrd eax,edx,16";


#endif
