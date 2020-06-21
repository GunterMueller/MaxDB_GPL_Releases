/*!***************************************************************************

  module      : LVCSim.cpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : Global functions for simulator

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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

#   FreeBSD portions added by Kai Mosebach,
#   For more informations see : http://www.komadev.de/sapdb
#

*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_liveCacheSink.hpp"
#include "LVCSimulator/LVCSim_KernelInterface.hpp"
#include "LVCSimulator/LVCSim_OIDAllocator.hpp"
#include "LVCSimulator/LVCSim_DebugAllocator.hpp"
#include "LVCSimulator/LVCSim_DebugCallback.hpp"

#include "liveCache/LVC_Dispatcher.hpp"
#define	LC_DBPROC_PROTO
#include "Oms/OMS_DbpGlobals.hpp"
#include "heo07.h"
#include "heo670.h"
#include "hsp77.h"

#ifdef WIN32
#include <dbghelp.h>
#else
#include <sys/types.h>
#include <signal.h>
#endif

#if defined(WIN32) && defined(I386) && !defined(BIT64)
#include "LVCSimulator/MultiSessionSimulator/MSSim_Interface.hpp"
#endif

// Initialization

externC void* co10_GetInterface();

void *operator new(size_t sz) THROW_NEW_GEO573
{
    return LVCSim_KernelInterface::Instance()->Allocate(sz, false/*isUserRequest*/);
}

void operator delete(void *ptr) THROW_DELETE_GEO573
{
    LVCSim_KernelInterface::Instance()->Deallocate(ptr, false/*isUserRequest*/);
}

void *operator new[](size_t sz) THROW_NEW_GEO573
{
    return LVCSim_KernelInterface::Instance()->Allocate(sz, false/*isUserRequest*/);
}

void operator delete[](void *ptr) THROW_DELETE_GEO573
{
    LVCSim_KernelInterface::Instance()->Deallocate(ptr, false/*isUserRequest*/);
}


#if defined(OSF1)
#ifdef __MODEL_ANSI
extern "C" void *__7__nw__FUl(size_t Size) {
#else  //__MODEL_ARM
extern "C" void *__nw__XUl(size_t Size) {
#endif
  return LVCSim_KernelInterface::Instance()->Allocate(Size, false/*isUserRequest*/);
}
#endif


#ifndef WIN32
#undef min
#undef max
#include <iostream.h>
#endif


extern void omsResetForSimulator();


// WARNING!!! This is only temporary solution! It reads the indirect pointer
// to the sink (m_session). This pointer must be as the first in OmsHandle.

#define	GET_SINK(omshandle) (*((LVCSim_liveCacheSink***)	\
	(((char*) &omshandle) + sizeof(DbpBase))))
//#define	GET_CANCEL(omshandle) (*((bool**)	\
//	(((char*) &omshandle) + sizeof(DbpBase) + 3 * sizeof(void*))))



static LVCSim_KernelInterface	*kernelInterface;
static LVCSim_liveCacheSink		*defaultSink;
static double                   defaultSink_space[sizeof(LVCSim_liveCacheSink) / sizeof(double) + 1];
OMS_LibOmsInterface				*omsInterface = NULL;

static tsp00_Int4               tls_session_key = 0;

#ifdef OSF1
static bool stackTraceEnabled = false;
#else
static bool stackTraceEnabled = true;
#endif

static bool sessionSimulatorEnabled = false; 

LVCSim_CallbackInterface *LVCSim::m_callback = NULL;
int LVCSim::m_heapCheckLevel = 0;

void LVCSim_CallbackInterface::simBreak()
{
#if defined(WIN32)
  // Win32: in debug mode cause breakpoint
#ifdef _DEBUG
  _CrtDbgBreak();
#else   // _DEBUG
#ifndef _WIN64
  __asm int3;
#else
  fprintf(stderr, "Aborting the program on simBreak\n");
  abort();
#endif  // _WIN64
#endif  // !_DEBUG
#else   // WIN32
  // Unix: kill self with stop signal
  fprintf(stderr, "Breaking the program with SIGSTOP, attach debugger\n");
  kill(0, SIGSTOP);
#endif  // !WIN32
}

#if defined(WIN32) && defined(I386) && !defined(BIT64)
void LVCSim::SuspendSessionSimulator()
{
  MSSim_Suspend(); 
}

void LVCSim::ResumeSessionSimulator()
{
  MSSim_Resume(); 
}
#else

void LVCSim::SuspendSessionSimulator() { }
void LVCSim::ResumeSessionSimulator()  { }

#endif

/*!
 * Reset the internal state of the liveCache simulator. Be sure that
 * no transactions are running. This command resets the simulator
 * to the state after Init.
 */
