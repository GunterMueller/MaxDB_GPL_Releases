/*!
  @file           IFRUtil_Tls.cpp
  @author         D039759
  @ingroup        IFR_Util
  @brief          Overlay on the teo07 functions for locking and thread handling.

  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "Interfaces/Runtime/IFR_Types.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/Util/IFRUtil_Tls.h"


/** as is from veo07.c **/
#include <errno.h>

#if defined(OSF1) || defined(_IBMR2) || defined(NMP) || defined(LINUX) || defined(SUN) || defined(SOLARIS)
#include <pthread.h>
#endif

#if defined(HP9)
#if defined(PA11) || defined(PA20W)
#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
#endif /* PA11 || PA20W */
#endif /* HP9 */

#if defined(HP_IA64)
#define _POSIX_C_SOURCE 199506L
#include <pthread.h>
#endif /* HP_IA64 */

#include <signal.h>

#if defined(_WIN32)

# define YIELD_CALL Sleep(0)

#else

# undef CTHREADS
# undef PTHREADS
# undef PTHREAD_DRAFT
# undef HAVE_PTHREAD_SCOPE_PROCESS
# undef TIMEOUT_SYSTEM_RC
    /* PTS 1105217 */
# undef USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID
# undef USE_GETSEQUENCE_NP_FOR_THREAD_ID
# undef USE_CLONE_PID_FOR_THREAD_ID

# if defined(SUN) || defined(SOLARIS)
#  define PTHREADS
#  define PTHREAD_DRAFT 10
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
# endif

# if defined(OSF1)
#  define PTHREADS
#  define PTHREAD_DRAFT 10
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_GETSEQUENCE_NP_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
#  define WANT_HIGHEST_ADDRESS_AS_STACK_ADDRESS
# endif

# if defined(_IBMR2)
#  define PTHREADS
#  define PTHREAD_DRAFT 7
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#  define YIELD_CALL yield()
#  define HAVE_PTHREAD_SCOPE_PROCESS
#  define WANT_HIGHEST_ADDRESS_AS_STACK_ADDRESS
# endif


# if defined(HP9)
#  if defined(PA11) || defined(PA20W)
#   define PTHREADS
#   define PTHREAD_DRAFT 10
#   define TIMEOUT_SYSTEM_RC ETIMEDOUT
#   define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#   define YIELD_CALL sched_yield()
#  else
#   define PTHREADS
#   define PTHREAD_DRAFT 4
#   define TIMEOUT_SYSTEM_RC EAGAIN
#   define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#   define YIELD_CALL pthread_yield()
#  endif 
# endif /* HP9 */

# if defined(HP_IA64)
#   define PTHREADS
#   define PTHREAD_DRAFT 10
#   define TIMEOUT_SYSTEM_RC ETIMEDOUT
#   define USE_EO07_GET_SEQUENCE_NUMBER_FOR_THREAD_ID 1
#   define YIELD_CALL sched_yield()
# endif /* HP_IA64 */

# if defined(NMP)
#  define PTHREADS
#  define PTHREAD_DRAFT 10
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_CLONE_PID_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
#  ifndef _POSIX_THREAD_ATTR_STACKSIZE
#    define _POSIX_THREAD_ATTR_STACKSIZE
#  endif
# endif  /* NMP */

# if defined(LINUX)
#  define PTHREADS
#  define PTHREAD_DRAFT 10
#  define TIMEOUT_SYSTEM_RC ETIMEDOUT
#  define USE_CLONE_PID_FOR_THREAD_ID 1
#  define YIELD_CALL sched_yield()
# endif  /* LINUX */

#endif /* !_WIN32 */

#if defined(_WIN32)
# define THREAD_TYPENAME_EO07 HANDLE
# define MUTEX_TYPENAME_EO07  CRITICAL_SECTION
#endif
#if defined(CTHREADS)
# define THREAD_TYPENAME_EO07 thread_t
# define MUTEX_TYPENAME_EO07  mutex_t
# define COND_TYPENAME_EO07   cond_t
#endif
#if defined(PTHREADS)
# define THREAD_TYPENAME_EO07 pthread_t
# define MUTEX_TYPENAME_EO07  pthread_mutex_t
# define COND_TYPENAME_EO07   pthread_cond_t
#endif


