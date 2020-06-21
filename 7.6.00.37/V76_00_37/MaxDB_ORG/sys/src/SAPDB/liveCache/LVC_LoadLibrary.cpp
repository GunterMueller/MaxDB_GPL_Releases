/*!
 * \file    LVC_LoadLibrary.cpp
 * \author  IvanS
 * \brief   Loading of shared libraries for OMS stored procedure.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/

#include "liveCache/LVC_LoadLibrary.hpp"
#include "liveCache/LVC_Dispatcher.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "ggg00.h"
#include "ggg01.h"
#include "ggg251.h"
#include "geo007_1.h"

#include "geo60.h"
#include "hsp77.h"
#include "heo670.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/

class LVC_LoadLibraryAllocator {
public :
  void*  allocate(size_t sz)
  { 
    void* p = malloc(sz);
    if ( !p ) throw DbpError(DbpError::DB_ERROR, e_new_failed, __FILE__, __LINE__);
    return p; 
  }

  void   deallocate(void* p)
  { 
    free(p); 
  }
};

/*---------------------------------------------------------------------------*/

class LVC_LoadLibraryDirEntry
{
private:
  HANDLE m_hd;
  short  m_usedCount;
  char*  m_namePtr;
  bool   m_global;
public:
  LVC_LoadLibraryDirEntry(HANDLE hd, const char* p, bool g) : m_hd(hd), m_usedCount(1), m_global(g)
  {
    m_namePtr = (char *)malloc(strlen(p)+1);
    if ( !m_namePtr ) 
      throw DbpError(DbpError::DB_ERROR, e_new_failed, __FILE__, __LINE__);
    strcpy(m_namePtr,p);
  }

  ~LVC_LoadLibraryDirEntry()
  {
    free(m_namePtr);
  }

  void  incrementUsedCount() { 
    m_usedCount++; 
  }

  short  decrementUsedCount() {
    if (m_usedCount > 0) {
      m_usedCount--;
    }
    return m_usedCount; 
  }

  const char * const getNamePtr() const {
    return m_namePtr;
  }

  HANDLE getHandle() const {
    return m_hd;
  };

  bool getGlobalFlg() const {
    return m_global;
  }

  void* operator new( size_t sz )                  /* PTS 1110403 */
  {                                                /* PTS 1110403 */
    return malloc( sz );                           /* PTS 1110403 */
  }                                                /* PTS 1110403 */

  void operator delete(void* p)                    /* PTS 1110403 */
  {                                                /* PTS 1110403 */
    free( p );                                     /* PTS 1110403 */
  }                                                /* PTS 1110403 */
};

/*---------------------------------------------------------------------------*/

static LVC_LoadLibraryAllocator                                        DefaultAllocator;
static cgg251DCList<LVC_LoadLibraryDirEntry*,LVC_LoadLibraryAllocator> LoadedLibDir(&DefaultAllocator);

/*---------------------------------------------------------------------------*/

void LVC_LoadLibrary(const char *path, 
                     bool      loadGlobal,
                     HANDLE    *hd,
                     char      *errtext,
                     int       errtext_size) 
{
  LVC_DispatcherLock cs;                   /* PTS 1109279 */
  //RTESync_LockedScope lck(LVC_Dispatcher::getDCOMSpinlock());

  bool found = false;
  LVC_LoadLibraryDirEntry *curr;
  for ( cgg251dclIterator<LVC_LoadLibraryDirEntry*,LVC_LoadLibraryAllocator> iter = LoadedLibDir.begin(); iter; ++iter) {
    curr = *iter();
    if ( !strcmp(curr->getNamePtr(), path) ) {
      found = true;
      break;
    }
  }
  cs.Leave();                               /* PTS 1109279 */

  if ( found ) {
    curr->incrementUsedCount();
    *hd = curr->getHandle();
  }
  else {
    *hd = sqlLoadLibraryEx(CONST_CAST(char*,path), (loadGlobal) ? RTLD_GLOBAL_EO02 : RTLD_LOCAL_EO02, errtext, errtext_size);
    if ( *hd ) {
      try {
        curr = new LVC_LoadLibraryDirEntry(*hd, path, loadGlobal);
        cs.Enter();                                     
        LoadedLibDir.push_front(curr);
        cs.Leave(); 
        
        // Reload Symbols (PTS 1131668)
        eo670_UpdateSymbols();
      }
      catch (DbpError &e) {
        DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
        if (pCBInterface){
          pCBInterface->dbpCaughtError(e);
        }

        sqlFreeLibrary(*hd, errtext, errtext_size);
        *hd = NULL;
      }
    }
    else  {
      char buf[256];                                  
      sp77sprintf(&buf[0], sizeof(buf), "LVC_LoadLibrary - File: %.100s  err: %.64s", path, errtext );
      MSGD ((ERR_DCOM_DUMP_INFO, &buf[0] ));
    }
  }
};

/*---------------------------------------------------------------------------*/

void LVC_FreeLibrary(HANDLE      hd,
                     bool        *ok,
                     char        *errtext,
                     int         errtext_size)
{
  LVC_DispatcherLock cs;                   /* PTS 1109279 */
  // RTESync_LockedScope lck(LVC_Dispatcher::getDCOMSpinlock());

  bool found = false;
  *ok = false;
  LVC_LoadLibraryDirEntry *curr;
  for ( cgg251dclIterator<LVC_LoadLibraryDirEntry*,LVC_LoadLibraryAllocator> iter = LoadedLibDir.begin(); iter; ++iter) {
    curr = *iter();
    if ( curr->getHandle() == hd ) {
      short useCnt = curr->decrementUsedCount();
#if 0 /* PTS 1108386 */
      if ( false /* PTS 1107191, 0 == useCnt */) {
        LoadedLibDir.remove(iter);
        delete curr;
        if ( sqlFreeLibrary(hd, errtext, errtext_size)) 
          *ok = true;
        else
          *ok = false;
      }
      else
#endif
        *ok = true;
      break;
    }
  }
};

/*---------------------------------------------------------------------------*/
