/****************************************************************************

  module      : LVCSim_Client.hpp

  -------------------------------------------------------------------------

  responsible : ChristophR

  special area: liveCache Simulator
  description : Main include for liveCache simulator test programs. Do not
	include this file in real COM objects! Use it only for your test programs
	that use COM DLLs.

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

#ifndef	LVCSIM_HPP
#define	LVCSIM_HPP

// compatibility defines

/// Interface version, lC independent (major * 1000 + minor)
#define LVCSIM_INTERFACE_VERSION    1000
/// We have symbol refresh in debug allocator
#define LVCSIM_HAS_DA_SYMREFRESH


// Win32 problems:

#undef	LVCSIM_API

#ifdef _WIN32

#pragma warning( disable : 4275 )
#pragma warning( disable : 4273 )
#pragma warning( disable : 4786 )
#pragma warning( disable : 4995 )

#define	_AFXDLL

#ifdef	LVCSIM_DLL_VMAKE_BUILD
#define LVCSIM_API		__declspec(dllexport)
#define	LVCSIM_EXP_API	__declspec(dllexport)
#else
#define LVCSIM_API		__declspec(dllimport)
#define	LVCSIM_EXP_API	__declspec(dllexport)
#endif

#else
#define	LVCSIM_API
#define	LVCSIM_EXP_API
#endif

#if defined _WIN32 || defined LINUX || defined AIX || defined SUN || defined SOLARIS || defined FREEBSD || defined HP_IA64 
#define	LVCSIM_STD	std::
#else
#define	LVCSIM_STD
#endif

#ifdef	HPUX
#define __HPACC_USING_MULTIPLIES_IN_FUNCTIONAL
#endif

#define SAPDB_LVCSIM_EXTRAS		// include extra functionality from livecache.h

#include <livecacheversion.h>
#include <lcbasetypes.h>
#include <livecache.h>
#include <livecachesql.h>

#ifndef	LVCSIM_GLOBFNC_HPP
#define	LVCSIM_GLOBFNC_HPP

#ifndef	_WIN32
#include <stddef.h>
#endif
#include <stdlib.h>

class	OmsHandle;

#ifndef LVCSIM_DEBUG_ALLOC_ESCAPE_ENUM
#define LVCSIM_DEBUG_ALLOC_ESCAPE_ENUM
/// Debug allocator escape constants.
enum LVCSim_DebugAllocatorEscapeEnum {
    LVCSIM_DA_ON,           ///< turn on memory debugger
    LVCSIM_DA_OFF,          ///< turn off memory debugger
    LVCSIM_DA_CLEAR_LEAKS,  ///< mark all memory blocks as no leaks
    LVCSIM_DA_ENTER_NOLEAK, ///< mark all block allocated between this
    LVCSIM_DA_EXIT_NOLEAK,  ///< and this as no leaks
    LVCSIM_DA_CHECK_LEAKS,  ///< check for leaks and dump leaks into trace/on screen
    LVCSIM_DA_CHECK_MEMORY, ///< check memory consistency
    LVCSIM_DA_MESSAGES_ON,  ///< turn on memory messages on-screen (e.g., leaks; can be nested)
    LVCSIM_DA_MESSAGES_OFF, ///< turn off memory messages on-screen
    LVCSIM_DA_ALLOCERR_ON,	///< turn on allocator error messages
    LVCSIM_DA_ALLOCERR_OFF,	///< turn off allocator error messages
    LVCSIM_DA_SYMREFRESH,   ///< refresh symbol table for stack traces
    LVCSIM_DA_STACKTRACE    ///< dump stack trace to the dump file
};
#endif

#ifndef LVCSIM_HEAP_CHECK_LEVEL_ENUM
#define LVCSIM_HEAP_CHECK_LEVEL_ENUM
enum LVCSim_HEAP_CHECK_LEVEL {
    LVCSIM_HEAP_FENCES = 1,	        // places a pattern after euch used chunk and checks this pattern, when the chunk is freed
    LVCSIM_HEAP_FREEPATTERN = 2,	  // fills each freed chunk with a pattern
    LVCSIM_HEAP_DOUBLEBOOK = 4,	    // double bookkeeping of used chunks
    LVCSIM_HEAP_BINTREE = 8,	      // check of binary tree of big free chunks
    LVCSIM_HEAP_EXCLUSIVE = 16, 	  //checks exclusive usage of unsnchronized heaps
    LVCSIM_HEAP_DELAYEDFREE = 32,   // delayed free of freed chunks
    LVCSIM_HEAP_DISABLESTACK = 64	  // use default context allocator for object frames instead of stack allocator
};
#endif

class LVCSim_Thread;                            ///< Class representing a thread in simulator
class LVCSim_Mutex;                             ///< Class representing a mutex in simulator
class LVCSim_Semaphore;                         ///< Class representing a semaphore in simulator
typedef unsigned long (*LVCSim_ThreadFunc) (void *arg);  ///< Thread main function signature.

/// Thread creation constants.
typedef enum {
    LVCSim_ThreadCreateNormal = 0,      ///< Create normal, running thread.
    LVCSim_ThreadCreateSuspended = 1,   ///< Create suspended thread (must be resumed by user).
    LVCSim_ThreadCreateDetached = 2     ///< Create detached (daemon) thread.
} LVCSim_ThreadCreateFlags;

/// Thread function result.
typedef enum {
    LVCSim_ThreadOK = 0,        ///< Success.
    LVCSim_ThreadNotOK = 1      ///< Failure.
} LVCSim_ThreadErrorCode;

/*!
 * \brief Callback interface for simulator.
 *
 * \since 7.4.4.1
 */