void LVCSim::Reset()
{
#if defined(WIN32) && defined(I386) && !defined(BIT64)
  if( sessionSimulatorEnabled )
  {
    MSSim_Reset(); 
  }
#endif
	//fprintf(stderr, "WARNING: Reset not yet fully implemented!\n");
    LVCSim::DebugAllocatorEscape(LVCSIM_DA_ENTER_NOLEAK);
	  defaultSink->reset();
	  omsResetForSimulator();
	  omsInterface->InitLibOms(defaultSink);
    LVCSim::DebugAllocatorEscape(LVCSIM_DA_EXIT_NOLEAK);
}


/*!
 * Set up liveCache sink for a handle. Since COM objects are
 * derived from a handle, you can simply instantiate a COM object.
 * Attach will be then called automatically by simulator. If you
 * instantiate OmsHandle as such, you MUST call Attach to attach it
 * to a sink. This automatically attaches the handle to the current
 * session in multithreading program.
 *
 * \param obj reference to a handle to attach.
 *
 * \see AttachNewSink for attaching handle to a new session within
 *  the same thread (multi-user simulation).
 */
void LVCSim::Attach(OmsHandle &obj)
{
    LVCSim_liveCacheSink *session = reinterpret_cast<LVCSim_liveCacheSink*>(
        sqlgettls(tls_session_key));
    if (!session) {
        session = new LVCSim_liveCacheSink;
        teo07_ThreadErr terr;
        tsp00_ErrTextc ttext;
        sqlsettls(tls_session_key, session, ttext, &terr);
		if (terr != THR_OK_EO07) {
			throw DbpError(e_terminate, (char*)ttext);
		}
    }
    session->AddRef();
	if (!GET_SINK(obj)) {
		// this is a simple handle, we must give it a sink pointer
		bool *cancelFlag;
		GET_SINK(obj) = defaultSink->getSinkPtr(&cancelFlag);
		//GET_CANCEL(obj) = cancelFlag;
	}
	if (*GET_SINK(obj)) Detach(obj);
	*GET_SINK(obj) = session;
	co90AttachDefaultContext(STATIC_CAST(OmsHandle*, &obj));
}

/*!
 * This form of Attach attaches the handle in \c obj
 * to a new sink. This handle then represents pseudo-new session
 * in a single-threaded program.
 *
 * \param obj reference to a handle to attach.
 *
 * \see Attach for attaching handle to the current session.
 */
void LVCSim::AttachNewSink(OmsHandle &obj)
{
	if (!GET_SINK(obj)) {
		// this is a simple handle, we must give it a sink pointer
		bool *cancelFlag;
		GET_SINK(obj) = defaultSink->getSinkPtr(&cancelFlag);
		//GET_CANCEL(obj) = cancelFlag;
	}
	if (*GET_SINK(obj)) Detach(obj);
	*GET_SINK(obj) = new LVCSim_liveCacheSink;
    (*GET_SINK(obj))->AddRef();
	co90AttachDefaultContext(STATIC_CAST(OmsHandle*, &obj));
}

void *LVCSim::GetCurrentSink()
{
  IliveCacheSink *session = reinterpret_cast<IliveCacheSink*>(
    sqlgettls(tls_session_key));
  if (!session) {
    session = new LVCSim_liveCacheSink;
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlsettls(tls_session_key, session, ttext, &terr);
    if (terr != THR_OK_EO07) {
      delete session;
      return NULL;
    }
  }
  return session;
}

void LVCSim::SetCurrentSink(void *sink)
{
  teo07_ThreadErr terr;
  tsp00_ErrTextc ttext;
  sqlsettls(tls_session_key, sink, ttext, &terr);
  // TODO: what if error?
}

extern HRESULT co90ReleaseDefaultContext(void* oms_handle);

/*!
 * Detach a COM object from liveCache sink. When all objects
 * using particular sink are detached, the transaction belonging to this
 * sink is rolled back (if not explicitly confirmed by Commit before Detach).
 *
 * This sink can be then reused by the simulator for new sessions (in fact,
 * it wills be deallocated, but the semantics is the same).
 */
void LVCSim::Detach(OmsHandle &obj)
{
  if ((*GET_SINK(obj))->GetRefCount() == 1) {
    // rollback current transaction + deallocate session - last user
    LVCSim::Rollback(obj, true);
    co90ReleaseDefaultContext(&obj);

    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlsettls(tls_session_key, NULL, ttext, &terr);
    if (terr != THR_OK_EO07) {
      throw DbpError(e_terminate, (char*)ttext);
    }
  } else {
    co90ReleaseDefaultContext(&obj);
  }
  if ((*GET_SINK(obj))->Release() == 0) {
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlsettls(tls_session_key, NULL, ttext, &terr);
    if (terr != THR_OK_EO07) {
      throw DbpError(e_terminate, (char*)ttext);
    }
  }
  defaultSink->returnSinkPtr(GET_SINK(obj));
  GET_SINK(obj) = NULL;
}

