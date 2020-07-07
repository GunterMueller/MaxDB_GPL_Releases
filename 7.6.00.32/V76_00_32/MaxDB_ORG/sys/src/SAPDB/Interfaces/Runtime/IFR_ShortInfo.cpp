/*!
  @file           IFR_ShortInfo.cpp
  @author         D039759
  @ingroup        IFR_Common
  @brief          Handling of short field infos
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
#include <string.h>
#include "Interfaces/Runtime/IFR_ShortInfo.h"

IFR_BEGIN_NAMESPACE

static const char * datatypenames[] = {
    "FIXED",
    "FLOAT",
    "CHAR ASCII",
    "CHAR EBCDIC",
    "CHAR BYTE",
    "ROWID",
    "STRA",
    "STRE",
    "STRB",
    "STRDB",
    "DATE",
    "TIME",
    "VFLOAT",
    "TIMESTAMP",
    "UNKNOWN",
    "NUMBER",
    "NONUMBER",
    "DURATION",
    "DBYTEEBCDIC",
    "LONG ASCII",
    "LONG EBCDIC",
    "LONG BYTE",
    "LONGDB",
    "BOOLEAN",
    "UNICODE",
    "DTFILLER1",
    "DTFILLER2",
    "DTFILLER3",
    "DTFILLER4",
    "SMALLINT",
    "INTEGER",
    "VARCHAR ASCII",
    "VARCHAR EBCDIC",
    "VARCHAR BYTE",
    "STRUNI",
    "LONG UNICODE",
    "VARCHAR UNICODE",
    "UDT",
    "ABAPTABHANDLE",
    "WYDE"
};

//----------------------------------------------------------------------
void
IFR_ShortInfo::toString(char *str, int strlength)
{
    memset(str, 0, strlength);  
    if(datatype > IFR_SQLTYPE_MAX) {
        strncpy(str, "invalid SQL type", strlength-1);
        str[strlength-1]='\0';
    } else {
        strncpy(str, datatypenames[datatype], strlength-1);
        str[strlength-1]='\0';
    }
    return;
}

//----------------------------------------------------------------------
inline const char *ShortInfo_Mode(IFR_UInt1 mode)
{
    switch(mode) {
    case 0:
        return "invalid mode";
    case  1:
        return "NOT NULL";
    case  2:
        return "NULL";
    case  3:
        return "invalid mode";
    case  4:
        return "HAS DEFAULT";
    case  5:
        return "NOT NULL, HAS DEFAULT";
    case  6:
        return "NULL, HAS DEFAULT";
    case  7:
        return "invalid mode";
    case  8:
        return "ESCAPE CHARACTER";
    case  9:
        return "NOT NULL, ESCAPE CHARACTER";
    case 10:
        return "NULL, ESCAPE CHARACTER";
    case 11:
        return "invalid mode";
    case 12:
        return "HAS DEFAULT, ESCAPE CHARACTER";
    case 13:
        return "NOT NULL, HAS DEFAULT, ESCAPE CHARACTER";
    case 14:
        return "NULL, HAS DEFAULT, ESCAPE CHARACTER";
    case 15:
        return "invalid mode";
    default:
        return "invalid mode";
    }
}

//----------------------------------------------------------------------
inline const char *ShortInfo_IOType(IFR_UInt1 iotype)
{
    switch(iotype)
    {
    case 0:
        return "IN";
    case 1:
        return "OUT";
    case 2:
        return "INOUT";
    default:
        return "invalid iotype";
    }
}


//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFR_ShortInfo& shortinfo)
{
    s << "(mode=" << ShortInfo_Mode(shortinfo.mode) 
      << ", iotype=" << ShortInfo_IOType(shortinfo.iotype)
      << ", datatype="; 

    switch(shortinfo.datatype) {
    case IFR_SQLTYPE_FIXED:
        s << "FIXED(" << shortinfo.length << "," << (IFR_Int4)shortinfo.frac << ")";
        break;
    case IFR_SQLTYPE_FLOAT:
        s << "FLOAT(" << shortinfo.length << ")";
        break;
    case IFR_SQLTYPE_CHA:
        s << "CHAR (" << shortinfo.length << ") ASCII";
        break;
    case IFR_SQLTYPE_CHE:
        s << "CHAR (" << shortinfo.length << ") EBCDIC";
        break;
    case IFR_SQLTYPE_CHB:
        s << "CHAR (" << shortinfo.length << ") BYTE";
        break;
    case IFR_SQLTYPE_ROWID:
        s << "ROWID";
        break;
    case IFR_SQLTYPE_STRA:
    case IFR_SQLTYPE_LONGA:
        s << "LONG ASCII";
        break;
    case IFR_SQLTYPE_STRE:
    case IFR_SQLTYPE_LONGE:
        s << "LONG EBCDIC ";
        break;
    case IFR_SQLTYPE_STRB:
    case IFR_SQLTYPE_LONGB:
        s << "LONG BYTE";
        break;
    case IFR_SQLTYPE_STRDB:
        s << "STRDB";
        break;
    case IFR_SQLTYPE_DATE:
        s << "DATE";
        break;
    case IFR_SQLTYPE_TIME:
        s << "TIME";
        break;
    case IFR_SQLTYPE_VFLOAT:
        s << "VFLOAT (" << shortinfo.length << ")";
        break;
    case IFR_SQLTYPE_TIMESTAMP:
        s << "TIMESTAMP";
        break;
    case IFR_SQLTYPE_BOOLEAN:
        s << "BOOLEAN";
        break;
    case IFR_SQLTYPE_UNICODE:
        s << "CHAR (" << shortinfo.length << ") UNICODE";
        break;
    case IFR_SQLTYPE_INTEGER:
        s << "INTEGER";
        break;
    case IFR_SQLTYPE_SMALLINT:
        s << "SMALLINT";
        break;
    case IFR_SQLTYPE_VARCHARA:
        s << "VARCHAR (" << shortinfo.length << ") ASCII";
        break;
    case IFR_SQLTYPE_VARCHARE:
        s << "VARCHAR (" << shortinfo.length << ") EBCDIC";
        break;
    case IFR_SQLTYPE_VARCHARB:
        s << "VARCHAR (" << shortinfo.length << ") BYTE";
        break;
    case IFR_SQLTYPE_STRUNI:
    case IFR_SQLTYPE_LONGUNI:
        s << "LONG UNICODE";
        break;
    case IFR_SQLTYPE_VARCHARUNI:
        s << "VARCHAR (" << shortinfo.length << ") UNICODE";
        break;
    case IFR_SQLTYPE_ABAPTABHANDLE:
        s << "ABAP/OMS STREAM (" << (shortinfo.frac + 1) << ")";
        break;
    default:
        s << "datatype(" << (IFR_Int4)shortinfo.datatype << ") (" << shortinfo.length << "," << shortinfo.frac << ")";
    }
    return s << ", iolength=" << shortinfo.iolength 
             << ", bufpos=" << shortinfo.pos.bufpos << ")";
}

IFR_END_NAMESPACE