/* 
   How it works:
   
   The easy case:
   
   For Unix, there is a TLS key allocated setting a specific cleanup
   function. This cleanup function will free the memory, if 
   that has not happened before. The individual keyed entries are
   kept in a list, the pointer stored in TLS will point to the list.
   
   The difficult case:
   
   For Windows, a static array of slots pointing to memory chunk structures 
   is kept. This array is currently 4096 entries large, which should
   be enough (you can create currently at most 2028 threads 
   at the same 
   
   Each thread has a slot in the array assigned if it comes here and
   allocates some memory. 
   
   First, it is checked whether the thread has a tls key stored under
   the global key. If yes, it is used.
   
   If no, a hash is computed from the thread's id, then by looking 
   up this index in the global slot table it is looked whether 
   the slot is occupied. The index is incremented until a free
   place is found, this operation is performed synchronized such
   that no other thread occupies that location while being on the 
   same trip. If a place is occupied, it is looked whether the thread
   is still alive, or has gone. If it has gone, the memory chunk is
   replaced and freed. If that run loops around certainly there are
   too many threads and we return 0.
*/


struct tls_memchunk 
{
    teo07_ThreadId         threadid;
    void *                 memory[IFR_TLSINDEX_MAX];
};

#define TLS_MAX_THREADS 4096

       

/**
 * Frees a list of memory chunks.
 */
extern "C" {

static void free_tlsmemory(void *_chunk)
{
    tls_memchunk *chunk=(tls_memchunk*)_chunk;
    if(chunk == 0) {
        return;
    }
    for(int i=0; i<IFR_TLSINDEX_MAX; ++i) {
        if(chunk->memory[i]) {
            char *to_del=(char *) chunk->memory[i];
            sqlfree((tsp00_Uint1 *)to_del);
        }
    }
    sqlfree((tsp00_Uint1 *)chunk);
}

}

#ifdef PTHREADS
#if PTHREAD_DRAFT < 7
#define PTHREAD_RC(_rc,_x) (_rc) = (_x);if ( (_rc) == -1 ) (_rc) = errno
#endif
#if PTHREAD_DRAFT > 6
#define PTHREAD_RC(_rc,_x) (_rc) = (_x)
#endif
#endif /* PTHREADS */


static IFR_Int4 tls_key = 0;
static IFR_Int4 tls_initialised = 0;
static IFR_Bool tls_init_successfully=false;

#ifdef _WIN32
static tls_memchunk tls_mem[TLS_MAX_THREADS];
static long         tls_mem_lock[TLS_MAX_THREADS];

static inline IFR_UInt4 hash_threadid (IFR_UInt4 threadid)
{
    IFR_UInt4 hashvalue = 0;
    IFR_UInt4 length = 0;
    IFR_UInt4 chr;
    const unsigned char *data = (const unsigned char*)&threadid;
    
    for(int i=0; i<4; ++i)
    {
        chr = *data++;
        hashvalue += chr + (chr << 17);
        hashvalue ^= hashvalue >> 2;
        ++length;
    }
    hashvalue += 4 + (4 << 17);
    hashvalue ^= hashvalue >> 2;
    return (hashvalue % TLS_MAX_THREADS);
}


#endif

extern "C" void *allocate_tls(void *error)
{
    IFR_Bool *ok=(IFR_Bool*)error;
#ifdef _WIN32
    if ((tls_key = TlsAlloc()) == 0xFFFFFFFF) 
    {
        *ok=false;
        tls_key = 0;
    } else {
        *ok=true;
    }
#endif  
  
#ifdef CTHREADS
    teo001_SystemRc rc;
    tls_key = 0; /* Solaris returns 0 if no threadlib is linked with the Applikation */
    rc = thr_keycreate((thread_key_t*)&tls_key, free_tlsmemory);
    if (rc == ENOMEM) {
        *ok = false;
        tls_key = 0;
    } else {
        *ok = true;
    }
#if defined(SUN) || defined(SOLARIS)
    if (tls_key == 0) {
        *ok = false;
    }
#endif /* SUN */
#endif    
        
#ifdef PTHREADS
    int rc;
    tls_key= 0; /* Solaris returns 0 if no threadlib is linked with the Applikation */
#if PTHREAD_DRAFT < 7
    PTHREAD_RC(rc,pthread_keycreate((pthread_key_t*)&tls_key, free_tlsmemory));
#else
    PTHREAD_RC(rc,pthread_key_create((pthread_key_t*)&tls_key, free_tlsmemory));
#endif /* PTHREAD_DRAFT < 7 */
    if (rc == ENOMEM) 
    {
        *ok = false;
    }
    else {
        *ok = true;
    }
#endif    
    return 0;
}
    


