#ifndef DEBUG
#line 20 "vpa09mc"
#endif
#include <windows.h>
#include <stdio.h>
#include <process.h>

#define PA09_CLASSNAME "LiveCache INTERFACE TRACE"

typedef struct {
  DWORD cbSize;
  UCHAR szComponent[MAX_PATH];
  DWORD cbVersion[4];
} pa09Component;

LPVOID pa09CreateSharedData(LPSTR szMapName, DWORD cbSize);

BOOL pa09RegisterTF(pa09Component *pComponent, HANDLE hModule);
BOOL pa09UnregisterTF(HANDLE hModule);

