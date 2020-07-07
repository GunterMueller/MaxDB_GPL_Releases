/*!
  @file           heo670.h
  @author         JoergM, RaymondR
  @ingroup        Stack Back Trace
  @brief          Interface that allows to produce a stack backtrace
  @see            example.html ...

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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



#ifndef HEO670_H
#define HEO670_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"

#ifdef   _WIN32
#include "RunTime/System/RTESys_MSWindows.h" /*nocheck*/
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!
   @brief   Address information used to to find text symbol for given address

    The eo670AddressInfo is filled in the following sequence

   @code
    void *callArray[MAX_CALL_ARRAY];
    void *context;
    SAPDB_Int4 depth;
    SAPDB_Int4 loopIndex;

    depth = eo670_FillCallArray(MAX_CALL_ARRAY, callArray);

    if ( depth > 0 
      && eo670_GetAddressInfoProlog(&context) )
    {
      for ( loopIndex = 0; loopIndex < depth; loopIndex++ )
      {
          eo670AddressInfo localAddrInfo;

          if ( eo670_GetAddressInfo(callArray[loopIndex],
                                    &localAddrInfo,
                                    context) )
          {
              ShowAddressInfo(loopIndex, callArray[loopIndex], &localAddrInfo); see below

              eo670_FreeAddressInfo(&localAddrInfo);
          }
      }
      eo670_GetAddressInfoEpilog(context);
    }
   @endcode


    First fill the callArray. This can be done at any time before, but notice that symbols of unloaded
    dynamic loaded libraries maybe no longer available... (should not be a problem for SAPDB, which does
    no dynamic unload...).

    Now the symbol information is taken frame by a prologue and epilogue function.
    Each addressInfo is retrieved, and after usage freed. If you forget eo670_FreeAddressInfo you produce a
    significant memory leak....

    Showing the address info should use such a coding

  @code
    void ShowAddressInfo(SAPDB_Int4 loopIndex, void *pcAddr, eo670AddressInfo *pAddrInfo)
    {
      if ( pAddrInfo->offsetToSymbol < 0 )
      {
        printf("%d: 0x%lx %s\n",
              loopIndex,
              (SAPDB_Long)((char *)pcAddr-(char *)0),
              pAddrInfo->symbol);
      }
      else
      {
        printf("%d: 0x%lx %s + 0x%x\n",
              loopIndex,
              (SAPDB_Long)((char *)pcAddr-(char *)0), 
              (0 != *pAddrInfo->demangledSymbol) ? pAddrInfo->demangledSymbol : pAddrInfo->symbol, 
              pAddrInfo->offsetToSymbol );

        if ( pAddrInfo->lineNumber >= 0 )
        {
          printf(" %s - %s:%d\n",
              pAddrInfo->moduleName,
              pAddrInfo->sourceFileName,
              pAddrInfo->lineNumber );
        }
        else if ( 0 != *pAddrInfo->sourceFileName )
        {
          printf(" %s - %s\n",
              pAddrInfo->moduleName,
              pAddrInfo->sourceFileName );
        }
        else if ( 0 != *pAddrInfo->moduleName )
        {
          printf(" %s - %s\n",
              pAddrInfo->moduleName );
        }
      }
    }
   @endcode

 */
typedef struct eo670AddressInfo_
{
    SAPDB_Char *symbol;          /*!< the undemangled symbol or the reason why it was not able to get it */
    SAPDB_Char *demangledSymbol; /*!< the demangled symbol or "" if no demangled symbol available        */
    SAPDB_Char *moduleName;      /*!< the module where the symbol was found or "" if not available       */
    SAPDB_Char *sourceFileName;  /*!< the source file name of the file defining the symbol or "" if NA   */
    SAPDB_Long offsetToSymbol;   /*!< the offset from symbol address to given pcAddr or -1 if NA         */
    SAPDB_Long lineNumber;       /*!< the line number inside the source file or -1 if NA                 */
} eo670AddressInfo;


#ifdef WIN32

typedef struct eo670TraceContext_
{
    HANDLE      threadHandle;     /*!< handle to the thread for which the stack trace has to be generated  */
    CONTEXT*    pThreadContext;   /*!< contains processor-specific register data snapshot of the thread */
} eo670TraceContext, * eo670TraceContextPtr;

#else /* UNIX */

typedef void*   eo670TraceContextPtr;

#endif


/*!
   @brief          Pointer type of callback functions used to write on stack back trace 
                   output channel.
   @param          str         [in] stack backtrace string to write
   @param          pOutContext [in] caller specific ouput context information (e.g. object pointer)
   @return         0 means no error

 */
typedef void (* OutputCallBackFuncPtr )( char const * const str, 
                                         void * const       pOutContext ); /* Callback Function type for */

/*!
   @brief          Initialize lock used to serialize tracebacks
 */
