/*!
  @file           IFRConversion_BooleanConverter.cpp
  @author         D039759
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
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_BooleanConverter.h"

#include <stdlib.h>
#include <errno.h>
#include <ctype.h>

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_BooleanConverter::IFRConversion_BooleanConverter(IFR_ShortInfo& shortinfo,
                                                               SAPDBMem_IRawAllocator& allocator)
:IFRConversion_Converter(shortinfo, allocator)
{}

//----------------------------------------------------------------------
IFRConversion_BooleanConverter::~IFRConversion_BooleanConverter()
{
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter
::translateBinaryInput(IFRPacket_DataPart& datapart,
                       char               *data,
                       IFR_Length            datalength,
                       IFR_Length*           lengthindicator,
                       IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateBinaryInput);
    IFR_Length offset = 0;
    DBUG_RETURN(appendBinaryInput(datapart, data, datalength, lengthindicator, clink, offset, 0));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter
::appendBinaryInput(IFRPacket_DataPart& datapart,
                    char               *data,
                    IFR_Length            datalength,
                    IFR_Length*           lengthindicator,
                    IFR_ConnectionItem & clink,
                    IFR_Length         &offset,
                    IFRConversion_Putval *putval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, appendBinaryInput);

    IFR_Length byteslength;
    if(IFRConversion_InputDataLength(datalength, lengthindicator, data, byteslength, true /* old behaviour */)) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_LENGTHINDICATOR_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(byteslength != 1) {
        clink.error().setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, (IFR_Int4)getIndex());
    }
    
    char putdata[1];
    if(data && *data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
    
}


//-------------------i---------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_Int1&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_UInt1&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_Int2&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_UInt2&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_Int4&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_UInt4&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_Int8&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               IFR_UInt8&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               double&           data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem & clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data != 0.0) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               float&              data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem &clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];
    if(data != 0.0f) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateInput(IFRPacket_DataPart& datapart,
                                               SQL_NUMERIC_STRUCT&   data,
                                               IFR_Length*           lengthindicator,
                                               IFR_ConnectionItem &clink,
                                               IFRConversion_Putval *pv)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateInput);
    char putdata[1];

    static char zero[SQL_MAX_NUMERIC_LENGTH]; // always initialized with 0 (static data)
    if(memcmp(data.val, zero, SQL_MAX_NUMERIC_LENGTH) != 0) {
        putdata[0]=1;
    } else {
        putdata[0]=0;
    }
    datapart.addBinaryParameter(putdata, 1, m_shortinfo);
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int1&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_Int1);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int2&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_Int2);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int4&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_Int4);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_Int8&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_Int8);
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt1&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_UInt1);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt2&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_UInt2);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt4&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_UInt4);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                IFR_UInt8&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1;
    } else {
        data=0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(IFR_UInt8);
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                double&             data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1.0;
    } else {
        data=0.0;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(double);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                float&           data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data=1.0f;
    } else {
        data=0.0f;
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(float);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateOutput(IFRPacket_DataPart& datapart,
                                                SQL_NUMERIC_STRUCT&   data,
                                                IFR_Length*           lengthindicator,
                                                IFR_ConnectionItem & clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateOutput);
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        data.precision = 1;
        data.scale     = 0;
        data.sign      = 1;
        memset(data.val, 0, SQL_MAX_NUMERIC_LENGTH);
        data.val[0] = 1;
    } else {
        data.precision = 1;
        data.scale     = 0;
        data.sign      = 1;
        memset(data.val, 0, SQL_MAX_NUMERIC_LENGTH);
    }
    if(lengthindicator) {
        *lengthindicator = sizeof(SQL_NUMERIC_STRUCT);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                                     char           *data,
                                                     IFR_Length        datalength,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_Bool        terminate,
                                                     IFR_Bool        ascii7bit,
                                                     IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateAsciiOutput);
    IFR_Retcode rc = IFR_OK;
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        // TRUE
        if(terminate) {
            if(datalength < 5) {
                memcpy(data, "TRUE", datalength);
                data[datalength -1] = '\0';
                rc = IFR_DATA_TRUNC;
            } else {
                memcpy(data, "TRUE", 5);
            }
        } else {
            if(datalength < 4) {
                memcpy(data, "TRUE", datalength);
                rc = IFR_DATA_TRUNC;
            } else {
                memcpy(data, "TRUE", 4);
            }
        }
        if(lengthindicator) {
            *lengthindicator = 4;
        }
    } else {
        // FALSE
        if(terminate) {
            if(datalength < 6) {
                memcpy(data, "FALSE", datalength);
                data[datalength -1] = '\0';
                rc = IFR_DATA_TRUNC;
            } else {
                memcpy(data, "FALSE", 6);
            }
        } else {
            if(datalength < 5) {
                memcpy(data, "FALSE", datalength);
                rc = IFR_DATA_TRUNC;
            } else {
                memcpy(data, "FALSE", 5);
            }
        }
        if(lengthindicator) {
            *lengthindicator = 5;
        }
    }
    DBUG_RETURN(rc);
}

