#ifndef MSSIM_PATCHED_SYS_FUNC_HPP
#define MSSIM_PATCHED_SYS_FUNC_HPP

#include "LVCSimulator/MultiSessionSimulator/MSSim_Coordinator.hpp"

// functions implemented by os specific adapter

class MSSim_Dispatcher; 

void MSSim_InitializeCallback( MSSim_Dispatcher& ); 

// initialize map patched functions
void  MSSim_InitPatchMap(); 

// direct calls to os API functions
void* MSSim_SemCreateSysApi( int lInitialCount, int lMaximumCount ) ; 
void  MSSim_SemWaitSysApi( void* h );
bool  MSSim_SemReleaseSysApi( void* h, int count ); 
bool  MSSim_SemCloseSysApi( void* sem ); 

void* MSSim_ThreadCurrentSysApi(); 
void* MSSim_ThreadCreateSysApi( MSSim_TaskMainFct start_fct , void* args ); 
void  MSSim_ThreadSuspendSysApi( void* t );
void  MSSim_ThreadResumeSysApi( void* t ); 


// Generic part
void* MSSim_GetFunctionPatch( void* ); 
void MSSim_AddToPatchMap( void* orgFct, void* newFct ); 

#endif // #ifndef MSSIM_PATCHED_SYS_FUNC_HPP