/*!
 * Call this function after call of a COM routine to check allocator
 * emptyness.
 *
 * Since the COM routine calls in test programs are normal language-level
 * calls to object methods, the simulator cannot automatically detect
 * end of COM routine. Therefore, you must call this method to ensure
 * that COM routine specific allocator is checked for emptyness.
 */
void LVCSim::COMEnd(OmsHandle &obj)
{
  OMS_LibOmsInterface *omsInt = (OMS_LibOmsInterface*)co10_GetInterface();
  omsInt->UserAllocatorCtl(OmsCallbackInterface::BlockCOMRoutine, NULL);
}

/*!
 * Commit the current transaction. Used to simulate end of transaction
 * in test program (after calling COM object method).
 *
 * If commiting fails, rollback is performed instead.
 */
void LVCSim::Commit(OmsHandle &obj)
{
	short err = obj.omsCommitHandling();
	if (err != 0) {
		(*GET_SINK(obj))->SimRollback();
		throw DbpError(err);
	}
	(*GET_SINK(obj))->SimCommit();
}

/*!
 * Rollback the current transaction. Used to simulate end of transaction
 * in test program (after calling COM object method).
 */
void LVCSim::Rollback(OmsHandle &obj, bool endSession)
{
	obj.omsRollbackHandling(endSession, false);
  
  (*GET_SINK(obj))->SimRollback();
}



// TODO: move into static sink functions and reset properly on LC reset

#include <map>

typedef LVCSIM_STD map<int, LVCSim_StreamIOBase *>  StreamDirMap;
static RTESync_Spinlock                             streamLock;
static int                                          streamID = 1;
static StreamDirMap                                 *streamDir = NULL;
static double streamDir_space[sizeof(LVCSIM_STD map<int,void*>) / sizeof(double) + 1];

/*!
 * Register stream. Used internally by ABAP stream simulation.
 *
 * This method registers a stream into the internal stream table.
 * Use wrapper classes LVCSim_ABAPTabHandle and LVCSim_StreamHandle
 * for simulating ABAP streams.
 *
 * \param stream stream I/O object
 *
 * \return new stream ID
 */
int LVCSim::RegisterStream(class LVCSim_StreamIOBase *stream)
{
	RTESync_LockedScope	lck(streamLock);

	if (!streamDir) streamDir = new(&streamDir_space) LVCSIM_STD map<int, LVCSim_StreamIOBase *>();
	streamDir->insert(LVCSIM_STD pair<const int,LVCSim_StreamIOBase *>(streamID, stream));

	return streamID++;
}

/*!
 * Unregister stream. Used internally by ABAP stream simulation.
 *
 * This method unregisters a stream from the internal stream table.
 * Use wrapper classes LVCSim_ABAPTabHandle and LVCSim_StreamHandle
 * for simulating ABAP streams.
 *
 * \param id stream ID to unregister
 */
void LVCSim::UnregisterStream(int id)
{
	RTESync_LockedScope	lck(streamLock);

    if (streamDir) {
     	LVCSIM_STD map<int, LVCSim_StreamIOBase *>::iterator i = streamDir->find(id);
	    if (i == streamDir->end()) return;
        streamDir->erase(i);
        if (streamDir->size() == 0) {
            streamDir->~StreamDirMap();
            streamDir = NULL;
        }
    }
}

/*!
 * Return stream for stream ID.
 *
 * \param id stream ID for a previously registered stream.
 *
 * \return stream I/O object associated with the stream ID or
 *  NULL if ID not registered.
 */
class LVCSim_StreamIOBase *LVCSim::GetStream(int id)
{
	RTESync_LockedScope	lck(streamLock);

    if (!streamDir) return NULL;
	LVCSIM_STD map<int, LVCSim_StreamIOBase *>::iterator i = streamDir->find(id);
	if (i == streamDir->end()) return NULL;
	else return i->second;
}



void LVCSim::setName(const OMS_ObjectId8 &oid, const char *name)
{
	// set name of given OID
	// unused - will go away
}

const char *LVCSim::getName(const OMS_ObjectId8 &oid)
{
	// get name of given OID
	// unused - will go away
	return NULL;
}

/*!
 * Set how many times does omsSimDump when called write HTML dump file.
 *
 * Setting this will limit count of HTML dumps created with omsSimDump.
 *
 * \param cnt how many times will be a HTML dump written.
 */