class LVCSIM_API LVCSim_CallbackInterface {
public:
  virtual ~LVCSim_CallbackInterface() {}

  /*!
   * \brief Callback called on not stored object.
   *
   * This callback will be called, when object with oid \c oid is committed,
   * it was modified, but it was not stored with omsStore.
   *
   * \param oid object ID,
   * \param className object's class name.
   */
  virtual void simNotStored(const OMS_ObjectId8 &oid, const char *className)
  {
    simBreak();
  }

  /// Break or abort the execution
  static void simBreak();
};

/*!
 * \brief liveCache simulator global functions.
 *
 * This class provides access to the most of liveCache simulator functionality.
 *
 * Generally, to use an OMS functionality, you can do the following:
 *
 * \code
 *  OmsHandle h;            // instantiate a handle
 *  LVCSim::Attach(h);      // attach the handle to the simulator
 *  ...
 *  h.omsMethod(...);       // call some OMS function(s)
 *  ...
 *  h.omsCommit();          // commit changes
 * \endcode
 */
class LVCSIM_API LVCSim {
public:
    /*!
     * \name Basic functionality
     */
    //@{

    /// Attach the handle to the current sink in the simulator.
    static void Attach(OmsHandle &obj);

    /// Attach the handle to a new sink in the simulator.
    static void AttachNewSink(OmsHandle &obj);

protected:
    friend class LVCSim_ProcServerInterface;

    /// Detach the handle from the sink in the simulator (internal).
    static void Detach(OmsHandle &obj);

    /// Return pointer to the current sink (internal).
    static void *GetCurrentSink();

    /// Set sink pointer (internal, for ProcServer).
    static void SetCurrentSink(void *sink);

#if (defined(WIN32) && !defined(_WIN64)) || defined(LINUX) || defined (FREEBSD)
    /// Set stack base for stack traces (internal).
    static void SetStackBase(void *ptr);
#endif

public:
    /// Initialize internal structures.
    static void Init(bool useStackTrace = true, bool useSessionSimulator = false );

    // suspend MultiSessionSimulator
    static void SuspendSessionSimulator(); 

    // resume MultiSessionSimulator
    static void ResumeSessionSimulator(); 

    /// Reset the internal state of the liveCache simulator.
    static void Reset();

    /// Commit the current transaction.
    static void Commit(OmsHandle &obj);

    /// Rollback the current transaction.
    static void Rollback(OmsHandle &obj, bool endSession = false);

    /// Signalize end of COM routine.
    static void COMEnd(OmsHandle &obj);

    //@}

    /*!
     * \name ABAP stream simulation support
     */
    //@{

    /// Register stream. Used internally by ABAP stream simulation.
    static int RegisterStream(class LVCSim_StreamIOBase *stream);

    /// Unregister stream. Used internally by ABAP stream simulation.
    static void UnregisterStream(int id);

    /// Return stream for stream ID.
    static class LVCSim_StreamIOBase *GetStream(int id);

