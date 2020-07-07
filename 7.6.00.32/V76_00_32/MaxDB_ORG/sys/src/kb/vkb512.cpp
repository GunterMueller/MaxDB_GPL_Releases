/*!
  @file     vkb512.cpp
  @author   FerdiF
  @ingroup  Transaction
  @brief    This module is used access c++ code from pascal code

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

#include "Transaction/Trans_ExtendedMonitoring.hpp"
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "gsp00.h"
#include "ggg00.h"
#include "hkb512.h"

/*************************************************************************/

externPascal void 
k512MonEndTransaction (tsp00_TaskId      TaskId,
                       tgg00_TransIndex  TransIndex,
                       pasbool           committed)
{
    Trans_ExtendedMonitoring::EndTransaction(TaskId, TransIndex, ((committed == 0) ? SAPDB_FALSE : SAPDB_TRUE));
};

/*************************************************************************/

externPascal 
void k512MonInit() 
{
    Trans_ExtendedMonitoring::InitializeMonitoring();
};

/*************************************************************************/

externPascal void 
k512MonRestartTransaction (tsp00_Addr& monContextPtr)
{
    Trans_ExtendedMonitoring::RestartTransaction(
        reinterpret_cast<Trans_ExtendedMonitoringContext *&>(monContextPtr) );
};

/*************************************************************************/
externPascal void 
k512MonStartTransaction (
    tgg00_TransContext    VAR_VALUE_REF  Trans,
    tsp00_Addr            VAR_VALUE_REF  monContextPtr)
{
   Trans_ExtendedMonitoring::StartTransaction(
       Trans,
       reinterpret_cast<Trans_ExtendedMonitoringContext *&>(monContextPtr));
};

/*************************************************************************/

externPascal void 
k512SetTransactionComment (tsp00_TaskId                         TaskId,
                           tgg00_TransIndex                     TransIndex,
                           tsp00_Int2                           commentLength,
                           SAPDB_Char*                          pComment)
{
    Trans_ExtendedMonitoring::SetTransactionComment(TaskId, TransIndex, commentLength, pComment );
};
