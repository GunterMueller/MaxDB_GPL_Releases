/*!
  @file           vos38.cpp
  @author         FerdinandF
  @brief          COM_Interface
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "gos00.h"
#include "heo02x.h"
//#include "heo78.h"
#include "geo007_1.h"
#include "gos00k.h"
#include "geo00d.h"
#include "hak341.h"
#include "hos38.h"
#include "geo38.h"
#include "liveCache/LVC_Dispatcher.hpp"
#include "liveCache/LVC_LoadLibrary.hpp"
#include "heo38.h"
#include "hsp100.h"
#include "heo670.h"
#include "heo36.h"

#undef _CDECL       // PTS 1112369
#include "geo573.h" // PTS 1112369

#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

#define	  STATUS_UNHANDLED_CPP_EXCEPTION     (unsigned long)0xE06D7363L   

#define REG_SK_COMPONENTS                      _T("Components")

externC
{ 
  typedef HRESULT (__stdcall *DLLGETCLASSOBJECT)(REFCLSID rclsid, REFIID riid, LPVOID* ppv);
}

/* ++++++++++++++ PTS 1112243 +++++++++++++++++ */
#if defined(INSTALL_HOOKS) 

#define MAKEPTR_OS38( cast, ptr, AddValue )\
  (cast)((teo00_Longuint)(ptr) + (teo00_Longuint)(AddValue))

typedef enum tos38_HookResult {
	success_eo38,
  module_not_imported_eo38,
  change_protection_failed_eo38,
  illegal_parameter_eo38
} tos38_HookResult;

#if defined (_WIN64)   
  typedef __int64  (__stdcall *tos38_WinProcPtr)();
#else
  typedef int      (__stdcall *tos38_WinProcPtr)();
#endif

typedef struct tos38_HookFuncDesc
{
    char *           pFuncName; /* The name of the function to hook. */
    tos38_WinProcPtr pProcAddr; /* The procedure to blast in.        */
} tos38_HookFuncDesc;

static int sqlHookCrtIOFunctions();

#endif

/*---------------------------------------------------------------------------*/
/* ++++++++++++ PTS 1110734 ++++++++++++++++++++++++++ */
externC int eo38_ErrorInExceptionHandler ( LPEXCEPTION_POINTERS lpexceptInfo )
{
  __try
  {
    char          dmp_buf[132];
    char          cExceptCode[32];
    
    MSGD ((ERR_DCOM_DUMP_INFO,"Exception within exception handling for db procedures"));   
    PTASK_CTRL_REC pTaskCtrl = THIS_UKT_CTRL->pCTask;   // CR 1103791
    switch (lpexceptInfo->ExceptionRecord->ExceptionCode)
    {
    case STATUS_WAIT_0:                       // 0x00000000L    
      strcpy(cExceptCode,"WAIT_0"); break;
    case STATUS_ABANDONED_WAIT_0:             // 0x00000080L    
      strcpy(cExceptCode,"ABANDONED_WAIT_0"); break;
    case STATUS_USER_APC:                     // 0x000000C0L    
      strcpy(cExceptCode,"USER_APC"); break;
    case STATUS_TIMEOUT:                      // 0x00000102L    
      strcpy(cExceptCode,"TIMEOUT"); break;
    case STATUS_PENDING:                      // 0x00000103L    
      strcpy(cExceptCode,"PENDING"); break;
    case STATUS_SEGMENT_NOTIFICATION:         // 0x40000005L    
      strcpy(cExceptCode,"SEGMENT_NOTIFICATION"); break;
    case STATUS_GUARD_PAGE_VIOLATION:         // 0x80000001L    
      strcpy(cExceptCode,"GUARD_PAGE_VIOLATION"); break;
    case STATUS_NONCONTINUABLE_EXCEPTION:     // 0xC0000025L    
      strcpy(cExceptCode,"GUARD_PAGE_VIOLATION"); break;
    case STATUS_BREAKPOINT:                   // 0x80000003L    
      strcpy(cExceptCode,"BREAKPOINT"); break;
    case STATUS_SINGLE_STEP:                  // 0x80000004L    
      strcpy(cExceptCode,"SINGLE_STEP"); break;
    case STATUS_IN_PAGE_ERROR:                // 0xC0000006L    
      strcpy(cExceptCode,"IN_PAGE_ERROR"); break;
    case STATUS_ILLEGAL_INSTRUCTION:          // 0xC000001DL    
      strcpy(cExceptCode,"ILLEGAL_INSTRUCTION"); break;
    case STATUS_PRIVILEGED_INSTRUCTION:       // 0xC0000096L    
      strcpy(cExceptCode,"PRIVILEGED_INSTRUCTION"); break;
    case STATUS_STACK_OVERFLOW:               // 0xC00000FDL    
      strcpy(cExceptCode,"STACK_OVERFLOW"); break;
    case STATUS_CONTROL_C_EXIT:               // 0xC000013AL    
      strcpy(cExceptCode,"CONTROL_C_EXIT"); break;
    case STATUS_DATATYPE_MISALIGNMENT:        // 0x80000002L   
      strcpy(cExceptCode,"DATATYPE_MISALIGNMENT"); break;
    case STATUS_ACCESS_VIOLATION:             // 0xC0000005L    
      strcpy(cExceptCode,"ACCESS_VIOLATION"); break;
    case STATUS_INVALID_HANDLE:               // 0xC0000008L    
      strcpy(cExceptCode,"INVALID_HANDLE"); break;
    case STATUS_NO_MEMORY:                    // 0xC0000017L    
      strcpy(cExceptCode,"NO_MEMORY"); break;
    case STATUS_INVALID_DISPOSITION:          // 0xC0000026L    
      strcpy(cExceptCode,"INVALID_DISPOSITION"); break;
    case STATUS_ARRAY_BOUNDS_EXCEEDED:        // 0xC000008CL    
      strcpy(cExceptCode,"ARRAY_BOUNDS_EXCEEDED"); break;
    case STATUS_FLOAT_DENORMAL_OPERAND:       // 0xC000008DL    
      strcpy(cExceptCode,"FLOAT_DENORMAL_OPERAND"); break;
    case STATUS_FLOAT_DIVIDE_BY_ZERO:         // 0xC000008EL    
      strcpy(cExceptCode,"FLOAT_DIVIDE_BY_ZERO"); break;
    case STATUS_FLOAT_INEXACT_RESULT:         // 0xC000008FL    
      strcpy(cExceptCode,"FLOAT_INEXACT_RESULT"); break;
    case STATUS_FLOAT_INVALID_OPERATION:      // 0xC0000090L    
      strcpy(cExceptCode,"FLOAT_INVALID_OPERATION"); break;
    case STATUS_FLOAT_OVERFLOW:               // 0xC0000091L    
      strcpy(cExceptCode,"FLOAT_OVERFLOW"); break;
    case STATUS_FLOAT_STACK_CHECK:            // 0xC0000092L    
      strcpy(cExceptCode,"FLOAT_STACK_CHECK"); break;
    case STATUS_FLOAT_UNDERFLOW:              // 0xC0000093L    
      strcpy(cExceptCode,"FLOAT_UNDERFLOW"); break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO:       // 0xC0000094L    
      strcpy(cExceptCode,"INTEGER_DIVIDE_BY_ZERO"); break;
    case STATUS_INTEGER_OVERFLOW:             // 0xC0000095L    
      strcpy(cExceptCode,"INTEGER_OVERFLOW"); break;
    case STATUS_SETERROR_EXCEPTION:           // own code
      strcpy(cExceptCode,"SETERROR_EXCEPTION"); break;
    case STATUS_UNHANDLED_CPP_EXCEPTION:      // 0xE06D7363L   
      strcpy(cExceptCode,"UNHANDLED_C++_EXCEPTION");
      break;  
    default:
      sprintf(cExceptCode,"ExceptionCode: 0x%8.8X ", 
        lpexceptInfo->ExceptionRecord->ExceptionCode);
    }
    sprintf(dmp_buf,"%s  at address 0x%p  flags: 0x%8.8X parms: %d TaskIndex: %d",
      cExceptCode, 
      lpexceptInfo->ExceptionRecord-> ExceptionAddress, 
      lpexceptInfo->ExceptionRecord-> ExceptionFlags, 
      lpexceptInfo->ExceptionRecord-> NumberParameters, 
      pTaskCtrl->ulTaskIndex);                          // CR 1103791
    MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));
    
    for ( unsigned int ix = 0; ix < lpexceptInfo->ExceptionRecord-> NumberParameters; ix++ ) {
      sprintf(dmp_buf,"Parameter_%d:  0x%8.8X", ix, 
        lpexceptInfo->ExceptionRecord-> ExceptionInformation[ix]); 
      MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));   
    }    
    sprintf(dmp_buf,"CurrStackPointer: 0x%p  ulTaskStackSize: 0x%8.8X", 
      pTaskCtrl->pCurrStackPointer, pTaskCtrl->ulTaskStackSize); 
    MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));   
  }
  __except (EXCEPTION_EXECUTE_HANDLER) 
  {
  }
  return EXCEPTION_EXECUTE_HANDLER;
};
/* ----------------- PTS 1110734 ---------------------- */

