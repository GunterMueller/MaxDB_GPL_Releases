
#ifndef MSSim_WIN_IAT_HOOK_HPP
#define MSSim_WIN_IAT_HOOK_HPP

#include <windows.h>

void MSSim_WinIatHook_PatchModule( HMODULE moduleAddress ); 
void MSSim_WinIatHook_PatchAllModules(); 

#endif // #ifndef MSSim_WIN_IAT_HOOK_HPP
