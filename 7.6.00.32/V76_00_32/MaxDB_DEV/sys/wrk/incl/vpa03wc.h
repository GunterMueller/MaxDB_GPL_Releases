#ifndef DEBUG
#line 20 "vpa03wc"
#endif

#include "vpa00global.h"

#include <windows.h>
#include <direct.h>

#ifndef DYNAMICLOAD
/* #define DYNAMICLOAD */
BOOL pa03LoadRte();
VOID pa03FreeRte(VOID);
#endif

#ifndef CTL3D_ALL
#define CTL3D_ALL 0xffff
VOID pa03InstallCtl3d(HANDLE hModule);
VOID pa03RemoveCtl3d(HANDLE hinst);
VOID pa03Set3dDialog (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
#endif