void LVCSim::SetDumpCount(int cnt)
{
  LVCSim_liveCacheSink::SetDumpCount(cnt);
}

/*!
 * Dump the simulator state into HTML files. There will be an index page
 * dumped and for each container one overview page with links to dumped
 * objects. If you use multiple dumps, each dump version will be linked
 * with previous and next, so you can see how the objects change.
 *
 * \param obj OMS handle from which to take consistent view,
 * \param param parameter specifying additional functionality:
 *  - 0: recreate dump files,
 *  - greater than 0: keep dump files,
 *  - less than 0: recreate dump files, do not make links for different dump versions.
 *
 * Use OmsHandle::omsDump() to trigger the dump.
 */
void LVCSim::omsSimDump(OmsHandle &obj, int param)
{
	// dump the state into a HTML file
	(*GET_SINK(obj))->omsSimDump(obj, param);
}

/*!
 * Count and calculate the size of all var and non-var objects in all containers.
 *
 * \param h [in] OMS handle from which to take consistent view
 * \param objCount [out] number of all non-var objects
 * \param objSize [out] size of all non-var objects
 * \param varObjCount [out] number of all var objects
 * \param varObjSize [out] size of all var objects
 */
void LVCSim::countObjects(OmsHandle &h, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize)
{
  // Count all objects in all schemas
  countObjects( h, (OmsSchemaHandle)0, objCount, objSize, varObjCount, varObjSize);
}

/*!
 * Count and calculate the size of all var and non-var objects in all containers of a specified schema.
 *
 * \param h [in] OMS handle from which to take consistent view
 * \param schema [in] OMS schema handle in which the objects are counted
 *  - 0: count objects in all schemas
 *  - greater than 0: OMS schema handle
 * \param objCount [out] number of non-var objects
 * \param objSize [out] size of non-var objects
 * \param varObjCount [out] number of var objects
 * \param varObjSize [out] size of var objects
 */
void LVCSim::countObjects( OmsHandle &h, const OmsSchemaHandle schema, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize)
{
  // Count all objects in the specified schema
  (*GET_SINK(h))->countObjects(h, schema, objCount, objSize, varObjCount, varObjSize);
}

/*!
 * Flush trace information (actually a NOP). Included
 * for compatibility with old simulator.
 */
void LVCSim::FlushVtrace()
{
	// NOP - vtrace is flushed always automatically
}

/*!
 * Write checkpoint containing current simulator state.
 *
 * Write current simulator state into file. All transactions
 * must be commited or rolled back at the time of this call,
 * otherwise is the behaviour undefined!!!
 *
 * \param fname file name where to save the checkpoint.
 */
void LVCSim::SaveFullState(const char *fname)
{
	LVCSim_CheckpointFileWriter o(fname);
	LVCSim_liveCacheSink::writeToStream(o);
}

/*!
 * Load simulator state from a checkpoint.
 *
 * Read simulator state from a file. All transactions
 * must be commited or rolled back at the time of this call!!!
 *
 * \param fname file name where to read the checkpoint.
 */
void LVCSim::RestoreFullState(const char *fname)
{
	//fprintf(stderr, "WARNING: Restore checkpoint not yet fully implemented!\n");
	defaultSink->reset();
	omsResetForSimulator();

	LVCSim_CheckpointFileReader i(fname);
	LVCSim_liveCacheSink::readFromStream(i);

	omsInterface->InitLibOms(defaultSink);
}

/*!
 * Dump simulator memory statistics.
 *
 * Not implemented as of yet.
 */
void LVCSim::DumpMemoryStat()
{
	// TODO: dump simulator memory status
}

/*!
 * Set trace file for simulated vtrace.
 */
void LVCSim::SetTraceFile(const char *fname)
{
	if (!fname) {
		LVCSim_liveCacheSink::SetVTraceFile(stderr);
	} else {
		FILE *f = fopen(fname, "w");
		if (!f) {
			fprintf(stderr, "Error opening trace file '%s' for output, "
				"tracing to console\n", fname);
			LVCSim_liveCacheSink::SetVTraceFile(stderr);
		} else {
			LVCSim_liveCacheSink::SetVTraceFile(f);
		}
	}
}

/*!
 * Set/reset OMS interface trace level.
 *
 * \param level trace level name,
 * \param enable if \c true, enable specified trace, if \c false, disable the trace.
 *
 * There are the following trace levels possible:
 *  - OMS_ERROR - OMS errors,
 *  - OMS_INTERFACE - OMS interface functions,
 *  - OMS_CONTAINERDIR - OMS container directory functions,
 *  - OMS_MEMORY - memory trace,
 *  - OMS_VAROBJECT - variable-object related trace,
 *  - OMS_SESSION - session related trace,
 *  - OMS_LOCK - lock related trace.
 *
 * It makes most sense to enable only OMS_INTERFACE for an end-user.
 */
