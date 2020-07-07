/*!
  @file           IFRConversion_StreamConverter.cpp
  @author         D030044
  @ingroup        IFRConversion_StreamConverter
  @brief          Implements a class for processing long descriptors
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
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_StreamConverter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Putval.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/IFR_LOBHost.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_StreamConverter::IFRConversion_StreamConverter(IFR_ShortInfo &shortinfo,
                                                             IFR_Bool binary,
                                                             SAPDBMem_IRawAllocator& allocator,
                                                             IFR_Connection& connection)
:IFRConversion_Converter(shortinfo, allocator)
{
    m_bin2hex           = connection.odbcBinToHex() && shortinfo.isBinary();
    m_binary            = binary;
}

//----------------------------------------------------------------------
IFRConversion_StreamConverter::~IFRConversion_StreamConverter()
{}

//----------------------------------------------------------------------
IFRConversion_Putval *
IFRConversion_StreamConverter::createPutval(IFRPacket_DataPart& part, 
                                            IFR_Parameter& parameter, 
                                            IFR_ConnectionItem& clink,
                                            IFR_Int4 dataoffset,
                                            IFR_size_t rowsize)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, createPutval);

  IFR_Length *param_lengthindicator=parameter.getLengthIndicator(dataoffset);
  
  // Create no PUTVAL if it is clear the it is NULL or DEFAULT
  if(param_lengthindicator && 
     (*param_lengthindicator == IFR_NULL_DATA  ||
      *param_lengthindicator == IFR_DEFAULT_PARAM )) {
      DBUG_RETURN((IFRConversion_Putval*)0);
  }
  IFRConversion_Putval *p=new IFR_ALLOCATOR(m_name.getAllocator()) IFRConversion_Putval(getIndex(),
                                                                                        dataoffset + 1,
                                                                                        m_shortinfo,
                                                                                        parameter.getHostType(),
                                                                                        parameter.asChar(dataoffset, rowsize),
                                                                                        parameter.getBytesLength(),
                                                                                        parameter.getLengthIndicator(dataoffset),
                                                                                        clink,
                                                                                        m_bin2hex && !parameter.isBinary(),
                                                                                        part.getEncoding());
  
  if(p) {
      if(clink.error()) {
          IFRUtil_Delete(p, m_name.getAllocator());
          return 0;
      } else {
          p->putDescriptor (part);
      }
  } else {
      clink.error().setMemoryAllocationFailed();
  }
  DBUG_RETURN(p);
}

//----------------------------------------------------------------------
IFRConversion_Getval *
IFRConversion_StreamConverter::createGetval (IFRPacket_DataPart& part, 
                                             IFR_Parameter* parameter, 
                                             IFR_ConnectionItem& clink,
                                             IFR_Int4 row,
                                             IFR_Int4 dataoffset,
                                             IFR_size_t rowsize)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, createGetval);
    
    IFR_Length *param_lengthindicator=parameter ? parameter->getLengthIndicator (dataoffset) : 0;
    
    // look if it is null ...
    if(isNull(part, clink)) {
        if(param_lengthindicator) { 
            *param_lengthindicator = IFR_NULL_DATA; 
        }
        // ODBC wants a null terminator for NULL values, if possible at all.
        if(parameter && parameter->data()!=0 && parameter->shouldTerminated()) {
            switch(parameter->getHostType()) {
            case IFR_HOSTTYPE_ASCII:
            case IFR_HOSTTYPE_OMS_ASCII:
            case IFR_HOSTTYPE_UTF8:
                if(parameter->getBytesLength() >= 1) {
                    *(parameter->asChar(dataoffset, rowsize)) = '\0';
                }
                break;
            case IFR_HOSTTYPE_UCS2:
            case IFR_HOSTTYPE_UCS2_SWAPPED:
                if(parameter->getBytesLength() >= 2) {
                    char *b = parameter->asChar(dataoffset, rowsize);
                    *(b)   = '\0';
                    *(b+1) = '\0';
                }
                break;
            }
        }
        DBUG_RETURN((IFRConversion_Getval*)0);
    }
    
    IFRConversion_Getval *p = 
        new IFR_ALLOCATOR(clink.getConnection()->allocator) IFRConversion_Getval (getIndex(),
                                                                                  row,
                                                                                  getLength(),
                                                                                  getIOLength(),
                                                                                  getDefinedByte(),
                                                                                  getSQLType(),
                                                                                  clink,
                                                                                  m_bin2hex && (parameter && !parameter->isBinary()),
                                                                                  part.getEncoding());
    
    if(p == 0) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN((IFRConversion_Getval*)0);
    }
    
    p->updateDescriptor (part, getBufpos());
    
    DBUG_RETURN(p);
}

IFR_Retcode
IFRConversion_StreamConverter::translateBinaryInput(IFRPacket_DataPart& datapart,
                                                    char               *data,
                                                    IFR_Length            datalength,
                                                    IFR_Length*           lengthindicator,
                                                    IFR_ConnectionItem & clink)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateBinaryInput);   
  DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter
::translateAsciiInput(IFRPacket_DataPart& datapart,
                      char               *data,
                      IFR_Length            datalength,
                      IFR_Length*           lengthindicator,
                      IFR_Bool              terminate,
                      IFR_Bool              ascii7bit,
                      IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateAsciiInput);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUCS2Input(IFRPacket_DataPart& datapart,
                                                  char               *data,
                                                  IFR_Bool            swapped,
                                                  IFR_Length            datalength,
                                                  IFR_Length*           lengthindicator,
                                                  IFR_Bool              terminate,
                                                  IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateUCS2Input);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUTF8Input(IFRPacket_DataPart& datapart,
                                                  char               *data,
                                                  IFR_Length          datalength,
                                                  IFR_Length*         lengthindicator,
                                                  IFR_Bool            terminate,
                                                  IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateUTF8Input);
    DBUG_RETURN(IFR_OK);
}
//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendBinaryInput(IFRPacket_DataPart& datapart,
                                                 char               *data,
                                                 IFR_Length            datalength,
                                                 IFR_Length           *lengthindicator,
                                                 IFR_ConnectionItem &clink,
                                                 IFR_Length&         offset,
                                                 IFRConversion_Putval *putval)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendBinaryInput);
  DBUG_PRINT(datalength);
  DBUG_PRINT(lengthindicator);
  DBUG_PRINT(offset);
  DBUG_RETURN(putval->transferStream(datapart, data, datalength, lengthindicator, false, offset));
}
                                                 
//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendAsciiInput(IFRPacket_DataPart& datapart,
                                                char               *data,
                                                IFR_Length            datalength,
                                                IFR_Length           *lengthindicator,
                                                IFR_Bool           terminate,
                                                IFR_Bool           ascii7bit,
                                                IFR_ConnectionItem &clink,
                                                IFR_Length&         offset,
                                                IFRConversion_Putval *putval)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendAsciiInput);
  DBUG_PRINT(datalength);
  DBUG_PRINT(lengthindicator);
  DBUG_PRINT(offset);
  DBUG_RETURN(putval->transferStream(datapart, data, datalength, lengthindicator, ascii7bit, offset));
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendUCS2Input(IFRPacket_DataPart&  datapart,
                                               char                *data,
                                               IFR_Bool            swapped,
                                               IFR_Length           datalength,
                                               IFR_Length*          lengthindicator,
                                               IFR_Bool           terminate,
                                               IFR_ConnectionItem  &clink,
                                               IFR_Length&          offset,
                                               IFRConversion_Putval *putval)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendUCS2Input);
  DBUG_PRINT(datalength);
  DBUG_PRINT(lengthindicator);
  DBUG_PRINT(offset);
  DBUG_RETURN(putval->transferStream(datapart, data, datalength, lengthindicator, false, offset));
}
                                                 
//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendUTF8Input(IFRPacket_DataPart&  datapart,
                                               char                *data,
                                               IFR_Length           datalength,
                                               IFR_Length*          lengthindicator,
                                               IFR_Bool           terminate,
                                               IFR_ConnectionItem  &clink,
                                               IFR_Length&          offset,
                                               IFRConversion_Putval *putval)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendUTF8Input);
  DBUG_PRINT(datalength);
  DBUG_PRINT(lengthindicator);
  DBUG_PRINT(offset);
  DBUG_RETURN(putval->transferStream(datapart, data, datalength, lengthindicator, false, offset));
}


//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::translateAsciiOutput(IFRPacket_DataPart& datapart,
                                                    char               *data,
                                                    IFR_Length          datalength,
                                                    IFR_Length*         lengthindicator,
                                                    IFR_Bool            terminate,
                                                    IFR_Bool ascii7bit,
                                                    IFR_ConnectionItem &clink)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateAsciiOutput);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendAsciiOutput(IFRPacket_DataPart& datapart,
                                                 char               *data,
                                                 IFR_Length            datalength,
                                                 IFR_Length           *lengthindicator,
                                                 IFR_Bool            terminate,
                                                 IFR_Bool            ascii7bit,
                                                 IFR_ConnectionItem &clink,
                                                 IFR_Length&         dataoffset,
                                                 IFR_Length&         offset,
                                                 IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendAsciiOutput);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(dataoffset);
    DBUG_PRINT(offset);

  
    if(offset) {
        getval->setReadOffset(offset);
    }
    IFR_Retcode rc = getval->transferStream(datapart, 
                                            data, 
                                            datalength, 
                                            lengthindicator, 
                                            dataoffset, 
                                            IFR_StringEncodingAscii,
                                            terminate,
                                            ascii7bit);
    if(rc == IFR_OK || rc==IFR_DATA_TRUNC || rc == IFR_NO_DATA_FOUND ||  rc==IFR_NEED_DATA) {
        offset = getval->getReadOffset();
    } else {
        offset = 1;
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::translateBinaryOutput(IFRPacket_DataPart& datapart,
						     char               *data,
						     IFR_Length          datalength,
						     IFR_Length*         lengthindicator,
						     IFR_ConnectionItem &clink)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateBinaryOutput);
  DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_StreamConverter::appendBinaryOutput(IFRPacket_DataPart& datapart,
                                                 char               *data,
                                                 IFR_Length            datalength,
                                                 IFR_Length           *lengthindicator,
                                                 IFR_ConnectionItem &clink,
                                                 IFR_Length&         dataoffset,
                                                 IFR_Length&         offset,
                                                 IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendBinaryOutput);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(dataoffset);
    DBUG_PRINT(offset);
    if(offset) {
        getval->setReadOffset(offset);
    }
    IFR_Retcode rc = getval->transferStream(datapart, data, datalength, lengthindicator, 
                                            dataoffset, getval->getEncoding(), false, false);
    if(rc == IFR_OK || rc==IFR_DATA_TRUNC || rc == IFR_NO_DATA_FOUND ||  rc==IFR_NEED_DATA) {
        offset = getval->getReadOffset();
    } else {
        offset = 1;
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUCS2Output(IFRPacket_DataPart& datapart,
                                                   char           *data,
                                                   IFR_Bool        swapped,
                                                   IFR_Length        datalength,
                                                   IFR_Length*       lengthindicator,
                                                   IFR_Bool        terminate,
                                                   IFR_ConnectionItem &clink)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateUCS2Output);
  DBUG_RETURN(IFR_OK);  
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUTF8Output(IFRPacket_DataPart& datapart,
                                                   char           *data,
                                                   IFR_Length        datalength,
                                                   IFR_Length*       lengthindicator,
                                                   IFR_Bool        terminate,
                                                   IFR_ConnectionItem &clink)
{
  DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, translateUCS2Output);
  DBUG_RETURN(IFR_OK);  
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::appendUCS2Output(IFRPacket_DataPart& datapart,
                                                char               *data,
                                                IFR_Bool            swapped,
                                                IFR_Length            datalength,
                                                IFR_Length           *lengthindicator,
                                                IFR_Bool              terminate,
                                                IFR_ConnectionItem &clink,
                                                IFR_Length&         dataoffset,
                                                IFR_Length&         offset,
                                                IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendUCS2Output);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(offset);
    DBUG_PRINT(dataoffset);

    if(offset) {
        getval->setReadOffset(offset);
    }
    IFR_Retcode rc = getval->transferStream(datapart, data, datalength, lengthindicator, dataoffset, 
                                            swapped?IFR_StringEncodingUCS2Swapped:IFR_StringEncodingUCS2, terminate, false);  
    if(rc == IFR_OK || rc==IFR_DATA_TRUNC || rc == IFR_NO_DATA_FOUND ||  rc==IFR_NEED_DATA) {
        offset = getval->getReadOffset();
    } else {
        offset = 1;
    }
    DBUG_RETURN(rc);
}                                       

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::appendUTF8Output(IFRPacket_DataPart& datapart,
                             char           *data,
                             IFR_Length        datalength,
                             IFR_Length*       lengthindicator,
                             IFR_Bool          terminate,
                             IFR_ConnectionItem &clink,
                             IFR_Length&         dataoffset,
                             IFR_Length     &offset,
                             IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, appendUTF8Output);
    DBUG_PRINT(datalength);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(dataoffset);
    DBUG_PRINT(offset);
    if(offset) {
        getval->setReadOffset(offset);
    }
    IFR_Retcode rc = getval->transferStream(datapart, data, datalength, lengthindicator, dataoffset, 
                                            IFR_StringEncodingUTF8, terminate, false);
    if(rc == IFR_OK || rc==IFR_DATA_TRUNC || rc == IFR_NO_DATA_FOUND ||  rc==IFR_NEED_DATA) {
        offset = getval->getReadOffset();
    } else {
        offset = 1;
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_StreamConverter::translateBinaryLOBInput(IFRPacket_DataPart& datapart,
                                                       IFR_LOBData&     data,
                                                       IFR_Length*      lengthindicator,
                                                       IFR_Int4              dataoffset,
                                                       IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    if(lengthindicator && 
       (*lengthindicator == IFR_NULL_DATA ||
        *lengthindicator == IFR_DEFAULT_PARAM)) {
        
        data.lobdata=0;
        data.connectionitem = &clink;

    } else {
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1, 
                                                                        IFR_HOSTTYPE_BLOB,
                                                                        clink,
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_StreamConverter::translateAsciiLOBInput(IFRPacket_DataPart& datapart,
                                                 IFR_LOBData&     data,
                                                      IFR_Length*           lengthindicator,
                                                      IFR_Int4              dataoffset,
                                                      IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);
    if(lengthindicator && 
       (*lengthindicator == IFR_NULL_DATA ||
        *lengthindicator == IFR_DEFAULT_PARAM)) {
        
        data.lobdata=0;
        data.connectionitem = &clink;

    } else {
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1,
                                                                        IFR_HOSTTYPE_ASCII_LOB,
                                                                        clink,
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_StreamConverter::translateUCS2LOBInput(IFRPacket_DataPart& datapart,
                                                     IFR_LOBData&     data,
                                                     IFR_Bool      swapped,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_Int4              dataoffset,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_Converter, translateInput, &clink);

    if(lengthindicator && 
       (*lengthindicator == IFR_NULL_DATA ||
        *lengthindicator == IFR_DEFAULT_PARAM)) {
        
        data.lobdata=0;
        data.connectionitem = &clink;

    } else {
        
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1,
                                                                        swapped?IFR_HOSTTYPE_UCS2_SWAPPED_LOB:IFR_HOSTTYPE_UCS2_LOB,
                                                                        clink, 
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------.
IFR_Retcode
IFRConversion_StreamConverter::translateUTF8LOBInput(IFRPacket_DataPart& datapart,
                                                     IFR_LOBData&     data,
                                                     IFR_Length*           lengthindicator,
                                                     IFR_Int4              dataoffset,
                                                     IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateUTF8LOBInput, &clink);

    if(lengthindicator && 
       (*lengthindicator == IFR_NULL_DATA ||
        *lengthindicator == IFR_DEFAULT_PARAM)) {

        data.lobdata=0;
        data.connectionitem = &clink;

    } else {
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1,
                                                                        IFR_HOSTTYPE_UTF8_LOB,
                                                                        clink,
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateBinaryLOBOutput(IFRPacket_DataPart& datapart,
                                                        IFR_LOBData        &data,
                                                        IFR_Length*         lengthindicator,
                                                        IFR_Int4            dataoffset,
                                                        IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateBinaryLOBOutput, &clink);

    data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                    dataoffset + 1,
                                                                    IFR_HOSTTYPE_BLOB,
                                                                    clink,
                                                                    false);
    data.connectionitem = &clink;
    if(data.lobdata == 0) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok=true;
    clink.getGetvalHost()->addLOB(data.lobdata, memory_ok);
    if(!memory_ok) {
        IFRUtil_Delete(data.lobdata, m_name.getAllocator());
        data.lobdata = 0;
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateAsciiLOBOutput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData        &data,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4            dataoffset,
                                                IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateAsciiLOBOutput, &clink);

    data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                    dataoffset + 1,
                                                                    IFR_HOSTTYPE_ASCII_LOB,
                                                                    clink,
                                                                    false);
    data.connectionitem = &clink;
    if(data.lobdata == 0) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok=true;
    clink.getGetvalHost()->addLOB(data.lobdata, memory_ok);
    if(!memory_ok) {
        IFRUtil_Delete(data.lobdata, m_name.getAllocator());
        data.lobdata = 0;
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUCS2LOBOutput(IFRPacket_DataPart& datapart,
                                                      IFR_LOBData        &data,
                                                      IFR_Bool swapped,
                                                      IFR_Length*         lengthindicator,
                                                      IFR_Int4            dataoffset,
                                                      IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateUCS2LOBOutput, &clink);

    data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                    dataoffset + 1,
                                                                    swapped?IFR_HOSTTYPE_UCS2_SWAPPED_LOB:IFR_HOSTTYPE_UCS2_LOB,
                                                                    clink,
                                                                    false);
    data.connectionitem = &clink;
    if(data.lobdata == 0) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok=true;
    clink.getGetvalHost()->addLOB(data.lobdata, memory_ok);
    if(!memory_ok) {
        IFRUtil_Delete(data.lobdata, m_name.getAllocator());
        data.lobdata = 0;
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateUTF8LOBOutput(IFRPacket_DataPart& datapart,
                                                      IFR_LOBData        &data,
                                                      IFR_Length*         lengthindicator,
                                                      IFR_Int4            dataoffset,
                                                      IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateUTF8LOBOutput, &clink);

    data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                    dataoffset + 1,
                                                                    IFR_HOSTTYPE_UTF8_LOB,
                                                                    clink,
                                                                    false);
    data.connectionitem = &clink;
    if(data.lobdata == 0) {
        clink.error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok=true;
    clink.getGetvalHost()->addLOB(data.lobdata, memory_ok);
    if(!memory_ok) {
        IFRUtil_Delete(data.lobdata, m_name.getAllocator());
        data.lobdata = 0;
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}
    

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateNullInput(IFR_Parameter& parameter,
                                                  IFRPacket_DataPart& datapart,
                                                  IFR_ConnectionItem& clink,
                                                  IFR_Int4 dataoffset,
                                                  IFR_size_t rowsize)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateNullInput, &clink);
    
    IFR_Retcode rc = IFRConversion_Converter::translateNullInput(datapart, clink);
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }

    if(parameter.data() && parameter.isLOB()) {
        IFR_LOBData& data = parameter.asLOBData(dataoffset, rowsize);
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1, 
                                                                        parameter.getHostType(),
                                                                        clink,
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
        data.lobdata->setNull();
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::translateDefaultInput(IFR_Parameter& parameter,
                                                     IFRPacket_DataPart& datapart,
                                                     IFR_ConnectionItem& clink,
                                                     IFR_Int4 dataoffset,
                                                     IFR_size_t rowsize)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_StreamConverter, translateDefaultInput, &clink);
    
    IFR_Retcode rc = IFRConversion_Converter::translateDefaultInput(parameter, datapart, clink, dataoffset, rowsize);
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }
    if(parameter.data() && parameter.isLOB()) {
        IFR_LOBData& data = parameter.asLOBData(dataoffset, rowsize);
        data.lobdata = new IFR_ALLOCATOR(m_name.getAllocator()) IFR_LOB(getIndex(),
                                                                        dataoffset + 1,
                                                                        parameter.getHostType(),
                                                                        clink,
                                                                        true);
        data.connectionitem = &clink;
        if(data.lobdata == 0) {
            clink.error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool memory_ok=true;
        clink.getPutvalHost()->addLOB(data.lobdata, memory_ok);
        if(!memory_ok) {
            IFRUtil_Delete(data.lobdata, m_name.getAllocator());
            data.lobdata = 0;
            DBUG_RETURN(IFR_NOT_OK);
        }
        data.lobdata->setNull();
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRConversion_StreamConverter::getIntegralData(IFRPacket_DataPart& datapart,
                                               void *data,
                                               IFR_Length* lengthindicator,
                                               IFR_Parameter& parameter,
                                               IFR_Length& dataoffset,
                                               IFR_Length& offset,
                                               IFR_ConnectionItem& clink,
                                               IFR_Length&         datalength,
                                               IFRConversion_Getval *getval)
{
    DBUG_CLINK_METHOD_ENTER(IFRConversion_StreamConverter, getIntegralData);
    
    getval->setReadOffset(1);
    IFR_Retcode rc = getval->transferIntegral(datapart, parameter.getHostType(), data, lengthindicator);
    getval->setReadOffset(1);
    offset = 1; 
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
#define TRANSLATE(xxtype)                                               \
IFR_Retcode                                                             \
IFRConversion_StreamConverter::translateInput(IFRPacket_DataPart& datapart, \
                                              xxtype& data,             \
                                              IFR_Length *lengthindicator, \
                                              IFR_ConnectionItem& clink, \
                                              IFRConversion_Putval *pv) \
{                                                                       \
    if(m_binary) {                                                      \
         clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, \
                                       (IFR_Int4)this->m_index);        \
         return IFR_NOT_OK;                                             \
    }                                                                   \
    return IFR_OK;                                                      \
}                                                                       \
                                                                        \
IFR_Retcode                                                             \
IFRConversion_StreamConverter::translateOutput(IFRPacket_DataPart& datapart, \
                                               xxtype& data,            \
                                               IFR_Length *lengthindicator, \
                                               IFR_ConnectionItem& clink) \
{                                                                       \
    if(m_binary) {                                                      \
         clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, \
                                       (IFR_Int4)this->m_index);        \
         return IFR_NOT_OK;                                             \
    }                                                                   \
    return IFR_OK;                                                      \
}

TRANSLATE(IFR_Int1)
TRANSLATE(IFR_Int2)
TRANSLATE(IFR_Int4)
TRANSLATE(IFR_Int8)
TRANSLATE(IFR_UInt1)
TRANSLATE(IFR_UInt2)
TRANSLATE(IFR_UInt4)
TRANSLATE(IFR_UInt8)
TRANSLATE(double)
TRANSLATE(float)
TRANSLATE(SQL_DATE_STRUCT)
TRANSLATE(SQL_TIME_STRUCT)
TRANSLATE(SQL_TIMESTAMP_STRUCT)

#undef TRANSLATE
