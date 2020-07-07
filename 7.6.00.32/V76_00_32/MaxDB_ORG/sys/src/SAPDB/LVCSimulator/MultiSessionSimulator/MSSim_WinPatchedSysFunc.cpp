
#include "LVCSimulator/MultiSessionSimulator/MSSim_WinIATHook.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_PatchedSysFunc.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Trace.hpp"

#include <windows.h>

HMODULE GetKernel32DLL() 
{
  static HMODULE kern32DLL = GetModuleHandleA( "kernel32.dll" ); 
  return kern32DLL; 
}

typedef DWORD (*SleepExFct)( DWORD dwMilliseconds, BOOL bAlertable ); 
DWORD WINAPI MSSim_SleepEx( DWORD dwMilliseconds, BOOL bAlertable )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_SleepEx" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskSleep( dwMilliseconds ); 
  return 0; 
}

typedef VOID (*SleepFct)( IN DWORD dwMilliseconds ); 
VOID WINAPI MSSim_Sleep( IN DWORD dwMilliseconds ) 
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_Sleep" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskSleep( dwMilliseconds ); 
}

typedef HMODULE (*LoadLibraryAFct)( LPCSTR lpLibFileName ); 
HMODULE WINAPI MSSim_LoadLibraryA( LPCSTR lpLibFileName )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "LoadLibraryA" ); 
  HMODULE mod = ((LoadLibraryAFct)func)( lpLibFileName ); 
  MSSim_WinIatHook_PatchAllModules();
  return GetModuleHandleA( lpLibFileName ); 
}

typedef HMODULE (*LoadLibraryWFct)( LPWSTR lpLibFileName ); 
HMODULE WINAPI MSSim_LoadLibraryW( LPWSTR lpLibFileName )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "LoadLibraryW" ); 
  HMODULE mod = ((LoadLibraryWFct)func)( lpLibFileName ); 
  MSSim_WinIatHook_PatchAllModules();
  return GetModuleHandleW( lpLibFileName ); 
}

typedef HMODULE (*LoadLibraryExAFct)( LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags ); 
HMODULE WINAPI MSSim_LoadLibraryExA( LPCSTR lpLibFileName, HANDLE hFile, DWORD dwFlags )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "LoadLibraryExA" ); 
  HMODULE mod = ((LoadLibraryExAFct)func)( lpLibFileName, hFile, dwFlags ); 
  MSSim_WinIatHook_PatchAllModules();
  return GetModuleHandleA( lpLibFileName ); 
}

typedef HMODULE (*LoadLibraryExWFct)( LPWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags ); 
HMODULE WINAPI MSSim_LoadLibraryExW( LPWSTR lpLibFileName, HANDLE hFile, DWORD dwFlags )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "LoadLibraryExW" ); 
  HMODULE mod = ((LoadLibraryExWFct)func)( lpLibFileName, hFile, dwFlags ); 
  MSSim_WinIatHook_PatchAllModules();
  return GetModuleHandleW( lpLibFileName ); 
}

typedef DWORD (*WaitForSingleObjectFct)( HANDLE hHandle, DWORD dwMilliseconds );
DWORD WINAPI MSSim_WaitForSingleObject( HANDLE hHandle, DWORD dwMilliseconds )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_WaitForSingleObject" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskWaitFor( hHandle ); 
  return 0; 
}

typedef HANDLE (*CreateSemaphoreAFct) (
                                      IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                                      IN LONG lInitialCount,
                                      IN LONG lMaximumCount,
                                      IN LPCSTR lpName
                                      );
HANDLE WINAPI MSSim_CreateSemaphoreA (
                        IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                        IN LONG lInitialCount,
                        IN LONG lMaximumCount,
                        IN LPCSTR lpName
                        )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->semCreate( lInitialCount, lMaximumCount ); 
}

typedef HANDLE (*CreateSemaphoreWFct) (
                                      IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                                      IN LONG lInitialCount,
                                      IN LONG lMaximumCount,
                                      IN LPWSTR lpName
                                      );