void LVCSim::SetTraceLevel(const char *level, bool enable)
{
	omsInterface->SetTraceLevel(level, enable);
}

/*!
 * Set trace levels numerically (actually a NOP).
 *
 * This is included only for compatibility with the old simulator.
 */
void LVCSim::SetTraceLevel(long level)
{
	// TODO: set OMS trace level numerically
}


/*!
 * Dump all memory leaks and check all blocks in freelist.
 *
 * This should be called only when no blocks are explicitly or implicitly
 * allocated. It will be called automatically on program end to check
 * for memory leaks.
 *
 * \return \c true, if any memory leaks found.
 */
bool LVCSim::CheckMemoryLeaks()
{
    return LVCSim_DebugAllocator::Escape(LVCSIM_DA_CHECK_LEAKS);
}

/*!
 * Check all memory allocations and deallocated memory.
 *
 * This method will check the debug allocator for consistency. All free
 * blocks will be checked if they contain proper bit pattern and all
 * allocated blocks will be checked, if their fences contain proper
 * bit patterns.
 *
 * This method is usable to pinpoint memory overwriters.
 *
 * \return \c true, if any error found.
 */
bool LVCSim::CheckMemory()
{
    return LVCSim_DebugAllocator::Escape(LVCSIM_DA_CHECK_MEMORY);
}

/*!
 * Debug allocator service function call.
 *
 * \param what service function ID.
 *
 * This method calls debug allocator service function. Possible
 * functions:
 *  - LVCSIM_DA_CLEAR_LEAKS - mark all memory blocks as no leaks,
 *  - LVCSIM_DA_ENTER_NOLEAK - mark all block allocated between this
 *  - LVCSIM_DA_EXIT_NOLEAK - and this as no leaks,
 *  - LVCSIM_DA_CHECK_LEAKS - check for leaks and dump leaks into trace/on screen (see also CheckMemoryLeaks),
 *  - LVCSIM_DA_CHECK_MEMORY - check memory consistency (see also CheckMemory),
 *  - LVCSIM_DA_MESSAGES_ON - turn on memory messages on-screen (e.g., leaks; can be nested),
 *  - LVCSIM_DA_MESSAGES_OFF - turn off memory messages on-screen,
 *  - LVCSIM_DA_ALLOCERR_ON - turn on allocator error messages,
 *  - LVCSIM_DA_ALLOCERR_OFF - turn off allocator error messages,
 *  - LVCSIM_DA_SYMREFRESH - refresh symbol table for stack traces.
 *
 * \return \c true, if error found for LVCSIM_DA_CHECK_LEAKS and LVCSIM_DA_CHECK_MEMORY. Undefined
 *  for other calls.
 */
bool LVCSim::DebugAllocatorEscape(LVCSim_DebugAllocatorEscapeEnum what)
{
    return LVCSim_DebugAllocator::Escape(what);
}




class LVCSim_ThreadParams {
public:
    LVCSim_ThreadFunc   func;           // thread main function
    void                *arg;           // function argument
    size_t              stacksize;      // stack size
    void                *top;           // stack highest address
    void                *bottom;        // stack lowest address
};

static int tls_stack_key = 0;

#if (defined(WIN32) && !defined(_WIN64)) || (defined(LINUX) && defined(I386)) || defined (FREEBSD)
void LVCSim::SetStackBase(void *ptr)
{
    int err;
    char errbuf[64];
    TLSCreate(tls_stack_key, &err, errbuf, sizeof(errbuf));
    if (err != LVCSim_ThreadOK) {
    	fprintf(stderr, "Error creating thread stack info: %s\n", errbuf);
		exit(1);
    }

    LVCSim_ThreadParams *sinfo = (LVCSim_ThreadParams*) malloc(sizeof(LVCSim_ThreadParams));
    sinfo->top = (void*) ((((int) ptr) + 4095) & ~4095);
    sinfo->bottom = ((char*) sinfo->top) - 1024*1024;
    sinfo->stacksize = 1024*1024;
    sinfo->arg = NULL;
    sinfo->func = NULL;
    TLSSet(tls_stack_key, sinfo, &err, errbuf, sizeof(errbuf));
    if (err != LVCSim_ThreadOK) {
    	fprintf(stderr, "Error setting thread stack info: %s\n", errbuf);
		exit(1);
    }
}
#endif

