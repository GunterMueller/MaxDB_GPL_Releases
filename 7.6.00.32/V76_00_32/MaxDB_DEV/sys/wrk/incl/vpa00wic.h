#ifndef DEBUG
#line 67 "vpa00wic"
#endif
#ifndef APWIN                     /* don't reinclude    */
#define APWIN
#ifndef DEBUG
#line 88 "vpa00wic"
#endif

#ifdef WININC
#  include <windows.h>              /* normal windows.h    */
#  ifdef WIN32
#    include <windowsx.h>         /* windows extensions  */
#    define GetCurrentTask	(HTASK)GetCurrentProcess
#    define GetDOSEnvironment	GetEnvironmentStrings
#    define PCALL
#  else
#    define PCALL PASCAL
#  endif
#else                             /* not windows         */
/******* Common definitions and typedefs **********************/

#ifndef VOID
#  define VOID                void
#endif

#ifdef WIN
#  define LONG long
#endif

#ifndef _DWORD
#  define _DWORD
/*   typedef unsigned long       DWORD;  PTS 1119281 WORDs have 2 bytes */
   typedef unsigned int       DWORD;
#endif

#ifndef _UINT
#  define _UINT
   typedef unsigned int        UINT;
#endif

#ifdef PC
#  define FAR                 _far
#  define NEAR                _near
#  define PASCAL              _pascal
#  define CDECL               _cdecl
#ifndef WINAPI
#  define WINAPI              _far _pascal
#endif
#  define CALLBACK            _far _pascal
#  define EXPORT	      _export
#else
#  define FAR
#  define NEAR
#  define PASCAL
#  define CDECL
#ifndef WINAPI
#  define WINAPI
#endif
#  define CALLBACK
#  define EXPORT
#endif
#define PCALL PASCAL

/****** Simple types & common helper macros ********************/

typedef int                 BOOL;
#ifndef FALSE
#define FALSE               0
#endif
#ifndef TRUE
#define TRUE                1
#endif

typedef unsigned char       BYTE;
typedef unsigned short      WORD;

#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)(((UINT)(w) >> 8) & 0xFF))

#define LOWORD(l)           ((WORD)(DWORD)(l))
#define HIWORD(l)           ((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) |\
			    (((DWORD)((WORD)(high))) << 16)))

#define max(a,b)           (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

/****** Common pointer types **********************************/

#ifndef NULL
#define NULL                0
#endif

/****** Common handle types *************************************/

#ifndef __HANDLE_T__
#define __HANDLE_T__ __HANDLE_T__
typedef void *               HANDLE;
#endif
typedef UINT                    HWND;
#include <wchar.h>
#ifndef _WCHAR
#define _WCHAR
typedef wchar_t WCHAR;
#endif
typedef const WCHAR FAR *   LPCWSTR;

#endif /* ifdef not WIN */

#endif /* ifdef APWIN */

/* end of apwin.h */
