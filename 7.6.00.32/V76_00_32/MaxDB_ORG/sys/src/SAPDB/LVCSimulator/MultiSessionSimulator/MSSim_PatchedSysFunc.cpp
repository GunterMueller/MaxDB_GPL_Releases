
#include "LVCSimulator/MultiSessionSimulator/MSSim_PatchedSysFunc.hpp"

#include < map >

using namespace std; 

typedef map<void*,void*> PatchMap; 

static PatchMap patch_map; 

void MSSim_AddToPatchMap( void* orgFct, void* newFct )
{
  patch_map[ orgFct ] = newFct; 
}

void* MSSim_GetFunctionPatch( void* func )
{
  static bool doInit( true ); 
  if( doInit )
  {
    MSSim_InitPatchMap(); 
    doInit = false; 
  }
  PatchMap::iterator iter = patch_map.find( func ); 
  if( iter != patch_map.end() )
  {
    return iter->second; 
  }
  return 0; 
}

