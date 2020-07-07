
#include "LVCSimulator/MultiSessionSimulator/MSSim_WinIATHook.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_Dispatcher.hpp"
#include "LVCSimulator/MultiSessionSimulator/MSSim_PatchedSysFunc.hpp"

#include <windows.h>
#include "psapi.h" // nocheck

#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <string>

using namespace std; 

// patched system functions

enum MSSim_PatchModuleType 
{
  patch_none = 0,
  patch_minimal = 1,
  patch_imports = 2,
  patch_symbols = 3
};

typedef map< string, MSSim_PatchModuleType > PatchTypeMap; 

static MSSim_Dispatcher* dispatcher; 

// function prototypes
void MSSim_WinIatHook_PatchAllModules(); 
void MSSim_WinIatHook_PatchModule( HMODULE moduleAddress ); 
void MSSim_WinIatHook_PatchIATEntry( PIMAGE_THUNK_DATA pITDA, LPVOID myHookFunction ); 

bool MSSim_WinIatHook_endsWith( const char* name, const char* end );
bool MSSim_WinIatHook_startsWith( const char* name, const char* start );

bool MSSim_WinIatHook_isFunctionEnabled( LPSTR modSrc, LPSTR modTgt, LPSTR apiName );
MSSim_PatchModuleType MSSim_WinIatHook_getPatchType( LPSTR mod );

bool MSSim_WinIatHook_HookApi( PIMAGE_THUNK_DATA pITDA, char* srcModName, char* inModName, char* inApiName, bool minimal );

set< HMODULE >& getPatchedModules() 
{
  static set< HMODULE > patched_modules; 
  return patched_modules; 
}

PatchTypeMap& getPatchTypeMap() 
{
  static PatchTypeMap patch_types; 
  return patch_types; 
}

//******************************************************************************
// Utility Functions
//******************************************************************************


bool MSSim_WinIatHook_endsWith( const char* name, const char* end )
{
  int delta = strlen( name ) - strlen( end ); 

  if( delta < 0 )
    return false; 

  return ( strcmp( end, ( name + delta ) ) == 0 ); 
}

bool MSSim_WinIatHook_startsWith( const char* name, const char* start )
{
  size_t len = strlen( start ); 
  if( len > strlen( name ) )
    return false; 

  return ( _strnicmp( name, start, len ) == 0 ); 
}

//******************************************************************************
// Hooking Control
//******************************************************************************

bool MSSim_WinIatHook_isFunctionEnabled( LPSTR modSrc, LPSTR modTgt, LPSTR apiName )
{
  // do not hook any calls to runtime DLLs
  if( MSSim_WinIatHook_startsWith( modTgt, "MSVCR" ) ) return false; 

  // do not hook any calls to the simulator
  // this prevents from modifying state when disabling the dispatcher temporary
  if( _stricmp( modTgt, "LIBMSSIM.DLL" ) == 0 ) return false; 

  return true; 
}

MSSim_PatchModuleType MSSim_WinIatHook_getPatchType( LPSTR mod ) 
{
  char szSystemDir[MAX_PATH]; 
  GetSystemDirectory( szSystemDir, sizeof( szSystemDir ) ); 
  _strupr( szSystemDir );

  // do not patch system libraries
  if( ! strncmp( szSystemDir, mod, strlen( szSystemDir ) ) )
    return patch_none;

  // do not patch the simulator library itself
  // this makes system calls in the library non-interruptable
  if( MSSim_WinIatHook_endsWith( mod, "LIBMSSIM.DLL" ) )  return patch_none; 
  // do not patch calls from the memory debugger
//  if( MSSim_WinIatHook_endsWith( mod, "LIBMEMDBG.DLL" ) ) return patch_none; 

  // check config file
  PatchTypeMap& patch_types = getPatchTypeMap(); 

  // modname is full path, search start of dll name
  char* dllName = strrchr( mod, '\\' ); 
  if( dllName )
  {
    ++dllName; 
  } else {
    dllName = mod; 
  }
 
  PatchTypeMap::iterator iter( patch_types.find( string( dllName ) ) ); 
  if( iter != patch_types.end() )
  {
    return iter->second;       
  }

  return patch_imports; 
}

void MSSim_WinIatHook_ReadPatchTypeMap() 
{
  char line[ 255 ]; 
  PatchTypeMap& patch_types = getPatchTypeMap(); 
  ifstream map_file( "patch_map.txt" );
  if( map_file )
  {
    while( ! map_file.eof() )
    {
      map_file.getline( line, 255, '\n'); 
      _strupr( line ); 
      // find delimiter
      char* libname = strtok( line, " " ); 
      char* type = strtok( 0, " " ); 
      if( libname && type )
      {
        MSSim_PatchModuleType patch_type = patch_minimal; 
        if( _stricmp( type, "SYMBOLS" ) == 0 )
        {
          patch_type = patch_symbols; 
        } else if( _stricmp( type, "IMPORTS" ) == 0 ) {
          patch_type = patch_imports; 
        } else if( ( _stricmp( type, "NONE" ) == 0 ) )
        {
          patch_type = patch_none; 
        }
        patch_types[ string( libname ) ] = patch_type; 
      }
    }
    map_file.close(); 
  }
}

