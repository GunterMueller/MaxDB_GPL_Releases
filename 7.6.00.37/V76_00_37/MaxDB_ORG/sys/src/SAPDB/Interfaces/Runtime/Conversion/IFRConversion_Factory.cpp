/*!
  @file           IFRConversion_Factory.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_DataConv
  @brief
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
#include "Interfaces/Runtime/Conversion/IFRConversion_Factory.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Connection.h"


#include "Interfaces/Runtime/Conversion/IFRConversion_CharAsciiConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_VarcharAsciiConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_VarcharUCS2Converter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_CharUCS2Converter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_CharByteConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_VarcharByteConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_NumericConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_DateConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_TimeConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_DateTimeConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_BooleanConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_StreamConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_ABAPStreamConverter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_DateUCS2Converter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_TimeUCS2Converter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_DateTimeUCS2Converter.h"

//----------------------------------------------------------------------
IFRConversion_Converter*
IFRConversion_Factory::createInstance(IFR_ShortInfo* shortinfoarray,
                                      IFR_Int4 index,
                                      IFR_Connection& connection,
                                      IFR_Bool isForUnicode,
                                      IFR_ErrorHndl& error)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Factory,
                             createInstance,
                             &connection);

    IFR_ShortInfo& shortinfo = shortinfoarray[index];
    SAPDBMem_IRawAllocator& allocator=connection.allocator;

    switch(shortinfo.datatype) {
    case IFR_SQLTYPE_CHA: {
        // for historic reasons, the database wants to have it blown up for ASCII if operating in UNICODE mode.
        if(isForUnicode) {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_CharUCS2Converter(shortinfo, allocator, connection));
        } else {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_CharAsciiConverter(shortinfo, allocator, connection));
        }
    }
    case IFR_SQLTYPE_CHB:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_CharByteConverter(shortinfo, allocator, connection));
    case IFR_SQLTYPE_VARCHARA: {
        // see above 
        if(isForUnicode) {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_VarcharUCS2Converter(shortinfo, allocator, connection));
        } else {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_VarcharAsciiConverter(shortinfo, allocator, connection));
        }
    }
    case IFR_SQLTYPE_VARCHARB:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_VarcharByteConverter(shortinfo, allocator, connection));
    case IFR_SQLTYPE_BOOLEAN:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_BooleanConverter(shortinfo, allocator));
    case  IFR_SQLTYPE_FIXED:
    case  IFR_SQLTYPE_FLOAT:
    case  IFR_SQLTYPE_VFLOAT:
    case  IFR_SQLTYPE_SMALLINT:
    case  IFR_SQLTYPE_INTEGER:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_NumericConverter(shortinfo, allocator, connection));

    case IFR_SQLTYPE_DATE:
        if(isForUnicode) {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_DateUCS2Converter(shortinfo, allocator, connection));
        } else {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_DateConverter(shortinfo, allocator, connection));
        }
    case IFR_SQLTYPE_TIME:
        if(isForUnicode) {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_TimeUCS2Converter(shortinfo, allocator, connection));
        } else {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_TimeConverter(shortinfo, allocator, connection));
        }
    case IFR_SQLTYPE_TIMESTAMP:
        if(isForUnicode) {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_DateTimeUCS2Converter(shortinfo, allocator, connection));
        } else {
            DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_DateTimeConverter(shortinfo, allocator, connection));
        }
    case IFR_SQLTYPE_STRA:
    case IFR_SQLTYPE_STRE:
    case IFR_SQLTYPE_LONGA:
    case IFR_SQLTYPE_LONGE:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_StreamConverter(shortinfo, false, allocator, connection));
    case IFR_SQLTYPE_STRB:
    case IFR_SQLTYPE_LONGB:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_StreamConverter(shortinfo, true, allocator, connection));
    case IFR_SQLTYPE_STRUNI:
    case IFR_SQLTYPE_LONGUNI:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_StreamConverter(shortinfo, false, allocator, connection));
    case IFR_SQLTYPE_UNICODE:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_CharUCS2Converter(shortinfo, allocator, connection));
    case IFR_SQLTYPE_VARCHARUNI:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_VarcharUCS2Converter(shortinfo, allocator, connection));
    case IFR_SQLTYPE_ABAPTABHANDLE:
        DBUG_RETURN(new (IFR_ALLOCATOR(allocator)) IFRConversion_ABAPStreamConverter(shortinfo, allocator));
    default:
        error.setRuntimeError(IFR_ERR_UNSUPPORTED_PARAMETERTYPE_I, index + 1);
        DBUG_RETURN(((IFRConversion_Converter *)0));
    }
}


