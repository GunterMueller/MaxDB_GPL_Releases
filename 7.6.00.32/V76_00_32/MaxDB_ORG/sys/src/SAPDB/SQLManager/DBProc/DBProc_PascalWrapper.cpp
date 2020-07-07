/*!
  @file           DBProc_PascalWrapper.cpp
  @author         ThomasA
  @ingroup        DBProc
  @brief          pascal interface for db-procedure and db-function execution

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

#include "hak101.h"
#include "SQLManager/DBProc/DBProc_Handler.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

void a101_ExecuteSqlStatement (
    tak_acv_address                      acv_addr,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  sel,
    tsp00_MoveObjPtr                     retcode_addr,
    tgg00_StackList       VAR_ARRAY_REF  StackList,
    tsp00_Int4            VAR_VALUE_REF  qual_ptr,
    tgg00_BasisError      VAR_VALUE_REF  e)
{
    e = DBProc_Handler::ExecuteSqlStatement (REINTERPRET_CAST(SQLMan_Context*, acv_addr), sel,
        retcode_addr, &StackList[0], qual_ptr);
}

/*!
   @brief executes a user defined function
*/
void a101_UserDefinedFunctionCall (
    tgg00_TransContext       VAR_VALUE_REF  transContext,
    tgg00_SelectFieldsParam  VAR_VALUE_REF  interpreterContext,
    const tgg00_StackEntry   VAR_VALUE_REF  functionCode,
    const tgg00_StackEntry   VAR_VALUE_REF  functionId,
    tgg00_BasisError         VAR_VALUE_REF  error)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_UserDefinedFunctionCall", SQLMan_Trace, 1);
    DBProc_Handler::UserDefinedFunctionCall (transContext, interpreterContext,
        functionCode, functionId, error);
}

//----------------------------------------------------------------------------

/*!
   @brief executes a db-procedure
*/
void a101_ProcedureCall (
                         tak_all_command_glob  VAR_VALUE_REF  acv,
                         integer               VAR_VALUE_REF  outParamCount)
{
    SAPDBTRACE_ROUTINE_DEBUG ("a101_DetachColumnBuffer", SQLMan_Trace, 1);
    SQLMan_Context& context = SQLMan_Context::AcvToContext(acv);
    DBProc_Handler::ProcedureCall (context, outParamCount);
}
