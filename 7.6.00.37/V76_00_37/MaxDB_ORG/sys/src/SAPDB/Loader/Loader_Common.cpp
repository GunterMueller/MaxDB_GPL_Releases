/*!
  \file    Loader_Common.cpp
  \author  SteffenS
  \brief   Special support functions for Fastloader

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

#include "Loader/Loader_Common.hpp"

//----------------------------------------------------------------------
const char *
LOADER_HostTypeToString(const LOADER_HostType hosttype)
{
    switch(hosttype)
    {
        case LOADER_HostType_PARAMETER_NOTSET:
            return "*** NOT SET ***";
        case LOADER_HostType_BINARY:
            return "BINARY";
        case LOADER_HostType_ASCII:
            return "ASCII";
        case LOADER_HostType_UCS2:
            return "UCS2 (BE)";
        case LOADER_HostType_UCS2_SWAPPED:
            return "UCS2 (LE)";
        case LOADER_HostType_UTF8:
            return "UTF8";
        case LOADER_HostType_UINT1:
            return "UINT1";
        case LOADER_HostType_INT1:
            return "INT1";
        case LOADER_HostType_UINT2:
            return "UINT2";
        case LOADER_HostType_INT2:
            return "INT2";
        case LOADER_HostType_UINT4:
            return "UINT4";
        case LOADER_HostType_INT4:
            return "INT4";
        case LOADER_HostType_UINT8:
            return "UINT8";
        case LOADER_HostType_INT8:
            return "INT8";
        case LOADER_HostType_DOUBLE:
            return "DOUBLE";
        case LOADER_HostType_FLOAT:
            return "FLOAT";
        case LOADER_HostType_DECIMAL:
            return "DECIMAL";
        case LOADER_HostType_SAP_LONG_BINARY:
            return "SAP LONG BINARY";
        case LOADER_HostType_SAP_LONG_VARCHAR_ASCII:
            return "SAP LONG VARCHAR ASCII";
        case LOADER_HostType_SAP_LONG_VARCHAR_UTF8:
            return "SAP LONG VARCHAR UTF8";
        case LOADER_HostType_SAP_LONG_VARCHAR_UCS2:
            return "SAP LONG VARCHAR UCS2";
        case LOADER_HostType_SAP_LONG_VARCHAR_UCS2_SWAPPED:
            return "SAP LONG VARCHAR UCS2 SWAPPED";
        default:
            return "UNKNOWN";
    }
}


const char *
LOADER_SQLTypeToString(const LOADER_SQLType sqltype)
{
    switch(sqltype)
    {
        case LOADER_SQLTYPE_FIXED:
            return "FIXED";
        case LOADER_SQLTYPE_FLOAT:
            return "FLOAT";
        case LOADER_SQLTYPE_CHA:
            return "CHAR ASCII";
        case LOADER_SQLTYPE_CHE:
            return "CHAR EBCDIC";
        case LOADER_SQLTYPE_CHB:
            return "CHAR BYTE";
        case LOADER_SQLTYPE_ROWID:
            return "ROWID";
        case LOADER_SQLTYPE_STRA:
            return "LONG ASCII";
        case LOADER_SQLTYPE_STRE:
            return "LONG EBCDIC";
        case LOADER_SQLTYPE_STRB:
            return "LONG BYTE";
        case LOADER_SQLTYPE_STRDB:
            return "LONG DOUBLE BYTE";
        case LOADER_SQLTYPE_DATE:
            return "DATE";
        case LOADER_SQLTYPE_TIME:
            return "TIME";
        case LOADER_SQLTYPE_VFLOAT:
            return "FLOAT";
        case LOADER_SQLTYPE_TIMESTAMP:
            return "TIMESTAMP";
        case LOADER_SQLTYPE_UNKNOWN:
            return "UNKNOWN";
        case LOADER_SQLTYPE_NUMBER:
            return "NUMBER";
        case LOADER_SQLTYPE_NONUMBER:
            return "UNKNOWN";
        case LOADER_SQLTYPE_DURATION:
            return "DURATION";
        case LOADER_SQLTYPE_DBYTEEBCDIC:
            return "DOUBLE BYTE EBCDIC";
        case LOADER_SQLTYPE_LONGA:
            return "LONG ASCII";
        case LOADER_SQLTYPE_LONGE:
            return "LONG EBCDIC";
        case LOADER_SQLTYPE_LONGB:
            return "LONG BYTE";
        case LOADER_SQLTYPE_LONGDB:
            return "LONG DOUBLE BYTE";
        case LOADER_SQLTYPE_BOOLEAN:
            return "BOOLEAN";
        case LOADER_SQLTYPE_UNICODE:
            return "UNICODE";
        case LOADER_SQLTYPE_DTFILLER1:
            return "UNKNOWN";
        case LOADER_SQLTYPE_DTFILLER2:
            return "UNKNOWN";
        case LOADER_SQLTYPE_DTFILLER3:
            return "UNKNOWN";
        case LOADER_SQLTYPE_DTFILLER4:
            return "UNKNOWN";
        case LOADER_SQLTYPE_SMALLINT:
            return "SMALLINT";
        case LOADER_SQLTYPE_INTEGER:
            return "INTEGER";
        case LOADER_SQLTYPE_VARCHARA:
            return "VARCHAR ASCII";
        case LOADER_SQLTYPE_VARCHARE:
            return "VARCHAR EBCDIC";
        case LOADER_SQLTYPE_VARCHARB:
            return "VARCHAR BYTE";
        case LOADER_SQLTYPE_STRUNI:
            return "UNICODE";
        case LOADER_SQLTYPE_LONGUNI:
            return "LONG UNICODE";
        case LOADER_SQLTYPE_VARCHARUNI:
            return "VARCHAR UNICODE";
        case LOADER_SQLTYPE_UDT:
            return "UDT";
        case LOADER_SQLTYPE_ABAPTABHANDLE:
            return "ABABTABHANDLE";
        case LOADER_SQLTYPE_DWYDE:
            return "DWYDE";
        default:
            return "UNKNOWN";
    }
}
