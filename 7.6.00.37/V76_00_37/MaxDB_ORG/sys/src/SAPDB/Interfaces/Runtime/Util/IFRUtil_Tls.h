/*!
  @file           IFRUtil_Tls.h
  @author         D039759
  @ingroup        IFR_Util
  @brief          Supplemental functions for allocating memory specific to threads.
  The memory is returned to the OS on thread exist, or some time later on.

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
#ifndef IFRUTIL_TLS_H
#define IFRUTIL_TLS_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "heo07.h"


#define IFR_TLSINDEX_MAX 4

/// The index used for the trace information.
#define IFR_TRACE_TLSINDEX    0

extern "C" {
    
    /**
     * Gets thread-local memory that is mapped to a specific key.
     * The memory is freed at thread exit, if not before.
     * @param key The key to the TLS, which must be less than <code>IFR_TLSINDEX_MAX</code>.
     * @param bytes The size in bytes that are to be allocated.
     * @param initfunc A function called once when the memory is allocated, if not NULL
     * @param initialize Whether the allocation should be done, if the key is not found.
     * @return The memory identified by <code>key</code>, or 0 if there is a system
     *         level error, or there is no memory and <code>initialize</code> was
     *         <code>false</code>.
     */
    void  *sqltlsgetmemory(int key, size_t bytes, void (*initfunc)(void*), IFR_Bool initialize);
    
    /**
     * Releases thread-local memory attached to a key.
     */
    void  sqltlsfreememory(int key);
}

#endif  /* IFRUTIL_LOCK_H */
