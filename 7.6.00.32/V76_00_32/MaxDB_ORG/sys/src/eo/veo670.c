/*!
  @file           veo670.c
  @author         JoergM, RaymondR
  @special area   Stack Back Trace
  @brief          Interface that allows to produce a stack backtrace

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/* PTS 1108958 */
#undef _FILE_OFFSET_BITS
#include "heo670.h"
#include "gsp00.h"
#include "hsp77.h" /* PTS 1110802 */
#include "geo60.h"
#include "RunTime/System/RTESys_Spinlock.h"
#include "SAPDBCommon/SAPDB_string.h"
/*!
  On Platforms supporting stack backtrace this defined is undefined later....
 */
#define HAS_NO_STACKBACKTRACE_IMPLEMENTATION

#define NO_CALLBACK     (OutputCallBackFuncPtr)0
#define NO_DISPLACEMENT (void*)0


#if defined(KERNEL_LZU)
#  undef WANT_DEMANGLING_SUPPORT
#  define WANT_DEMANGLING_SUPPORT
#  include "heo01.h" /* nocheck */
#  include "geo50_0.h" /* nocheck */
#  include "geo002.h" /* nocheck */
#  include "heo51.h" /* nocheck */
#  include "RunTime/Diagnose/RTEDiag_SymbolResolutionLock.h" /* nocheck */
#endif

#if defined(WANT_DEMANGLING_SUPPORT)
#  if defined(SUN) || defined(SOLARIS) || defined(AIX)
/* only needed if C++ demangling is needed */
#    include <demangle.h> /* PTS 1110802 */
#  endif /* SOLARIS */
#endif /* WANT_DEMANGLING_SUPPORT */

#include <stdio.h>

#if defined(_WIN32)

#  undef HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#  define FILL_SKIP_COUNT 1
#  if defined (_WIN64)
#    if !defined (BIT64)
#      define BIT64
#    endif
#  else
#    define HAS_FAST_FILL_CALL_ARRAY
#    undef BIT64
#  endif

/*WIN*/
#  include <windows.h>
#  if WINVER > 0x0500
#    include <dbghelp.h>
#  else
#    include <imagehlp.h>
#  endif
/*WIN*/

#else /* only on UNIX */

/*UNIX*/
#  include <unistd.h>
#  include <stdlib.h>
#  include <errno.h>
#  include <string.h>
#  include <signal.h>
#  include <setjmp.h>
/*UNIX*/

/*OSF1*/
#  if defined(OSF1)
#    undef HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#    define HAS_FAST_FILL_CALL_ARRAY
#    define FILL_SKIP_COUNT 2
#    include <loader.h>
#    include <excpt.h>
#    include <ldfcn.h>
#  endif /* OSF1 */
/*OSF1*/

/*FREEBSD*/
#  ifdef FREEBSD
#    define HAS_FAST_FILL_CALL_ARRAY
#    define FILL_SKIP_COUNT 1
#    include <sys/types.h>
#    include <fcntl.h>
#    include <elf.h>
#    include <sys/file.h>
#    include <sys/mman.h>
#    include <sys/param.h>
#    include <sys/stat.h>
#    include <sys/ucontext.h>

enum			// RIPPED from LINUX ucontext for linuxthread compatibility in signal handling
{
  REG_GS = 0,
# define REG_GS         REG_GS
  REG_FS,
# define REG_FS         REG_FS
  REG_ES,
# define REG_ES         REG_ES
  REG_DS,
# define REG_DS         REG_DS
  REG_EDI,
# define REG_EDI        REG_EDI
  REG_ESI,
# define REG_ESI        REG_ESI
  REG_EBP,
# define REG_EBP        REG_EBP
  REG_ESP,
# define REG_ESP        REG_ESP
  REG_EBX,
# define REG_EBX        REG_EBX
  REG_EDX,
# define REG_EDX        REG_EDX
  REG_ECX,
# define REG_ECX        REG_ECX
  REG_EAX,
# define REG_EAX        REG_EAX
  REG_TRAPNO,
# define REG_TRAPNO     REG_TRAPNO
  REG_ERR,
# define REG_ERR        REG_ERR
  REG_EIP,
# define REG_EIP        REG_EIP
  REG_CS,
# define REG_CS         REG_CS
  REG_EFL,
# define REG_EFL        REG_EFL
  REG_UESP,
# define REG_UESP       REG_UESP
  REG_SS
};

#  endif
/*FREEBSD*/


/*LINUX*/
#  if defined(LINUX) && !defined(S390X)

#    undef  HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#    define HAS_FAST_FILL_CALL_ARRAY

#    if defined(IA64) && defined(HAS_NPTL)
/* current implementation of sigsetjmp() in SLES9 glibc produces stack framing that does not allow stack backtrace... */
/* This define disables that coding */
/* #       define HAS_SETJMP_BUG 1 */
#    endif

#    if defined(IA64) || defined(HAS_EXTERN_LIBUNWIND)
/* 10.5.2004 jrg
   Reenabled! Stripped libunwind-0.96 of all modules that confused libstd++... */
#      define HAS_LIBUNWIND_IMPLEMENTATION
#      define FILL_SKIP_COUNT 1
#      include <sys/types.h>
#      include <fcntl.h>
#      include <elf.h>
#      include <sys/file.h>
#      include <sys/procfs.h>
#    elif defined(I386)
#      define FILL_SKIP_COUNT 1
#      include <sys/types.h>
#      include <fcntl.h>
#      include <elf.h>
#      include <sys/file.h>
#      include <sys/procfs.h>
/* PTS 1106738 */
/* With RedHat6.2 the mmap() prototype was fixed. This prevented the kernel from */
/* starting. The message given was 'invalid argument'. This Message was given due */
/* to the last argument of mmap() which was defined as 'off_t' and later 'off64_t' */
/* So on RedHat6.2 the last argument was expected to be a 64bit value, therefore */
/* the extra 32bits are collected from the stack. Such an offset is not PAGESIZE */
/* aligned, so it leads to 'invalid argument'... */
#      undef __USE_FILE_OFFSET64
#      define __USE_LARGEFILE64
#      include <sys/mman.h>
#      include <sys/param.h>
#      include <sys/stat.h>
#      include <linux/version.h>
/*
  LINUX_VERSION_CODE 132099 maybe to high, but the definite number is not known to me....

  This code is able to produce a stack backtrace even on these old LINUX machines we are currently
  using (Kernel 2.2.11-sap2_serial_sysrq). We have another test machine with a Kernel 2.4.4.
  From this newer machine i took the LINUX_VERSION_CODE. The reason for this effort is that
  even the gdb dumps a core on the old linux machine while trying to load our kernel....
  I took the inline assembler to get the current frame pointer and analyzed the stack frame
  with a memory dump (optimized and debug code). This way i found the offset for the 'sigcontext'
  and put it into the magic number below... I am not happy with this, but it is an old linux
  kernel anyhow... 6.2.2002 jrg

 */
#      if LINUX_VERSION_CODE < 132099
#        define LINUX_WITHOUT_SIGINFO
/* #    include <sys/ucontext.h> can not be used due to conflict with <procfs.h> ... */
/* "asm/sigcontext" and "sys/ucontext" contain the same structure with different naming conventions... */
#        include <asm/sigcontext.h>
#      else
#        undef LINUX_WITHOUT_SIGINFO
#        define __USE_GNU
#        include <sys/ucontext.h>
#      endif
#    elif defined(LINUX) && defined(X86_64) && !defined(HAS_LIBUNWIND_IMPLEMENTATION)
#      define FILL_SKIP_COUNT 2
#      include <sys/types.h>
#      define __USE_GNU
#      include <sys/ucontext.h>
#      include <fcntl.h>
#      include <elf.h>
#      include <sys/file.h>
#      include <sys/procfs.h>
#    elif defined(LINUX) && defined(SDBonPPC64)
#      define FILL_SKIP_COUNT 2
#      include <sys/types.h>
#      include <fcntl.h>
#      include <elf.h>
#      include <sys/file.h>
#      include <sys/procfs.h>
#    endif /* LINUX && SDBonPPC64 */
#  endif /* LINUX */
/*LINUX*/

/*SOLARIS*/
#  if defined(SOLARIS) || defined(SUN)
#    undef HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#    define HAS_FAST_FILL_CALL_ARRAY
#    define FILL_SKIP_COUNT 1
#    include <sys/types.h>
#    include <fcntl.h>
#    include <libelf.h>
#    include <sys/frame.h>
#    include <ucontext.h>
#    include <sys/stack.h>
#    include <sys/file.h>
#    include <procfs.h> /* this is the new procfs interface, not based on ioctl!!!! */
#    include <sys/mman.h>
#    include <sys/param.h>
#    include <sys/stat.h>
#  endif /* SOLARIS */
/*SOLARIS*/

/*AIX*/
#  ifdef AIX
#    undef HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#    define HAS_FAST_FILL_CALL_ARRAY
#    define FILL_SKIP_COUNT 2
#    include <sys/debug.h>
#    include <sys/ldr.h>
#  endif /* AIX */
/*AIX*/

/*HPUX*/
#  if defined(HPUX)
#    undef HAS_NO_STACKBACKTRACE_IMPLEMENTATION
#    define HAS_FAST_FILL_CALL_ARRAY
#    define FILL_SKIP_COUNT 1
#    include <dl.h>
#    include <fcntl.h>
#    ifdef BIT64
#      include <elf.h>
#    else
#      include <a.out.h>
#    endif
#    ifdef HP_IA64
#      include <dlfcn.h>
#      include <uwx.h>
#      include <uwx_self.h>
#    endif
#  endif /* BIT64 */
/*HPUX*/

#endif /* only on UNIX */

#ifdef HAS_LIBUNWIND_IMPLEMENTATION
/* This define selects '_UL... function from libunwind-ia64.a! */
/* In addition it selects the 'local version' of unwinding routines which are faster */
#  define UNW_LOCAL_ONLY
#  ifdef HAS_EXTERN_LIBUNWIND
#    include <extern/include/libunwind.h> /* nocheck */
#  else
#    include "libunwind.h" /* nocheck */
#  endif
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define MESSAGE_TEXT_LIMIT  115
#define MAX_SYMBOL_LENGTH 4096

#if defined(_WIN32)
#  define EXCEPT_GEN_CALL_STACK         0xE000BAFF
#  define EXCEPT_FILL_CALL_ARRAY        0xE000BAFE
/* Define common message id used for all output messages */
#  define MSG_ID 19999
#  define NONE_VERBOSE_CALL_STACK    0
#  define VERBOSE_CALL_STACK         1

#  define MAXSYMNAMELEN       2048 // max name length for found symbols
#  define TTBUFLEN            65536 // for a temp buffer
#  define MAX_MODULE_NAME32   255
#  define TH32CS_SNAPMODULE   0x00000008
#  define MINI_DMP_TEMP_FILE  "~minidmp.tmp"
#else
/* Define common message id used for all output messages */
#  define MSG_ID 11599
#endif /* _WIN32 */

/* Define maximum size of all module list informations */
#define SIZE_OF_MODULE_INFO_BUFFER_EO670 (64*1024)

/* Define maximum size of single output line */
#define SIZE_OF_MESSAGE_BUFFER_EO670 (4096)
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#if defined(_WIN32)
#  define GIVEUP_TIME_SLICE Sleep(0)
#else
#  define GIVEUP_TIME_SLICE sleep(0)
#endif

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

#if defined (_WIN32)
typedef struct eo670_CallStackWorkerStruct_
{
  HANDLE                threadHandle;
  OutputCallBackFuncPtr pOutput;
  void *                pOutContext;
  CONTEXT *             pThreadContext;
} eo670_CallStackWorkerStruct;

typedef struct eo670_MiniDumpWorkerStruct_
{
  HANDLE                threadHandle;
  DWORD                 threadId;
  LPEXCEPTION_POINTERS  pExcPtrs;
  const char*           pszFile;
  bool                  WriteFullMemory;
  bool                  IfDumpFileExists;
  OutputCallBackFuncPtr pOutput;
  void *                pOutContext;
  PSECURITY_ATTRIBUTES  pSA;
} eo670_MiniDumpWorkerStruct;

typedef struct eo670_GetAddressInfoContext_
{
    HANDLE                m_handle;
} eo670_GetAddressInfoContext;

#else

#  if defined(SOLARIS) || defined(SUN)
typedef struct
{
    void *pcAddr;
    void *frame;
#     if defined(SPARC) || defined(SUN)
    unsigned long arg[6];
#     endif
} eo670_CallInfo;
#     if defined(SPARC) || defined(SUN)
#undef HAS_NO_ADDITIONAL_FRAME_INFO
#     else
#define HAS_NO_ADDITIONAL_FRAME_INFO
#     endif

#  elif defined(LINUX) && (defined(I386))
/* #  elif defined(LINUX) && (defined(I386) || defined(X86_64)) */
typedef struct
{
    void *pcAddr;
    void *frame;
    unsigned long arg[4];
} eo670_CallInfo;
#undef HAS_NO_ADDITIONAL_FRAME_INFO
#  else /* if arguments are not found in frame, at least show frame pointer */
typedef struct
{
    void *pcAddr;
    void *frame;
} eo670_CallInfo;
#define HAS_NO_ADDITIONAL_FRAME_INFO
#  endif /* !SOLARIS */

#endif /* _WIN32 */

typedef struct eo670_ModuleChain_
{
    struct eo670_ModuleChain_ *next;
    char *file_name;
    void *text_mapaddr;
    void *text_endaddr;
    void *osDependend;
    SAPDB_ULong osDependendSize;
} eo670_ModuleChain;

/*===========================================================================*
 *  STATIC VARIABLES                                                         *
 *===========================================================================*/

/* Flag to notice first call ever */
static volatile int eo670_TraceEverCalled = 0;

#if !defined(KERNEL_LZU)
/* Spinlock used to interlock stack backtrace requests */
static RTE_Lock eo670_inTraceStack;
#endif

/* Anchor for list of loaded modules */
static eo670_ModuleChain *eo670_ModuleAnchor = 0;
static eo670_ModuleChain *eo670_LastModuleEntry = 0;

/* A 'lot of' space used to store loaded module infos (not including symbol tables...) */
static char eo670_ModuleInfoBuffer[SIZE_OF_MODULE_INFO_BUFFER_EO670];

/* Common Message buffer */
static char eo670MessageBuffer[SIZE_OF_MESSAGE_BUFFER_EO670];

/* Default output callback and context */
static OutputCallBackFuncPtr pDefaultOutput = NO_CALLBACK;
static void *                pDefaultOutContext = (void*)0;


/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/

/*ALL*/static void eo670_OsFreeAddressInfo(eo670AddressInfo *pAddrInfo);

#ifdef WIN32
/*WIN*/static void       eo670_WindowsTraceStack       ( eo670TraceContextPtr  pBTraceContext,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static void       eo670_WinShowStack            ( HANDLE                hThread,
/*WIN*/                                                  CONTEXT *             pBTraceContext,
/*WIN*/                                                  int                   VerbosityLevel,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static void       eo670_WinFillCallArray        ( SAPDB_Int4            givenMaxLevel,
/*WIN*/                                                  void **               callArray,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext,
/*WIN*/                                                  SAPDB_Int4 * const    pDepth );
/*WIN*/static void       eo670_WinGetAddressInfoEpilog ( void*                 pAddrInfoContext,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static SAPDB_Bool eo670_WinGetAddressInfoProlog ( void**                pAddrInfoContext,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static SAPDB_Bool eo670_WinGetAddressInfo       ( void*                 pcAddr,
/*WIN*/                                                  eo670AddressInfo *    pAddrInfo,
/*WIN*/                                                  void*                 pAddrInfoContext,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static bool       eo670_WinFillModuleList       ( DWORD                 pid,
/*WIN*/                                                  HANDLE                hProcess,
/*WIN*/                                                  int                   VerbosityLevel,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static void       eo670_WinRegisterDump         ( CONTEXT*              pBTraceContext,
/*WIN*/                                                  OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static char const * const eo670_WinSymSearchPath( OutputCallBackFuncPtr pOutput,
/*WIN*/                                                  void * const          pOutContext );
/*WIN*/static SAPDB_Bool eo670_WinUpdateSymbols        ( OutputCallBackFuncPtr pOutput,
/*WIN*/static                                             void * const          pOutContext );
/*WIN*/static DWORD WINAPI eo670_CallStackWorker       ( eo670_CallStackWorkerStruct* workerParam );
/*WIN*/static DWORD WINAPI eo670_MiniDumpWorker        ( eo670_MiniDumpWorkerStruct* workerParam );
/*WIN*/static bool eo670_WriteMiniDumpToFile           ( DWORD                      threadId,
/*WIN*/                                                 LPEXCEPTION_POINTERS       pExcPtrs,
/*WIN*/                                                 const char*                pszFile,
/*WIN*/                                                 bool                       WriteFullMemory,
/*WIN*/                                                 bool                       IfDumpFileExists,
/*WIN*/                                                 OutputCallBackFuncPtr      pOutput,    /* with ouput callback func.*/
/*WIN*/                                                 void * const               pOutContext,
/*WIN*/                                                 PSECURITY_ATTRIBUTES       pSA );

#else
/*
   Function: eo670_UnixTraceStack
   Description: This routine controls the several steps of stack back trace creation

   It set up signal handler for SIGSEGV and SIGBUS usings siglongjmp() to protect as much as possible.

   The stack back trace creation consists of

    Creating Emergency Dump (non symbolic without any heap usage except for static data)
    Creating Regioster Dump (optional if pBTraceContext != 0 that is in signal context
    Creating Loaded Module List
    Creating Symbolic Stack Back Trace
 */
/*UNIX*/void eo670_UnixTraceStack( void*                 pBTraceContext,
                                   OutputCallBackFuncPtr pOutput,
                                   void * const          pOutContext );

/*
   Function: eo670_UpdateModuleChain
   Description: Fill the module chain with loaded module information

   Internally eo670_AddToLoadedModuleChain() is used to actually fill. This routine is responsible
   for gathering text segment start and end address and the module name.

    NOTE: To be implemented individually for each platform...
 */
/*UNIX*/static SAPDB_Bool eo670_UpdateModuleChain ( OutputCallBackFuncPtr pOutput,
                                                    void * const          pOutContext );

/*
   Function: eo670_FillAddressInfo
   Description: Fill the address info using given program counter

   The given address info is cleared and filled. The entries will point to static storage afterwards,
   so caller is responsible to copy data if needed.

   NOTE To be implemented individually for each platform...

   Arguments: pcAddr   [in] the program counter used to search symbolic information
              pAddrInfo [out] the address info structure to be filled
   Return value: true if pAddrInfo contains valid data, false if symbol resoluition failed completely

 */
/*UNIX*/static void eo670_FillAddressInfo( char*                 pcAddr,
                                           eo670AddressInfo*     pAddrInfo,
                                           OutputCallBackFuncPtr pOutput,
                                           void * const          pOutContext );
/*
   Function: eo670_UnixGetAddressInfo
   Description: Get the symbolic information using given program counter

   The given address info is cleared and filled. The entries will be filled using eo670_FillAddressInfo
   and copied afterwards. So it is save to call this function for different addresses without side effects.

   NOTE To be implemented individually for each platform...

   Arguments: pcAddr   [in] the program counter used to search symbolic information
              pAddrInfo [out] the address info structure to be filled
              pOutput  [in] callback function, write to user defined ouput channel
   Return value: true if pAddrInfo contains valid data, false if symbol resoluition failed completely
 */
/*UNIX*/static SAPDB_Bool eo670_UnixGetAddressInfo( void*                 pcAddr,
                                                    eo670AddressInfo*     pAddrInfo,
                                                    OutputCallBackFuncPtr pOutput,
                                                    void * const          pOutContext );

typedef enum { eo670_NoArrayJustOutput = 0, eo670_PointerArray = 1, eo670_FullInfoArray = 2 } eo670_CallArrayType;
/*
   Function: eo670_UnixFillCallArray
   Description: Fill the given call array up to limit

   The given call array is filled. The type of call array is either void *[] if addCallInfo is false,
   or eo670_CallInfo[] if addCallInfo is true. In the first case, only the program counter value is
   collected and otherwise additional information gathered.

   NOTE To be implemented individually for each platform...

   addCallInfo eo670_NoArrayJustOutput is just for emergency stack back trace (no memory allocation allowed!)
   addCallInfo eo670_PointerArray is just to collect information for deferred stack backtrace (i.e. memory
    leak detection code uses it to find the procedures, that did the allocation calls....)
   addCallInfo eo670_FullInfoArray is used for symbolic stack backtrace

   Arguments: skipCount      [in] number of stack entries to skip from current context (only used if pBTraceContext != 0)
              maxLevel       [in] size of given array
              pFilledLevel   [out] number of filled entries
              addCallInfo    [in] flag to indicate type of array or output only mode
              callArray      [out] the call array to fill
              pBTraceContext [in] the signal context or 0 if current context to be used
              pOutput        [in] callback function, write to user defined ouput channel
 */
/*UNIX*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
                                      SAPDB_Int4            maxLevel,
                                      SAPDB_Int4 *          pFilledLevel,
                                      eo670_CallArrayType   addCallInfo,
                                      void **               callArray,
                                      void*                 pBTraceContext,
                                      OutputCallBackFuncPtr pOutput,
                                      void * const          pOutContext );

/*
   Function: eo670_RegisterDump
   Description: Create a register dump

   Print out register dump using given signal context.

   NOTE To be implemented individually for each platform...

   Arguments: pBTraceContext [in] the signal context or 0 if current context to be used
 */
/*UNIX*/static void eo670_RegisterDump( void*                     pBTraceContext,
                                        OutputCallBackFuncPtr     pOutput,
                                        void * const              pOutContext );

/*
   Function: eo670_ShowCallInfoAddon
   Description: Show additional frame information

   Print out frame pointer value + additional information if available

   NOTE To be implemented individually for each platform if additional frame info is displayed

   Arguments: callInfo [in] the call information filled during eo670_UnixFillCallArrey
 */
/*UNIX*/static void eo670_UnixShowCallInfoAddon( eo670_CallInfo *      callInfo,
                                                 OutputCallBackFuncPtr pOutput,
                                                 void * const          pOutContext );

#endif /* _WIN32 */

/*
  Function: eo670_AddToLoadedModuleChain
  Description: Add the collected information to the module list

  If text_mapaddr is already found in list, the new entry is ignored.
  Otherwise the new element is allocated and added to list anchored on eo670_ModuleAnchor.
  Module list element is allocated via calloc( ).

  Arguments: file_name [in] module or module file name if available
             text_mapaddr [in] text segment start address
             text_endaddr [in] text segment end address
             osDependend  [in] context pointer used to save local context
             pOutput      [in] callback function, write to user defined ouput channel
  Return value: 0 duplicated entry
                1 entry added
                -1 entry adding failed due to memory problem
 */
static int eo670_AddToLoadedModuleChain( const char *              file_name,
                                         void *                    text_mapaddr,
                                         void *                    text_endaddr,
                                         void *                    osDependend,
                                         SAPDB_ULong               osDependendSize,
                                         OutputCallBackFuncPtr     pOutput,
                                         void * const              pOutContext );

/*
  Function: eo670_ShowLoadedModules

  Description: Show collected list of loaded modules

  Arguments: pOutput      [in] callback function, write to user defined ouput channel

  Using eo670_ModuleAnchor all modules found are listed.
 */
static void eo670_ShowLoadedModules( OutputCallBackFuncPtr           pOutput,
                                     void * const                    pOutContext );

#ifdef HAS_FAST_FILL_CALL_ARRAY
SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4                      skipCount,
                                    SAPDB_Int4                      givenMaxLevel,
                                    void **                         callArray,
                                    OutputCallBackFuncPtr           pOutput,
                                    void * const                    pOutContext );
#endif

/*
   Function: eo670_GetStackLimit
   Description: Get the lowest allowed stack address

   In KERNEL the current task stack is used to find the limit using vgetpid() to find it.
   In non KERNEL this is returning 0 instead.

   Return value: lowest known stack address
 */
static SAPDB_Byte *eo670_GetStackLimit(void *currentStackPointer);

/*!
   Description:    Write a string into the stack back trace output channel.
   Arguments:      str           [in] string to write
                   pDisplacement [in] offset to to begin of line
                   pOutput       [in] callback function, write to user defined ouput channel.
                                      Use NO_CALLBACK to write to standard output channel.
                   pOutContext   [in] caller specific ouput context information (e.g. object pointer)
*/
static void eo670WriteOutput( const char*           str,
                              const char*           pDisplacement,
                              OutputCallBackFuncPtr pOutput,
                              void * const          pOutContext );

/*===========================================================================*
 *  CRITICAL SECTION                                                         *
 *===========================================================================*/

#if defined (_WIN32)

# define CS_TRACE_BACK 0
# define CS_WRITE_DUMP 1
# define CS_LAST       1

typedef struct _tCritSec { CRITICAL_SECTION  CritSec;
                            BOOL             CritSecInitialized;
                            INT              CritSecCount; } tCritSec;

static tCritSec CS[CS_LAST + 1];

#  define  ENTER_CRIT_SEC(_No)                                                    \
           if (!CS[_No].CritSecInitialized) { CS[_No].CritSecInitialized = TRUE; \
             InitializeCriticalSection(&CS[_No].CritSec); }                      \
           EnterCriticalSection(&CS[_No].CritSec); CS[_No].CritSecCount++;

#  define  EXIT_CRIT_SEC(_No)                                                     \
           if (CS[_No].CritSecCount) { CS[_No].CritSecCount--;                   \
             LeaveCriticalSection(&CS[_No].CritSec); }

#endif /* WIN32 */


/* Kernel LZU implementation found in RTEDiag_SymbolResolutionLock.cpp */
#if !defined(KERNEL_LZU)
/*!
  @brief Init or Reinit of lock

  If never called before this function just initializes the lock.
  If called before and the lock was aquired, it is simply released.
  If called before and the lock is engaged, it is forced to reinitialize!!!!

  The return value is used too output a message if lock reinit was forced

  @param pEverCalled [in] pointer to flag indicating initialization or reinitialization
  @param pAnchor [in] pointer to module list anchor which must be reset too
  @return true if lock is just initialized, false if lock reinitialization was forced
 */
SAPDB_Bool RTEDiag_SymbolResolutionInit(volatile int *pEverCalled, void **pAnchor)
{
    if ( !*pEverCalled )
    {
        RTESys_InitLock(&eo670_inTraceStack);
        *pEverCalled = 1;
    }
    else
    {
        /* since all calls of eo670_CTraceStackInit are protected via eo670_TraceEverCalled,
           this must wanted reinit... */
        if ( RTESys_TestAndLock(&eo670_inTraceStack) )
        {
            *pAnchor = 0; /* reset module list anchor too */
            RTESys_InitLock(&eo670_inTraceStack);
            return false;
        }
        else
        {
            RTESys_Unlock(&eo670_inTraceStack);
        }
    }
    return true;
}

/*!
  @brief lock the symbol resolution lock
 */
void RTEDiag_SymbolResolutionLock()
{
    if ( RTESys_TestAndLock(&eo670_inTraceStack) )
    {
        while ( RTESys_TestAndLock(&eo670_inTraceStack) )
        {
            GIVEUP_TIME_SLICE;
        }
    }
}

/*!
  @brief unlock the symbol resolution lock
 */
void RTEDiag_SymbolResolutionUnlock()
{
    RTESys_Unlock(&eo670_inTraceStack);
}
#endif

/*===========================================================================*
 *  GLOBAL FUNCTIONS                                                         *
 *===========================================================================*/

#ifdef HAS_NO_STACKBACKTRACE_IMPLEMENTATION

/* Dummy stubs... */
/*DUMMYSTUBS*/void eo670_CTraceStackInit()
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void vtracestack (void) {
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_SetDefaultOutputCallback(OutputCallBackFuncPtr pOutput, void * const pOutContext)
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_CTraceContextStack( eo670TraceContextPtr      pBTraceContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_CTraceContextStackOCB( eo670TraceContextPtr  pBTraceContext,
/*DUMMYSTUBS*/                                  OutputCallBackFuncPtr pOutput,
/*DUMMYSTUBS*/                                  void * const          pOutContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_CTraceStack(void)
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_CTraceStackOCB( OutputCallBackFuncPtr pOutput,
/*DUMMYSTUBS*/                           void * const          pOutContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/SAPDB_Bool eo670_GetAddressInfo( void *            pcAddr,
/*DUMMYSTUBS*/                                 eo670AddressInfo *pAddrInfo,
/*DUMMYSTUBS*/                                 void *            pAddrInfoContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return 0;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_FreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/SAPDB_Bool eo670_GetAddressInfoProlog ( void** pAddrInfoContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return 0;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670_GetAddressInfoEpilog ( void* pAddrInfoContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/SAPDB_Int4 eo670_FillCallArray( SAPDB_Int4 givenMaxLevel,
/*DUMMYSTUBS*/                                void **    callArray )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/    return 0;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670WriteString( const char *str )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/	return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/void eo670WriteStringOCB( const char*           str,
/*DUMMYSTUBS*/                          OutputCallBackFuncPtr pOutput,
/*DUMMYSTUBS*/                          void * const          pOutContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/	return;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/SAPDB_Bool eo670_UpdateSymbols()
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/  return true;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
/*DUMMYSTUBS*/SAPDB_Bool eo670_UpdateSymbolsOCB( OutputCallBackFuncPtr pOutput,
/*DUMMYSTUBS*/                                    void * const          pOutContext )
/*DUMMYSTUBS*/{
/*DUMMYSTUBS*/  return true;
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/SAPDB_Bool eo670_WriteDump ( bool WriteFullMemory, bool IfDumpFileExists )
/*DUMMYSTUBS*/{ 
/*DUMMYSTUBS*/	return true; 
/*DUMMYSTUBS*/}
/*DUMMYSTUBS*/
#else /* HAS_NO_STACKBACKTRACE_IMPLEMENTATION */

/*!
   Description:    Fill the given array with trace back entries.
   Arguments:      givenMaxLevel [in] the number of level to fill
                   callArray [out] the array to fill
   Return value:   The actual number of entries filled

 */

SAPDB_Int4 eo670_FillCallArray( SAPDB_Int4 givenMaxLevel,
                                void **    callArray )
{
    SAPDB_Int4            filledLevel = 0;
    OutputCallBackFuncPtr pOutput     = pDefaultOutput;
    void*                 pOutContext = pDefaultOutContext;

    if ( givenMaxLevel <= 0 ) return 0;

    SAPDB_memset(callArray, 0, sizeof(void *) * givenMaxLevel );

#  ifdef HAS_FAST_FILL_CALL_ARRAY
    filledLevel = eo670_FastFillCallArray(FILL_SKIP_COUNT, givenMaxLevel, callArray, pOutput, pOutContext);
#  else

    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB(pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

#ifdef WIN32
/*WIN32*/ eo670_WinFillCallArray(givenMaxLevel, callArray, pOutput, pOutContext, &filledLevel );
#else /* WIN32 */
/*UNIX*/ eo670_UnixFillCallArray(FILL_SKIP_COUNT, givenMaxLevel, &filledLevel, eo670_PointerArray, callArray, 0, pOutput, pOutContext);
#    endif /* WIN32 */

    RTEDiag_SymbolResolutionUnlock();

#  endif
    return filledLevel;
}

/*---------------------------------------------------------------------------
  Function: eo670_GetAddressInfo
  Description: Get mapping information about a given PC address
  Arguments: pcAddr [in] a hopefully valid program counter
             pAddrInfo [out] structure to be filled with address information
  Return value: true if mapping succeeded, false if not (pAddrInfo still valid)
 */
SAPDB_Bool eo670_GetAddressInfo( void *            pcAddr,
                                 eo670AddressInfo *pAddrInfo,
                                 void *            pAddrInfoContext )
{
    SAPDB_Bool            callResult;
    OutputCallBackFuncPtr pOutput = pDefaultOutput;
    void *                pOutContext = pDefaultOutContext;

#if defined(KERNEL_LZU)
    if ( !RTEDiag_SymbolResolutionAllowed()
#if defined(_WIN32)
    /* This addition check prevents problems during online switching of the SYMBOL_RESOLUTION parameter */
       || !pAddrInfoContext /* on windows the context must exist! */
#endif
       )
    {
        if ( pAddrInfo )
        {
                memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
                pAddrInfo->symbol = "resolving symbols switched off";
                pAddrInfo->demangledSymbol = "resolving symbols switched off";
                pAddrInfo->moduleName = "";
                pAddrInfo->sourceFileName  = "";
                pAddrInfo->offsetToSymbol = 0;
                pAddrInfo->lineNumber = 0;
        }
        return true;
    }
#endif

    SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));

    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB(pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

#  if defined(WIN32)
/*WIN32*/ callResult = eo670_WinGetAddressInfo(pcAddr, pAddrInfo, pAddrInfoContext, pOutput, pOutContext);
#  else
/*UNIX*/  callResult = eo670_UnixGetAddressInfo(pcAddr, pAddrInfo, pOutput, pOutContext);
#  endif /* WIN32 */

    RTEDiag_SymbolResolutionUnlock();

    return callResult;
}

/*---------------------------------------------------------------------------
  Function: eo670_FreeAddressInfo
  Description: Free the mapping information about a given PC address
  Arguments: pAddrInfo [in] structure to be freed
 */
void eo670_FreeAddressInfo( eo670AddressInfo *pAddrInfo )
{
    OutputCallBackFuncPtr pOutput     = pDefaultOutput;
    void *                pOutContext = pDefaultOutContext;

#if defined(KERNEL_LZU)
    if ( !RTEDiag_SymbolResolutionAllowed()
       && ( !pAddrInfo
         || !pAddrInfo->symbol
         || 0 == strcmp(pAddrInfo->symbol, "resolving symbols switched off") )
       )
    {
        return;
    }
#endif

    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB(pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

    eo670_OsFreeAddressInfo(pAddrInfo);

    RTEDiag_SymbolResolutionUnlock();
}


/*!
   Description:    Update symbol information (should be called after a DLL/shared library has been loaded)
 */

SAPDB_Bool eo670_UpdateSymbols()
{
    return eo670_UpdateSymbolsOCB( pDefaultOutput, pDefaultOutContext );
}

SAPDB_Bool eo670_UpdateSymbolsOCB( OutputCallBackFuncPtr pOutput,
                                    void * const          pOutContext )
{
    SAPDB_Bool            callResult = true;

    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB(pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

#  if defined(WIN32)
/*WIN32*/ callResult = eo670_WinUpdateSymbols(pOutput, pOutContext);
#  else
/*UNIX*/  callResult = eo670_UpdateModuleChain(pOutput, pOutContext);
#  endif /* WIN32 */

    RTEDiag_SymbolResolutionUnlock();

    return callResult;
}

void eo670_SetDefaultOutputCallback(OutputCallBackFuncPtr pOutput, void * const pOutContext)
{
    pDefaultOutput = pOutput;
    pDefaultOutContext = pOutContext;
}

/*!
   Description:    Produce a Stackbacktrace on stderr using given context

 */
void eo670_CTraceContextStack( eo670TraceContextPtr    pBTraceContext )
{
    eo670_CTraceContextStackOCB( pBTraceContext, pDefaultOutput, pDefaultOutContext );
}

void eo670_CTraceContextStackOCB( eo670TraceContextPtr  pBTraceContext,
                                  OutputCallBackFuncPtr pOutput,
                                  void * const          pOutContext )
{
#  if defined(_WIN32)
/*WIN32*/ eo670_WindowsTraceStack(pBTraceContext, pOutput, pOutContext);
#  else
/*UNIX*/  eo670_UnixTraceStack(pBTraceContext, pOutput, pOutContext);
#  endif /* WIN32 */
}

/*!
   Description:    Produce a Stackbacktrace on stderr

 */
void eo670_CTraceStack( void )
{
    eo670_CTraceStackOCB(pDefaultOutput, pDefaultOutContext);
}

void eo670_CTraceStackInit()
{
    eo670_CTraceStackInitOCB( pDefaultOutput, (void *)0 );
}

void eo670_CTraceStackInitOCB( OutputCallBackFuncPtr pOutput,
                               void * const          pOutContext )
{
#if !defined(_WIN32)
/*UNIX*/    int everCalledBefore = eo670_TraceEverCalled;
#endif /* !_WIN32 */
    if ( !RTEDiag_SymbolResolutionInit(&eo670_TraceEverCalled, (void **)&eo670_ModuleAnchor) )
    {
        /* since all calls of eo670_CTraceStackInit are protected via eo670_TraceEverCalled,
           this must be a wanted reinit... */
        eo670WriteOutput("Stack back trace lock forced to reininitalize\n", NO_DISPLACEMENT, pOutput, pOutContext);
    }
#if !defined(_WIN32)
/*UNIX*/ if ( !everCalledBefore )
/*UNIX*/ {
/*UNIX*/     eo670_UpdateModuleChain(pOutput, pOutContext);
/*UNIX*/ }
#endif /* !_WIN32 */
}

void eo670_CTraceStackOCB( OutputCallBackFuncPtr pOutput,
                           void * const          pOutContext )
{
    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB( pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

    eo670_CTraceContextStackOCB((eo670TraceContextPtr)0, pOutput, pOutContext);

    RTEDiag_SymbolResolutionUnlock();
}

/*!
   Description:    Pascal wrapper for eo670_CTraceStack

 */

void vtracestack( void )
{
    OutputCallBackFuncPtr pOutput     = pDefaultOutput;
    void *                pOutContext = pDefaultOutContext;

    if ( !eo670_TraceEverCalled )
    {
        eo670_CTraceStackInitOCB(pOutput, pOutContext);
    }

    RTEDiag_SymbolResolutionLock();

    eo670_CTraceContextStackOCB((eo670TraceContextPtr)0, pOutput, pOutContext);

    RTEDiag_SymbolResolutionUnlock();
}

/*!
   Description:    finishes a sequences of eo670_GetAddressInfo calls and
                   release all resources hold by eo670_GetAddressInfoProlog.
   Arguments:      context [in] pointer returned by eo670_GetAddressInfoProlog
   Return value:   none

 */


void eo670_GetAddressInfoEpilog ( void* pAddrInfoContext )
{
#  ifdef WIN32
/*WIN32*/ OutputCallBackFuncPtr pOutput     = pDefaultOutput;
/*WIN32*/ void *                pOutContext = pDefaultOutContext;
/*WIN32*/
#    if defined(KERNEL_LZU)
/*WIN32*/ if ( !RTEDiag_SymbolResolutionAllowed()
/*WIN32*/ /* This addition check prevents problems during online switching of the SYMBOL_RESOLUTION parameter */
/*WIN32*/   || !pAddrInfoContext /* on windows the context must exist! */
/*WIN32*/    )
/*WIN32*/ {
/*WIN32*/         return;
/*WIN32*/ }
#    endif /* KERNEL_LZU */
/*WIN32*/
/*WIN32*/ if ( !eo670_TraceEverCalled )
/*WIN32*/ {
/*WIN32*/    eo670_CTraceStackInitOCB(pOutput, pOutContext);
/*WIN32*/ }
/*WIN32*/
/*WIN32*/ RTEDiag_SymbolResolutionLock();
/*WIN32*/
/*WIN32*/ eo670_WinGetAddressInfoEpilog(pAddrInfoContext, pOutput, pOutContext);
/*WIN32*/
/*WIN32*/ RTEDiag_SymbolResolutionUnlock();
#  endif /* WIN32 */
}

/*!
   Description:    prepares for following calls of eo670_GetAddressInfo
   Arguments:      context [out] pointer to context required for eo670_GetAddressInfo.
                   may be null on several plattforms
   Return value:   true if mapping returns valid data, false if not
 */
SAPDB_Bool eo670_GetAddressInfoProlog ( void** pAddrInfoContext )
{
#  ifdef WIN32
/*WIN32*/ OutputCallBackFuncPtr pOutput     = pDefaultOutput;
/*WIN32*/ void *                pOutContext = pDefaultOutContext;
/*WIN32*/ SAPDB_Bool            result;
/*WIN32*/
#    if defined(KERNEL_LZU)
/*WIN32*/ if ( !RTEDiag_SymbolResolutionAllowed() )
/*WIN32*/ {
/*WIN32*/     *pAddrInfoContext = 0;
/*WIN32*/     return true;
/*WIN32*/ }
#    endif /* KERNEL_LZU */
/*WIN32*/
/*WIN32*/ if ( !eo670_TraceEverCalled )
/*WIN32*/ {
/*WIN32*/   eo670_CTraceStackInitOCB(pOutput, pOutContext);
/*WIN32*/ }
/*WIN32*/
/*WIN32*/ RTEDiag_SymbolResolutionLock();
/*WIN32*/
/*WIN32*/ result = eo670_WinGetAddressInfoProlog(pAddrInfoContext, pOutput, pOutContext);
/*WIN32*/
/*WIN32*/ RTEDiag_SymbolResolutionUnlock();
/*WIN32*/
/*WIN32*/ return result;
#  else
/*UNIX*/ return true;
#  endif /* WIN32 */
}

/*===========================================================================*
 *  LOCAL/PRIVATE/PROTECTED FUNCTIONS (CODE)                                 *
 *===========================================================================*/

static void eo670WriteOutput( const char*           str,
                              const char*           pDisplacement,
                              OutputCallBackFuncPtr pOutput,
                              void * const          pOutContext )
{
	/* PTS 1112092 */
    /* Message Text limit is used for several reasons.
       dbmgetf reads out knldiag on a line by line base and copies atmost
       255 character per line. Each knldiag line begins with 44 characters of
       header information. Two lines on an 80 character display 160 minus 44 character
       header result in 116 character. One character less to prevent automatic wrap
       around leads to 115.
       This value is only needed due to the line oriented output of knldiag.
    */

    static const char *pc;
    static int         msgLength;
    static int         Limit;
    static const char  *disp;
    static char        MessageBuffer[MESSAGE_TEXT_LIMIT + 1];

    disp = ( pDisplacement == NO_DISPLACEMENT ) ? "" : pDisplacement;

    pc        = strchr(str,'\n');
    msgLength = ( pc != 0 ? (int)(pc-str) : (int)strlen(str) );

    pc = str;
    sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "%.*s", msgLength > MESSAGE_TEXT_LIMIT ? MESSAGE_TEXT_LIMIT : msgLength, pc );

    if ( pOutput )
        pOutput(MessageBuffer, pOutContext);
    else
    {
#     if defined(KERNEL_LZU) || defined(SERVER_LZU)
        MSGD (( MSG_ID, 1, "BTRACE  ", MessageBuffer ));
#     else /* not in kernel variant */
#       if defined(_WIN32)
         fprintf(stderr, "%s", MessageBuffer);
#       else
         write(2, MessageBuffer, strlen(MessageBuffer));
#       endif
#     endif /* KERNEL_LZU */
    }

    msgLength -= MESSAGE_TEXT_LIMIT;
    pc        += MESSAGE_TEXT_LIMIT;

    Limit = MESSAGE_TEXT_LIMIT - (int)strlen(disp);

    while ( msgLength > 0 )
    {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "%s%.*s", disp, msgLength > Limit ? Limit : msgLength, pc );

        if ( pOutput )
            pOutput(MessageBuffer, pOutContext);
        else
        {
#        if defined(KERNEL_LZU) || defined(SERVER_LZU)
            MSGD (( MSG_ID, 1, "BTRACE  ", MessageBuffer ));
#         else /* not in kernel variant */
#           if defined(_WIN32)
             fprintf(stderr, "%s", MessageBuffer);
#           else
             write(2, MessageBuffer, strlen(MessageBuffer));
#           endif
#         endif /* KERNEL_LZU */
        }

        msgLength -= Limit;
        pc        += Limit;
    }
}

/*---------------------------------------------------------------------------*/

void eo670WriteString( const char *str )
{
  eo670WriteOutput(str, NO_DISPLACEMENT, pDefaultOutput, pDefaultOutContext);
}

/*---------------------------------------------------------------------------*/

void eo670WriteStringOCB( const char *          str,
                          OutputCallBackFuncPtr pOutput,
                          void * const          pOutContext )
{
  eo670WriteOutput(str, NO_DISPLACEMENT, pOutput, pOutContext);
}

/*---------------------------------------------------------------------------
  Function: eo670_AddToLoadedModuleChain
  Description: Add a module entry into sorted list of modules
 */
static int eo670_AddToLoadedModuleChain( const char *          file_name,
                                         void *                text_mapaddr,
                                         void *                text_endaddr,
                                         void *                osDependend,
                                         SAPDB_ULong           osDependendSize,
                                         OutputCallBackFuncPtr pOutput,
                                         void * const          pOutContext )
{
    static eo670_ModuleChain * modulePointer;
    static eo670_ModuleChain ** pSearchPointer;

    pSearchPointer = &eo670_ModuleAnchor;
    do
    {
        if ( 0 != *pSearchPointer )
        {
            if ( (*pSearchPointer)->text_mapaddr > text_mapaddr )
            {
                break;
            }

            if ( (*pSearchPointer)->text_mapaddr == text_mapaddr )
            {
                return 0;
            }
            pSearchPointer = &((*pSearchPointer)->next);
        }
    } while ( 0 != (*pSearchPointer) );

    if ( eo670_ModuleAnchor )
    {
        /* We need space for the ModuleChain structure and the copy of the file_name + alignment */
        /* We know position of 'last' pointer which was used */

#define ALIGNED_STRLEN_EO670(string_) ( (strlen(string_)+sizeof(void*)) & ~(sizeof(void*)-1) )
#define ALIGNED_SIZE_EO670(givensize_)   ( (givensize_+sizeof(void*)-1) & ~(sizeof(void*)-1) )

        /* First we use the last pointer to calculate the next last pointer, which is used */
        /* later to store the new entry */
        eo670_LastModuleEntry = (eo670_ModuleChain *) ( ((char *)(eo670_LastModuleEntry+1))
                                          + ALIGNED_STRLEN_EO670(eo670_LastModuleEntry->file_name)
                                          + ALIGNED_SIZE_EO670(eo670_LastModuleEntry->osDependendSize) );
    }
    else
    {
        /* if we never stored anything we begin at start of buffer */
        eo670_LastModuleEntry = (eo670_ModuleChain *)&eo670_ModuleInfoBuffer[0];
    }

    if ( (int)( sizeof(eo670_ModuleChain) + ALIGNED_STRLEN_EO670(file_name) + ALIGNED_SIZE_EO670(osDependendSize) )
       > (int)(&eo670_ModuleInfoBuffer[sizeof(eo670_ModuleInfoBuffer)]  - (((char *)eo670_LastModuleEntry) ) ) )
    {
        eo670WriteOutput("Out of memory for ModuleChain entry\n", NO_DISPLACEMENT, pOutput, pOutContext);
        return -1;
    }

    eo670_LastModuleEntry->next         = *pSearchPointer;
    eo670_LastModuleEntry->file_name    = (char *)(eo670_LastModuleEntry+1);
    SAPDB_memcpy(eo670_LastModuleEntry->file_name, file_name, SAPDB_strlen(file_name)+1);
    eo670_LastModuleEntry->text_mapaddr = text_mapaddr;
    eo670_LastModuleEntry->text_endaddr = text_endaddr;

    if ( osDependendSize > 0 )
    {
        eo670_LastModuleEntry->osDependend  = ((char *)eo670_LastModuleEntry->file_name) + ALIGNED_STRLEN_EO670(file_name);
        eo670_LastModuleEntry->osDependendSize = osDependendSize;
        memcpy(eo670_LastModuleEntry->osDependend, osDependend, osDependendSize);
    }
    else
    {
        eo670_LastModuleEntry->osDependend  = 0;
        eo670_LastModuleEntry->osDependendSize = 0;
    }

    *pSearchPointer = eo670_LastModuleEntry; /* This also sets eo670_ModuleAnchor if never used before */

    return 1;
}

/*---------------------------------------------------------------------------
  Function: eo670_ShowLoadedModules
  Description: Show sorted list of modules
 */
static void eo670_ShowLoadedModules( OutputCallBackFuncPtr pOutput,
                                     void * const          pOutContext )
{
    static eo670_ModuleChain * modulePointer = 0;

	sp77sprintf( eo670MessageBuffer,
                 sizeof(eo670MessageBuffer),
#  if defined(BIT64)
                 "|%-18s|%-18s|%s\n",
#  else
                 "|%-12s|%-12s|%s\n",
#  endif /* BIT64 */
                 ".text Start",
                 ".text End",
                 " Module File Name" );

    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);

    for ( modulePointer = eo670_ModuleAnchor;
          NULL != modulePointer;
          modulePointer = modulePointer->next )
    {
	    sp77sprintf( eo670MessageBuffer,
                     sizeof(eo670MessageBuffer),
#  if defined(BIT64)
                     "|0x%016lx|0x%016lx| %s\n",
#    define SHOW_MODULE_DISPLACEMENT_EO670 ":                  :                  : "
#  else
                     "| 0x%08lx | 0x%08lx | %s\n",
#    define SHOW_MODULE_DISPLACEMENT_EO670 ":            :            : "
#  endif /* BIT64 */
                     (SAPDB_Long)modulePointer->text_mapaddr,
                     (SAPDB_Long)modulePointer->text_endaddr,
                     modulePointer->file_name );

        eo670WriteOutput(eo670MessageBuffer, SHOW_MODULE_DISPLACEMENT_EO670, pOutput, pOutContext);
    }
    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
}

/*---------------------------------------------------------------------------
  Function: eo670_ShowAddressInfo
  Description: Show address information

  If no symbol is found, the offsetToSymbol is negativ.
  If a offsetToSymbol is given, it is show using the
  demangledSymbol if available.
  If additonal line number information is found, it is
  show too, otherwise only the module name is given.
 */
static void eo670_ShowAddressInfo( int                    entryIndex,
                                   void *                 pcAddr,
                                   eo670AddressInfo *     pAddrInfo,
                                   OutputCallBackFuncPtr  pOutput,
                                   void * const           pOutContext )
{
    if ( pAddrInfo->offsetToSymbol >= 0 )
    {
        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
#  if defined(BIT64)
                     "%4d: 0x%016lx %s +0x%04x\n",
#    define DISPLACEMENT_SHOW_SYMBOL_EO670 "                         "
#  else
                     "%4d: 0x%08lx %s +0x%04x\n",
#    define DISPLACEMENT_SHOW_SYMBOL_EO670 "                 "
#  endif /* BIT64 */
                     entryIndex,
                     (SAPDB_Long)pcAddr,
    ( *pAddrInfo->demangledSymbol ? pAddrInfo->demangledSymbol : pAddrInfo->symbol ),
                     pAddrInfo->offsetToSymbol );
        eo670WriteOutput(eo670MessageBuffer, DISPLACEMENT_SHOW_SYMBOL_EO670, pOutput, pOutContext);
        if ( pAddrInfo->lineNumber >= 0 )
        {
            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
                         "       %s - %s:%d\n",
                         pAddrInfo->moduleName,
                         pAddrInfo->sourceFileName,
                         pAddrInfo->lineNumber );
        }
        else if ( 0 != *pAddrInfo->sourceFileName )
        {
            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
                         "       %s - %s\n",
                         pAddrInfo->moduleName,
                         pAddrInfo->sourceFileName );
        }
        else
        {
            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
                         "       %s\n",
                         pAddrInfo->moduleName );
        }
    }
    else
    {
        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
                     "%4d: 0x%016lx %s\n",
                     entryIndex,
                     (SAPDB_Long)pcAddr,
                     pAddrInfo->symbol );
    }
    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
}

/* dump a section of memory as ints */

static void eo670_MemoryDump( void *                 startAddr,
                              SAPDB_Int4             cnt,
                              OutputCallBackFuncPtr  pOutput,
                              void * const           pOutContext )
{
    SAPDB_Int4  firstBytes;
    static SAPDB_Int4 iOffset;

    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
#  ifdef BIT64
                 "Memory dump from startAddr 0x%016lx endAddr 0x%016lx\n",
#  else
                 "Memory dump from startAddr 0x%08lx endAddr 0x%08lx\n",
#  endif /* BIT64 */
                 ((char *)startAddr)-((char *)0),
                 ((char *)startAddr)+cnt-((char *)0) );
    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);

    firstBytes = (SAPDB_Int4)(((char *)startAddr)-((char *)0)) & (sizeof(SAPDB_UInt4)-1);
    if ( firstBytes > 0 )
    {
        firstBytes = sizeof(SAPDB_Int4)-firstBytes;
        cnt -= firstBytes;
        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
#  ifdef BIT64
                     "0x%016lx",
#  else
                     "0x%08lx",
#  endif /* BIT64 */
                     startAddr );
        while ( firstBytes > 0 )
        {
            sp77sprintf( eo670MessageBuffer + strlen(eo670MessageBuffer),
                         (int)(sizeof(eo670MessageBuffer) - strlen(eo670MessageBuffer)),
                         " 0x%02x",
                          *(unsigned char *)startAddr );
            startAddr = ((char *)startAddr)+1;
            --firstBytes;
        }
        sp77sprintf( eo670MessageBuffer + strlen(eo670MessageBuffer),
                     (int)(sizeof(eo670MessageBuffer) - strlen(eo670MessageBuffer)),
                     "\n" );
        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
    }
    cnt = (cnt + sizeof(SAPDB_UInt4)-1)/sizeof(SAPDB_UInt4);

    for ( iOffset = 0; iOffset < cnt; iOffset += 4 )
    {
        sp77sprintf( eo670MessageBuffer,
                     sizeof(eo670MessageBuffer),
                     "0x%08lx 0x%08lx 0x%08lx 0x%08lx 0x%08lx\n",
                     ((char *)(((SAPDB_Int4*)startAddr)+iOffset))-((char *)0),
                    *(((SAPDB_UInt4*)startAddr)+iOffset), *(((SAPDB_UInt4*)startAddr)+iOffset+1),
                    *(((SAPDB_UInt4*)startAddr)+iOffset+2), *(((SAPDB_UInt4*)startAddr)+iOffset+3) );
        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
    }
}


#if !defined(WIN32)
/*===========================================================================*
 *  UNIX                                                                     *
 *===========================================================================*/

#  if defined(WANT_DEMANGLING_SUPPORT)
/*--------------------------
  Signal handling code during demangle
 */
/*UNIX*/
/*UNIX*/static struct sigaction oldSigbusForDemangle;
/*UNIX*/static struct sigaction oldSigsegvForDemangle;
#    if defined(LINUX) || defined(SOLARIS) || defined(SUN) || defined (FREEBSD)
/*SOLARIS+LINUX*/static sigjmp_buf eo670_emergencyForDemangle;
#    else
/*AIX+OSF1*/static jmp_buf eo670_emergencyForDemangle;
#    endif /* LINUX || SOLARIS */
/*UNIX*/
/*UNIX*//*	This is the stack trace back SIGBUS/SIGSEGV signal handler. */
/*UNIX*/static void eo670_StackTraceSegvFuncForDemangle(int Signal)
/*UNIX*/{
/*UNIX*/    siglongjmp(eo670_emergencyForDemangle, Signal != 0 ? Signal : 1);
/*UNIX*/}
/*UNIX*/
/*UNIX*/static char *eo670_demangle(char *symbol, char *(*demangleCall)(char *))
/*UNIX*/{
/*UNIX*/    static char *demangledSymbol;
/*UNIX*/    static void *           old_sigsegvForDemangle;
/*UNIX*/    static void *           old_sigbusForDemangle;
/*UNIX*/    static sigset_t         old_setForDemangle;
/*UNIX*/    if ( 0 == sigsetjmp( eo670_emergencyForDemangle, 1) )
/*UNIX*/    {
/*UNIX*/        static struct sigaction saForDemangle;
/*UNIX*/        static sigset_t         mask_setForDemangle;
/*UNIX*/
/*UNIX*/        saForDemangle.sa_handler = eo670_StackTraceSegvFuncForDemangle;
/*UNIX*/        saForDemangle.sa_flags   = 0;
/*UNIX*/        sigemptyset( &saForDemangle.sa_mask );
/*UNIX*/
/*UNIX*/        sigaction(SIGSEGV, &saForDemangle, &oldSigsegvForDemangle);
/*UNIX*/        sigaction(SIGBUS, &saForDemangle, &oldSigbusForDemangle);
/*UNIX*/
/*UNIX*/        sigfillset(&mask_setForDemangle);
/*UNIX*/        sigdelset(&mask_setForDemangle, SIGSEGV);
/*UNIX*/        sigdelset(&mask_setForDemangle, SIGBUS);
/*UNIX*/        sigprocmask(SIG_SETMASK, &mask_setForDemangle, &old_setForDemangle);
/*UNIX*/
/*UNIX*/        demangledSymbol = (*demangleCall)(symbol);
/*UNIX*/    }
/*UNIX*/    sigaction(SIGSEGV, &oldSigsegvForDemangle, 0);
/*UNIX*/    sigaction(SIGBUS, &oldSigbusForDemangle, 0);
/*UNIX*/    sigprocmask(SIG_SETMASK, &old_setForDemangle, (void*)0);
/*UNIX*/    return demangledSymbol;
/*UNIX*/}
/*UNIX*/
#  endif /* WANT_DEMANGLING_SUPPORT */

/*--------------------------
  Signal handling code during filling address info (symbol lookup)
 */
/*UNIX*/
/*UNIX*/static struct sigaction oldSigbusForLookup;
/*UNIX*/static struct sigaction oldSigsegvForLookup;
#  if defined(LINUX) || defined(SOLARIS) || defined(SUN) || defined(FREEBSD)
/*SOLARIS+LINUX*/static sigjmp_buf eo670_emergencyForLookup;
#  else
/*AIX+OSF1*/static jmp_buf eo670_emergencyForLookup;
#  endif /* LINUX || SOLARIS */
/*UNIX*/
/*UNIX*//*	This is the stack trace back SIGBUS/SIGSEGV signal handler. */
/*UNIX*/static void eo670_StackTraceSegvFuncForLookup(int Signal)
/*UNIX*/{
/*UNIX*/    siglongjmp(eo670_emergencyForLookup, Signal != 0 ? Signal : 1);
/*UNIX*/}
/*UNIX*/
/*UNIX*/static void eo670_SaveFillAddressInfo( char *                pcAddr,
/*UNIX*/                                       eo670AddressInfo *    pAddrInfo,
/*UNIX*/                                       OutputCallBackFuncPtr pOutput,
/*UNIX*/                                       void * const          pOutContext )
/*UNIX*/{
/*UNIX*/    static char *demangledSymbol;
/*UNIX*/    static void *           old_sigsegvForLookup;
/*UNIX*/    static void *           old_sigbusForLookup;
/*UNIX*/    static sigset_t         old_setForLookup;
/*UNIX*/    if ( 0 == sigsetjmp( eo670_emergencyForLookup, 1) )
/*UNIX*/    {
/*UNIX*/        static struct sigaction saForLookup;
/*UNIX*/        static sigset_t         mask_setForLookup;
/*UNIX*/
/*UNIX*/        saForLookup.sa_handler = eo670_StackTraceSegvFuncForLookup;
/*UNIX*/        saForLookup.sa_flags   = 0;
/*UNIX*/        sigemptyset( &saForLookup.sa_mask );
/*UNIX*/
/*UNIX*/        sigaction(SIGSEGV, &saForLookup, &oldSigsegvForLookup);
/*UNIX*/        sigaction(SIGBUS, &saForLookup, &oldSigbusForLookup);
/*UNIX*/
/*UNIX*/        sigfillset(&mask_setForLookup);
/*UNIX*/        sigdelset(&mask_setForLookup, SIGSEGV);
/*UNIX*/        sigdelset(&mask_setForLookup, SIGBUS);
/*UNIX*/        sigprocmask(SIG_SETMASK, &mask_setForLookup, &old_setForLookup);
/*UNIX*/
/*UNIX*/        eo670_FillAddressInfo(pcAddr, pAddrInfo, pOutput, pOutContext);
/*UNIX*/    }
/*UNIX*/    sigaction(SIGSEGV, &oldSigsegvForLookup, 0);
/*UNIX*/    sigaction(SIGBUS, &oldSigbusForLookup, 0);
/*UNIX*/    sigprocmask(SIG_SETMASK, &old_setForLookup, (void*)0);
/*UNIX*/}
/*UNIX*/
/*--------------------------
  Signal handling code during filling call array
 */
/*UNIX*/
/*UNIX*/static struct sigaction oldSigbusForFill;
/*UNIX*/static struct sigaction oldSigsegvForFill;
#  if defined(LINUX) || defined(SOLARIS) || defined(SUN) || defined(FREEBSD)
/*SOLARIS+LINUX*/static sigjmp_buf eo670_emergencyForFill;
#  else
/*AIX+OSF1*/static jmp_buf eo670_emergencyForFill;
#  endif /* LINUX || SOLARIS */
/*UNIX*/
/*UNIX*//*	This is the stack trace back SIGBUS/SIGSEGV signal handler. */
/*UNIX*/static void eo670_StackTraceSegvFuncForFill(int Signal)
/*UNIX*/{
/*UNIX*/    siglongjmp(eo670_emergencyForFill, Signal != 0 ? Signal : 1);
/*UNIX*/}
/*UNIX*/
/*UNIX*/static void eo670_SaveFillCallArray( SAPDB_Int4            skipCount,
/*UNIX*/                                     SAPDB_Int4            maxLevel,
/*UNIX*/                                     SAPDB_Int4 *          pFilledLevel,
/*UNIX*/                                     eo670_CallArrayType   addCallInfo,
/*UNIX*/                                     void **               callArray,
/*UNIX*/                                     void*                 pBTraceContext,
/*UNIX*/                                     OutputCallBackFuncPtr pOutput,
/*UNIX*/                                     void * const          pOutContext )
/*UNIX*/{
#ifndef HAS_SETJMP_BUG
/*UNIX*/    static void *           old_sigsegvForFill;
/*UNIX*/    static void *           old_sigbusForFill;
/*UNIX*/    static sigset_t         old_setForFill;
/*UNIX*/    if ( 0 == sigsetjmp( eo670_emergencyForFill, 1) )
/*UNIX*/    {
/*UNIX*/        static struct sigaction saForFill;
/*UNIX*/        static sigset_t         mask_setForFill;
/*UNIX*/
/*UNIX*/        saForFill.sa_handler = eo670_StackTraceSegvFuncForFill;
/*UNIX*/        saForFill.sa_flags   = 0;
/*UNIX*/        sigemptyset( &saForFill.sa_mask );
/*UNIX*/
/*UNIX*/        sigaction(SIGSEGV, &saForFill, &oldSigsegvForFill);
/*UNIX*/        sigaction(SIGBUS, &saForFill, &oldSigbusForFill);
/*UNIX*/
/*UNIX*/        sigfillset(&mask_setForFill);
/*UNIX*/        sigdelset(&mask_setForFill, SIGSEGV);
/*UNIX*/        sigdelset(&mask_setForFill, SIGBUS);
/*UNIX*/        sigprocmask(SIG_SETMASK, &mask_setForFill, &old_setForFill);
/*UNIX*/
#endif /* HAS_SETJMP_BUG */
/*UNIX*/        eo670_UnixFillCallArray(skipCount, maxLevel, pFilledLevel, addCallInfo, callArray, pBTraceContext, pOutput, pOutContext);
#ifndef HAS_SETJMP_BUG
/*UNIX*/    }
/*UNIX*/    sigaction(SIGSEGV, &oldSigsegvForFill, 0);
/*UNIX*/    sigaction(SIGBUS, &oldSigbusForFill, 0);
/*UNIX*/    sigprocmask(SIG_SETMASK, &old_setForFill, (void*)0);
#endif /* HAS_SETJMP_BUG */
/*UNIX*/}
/*UNIX*/
/*--------------------------
  Signal handling code
 */
/*UNIX*/
/*UNIX*/static struct sigaction oldSigbus;
/*UNIX*/static struct sigaction oldSigsegv;
/*UNIX*/
#  if defined(LINUX) || defined(SOLARIS) || defined(SUN) || defined(FREEBSD)
/*SOLARIS+LINUX*/static sigjmp_buf eo670_emergency;
#  else
/*AIX+OSF1*/static jmp_buf eo670_emergency;
#  endif /* LINUX || SOLARIS */
/*UNIX*/
/*UNIX*//*	This is the stack trace back SIGBUS/SIGSEGV signal handler. */
/*UNIX*/static void eo670_StackTraceSegvFunc(int Signal)
/*UNIX*/{
/*UNIX*/    siglongjmp(eo670_emergency, Signal != 0 ? Signal : 1);
/*UNIX*/}
/*UNIX*/
/*--------------------------
  End of signal handling code
 */

/* Number of stack frame entries to skip if called without signal context */
#  if defined(OSF1)
#    define SKIP_COUNT 4
#  elif defined(HPUX) || defined(AIX)
#    if defined(HP_IA64)
#      define SKIP_COUNT 3
#    else
#      define SKIP_COUNT 3
#    endif
#  else
#    define SKIP_COUNT 2
#  endif

#  ifdef HAS_NO_ADDITIONAL_FRAME_INFO
/*UNIX*/static void eo670_UnixShowCallInfoAddon( eo670_CallInfo *      callInfo,
/*UNIX*/                                         OutputCallBackFuncPtr pOutput,
/*UNIX*/                                         void * const          pOutContext )
/*UNIX*/{
/*UNIX*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*UNIX*/         "      Frameinfo [0x%lx]\n",
/*UNIX*/         (long)callInfo->frame );
/*UNIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/}
#  endif /* HAS_NO_ADDITIONAL_FRAME_INFO */

/*UNIX*/void eo670_UnixTraceStack( void *                pBTraceContext,
/*UNIX*/                           OutputCallBackFuncPtr pOutput,
/*UNIX*/                           void * const          pOutContext )
/*UNIX*/{
#ifndef HAS_SETJMP_BUG
/*UNIX*/static void *           old_sigsegv;
/*UNIX*/static void *           old_sigbus;
/*UNIX*/static sigset_t         old_set;
/*UNIX*/static sigset_t         mask_set;
/*UNIX*/static int              rc;
#endif /* HAS_SETJMP_BUG */
/*UNIX*/static eo670AddressInfo localAddrInfo;
/*UNIX*/static char *           pcAddr;
/*UNIX*/static int              i;
/*UNIX*/static const char *     routine = "<unknown>";
/*UNIX*/static SAPDB_Int4       stackDepth;
#  define MAX_STACK     (256)
/*UNIX*/static eo670_CallInfo   callInfoArray[MAX_STACK];
/*UNIX*/static int              nextAction;
/*UNIX*/
#ifndef HAS_SETJMP_BUG
/*UNIX*//* 	First insure that we do not cause a segment violation when we look for  */
/*UNIX*//* 	the TraceBack table in the code segment.  So, we add a signal handle    */
/*UNIX*//* 	for SIGSEGV and SIGBUS.                                                 */
/*UNIX*/    rc = sigsetjmp( eo670_emergency, 1);
/*UNIX*/    if ( rc == 0 )
/*UNIX*/    {
/*UNIX*/        static struct sigaction sa;
/*UNIX*/
/*UNIX*/        sa.sa_handler = eo670_StackTraceSegvFunc;
/*UNIX*/        sa.sa_flags   = 0;
/*UNIX*/        sigemptyset( &sa.sa_mask );
/*UNIX*/
/*UNIX*/        sigaction(SIGSEGV, &sa, &oldSigsegv);
/*UNIX*/        sigaction(SIGBUS, &sa, &oldSigbus);
/*UNIX*/
/*UNIX*/        sigfillset(&mask_set);
/*UNIX*/        sigdelset(&mask_set, SIGSEGV);
/*UNIX*/        sigdelset(&mask_set, SIGBUS);
/*UNIX*/        sigprocmask(SIG_SETMASK, &mask_set, &old_set);
#endif /* HAS_SETJMP_BUG */
/*UNIX*/
/*UNIX*/        nextAction = 1;
#ifndef HAS_SETJMP_BUG
/*UNIX*/    }
/*UNIX*/    else
/*UNIX*/    {
/*UNIX*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*UNIX*/           "*** Error in stack backtrace: Signal %d in %s\n", rc, routine);
/*UNIX*/        eo670WriteOutput ( eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/        ++nextAction;
/*UNIX*/    }
#endif /* HAS_SETJMP_BUG */
/*UNIX*/
/*UNIX*/    while( 0 != nextAction )
/*UNIX*/    {
/*UNIX*/        switch(nextAction)
/*UNIX*/        {
/*UNIX*/        case 1:
#ifndef HAS_SETJMP_BUG
/*UNIX*/            eo670WriteOutput("----> Emergency Stack Back Trace <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/            routine = "eo670_UnixFillCallArray(OutputOnly)";
/*UNIX*/            eo670_SaveFillCallArray (SKIP_COUNT, MAX_STACK, &stackDepth, eo670_NoArrayJustOutput, (void**)&callInfoArray[0], pBTraceContext, pOutput, pOutContext);
#else
/*UNIX*/            eo670WriteOutput("----> Emergency Stack Back Trace skipped (due to buggy sigsetjmp ...) <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/            stackDepth = 20; /* half harted attempt to prevent SIGSEGV during symbolic traceback... */
#endif /* HAS_SETJMP_BUG */
/*UNIX*/            ++nextAction;
/*UNIX*/            break;
/*UNIX*/
/*UNIX*/        case 2:
/*UNIX*/            if ( pBTraceContext )
/*UNIX*/            {
/*UNIX*/                eo670WriteOutput("----> Register Dump <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/                routine = "eo670_RegisterDump";
/*UNIX*/                eo670_RegisterDump(pBTraceContext,pOutput, pOutContext);
/*UNIX*/            }
/*UNIX*/            ++nextAction;
/*UNIX*/            break;
/*UNIX*/
/*UNIX*/        case 3:
/*UNIX*/            eo670WriteOutput("----> Module List <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/            routine = "eo670_UpdateModuleChain";
/*UNIX*/            eo670_UpdateModuleChain(pOutput, pOutContext);
/*UNIX*/
/*UNIX*/            routine = "eo670_ShowLoadedModules";
/*UNIX*/            eo670_ShowLoadedModules(pOutput, pOutContext);
/*UNIX*/            ++nextAction;
/*UNIX*/            break;
/*UNIX*/
/*UNIX*/        case 4:
/*UNIX*/           eo670WriteOutput("----> Symbolic Stack Back Trace <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/* This is still needed, since shl_getsymbols does not return all symbols (HPUX 32Bit only) (but nm does...) */
/*UNIX*/           routine = "eo670_UnixFillCallArray";
/*UNIX*/           eo670_SaveFillCallArray (SKIP_COUNT, stackDepth, &stackDepth, eo670_FullInfoArray, (void**)&callInfoArray[0], pBTraceContext, pOutput, pOutContext);
/*UNIX*/           for ( i = 0; i < stackDepth; i++ )
/*UNIX*/           {
/*UNIX*/    	       pcAddr = (char *)(callInfoArray[i].pcAddr);
/*UNIX*/
/*UNIX*/               routine = "eo670_FillAddressInfo";
/*UNIX*/               eo670_SaveFillAddressInfo(pcAddr, &localAddrInfo, pOutput, pOutContext);
/*UNIX*/
/*UNIX*/               routine = "eo670_ShowAddressInfo";
/*UNIX*/               eo670_ShowAddressInfo(i, pcAddr, &localAddrInfo, pOutput, pOutContext);
/*UNIX*/
/*UNIX*/               routine = "eo670_UnixShowCallInfoAddon";
/*UNIX*/               eo670_UnixShowCallInfoAddon(&callInfoArray[i], pOutput, pOutContext);
/*UNIX*/
/*UNIX*/               /* Main stack ends here... */
/*UNIX*/               if (strcmp (localAddrInfo.symbol, "main") == 0)
/*UNIX*/                   break;
/*UNIX*/
/*UNIX*/               /* UKT Stack ends here... */
/*UNIX*/               if (strcmp (localAddrInfo.symbol, "en88_RunPthreadTask") == 0)
/*UNIX*/                   break;
/*UNIX*/
/*UNIX*/               /* or here... */
/*UNIX*/               if (strcmp (localAddrInfo.symbol, "en88RunCoroutineUKT") == 0)
/*UNIX*/                   break;
/*UNIX*/
/*UNIX*/               /* Coroutine task ends here... */
/*UNIX*/               if (strcmp (localAddrInfo.symbol, "en88_CallCoroutineKernelTaskMain") == 0)
/*UNIX*/                   break;
#  ifdef AIX
/*AIX*/                /* Thread stack ends here... */
/*AIX*/                if (strcmp (localAddrInfo.symbol, "_uc_trampoline") == 0)
/*AIX*/                    break;
/*AIX*/
#  endif /* AIX */
/*UNIX*/            } /* end for loop over stack-array */
/*UNIX*/            ++nextAction;
/*UNIX*/            break;
/*UNIX*/
/*UNIX*/        default:
/*UNIX*/            nextAction = 0;
/*UNIX*/            eo670WriteOutput("----> End of Stack Back Trace <----\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*UNIX*/            break;
/*UNIX*/        }
/*UNIX*/    }
/*UNIX*/
#ifndef HAS_SETJMP_BUG
/*UNIX*/    sigaction(SIGSEGV, &oldSigsegv, 0);
/*UNIX*/    sigaction(SIGBUS, &oldSigbus, 0);
/*UNIX*/    sigprocmask(SIG_SETMASK, &old_set, (void*)0);
#endif /* HAS_SETJMP_BUG */
/*UNIX*/    return;
/*UNIX*/}

/*------------------------------------------------------------------------------------------*/

#  if defined(SOLARIS) || defined(SUN) || defined(LINUX) || defined (FREEBSD) || (defined(HPUX) && defined(BIT64))
#    define HAS_ELF_SUPPORT
#  else
#    undef  HAS_ELF_SUPPORT
#  endif

#  ifdef HAS_ELF_SUPPORT
#    ifdef BIT64
/*ELF*/typedef Elf64_Sym  Elf_Symbol;
/*ELF*/typedef Elf64_Ehdr Elf_ElfHeader;
/*ELF*/typedef Elf64_Shdr Elf_SectionHeader;
#ifndef FREEBSD		/* already defined in sys/elf_generic.h */
/*ELF*/typedef Elf64_Word Elf_Word;
/*ELF*/typedef Elf64_Addr Elf_Addr;
#endif
#      define ELF_ST_TYPE(_i) ELF64_ST_TYPE(_i)
#    else
/*ELF*/typedef Elf32_Sym  Elf_Symbol;
/*ELF*/typedef Elf32_Ehdr Elf_ElfHeader;
/*ELF*/typedef Elf32_Shdr Elf_SectionHeader;
/*ELF*/typedef Elf32_Word Elf_Word;
/*ELF*/typedef Elf32_Addr Elf_Addr;
#      define ELF_ST_TYPE(_i) ELF32_ST_TYPE(_i)
#    endif /* BIT64 */
#  endif /* HAS_ELF_SUPPORT */

#  ifdef HAS_ELF_SUPPORT
/*
  Function: eo670_ElfUpdateBestSymbol

  Helper routine, that updates the bestEntry and bestOffset found so far scanning the given symbol array
  If a better entry is found *pBestEntry and *pBestOffset are updated. If a matching entry was found,
  1 is returned else 0.
 */
/*ELF*/static int eo670_ElfUpdateBestSymbol( Elf_Addr    pcAddr,
/*ELF*/                                      caddr_t     voffset,
/*ELF*/                                      Elf_Word *  pBestEntry,
/*ELF*/                                      SAPDB_Long *pBestOffset,
/*ELF*/                                      Elf_Symbol *bufferedSymtab,
/*ELF*/                                      size_t      workSymCount )
/*ELF*/{
/*ELF*/    static int iSym;
/*ELF*/    for ( iSym = 0; iSym < workSymCount; iSym++ )
/*ELF*/    {
/*ELF*/        if ( bufferedSymtab[iSym].st_name
/*ELF*/         && (ELF_ST_TYPE(bufferedSymtab[iSym].st_info) == STT_FUNC) )
/*ELF*/        {
/*ELF*/            static Elf_Addr symAddr;
#ifdef HPUX
/* Relocatable symbols in HPUX (PARisc + IPF) Elf64 specification contain an address alignment and a 'quadrant' */
/* mark which have to be masked out to allow correct best symbol address seach */
/*ELF*/            symAddr = (Elf_Addr)((((long)bufferedSymtab[iSym].st_value)&(0x3FFFFFFFFFFFFFFCL)) + voffset);
#else
/*ELF*/            symAddr = (Elf_Addr)(((long)bufferedSymtab[iSym].st_value) + voffset);
#endif
/*ELF*/            if ( symAddr == pcAddr )
/*ELF*/            {
/*ELF*/                 *pBestOffset = 0;
/*ELF*/                 *pBestEntry  = bufferedSymtab[iSym].st_name;
/*ELF*/                 return 1;
/*ELF*/            }
/*ELF*/
/*ELF*/            if (  (symAddr  < pcAddr)
/*ELF*/             && ( (*pBestOffset == (Elf_Word)-1) || ((pcAddr - symAddr) < *pBestOffset) ) )
/*ELF*/            {
/*ELF*/                 *pBestOffset = pcAddr - symAddr;
/*ELF*/                 *pBestEntry  = bufferedSymtab[iSym].st_name;
/*ELF*/            }
/*ELF*/        }
/*ELF*/    }
/*ELF*/    return 0;
/*ELF*/}
/*
  Function: eo670_ReadElfSymbol

  Helper routine, that reads a symbol. Symbol is overwritten with each call
 */
/*ELF*/static int eo670_ReadElfSymbol( int                   fd,
/*ELF*/                                Elf_Word              stringTabOffset,
/*ELF*/                                Elf_Word              entry,
/*ELF*/                                SAPDB_Char **         pSymbol,
/*ELF*/                                OutputCallBackFuncPtr pOutput,
/*ELF*/                                void * const          pOutContext )
/*ELF*/{
/*ELF*/    static SAPDB_Char lastSymbolBuffer[MAX_SYMBOL_LENGTH];
/*ELF*/
/*ELF*/    *pSymbol = (char *)"";
/*ELF*/    if ( lseek(fd, stringTabOffset + entry, SEEK_SET) != stringTabOffset + entry )
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Failed to seek to symbol string offset\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return 0;
/*ELF*/    }
/*ELF*/    if ( read(fd, lastSymbolBuffer, sizeof(lastSymbolBuffer)-1) <= 0 )
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Failed to read symbol from file\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return 0;
/*ELF*/    }
/*ELF*/    *pSymbol = lastSymbolBuffer;
/*ELF*/    return 1;
/*ELF*/}
/*ELF*/
/*
  Function: eo670_ELFFindSymbol

  Since symbol resolution should not use giant amount of memory (i.e. APO Dll tend to have a lot of symbols...
  this routine find the symbol for exactly one address by looking up symbol table without copying it into memory
  This will not produce a lot of seek overhead, since the actual symbol value of interest, once found, needs
  only one specific seek operation into the string table. The symbol table can be scanned in a single pass
  to find the best fitting symbol.
 */
/*ELF*/static int eo670_ELFFindSymbol( int                   fd,
/*ELF*/                                char *                pcAddr,
/*ELF*/                                caddr_t               voffset,
/*ELF*/                                SAPDB_Char **         pSymbol,
/*ELF*/                                SAPDB_Long *          pOffsetToSymbol,
/*ELF*/                                OutputCallBackFuncPtr pOutput,
/*ELF*/                                void * const          pOutContext )
/*ELF*/{
/*ELF*/  static  Elf_ElfHeader     elfhdr;           /* Elf header                (read once) */
/*ELF*/  static  Elf_SectionHeader symbolSectionHdr; /* Elf symbol section header (read once) */
/*ELF*/  static  Elf_SectionHeader elfSectionHdr;    /* temp Elf section header */
/*ELF*/  static  Elf_Word          stringSection;
/*ELF*/  static  Elf_Word          stringTabOffset;
/*ELF*/  static  int               i;
/*ELF*/  static  size_t            syms;
/*ELF*/  static  Elf_Word          bestEntry;
/*ELF*/  static  SAPDB_Long        bestOffset;
#    define MAX_BUFFERED_SYMBOLS 4096
/*ELF*/  static  Elf_Symbol        bufferedSymtab[MAX_BUFFERED_SYMBOLS];
/*ELF*/  static  SAPDB_Int8        systemRc;
/*ELF*/
/*ELF*/    /* Get the ELF header                                             */
/*ELF*/
/*ELF*/    if ( (systemRc = read (fd, &elfhdr, sizeof(elfhdr))) != sizeof(elfhdr)){
/*ELF*/        eo670WriteOutput("Unable to read ELF header \n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
#    if defined(SOLARIS) || defined(SUN) || defined(LINUX) || defined (FREEBSD)
/*ELF*/    if (elfhdr.e_type == ET_EXEC)
/*ELF*/        voffset = 0;
#    endif /* SOLARIS || LINUX || FREEBSD */
/*ELF*/
/*ELF*/    if (elfhdr.e_shnum == 0) {
/*ELF*/        eo670WriteOutput("No section header entries.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    if (elfhdr.e_shentsize > sizeof(elfSectionHdr) ) {
/*ELF*/        eo670WriteOutput("Elf section header entries larger than expected.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    /* Seek to the section header table                                   */
/*ELF*/
/*ELF*/    if ( (systemRc = lseek(fd, (long) elfhdr.e_shoff, SEEK_SET)) != elfhdr.e_shoff) {
/*ELF*/        eo670WriteOutput("Unable to seek to section header.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    stringSection = 0;
/*ELF*/
/*ELF*/    for (i = 0; i < (int) elfhdr.e_shnum; i++ )
/*ELF*/    {
/*ELF*/        if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*ELF*/        {
/*ELF*/            eo670WriteOutput("Unable to read ELF header section.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/            return(0);
/*ELF*/        }
/*ELF*/        if ( elfSectionHdr.sh_type ==  SHT_SYMTAB )
/*ELF*/        {
/*ELF*/            SAPDB_memcpy(&symbolSectionHdr, &elfSectionHdr, sizeof(elfSectionHdr));
/*ELF*/            stringSection = elfSectionHdr.sh_link;
/*ELF*/        }
/*ELF*/    }
/*ELF*/
/*ELF*/    if ( 0 == stringSection
/*ELF*/      || stringSection >= elfhdr.e_shnum )
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Warning - no ELF string section defined.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    /*Skip section up to wanted string section */
/*ELF*/    if ( (systemRc = lseek(fd, (long) elfhdr.e_shoff, SEEK_SET)) != elfhdr.e_shoff) {
/*ELF*/        eo670WriteOutput("Unable to do second seek to ELF section headers.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    for (i = 0; i < stringSection; i++ )
/*ELF*/    {
/*ELF*/        if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*ELF*/        {
/*ELF*/            eo670WriteOutput("Unable to read ELF header section.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/            return(0);
/*ELF*/        }
/*ELF*/    }
/*ELF*/
/*ELF*/    if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Unable to read ELF string section header.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    if( elfSectionHdr.sh_size == 0)
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Warning - empty ELF string table; no symbols.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    stringTabOffset = elfSectionHdr.sh_offset;
/*ELF*/
/*ELF*/    /* calculate the number of symbols found */
/*ELF*/    syms = symbolSectionHdr.sh_size/symbolSectionHdr.sh_entsize;
/*ELF*/
/*ELF*/    bestEntry  = (Elf_Word)-1;
/*ELF*/    bestOffset = (SAPDB_Long)-1;
/*ELF*/
/*ELF*/    /*Seek to symbol table section */
/*ELF*/    if ( (systemRc = lseek(fd, (long) symbolSectionHdr.sh_offset, SEEK_SET)) != symbolSectionHdr.sh_offset) {
/*ELF*/        eo670WriteOutput("Unable to do seek to ELF symbol table\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return(0);
/*ELF*/    }
/*ELF*/
/*ELF*/    while ( syms > 0 )
/*ELF*/    {
/*ELF*/        static int workSymCount; /* number of symbols handled in this loop */
/*ELF*/
/*ELF*/        workSymCount = (syms > MAX_BUFFERED_SYMBOLS ? MAX_BUFFERED_SYMBOLS : syms);
/*ELF*/
/*ELF*/        if ( ( systemRc = read(fd, &bufferedSymtab[0], workSymCount * sizeof(Elf_Symbol)) )
/*ELF*/                                  != (workSymCount * sizeof(Elf_Symbol)) )
/*ELF*/        {
/*ELF*/            sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*ELF*/                        "Warning - reading ELF symbol table failed; got %d expected %d error %d\n",
/*ELF*/                        systemRc, workSymCount * sizeof(Elf_Symbol), errno);
/*ELF*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/            return(0);
/*ELF*/        }
/*ELF*/
/*ELF*/        if ( eo670_ElfUpdateBestSymbol( (Elf_Addr)pcAddr, voffset,
/*ELF*/                                        &bestEntry, &bestOffset,
/*ELF*/                                        &bufferedSymtab[0], workSymCount) )
/*ELF*/        {
/*ELF*/            break; /* Find the best matching symbol */
/*ELF*/        }
/*ELF*/
/*ELF*/        syms -= workSymCount;
/*ELF*/    }
/*ELF*/
/*ELF*/    if ( bestEntry == -1 )
/*ELF*/    {
/*ELF*/        eo670WriteOutput("Symbol lookup failed\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*ELF*/        return 0;
/*ELF*/    }
/*ELF*/
/*ELF*/   *pOffsetToSymbol = bestOffset;
/*ELF*/
/*ELF*/   return eo670_ReadElfSymbol(fd, stringTabOffset, bestEntry, pSymbol, pOutput, pOutContext);
/*ELF*/}
#  endif /* HAS_ELF_SUPPORT */

#    define USRTEXT  0x2000

#  if defined LINUX
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static int AnalyzeProcMapEntry( char *procMapEntry
/*LINUX*/                              , caddr_t *pVaddr
/*LINUX*/                              , caddr_t *pVendaddr
/*LINUX*/                              , char **pFileName )
/*LINUX*/{
/*LINUX*/    static char * pStartVaddr;
/*LINUX*/    static char * pStartVendaddr;
/*LINUX*/    static iChar;
/*LINUX*/    iChar = 0;
/*LINUX*/
/*LINUX*/    pStartVaddr = procMapEntry;
/*LINUX*/    while ( 0 != procMapEntry[iChar] )
/*LINUX*/    {
/*LINUX*/        ++iChar;
/*LINUX*/        if ( '-' == procMapEntry[iChar] )
/*LINUX*/        {
/*LINUX*/             ++iChar;
/*LINUX*/             break;
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    pStartVendaddr = &procMapEntry[iChar];
/*LINUX*/    while ( procMapEntry[iChar] )
/*LINUX*/    {
/*LINUX*/        ++iChar;
/*LINUX*/        if ( ' ' == procMapEntry[iChar] )
/*LINUX*/        {
/*LINUX*/             ++iChar;
/*LINUX*/             break;
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    if ( 0 == procMapEntry[iChar]
/*LINUX*/      || 0 != strncmp("r-xp ", &procMapEntry[iChar], 5) )
/*LINUX*/    {
/*LINUX*/        return 0;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    iChar += 5;
/*LINUX*/    while ( procMapEntry[iChar] )      /* skip size */
/*LINUX*/    {
/*LINUX*/        ++iChar;
/*LINUX*/        if ( ' ' == procMapEntry[iChar] )
/*LINUX*/        {
/*LINUX*/             ++iChar;
/*LINUX*/             break;
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    while ( procMapEntry[iChar] )      /* skip dd:dd */
/*LINUX*/    {
/*LINUX*/        ++iChar;
/*LINUX*/        if ( ' ' == procMapEntry[iChar] )
/*LINUX*/        {
/*LINUX*/             ++iChar;
/*LINUX*/             break;
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    while ( procMapEntry[iChar] )      /* skip ???? */
/*LINUX*/    {
/*LINUX*/        ++iChar;
/*LINUX*/        if ( ' ' == procMapEntry[iChar] )
/*LINUX*/        {
/*LINUX*/             while ( ' ' == procMapEntry[iChar] )
/*LINUX*/             {
/*LINUX*/                 ++iChar;
/*LINUX*/             }
/*LINUX*/             break;
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    if ( 0 == procMapEntry[iChar] )
/*LINUX*/    {
/*LINUX*/        return 0;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    *pVaddr = (caddr_t) strtoul(pStartVaddr, NULL, 16);
/*LINUX*/    *pVendaddr = (caddr_t) strtoul(pStartVendaddr, NULL, 16);
/*LINUX*/    *pFileName = &procMapEntry[iChar];
/*LINUX*/    return 1;
/*LINUX*/}
/*LINUX*/
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static void PrepareProcMap(char *procMap, char *endProcMap)
/*LINUX*/{
/*LINUX*/  static char *p;
/*LINUX*/
/*LINUX*/    p = procMap;
/*LINUX*/    while ( p < endProcMap ) {
/*LINUX*/        if ( *p == '\n' ) *p = 0;
/*LINUX*/        ++p;
/*LINUX*/    }
/*LINUX*/}
/*LINUX*/
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static char *NextProcMap( char *                procMap,
/*LINUX*/                          size_t *              pBufferedChars,
/*LINUX*/                          int                   proc_fd,
/*LINUX*/                          OutputCallBackFuncPtr pOutput,
/*LINUX*/                          void * const          pOutContext )
/*LINUX*/{
/*LINUX*/  static char *p;
/*LINUX*/  static size_t bytesStored;
/*LINUX*/  static int    bytesRead;
/*LINUX*/
/*LINUX*/    bytesStored = *pBufferedChars;
/*LINUX*/
/*LINUX*/    p = procMap + strlen(procMap);
/*LINUX*/    while ( *p == 0 && p < (procMap + bytesStored) ) {
/*LINUX*/        ++p;
/*LINUX*/    }
/*LINUX*/    bytesStored -= (p-procMap);
/*LINUX*/
/*LINUX*/    SAPDB_memcpy(procMap, p, bytesStored);
/*LINUX*/    bytesRead = read(proc_fd, procMap + bytesStored, 1024 - bytesStored );
/*LINUX*/    if ( bytesRead < 0 )
/*LINUX*/    {
/*LINUX*/        eo670WriteOutput("cant read from /proc file\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        return ((void *)0);
/*LINUX*/    }
/*LINUX*/    PrepareProcMap(procMap + bytesStored, procMap + bytesStored + bytesRead);
/*LINUX*/
/*LINUX*/    *pBufferedChars = bytesStored + bytesRead;
/*LINUX*/
/*LINUX*/    return ( *pBufferedChars > 0 ? procMap : NULL );
/*LINUX*/}
/*LINUX*/
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/

/* Structure of /proc/xxx/maps file entry: HexVAddr-HexVEndAddr rwxp HexOffset 00:00 <11ByteSize>FileName */
/* Size of HexAddr = 2 * sizeof(void *) but varies on SDBonPPC64 */
/*LINUX*/
/*LINUX*/static SAPDB_Bool eo670_UpdateModuleChain( OutputCallBackFuncPtr pOutput,
/*LINUX*/                                     void * const          pOutContext )
/*LINUX*/{
/*LINUX*/  static int       fd_proc;	/* /proc fd                           */
/*LINUX*/  static char      procMapBuffer[1024];
/*LINUX*/  static char     *procMap;
/*LINUX*/  static size_t    bufferedChars; /* Number of currently buffered character */
/*LINUX*/  static caddr_t   vaddr;
/*LINUX*/  static caddr_t   vendaddr;
/*LINUX*/  static char     *fileName;
/*LINUX*/
/*LINUX*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer), "/proc/%d/maps", getpid());
/*LINUX*/
/*LINUX*/    if ((fd_proc = open(eo670MessageBuffer, O_RDONLY)) == -1)
/*LINUX*/    {
/*LINUX*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                  "Cant open /proc/%d/maps : %d\n", getpid(), errno);
/*LINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        return false;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    if ( (bufferedChars = read(fd_proc, procMapBuffer, sizeof(procMapBuffer))) <= 0 )
/*LINUX*/    {
/*LINUX*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                  "Cant read from /proc/%d/maps : %d\n", getpid(), errno);
/*LINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        close(fd_proc);
/*LINUX*/        return false;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    procMap = &procMapBuffer[0];
/*LINUX*/    /* Split lines into zero terminated strings */
/*LINUX*/    PrepareProcMap(procMap, procMap + bufferedChars);
/*LINUX*/
/*LINUX*/    do
/*LINUX*/    {
/*LINUX*/        if ( !AnalyzeProcMapEntry( procMap, &vaddr, &vendaddr, &fileName ) )
/*LINUX*/            continue;
/*LINUX*/
/*LINUX*/        if ( eo670_AddToLoadedModuleChain( fileName,
/*LINUX*/                                           vaddr,
/*LINUX*/                                           vendaddr,
/*LINUX*/                                           0,
/*LINUX*/                                           0,
/*LINUX*/                                           pOutput, pOutContext) < 0 )
/*LINUX*/        {
/*LINUX*/            close(fd_proc);
/*LINUX*/            return false;
/*LINUX*/        }
/*LINUX*/    } while ( 0 != NextProcMap(procMap, &bufferedChars, fd_proc, pOutput, pOutContext) );
/*LINUX*/
/*LINUX*/    (void) close(fd_proc);
/*LINUX*/    return true;
/*LINUX*/}
/*LINUX*/

#    if defined (LINUX) && defined(I386) && !defined(HAS_LIBUNWIND_IMPLEMENTATION)
/*LINUX*//*---------------------------------------------------------------------------*/
#      ifdef LINUX_WITHOUT_SIGINFO
/*OLDLINUX*/
/*OLDLINUX*/        /* libc5 doesn't have <sigcontext.h>: go directly with the kernel   */
/*OLDLINUX*/        /* one.  Check LINUX_VERSION_CODE to see which we should reference. */
/*OLDLINUX*/
/*OLDLINUX*/struct LinuxGccI386ExceptionFrame {
/*OLDLINUX*/    struct LinuxGccI386Frame *fr_savfp;
/*OLDLINUX*/    long   dummy_pc;
/*OLDLINUX*/    long   dummy[40]; /* magic number... but it works.... */
/*OLDLINUX*/    struct sigcontext fr_context;
/*OLDLINUX*/
/*OLDLINUX*/
/*OLDLINUX*/struct LinuxGccI386SignalHandlerFrame
/*OLDLINUX*/{
/*OLDLINUX*/    struct LinuxGccI386ExceptionFrame *fr_savfp;
/*OLDLINUX*/    long  dummy_pc;
/*OLDLINUX*/    int   signal;
/*OLDLINUX*/    long  dummy_arg2;
/*OLDLINUX*/    long  dummy_arg3;
/*OLDLINUX*/    long  dummy_arg4;
/*OLDLINUX*/};
/*OLDLINUX*/
/*OLDLINUX*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*OLDLINUX*/                                OutputCallBackFuncPtr pOutput,
/*OLDLINUX*/                                void * const          pOutContext )
/*OLDLINUX*/{
/*OLDLINUX*/    static struct sigcontext *context;
/*OLDLINUX*/    static int iReg;
/*OLDLINUX*/
/*OLDLINUX*/    context = (struct sigcontext *)pBTraceContext;
/*OLDLINUX*/
/*OLDLINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "ds   es   0x%08x 0x%08x\n",
/*OLDLINUX*/                 *(long*)&context->ds,
/*OLDLINUX*/                 *(long*)&context->es );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "fs   gs   0x%08x 0x%08x\n",
/*OLDLINUX*/                 *(long*)&context->fs,
/*OLDLINUX*/                 *(long*)&context->gs );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "trap err  0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->trapno,
/*OLDLINUX*/                 context->err );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "efl  ss   0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->eflags,
/*OLDLINUX*/                 *(long*)&context->ss );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "edi  esi  0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->edi,
/*OLDLINUX*/                 context->esi );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "eax  ebx  0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->eax,
/*OLDLINUX*/                 context->ebx );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "ecx  edx  0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->ecx,
/*OLDLINUX*/                 context->edx );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "esp  uesp 0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->esp,
/*OLDLINUX*/                 context->esp_at_signal );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "ebp  eip  0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->ebp,
/*OLDLINUX*/                 context->eip );
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "cs   cr2  0x%08x 0x%08x\n",
/*OLDLINUX*/                 *(long*)&context->cs,
/*OLDLINUX*/                 context->cr2);
/*OLDLINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/    if ( context->fpstate && 0 == (((long)context->fpstate)&3) )
/*OLDLINUX*/    {
/*OLDLINUX*/        sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "cw - sw - tag   0x%08x 0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->fpstate->cw,
/*OLDLINUX*/                 context->fpstate->sw,
/*OLDLINUX*/                 context->fpstate->tag);
/*OLDLINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/        sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "ipoff - cssel   0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->fpstate->ipoff,
/*OLDLINUX*/                 context->fpstate->cssel);
/*OLDLINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/        sp77sprintf( eo670MessageBuffer,
/*OLDLINUX*/                 sizeof(eo670MessageBuffer),
/*OLDLINUX*/                 "dataoff-datasel 0x%08x 0x%08x\n",
/*OLDLINUX*/                 context->fpstate->dataoff,
/*OLDLINUX*/                 context->fpstate->datasel);
/*OLDLINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/
/*OLDLINUX*/        for ( iReg = 0; iReg < sizeof(context->fpstate->_st)/sizeof(struct _fpreg); iReg++ )
/*OLDLINUX*/        {
/*OLDLINUX*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OLDLINUX*/                     "f%02d      [0x%04x 0x%04x 0x%04x 0x%04x - 0x%04x]\n",
/*OLDLINUX*/                      iReg,
/*OLDLINUX*/                      context->fpstate->_st[iReg].significand[0],
/*OLDLINUX*/                      context->fpstate->_st[iReg].significand[1],
/*OLDLINUX*/                      context->fpstate->_st[iReg].significand[2],
/*OLDLINUX*/                      context->fpstate->_st[iReg].significand[3],
/*OLDLINUX*/                      context->fpstate->_st[iReg].exponent );
/*OLDLINUX*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/        }
/*OLDLINUX*/    }
/*OLDLINUX*/    else
/*OLDLINUX*/    {
/*OLDLINUX*/        eo670WriteOutput( "Floating point register unused\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/    }
/*OLDLINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OLDLINUX*/}
#       else /* LINUX_WITHOUT_SIGINFO */
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*LINUX*/                                OutputCallBackFuncPtr pOutput,
/*LINUX*/                                void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    static ucontext_t *context;
/*LINUX*/    static int iReg;
/*LINUX*/
/*LINUX*/    context = (ucontext_t *)pBTraceContext;
/*LINUX*/
/*LINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "ds   es   0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_DS],
/*LINUX*/                 context->uc_mcontext.gregs[REG_ES] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "fs   gs   0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_FS],
/*LINUX*/                 context->uc_mcontext.gregs[REG_GS] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "trap err  0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_TRAPNO],
/*LINUX*/                 context->uc_mcontext.gregs[REG_ERR] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "efl  ss   0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_EFL],
/*LINUX*/                 context->uc_mcontext.gregs[REG_SS] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "edi  esi  0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_EDI],
/*LINUX*/                 context->uc_mcontext.gregs[REG_ESI] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "eax  ebx  0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_EAX],
/*LINUX*/                 context->uc_mcontext.gregs[REG_EBX] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "ecx  edx  0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_ECX],
/*LINUX*/                 context->uc_mcontext.gregs[REG_EDX] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "esp  uesp 0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_ESP],
/*LINUX*/                 context->uc_mcontext.gregs[REG_UESP] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "ebp  eip  0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_EBP],
/*LINUX*/                 context->uc_mcontext.gregs[REG_EIP] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "cs   cr2  0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.gregs[REG_CS],
/*LINUX*/                 context->uc_mcontext.cr2);
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/    if ( context->uc_mcontext.fpregs && 0 == (((long)context->uc_mcontext.fpregs)&3) )
/*LINUX*/    {
/*LINUX*/        sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "cw - sw - tag   0x%08x 0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.fpregs->cw,
/*LINUX*/                 context->uc_mcontext.fpregs->sw,
/*LINUX*/                 context->uc_mcontext.fpregs->tag);
/*LINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/        sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "ipoff - cssel   0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.fpregs->ipoff,
/*LINUX*/                 context->uc_mcontext.fpregs->cssel);
/*LINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/        sp77sprintf( eo670MessageBuffer,
/*LINUX*/                 sizeof(eo670MessageBuffer),
/*LINUX*/                 "dataoff-datasel 0x%08x 0x%08x\n",
/*LINUX*/                 context->uc_mcontext.fpregs->dataoff,
/*LINUX*/                 context->uc_mcontext.fpregs->datasel);
/*LINUX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/
/*LINUX*/        for ( iReg = 0; iReg < sizeof(context->uc_mcontext.fpregs->_st)/sizeof(struct _libc_fpreg); iReg++ )
/*LINUX*/        {
/*LINUX*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                     "f%02d      [0x%04x 0x%04x 0x%04x 0x%04x - 0x%04x]\n",
/*LINUX*/                      iReg,
/*LINUX*/                      context->uc_mcontext.fpregs->_st[iReg].significand[0],
/*LINUX*/                      context->uc_mcontext.fpregs->_st[iReg].significand[1],
/*LINUX*/                      context->uc_mcontext.fpregs->_st[iReg].significand[2],
/*LINUX*/                      context->uc_mcontext.fpregs->_st[iReg].significand[3],
/*LINUX*/                      context->uc_mcontext.fpregs->_st[iReg].exponent );
/*LINUX*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        }
/*LINUX*/    }
/*LINUX*/    else
/*LINUX*/    {
/*LINUX*/        eo670WriteOutput( "Floating point register unused\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/    }
/*LINUX*/    eo670WriteOutput( "-------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/}
#       endif /* LINUX_WITHOUT_SIGINFO */
/*LINUX*/
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/struct LinuxGccI386Frame {
/*LINUX*/    struct LinuxGccI386Frame *fr_savfp;
/*LINUX*/    unsigned long fr_savpc;
/*LINUX*/    unsigned long fr_arg[4];
/*LINUX*/};
/*LINUX*/
/*LINUX*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*LINUX*/                                    SAPDB_Int4            givenMaxLevel,
/*LINUX*/                                    void **               callArray,
/*LINUX*/                                    OutputCallBackFuncPtr pOutput,
/*LINUX*/                                    void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    struct LinuxGccI386Frame  *fp;
/*LINUX*/    SAPDB_Int4 depth;
/*LINUX*/    SAPDB_Byte * stackLimit;
/*LINUX*/    SAPDB_Byte * oldFp;
/*LINUX*/
/*LINUX*/    depth = 0;
/*LINUX*/
/*LINUX*/    asm( "movl %%ebp,%0" : "=r" (fp) : /* no input operand */ );
/*LINUX*/
/*LINUX*/    stackLimit = eo670_GetStackLimit(fp);
/*LINUX*/    oldFp = ((SAPDB_Byte*)fp)-1;
/*LINUX*/
/*LINUX*/    while ( depth < givenMaxLevel
/*LINUX*/         && ((SAPDB_Byte *)fp) >  oldFp
/*LINUX*/         && ((SAPDB_Byte *)fp) <= stackLimit
/*LINUX*/         && ((((long)fp)&3) == 0) )
/*LINUX*/    {
/*LINUX*/         oldFp = (SAPDB_Byte *)fp;
/*LINUX*/         if ( skipCount > 0 )
/*LINUX*/         {
/*LINUX*/             --skipCount;
/*LINUX*/         }
/*LINUX*/         else
/*LINUX*/         {
/*LINUX*/            callArray[depth] = ((char *)0) + fp->fr_savpc;
/*LINUX*/            ++depth;
/*LINUX*/         }
/*LINUX*/         fp = fp->fr_savfp;
/*LINUX*/    }
/*LINUX*/    return depth;
/*LINUX*/} /* eo670_FastFillCallArray */
/*LINUX*/
/*LINUX*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*LINUX*/                              SAPDB_Int4            maxLevel,
/*LINUX*/                              SAPDB_Int4 *          pFilledLevel,
/*LINUX*/                              eo670_CallArrayType   addCallInfo,
/*LINUX*/                              void **               callArray,
/*LINUX*/                              void*                 pBTraceContext,
/*LINUX*/                              OutputCallBackFuncPtr pOutput,
/*LINUX*/                              void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    static struct LinuxGccI386Frame  *fp;
/*LINUX*/    static struct LinuxGccI386Frame  *oldfp;
/*LINUX*/    static unsigned long pc;
/*LINUX*/    static SAPDB_Int4 givenMaxLevel;
/*LINUX*/
/*LINUX*/    givenMaxLevel = maxLevel;
/*LINUX*/    *pFilledLevel = 0;
/*LINUX*/
/*LINUX*/    if ( 0 == pBTraceContext )
/*LINUX*/    {
/*LINUX*/        pc = (unsigned long)eo670_UnixFillCallArray;
/*LINUX*/        asm( "movl %%ebp,%0" : "=r" (fp) : /* no input operand */ );
/*LINUX*/    }
/*LINUX*/    else
/*LINUX*/    {
#      ifdef LINUX_WITHOUT_SIGINFO
/*OLDLINUX*/     static struct sigcontext *context;
/*OLDLINUX*/
/*OLDLINUX*/     context = &(((struct LinuxGccI386SignalHandlerFrame *)pBTraceContext)->fr_savfp->fr_context);
/*OLDLINUX*/     fp = (struct LinuxGccI386Frame *)(((char *)0)+context->ebp);
/*OLDLINUX*/     pc = context->eip;
#      else /* LINUX_WITHOUT_SIGINFO */
/*LINUX*/        static ucontext_t *context;
/*LINUX*/        context = (ucontext_t *)pBTraceContext;
/*LINUX*/        fp = (struct LinuxGccI386Frame *)(((char *)0)+context->uc_mcontext.gregs[REG_EBP]);
/*LINUX*/        pc = context->uc_mcontext.gregs[REG_EIP];
#      endif /* LINUX_WITHOUT_SIGINFO */
/*LINUX*/        skipCount = 0;
/*LINUX*/        switch ( addCallInfo )
/*LINUX*/        {
/*LINUX*/        case eo670_FullInfoArray:
/*LINUX*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (char *)0 + pc;
/*LINUX*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].frame  = fp;
/*LINUX*/            if ( fp && ((((long)fp)&3) == 0) )
/*LINUX*/            {
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = fp->fr_arg[0];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = fp->fr_arg[1];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = fp->fr_arg[2];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = fp->fr_arg[3];
/*LINUX*/            }
/*LINUX*/            else
/*LINUX*/            {
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = 0xdead;
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = 0xc0de;
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = 0xdeed;
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = 0xc0de;
/*LINUX*/            }
/*LINUX*/            break;
/*LINUX*/        case eo670_PointerArray:
/*LINUX*/            callArray[*pFilledLevel] = ((char *)0) + pc;
/*LINUX*/            break;
/*LINUX*/        default: /* eo670_NoArrayJustOutput */
/*LINUX*/            if ( fp && ((((long)fp)&3) == 0) )
/*LINUX*/            {
/*LINUX*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                         "(%d):0x%lx [0x%lx](0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*LINUX*/                         *pFilledLevel, pc, (long)fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3] );
/*LINUX*/            }
/*LINUX*/            else
/*LINUX*/            {
/*LINUX*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                         "(%d):0x%lx [0x%lx](?,?,?,?)",
/*LINUX*/                         *pFilledLevel, pc, (long)fp );
/*LINUX*/            }
/*LINUX*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/            break;
/*LINUX*/        }
/*LINUX*/        ++*pFilledLevel;
/*LINUX*/    }
/*LINUX*/    oldfp = fp - 1;
/*LINUX*/    while ( *pFilledLevel < givenMaxLevel
/*LINUX*/         && fp
/*LINUX*/         && ((((long)fp)&3) == 0)
/*LINUX*/         && (fp > oldfp)
/*LINUX*/         && ((unsigned long)fp->fr_savfp >= USRTEXT) )
/*LINUX*/    {
/*LINUX*/         if ( skipCount > 0 )
/*LINUX*/         {
/*LINUX*/             --skipCount;
/*LINUX*/         }
/*LINUX*/         else
/*LINUX*/         {
/*LINUX*/            switch ( addCallInfo )
/*LINUX*/            {
/*LINUX*/            case eo670_FullInfoArray:
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (char *)0 + fp->fr_savpc;
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = fp;
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = fp->fr_arg[0];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = fp->fr_arg[1];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = fp->fr_arg[2];
/*LINUX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = fp->fr_arg[3];
/*LINUX*/                break;
/*LINUX*/            case eo670_PointerArray:
/*LINUX*/                callArray[*pFilledLevel] = ((char *)0) + fp->fr_savpc;
/*LINUX*/                break;
/*LINUX*/            default: /* eo670_NoArrayJustOutput */
/*LINUX*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/                         "(%d):0x%lx [0x%lx](0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*LINUX*/                         *pFilledLevel, fp->fr_savpc, (long)fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3] );
/*LINUX*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/                ++*pFilledLevel;
/*LINUX*/                break;
/*LINUX*/            }
/*LINUX*/            ++*pFilledLevel;
/*LINUX*/         }
/*LINUX*/         oldfp = fp;
/*LINUX*/         fp = fp->fr_savfp;
/*LINUX*/    }
/*LINUX*/} /* eo670_UnixFillCallArray */
/*LINUX*/
#    elif defined (LINUX) && defined(X86_64) && !defined(HAS_LIBUNWIND_IMPLEMENTATION)
/*LINUX_X86_64*/
/*LINUX_X86_64*//*---------------------------------------------------------------------------*/
/*LINUX_X86_64*/
/*LINUX_X86_64*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*LINUX_X86_64*/                                OutputCallBackFuncPtr pOutput,
/*LINUX_X86_64*/                                void * const          pOutContext )
/*LINUX_X86_64*/{
/*LINUX_X86_64*/    static ucontext_t *pContext;
/*LINUX_X86_64*/    static int iReg;
/*LINUX_X86_64*/
/*LINUX_X86_64*/    pContext = (ucontext_t *)pBTraceContext;
/*LINUX_X86_64*/    eo670WriteOutput( "-----------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rax rbx      0x%016qx 0x%016qx \n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RAX],
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RBX]);
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rcx rdx      0x%016qx 0x%016qx \n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RCX],
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RDX]);
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rsi rdi      0x%016qx 0x%016qx \n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RSI],
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RDI]);
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rbp rsp      0x%016qx 0x%016qx \n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RBP],
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RSP]);
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    for ( iReg = 0; iReg < 4; iReg++ )
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                     "r%02d r%02d      0x%016qx 0x%016qx\n",
/*LINUX_X86_64*/                      8+(2*iReg),
/*LINUX_X86_64*/                      9+(2*iReg),
/*LINUX_X86_64*/                      pContext->uc_mcontext.gregs[REG_R8+(2*iReg)],
/*LINUX_X86_64*/                      pContext->uc_mcontext.gregs[REG_R9+(2*iReg)] );
/*LINUX_X86_64*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rip eflags   0x%016qx 0x%016qx \n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_RIP],
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_EFL]);
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "cs fs gs     0x%016qx\n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.gregs[REG_CSGSFS] );
/*LINUX_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    eo670WriteOutput( "--------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/    if ( pContext->uc_mcontext.fpregs )
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "cwd swd      0x%04x             0x%04\n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->cwd,
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->swd );
/*LINUX_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "ftw fop      0x%04x             0x%04\n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->ftw,
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->fop );
/*LINUX_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "rip rdp      0x%016qx 0x%016qx]\n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->rip,
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->rdp );
/*LINUX_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*LINUX_X86_64*/                 sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                 "mxcsr mask   0x%018x         0x%08x]\n",
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->mxcsr,
/*LINUX_X86_64*/                 pContext->uc_mcontext.fpregs->mxcr_mask );
/*LINUX_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/
/*LINUX_X86_64*/        for ( iReg = 0; iReg < 8; iReg++ )
/*LINUX_X86_64*/        {
/*LINUX_X86_64*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                        "st%d      [0x%04x 0x%04x 0x%04x 0x%04x - 0x%04x]\n",
/*LINUX_X86_64*/                         iReg,
/*LINUX_X86_64*/                         pContext->uc_mcontext.fpregs->_st[iReg].significand[0],
/*LINUX_X86_64*/                         pContext->uc_mcontext.fpregs->_st[iReg].significand[1],
/*LINUX_X86_64*/                         pContext->uc_mcontext.fpregs->_st[iReg].significand[2],
/*LINUX_X86_64*/                         pContext->uc_mcontext.fpregs->_st[iReg].significand[3],
/*LINUX_X86_64*/                         pContext->uc_mcontext.fpregs->_st[iReg].exponent );
/*LINUX_X86_64*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/        }
/*LINUX_X86_64*/
/*LINUX_X86_64*/        for ( iReg = 0; iReg < 16; iReg++ )
/*LINUX_X86_64*/        {
/*LINUX_X86_64*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                        "xmm%02d   [0x%08x 0x%08x 0x%08x 0x%08x]\n",
/*LINUX_X86_64*/                         iReg,
/*LINUX_X86_64*/                        pContext->uc_mcontext.fpregs->_xmm[iReg].element[0],
/*LINUX_X86_64*/                        pContext->uc_mcontext.fpregs->_xmm[iReg].element[1],
/*LINUX_X86_64*/                        pContext->uc_mcontext.fpregs->_xmm[iReg].element[2],
/*LINUX_X86_64*/                        pContext->uc_mcontext.fpregs->_xmm[iReg].element[3] );
/*LINUX_X86_64*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/        }
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/    else
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        eo670WriteOutput( "Floating point register unused\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/    eo670WriteOutput( "--------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/}
/*LINUX_X86_64*/
/*LINUX_X86_64*//*---------------------------------------------------------------------------*/
/*LINUX_X86_64*/
/*LINUX_X86_64*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*LINUX_X86_64*/                                    SAPDB_Int4            givenMaxLevel,
/*LINUX_X86_64*/                                    void **               callArray,
/*LINUX_X86_64*/                                    OutputCallBackFuncPtr pOutput,
/*LINUX_X86_64*/                                    void * const          pOutContext )
/*LINUX_X86_64*/{
/*LINUX_X86_64*/    void *tmpArray[skipCount + givenMaxLevel]; /* gcc extension variable sized array used... */
/*LINUX_X86_64*/    int depth = backtrace(tmpArray, skipCount + givenMaxLevel);
/*LINUX_X86_64*/    if ( depth > skipCount )
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        int iDepth;
/*LINUX_X86_64*/        for ( depth -= skipCount, iDepth = 0; iDepth < depth; ++iDepth )
/*LINUX_X86_64*/        {
/*LINUX_X86_64*/            callArray[iDepth] = tmpArray[iDepth+skipCount];
/*LINUX_X86_64*/        }
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/    else
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        depth = 0;
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/
/*LINUX_X86_64*/    return depth;
/*LINUX_X86_64*/}
/*LINUX_X86_64*/
/*LINUX_X86_64*//*---------------------------------------------------------------------------*/
/*LINUX_X86_64*/
/*LINUX_X86_64*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*LINUX_X86_64*/                              SAPDB_Int4            maxLevel,
/*LINUX_X86_64*/                              SAPDB_Int4 *          pFilledLevel,
/*LINUX_X86_64*/                              eo670_CallArrayType   addCallInfo,
/*LINUX_X86_64*/                              void **               callArray,
/*LINUX_X86_64*/                              void*                 pBTraceContext,
/*LINUX_X86_64*/                              OutputCallBackFuncPtr pOutput,
/*LINUX_X86_64*/                              void * const          pOutContext )
/*LINUX_X86_64*/{
/*LINUX_X86_64*/    static int iDepth;
/*LINUX_X86_64*/    static int depth;
/*LINUX_X86_64*/    static void *tmpCallArray[MAX_STACK+FILL_SKIP_COUNT];
/*LINUX_X86_64*/
/*LINUX_X86_64*/    *pFilledLevel = 0;
/*LINUX_X86_64*/
/*LINUX_X86_64*/    for ( iDepth = 1; iDepth <= (skipCount + maxLevel); iDepth++ )
/*LINUX_X86_64*/    {
/*LINUX_X86_64*/        depth = backtrace(tmpCallArray, skipCount + iDepth);
/*LINUX_X86_64*/        if ( depth == (skipCount+iDepth) )
/*LINUX_X86_64*/        {
/*LINUX_X86_64*/            switch ( addCallInfo )
/*LINUX_X86_64*/            {
/*LINUX_X86_64*/            case eo670_FullInfoArray:
/*LINUX_X86_64*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = tmpCallArray[skipCount+iDepth-1];
/*LINUX_X86_64*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame  = 0; /* ffs... */
/*LINUX_X86_64*/                break;
/*LINUX_X86_64*/            case eo670_PointerArray:
/*LINUX_X86_64*/                callArray[*pFilledLevel] = tmpCallArray[skipCount+iDepth-1];
/*LINUX_X86_64*/                break;
/*LINUX_X86_64*/            default: /* eo670_NoArrayJustOutput */
/*LINUX_X86_64*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX_X86_64*/                         "(%d):0x%lx\n",
/*LINUX_X86_64*/                         *pFilledLevel, tmpCallArray[skipCount+iDepth-1]);
/*LINUX_X86_64*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX_X86_64*/                break;
/*LINUX_X86_64*/            }
/*LINUX_X86_64*/            ++*pFilledLevel;
/*LINUX_X86_64*/        }
/*LINUX_X86_64*/    }
/*LINUX_X86_64*/}
#    elif defined(LINUX) && defined(SDBonPPC64)
/*LoP*/
/*LoP*//*---------------------------------------------------------------------------*/
/*LoP*/
/*LoP*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*LoP*/                                OutputCallBackFuncPtr pOutput,
/*LoP*/                                void * const          pOutContext )
/*LoP*/{
/*LoP*/    static ucontext_t *pContext;
/*LoP*/    static int iReg;
/*LoP*/
/*LoP*/    pContext = (ucontext_t *)pBTraceContext;
/*LoP*/    eo670WriteOutput( "-------------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    eo670WriteOutput( "Aliases: r1 == sp, r13 == thread_id\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    eo670WriteOutput( "         r3  -  r10 == iarg0 - iarg7\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    eo670WriteOutput( "         f1  -  f13 == farg0 - farg12\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    eo670WriteOutput( "\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    for ( iReg = 0; iReg < (32/2); iReg++ )
/*LoP*/    {
/*LoP*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LoP*/                     "r%02d r%02d  0x%016qx 0x%016qx\n",
/*LoP*/                      2*iReg,
/*LoP*/                      1+2*iReg,
/*LoP*/                      pContext->uc_mcontext.regs->gpr[2*iReg],
/*LoP*/                      pContext->uc_mcontext.regs->gpr[1+2*iReg] );
/*LoP*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    }
/*LoP*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "nip       0x%016qx\n",
/*LoP*/                 pContext->uc_mcontext.regs->nip);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "msr       0x%016qx\n",
/*LoP*/                 pContext->uc_mcontext.regs->msr);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "orig_gpr3 0x%016qx\n",
/*LoP*/                 pContext->uc_mcontext.regs->orig_gpr3);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "ctr       0x%016qx\n",
/*LoP*/                 pContext->uc_mcontext.regs->ctr);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "link      0x%016qx\n",
/*LoP*/                 pContext->uc_mcontext.regs->link);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "xer       0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->xer);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "ccr       0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->ccr);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "softe     0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->softe);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "trap      0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->trap);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "dar       0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->dar);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "dsisr     0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->dsisr);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "result    0x%08x\n",
/*LoP*/                 pContext->uc_mcontext.regs->result);
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    eo670WriteOutput( "-----------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/    for ( iReg = 0; iReg < 32; iReg++ )
/*LoP*/    {
/*LoP*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LoP*/                    "f%02d      [0x%016qx]%.20e\n",
/*LoP*/                    iReg,
/*LoP*/                    *(SAPDB_UInt8 *)&pContext->uc_mcontext.fp_regs[iReg],
/*LoP*/                    *(double *)&pContext->uc_mcontext.fp_regs[iReg] );
/*LoP*/         eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    }
/*LoP*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    sp77sprintf( eo670MessageBuffer,
/*LoP*/                 sizeof(eo670MessageBuffer),
/*LoP*/                 "fpscr   [0x%016qx]%.20e\n",
/*LoP*/                 *(SAPDB_UInt8 *)&pContext->uc_mcontext.fp_regs[32],
/*LoP*/                 *(double *)&pContext->uc_mcontext.fp_regs[32] );
/*LoP*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/    eo670WriteOutput( "-----------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/
/*LoP*/}
/*LoP*/
/*LoP*//*---------------------------------------------------------------------------*/
/*LoP*/
/*LoP*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*LoP*/                                    SAPDB_Int4            givenMaxLevel,
/*LoP*/                                    void **               callArray,
/*LoP*/                                    OutputCallBackFuncPtr pOutput,
/*LoP*/                                    void * const          pOutContext )
/*LoP*/{
/*LoP*/    void *tmpArray[skipCount + givenMaxLevel]; /* gcc extension variable sized array used... */
/*LoP*/    int depth = backtrace(tmpArray, skipCount + givenMaxLevel);
/*LoP*/    if ( depth > skipCount )
/*LoP*/    {
/*LoP*/        int iDepth;
/*LoP*/        for ( depth -= skipCount, iDepth = 0; iDepth < depth; ++iDepth )
/*LoP*/        {
/*LoP*/            callArray[iDepth] = tmpArray[iDepth+skipCount];
/*LoP*/        }
/*LoP*/    }
/*LoP*/    else
/*LoP*/    {
/*LoP*/        depth = 0;
/*LoP*/    }
/*LoP*/
/*LoP*/    return depth;
/*LoP*/}
/*LoP*/
/*LoP*//*---------------------------------------------------------------------------*/
/*LoP*/
/*LoP*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*LoP*/                              SAPDB_Int4            maxLevel,
/*LoP*/                              SAPDB_Int4 *          pFilledLevel,
/*LoP*/                              eo670_CallArrayType   addCallInfo,
/*LoP*/                              void **               callArray,
/*LoP*/                              void*                 pBTraceContext,
/*LoP*/                              OutputCallBackFuncPtr pOutput,
/*LoP*/                              void * const          pOutContext )
/*LoP*/{
/*LoP*/    static int iDepth;
/*LoP*/    static int depth;
/*LoP*/    static void *tmpCallArray[MAX_STACK+FILL_SKIP_COUNT];
/*LoP*/
/*LoP*/    *pFilledLevel = 0;
/*LoP*/
/*LoP*/    for ( iDepth = 1; iDepth <= (skipCount + maxLevel); iDepth++ )
/*LoP*/    {
/*LoP*/        depth = backtrace(tmpCallArray, skipCount + iDepth);
/*LoP*/        if ( depth == (skipCount+iDepth) )
/*LoP*/        {
/*LoP*/            switch ( addCallInfo )
/*LoP*/            {
/*LoP*/            case eo670_FullInfoArray:
/*LoP*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = tmpCallArray[skipCount+iDepth-1];
/*LoP*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame  = 0; /* ffs... */
/*LoP*/                break;
/*LoP*/            case eo670_PointerArray:
/*LoP*/                callArray[*pFilledLevel] = tmpCallArray[skipCount+iDepth-1];
/*LoP*/                break;
/*LoP*/            default: /* eo670_NoArrayJustOutput */
/*LoP*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LoP*/                         "(%d):0x%lx\n",
/*LoP*/                         *pFilledLevel, tmpCallArray[skipCount+iDepth-1]);
/*LoP*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LoP*/                break;
/*LoP*/            }
/*LoP*/            ++*pFilledLevel;
/*LoP*/        }
/*LoP*/    }
/*LoP*/}
#    endif /* defined(LINUX) && defined(SDBonPPC64) */

/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static void eo670_FillAddressInfo( char *                pcAddr,
/*LINUX*/                                   eo670AddressInfo *    pAddrInfo,
/*LINUX*/                                   OutputCallBackFuncPtr pOutput,
/*LINUX*/                                   void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    static int                fd;
/*LINUX*/    static eo670_ModuleChain *modulePointer;
/*LINUX*/
/*LINUX*/    SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
/*LINUX*/    pAddrInfo->demangledSymbol = ""; /* No demangling support on LINUX */
/*LINUX*/    pAddrInfo->sourceFileName = ""; /* No Source information on LINUX... */
/*LINUX*/    pAddrInfo->lineNumber = -1;
/*LINUX*/
/*LINUX*/    for ( modulePointer = eo670_ModuleAnchor
/*LINUX*/        ; modulePointer
/*LINUX*/        ; modulePointer = modulePointer->next )
/*LINUX*/    {
/*LINUX*/        if (  ( pcAddr >= ((char *)modulePointer->text_mapaddr) )
/*LINUX*/           && ( pcAddr <  ((char *)modulePointer->text_endaddr) ) )
/*LINUX*/        {
/*LINUX*/	         break;
/*LINUX*/   	 }
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    if ( !modulePointer )
/*LINUX*/    {
/*LINUX*/        pAddrInfo->demangledSymbol = "";
/*LINUX*/        pAddrInfo->offsetToSymbol = -1;
/*LINUX*/        pAddrInfo->moduleName = "<not in any loaded module>";
/*LINUX*/        pAddrInfo->symbol = "<not in any loaded region>";
/*LINUX*/        return;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    pAddrInfo->moduleName = modulePointer->file_name;
/*LINUX*/
/*LINUX*/    fd = open(modulePointer->file_name, O_RDONLY);
/*LINUX*/    if ( fd < 0 )
/*LINUX*/    {
/*LINUX*/        eo670WriteOutput("Failed to open DLL to retrieve symbols\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        pAddrInfo->symbol = "<no symbols for this region>";
/*LINUX*/        pAddrInfo->demangledSymbol = "";
/*LINUX*/        pAddrInfo->offsetToSymbol = -1;
/*LINUX*/        return;
/*LINUX*/    }
/*LINUX*/
/*LINUX*/    if ( !eo670_ELFFindSymbol(fd, pcAddr, modulePointer->text_mapaddr, &pAddrInfo->symbol,
/*LINUX*/                              &pAddrInfo->offsetToSymbol, pOutput, pOutContext) )
/*LINUX*/    {
/*LINUX*/        pAddrInfo->symbol = "<symbol lookup failed>";
/*LINUX*/        pAddrInfo->offsetToSymbol = -1;
/*LINUX*/    }
/*LINUX*/    close(fd);
/*LINUX*/} /* eo670_FillAddressInfo */
/*LINUX*/
/*LINUX*//*---------------------------------------------------------------------------*/
/*LINUX*/
/*LINUX*/static SAPDB_Bool eo670_UnixGetAddressInfo( void *                pcAddr,
/*LINUX*/                                            eo670AddressInfo *    pAddrInfo,
/*LINUX*/                                            OutputCallBackFuncPtr pOutput,
/*LINUX*/                                            void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    int    fillResult;
/*LINUX*/    static eo670AddressInfo localAddrInfo;
/*LINUX*/
/*LINUX*/    eo670_FillAddressInfo(pcAddr, &localAddrInfo, pOutput, pOutContext);
/*LINUX*/    pAddrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*LINUX*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*LINUX*/                                 + strlen(localAddrInfo.moduleName)+1
/*LINUX*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*LINUX*/    if ( !pAddrInfo->symbol )
/*LINUX*/    {
/*LINUX*/        eo670WriteOutput("Out of memory for symbol copy\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/        fillResult = 0;
/*LINUX*/    }
/*LINUX*/    else
/*LINUX*/    {
/*LINUX*/        strcpy(pAddrInfo->symbol, localAddrInfo.symbol);
/*LINUX*/        pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*LINUX*/        strcpy(pAddrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*LINUX*/        pAddrInfo->moduleName = pAddrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*LINUX*/        strcpy(pAddrInfo->moduleName, localAddrInfo.moduleName);
/*LINUX*/        pAddrInfo->sourceFileName = pAddrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*LINUX*/        strcpy(pAddrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*LINUX*/        pAddrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*LINUX*/        pAddrInfo->lineNumber = localAddrInfo.lineNumber;
/*LINUX*/        fillResult = 1;
/*LINUX*/    }
/*LINUX*/    return fillResult;
/*LINUX*/}
/*LINUX*/
/*LINUX*/static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*LINUX*/{
/*LINUX*/    free(pAddrInfo->symbol); /* allocated in one chunk.... */
/*LINUX*/}
/*LINUX*/
#    ifndef HAS_NO_ADDITIONAL_FRAME_INFO
/*LINUX*/static void eo670_UnixShowCallInfoAddon( eo670_CallInfo *      callInfo,
/*LINUX*/                                         OutputCallBackFuncPtr pOutput,
/*LINUX*/                                         void * const          pOutContext )
/*LINUX*/{
/*LINUX*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LINUX*/         "      Frameinfo [0x%lx] (0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*LINUX*/         (long)callInfo->frame,
/*LINUX*/         callInfo->arg[0],
/*LINUX*/         callInfo->arg[1],
/*LINUX*/         callInfo->arg[2],
/*LINUX*/         callInfo->arg[3] );
/*LINUX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LINUX*/}
#    endif /* HAS_NO_ADDITIONAL_FRAME_INFO */

#  endif /* LINUX */


#if defined FREEBSD
//
//  This is really stressy to port, and it seems to change from version to version
//  Problem, every register type and name is defined differently that in linux.
//  Floating point dump is not yet ported, i saw not the sense of that so far.
//  So if the kernel crashes because of some FP stuff, its on you to implement ...
//  Sorry for that though.
//
//  see <sys/ucontext.h> for most of the definitions
//
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
/*FREEBSD*/static void PrepareProcMap(char *procMap, char *endProcMap)
/*FREEBSD*/{
/*FREEBSD*/  static char *p;
/*FREEBSD*/
/*FREEBSD*/    p = procMap;
/*FREEBSD*/    while ( p < endProcMap ) {
/*FREEBSD*/        if ( *p == '\n' ) *p = 0;
/*FREEBSD*/        ++p;
/*FREEBSD*/    }
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
/*FREEBSD*/static char *NextProcMap(char *procMap, size_t *pBufferedChars, int proc_fd)
/*FREEBSD*/{
/*FREEBSD*/  static char *p;
/*FREEBSD*/  static size_t bytesStored;
/*FREEBSD*/  static int    bytesRead;
/*FREEBSD*/
/*FREEBSD*/    bytesStored = *pBufferedChars;
/*FREEBSD*/
/*FREEBSD*/    p = procMap + strlen(procMap);
/*FREEBSD*/    while ( *p == 0 && p < (procMap + bytesStored) ) {
/*FREEBSD*/        ++p;
/*FREEBSD*/    }
/*FREEBSD*/    bytesStored -= (p-procMap);
/*FREEBSD*/
/*FREEBSD*/    SAPDB_memcpy(procMap, p, bytesStored);
/*FREEBSD*/    bytesRead = read(proc_fd, procMap + bytesStored, 1024 - bytesStored );
/*FREEBSD*/    if ( bytesRead < 0 )
/*FREEBSD*/    {
/*FREEBSD*/        eo670WriteString("cant read from /proc file\n");
/*FREEBSD*/        return (NULL);
/*FREEBSD*/    }
/*FREEBSD*/    PrepareProcMap(procMap + bytesStored, procMap + bytesStored + bytesRead);
/*FREEBSD*/
/*FREEBSD*/    *pBufferedChars = bytesStored + bytesRead;
/*FREEBSD*/
/*FREEBSD*/    return ( *pBufferedChars > 0 ? procMap : NULL );
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
#   define PROC_MMAP_FILE_OFFSET     49
#   define PROC_MMAP_ACCESS_OFFSET   18
#   define PROC_MMAP_VADDR_OFFSET    0
#   define PROC_MMAP_VENDADDR_OFFSET 9
/*FREEBSD*/
/*FREEBSD*/static SAPDB_Bool eo670_UpdateModuleChain(OutputCallBackFuncPtr pOutput,
/*FREEBSD*/                                    void * const          pOutContext )
/*FREEBSD*/{
/*FREEBSD*/  static int       fd_proc;	/* /proc fd                           */
/*FREEBSD*/  static char      procMapBuffer[1024];
/*FREEBSD*/  static char     *procMap;
/*FREEBSD*/  static size_t    bufferedChars; /* Number of currently buffered character */
/*FREEBSD*/  static caddr_t   vaddr;
/*FREEBSD*/  static caddr_t   vendaddr;
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer), "/proc/%d/maps", getpid());
/*FREEBSD*/
/*FREEBSD*/    if ((fd_proc = open(eo670MessageBuffer, O_RDONLY)) == -1)
/*FREEBSD*/    {
/*FREEBSD*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/                  "FREEBSD : /proc/%d/map : not supported yet. %d\n", getpid(), errno);
//*FREEBSD*/                  "Cant open /proc/%d/map : %d\n", getpid(), errno);
/*FREEBSD*/        eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/        return false;
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    if ( (bufferedChars = read(fd_proc, procMapBuffer, sizeof(procMapBuffer))) <= 0 )
/*FREEBSD*/    {
/*FREEBSD*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/                  "Cant read from /proc/%d/maps : %d\n", getpid(), errno);
/*FREEBSD*/        eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/        close(fd_proc);
/*FREEBSD*/        return false;
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    procMap = &procMapBuffer[0];
/*FREEBSD*/    /* Split lines into zero terminated strings */
/*FREEBSD*/    PrepareProcMap(procMap, procMap + bufferedChars);
/*FREEBSD*/
/*FREEBSD*/    do
/*FREEBSD*/    {
/*FREEBSD*/        if ( strlen(procMap) < PROC_MMAP_FILE_OFFSET )
/*FREEBSD*/            continue;
/*FREEBSD*/
/*FREEBSD*/        if ( strncmp(procMap+PROC_MMAP_ACCESS_OFFSET, "r-xp", 4) != 0 )
/*FREEBSD*/            continue;
/*FREEBSD*/
/*FREEBSD*/        vaddr = (caddr_t)
/*FREEBSD*/                strtoul(procMap+PROC_MMAP_VADDR_OFFSET, NULL, 16);
/*FREEBSD*/
/*FREEBSD*/        vendaddr = (caddr_t)
/*FREEBSD*/                strtoul(procMap+PROC_MMAP_VENDADDR_OFFSET, NULL, 16);
/*FREEBSD*/
/*FREEBSD*/        if ( eo670_AddToLoadedModuleChain( procMap + PROC_MMAP_FILE_OFFSET,
/*FREEBSD*/                                           vaddr,
/*FREEBSD*/                                           vendaddr,
/*FREEBSD*/                                           0,
/*FREEBSD*/                                           0,
/*FREEBSD*/                                           pOutput,
/*FREEBSD*/                                           pOutContext
/*FREEBSD*/                                          ) < 0 )
/*FREEBSD*/        {
/*FREEBSD*/             close(fd_proc);
/*FREEBSD*/             return false;
/*FREEBSD*/        }
/*FREEBSD*/    } while ( 0 != NextProcMap(procMap, &bufferedChars, fd_proc) );
/*FREEBSD*/
/*FREEBSD*/    (void) close(fd_proc);
/*FREEBSD*/    return true;
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/

// to be fixed

static void eo670_RegisterDump( void*                 pContext,
                                OutputCallBackFuncPtr pOutput,
                                void * const          pOutContext )
/*FREEBSD*/{
/*FREEBSD*/    static ucontext_t *context;
/*FREEBSD*/    static int iReg;
/*FREEBSD*/
/*FREEBSD*/    context = (ucontext_t *)pContext;
/*FREEBSD*/
/*FREEBSD*/    eo670WriteString( "-------------------------------\n" );
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "ds   es   0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_ds,
/*FREEBSD*/                 context->uc_mcontext.mc_es);
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "fs   gs   0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_fs,
/*FREEBSD*/                 context->uc_mcontext.mc_gs );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "trap err  0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_trapno,
/*FREEBSD*/                 context->uc_mcontext.mc_err );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "(efl)  ss   0x%08x 0x%08x\n",
/*FREEBSD*/                 0, //context->uc_mcontext.mc_efl,
/*FREEBSD*/                 context->uc_mcontext.mc_ss );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "edi  esi  0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_edi,
/*FREEBSD*/                 context->uc_mcontext.mc_esi );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "eax  ebx  0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_eax,
/*FREEBSD*/                 context->uc_mcontext.mc_ebx );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "ecx  edx  0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_ecx,
/*FREEBSD*/                 context->uc_mcontext.mc_edx );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "esp (uesp) 0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_esp,
/*FREEBSD*/                 0);//context->uc_mcontext.mc_uesp );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "ebp  eip  0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_ebp,
/*FREEBSD*/                 context->uc_mcontext.mc_eip );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    sp77sprintf( eo670MessageBuffer,
/*FREEBSD*/                 sizeof(eo670MessageBuffer),
/*FREEBSD*/                 "cs   (cr2)  0x%08x 0x%08x\n",
/*FREEBSD*/                 context->uc_mcontext.mc_cs,
/*FREEBSD*/                 0);//context->uc_mcontext.uc_flags);
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/
/*FREEBSD*/    eo670WriteString( "-------------------------------\n" );
/*FREEBSD*/
//    if ( context->uc_mcontext.fpregs && 0 == (((long)context->uc_mcontext.fpregs)&3) )
//    {
//        sp77sprintf( eo670MessageBuffer,
//                 sizeof(eo670MessageBuffer),
//                 "cw - sw - tag   0x%08x 0x%08x 0x%08x\n",
//                 context->uc_mcontext.fpregs->cw,
//                 context->uc_mcontext.fpregs->sw,
//                 context->uc_mcontext.fpregs->tag);
//        eo670WriteString(eo670MessageBuffer);
//
//        sp77sprintf( eo670MessageBuffer,
//                 sizeof(eo670MessageBuffer),
//                 "ipoff - cssel   0x%08x 0x%08x\n",
//                 context->uc_mcontext.fpregs->ipoff,
//                 context->uc_mcontext.fpregs->cssel);
//        eo670WriteString(eo670MessageBuffer);
//
//        sp77sprintf( eo670MessageBuffer,
//                 sizeof(eo670MessageBuffer),
//                 "dataoff-datasel 0x%08x 0x%08x\n",
//                 context->uc_mcontext.fpregs->dataoff,
//                 context->uc_mcontext.fpregs->datasel);
//        eo670WriteString(eo670MessageBuffer);
//
//        for ( iReg = 0; iReg < sizeof(context->uc_mcontext.fpregs->_st)/sizeof(struct _libc_fpreg); iReg++ )
//        {
//            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
//                     "f%02d      [0x%04x 0x%04x 0x%04x 0x%04x - 0x%04x]\n",
//                      iReg,
//                      context->uc_mcontext.fpregs->_st[iReg].significand[0],
//                      context->uc_mcontext.fpregs->_st[iReg].significand[1],
//                      context->uc_mcontext.fpregs->_st[iReg].significand[2],
//                      context->uc_mcontext.fpregs->_st[iReg].significand[3],
//                      context->uc_mcontext.fpregs->_st[iReg].exponent );
//            eo670WriteString( eo670MessageBuffer );
//        }
//    }
//    else
//    {
/*FREEBSD*/        eo670WriteString( "FREEBSD : Floating point register debug not supported yet.\n" );
//    }
/*FREEBSD*/    eo670WriteString( "-------------------------------\n" );
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
/*FREEBSD*/struct LinuxGccI386Frame {
/*FREEBSD*/    struct LinuxGccI386Frame *fr_savfp;
/*FREEBSD*/    unsigned long fr_savpc;
/*FREEBSD*/    unsigned long fr_arg[4];
/*FREEBSD*/};
/*FREEBSD*/
SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4                      skipCount,
                                    SAPDB_Int4                      level,
                                    void **                         callArray,
                                    OutputCallBackFuncPtr           pOutput,
                                    void * const                    pOutContext )
//SAPDB_Int4 eo670_FastFillCallArray(SAPDB_Int4 skipCount, SAPDB_Int4 level, void **callArray)
/*FREEBSD*/{
/*FREEBSD*/    struct LinuxGccI386Frame  *fp;
/*FREEBSD*/    unsigned long pc;
/*FREEBSD*/    SAPDB_Int4 depth;
/*FREEBSD*/
/*FREEBSD*/    depth = 0;
/*FREEBSD*/
/*FREEBSD*/    pc = (unsigned long)eo670_FastFillCallArray;
/*FREEBSD*/    asm( "movl %%ebp,%0" : "=r" (fp) : /* no input operand */ );
/*FREEBSD*/
/*FREEBSD*/    while ( depth < level
/*FREEBSD*/         && fp
/*FREEBSD*/         && ((((long)fp)&3) == 0)
/*FREEBSD*/         && (fp != fp->fr_savfp) )
/*FREEBSD*/    {
/*FREEBSD*/         if ( skipCount > 0 )
/*FREEBSD*/         {
/*FREEBSD*/             --skipCount;
/*FREEBSD*/         }
/*FREEBSD*/         else
/*FREEBSD*/         {
/*FREEBSD*/            callArray[depth] = ((char *)0) + fp->fr_savpc;
/*FREEBSD*/            ++depth;
/*FREEBSD*/         }
/*FREEBSD*/         fp = fp->fr_savfp;
/*FREEBSD*/    }
/*FREEBSD*/    return depth;
/*FREEBSD*/} /* eo670_FastFillCallArray */
/*FREEBSD*/
SAPDB_Int4 eo670_UnixFillCallArray(SAPDB_Int4 skipCount, SAPDB_Int4 level, eo670_CallArrayType addCallInfo, void **callArray, void *pContext,
                                  OutputCallBackFuncPtr           pOutput,
                                  void * const                    pOutContext )
/*FREEBSD*/{
/*FREEBSD*/    static struct LinuxGccI386Frame  *fp;
/*FREEBSD*/    static unsigned long pc;
/*FREEBSD*/    static SAPDB_Int4 depth;
/*FREEBSD*/
/*FREEBSD*/    depth = 0;
/*FREEBSD*/
/*FREEBSD*/    if ( 0 == pContext )
/*FREEBSD*/    {
/*FREEBSD*/        pc = (unsigned long)eo670_UnixFillCallArray;
/*FREEBSD*/        asm( "movl %%ebp,%0" : "=r" (fp) : /* no input operand */ );
/*FREEBSD*/    }
/*FREEBSD*/    else
/*FREEBSD*/    {
#      ifdef LINUX_WITHOUT_SIGINFO
/*OLDFREEBSD*/     static struct sigcontext *context;
/*OLDFREEBSD*/
/*OLDFREEBSD*/     context = &(((struct LinuxGccI386SignalHandlerFrame *)pContext)->fr_savfp->fr_context);
/*OLDFREEBSD*/     fp = (struct LinuxGccI386Frame *)(((char *)0)+context->ebp);
/*OLDFREEBSD*/     pc = context->eip;
#      else /* LINUX_WITHOUT_SIGINFO */
/*FREEBSD*/        static ucontext_t *context;
/*FREEBSD*/        context = (ucontext_t *)pContext;
/*FREEBSD*/        fp = (struct LinuxGccI386Frame *)(((char *)0)+context->uc_mcontext.mc_ebp);
/*FREEBSD*/        pc = context->uc_mcontext.mc_eip;
#     endif /* LINUX_WITHOUT_SIGINFO */
/*FREEBSD*/        skipCount = 0;
/*FREEBSD*/        switch ( addCallInfo )
/*FREEBSD*/        {
/*FREEBSD*/        case eo670_FullInfoArray:
/*FREEBSD*/            ((eo670_CallInfo *)callArray)[depth].pcAddr = (char *)0 + pc;
/*FREEBSD*/            ((eo670_CallInfo *)callArray)[depth].frame  = fp;
/*FREEBSD*/            if ( fp && ((((long)fp)&3) == 0) )
/*FREEBSD*/            {
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[0] = fp->fr_arg[0];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[1] = fp->fr_arg[1];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[2] = fp->fr_arg[2];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[3] = fp->fr_arg[3];
/*FREEBSD*/            }
/*FREEBSD*/            else
/*FREEBSD*/            {
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[0] = 0xdead;
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[1] = 0xc0de;
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[2] = 0xdeed;
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[3] = 0xc0de;
/*FREEBSD*/            }
/*FREEBSD*/            break;
/*FREEBSD*/        case eo670_PointerArray:
/*FREEBSD*/            callArray[depth] = ((char *)0) + pc;
/*FREEBSD*/            break;
/*FREEBSD*/        default: /* eo670_NoArrayJustOutput */
/*FREEBSD*/            if ( fp && ((((long)fp)&3) == 0) )
/*FREEBSD*/            {
/*FREEBSD*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/                         "(%d):0x%lx [0x%lx](0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*FREEBSD*/                         depth, pc, (long)fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3] );
/*FREEBSD*/            }
/*FREEBSD*/            else
/*FREEBSD*/            {
/*FREEBSD*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/                         "(%d):0x%lx [0x%lx](?,?,?,?)",
/*FREEBSD*/                         depth, pc, (long)fp );
/*FREEBSD*/            }
/*FREEBSD*/            eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/            break;
/*FREEBSD*/        }
/*FREEBSD*/        ++depth;
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    while ( depth < level
/*FREEBSD*/         && fp
/*FREEBSD*/         && ((((long)fp)&3) == 0)
/*FREEBSD*/         && (fp != fp->fr_savfp)
/*FREEBSD*/         && ((unsigned long)fp->fr_savfp >= USRTEXT) )
/*FREEBSD*/    {
/*FREEBSD*/         if ( skipCount > 0 )
/*FREEBSD*/         {
/*FREEBSD*/             --skipCount;
/*FREEBSD*/         }
/*FREEBSD*/         else
/*FREEBSD*/         {
/*FREEBSD*/            switch ( addCallInfo )
/*FREEBSD*/            {
/*FREEBSD*/            case eo670_FullInfoArray:
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].pcAddr = (char *)0 + fp->fr_savpc;
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].frame = fp;
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[0] = fp->fr_arg[0];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[1] = fp->fr_arg[1];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[2] = fp->fr_arg[2];
/*FREEBSD*/                ((eo670_CallInfo *)callArray)[depth].arg[3] = fp->fr_arg[3];
/*FREEBSD*/                break;
/*FREEBSD*/            case eo670_PointerArray:
/*FREEBSD*/                callArray[depth] = ((char *)0) + fp->fr_savpc;
/*FREEBSD*/                break;
/*FREEBSD*/            default: /* eo670_NoArrayJustOutput */
/*FREEBSD*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/                         "(%d):0x%lx [0x%lx](0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*FREEBSD*/                         depth, fp->fr_savpc, (long)fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3] );
/*FREEBSD*/                eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/                break;
/*FREEBSD*/            }
/*FREEBSD*/            ++depth;
/*FREEBSD*/         }
/*FREEBSD*/         fp = fp->fr_savfp;
/*FREEBSD*/    }
/*FREEBSD*/    return depth;
/*FREEBSD*/} /* eo670_UnixFillCallArray */
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
#     if defined(WANT_DEMANGLING_SUPPORT)
/*FREEBSD*/static char   demanglingBuffer[MAX_SYMBOL_LENGTH];
#     endif /* WANT_DEMANGLING_SUPPORT */
/*FREEBSD*/
/*FREEBSD*/static void eo670_FillAddressInfo(char *pcAddr, eo670AddressInfo *addrInfo,
                                             OutputCallBackFuncPtr           pOutput,
                                             void * const                    pOutContext )
/*FREEBSD*/{
/*FREEBSD*/    static int                fd;
/*FREEBSD*/    static eo670_ModuleChain *modulePointer;
/*FREEBSD*/
/*FREEBSD*/    SAPDB_memset(addrInfo, 0, sizeof(eo670AddressInfo));
/*FREEBSD*/    addrInfo->demangledSymbol = ""; /* No demangling support on LINUX */
/*FREEBSD*/    addrInfo->sourceFileName = ""; /* No Source information on LINUX... */
/*FREEBSD*/    addrInfo->lineNumber = -1;
/*FREEBSD*/
/*FREEBSD*/    for ( modulePointer = eo670_ModuleAnchor
/*FREEBSD*/        ; modulePointer
/*FREEBSD*/        ; modulePointer = modulePointer->next )
/*FREEBSD*/    {
/*FREEBSD*/        if (  ( pcAddr >= ((char *)modulePointer->text_mapaddr) )
/*FREEBSD*/           && ( pcAddr <  ((char *)modulePointer->text_endaddr) ) )
/*FREEBSD*/        {
/*FREEBSD*/	         break;
/*FREEBSD*/   	 }
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    if ( !modulePointer )
/*FREEBSD*/    {
/*FREEBSD*/        addrInfo->demangledSymbol = "";
/*FREEBSD*/        addrInfo->offsetToSymbol = -1;
/*FREEBSD*/        addrInfo->moduleName = "<not in any loaded module>";
/*FREEBSD*/        addrInfo->symbol = "<not in any loaded region>";
/*FREEBSD*/        return;
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    addrInfo->moduleName = modulePointer->file_name;
/*FREEBSD*/
/*FREEBSD*/    fd = open(modulePointer->file_name, O_RDONLY);
/*FREEBSD*/    if ( fd < 0 )
/*FREEBSD*/    {
/*FREEBSD*/        eo670WriteString("Failed to open DLL to retrieve symbols\n");
/*FREEBSD*/        addrInfo->symbol = "<no symbols for this region>";
/*FREEBSD*/        addrInfo->demangledSymbol = "";
/*FREEBSD*/        addrInfo->offsetToSymbol = -1;
/*FREEBSD*/        return;
/*FREEBSD*/    }
/*FREEBSD*/
/*FREEBSD*/    if ( !eo670_ELFFindSymbol(fd, pcAddr, modulePointer->text_mapaddr, &addrInfo->symbol, &addrInfo->offsetToSymbol, pOutput, pOutContext) )
/*FREEBSD*/    {
/*FREEBSD*/        addrInfo->symbol = "<symbol lookup failed>";
/*FREEBSD*/        addrInfo->offsetToSymbol = -1;
/*FREEBSD*/    }
/*FREEBSD*/    close(fd);
/*FREEBSD*/} /* eo670_FillAddressInfo */
/*FREEBSD*/
/*FREEBSD*//*---------------------------------------------------------------------------*/
/*FREEBSD*/
/*FREEBSD*/static SAPDB_Bool eo670_UnixGetAddressInfo(void *pcAddr, eo670AddressInfo *addrInfo,
                                                     OutputCallBackFuncPtr           pOutput,
                                                     void * const                    pOutContext )
/*FREEBSD*/{
/*FREEBSD*/    int    fillResult;
/*FREEBSD*/    static eo670AddressInfo localAddrInfo;
/*FREEBSD*/
/*FREEBSD*/    eo670_FillAddressInfo(pcAddr, &localAddrInfo, pOutput, pOutContext);
/*FREEBSD*/    addrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*FREEBSD*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*FREEBSD*/                                 + strlen(localAddrInfo.moduleName)+1
/*FREEBSD*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*FREEBSD*/    if ( !addrInfo->symbol )
/*FREEBSD*/    {
/*FREEBSD*/        eo670WriteString("Out of memory for symbol copy\n");
/*FREEBSD*/        fillResult = 0;
/*FREEBSD*/    }
/*FREEBSD*/    else
/*FREEBSD*/    {
/*FREEBSD*/        strcpy(addrInfo->symbol, localAddrInfo.symbol);
/*FREEBSD*/        addrInfo->demangledSymbol = addrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*FREEBSD*/        strcpy(addrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*FREEBSD*/        addrInfo->moduleName = addrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*FREEBSD*/        strcpy(addrInfo->moduleName, localAddrInfo.moduleName);
/*FREEBSD*/        addrInfo->sourceFileName = addrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*FREEBSD*/        strcpy(addrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*FREEBSD*/        addrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*FREEBSD*/        addrInfo->lineNumber = localAddrInfo.lineNumber;
/*FREEBSD*/        fillResult = 1;
/*FREEBSD*/    }
/*FREEBSD*/    return fillResult;
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*/static void eo670_OsFreeAddressInfo(eo670AddressInfo *addrInfo)
/*FREEBSD*/{
/*FREEBSD*/    free(addrInfo->symbol); /* allocated in one chunk.... */
/*FREEBSD*/}
/*FREEBSD*/
/*FREEBSD*/static void eo670_UnixShowCallInfoAddon(eo670_CallInfo *callInfo, OutputCallBackFuncPtr pOutput, void * const pOutContext )
/*FREEBSD*/{
/*FREEBSD*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*FREEBSD*/         "      Frameinfo [0x%lx] (0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*FREEBSD*/         (long)callInfo->frame,
/*FREEBSD*/         callInfo->arg[0],
/*FREEBSD*/         callInfo->arg[1],
/*FREEBSD*/         callInfo->arg[2],
/*FREEBSD*/         callInfo->arg[3] );
/*FREEBSD*/    eo670WriteString(eo670MessageBuffer);
/*FREEBSD*/}
#    endif /* FREEBSD */

#  if defined(SOLARIS) || defined(SUN)
#include <sys/regset.h>

/*SUN*/static void eo670_SunGetModuleFileName( char *                mapName,
/*SUN*/                                        char **               pModuleFileName,
/*SUN*/                                        OutputCallBackFuncPtr pOutput,
/*SUN*/                                        void * const          pOutContext )
/*SUN*/{
/*SUN*/  static  int               fd;
/*SUN*/  static  Elf_ElfHeader     elfhdr;           /* Elf header                (read once) */
/*SUN*/  static  Elf_SectionHeader symbolSectionHdr; /* Elf symbol section header (read once) */
/*SUN*/  static  Elf_SectionHeader elfSectionHdr;    /* temp Elf section header */
/*SUN*/  static  Elf_Word          stringSection;
/*SUN*/  static  Elf_Word          stringTabOffset;
/*SUN*/  static  int               i;
/*SUN*/  static  size_t            syms;
/*SUN*/  static  Elf_Symbol        symtabEntry;
/*SUN*/  static  SAPDB_Int8        systemRc;
/*SUN*/
/*SUN*/    *pModuleFileName = (char *)"????";
/*SUN*/
/*SUN*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                "/proc/%d/object/%s", getpid(), mapName);
/*SUN*/    fd = open(eo670MessageBuffer, O_RDONLY);
/*SUN*/    if(fd < 0)
/*SUN*/    {
/*SUN*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                "Cannot open mapfile /proc/%d/object/%s :%d\n", getpid(), mapName, errno);
/*SUN*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        return;
/*SUN*/    }
/*SUN*/    /* Get the ELF header                                             */
/*SUN*/
/*SUN*/    if ( (systemRc = read (fd, &elfhdr, sizeof(elfhdr))) != sizeof(elfhdr)){
/*SUN*/        eo670WriteOutput("Unable to read ELF header \n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    if (elfhdr.e_shnum == 0) {
/*SUN*/        eo670WriteOutput("No section header entries.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    if (elfhdr.e_shentsize > sizeof(elfSectionHdr) ) {
/*SUN*/        eo670WriteOutput("Elf section header entries larger than expected.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    /* Seek to the section header table                                   */
/*SUN*/
/*SUN*/    if ( (systemRc = lseek(fd, (long) elfhdr.e_shoff, SEEK_SET)) != elfhdr.e_shoff) {
/*SUN*/        eo670WriteOutput("Unable to seek to section header.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    stringSection = 0;
/*SUN*/
/*SUN*/    for (i = 0; i < (int) elfhdr.e_shnum; i++ )
/*SUN*/    {
/*SUN*/        if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*SUN*/        {
/*SUN*/            eo670WriteOutput("Unable to read ELF header section.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/            goto done;
/*SUN*/        }
/*SUN*/        if ( elfSectionHdr.sh_type ==  SHT_SYMTAB )
/*SUN*/        {
/*SUN*/            SAPDB_memcpy(&symbolSectionHdr, &elfSectionHdr, sizeof(elfSectionHdr));
/*SUN*/            stringSection = elfSectionHdr.sh_link;
/*SUN*/        }
/*SUN*/    }
/*SUN*/
/*SUN*/    if ( 0 == stringSection
/*SUN*/      || stringSection >= elfhdr.e_shnum )
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Warning - no ELF string section defined.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    /*Skip section up to wanted string section */
/*SUN*/    if ( (systemRc = lseek(fd, (long) elfhdr.e_shoff, SEEK_SET)) != elfhdr.e_shoff) {
/*SUN*/        eo670WriteOutput("Unable to do second seek to ELF section headers.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    for (i = 0; i < stringSection; i++ )
/*SUN*/    {
/*SUN*/        if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*SUN*/        {
/*SUN*/            eo670WriteOutput("Unable to read ELF header section.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/            goto done;
/*SUN*/        }
/*SUN*/    }
/*SUN*/
/*SUN*/    if ( (systemRc = read(fd, &elfSectionHdr, elfhdr.e_shentsize)) !=  elfhdr.e_shentsize )
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Unable to read ELF string section header.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    if( elfSectionHdr.sh_size == 0)
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Warning - empty ELF string table; no symbols.\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    stringTabOffset = elfSectionHdr.sh_offset;
/*SUN*/
/*SUN*/    /* calculate the number of symbols found */
/*SUN*/    syms = symbolSectionHdr.sh_size/symbolSectionHdr.sh_entsize;
/*SUN*/
/*SUN*/    /*Seek to symbol table section */
/*SUN*/    if ( (systemRc = lseek(fd, (long) symbolSectionHdr.sh_offset, SEEK_SET)) != symbolSectionHdr.sh_offset) {
/*SUN*/        eo670WriteOutput("Unable to do seek to ELF symbol table\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        goto done;
/*SUN*/    }
/*SUN*/
/*SUN*/    for ( i = 0; i < syms; i++ )
/*SUN*/    {
/*SUN*/        if ( ( systemRc = read(fd, &symtabEntry, sizeof(Elf_Symbol)) ) != sizeof(Elf_Symbol) )
/*SUN*/        {
/*SUN*/            sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                        "Warning - reading ELF symbol table failed; got %d expected %d error %d\n",
/*SUN*/                        systemRc, sizeof(Elf_Symbol), errno);
/*SUN*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/            goto done;
/*SUN*/        }
/*SUN*/        if ( symtabEntry.st_name != (Elf_Word)0 )
/*SUN*/        {
/*SUN*/            (void)eo670_ReadElfSymbol(fd, stringTabOffset, symtabEntry.st_name, pModuleFileName, pOutput, pOutContext);
/*SUN*/            goto done;
/*SUN*/        }
/*SUN*/    }
/*SUN*/done:
/*SUN*/    close(fd);
/*SUN*/} /* eo670_SunGetModuleFileName */
/*SUN*/
/*SUN*//*-----------------------------*/
/*SUN*/static SAPDB_Bool
/*SUN*/eo670_UpdateModuleChain( OutputCallBackFuncPtr pOutput,
/*SUN*/                         void * const          pOutContext )
/*SUN*/{
/*SUN*/  static int       fd_proc;	 /* /proc fd          */
#define PMAP_ENTRY_ARRAY_SIZE 1024
/*SUN*/  static prmap_t   pmapEntry[PMAP_ENTRY_ARRAY_SIZE]; /* process map entry */
/*SUN*/  static char     *moduleFileName; /* SUN hides module file name in symbol table... */
/*SUN*/  static ssize_t   entryReadCount;
/*SUN*/  static int       iEntry;
/*SUN*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer), "/proc/%d/map", getpid());
/*SUN*/
/*SUN*/    if ((fd_proc = open(eo670MessageBuffer, O_RDONLY)) == -1)
/*SUN*/    {
/*SUN*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                  "Cant open /proc/%d/map : %d\n", getpid(), errno);
/*SUN*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        return false;
/*SUN*/    }
/*SUN*/
/*SUN*/    /* Get address space (shared library) mappings */
/*SUN*/    while (  (entryReadCount = read(fd_proc, &pmapEntry[0], sizeof(prmap_t)*PMAP_ENTRY_ARRAY_SIZE)) >= sizeof(prmap_t) )
/*SUN*/    {
/*SUN*/        static int iEntry;
/*SUN*/        static int fd_obj;
/*SUN*/        for ( iEntry = 0; iEntry < (entryReadCount/sizeof(prmap_t)); iEntry++ )
/*SUN*/        {
/*SUN*/
/*SUN*/            if (pmapEntry[iEntry].pr_mflags != (MA_READ | MA_EXEC))
/*SUN*/            {
/*SUN*/                continue; /* only text address maps are of interest */
/*SUN*/            }
/*SUN*/
/*SUN*/            sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "/proc/%d/object/%s", getpid(), pmapEntry[iEntry].pr_mapname);
/*SUN*/            fd_obj = open(eo670MessageBuffer, O_RDONLY);
/*SUN*/            if(fd_obj == -1)
/*SUN*/            {
/*SUN*/                continue;
/*SUN*/            }
/*SUN*/            moduleFileName = (char *)"????";
/*SUN*/            eo670_SunGetModuleFileName(pmapEntry[iEntry].pr_mapname, &moduleFileName, pOutput, pOutContext);
/*SUN*/            close(fd_obj);
/*SUN*/
/*SUN*/            if ( eo670_AddToLoadedModuleChain( moduleFileName,
/*SUN*/                                               (char *)0 + pmapEntry[iEntry].pr_vaddr,
/*SUN*/                                               ((char *)pmapEntry[iEntry].pr_vaddr) + pmapEntry[iEntry].pr_size,
/*SUN*/                                               pmapEntry[iEntry].pr_mapname,
/*SUN*/                                               (SAPDB_ULong)strlen(pmapEntry[iEntry].pr_mapname)+1,
/*SUN*/                                               pOutput, pOutContext  ) < 0 )
/*SUN*/            {
/*SUN*/                close(fd_proc);
/*SUN*/                return false;
/*SUN*/            }
/*SUN*/        }
/*SUN*/    }
/*SUN*/
/*SUN*/    close(fd_proc);
/*SUN*/    return true;
/*SUN*/}
/*SUN*/
/*SUN*/

#define SP_BIAS STACK_BIAS

/*SUN*//*---------------------------------------------------------------------------*/
/*SUN*/static struct frame *BiasedFramePtr( void *fp )
/*SUN*/{
/*SUN*/    return (struct frame *)(((unsigned long)fp)+SP_BIAS);
/*SUN*/}
/*SUN*/
/*SUN*//*---------------------------------------------------------------------------*/
/*SUN*/

#if defined(SPARC) || defined(SUN)
/*SUN*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*SUN*/                                OutputCallBackFuncPtr pOutput,
/*SUN*/                                void * const          pOutContext )
/*SUN*/{
/*SUN*/    static ucontext_t *context;
/*SUN*/    static int iReg;
/*SUN*/
/*SUN*/    context = (ucontext_t *)pBTraceContext;
/*SUN*/
/*SUN*/    eo670WriteOutput("---------------------------------------\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    eo670WriteOutput( "Aliases: o0 == r0, o1 == r1, o6 == sp\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    eo670WriteOutput( "         o0 == y, fp == i6, rtn == i7\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    eo670WriteOutput( "\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/    for ( iReg = 0; iReg < 4; iReg++ )
/*SUN*/    {
/*SUN*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "g%d  g%d   0x%016lx 0x%016lx\n",
/*SUN*/                      2*iReg,
/*SUN*/                      1+2*iReg,
/*SUN*/    iReg==0 ? (SAPDB_Long)0 : (SAPDB_Long)context->uc_mcontext.gregs[(REG_G1-1)+2*iReg],
/*SUN*/                      (SAPDB_Long)context->uc_mcontext.gregs[REG_G1+2*iReg] );
/*SUN*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/    for ( iReg = 0; iReg < 4; iReg++ )
/*SUN*/    {
/*SUN*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "o%d  o%d   0x%016lx 0x%016lx\n",
/*SUN*/                      2*iReg,
/*SUN*/                      1+2*iReg,
/*SUN*/                      (SAPDB_Long)context->uc_mcontext.gregs[REG_O0+2*iReg],
/*SUN*/                      (SAPDB_Long)context->uc_mcontext.gregs[REG_O1+2*iReg] );
/*SUN*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/    if ( context->uc_mcontext.gwins )
/*SUN*/    {
/*SUN*/      eo670WriteOutput( "Unexpected register window setting, cant dump %l0-%l7,%i0-%i7\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/      static struct frame   *fp;
/*SUN*/      fp = BiasedFramePtr((void *)context->uc_mcontext.gregs[REG_SP]); /* stack pointer*/
/*SUN*/      for ( iReg = 0; iReg < 4; iReg++ )
/*SUN*/      {
/*SUN*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "l%d  l%d   0x%016lx 0x%016lx\n",
/*SUN*/                      2*iReg,
/*SUN*/                      1+2*iReg,
/*SUN*/                      fp->fr_local[2*iReg],
/*SUN*/                      fp->fr_local[1+2*iReg] );
/*SUN*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/      }
/*SUN*/      eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/      for ( iReg = 0; iReg < 3; iReg++ )
/*SUN*/      {
/*SUN*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "i%d  i%d   0x%016lx 0x%016lx\n",
/*SUN*/                      2*iReg,
/*SUN*/                      1+2*iReg,
/*SUN*/                      fp->fr_arg[2*iReg],
/*SUN*/                      fp->fr_arg[1+2*iReg] );
/*SUN*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/      }
/*SUN*/      sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                   "fp  rtn  0x%016lx 0x%016lx\n",
/*SUN*/                      (SAPDB_Long)fp->fr_savfp,
/*SUN*/                      (SAPDB_Long)fp->fr_savpc );
/*SUN*/      eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/      eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "y        0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_Y] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
#    if defined(__sparcv9)
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "ccr      0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_CCR] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "asi      0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_ASI] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "fprs     0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_FPRS] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
#    else /* __sparcv9 */
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "psr      0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_PSR] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
#    endif /* __sparcv9 */
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "pc       0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_PC] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "npc      0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.gregs[REG_nPC] );
/*SUN*/    eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/    eo670WriteOutput("---------------------------------------\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/    if ( context->uc_mcontext.fpregs.fpu_en )
/*SUN*/    {
#    if defined(__sparcv9)
/*SUN*/        for ( iReg = 0; iReg < 32; iReg++ )
#    else /* __sparcv9 */
/*SUN*/        for ( iReg = 0; iReg < 16; iReg++ )
#    endif /* __sparcv9 */
/*SUN*/        {
/*SUN*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                     "f%02df%02d  [0x%016qx]%.20e\n",
/*SUN*/                      2*iReg,
/*SUN*/                      1+2*iReg,
/*SUN*/                      *(SAPDB_UInt8 *)&context->uc_mcontext.fpregs.fpu_fr.fpu_dregs[iReg],
/*SUN*/                      *(double *)&(context->uc_mcontext.fpregs.fpu_fr.fpu_dregs[iReg]) );
/*SUN*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        }
/*SUN*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                 "fsr      0x%016lx\n",
/*SUN*/                 (SAPDB_Long)context->uc_mcontext.fpregs.fpu_fsr );
/*SUN*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Floating point unit unused\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/    }
/*SUN*/    eo670WriteOutput("---------------------------------------\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/
/*SUN*/}

#elif defined(X86_64)
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*//*---------------------------------------------------------------------------*/
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*SOLARIS_X86_64*/                                OutputCallBackFuncPtr pOutput,
/*SOLARIS_X86_64*/                                void * const          pOutContext )
/*SOLARIS_X86_64*/{
/*SOLARIS_X86_64*/    static ucontext_t *pContext;
/*SOLARIS_X86_64*/    static int iReg;
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    pContext = (ucontext_t *)pBTraceContext;
/*SOLARIS_X86_64*/    eo670WriteOutput( "-----------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rax rbx      0x%016qx 0x%016qx \n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RAX],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RBX]);
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rcx rdx      0x%016qx 0x%016qx \n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RCX],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RDX]);
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rsi rdi      0x%016qx 0x%016qx \n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RSI],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RDI]);
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rbp rsp      0x%016qx 0x%016qx \n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RBP],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RSP]);
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    for ( iReg = 0; iReg < 4; iReg++ )
/*SOLARIS_X86_64*/    {
/*SOLARIS_X86_64*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                     "r%02d r%02d      0x%016qx 0x%016qx\n",
/*SOLARIS_X86_64*/                      8+(2*iReg),
/*SOLARIS_X86_64*/                      9+(2*iReg),
/*SOLARIS_X86_64*/                      pContext->uc_mcontext.gregs[REG_R8+(2*iReg)],
/*SOLARIS_X86_64*/                      pContext->uc_mcontext.gregs[REG_R9+(2*iReg)] );
/*SOLARIS_X86_64*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/    }
/*SOLARIS_X86_64*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rip rflags   0x%016qx 0x%016qx \n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RIP],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_RFL]);
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "cs fs gs     0x%016qx 0x%016qx 0x%016qx\n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_CS],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_FS],
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.gregs[REG_GS] );
/*SOLARIS_X86_64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/    eo670WriteOutput( "--------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
 /*SOLARIS_X86_64*/    if ( 1 /* pContext->uc_mcontext.fpregs */ /* On Linux fpregs is a pointer!!! */ )
/*SOLARIS_X86_64*/    {
/*SOLARIS_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "cw swd     0x%04x             0x%04\n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.cw,
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.sw );
/*SOLARIS_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "fctw __fx_rsvd fop      0x%02x 0x%02x             0x%04\n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.fctw,
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.__fx_rsvd,
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.fop );
/*SOLARIS_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "rip rdp      0x%016qx 0x%016qx]\n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.rip,
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.rdp );
/*SOLARIS_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/        sp77sprintf( eo670MessageBuffer,
/*SOLARIS_X86_64*/                 sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                 "mxcsr mask   0x%018x         0x%08x]\n",
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.mxcsr,
/*SOLARIS_X86_64*/                 pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.mxcsr_mask );
/*SOLARIS_X86_64*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/        for ( iReg = 0; iReg < 8; iReg++ )
/*SOLARIS_X86_64*/        {
/*SOLARIS_X86_64*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                        "st%d      [0x%04x 0x%04x 0x%04x 0x%04x - 0x%04x]\n",
/*SOLARIS_X86_64*/                         iReg,
/*SOLARIS_X86_64*/                         pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.st[iReg].fpr_16[0],
/*SOLARIS_X86_64*/                         pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.st[iReg].fpr_16[1],
/*SOLARIS_X86_64*/                         pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.st[iReg].fpr_16[2],
/*SOLARIS_X86_64*/                         pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.st[iReg].fpr_16[3],
/*SOLARIS_X86_64*/                         pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.st[iReg].fpr_16[4] );
/*SOLARIS_X86_64*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/        }
/*SOLARIS_X86_64*/
/*SOLARIS_X86_64*/        for ( iReg = 0; iReg < 16; iReg++ )
/*SOLARIS_X86_64*/        {
/*SOLARIS_X86_64*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                        "xmm%02d   [0x%08x 0x%08x 0x%08x 0x%08x]\n",
/*SOLARIS_X86_64*/                         iReg,
/*SOLARIS_X86_64*/                        pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[iReg]._l[0],
/*SOLARIS_X86_64*/                        pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[iReg]._l[1],
/*SOLARIS_X86_64*/                        pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[iReg]._l[2],
/*SOLARIS_X86_64*/                        pContext->uc_mcontext.fpregs.fp_reg_set.fpchip_state.xmm[iReg]._l[3] );
/*SOLARIS_X86_64*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/        }
/*SOLARIS_X86_64*/    }
/*SOLARIS_X86_64*/    else
/*SOLARIS_X86_64*/    {
/*SOLARIS_X86_64*/        eo670WriteOutput( "Floating point register unused\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/    }
/*SOLARIS_X86_64*/    eo670WriteOutput( "--------------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*SOLARIS_X86_64*/}
/*SOLARIS_X86_64*/  
#elif defined(I386)
/* here comes the I386 version of all */
#endif
/*SUN*//*---------------------------------------------------------------------------*/
/*SUN*/
/*SUN*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*SUN*/                                    SAPDB_Int4            givenMaxLevel,
/*SUN*/                                    void **               callArray,
/*SUN*/                                    OutputCallBackFuncPtr pOutput,
/*SUN*/                                    void * const          pOutContext )
/*SUN*/{
/*SUN*/    ucontext_t     *context;
/*SUN*/    SAPDB_Int4      depth;
/*SUN*/    unsigned long   pc;
/*SUN*/    struct frame   *fp;
/*SUN*/    ucontext_t      localContext;
/*SUN*/    SAPDB_Byte * stackLimit;
/*SUN*/    SAPDB_Byte * oldFp;
/*SUN*/
/*SUN*/    if ( getcontext (&localContext) )
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Failed to get ucontext\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        return 0;
/*SUN*/    }
/*SUN*/    context = &localContext;
/*SUN*/
/*SUN*/    pc = context->uc_mcontext.gregs[REG_PC]; /* program counter           */
#if defined(SPARC) || defined(SUN)
/*SUN*/    fp = BiasedFramePtr((void *)context->uc_mcontext.gregs[REG_SP]); /* stack pointer*/
#elif defined(X86_64) || defined(I386)
/*SOLARIS_X86_64*/ fp = BiasedFramePtr((void *)context->uc_mcontext.gregs[REG_FP]); /* frame pointer*/
#endif
/*SUN*/
/*SUN*/    stackLimit = eo670_GetStackLimit(fp);
/*SUN*/    oldFp = ((SAPDB_Byte *)fp)-1;
/*SUN*/    depth = 0;
/*SUN*/    if ( depth < givenMaxLevel && skipCount <= 0 )
/*SUN*/    {
/*SUN*/        callArray[depth] = ((char *)0) + pc;
/*SUN*/        ++depth;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        --skipCount;
/*SUN*/    }
/*SUN*/
/*SUN*/    while ( depth < givenMaxLevel
#if defined(SPARC) || defined(SUN)
/*SUN*/        && (((long)pc)&3) == 0
#endif
/*SUN*/        && ((SAPDB_Byte *)fp) >  oldFp
/*SUN*/        && ((SAPDB_Byte *)fp) <= stackLimit
/*SUN*/        && (((long)fp)&3) == 0 )
/*SUN*/    {
/*SUN*/        oldFp = (SAPDB_Byte *)fp;
/*SUN*/        pc = fp->fr_savpc;
/*SUN*/
/*SUN*/        if ( skipCount > 0 )
/*SUN*/        {
/*SUN*/            --skipCount;
/*SUN*/	       }
/*SUN*/	       else
/*SUN*/        {
/*SUN*/            callArray[depth] = ((char *)0) + pc;
/*SUN*/            ++depth;
/*SUN*/        }
/*SUN*/
/*SUN*/        fp = BiasedFramePtr((void*)fp->fr_savfp);
/*SUN*/    }
/*SUN*/    return depth;
/*SUN*/}
/*SUN*/
/*SUN*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*SUN*/                              SAPDB_Int4            maxLevel,
/*SUN*/                              SAPDB_Int4 *          pFilledLevel,
/*SUN*/                              eo670_CallArrayType   addCallInfo,
/*SUN*/                              void **               callArray,
/*SUN*/                              void*                 pBTraceContext,
/*SUN*/                              OutputCallBackFuncPtr pOutput,
/*SUN*/                              void * const          pOutContext )
/*SUN*/{
/*SUN*/    static ucontext_t      *context;
/*SUN*/    static unsigned long   pc;
/*SUN*/    static struct frame   *fp;
/*SUN*/    static struct frame   *oldfp;
/*SUN*/    static SAPDB_Int4 givenMaxLevel;
/*SUN*/
/*SUN*/    givenMaxLevel = maxLevel;
/*SUN*/    *pFilledLevel = 0;
/*SUN*/
/*SUN*/    if ( 0 == pBTraceContext )
/*SUN*/    {
/*SUN*/        static ucontext_t localContext;
/*SUN*/        if ( getcontext (&localContext) )
/*SUN*/        {
/*SUN*/            eo670WriteOutput("Failed to get ucontext\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/            return;
/*SUN*/        }
/*SUN*/        context = &localContext;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        skipCount = 0;
/*SUN*/        context = (ucontext_t *)pBTraceContext;
/*SUN*/    }
/*SUN*/
/*SUN*/    pc = context->uc_mcontext.gregs[REG_PC]; /* program counter           */
#if defined(SPARC) || defined(SUN)
/*SUN*/    fp = BiasedFramePtr((void *)context->uc_mcontext.gregs[REG_SP]); /* stack pointer*/
#elif defined(X86_64) || defined(I386)
/*SUN*/    fp = BiasedFramePtr((void *)context->uc_mcontext.gregs[REG_FP]); /* frame pointer*/
#endif
/*SUN*/
/*SUN*/    if ( *pFilledLevel < givenMaxLevel && skipCount <= 0 )
/*SUN*/    {
/*SUN*/        switch ( addCallInfo )
/*SUN*/        {
/*SUN*/        case eo670_FullInfoArray:
/*SUN*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (char *)0 + pc;
/*SUN*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = fp;
#if defined(SPARC) || defined(SUN)
/*SUN*/            if ( fp && (0 == (((long)fp)&(sizeof(long)-1))) )
/*SUN*/            {
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = fp->fr_arg[0];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = fp->fr_arg[1];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = fp->fr_arg[2];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = fp->fr_arg[3];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[4] = fp->fr_arg[4];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[5] = fp->fr_arg[5];
/*SUN*/            }
/*SUN*/            else
/*SUN*/            {
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = 0xdead;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = 0xc0de;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = 0xdead;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = 0xc0de;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[4] = 0xdead;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[5] = 0xc0de;
/*SUN*/            }
#endif
/*SUN*/            break;
/*SUN*/        case eo670_PointerArray:
/*SUN*/            callArray[*pFilledLevel] = ((char *)0) + pc;
/*SUN*/            break;
/*SUN*/        default: /* eo670_NoArrayJustOutput */
/*SUN*/            if ( fp && (0 == (((long)fp)&(sizeof(long)-1))) )
/*SUN*/            {
#if defined(SPARC) || defined(SUN)
/*SUN*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                             "(%d):0x%lx [0x%lx] (0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*SUN*/                             *pFilledLevel, pc, fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3], fp->fr_arg[4], fp->fr_arg[5] );
#elif defined(X86_64) || defined(I386)
/*SOLARIS_X86_64*/     sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                  "(%d):0x%lx [0x%lx]\n",
/*SOLARIS_X86_64*/                  *pFilledLevel, pc, fp );
#endif
/*SUN*/            }
/*SUN*/            else
/*SUN*/            {
/*SUN*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                             "(%d):0x%lx [0x%lx] (?,?,?,?,?,?)\n",
/*SUN*/                             *pFilledLevel, pc, fp );
/*SUN*/            }
/*SUN*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/            break;
/*SUN*/        }
/*SUN*/        ++*pFilledLevel;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        --skipCount;
/*SUN*/    }
/*SUN*/
/*SUN*/    oldfp = fp - 1;
/*SUN*/    while ( *pFilledLevel < givenMaxLevel
#if defined(SPARC) || defined(SUN)
/*SUN*/        && (((long)pc)&3) == 0
#endif
/*SUN*/        &&  fp
/*SUN*/        && (((long)fp)&3) == 0
/*SUN*/        && (fp > oldfp)
#if defined(SPARC) || defined(SUN) 
/*SUN*/	    && ((((unsigned long) BiasedFramePtr((void*)fp->fr_savfp)) + SP_BIAS) >= USRTEXT) )
#elif defined(X86_64) || defined(I386)
/*SOLARIS_X86_64*/         && ((unsigned long)fp->fr_savfp >= USRTEXT) )
#endif
/*SUN*/    {
/*SUN*/        pc = fp->fr_savpc;
/*SUN*/
/*SUN*/        if ( skipCount > 0 )
/*SUN*/        {
/*SUN*/            --skipCount;
/*SUN*/	       }
/*SUN*/	       else
/*SUN*/        {
/*SUN*/            switch ( addCallInfo )
/*SUN*/            {
/*SUN*/            case eo670_FullInfoArray:
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (char *)0 + pc;
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = fp;
#if defined(SPARC) || defined(SUN)
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[0] = fp->fr_arg[0];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[1] = fp->fr_arg[1];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[2] = fp->fr_arg[2];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[3] = fp->fr_arg[3];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[4] = fp->fr_arg[4];
/*SUN*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].arg[5] = fp->fr_arg[5];
#endif
/*SUN*/                break;
/*SUN*/            case eo670_PointerArray:
/*SUN*/                callArray[*pFilledLevel] = ((char *)0) + pc;
/*SUN*/                break;
/*SUN*/            default: /* eo670_NoArrayJustOutput */
#if defined(SPARC) || defined(SUN)
/*SUN*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                             "(%d):0x%lx [0x%lx] (0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*SUN*/                             *pFilledLevel, pc, fp, fp->fr_arg[0], fp->fr_arg[1], fp->fr_arg[2], fp->fr_arg[3], fp->fr_arg[4], fp->fr_arg[5] );
#elif defined(X86_64) || defined(I386)
/*SOLARIS_X86_64*/     sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOLARIS_X86_64*/                  "(%d):0x%lx [0x%lx]\n",
/*SOLARIS_X86_64*/                  *pFilledLevel, pc, fp );
#endif
/*SUN*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/                break;
/*SUN*/            }
/*SUN*/            ++*pFilledLevel;
/*SUN*/        }
/*SUN*/
/*SUN*/        oldfp = fp;
/*SUN*/        fp = BiasedFramePtr((void*)fp->fr_savfp);
/*SUN*/    }
/*SUN*/}
/*SUN*/
/*SUN*//*---------------------------------------------------------------------------*/
/*SUN*/
#    if defined(WANT_DEMANGLING_SUPPORT)
/*SUN*/static char *eo670_SunDemangle(char *symbol)
/*SUN*/{
/*SUN*/    static char   demanglingBuffer[MAX_SYMBOL_LENGTH];
/*SUN*/    cplus_demangle(symbol, demanglingBuffer, sizeof(demanglingBuffer));
/*SUN*/    return &demanglingBuffer[0];
/*SUN*/}
#    endif /* WANT_DEMANGLING_SUPPORT */
/*SUN*/
/*SUN*//*---------------------------------------------------------------------------*/
/*SUN*/
/*SUN*/static void eo670_FillAddressInfo( char *                pcAddr,
/*SUN*/                                   eo670AddressInfo *    pAddrInfo,
/*SUN*/                                   OutputCallBackFuncPtr pOutput,
/*SUN*/                                   void * const          pOutContext )
/*SUN*/{
/*SUN*/  static char              *mapname;
/*SUN*/  static eo670_ModuleChain *modulePointer;
/*SUN*/  static int                fd;
/*SUN*/
/*SUN*/    SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
/*SUN*/    pAddrInfo->sourceFileName = ""; /* No Source information on SUN... */
/*SUN*/    pAddrInfo->lineNumber = -1;
/*SUN*/    mapname = "";
/*SUN*/
/*SUN*/    for ( modulePointer = eo670_ModuleAnchor
/*SUN*/        ; modulePointer
/*SUN*/        ; modulePointer = modulePointer->next )
/*SUN*/    {
/*SUN*/        if (  ( pcAddr >= ((char *)modulePointer->text_mapaddr) )
/*SUN*/           && ( pcAddr <  ((char *)modulePointer->text_endaddr) ) )
/*SUN*/        {
/*SUN*/            mapname = (char *)modulePointer->osDependend;
/*SUN*/	           break;
/*SUN*/   	   }
/*SUN*/    }
/*SUN*/
/*SUN*/    if ( !modulePointer )
/*SUN*/    {
/*SUN*/        pAddrInfo->demangledSymbol = "";
/*SUN*/        pAddrInfo->offsetToSymbol = -1;
/*SUN*/        pAddrInfo->moduleName = "<not in any loaded module>";
/*SUN*/        pAddrInfo->symbol = "<not in any loaded region>";
/*SUN*/        return;
/*SUN*/    }
/*SUN*/
/*SUN*/    pAddrInfo->moduleName = modulePointer->file_name;
/*SUN*/
/*SUN*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/                "/proc/%d/object/%s", getpid(), mapname);
/*SUN*/    fd = open(eo670MessageBuffer, O_RDONLY);
/*SUN*/    if ( fd < 0 )
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Failed to open DLL to retrieve symbols\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        pAddrInfo->symbol = "<no symbols for this region>";
/*SUN*/        pAddrInfo->demangledSymbol = "";
/*SUN*/        pAddrInfo->offsetToSymbol = -1;
/*SUN*/        return;
/*SUN*/    }
/*SUN*/
/*SUN*/    if ( !eo670_ELFFindSymbol(fd, pcAddr, modulePointer->text_mapaddr, &pAddrInfo->symbol,
/*SUN*/                              &pAddrInfo->offsetToSymbol, pOutput, pOutContext) )
/*SUN*/    {
/*SUN*/        pAddrInfo->symbol = "<symbol lookup failed>";
/*SUN*/        pAddrInfo->demangledSymbol = "";
/*SUN*/        pAddrInfo->offsetToSymbol = -1;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
#    if defined(WANT_DEMANGLING_SUPPORT)
#      if defined(KERNEL_LZU)
/*SUN*/      if ( kgs && XPARAM(allowDemangle) )
#      else
/*SUN*/      if ( pAddrInfo->symbol )
#      endif /* KERNEL_LZU */
/*SUN*/      {
/*SUN*/         pAddrInfo->demangledSymbol = eo670_demangle( pAddrInfo->symbol, eo670_SunDemangle);
/*SUN*/      }
/*SUN*/      else
#    endif /* WANT_DEMANGLING_SUPPORT */
/*SUN*/      {
/*SUN*/         pAddrInfo->demangledSymbol = "";
/*SUN*/      }
/*SUN*/    }
/*SUN*/    close(fd);
/*SUN*/}
/*SUN*/
/*SUN*/static SAPDB_Bool eo670_UnixGetAddressInfo( void *                pcAddr,
/*SUN*/                                            eo670AddressInfo *    pAddrInfo,
/*SUN*/                                            OutputCallBackFuncPtr pOutput,
/*SUN*/                                            void * const          pOutContext )
/*SUN*/{
/*SUN*/    int    fillResult;
/*SUN*/    static eo670AddressInfo localAddrInfo;
/*SUN*/
/*SUN*/    eo670_FillAddressInfo(pcAddr, &localAddrInfo, pOutput, pOutContext);
/*SUN*/    pAddrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*SUN*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*SUN*/                                 + strlen(localAddrInfo.moduleName)+1
/*SUN*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*SUN*/    if ( !pAddrInfo->symbol )
/*SUN*/    {
/*SUN*/        eo670WriteOutput("Out of memory for symbol copy\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/        fillResult = 0;
/*SUN*/    }
/*SUN*/    else
/*SUN*/    {
/*SUN*/        strcpy(pAddrInfo->symbol, localAddrInfo.symbol);
/*SUN*/        pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*SUN*/        strcpy(pAddrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*SUN*/        pAddrInfo->moduleName = pAddrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*SUN*/        strcpy(pAddrInfo->moduleName, localAddrInfo.moduleName);
/*SUN*/        pAddrInfo->sourceFileName = pAddrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*SUN*/        strcpy(pAddrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*SUN*/        pAddrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*SUN*/        pAddrInfo->lineNumber = localAddrInfo.lineNumber;
/*SUN*/        fillResult = 1;
/*SUN*/    }
/*SUN*/    return fillResult;
/*SUN*/}
/*SUN*/
/*SUN*/static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*SUN*/{
/*SUN*/    free(pAddrInfo->symbol); /* allocated in one chunk.... */
/*SUN*/}
/*SUN*/
#    ifndef HAS_NO_ADDITIONAL_FRAME_INFO
/*SUN*/static void eo670_UnixShowCallInfoAddon( eo670_CallInfo *      callInfo,
/*SUN*/                                         OutputCallBackFuncPtr pOutput,
/*SUN*/                                         void * const          pOutContext )
/*SUN*/{
/*SUN*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SUN*/         "      Frameinfo [0x%lx] (0x%lx,0x%lx,0x%lx,0x%lx,0x%lx,0x%lx)\n",
/*SUN*/         (long)callInfo->frame,
/*SUN*/         callInfo->arg[0],
/*SUN*/         callInfo->arg[1],
/*SUN*/         callInfo->arg[2],
/*SUN*/         callInfo->arg[3],
/*SUN*/         callInfo->arg[4],
/*SUN*/         callInfo->arg[5] );
/*SUN*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SUN*/}
#    endif /* HAS_NO_ADDITIONAL_FRAME_INFO */
#  endif /* SUN */

#  ifdef HPUX
#    if defined(PA20W) && !defined(HP_IA64)
/*HPUX_PA20W*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*HPUX_PA20W*/                                OutputCallBackFuncPtr pOutput,
/*HPUX_PA20W*/                                void * const          pOutContext )
/*HPUX_PA20W*/{
/*HPUX_PA20W*/    static ucontext_t *context;
/*HPUX_PA20W*/    static unsigned long *pRegs;
/*HPUX_PA20W*/    static int        iReg;
/*HPUX_PA20W*/
/*HPUX_PA20W*/    context = (ucontext_t *)pBTraceContext;
/*HPUX_PA20W*/
/*HPUX_PA20W*/    pRegs = (unsigned long *)&(context->uc_mcontext.ss_wide.ss_64);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    eo670WriteOutput( "Aliases: r2  == rp,   r23 == arg3, r24 == arg2\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    eo670WriteOutput( "         r25 == arg1, r26 == arg0, r27 == dp\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    eo670WriteOutput( "         r28 == ret0, r29 == ret1, r30 == sp\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    eo670WriteOutput( "\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    for ( iReg = 0; iReg < 16; iReg++ )
/*HPUX_PA20W*/    {
/*HPUX_PA20W*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                     "r%02d  r%02d 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                      2*iReg,
/*HPUX_PA20W*/                      1+2*iReg,
/*HPUX_PA20W*/                      pRegs[2*iReg],
/*HPUX_PA20W*/                      pRegs[1+2*iReg] );
/*HPUX_PA20W*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/    }
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cpustate 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cpustate);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "pcoqHead pcsqHead 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_pcoq_head,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_pcsq_head);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "pcoqTail pcsqTail 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_pcoq_tail,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_pcsq_tail);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr0  cr8  0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr0,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr8);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr9  cr10 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr9,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr10);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "sar  cr12 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr11,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr12);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr13 cr15 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr13,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr15);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr19 cr20 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr19,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr20);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr21 cr22 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr21,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr22);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr24 cr25 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr24,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr25);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "cr26 cr27 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr26,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_cr27);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "sr0 sr1 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr0,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr1);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "sr2 sr3 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr2,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr3);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "sr4 sr5 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr4,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr5);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA20W*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                 "sr6 sr7 0x%016lx 0x%016lx\n",
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr6,
/*HPUX_PA20W*/                 context->uc_mcontext.ss_wide.ss_64.ss_sr7);
/*HPUX_PA20W*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/        for ( iReg = 0; iReg < (sizeof(fp_dbl_block_t)/sizeof(double)); iReg++ )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                     "f%02d      [0x%016lx]%.20e\n",
/*HPUX_PA20W*/                      iReg,
/*HPUX_PA20W*/                      *(SAPDB_UInt8 *)&((double *)&context->uc_mcontext.ss_fpblock.fpdbl)[iReg],
/*HPUX_PA20W*/                      ((double *)&context->uc_mcontext.ss_fpblock.fpdbl)[iReg] );
/*HPUX_PA20W*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/
/*HPUX_PA20W*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/
/*HPUX_PA20W*/}
/*HPUX_PA20W*/
/*HPUX_PA20W*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA20W*/
/*HPUX_PA20W*/typedef unsigned long general_reg;
/*HPUX_PA20W*/
/*HPUX_PA20W*/typedef struct {
/*HPUX_PA20W*/    unsigned long unwind_descriptor_bits;
/*HPUX_PA20W*/    unsigned int region_start_address;
/*HPUX_PA20W*/    unsigned int region_end_address;
/*HPUX_PA20W*/} uw_rec_def;
/*HPUX_PA20W*/
/*HPUX_PA20W*/typedef struct {
/*HPUX_PA20W*/    unsigned long size;
/*HPUX_PA20W*/    general_reg   sp;
/*HPUX_PA20W*/    general_reg   return_link_offset;
/*HPUX_PA20W*/    general_reg   gp;
/*HPUX_PA20W*/    general_reg   rp;
/*HPUX_PA20W*/    general_reg   mrp;
/*HPUX_PA20W*/    general_reg   r3;
/*HPUX_PA20W*/    general_reg   r4;
/*HPUX_PA20W*/    unsigned long reserved[4];
/*HPUX_PA20W*/} curr_frame_info;
/*HPUX_PA20W*/
/*HPUX_PA20W*/typedef struct {
/*HPUX_PA20W*/    unsigned long size;
/*HPUX_PA20W*/    general_reg   sp;
/*HPUX_PA20W*/    general_reg   return_link_offset;
/*HPUX_PA20W*/    general_reg   gp;
/*HPUX_PA20W*/    uw_rec_def    uw_rec;
/*HPUX_PA20W*/    long          uw_index;
/*HPUX_PA20W*/    general_reg   r3;
/*HPUX_PA20W*/    general_reg   r4;
/*HPUX_PA20W*/    unsigned long reserved[4];
/*HPUX_PA20W*/} prev_frame_info;
/*HPUX_PA20W*/
/*HPUX_PA20W*/extern curr_frame_info U_get_current_frame();
/*HPUX_PA20W*/extern int             U_get_previous_frame(curr_frame_info *pCurr, prev_frame_info *pPrev);
/*HPUX_PA20W*/extern void            U_copy_frame_info(curr_frame_info *pCurr, prev_frame_info *pPrev);
/*HPUX_PA20W*/
/*HPUX_PA20W*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA20W*/
/*HPUX_PA20W*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_PA20W*/                                    SAPDB_Int4            givenMaxLevel,
/*HPUX_PA20W*/                                    void **               callArray,
/*HPUX_PA20W*/                                    OutputCallBackFuncPtr pOutput,
/*HPUX_PA20W*/                                    void * const          pOutContext )
/*HPUX_PA20W*/{
/*HPUX_PA20W*/    curr_frame_info   context;
/*HPUX_PA20W*/    prev_frame_info   prevContext;
/*HPUX_PA20W*/    SAPDB_Int4        depth;
/*HPUX_PA20W*/
/*HPUX_PA20W*/    depth = 0;
/*HPUX_PA20W*/    context = U_get_current_frame();
/*HPUX_PA20W*/
/*HPUX_PA20W*/    while ( depth < givenMaxLevel )
/*HPUX_PA20W*/    {
/*HPUX_PA20W*/        U_get_previous_frame(&context, &prevContext);
/*HPUX_PA20W*/        if ( !prevContext.return_link_offset )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            break;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/
/*HPUX_PA20W*/        U_copy_frame_info(&context, &prevContext);
/*HPUX_PA20W*/        if ( skipCount > 0 )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            --skipCount;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/        else
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/	          callArray[depth] = (void *)(((long)context.return_link_offset)&(~3));
/*HPUX_PA20W*/            ++depth;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/    }
/*HPUX_PA20W*/    return depth;
/*HPUX_PA20W*/}
/*HPUX_PA20W*/
/*HPUX_PA20W*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA20W*/
/*HPUX_PA20W*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_PA20W*/                              SAPDB_Int4            maxLevel,
/*HPUX_PA20W*/                              SAPDB_Int4 *          pFilledLevel,
/*HPUX_PA20W*/                              eo670_CallArrayType   addCallInfo,
/*HPUX_PA20W*/                              void **               callArray,
/*HPUX_PA20W*/                              void*                 pBTraceContext,
/*HPUX_PA20W*/                              OutputCallBackFuncPtr pOutput,
/*HPUX_PA20W*/                              void * const          pOutContext )
/*HPUX_PA20W*/{
/*HPUX_PA20W*/    static curr_frame_info   context;
/*HPUX_PA20W*/    static prev_frame_info   prevContext;
/*HPUX_PA20W*/    static SAPDB_Int4 givenMaxLevel;
/*HPUX_PA20W*/
/*HPUX_PA20W*/    givenMaxLevel = maxLevel;
/*HPUX_PA20W*/    *pFilledLevel = 0;
/*HPUX_PA20W*/
/*HPUX_PA20W*/    if ( pBTraceContext )
/*HPUX_PA20W*/    {
/*HPUX_PA20W*/        static void             *crashPc;
/*HPUX_PA20W*/        ucontext_t *sigContext = (ucontext_t *)pBTraceContext;
/*HPUX_PA20W*/        context = U_get_current_frame();
/*HPUX_PA20W*/
/*HPUX_PA20W*/        crashPc = (char *)0 + (((long)sigContext->uc_mcontext.ss_wide.ss_64.ss_pcoq_head)&(~3));
/*HPUX_PA20W*/        switch ( addCallInfo )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/        case eo670_FullInfoArray:
/*HPUX_PA20W*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = crashPc;
/*HPUX_PA20W*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].frame =
/*HPUX_PA20W*/                  (char *)0 + (((long)sigContext->uc_mcontext.ss_wide.ss_64.ss_sp)&(~3));
/*HPUX_PA20W*/            break;
/*HPUX_PA20W*/        case eo670_PointerArray:
/*HPUX_PA20W*/	          callArray[*pFilledLevel] = crashPc;
/*HPUX_PA20W*/            break;
/*HPUX_PA20W*/        default: /* eo670_NoArrayJustOutput */
/*HPUX_PA20W*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                         "(%d):0x%lx [0x%lx]\n",
/*HPUX_PA20W*/                         *pFilledLevel,
/*HPUX_PA20W*/	                       crashPc,
/*HPUX_PA20W*/                         (((long)sigContext->uc_mcontext.ss_wide.ss_64.ss_sp)&(~3)) );
/*HPUX_PA20W*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/            break;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/        ++*pFilledLevel;
/*HPUX_PA20W*/        for ( skipCount = 256; skipCount > 0; --skipCount )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            U_get_previous_frame(&context, &prevContext);
/*HPUX_PA20W*/            if ( !prevContext.return_link_offset )
/*HPUX_PA20W*/            {
/*HPUX_PA20W*/                 return;
/*HPUX_PA20W*/            }
/*HPUX_PA20W*/
/*HPUX_PA20W*/            U_copy_frame_info(&context, &prevContext);
/*HPUX_PA20W*/	          if ( ((void *)(((long)context.return_link_offset)&(~3))) == crashPc )
/*HPUX_PA20W*/            {
/*HPUX_PA20W*/                 break;
/*HPUX_PA20W*/            }
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/        skipCount = 0;
/*HPUX_PA20W*/    }
/*HPUX_PA20W*/    else
/*HPUX_PA20W*/    {
/*HPUX_PA20W*/        context = U_get_current_frame();
/*HPUX_PA20W*/    }
/*HPUX_PA20W*/
/*HPUX_PA20W*/    while ( *pFilledLevel < givenMaxLevel )
/*HPUX_PA20W*/    {
/*HPUX_PA20W*/        U_get_previous_frame(&context, &prevContext);
/*HPUX_PA20W*/        if ( !prevContext.return_link_offset )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            break;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/
/*HPUX_PA20W*/        U_copy_frame_info(&context, &prevContext);
/*HPUX_PA20W*/        if ( skipCount > 0 )
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            --skipCount;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/        else
/*HPUX_PA20W*/        {
/*HPUX_PA20W*/            switch ( addCallInfo )
/*HPUX_PA20W*/            {
/*HPUX_PA20W*/            case eo670_FullInfoArray:
/*HPUX_PA20W*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr =
/*HPUX_PA20W*/                   (char *)0 + (((long)context.return_link_offset)&(~3));
/*HPUX_PA20W*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame =
/*HPUX_PA20W*/                   (char *)0 + (((long)context.sp)&(~3));
/*HPUX_PA20W*/                break;
/*HPUX_PA20W*/            case eo670_PointerArray:
/*HPUX_PA20W*/	              callArray[*pFilledLevel] = (void *)(((long)context.return_link_offset)&(~3));
/*HPUX_PA20W*/                break;
/*HPUX_PA20W*/            default: /* eo670_NoArrayJustOutput */
/*HPUX_PA20W*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA20W*/                             "(%d):0x%lx [0x%lx]\n",
/*HPUX_PA20W*/                             *pFilledLevel,
/*HPUX_PA20W*/	                           (((long)context.return_link_offset)&(~3)),
/*HPUX_PA20W*/                             (((long)context.sp)&(~3)) );
/*HPUX_PA20W*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA20W*/                break;
/*HPUX_PA20W*/            }
/*HPUX_PA20W*/            ++*pFilledLevel;
/*HPUX_PA20W*/        }
/*HPUX_PA20W*/    }
/*HPUX_PA20W*/}
/*HPUX_PA20W*/
/*HPUX_PA20W*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA20W*/
/*HPUX_PA20W*/ /* on 64-bit HP/UX the symbol pointer value is a pointer to a Procedure Descriptor */
/*HPUX_PA20W*/typedef struct {
/*HPUX_PA20W*/       unsigned long Reserved0;
/*HPUX_PA20W*/       unsigned long Reserved1;
/*HPUX_PA20W*/       void *Address;
/*HPUX_PA20W*/       void *GlobalPointer;
/*HPUX_PA20W*/} eo670_ProcedureDescriptor_t;
/*HPUX_PA20W*/
/*HPUX_PA20W*/static void *eo670_getFuncAddr ( void *pFunc )
/*HPUX_PA20W*/{
/*HPUX_PA20W*/	eo670_ProcedureDescriptor_t *pPD;
/*HPUX_PA20W*/	pPD = (eo670_ProcedureDescriptor_t *)pFunc;
/*HPUX_PA20W*/	return ( pPD ? pPD->Address : (void *)0 );
/*HPUX_PA20W*/}
/*HPUX_PA20W*/
#    elif defined(PA11)
/*HPUX_PA11*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*HPUX_PA11*/                                OutputCallBackFuncPtr pOutput,
/*HPUX_PA11*/                                void * const          pOutContext )
/*HPUX_PA11*/{
/*HPUX_PA11*/    static ucontext_t *context;
/*HPUX_PA11*/    static int        iReg;
/*HPUX_PA11*/
#if defined (BIT64)
/* register variables don't match for PA11 */
/*HPUX_PA11*/    context = (ucontext_t *)pBTraceContext;
/*HPUX_PA11*/
/*HPUX_PA11*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/    eo670WriteOutput( "Aliases: r2  == rp,   r23 == arg3, r24 == arg2\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/    eo670WriteOutput( "         r25 == arg1, r26 == arg0, r27 == dp\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/    eo670WriteOutput( "         r28 == ret0, r29 == ret1, r30 == sp\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/    eo670WriteOutput( "\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/    if ( UseWideRegs(&context->uc_mcontext) )
/*HPUX_PA11*/    {
/*HPUX_PA11*/      static int64_t *pRegs;
/*HPUX_PA11*/      pRegs = (int64_t *)&(context->uc_mcontext.ss_wide.ss_64);
/*HPUX_PA11*/      for ( iReg = 0; iReg < 16; iReg++ )
/*HPUX_PA11*/      {
/*HPUX_PA11*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                     "r%02d  r%02d 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                      2*iReg,
/*HPUX_PA11*/                      1+2*iReg,
/*HPUX_PA11*/                      (long)pRegs[2*iReg],
/*HPUX_PA11*/                      (long)pRegs[1+2*iReg] );
/*HPUX_PA11*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/      }
/*HPUX_PA11*/    }
/*HPUX_PA11*/    else
/*HPUX_PA11*/    {
/*HPUX_PA11*/      static unsigned int *pRegs;
/*HPUX_PA11*/      pRegs = (unsigned int *)&(context->uc_mcontext.ss_narrow);
/*HPUX_PA11*/      for ( iReg = 0; iReg < 16; iReg++ )
/*HPUX_PA11*/      {
/*HPUX_PA11*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                     "r%02d  r%02d 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                      2*iReg,
/*HPUX_PA11*/                      1+2*iReg,
/*HPUX_PA11*/                      pRegs[2*iReg],
/*HPUX_PA11*/                      pRegs[1+2*iReg] );
/*HPUX_PA11*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/      }
/*HPUX_PA11*/    }
/*HPUX_PA11*/
/*HPUX_PA11*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cpustate 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cpustate) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "pcoqHead pcsqHead 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_pcoq_head),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_pcsq_head) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "pcoqTail pcsqTail 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_pcoq_tail),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_pcsq_tail) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr0  cr8  0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr0),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr8) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr9  cr10 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr9),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr10) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "sar  cr12 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr11),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr12) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr13 cr15 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr13),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr15) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr19 cr20 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr19),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr20) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr21 cr22 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr21),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr22) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr24 cr25 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr24),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr25) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "cr26 cr27 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr26),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_cr27) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "sr0 sr1 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr0),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr1) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "sr2 sr3 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr2),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr3) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "sr4 sr5 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr4),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr5) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    sp77sprintf( eo670MessageBuffer,
/*HPUX_PA11*/                 sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                 "sr6 sr7 0x%016lx 0x%016lx\n",
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr6),
/*HPUX_PA11*/                 GetSSReg(&context->uc_mcontext, ss_sr7) );
/*HPUX_PA11*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/        for ( iReg = 0; iReg < (sizeof(fp_dbl_block_t)/sizeof(double)); iReg++ )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                     "f%02d      [0x%016lx]%.20e\n",
/*HPUX_PA11*/                      iReg,
/*HPUX_PA11*/                      *(SAPDB_UInt8 *)&((double *)&context->uc_mcontext.ss_fpblock.fpdbl)[iReg],
/*HPUX_PA11*/                      ((double *)&context->uc_mcontext.ss_fpblock.fpdbl)[iReg] );
/*HPUX_PA11*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/        }
/*HPUX_PA11*/
#endif
/*HPUX_PA11*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/
/*HPUX_PA11*/}
/*HPUX_PA11*/
/*HPUX_PA11*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA11*/
/*HPUX_PA11*/typedef struct Hp32CurrentFrame_ {
/*HPUX_PA11*/    unsigned curr_frame_size;
/*HPUX_PA11*/    unsigned curr_sp;
/*HPUX_PA11*/    unsigned curr_pcspace;
/*HPUX_PA11*/    unsigned curr_pcoffset;
/*HPUX_PA11*/    unsigned curr_dp;
/*HPUX_PA11*/    unsigned curr_rp;
/*HPUX_PA11*/    unsigned curr_mrp;
/*HPUX_PA11*/    unsigned curr_sr0;
/*HPUX_PA11*/    unsigned curr_sr4;
/*HPUX_PA11*/    unsigned r3;
/*HPUX_PA11*/    unsigned curr_r19;
/*HPUX_PA11*/    int r4;
/*HPUX_PA11*/    int reserved;
/*HPUX_PA11*/} Hp32CurrentFrame;
/*HPUX_PA11*/
/*HPUX_PA11*/typedef struct Hp32PreviousFrame_ {
/*HPUX_PA11*/    unsigned prev_frame_size;
/*HPUX_PA11*/    unsigned prev_sp;
/*HPUX_PA11*/    unsigned prev_pcspace;
/*HPUX_PA11*/    unsigned prev_pcoffset;
/*HPUX_PA11*/    unsigned prev_dp;
/*HPUX_PA11*/    unsigned udescr0;
/*HPUX_PA11*/    unsigned udescr1;
/*HPUX_PA11*/    unsigned ustart;
/*HPUX_PA11*/    unsigned uend;
/*HPUX_PA11*/    unsigned uw_index;
/*HPUX_PA11*/    unsigned prev_r19;
/*HPUX_PA11*/    int r3;
/*HPUX_PA11*/    int r4;
/*HPUX_PA11*/} Hp32PreviousFrame;
/*HPUX_PA11*/      /* defined in libcl */
/*HPUX_PA11*/extern void   U_get_frame_info(Hp32CurrentFrame *);
/*HPUX_PA11*/extern int    U_get_previous_frame_x(Hp32CurrentFrame *, Hp32PreviousFrame *, int);
/*HPUX_PA11*/
/*HPUX_PA11*/static void eo670_Hp32copy_prev_to_curr( Hp32CurrentFrame * currFrame,
/*HPUX_PA11*/                                         Hp32PreviousFrame *prevFrame )
/*HPUX_PA11*/{
/*HPUX_PA11*/    currFrame->curr_frame_size = prevFrame->prev_frame_size;
/*HPUX_PA11*/    currFrame->curr_sp         = prevFrame->prev_sp;
/*HPUX_PA11*/    currFrame->curr_pcspace    = prevFrame->prev_pcspace;
/*HPUX_PA11*/    currFrame->curr_pcoffset   = prevFrame->prev_pcoffset;
/*HPUX_PA11*/    currFrame->curr_dp         = prevFrame->prev_dp;
/*HPUX_PA11*/}
/*HPUX_PA11*/
/*HPUX_PA11*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA11*/
/*HPUX_PA11*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_PA11*/                                    SAPDB_Int4            givenMaxLevel,
/*HPUX_PA11*/                                    void **               callArray,
/*HPUX_PA11*/                                    OutputCallBackFuncPtr pOutput,
/*HPUX_PA11*/                                    void * const          pOutContext )
/*HPUX_PA11*/{
/*HPUX_PA11*/    Hp32CurrentFrame  currFrame;
/*HPUX_PA11*/    Hp32PreviousFrame prevFrame;
/*HPUX_PA11*/    SAPDB_Int4        depth;
/*HPUX_PA11*/    int status;
/*HPUX_PA11*/
/*HPUX_PA11*/    depth = 0;
/*HPUX_PA11*/        U_get_frame_info(&currFrame);
/*HPUX_PA11*/        currFrame.curr_sr0 = 0;
/*HPUX_PA11*/        currFrame.curr_sr4 = 0;
/*HPUX_PA11*/
/*HPUX_PA11*/    while ( depth < givenMaxLevel )
/*HPUX_PA11*/    {
/*HPUX_PA11*/        status = U_get_previous_frame_x(&currFrame, &prevFrame, sizeof(Hp32CurrentFrame));
/*HPUX_PA11*/        eo670_Hp32copy_prev_to_curr(&currFrame, &prevFrame);
/*HPUX_PA11*/        if ( !currFrame.curr_pcoffset )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        }
/*HPUX_PA11*/
/*HPUX_PA11*/        if ( status != 0 )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                         "Failed to get previous frame:%d\n", status);
/*HPUX_PA11*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        }
/*HPUX_PA11*/
/*HPUX_PA11*/        if ( skipCount > 0 )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            --skipCount;
/*HPUX_PA11*/        }
/*HPUX_PA11*/        else
/*HPUX_PA11*/        {
/*HPUX_PA11*/	          callArray[depth] = (void *)(((long)currFrame.curr_pcoffset)&(~3));
/*HPUX_PA11*/            ++depth;
/*HPUX_PA11*/        }
/*HPUX_PA11*/    }
/*HPUX_PA11*/    return depth;
/*HPUX_PA11*/}
/*HPUX_PA11*/
/*HPUX_PA11*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA11*/
/*HPUX_PA11*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_PA11*/                              SAPDB_Int4            maxLevel,
/*HPUX_PA11*/                              SAPDB_Int4 *          pFilledLevel,
/*HPUX_PA11*/                              eo670_CallArrayType   addCallInfo,
/*HPUX_PA11*/                              void **               callArray,
/*HPUX_PA11*/                              void*                 pBTraceContext,
/*HPUX_PA11*/                              OutputCallBackFuncPtr pOutput,
/*HPUX_PA11*/                              void * const          pOutContext )
/*HPUX_PA11*/{
/*HPUX_PA11*/    static Hp32CurrentFrame  currFrame;
/*HPUX_PA11*/    static Hp32PreviousFrame prevFrame;
/*HPUX_PA11*/    static int status;
/*HPUX_PA11*/    static SAPDB_Int4 givenMaxLevel;
/*HPUX_PA11*/
/*HPUX_PA11*/    givenMaxLevel = maxLevel;
/*HPUX_PA11*/    *pFilledLevel = 0;
/*HPUX_PA11*/
#if defined (BIT64)
/* register variables don't match for PA11 */
/*HPUX_PA11*/    if ( pBTraceContext )
/*HPUX_PA11*/    {
/*HPUX_PA11*/        static void *crashPc;
/*HPUX_PA11*/        ucontext_t *sigContext = (ucontext_t *)pBTraceContext;
/*HPUX_PA11*/        crashPc = ((char *)0+(WideOrNarrowSSReg32(&sigContext->uc_mcontext, ss_pcoq_head)&(~3)));
/*HPUX_PA11*/        U_get_frame_info(&currFrame);
/*HPUX_PA11*/        currFrame.curr_sr0 = 0;
/*HPUX_PA11*/        currFrame.curr_sr4 = 0;
/*HPUX_PA11*/        status = U_get_previous_frame_x(&currFrame, &prevFrame, sizeof(Hp32CurrentFrame));
/*HPUX_PA11*/        if ( status != 0 )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                         "Failed to get previous frame:%d\n", status);
/*HPUX_PA11*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/            return;
/*HPUX_PA11*/        }
/*HPUX_PA11*/        eo670_Hp32copy_prev_to_curr(&currFrame, &prevFrame);
/*HPUX_PA11*/        switch ( addCallInfo )
/*HPUX_PA11*/        {
/*HPUX_PA11*/        case eo670_FullInfoArray:
/*HPUX_PA11*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = crashPc;
/*HPUX_PA11*/            ((eo670_CallInfo *)callArray)[*pFilledLevel].frame =
/*HPUX_PA11*/	             ((char *)0+(WideOrNarrowSSReg32(&sigContext->uc_mcontext, ss_sp)&(~3)));
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        case eo670_PointerArray:
/*HPUX_PA11*/	          callArray[*pFilledLevel] = crashPc;
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        default: /* eo670_NoArrayJustOutput */
/*HPUX_PA11*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                         "(%d):0x%lx [0x%lx]\n",
/*HPUX_PA11*/                         *pFilledLevel,
/*HPUX_PA11*/	                       crashPc,
/*HPUX_PA11*/	                       WideOrNarrowSSReg32(&sigContext->uc_mcontext, ss_sp) );
/*HPUX_PA11*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        }
/*HPUX_PA11*/        ++*pFilledLevel;
/*HPUX_PA11*/        for ( skipCount = 256; skipCount > 0; --skipCount )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            if ( !currFrame.curr_pcoffset )
/*HPUX_PA11*/            {
/*HPUX_PA11*/               return;
/*HPUX_PA11*/            }
/*HPUX_PA11*/            status = U_get_previous_frame_x(&currFrame, &prevFrame, sizeof(Hp32CurrentFrame));
/*HPUX_PA11*/            if ( status != 0 )
/*HPUX_PA11*/            {
/*HPUX_PA11*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                         "Failed to get previous frame:%d\n", status);
/*HPUX_PA11*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/                return;
/*HPUX_PA11*/            }
/*HPUX_PA11*/            eo670_Hp32copy_prev_to_curr(&currFrame, &prevFrame);
/*HPUX_PA11*/	          if ( ((void *)(((long)currFrame.curr_pcoffset)&(~3))) == crashPc )
/*HPUX_PA11*/            {
/*HPUX_PA11*/                 break;
/*HPUX_PA11*/            }
/*HPUX_PA11*/        }
/*HPUX_PA11*/        skipCount = 0;
/*HPUX_PA11*/    }
/*HPUX_PA11*/    else
/*HPUX_PA11*/    {
/*HPUX_PA11*/        U_get_frame_info(&currFrame);
/*HPUX_PA11*/        currFrame.curr_sr0 = 0;
/*HPUX_PA11*/        currFrame.curr_sr4 = 0;
/*HPUX_PA11*/    }
/*HPUX_PA11*/
#endif
/*HPUX_PA11*/    while ( *pFilledLevel < givenMaxLevel )
/*HPUX_PA11*/    {
/*HPUX_PA11*/        status = U_get_previous_frame_x(&currFrame, &prevFrame, sizeof(Hp32CurrentFrame));
/*HPUX_PA11*/        eo670_Hp32copy_prev_to_curr(&currFrame, &prevFrame);
/*HPUX_PA11*/        if ( !currFrame.curr_pcoffset )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        }
/*HPUX_PA11*/
/*HPUX_PA11*/        if ( status != 0 )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                         "Failed to get previous frame:%d\n", status);
/*HPUX_PA11*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/            break;
/*HPUX_PA11*/        }
/*HPUX_PA11*/
/*HPUX_PA11*/        if ( skipCount > 0 )
/*HPUX_PA11*/        {
/*HPUX_PA11*/            --skipCount;
/*HPUX_PA11*/        }
/*HPUX_PA11*/        else
/*HPUX_PA11*/        {
/*HPUX_PA11*/            switch ( addCallInfo )
/*HPUX_PA11*/            {
/*HPUX_PA11*/            case eo670_FullInfoArray:
/*HPUX_PA11*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr =
/*HPUX_PA11*/                   (char *)0 + (currFrame.curr_pcoffset&(~3));
/*HPUX_PA11*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame =
/*HPUX_PA11*/                   (char *)0 + currFrame.curr_sp;
/*HPUX_PA11*/                break;
/*HPUX_PA11*/            case eo670_PointerArray:
/*HPUX_PA11*/	            callArray[*pFilledLevel] = (void *)(((long)currFrame.curr_pcoffset)&(~3));
/*HPUX_PA11*/                break;
/*HPUX_PA11*/            default: /* eo670_NoArrayJustOutput */
/*HPUX_PA11*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_PA11*/                             "(%d):0x%lx [0x%lx]\n",
/*HPUX_PA11*/                             *pFilledLevel,
/*HPUX_PA11*/	                           currFrame.curr_pcoffset&(~3),
/*HPUX_PA11*/                             currFrame.curr_sp );
/*HPUX_PA11*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_PA11*/                break;
/*HPUX_PA11*/            }
/*HPUX_PA11*/            ++*pFilledLevel;
/*HPUX_PA11*/        }
/*HPUX_PA11*/    }
/*HPUX_PA11*/}
/*HPUX_PA11*/
/*HPUX_PA11*/ /*----------------------------------------------------------------------------------*/
/*HPUX_PA11*/
/*HPUX_PA11*/typedef struct {
/*HPUX_PA11*/       void *Address;
/*HPUX_PA11*/       void *GlobalPointer;
/*HPUX_PA11*/} eo670_Hp32PLTEntry;
/*HPUX_PA11*/
/*HPUX_PA11*/static void *eo670_getFuncAddr ( void *FuncPtr )
/*HPUX_PA11*/{
/*HPUX_PA11*/ /* warning: it is assumed that sizeof (unsigned long) == sizeof (void *) */
/*HPUX_PA11*/	unsigned long ulFuncPtr;
/*HPUX_PA11*/ /* a function pointer on 32-bit HP/UX may be the address of */
/*HPUX_PA11*/ /* the pointer to an entry in the Procedure Label Table */
/*HPUX_PA11*/  if ( !FuncPtr ) return (void *)0;
/*HPUX_PA11*/
/*HPUX_PA11*/	ulFuncPtr = (char *)FuncPtr-(char *)0;
/*HPUX_PA11*/
/*HPUX_PA11*/	/* nothing to do if this is a real function address */	
/*HPUX_PA11*/	if ( 0 == (ulFuncPtr & 3) )
/*HPUX_PA11*/  {
/*HPUX_PA11*/       return FuncPtr;
/*HPUX_PA11*/  }
/*HPUX_PA11*/
/*HPUX_PA11*/	/* look up real address in Procedure Label Table (PLT) */
/*HPUX_PA11*/	ulFuncPtr &= ~3;
/*HPUX_PA11*/	return ((eo670_Hp32PLTEntry *)((char *)0+ulFuncPtr))->Address;
/*HPUX_PA11*/}
/*
  Function: eo670_SOMUpdateBestSymbol

  Helper routine, that updates the bestEntry and bestOffset found so far scanning the given symbol array
  If a better entry is found *pBestEntry and *pBestOffset are updated. If a matching entry was found,
  1 is returned else 0.
 */
/*SOM*/static int eo670_SOMUpdateBestSymbol( char *                           pcAddr,
/*SOM*/                                      caddr_t                          voffset,
/*SOM*/                                      unsigned int *                   pBestEntry,
/*SOM*/                                      SAPDB_Long *                     pBestOffset,
/*SOM*/                                      struct symbol_dictionary_record *bufferedSymtab,
/*SOM*/                                      size_t                           workSymCount )
/*SOM*/{
/*SOM*/    static int iSym;
/*SOM*/    for ( iSym = 0; iSym < workSymCount; iSym++ )
/*SOM*/    {
/*SOM*/        if ( ( bufferedSymtab[iSym].symbol_type == ST_CODE
/*SOM*/            || bufferedSymtab[iSym].symbol_type == ST_ENTRY )
/*SOM*/         &&  bufferedSymtab[iSym].symbol_scope == SS_UNIVERSAL )
/*SOM*/        {
/*SOM*/            static char * symAddr;
/*SOM*/            symAddr = voffset + (bufferedSymtab[iSym].symbol_value&0x3FFFFFFC);
/*SOM*/            if ( symAddr == pcAddr )
/*SOM*/            {
/*SOM*/                 *pBestOffset = 0;
/*SOM*/                 *pBestEntry  = bufferedSymtab[iSym].n_offset;
/*SOM*/                 return 1;
/*SOM*/            }
/*SOM*/
/*SOM*/            if (  (symAddr  < pcAddr)
/*SOM*/             && ( (*pBestOffset == (unsigned int)-1) || ((pcAddr - symAddr) < *pBestOffset) ) )
/*SOM*/            {
/*SOM*/                 *pBestOffset = pcAddr - symAddr;
/*SOM*/                 *pBestEntry  = bufferedSymtab[iSym].n_offset;
/*SOM*/            }
/*SOM*/        }
/*SOM*/    }
/*SOM*/    return 0;
/*SOM*/}
/*
  Function: eo670_SOMReadSymbol

  Helper routine, that reads a symbol. Symbol is overwritten with each call
 */
/*SOM*/static int eo670_SOMReadSymbol( int                   fd,
/*SOM*/                                unsigned int          stringTabOffset,
/*SOM*/                                unsigned int          entry,
/*SOM*/                                SAPDB_Char **         pSymbol,
/*SOM*/                                OutputCallBackFuncPtr pOutput,
/*SOM*/                                void * const          pOutContext )
/*SOM*/{
/*SOM*/    static SAPDB_Char lastSymbolBuffer[MAX_SYMBOL_LENGTH];
/*SOM*/
/*SOM*/    *pSymbol = (char *)"";
/*SOM*/    if ( lseek(fd, stringTabOffset + entry, SEEK_SET) != stringTabOffset + entry )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("Failed to seek to symbol string offset\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return 0;
/*SOM*/    }
/*SOM*/    if ( read(fd, lastSymbolBuffer, sizeof(lastSymbolBuffer)-1) <= 0 )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("Failed to read symbol from file\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return 0;
/*SOM*/    }
/*SOM*/    *pSymbol = lastSymbolBuffer;
/*SOM*/    return 1;
/*SOM*/}
/*SOM*/
/*
  Function: eo670_SOMFindSymbol

  Since symbol resolution should not use giant amount of memory (i.e. APO Dll tend to have a lot of symbols...
  this routine find the symbol for exactly one address by looking up symbol table without copying it into memory
  This will not produce a lot of seek overhead, since the actual symbol value of interest, once found, needs
  only one specific seek operation into the string table. The symbol table can be scanned in a single pass
  to find the best fitting symbol.
 */
/*SOM*/static int
/*SOM*/eo670_SOMFindSymbol( int                   fd,
/*SOM*/                     char *                pcAddr,
/*SOM*/                     caddr_t               voffset,
/*SOM*/                     SAPDB_Char **         pSymbol,
/*SOM*/                     SAPDB_Long *          pOffsetToSymbol,
/*SOM*/                     OutputCallBackFuncPtr pOutput,
/*SOM*/                     void * const          pOutContext )
/*SOM*/{
/*SOM*/  static  struct header          fileHdr;          /* File header               */
/*SOM*/  static  struct som_exec_auxhdr auxHdr;           /* HPUX-auxhdr               */
/*SOM*/  static  size_t                 syms;
/*SOM*/  static  unsigned int           bestEntry;
/*SOM*/  static  SAPDB_Long             bestOffset;
#define MAX_BUFFERED_SYMBOLS 4096
/*SOM*/  static  struct symbol_dictionary_record bufferedSymtab[MAX_BUFFERED_SYMBOLS];
/*SOM*/  static  SAPDB_Int8             systemRc;
/*SOM*/
/*SOM*/    /* Get the file header                                            */
/*SOM*/
/*SOM*/    if ( (systemRc = read (fd, &fileHdr, sizeof(fileHdr))) != sizeof(fileHdr))
/*SOM*/    {
/*SOM*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOM*/                    "Warning - reading SOM filehdr failed got %d expected %d error %d\n",
/*SOM*/                    systemRc, sizeof(fileHdr), errno );
/*SOM*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return(0);
/*SOM*/    }
/*SOM*/
/*SOM*/    if (fileHdr.symbol_total == 0 )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("No symbols in filehdr defined\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return(0);
/*SOM*/    }
/*SOM*/
/*SOM*/    if ( (systemRc = read (fd, &auxHdr, sizeof(auxHdr))) != sizeof(auxHdr))
/*SOM*/    {
/*SOM*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOM*/                    "Warning - reading SOM HP-UX auxhdr failed got %d expected %d error %d\n",
/*SOM*/                    systemRc, sizeof(auxHdr), errno );
/*SOM*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return(0);
/*SOM*/    }
/*SOM*/
/*SOM*/    if ( auxHdr.som_auxhdr.type != HPUX_AUX_ID )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("Wrong type of aux header found\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return(0);
/*SOM*/    }
/*SOM*/
/*SOM*/    /* need exec_tmem from HPUX aux header as additional bias */
/*SOM*/    voffset -= auxHdr.exec_tmem;
/*SOM*/    bestEntry  = (unsigned int)-1;
/*SOM*/    bestOffset = (SAPDB_Long)-1;
/*SOM*/
/*SOM*/    /*Seek to symbol table section */
/*SOM*/    if ( (systemRc = lseek(fd, (long) fileHdr.symbol_location, SEEK_SET)) != fileHdr.symbol_location )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("Unable to do seek to SOM symbol table\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return(0);
/*SOM*/    }
/*SOM*/
/*SOM*/    for ( syms = fileHdr.symbol_total; syms > 0; )
/*SOM*/    {
/*SOM*/        static int workSymCount; /* number of symbols handled in this loop */
/*SOM*/
/*SOM*/        workSymCount = (syms > MAX_BUFFERED_SYMBOLS ? MAX_BUFFERED_SYMBOLS : syms);
/*SOM*/
/*SOM*/        if ( ( systemRc = read(fd, &bufferedSymtab[0], workSymCount * sizeof(struct symbol_dictionary_record)) )
/*SOM*/                                  != (workSymCount * sizeof(struct symbol_dictionary_record)) )
/*SOM*/        {
/*SOM*/            sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*SOM*/                        "Warning - reading SOM symbol table failed; got %d expected %d error %d\n",
/*SOM*/                        systemRc, workSymCount * sizeof(struct symbol_dictionary_record), errno);
/*SOM*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/            return(0);
/*SOM*/        }
/*SOM*/
/*SOM*/        if ( eo670_SOMUpdateBestSymbol( pcAddr, voffset,
/*SOM*/                                        &bestEntry, &bestOffset,
/*SOM*/                                        &bufferedSymtab[0], workSymCount) )
/*SOM*/        {
/*SOM*/            break; /* Find the best matching symbol */
/*SOM*/        }
/*SOM*/
/*SOM*/        syms -= workSymCount;
/*SOM*/    }
/*SOM*/
/*SOM*/    if ( bestEntry == -1 )
/*SOM*/    {
/*SOM*/        eo670WriteOutput("Symbol lookup failed\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*SOM*/        return 0;
/*SOM*/    }
/*SOM*/
/*SOM*/   *pOffsetToSymbol = bestOffset;
/*SOM*/
/*SOM*/   return eo670_SOMReadSymbol(fd, fileHdr.symbol_strings_location, bestEntry, pSymbol, pOutput, pOutContext);
/*SOM*/}
/*SOM*/ /*---------------------------------------------------------------------------------*/
#    elif defined(HP_IA64)
/*HPUX_IA64*/
/*HPUX_IA64*/typedef union
/*HPUX_IA64*/{
/*HPUX_IA64*/    uint64_t all;
/*HPUX_IA64*/    struct
/*HPUX_IA64*/    {
/*HPUX_IA64*/        unsigned long ppl : 2;
/*HPUX_IA64*/        unsigned long reserved1 : 4;
/*HPUX_IA64*/        unsigned long pec : 6;
/*HPUX_IA64*/        unsigned long reserved2 : 14;
/*HPUX_IA64*/        unsigned long rrb_pr : 6;
/*HPUX_IA64*/        unsigned long rrb_fr : 7;
/*HPUX_IA64*/        unsigned long rrb_gr : 7;
/*HPUX_IA64*/        unsigned long sor : 4;
/*HPUX_IA64*/        unsigned long sol : 7;
/*HPUX_IA64*/        unsigned long sof : 7;
/*HPUX_IA64*/    } f;
/*HPUX_IA64*/} FrameMarker;
/*HPUX_IA64*/
#              define S_NOT_VALID     "Not saved"
/*HPUX_IA64*/
/*HPUX_IA64*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*HPUX_IA64*/                                OutputCallBackFuncPtr pOutput,
/*HPUX_IA64*/                                void * const          pOutContext )
/*HPUX_IA64*/{
/*HPUX_IA64*/    struct uwx_env *env;
/*HPUX_IA64*/    struct uwx_self_info *info;
/*HPUX_IA64*/    FrameMarker cfm, pfs;
/*HPUX_IA64*/    __fpreg fpval;
/*HPUX_IA64*/    uint64_t val, mask, unat;
/*HPUX_IA64*/    char fp_buf[24];
/*HPUX_IA64*/    int i, buflen;
/*HPUX_IA64*/    int status;
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Initialize unwind express. */
/*HPUX_IA64*/    env = uwx_init();
/*HPUX_IA64*/    info = uwx_self_init_info(env);
/*HPUX_IA64*/    status = uwx_register_callbacks(env, (intptr_t)info,
/*HPUX_IA64*/                                    uwx_self_copyin,
/*HPUX_IA64*/                                    uwx_self_lookupip);
/*HPUX_IA64*/
/*HPUX_IA64*/    if ( pBTraceContext )
/*HPUX_IA64*/    {   /* If we have a valid signal context, use it */
/*HPUX_IA64*/        static void             *crashPc;
/*HPUX_IA64*/        ucontext_t *sigContext = (ucontext_t *)pBTraceContext;
/*HPUX_IA64*/        status = uwx_self_init_from_sigcontext(env, info,
/*HPUX_IA64*/                                               sigContext);
/*HPUX_IA64*/    }
/*HPUX_IA64*/    else
/*HPUX_IA64*/    {   /* Create context for ourselves */
/*HPUX_IA64*/        status = uwx_self_init_context(env);
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Dump IP, RP, SP and GP */
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_IP, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "IP=0x%016lx\n" :
/*HPUX_IA64*/                "IP=" S_NOT_VALID "\n", val);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_RP, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "RP=0x%016lx\n" :
/*HPUX_IA64*/                "RP=" S_NOT_VALID "\n", val);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_SP, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "SP=0x%016lx\n" :
/*HPUX_IA64*/                "SP=" S_NOT_VALID "\n", val);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_GR(1), &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "GP=0x%016lx\n" :
/*HPUX_IA64*/                "GP=" S_NOT_VALID "\n", val);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* dump current and previous frame markers. */
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_CFM, &cfm.all);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ?
/*HPUX_IA64*/                "               CFM: rrb.pr=%3d rrb.fr=%3d "
/*HPUX_IA64*/                "rrb.gr=%3d sor=%2d sol=%3d sof=%3d\n" :
/*HPUX_IA64*/                "               CFM: " S_NOT_VALID "\n",
/*HPUX_IA64*/                cfm.f.rrb_pr, cfm.f.rrb_fr, cfm.f.rrb_gr, 
/*HPUX_IA64*/                cfm.f.sor, cfm.f.sol, cfm.f.sof);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/    
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_PFS, &pfs.all);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ?
/*HPUX_IA64*/                "ar.pfs: ppl=%d pec=%d rrb.pr=%3d rrb.fr=%3d "
/*HPUX_IA64*/                "rrb.gr=%3d sor=%2d sol=%3d sof=%3d\n" :
/*HPUX_IA64*/                "ar.pfs: " S_NOT_VALID "\n",
/*HPUX_IA64*/                pfs.f.ppl, pfs.f.pec, pfs.f.rrb_pr, pfs.f.rrb_fr, 
/*HPUX_IA64*/                pfs.f.rrb_gr, pfs.f.sor, pfs.f.sol, pfs.f.sof);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* dump miscellaneous ARs like LC, FPSR, RNAT and BSP */
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_LC, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "ar.lc=%d\n" :
/*HPUX_IA64*/                "ar.lc = " S_NOT_VALID "\n", val);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_FPSR, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "ar.fpsr: sf3=%04x sf2=%04x "
/*HPUX_IA64*/                "sf1=%04x sf0=%04x traps=%02x\n" :
/*HPUX_IA64*/                "ar.fpsr: " S_NOT_VALID "\n", 
/*HPUX_IA64*/                (val>>45)&0x1fff, (val>>32)&0x1fff, 
/*HPUX_IA64*/                (val>>19)&0x1fff, (val>>6)&0x1fff, val&0x1f);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_RNAT, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "ar.rnat=0x%016lx\n" :
/*HPUX_IA64*/                "ar.rnat: " S_NOT_VALID "\n", val); 
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_BSP, &val);
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                status==UWX_OK ? "ar.bsp=0x%016lx\n" :
/*HPUX_IA64*/                "ar.bsp: " S_NOT_VALID "\n", val); 
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Get ar.unat.  Use it when displaying GRs */
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_UNAT, &unat);
/*HPUX_IA64*/    if (status != UWX_OK)
/*HPUX_IA64*/        unat = 0;
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Dump callee save and register stack GRs */
/*HPUX_IA64*/    sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                "GR4 - GR7 & GR32 - GR%d\n", 32+cfm.f.sof);
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 4, mask = 1L<<4; i <= 7; i++, mask <<= 1)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        if ((i%4) == 0)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                        "%3d:", i);
/*HPUX_IA64*/
/*HPUX_IA64*/        status = uwx_get_reg(env, UWX_REG_GR(i), &val);
/*HPUX_IA64*/
/*HPUX_IA64*/        buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (status != UWX_OK)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "     Not saved    ");
/*HPUX_IA64*/        else if (unat & mask)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "        NaT       ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "0x%016lx", val);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (i%4 != 3)
/*HPUX_IA64*/            strcat(eo670MessageBuffer, " ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/            eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 32, mask = 1L<<32; i < 32+cfm.f.sof; i++, mask <<= 1)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        if ((i%4) == 0)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                        "%3d:", i);
/*HPUX_IA64*/
/*HPUX_IA64*/        status = uwx_get_reg(env, UWX_REG_GR(i), &val);
/*HPUX_IA64*/
/*HPUX_IA64*/        buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (status != UWX_OK)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "     Not saved    ");
/*HPUX_IA64*/        else if (unat & mask)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "               NaT");
/*HPUX_IA64*/        else
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "0x%016lx", val);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (i%4 != 3)
/*HPUX_IA64*/            strcat(eo670MessageBuffer, " ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/            eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    if ((i-1)%4 != 3)
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Dump predicate registers displaying them bit by bit. */
/*HPUX_IA64*/    eo670WriteOutput("PRs 63 60        50        40        30"
/*HPUX_IA64*/                     "        20        10         0\n",
/*HPUX_IA64*/                     NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    strcpy(eo670MessageBuffer, "     ");
/*HPUX_IA64*/    buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/    status = uwx_get_reg(env, UWX_REG_PREDS, &val);
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 63, mask = 1l<<63; i >= 0; i--, mask >>= 1)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        eo670MessageBuffer[buflen++] = (val & mask) ? '1' : '0';
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    eo670MessageBuffer[buflen] = '\0';
/*HPUX_IA64*/
/*HPUX_IA64*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                     pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Show BR1 through BR5 (callee saves) */
/*HPUX_IA64*/    eo670WriteOutput("BR1 - BR5\n",
/*HPUX_IA64*/                     NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 1; i <= 5; i++)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        if (((i-1)%4) == 0)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                        "%3d:", i);
/*HPUX_IA64*/
/*HPUX_IA64*/        status = uwx_get_reg(env, UWX_REG_BR(i), &val);
/*HPUX_IA64*/
/*HPUX_IA64*/        buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (status != UWX_OK)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "     Not saved    ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "0x%016lx", val);
/*HPUX_IA64*/
/*HPUX_IA64*/        if ((i-1)%4 != 3)
/*HPUX_IA64*/            strcat(eo670MessageBuffer, " ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/            eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    if ((i-2)%4 != 3)
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    /* Show FR2 through FR5 & FR16 through FR31 (callee saves) */
/*HPUX_IA64*/    eo670WriteOutput("FR2 - FR5 & FR16 - FR31\n",
/*HPUX_IA64*/                     NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 2; i <= 5; i++)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        if (((i-2)%4) == 0)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                        "%3d:", i);
/*HPUX_IA64*/
/*HPUX_IA64*/        status = uwx_get_reg(env, UWX_REG_FR(i), 
/*HPUX_IA64*/                             (uint64_t *)&fpval);
/*HPUX_IA64*/
/*HPUX_IA64*/        buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (status != UWX_OK)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "     Not saved    ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            sprintf(fp_buf, "%- 18.10g", (double)fpval);
/*HPUX_IA64*/            strcat(eo670MessageBuffer, fp_buf);
/*HPUX_IA64*/        }
/*HPUX_IA64*/
/*HPUX_IA64*/        if ((i-2)%4 != 3)
/*HPUX_IA64*/            strcat(eo670MessageBuffer, " ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/            eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    if ((i-3)%4 != 3)
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/    eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/
/*HPUX_IA64*/    for (i = 16; i <= 31; i++)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        if ((i%4) == 0)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                        "%3d:", i);
/*HPUX_IA64*/
/*HPUX_IA64*/        status = uwx_get_reg(env, UWX_REG_FR(i), 
/*HPUX_IA64*/                             (uint64_t *)&fpval);
/*HPUX_IA64*/
/*HPUX_IA64*/        buflen = strlen(eo670MessageBuffer);
/*HPUX_IA64*/
/*HPUX_IA64*/        if (status != UWX_OK)
/*HPUX_IA64*/            sp77sprintf(eo670MessageBuffer + buflen,
/*HPUX_IA64*/                        sizeof(eo670MessageBuffer) - buflen,
/*HPUX_IA64*/                        "     Not saved    ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            sprintf(fp_buf, "%- 18.10g", (double)fpval);
/*HPUX_IA64*/            strcat(eo670MessageBuffer, fp_buf);
/*HPUX_IA64*/        }
/*HPUX_IA64*/
/*HPUX_IA64*/        if (i%4 != 3)
/*HPUX_IA64*/            strcat(eo670MessageBuffer, " ");
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/            eo670MessageBuffer[0] = '\0';
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    if ((i-1)%4 != 3)
/*HPUX_IA64*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT,
/*HPUX_IA64*/                             pOutput, pOutContext);
/*HPUX_IA64*/
/*HPUX_IA64*/    return;
/*HPUX_IA64*/}
/*HPUX_IA64*/
/*HPUX_IA64*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_IA64*/                                    SAPDB_Int4            givenMaxLevel,
/*HPUX_IA64*/                                    void **               callArray,
/*HPUX_IA64*/                                    OutputCallBackFuncPtr pOutput,
/*HPUX_IA64*/                                    void * const          pOutContext )
/*HPUX_IA64*/{
/*HPUX_IA64*/    struct uwx_env *env;
/*HPUX_IA64*/    struct uwx_self_info *info;
/*HPUX_IA64*/    uint64_t ip;
/*HPUX_IA64*/    int status;
/*HPUX_IA64*/    SAPDB_Int4        depth;
/*HPUX_IA64*/
/*HPUX_IA64*/    env = uwx_init();
/*HPUX_IA64*/    info = uwx_self_init_info(env);
/*HPUX_IA64*/    status = uwx_register_callbacks(env, (intptr_t)info,
/*HPUX_IA64*/                                    uwx_self_copyin,
/*HPUX_IA64*/                                    uwx_self_lookupip);
/*HPUX_IA64*/    status = uwx_self_init_context(env);
/*HPUX_IA64*/
/*HPUX_IA64*/    depth = 0;
/*HPUX_IA64*/
/*HPUX_IA64*/    while (depth < givenMaxLevel)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        status = uwx_step(env);
/*HPUX_IA64*/        if (status == UWX_ABI_FRAME)
/*HPUX_IA64*/        {    /* Unwind past signal context frame if we find one */
/*HPUX_IA64*/             status = uwx_self_do_context_frame(env, info);
/*HPUX_IA64*/        }
/*HPUX_IA64*/        else if (status != UWX_OK)
/*HPUX_IA64*/            break;
/*HPUX_IA64*/
/*HPUX_IA64*/        if ( skipCount > 0 )
/*HPUX_IA64*/        {
/*HPUX_IA64*/            --skipCount;
/*HPUX_IA64*/        }
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/             status = uwx_get_reg(env, UWX_REG_IP, &ip);
/*HPUX_IA64*/             callArray[depth] = (void *)ip;
/*HPUX_IA64*/            ++depth;
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    uwx_self_free_info(info);
/*HPUX_IA64*/    uwx_free(env);
/*HPUX_IA64*/
/*HPUX_IA64*/    return depth;
/*HPUX_IA64*/}
/*HPUX_IA64*/
/*HPUX_IA64*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*HPUX_IA64*/                              SAPDB_Int4            maxLevel,
/*HPUX_IA64*/                              SAPDB_Int4 *          pFilledLevel,
/*HPUX_IA64*/                              eo670_CallArrayType   addCallInfo,
/*HPUX_IA64*/                              void **               callArray,
/*HPUX_IA64*/                              void*                 pBTraceContext,
/*HPUX_IA64*/                              OutputCallBackFuncPtr pOutput,
/*HPUX_IA64*/                              void * const          pOutContext )
/*HPUX_IA64*/{
/*HPUX_IA64*/    struct uwx_env *env;
/*HPUX_IA64*/    struct uwx_self_info *info;
/*HPUX_IA64*/    uint64_t ip, sp;
/*HPUX_IA64*/    int status;
/*HPUX_IA64*/
/*HPUX_IA64*/    env = uwx_init();
/*HPUX_IA64*/    info = uwx_self_init_info(env);
/*HPUX_IA64*/    status = uwx_register_callbacks(env, (intptr_t)info,
/*HPUX_IA64*/                                    uwx_self_copyin,
/*HPUX_IA64*/                                    uwx_self_lookupip);
/*HPUX_IA64*/
/*HPUX_IA64*/    if ( pBTraceContext )
/*HPUX_IA64*/    {   /* Create unwind context from signal context */
/*HPUX_IA64*/        ucontext_t *sigContext = (ucontext_t *)pBTraceContext;
/*HPUX_IA64*/        status = uwx_self_init_from_sigcontext(env, info,
/*HPUX_IA64*/                                               sigContext);
/*HPUX_IA64*/        skipCount = 0;     /* Unwind whole stack */
/*HPUX_IA64*/    }
/*HPUX_IA64*/    else
/*HPUX_IA64*/    {   /* Create unwind context from current stack */
/*HPUX_IA64*/        status = uwx_self_init_context(env);
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    *pFilledLevel = 0;
/*HPUX_IA64*/
/*HPUX_IA64*/    while (*pFilledLevel < maxLevel)
/*HPUX_IA64*/    {
/*HPUX_IA64*/        status = uwx_step(env);
/*HPUX_IA64*/        if (status == UWX_ABI_FRAME)
/*HPUX_IA64*/        {    /* Unwind past signal context frame if we find one */
/*HPUX_IA64*/             status = uwx_self_do_context_frame(env, info);
/*HPUX_IA64*/        }
/*HPUX_IA64*/        else if (status != UWX_OK)
/*HPUX_IA64*/            break;
/*HPUX_IA64*/
/*HPUX_IA64*/        if ( skipCount > 0 )
/*HPUX_IA64*/        {
/*HPUX_IA64*/            --skipCount;
/*HPUX_IA64*/        }
/*HPUX_IA64*/        else
/*HPUX_IA64*/        {
/*HPUX_IA64*/            switch ( addCallInfo )
/*HPUX_IA64*/            {
/*HPUX_IA64*/            case eo670_FullInfoArray:
/*HPUX_IA64*/                status = uwx_get_reg(env, UWX_REG_IP, &ip);
/*HPUX_IA64*/                status = uwx_get_reg(env, UWX_REG_SP, &sp);
/*HPUX_IA64*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr =
/*HPUX_IA64*/                   (void *)ip;
/*HPUX_IA64*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame =
/*HPUX_IA64*/                   (void *)sp;
/*HPUX_IA64*/                break;
/*HPUX_IA64*/            case eo670_PointerArray:
/*HPUX_IA64*/                status = uwx_get_reg(env, UWX_REG_IP, &ip);
/*HPUX_IA64*/                callArray[*pFilledLevel] = (void *)ip;
/*HPUX_IA64*/                break;
/*HPUX_IA64*/            default: /* eo670_NoArrayJustOutput */
/*HPUX_IA64*/                status = uwx_get_reg(env, UWX_REG_IP, &ip);
/*HPUX_IA64*/                status = uwx_get_reg(env, UWX_REG_SP, &sp);
/*HPUX_IA64*/                sp77sprintf( eo670MessageBuffer,
/*HPUX_IA64*/                             sizeof(eo670MessageBuffer),
/*HPUX_IA64*/                             "(%d):0x%lx [0x%lx]\n",
/*HPUX_IA64*/                             *pFilledLevel, ip, sp );
/*HPUX_IA64*/                eo670WriteOutput(eo670MessageBuffer,
/*HPUX_IA64*/                                 NO_DISPLACEMENT, pOutput,
/*HPUX_IA64*/                                 pOutContext);
/*HPUX_IA64*/                break;
/*HPUX_IA64*/            }
/*HPUX_IA64*/
/*HPUX_IA64*/            ++*pFilledLevel;
/*HPUX_IA64*/        }
/*HPUX_IA64*/    }
/*HPUX_IA64*/
/*HPUX_IA64*/    uwx_self_free_info(info);
/*HPUX_IA64*/    uwx_free(env);
/*HPUX_IA64*/}
#    endif /* HPUX */
/*HPUX*/ /*----------------------------------------------------------------------------------*/
/*HPUX*/extern void *U_get_shLib_text_addr(void *);
/*HPUX*/static SAPDB_Bool eo670_UpdateModuleChain( OutputCallBackFuncPtr pOutput,
/*HPUX*/                                     void * const          pOutContext )
/*HPUX*/{
/*HPUX*/    static int dlIndex;
/*HPUX*/    static int retCode;
/*HPUX*/    static struct shl_descriptor *dlDesc;
/*HPUX*/
/*HPUX*/    for ( dlIndex = 0; 0 == (retCode = shl_get(dlIndex, &dlDesc)); ++dlIndex )
/*HPUX*/    {
/*HPUX*/        /* eo670_AddToLoadedModuleChain would report errors,     */
/*HPUX*/        /* so void is ok since no local stoarge needed.          */
/*HPUX*/        /* The buffer returned is 'static' and content           */
/*HPUX*/        /* is copied in the called eo670_AddToLoadedModuleChain. */
/*HPUX*/        if ( eo670_AddToLoadedModuleChain(
/*HPUX*/                        dlDesc->filename,
/*HPUX*/                        (char *)0 + dlDesc->tstart,
/*HPUX*/                        (char *)0 + dlDesc->tend,
/*HPUX*/                        &dlIndex,
/*HPUX*/                        (SAPDB_ULong)sizeof(dlIndex),
/*HPUX*/                        pOutput, pOutContext ) < 0 )
/*HPUX*/        {
/*HPUX*/            return false;
/*HPUX*/        }
/*HPUX*/    }
/*HPUX*/    return true;
/*HPUX*/}
/*HPUX*/
/*HPUX*/ /*----------------------------------------------------------------------------------*/
/*HPUX*/ /*---------------------------------------------------------------------------------------*/
/*HPUX*/
/*HPUX*/static void eo670_FillAddressInfo( char *                pcAddr,
/*HPUX*/                                   eo670AddressInfo *    pAddrInfo,
/*HPUX*/                                   OutputCallBackFuncPtr pOutput,
/*HPUX*/                                   void * const          pOutContext )
/*HPUX*/{
/*HPUX*/   static eo670_ModuleChain *modulePointer;
/*HPUX*/   static struct shl_descriptor *dlDesc;
/*HPUX*/   static int dlIndex;
/*HPUX*/   static int retCode;
/*HPUX*/   static int fd;
/*HPUX*/
/*HPUX*/   SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
/*HPUX*/   pAddrInfo->sourceFileName = ""; /* No Source information on HPUX... */
/*HPUX*/   pAddrInfo->lineNumber = -1;
/*HPUX*/
/*HPUX*/   for ( modulePointer = eo670_ModuleAnchor
/*HPUX*/       ; modulePointer
/*HPUX*/       ; modulePointer = modulePointer->next )
/*HPUX*/   {
/*HPUX*/       if (  ( pcAddr >= ((char *)modulePointer->text_mapaddr) )
/*HPUX*/          && ( pcAddr <  ((char *)modulePointer->text_endaddr) )
/*HPUX*/          && (      0 !=          modulePointer->osDependend ) )
/*HPUX*/       {
/*HPUX*/           dlIndex = *(int *)modulePointer->osDependend;
/*HPUX*/           break;
/*HPUX*/   	   }
/*HPUX*/   }
/*HPUX*/
/*HPUX*/   if ( !modulePointer )
/*HPUX*/   {
/*HPUX*/       pAddrInfo->moduleName = "<not in any loaded module>";
/*HPUX*/       pAddrInfo->symbol = "<not in any loaded region>";
/*HPUX*/       pAddrInfo->demangledSymbol = "";
/*HPUX*/       pAddrInfo->offsetToSymbol = -1;
/*HPUX*/       return;
/*HPUX*/   }
/*HPUX*/
/*HPUX*/   pAddrInfo->moduleName = modulePointer->file_name;
/*HPUX*/
/*HPUX*/   retCode = shl_get(dlIndex, &dlDesc);
/*HPUX*/   if ( retCode != 0 )
/*HPUX*/   {
/*HPUX*/       eo670WriteOutput("Module index no longer valid\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX*/       pAddrInfo->symbol = "<module description no longer available>";
/*HPUX*/       pAddrInfo->demangledSymbol = "";
/*HPUX*/       pAddrInfo->offsetToSymbol = -1;
/*HPUX*/       return;
/*HPUX*/   }
/*HPUX*/
/*HPUX*/   fd = open(dlDesc->filename, O_RDONLY);
/*HPUX*/   if ( fd < 0 )
/*HPUX*/   {
/*HPUX*/       eo670WriteOutput("Failed to open DLL to retrieve symbols\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX*/       pAddrInfo->symbol = "<no symbols for this region>";
/*HPUX*/       pAddrInfo->demangledSymbol = "";
/*HPUX*/       pAddrInfo->offsetToSymbol = -1;
/*HPUX*/       return;
/*HPUX*/   }
/*HPUX*/
#ifdef BIT64
/*HPUX_PA20W||HPUX_IA64*/ if ( !eo670_ELFFindSymbol(fd, pcAddr, modulePointer->text_mapaddr, &pAddrInfo->symbol,
/*HPUX_PA20W||HPUX_IA64*/                           &pAddrInfo->offsetToSymbol, pOutput, pOutContext) )
#else
/*HPUX_PA11*/ if ( !eo670_SOMFindSymbol(fd, pcAddr, modulePointer->text_mapaddr,
/*HPUX_PA11*/                           &pAddrInfo->symbol, &pAddrInfo->offsetToSymbol, pOutput, pOutContext) )
#endif /* BIT64 */
/*HPUX*/   {
/*HPUX*/       pAddrInfo->symbol = "<symbol lookup failed>";
/*HPUX*/       pAddrInfo->demangledSymbol = "";
/*HPUX*/       pAddrInfo->offsetToSymbol = -1;
/*HPUX*/   }
/*HPUX*/   else
/*HPUX*/   {
#    if defined(WANT_DEMANGLING_SUPPORT)
#        if defined(KERNEL_LZU)
/*HPUX*/      if ( kgs && XPARAM(allowDemangle) )
#        else
/*HPUX*/      if ( pAddrInfo->symbol )
#        endif /* KERNEL_LZU */
/*HPUX*/      {
/*HPUX*/         pAddrInfo->demangledSymbol = "";
/*HPUX*/      }
/*HPUX*/      else
#    endif /* WANT_DEMANGLING_SUPPORT */
/*HPUX*/      {
/*HPUX*/         pAddrInfo->demangledSymbol = "";
/*HPUX*/      }
/*HPUX*/   }
/*HPUX*/   close(fd);
/*HPUX*/}
/*HPUX*/
/*HPUX*/static SAPDB_Bool eo670_UnixGetAddressInfo( void *                 pcAddr,
/*HPUX*/                                            eo670AddressInfo *     pAddrInfo,
/*HPUX*/                                            OutputCallBackFuncPtr  pOutput,
/*HPUX*/                                            void * const           pOutContext )
/*HPUX*/{
/*HPUX*/    static eo670AddressInfo localAddrInfo;
/*HPUX*/    static SAPDB_Bool fillResult;
/*HPUX*/
/*HPUX*/    eo670_FillAddressInfo((char *)pcAddr, &localAddrInfo, pOutput, pOutContext);
/*HPUX*/    pAddrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*HPUX*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*HPUX*/                                 + strlen(localAddrInfo.moduleName)+1
/*HPUX*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*HPUX*/    if ( !pAddrInfo->symbol )
/*HPUX*/    {
/*HPUX*/        eo670WriteOutput("Out of memory for symbol copy\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*HPUX*/        fillResult = 0;
/*HPUX*/    }
/*HPUX*/    else
/*HPUX*/    {
/*HPUX*/        strcpy(pAddrInfo->symbol, localAddrInfo.symbol);
/*HPUX*/        pAddrInfo->moduleName = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*HPUX*/        pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*HPUX*/        strcpy(pAddrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*HPUX*/        pAddrInfo->moduleName = pAddrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*HPUX*/        strcpy(pAddrInfo->moduleName, localAddrInfo.moduleName);
/*HPUX*/        pAddrInfo->sourceFileName = pAddrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*HPUX*/        strcpy(pAddrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*HPUX*/        pAddrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*HPUX*/        pAddrInfo->lineNumber = localAddrInfo.lineNumber;
/*HPUX*/        fillResult = 1;
/*HPUX*/    }
/*HPUX*/    return fillResult;
/*HPUX*/}
/*HPUX*/
/*HPUX*/static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*HPUX*/{
/*HPUX*/    free(pAddrInfo->symbol); /* allocated in one chunk.... */
/*HPUX*/}
/*HPUX*/
#  endif /*HPUX*/

#  if defined(OSF1)
/*OSF1*/struct modul_chain_elem
/*OSF1*/{
/*OSF1*/    ldr_module_info_t	      moduleInfo;
/*OSF1*/    ldr_region_info_t	      textInfo;
/*OSF1*/    elf32_LDFILE	         *ldptr;
/*OSF1*/    int			opened;		/* opened or at at least tried to open */
/*OSF1*/    int			symtab_ok;
/*OSF1*/};
/*OSF1*/typedef struct modul_chain_elem mce;
/*OSF1*/typedef	struct modul_chain_elem	*mce_p;
/*OSF1*/
/*OSF1*/static long ipd_adr	  (LDFILE *ldptr, unsigned long adr, PDR *pApd);
/*OSF1*/extern int ld_ifd_symnum(LDFILE *ldptr, int);
/*OSF1*/extern char * st_str_ifd_iss(int, int);
/*OSF1*/
/*OSF1*/static void eo670_FillAddressInfo( char *                pcAddr,
/*OSF1*/                                   eo670AddressInfo *    pAddrInfo,
/*OSF1*/                                   OutputCallBackFuncPtr pOutput,
/*OSF1*/                                   void * const          pOutContext )
/*OSF1*/{
/*OSF1*/  static mce_p	       me_p;
/*OSF1*/  static PDR	       apd;
/*OSF1*/  static SYMR	       asym;
/*OSF1*/  static char	      *pname;
/*OSF1*/  static long	       isym;
/*OSF1*/  static int	       ifd;
/*OSF1*/  static eo670_ModuleChain *modulePointer;
/*OSF1*/
/*OSF1*/    SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
/*OSF1*/
/*OSF1*/    for ( modulePointer = eo670_ModuleAnchor, me_p = 0
/*OSF1*/        ; modulePointer
/*OSF1*/        ; modulePointer = modulePointer->next )
/*OSF1*/    {
/*OSF1*/        if (  ( pcAddr >= ((char *)modulePointer->text_mapaddr) )
/*OSF1*/           && ( pcAddr <  ((char *)modulePointer->text_endaddr) ) )
/*OSF1*/        {
/*OSF1*/            me_p = (mce_p)modulePointer->osDependend;
/*OSF1*/	        break;
/*OSF1*/   	    }
/*OSF1*/    }
/*OSF1*/
/*OSF1*/    pAddrInfo->demangledSymbol = ""; /* not supported by DEC */
/*OSF1*/    pAddrInfo->offsetToSymbol = -1;
/*OSF1*/    pAddrInfo->sourceFileName = "";
/*OSF1*/    pAddrInfo->lineNumber = -1;
/*OSF1*/
/*OSF1*/   	if ( modulePointer )
/*OSF1*/    {
/*OSF1*/        pAddrInfo->moduleName = modulePointer->file_name;
/*OSF1*/	    if ( me_p && me_p->opened == 0 )
/*OSF1*/	    {
/*OSF1*/    	    me_p->opened = 1;
/*OSF1*/	       	me_p->ldptr = ldopen(modulePointer->file_name, 0);
/*OSF1*/       		if ( me_p->ldptr == 0 )
/*OSF1*/	        {
/*OSF1*/        	    sp77sprintf ( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/        			        "*** FillAddressInfo: Cannot ldopen %s (errno: %d)\n",
/*OSF1*/			                modulePointer->file_name, errno );
/*OSF1*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/            }
/*OSF1*/        	else
/*OSF1*/	        {
/*OSF1*/       		    if (PSYMTAB( me_p->ldptr ) == 0)
/*OSF1*/        	    {
/*OSF1*/       		        sp77sprintf ( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                                      "*** FillAddressInfo: No symbol table for module %s\n",
/*OSF1*/                                      modulePointer->file_name );
/*OSF1*/                    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/                } /* PSYMTAB( me_p->ldptr ) == 0 */
/*OSF1*/      		    else
/*OSF1*/	            {
/*OSF1*/               		me_p->symtab_ok = 1;
/*OSF1*/	           	}
/*OSF1*/	   	    }
/*OSF1*/        } /* if me_p->opened == 0 */
/*OSF1*/
/*OSF1*/	    if ( me_p && me_p->symtab_ok )
/*OSF1*/	    {
/*OSF1*/    	    if ( ipd_adr(me_p->ldptr, (unsigned long)(pcAddr-(char *)0), &apd) == FAILURE )
/*OSF1*/            {
/*OSF1*/	   	        isym = isymNil;
/*OSF1*/            }
/*OSF1*/            else
/*OSF1*/            {
/*OSF1*/	   	        isym = apd.isym;
/*OSF1*/            }
/*OSF1*/
/*OSF1*/	        if (isym != isymNil)
/*OSF1*/    	    {
/*OSF1*/	   	        if (ldtbread(me_p->ldptr, isym, &asym) == FAILURE)
/*OSF1*/                {
/*OSF1*/	                pAddrInfo->symbol = "<symbol not available>";
/*OSF1*/                }
/*OSF1*/                else
/*OSF1*/                {
/*OSF1*/                    pname = ldgetname(me_p->ldptr, &asym);
/*OSF1*/                    if ( 0 == pname )
/*OSF1*/                    {
/*OSF1*/                        pAddrInfo->symbol = "<symbol not found>";
/*OSF1*/                    }
/*OSF1*/                    else
/*OSF1*/                    {
/*OSF1*/                         pAddrInfo->symbol = pname;
/*OSF1*/                         pAddrInfo->offsetToSymbol = (pcAddr - (char *)apd.adr);
/*OSF1*/   		                 ifd = ld_ifd_symnum ( me_p->ldptr, apd.isym );
/*OSF1*/    	                 if ( ifd != -1 )
/*OSF1*/	                     {
/*OSF1*/                             pAddrInfo->sourceFileName = st_str_ifd_iss(ifd, 1);
/*OSF1*/                             pAddrInfo->lineNumber =
/*OSF1*/                                  SYMTAB(me_p->ldptr)->pline[apd.iline
/*OSF1*/						             + (pAddrInfo->offsetToSymbol/sizeof(int))];
/*OSF1*/		                 }
/*OSF1*/                    }
/*OSF1*/		        }
/*OSF1*/		    } /* isym != isymNil */
/*OSF1*/            else
/*OSF1*/	   	    {
/*OSF1*/	            pAddrInfo->symbol = "<symbol lookup failed>";
/*OSF1*/    	    }
/*OSF1*/
/*OSF1*/	    } /* me_p->symtab_ok == TRUE */		
/*OSF1*/	    else
/*OSF1*/        {
/*OSF1*/            pAddrInfo->symbol = "<no symbols available for this region>";
/*OSF1*/        }
/*OSF1*/    }
/*OSF1*/    else
/*OSF1*/    {
/*OSF1*/        pAddrInfo->moduleName = "<not in any loaded module>";
/*OSF1*/        pAddrInfo->symbol = "<not in any loaded region>";
/*OSF1*/    }
/*OSF1*/}
/*OSF1*/
/*OSF1*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*OSF1*/                                OutputCallBackFuncPtr pOutput,
/*OSF1*/                                void * const          pOutContext )
/*OSF1*/{
/*OSF1*/    static CONTEXT *context;
/*OSF1*/    static int iReg;
/*OSF1*/
/*OSF1*/    context = (CONTEXT *)pBTraceContext;
/*OSF1*/
/*OSF1*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/    for ( iReg = 0; iReg < (sizeof(context->sc_regs)/sizeof(long)/2); iReg++ )
/*OSF1*/    {
/*OSF1*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                     "r%02d  r%02d 0x%016lx 0x%016lx\n",
/*OSF1*/                      2*iReg,
/*OSF1*/                      1+2*iReg,
/*OSF1*/                      context->sc_regs[2*iReg],
/*OSF1*/                      context->sc_regs[1+2*iReg] );
/*OSF1*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/    }
/*OSF1*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/    sp77sprintf( eo670MessageBuffer,
/*OSF1*/                 sizeof(eo670MessageBuffer),
/*OSF1*/                 "pc       0x%016lx\n",
/*OSF1*/                 context->sc_pc);
/*OSF1*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/    sp77sprintf( eo670MessageBuffer,
/*OSF1*/                 sizeof(eo670MessageBuffer),
/*OSF1*/                 "ps       0x%016lx\n",
/*OSF1*/                 context->sc_ps);
/*OSF1*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/    if ( context->sc_ownedfp )
/*OSF1*/    {
/*OSF1*/        for ( iReg = 0; iReg < (sizeof(context->sc_fpregs)/sizeof(long)); iReg++ )
/*OSF1*/        {
/*OSF1*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                     "f%02d      [0x%016lx]%.20e\n",
/*OSF1*/                      iReg,
/*OSF1*/                      *(SAPDB_UInt8 *)&context->sc_fpregs[iReg],
/*OSF1*/                      *(double *)&context->sc_fpregs[iReg] );
/*OSF1*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/        }
/*OSF1*/        eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/        sp77sprintf( eo670MessageBuffer,
/*OSF1*/                 sizeof(eo670MessageBuffer),
/*OSF1*/                 "fpcr     0x%016lx\n",
/*OSF1*/                 context->sc_fpcr);
/*OSF1*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/    }
/*OSF1*/    else
/*OSF1*/    {
/*OSF1*/        eo670WriteOutput( "Floating point unit unused\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/    }
/*OSF1*/    eo670WriteOutput( "-------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/
/*OSF1*/}
/*OSF1*/
/*OSF1*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*OSF1*/                                    SAPDB_Int4            givenMaxLevel,
/*OSF1*/                                    void **               callArray,
/*OSF1*/                                    OutputCallBackFuncPtr pOutput,
/*OSF1*/                                    void * const          pOutContext )
/*OSF1*/{
/*OSF1*/    CONTEXT	         context;
/*OSF1*/    SAPDB_Int4       depth;     /* depth counter */
/*OSF1*/
/*OSF1*/    depth = 0;
/*OSF1*/
/*OSF1*/    exc_capture_context(&context);
/*OSF1*/
/*OSF1*/    do
/*OSF1*/    {
/*OSF1*/        PRUNTIME_FUNCTION prf;
/*OSF1*/
/*OSF1*/        if ( skipCount > 0 )
/*OSF1*/        {
/*OSF1*/            --skipCount;
/*OSF1*/	    }
/*OSF1*/	    else
/*OSF1*/        {
/*OSF1*/	        callArray[depth] = (void *)context.sc_pc;
/*OSF1*/            ++depth;
/*OSF1*/	    }
/*OSF1*/
/*OSF1*/	    prf = exc_lookup_function_entry (context.sc_pc);
/*OSF1*/        if ( 0 == prf ) /* pc did not point into code segment ... */
/*OSF1*/        {
/*OSF1*/            sp77sprintf( eo670MessageBuffer,
/*OSF1*/                         sizeof(eo670MessageBuffer),
/*OSF1*/                         "Damaged stack: exc_lookup_function_entry(0x%016p) failed\n",
/*OSF1*/                         context.sc_pc );
/*OSF1*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/            return depth;
/*OSF1*/	    }
/*OSF1*/	    exc_virtual_unwind (prf, &context); /* next frame */
/*OSF1*/
/*OSF1*/    } while ( context.sc_pc && (depth < givenMaxLevel) );
#    define SKIP_BOT	(1)
/*OSF1*/    if ( depth >= SKIP_BOT ) depth -= SKIP_BOT; /* eliminate last entry */
/*OSF1*/    return depth;
/*OSF1*/}
/*OSF1*/
/*OSF1*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*OSF1*/                              SAPDB_Int4            maxLevel,
/*OSF1*/                              SAPDB_Int4 *          pFilledLevel,
/*OSF1*/                              eo670_CallArrayType   addCallInfo,
/*OSF1*/                              void **               callArray,
/*OSF1*/                              void*                 pBTraceContext,
/*OSF1*/                              OutputCallBackFuncPtr pOutput,
/*OSF1*/                              void * const          pOutContext )
/*OSF1*/{
/*OSF1*/    static CONTEXT	         context;
/*OSF1*/    static char *            oldsp;
/*OSF1*/    static SAPDB_Int4 givenMaxLevel;
/*OSF1*/
/*OSF1*/    givenMaxLevel = maxLevel;
/*OSF1*/    *pFilledLevel = 0;
/*OSF1*/
/*OSF1*/    exc_capture_context(&context);
/*OSF1*/
/*OSF1*/    do
/*OSF1*/    {
/*OSF1*/        static PRUNTIME_FUNCTION prf;
/*OSF1*/
/*OSF1*/        oldsp = (char *)0 + context.sc_regs[30];
/*OSF1*/        if ( skipCount > 0 )
/*OSF1*/        {
/*OSF1*/            --skipCount;
/*OSF1*/	    }
/*OSF1*/	    else
/*OSF1*/        {
/*OSF1*/            switch ( addCallInfo )
/*OSF1*/            {
/*OSF1*/            case eo670_FullInfoArray:
/*OSF1*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr =
/*OSF1*/                   (char *)0 + context.sc_pc;
/*OSF1*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = oldsp;
/*OSF1*/                break;
/*OSF1*/            case eo670_PointerArray:
/*OSF1*/	            callArray[*pFilledLevel] = (void *)context.sc_pc;
/*OSF1*/                break;
/*OSF1*/            default: /* eo670_NoArrayJustOutput */
/*OSF1*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                             "(%d):0x%lx [0x%lx]\n",
/*OSF1*/                             *pFilledLevel, context.sc_pc, oldsp );
/*OSF1*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/                break;
/*OSF1*/            }
/*OSF1*/            ++*pFilledLevel;
/*OSF1*/	    }
/*OSF1*/
/*OSF1*/	    prf = exc_lookup_function_entry (context.sc_pc);
/*OSF1*/        if ( 0 == prf ) /* pc did not point into code segment ... */
/*OSF1*/        {
/*OSF1*/            sp77sprintf( eo670MessageBuffer,
/*OSF1*/                         sizeof(eo670MessageBuffer),
/*OSF1*/                         "Damaged stack: exc_lookup_function_entry(0x%016p) failed\n",
/*OSF1*/                         context.sc_pc );
/*OSF1*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/            return;
/*OSF1*/	    }
/*OSF1*/	    exc_virtual_unwind (prf, &context); /* next frame */
/*OSF1*/
/*OSF1*/    } while ( context.sc_pc && (*pFilledLevel < givenMaxLevel) && (((char *)0 + context.sc_regs[30]) > oldsp) );
#    define SKIP_BOT	(1)
/*OSF1*/    if ( *pFilledLevel >= SKIP_BOT ) *pFilledLevel -= SKIP_BOT; /* eliminate last entry */
/*OSF1*/}
/*OSF1*/
/*OSF1*/static SAPDB_Bool eo670_UpdateModuleChain( OutputCallBackFuncPtr pOutput,
/*OSF1*/                                     void * const          pOutContext )
/*OSF1*/{
/*OSF1*/    static ldr_process_t my_proc;
/*OSF1*/    static ldr_module_t	 module_id;
/*OSF1*/    static mce           moduleChainElement;
/*OSF1*/    static size_t		 ret_size;
/*OSF1*/    static int	         regionIndex;
/*OSF1*/
/*OSF1*/    my_proc = ldr_my_process();
/*OSF1*/    module_id = LDR_NULL_MODULE;
/*OSF1*/
/*OSF1*/	for ( ;; )
/*OSF1*/    {
/*OSF1*/        static int	 rc;
/*OSF1*/        static mce_p me_p; /* ModuleChainElement pointer */
/*OSF1*/        static ldr_module_info_t * mInfo;
/*OSF1*/
/*OSF1*/        me_p = &moduleChainElement;
/*OSF1*/        mInfo = &me_p->moduleInfo;
/*OSF1*/        rc = ldr_next_module ( my_proc, &module_id );
/*OSF1*/        if ( rc )
/*OSF1*/    	{
/*OSF1*/	        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                        "ldr_next_module failed:%d\n", errno);
/*OSF1*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/    	    return false;
/*OSF1*/	    }
/*OSF1*/
/*OSF1*/        /* end of module list found */
/*OSF1*/    	if ( module_id == LDR_NULL_MODULE)
/*OSF1*/	    {
/*OSF1*/    	    break;
/*OSF1*/	    }
/*OSF1*/
/*OSF1*/        /* inquire module information */
/*OSF1*/    	rc = ldr_inq_module ( my_proc,
/*OSF1*/                              module_id,
/*OSF1*/			                  mInfo,
/*OSF1*/                              sizeof(ldr_module_info_t),
/*OSF1*/			                 &ret_size );
/*OSF1*/	    if ( rc )
/*OSF1*/	    {
/*OSF1*/	        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                        "ldr_inq_module failed:%d\n", errno);
/*OSF1*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/    	    continue;
/*OSF1*/	    }
/*OSF1*/
/*OSF1*/        if ( mInfo->lmi_nregion > 0 )
/*OSF1*/        {
/*OSF1*/            static ldr_region_info_t *rInfo;
/*OSF1*/            rInfo = &me_p->textInfo;
/*OSF1*/
/*OSF1*/    	    for ( regionIndex = 0
/*OSF1*/	            ; regionIndex < mInfo->lmi_nregion
/*OSF1*/	            ; regionIndex++ )
/*OSF1*/	        {
/*OSF1*/	            rc = ldr_inq_region ( my_proc, module_id, regionIndex,
/*OSF1*/				                      rInfo, sizeof(ldr_region_info_t),
/*OSF1*/				                      &ret_size );
/*OSF1*/	            if ( rc )
/*OSF1*/	            {
/*OSF1*/                   sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*OSF1*/                               "ldr_inq_region failed for %s failed:%d\n",
/*OSF1*/                               mInfo->lmi_name,
/*OSF1*/                               errno);
/*OSF1*/                   eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/	    	       break;
/*OSF1*/	            }
/*OSF1*/                if ( 0 == memcmp(rInfo->lri_name, ".text", strlen(".text")+1) )
/*OSF1*/	            {
/*OSF1*/                   regionIndex = mInfo->lmi_nregion;
/*OSF1*/	            }
/*OSF1*/            }
/*OSF1*/
/*OSF1*/            if ( regionIndex < mInfo->lmi_nregion )
/*OSF1*/            {
/*OSF1*/	    	    continue; /* no text region in this module found */
/*OSF1*/	        }
/*OSF1*/
/*OSF1*/            if ( eo670_AddToLoadedModuleChain( mInfo->lmi_name,
/*OSF1*/                                                rInfo->lri_mapaddr,
/*OSF1*/                                                ((char *)rInfo->lri_mapaddr) + rInfo->lri_size,
/*OSF1*/                                                &moduleChainElement, sizeof(moduleChainElement),
/*OSF1*/                                                pOutput, pOutContext) < 0 )
/*OSF1*/            {
/*OSF1*/	    	    return false;
/*OSF1*/	        }
/*OSF1*/        }
/*OSF1*/	} /* for(;;) break at end of module list */
/*OSF1*/    return true;
/*OSF1*/}
/*OSF1*/
/*OSF1*/static SAPDB_Bool eo670_UnixGetAddressInfo( void *                pcAddr,
/*OSF1*/                                            eo670AddressInfo *    pAddrInfo,
/*OSF1*/                                            OutputCallBackFuncPtr pOutput,
/*OSF1*/                                            void * const          pOutContext )
/*OSF1*/{
/*OSF1*/  static eo670AddressInfo localAddrInfo;
/*OSF1*/  static SAPDB_Bool fillResult;
/*OSF1*/    eo670_FillAddressInfo((char *)pcAddr, &localAddrInfo, pOutput, pOutContext);
/*OSF1*/    pAddrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*OSF1*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*OSF1*/                                 + strlen(localAddrInfo.moduleName)+1
/*OSF1*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*OSF1*/    if ( !pAddrInfo->symbol )
/*OSF1*/    {
/*OSF1*/        eo670WriteOutput("Out of memory for symbol copy\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*OSF1*/        fillResult = 0;
/*OSF1*/    }
/*OSF1*/    else
/*OSF1*/    {
/*OSF1*/        strcpy(pAddrInfo->symbol, localAddrInfo.symbol);
/*OSF1*/        pAddrInfo->moduleName = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*OSF1*/        pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*OSF1*/        strcpy(pAddrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*OSF1*/        pAddrInfo->moduleName = pAddrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*OSF1*/        strcpy(pAddrInfo->moduleName, localAddrInfo.moduleName);
/*OSF1*/        pAddrInfo->sourceFileName = pAddrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*OSF1*/        strcpy(pAddrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*OSF1*/        pAddrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*OSF1*/        pAddrInfo->lineNumber = localAddrInfo.lineNumber;
/*OSF1*/        fillResult = 1;
/*OSF1*/    }
/*OSF1*/    return fillResult;
/*OSF1*/}
/*OSF1*/
/*OSF1*/static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*OSF1*/{
/*OSF1*/    free(pAddrInfo->symbol); /* allocated in one chunk.... */
/*OSF1*/}
/*OSF1*/
/*OSF1*/static long ipd_adr ( LDFILE *      ldptr,
/*OSF1*/                      unsigned long adr,
/*OSF1*/                      PDR *         pApd )
/*OSF1*/{
/*OSF1*/    static int ilow;
/*OSF1*/    static int ihigh;
/*OSF1*/    static int ibest;
/*OSF1*/    static unsigned long bestAdr;
/*OSF1*/
/*OSF1*/    ibest = -1;
/*OSF1*/    ihigh = SYMHEADER(ldptr).ipdMax-1;
/*OSF1*/    /* linear search proc table */
/*OSF1*/
/*OSF1*/    for ( ilow = 0; ilow <= ihigh; ++ilow )
/*OSF1*/    {
/*OSF1*/        if ( FAILURE == ldgetpd(ldptr, (long)ilow, pApd) )
/*OSF1*/        {
/*OSF1*/            return FAILURE;
/*OSF1*/        }
/*OSF1*/    	if ( (adr > pApd->adr)
/*OSF1*/          && ( (ibest == -1) || (bestAdr < pApd->adr) ) )
/*OSF1*/	    {
/*OSF1*/	        bestAdr = pApd->adr;
/*OSF1*/            ibest = ilow;
/*OSF1*/	    }
/*OSF1*/        if ( adr == pApd->adr )
/*OSF1*/        {
/*OSF1*/            return SUCCESS;
/*OSF1*/        }
/*OSF1*/    } /* while */
/*OSF1*/
/*OSF1*/    return ldgetpd(ldptr, (long)ibest, pApd);
/*OSF1*/}
#  endif /* OSF1 */

#  ifdef AIX
#    if defined(WANT_DEMANGLING_SUPPORT)
/*AIX*/
/*AIX*/static char *eo670_AixDemangle(char *symbol)
/*AIX*/{
/*AIX*/    static char demanglingBuffer[MAX_SYMBOL_LENGTH];
/*AIX*/    static char *AixDemangledSymbol;
/*AIX*/    static Name *demangledName;
/*AIX*/    static char *restSymbol;
/*AIX*/    demangledName = demangle(symbol, &restSymbol,
/*AIX*/                RegularNames | ClassNames | SpecialNames | ParameterText | QualifierText);
/*AIX*/    if ( demangledName )
/*AIX*/    {
/*AIX*/        AixDemangledSymbol = text(demangledName);
/*AIX*/        if ( AixDemangledSymbol )
/*AIX*/        {
/*AIX*/            if ( strlen(AixDemangledSymbol) >= MAX_SYMBOL_LENGTH )
/*AIX*/            {
/*AIX*/                SAPDB_memcpy( demanglingBuffer,
/*AIX*/                        AixDemangledSymbol,
/*AIX*/                        MAX_SYMBOL_LENGTH-1 );
/*AIX*/                demanglingBuffer[MAX_SYMBOL_LENGTH-1] = 0; /* zero termination added */
/*AIX*/            }
/*AIX*/            else
/*AIX*/            {
/*AIX*/                SAPDB_memcpy( demanglingBuffer,
/*AIX*/                        AixDemangledSymbol,
/*AIX*/                        strlen(AixDemangledSymbol)+1 ); /* copy in cludes zero termination */
/*AIX*/            }
/*AIX*/            AixDemangledSymbol = &demanglingBuffer[0];
/*AIX*/        }
/*AIX*/        else
/*AIX*/        {
/*AIX*/            AixDemangledSymbol = "";
/*AIX*/        }
/*AIX*/        /* erase(demangledName); */ /* Cannot free the Name * ---> this call produces SIGSEGV only ... */
/*AIX*/    }
/*AIX*/    else
/*AIX*/    {
/*AIX*/        AixDemangledSymbol = "";
/*AIX*/    }
/*AIX*/    return AixDemangledSymbol;
/*AIX*/}
/*AIX*/
#    endif /* WANT_DEMANGLING_SUPPORT */
/*AIX*/
/**************************************************************************/
/* All the following code is required for using the StackTrace() function */
/* Each function has a word of 0000 after it's code followed by a pointer */
/* to a traceback table.  Contained within the traceback table is the name*/
/* of that function.  Due to the possiblity of very large functions or    */
/* functions without this information, there is a literal defined below   */
/* called MAX_FUNC_SEARCH_LEN.  This is the maximum number of words that  */
/* will be searched (following any function return address located on the */
/* stack) before the getname function decides to just return NONAME.      */
/* IMPORTANT NOTE:  This function also sets up a signal handler for       */
/* SIGSEGV.  This is to handle having this function run off the end of    */
/* the code segment when attempting to locate the tracetable and function */
/* name.  Please NOTE that YOU MUST call sigset() after this function to  */
/* re-install/set your signal handler for the signal SIGSEGV!.            */
/**************************************************************************/
/* ADDITIONAL NOTE:  When AIX prepares to call into a new function it will*/
/* have a small section of the stack set aside for parameters/etc.  It    */
/* will then execute a BL instruction which will save the iar (Instruction*/
/* pointer after execution of the BL instruction) into the link regiser.  */
/*  The function that receives control will then normally move the link   */
/* register into the r0 register and from there it will move the r0 info  */
/* into the stack area set aside from the calling process.  It will do    */
/* this at an offset of 2 32 bit words higher than the current stack ptr. */
/*   Unfortunately, some functions (OS specific) will not save off the    */
/* link register information when entered.  This is because they are not  */
/* going to be passing control off to any lower levels using this stack.  */
/* So...You may sometimes notice 1 line of invalid function information   */
/* from this routine if there was a signal involved and the currently     */
/* executing code was somewhere inbetween the creation of the stack area  */
/* and the moving of the link register into that area after function      */
/* transfer via the bl instruction.  Due to this inaccuracy it is very    */
/* important to take all pre signal stack information carefully.  This    */
/* is especially true if the last instruction was a bctr - Russ Blaisdell */
/**************************************************************************/
/*AIX*/void eo670GetFuncName( unsigned int *lr,
/*AIX*/                       char *        funcname,
/*AIX*/                       size_t        maxLength );
#    define MAX_FUNC_SEARCH_LEN 5000
/*AIX*/static void eo670GetFuncNameLength(unsigned int *lr, char **pFuncname, size_t *pNameLength, unsigned int *pOffset)
/*AIX*/{
/*AIX*/static struct tbtable *tb;
/*AIX*/static unsigned int   *ip;
/*AIX*/static short          name_len;
/*AIX*/static unsigned int  searchcount;
/*AIX*/ searchcount = 0;
/*AIX*//*	First, search from the input LR address looking for a 32 bit work */
/*AIX*//*	of zeros. */
/*AIX*/	ip = lr;
/*AIX*/	while((*ip != 0) && (searchcount++ < MAX_FUNC_SEARCH_LEN)) ip++;
/*AIX*/
/*AIX*/	if(*ip != 0) return;
/*AIX*/
/*AIX*//* set up addressability to the traceback table. */
/*AIX*/	tb = (struct tbtable *) (ip+1);
/*AIX*/
/*AIX*//* 	Existence of fields in the tbtable extension are       */
/*AIX*//* 	contingent upon specific fields in the base table.     */
/*AIX*//* 	Check for their existence so that we can address the   */
/*AIX*//* 	function name if it exists.                            */
/*AIX*/
/*AIX*/        ip = (unsigned int *) tb +
/*AIX*/         		sizeof(struct tbtable_short)/sizeof(int);
/*AIX*/
/*AIX*/		if ( tb->tb.fixedparms != 0 || tb->tb.floatparms != 0 )
/*AIX*/      			ip++;
/*AIX*/
/*AIX*/		if ( tb->tb.has_tboff == TRUE )
/*AIX*/     {
/*AIX*/         if ( pOffset )
/*AIX*/         {
/*AIX*/             *pOffset = *ip - ((char *)tb - (char *)lr);
/*AIX*/         }
/*AIX*/			ip++;
/*AIX*/     }
/*AIX*/
/*AIX*/		if ( tb->tb.int_hndl == TRUE )
/*AIX*/			ip++;
/*AIX*/
/*AIX*/		if ( tb->tb.has_ctl == TRUE )
/*AIX*/			ip += (*ip) + 1; /* don't care */
/*AIX*/
/*AIX*//* Get function name if it exists. */
/*AIX*/		if ( tb->tb.name_present == TRUE )
/*AIX*/		{
/*AIX*/			*pNameLength = * (short *) ip;
/*AIX*/         *pFuncname = (char *)ip+2;
/*AIX*/		}
/*AIX*/     else
/*AIX*/		{
/*AIX*/         *pFuncname = "<symbol not available>";
/*AIX*/         *pNameLength = strlen(*pFuncname);
/*AIX*/		}
/*AIX*/}
/*AIX*/
/*AIX*/
/*AIX*/void eo670GetFuncName( unsigned int *lr,
/*AIX*/                       char *        funcname,
/*AIX*/                       size_t        maxLength )
/*AIX*/{
/*AIX*/    static size_t foundNameLength;
/*AIX*/    static char * foundFuncName;
/*AIX*/
/*AIX*/    eo670GetFuncNameLength(lr, &foundFuncName, &foundNameLength, (unsigned int *)0);
/*AIX*/    if ( foundNameLength >= maxLength )
/*AIX*/    {
/*AIX*/        foundNameLength = maxLength-1;
/*AIX*/    }
/*AIX*/    funcname[foundNameLength] = 0;
/*AIX*/    if ( foundNameLength > 0 )
/*AIX*/    {
/*AIX*/        SAPDB_memcpy (funcname, foundFuncName, foundNameLength);
/*AIX*/    }
/*AIX*/}
/*AIX*/
/*AIX*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*AIX*/                                OutputCallBackFuncPtr pOutput,
/*AIX*/                                void * const          pOutContext )
/*AIX*/{
/*AIX*/    static struct sigcontext *context;
/*AIX*/    static int iReg;
/*AIX*/
/*AIX*/    context = (struct sigcontext *)pBTraceContext;
/*AIX*/
/*AIX*/    eo670WriteOutput( "-----------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/    for ( iReg = 0; iReg < (_NGPRS/2); iReg++ )
/*AIX*/    {
/*AIX*/        sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*AIX*/                     "r%02d r%02d  0x%016qx 0x%016qx\n",
/*AIX*/                      2*iReg,
/*AIX*/                      1+2*iReg,
/*AIX*/                      context->sc_jmpbuf.jmp_context.gpr[2*iReg],
/*AIX*/                      context->sc_jmpbuf.jmp_context.gpr[1+2*iReg] );
/*AIX*/        eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/    }
/*AIX*/    eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "msr      0x%016qx\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.msr);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "iar      0x%016qx\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.iar);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "pc       0x%016qx\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.lr);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "ctr      0x%016qx\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.ctr);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "cr       0x%08x\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.cr);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "xer      0x%08x\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.xer);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "fpscr    0x%08x\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.fpscr);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "fpscrx   0x%08x\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.fpscrx);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "except   0x%016qx\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.except[0]);
/*AIX*/    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    eo670WriteOutput( "-----------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/    if ( context->sc_jmpbuf.jmp_context.fpeu )
/*AIX*/    {
/*AIX*/        for ( iReg = 0; iReg < _NFPRS; iReg++ )
/*AIX*/        {
/*AIX*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*AIX*/                     "f%02d      [0x%016qx]%.20e\n",
/*AIX*/                      iReg,
/*AIX*/                      *(SAPDB_UInt8 *)&context->sc_jmpbuf.jmp_context.fpr[iReg],
/*AIX*/                      *(double *)&context->sc_jmpbuf.jmp_context.fpr[iReg] );
/*AIX*/            eo670WriteOutput( eo670MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/        }
/*AIX*/        eo670WriteOutput("\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/        sp77sprintf( eo670MessageBuffer,
/*AIX*/                 sizeof(eo670MessageBuffer),
/*AIX*/                 "fpinfo   0x%02x\n",
/*AIX*/                 context->sc_jmpbuf.jmp_context.fpinfo);
/*AIX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/    }
/*AIX*/    else
/*AIX*/    {
/*AIX*/        eo670WriteOutput( "Floating point unit unused\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/    }
/*AIX*/    eo670WriteOutput( "-----------------------------------------------\n" , NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/
/*AIX*/}
/*AIX*/
/*AIX*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*AIX*/                                    SAPDB_Int4            givenMaxLevel,
/*AIX*/                                    void **               callArray,
/*AIX*/                                    OutputCallBackFuncPtr pOutput,
/*AIX*/                                    void * const          pOutContext )
/*AIX*/{
/*AIX*/    unsigned long *sp; /* stack pointer */
/*AIX*/    unsigned long *lr; /* link register */
/*AIX*/    ucontext_t localContext;
/*AIX*/    SAPDB_Int4     depth;
/*AIX*/    SAPDB_Byte * stackLimit;
/*AIX*/    SAPDB_Byte * oldFp;
/*AIX*/
/*AIX*/
/*AIX*/    if ( getcontext (&localContext) )
/*AIX*/    {
/*AIX*/        eo670WriteOutput("Failed to get ucontext\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/        return 0;
/*AIX*/    }
/*AIX*/    /* On PowerPC-AIX-Maschine the stack pointer is in GPR 1 */
/*AIX*/    sp = (unsigned long *)(localContext.uc_mcontext.jmp_context.gpr[1]);
/*AIX*/
/*AIX*/    stackLimit = eo670_GetStackLimit(sp);
/*AIX*/    oldFp = ((SAPDB_Byte *)sp)-1;
/*AIX*/    depth = 0;
/*AIX*/    /* Loop while we still have a valid stack pointer */
/*AIX*/    while( depth < givenMaxLevel
/*AIX*/       && ( (( (char*)sp - (char *)0 ) & (sizeof(unsigned long)-1)) == 0 )
/*AIX*/       && ((SAPDB_Byte *)sp) <= stackLimit
/*AIX*/       && ((SAPDB_Byte *)sp) > oldFp )
/*AIX*/    {
/*AIX*/        oldFp = (SAPDB_Byte *)sp;
/*AIX*/
/*AIX*/        /* See if this is a signal.  This is done by looking at the LR field  */
/*AIX*/        /* on the stack.  The LR is at offset long 2 (byte 8 in 32 bit mode   */
/*AIX*/        /* or byte 16 in 64 bit mode).  If the address is from segment 0,     */
/*AIX*/        /* then this must be a signal.                                        */
/*AIX*/        if (*(sp+2) < 0x0fffffff) 	/* check LR field on stack */
/*AIX*/    	   {
/*AIX*/            break;
/*AIX*/    	   }
/*AIX*/        if ( skipCount > 0 )
/*AIX*/        {
/*AIX*/            --skipCount;
/*AIX*/        }
/*AIX*/        else
/*AIX*/        {
/*AIX*/            /* Handle potential local stack allocations within a function  */
/*AIX*/            /* these produces extra frames which are skipped here          */
/*AIX*/            if ( 0 != *(sp+2) )
/*AIX*/            {
/*AIX*/                callArray[depth] = (void *)*(sp+2);
/*AIX*/                ++depth;
/*AIX*/            }
/*AIX*/        }
/*AIX*/
/*AIX*/        /* If here, the stack frame is not the result of a signal.  The LR is */
/*AIX*/        /* accessed at long + 2, but the pointer to the next stack frame is   */
/*AIX*/        /* pointed to by the contents of where the current pointer points.    */
/*AIX*/        sp = (unsigned long *) *sp;
/*AIX*/    }
/*AIX*/    return depth;
/*AIX*/}
/*AIX*/
/*AIX*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*AIX*/                              SAPDB_Int4            maxLevel,
/*AIX*/                              SAPDB_Int4 *          pFilledLevel,
/*AIX*/                              eo670_CallArrayType   addCallInfo,
/*AIX*/                              void **               callArray,
/*AIX*/                              void*                 pBTraceContext,
/*AIX*/                              OutputCallBackFuncPtr pOutput,
/*AIX*/                              void * const          pOutContext )
/*AIX*/{
/*AIX*/    static unsigned long *sp; /* stack pointer */
/*AIX*/    static unsigned long *lr; /* link register */
/*AIX*/    static unsigned long *oldsp;
/*AIX*/    static SAPDB_Int4 givenMaxLevel;
/*AIX*/
/*AIX*/    givenMaxLevel = maxLevel;
/*AIX*/    *pFilledLevel = 0;
/*AIX*/
/*AIX*/    if ( 0 == pBTraceContext )
/*AIX*/    {
/*AIX*/        static ucontext_t localContext;
/*AIX*/        if ( getcontext (&localContext) )
/*AIX*/        {
/*AIX*/            eo670WriteOutput("Failed to get ucontext\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/            return;
/*AIX*/        }
/*AIX*/        /* On PowerPC-AIX-Maschine the stack pointer is in GPR 1 */
/*AIX*/        sp = (unsigned long *)(localContext.uc_mcontext.jmp_context.gpr[1]);
/*AIX*/    }
/*AIX*/    else
/*AIX*/    {
/*AIX*/        skipCount = 0;
/*AIX*/        sp = (unsigned long *)(((struct sigcontext *)pBTraceContext)->sc_jmpbuf.jmp_context.gpr[1]);
/*AIX*/        switch ( addCallInfo )
/*AIX*/        {
/*AIX*/        case eo670_FullInfoArray:
/*AIX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr =
/*AIX*/                   (char *)0 + (((struct sigcontext *)pBTraceContext)->sc_jmpbuf.jmp_context.iar);
/*AIX*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = sp;
/*AIX*/            break;
/*AIX*/        case eo670_PointerArray:
/*AIX*/            callArray[*pFilledLevel] = (void *)(((struct sigcontext *)pBTraceContext)->sc_jmpbuf.jmp_context.iar);
/*AIX*/            break;
/*AIX*/        default: /* eo670_NoArrayJustOutput */
/*AIX*/            sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*AIX*/                         "(%d):0x%lx [0x%lx]\n",
/*AIX*/                         *pFilledLevel, (((struct sigcontext *)pBTraceContext)->sc_jmpbuf.jmp_context.iar), sp );
/*AIX*/            eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/            break;
/*AIX*/        }
/*AIX*/        ++*pFilledLevel;
/*AIX*/    }
/*AIX*/
/*AIX*/    /* Start off with our caller */
/*AIX*/    oldsp = sp-10;
/*AIX*/    /* Loop while we still have a valid stack pointer */
/*AIX*/    while( *pFilledLevel < givenMaxLevel
/*AIX*/       && ( (( (char*)sp - (char *)0 ) & (sizeof(unsigned long)-1)) == 0 )
/*AIX*/       && (sp > oldsp) )
/*AIX*/    {
/*AIX*/        oldsp = sp;
/*AIX*/
/*AIX*/        /* See if this is a signal.  This is done by looking at the LR field  */
/*AIX*/        /* on the stack.  The LR is at offset long 2 (byte 8 in 32 bit mode   */
/*AIX*/        /* or byte 16 in 64 bit mode).  If the address is from segment 0,     */
/*AIX*/        /* then this must be a signal.                                        */
/*AIX*/        if (*(sp+2) < 0x0fffffff) 	/* check LR field on stack */
/*AIX*/    	   {
/*AIX*/            break;
/*AIX*/    	   }
/*AIX*/        if ( skipCount > 0 )
/*AIX*/        {
/*AIX*/            --skipCount;
/*AIX*/        }
/*AIX*/        else
/*AIX*/        {
/*AIX*/            /* Handle potential local stack allocations within a function  */
/*AIX*/            /* these produces extra frames which are skipped here          */
/*AIX*/            if ( 0 != *(sp+2) )
/*AIX*/            {
/*AIX*/                switch ( addCallInfo )
/*AIX*/                {
/*AIX*/                case eo670_FullInfoArray:
/*AIX*/                    ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (void *)*(sp+2);
/*AIX*/                    ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = sp;
/*AIX*/                    break;
/*AIX*/                case eo670_PointerArray:
/*AIX*/                    callArray[*pFilledLevel] = (void *)*(sp+2);
/*AIX*/                    break;
/*AIX*/                default: /* eo670_NoArrayJustOutput */
/*AIX*/                    sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*AIX*/                                 "(%d):0x%lx [0x%lx]\n",
/*AIX*/                                 *pFilledLevel, *(sp+2), sp );
/*AIX*/                    eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/                    break;
/*AIX*/                }
/*AIX*/                ++*pFilledLevel;
/*AIX*/            }
/*AIX*/        }
/*AIX*/
/*AIX*/        /* If here, the stack frame is not the result of a signal.  The LR is */
/*AIX*/        /* accessed at long + 2, but the pointer to the next stack frame is   */
/*AIX*/        /* pointed to by the contents of where the current pointer points.    */
/*AIX*/        sp = (unsigned long *) *sp;
/*AIX*/    }
/*AIX*/}
/*AIX*/
/*AIX*/static SAPDB_Bool eo670_UpdateModuleChain( OutputCallBackFuncPtr pOutput,
/*AIX*/                                     void * const          pOutContext )
/*AIX*/{
/*AIX*/    static char moduleInfo[4096];
/*AIX*/    static struct ld_info *pInfo;
/*AIX*/    static int rc;
/*AIX*/
/*AIX*/    rc = loadquery(L_GETINFO, moduleInfo, sizeof(moduleInfo));
/*AIX*/    if ( rc < 0 )
/*AIX*/    {
/*AIX*/        sp77sprintf(eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*AIX*/                    "No module list since loadquery failed: %d\n", errno);
/*AIX*/        eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/        return;
/*AIX*/    }
/*AIX*/
/*AIX*/    for ( pInfo = (struct ld_info *)moduleInfo;
/*AIX*/          pInfo;
/*AIX*/          pInfo = (struct ld_info *)( ((char *)pInfo) + pInfo->ldinfo_next) )
/*AIX*/    {
/*AIX*/        if ( eo670_AddToLoadedModuleChain( pInfo->ldinfo_filename,
/*AIX*/                                           pInfo->ldinfo_textorg,
/*AIX*/                                           ((char *)pInfo->ldinfo_textorg) + pInfo->ldinfo_textsize,
/*AIX*/                                           0,
/*AIX*/                                           0,
/*AIX*/                                           pOutput, pOutContext) < 0 )
/*AIX*/        {
/*AIX*/            break; /* cannot add module anymore */
/*AIX*/        }
/*AIX*/
/*AIX*/        if ( !pInfo->ldinfo_next )
/*AIX*/        {
/*AIX*/            break;
/*AIX*/        }
/*AIX*/    }
/*AIX*/}
/*AIX*/
/*AIX*/static void eo670_FillAddressInfo( char *                pcAddr,
/*AIX*/                                   eo670AddressInfo *    pAddrInfo,
/*AIX*/                                   OutputCallBackFuncPtr pOutput,
/*AIX*/                                   void * const          pOutContext )
/*AIX*/{
/*AIX*/    static size_t foundNameLength;
/*AIX*/    static char * foundFuncName;
/*AIX*/    static eo670_ModuleChain *modulePointer;
/*AIX*/    static unsigned int offsetToSymbol;
/*AIX*/    SAPDB_memset(pAddrInfo, 0, sizeof(eo670AddressInfo));
/*AIX*/
/*AIX*/    pAddrInfo->sourceFileName = ""; /* No Source information on AIX... */
/*AIX*/    pAddrInfo->lineNumber = -1;
/*AIX*/
/*AIX*/    for ( modulePointer = eo670_ModuleAnchor
/*AIX*/        ; modulePointer
/*AIX*/        ; modulePointer = modulePointer->next )
/*AIX*/    {
/*AIX*/        if ( ( (pcAddr) >= ((char *)modulePointer->text_mapaddr) )
/*AIX*/          && ( (pcAddr) <  ((char *)modulePointer->text_endaddr) ) )
/*AIX*/        {
/*AIX*/            /* cast is ok, since modulePointer file_name points to */
/*AIX*/            /* dynamically allocated space (see eo670_UpdateModuleChain() */
/*AIX*/            pAddrInfo->moduleName = modulePointer->file_name;
/*AIX*/	           break;
/*AIX*/   	   }
/*AIX*/    }
/*AIX*/
/*AIX*/    if ( !modulePointer )
/*AIX*/    {
/*AIX*/        pAddrInfo->offsetToSymbol = -1;
/*AIX*/        pAddrInfo->symbol = "<not in any loaded region>";
/*AIX*/        pAddrInfo->demangledSymbol = "";
/*AIX*/        pAddrInfo->moduleName = "<not in any loaded module>";
/*AIX*/        return;
/*AIX*/    }
/*AIX*/
/*AIX*/    eo670GetFuncNameLength((unsigned int *)pcAddr, &foundFuncName, &foundNameLength, &offsetToSymbol);
/*AIX*/    if ( foundNameLength > 0 )
/*AIX*/    {
/*AIX*/        static char lastSymbolBuffer[MAX_SYMBOL_LENGTH]; /* next call overwrites it! */
/*AIX*/        if ( foundNameLength >= MAX_SYMBOL_LENGTH )
/*AIX*/        {
/*AIX*/            eo670WriteOutput("Symbol too long, truncated...", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/            SAPDB_memcpy (lastSymbolBuffer, foundFuncName, MAX_SYMBOL_LENGTH-1);
/*AIX*/            lastSymbolBuffer[MAX_SYMBOL_LENGTH-1] = 0;
/*AIX*/        }
/*AIX*/        else
/*AIX*/        {
/*AIX*/            SAPDB_memcpy (lastSymbolBuffer, foundFuncName, foundNameLength);
/*AIX*/            lastSymbolBuffer[foundNameLength] = 0;
/*AIX*/        }
/*AIX*/        pAddrInfo->symbol = lastSymbolBuffer;
#    if defined(WANT_DEMANGLING_SUPPORT)
#        if defined(KERNEL_LZU)
/*AIX*/      if ( kgs && XPARAM(allowDemangle) )
#        else
/*AIX*/      if ( pAddrInfo->symbol )
#        endif /* KERNEL_LZU */
/*AIX*/      {
/*AIX*/           pAddrInfo->demangledSymbol = eo670_demangle( pAddrInfo->symbol, eo670_AixDemangle);
/*AIX*/      }
/*AIX*/      else
#    endif /* WANT_DEMANGLING_SUPPORT */
/*AIX*/      {
/*AIX*/         pAddrInfo->demangledSymbol = "";
/*AIX*/      }
/*AIX*/    }
/*AIX*/    else
/*AIX*/    {
/*AIX*/        pAddrInfo->symbol = "<symbol not available>";
/*AIX*/        pAddrInfo->demangledSymbol = "";
/*AIX*/    }
/*AIX*/    pAddrInfo->offsetToSymbol = offsetToSymbol;
/*AIX*/
/*AIX*/    return;
/*AIX*/}
/*AIX*/
/*AIX*/static SAPDB_Bool eo670_UnixGetAddressInfo( void *                pcAddr,
/*AIX*/                                            eo670AddressInfo *    pAddrInfo,
/*AIX*/                                            OutputCallBackFuncPtr pOutput,
/*AIX*/                                            void * const          pOutContext )
/*AIX*/{
/*AIX*/    int    result;
/*AIX*/    static eo670AddressInfo localAddrInfo;
/*AIX*/
/*AIX*/    eo670_FillAddressInfo((char *)pcAddr, &localAddrInfo, pOutput, pOutContext);
/*AIX*/    pAddrInfo->symbol = (char *)calloc( 1, strlen(localAddrInfo.symbol)+1
/*AIX*/                                 + strlen(localAddrInfo.demangledSymbol)+1
/*AIX*/                                 + strlen(localAddrInfo.moduleName)+1
/*AIX*/                                 + strlen(localAddrInfo.sourceFileName)+1 );
/*AIX*/    if ( !pAddrInfo->symbol )
/*AIX*/    {
/*AIX*/        eo670WriteOutput("Out of memory for symbol copy\n", NO_DISPLACEMENT, pOutput, pOutContext);
/*AIX*/        result = false;
/*AIX*/    }
/*AIX*/    else
/*AIX*/    {
/*AIX*/        strcpy(pAddrInfo->symbol, localAddrInfo.symbol);
/*AIX*/        pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(localAddrInfo.symbol)+1;
/*AIX*/        strcpy(pAddrInfo->demangledSymbol, localAddrInfo.demangledSymbol);
/*AIX*/        pAddrInfo->moduleName = pAddrInfo->demangledSymbol + strlen(localAddrInfo.demangledSymbol)+1;
/*AIX*/        strcpy(pAddrInfo->moduleName, localAddrInfo.moduleName);
/*AIX*/        pAddrInfo->sourceFileName = pAddrInfo->moduleName + strlen(localAddrInfo.moduleName)+1;
/*AIX*/        strcpy(pAddrInfo->sourceFileName, localAddrInfo.sourceFileName);
/*AIX*/        pAddrInfo->offsetToSymbol = localAddrInfo.offsetToSymbol;
/*AIX*/        pAddrInfo->lineNumber = localAddrInfo.lineNumber;
/*AIX*/        result = true;
/*AIX*/    }
/*AIX*/    return result;
/*AIX*/}
/*AIX*/
/*AIX*/static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
/*AIX*/{
/*AIX*/    free(pAddrInfo->symbol); /* allocated in one chunk.... */
/*AIX*/}
#  endif /* AIX */

#endif /* no _WIN32 */

#ifdef HAS_LIBUNWIND_IMPLEMENTATION
/*LIBUNWIND*/static void eo670_RegisterDump( void*                 pBTraceContext,
/*LIBUNWIND*/                                OutputCallBackFuncPtr pOutput,
/*LIBUNWIND*/                                void * const          pOutContext )
/*LIBUNWIND*/{
/*LIBUNWIND*/    static unw_cursor_t cursor;
/*LIBUNWIND*/    static unw_context_t *pUc;
/*LIBUNWIND*/    static unw_context_t uc;
/*LIBUNWIND*/    static unw_word_t regValue;
/*LIBUNWIND*/    static unw_fpreg_t fpregValue;
/*LIBUNWIND*/    static unw_word_t ip;
/*LIBUNWIND*/    static int registerIndex;
/*LIBUNWIND*/
/*LIBUNWIND*/    if ( pBTraceContext )
/*LIBUNWIND*/    {
/*LIBUNWIND*/        pUc = (unw_context_t *)pBTraceContext;
/*LIBUNWIND*/    }
/*LIBUNWIND*/    else
/*LIBUNWIND*/    {
/*LIBUNWIND*/        pUc = &uc;
/*LIBUNWIND*/        unw_getcontext( pUc );
/*LIBUNWIND*/    }
/*LIBUNWIND*/
/*LIBUNWIND*/    if ( unw_init_local(&cursor, pUc) <0 )
/*LIBUNWIND*/    {
/*LIBUNWIND*/        return;
/*LIBUNWIND*/    }
/*LIBUNWIND*/
/*LIBUNWIND*/    for ( registerIndex = 0; registerIndex <= UNW_REG_LAST; ++registerIndex )
/*LIBUNWIND*/    {
/*LIBUNWIND*/         if ( unw_is_fpreg(registerIndex) )
/*LIBUNWIND*/         {
/*LIBUNWIND*/             unw_get_fpreg(&cursor, registerIndex, &fpregValue);
/*LIBUNWIND*/             if ( sizeof(unw_fpreg_t) == sizeof(long double) )
/*LIBUNWIND*/             {   /* we can use string conversion to display values */
/*LIBUNWIND*/                 sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LIBUNWIND*/                          "%10.10s 0x%.*r %g\n",
/*LIBUNWIND*/                          unw_regname(registerIndex),
/*LIBUNWIND*/                          sizeof(unw_fpreg_t),
/*LIBUNWIND*/                          &fpregValue,
/*LIBUNWIND*/                          (double)(*(long double *)&fpregValue) );
/*LIBUNWIND*/             }
/*LIBUNWIND*/             else
/*LIBUNWIND*/             {
/*LIBUNWIND*/                 sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LIBUNWIND*/                          "%10.10s 0x%.*r %g\n",
/*LIBUNWIND*/                          unw_regname(registerIndex),
/*LIBUNWIND*/                          sizeof(unw_fpreg_t),
/*LIBUNWIND*/                          &fpregValue );
/*LIBUNWIND*/             }
/*LIBUNWIND*/         }
/*LIBUNWIND*/         else
/*LIBUNWIND*/         {
/*LIBUNWIND*/             unw_get_reg(&cursor, registerIndex, &regValue);
/*LIBUNWIND*/             if ( sizeof(unw_word_t) == sizeof(long) )
/*LIBUNWIND*/             {   /* we can use string conversion to display values */
/*LIBUNWIND*/                 sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LIBUNWIND*/                              "%10.10s 0x%.*r 0x%0*lx %ld\n",
/*LIBUNWIND*/                              unw_regname(registerIndex),
/*LIBUNWIND*/                              sizeof(unw_word_t),
/*LIBUNWIND*/                              &regValue,
/*LIBUNWIND*/                              2 * sizeof(unw_word_t),
/*LIBUNWIND*/                              regValue,
/*LIBUNWIND*/                              regValue );
/*LIBUNWIND*/             }
/*LIBUNWIND*/             else
/*LIBUNWIND*/             {
/*LIBUNWIND*/                 sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LIBUNWIND*/                              "%10.10s 0x%.*r\n",
/*LIBUNWIND*/                              unw_regname(registerIndex),
/*LIBUNWIND*/                              sizeof(unw_word_t),
/*LIBUNWIND*/                              &regValue );
/*LIBUNWIND*/             }
/*LIBUNWIND*/         }
/*LIBUNWIND*/         eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LIBUNWIND*/    }
/*LIBUNWIND*/}
/*LIBUNWIND*/
/*LIBUNWIND*/ /*----------------------------------------------------------------------------------*/
/*LIBUNWIND*/
/*LIBUNWIND*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*LIBUNWIND*/                                    SAPDB_Int4            givenMaxLevel,
/*LIBUNWIND*/                                    void **               callArray,
/*LIBUNWIND*/                                    OutputCallBackFuncPtr pOutput,
/*LIBUNWIND*/                                    void * const          pOutContext )
/*LIBUNWIND*/{
/*LIBUNWIND*/    unw_cursor_t cursor;
/*LIBUNWIND*/    unw_context_t uc;
/*LIBUNWIND*/    unw_word_t ip;
/*LIBUNWIND*/    SAPDB_Int4 depth = 0;
/*LIBUNWIND*/
/*LIBUNWIND*/    unw_getcontext (&uc);
/*LIBUNWIND*/    if (unw_init_local (&cursor, &uc) < 0)
/*LIBUNWIND*/        return 0;
/*LIBUNWIND*/
/*LIBUNWIND*/    do
/*LIBUNWIND*/    {
/*LIBUNWIND*/        if ( skipCount > 0 )
/*LIBUNWIND*/        {
/*LIBUNWIND*/            --skipCount;
/*LIBUNWIND*/        }
/*LIBUNWIND*/        else
/*LIBUNWIND*/        {
/*LIBUNWIND*/            if (depth >= givenMaxLevel)
/*LIBUNWIND*/                return depth;
/*LIBUNWIND*/
/*LIBUNWIND*/            unw_get_reg (&cursor, UNW_REG_IP, &ip);
/*LIBUNWIND*/            callArray[depth++] = (void *) ip;
/*LIBUNWIND*/        }
/*LIBUNWIND*/    } while (unw_step (&cursor) > 0);
/*LIBUNWIND*/    return depth;
/*LIBUNWIND*/}
/*LIBUNWIND*/ /*----------------------------------------------------------------------------------*/
/*LIBUNWIND*/
/*LIBUNWIND*/void eo670_UnixFillCallArray( SAPDB_Int4            skipCount,
/*LIBUNWIND*/                              SAPDB_Int4            maxLevel,
/*LIBUNWIND*/                              SAPDB_Int4 *          pFilledLevel,
/*LIBUNWIND*/                              eo670_CallArrayType   addCallInfo,
/*LIBUNWIND*/                              void **               callArray,
/*LIBUNWIND*/                              void*                 pBTraceContext,
/*LIBUNWIND*/                              OutputCallBackFuncPtr pOutput,
/*LIBUNWIND*/                              void * const          pOutContext )
/*LIBUNWIND*/{
/*LIBUNWIND*/    static unw_cursor_t cursor;
/*LIBUNWIND*/    static unw_context_t uc;
/*LIBUNWIND*/    static unw_word_t ip;
/*LIBUNWIND*/    static unw_word_t sp;
/*LIBUNWIND*/    static SAPDB_Int4 depth = 0;
/*LIBUNWIND*/    static SAPDB_Int4 givenMaxLevel;
/*LIBUNWIND*/
/*LIBUNWIND*/    givenMaxLevel = maxLevel;
/*LIBUNWIND*/    *pFilledLevel = 0;
/*LIBUNWIND*/
/*LIBUNWIND*/    unw_getcontext( &uc );
/*LIBUNWIND*/
/*LIBUNWIND*/    if ( unw_init_local(&cursor, &uc) <0 )
/*LIBUNWIND*/        return 0;
/*LIBUNWIND*/
/*LIBUNWIND*/    while (unw_step (&cursor) > 0)
/*LIBUNWIND*/    {
/*LIBUNWIND*/        if ( skipCount > 0 )
/*LIBUNWIND*/        {
/*LIBUNWIND*/            --skipCount;
/*LIBUNWIND*/        }
/*LIBUNWIND*/        else
/*LIBUNWIND*/        {
/*LIBUNWIND*/            if ( *pFilledLevel >= givenMaxLevel )
/*LIBUNWIND*/            {
/*LIBUNWIND*/                break;
/*LIBUNWIND*/            }
/*LIBUNWIND*/
/*LIBUNWIND*/            unw_get_reg (&cursor, UNW_REG_IP, &ip);
/*LIBUNWIND*/            unw_get_reg (&cursor, UNW_REG_SP, &sp);
/*LIBUNWIND*/            switch ( addCallInfo )
/*LIBUNWIND*/            {
/*LIBUNWIND*/            case eo670_FullInfoArray:
/*LIBUNWIND*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].pcAddr = (void *)ip;
/*LIBUNWIND*/                ((eo670_CallInfo *)callArray)[*pFilledLevel].frame = (void *)sp;
/*LIBUNWIND*/                break;
/*LIBUNWIND*/            case eo670_PointerArray:
/*LIBUNWIND*/                callArray[*pFilledLevel] = (void *)ip;
/*LIBUNWIND*/                break;
/*LIBUNWIND*/            default: /* eo670_NoArrayJustOutput */
/*LIBUNWIND*/                sp77sprintf( eo670MessageBuffer, sizeof(eo670MessageBuffer),
/*LIBUNWIND*/                             "(%d):0x%lx [0x%lx]\n",
/*LIBUNWIND*/                             *pFilledLevel,
/*LIBUNWIND*/	                          (long)ip,
/*LIBUNWIND*/                             (long)sp );
/*LIBUNWIND*/                eo670WriteOutput(eo670MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
/*LIBUNWIND*/                break;
/*LIBUNWIND*/            }
/*LIBUNWIND*/            ++*pFilledLevel;
/*LIBUNWIND*/        }
/*LIBUNWIND*/    }
/*LIBUNWIND*/}
/*LIBUNWIND*/
#endif
/*===========================================================================*
 *  WINDOWS                                                                  *
 *===========================================================================*/

#if defined (_WIN32)

// miscellaneous toolhelp32 declarations; we cannot #include the header
// because not all systems may have it
#  pragma pack( push, 8 )
 typedef struct tagMODULEENTRY32
   {
   DWORD   dwSize;
   DWORD   th32ModuleID;       // This module
   DWORD   th32ProcessID;      // owning process
   DWORD   GlblcntUsage;       // Global usage count on the module
   DWORD   ProccntUsage;       // Module usage count in th32ProcessID's context
   BYTE  * modBaseAddr;        // Base address of module in th32ProcessID's context
   DWORD   modBaseSize;        // Size in bytes of module starting at modBaseAddr
   HMODULE hModule;            // The hModule of this module in th32ProcessID's context
   char    szModule[MAX_MODULE_NAME32 + 1];
   char    szExePath[MAX_PATH];
   } MODULEENTRY32;

 typedef MODULEENTRY32 *  PMODULEENTRY32;
 typedef MODULEENTRY32 *  LPMODULEENTRY32;

#  if defined (_WIN64)
typedef  DWORD64       _PCADDR;
#else
typedef  DWORD         _PCADDR;
#endif
#  pragma pack( pop )

/*---------------------------------------------------------------------------*/

// SymCleanup()
typedef BOOL (__stdcall *tSC)( IN HANDLE hProcess );

// SymGetOptions()
typedef DWORD (__stdcall *tSGO)( VOID );

 // SymInitialize()
 typedef BOOL (__stdcall *tSI)( IN HANDLE hProcess, IN PCSTR UserSearchPath, IN BOOL fInvadeProcess );

 // SymSetOptions()
 typedef DWORD (__stdcall *tSSO)( IN DWORD SymOptions );

 // UnDecorateSymbolName()
 typedef DWORD (__stdcall WINAPI *tUDSN)( PCSTR DecoratedName, PSTR UnDecoratedName,
	 DWORD UndecoratedLength, DWORD Flags );

#  if defined (_WIN64)
 // SymFunctionTableAccess()
 typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD64 AddrBase );

 // SymGetLineFromAddr()
 typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
	 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line );

 // SymGetModuleBase()
 typedef DWORD64 (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD64 dwAddr );

 // SymGetModuleInfo()
 typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PIMAGEHLP_MODULE64 ModuleInfo );

 // SymGetSymFromAddr()
 typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD64 dwAddr,
 	 OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol );

 // SymLoadModule()
 typedef DWORD64 (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
 	 IN PSTR ImageName, IN PSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll );

// ImagehlpApiVersion
 typedef LPAPI_VERSION (__stdcall *tIHV)(void);
 
 // StackWalk()
 typedef BOOL (__stdcall *tSW)( DWORD MachineType, HANDLE hProcess,
 	 HANDLE hThread, LPSTACKFRAME64 StackFrame, PVOID ContextRecord,
	 PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
	 PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
	 PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
	 PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress );

#  else /* WIN32 only */

 // SymFunctionTableAccess()
 typedef PVOID (__stdcall *tSFTA)( HANDLE hProcess, DWORD AddrBase );

 // SymGetLineFromAddr()
 typedef BOOL (__stdcall *tSGLFA)( IN HANDLE hProcess, IN DWORD dwAddr,
	 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE Line );

 // SymGetModuleBase()
 typedef DWORD (__stdcall *tSGMB)( IN HANDLE hProcess, IN DWORD dwAddr );

 // SymGetModuleInfo()
 typedef BOOL (__stdcall *tSGMI)( IN HANDLE hProcess, IN DWORD dwAddr, OUT PIMAGEHLP_MODULE ModuleInfo );

 // SymGetSymFromAddr()
 typedef BOOL (__stdcall *tSGSFA)( IN HANDLE hProcess, IN DWORD dwAddr,
 	 OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_SYMBOL Symbol );

 // SymLoadModule()
 typedef BOOL (__stdcall *tSLM)( IN HANDLE hProcess, IN HANDLE hFile,
 	 IN PSTR ImageName, IN PSTR ModuleName, IN DWORD BaseOfDll, IN DWORD SizeOfDll );

// ImagehlpApiVersion
 typedef LPAPI_VERSION (__stdcall *tIHV)(void);

 // StackWalk()
 typedef BOOL (__stdcall *tSW)( DWORD MachineType, HANDLE hProcess,
 	 HANDLE hThread, LPSTACKFRAME StackFrame, PVOID ContextRecord,
	 PREAD_PROCESS_MEMORY_ROUTINE ReadMemoryRoutine,
	 PFUNCTION_TABLE_ACCESS_ROUTINE FunctionTableAccessRoutine,
	 PGET_MODULE_BASE_ROUTINE GetModuleBaseRoutine,
	 PTRANSLATE_ADDRESS_ROUTINE TranslateAddress );
#  endif /* _WIN64 */

/*---------------------------------------------------------------------------*/

static tSLM      pSymLoadModule = NULL;
static tSFTA     pSymFunctionTableAccess = NULL;
static tSGMB     pSymGetModuleBase = NULL;
static tSW       pStackWalk = NULL;
static tIHV      pImagehlpApiVersion = NULL;
static tSC       pSymCleanup = NULL;
static tSGLFA    pSymGetLineFromAddr = NULL;
static tSGMI     pSymGetModuleInfo = NULL;
static tSGO      pSymGetOptions = NULL;
static tSGSFA    pSymGetSymFromAddr = NULL;
static tSI       pSymInitialize = NULL;
static tSSO      pSymSetOptions = NULL;
static tUDSN     pUnDecorateSymbolName = NULL;
static HANDLE    processHandle = 0;
static DWORD     symInitCount  = 0;
static CHAR      dbgHelpPath[MAX_PATH];

/*---------------------------------------------------------------------------*/

externC bool eo670_SetDbgHelpPath ( const char* path )
{
    static char              *p;

    strncpy( dbgHelpPath, path, sizeof( dbgHelpPath ) - sizeof("\\dbghelp.dll") );
    dbgHelpPath[sizeof( dbgHelpPath ) - sizeof("\\dbghelp.dll")] = '\0';

    for ( p = dbgHelpPath + strlen( dbgHelpPath ) - 1; p >= dbgHelpPath; -- p )
    {
      // locate the rightmost path separator
      if ( *p == '\\' || *p == '/' || *p == ':' )
        break;
    }
    // if we found one, p is pointing at it; if not, tt only contains
    // an dll name (no path), and p points before its first byte
    if ( p != dbgHelpPath ) // path sep found?
    {
      if ( *p == ':' ) // we leave colons in place
        ++ p;
      *p = '\0'; // eliminate the exe name and last path sep

      strcat( dbgHelpPath, "\\dbghelp.dll" );
    }

    return true;
}


/*---------------------------------------------------------------------------*/


/* PTS 1110627 */
static FARPROC eo670_WinGetProcAddress( HMODULE               module,
                                        LPCSTR                lpProcName,
                                        bool                  Silent,
                                        OutputCallBackFuncPtr pOutput,
                                        void * const          pOutContext )
{
    FARPROC funcPtr = GetProcAddress(module, lpProcName);

    if ( funcPtr == NULL )
    {
        static char MessageLine[1024];
        if ( !Silent )
        {
          sp77sprintf(MessageLine, sizeof(MessageLine), "GetProcAddress(%s) failed: rc = %lu\n", lpProcName, GetLastError() );
          eo670WriteOutput( MessageLine, NO_DISPLACEMENT, pOutput, pOutContext );
        }
    }
    return funcPtr;
}

/*---------------------------------------------------------------------------*/

static HANDLE eo670_WinLoadImageHlpDll( OutputCallBackFuncPtr pOutput,
                                        void * const          pOutContext )
{
  static HANDLE    hImagehlpDll = NULL;
  bool             loaded       = false;

  if ( hImagehlpDll != NULL )
    return hImagehlpDll;

  // On Windows 2000 Systems the standard 'dbghelp.dll' generates a
  // wrong call stack if the code has been compiled with the Microsoft .NET Compiler.
  // We are trying to load the 'dbghelp.dll' from the pgm-subdirectory.
  if ( '\0' != dbgHelpPath[0] ) loaded = ( 0 != LoadLibrary( dbgHelpPath ));
  if ( false == loaded )        loaded = ( 0 != LoadLibrary( "..\\pgm\\dbghelp.dll" ));
  if ( false == loaded )        loaded = ( 0 != LoadLibrary( "dbghelp.dll" ));

  hImagehlpDll = LoadLibrary( "imagehlp.dll" );

  if ( hImagehlpDll != NULL )
  {
    pImagehlpApiVersion     = (tIHV) eo670_WinGetProcAddress( hImagehlpDll,   "ImagehlpApiVersion", false, pOutput, pOutContext );
    pSymCleanup             = (tSC) eo670_WinGetProcAddress( hImagehlpDll,    "SymCleanup", false, pOutput, pOutContext );
    pSymGetOptions          = (tSGO) eo670_WinGetProcAddress( hImagehlpDll,   "SymGetOptions", false, pOutput, pOutContext );
    pSymInitialize          = (tSI) eo670_WinGetProcAddress( hImagehlpDll,    "SymInitialize", false, pOutput, pOutContext );
    pSymSetOptions          = (tSSO) eo670_WinGetProcAddress( hImagehlpDll,   "SymSetOptions", false, pOutput, pOutContext );
#  if defined (_M_IA64)
    pStackWalk              = (tSW) eo670_WinGetProcAddress( hImagehlpDll,    "StackWalk64", false, pOutput, pOutContext );
    pSymGetSymFromAddr      = (tSGSFA) eo670_WinGetProcAddress( hImagehlpDll, "SymGetSymFromAddr64", false, pOutput, pOutContext );
    pSymGetModuleInfo       = (tSGMI) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleInfo64", false, pOutput, pOutContext );
    pSymGetLineFromAddr     = (tSGLFA) eo670_WinGetProcAddress( hImagehlpDll,   "SymGetLineFromAddr64", false, pOutput, pOutContext );
    pSymFunctionTableAccess = (tSFTA) eo670_WinGetProcAddress( hImagehlpDll,  "SymFunctionTableAccess64", false, pOutput, pOutContext );
    pSymGetModuleBase       = (tSGMB) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleBase64", false, pOutput, pOutContext );
    pSymLoadModule          = (tSLM) eo670_WinGetProcAddress( hImagehlpDll,   "SymLoadModule64", false, pOutput, pOutContext );
#  elif defined (_M_AMD64)
    pStackWalk              = (tSW) eo670_WinGetProcAddress( hImagehlpDll,    "StackWalk64", false, pOutput, pOutContext );
    pSymGetSymFromAddr      = (tSGSFA) eo670_WinGetProcAddress( hImagehlpDll, "SymGetSymFromAddr64", false, pOutput, pOutContext );
    pSymGetModuleInfo       = (tSGMI) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleInfo64", false, pOutput, pOutContext );
    pSymGetLineFromAddr     = (tSGLFA) eo670_WinGetProcAddress( hImagehlpDll,   "SymGetLineFromAddr64", false, pOutput, pOutContext );
    pSymFunctionTableAccess = (tSFTA) eo670_WinGetProcAddress( hImagehlpDll,  "SymFunctionTableAccess64", false, pOutput, pOutContext );
    pSymGetModuleBase       = (tSGMB) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleBase64", false, pOutput, pOutContext );
    pSymLoadModule          = (tSLM) eo670_WinGetProcAddress( hImagehlpDll,   "SymLoadModule64", false, pOutput, pOutContext );
#  else
	// probably _M_IX86
    pStackWalk              = (tSW) eo670_WinGetProcAddress( hImagehlpDll,    "StackWalk", false, pOutput, pOutContext );
    pSymGetSymFromAddr      = (tSGSFA) eo670_WinGetProcAddress( hImagehlpDll, "SymGetSymFromAddr", false, pOutput, pOutContext );
    pSymGetModuleInfo       = (tSGMI) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleInfo", false, pOutput, pOutContext );
    pSymGetLineFromAddr     = (tSGLFA) eo670_WinGetProcAddress( hImagehlpDll,   "SymGetLineFromAddr", false, pOutput, pOutContext );
    pSymFunctionTableAccess = (tSFTA) eo670_WinGetProcAddress( hImagehlpDll,  "SymFunctionTableAccess", false, pOutput, pOutContext );
    pSymGetModuleBase       = (tSGMB) eo670_WinGetProcAddress( hImagehlpDll,  "SymGetModuleBase", false, pOutput, pOutContext );
    pSymLoadModule          = (tSLM) eo670_WinGetProcAddress( hImagehlpDll,   "SymLoadModule", false, pOutput, pOutContext );
#  endif
    pUnDecorateSymbolName = (tUDSN) eo670_WinGetProcAddress( hImagehlpDll, "UnDecorateSymbolName", false, pOutput, pOutContext );
  }

  if ( pSymCleanup == NULL
    || pSymFunctionTableAccess == NULL
    || pSymGetModuleBase == NULL
    || pSymGetModuleInfo == NULL
    || pSymGetOptions == NULL
    || pSymGetSymFromAddr == NULL
    || pSymInitialize == NULL
    || pSymLoadModule == NULL
    || pSymSetOptions == NULL
    || pStackWalk == NULL
    || pImagehlpApiVersion == NULL
    || pUnDecorateSymbolName == NULL )
  {
    FreeLibrary( hImagehlpDll );
    hImagehlpDll = NULL;
  }

  return hImagehlpDll;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool eo670_WinSymbolsLoaded() 
{
    return symInitCount != 0;
}

/*---------------------------------------------------------------------------*/

static HANDLE eo670_WinInitializeSymbols( OutputCallBackFuncPtr pOutput,
                                          void * const          pOutContext,
                                          char const **         ppSymSearchPath,
                                          bool                  forceUpdate )
{
    DWORD                        symOptions;     // symbol handler settings
    static char const *          symSearchPath;

    if (( 0 == symSearchPath ) || ( forceUpdate ))
        symSearchPath = eo670_WinSymSearchPath( pOutput, pOutContext );

    if ( 0 != ppSymSearchPath )
        *ppSymSearchPath = symSearchPath;

    if (( symInitCount != 0 ) && forceUpdate )
    {
        pSymCleanup( processHandle );
        if ( GetCurrentProcess() != processHandle )
        {
            CloseHandle(processHandle);
            processHandle = GetCurrentProcess();
        }
    }

    if (( symInitCount == 0 ) || ( forceUpdate ))
    {
        if( !DuplicateHandle( GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
            &processHandle, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ))
        {
            processHandle = GetCurrentProcess();
        }

        symOptions  = pSymGetOptions();
        symOptions |= SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS;
        symOptions &= ~SYMOPT_UNDNAME;
        pSymSetOptions( symOptions );

        if ( ! pSymInitialize( processHandle, symSearchPath, false ) )
        {
            eo670WriteOutput("SymInitialize() failed\n", NO_DISPLACEMENT, pOutput, pOutContext);
            CloseHandle(processHandle);
            processHandle = 0;
        }
        else if ( symInitCount == 0 )
            symInitCount++;
    }
    else
        symInitCount++;

    return processHandle;
}

/*---------------------------------------------------------------------------*/

static void  eo670_WinCleanupSymbols( OutputCallBackFuncPtr pOutput,
                                      void * const          pOutContext )
{
    if ( symInitCount != 0 ) 
    {
        if ( --symInitCount == 0 )
        {
            pSymCleanup( processHandle );
            if ( GetCurrentProcess() != processHandle )
            {
                CloseHandle(processHandle);
                processHandle = GetCurrentProcess();
            }
        }
    }

}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool eo670_WinUpdateSymbols( OutputCallBackFuncPtr pOutput,
                                          void * const          pOutContext )
{
  HANDLE     hProcess = 0;

  ENTER_CRIT_SEC( CS_TRACE_BACK );

  if ( !eo670_WinLoadImageHlpDll( pOutput, pOutContext) )
  {
    EXIT_CRIT_SEC( CS_TRACE_BACK );
    return false;
  }

  if ( true == eo670_WinSymbolsLoaded() )
  {
      hProcess = eo670_WinInitializeSymbols( pOutput, pOutContext, NULL, true );

      if ( !hProcess )
      {
          EXIT_CRIT_SEC( CS_TRACE_BACK );
          return false;
      }
      eo670_WinFillModuleList(GetCurrentProcessId(), hProcess, NONE_VERBOSE_CALL_STACK, pOutput, pOutContext);
  }

  EXIT_CRIT_SEC( CS_TRACE_BACK );
  return true;
}

/*---------------------------------------------------------------------------*/

#if !defined(_WIN64)
/*WIN32*/struct WinI386Frame
/*WIN32*/{
/*WIN32*/    struct WinI386Frame *fr_savfp;
/*WIN32*/    void *fr_savpc;
/*WIN32*/};
/*WIN32*/
/*WIN32*/SAPDB_Int4 eo670_FastFillCallArray( SAPDB_Int4            skipCount,
/*WIN32*/                                    SAPDB_Int4            givenMaxLevel,
/*WIN32*/                                    void **               callArray,
/*WIN32*/                                    OutputCallBackFuncPtr pOutput,
/*WIN32*/                                    void * const          pOutContext )
/*WIN32*/{
/*WIN32*/    struct WinI386Frame *fp;
/*WIN32*/    SAPDB_Int4 depth;
/*WIN32*/    SAPDB_Byte * stackLimit;
/*WIN32*/    SAPDB_Byte * oldFp;
/*WIN32*/
/*WIN32*/    depth = 0;
/*WIN32*/
/*WIN32*/	 __asm {
/*WIN32*/		mov fp, ebp
/*WIN32*/	 }
/*WIN32*/
/*WIN32*/    stackLimit = eo670_GetStackLimit(fp);
/*WIN32*/    oldFp = ((SAPDB_Byte *)fp)-1;
/*WIN32*/
/*WIN32*/    while ( depth < givenMaxLevel
/*WIN32*/         && ((SAPDB_Byte *)fp) >  oldFp
/*WIN32*/         && ((SAPDB_Byte *)fp) <= stackLimit
/*WIN32*/         && ((((long)fp)&3) == 0) )
/*WIN32*/    {
/*WIN32*/         oldFp = (SAPDB_Byte *)fp;
/*WIN32*/         if ( skipCount > 0 )
/*WIN32*/         {
/*WIN32*/             --skipCount;
/*WIN32*/         }
/*WIN32*/         else
/*WIN32*/         {
/*WIN32*/            callArray[depth] = fp->fr_savpc;
/*WIN32*/            ++depth;
/*WIN32*/         }
/*WIN32*/         fp = fp->fr_savfp;
/*WIN32*/    }
/*WIN32*/    return depth;
/*WIN32*/}
#endif



static DWORD WINAPI eo670_CallStackWorker ( eo670_CallStackWorkerStruct* workerParam )
 {
     CONTEXT  context;

     SuspendThread( workerParam->threadHandle );

     if ( 0 == workerParam->pThreadContext )
     {
         memset( &context, '\0', sizeof(context) );
         context.ContextFlags  = CONTEXT_FULL;
         workerParam->pThreadContext  = &context;

         if ( ! GetThreadContext( workerParam->threadHandle, &context ) )
         {
             eo670WriteString("Failed to get context\n");
             return GetLastError();
         }
     }

     eo670_WinShowStack( workerParam->threadHandle, workerParam->pThreadContext, VERBOSE_CALL_STACK,
                         workerParam->pOutput, workerParam->pOutContext );

     ResumeThread( workerParam->threadHandle );

     return NO_ERROR;
 }

/*---------------------------------------------------------------------------*/

static void eo670_WinFillCallArrayStackWalk( SAPDB_Int4              givenMaxLevel,
                                             void **                 callArray,
                                             LPEXCEPTION_POINTERS    except,
                                             OutputCallBackFuncPtr   pOutput,
                                             void * const            pOutContext,
                                             SAPDB_Int4 * const      pDepth )
{
#  if defined (_M_IA64)
   STACKFRAME64      stackFrame; // in/out stackframe
   DWORD             imageType = IMAGE_FILE_MACHINE_IA64;
#  elif defined (_M_AMD64)
   STACKFRAME64      stackFrame; // in/out stackframe
   DWORD             imageType = IMAGE_FILE_MACHINE_AMD64;
#  else
   // probably _M_IX86
   STACKFRAME        stackFrame; // in/out stackframe
   DWORD             imageType = IMAGE_FILE_MACHINE_I386;
#  endif
  static HANDLE     hProcess = 0;
  HANDLE            hThread;
  CONTEXT           copyOfContext;

  ENTER_CRIT_SEC( CS_TRACE_BACK );

  __try
  {
      if ( false == eo670_WinSymbolsLoaded() )
      {
          if ( !eo670_WinLoadImageHlpDll( pOutput, pOutContext) )
          {
              EXIT_CRIT_SEC( CS_TRACE_BACK );
              return;
          }

          hProcess = eo670_WinInitializeSymbols( pOutput, pOutContext, NULL, false );

          if ( !hProcess )
          {
              EXIT_CRIT_SEC( CS_TRACE_BACK );
              return;
          }

          eo670_WinFillModuleList(GetCurrentProcessId(), hProcess, NONE_VERBOSE_CALL_STACK, pOutput, pOutContext);
      }

      SAPDB_memcpy( &copyOfContext, except->ContextRecord, sizeof(copyOfContext)),

      hThread = GetCurrentThread();

      if( DuplicateHandle( GetCurrentProcess(), hThread, GetCurrentProcess(), &hThread,
                           DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) 
      {
          hThread  = GetCurrentThread();
      }

      SAPDB_memset( &stackFrame, 0, sizeof(stackFrame) );

      *pDepth = 0;

      // init STACKFRAME for first call
#     if defined (_M_IA64)
       stackFrame.AddrPC.Offset     = copyOfContext.StIIP;
       stackFrame.AddrPC.Mode       = AddrModeFlat;
       stackFrame.AddrStack.Offset  = copyOfContext.IntSp;
       stackFrame.AddrStack.Mode    = AddrModeFlat;
       stackFrame.AddrFrame.Offset  = 0;
       stackFrame.AddrFrame.Mode    = AddrModeFlat;
       stackFrame.AddrBStore.Offset = copyOfContext.RsBSP;
       stackFrame.AddrBStore.Mode   = AddrModeFlat;
#    elif defined (_M_AMD64)
      stackFrame.AddrPC.Offset     = copyOfContext.Rip;
      stackFrame.AddrPC.Mode       = AddrModeFlat;
      stackFrame.AddrStack.Offset  = copyOfContext.Rsp;
      stackFrame.AddrStack.Mode    = AddrModeFlat;
      stackFrame.AddrFrame.Offset  = copyOfContext.Rbp;
      stackFrame.AddrFrame.Mode    = AddrModeFlat;
#    else
	  // probably _M_IX86
      stackFrame.AddrPC.Offset     = copyOfContext.Eip;
      stackFrame.AddrPC.Mode       = AddrModeFlat;
      stackFrame.AddrStack.Offset  = copyOfContext.Esp;
      stackFrame.AddrStack.Mode    = AddrModeFlat;
      stackFrame.AddrFrame.Offset  = copyOfContext.Ebp;
      stackFrame.AddrFrame.Mode    = AddrModeFlat;
#    endif

     while ( *pDepth < givenMaxLevel )
     {
         if ( ! pStackWalk( imageType, hProcess, hThread,
                            &stackFrame, &copyOfContext, NULL,
                            pSymFunctionTableAccess, pSymGetModuleBase,  NULL ) )
         {
             break;
         }

         if ( stackFrame.AddrPC.Offset != 0 )
            callArray[(*pDepth)++] = (void *)stackFrame.AddrPC.Offset;

         // no return address means no deeper stackframe
         if (( stackFrame.AddrReturn.Offset == 0 ) ||
             ( stackFrame.AddrReturn.Offset == stackFrame.AddrPC.Offset ))
         {
             break;
         }
     }
  }
  __except( EXCEPTION_EXECUTE_HANDLER )
  {
    eo670WriteOutput( "---> StackWalk aborted! <---\n", NO_DISPLACEMENT, pOutput, pOutContext);
  }

  EXIT_CRIT_SEC( CS_TRACE_BACK );

  return;
}

/*---------------------------------------------------------------------------*/

static void eo670_WinFillCallArray( SAPDB_Int4              givenMaxLevel,
                                    void **                 callArray,
                                    OutputCallBackFuncPtr   pOutput,
                                    void * const            pOutContext,
                                    SAPDB_Int4 * const      pDepth )
{
  __try
  {
      RaiseException( EXCEPT_FILL_CALL_ARRAY, 0, 0, 0);
  }
  __except ( eo670_WinFillCallArrayStackWalk( givenMaxLevel, callArray, GetExceptionInformation(),
                                              pOutput, pOutContext, pDepth ),
             EXCEPTION_CONTINUE_EXECUTION  )
  {;}

  return;
}

/*---------------------------------------------------------------------------*/

static SAPDB_Bool eo670_WinGetAddressInfo ( void*                    pcAddr,
                                            eo670AddressInfo*        pAddrInfo,
                                            void*                    pAddrInfoContext,
                                            OutputCallBackFuncPtr    pOutput,
                                            void * const             pOutContext )
{
#  if defined (_M_IA64)
   static IMAGEHLP_LINE64      Line;
   static DWORD64              offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE64    Module;
   static DWORD                imageType = IMAGE_FILE_MACHINE_IA64;
#  elif defined (_M_AMD64)
   static IMAGEHLP_LINE64      Line;
   static DWORD64              offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE64    Module;
   static DWORD                imageType = IMAGE_FILE_MACHINE_AMD64;
#  else
   // probably _M_IX86
   static IMAGEHLP_LINE        Line;
   static DWORD                offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE      Module;
   static DWORD                imageType = IMAGE_FILE_MACHINE_I386;
#  endif
  static double                SymSpace[(sizeof(IMAGEHLP_SYMBOL) + MAXSYMNAMELEN)/sizeof(double)+1];
  static IMAGEHLP_SYMBOL*      pSym = (IMAGEHLP_SYMBOL *) SymSpace;
  static char                  undName[MAXSYMNAMELEN];     // undecorated name
  static char                  undFullName[MAXSYMNAMELEN]; // undecorated name with all shenanigans
  static char                  MessageLine[2048];
  eo670_GetAddressInfoContext* pLocalAddrInfoCtx;

  ENTER_CRIT_SEC( CS_TRACE_BACK );

  pLocalAddrInfoCtx = (eo670_GetAddressInfoContext*) pAddrInfoContext;

  SAPDB_memset( &Module, '\0', sizeof(Module) );
  Module.SizeOfStruct = sizeof(Module);

  if ( ! pSymGetModuleInfo( pLocalAddrInfoCtx->m_handle, (_PCADDR) pcAddr, &Module ) )
  {
      Module.ModuleName[0] = '?';
      Module.ModuleName[1] = '\0';
  }

  SAPDB_memset( pSym, '\0', sizeof(SymSpace));
  pSym->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
  pSym->MaxNameLength = MAXSYMNAMELEN;
  offsetFromSymbol    = 0;

  if ( ! pSymGetSymFromAddr( pLocalAddrInfoCtx->m_handle, (_PCADDR) pcAddr, &offsetFromSymbol, pSym ) )
  {
#  if defined (_WIN64)
           _snprintf  ( undName, (int)sizeof(undName), "%016I64x <nosymbols, rc = %d>",
                        pcAddr, GetLastError() );
#  else /* WIN32 only */
           sp77sprintf( undName, (int)sizeof(undName), "%08lx <nosymbols, rc = %d>",
                        pcAddr, GetLastError() );
#  endif /* WIN64 */

      pAddrInfo->symbol  = (char *)malloc(strlen(undName) + 1 + 
                                          strlen(undName) + 1 + 
                                          strlen(Module.ModuleName)+ 1 );
      strcpy(pAddrInfo->symbol, undName);
      pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(undName) + 1;
      strcpy(pAddrInfo->demangledSymbol, undName);
      pAddrInfo->moduleName      = pAddrInfo->demangledSymbol + strlen(undName) + 1;
  }
  else
  {
      // UnDecorateSymbolName()
      pUnDecorateSymbolName( pSym->Name, undName, MAXSYMNAMELEN, UNDNAME_NAME_ONLY );
      pUnDecorateSymbolName( pSym->Name, undFullName, MAXSYMNAMELEN, UNDNAME_COMPLETE );
      pAddrInfo->symbol          = (char *)malloc(strlen(undName)     + 1 + 
                                                  strlen(undFullName) + 1 +
                                                  strlen(Module.ModuleName) + 1 );
      strcpy(pAddrInfo->symbol, undName);
      pAddrInfo->demangledSymbol = pAddrInfo->symbol + strlen(undName) + 1;
      strcpy(pAddrInfo->demangledSymbol, undFullName);
      pAddrInfo->moduleName      = pAddrInfo->demangledSymbol + strlen(undFullName) + 1;
  }
    strcpy(pAddrInfo->moduleName, Module.ModuleName);
    pAddrInfo->offsetToSymbol  = offsetFromSymbol;
    pAddrInfo->sourceFileName  = "";
    pAddrInfo->lineNumber      = -1;

    EXIT_CRIT_SEC( CS_TRACE_BACK );

    return true;
}

/*---------------------------------------------------------------------------*/

static void eo670_OsFreeAddressInfo( eo670AddressInfo *pAddrInfo )
{
  ENTER_CRIT_SEC( CS_TRACE_BACK );
  free (pAddrInfo->symbol);
  EXIT_CRIT_SEC( CS_TRACE_BACK );
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool eo670_WinGetAddressInfoProlog ( void**                   pAddrInfoContext,
                                           OutputCallBackFuncPtr    pOutput,
                                           void * const             pOutContext )
{
  eo670_GetAddressInfoContext* pLocalAddrInfoCtx;

  ENTER_CRIT_SEC( CS_TRACE_BACK );

  pLocalAddrInfoCtx = (eo670_GetAddressInfoContext*) malloc (sizeof(eo670_GetAddressInfoContext));

  if (pLocalAddrInfoCtx)
  {
      if ( eo670_WinLoadImageHlpDll( pOutput, pOutContext) )
      {
          SAPDB_Bool symbolsPreLoaded = eo670_WinSymbolsLoaded();
          pLocalAddrInfoCtx->m_handle = eo670_WinInitializeSymbols( pOutput, pOutContext, NULL, false );

          if ( 0 != pLocalAddrInfoCtx->m_handle )
          {
              if ( false == symbolsPreLoaded)
                  eo670_WinFillModuleList (GetCurrentProcessId(), pLocalAddrInfoCtx->m_handle, 0, pOutput, pOutContext );

              *pAddrInfoContext = pLocalAddrInfoCtx;
              EXIT_CRIT_SEC( CS_TRACE_BACK );
              return true;
          }
      }
      /* some errors occured */
      free(pLocalAddrInfoCtx);
  }
  *pAddrInfoContext = (void*) 0;

  EXIT_CRIT_SEC( CS_TRACE_BACK );

  return false;
}

/*---------------------------------------------------------------------------*/

void eo670_WinGetAddressInfoEpilog ( void*                    pAddrInfoContext,
                                     OutputCallBackFuncPtr    pOutput,
                                     void * const             pOutContext )
{
  ENTER_CRIT_SEC( CS_TRACE_BACK );
  eo670_WinCleanupSymbols( pOutput, pOutContext);
  free(pAddrInfoContext);
  EXIT_CRIT_SEC( CS_TRACE_BACK );
}

/*---------------------------------------------------------------------------*/

static void eo670_WindowsTraceStack( eo670TraceContextPtr  pBTraceContext,
                                     OutputCallBackFuncPtr pOutput,
                                     void * const          pOutContext )
{
  HANDLE                         threadHandle;
  eo670_CallStackWorkerStruct    workerParam;
  HANDLE                         workerThread;

  if (( 0 == pBTraceContext ) || ( INVALID_HANDLE_VALUE == pBTraceContext->threadHandle ))
      threadHandle = GetCurrentThread(); // use pseudo handle
  else
      threadHandle = pBTraceContext->threadHandle;

  if( DuplicateHandle( GetCurrentProcess(), threadHandle, GetCurrentProcess(), &threadHandle,
                       DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) 
  {
      eo670WriteString("Failed to duplicate thread handle for call stack generation\n");

      if ( 0 == pBTraceContext )
      {    // try to use pseudo handle
          eo670_WinShowStack( GetCurrentThread(), (CONTEXT*)pBTraceContext, VERBOSE_CALL_STACK, pOutput, pOutContext );
      }
      return;
  }

  memset( &workerParam, '\0', sizeof(workerParam) );
  workerParam.threadHandle   = threadHandle;
  workerParam.pThreadContext = ( 0 == pBTraceContext ) ? 0 : pBTraceContext->pThreadContext;
  workerParam.pOutput        = pOutput;
  workerParam.pOutContext    = pOutContext;

  workerThread = CreateThread ( 0, 0, (LPTHREAD_START_ROUTINE)eo670_CallStackWorker,
                               (void*) &workerParam, 0, NULL );

  if ( INVALID_HANDLE_VALUE == workerThread )
  {
      eo670WriteString("Failed to start thread for call stack generation\n");

      if ( 0 == pBTraceContext )
      {
          int verbosity = VERBOSE_CALL_STACK;
          eo670_WinShowStack( threadHandle, (CONTEXT*)pBTraceContext, verbosity, pOutput, pOutContext );
      }
  }
  else
  {
      WaitForSingleObject( workerThread, INFINITE );
      CloseHandle( workerThread );
  }

  CloseHandle( threadHandle );  // release duplicated handle
}

/*---------------------------------------------------------------------------*/

static void eo670_WinShowStack( HANDLE                   hThread,
                                CONTEXT *                pBTraceContext,
                                int                      VerbosityLevel,
                                OutputCallBackFuncPtr    pOutput,
                                void * const             pOutContext )
{
  // normally, call ImageNtHeader() and use machine info from PE header
#  if defined (_M_IA64)
   static IMAGEHLP_LINE64    Line;
   static STACKFRAME64       sframe; // in/out stackframe
   static DWORD64            offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE64  Module;
   static DWORD              imageType = IMAGE_FILE_MACHINE_IA64;
#  elif defined (_M_AMD64)
   static IMAGEHLP_LINE64    Line;
   static STACKFRAME64       sframe; // in/out stackframe
   static DWORD64            offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE64  Module;
   static DWORD              imageType = IMAGE_FILE_MACHINE_AMD64;
#  else
   // probably _M_IX86
   static IMAGEHLP_LINE      Line;
   static STACKFRAME         sframe; // in/out stackframe
   static DWORD              offsetFromSymbol; // tells us how far from the symbol we were
   static IMAGEHLP_MODULE    Module;
   static DWORD              imageType = IMAGE_FILE_MACHINE_I386;
#  endif

  static CONTEXT             copyOfContext;
  static DWORD               offsetFromLine;
  static int                 frameNum; // counts walked frames
  static DWORD               symOptions; // symbol handler settings
  static double              SymSpace[(sizeof(IMAGEHLP_SYMBOL) + MAXSYMNAMELEN)/sizeof(double)+1];
  static IMAGEHLP_SYMBOL     *pSym = (IMAGEHLP_SYMBOL *) SymSpace;
  static char                undName[MAXSYMNAMELEN]; // undecorated name
  static char                undFullName[MAXSYMNAMELEN]; // undecorated name with all shenanigans
  static char                MessageLine[2048];
  static char                *p;
  static HANDLE              hProcess;
  static char const *        symSearchPath;
  static SAPDB_Bool          symbolsPreLoaded;
  static LPAPI_VERSION       pAPIVersion;

  ENTER_CRIT_SEC( CS_TRACE_BACK );

  symbolsPreLoaded = eo670_WinSymbolsLoaded();

  __try

  {
    if ( false == symbolsPreLoaded )
    {
        if ( !eo670_WinLoadImageHlpDll( pOutput, pOutContext) )
        {
            sp77sprintf(MessageLine, sizeof(MessageLine), "Error loading 'imagehlp.dll' or some required functions:%lu\n", GetLastError() );
            eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
            EXIT_CRIT_SEC( CS_TRACE_BACK );
            return;
        }
        pAPIVersion = pImagehlpApiVersion();

        if ( pAPIVersion )
        {
            sp77sprintf( MessageLine, sizeof(MessageLine), "Using 'imagehlp.dll' version: %lu.%lu.%lu \n", 
                         pAPIVersion->MajorVersion, pAPIVersion->MinorVersion, pAPIVersion->Revision );
            eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
        }
    }

    hProcess = eo670_WinInitializeSymbols( pOutput, pOutContext, &symSearchPath, false == symbolsPreLoaded);

    if ( !hProcess )
    {
        EXIT_CRIT_SEC( CS_TRACE_BACK );
        return;
    }

    SAPDB_memcpy( &copyOfContext, pBTraceContext, sizeof(copyOfContext));

    if ( VerbosityLevel == VERBOSE_CALL_STACK )
    {
        sp77sprintf(MessageLine, sizeof(MessageLine), "SymbolSearchPath:  %s\n", symSearchPath);
        eo670WriteOutput(MessageLine, "                   ", pOutput, pOutContext );
    }

    eo670_WinRegisterDump( &copyOfContext, pOutput, pOutContext );

    SAPDB_memset( &sframe, '\0', sizeof(sframe) );

    // init STACKFRAME for first call
#  if defined (_M_IA64)
     sframe.AddrPC.Offset     = copyOfContext.StIIP;
     sframe.AddrPC.Mode       = AddrModeFlat;
     sframe.AddrStack.Offset  = copyOfContext.IntSp;
     sframe.AddrStack.Mode    = AddrModeFlat;
     sframe.AddrFrame.Offset  = 0;
     sframe.AddrFrame.Mode    = AddrModeFlat;
     sframe.AddrBStore.Offset = copyOfContext.RsBSP;
     sframe.AddrBStore.Mode   = AddrModeFlat;
#  elif defined (_M_AMD64)
     sframe.AddrPC.Offset    = copyOfContext.Rip;
     sframe.AddrPC.Mode      = AddrModeFlat;
     sframe.AddrStack.Offset = copyOfContext.Rsp;
     sframe.AddrStack.Mode   = AddrModeFlat;
     sframe.AddrFrame.Offset = copyOfContext.Rbp;
     sframe.AddrFrame.Mode   = AddrModeFlat;
#  else
	 // probably _M_IX86
     sframe.AddrPC.Offset    = copyOfContext.Eip;
     sframe.AddrPC.Mode      = AddrModeFlat;
     sframe.AddrStack.Offset = copyOfContext.Esp;
     sframe.AddrStack.Mode   = AddrModeFlat;
     sframe.AddrFrame.Offset = copyOfContext.Ebp;
     sframe.AddrFrame.Mode   = AddrModeFlat;
#  endif

    SAPDB_memset( pSym, '\0', sizeof(SymSpace));
    pSym->SizeOfStruct  = sizeof(IMAGEHLP_SYMBOL);
    pSym->MaxNameLength = MAXSYMNAMELEN;

    SAPDB_memset( &Line, '\0', sizeof(Line) );
    Line.SizeOfStruct   = sizeof(Line);

    SAPDB_memset( &Module, '\0', sizeof(Module) );
    Module.SizeOfStruct = sizeof(Module);

    offsetFromSymbol = 0;
    offsetFromLine   = 0;

    eo670WriteOutput("----> Module List <----\n", NO_DISPLACEMENT, pOutput, pOutContext);

    eo670_WinFillModuleList(GetCurrentProcessId(), hProcess, VerbosityLevel, pOutput, pOutContext);

    eo670_ShowLoadedModules(pOutput, pOutContext);

    eo670WriteOutput( "----> Stack Back Trace <----\n", NO_DISPLACEMENT, pOutput, pOutContext);

    for ( frameNum = 0; frameNum < 100; ++frameNum )
    {
      // get next stack frame (StackWalk(), SymFunctionTableAccess(), SymGetModuleBase())
      // if this returns ERROR_INVALID_ADDRESS (487) or ERROR_NOACCESS (998), you can
      // assume that either you are done, or that the stack is so hosed that the next
      // deeper frame could not be found.
      if ( ! pStackWalk( imageType, hProcess, hThread, &sframe, &copyOfContext, NULL,
                         pSymFunctionTableAccess, pSymGetModuleBase, NULL ) )
      {
        break;
      }

      // display its contents
      if ( sframe.AddrPC.Offset == 0 )
      {
        strcat( MessageLine, "  <nosymbols> (PC == 0)\n" );
        eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
      }
      else
      { // we seem to have a valid PC

        // show module info (SymGetModuleInfo())
        if ( ! pSymGetModuleInfo( hProcess, sframe.AddrPC.Offset, &Module ) )
        { 
            Module.ModuleName[0] = '?';
            Module.ModuleName[1] = '\0';
        }

        // show procedure info (SymGetSymFromAddr())
        if ( ! pSymGetSymFromAddr( hProcess, sframe.AddrPC.Offset, &offsetFromSymbol, pSym ) )
        {
#  if defined (_WIN64)
           _snprintf  ( MessageLine, (int)sizeof(MessageLine), "  %s!%016I64x <nosymbols>[rc = %d]\n",
                        Module.ModuleName, sframe.AddrPC.Offset, GetLastError() );
#  else /* WIN32 only */
           sp77sprintf( MessageLine, (int)sizeof(MessageLine), "  %s!%08lx <nosymbols>[rc = %d]\n",
                        Module.ModuleName, sframe.AddrPC.Offset, GetLastError() );
#  endif /* WIN64 */

          eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
        }
        else
        {
          // UnDecorateSymbolName()
          pUnDecorateSymbolName( pSym->Name, undName, MAXSYMNAMELEN, UNDNAME_NAME_ONLY );
          pUnDecorateSymbolName( pSym->Name, undFullName, MAXSYMNAMELEN, UNDNAME_COMPLETE );

          if ( offsetFromSymbol != 0 )
          {
#  if defined (_WIN64)
             _snprintf  ( MessageLine, (int)sizeof(MessageLine), "  %s!%s + %I64d bytes\n",
                          Module.ModuleName, undName, offsetFromSymbol );
#  else /* WIN32 only */
             sp77sprintf( MessageLine, (int)sizeof(MessageLine), "  %s!%s + %ld bytes\n",
                          Module.ModuleName, undName, offsetFromSymbol );
#  endif /* WIN64 */
          }
          else
            sp77sprintf( MessageLine, (int)sizeof(MessageLine), "  %s!%s\n", Module.ModuleName, undName );

          eo670WriteOutput(MessageLine, "  ", pOutput, pOutContext );

          if ( strcmp(pSym->Name, undName) != 0 )
          {
            sp77sprintf(MessageLine, sizeof(MessageLine), "      Deco. :  %s\n", pSym->Name );
            eo670WriteOutput(MessageLine, "               ", pOutput, pOutContext );
          }
          if ( strcmp(undFullName, undName) != 0 )
          {
            sp77sprintf(MessageLine, sizeof(MessageLine), "      UDeco.:  %s\n", undFullName );
            eo670WriteOutput(MessageLine, "               ", pOutput, pOutContext );
          }
        }

        if ( VerbosityLevel == VERBOSE_CALL_STACK )
        {
#  if defined (_WIN64)
           _snprintf( MessageLine, sizeof(MessageLine), "      SFrame:  IP:%016I64x RA:%016I64x FP:%016I64x SP:%016I64x ",
                      sframe.AddrPC.Offset, sframe.AddrReturn.Offset,
                      sframe.AddrFrame.Offset, sframe.AddrStack.Offset );
           eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);

           _snprintf( MessageLine, sizeof(MessageLine), "      Params:  01:%016I64x 02:%016I64x 03:%016I64x 04:%016I64x ",
                      sframe.Params[0], sframe.Params[1], sframe.Params[2], sframe.Params[3] );
#  else /* WIN32 only */
           sp77sprintf( MessageLine, sizeof(MessageLine), "      SFrame:  IP:%08lx RA:%08lx FP:%08lx SP:%08lx ",
                        sframe.AddrPC.Offset, sframe.AddrReturn.Offset,
                        sframe.AddrFrame.Offset, sframe.AddrStack.Offset );
           eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);

           sp77sprintf( MessageLine, sizeof(MessageLine), "      Params:  01:%08lx 02:%08lx 03:%08lx 04:%08lx ",
                        sframe.Params[0], sframe.Params[1], sframe.Params[2], sframe.Params[3] );
#  endif /* WIN64 */
          eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);

          // show line number info
          if ( pSymGetLineFromAddr != NULL )
          {
            if ( ! pSymGetLineFromAddr( hProcess, sframe.AddrPC.Offset, &offsetFromLine, &Line ) )
            {
                DWORD rc = GetLastError();

                if (( rc != ERROR_INVALID_ADDRESS ) && ( rc != ERROR_MOD_NOT_FOUND ))
                {
                    sp77sprintf( MessageLine, sizeof(MessageLine), "  SymGetLineFromAddr() failed, rc = %d\n", rc);
                    eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
                }
            }
            else
            {
              p = max(strrchr(Line.FileName, '\\'), strrchr(Line.FileName, '/') );

              if ( p != NULL ) { *p = '\0'; p++; }
              else             p = Line.FileName;

              sp77sprintf(MessageLine, sizeof(MessageLine), "      Source:  %s ( Line: %lu )\n", p, Line.LineNumber );
              eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
            }
          }
        }
      } // we seem to have a valid PC

      // no return address means no deeper stackframe
      if (( sframe.AddrReturn.Offset == 0 ) ||
          ( sframe.AddrReturn.Offset == sframe.AddrPC.Offset ))
      {
        // avoid misunderstandings in the printf() following the loop
        SetLastError( 0 );
        break;
      }
    } // for ( frameNum )

    if ( GetLastError() != 0 )
    {
      sp77sprintf(MessageLine, sizeof(MessageLine), "StackWalk(): rc = %lu\n", GetLastError() );
      eo670WriteOutput(MessageLine, NO_DISPLACEMENT, pOutput, pOutContext);
    }
  }
  __except( EXCEPTION_EXECUTE_HANDLER )
  {
    eo670WriteOutput( "---> StackWalk aborted! <---\n", NO_DISPLACEMENT, pOutput, pOutContext);
  }

  if ( false == symbolsPreLoaded )
      eo670_WinCleanupSymbols( pOutput, pOutContext );

  EXIT_CRIT_SEC( CS_TRACE_BACK );


  return;
}

/*---------------------------------------------------------------------------*/

static bool eo670_WinFillModuleList( DWORD                  pid,
                                     HANDLE                 hProcess,
                                     int                    VerbosityLevel,
                                     OutputCallBackFuncPtr  pOutput,
                                     void * const           pOutContext )
{
  typedef HANDLE (__stdcall *tCT32S)( DWORD dwFlags, DWORD th32ProcessID );  // CreateToolhelp32Snapshot()
  typedef BOOL (__stdcall *tM32F)( HANDLE hSnapshot, LPMODULEENTRY32 lpme ); // Module32First()
  typedef BOOL (__stdcall *tM32N)( HANDLE hSnapshot, LPMODULEENTRY32 lpme ); // Module32Next()

  static HINSTANCE hToolhelp;
  static tCT32S    pCT32S;
  static tM32F     pM32F;
  static tM32N     pM32N;

  static HANDLE hSnap;
  static MODULEENTRY32 me = { sizeof(me) };
  static bool keepGoing;

  hToolhelp = LoadLibrary( "kernel32.dll" );

  if ( hToolhelp != 0 )
  {
    pCT32S = (tCT32S) eo670_WinGetProcAddress( hToolhelp, "CreateToolhelp32Snapshot", false, pOutput, pOutContext );
    pM32F  = (tM32F)  eo670_WinGetProcAddress( hToolhelp, "Module32First", false, pOutput, pOutContext );
    pM32N  = (tM32N)  eo670_WinGetProcAddress( hToolhelp, "Module32Next", false, pOutput, pOutContext );

    if ( pCT32S == 0 || pM32F == 0 || pM32N == 0 )
    {
      FreeLibrary( hToolhelp );
      return false;
    }

    hSnap = pCT32S( TH32CS_SNAPMODULE, pid );

    if ( hSnap == (HANDLE) -1 )
    {
      FreeLibrary( hToolhelp );
      return false;
    }
  }
  else
  {
    CloseHandle( hSnap );
    FreeLibrary( hToolhelp );
    return false;
  }

  keepGoing = !!pM32F( hSnap, &me );

  if ( !keepGoing )
  {
    CloseHandle( hSnap );
    FreeLibrary( hToolhelp );
    return false;
  }

  /* clear loaded module chain! */
  eo670_ModuleAnchor = 0;

  while ( keepGoing )
  {
    static char MessageBuffer[4096];
    // here, we have a filled-in MODULEENTRY32

    if ( eo670_AddToLoadedModuleChain( me.szExePath,
                                       me.modBaseAddr,
                                       ((char *)me.modBaseAddr)+me.modBaseSize,
                                       me.szExePath,
                                       SAPDB_strlen(me.szExePath)+1,
                                       pOutput, pOutContext ) < 0 )
    {
        break;
    }

    if ( !pSymLoadModule( hProcess, 0, me.szExePath, me.szModule, (tsp00_Longuint)me.modBaseAddr, me.modBaseSize ) )
    {
        DWORD rc = GetLastError();

        if ( rc != NO_ERROR )
        {
            sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "Error loading symbols for \"%s\", rc = %d\n",
                         me.szModule, rc );
            eo670WriteOutput(MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext);
        }
    }
    keepGoing = !!pM32N( hSnap, &me );
  }

  CloseHandle( hSnap );
  FreeLibrary( hToolhelp );

  return true;
}

/*---------------------------------------------------------------------------*/

static void eo670_WinRegisterDump( CONTEXT*               pBTraceContext,
                                   OutputCallBackFuncPtr  pOutput,
                                   void * const           pOutContext )
{
  static char                MessageLine[1024];

  eo670WriteOutput( "----> Register Dump <----\n" , NO_DISPLACEMENT, pOutput, pOutContext);

# if defined (_M_IA64)
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbi0 = %016I64x	     dbi1 = %016I64x\n", pBTraceContext->DbI0, pBTraceContext->DbI1 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbi2 = %016I64x	     dbi3 = %016I64x\n", pBTraceContext->DbI2, pBTraceContext->DbI3 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbi4 = %016I64x	     dbi5 = %016I64x\n", pBTraceContext->DbI4, pBTraceContext->DbI5 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbi6 = %016I64x	     dbi7 = %016I64x\n", pBTraceContext->DbI6, pBTraceContext->DbI7 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbd0 = %016I64x	     dbd1 = %016I64x\n", pBTraceContext->DbD0, pBTraceContext->DbD1 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbd2 = %016I64x	     dbd3 = %016I64x\n", pBTraceContext->DbD2, pBTraceContext->DbD3 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbd4 = %016I64x	     dbd5 = %016I64x\n", pBTraceContext->DbD4, pBTraceContext->DbD5 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    dbd6 = %016I64x	     dbd7 = %016I64x\n\n", pBTraceContext->DbD6, pBTraceContext->DbD7 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      gp = %016I64x	       r2 = %016I64x\n", pBTraceContext->IntGp, pBTraceContext->IntT0 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      r3 = %016I64x	       r4 = %016I64x\n", pBTraceContext->IntT1, pBTraceContext->IntS0 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      r5 = %016I64x	       r6 = %016I64x\n", pBTraceContext->IntS1, pBTraceContext->IntS2 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      r7 = %016I64x	       r8 = %016I64x\n", pBTraceContext->IntS3, pBTraceContext->IntV0 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      r9 = %016I64x	      r10 = %016I64x\n", pBTraceContext->IntT2, pBTraceContext->IntT3 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r11 = %016I64x	       sp = %016I64x\n", pBTraceContext->IntT4, pBTraceContext->IntSp );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r13 = %016I64x	      r14 = %016I64x\n", pBTraceContext->IntTeb, pBTraceContext->IntT5 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r15 = %016I64x	      r16 = %016I64x\n", pBTraceContext->IntT6, pBTraceContext->IntT7 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r17 = %016I64x	      r18 = %016I64x\n", pBTraceContext->IntT8, pBTraceContext->IntT9 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r19 = %016I64x	      r20 = %016I64x\n", pBTraceContext->IntT10, pBTraceContext->IntT11 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r21 = %016I64x	      r22 = %016I64x\n", pBTraceContext->IntT12, pBTraceContext->IntT13);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r23 = %016I64x	      r24 = %016I64x\n", pBTraceContext->IntT14, pBTraceContext->IntT15);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r25 = %016I64x	      r26 = %016I64x\n", pBTraceContext->IntT16, pBTraceContext->IntT17 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r27 = %016I64x	      r28 = %016I64x\n", pBTraceContext->IntT18, pBTraceContext->IntT19 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r29 = %016I64x	      r30 = %016I64x\n", pBTraceContext->IntT20, pBTraceContext->IntT21);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     r31 = %016I64x	  intnats = %016I64x\n\n", pBTraceContext->IntT22, pBTraceContext->IntNats);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

    _snprintf  ( MessageLine, sizeof(MessageLine), "   preds = %016I64x\n\n", pBTraceContext->Preds);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

    _snprintf  ( MessageLine, sizeof(MessageLine), "      b0 = %016I64x	       b1 = %016I64x\n", pBTraceContext->BrRp, pBTraceContext->BrS0 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      b2 = %016I64x	       b3 = %016I64x\n", pBTraceContext->BrS1, pBTraceContext->BrS2 );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      b4 = %016I64x	       b5 = %016I64x\n", pBTraceContext->BrS3, pBTraceContext->BrS4);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      b6 = %016I64x	       b7 = %016I64x\n\n", pBTraceContext->BrT0, pBTraceContext->BrT1);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

    _snprintf  ( MessageLine, sizeof(MessageLine), "    unat = %016I64x	       lc = %016I64x\n", pBTraceContext->ApUNAT, pBTraceContext->ApLC );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "      ec = %016I64x	      ccv = %016I64x\n", pBTraceContext->ApEC, pBTraceContext->ApCCV );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     dcr = %016I64x	      pfs = %016I64x\n", pBTraceContext->ApDCR, pBTraceContext->RsPFS);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     bsp = %016I64x	 bspstore = %016I64x\n", pBTraceContext->RsBSP, pBTraceContext->RsBSPSTORE);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     rsc = %016I64x	     rnat = %016I64x\n", pBTraceContext->RsRSC, pBTraceContext->RsRNAT );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "    ipsr = %016I64x	      iip = %016I64x\n", pBTraceContext->StIPSR, pBTraceContext->StIIP );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     ifs = %016I64x	      fcr = %016I64x\n", pBTraceContext->StIFS, pBTraceContext->StFCR);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "   eflag = %016I64x	      csd = %016I64x\n", pBTraceContext->Eflag, pBTraceContext->SegCSD);
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     ssd = %016I64x	    cflag = %016I64x\n", pBTraceContext->SegSSD, pBTraceContext->Cflag );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     fsr = %016I64x	      fir = %016I64x\n", pBTraceContext->StFSR, pBTraceContext->StFIR );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
    _snprintf  ( MessageLine, sizeof(MessageLine), "     fdr = %016I64x\n\n", pBTraceContext->StFDR );
    eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
# elif defined (_M_AMD64)
   sp77sprintf( MessageLine, sizeof(MessageLine), "  rax=0x%016x rbx=0x%016x rcx=0x%016x rdx=0x%016x\n",
                pBTraceContext->Rax, pBTraceContext->Rbx, pBTraceContext->Rcx, pBTraceContext->Rdx );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  r8 =0x%016x r9 =0x%016x r10=0x%016x r11=0x%016x\n",
                pBTraceContext->R8, pBTraceContext->R9, pBTraceContext->R10, pBTraceContext->R11);
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  r12=0x%016x r13=0x%016x r14=0x%016x r15=0x%016x\n",
                pBTraceContext->R12, pBTraceContext->R13, pBTraceContext->R14, pBTraceContext->R15);
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  rip=0x%016x rsp=0x%016x rbp=0x%016x\n",
                pBTraceContext->Rip, pBTraceContext->Rsp, pBTraceContext->Rbp  );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  rsi=0x%016x rdi=0x%016x\n",
                pBTraceContext->Rsi, pBTraceContext->Rdi );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

# else
   sp77sprintf( MessageLine, sizeof(MessageLine), "  Eax=0x%08x Ebx=0x%08x Ecx=0x%08x Edx=0x%08x Esi=0x%08x Edi=0x%08x\n",
                pBTraceContext->Eax, pBTraceContext->Ebx, pBTraceContext->Ecx, pBTraceContext->Edx, pBTraceContext->Esi, pBTraceContext->Edi );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  Eip=0x%08x Esp=0x%08x Ebp=0x%08x\n",
                pBTraceContext->Eip, pBTraceContext->Esp, pBTraceContext->Ebp  );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );

   sp77sprintf( MessageLine, sizeof(MessageLine), "  Cs=0x%04x  Ss=0x%04x  Ds=0x%04x  Es=0x%04x  Fs=0x%04x  Gs=0x%04x  Efl=0x%04x\n",
                pBTraceContext->SegCs, pBTraceContext->SegSs, pBTraceContext->SegDs, pBTraceContext->SegEs, pBTraceContext->SegFs, pBTraceContext->SegGs, pBTraceContext->EFlags );
   eo670WriteOutput ( MessageLine , NO_DISPLACEMENT, pOutput, pOutContext );
# endif
}

/*---------------------------------------------------------------------------*/

static char const * const eo670_WinSymSearchPath( OutputCallBackFuncPtr    pOutput,
                                                  void * const             pOutContext )
{
  static char              *p;
  static char              tt[TTBUFLEN];
  static char              symSearchPath[4096];

  symSearchPath[0] = '\0';

  // current directory
  if ( GetCurrentDirectory( TTBUFLEN, tt ) )
  {
    strcat(symSearchPath, tt);
    strcat(symSearchPath, ";");
  }
  // dir with executable
  if ( GetModuleFileName( 0, tt, TTBUFLEN ) )
  {
    for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )
    {
      // locate the rightmost path separator
      if ( *p == '\\' || *p == '/' || *p == ':' )
        break;
    }
    // if we found one, p is pointing at it; if not, tt only contains
    // an exe name (no path), and p points before its first byte
    if ( p != tt ) // path sep found?
    {
      if ( *p == ':' ) // we leave colons in place
        ++ p;
      *p = '\0'; // eliminate the exe name and last path sep
      strcat(symSearchPath, tt);
      strcat(symSearchPath, ";");

      for ( p = tt + strlen( tt ) - 1; p >= tt; -- p )
      {
        // locate the rightmost path separator
        if ( *p == '\\' || *p == '/' || *p == ':' )
          break;
      }
      if ( p != tt ) // path sep found?
      {
        if ( *p == ':' ) // we leave colons in place
          ++ p;
        *p = '\0'; // eliminate the last path sep
        strcat(symSearchPath, tt);
        strcat(symSearchPath, "\\symbols;");
      }
    }
  }
  // environment variable _NT_SYMBOL_PATH
  if ( GetEnvironmentVariable( "_NT_SYMBOL_PATH", tt, TTBUFLEN ) ) {
    strcat(symSearchPath, tt);
    strcat(symSearchPath, ";");
  }
  // environment variable _NT_ALTERNATE_SYMBOL_PATH
  if ( GetEnvironmentVariable( "_NT_ALTERNATE_SYMBOL_PATH", tt, TTBUFLEN ) ) {
    strcat(symSearchPath, tt);
    strcat(symSearchPath, ";");
  }
  // environment variable _NT_ALT_SYMBOL_PATH
  if ( GetEnvironmentVariable( "_NT_ALT_SYMBOL_PATH", tt, TTBUFLEN ) ) {
    strcat(symSearchPath, tt);
    strcat(symSearchPath, ";");
  }
  // environment variable SYSTEMROOT
  if ( GetEnvironmentVariable( "SYSTEMROOT", tt, TTBUFLEN ) ) {
    strcat(symSearchPath, tt);
    strcat(symSearchPath, ";");
  }

  if ( strlen(symSearchPath) > 0 ) // if we added anything, we have a trailing semicolon
    symSearchPath[strlen(symSearchPath)-1] = 0;

# if defined(KERNEL_LZU)
  {
    static tsp00_Pathc       szInstPath;
    static tsp01_RteError    rteError;
    if ( sqlGetDbrootSapPath ( szInstPath, TERM_WITH_DELIMITER_EO01, &rteError ))
    {
      strcat(symSearchPath, ";");
      strcat(symSearchPath, &szInstPath[0]);
    }
  }
#  endif /* KERNEL_LZU */

  return symSearchPath;
}

/*---------------------------------------------------------------------------*/

bool eo670_WriteMiniDump ( DWORD                      threadId,
                           LPEXCEPTION_POINTERS       pExcPtrs,
                           const char*                pszFile,
                           bool                       WriteFullMemory, 
                           bool                       IfDumpFileExists,
                           PSECURITY_ATTRIBUTES       pSA )
{
   return eo670_WriteMiniDumpOCB ( threadId, pExcPtrs, pszFile, WriteFullMemory, 
                                   IfDumpFileExists, pDefaultOutput, pDefaultOutContext, pSA );
}

/*---------------------------------------------------------------------------*/

//
// A user memory dump is written if a Microsoft 'dbghelp.dll' is found
// This dump can be  evaluated via 'windbg'.
//
// CAUTION: The dbghelp.dll version must be greater or equal 5.1!
//
bool eo670_WriteMiniDumpOCB ( DWORD                      threadId,
                              LPEXCEPTION_POINTERS       pExcPtrs,
                              const char*                pszFile,
                              bool                       WriteFullMemory,
                              bool                       IfDumpFileExists,
                              OutputCallBackFuncPtr      pOutput,    /* with ouput callback func.*/
                              void * const               pOutContext,
                              PSECURITY_ATTRIBUTES       pSA )
{
  eo670_MiniDumpWorkerStruct     workerParam;
  HANDLE                         workerThread;
  HANDLE                         threadHandle;
  bool                           ok;

  threadHandle = GetCurrentThread(); // use pseudo handle

  if( DuplicateHandle( GetCurrentProcess(), threadHandle, GetCurrentProcess(), &threadHandle,
                       DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ) == 0 ) 
  {
      eo670WriteString("Failed to duplicate thread handle for mini dump creation\n");
  }

  memset( &workerParam, '\0', sizeof(workerParam) );
  workerParam.threadId         = threadId;
  workerParam.threadHandle     = threadHandle;
  workerParam.pExcPtrs         = pExcPtrs;
  workerParam.pszFile          = pszFile;
  workerParam.WriteFullMemory  = WriteFullMemory;
  workerParam.IfDumpFileExists = IfDumpFileExists;
  workerParam.pOutput          = pOutput;
  workerParam.pOutContext      = pOutContext;
  workerParam.pSA              = pSA;

  workerThread = CreateThread ( 0, 0, (LPTHREAD_START_ROUTINE)eo670_MiniDumpWorker,
                               (void*) &workerParam, 0, NULL );

  if ( INVALID_HANDLE_VALUE == workerThread )
  {
      eo670WriteString("Failed to start thread for mini dump creation\n");

      ok = eo670_WriteMiniDumpToFile ( threadId, pExcPtrs, pszFile, WriteFullMemory, 
                                       IfDumpFileExists, pOutContext, pOutContext, pSA );
  }
  else
  {
      DWORD exitCode;

      WaitForSingleObject( workerThread, INFINITE );

      ok = GetExitCodeThread( workerThread, &exitCode );

      if ( ok )
          ok =  exitCode == NO_ERROR ? true : false;

      CloseHandle( workerThread );
  }

  CloseHandle( threadHandle );  // release duplicated handle
  return ok;
}

/*---------------------------------------------------------------------------*/

static DWORD WINAPI eo670_MiniDumpWorker ( eo670_MiniDumpWorkerStruct* workerParam )
 {
     bool ok;
     SuspendThread( workerParam->threadHandle );

     ok = eo670_WriteMiniDumpToFile ( workerParam->threadId,
                                      workerParam->pExcPtrs,
                                      workerParam->pszFile,
                                      workerParam->WriteFullMemory,
                                      workerParam->IfDumpFileExists,
                                      workerParam->pOutput,
                                      workerParam->pOutContext,
                                      workerParam->pSA );

     ResumeThread( workerParam->threadHandle );

     return ok ? NO_ERROR : (DWORD)-1;
 }

/*---------------------------------------------------------------------------*/

//
// A user memory dump is written if a Microsoft 'dbghelp.dll' is found
// This dump can be  evaluated via 'windbg'.
//
// CAUTION: The dbghelp.dll version must be greater or equal 5.1!
//
static bool eo670_WriteMiniDumpToFile ( DWORD                      threadId,
                                        LPEXCEPTION_POINTERS       pExcPtrs,
                                        const char*                pszFile,
                                        bool                       WriteFullMemory,
                                        bool                       IfDumpFileExists,
                                        OutputCallBackFuncPtr      pOutput,    /* with ouput callback func.*/
                                        void * const               pOutContext,
                                        PSECURITY_ATTRIBUTES       pSA )
{
  char MessageBuffer[4096];
  typedef BOOL (__stdcall *tMiniDumpWriteDump)( HANDLE                             hProcess,
                                                DWORD                              ProcessId,
                                                HANDLE                             hFile,
                                                MINIDUMP_TYPE                      DumpType,
                                                PMINIDUMP_EXCEPTION_INFORMATION    ExceptionParam,
                                                PMINIDUMP_USER_STREAM_INFORMATION  UserStreamParam,
                                                PMINIDUMP_CALLBACK_INFORMATION     CallbackParam );
  HANDLE                            hDbgHelp;
  HANDLE                            hFile;
  tMiniDumpWriteDump                pMiniDumpWriteDump;
  DWORD                             pid     = GetCurrentProcessId();
  bool                              Ok      = true;
  HANDLE                            hProcess;
  char                              tmpDumpFile[MAX_PATH];
  MINIDUMP_EXCEPTION_INFORMATION    ExceptionParam;

  ExceptionParam.ThreadId          = threadId;
  ExceptionParam.ExceptionPointers = pExcPtrs;
  ExceptionParam.ClientPointers    = FALSE;

  if( !DuplicateHandle( GetCurrentProcess(), GetCurrentProcess(), GetCurrentProcess(),
                        &hProcess, DUPLICATE_SAME_ACCESS, TRUE, DUPLICATE_SAME_ACCESS ))
  {
    hProcess = GetCurrentProcess();
  }

  hDbgHelp = LoadLibrary( "dbghelp.dll" );  // use dll version >=  5.1

  if ( hDbgHelp == 0 )
  {
    CloseHandle(hProcess);
    return false;
  }

  pMiniDumpWriteDump = (tMiniDumpWriteDump)eo670_WinGetProcAddress( hDbgHelp, "MiniDumpWriteDump", true, pOutput, pOutContext );

  if ( pMiniDumpWriteDump == 0 )
  {
    sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "DBGHELP is not suporting 'MiniDumpWriteDump' " );
    eo670WriteOutput ( MessageBuffer , NO_DISPLACEMENT, pOutput, pOutContext );

    CloseHandle(hProcess);
    FreeLibrary( hDbgHelp );
    return false;
  }

  ENTER_CRIT_SEC( CS_WRITE_DUMP );

  __try
  {
     // create temporary file using the path of the destination!
     if ( strrchr(pszFile, '\\' ))
     {
         char *p;

         tmpDumpFile[MAX_PATH-1] = '\0';
         strncpy( tmpDumpFile,  pszFile, MAX_PATH - 1 - sizeof(MINI_DMP_TEMP_FILE));

         p = strrchr(tmpDumpFile, '\\' );
         if ( p ) 
             *p = '\0';
         strcat( tmpDumpFile,  MINI_DMP_TEMP_FILE );
     }
     else
         strcpy( tmpDumpFile,  MINI_DMP_TEMP_FILE );

    if ( IfDumpFileExists )
      hFile = CreateFile( tmpDumpFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                          OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, pSA );
    else
      hFile = CreateFile( tmpDumpFile, GENERIC_WRITE, FILE_SHARE_READ, NULL,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, pSA );

    if ( hFile == INVALID_HANDLE_VALUE )
    {
      if (( IfDumpFileExists ) && ( GetLastError() == ERROR_FILE_NOT_FOUND ))
      {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "No mini dump file '%s' written ", pszFile );
        eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
      }
      else
      {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "Creating mini dump file \"%s\", rc = %d\n",
                     pszFile, GetLastError());
        eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
      }
      Ok = false;
    }
    else
    {
      MINIDUMP_TYPE  type = MiniDumpNormal | MiniDumpWithHandleData;

      if ( WriteFullMemory )
          type |= MiniDumpWithFullMemory | MiniDumpWithHandleData;

      if ( !pMiniDumpWriteDump( hProcess, pid, hFile, type, &ExceptionParam, NULL, NULL ))
      {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "Error writing mini dump, rc = %d\n", GetLastError());
        eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
        Ok = false;
      }
      else
      {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "Mini dump file '%s' written!", pszFile);
        eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
      }
    }
  }
  __except( EXCEPTION_EXECUTE_HANDLER )
  {
    sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "---> Write 'minidump' aborted! <---\n");
    eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
  }
  EXIT_CRIT_SEC( CS_WRITE_DUMP );

  CloseHandle(hProcess);
  CloseHandle( hFile );
  FreeLibrary( hDbgHelp );

  if ( Ok )
  {
    DeleteFile(pszFile); // remove old minidump

    if (!MoveFile( tmpDumpFile, pszFile ))
    {
        sp77sprintf( MessageBuffer, sizeof(MessageBuffer), "Creating mini dump file \"%s\", rc = %d\n",
                     pszFile, GetLastError());
        eo670WriteOutput ( MessageBuffer, NO_DISPLACEMENT, pOutput, pOutContext );
        Ok = false;
    }
    else
       DeleteFile(tmpDumpFile); // remove temporary minidump
  }

  return Ok;
}

#endif /* WIN32 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

static SAPDB_Byte *eo670_GetStackLimit( void *currentStackPointer )
{
    SAPDB_Byte* stackLimit;

#if defined(KERNEL_LZU)
    tsp00_TaskId taskId;
    TASK_CTRL pTaskCtrl;

    vgetpid(&taskId);
    if ( 0 != taskId )
    {
        pTaskCtrl = KGS->pFirstTaskCtrl + (taskId - 1);
#if defined(_WIN32)
        stackLimit = (SAPDB_Byte *)pTaskCtrl->Context.pStackLimit;
#elif defined(AIX)
        stackLimit = (SAPDB_Byte *)pTaskCtrl->StackChunkEnd; /* SIGALTSTACK makes testing stackLimit impossible */
#else
        if ( ((SAPDB_Byte *)currentStackPointer) > (SAPDB_Byte *)&stackLimit )
        {
            stackLimit = (SAPDB_Byte *)pTaskCtrl->StackChunkEnd;
        }
        else
        {
            /* stack grows to higher addresses (HP PA...) */
            stackLimit = (SAPDB_Byte *)pTaskCtrl->StackChunkStart;
        }
#endif
    }
    else
#endif /* KERNEL_LZU */
    {
#if defined(AIX)
        stackLimit = ((SAPDB_Byte *)-1L); /* SIGALTSTACK makes testing stackLimit impossible */
#else
        if ( ((SAPDB_Byte *)currentStackPointer) > (SAPDB_Byte *)&stackLimit )
        {
            stackLimit = ((SAPDB_Byte *)-1L);
        }
        else
        {
            /* stack grows to higher addresses (HP PA...) */
            stackLimit = 0;
        }
#endif
    }
    return stackLimit;
}

#endif /* HAS_NO_STACKBACKTRACE_IMPLEMENTATION */

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
