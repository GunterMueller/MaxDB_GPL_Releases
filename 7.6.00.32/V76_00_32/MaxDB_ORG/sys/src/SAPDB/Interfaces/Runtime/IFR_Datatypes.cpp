/*!
  @file           IFR_Datatypes.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_DataConv
  @brief          SQL and Hostvartypes
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
#include "Interfaces/Runtime/IFR_Datatypes.h"

#ifdef IFR_NAMESPACE
namespace IFR_NAMESPACE {
#endif

//----------------------------------------------------------------------
const char *
IFR_HostTypeToString(const IFR_HostType hosttype)
{
    switch(hosttype) {
    case IFR_HOSTTYPE_PARAMETER_NOTSET:
        return "*** NOT SET ***";
    case IFR_HOSTTYPE_BINARY:
        return "BINARY";
    case IFR_HOSTTYPE_ASCII:
        return "ASCII";
    case IFR_HOSTTYPE_UCS2:
        return "UCS2 (BE)";
    case IFR_HOSTTYPE_UCS2_SWAPPED:
        return "UCS2 (LE)";
    case IFR_HOSTTYPE_UTF8:
        return "UTF8";
    case IFR_HOSTTYPE_UINT1:
        return "UINT1";
    case IFR_HOSTTYPE_INT1:
        return "INT1";
    case IFR_HOSTTYPE_UINT2:
        return "UINT2";
    case IFR_HOSTTYPE_INT2:
        return "INT2";
    case IFR_HOSTTYPE_UINT4:
        return "UINT4";
    case IFR_HOSTTYPE_INT4:
        return "INT4";
    case IFR_HOSTTYPE_UINT8:
        return "UINT8";
    case IFR_HOSTTYPE_INT8:
        return "INT8";
    case IFR_HOSTTYPE_DOUBLE:
        return "DOUBLE";
    case IFR_HOSTTYPE_FLOAT:
        return "FLOAT";
    case IFR_HOSTTYPE_ODBCDATE:
        return "DATE";
    case IFR_HOSTTYPE_ODBCTIME:
        return "TIME";
    case IFR_HOSTTYPE_ODBCTIMESTAMP:
        return "TIMESTAMP";
    case IFR_HOSTTYPE_ODBCNUMERIC:
        return "NUMERIC";
    case IFR_HOSTTYPE_GUID:
        return "GUID";        
    case IFR_HOSTTYPE_BLOB:
        return "BLOB";
    case IFR_HOSTTYPE_ASCII_LOB:
        return "ASCII LOB";
    case IFR_HOSTTYPE_UCS2_LOB:
        return "UCS2 LOB (BE)";
    case IFR_HOSTTYPE_UCS2_SWAPPED_LOB:
        return "UCS2 LOB (LE)";
    case IFR_HOSTTYPE_UTF8_LOB:
        return "UTF8 LOB";
    case IFR_HOSTTYPE_STREAM:
        return "STREAM";
    case IFR_HOSTTYPE_RAWHEX:
        return "RAW HEX";
    case IFR_HOSTTYPE_DECIMAL:
        return "DECIMAL";
    case IFR_HOSTTYPE_OMS_PACKED_8_3:
        return "PACKED(8,3)";
    case IFR_HOSTTYPE_OMS_PACKED_15_3:
        return "PACKED(15,3)";
    case IFR_HOSTTYPE_OMS_TIMESTAMP:
        return "OMSTIMESTAMP";
    case IFR_HOSTTYPE_USERDEFINED:
        return "USERDEFINED";        
    default:
        return "UNKNOWN";
    }
}

//----------------------------------------------------------------------
const char *IFR_SQLTypeToString(IFR_SQLType sqltype, 
                                char *buffer, 
                                IFR_Length bufferlength)
{
    if(bufferlength == IFR_NTS) {
        bufferlength = 512;
    }
    switch(sqltype) {
    case IFR_SQLTYPE_FIXED:
        return strncpy(buffer, "FIXED", bufferlength);
    case IFR_SQLTYPE_FLOAT:
        return strncpy(buffer, "FLOAT", bufferlength);
    case IFR_SQLTYPE_CHA:
        return strncpy(buffer, "CHAR ASCII", bufferlength);
    case IFR_SQLTYPE_CHE:
        return strncpy(buffer, "CHAR EBCDIC", bufferlength);
    case IFR_SQLTYPE_CHB:
        return strncpy(buffer, "CHAR BYTE", bufferlength);
    case IFR_SQLTYPE_ROWID:
        return strncpy(buffer, "ROWID", bufferlength);
    case IFR_SQLTYPE_DATE:
        return strncpy(buffer, "DATE", bufferlength);
    case IFR_SQLTYPE_TIME:
        return strncpy(buffer, "TIME", bufferlength);
    case IFR_SQLTYPE_VFLOAT:
        return strncpy(buffer, "VFLOAT", bufferlength);
    case IFR_SQLTYPE_TIMESTAMP:
        return strncpy(buffer, "TIMESTAMP", bufferlength);
    case IFR_SQLTYPE_NUMBER:
        return strncpy(buffer, "NUMBER", bufferlength);
    case IFR_SQLTYPE_NONUMBER:
        return strncpy(buffer, "NONUMBER", bufferlength);
    case IFR_SQLTYPE_STRA:
    case IFR_SQLTYPE_LONGA:
        return strncpy(buffer, "LONG ASCII", bufferlength);
    case IFR_SQLTYPE_STRE:
    case IFR_SQLTYPE_LONGE:
        return strncpy(buffer, "LONG EBCDIC", bufferlength);
    case IFR_SQLTYPE_STRB:
    case IFR_SQLTYPE_LONGB:
        return strncpy(buffer, "LONG BYTE", bufferlength);
    case IFR_SQLTYPE_BOOLEAN:
        return strncpy(buffer, "BOOLEAN", bufferlength);
    case IFR_SQLTYPE_UNICODE:
        return strncpy(buffer, "CHAR UNICODE", bufferlength);
    case IFR_SQLTYPE_SMALLINT:
        return strncpy(buffer, "SMALLINT", bufferlength);
    case IFR_SQLTYPE_INTEGER:
        return strncpy(buffer, "INTEGER", bufferlength);
    case IFR_SQLTYPE_VARCHARA:
        return strncpy(buffer, "VARCHAR ASCII", bufferlength);
    case IFR_SQLTYPE_VARCHARE:
        return strncpy(buffer, "VARCHAR EBCDIC", bufferlength);
    case IFR_SQLTYPE_VARCHARB:
        return strncpy(buffer, "VARCHAR BYTE", bufferlength);
    case IFR_SQLTYPE_LONGUNI:
        return strncpy(buffer, "LONG UNICODE", bufferlength);
    case IFR_SQLTYPE_STRUNI:
        return strncpy(buffer, "LONG UNICODE", bufferlength);
    case IFR_SQLTYPE_VARCHARUNI:
        return strncpy(buffer, "VARCHAR UNICODE", bufferlength);
    case IFR_SQLTYPE_UDT:
        return strncpy(buffer, "UDT", bufferlength);
    case IFR_SQLTYPE_ABAPTABHANDLE:
        return strncpy(buffer, "ABAP TABLE", bufferlength);
    case IFR_SQLTYPE_DWYDE:
        return strncpy(buffer, "DWYDE", bufferlength);
    default:
        sp77sprintf(buffer, (int)bufferlength, "OTHER (%d)", (IFR_Int4)sqltype);
        return buffer;
    }
}

#ifdef IFR_NAMESPACE
};
#endif