HANDLE WINAPI MSSim_CreateSemaphoreW (
                                       IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                                       IN LONG lInitialCount,
                                       IN LONG lMaximumCount,
                                       IN LPWSTR lpName
                                       )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->semCreate( lInitialCount, lMaximumCount ); 
}

typedef BOOL (*ReleaseSemaphoreFct)(
                                   IN HANDLE hSemaphore,
                                   IN LONG lReleaseCount,
                                   OUT LPLONG lpPreviousCount
                                   );
BOOL WINAPI MSSim_ReleaseSemaphore(
                        IN HANDLE hSemaphore,
                        IN LONG lReleaseCount,
                        OUT LPLONG lpPreviousCount
                        )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->semRelease( hSemaphore, lReleaseCount ); 
}

typedef HANDLE (*CreateMutexAFct)(
                           IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
                           IN BOOL bInitialOwner,
                           IN LPCSTR lpName
                           );

HANDLE WINAPI MSSim_CreateMutexA(
             IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
             IN BOOL bInitialOwner,
             IN LPCSTR lpName
             )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->mtxCreate( bInitialOwner ? true : false ); 
}


typedef HANDLE (*CreateMutexWFct)(
                           IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
                           IN BOOL bInitialOwner,
                           IN LPCWSTR lpName
                           );
HANDLE WINAPI MSSim_CreateMutexW(
             IN LPSECURITY_ATTRIBUTES lpMutexAttributes,
             IN BOOL bInitialOwner,
             IN LPCWSTR lpName
             )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->mtxCreate( bInitialOwner ? true : false ); 
}

typedef BOOL (*ReleaseMutexFct)( IN HANDLE hMutex );
BOOL WINAPI MSSim_ReleaseMutex( IN HANDLE hMutex )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->mtxRelease( hMutex); 
}


typedef HANDLE 
(*CreateThreadFct)(
                   IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
                   IN SIZE_T dwStackSize,
                   IN LPTHREAD_START_ROUTINE lpStartAddress,
                   IN LPVOID lpParameter,
                   IN DWORD dwCreationFlags,
                   OUT LPDWORD lpThreadId
                   );
HANDLE WINAPI
MSSim_CreateThread(
                       IN LPSECURITY_ATTRIBUTES lpThreadAttributes,
                       IN SIZE_T dwStackSize,
                       IN LPTHREAD_START_ROUTINE lpStartAddress,
                       IN LPVOID lpParameter,
                       IN DWORD dwCreationFlags,
                       OUT LPDWORD lpThreadId
                       )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_CreateThread" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  return inst->taskCreate( (MSSim_ThreadMainFct)lpStartAddress, lpParameter, ( dwCreationFlags & CREATE_SUSPENDED ) ? true : false ); 
}

typedef bool (*TerminateThreadFct)( IN OUT HANDLE hThread, IN DWORD dwExitCode ); 
bool WINAPI MSSim_TerminateThread ( IN OUT HANDLE hThread, IN DWORD dwExitCode )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_TerminateThread" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskKill( (MSSim_Task*)hThread ); 
  return true; 
}

typedef int (*SuspendThreadFct)( IN HANDLE hThread );
int WINAPI MSSim_SuspendThread( IN HANDLE hThread )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_SuspendThread" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskSuspend( (MSSim_Task*)hThread ); 
  return 0; 
}

typedef int (*ResumeThreadFct)( IN HANDLE hThread ); 
int WINAPI MSSim_ResumeThread( IN HANDLE hThread )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_ResumeThread" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->taskResume( (MSSim_Task*)hThread ); 
  return 0; 
}

typedef void (*ExitThreadFct)( DWORD dwExitCode ); 
void WINAPI MSSim_ExitThread( DWORD dwExitCode )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  // remove internal structures 
  inst->taskExit( dwExitCode ); 
  // forward call to win api
  static PROC func = GetProcAddress( GetKernel32DLL(), "ExitThread" ); 
  ((ExitThreadFct)func)( dwExitCode ); 
}