extern "C" void *LVCSim_ThreadMainFunction(void *arg)
{
    LVCSim_ThreadParams *p = (LVCSim_ThreadParams*) arg;
#if (defined(WIN32) && !defined(_WIN64)) || (defined(LINUX) && defined(I386)) || defined (FREEBSD)
    p->top = (void*) ((((int) &p) + 4095) & ~4095);
    p->bottom = ((char*) p->top) - p->stacksize;
#endif

    int err;
    char errbuf[64];
    LVCSim::TLSSet(tls_stack_key, p, &err, errbuf, sizeof(errbuf));
    if (err != LVCSim_ThreadOK) {
    	fprintf(stderr, "Error setting thread stack info: %s\n", errbuf);
    }

    long retcode = 0;
    try {
        retcode = (p->func)(p->arg);
    } catch (const DbpError &e) {
    	fprintf(stderr, "***** Unhandled DbpError %d exception in thread\n", e.dbpError());
        retcode = -1;
    } catch (...) {
    	fprintf(stderr, "***** Unhandled exception in thread\n");
        retcode = -2;
    }

    LVCSim::ThreadEnd((int) retcode);
    return NULL;
}


LVCSim_Thread *LVCSim::ThreadBegin(size_t stacksize, LVCSim_ThreadCreateFlags flags,
    LVCSim_ThreadFunc threadProc, void *arg, int *err, char *errbuf, size_t bufsize)
{
    teo07_Thread thread;
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    LVCSim_ThreadParams *p = reinterpret_cast<LVCSim_ThreadParams*>(malloc(sizeof(LVCSim_ThreadParams)));
    p->func = threadProc;
    p->arg = arg;
    p->stacksize = stacksize;
    sqlbeginthread(stacksize, LVCSim_ThreadMainFunction, p, flags, &thread, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
        free(p);
    }
    return reinterpret_cast<LVCSim_Thread*>(thread);
}

void LVCSim::ThreadEnd(int return_code)
{
    LVCSim_ThreadParams *p = reinterpret_cast<LVCSim_ThreadParams*>(TLSGet(tls_stack_key));
    if (p) {
        free(p);
    }

    LVCSim_liveCacheSink *session = reinterpret_cast<LVCSim_liveCacheSink*>(
        sqlgettls(tls_session_key));
    if (session) {
        session->AddRef();
        if (session->Release()) {
            // release all open handles
            int count = 1;
            while (session->Release()) {
                ++count;
            }
	        fprintf(stderr, "WARNING: %d handles still open at thread termination, sink %p\n",
                count, session);
        }
    }
    sqlendthread((tsp00_Int4) return_code);
}

void LVCSim::ThreadYield()
{
    sqlyieldthread();
}

LVCSim_Thread *LVCSim::ThreadGetCurrent()
{
    return reinterpret_cast<LVCSim_Thread*>(sqlgetthread());
}