//static int eo38_Eval_ExceptionLL( tsp_dcom_dispatch_parms * parms, LPEXCEPTION_POINTERS lpexceptInfo )
//{
//  int action;
//  IID loc_iid;
//  IDispatch               *lpIDispatch;
//  IClassFactory           *lpIFactory;
//  LVC_DispatcherCOMClassHash            *HashPtr;
//  LVC_DispatcherHashEntryPtr           currEntry;
//
//  //RTESync_LockedScope lck(LVC_Dispatcher::getDCOMMemory().lExclusive);  // PTS 1110734   
//  
//  __try {   // PTS 1110734 
//  // typedef struct _EXCEPTION_POINTERS { // exp 
//  //    PEXCEPTION_RECORD ExceptionRecord; 
//  //    PCONTEXT ContextRecord; 
//  // } EXCEPTION_POINTERS; 
//  
//  //  typedef struct _EXCEPTION_RECORD { // exr 
//  //    DWORD ExceptionCode; 
//  //    DWORD ExceptionFlags; 
//  //    struct _EXCEPTION_RECORD *ExceptionRecord; 
//  //    PVOID ExceptionAddress; 
//  //    DWORD NumberParameters; 
//  //    DWORD ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS]; 
//  // } EXCEPTION_RECORD; 
//  memset(&parms->errtext[0],' ',sizeof(parms->errtext));
//  switch (lpexceptInfo->ExceptionRecord->ExceptionCode)
//  {
//  case STATUS_WAIT_0:                     // 0x00000000L    
//  case STATUS_ABANDONED_WAIT_0:           // 0x00000080L    
//  case STATUS_USER_APC:                   // 0x000000C0L    
//  case STATUS_TIMEOUT:                    // 0x00000102L    
//  case STATUS_PENDING:                    // 0x00000103L    
//  case STATUS_SEGMENT_NOTIFICATION:       // 0x40000005L    
//  case STATUS_GUARD_PAGE_VIOLATION:       // 0x80000001L    
//  case STATUS_NONCONTINUABLE_EXCEPTION:   // 0xC0000025L    
//  case STATUS_BREAKPOINT:                 // 0x80000003L    
//  case STATUS_SINGLE_STEP:                // 0x80000004L    
//  case STATUS_IN_PAGE_ERROR:              // 0xC0000006L    
//  case STATUS_ILLEGAL_INSTRUCTION:        // 0xC000001DL    
//  case STATUS_PRIVILEGED_INSTRUCTION:     // 0xC0000096L    
//  case STATUS_STACK_OVERFLOW:             // 0xC00000FDL    
//  case STATUS_CONTROL_C_EXIT:             // 0xC000013AL    
//  /* following exceptions had been catched before PTS 1110734 */
//  case STATUS_DATATYPE_MISALIGNMENT:      // 0x80000002L    
//  case STATUS_ACCESS_VIOLATION:           // 0xC0000005L    
//  case STATUS_INVALID_HANDLE:             // 0xC0000008L    
//  case STATUS_NO_MEMORY:                  // 0xC0000017L    
//  case STATUS_INVALID_DISPOSITION:        // 0xC0000026L    
//  case STATUS_ARRAY_BOUNDS_EXCEEDED:      // 0xC000008CL    
//  case STATUS_FLOAT_DENORMAL_OPERAND:     // 0xC000008DL    
//  case STATUS_FLOAT_INVALID_OPERATION:    // 0xC0000090L    
//  case STATUS_FLOAT_STACK_CHECK:          // 0xC0000092L    
//  case XCPT_ABORT_EXCEPTION:              // 0xE000AFFE  abort       PTS 1110787 FF 
//  case XCPT_ABORT_EXCEPTION_MINI_DUMP:    // 0xE000AAAA  mini dump' PTS 1110787 FF 
//    action = EXCEPTION_CONTINUE_SEARCH;   // exceptions - dump
//    //CLEARLOCK (DCOM_EXCLUSIVE);                                   /* PTS 1110787 FF */
//    return action;         /* subsequent handler documents exception PTS 1110787 FF */
//  case STATUS_FLOAT_DIVIDE_BY_ZERO:       // 0xC000008EL    
//  case STATUS_FLOAT_INEXACT_RESULT:       // 0xC000008FL    
//  case STATUS_FLOAT_OVERFLOW:             // 0xC0000091L    
//  case STATUS_FLOAT_UNDERFLOW:            // 0xC0000093L    
//  case STATUS_INTEGER_DIVIDE_BY_ZERO:     // 0xC0000094L    
//  case STATUS_INTEGER_OVERFLOW:           // 0xC0000095L    
//  case STATUS_UNHANDLED_CPP_EXCEPTION:    // 0xE06D7363L   
//    tsp00_C4                session;
//    CIliveCacheSink*        pIliveCacheSink;
//    tsp1_packet*            SQLBuffer_ptr;
//    
//    action = EXCEPTION_CONTINUE_SEARCH;   // exceptions - dump
//    if (parms->session_context != NULL )
//    {
//      ak341GetDebugContext ((tak_all_command_glob*)parms->session_context,
//        *REINTERPRET_CAST(tgg91_SessionNo*, &session),
//        *REINTERPRET_CAST(tsp00_Addr*, &pIliveCacheSink),
//        SQLBuffer_ptr );
//      if ( !pIliveCacheSink->CallbackIsActive)
//        action = EXCEPTION_EXECUTE_HANDLER; // call Handler and stay alive
//    }
//    if ( action != EXCEPTION_EXECUTE_HANDLER ) {                    /* PTS 1110787 FF */
//      //CLEARLOCK (DCOM_EXCLUSIVE);                                   /* PTS 1110787 FF */
//      return action;         /* subsequent handler documents exception PTS 1110787 FF */
//    }                                                               /* PTS 1110787 FF */
//    
//    parms->hresult = GEO00D_DCOM_DBPROC_CRASHED;
//    break;
//  case STATUS_SETERROR_EXCEPTION:                  // own code
//    action = EXCEPTION_EXECUTE_HANDLER;            // call Handler and stay alive
//    parms->hresult = GEO00D_DCOM_EXCEPTION_RAISED; // no CRASH message into knldiag file
//    break;
//  case STATUS_TERMINATE_EXCEPTION:           // own code 0xC0004712L
//    action = EXCEPTION_EXECUTE_HANDLER;      // call Handler and stay alive
//    parms->hresult = GEO00D_DCOM_TERMINATED; // no CRASH message into knldiag file
//    break;
//  default:
//    action = EXCEPTION_CONTINUE_SEARCH;   //  exceptions - dump
//    //CLEARLOCK (DCOM_EXCLUSIVE);                                   /* PTS 1110787 FF */
//    return action;         /* subsequent handler documents exception PTS 1110787 FF */
//  }
//  if ((GEO00D_DCOM_EXCEPTION_RAISED == parms->hresult) ||
//    ( GEO00D_DCOM_TERMINATED       == parms->hresult))
//  {
//    //CLEARLOCK (DCOM_EXCLUSIVE);  // PTS 1110734 
//    return action;
//  }
//  lpIDispatch = (IDispatch *) parms->idispatch;
//  lpIFactory = (IClassFactory *) parms->ifactory;
//  
//  /*  PTS 1109308   +++++                              */
//  /*  to avoid deadlocks when an error occurred within */
//  /*  critical section, we will wait only 30 Seconds   */
//  /*  for the critical region to be released, after    */
//  /*  this time, we go on without getting the semaphore*/
//  /*  and forget about searchin for the db-proc name   */
//  // PTS 1110734 if ( DCOM_EXCLUSIVE ) {
//  // PTS 1110734  for ( int i=0; i < 3000; i++ ) {
//  // PTS 1110734    Sleep( 10 );
//  // PTS 1110734    if ( !DCOM_EXCLUSIVE ) break;
//  // PTS 1110734  };
//  // PTS 1110734  if ( DCOM_EXCLUSIVE ) 
//  // PTS 1110734    Dirty_Flag = true;
//  // PTS 1110734  else
//  // PTS 1110734    WAIT_UNTIL_ALONE (DCOM_EXCLUSIVE);
//  // PTS 1110734}
//  // PTS 1110734else
//    /*  PTS 1109308   -----                              */
//  // PTS 1110734  WAIT_UNTIL_ALONE (DCOM_EXCLUSIVE);     
//  
//  //try {   // PTS 1109308 PTS 1110734 
//    HashPtr = LVC_Dispatcher::getDCOMMemory().lphead;
//    if ( NULL != HashPtr ) // PTS 1110734 && !Dirty_Flag ) /* !Dirty_Flag added PTS 1109308 */
//    {
//      /* Release session related debugger                        */
//      HashPtr->ResetEntrySearch( 0, LVC_DispatcherCOMClassHash::SearchALL );
//      currEntry = HashPtr->NextEntry();
//      while ( NULL != currEntry)
//      {
//        if ((currEntry->getIUnknown() == lpIDispatch) ||
//          (currEntry->getIUnknown() == lpIFactory))
//        {
//          SAPDB_memcpy (&loc_iid.Data1, currEntry->getGuid(), sizeof (loc_iid));
//          if (currEntry->getIUnknown() == lpIDispatch)
//          {
//            MSGD ((ERR_DCOM_COMETHOD_CRASHED,
//              loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
//              loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
//              loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
//              loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
//          }
//          else
//          {
//            MSGD ((ERR_DCOM_DBPROC_CRASHED,
//              loc_iid.Data1, loc_iid.Data2, loc_iid.Data3,
//              loc_iid.Data4[0], loc_iid.Data4[1], loc_iid.Data4[2],
//              loc_iid.Data4[3], loc_iid.Data4[4], loc_iid.Data4[5],
//              loc_iid.Data4[6], loc_iid.Data4[7], parms->dispid));
//          }
//          break;
//        }
//        currEntry = HashPtr->NextEntry();;
//      }
//    }
//    eo38_Dump_Exception (lpexceptInfo);   
//  }                                       // PTS 1109308
//__except ( eo38_ErrorInExceptionHandler ( GetExceptionInformation() ) ) { //1110734
//    action = EXCEPTION_CONTINUE_SEARCH;   // PTS 1109308
//  };                                      // PTS 1109308
//  // PTS 1110734#ifdef USE_KGS                            
//  // PTS 1110734if ( !Dirty_Flag )                      // PTS 1109308
//    //CLEARLOCK (DCOM_EXCLUSIVE);
//  // PTS 1110734#endif                                    
//  return action;
//}

