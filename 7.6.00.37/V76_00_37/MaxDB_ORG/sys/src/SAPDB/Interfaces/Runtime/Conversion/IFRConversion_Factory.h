/*!
  @file           IFRConversion_Factory.h
  @author         D030044
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
#ifndef IFRCONVERSION_FACTORY_H
#define IFRCONVERSION_FACTORY_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_ShortInfo.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

class IFRConversion_Converter;

/**
 * @ingroup IFR_DataConv
 * Factory class used to create converter instance depending on the type
 * of the table.
 */
class IFRConversion_Factory 
{ 
public:    
    /**
     * Creates an instance of the appropriate conversion class
     * that matches the database type found the <var>shortinfo</var>
     * parameter.
     * @param shortinfo The short field info array that is the source.
     * @param index     The index within @c shortinfo
     * @param connection The connection.
     * @param isForUnicode Whether it is UCS2 or not (necessary for DATE/TIME/TIMESTAMP).
     * @param error An error handle to populate in case of an error.
     * @return The converter instance that is able to translate the
     *   database format into the host types for the format specified 
     *   by the short info.
     */
    static IFRConversion_Converter* createInstance(IFR_ShortInfo*  shortinfo,
                                                   IFR_Int4 index,
                                                   IFR_Connection& connection,
                                                   IFR_Bool isForUnicode,
                                                   IFR_ErrorHndl& error);
};

IFR_END_NAMESPACE

#endif //IFRCONVERSION_FACTORY_H