void LVCSim::ThreadKill(LVCSim_Thread *thread, int *err, char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return;
    }
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlkillthread(reinterpret_cast<teo07_Thread>(thread), ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void LVCSim::ThreadCancel(LVCSim_Thread *thread, int return_code,
    int *err, char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return;
    }
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlcancelthread(reinterpret_cast<teo07_Thread>(thread), return_code, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void LVCSim::ThreadForget(LVCSim_Thread *thread)
{
    if (thread) {
        sqlforgetthread(reinterpret_cast<teo07_Thread>(thread));
    }
}

void LVCSim::ThreadSuspend(LVCSim_Thread *thread, int *err,
    char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return;
    }
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlsuspendthread(reinterpret_cast<teo07_Thread>(thread), ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void LVCSim::ThreadResume(LVCSim_Thread *thread, int *err,
    char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return;
    }
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlresumethread(reinterpret_cast<teo07_Thread>(thread), ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

int LVCSim::ThreadJoin(LVCSim_Thread *thread, int *err,
    char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return -1;
    }
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    tsp00_Int4 status = -1;
    sqljointhread(reinterpret_cast<teo07_Thread>(thread), &status, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
    return status;
}

bool LVCSim::ThreadIsSuspended(LVCSim_Thread *thread)
{
    if (thread) {
        return sqlIsThreadSuspended(reinterpret_cast<teo07_Thread>(thread)) != 0;
    } else {
        return false;
    }
}

bool LVCSim::ThreadIsAlive(LVCSim_Thread *thread, int *err,
    char *errbuf, size_t bufsize)
{
    if (!thread) {
        sp77sprintf(errbuf, (int) bufsize, "Invalid NULL thread handle");
        *err = LVCSim_ThreadNotOK;
        return false;
    }
#ifdef WIN32
    DWORD exit_code;
    GetExitCodeThread(*reinterpret_cast<HANDLE*>(thread), &exit_code );
    return exit_code == STILL_ACTIVE;
#else
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    int status = sqlthreadalive(reinterpret_cast<teo07_Thread>(thread), ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
    return status != FALSE;
#endif
}


LVCSim_Mutex *LVCSim::MutexCreate()
{
    teo07_Mutex *mutex = new teo07_Mutex;
    sqlcreatemutex(mutex);
    return reinterpret_cast<LVCSim_Mutex*>(mutex);
}

void LVCSim::MutexDestroy(LVCSim_Mutex *mutex)
{
    sqldestroymutex(reinterpret_cast<teo07_Mutex*>(mutex));
    delete reinterpret_cast<teo07_Mutex*>(mutex);
}

void LVCSim::MutexBegin(LVCSim_Mutex *mutex)
{
    sqlbeginmutex(reinterpret_cast<teo07_Mutex*>(mutex));
}

bool LVCSim::MutexTryBegin(LVCSim_Mutex *mutex)
{
    return sqltrybeginmutex(reinterpret_cast<teo07_Mutex*>(mutex)) != 0;
}

void LVCSim::MutexEnd(LVCSim_Mutex *mutex)
{
    sqlendmutex(reinterpret_cast<teo07_Mutex*>(mutex));
}


void LVCSim::TLSCreate(int &key, int *err, char *errbuf, size_t bufsize)
{
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    tsp00_Int4 tkey;
    sqlcreatetls(&tkey, ttext, &terr);
    key = tkey;
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void LVCSim::TLSDestroy(int key, int *err, char *errbuf, size_t bufsize)
{
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqldestroytls((tsp00_Int4) key, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void LVCSim::TLSSet(int key, void *val, int *err, char *errbuf, size_t bufsize)
{
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    sqlsettls((tsp00_Int4) key, val, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
}

void *LVCSim::TLSGet(int key)
{
    return sqlgettls(key);
}


LVCSim_Semaphore *LVCSim::SemCreate(int initval, int *err, char *errbuf, size_t bufsize)
{
    teo07_ThreadErr terr;
    tsp00_ErrTextc ttext;
    teo07_ThreadSemaphore tsem = NULL;
    sqlcreatesem(&tsem, (tsp00_Int4) initval, ttext, &terr);
    if (terr == THR_OK_EO07) {
        if (err) *err = LVCSim_ThreadOK;
        if (errbuf) *errbuf = 0;
    } else {
        if (err) *err = LVCSim_ThreadNotOK;
        if (errbuf) {
            strncpy(errbuf, ttext.asCharp(), bufsize);
            errbuf[bufsize - 1] = 0;
        }
    }
    return reinterpret_cast<LVCSim_Semaphore*>(tsem);
}

void LVCSim::SemDestroy(LVCSim_Semaphore *sem)
{
    sqldestroysem(reinterpret_cast<teo07_ThreadSemaphore>(sem));
}

void LVCSim::SemSignal(LVCSim_Semaphore *sem)
{
    sqlsignalsem(reinterpret_cast<teo07_ThreadSemaphore>(sem));
}

void LVCSim::SemWait(LVCSim_Semaphore *sem)
{
    sqlwaitsem(reinterpret_cast<teo07_ThreadSemaphore>(sem));
}

bool LVCSim::SemTimedWait(LVCSim_Semaphore *sem, int seconds)   // true on success
{
    teo07_ThreadErr terr;
    sqltimedwaitsem(reinterpret_cast<teo07_ThreadSemaphore>(sem), seconds, &terr);
    return terr == THR_OK_EO07;
}


/*!
 * Get stack trace to current program position (internal).
 *
 * \param trace array of pointers to return addresses,
 * \param traceSize size of the array (how many trace levels we want),
 * \param traceSkip how many trace levels to ignore, before writing to the array.
 *
 * \return number of stored trace levels in the array.
 */
int LVCSim::GetStackTrace(void** &trace, size_t traceSize, size_t traceSkip)
{
    if (!stackTraceEnabled) {
        while (traceSize--)
          *(trace++) = 0;
        return 0;
    }
#if (defined(WIN32) && !defined(_WIN64)) || (defined(LINUX) && defined(I386)) || defined (FREEBSD)
    LVCSim_ThreadParams *sinfo = (LVCSim_ThreadParams*) TLSGet(tls_stack_key);
    if (!sinfo) {
        // cannot create stack trace - no stack info
        void **trace2 = trace;
        while (traceSize--)
            *(trace2++) = 0;
        return 0;
    }
    register void *top = sinfo->top, *bottom = sinfo->bottom;
	register void **traceStart;
#if defined LINUX || defined (FREEBSD)
    asm("movl %%ebp, %0" : "=r" (traceStart));
#else
	__asm {
		mov traceStart, ebp
	}
#endif
	size_t level = 0;
	int skip = traceSkip;
    while (level < traceSize && traceStart >= bottom && traceStart < top) {
        if (--skip <= 0)
            trace[level++] = traceStart[1];
    	traceStart = (void**) *traceStart;
  	}
    size_t level2 = level;
    while (level2 < traceSize)
        trace[level2++] = NULL;
#else
    int level = eo670_FillCallArray(traceSize + traceSkip, trace);
    trace += traceSkip;
#endif
    return level;
}

void LVCSim::StartInUDEServer(OmsHandle &h, const char *libname, const char *methodname, ...)
{
  va_list vargs;
  va_start(vargs, methodname);
  (*GET_SINK(h))->StartInUDEServer(libname, methodname, vargs);
  va_end(vargs);
}

void LVCSim::CommitInProcServer(OmsHandle &h, const char *libname)
{
  (*GET_SINK(h))->CommitInProcServer(libname);
  h.omsCommit();
}

void LVCSim::RollbackInProcServer(OmsHandle &h, const char *libname)
{
  (*GET_SINK(h))->RollbackInProcServer(libname);
  h.omsRollback();
}

#if defined(LINUX) || defined(OSF1) || defined(SUN) || defined(SOLARIS)
extern "C" void e76_alarm0(void)
{
  sqlyieldthread();
}
#endif

class LVCSim_Initer {
public:
  LVCSim_Initer()
  {
#if (defined(WIN32) && !defined(_WIN64)) || (defined(LINUX) && defined(I386)) || defined (FREEBSD)
    int variableonstack;
    LVCSim::SetStackBase(&variableonstack);
#endif
    teo07_Thread currentThread;
    tsp00_ErrTextc errtext;
    teo07_ThreadErr err;

    LVCSim::DebugAllocatorEscape(LVCSIM_DA_ENTER_NOLEAK);
    sqlinit_multithreading(&currentThread, errtext, &err);
    if (err != THR_OK_EO07) {
      fprintf(stderr, "Error initializing threads: %s\n", ((char*)errtext));
      exit(1);
    }
    sqlcreatetls(&tls_session_key, errtext, &err);
    if (err != THR_OK_EO07) {
      fprintf(stderr, "Error getting session TLS key: %s\n", ((char*)errtext));
      exit(1);
    }

    kernelInterface = LVCSim_KernelInterface::Instance();
    omsInterface = (OMS_LibOmsInterface*) co10_GetInterface();
    LVC_LibOmsInterface::SetInterface(omsInterface);  // PTS 1122540
    AdviseSimulatorSink();

    defaultSink = new(&defaultSink_space) LVCSim_liveCacheSink(false);
    defaultSink->init();
    omsInterface->InitLibOms(defaultSink);
    LVCSim::DebugAllocatorEscape(LVCSIM_DA_EXIT_NOLEAK);
  }

  ~LVCSim_Initer()
  {
//#ifdef OSF1
    stackTraceEnabled = false;
//#endif
    LVCSim_KernelInterface *ifc = kernelInterface;
    kernelInterface = NULL;
    defaultSink->done();
    defaultSink->~LVCSim_liveCacheSink();
    omsInterface->UnAdviseKernelInterface();
    ifc->~LVCSim_KernelInterface();
    if (LVCSim::CheckMemoryLeaks() || LVCSim_DebugCallback::hasErrors()) {
      printf("Memory errors found. Detail info in mem_trace.txt.\n");
      //printf("Press Enter...\n");
      //getchar();
    }
  }
private:
  void AdviseSimulatorSink() const
  {
    omsInterface->AdviseKernelInterface(*kernelInterface, false /*isKernel*/, false /*isUDEServer*/);
  }
};

/*!
* Initialize internal structures. The simulator is in unknown state
* before Init is called. Result of calling any other simulator function
* before calling Init is undefined.
*/
void LVCSim::Init(bool useStackTrace, bool useSessionSimulator )
{
#if defined(WIN32) && defined(I386) && !defined(BIT64)
  sessionSimulatorEnabled = useSessionSimulator; 
  if( useSessionSimulator )
  {
    MSSim_Activate(); 
  }
#endif
  // Init global structures
  static LVCSim_Initer	initer_instance;
  stackTraceEnabled = useStackTrace;
#ifndef WIN32
  cout << "Initializing simulator V " << LVCSIM << endl;
  LVCSim::DebugAllocatorEscape(LVCSIM_DA_CLEAR_LEAKS);
#endif
}

void ak341UserAllocatorCtl (int msg, void  *param)
{
    OMS_LibOmsInterface *omsInt = (OMS_LibOmsInterface*)co10_GetInterface();
    omsInt->UserAllocatorCtl(msg, param);
}