    //@}


    /*!
     * \name Dumping and tracing
     */
    //@{

    /// Set count of HTML dumps to create.
    static void SetDumpCount(int cnt = 9999999);

    /// Flush trace information (actually a NOP).
    static void FlushVtrace();

    /// Write checkpoint containing current simulator state.
    static void SaveFullState(const char *fname);

    /// Load simulator state from a checkpoint.
    static void RestoreFullState(const char *fname);

    /// Dump memory statistics.
    static void DumpMemoryStat();

    /// Set trace file for simulated vtrace.
    static void SetTraceFile(const char *fname);

    /// Set/reset OMS interface trace level (OMS_ERROR, OMS_INTERFACE, OMS_CONTAINERDIR, OMS_MEMORY, OMS_VAROBJECT, OMS_SESSION, OMS_LOCK)
    static void SetTraceLevel(const char *level, bool enable = true);

    /// Set trace levels numerically (actually a NOP).
    static void SetTraceLevel(long level);

    /// Count and calculate the size of all var and non-var objects in all containers.
    static void countObjects(OmsHandle &h, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize);

    /// Count and calculate the size of all var and non-var objects in all containers of a specified schema.
    static void countObjects(OmsHandle &h, const OmsSchemaHandle schema, int &objCount, size_t &objSize, int &varObjCount, size_t &varObjSize);

    //@}


    /*!
     * \name Multithreading support
     */
    //@{