extern "C" {
    void  *sqltlsgetmemory(int key, size_t bytes, void (*initfunction)(void*), IFR_Bool initialize)
    {
        if(key <0 || key>=IFR_TLSINDEX_MAX) {
            return 0;
        }
        
        // !!!
        // we don't have to wrap this into sqlonce, and we 
        // MUST NOT do so, as it is called the 1st defined time
        // already in a sqlonce context
        // !!!
        if(!tls_initialised) {
            if(!initialize) {
                return 0;
            }
            allocate_tls(&tls_init_successfully);
            tls_initialised=1;
        }
        
        if(!tls_init_successfully) {
            return 0;
        }
        // we can get the content 
        void *handle=sqlgettls(tls_key);
        
        if(handle == 0 && !initialize) {
            return 0;
        }
        
#ifdef _WIN32
        if(handle == 0) {
            teo07_ThreadId tid=sqlgetthreadid();
            IFR_UInt4 hash=hash_threadid(tid);
            IFR_UInt4 initialhash=hash;
            
            while(true) {
                // we must lock the position before we can enter
                while(InterlockedCompareExchange(tls_mem_lock + hash, 1, 0)) {
                    Sleep(50);
                }
                if(tls_mem[hash].threadid == 0) {
                    tls_mem[hash].threadid = tid;
                    InterlockedExchange(tls_mem_lock + hash, 0);
                    break;
                } else {
                    // check whether there is someone real in this ...
                    IFR_Bool canOccupy=false;
                    HANDLE h = OpenThread(THREAD_QUERY_INFORMATION,
                                          FALSE,
                                          tls_mem[hash].threadid);
                    // if the open fails, this thread does not exist,
                    // otherwise we need to check whether it is still executing.
                    if(h != NULL) {
                        DWORD exitcode;
                        BOOL  getexit_result;
                        getexit_result=GetExitCodeThread(h, &exitcode);
                        CloseHandle(h);
                        if(getexit_result && exitcode!=STILL_ACTIVE) {
                            canOccupy=true;
                        }
                    } else {
                        canOccupy=true;
                    }
                    
                    if(canOccupy) {
                        tls_mem[hash].threadid = tid;
                        InterlockedExchange(tls_mem_lock + hash, 0);
                        // recycle the waste of the previous thread
                        for(int i=0; i<IFR_TLSINDEX_MAX; ++i) {
                            if(tls_mem[hash].memory[i]) {
                                char *todel=(char *) tls_mem[hash].memory[i];
                                sqlfree((tsp00_Uint1 *)todel);
                                tls_mem[hash].memory[i]=0;
                            }
                        }
                        break;
                    } else {
                        InterlockedExchange(tls_mem_lock + hash, 0);
                        hash= (hash+1) % TLS_MAX_THREADS;
                        if(hash == initialhash) {
                            // if we have traveled one round, we give up
                            return 0;
                        }
                    }
                } 
            }
            
            handle = tls_mem + hash;
            tsp00_ErrTextc  errtext;
            teo07_ThreadErr threaderr;
            sqlsettls(tls_key, handle, errtext, &threaderr);
        }
#else
        // Unix - we look whether we have a handle, otherwise simply create one.
        if(handle == 0) {
            tsp00_Bool wasok;
            tls_memchunk *chunk;
            sqlallocat(sizeof(tls_memchunk), ( tsp00_Uint1 **) &chunk, &wasok);
            for(int i=0; i<IFR_TLSINDEX_MAX; ++i) {
                chunk->memory[i]=0;
            }
            chunk->threadid=sqlgetthreadid();
            handle = chunk;
            tsp00_ErrTextc  errtext;
            teo07_ThreadErr threaderr;
            sqlsettls(tls_key, handle, errtext, &threaderr);
        }
#endif
           
        tls_memchunk *chunk=(tls_memchunk*)handle;
        if(chunk->memory[key] == 0) {
            if(!initialize) {
                return 0;
            }
            tsp00_Bool wasok;
            sqlallocat((tsp00_Int4)bytes, (tsp00_Uint1 **) &(chunk->memory[key]), &wasok);
            if(initfunction != 0) {
                (*initfunction)(chunk->memory[key]);
            }
        }
        return chunk->memory[key];

    }
    
    void sqltlsfreememory(int key)
    {
        if(key <0 || key>=IFR_TLSINDEX_MAX) {
            return;
        }
        
        sqlonce(&tls_initialised, 
                allocate_tls,
                &tls_init_successfully);
        if(!tls_init_successfully) {
            return;
        }
        void *handle=sqlgettls(tls_key);

        if(handle == 0) {
            return;
        } 
        tls_memchunk *chunk=(tls_memchunk*) handle;

        if(chunk->memory[key]) {
            char *todel=(char *) chunk->memory[key];
            sqlfree((tsp00_Uint1*)todel);
        }
        
        return;
    }
    
}

