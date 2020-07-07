/*!
  @file           IFRConversion_ABAPStreamConverter.cpp
  @author         D039759
  @ingroup        IFRConversion_ABAPStreamConverter
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
#include "livecachetypes.h" /* nocheck */
#include "Interfaces/Runtime/Conversion/IFRConversion_ABAPStreamConverter.h"
#include "Interfaces/Runtime/IFR_Connection.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

#define IFR_DEBUG_STREAMCALL 0

//----------------------------------------------------------------------
IFRConversion_ABAPStreamConverter::IFRConversion_ABAPStreamConverter(IFR_ShortInfo &shortinfo,
                                                             SAPDBMem_IRawAllocator& allocator)
:IFRConversion_Converter(shortinfo, allocator)
{}

//----------------------------------------------------------------------
IFRConversion_ABAPStreamConverter::~IFRConversion_ABAPStreamConverter()
{}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ABAPStreamConverter::translateABAPStreamInput(IFRPacket_DataPart& datapart,
                                                            void               *streamDescriptor,
                                                            IFR_Length          datalength,
                                                            IFR_Length         *lengthindicator,
                                                            IFR_Int4            dataoffset,
                                                            IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_ABAPStreamConverter, translateABAPStreamInput, &clink);

    SQLStreamDesc *streamdesc = (SQLStreamDesc *)streamDescriptor;

    if(streamdesc == 0) {
        clink.error().setRuntimeError(IFR_ERR_NULL_STREAMHANDLE_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        if(streamdesc->StreamParam == 0) {
            clink.error().setRuntimeError(IFR_ERR_NULL_STREAMPARAM_I, (IFR_Int4)m_index);
            DBUG_RETURN(IFR_NOT_OK);
        } else if(streamdesc->ReadProc == 0 && streamdesc->WriteProc == 0) {
            clink.error().setRuntimeError(IFR_ERR_STREAM_NO_FUNCTION_I, (IFR_Int4)m_index);
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(streamdesc->StreamParam->Stream.hStream.memberCount != getPrecision()+1) {
            clink.error().setRuntimeError(IFR_ERR_STREAM_MEMBERCOUNT_MISMATCH_III,
                                          (IFR_Int4)m_index,
                                          (IFR_Int4)streamdesc->StreamParam->Stream.hStream.memberCount,
                                          (IFR_Int4)(getPrecision() + 1));
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    

    IFR_Retcode rc = datapart.addBinaryParameter((char *)streamdesc->StreamParam,
                                                 getIOLength() - 1,
                                                 m_shortinfo);
    if (rc == IFR_NOT_OK) {
        clink.error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, (IFR_Int4)this->m_index);
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ABAPStreamConverter::putABAPStream(IFR_Parameter& parameter,
                                                 IFRPacket_DataPart& datapart,
                                                 void               *maskptr,
                                                 IFR_Int4            masklength,
                                                 IFR_Int4            requiredrows,
                                                 IFR_Bool& last,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_ABAPStreamConverter, putABAPStream, &clink);
    SQLStreamDesc *streamdesc = (SQLStreamDesc *)parameter.asChar();
    if(streamdesc->ReadProc == 0) {
        clink.error().setRuntimeError(IFR_ERR_STREAMHANDLE_NOTINPUT_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(IFR_SQL_TRACE_ENABLED) {
        if(maskptr == 0) {
            IFR_SQL_TRACE << "MASK: NULL" << endl;
        } else {
            IFR_SQL_TRACE << "MASK: " << hex << inputlength(MIN(IFR_MAX_TRACELENGTH/2, masklength)) << maskptr;
            if(masklength > IFR_MAX_TRACELENGTH/2) {
                IFR_SQL_TRACE << "..." << endl;
            } else {
                IFR_SQL_TRACE << endl;
            }
        }
        IFR_SQL_TRACE << "REQUESTED ROWS: " << requiredrows;
        if(requiredrows == -2) {
            IFR_SQL_TRACE << "(STREAM RESET)" << endl;
        } else {
            IFR_SQL_TRACE << endl;
        }
    }

    IFR_Int4 remainingBytes = datapart.getRemainingBytes() ;
#if 0
     if(remainingBytes > 32767) {
         remainingBytes = 32767;
     }
#endif
    void    *dataptr        = (void *)datapart.GetReadData(0);
    int      streamlength   = 0;
    int      rowsread       = requiredrows;
    

    
    int abap_rc = streamdesc->ReadProc(streamdesc->StreamParam,
                                       dataptr,
                                       remainingBytes,
                                       &streamlength,
                                       &rowsread, 
                                       maskptr);

    if(IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE << "STREAM LENGTH: " << streamlength << endl
                      << "STREAM ROWS: " << rowsread << endl
                      << "CALLBACK RETURNED: ";
        
        switch(abap_rc) {
        case SQL_STREAM_NO_MORE_DATA: {
            IFR_SQL_TRACE << "NO MORE DATA" << endl;
            break;
        } 
        case SQL_STREAM_OK: {
            IFR_SQL_TRACE << "OK" << endl;
            break;
        }
        case SQL_STREAM_EXCEPTION: {
            IFR_SQL_TRACE << "ERROR" << endl;
            break;
        }
        default: {
            IFR_SQL_TRACE << "UNKNOWN ERROR(" << abap_rc << ")" << endl;
            break;
        }
        }
    }
    
    // If the function wrote too much, we flee.
    if(streamlength > remainingBytes) {
        clink.error().setRuntimeError(IFR_ERR_STREAM_OVERFLOW_III, 
                                      (IFR_Int4)streamdesc->StreamParam->Stream.hStream.ABAPTabId,
                                      (IFR_Int4)remainingBytes,
                                      (IFR_Int4)streamlength);
        
        DBUG_RETURN(IFR_NOT_OK);
    }

    switch(abap_rc) {
    case SQL_STREAM_NO_MORE_DATA:
        datapart.setLastPacket();
    case SQL_STREAM_OK:
        datapart.setArgCount(rowsread);
        datapart.setBufferLength(streamlength);
        DBUG_RETURN(IFR_OK);
    case SQL_STREAM_EXCEPTION:
    default:
        clink.error().setRuntimeError(IFR_ERR_STREAM_ERROR_INPUT_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }
    // if there is nothing inside it, finish the insert
    if(rowsread == 0) {
        last = true;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRConversion_ABAPStreamConverter::getABAPStream(IFR_Parameter& parameter,
                                                 IFRPacket_DataPart& datapart,
                                                 IFR_Int4 masklength,
                                                 void *maskptr,
                                                 IFR_Int4& abap_rc,
                                                 IFR_ConnectionItem& clink)
{
    DBUG_CONTEXT_METHOD_ENTER(IFRConversion_ABAPStreamConverter, putABAPStream, &clink);
    SQLStreamDesc *streamdesc = (SQLStreamDesc *)parameter.asChar();
    if(streamdesc->WriteProc == 0) {
        clink.error().setRuntimeError(IFR_ERR_STREAMHANDLE_NOTOUTPUT_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

    void *dataptr        = (void *)datapart.GetReadData(0);
    int   numberofrows   = datapart.getPartArguments();
    int   bufferlength   = datapart.getBufferLength();

#if IFR_DEBUG_STREAMCALL
    SAPDBMem_IRawAllocator& allocator = clink.getConnection()->allocator;
    unsigned char *x_dataptr = (unsigned char *)allocator.Allocate(32 * 5 +
                                                                   bufferlength 
                                                                   + 32 * 5);
    memset(x_dataptr, 'A', 32 * 5);
    memset(((char *)x_dataptr) + 32 * 5 + bufferlength,  'E', 32 * 5);
    memcpy(((char *)x_dataptr) + 32 * 5, dataptr, bufferlength);
#endif
    if(IFR_SQL_TRACE_ENABLED) {
        if(maskptr == 0) {
            IFR_SQL_TRACE << "MASK: NULL" << endl;
        } else {
            IFR_SQL_TRACE << "MASK: " <<  hex << inputlength(MIN(IFR_MAX_TRACELENGTH/2, masklength)) << maskptr;
            if(masklength > IFR_MAX_TRACELENGTH/2) {
                IFR_SQL_TRACE << "..." << endl;
            } else {
                IFR_SQL_TRACE << endl;
            }
        }
        IFR_SQL_TRACE << "STREAM ROWS: " << numberofrows << endl
                      << "STREAM LENGTH: " << bufferlength << endl;
        
    }
    // LIKE in vpr03sc: do not call the write procedure if the row count
    //                  is <= 0, but set
    if(numberofrows <= 0 && numberofrows != -2) {
        IFR_SQL_TRACE << "NO CALL TO WRITE PROC: (NO MORE DATA)" << endl;
        abap_rc = SQL_STREAM_NO_MORE_DATA;
        DBUG_RETURN(IFR_OK);
    }
    

    abap_rc = streamdesc->WriteProc(streamdesc->StreamParam,
#if IFR_DEBUG_STREAMCALL
                                    x_dataptr + 32*5,
#else
                                    dataptr,
#endif
                                    numberofrows,
                                    bufferlength,
                                    maskptr);

    if(IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE << "CALLBACK RETURNED: ";
        switch(abap_rc) {
        case SQL_STREAM_NO_MORE_DATA: {
            IFR_SQL_TRACE << "NO MORE DATA" << endl;
            break;
        } 
        case SQL_STREAM_OK: {
            IFR_SQL_TRACE << "OK" << endl;
            break;
        }
        case SQL_STREAM_EXCEPTION: {
            IFR_SQL_TRACE << "ERROR" << endl;
            break;
        }
        default: {
            IFR_SQL_TRACE << "UNKNOWN ERROR(" << abap_rc << ")" << endl;
            break;
        }
        }
    }
        
#if IFR_DEBUG_STREAMCALL
    for(int s1 = 0; s1 < 32 * 5; ++s1) {
        if(x_dataptr[s1] != 'A') {
            clink.error().setRuntimeError(IFR_ERR_STREAM_ERROR_SI, 
                                          "Callback function wrote into memory before input",
                                          (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    for(int s2 = 0; s2 < 32 * 5; ++s2) {
        if(x_dataptr[s2 + bufferlength + 32 * 5] != 'E') {
            clink.error().setRuntimeError(IFR_ERR_STREAM_ERROR_SI, 
                                          "Callback function wrote into memory after input",
                                          (IFR_Int4)getIndex());
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    
    allocator.Deallocate(x_dataptr);
#endif
        
    switch(abap_rc) {
    case SQL_STREAM_OK:
        DBUG_RETURN(IFR_OK);
    default:
        clink.error().setRuntimeError(IFR_ERR_STREAM_ERROR_OUTPUT_I, (IFR_Int4)this->m_index);
        DBUG_RETURN(IFR_NOT_OK);
    }

}