    /// Create new thread.
    static LVCSim_Thread *ThreadBegin(size_t stacksize, LVCSim_ThreadCreateFlags flags,
        LVCSim_ThreadFunc threadProc, void *arg = NULL, int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Finish a thread.
    static void ThreadEnd(int return_code);

    /// Yield processor to another thread.
    static void ThreadYield();

    /// Return thread descriptor for current thread.
    static LVCSim_Thread *ThreadGetCurrent();

    /// Kill a thread.
    static void ThreadKill(LVCSim_Thread *thread, int *err = NULL,
        char *errbuf = NULL, size_t bufsize = 0);

    /// Set cancel flag for a thread.
    static void ThreadCancel(LVCSim_Thread *thread, int return_code,
        int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Put a thread into background.
    static void ThreadForget(LVCSim_Thread *thread);

    /// Suspend a thread.
    static void ThreadSuspend(LVCSim_Thread *thread, int *err = NULL,
        char *errbuf = NULL, size_t bufsize = 0);

    /// Resume a thread.
    static void ThreadResume(LVCSim_Thread *thread, int *err = NULL,
        char *errbuf = NULL, size_t bufsize = 0);

    /// Wait for a child thread to terminate.
    static int ThreadJoin(LVCSim_Thread *thread, int *err = NULL,
        char *errbuf = NULL, size_t bufsize = 0);

    /// Get suspended status for a thread.
    static bool ThreadIsSuspended(LVCSim_Thread *thread);

    /// Get alive status for a thread.
    static bool ThreadIsAlive(LVCSim_Thread *thread, int *err = NULL,
        char *errbuf = NULL, size_t bufsize = 0);
    //@}


    /*!
     * \name Mutex synchronization support
     */
    //@{

    /// Create a new mutex.
    static LVCSim_Mutex *MutexCreate();

    /// Destroy a mutex.
    static void MutexDestroy(LVCSim_Mutex *mutex);

    /// Enter critical section protected by a mutex.
    static void MutexBegin(LVCSim_Mutex *mutex);

    /// Try to enter critical section protected by a mutex.
    static bool MutexTryBegin(LVCSim_Mutex *mutex); // true on success

    /// Exit critical section protected by a mutex.
    static void MutexEnd(LVCSim_Mutex *mutex);

    //@}


    /*!
     * \name Thread local storage support
     */
    //@{

    /// Create a new thread-local-storage key.
    static void TLSCreate(int &key, int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Destroy a thread-local-storage key.
    static void TLSDestroy(int key, int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Set value for a key in thread-local-storage.
    static void TLSSet(int key, void *val, int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Get value for a key in thread-local-storage.
    static void *TLSGet(int key);

    //@}


    /*!
     * \name Semaphore synchronization support
     */
    //@{

    /// Create a new semaphore.
    static LVCSim_Semaphore *SemCreate(int initval = 0, int *err = NULL, char *errbuf = NULL, size_t bufsize = 0);

    /// Destroy a semaphore.
    static void SemDestroy(LVCSim_Semaphore *sem);

    /// Signal waiting processes waiting on a semaphore.
    static void SemSignal(LVCSim_Semaphore *sem);

    /// Wait on a semaphore.
    static void SemWait(LVCSim_Semaphore *sem);

    /// Wait on a semaphore, with timeout.
    static bool SemTimedWait(LVCSim_Semaphore *sem, int seconds);   // true on success

    //@}


    /*!
     * \name Debug allocator support
     */
    //@{

    /// Dump all memory leaks and check all blocks in freelist.
    static bool CheckMemoryLeaks();

    /// Check all memory allocations and deallocated memory.
    static bool CheckMemory();

    /// Debug allocator service function call.
    static bool DebugAllocatorEscape(LVCSim_DebugAllocatorEscapeEnum what);

    /// Get stack trace to current program position (internal).
    static int GetStackTrace(void** &trace, size_t TRACE_SIZE, size_t TRACE_SKIP);

    //@}

    /*!
     * \name Callbacks
     */
    //@{

    /// Set new callback interface.
    static void SetCallback(LVCSim_CallbackInterface *cb)
    {
      m_callback = cb;
    }

    /// Get current callback interface.
    static LVCSim_CallbackInterface *GetCallback()
    {
      return m_callback;
    }
    //@}

    /// Start COM routine in simulated ProcServer (in new pseudo-session).
    static void StartInUDEServer(OmsHandle &h, const char *libname, const char *methodname, ...);

    /// Commit in simulated ProcServer (in new pseudo-session).
    static void CommitInProcServer(OmsHandle &h, const char *libname);

    /// Rollback in simulated ProcServer (in new pseudo-session).
    static void RollbackInProcServer(OmsHandle &h, const char *libname);

    static void SetHeapCheckLevel(int flags){m_heapCheckLevel = flags;}

protected:
    // definition of extensions for liveCache simulator in Oms
    friend class LVCSim_liveCacheSink;
    friend class LVCSim_KernelInterface;
    friend class OmsHandle;
    friend class OmsAbstractObject;
    friend class LVCSim_Initer;

    /*!
     * \name Dump functionality
     */
    //@{

    /// Set object name (deprecated).
    static void setName(const OMS_ObjectId8 &oid, const char *name);

    /// Get object name (deprecated).
    static const char *getName(const OMS_ObjectId8 &oid);

    /// Dump simulator state.
    static void omsSimDump(OmsHandle &obj, int param);

    //@}

    /// Callback interface.
    static LVCSim_CallbackInterface *m_callback;

    static int m_heapCheckLevel;
};



extern
void omsFree(void* p);

/*!
 * This is the base class for all different stream classes with different
 * input and output behavior. You must define LVCSim_StreamIO<T> for a given storage
 * class T. Simulator defines the template LVCSim_StreamStorage as the base for table
 * storage. In order to declare a test handle, simply instantiate
 * LVCSim_StreamHandle.
 *		
 * Then, you can use something like:
 *
 * \code
 *	LVCSim_StreamStorage<T> myStorage;
 *	LVCSim_StreamHandle	handle(&myStorage);	// or OmsTypeStreamHandle handle(&myStorage);
 *	...
 *	myStorage.whatever_is_append(my_data);	// for input stream parameter
 *	...
 *	myCOMInstance.MYMETHOD(..., &handle, ...)
 *	...
 *	my_data = myStorage.whatever_is_read();	// for output stream parameter
 * \endcode
 */
class LVCSIM_API LVCSim_StreamIOBase {
public:
    /// Initialize instance.
    LVCSim_StreamIOBase() : m_refcnt(1)
    {
    }

    /// Destroy instance.
    virtual ~LVCSim_StreamIOBase();

    /// Reset stream
    virtual void resetStream(bool isInStream) = 0;

    /// Return size of one row of data managed by this object.
    virtual size_t getRowSize() const = 0;

    /// Return pointer to the next row from the stream.
    virtual const void *readRow() = 0;

    /// Write one row to the stream.
    virtual void writeRow(const void *data) = 0;

    /// Increase reference count.
    inline int ref()
    {
        return ++m_refcnt;
    }

    /// Decrease reference count.
    inline int unref()
    {
        int cnt = --m_refcnt;
        if (cnt == 0) {
            this->~LVCSim_StreamIOBase();
            omsFree(this);
        }
        return cnt;
    }

private:
    int	m_refcnt;   ///< Reference counter.
};

#endif	// LVCSIM_GLOBFNC_HPP



#define	LVCSIM	74030402			// LVC Simulator API version


#ifndef	LVCSIM_ONLY_GLOBALS

// Compatibility classes for old simulator
#if _MSC_VER >= 1310
  #include <iostream>   //   PTS 1129537
  using namespace std;  //   PTS 1129537
#else
  #include <iostream.h>
#endif

LVCSIM_API inline ostream& operator<<(ostream &os, const OMS_ObjectId8 &r)
{
    // TODO: return os << makeStr(r);
    return os;
}

LVCSIM_API inline ostream& operator<<(ostream &os, const OmsObjectId &r)
{
    // TODO: return os << makeStr(r);
    return os;
}

LVCSIM_API inline ostream& operator<<(ostream &os, const DbpError &r)
{
    return os << r.dbpDBError() << ": " << r.dbpErrorTextUnicode();
}

/*!
 * \name Compatibility macros for old simulator.
 */
//@{

/// Textual string, collector of simulator dump output.
#define	lCStr				OmsSimDumpCollector

/// Dump count setting.
#define	enableFullHtmlDump	LVCSim::SetDumpCount

/// Vtrace flush (no-op in new simulator).
#define	flushVtrace			LVCSim::FlushVtrace

/// Simulator reset.
#define	resetLcSim			LVCSim::Reset

/// Save checkpoint to a file.
#define	writeCheckpoint(X)	LVCSim::SaveFullState(X)

/// Read checkpoint from a file.
#define	readCheckpoint(X)	LVCSim::RestoreFullState(X)

/// Set trace file to use.
#define	openVtrace(X)		LVCSim::SetTraceFile(X)

// TODO:
/// Dump simulator memory statistics.
#define	dumpLCMemoryStat	LVCSim::DumpMemoryStat

/// Set trace level.
#define	enableVtrace(X)		LVCSim::SetTraceLevel(X)

//@}

/// Helper class to fool placement new and delete.
class LVCSim_FoolNewClass
{
};

/// Helper placement new.
inline void *operator new(size_t sz, LVCSim_FoolNewClass *p)
{
    return p;
}

/// Helper placement delete.
inline void operator delete(void *ptr, LVCSim_FoolNewClass *p)
{
}

/*!
 * Default stream storage.
 */
template <class T>
class LVCSim_StreamStorage {
private:
    /// One stream bucket containing one element (row).
    class Bucket {
    public:
        T       m_data;     ///< Row data.
        Bucket	*m_next;    ///< Next bucket in chain.

        /// Construct a bucket from data.
        Bucket(const T *_data) : m_data(*_data), m_next(NULL)
        {
        }

    };

    Bucket  *last;          ///< Last bucket in chain.
    Bucket  *first;         ///< First bucket in chain.
    Bucket  *next;

public:
    /// Construct an empty stream storage object.
    LVCSim_StreamStorage() : last(NULL), first(NULL), next(NULL)
    {
    }

    void resetStream(bool isInStream)
    {
      if(isInStream)
      {
        next=first;
      }
      else
      {
        next = first;
        while(next) {
          Bucket *toFree = next;
          next = next->m_next;
          omsFree(toFree);
        }
        first = NULL;
        last = NULL;
      }
    }

    /// Destroy stream storage object.
    ~LVCSim_StreamStorage()
    {
        next = first;
        while(next) {
          Bucket *toFree = next;
          next = next->m_next;
          omsFree(toFree);
        }
        first = NULL;
        last = NULL;
    }

    /// Read one row from the simulated stream.
    const T *readRow()
    {

      if (next) {
        Bucket *current=next;
        next = next->m_next;
        return &current->m_data;
      }
      else {
        return NULL;
      }
    }

    /// Write one row to the simulated stream.
    void writeRow(const T *m_data)
    {
      // append after last
      Bucket *p = new(reinterpret_cast<LVCSim_FoolNewClass*>(
        omsMalloc(sizeof(Bucket)))) Bucket(m_data);
      if (!last) {
        last = first = next = p;
      } else {
        last->m_next = p;
        last = p;
        if(!next){
          next = p;
        }
      }
    }
};




/*!
 * Default simulator stream I/O object
 */
template <class T>
class LVCSim_StreamIO : public LVCSim_StreamIOBase {
public:
  /// Create a new I/O object for given stream storage object.
  LVCSim_StreamIO(LVCSim_StreamStorage<T> *data)
  {
    m_storage = data;
    m_compressionActive = false;
  }

  virtual void setCompression(bool compressionActive)
  {
    m_compressionActive = compressionActive;
  }

  virtual bool getCompression()
  {
    return m_compressionActive;
  }

  virtual void resetStream(bool isInStream)
  {
    m_storage->resetStream(isInStream);
  }

  /// Get stream row size.
  virtual size_t getRowSize() const
  {
    return sizeof(T);
  }

  /// Read one row from the simulated stream.
  virtual const void *readRow()
  {
    return m_storage->readRow();
  }

  /// Write one row to the simulated stream.
  virtual void writeRow(const void *data)
  {
    m_storage->writeRow((const T*) data);
  }

private:
    LVCSim_StreamStorage<T>	*m_storage;   ///< Underlying stream storage.
    bool m_compressionActive;
};

/*!
 * This is the overridden handle used for simulating streams in simulator.
 *
 * This class uses some attributes in OmsTypeStreamHandle to store pointer
 * to the I/O object. It is a big hack, but necessary in order not to change
 * size of the handle.
 */
class LVCSim_StreamHandleBase : public OmsTypeStreamHandle {
private:
  /// Return reference to the pointer to the I/O object.
  LVCSim_StreamIOBase* &base()
  {
    char *addr = reinterpret_cast<char*>(&colDesc[1]);
#if defined(HPUX) || defined(OSF1) || defined(_WIN64)
    addr += 4;
#endif
    return *reinterpret_cast<LVCSim_StreamIOBase**>(addr);
  }

protected:
  /// Register I/O object.
  void registerStream(LVCSim_StreamIOBase *stream)
  {
    ABAPTabId = LVCSim::RegisterStream(stream);
    base() = stream;
    size = (unsigned short) stream->getRowSize();
    memberCount = 1;
    filler = 0;
    colDesc[0].length = size;
    colDesc[0].memberType = 0;
    colDesc[0].offset = 0;
    colDesc[0].inout = 0;
    colDesc[0].dec = 0;
  }

public:
  /// Create an empty instance of the stream handle.
  LVCSim_StreamHandleBase()
  {
    base() = NULL;
  }

  /// Explicit copy constructor.
  explicit LVCSim_StreamHandleBase(const LVCSim_StreamHandleBase &x)
  {
    base() = NULL;
    *this = x;
  }

  /// Destroy stream handle.
  ~LVCSim_StreamHandleBase()
  {
    if (base())
      if (base()->unref() == 0) {
        LVCSim::UnregisterStream(ABAPTabId);
      }
  }

  /// Assignment operator.
  LVCSim_StreamHandleBase &operator =(const LVCSim_StreamHandleBase &x)
  {
    if (base())
      if (base()->unref() == 0) {
        LVCSim::UnregisterStream(ABAPTabId);
      }
      memmove(this, &x, sizeof(x));
      if (base())	base()->ref();
      return *this;
  }
};



/*!
 * This is the overridden handle used for simulating ABAP streams
 * in simulator.
 */
class LVCSim_StreamHandle : public LVCSim_StreamHandleBase {
public:
  /// Create an empty instance.
  LVCSim_StreamHandle() : LVCSim_StreamHandleBase()
  {
  }

  /// Explicit copy constructor.
  explicit LVCSim_StreamHandle(const LVCSim_StreamHandle &x)
    : LVCSim_StreamHandleBase(x)
  {
  }

  /// This constructor creates a handle for given stream storage.
  template <class T>
  inline LVCSim_StreamHandle(LVCSim_StreamStorage<T> *source)
  {
    // register new stream with given source
    registerStream(new(reinterpret_cast<LVCSim_FoolNewClass*>(
      omsMalloc(sizeof(LVCSim_StreamIO<T>))))
      LVCSim_StreamIO<T>(source));
  }
};


/*!
 * This is the overridden handle used for simulating ABAP tables
 * in simulator.
 *
 * This class uses some attributes in OmsTypeABAPTabHandle to store pointer
 * to the I/O object. It is a big hack, but necessary in order not to change
 * size of the handle.
 */
class LVCSim_ABAPTabHandleBase : public OmsTypeABAPTabHandle {
private:
  /// Return reference to the pointer to the I/O object.
  LVCSim_StreamIOBase* &base()
  {
    char *addr = reinterpret_cast<char*>(&colDesc[1]);
#if defined(HPUX) || defined(OSF1) || defined(_WIN64)
    addr += 4;
#endif
    return *reinterpret_cast<LVCSim_StreamIOBase**>(addr);
  }

protected:
  /// Register I/O object.
  void registerStream(LVCSim_StreamIOBase *stream)
  {
    ABAPTabId = LVCSim::RegisterStream(stream);
    base() = stream;
    rowSize = (unsigned short) stream->getRowSize();
    colCount = 1;
    rowCount = 1;
    colDesc[0].length = rowSize;
    colDesc[0].abap_type = 0;
    colDesc[0].offset = 0;
    colDesc[0].inout = 0;
    colDesc[0].dec = 0;
  }

public:
  /// Create an empty instance.
  LVCSim_ABAPTabHandleBase()
  {
    base() = NULL;
  }

  /// Explicit copy constructor.
  explicit LVCSim_ABAPTabHandleBase(const LVCSim_ABAPTabHandleBase &x)
  {
    base() = NULL;
    *this = x;
  }

  /// Destroy ABAP table handle.
  ~LVCSim_ABAPTabHandleBase()
  {
    if (base())
      if (base()->unref() == 0) {
        LVCSim::UnregisterStream(ABAPTabId);
      }
  }

  /// Assignment operator.
  LVCSim_ABAPTabHandleBase &operator =(const LVCSim_ABAPTabHandleBase &x)
  {
    if (base())
      if (base()->unref() == 0) {
        LVCSim::UnregisterStream(ABAPTabId);
      }
      memmove(this, &x, sizeof(x));
      if (base()) base()->ref();
      return *this;
  }
};



/*!
 * This is the overridden handle used for simulating ABAP tables
 * in simulator.
 */
class LVCSim_ABAPTabHandle : public LVCSim_ABAPTabHandleBase {
public:
  /// Create an empty instance.
  LVCSim_ABAPTabHandle() : LVCSim_ABAPTabHandleBase()
  {
  }

  /// Explicit copy constructor.
  explicit LVCSim_ABAPTabHandle(const LVCSim_ABAPTabHandle &x)
    : LVCSim_ABAPTabHandleBase(x)
  {
  }

  /// This constructor creates a handle for given stream storage.
  template <class T>
  inline LVCSim_ABAPTabHandle(LVCSim_StreamStorage<T> *source)
  {
    // register new stream with given source
    registerStream(new(reinterpret_cast<LVCSim_FoolNewClass*>(
      omsMalloc(sizeof(LVCSim_StreamIO<T>))))
      LVCSim_StreamIO<T>(source));
  }
};


/*!
 * \brief Dummy SQL handle.
 */
class LVCSIM_API LVCSim_SqlHandle : public SqlHandle {
public:
  /// Create a dummy SQL handle.
  LVCSim_SqlHandle();

  /// Destroy a dummy SQL handle.
  ~LVCSim_SqlHandle();
};

#ifdef	SAPDB_LVCSIM_DEPRECATED
// #define to allow old test programs run without change
#define	toHtmlShort				omsSimDumpToHtmlShort
#define	toHtmlLong				omsSimDumpToHtmlLong
#define	setName					omsSimSetObjectName
#define	getName					omsSimGetObjectName
#define	omsDump					omsSimDump

#ifdef _WIN32
#define LCSIM_USE_THREADS
#else
#define LCSIM_USE_THREADS
#define USE_POSIX_THREADS
#endif	// _WIN32

#ifdef USE_POSIX_THREADS
#include <signal.h>
#include <pthread.h>
#endif

#endif	// SAPDB_LVCSIM_DEPRECATED
#endif	// LVCSIM_ONLY_GLOBALS

#endif  // LVCSIM_HPP