//******************************************************************************
// Hooking Runtime
//******************************************************************************

void __stdcall MSSim_WinIatHook_HookFunctionProcessing() 
{
  // check dispatcher
  dispatcher->tryDispatch(); 
}

__declspec (naked) void MSSim_WinIatHook_HookFunctionEntry()
{
  _asm 
  {
    // load fct ptr in eax
    push ebp  
    mov ebp,esp 
    push ebx; 
    push esi; 
    push edi; 

    mov ebx,[ebp+4];
    mov eax,[ebx];
    // save regs before call
    push eax; 
    push ecx; 
    push edx; 
    // function param
//    push eax; 

    call MSSim_WinIatHook_HookFunctionProcessing; 

    // get reg values back
    pop edx; 
    pop ecx; 
    pop eax; 

    pop edi; 
    pop esi; 
    pop ebx; 

    // fix stackpointer to point on original call data
    pop ebp; 
    add esp,4; 

    //  forward call
    jmp eax;
  }
}

//******************************************************************************
// Hooking Startup
//******************************************************************************

void MSSim_WinIatHook_PatchIATEntry( PIMAGE_THUNK_DATA pITDA, LPVOID myHookFunction )
{
  DWORD dwOldProtect;
  DWORD dwTemp;
  // Make the page writable
  if( VirtualProtect( &pITDA->u1.Function, sizeof(DWORD), PAGE_READWRITE, &dwOldProtect) )
  {
    // Point the IAT entry to our hook function
    pITDA->u1.Function = (DWORD)myHookFunction;
    VirtualProtect( &pITDA->u1.Function, sizeof(DWORD), dwOldProtect, &dwTemp );
  }
}

bool MSSim_WinIatHook_HookApi( PIMAGE_THUNK_DATA pITDA, char* srcModName, char* inModName, char* inApiName, bool minimal )
{
  if( MSSim_WinIatHook_isFunctionEnabled( srcModName, inModName, inApiName ) )
  {
    // Allocate memory in the process for executable instructions
    // ioPatchCode is the address with which the IAT entry will be
    // replaced.
    //
    unsigned char* ioPatchCode = (unsigned char*)VirtualAlloc( NULL, 14, MEM_COMMIT, PAGE_EXECUTE_READWRITE );
    if (ioPatchCode)
    {
      // The code below is in 6-byte indirect CALL instruction
      // format. Once this code is executed, apiIndex is
      // pushed on the stack as return address and control is
      // transferred to HookFunctionEntry
      //
      ioPatchCode[0] = 0xFF;
      ioPatchCode[1] = 0x15;

      *((DWORD*)&ioPatchCode[2]) = (DWORD)&ioPatchCode[10];
      *((DWORD*)&ioPatchCode[6]) = (DWORD)pITDA->u1.Function;
      *((DWORD*)&ioPatchCode[10]) = (DWORD)&MSSim_WinIatHook_HookFunctionEntry;

      MSSim_WinIatHook_PatchIATEntry( pITDA, ioPatchCode );

      return true; 
    }
  }
  return false; 
}


bool isFunctionPointer( DWORD pFunc, DWORD codePtr, DWORD dataPtr )
{
  if( pFunc < codePtr )
    return false; 
  if( pFunc >= dataPtr )
    return false; 

  return true; 
}