//externC int eo38_Eval_Exception ( tsp_dcom_dispatch_parms * parms, LPEXCEPTION_POINTERS lpexceptInfo )
//{
//  // wrapper needed, since in one routine it's not possible to mix SEH and C++ exceptions
//  RTESync_LockedScope lck(LVC_Dispatcher::getDCOMMemory().lExclusive);
//  return eo38_Eval_ExceptionLL(parms, lpexceptInfo);
//}

/*---------------------------------------------------------------------------*/

externC void eo38_Dump_Exception ( LPEXCEPTION_POINTERS lpexceptInfo )
{
  char                     dmp_buf[132];
  char                     cExceptCode[32];
  OSVERSIONINFO            stOSVI ;
  BOOL                     bRet;
  eo670TraceContext        traceContext;
  
  PTASK_CTRL_REC pTaskCtrl = THIS_UKT_CTRL->pCTask;   // CR 1103791
  switch (lpexceptInfo->ExceptionRecord->ExceptionCode)
  {
  case STATUS_WAIT_0:                       // 0x00000000L    
    strcpy(cExceptCode,"WAIT_0"); break;
  case STATUS_ABANDONED_WAIT_0:             // 0x00000080L    
    strcpy(cExceptCode,"ABANDONED_WAIT_0"); break;
  case STATUS_USER_APC:                     // 0x000000C0L    
    strcpy(cExceptCode,"USER_APC"); break;
  case STATUS_TIMEOUT:                      // 0x00000102L    
    strcpy(cExceptCode,"TIMEOUT"); break;
  case STATUS_PENDING:                      // 0x00000103L    
    strcpy(cExceptCode,"PENDING"); break;
  case STATUS_SEGMENT_NOTIFICATION:         // 0x40000005L    
    strcpy(cExceptCode,"SEGMENT_NOTIFICATION"); break;
  case STATUS_GUARD_PAGE_VIOLATION:         // 0x80000001L    
    strcpy(cExceptCode,"GUARD_PAGE_VIOLATION"); break;
  case STATUS_NONCONTINUABLE_EXCEPTION:     // 0xC0000025L    
    strcpy(cExceptCode,"GUARD_PAGE_VIOLATION"); break;
  case STATUS_BREAKPOINT:                   // 0x80000003L    
    strcpy(cExceptCode,"BREAKPOINT"); break;
  case STATUS_SINGLE_STEP:                  // 0x80000004L    
    strcpy(cExceptCode,"SINGLE_STEP"); break;
  case STATUS_IN_PAGE_ERROR:                // 0xC0000006L    
    strcpy(cExceptCode,"IN_PAGE_ERROR"); break;
  case STATUS_ILLEGAL_INSTRUCTION:          // 0xC000001DL    
    strcpy(cExceptCode,"ILLEGAL_INSTRUCTION"); break;
  case STATUS_PRIVILEGED_INSTRUCTION:       // 0xC0000096L    
    strcpy(cExceptCode,"PRIVILEGED_INSTRUCTION"); break;
  case STATUS_STACK_OVERFLOW:               // 0xC00000FDL    
    strcpy(cExceptCode,"STACK_OVERFLOW"); break;
  case STATUS_CONTROL_C_EXIT:               // 0xC000013AL    
    strcpy(cExceptCode,"CONTROL_C_EXIT"); break;
  case STATUS_DATATYPE_MISALIGNMENT:        // 0x80000002L   
    strcpy(cExceptCode,"DATATYPE_MISALIGNMENT"); break;
  case STATUS_ACCESS_VIOLATION:             // 0xC0000005L    
    strcpy(cExceptCode,"ACCESS_VIOLATION"); break;
  case STATUS_INVALID_HANDLE:               // 0xC0000008L    
    strcpy(cExceptCode,"INVALID_HANDLE"); break;
  case STATUS_NO_MEMORY:                    // 0xC0000017L    
    strcpy(cExceptCode,"NO_MEMORY"); break;
  case STATUS_INVALID_DISPOSITION:          // 0xC0000026L    
    strcpy(cExceptCode,"INVALID_DISPOSITION"); break;
  case STATUS_ARRAY_BOUNDS_EXCEEDED:        // 0xC000008CL    
    strcpy(cExceptCode,"ARRAY_BOUNDS_EXCEEDED"); break;
  case STATUS_FLOAT_DENORMAL_OPERAND:       // 0xC000008DL    
    strcpy(cExceptCode,"FLOAT_DENORMAL_OPERAND"); break;
  case STATUS_FLOAT_DIVIDE_BY_ZERO:         // 0xC000008EL    
    strcpy(cExceptCode,"FLOAT_DIVIDE_BY_ZERO"); break;
  case STATUS_FLOAT_INEXACT_RESULT:         // 0xC000008FL    
    strcpy(cExceptCode,"FLOAT_INEXACT_RESULT"); break;
  case STATUS_FLOAT_INVALID_OPERATION:      // 0xC0000090L    
    strcpy(cExceptCode,"FLOAT_INVALID_OPERATION"); break;
  case STATUS_FLOAT_OVERFLOW:               // 0xC0000091L    
    strcpy(cExceptCode,"FLOAT_OVERFLOW"); break;
  case STATUS_FLOAT_STACK_CHECK:            // 0xC0000092L    
    strcpy(cExceptCode,"FLOAT_STACK_CHECK"); break;
  case STATUS_FLOAT_UNDERFLOW:              // 0xC0000093L    
    strcpy(cExceptCode,"FLOAT_UNDERFLOW"); break;
  case STATUS_INTEGER_DIVIDE_BY_ZERO:       // 0xC0000094L    
    strcpy(cExceptCode,"INTEGER_DIVIDE_BY_ZERO"); break;
  case STATUS_INTEGER_OVERFLOW:             // 0xC0000095L    
    strcpy(cExceptCode,"INTEGER_OVERFLOW"); break;
  case STATUS_SETERROR_EXCEPTION:           // own code
    strcpy(cExceptCode,"SETERROR_EXCEPTION"); break;
  case STATUS_UNHANDLED_CPP_EXCEPTION:      // 0xE06D7363L   
    strcpy(cExceptCode,"UNHANDLED_C++_EXCEPTION");
    break;  
  default:
    sprintf(cExceptCode,"ExceptionCode: 0x%8.8X ", 
      lpexceptInfo->ExceptionRecord->ExceptionCode);
  }
  sprintf(dmp_buf,"%s  at address 0x%p  flags: 0x%8.8X parms: %d TaskIndex: %d",
    cExceptCode, 
    lpexceptInfo->ExceptionRecord-> ExceptionAddress, 
    lpexceptInfo->ExceptionRecord-> ExceptionFlags, 
    lpexceptInfo->ExceptionRecord-> NumberParameters, 
    pTaskCtrl->ulTaskIndex);                          // CR 1103791
  MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));

  for ( unsigned int ix = 0; ix < lpexceptInfo->ExceptionRecord-> NumberParameters; ix++ ) {
    sprintf(dmp_buf,"Parameter_%d:  0x%8.8X", ix, 
      lpexceptInfo->ExceptionRecord-> ExceptionInformation[ix]); 
    MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));   
  }

  if (STATUS_UNHANDLED_CPP_EXCEPTION == lpexceptInfo->ExceptionRecord->ExceptionCode) {
    unsigned *errorObjAdr = (unsigned *)lpexceptInfo->ExceptionRecord-> ExceptionInformation[1];
    for ( int i=0; i < 32; i=i+8 )
    if (!IsBadReadPtr((void *)(errorObjAdr+i),64)) {
      sprintf(dmp_buf,"ErrorObj: 0x%p - %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X %8.8X", errorObjAdr+i,
        *(errorObjAdr+i), *(errorObjAdr+i+1), *(errorObjAdr+i+2), *(errorObjAdr+i+3),
        *(errorObjAdr+i+4), *(errorObjAdr+i+5), *(errorObjAdr+i+6), *(errorObjAdr+i+7));
      MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));   
    };
  }

  traceContext.threadHandle   = GetCurrentThread();
  traceContext.pThreadContext = lpexceptInfo->ContextRecord;
  eo670_CTraceContextStack( &traceContext );

  sprintf(dmp_buf,"CurrStackPointer: 0x%p  ulTaskStackSize: 0x%8.8X", 
    pTaskCtrl->pCurrStackPointer, pTaskCtrl->ulTaskStackSize); 
  MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));   
  
  /* Check Version */
  memset (&stOSVI, NULL, sizeof (OSVERSIONINFO)) ;
  stOSVI.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  bRet = GetVersionEx ( &stOSVI ) ;
  if (FALSE == bRet) return;
  if ((VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId) &&
    (4 == stOSVI.dwMajorVersion))
  {
    MEMORY_BASIC_INFORMATION   vmq;
    MEMORY_BASIC_INFORMATION   vmq2;
    char          szBuff[MAX_PATH];
    void*         pvAddress = 0x00000000;
    void*         pvAddress2;
    bool          fok = true;
    unsigned long len;

    while(fok)
    {
      fok = ( VirtualQuery(pvAddress, &vmq, sizeof(vmq)) == sizeof(vmq));
      if (fok)
      {
        pvAddress2 = ((BYTE *) vmq.AllocationBase + vmq.RegionSize);
        
        fok = ( VirtualQuery(pvAddress2, &vmq2, sizeof(vmq2)) == sizeof(vmq2));
        while ( fok && (( vmq2.AllocationBase == vmq.AllocationBase ) || (vmq2.AllocationBase ==0)))
        {
          vmq.RegionSize = vmq.RegionSize + vmq2.RegionSize;
          pvAddress2 = ((BYTE *) vmq2.BaseAddress + vmq2.RegionSize);
          fok = ( VirtualQuery(pvAddress2, &vmq2, sizeof(vmq2)) == sizeof(vmq2));
        }
        
        // check if exception address lies within region
        if (( lpexceptInfo->ExceptionRecord->ExceptionAddress >= vmq.AllocationBase ) &&
          ( lpexceptInfo->ExceptionRecord->ExceptionAddress <= ((BYTE*)vmq.AllocationBase + vmq.RegionSize) ) && 
          ( vmq.AllocationBase != NULL ) && ( vmq.AllocationBase == vmq.BaseAddress ))
        {
          len = GetModuleFileName((HINSTANCE) vmq.AllocationBase, &szBuff[0], sizeof(szBuff));
          if (len)
          {
            sprintf (dmp_buf, "0x%p - %.120s", vmq.AllocationBase , szBuff ) ;
            MSGD ((ERR_DCOM_DUMP_INFO,dmp_buf));
          }  
          fok = false;
        }
        // Get address of next area to be examined
        pvAddress = ((BYTE *) vmq.BaseAddress + vmq.RegionSize);
      }
    }
  }
}

