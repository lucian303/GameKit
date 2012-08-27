#ifndef GKIT_H
#define GKIT_H

typedef unsigned char       UCHAR;
typedef signed char         INT8;
typedef unsigned char       UINT8;
typedef signed short int    INT16;
typedef unsigned short int  UINT16;
typedef signed long int     INT32;
typedef unsigned long int   UINT32;

typedef unsigned short int  WORD;
typedef unsigned long int   LONG;

typedef unsigned char       BOOL;

#define TRUE (1)
#define FALSE (0)

#define DIM(x) (sizeof(x)/sizeof(x[0]))

#define LOWBYTE(x) ((x) & 0xFF)
#define HIGHBYTE(x)  (((x) >> 8) & 0xFF)

/* just define these in terms of the Borland library macros */
//#define MIN(a,b) min(a,b)
//#define MAX(a,b) max(a,b)

/* interrupt acknowledgement stuff */
#define PIC             (0x20)
#define NONSPECIFIC_EOI    (0x20)

/* typedef struct {
   int Top;
    int Left;
    int Bottom;
    int Right;
} RECT;
*/

#endif
