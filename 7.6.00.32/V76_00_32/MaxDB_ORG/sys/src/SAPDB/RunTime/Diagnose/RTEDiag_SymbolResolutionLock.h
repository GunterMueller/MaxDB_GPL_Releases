/*!
  @file           RTEDiag_SymbolResolutionLock.h
  @author         JoergM
  @ingroup        RunTime
  @brief          Symbol resolution lock interface

  The symbol resolution lock is a well known lock used to synchronize symbol resolution
  between different threads. It is special in such a way it must be resettable by a signal
  handler to allow to 'force' usage in a crash situation...

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
#ifndef RTEDIAG_SYMBOLRESOLUTIONLOCK_H
#define RTEDIAG_SYMBOLRESOLUTIONLOCK_H


/*===========================================================================*
 *  PROTOTYPES ...                                                           *
 *===========================================================================*/

#include "SAPDBCommon/SAPDB_Types.h"

#if defined (__cplusplus)
extern "C" 
{ /* begin of extern "C" scope */
#endif
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
SAPDB_Bool RTEDiag_SymbolResolutionInit(volatile int *pEverCalled, void **pAnchor);

/*!
  @brief lock the symbol resolution lock
 */
void RTEDiag_SymbolResolutionLock();

/*!
  @brief unlock the symbol resolution lock
 */
void RTEDiag_SymbolResolutionUnlock();

/*!
  @brief call to check if symbol resolution is switched off
  @return true if switched off, false if symbol resolution switch on
 */
SAPDB_Bool RTEDiag_SymbolResolutionAllowed();

#if defined (__cplusplus)
} /* end of extern "C" scope */

/*!
  @brief create registered named symbol resolution lock
  @return true if creation succeeded, false if creation failed
 */
SAPDB_Bool RTEDiag_CreatedRegisteredSymbolResolutionLock();

/*!
  @brief return number of symbol resolution lock calls up to now

  This information is used by the x_cons show total command. For more statistic information
  you should use the system view SPINLOCKSTATISTICS for spinlock named "SYMBOL_RESOLUTION".

  @return number of symbol resolution lock calls
 */
SAPDB_UInt8 RTEDiag_SymbolResolutionLockCalls();

#endif

#endif  /* RTEDIAG_SYMBOLRESOLUTIONLOCK_H */