/* ++++++++++++++ PTS 1112243 +++++++++++++++++ */
#if defined(INSTALL_HOOKS) 

/*---------------------------------------------------------------------------*/

const tsp00_Uint4 cuiToHookFuncCnt = 17;
static tos38_HookFuncDesc g_aToHookIOFuncDsc_os38[cuiToHookFuncCnt];

int _creatHook( const char *filename, int pmode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_creat' isn't supported"));
  /* signal error */
  return ( -1 );
};

int _wcreatHook( const wchar_t *filename, int pmode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wcreat' isn't supported"));
  /* signal error */
  return ( -1 );
};

int _open_osfhandleHook ( long osfhandle, int flags ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_open_osfhandle' isn't supported"));
  /* signal error */
  return ( -1 );
};

FILE* fopenHook(const char *filename, const char *mode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function 'fopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *_wfopenHook( const wchar_t *filename, const wchar_t *mode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wfopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *_fdopenHook( int handle, const char *mode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_fdopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *_wfdopenHook( int handle, const wchar_t *mode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wfdopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *freopenHook( const char *path, const char *mode, FILE *stream ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function 'freopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *_wfreopenHook( const wchar_t *path, const wchar_t *mode, FILE *stream ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wfreopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

int _openHook( const char *filename, int oflag , int pmode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_open' isn't supported"));
  /* signal error */
  return ( -1 );
};

int _wopenHook( const wchar_t *filename, int oflag , int pmode ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wopen' isn't supported"));
  /* signal error */
  return ( -1 );
};

int _sopenHook( const char *filename, int oflag, int shflag , int pmode  ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_sopen' isn't supported"));
  /* signal error */
  return ( -1 );
};

int _wsopenHook( const wchar_t *filename, int oflag, int shflag , int pmode  ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wsopen' isn't supported"));
  /* signal error */
  return ( -1 );
};

FILE *_fsopenHook( const char *filename, const char *mode, int shflag ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_fsopen' isn't supported"));
  /* signal error */
  return ( NULL );
};

FILE *_wfsopenHook( const wchar_t *filename, const wchar_t *mode, int shflag ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_wfsopen' isn't supported"));
  /* signal error */
  return ( NULL );
};


int  fcloseHook( FILE *stream ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function 'fclose' isn't supported"));
  /* signal error */
  return ( EOF );
};

int _fcloseallHook( void ) {
  MSGD ((ERR_DCOM_DUMP_INFO,"C rte function '_fcloseall' isn't supported"));
  /* signal error */
  return ( EOF );
};

/*---------------------------------------------------------------------------*/

static PIMAGE_IMPORT_DESCRIPTOR
sql38GetNamedImportDescriptor (tsp00_Void * hModule,
                               char *       pModuleName )
{
    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule ;

    /* Is this the MZ header? */
    if ( ( TRUE == IsBadReadPtr( pDOSHeader, sizeof( IMAGE_DOS_HEADER ))) ||
         ( IMAGE_DOS_SIGNATURE != pDOSHeader->e_magic )) {
        return ( NULL ) ;
    }

    /* Get the PE header.   */
    PIMAGE_NT_HEADERS pNTHeader = MAKEPTR_OS38 ( PIMAGE_NT_HEADERS,
                                            pDOSHeader,
                                            pDOSHeader->e_lfanew );

    /* Is this a real PE image? */
    if (( TRUE == IsBadReadPtr( pNTHeader, sizeof( IMAGE_NT_HEADERS ))) ||
         ( IMAGE_NT_SIGNATURE != pNTHeader->Signature )) {
        return ( NULL ) ;
    }

    /* Get the pointer to the imports section. */
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc = MAKEPTR_OS38 ( PIMAGE_IMPORT_DESCRIPTOR,
      pDOSHeader,
      pNTHeader->OptionalHeader.
      DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].
      VirtualAddress );

    /* Does it have an imports section? */
    if ( 0 == pImportDesc ) {
        return ( NULL ) ;
    }

    /* Loop through the import module descriptors looking for the  *
     * module whose name matches pModuleName.                      */
    while ( NULL != pImportDesc->Name ) {
      PSTR szCurrMod = MAKEPTR_OS38( PSTR, pDOSHeader, pImportDesc->Name );
      if ( 0 == stricmp( szCurrMod , pModuleName )) {
        break;  /* found it */
      }
      pImportDesc++ ;
    }

    /* If the name is NULL, then the module is not imported. */
    if ( NULL == pImportDesc->Name ) {
      return ( NULL );
    }
    return ( pImportDesc ) ;
}

/*---------------------------------------------------------------------------*/

static tos38_HookResult 
sql38HookImportedFunctionsByName(tsp00_Void *         hModule     ,
                                 char *               pModuleName ,
                                 tsp00_Uint4          uiCount     ,
                                 tos38_HookFuncDesc * paHookArray ,
                                 tos38_WinProcPtr *   paOrigFuncs ,
                                 tsp00_Uint4 *        puiHooked    )
{
  /* check Parameters  */
  if(( hModule == NULL ) || 
    ( pModuleName == NULL ) ||
    ( uiCount == 0 ) ||
    ( paHookArray == NULL ) ||
    ( TRUE == IsBadReadPtr ( paHookArray, sizeof(tos38_HookFuncDesc) * uiCount )) ||
    (( paOrigFuncs != NULL ) &&
    ( TRUE == IsBadWritePtr ( paOrigFuncs, sizeof(tos38_WinProcPtr) * uiCount ))) ||
    ( puiHooked == NULL ) ||
    ( TRUE == IsBadWritePtr ( puiHooked , sizeof(tsp00_Uint4) )) ) {
    return ( illegal_parameter_eo38 );
  }

  /* initialization */
  if ( NULL != paOrigFuncs ) {
    memset ( paOrigFuncs , NULL , sizeof (tos38_WinProcPtr) * uiCount );
  }
  *puiHooked = 0;
  
  /* Get the specific import descriptor. */
  PIMAGE_IMPORT_DESCRIPTOR pImportDesc = 
    sql38GetNamedImportDescriptor( hModule, pModuleName );
  if ( NULL == pImportDesc ) {
    return ( module_not_imported_eo38 ) ;
  }
  
  /* Get the original thunk information for this DLL.  
  * The thunk information stored in the pImportDesc->FirstThunk
  * cannot be used because that is the array that the loader has 
  * already bashed to fix up all the imports.  
  * This pointer gives us access to the function names. 
  */
  PIMAGE_THUNK_DATA pOrigThunk =
    MAKEPTR_OS38( PIMAGE_THUNK_DATA,
    hModule,
    pImportDesc->OriginalFirstThunk );
  
  /* Get the array pointed to by the pImportDesc->FirstThunk.  
  * This is where the actual bash will be done.*/
  PIMAGE_THUNK_DATA pRealThunk = 
    MAKEPTR_OS38( PIMAGE_THUNK_DATA, 
    hModule, 
    pImportDesc->FirstThunk );

  /* Loop through and look for the one that matches the name. */
  while ( NULL != pOrigThunk->u1.Function ) {
    /* Only look at those that are imported by name, not ordinal. */
    if ( IMAGE_ORDINAL_FLAG != 
      ( pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG )) {
      /* Look get the name of this imported function. */
      PIMAGE_IMPORT_BY_NAME pByName;
      
      pByName = MAKEPTR_OS38( PIMAGE_IMPORT_BY_NAME,
        hModule                  ,
        pOrigThunk->u1.AddressOfData );
      
      /* If the name starts with NULL, then just skip out now. */
      if ( '\0' == pByName->Name[ 0 ] ) {
        continue ;
      }
      
      /* Determines if the hook will be done. */
      BOOL bDoHook = FALSE ;
      
      for ( tsp00_Uint4 i = 0 ; i < uiCount ; i++ ) {
        if (( paHookArray[i].pFuncName[0] ==
          pByName->Name[0] ) &&
          ( 0 == strcmpi ( paHookArray[i].pFuncName ,
          (char*)pByName->Name ))) {
          /* If the proc is NULL, kick out, otherwise go
           *  ahead and hook it. */
          if ( NULL != paHookArray[ i ].pProcAddr ) {
            bDoHook = TRUE ;
          }
          break ;
        }
      }
      
      if ( TRUE == bDoHook )
      {
        /* Found ! Now  change the protection to
        *  writable before doing the blast.  Note, 
        *  blasting is done now into the real thunk area! */
        MEMORY_BASIC_INFORMATION mbi_thunk ;
        
        VirtualQuery( pRealThunk,
          &mbi_thunk,
          sizeof( MEMORY_BASIC_INFORMATION ));
        
        if ( 0 == VirtualProtect ( mbi_thunk.BaseAddress ,
          mbi_thunk.RegionSize,
          PAGE_READWRITE,
          &mbi_thunk.Protect )) {
          return ( change_protection_failed_eo38 );
        };
        
        /* Save the original address if requested. */
        if ( NULL != paOrigFuncs ) {
          paOrigFuncs[i] = (PROC)pRealThunk->u1.Function;
        }

#if defined (_WIN64)
        pRealThunk->u1.Function = (LONGLONG)paHookArray[i].pProcAddr;
#else
        pRealThunk->u1.Function = (PDWORD)paHookArray[i].pProcAddr;
#endif

        DWORD dwOldProtect ;
        
        /* Change the protection back to what it was before */
        if ( 0 == VirtualProtect ( mbi_thunk.BaseAddress ,
          mbi_thunk.RegionSize,
          mbi_thunk.Protect,
          &dwOldProtect )) {
          return ( change_protection_failed_eo38 );
        };;
        
        *puiHooked += 1; /* Increment the total number hooked. */
      }
    }
    pOrigThunk++ ;
    pRealThunk++ ;
  }

  return ( success_eo38 );
}

/*---------------------------------------------------------------------------*/

static bool isHookDll(char* moduleName) {
  static const char* aDontHookDlls[] = { 
    "NTDLL", 
    "USER32", 
    "KERNEL32", 
    "GDI32",
    "ADVAPI",
    "RPCRT4",
    "SHELL32",
    "COMCTL32",
    "OLE32",
    "RPCLTC1",
    "SQLTCP",
    "WSOCK32",
    "WS2_32",
    "MSVCRT",
    "WS2HELP",
    "RASADHLP",
    "MSVCP60",
    "MFC42",
    "DBPINSTALL",
    "LIBOMS",
    "LIVECACHEDBG"
  };

  for ( int i=0; i < (sizeof(aDontHookDlls) / sizeof(char*)); i++ ) {
    if ( NULL != strstr(moduleName,aDontHookDlls[i] )) {
      return ( false );
    }
  }

  return ( true );
}

/*---------------------------------------------------------------------------*/

static int sqlHookCrtIOFunctions(){
  tsp00_Uint4 uiCount;
  tos38_HookResult HookRc;
  OSVERSIONINFO stOSVI ;
  BOOL          bRet;
  int i = 0;

  g_aToHookIOFuncDsc_os38[i].pFuncName = "_creat" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_creatHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wcreat" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wcreatHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_open_osfhandle" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_open_osfhandleHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "fopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)fopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wfopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wfopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_fdopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_fdopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wfopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wfopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "freopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)freopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wfreopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wfreopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_open" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_openHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wopenHook ;
  
  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_sopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_sopenHook ;
  
  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wsopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wsopenHook ;
  
  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_fsopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_fsopenHook ;
  
  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_wfsopen" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_wfsopenHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "fclose" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)fcloseHook ;

  ++i;
  g_aToHookIOFuncDsc_os38[i].pFuncName = "_fcloseall" ;
  g_aToHookIOFuncDsc_os38[i].pProcAddr = (tos38_WinProcPtr)_fcloseallHook ;
  
  /* Check Version */
  memset (&stOSVI, NULL, sizeof (OSVERSIONINFO)) ;
  stOSVI.dwOSVersionInfoSize = sizeof (OSVERSIONINFO) ;
  bRet = GetVersionEx ( &stOSVI ) ;
  if (FALSE == bRet) return(0);
  if ((VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId) &&
    (4 == stOSVI.dwMajorVersion))
  {
    MEMORY_BASIC_INFORMATION   vmq;
    MEMORY_BASIC_INFORMATION   vmq2;
    char          szBuff[MAX_PATH];
    void*         pvAddress = 0x00000000;
    void*         pvAddress2;
    bool          fok = true;
    unsigned long len;
    
    while(fok)
    {
      fok = ( VirtualQuery(pvAddress, &vmq, sizeof(vmq)) == sizeof(vmq));
      if (fok)
      {
        pvAddress2 = ((BYTE *) vmq.AllocationBase + vmq.RegionSize);
        
        fok = ( VirtualQuery(pvAddress2, &vmq2, sizeof(vmq2)) == sizeof(vmq2));
        while ( fok && (( vmq2.AllocationBase == vmq.AllocationBase ) || (vmq2.AllocationBase ==0)))
        {
          vmq.RegionSize = vmq.RegionSize + vmq2.RegionSize;
          pvAddress2 = ((BYTE *) vmq2.BaseAddress + vmq2.RegionSize);
          fok = ( VirtualQuery(pvAddress2, &vmq2, sizeof(vmq2)) == sizeof(vmq2));
        }
        
        // check if exception address lies within region
        if (( vmq.AllocationBase != NULL ) && ( vmq.AllocationBase == vmq.BaseAddress ))
        {
          len = GetModuleFileName((HINSTANCE) vmq.AllocationBase, &szBuff[0], sizeof(szBuff));
          if ( (0 < len) && (isHookDll(strupr(&szBuff[0])) ))
          {
              HookRc =  sql38HookImportedFunctionsByName ( 
              (tsp00_Void *)vmq.AllocationBase,
              "MSVCRT.DLL",
              cuiToHookFuncCnt,
              &g_aToHookIOFuncDsc_os38[0],
              NULL,
              &uiCount);
            if ( success_eo38 != HookRc ) {
              if ( module_not_imported_eo38 == HookRc ) {
                /* try debug version */
                HookRc =  sql38HookImportedFunctionsByName ( 
                  (tsp00_Void *)vmq.AllocationBase,
                  "MSVCRTD.DLL",
                  cuiToHookFuncCnt,
                  &g_aToHookIOFuncDsc_os38[0],
                  NULL,
                  &uiCount);
              }
            }
          }  
        }
        // Get address of next area to be examined
        pvAddress = ((BYTE *) vmq.BaseAddress + vmq.RegionSize);
      }
    }
  }


  if ( success_eo38 != HookRc ) {
    return ( 0 );
  }
  else {
    return ( 1 );
  };  
};

#endif
/* ---------------- PTS 1112243 ----------------- */
