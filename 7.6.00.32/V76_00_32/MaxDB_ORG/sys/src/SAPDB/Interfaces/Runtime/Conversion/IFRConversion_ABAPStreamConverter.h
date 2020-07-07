/*!
  @file           IFRConversion_ABAPStreamConverter.h
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Implements a class for processing ABAP/OMS Streams.
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
#ifndef IFRCONVERSION_ABAPSTREAMCONVERTER_H
#define IFRCONVERSION_ABAPSTREAMCONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

/**
 * @ingroup IFR_DataConv
 * @brief Converter for stream data type used in DB Procedures.
 */
class IFRConversion_ABAPStreamConverter
    : public IFRConversion_Converter
{
public:
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_ABAPStreamConverter(IFR_ShortInfo& shortinfo,
                                  SAPDBMem_IRawAllocator& allocator);
    /**
     * @brief Destructor.
     */
    virtual ~IFRConversion_ABAPStreamConverter();

    virtual IFR_Retcode translateABAPStreamInput(IFRPacket_DataPart& datapart,
                                                 void               *streamDescriptor,
                                                 IFR_Length          datalength,
                                                 IFR_Length         *lengthindicator,
                                                 IFR_Int4            dataoffset,
                                                 IFR_ConnectionItem& clink);

    virtual IFR_Retcode putABAPStream(IFR_Parameter& parameter,
                                      IFRPacket_DataPart& datapart,
                                      void * maskptr,
                                      IFR_Int4 masklength,
                                      IFR_Int4 requiredrows,
                                      IFR_Bool& last,
                                      IFR_ConnectionItem& clink);

    virtual IFR_Retcode getABAPStream(IFR_Parameter& parameter,
                                      IFRPacket_DataPart& datapart,
                                      IFR_Int4 masklength,
                                      void * maskptr,
                                      IFR_Int4& abap_rc,
                                      IFR_ConnectionItem& clink);

};


#endif // IFRCONVERSION_ABAPSTREAMCONVERTER_H

