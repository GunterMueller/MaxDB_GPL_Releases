/*!
    @file     SharedSQL_CommandCache_Wrapper.cpp
    @ingroup  SharedSQL
    @author   DirkT
    @brief    PascalWrapper for SharedSQL_CommandCache
    @see            

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

#include "SQLManager/SharedSQL/SharedSQL_CommandCache.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"

#include "hak101.h"  /* pascal wrapper */
#include <string.h>

/// ...
static pasbool SharedSQL_OnOffStatus = true;
/// ...
static pasbool SharedSQL_StatusInit  = false;

/*!
    @brief  ...
    @return ...
*/
externPascal pasbool a101_SharedSQL_GetStatusParameter( void )
{
    if ( !SharedSQL_StatusInit )
        a101_SharedSQL_ReadStatusParameter();
    return SharedSQL_OnOffStatus; 
}

/*!
    @brief  ...
    @param  Status [in]
*/
externPascal void a101_SharedSQL_SetStatusParameter( pasbool Status )
{
    SharedSQL_OnOffStatus = Status; 
    SharedSQL_StatusInit  = true;
}

/*!
    @brief  ...
*/
externPascal void a101_SharedSQL_ReadStatusParameter( void )
{
    RTEConf_Parameter::String   stringValue;
    SAPDBErr_MessageList        errMsg;
    if( RTEConf_ParameterAccess::Instance()->GetString( UTF8( KERNELPARAM_SHAREDSQL ), stringValue, errMsg ))
    {
        if ( strcmp((SAPDB_Char *)(stringValue), (SAPDB_Char *)("YES")) == 0 )
            SharedSQL_OnOffStatus = true;
        else
            SharedSQL_OnOffStatus = false;
    }
    SharedSQL_StatusInit = true;
}