typedef BOOL (*GetExitCodeThreadFct)( HANDLE hThread, LPDWORD lpExitCode );
BOOL WINAPI MSSim_GetExitCodeThread( HANDLE hThread, LPDWORD lpExitCode )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  if( inst->taskIsAlive( (MSSim_Task*)hThread ) )  
  {
    *lpExitCode = STILL_ACTIVE; 
  } else {
    *lpExitCode = 0; 
  }
  // forward call to win api
//  static PROC func = GetProcAddress( GetKernel32DLL(), "GetExitCodeThread" ); 
//  ((GetExitCodeThreadFct)func)( hThread, lpExitCode ); 
  return true; 
}

typedef void (*CloseHandleFct)( HANDLE handle );
void WINAPI MSSim_CloseHandle( HANDLE handle )
{
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  if( handle )
  {
    HANDLE nativeHandle = inst->closeHandle( handle ); 
    // remove internal structures 
    if( nativeHandle )
    {
      static PROC func = GetProcAddress( GetKernel32DLL(), "CloseHandle" ); 
      ((CloseHandleFct)func)( nativeHandle ); 
    }
  }
}

typedef VOID (*EnterCriticalSectionFct)( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI MSSim_EnterCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_EnterCriticalSection" )
  
  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  if( inst )
  {
    inst->critEnter( lpCriticalSection ); 
  } else {
    static PROC func = GetProcAddress( GetKernel32DLL(), "EnterCriticalSection" ); 
    ((EnterCriticalSectionFct)func)( lpCriticalSection ); 
  }
}

typedef VOID (*LeaveCriticalSectionFct)( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI MSSim_LeaveCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_LeaveCriticalSection" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  if( inst )
  {
    inst->critLeave( lpCriticalSection ); 
  } else {
    static PROC func = GetProcAddress( GetKernel32DLL(), "LeaveCriticalSection" ); 
    ((LeaveCriticalSectionFct)func)( lpCriticalSection ); 
  }
}

typedef VOID (*InitializeCriticalSectionFct)( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI MSSim_InitializeCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_InitializeCriticalSection" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  lpCriticalSection = (LPCRITICAL_SECTION)inst->critCreate(); 

//  static PROC func = GetProcAddress( GetKernel32DLL(), "InitializeCriticalSection" ); 
//  ((InitializeCriticalSectionFct)func)( lpCriticalSection ); 
}

typedef VOID (*DeleteCriticalSectionFct)( LPCRITICAL_SECTION lpCriticalSection );
VOID WINAPI MSSim_DeleteCriticalSection( LPCRITICAL_SECTION lpCriticalSection )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_DeleteCriticalSection" )

  MSSim_Coordinator* inst( MSSim_Coordinator::GetInstance() ); 
  inst->critClose( lpCriticalSection ); 

//  static PROC func = GetProcAddress( GetKernel32DLL(), "DeleteCriticalSection" ); 
//  ((DeleteCriticalSectionFct)func)( lpCriticalSection ); 

}

/*
typedef BOOL (*InitializeCriticalSectionAndSpinCountFct)( LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount );
BOOL WINAPI MSSim_InitializeCriticalSectionAndSpinCount( LPCRITICAL_SECTION lpCriticalSection, DWORD dwSpinCount )
{
  MSSIM_TRACE( TraceSimFunctionCall, "MSSim_InitializeCriticalSectionAndSpinCount" )

    MSSim_Coordinator& inst = MSSim_Coordinator::GetInstance(); 
  lpCriticalSection = (LPCRITICAL_SECTION)inst.critCreate(); 
  return true; 
}
*/

//***************************************************************************

void* MSSim_SemCreateSysApi( int lInitialCount, int lMaximumCount ) 
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "CreateSemaphoreA" ); 
  return ((CreateSemaphoreAFct)func)( 0, lInitialCount, lMaximumCount, 0 ); 
}

bool MSSim_SemReleaseSysApi( void* h, int count )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "ReleaseSemaphore" ); 
  return ( ((ReleaseSemaphoreFct)func)( h, count, 0 ) ? true : false );
}