IFR_Retcode 
IFRConversion_BooleanConverter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                                    char           *data,
                                                    IFR_Bool        swapped,
                                                    IFR_Length        datalength,
                                                    IFR_Length*       lengthindicator,
                                                    IFR_Bool        terminate,
                                                    IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateUCS2Output);
    char true_ucs2 [] = { 'T', '\0',
                          'R', '\0',
                          'U', '\0',
                          'E', '\0'
    };
    char true_ucs2_swapped [] = { '\0', 'T',
                                  '\0', 'R',
                                  '\0', 'U',
                                  '\0', 'E'
    };
    char false_ucs2 [] = { 'F', '\0',
                           'A', '\0',
                           'L', '\0',
                           'S', '\0',
                           'E', '\0'
    };
    char false_ucs2_swapped [] = { '\0', 'F',
                                   '\0', 'A',
                                   '\0', 'L',
                                   '\0', 'S',
                                   '\0', 'E'
    };
    char *resultdata;
    IFR_Length resultlength;
    if(((unsigned char *)datapart.getOutputData(m_shortinfo.pos.bufpos))[0]) {
        resultlength = 4;
        resultdata = swapped ? true_ucs2_swapped : true_ucs2;
    } else {
        resultlength = 5;
        resultdata = swapped ? false_ucs2_swapped : false_ucs2;
    }
    IFR_Retcode rc = IFR_OK;
    if(terminate) {
        if(datalength < (resultlength * 2) + 2) {
            memcpy(data, resultdata, (datalength / 2) * 2);
            data[((datalength / 2) * 2) - 2 ] = 0;
            data[((datalength / 2) * 2) - 1 ] = 0;
            rc = IFR_DATA_TRUNC;
        } else {
            memcpy(data, resultdata, resultlength * 2);
            data[ (resultlength * 2)    ] = 0;
            data[ (resultlength * 2) + 1] = 0;
        }
    } else {
        if(datalength < (resultlength * 2)) {
            memcpy(data, resultdata, (datalength / 2) * 2);
            rc = IFR_DATA_TRUNC;
        } else {
            memcpy(data, resultdata, resultlength * 2);
        }
    }
    if(lengthindicator) {
        *lengthindicator = (resultlength * 2);
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                                    char           *data,
                                                    IFR_Length        datalength,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_Bool        terminate,
                                                    IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, translateUTF8Output);
    DBUG_RETURN(translateAsciiOutput(datapart, data, datalength, lengthindicator, terminate, false, clink));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_BooleanConverter::translateBinaryOutput(IFRPacket_DataPart& datapart,
                                                      char *data,
                                                      IFR_Length datalength,
                                                      IFR_Length *lengthindicator,
                                                      IFR_ConnectionItem& clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_NumericConverter, translateBinaryOutput);
    IFR_Length  offset    = 1;
    IFR_Length  dataoffset= 0;
    DBUG_RETURN(appendBinaryOutput(datapart, data, datalength, lengthindicator, clink, dataoffset, offset, 0));
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_BooleanConverter::appendBinaryOutput(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length*         lengthindicator,
                                                   IFR_ConnectionItem& clink,
                                                   IFR_Length         &dataoffset,
                                                   IFR_Length         &offset,
                                                   IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_BooleanConverter, appendBinaryOutput);
    
    if(dataoffset != 0 || offset != 1) {
        clink.error().setRuntimeError(IFR_ERR_BINARY_APPEND_I, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    }

    char       *read_data   = datapart.getOutputData(m_shortinfo.pos.bufpos);
    IFR_Length  byteslength = 1;
    
    if(lengthindicator) {
        *lengthindicator = byteslength;
    }
    
    if(datalength < byteslength) {
        char buffer[40];
        m_shortinfo.toString(buffer, sizeof(buffer));
        clink.error().setRuntimeError(IFR_ERR_BINARY_TRUNCATION_SI, buffer, (IFR_Int4)getIndex());
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        memcpy(data, read_data, byteslength);
        DBUG_RETURN(IFR_OK);
    }    
}




