/*!
  \file    DBMSrvBTools_ToolEnum.cpp
  \author  TiloH
  \ingroup external backup tool handling by the DBMServer
  \brief   implementing an enumeration for directly supported
           external backup tools

\if EMIT_LICENCE

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


\endif
*/


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <string.h>
#include "SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp"
#include "hcn36.h"


// -----------------------------------------------------------------------------
// constants
// -----------------------------------------------------------------------------

const char * DBMSrvBTools_ToolEnum_ADSM="ADSM";
const char * DBMSrvBTools_ToolEnum_TSM="TSM";
const char * DBMSrvBTools_ToolEnum_NSR ="NSR";
const char * DBMSrvBTools_ToolEnum_BACK="BACK";
const char * DBMSrvBTools_ToolEnum_NONE="NONE";

// -----------------------------------------------------------------------------
// member functions of class DBMSrvBTools_ToolEnum
// -----------------------------------------------------------------------------

DBMSrvBTools_ToolEnum::DBMSrvBTools_ToolEnum()
{
    Value=None;
}

DBMSrvBTools_ToolEnum::DBMSrvBTools_ToolEnum(const AValue TheValue)
{
    Value=TheValue;
}

DBMSrvBTools_ToolEnum::DBMSrvBTools_ToolEnum(const DBMSrvBTools_ToolEnum & Original)
{
    Value=Original.Value;
}

DBMSrvBTools_ToolEnum::DBMSrvBTools_ToolEnum(const char * ToolString)
{
    Value=DetermineToolFromString(ToolString, strlen(ToolString));
}

DBMSrvBTools_ToolEnum::DBMSrvBTools_ToolEnum(const char * ToolString, size_t ToolStringLength)
{
    Value=DetermineToolFromString(ToolString, ToolStringLength);
}

DBMSrvBTools_ToolEnum::AValue DBMSrvBTools_ToolEnum::DetermineToolFromString(const char * String, size_t StringLength) const
{
    AValue rc=None;

    if(0!=String)
    {
        if(StrNStartsWithPrefix(String, StringLength, DBMSrvBTools_ToolEnum_ADSM) ||
           StrNStartsWithPrefix(String, StringLength, DBMSrvBTools_ToolEnum_TSM)     )
        {
            rc=ADSM;
        }
        else
            if(StrNStartsWithPrefix(String, StringLength, DBMSrvBTools_ToolEnum_NSR))
                rc=NSR;
            else
                if(StrNStartsWithPrefix(String, StringLength, DBMSrvBTools_ToolEnum_BACK))
                    rc=BACK;
    }

    return rc;
}

int DBMSrvBTools_ToolEnum::StrNStartsWithPrefix(const char *String, size_t StringLength, const char *Prefix)
{
    return (StringLength>=strlen(Prefix) &&                      // if String has more characters than Prefix
            0==cn36_StrNUprCmp(String, Prefix, strlen(Prefix))); // and starts with Prefix, 1 is returned
}

DBMSrvBTools_ToolEnum::operator DBMSrvBTools_ToolEnum::AValue() const
{
    return Value;
}

int DBMSrvBTools_ToolEnum::operator == (const DBMSrvBTools_ToolEnum &Other) const
{
    return Value==Other.Value;
}

const char * DBMSrvBTools_ToolEnum::AsString() const
{
    const char *rc=0;

    switch(Value)
    {
        case ADSM: rc=DBMSrvBTools_ToolEnum_ADSM; break;
        case NSR:  rc=DBMSrvBTools_ToolEnum_NSR;  break;
        case BACK: rc=DBMSrvBTools_ToolEnum_BACK; break;
        default:   rc=DBMSrvBTools_ToolEnum_NONE;
    }

    return rc;
}

int DBMSrvBTools_ToolEnum::IsAValidToolEnumString(const char * String)
{
    int rc=0;

    if(0==cn36_StrUprCmp(String, DBMSrvBTools_ToolEnum_ADSM) ||
       0==cn36_StrUprCmp(String, DBMSrvBTools_ToolEnum_TSM)  ||
       0==cn36_StrUprCmp(String, DBMSrvBTools_ToolEnum_NSR)  ||
       0==cn36_StrUprCmp(String, DBMSrvBTools_ToolEnum_BACK) ||
       0==cn36_StrUprCmp(String, DBMSrvBTools_ToolEnum_NONE)    )
    {
        rc=1;
    }

    return rc;
}

int DBMSrvBTools_ToolEnum::NumberOfBackupToolConnectors()
{
    return int(None);
}