externC void eo670_CTraceStackInit   ( );
/*!
   @brief          Initialize lock used to serialize tracebacks
   @param          pOutput     [in] stack backtrace write callback
   @param          pOutContext [in] caller specific ouput context information (e.g. object pointer)
 */
externC void eo670_CTraceStackInitOCB( OutputCallBackFuncPtr pOutput,    /* with ouput callback func.*/
                                       void * const          pOutContext); 

/*!
   @brief          Create a stack backtrace on DEFAULT stack back trace output channel


  This call is usefull for debugging 'crashing' code that already detected a severe problem.
  It does not use 'normal' memory allocation and tries to prevent anything else, that could
  lead to inflictions with other modules.
  The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
  'eo670_CTraceStackOCB' is used which allows to define an own output channel via callback 
  function.
  The amount of output visible depends on optimization level used. At least module overlapping
  call sequences are visible, that means all inner module calls must not necessarily be visibile.
  Be carefull to keep this in mind while interpreting the stack backtrace!

 */
externC void eo670_CTraceStack   ( void );
/*!
   @brief          Create a stack backtrace on stdout into the user given stack back trace output channel

   This call is usefull for debugging 'crashing' code that already detected a severe problem.
   It does not use 'normal' memory allocation and tries to prevent anything else, that could
   lead to inflictions with other modules.
   The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
   'eo670_CTraceStackOCB' is used which allows to define an own output channel via callback 
   function.
   The amount of output visible depends on optimization level used. At least module overlapping
   call sequences are visible, that means all inner module calls must not necessarily be visibile.
   Be carefull to keep this in mind while interpreting the stack backtrace!

   @param pOutput [in] output callback
   @param pOutContext [in] output callback context
 */
externC void eo670_CTraceStackOCB( OutputCallBackFuncPtr pOutput,
                                   void * const          pOutContext );
/*!
   @brief          Create a stack backtrace from pascal

  This call is usefull for debugging 'crashing' code that already detected a severe problem.
  It does not use 'normal' memory allocation and tries to prevent anything else, that could
  lead to inflictions with other modules.
  The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
  'eo670_CTraceStackOCB' is used which allows to define an own output channel via callback 
  function.
  The amount of output visible depends on optimization level used. At least module overlapping
  call sequences are visible, that means all inner module calls must not necessarily be visibile.
  Be carefull to keep this in mind while interpreting the stack backtrace!
 */
externC void vtracestack         ( void ); /* alias for PASCAL sources */


/*!
   @brief          Update symbol information

   This function should be called after a DLL/shared library has been loaded. 
   This is needed to update the internal symbol information for subsequent 
   'FillCallArray' operations.

   @return         true if operation was successful, false if not
 */
externC SAPDB_Bool eo670_UpdateSymbols();

/*!
   @brief          Update symbol information

   This function should be called after a DLL/shared library has been loaded. 
   This is needed to update the internal symbol information for subsequent 
   'FillCallArray' operations.

   The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
   'pOutput' is used which allows to define an own output channel via callback 
   function.

   @param    pOutput     [in] output callback
   @param    pOutContext [in] output callback context

   @return         true if operation was successful, false if not
 */
externC SAPDB_Bool eo670_UpdateSymbolsOCB( OutputCallBackFuncPtr pOutput,
                                           void * const          pOutContext );

/*!
   @brief          Fill the given array with at most level trace back entries.

   Supported on most platforms (except IA64 + HPUX 32 where always 0 is returned)
   @param          level      [in] the number of level to fill
   @param          callArray [out] the array to fill
   @return         The actual number of entries filled
 */
externC SAPDB_Int4 eo670_FillCallArray(SAPDB_Int4  level,
                                       void**      callArray);

/*!
   @brief          Get mapping information about a given PC address


  Supported on most platforms (except on HPUX 32 + 64 where a dummy address info is filled)
   @param          pcAddr           [in] a hopefully valid program counter
   @param          pAddrInfo       [out] structure to be filled with address information
   @param          pAddrInfoContext [in] context received by eo670_GetAddressInfoProlog
   @return         true if mapping returns valid data, false if not

 */

externC SAPDB_Bool eo670_GetAddressInfo(void*             pcAddr,
                                        eo670AddressInfo* pAddrInfo,
                                        void*             pAddrInfoContext);

/*!
   @brief          finishes a sequences of eo670_GetAddressInfo calls and
                   release all resources hold by eo670_GetAddressInfoProlog.
   @param          pAddrInfoContext [in] pointer returned by eo670_GetAddressInfoProlog
   @return         none

 */


externC void eo670_GetAddressInfoEpilog (void* pAddrInfoContext);  

/*!
   @brief          prepares for following calls of eo670_GetAddressInfo
   @param          pAddrInfoContext [out] pointer to context required for eo670_GetAddressInfo.                may be null on several plattforms
   @return         true if mapping returns valid data, false if not

 */

externC SAPDB_Bool eo670_GetAddressInfoProlog (void** pAddrInfoContext);  

