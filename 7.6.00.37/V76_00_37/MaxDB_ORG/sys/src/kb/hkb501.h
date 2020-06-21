/*!******************************************************************************

  @file         hkb501.h

  ------------------------------------------------------------------------------

  @author       FerdiF

  @brief        KB_ObjShare_lock

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

********************************************************************************/

#ifndef HKB501_H
#define HKB501_H


/*===========================================================================*
 *     RefType: Both                                                         *
 *===========================================================================*/

/*===========================================================================*
 *     MD5: '\x96\xb7\xb2\xc9\x8c\xe3\x6c\x25\xb6\x9b\xf2\x1f\x39\x8a\xd7\xdf'
 *===========================================================================*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "ggg00.h"
#include "gkb05.h"


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

externPascal void k501DestroyTransLockInfo (
    tkb05_TransEntry      VAR_VALUE_REF  Trans,
    tgg91_TransNo         VAR_VALUE_REF  ThisWriteTransId);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_TransIndex k501GetCollEntryIdx (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tkb05_KernelOid       VAR_VALUE_REF  Oid);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501GetShareLock (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_C8              VAR_ARRAY_REF  ObjFileNo,
    tgg00_ObjTransInfo    VAR_VALUE_REF  ObjTransInfo,
    tkb05_KernelOid       VAR_VALUE_REF  Oid,
    pasbool                              ignColEntry);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501GetShareLock2 (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_C8              VAR_ARRAY_REF  ObjFileNo,
    tkb05_KernelOid       VAR_VALUE_REF  Oid,
    pasbool                              ignColEntry);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tgg00_TransIndex k501GetExclusiveLock (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_C8              VAR_ARRAY_REF  ObjFileNo,
    tkb05_KernelOid       VAR_VALUE_REF  Oid,
    pasbool                              ignShrLckCsView,
    pasbool                              ignColEntry);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tkb05_ObjShareLockCtrlPtr k501GetHashHdrList (
    tsp00_Int4                           ListIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal tsp00_Int4 k501GetHashHdrSize ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501GetStatistics (
    tsp00_Int4            VAR_VALUE_REF  LockCntrlEntries,
    tsp00_Int4            VAR_VALUE_REF  LockCntrlEntriesUsed,
    tsp00_Int4            VAR_VALUE_REF  ShareLockEntries,
    tsp00_Int4            VAR_VALUE_REF  ShareLockEntriesUsed,
    tsp00_Int4            VAR_VALUE_REF  CollisionEntriesUsed);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501InitOMSShareLocks ();

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal pasbool k501IsObjTransUsed (
    tsp00_TaskId                         TaskId,
    tgg92_KernelOid&                     Oid,
    pasbool                              ignoreVersions,
    tgg91_TransNo         VAR_VALUE_REF  UpdTransId_gg00,
    tgg91_TransNo         VAR_VALUE_REF  ActTransNo);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501IsSelfLocked (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tkb05_KernelOid       VAR_VALUE_REF  Oid,
    pasbool                              bIsSharedLocked);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501RemoveCollisionEntry (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tkb05_ObjCollisionEntry  VAR_VALUE_REF  CollEntry);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501MarkTransend (
    tgg00_TransIndex                     TransIdx,
    pasbool                              committed);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501SetMinTrans (
    tsp00_TaskId                         TaskId,
    tgg91_TransNo         VAR_VALUE_REF  MinTrans);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501StoreCollisionEntry (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tkb05_ObjCollisionEntry  VAR_VALUE_REF  CollEntry);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501Unlock (
    tkb05_TransEntry      VAR_VALUE_REF  Trans,
    tkb05_KernelOid       VAR_VALUE_REF  Oid,
    pasbool                              isExclLock);

/* Don't edit this file by hand, it is generated by ptocProto.py */

externPascal void k501UnlockHashHdrList (
    tsp00_Int4                           ListIndex);

/* Don't edit this file by hand, it is generated by ptocProto.py */


#endif