void MSSim_WinIatHook_PatchImports( HMODULE moduleAddress, char* szModName, bool minimal )
{
  byte* moduleBaseAddress = (byte*)moduleAddress; 
  PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)moduleBaseAddress;
  PIMAGE_NT_HEADERS pINTH = (PIMAGE_NT_HEADERS)(moduleBaseAddress + pIDH->e_lfanew);
  DWORD dwImportTableOffset = pINTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(moduleBaseAddress + dwImportTableOffset);

  //
  // At this point pIID points to an array of Import Address Table, each
  // entry of the array is specific to a particular DLL. The code below
  // shows how we can traverse each array and fin d out which functions
  // are imported from which DLL.
  //
  while ( pIID->FirstThunk != 0 && pIID->OriginalFirstThunk != 0 )
  {
    //
    // DLL name from which functions are imported
    //
    LPSTR pszModuleName = (LPSTR)(moduleBaseAddress + pIID ->Name);
    HMODULE modAddress = GetModuleHandleA( pszModuleName );
    // get address of data section & address of code section 

    byte* modBase = (byte*)modAddress; 
    PIMAGE_DOS_HEADER pImpDH = (PIMAGE_DOS_HEADER)modBase; 
    PIMAGE_NT_HEADERS pImpNTH = (PIMAGE_NT_HEADERS)(modBase + pImpDH->e_lfanew); 

    // get address of code section & data section
    // in order to verify, if patched entry is not a pointer to data
    DWORD codeAdr = pImpNTH->OptionalHeader.BaseOfCode + (DWORD)modBase; 
    DWORD codeSize= pImpNTH->OptionalHeader.SizeOfCode; 
    DWORD dataAdr = pImpNTH->OptionalHeader.BaseOfData + (DWORD)modBase; 

    {
      //
      // First thunk points to IMAGE_THUNK_DATA
      //
      PIMAGE_THUNK_DATA pITDA = (PIMAGE_THUNK_DATA)(moduleBaseAddress + (DWORD)pIID ->FirstThunk);
      //
      // OriginalFirstThunk points to IMAGE_IMPORT_BY_NAME array but
      // due to its identical structure to IMAGE_THUNK_ DATA, we can use
      // IMAGE_THUNK_DATA object to dereference it.
      //
      PIMAGE_THUNK_DATA pIINA = (PIMAGE_THUNK_DATA)(moduleBaseAddress + (DWORD)pIID ->OriginalFirstThunk);
      // While Function address is not NULL

      while (pITDA->u1.Ordinal != 0)
      {
        // Ordinal and Function are a part of union u1
        // pFunction gives the address of function which
        // this IAT entry correspond too.
        PVOID pFunction = (PVOID)pITDA->u1.Function;
        //
        // If the function is Imported by name
        //

        if( IsBadReadPtr( &pIINA->u1.Ordinal, 8 ) )
        {
          // do not patch if memory cannot be read
          return; 
        }
        {
          if (!IMAGE_SNAP_BY_ORDINAL(pIINA->u1.Ordinal))
          {
            PIMAGE_IMPORT_BY_NAME pIIN = (PIMAGE_IMPORT_BY_NAME)(moduleBaseAddress + pIINA->u1.AddressOfData);

            // first check, if function is in special patch table
            void* patch_func = MSSim_GetFunctionPatch( (void*)pITDA->u1.Function ); 
            if( patch_func )
            {
              MSSim_WinIatHook_PatchIATEntry( pITDA, patch_func );
            } else {
              // patch only runtime functions if minimal
              if( ! minimal ) {
                // check, if pointer is inside code section
                // -> do not try to patch data imports
                if( codeAdr <= (DWORD)pFunction && (DWORD)pFunction < (codeAdr+codeSize) )
                {     
                  // pIIN->Name - points to name of the function
                  bool res = MSSim_WinIatHook_HookApi( pITDA, szModName, pszModuleName, (LPSTR)pIIN->Name, minimal );
                }
              }
            }
          }
        }
        pITDA++;
        pIINA++;
      }
    }
    pIID++;
  }
}

void MSSim_WinIatHook_PatchSymbols( HMODULE moduleAddress, char* szModName )
{
  byte* moduleBaseAddress = (byte*)moduleAddress; 
  PIMAGE_DOS_HEADER pIDH = (PIMAGE_DOS_HEADER)moduleBaseAddress;
  PIMAGE_NT_HEADERS pINTH = (PIMAGE_NT_HEADERS)(moduleBaseAddress + pIDH->e_lfanew);
  DWORD dwImportTableOffset = pINTH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  PIMAGE_IMPORT_DESCRIPTOR pIID = (PIMAGE_IMPORT_DESCRIPTOR)(moduleBaseAddress + dwImportTableOffset);
}

void MSSim_WinIatHook_PatchModule( HMODULE moduleAddress )
{
  // check for 0 
  if( ! moduleAddress )
  {
    return;
  }

  // check, if module is already patched
  set< HMODULE >& patched_modules = getPatchedModules(); 
  if( patched_modules.find( moduleAddress ) != patched_modules.end() )
    return; 

  char szModName[MAX_PATH];
  GetModuleFileNameEx( GetCurrentProcess(), moduleAddress, szModName, sizeof(szModName)); 
  _strupr( szModName ); 

  // check, if module is enabled
  MSSim_PatchModuleType patch_type = MSSim_WinIatHook_getPatchType( szModName ); 
  if( patch_type == patch_none )
  {
//    cout << " .. skipping " << szModName << endl; 
    return; 
  }

  cout << "... hooking ";

  switch( patch_type )
  {
  case patch_minimal: 
    cout << "MINIMAL " << szModName << endl; 
    MSSim_WinIatHook_PatchImports( moduleAddress, szModName, true ); 
    break; 
  case patch_imports: 
    cout << "IMPORTS " << szModName << endl; 
    MSSim_WinIatHook_PatchImports( moduleAddress, szModName, false ); 
    break; 
  case patch_symbols: 
    cout << "SYMBOLS " << szModName << endl; 
    MSSim_WinIatHook_PatchSymbols( moduleAddress, szModName ); 
    break; 
  }

  patched_modules.insert( moduleAddress ); 
}

void MSSim_WinIatHook_PatchAllModules() 
{
  HMODULE hMods[1024];
  DWORD cbNeeded;
  unsigned int i;

  if( EnumProcessModules( GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded) )
  {
    for ( i = 0; i < (cbNeeded / sizeof(HMODULE)); i++ )
    {
      MSSim_WinIatHook_PatchModule( hMods[i] ); 
    }
  }
}

void MSSim_InitializeCallback( MSSim_Dispatcher& disp )
{
  dispatcher = &disp;   
  MSSim_WinIatHook_ReadPatchTypeMap(); 
  MSSim_WinIatHook_PatchAllModules(); 
}

