/*! @ingroup   KernelAdministration
    @file      Kernel_SharedState.h
    @author    JörgM
    @brief     This defines Kernel_SharedState struct

    This struct allow a representation of a current kernel administration state snapshot in a shared memory
    area.
*/
/*!
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
#ifndef KERNEL_SHAREDSTATE_H
#define KERNEL_SHAREDSTATE_H

#include "SAPDBCommon/SAPDB_Types.h"

struct Kernel_SharedState
{
    /*!
       This payload MUST only contain POD or structs of POD! No classes allowed in shared memory!!!
   
       To extend this struct add items at the end of the struct! This way older client will be able to see
       the part they know and all other parts as uninitialized!
     */
    SAPDB_UInt4 databaseFullState;        /* extended database full state */
    SAPDB_UInt4 logvolumeFullState;       /* extended log volume full state */
    /* Waiting for extensions... */
};
#endif  /* KERNEL_SHAREDSTATE_H */