/*!
   @brief          Free mapping information


  Necessary call to prevent memory leaks
   @param          pAddrInfo [in] structure filled by a previous call to eo670_GetAddressInfo

 */

externC void eo670_FreeAddressInfo(eo670AddressInfo* pAddrInfo);

/*!
   @brief          Create a stack backtrace using given context


  This call is usefull for debugging 'crashing' code that already detected a severe problem.
  It does not use 'normal' memory allocation and tries to prevent anything else, that could
  lead to inflictions with other modules.
  The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
  'eo670_CTraceContextStackOCB' is used which allows to define an own output channel 
  via callback function.
  The amount of output visible depends on optimization level used. At least module overlapping
  call sequences are visible, that means all inner module calls must not necessarily be visibile.
  Be carefull to keep this in mind while interpreting the stack backtrace!
  The given context is retrieved as third argument of a sigaction handler on UNIX and as
  exception context on NT.
  If 0 the current context is retrieved.
   @param          pBTraceContext [in] a struct sigcontext * on UNIX

 */

externC void eo670_CTraceContextStack   ( eo670TraceContextPtr    pBTraceContext );
/*!
  @brief          Create a stack backtrace using given context into the user given stack back trace output channel


  This call is usefull for debugging 'crashing' code that already detected a severe problem.
  It does not use 'normal' memory allocation and tries to prevent anything else, that could
  lead to inflictions with other modules.
  The stack backtrace is normally visible in 'knldiag' if compiled for kernel unless 
  'eo670_CTraceContextStackOCB' is used which allows to define an own output channel 
  via callback function.
  The amount of output visible depends on optimization level used. At least module overlapping
  call sequences are visible, that means all inner module calls must not necessarily be visibile.
  Be carefull to keep this in mind while interpreting the stack backtrace!
  The given context is retrieved as third argument of a sigaction handler on UNIX and as
  exception context on NT.
  If 0 the current context is retrieved.

  @param          pBTraceContext [in] a struct sigcontext * on UNIX
  @param pOutput [in] output callback
  @param pOutContext [in] output callback context
 */
externC void eo670_CTraceContextStackOCB( eo670TraceContextPtr  pBTraceContext,
                                          OutputCallBackFuncPtr pOutput,    /* with ouput callback func.*/
                                          void * const          pOutContext); 

#ifdef WIN32
/*!
  @brief          Set path to dbghelp.dll

  @param path [in] - base path to the dbghelp.dll
 */
externC bool eo670_SetDbgHelpPath ( const char* path );


/*!
  @brief          Write a minidump (equivalent to UNIX core dump)

  @param threadId[in] thread id throwing the exception
  @param pExcPtrs[in] pointer to the exception context          
  @param pszFile [in] dump file name
  @param WriteFullMemoryDump [in] write a full memory dump
  @param IfDumpFileExists [in] write dump only if dump file exists
 */
externC bool eo670_WriteMiniDump ( DWORD                      threadId,
                                   LPEXCEPTION_POINTERS       pExcPtrs,
                                   const char*                pszFile,
                                   bool                       WriteFullMemoryDump,
                                   bool                       IfDumpFileExists,
                                   PSECURITY_ATTRIBUTES       pSA );
/*!
  @brief          Write a minidump (equivalent to UNIX core dump)

  @param threadId[in] thread id throwing the exception
  @param pExcPtrs[in] pointer to the exception context          
  @param pszFile [in] dump file name
  @param WriteFullMemoryDump [in] write a full memory dump
  @param IfDumpFileExists [in] write dump only if dump file exists
  @param pOutput [in] output callback
  @param pOutContext [in] output callback context
 */
externC bool eo670_WriteMiniDumpOCB ( DWORD                      threadId,
                                      LPEXCEPTION_POINTERS       pExcPtrs,
                                      const char*                pszFile,
                                      bool                       WriteFullMemoryDump,
                                      bool                       IfDumpFileExists,
                                      OutputCallBackFuncPtr      pOutput,    /* with ouput callback func.*/
                                      void * const               pOutContext,
                                      PSECURITY_ATTRIBUTES       pSA );

#endif

/*!
   @brief          Write a string into the DEFAULT stack back trace output channel.
   @param          str [in] string to write
 */
externC void eo670WriteString   (const char* str);
/*!
   @brief          Write a string into the user given stack back trace output channel.
   @param str [in] string to write
   @param pOutput [in] output callback
   @param pOutContext [in] output callback context
 */
externC void eo670WriteStringOCB(const char*           str,
                                 OutputCallBackFuncPtr pOutput, /* with ouput callback func.*/
                                 void * const          pOutContext); 

/*!
   @brief Set default output callback function and context used for calls, that do not specify it
   @param pOutput [in] output callback
   @param pOutContext [in] output callback context
 */
externC void eo670_SetDefaultOutputCallback( OutputCallBackFuncPtr pOutput
                                           , void * const pOutContext);

#endif  /* HEO670_H */
