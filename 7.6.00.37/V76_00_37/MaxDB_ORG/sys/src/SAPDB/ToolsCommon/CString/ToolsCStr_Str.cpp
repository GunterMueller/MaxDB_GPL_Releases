/*!
    \file    ToolsCStr_Str.cpp
    \author  TiloH
    \ingroup infrastructure for tools group
    \brief   some useful functions combining C Strings with message lists

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#include <limits.h>

#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_New.hpp"

#include "hcn36.h"

bool ToolsCStr_Str::Alloc(char * & dest, size_t length, DBMSrvMsg_Error & error)
{
    bool rc=true;

    if(dest!=0)
        delete [] dest;

    dest=ToolsMeMa_New char [length+1];

    if(0==dest)
    {
        rc=false;
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
    }
    else
        dest[0]='\0';

    return rc;
}

bool ToolsCStr_Str::AllocCpy(char * & Dest, const char * Src, DBMSrvMsg_Error & error)
{
    bool rc=ToolsCStr_Str::Alloc(Dest, strlen(Src), error);

    if(rc)
        SAPDB_strcpy(Dest, Src);

    return rc;
}

bool ToolsCStr_Str::NAllocCpy(
    char            *& dest,
    const char      *  src,
    size_t             length,
    DBMSrvMsg_Error &  error)
{
    bool rc=ToolsCStr_Str::Alloc(dest, length, error);

    if(rc)
        ToolsCStr_Str::NCpy(dest, src, length);

    return rc;
}

void ToolsCStr_Str::Dealloc(
    char *& dest) {

    if(NULL != dest) {
        delete [] dest;
        dest = NULL;
    }
}

bool ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(
    char           *& dest,
    const char      * src,
    DBMSrvMsg_Error & error)
{
    const char * endLeadingSpaces=cn36_FirstNonWhiteSpaceOf(src);
    const char * startTrailingSpaces=cn36_StartOfTrailingWhiteSpaces(endLeadingSpaces);

    return ToolsCStr_Str::NAllocCpy(dest, endLeadingSpaces , startTrailingSpaces-endLeadingSpaces, error);
}


void ToolsCStr_Str::NCpy(
    char       * dest,
    const char * src,
    size_t       length)
{
    strncpy(dest, src, length);
    dest[length]='\0';
}

bool ToolsCStr_Str::ToInt(
    int             & integer,
    const char      * buf,
    DBMSrvMsg_Error & error)
{
    bool rc=cn36_StrToInt(integer, buf)?true:false;

    if(!rc)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_INTEGER,
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_INTEGER__STRING, buf),
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_INTEGER__BIT_SIZE, 8*sizeof(int)),
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_INTEGER__LIMIT_LOWER, INT_MIN),
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_INTEGER__LIMIT_UPPER, INT_MAX)));

    return rc;
}

bool ToolsCStr_Str::ToUInt8(
    teo00_Uint8     & unsignedInteger8,
    const char      * buf,
    DBMSrvMsg_Error & error)
{
    bool rc=cn36_StrToUInt8(unsignedInteger8, buf)?true:false;

    if(!rc)
        error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_UINTEGER,
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_UINTEGER__STRING, buf),
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_UINTEGER__BIT_SIZE, 8*sizeof(teo00_Uint8)),
	                                   Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_UINTEGER__LIMIT_UPPER, MAX_UINT8_CN36)));

    return rc;
}