bool MSSim_SemCloseSysApi( void* handle ) 
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "CloseHandle" ); 
  ((CloseHandleFct)func)( handle ); 
  return true; 
}

void MSSim_SemWaitSysApi( void* h )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "WaitForSingleObject" ); 
  ((WaitForSingleObjectFct)func)( h, INFINITE ); 
}

typedef HANDLE (*GetCurrentThreadFct)( VOID );
void* MSSim_ThreadCurrentSysApi()
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "GetCurrentThread" ); 
  return ((GetCurrentThreadFct)func)(); 
}

void* MSSim_ThreadCreateSysApi( MSSim_TaskMainFct start_fct , void* args )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "CreateThread" ); 
  return ((CreateThreadFct)func)( 0, 0, (LPTHREAD_START_ROUTINE)start_fct, args, CREATE_SUSPENDED, 0 ); 
}

void MSSim_ThreadSuspendSysApi( void* t )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "SuspendThread" ); 
  ((SuspendThreadFct)func)( t ); 
}

void MSSim_ThreadResumeSysApi( void* t )
{
  static PROC func = GetProcAddress( GetKernel32DLL(), "ResumeThread" ); 
  ((ResumeThreadFct)func)( t ); 
}

//***************************************************************************

void MSSim_InitPatchMap() 
{
  MSSim_AddToPatchMap( LoadLibraryA, MSSim_LoadLibraryA ); 
  MSSim_AddToPatchMap( LoadLibraryW, MSSim_LoadLibraryW ); 
  MSSim_AddToPatchMap( LoadLibraryExA, MSSim_LoadLibraryExA ); 
  MSSim_AddToPatchMap( LoadLibraryExW, MSSim_LoadLibraryExW ); 

  MSSim_AddToPatchMap( SleepEx, MSSim_SleepEx ); 
  MSSim_AddToPatchMap( Sleep, MSSim_Sleep ); 
  MSSim_AddToPatchMap( WaitForSingleObject, MSSim_WaitForSingleObject ); 
  MSSim_AddToPatchMap( CreateSemaphoreA, MSSim_CreateSemaphoreA ); 
  MSSim_AddToPatchMap( CreateSemaphoreW, MSSim_CreateSemaphoreW ); 
  MSSim_AddToPatchMap( ReleaseSemaphore, MSSim_ReleaseSemaphore ); 
  MSSim_AddToPatchMap( CreateThread, MSSim_CreateThread ); 
  MSSim_AddToPatchMap( TerminateThread, MSSim_TerminateThread ); 
  MSSim_AddToPatchMap( SuspendThread, MSSim_SuspendThread ); 
  MSSim_AddToPatchMap( ResumeThread, MSSim_ResumeThread ); 
  MSSim_AddToPatchMap( ExitThread, MSSim_ExitThread ); 
  MSSim_AddToPatchMap( GetExitCodeThread, MSSim_GetExitCodeThread ); 
  MSSim_AddToPatchMap( CloseHandle, MSSim_CloseHandle ); 
  MSSim_AddToPatchMap( CreateMutexW, MSSim_CreateMutexW ); 
  MSSim_AddToPatchMap( CreateMutexA, MSSim_CreateMutexA ); 
  MSSim_AddToPatchMap( ReleaseMutex, MSSim_ReleaseMutex ); 
  MSSim_AddToPatchMap( EnterCriticalSection, MSSim_EnterCriticalSection ); 
  MSSim_AddToPatchMap( LeaveCriticalSection, MSSim_LeaveCriticalSection ); 
//  MSSim_AddToPatchMap( InitializeCriticalSection, MSSim_InitializeCriticalSection ); 
//  MSSim_AddToPatchMap( DeleteCriticalSection, MSSim_DeleteCriticalSection ); 

  //  MSSim_AddToPatchMap( InitializeCriticalSectionAndSpinCount, MSSim_InitializeCriticalSectionAndSpinCount ); 
}
/*
BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved )
{
  if( fdwReason == DLL_PROCESS_ATTACH )
  {
    // initialize coordinator
    MSSim_Coordinator::GetInstance(); 
  }
  return true; 
}
*/
