/*!
  @file           SQLDBC_Internals.h
  @author         DL SAP DB INTERFACES
  @ingroup        SQLDBC
  @brief          Contains typedefs and class definitions for internal use.

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
#ifndef SQLDBC_INTERNALS_H
#define SQLDBC_INTERNALS_H

#include "SAPDBCommon/SAPDB_Types.h"

#ifdef SQLDBC_RUNTIME_IN_NAMESPACE
namespace SQLDBC {
#endif

/** 
 * SQLDBC_Spinlock is the interface of a class that provides a Spinlock. 
 * 
 * You can create a SQLDBC_Spinlock using SQLDBC_IRuntime->createSpinlock(...)
 * The implementation of the SQLDBC_Spinlock will be part of the 
 * SQLDBC_Runtime implementation.
 *
 * The same applies to multi-threading - if it is not needed you may 
 * supply dummy placeholder implementations for the lock operations.
 */
class SQLDBC_Spinlock{
public:
  virtual ~SQLDBC_Spinlock(){};
  
    /**
     * Locks a spinlock.
     * @return <code>true</code> if the method succeeds,
     * <code>false</code>  if it fails.
     */
    virtual SAPDB_Bool lock() = 0;
    
    /**
     * Unlocks a spinlock.
     * @return <code>true</code> if the spinlock was unlocked successfully,
     * <code>false</code> if it fails. 
     */
    virtual SAPDB_Bool unlock() = 0;

    /**
     * Deletes a spinlock.
     * @return <code>true</code> if the spinlock was unlocked successfully,
     * <code>false</code> if it fails. 
     */
    virtual SAPDB_Bool deleteLock() = 0;

};

#ifdef SQLDBC_RUNTIME_IN_NAMESPACE
};
#endif

#endif
