/*!
  @file           IFR_PreparedStmt.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Statement
  @brief          Implements a class for prepared statement processing
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

// Prevents a Warning 5004 Uninitialized variable "static_i"
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif

#include "Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_PreparedStmt.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_PartEnum.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"
#include "SAPDB/Interfaces/Runtime/IFR_ParseInfoCache.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_AsyncOperation.h"

IFR_BEGIN_NAMESPACE

#define PARSE_RETRY 10

class IFR_BatchFillOperation
    : public IFRUtil_AsyncOperation
{
public:
    IFR_BatchFillOperation(IFR_PreparedStmt *ps,
                           IFR_Int4 _inputcursor,
                           IFRPacket_RequestPacket& _packet,
                           IFRPacket_RequestSegment& _segment,
                           IFR_Int4 _paramcount,
                           IFR_Bool _masscommand,
                           IFR_UInt4 _recordsize)
    :statement(ps),
     error(ps->allocator, ps->getConnection()),
     inputcursor(_inputcursor),
     paramcount(_paramcount),
     masscommand(_masscommand),
     recordsize(_recordsize),
     packet(*ps),
     snd_packet(&_packet),
     snd_segment(&_segment),
     rc(IFR_OK)
    {
        op_packet = &packet;
        op_segment = &segment;
    }

    virtual ~IFR_BatchFillOperation()
    {}

    virtual void run()
    {
        DBUG_CONTEXT_METHOD_ENTER(IFR_BatchFillOperation, run, statement);

        rc = IFR_OK;
        // we may have catched the last packet, then the
        // inputcursor is already
        if(inputcursor < statement->m_rowarraysize) {

            IFR_SQL_TRACE << "::FILL NEXT BUFFER" << endl;

            firstrecord = inputcursor;
            rc = statement->executeBatchInitPacket(*op_packet, *op_segment, datapart, 0, parseidpart, resultcountpart, error);
            if(rc == IFR_OK) {
                if(paramcount != 0) {
                    last_inputcursor = inputcursor;
                    rc = statement->executeBatchFillData(*op_packet,
                                                         *op_segment,
                                                         datapart,
                                                         inputcursor,
                                                         firstrecord,
                                                         recordsize,
                                                         masscommand,
                                                         0,
                                                         0,
                                                         error);
                    op_segment->closePart();
                    datalength = datapart.getBufferLength();
                } else {
                    ++inputcursor;
                }
            }
        }

    }

    virtual inline void reset()
    {
        IFRUtil_AsyncOperation::reset();
    }

    inline void reset(IFR_Int4 _inputcursor)
    {
        reset();
        inputcursor = _inputcursor;
        if(snd_packet == &packet) {
            IFRUtil_Swap(snd_packet, op_packet);
            IFRUtil_Swap(snd_segment, op_segment);
        }
    }


    inline void switchPackets()
    {
        IFRUtil_Swap(op_packet, snd_packet);
        IFRUtil_Swap(op_segment, snd_segment);
        reset();
    }

    inline void setParseID(IFR_ParseID& parseid)
    {
        if(parseidpart.IsValid()) {
            parseidpart.setParseID(parseid);
        }
        return;
    }

    inline void setResultCount(IFR_Int4 resultcount)
    {
        if(resultcountpart.IsValid()) {
            if(resultcount == -1) {
                resultcountpart.setUndefResultCount();
            } else {
                resultcountpart.setResultCount(resultcount);
            }
        }
        return;
    }

    IFR_PreparedStmt *statement;
    IFR_ErrorHndl     error;
    IFR_Int4          inputcursor;
    IFR_Int4          firstrecord;
    IFR_Int4          datalength;
    IFR_Int4          paramcount;
    IFR_Bool          masscommand;
    IFR_UInt4         recordsize;

    IFRPacket_RequestPacket packet;
    IFRPacket_RequestSegment segment;
    IFRPacket_LongDataPart   datapart;

    IFRPacket_ParseIDPart      parseidpart;
    IFRPacket_ResultCountPart  resultcountpart;

    IFRPacket_RequestPacket  *op_packet;
    IFRPacket_RequestSegment *op_segment;

    IFRPacket_RequestPacket  *snd_packet;
    IFRPacket_RequestSegment *snd_segment;

    // work data, not initialized here
    IFR_Retcode rc;
    IFR_Int4    last_inputcursor;
};

//----------------------------------------------------------------------
class ClearCommandInfo
{
public:
    inline ClearCommandInfo(IFR_String* acmd, IFR_Int4& cmd_status)
    :m_info(acmd),
     m_cmdstatus(cmd_status)
    {
        switch(cmd_status) {
        case IFR_Statement::CommandInfoSet_C:
        case IFR_Statement::CommandInfoNotSet_C:
            break;
        default: {
            cmd_status = IFR_Statement::CommandInfoNotSet_C;
            m_info->clear();
        }
        }

    }

    inline ~ClearCommandInfo()
    {
        switch(m_cmdstatus) {
        case IFR_Statement::CommandInfoPrepared_C:
            break;
        default:
            m_cmdstatus = IFR_Statement::CommandInfoNotSet_C;
            m_info->clear();
        }
    }
private:
    IFR_String* m_info;
    IFR_Int4    &m_cmdstatus;
};

/**
 * Descriptor for <code>DATA AT EXECUTE</code> values
 * for one row.
 */
class IFR_DataAtExecuteDescriptor
{
public:
    IFR_DataAtExecuteDescriptor(IFRUtil_RuntimeItem& runtimeitem)
    :lateparams(runtimeitem.allocator),
     requestpacket(runtimeitem),
     m_index(-1),
     m_numberintegrals(0),
     currentreadoffset(0),
     nullordefault(false),
     integralfinished(false),
     putdatasuccess(false),
     isexecute(true),
     streamstruncated(false),
     putvalstreamtruncated(false),
     inputcursor(0),
     firstrecord(0),
     executecount(0),
     lastputval(0)
    {}

    IFR_DataAtExecuteDescriptor(IFRPacket_RequestPacket& packet,
                                IFRPacket_RequestSegment& _segment,
                                IFRPacket_DataPart& part,
                                IFR_Int4 _inputcursor,
                                IFR_Int4 _firstrecord,
                                IFR_Int4 _executecount,
                                SAPDBMem_IRawAllocator& allocator)
    :lateparams(allocator),
     m_index(-1),
     m_numberintegrals(0),
     currentreadoffset(0),
     nullordefault(false),
     integralfinished(false),
     putdatasuccess(false),
     isexecute(true),
     streamstruncated(false),
     putvalstreamtruncated(false),
     inputcursor(_inputcursor),
     requestpacket(packet),
     segment(_segment),
     datapart(part),
     firstrecord(_firstrecord),
     executecount(_executecount),
     lastputval(0)
    {
        // adjust segment reference.
        segment.setRequestPacket(requestpacket);
    }

    inline ~IFR_DataAtExecuteDescriptor()
    {
        // printf("DESTROY DESCRIPTOR\n");
    }


    void addParameter(IFR_Int2 index, IFR_Bool& memory_ok)
    {
        if(memory_ok) {
            IFRUtil_Vector<IFR_Int2>::Iterator p = lateparams.Begin() + m_numberintegrals;
            lateparams.Insert(p, index, memory_ok);
            if(memory_ok) {
                m_numberintegrals++;
            }
        }
    }

    void addLongParameter(IFR_Int2 index, IFR_Bool& memory_ok)
    {
        lateparams.InsertEnd(index, memory_ok);
    }

private:
    /**
     * Switch to the next 'DATA AT EXECUTE' parameter.
     * @return <code>true</code> if the parameter was switched,
     *         <code>false</code> if the parameter was not switched,
     *         because there is none remaining.
     */
    IFR_Bool next()
    {
        if(m_index +1 >= (IFR_Int2)lateparams.GetSize()) {
            return false;
        }

        currentreadoffset     = 0;
        nullordefault         = false;
        integralfinished      = false;
        putdatasuccess        = false;
        putvalstreamtruncated = false;
        sizeputted            = 0;
        lastputval            = 0;

        m_index++;

        return true;
    }

public:
    /**
     * Switches to a next parameter that is defined by the application.
     */
    IFR_Retcode next(IFR_Int2 nextindex, IFR_Bool variableinput, IFR_ErrorHndl& error)
    {
        if(nextindex != 0) {
            nextindex--; // from here, the indexes are managed 0-based
            IFR_Int4 lateparams_size=lateparams.GetSize();

            if(m_index +1 >= (IFR_Int2)lateparams_size) {
                return IFR_NO_DATA_FOUND;
            }
            if(nextindex < 0) {
                error.setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX_I, (IFR_Int4)nextindex);
                return IFR_NOT_OK;
            }
            // find the parameter
            IFR_Int4 parindex=0;
            for(; parindex <= lateparams_size; ++parindex) {
                if(parindex == lateparams_size) {
                    error.setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX_I, (IFR_Int4)nextindex);
                    return IFR_NOT_OK;
                } else if(lateparams[parindex] == nextindex) {
                    break;
                }
            }
            if(parindex <= m_index) {
                error.setRuntimeError(IFR_ERR_PARAMETER_ALREADY_PROCESSED_I, (IFR_Int4)nextindex);
                return IFR_NOT_OK;
            }
            // if we come up to here, we must look wheter it's the only one.
            if(lateparams_size != 1) {
                if(!variableinput) {
                    if(m_index < m_numberintegrals &&
                       parindex >= m_numberintegrals) {
                        error.setRuntimeError(IFR_ERR_INVALID_PARAMETER_SEQUENCE_I, (IFR_Int4)nextindex);
                        return IFR_NOT_OK;
                    }
                    IFR_Int2 tmp = lateparams[m_index+1];
                    lateparams[m_index+1]=lateparams[parindex];
                    lateparams[parindex]=tmp;
                } else {
                    if(parindex != m_index + 1) {
                        error.setRuntimeError(IFR_ERR_INVALID_PARAMETER_SEQUENCE_I, (IFR_Int4)nextindex);
                        return IFR_NOT_OK;
                    }
                }
            }
        }
        if(next()) {
            return IFR_OK;
        } else {
            return IFR_NO_DATA_FOUND;
        }
    }
public:
    /**
     * Get the parameter index of the parameter currently handled.
     */
    inline IFR_Int2 index()
    {
        return lateparams[m_index];
    }

    /**
     * Check whether this parameter is the last 'DATA AT EXECUTE' bound
     * parameter of this row.
     * @return <code>true</code> if it is the last, <code>false</code> if not.
     */
    inline IFR_Bool isLast()
    {
        return m_index + 1 == lateparams.GetSize();
    }


public:
    IFRPacket_RequestPacket  requestpacket;   //!< Packet on which the current putData sequence operates.
    IFRPacket_RequestSegment segment;         //!< Segment on which the current putData sequence operates.

    IFRPacket_LongDataPart   datapart;        //!< Part on which the current putData sequence operates.
    IFR_Bool                 isexecute;       //!< Is the command in the request packet an execute or a PUTVAL?

    IFRUtil_Vector<IFR_Int2> lateparams;      //!< Vector of parameter indices of all DATA AT EXECUTE parameters of the current row.
    IFR_Int2                 m_index;                //!< Current index within <code>lateparams</code>.
    IFR_Int2                 m_numberintegrals;      //!< Number of non-long parameters.

    IFR_Length               currentreadoffset;      //!< Offset WITHIN the current packet.
    IFR_Bool                 nullordefault;          //!< Flag: NULL or DEFAULT value inserted?
    IFR_Bool                 integralfinished;       //!< Flag: putData operation finished for integral type?
    IFR_Bool                 putdatasuccess;         //!< Flag: putData operation took place and was successfully?
    IFR_Bool                 streamstruncated;       //!< Flag: execute was done, but there are pending - non 'data at execute'
                                                     //!< PUTVAL operations.
    IFR_Bool                 putvalstreamtruncated;  //!< Flag: Was the LONG transferred using a PUTVAL with more than one
                                                     //!< packet - (needed to decide on all data or last data flag on close).
    IFR_Length               sizeputted;             //!< Total size that is yet transferred.

    IFR_Int4                 inputcursor;            //!< Index of the current row in the packet (mass commands).
    IFR_Int4                 firstrecord;            //!< Index of the first row in the packet (mass commands).
    IFR_Int4                 executecount;           //!< Row count as managed by the kernel, and inserted into the
                                                     //!< mass commands.
    IFRConversion_Putval    *lastputval;             //!< The last PUTVAL object that was in use.
};

//----------------------------------------------------------------------
IFR_PreparedStmt::IFR_PreparedStmt (IFR_Connection &conn, IFR_Bool &memory_ok)
:IFR_Statement(conn, memory_ok),
 IFR_LOBHost(conn.allocator),    // accessing the 'allocator' member at this time has proven as unsafe.
 IFR_GetvalHost(&conn, conn.allocator),
 IFR_PutvalHost(conn.allocator),
 m_parseinfo(0),
 m_paramvector(conn.allocator),
 m_param_info_hash(-1),
 m_bindingtype(0),
 m_status(Status_Other_C),
 m_paramdata(0),
 m_lastgetobjcol(0),
 m_offset(1),
 m_copieddata(0)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, IFR_PreparedStmt);
}

//----------------------------------------------------------------------
IFR_PreparedStmt::~IFR_PreparedStmt ()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, ~IFR_PreparedStmt);
    IFRUtil_Delete(m_parseinfo, allocator);
    IFRUtil_Delete(m_copieddata, allocator);
    clearParamData();
    clearInputLongs();
    closeOutputLongs();
 }

//----------------------------------------------------------------------
IFR_PutvalHost *
IFR_PreparedStmt::getPutvalHost()
{
    return this;
}


//----------------------------------------------------------------------
IFR_GetvalHost *
IFR_PreparedStmt::getGetvalHost()
{
    return this;
}


//----------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::addBatch(const char *sql, const IFR_Length sqlLength, const IFR_StringEncoding encoding){
    DBUG_METHOD_ENTER(IFR_PreparedStmt, addBatch);
    error().setRuntimeError(IFR_ERR_METHOD_NOT_ALLOWED, "addBatch(...)", "PreparedStatement");
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::addBatch(const char *sql, const IFR_StringEncoding encoding){
    DBUG_METHOD_ENTER(IFR_PreparedStmt, addBatch);
    error().setRuntimeError(IFR_ERR_METHOD_NOT_ALLOWED, "addBatch(...)", "PreparedStatement");
    DBUG_RETURN(IFR_NOT_OK);
}
//----------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::clearBatch(){
    DBUG_METHOD_ENTER(IFR_PreparedStmt, addBatch);
    error().setRuntimeError(IFR_ERR_METHOD_NOT_ALLOWED, "clearBatch()", "PreparedStatement");
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::clearParameters(IFR_Bool unbind, IFR_Bool clearLobs)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, clearParameters);
    switch(m_status) {
    case (Status_Other_C):
    case (Status_Keep_C):
    case (Status_LobsClosed_C):
        if(unbind) {
            m_paramvector.Clear();
        }
        if(clearLobs) {
            clearLOBs();
        }
        clearInputLongs();
        closeOutputLongs();
        clearError();
        m_status = Status_Other_C; // we must not pretend we are in
                                   // 'keep' mode any longer.
        DBUG_RETURN(IFR_OK);
    default:
        error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::bindParameter (IFR_UInt2 paramindex,
                                 IFR_HostType paramtype,
                                 void *paramaddr,
                                 IFR_Length *paramlengthindicator,
                                 IFR_Length parambytelength,
                                 IFR_Bool terminate)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, bindParameter);
    DBUG_PRINT(paramindex);
    DBUG_PRINT(paramtype);
    DBUG_PRINT(paramaddr);
    DBUG_PRINT(paramlengthindicator);
    DBUG_PRINT(parambytelength);
    DBUG_PRINT(terminate);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::BIND PARAM " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY
    IFR_Parameter parameter(paramtype,
                            terminate,
                            paramaddr,
                            paramlengthindicator,
                            parambytelength);
    //>>> SQL TRACE
    if (IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << IFR_SQLTRACE_PARAMETER_HEADER << endl;
        parameter.sqlTraceParameter(IFR_TRACE_STREAM, paramindex);
        IFR_TRACE_STREAM << endl;
    }
    //<<< SQL TRACE

    if(m_status != Status_Other_C) {
        error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(paramindex == 0) {
        error().setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(paramaddr == 0 && paramlengthindicator == 0) {
        error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)paramindex);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(parambytelength < 0) {
        error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, (IFR_Int4)paramindex, (IFR_Int4)parambytelength);
        DBUG_RETURN(IFR_NOT_OK);
    }

    error().clear();
    IFR_Bool memory_ok=true;
    if(m_paramvector.GetSize() < paramindex) {
        m_paramvector.Resize(paramindex, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    m_paramvector[paramindex-1] = parameter;
    m_param_info_hash = -1;
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::bindParameterAddr (IFR_UInt2 paramindex,
                                     IFR_HostType paramtype,
                                     void *paramaddr,
                                     IFR_Length *paramlengthindicator,
                                     IFR_Length parambytelength,
                                     IFR_Bool terminate)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, bindParameterAddr);
    DBUG_PRINT(paramindex);
    DBUG_PRINT(paramtype);
    DBUG_PRINT(paramaddr);
    DBUG_PRINT(paramlengthindicator);
    DBUG_PRINT(parambytelength);
    DBUG_PRINT(terminate);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::BIND PARAMADDR " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    IFR_Parameter parameter(paramtype,
                            terminate,
                            paramaddr,
                            paramlengthindicator,
                            parambytelength,
                            0,
                            true);
    //>>> SQL TRACE

    if (IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << IFR_SQLTRACE_PARAMETER_HEADER << endl;
        parameter.sqlTraceParameter(IFR_TRACE_STREAM, paramindex);
        IFR_TRACE_STREAM << endl;
    }
    //<<< SQL TRACE


    if(m_status != Status_Other_C) {
        error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(paramindex == 0) {
        error().setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(paramaddr == 0 && paramlengthindicator == 0) {
        error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)paramindex);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(parambytelength < 0) {
        error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, (IFR_Int4)paramindex, (IFR_Length)parambytelength);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok = true;
    if(m_paramvector.GetSize() < paramindex) {
        m_paramvector.Resize(paramindex, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    m_paramvector[paramindex-1]=parameter;
    m_param_info_hash = -1;
    clearError();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::setBindingType(IFR_size_t size)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, setBindingType);
    DBUG_PRINT(size);
    clearError();
    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::SET BINDING TYPE " << currenttime
                  << " [0x" << (void*)this << "]" << endl
                  << "BINDING TYPE: " <<  size << endl;
    //<<< SQL TRACE ENTRY
    m_bindingtype = size;
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_UInt4
IFR_PreparedStmt::getPreferredRowArraySize()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getPreferredRowArraySize);
    // not prepared ???
    if(m_parseinfo == 0) {
        DBUG_RETURN(4294967295U);
    }
    IFR_UInt4 result = (IFR_UInt4) m_Connection->getPacketSize();
    result -= (result % 8);                  // alignment
    result -= sizeof(tsp1_packet_header);    // packet
    result -= sizeof(tsp1_segment_header);   // segment
    result -= sizeof(tsp1_part_header);      // 1st part (parseid)
    result -= 12;                            // parse id
    result -= sizeof(tsp1_part_header);      // 2nd part (result count)
    result -= 7;                             // result count
    result -= sizeof(tsp1_part_header);      // 3rd part (data)
    result /= m_parseinfo->getInputParameterRowSize();
    DBUG_RETURN(result);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::doParseAgain(ColumnChange columnChange,
                               IFR_Bool parseagain)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, doParseAgain);
    // precondition: The command was prepared before.
    if(m_parseinfo==0) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NOTPREPARED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    m_parseinfo->lock();

    // as it was invalid before, it must be invalidated.
    m_parseinfo->invalidate();

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::PARSE " << m_CursorName << " "
                  << currenttime << " [0x" << (void*)this << "]" << endl
                  << "SQL COMMAND:" << m_parseinfo->getSQLCommand() << endl;
    IFR_SQL_TRACE_IF((m_parseinfo->getSQLCommand().getEncoding() != IFR_StringEncodingAscii))
      << "ENCODING : " << m_parseinfo->getSQLCommand().getEncoding() << endl;
    //<<< SQL TRACE ENTRY

    IFRPacket_ReplyPacket replypacket;
    IFR_Retcode rc;

    // So a parse again will add silently the needed data.
    rc=sendSQL(m_parseinfo->getSQLCommand(),
               replypacket,
               m_parseinfo->getFunctionCode().isMassCommand(),
               true,
               parseagain,
               IFR_Connection::AppendAllowed_C); // do a reparse
    if(rc != IFR_OK) {
        m_parseinfo->unlock();
        DBUG_RETURN(rc);
    }

    IFRPacket_ReplySegment segment = replypacket.GetFirstSegment();
    segment.setEncoding(replypacket.getEncoding());

    if(segment.getSQLError(error(), allocator)) {
        m_parseinfo->unlock();
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_PartEnum partEnum(segment);

    // THE REPLY PACKET IS UNLOCKED AFTER CALLING THIS METHOD AND MUST
    // NOT BE USED AGAIN IN THIS METHOD !!!
    rc=this->handleParseResult(replypacket, segment);

    // Memory problem while parsing result info.
    if(rc == IFR_NOT_OK) {
        m_parseinfo->unlock();
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(m_parseinfo->hasChangedFields()) {
        if(columnChange == ColumnChange_Forbidden_C) {
            error().setRuntimeError(IFR_ERR_SCHEMAMODIFIED);
            m_parseinfo->unlock();
            DBUG_RETURN(IFR_NOT_OK);
        }
    } else if(columnChange == ColumnChange_Required_C) {
        // normal exit, but set return code so that
        // it can be seen that nothing has been changed
        rc = IFR_NOT_OK;
    }

    m_parseinfo->unlock();
    //<<< SQL TRACE
    IFR_SQL_TRACE << "PARSE ID: " << *m_parseinfo << endl;
    //>>> SQL TRACE
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::doParse(const IFR_String &sql)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, doParse);

    // The prepare commando shall ignore what is currently engaged, and
    // if necessary, it will close preceding operations

    IFR_Bool memory_ok=true;

    if (sql.getLength () == 0) {
        error().setRuntimeError(IFR_ERR_EMPTY_SQL_STATEMENT);
        DBUG_RETURN(IFR_NOT_OK);
    }

    // Get rid of data belonging to the previous execute, if
    // any.
    if(m_parseinfo) {
        IFRUtil_Delete(m_parseinfo, allocator);
        m_parseinfo = 0;
    }

    if(m_copieddata) {
        IFRUtil_Delete(m_copieddata, allocator);
        m_copieddata = 0;
    }

    clearLOBs();
    clearInputLongs();
    clearOutputLongs();


    // Try to get the parse info from the cache. If there have been parameters bound,
    // we will not fetch the parse info from the cache, because the 'appl_parameter_description'
    // part that will be sent during the parse may differ. Also if a command info was set,
    // we will not fetch the parse info from the cache, because we have to send an abapinfo part.
    IFR_ParseInfoCache *pic=m_Connection->getParseInfoCache();
    IFR_Bool            parameternotmatch = false;
    if(pic!=0 && (m_commandInfo.getLength() == 0)
       && (m_Connection->getCacheAllStatements() || m_paramvector.GetSize() == 0)) {
        m_parseinfo=pic->getParseInfo(sql, m_Connection->getTransactionIsolation(), getParameterInfoHash(), parameternotmatch);
        if (m_parseinfo) {
            IFR_STMT_PROFILE_COUNT_INC(IFR_PARSEINFOHITCOUNT);
            unsafeSetCommandEncoding(m_parseinfo->getCommandEncoding());
            IFR_SQL_TRACE << "CACHED PARSE ID: " << *m_parseinfo << endl;
        }else{
            IFR_STMT_PROFILE_COUNT_INC(IFR_PARSINFOMISSCOUNT);
        }
    }

    if(m_parseinfo == 0) {
        IFRPacket_ReplyPacket replyPacket;
        IFR_Retcode rc;

        rc = this->sendSQL (sql, replyPacket, parseAsMassCommand(sql), true, false, IFR_Connection::AppendAllowed_C);
        if(rc != IFR_OK) {
            DBUG_RETURN(rc);
        }
        
        IFRPacket_ReplySegment segment = replyPacket.GetFirstSegment();
        segment.setEncoding(replyPacket.getEncoding());

        if(segment.getSQLError(error(), allocator)) {
            DBUG_RETURN(IFR_NOT_OK);
        }

        m_parseinfo = new IFR_ALLOCATOR(allocator) IFR_ParseInfo(sql,
                                                                 m_Connection->getTransactionIsolation(),
                                                                 segment.FunctionCode(),
                                                                 m_Connection,
                                                                 getCommandEncoding(),
                                                                 getParameterInfoHash(),
                                                                 memory_ok);
        if(m_parseinfo == 0) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        } else if(!memory_ok) {
            IFRUtil_Delete(m_parseinfo, allocator);
            m_parseinfo=0;
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }

        // THE REPLY PACKET IS UNLOCKED AFTER THIS AND MUST NOT BE USED
        // AGAIN.
        rc = handleParseResult(replyPacket, segment);

        // memory allocation failed
        if(rc == IFR_NOT_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }

        // Again, we add it only if there were no application info needed, or, we should
        // cache all statements and the parameter hash did not not match
        if(pic && ((m_Connection->getCacheAllStatements() && !parameternotmatch)|| (m_paramvector.GetSize() == 0))) {
            pic->addParseInfo(m_parseinfo, memory_ok);
            if(!memory_ok) {
                IFRUtil_Delete(m_parseinfo, allocator);
                m_parseinfo=0;
                error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        if (m_parseinfo) {
            IFR_SQL_TRACE << "PARSE ID: " << *m_parseinfo << endl;
        }
    }
    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
      m_parseinfo->sqlTrace(IFR_TRACE_STREAM);
    }
    //<<< SQL TRACE
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleParseResult (IFRPacket_ReplyPacket& replypacket,
                                     IFRPacket_ReplySegment& segment)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleParseResult);
    IFRPacket_PartEnum partEnum(segment);
    IFRConversion_ConverterList shortInfos(allocator);
    IFR_Bool memory_ok = true;
    IFR_String oldCursorName(allocator);
    IFRUtil_Vector<IFR_String> columnames(allocator, 0, oldCursorName, memory_ok);
    IFR_Bool isQuery=false;
    IFR_Bool parsid_found = false;
    IFR_Bool is_explain = false;
    if(segment.FunctionCode().isQuery()) {
        isQuery = true;
    }
    if(segment.FunctionCode().isExplain()) {
        is_explain = true;
    }
    if(isQuery || m_cursorstate == CursorStateUsed_C) {
        oldCursorName.assign(this->m_CursorName, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    while (partEnum.hasMoreElements ()) {
        IFRPacket_PartKind::PartKind PartKind = partEnum.getPartKind();
        DBUG_PRINT(PartKind);
        switch(PartKind) {
        case (IFRPacket_PartKind::Parsid_C): {
            parsid_found = true;
            IFR_ParseID ParseID;
            IFRPacket_ParseIDPart part;
            partEnum.getPart(part);
            part.getParseID (ParseID);
            ParseID.setConnectCount(m_Connection->getConnectCount());
            DBUG_PRINT(ParseID);
            m_parseinfo->setParseID (ParseID, memory_ok);
            if(!memory_ok) {
                error().setMemoryAllocationFailed();
                replypacket.releaseLock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            break;
        }
        case (IFRPacket_PartKind::Shortinfo_C): {
            IFRPacket_ShortInfoPart part;
            partEnum.getPart(part);
            if(part.parseShortFields (shortInfos, *this, is_explain) == IFR_NOT_OK) {
                replypacket.releaseLock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            break;
        }
        case (IFRPacket_PartKind::VarDataShortInfo_C): {
            IFRPacket_VarDataShortInfoPart part;
            partEnum.getPart(part);
            if(part.parseShortFields(shortInfos, *this) == IFR_NOT_OK) {
                replypacket.releaseLock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            break;
        }
        case (IFRPacket_PartKind::Resulttablename_C): {
            IFRPacket_TableNamePart part;
            partEnum.getPart(part);
            part.getResultName(this->m_CursorName, memory_ok);
            // disobey a cursor name of empty length
            if(this->m_CursorName.getStrLen() == 0) {
                this->m_CursorName.assign(oldCursorName, memory_ok);
            }
            if(!memory_ok) {
                error().setMemoryAllocationFailed();
                replypacket.releaseLock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            DBUG_PRINT(this->m_CursorName);
            break;
        }
        case (IFRPacket_PartKind::Tablename_C): {
            IFRPacket_TableNamePart part;
            partEnum.getPart(part);
            part.getResultName (this->m_TableName, memory_ok);
            if(!memory_ok) {
                error().setMemoryAllocationFailed();
                replypacket.releaseLock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            DBUG_PRINT(this->m_TableName);
            break;
        }
        case (IFRPacket_PartKind::Columnnames_C): {
            if(!is_explain) {
                IFRPacket_ColumnNamesPart part;
                partEnum.getPart(part);
                part.getColumnNames(columnames, memory_ok);
                if(!memory_ok) {
                    error().setMemoryAllocationFailed();
                    replypacket.releaseLock();
                    DBUG_RETURN(IFR_NOT_OK);
                }
            }
            break;
        }
        default: {
            // do nothing with new parts because they should ignored by the interface
            break;
        }
        }
        partEnum.next();
    }
    // set the short infos and columm names possibly gathered before.
    IFR_Retcode rc = m_parseinfo->setShortInfosAndColumnNames(shortInfos,
                                                              &columnames);

    if(!parsid_found && m_parseinfo->getFunctionCode().isDiagnose()) {
        m_parseinfo->setNoExecute(true);
    }

    // Did the parseinfo update fail ?
    if(rc == IFR_NOT_OK) {
        error().setMemoryAllocationFailed();
        replypacket.releaseLock();
        DBUG_RETURN(IFR_NOT_OK);
    }
    // unlock reply packet because this call may send a new request.
    replypacket.releaseLock();

    if(!isQuery && m_cursorstate == CursorStateUsed_C) {
        getConnection()->dropCursor(oldCursorName, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        this->m_cursorstate = CursorStateNotUsed_C;
    } else if(isQuery && m_cursorstate==CursorStateUsed_C && IFR_String::compare(this->m_CursorName, oldCursorName, memory_ok)) {
        getConnection()->dropCursor(oldCursorName, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        this->m_cursorstate = CursorStateNotUsed_C;
    }

    DBUG_RETURN(IFR_OK);
}
//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::execute()
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, execute);
    DBUG_PRINT(m_Connection);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::EXECUTE " << m_CursorName << " "
                  << currenttime << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    if(m_commandInfoState != CommandInfoNotSet_C) {
        m_commandInfoState = CommandInfoExecuted_C;
    }

// SQLDBC always comes the 'executeBatch' path, and has done these checks
// before.
#if 0
    if(m_status != Status_Other_C) {
        error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }
#endif

    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if (!m_parseinfo) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NOTPREPARED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_STMT_PROFILE_COUNT_INC(IFR_EXECUTECOUNT);

    if(m_parseinfo->isNoExecute()) {
        IFR_SQL_TRACE << "SQL COMMAND: " << (m_parseinfo->getSQLCommand()) << endl;
        IFR_SQL_TRACE_IF((m_parseinfo->getSQLCommand().getEncoding() != IFR_StringEncodingAscii))
            << "ENCODING : " << m_parseinfo->getSQLCommand().getEncoding() << endl;
        IFR_SQL_TRACE << "WARNING: COMMAND NOT EXECUTED (ONLY ON PARSE)" << endl;
        this->resetResults();
        error().clear();
        m_rowstatusarray[0]=IFR_SUCCESS_NO_INFO;
        DBUG_RETURN(IFR_OK);
    }

    //<<< SQL TRACE
    IFR_SQL_TRACE << "PARSE ID: " << *m_parseinfo << endl;
    IFR_SQL_TRACE << "SQL COMMAND: " << (m_parseinfo->getSQLCommand()) << endl;
    IFR_SQL_TRACE_IF((m_parseinfo->getSQLCommand().getEncoding() != IFR_StringEncodingAscii))
      << "ENCODING : " << m_parseinfo->getSQLCommand().getEncoding() << endl;
    //>>> SQL TRACE


    this->resetResults();
    error().clear();
    m_rowstatusarray[0]=IFR_SUCCESS_NO_INFO;
    m_parseinfo->lock();
    IFR_Int2 parameterCount      = m_parseinfo->getDirectParameterCount ();
    IFR_Int2 inputParameterCount = m_parseinfo->getInputParameterCount ();
    IFR_Bool hasLOBs             = false;
    IFR_Bool rowNotFound = false;

    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        if(inputParameterCount) {
            IFR_SQL_TRACE << "INPUT PARAMETERS:" << endl
                          << "APPLICATION" << endl
                          << IFR_SQLTRACE_DATA_HEADER << endl;
        }
    }
    //<<< SQL TRACE

    // Determine whether there are 'data at execute' parameters.
    for (IFR_Int2 i = 0; i < parameterCount; ++i) {
        IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[i];
        if(c->supportsInput()) {
            if((IFR_Int2)m_paramvector.GetSize() <= i) {
                error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)(i+1));
                DBUG_RETURN(IFR_NOT_OK);
            }
            if(m_paramvector[i].hasDataAtExecute()) {
                m_status = Status_ParamData_C;
                //>>> SQL TRACE
                if(IFR_SQL_TRACE_ENABLED) {
                    m_paramvector[i].sqlTraceParameterData(IFR_TRACE_STREAM, i+1, IFR_OK, 0, 0, 0, 0, false);
                    IFR_TRACE_STREAM << endl;
                }
                //<<< SQL TRACE
                DBUG_RETURN(IFR_NEED_DATA);
            }
        }
    }

    m_parseinfo->unlock();

    IFR_Retcode rc=IFR_OK;
    IFR_Bool    streamstruncated=false;

    // The loop is only executed multiple times if a PARSE AGAIN situation happens.
    for(IFR_Int4 retryCounter=0; retryCounter < PARSE_RETRY; ++retryCounter) {

        IFRPacket_ReplyPacket   replypacket;
        IFRPacket_RequestPacket requestpacket(*this);
        m_Connection->getRequestPacket(requestpacket, error());
        IFRPacket_DataPart datapart;
        IFRPacket_RequestSegment segment;
        IFR_Bool appendallowed = true;
        rc=executeInitPacket(requestpacket, segment, datapart);

        if(rc == IFR_NO_DATA_FOUND) {
            m_parseinfo->lock();
            if(m_parseinfo->getParseID(true)->isExecuted()) {
                m_parseinfo->unlock();
                IFR_Retcode parse_retcode=doParseAgain(ColumnChange_Allowed_C, false);
                return parse_retcode;
            } else {
                m_parseinfo->unlock();
                IFR_Retcode parse_retcode=doParseAgain(ColumnChange_Allowed_C, true); // we can handle a parse again here
                // even if it changes our mapping, as we would detect this through the conversion
                if(parse_retcode!=IFR_OK) {
                    DBUG_RETURN(parse_retcode);
                } else {
                    DBUG_PRINT(retryCounter);
                    --retryCounter; // If we have come here, we had no valid parse
                    // info, so no penalty
                    continue;
                }
            }
        } else if(rc == IFR_NOT_OK) {
            DBUG_RETURN(rc);
        }
        IFR_Retcode tmp_rc=IFR_OK;
        if(datapart.isValid()) {
            tmp_rc=executeFillDirectData(segment, datapart, streamstruncated, false, hasLOBs, 0);
            if(tmp_rc == IFR_NOT_OK || tmp_rc==IFR_OVERFLOW) {
                clearLOBs();
                clearInputLongs();
                m_parseinfo->unlock();
                if(getConnection()->reparseOnConversionError()) {

                    IFR_SQL_TRACE << "::CHECK PARSED STATEMENT VALIDITY" << endl;

                    IFR_ErrorHndl tmperror(allocator, getConnection());
                    tmperror.assign(error(), false);
                    IFR_Retcode parse_rc = doParseAgain(ColumnChange_Required_C, true);
                    if(parse_rc != IFR_OK) {
                        error().assign(tmperror, false);
                        DBUG_RETURN(IFR_NOT_OK);
                    } else {
                        --retryCounter;
                        continue;
                    }
                }
                DBUG_RETURN(IFR_NOT_OK);
            }
            DBUG_PRINT(tmp_rc);
        }
        if(m_cursorstate == CursorStateUsedClosed_C) {
            segment.setNoResultSetCloseNeeded();
        }
        if(m_parseinfo->isQuery() && m_Connection->getSelectFetchStrategy() == IFR_Connection::SelectFetchFetchImmediate_C) {
            IFR_Bool memory_ok=true;
            // make a fetch command ...
            segment.close();
            int currentSQLMode = requestpacket.switchSqlMode (IFR_INTERNAL);
            IFRPacket_RequestSegment fetchsegment(requestpacket,
                                                  IFRPacket_CommandMessageType::Dbs_C,
                                                  false);
            requestpacket.switchSqlMode (currentSQLMode);
            IFR_String fetchcmd(getCommandEncoding(), allocator);
            fetchcmd.append("FETCH FIRST \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            fetchcmd.append(m_CursorName, memory_ok);
            fetchcmd.append("\" INTO ? ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                m_parseinfo->unlock();
                error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
            IFRPacket_CommandPart commandpart;
            if((rc = fetchsegment.addPart(commandpart)) != IFR_OK) {
                m_parseinfo->unlock();
                error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                DBUG_RETURN(rc);
            }
            if (IFR_OK != commandpart.setText(fetchcmd, error())) {
                m_parseinfo->unlock();
                DBUG_RETURN(IFR_NOT_OK);
            }
            fetchsegment.closePart();
            fetchsegment.setMassCommand();
            fetchsegment.addFetchSize(32767);
            fetchsegment.close();
            appendallowed = false;
        }
        m_parseinfo->unlock();

        if (tmp_rc != IFR_OK) {
            rc = tmp_rc;
        }


        tmp_rc =  m_Connection->sqlaexecute (requestpacket, replypacket,
                                             appendallowed?m_parseinfo->getAppendMode():IFR_Connection::AppendNotAllowed_C,
                                             error());
        DBUG_PRINT(tmp_rc);

        if(tmp_rc != IFR_OK) {
            clearInputLongs();
            m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
            DBUG_RETURN(tmp_rc);
        }


        IFRPacket_ReplySegment replysegment (replypacket);
        if(replysegment.getSQLError(error(), allocator, false)) {
            if(error().isParseAgain()) {
                // We MUST release the reply packet, otherwise this is
                // deadlock-prone
                //>>> SQL TRACE
                IFR_SQL_TRACE << "PARSE AGAIN" << endl;
                //<<< SQL TRACE
                if(retryCounter == PARSE_RETRY - 1 ) {
                    DBUG_RETURN(IFR_NOT_OK);
                } else {
                    if(retryCounter == PARSE_RETRY - 1) {
                        DBUG_RETURN(IFR_NOT_OK);
                    } else {
                        replypacket.releaseLock();
                        clearLOBs();
                        clearInputLongs();
                        clearOutputLongs();
                        IFR_Retcode parse_retcode=doParseAgain(ColumnChange_Allowed_C, true);
                        if(parse_retcode!=IFR_OK) {
                            DBUG_RETURN(parse_retcode);
                        } else {
                            error().clear();
                            continue;
                        }
                    }
                }
            }
            // Check the 'NO DATA FOUND' condition. It is an issue if no
            // result set is created by the method.
            if(error().getErrorCode() != 100 && !m_parseinfo->isQuery()) {
                clearInputLongs();
                rc=IFR_NOT_OK;
                m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                DBUG_RETURN(rc);
            }
            if(error().getErrorCode() == 100) {
                error().clear();
                //>>> SQL TRACE
                IFR_SQL_TRACE << "*** ROW NOT FOUND ***" << endl;
                //<<< SQL TRACE
                clearInputLongs();
                rowNotFound = true;
                rc=IFR_NO_DATA_FOUND;
                m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
            }
        }

        if(m_parseinfo->isQuery()) {
            tmp_rc=parseResult(0, replypacket, false);
        } else {
            this->functionCode2Statistic(m_parseinfo->getFunctionCode());
            // in case of a procedure call, we need to check for streams
            if(m_parseinfo->getFunctionCode().isProcedureCall()) {

                IFRPacket_PartKind::PartKind partkind = replysegment.hasABAPStreams();
                if(partkind != IFRPacket_PartKind::Nil_C) {
                    tmp_rc = handleABAPStreams(partkind, replypacket);
                    if(tmp_rc != IFR_OK) {
                        DBUG_RETURN(tmp_rc);
                    }
                    replysegment = IFRPacket_ReplySegment(replypacket);
                    if(replysegment.getSQLError(error(), allocator, true)) {
                        clearInputLongs();
                        rc=IFR_NOT_OK;
                        m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                        DBUG_RETURN(rc);
                    }
                }
            }

            if(tmp_rc ==  IFR_OK) {
                IFRPacket_DataPart dp;
                replysegment.getPart(dp);
                if (dp.isValid ()) {
                    IFR_size_t copysize=sizeof(tsp1_part_header) + dp.Length();
                    IFR_Byte *bsource=(IFR_Byte*) dp.GetRawPart();
                    IFR_Byte *bdest = (IFR_Byte *) IFR_ALLOCATOR(allocator).Allocate(copysize);
                    memcpy(bdest, bsource, copysize);
                    if (m_copieddata)
                        IFRUtil_Delete(m_copieddata, allocator);
                    m_copieddata=new IFR_ALLOCATOR(allocator) CopiedDataPart(bdest,
                                                                             dp.getEncoding(),
                                                                             allocator);
                    tmp_rc=executeFillOutputData(replypacket, hasLOBs);

                    // handle the LONG values that were opened
                    checkOpenLongs(m_parseinfo->getParameterInfos(),
                                   1,
                                   1,
                                   0,
                                   dp);
                } else {
                    tmp_rc=executeFillOutputData(replypacket, hasLOBs);
                }

            }
        }
        if(hasLOBs && tmp_rc==IFR_OK) {
            m_status = Status_Keep_C;
        }

        if(tmp_rc == IFR_OK) {
            if(streamstruncated && this->m_parseinfo->hasLongs()) {
                tmp_rc = this->handleStreamsForPutval (replypacket, replysegment);
                DBUG_PRINT(tmp_rc);
            } else if(hasLOBs) {
                tmp_rc = this->getChangedPutvalDescriptors(replysegment);
            }
        }
        if (tmp_rc != IFR_OK)
          rc = tmp_rc;

        break;
    }
    DBUG_PRINT(rowNotFound);
    if(!hasLOBs) {
        clearInputLongs();
    }
    if ((rc != IFR_NOT_OK) && rowNotFound)
      DBUG_RETURN(IFR_NO_DATA_FOUND);
    else
      DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeFillOutputData(IFRPacket_ReplyPacket& replypacket,
                                        IFR_Bool hasLOBs)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeFillOutputData);
    IFRPacket_ReplySegment segment(replypacket);
    IFRPacket_DataPart     datapart;
    segment.getPart(datapart);
    IFR_Retcode rc=IFR_OK;
    IFR_Retcode *rc_params=0;  // return codes needed for SQLDBC trace
    IFRConversion_Converter **cv=0;
    this->m_RowsAffected = 0;
    if(datapart.isValid()) {
        this->m_counter_8[IFR_DATAREADSIZE_8_STMT] += datapart.getBufferLength();
        IFR_Int4 paramcount=m_parseinfo->getDirectParameterCount();
        IFR_Int4 enumCnt, varcount=m_paramvector.GetSize();
        IFRConversion_ConverterList& cv=m_parseinfo->getParameterInfos();
        IFR_Retcode tmp_rc = IFR_OK;
		enumCnt = (varcount< paramcount)?varcount:paramcount;
        for(IFR_Int4 i=0; i<enumCnt ; ++i) {
            if(cv[i]->supportsOutput()) {
                //<<< SQL TRACE
                if(IFR_SQL_TRACE_ENABLED && rc_params == 0) {
                    rc_params = (IFR_Retcode *) alloca(sizeof(IFR_Retcode) * paramcount);
                    memset(rc_params, 0, sizeof(IFR_Retcode) * paramcount);
                }
                //>>> SQL TRACE
                // We need to ignore the parameter if it has not been set.
                tmp_rc=cv[i]->translateOutput(datapart, m_paramvector[i], *this, 0, 0, true);
                if(rc_params) {
                    rc_params[i] = tmp_rc;
                }

                if(tmp_rc==IFR_NOT_OK) {
                    m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                    if(!error()) {
                        error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED);
                    }
                    DBUG_RETURN(tmp_rc);
                }
            }
        }

        // report possibly truncation result
        if (tmp_rc != IFR_OK) {
            rc = tmp_rc;
        }

        tmp_rc = this->handleStreamsForGetval(replypacket,
                                              datapart,
                                              cv,
                                              rc_params);
        if(tmp_rc==IFR_NOT_OK) {
            m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
            if(!error()) {
                error().setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED);
            }
            DBUG_RETURN(tmp_rc);
        }
        if (tmp_rc != IFR_OK) {
            rc = tmp_rc;
        }
        if (rc == IFR_OK || rc == IFR_DATA_TRUNC) {
            this->m_RowsAffected = 1;
        }

        //>>> SQL TRACE
        if(rc_params && IFR_SQL_TRACE_ENABLED) {
            IFR_SQL_TRACE << "OUTPUT PARAMETERS:" << endl
                          << "APPLICATION" << endl
                          << IFR_SQLTRACE_DATA_HEADER << endl;
			for(IFR_Int4 j=1; j<=enumCnt; ++j) {
                if(cv[j-1]->supportsOutput()) {
                    m_paramvector[j-1].sqlTraceParameterData(IFR_TRACE_STREAM, j, rc_params[j-1], 0, 0, 0, 0, false);
                    IFR_TRACE_STREAM << endl;
                }
            }
        }
        //<<< SQL TRACE
    } else {
        IFR_Int4 tmp_resultcount;
        if(segment.getResultCount(tmp_resultcount) == IFR_OK) {
            this->m_RowsAffected = tmp_resultcount;
            //>>> SQL TRACE
            IFR_SQL_TRACE << "RESULT COUNT: " << tmp_resultcount << endl;
            //<<< SQL TRACE
        }
        {
            IFRPacket_DataPart serialData;
            if(segment.getSerialPart(serialData) == IFR_OK) {
                rc = setSerialPart(serialData);
            }
        }
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeFillDirectData(IFRPacket_RequestSegment& segment,
                                        IFRPacket_DataPart& datapart,
                                        IFR_Bool& streamstruncated,
                                        IFR_Bool dontclose,
                                        IFR_Bool& hasLOBs,
                                        IFR_Int2 startindex)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeFillDirectData);
    if(!datapart.isValid()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Bool memory_ok=true;
    IFR_Int2 parameterCount = m_parseinfo->getDirectParameterCount();
    IFR_Int2 inputParameterCount = m_parseinfo->getInputParameterCount();
    IFR_Bool variable_input = m_parseinfo->hasVariableInput();

    IFR_Int2 i,j;
    IFR_Retcode rc=IFR_OK;

    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Int4 paramVectorSize = m_paramvector.GetSize();
    IFR_Bool dae_found=false; // data at execute found ?
    IFR_Bool lob_found=false;
    IFR_Bool dae_integral_found = false;
    // no-op in case of old format, necessary in case of variable
    // input arguments. also this is only needed if we are at
    // the beginning of a row (startindex is 0)
    if(startindex == 0) {
        datapart.setInputParameterCount(inputParameterCount);
    }
    for(i=startindex,j=0 ; i<parameterCount && j<inputParameterCount; ++i) {
        IFR_Int4 position = m_parseinfo->getParameterInfos().parameterForInputPosition(i);
        IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[position];
        if(c->supportsInput()) {
            ++j;
            if((IFR_Int2)paramVectorSize <= position) {
                m_paramvector[position].sqlTraceParameterData(IFR_TRACE_STREAM, position+1, IFR_NOT_OK, 0, 0, 0, 0, false);
                error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)position);
                DBUG_RETURN(IFR_NOT_OK);
            }

            IFR_Bool c_is_long = c->isLong();
            // Data is transferred into the part in case if it is
            // present (not 'AT EXECUTE')
            // For LONG values this will handle the common
            // cases, as NULL values, and otherwis fall through.
            if(!m_paramvector[i].hasDataAtExecute()) {
                if((!variable_input) ||
                   (variable_input && !dae_integral_found)) {
                    rc = c->translateInput(datapart, m_paramvector[position], *this);
                    //>>> SQL TRACE
                    if(startindex == 0 && IFR_SQL_TRACE_ENABLED) {
                        m_paramvector[position].sqlTraceParameterData(IFR_TRACE_STREAM, position+1, rc, 0, 0, 0, 0, true);
                        IFR_TRACE_STREAM << endl;
                    }
                    //<<< SQL TRACE
                    if(rc==IFR_NOT_OK || rc==IFR_OVERFLOW) {
                        m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                        DBUG_RETURN(rc);
                    }
                }
            }
            // Sanity check: DATA AT EXECUTE and LOBs are mutually
            // exclusive
            if(m_paramvector[position].hasDataAtExecute()) {
                dae_found = true;
                if(!c_is_long) {
                    dae_integral_found = true;
                }
                if(lob_found) {
                    m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                    error().setRuntimeError(IFR_ERR_DATA_AT_EXECUTE_NOT_ALLOWED_I, (IFR_Int4)position);
                    IFR_SQL_TRACE << error();
                    DBUG_RETURN(rc);
                }
            }

            if(m_paramvector[position].isLOB()) {
                lob_found = true;
                hasLOBs = true;
                if(dae_found) {
                    m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                    error().setRuntimeError(IFR_ERR_LOB_NOT_ALLOWED_I, (IFR_Int4)position);
                    IFR_SQL_TRACE << error();
                    DBUG_RETURN(rc);
                }
            }

            if(c_is_long && ((!variable_input) || (variable_input && !dae_integral_found))) {
                IFR_Bool memory_ok=true;
                IFRConversion_Putval *pv = c->createPutval(datapart, m_paramvector[position], *this);
                if(error()) {
                    this->m_parseinfo->unlock();
                    DBUG_RETURN(IFR_NOT_OK);
                } else if(pv) {
                    addInputLong(pv, memory_ok);

                    if(!memory_ok) {
                        error().setMemoryAllocationFailed();
                        IFRUtil_Delete(pv, allocator);
                        this->m_parseinfo->unlock();
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
            }

            // If we collect data, we have to register the
            // 'DATA AT EXECUTE' parameters.
            if(m_paramdata && startindex==0 && m_paramvector[position].hasDataAtExecute()) {
                if(c_is_long) {
                    m_paramdata->addLongParameter(position, memory_ok);
                    if(!memory_ok) {
                        this->m_parseinfo->unlock();
                        error().setMemoryAllocationFailed();
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                } else {
                    m_paramdata->addParameter(position, memory_ok);
                    if(!memory_ok) {
                        this->m_parseinfo->unlock();
                        error().setMemoryAllocationFailed();
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
            } else if(startindex != 0 && dae_found) {
                // if we call it from the middle of a nextparameter/putdata sequence,
                // we will leave at the first data at execute.
                DBUG_RETURN(IFR_OK);
            }
        }
    }
    IFR_Retcode streamexecute_rc;
    if (this->m_parseinfo->hasLongs() && ((!variable_input) || (variable_input && !dae_integral_found))) {
        streamexecute_rc = this->handleStreamsForExecute (datapart);
        DBUG_PRINT(streamexecute_rc);
        if(streamexecute_rc != IFR_OK &&
           streamexecute_rc != IFR_DATA_TRUNC) {
            rc=streamexecute_rc;
        }
        if(streamexecute_rc == IFR_DATA_TRUNC) {
            streamstruncated=true;
        }
    }
    if(rc == IFR_NOT_OK || rc == IFR_OVERFLOW) {
        m_parseinfo->unlock();
        DBUG_RETURN(rc);
    }
    datapart.setArgCount(1);
    IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, datapart.getBufferLength());

    if(!dontclose) {
        segment.closePart();
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeInitPacket(IFRPacket_RequestPacket& requestpacket,
                                    IFRPacket_RequestSegment& segment,
                                    IFRPacket_DataPart& datapart)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeInitPacket);

    m_parseinfo->lock();

    IFR_ParseID* parseid_ptr=m_parseinfo->getParseID();
    IFR_Bool     is_query   = m_parseinfo->isQuery();
    if(parseid_ptr == 0) {
        m_parseinfo->unlock();
        return IFR_NO_DATA_FOUND;
    }

    IFR_ParseID& parseid = *parseid_ptr;
    DBUG_PRINT(parseid);

    if(!parseid.isValid(m_Connection->getConnectCount())) {
        m_parseinfo->unlock();
        return IFR_NO_DATA_FOUND;
    }

    segment=IFRPacket_RequestSegment(requestpacket, sp1m_execute);
    IFR_Retcode rc=IFR_OK;

    requestpacket.setEncoding(getCommandEncoding());

    segment.setProducer (m_Connection->getProducer());

    if(m_Connection->getAutoCommit()) {
        segment.setCommitImmediately();
    }

    do {
        IFRPacket_ParseIDPart parseid_part;
        rc = segment.addPart(parseid_part);
        if(rc != IFR_OK) {
            break;
        }
        rc = parseid_part.addParseID(parseid);
        if(rc != IFR_OK) {
            break;
        }
        segment.closePart();

        if(is_query) {
            IFRPacket_TableNamePart tablename_part;
            rc = segment.addPart(tablename_part);
            if(rc != IFR_OK) {
                break;
            }
            rc = tablename_part.setText(this->m_CursorName, error());
            segment.closePart();

            segment.setScrollableOption(getConnection()->getScrollableOption(m_ResultSetType));
        }

        if(m_parseinfo->getInputParameterCount()) {
            rc = segment.addPart(m_parseinfo->hasVariableInput()?IFRPacket_PartKind::VarData_C:IFRPacket_PartKind::Data_C,
                                 datapart);
            if(rc != IFR_OK) {
                break;
            }
        }
        DBUG_RETURN(rc);

    } while(false);

    m_parseinfo->unlock();
    error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::putvalInitPacket(IFRPacket_RequestPacket& requestpacket,
                                   IFRPacket_RequestSegment& segment,
                                   IFRPacket_LongDataPart &datapart)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, putvalInitPacket);

    segment = IFRPacket_RequestSegment(requestpacket,
                                       IFRPacket_CommandMessageType::Putval_C);

    IFR_Retcode rc = segment.addPart(datapart);
    if(rc != IFR_OK) {
        error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
        DBUG_RETURN(rc);
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterByIndex(IFR_Int2& parameterindex,
                                       void *&parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterByIndex);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::NEXTPARAMBYINDEX "  << m_CursorName << " " << parameterindex
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    DBUG_RETURN(nextParameterInternal(parameterindex, parameterdata));
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameter(IFR_Int2& parameterindex,
                                void *&parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameter);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::NEXTPARAM " << m_CursorName << " "
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    IFR_Int2 tmp_parameterindex=0;
    IFR_Retcode rc = nextParameterInternal(tmp_parameterindex, parameterdata);
    if(rc == IFR_NEED_DATA) {
        parameterindex = tmp_parameterindex;
    }

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterInternal(IFR_Int2& parameterindex,
                                        void *&parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterInternal);


    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFR_Retcode rc = IFR_OK;

    switch(m_status) {
    case Status_ParamData_C: {
      rc = nextParameterParamData(parameterindex, parameterdata);
      break;
    }
    case Status_PutData_C: {
      rc = nextParameterPutData(parameterindex, parameterdata);
      break;
    }
    case Status_ParamDataBatch_C: {
      rc = nextParameterParamDataBatch(parameterindex, parameterdata);
      break;
    }
    case Status_PutDataBatch_C: {
      rc = nextParameterPutDataBatch(parameterindex, parameterdata);
      break;
    }
    case Status_Other_C: {}
    default: {
        error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        rc = IFR_NOT_OK;
        break;
    }
    }
    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
      if (rc == IFR_NEED_DATA)
        IFR_SQL_TRACE << "NEED DATA PARAMETER: " << parameterindex << endl;
    }
    //<<< SQL TRACE
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterParamData(IFR_Int2& parameterindex,
                                         void *& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterParamData);

    // initialise the
    m_paramdata = new IFR_ALLOCATOR(allocator) IFR_DataAtExecuteDescriptor(*this);
    m_Connection->getRequestPacket
        (m_paramdata->requestpacket,
         error(),
         IFRPacket_RequestPacket::Dynamic_C);

    IFR_Retcode rc = IFR_OK;
    IFR_Bool hasLOBsDummy=false;
    for(IFR_Int4 retryCounter=0; retryCounter < PARSE_RETRY; ++retryCounter) {
        rc = executeInitPacket(m_paramdata->requestpacket,
                               m_paramdata->segment,
                               m_paramdata->datapart);
        if(rc == IFR_NO_DATA_FOUND) {
            IFR_Retcode parse_retcode=doParseAgain(ColumnChange_Allowed_C, true);
            if(parse_retcode!=IFR_OK) {
                DBUG_RETURN(clearParamForReturn(parse_retcode));
            } else {
                DBUG_PRINT(retryCounter);
                --retryCounter; // no penalty here
                continue;
            }
        } else if(rc == IFR_NOT_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }
        break;
    }
    if(m_paramdata->datapart.isValid()) {
        rc=executeFillDirectData(m_paramdata->segment,
                                 m_paramdata->datapart,
                                 m_paramdata->streamstruncated,
                                 true,
                                 hasLOBsDummy,
                                 0);
        m_parseinfo->unlock();
        if(rc == IFR_NOT_OK || rc==IFR_OVERFLOW) {
            DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
        }
    } else {
        m_parseinfo->unlock();
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    // step toward the first parameter
    m_status=Status_PutData_C;

    rc = m_paramdata->next(parameterindex, m_parseinfo->hasVariableInput(), error());
    if(rc == IFR_NOT_OK) {
        DBUG_RETURN(rc);
    }

    parameterindex = m_paramdata->index();

    parameterdata = (void *) m_paramvector[parameterindex].data();

    // if the next parameter is a LONG and the streams are already truncated, it is
    // time for an execute.
    if(m_parseinfo->getParameterInfos()[parameterindex]->isLong() && m_paramdata->streamstruncated) {
        rc = handleExecuteForLateBinding();
        if(rc != IFR_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }
    }
    parameterindex ++;

    DBUG_RETURN(IFR_NEED_DATA);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterParamDataBatch(IFR_Int2& parameterindex,
                                              void *& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterParamDataBatch);
    IFR_Retcode rc;

    // The executeBatch method already provided the descriptor

    m_status = Status_PutDataBatch_C;
    rc= m_paramdata->next(parameterindex, m_parseinfo->hasVariableInput(), error());
    if(rc == IFR_NOT_OK) {
        DBUG_RETURN(clearParamForReturn(rc));
    }
    parameterindex = m_paramdata->index();
    parameterdata = (void *)
        ( m_paramvector[parameterindex].data(m_paramdata->inputcursor, m_bindingtype) );

    // if the next parameter is a LONG and the streams are already truncated, it is
    // time for an execute.
    if(m_parseinfo->getParameterInfos()[parameterindex]->isLong() && m_paramdata->streamstruncated) {

        m_paramdata->datapart.setArgCount(MIN(m_rowarraysize - 1, m_paramdata->inputcursor) - m_paramdata->firstrecord + 1);
        m_paramdata->segment.closePart();
        m_paramdata->segment.close();
        IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, m_paramdata->datapart.getBufferLength());
        rc = executeBatchSendCommand(m_paramdata->requestpacket,
                                                 m_paramdata->segment,
                                                 m_paramdata->inputcursor,
                                                 m_paramdata->firstrecord,
                                                 m_paramdata->executecount,
                                                 m_parseinfo->isMassCommand());
        if(rc != IFR_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }
    }

    parameterindex ++;
    DBUG_PRINT(parameterindex);
    DBUG_RETURN(IFR_NEED_DATA);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleExecuteForLateBinding()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleExecuteForLateBinding);

    IFR_Retcode rc = IFR_OK;
    IFR_Bool memory_ok = true;
    m_paramdata->segment.closePart();
    m_paramdata->segment.close();

    IFRPacket_ReplyPacket replypacket;

    for(IFR_Int4 retryCounter=0; retryCounter< PARSE_RETRY; ++retryCounter) {


        IFR_size_t original_size=m_paramdata->requestpacket.Length();

        rc = m_Connection->sqlaexecute(m_paramdata->requestpacket,
                                       replypacket,
                                       m_parseinfo->getAppendMode(),
                                       error());

        if(rc != IFR_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }

        IFRPacket_ReplySegment replysegment (replypacket);

        if(replysegment.getSQLError(error(), allocator)) {
            if(error().isParseAgain()) {
                if(retryCounter == PARSE_RETRY - 1) {
                    DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                } else {
                    replypacket.releaseLock();

                    // get the BUFPOS forall data fields, just in case they will not
                    // match.
                    IFRUtil_Vector<IFR_Int4> bufpos_vector(allocator);
                    IFR_Int2 parameterCount = m_parseinfo->getDirectParameterCount();
                    for(IFR_Int2 i=0; i<parameterCount; ++i) {
                        IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[i];
                        bufpos_vector.InsertEnd(c->getBufpos(), memory_ok);
                        if(!memory_ok) {
                            error().setMemoryAllocationFailed();
                            DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                        }
                    }

                    // we do this parse again with restrictions on changing columns,
                    // just to make sure that our current parameters do fit.
                    IFR_Retcode parse_retcode=doParseAgain(ColumnChange_Forbidden_C, true);


                    if(parse_retcode!=IFR_OK) {
                        DBUG_RETURN(clearParamForReturn(parse_retcode));
                    } else {
                        error().clear();
                        // Poke the new parse id into the request packet.
                        // Also, the number of segments must be set to 1
                        // to prevent already recycled garbage to be recycled again

                        rc = m_paramdata->requestpacket.replaceParseIDAdjustData(m_parseinfo->getParseID(),
                                                                                 original_size,
                                                                                 bufpos_vector,
                                                                                 m_parseinfo->getParameterInfos(),
                                                                                 *this);
                        if(rc != IFR_OK) {
                            DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                        }
                        replypacket.releaseLock();
                        continue;
                    }
                }
            }
            // Check the 'NO DATA FOUND' condition. It is an issue if no
            // result set is created by the method.
            if(error().getErrorCode() != 100 || !m_parseinfo->isQuery()) {
                rc=IFR_NOT_OK;
                m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
                DBUG_RETURN(clearParamForReturn(rc));
            } else {
                error().clear();
            }
        }

        if(m_parseinfo->isQuery()) {
            rc=parseResult(0, replypacket, false);
        } else {
            // fill output data, there are never LOBs in it.
            rc=executeFillOutputData(replypacket, false);
        }
        if(rc == IFR_OK) {
            // now the PUTVALs will follow
            m_paramdata->isexecute=false;
            if(this->m_parseinfo->hasLongs() && m_paramdata->streamstruncated) {
                IFR_Retcode putval_rc = this->handleStreamsForPutval (replypacket,
                                                                      replysegment,
                                                                      m_paramdata->requestpacket,
                                                                      m_paramdata->segment,
                                                                      m_paramdata->datapart);
                DBUG_PRINT(putval_rc);
                if(putval_rc != IFR_OK && putval_rc != IFR_NEED_DATA) {
                    DBUG_RETURN(clearParamForReturn(putval_rc));
                }
            } else if(m_parseinfo->hasLongs()) {
                getChangedPutvalDescriptors(replysegment);
                replypacket.releaseLock();
                putvalInitPacket(m_paramdata->requestpacket,
                                 m_paramdata->segment,
                                 m_paramdata->datapart);
            }
        } else {
            clearParamForReturn(rc);
        }
        break;
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handlePutvalForLateBinding(IFR_Bool close)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handlePutvalForLateBinding);
    DBUG_PRINT(close);

    // The LONG segment is already closed here (vm_close).
    IFR_Bool requiresTrailingPacket=false;

    if(close) {
        if(m_paramdata->datapart.closePutval() != IFR_OK) {
            requiresTrailingPacket = true;
        }
    }

    m_paramdata->segment.closePart();
    m_paramdata->segment.close();

    IFRPacket_ReplyPacket replypacket;

    IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute(m_paramdata->requestpacket,
                                                           replypacket,
                                                           IFR_Connection::AppendNotAllowed_C,
                                                           error());

    if(sqlaexecute_rc != IFR_OK) {
        m_rowstatusarray[m_paramdata->inputcursor]=IFR_EXECUTE_FAILED;
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_ReplySegment replysegment(replypacket);

    if(replysegment.getSQLError(error(), allocator)) {
        m_rowstatusarray[m_paramdata->inputcursor]=IFR_EXECUTE_FAILED;
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    if(!close) {
        getChangedPutvalDescriptors(replysegment);
        replypacket.releaseLock();
        putvalInitPacket(m_paramdata->requestpacket,
                         m_paramdata->segment,
                         m_paramdata->datapart);
    }

    if(requiresTrailingPacket) {
        replypacket.releaseLock();
        IFR_Retcode rc=sendTrailingPutvalClose();
        // !!!
        // If the trailing close fails, the sub transaction is not closed.
        // That means, a following COMMIT will close the PUTVAL implicitely,
        // and insert the LONG, a ROLLBACK will not insert it. So, you may
        // end up with having this method returning an error, but the LONG
        // is inserted.
        // !!!
        if(rc != IFR_OK) {
            m_rowstatusarray[m_paramdata->inputcursor]=IFR_EXECUTE_FAILED;
            DBUG_RETURN(clearParamForReturn(rc));
        }
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::sendTrailingPutvalClose()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, sendTrailingPutvalClose);

    IFRPacket_RequestPacket requestpacket(*this);
    IFRPacket_ReplyPacket   replypacket;
    m_Connection->getRequestPacket(requestpacket, error());
    IFRPacket_RequestSegment requestsegment;
    IFRPacket_LongDataPart longdata;

    IFR_Retcode rc=putvalInitPacket(requestpacket, requestsegment, longdata);
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }

    longdata.closePutval();
    requestsegment.closePart();
    requestsegment.close();

    IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute(requestpacket,
                                                           replypacket,
                                                           IFR_Connection::AppendNotAllowed_C,
                                                           error());
    if(sqlaexecute_rc != IFR_OK) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterPutDataBatch(IFR_Int2& parameterindex,
                                            void *& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterPutDataBatch);

    if(!m_paramdata->putdatasuccess) {
        error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)m_paramdata->index());
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    closeCurrentPutData();

    if(m_paramdata->isLast()) {

        // we need an afterburner here for the remaining parameters of variable input.
        if(m_paramdata->isexecute && m_parseinfo->hasVariableInput()) {
            IFR_Int2 lastindex=m_paramdata->index();
            IFR_Retcode rc = executeBatchFillData(m_paramdata->requestpacket,
                                                  m_paramdata->segment,
                                                  m_paramdata->datapart,
                                                  m_paramdata->inputcursor,
                                                  m_paramdata->firstrecord,
                                                  m_parseinfo->getInputParameterRowSize(),
                                                  m_parseinfo->isMassCommand(),
                                                  m_paramdata->executecount,
                                                  lastindex + 1,
                                                  error());
            if(rc != IFR_OK) {
                DBUG_RETURN(clearParamForReturn(rc));
            }

            IFR_Retcode streamrc = this->handleStreamsForExecute(m_paramdata->datapart);

            if(streamrc != IFR_OK && streamrc != IFR_DATA_TRUNC) {
                rc = streamrc;
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }
            }
            if(streamrc == IFR_DATA_TRUNC) {
                m_paramdata->streamstruncated = true;
            }

            // The input cursor is now wrong ...
            if(m_paramdata->streamstruncated) {
                --m_paramdata->inputcursor;
            }

        }

        // continue execution of batch
        // we did just jump back into the batch loop
        // the case when everything was short enough, and we can
        // try to fill some data in
        if(m_paramdata->isexecute) {

            // Copy or get status information.
            IFR_Bool  massCommand = m_parseinfo->isMassCommand();
            IFR_UInt4 recordSize  = m_parseinfo->getInputParameterRowSize();
            IFR_Int4  inputcursor = m_paramdata->inputcursor;
            IFR_Int4  firstrecord = m_paramdata->firstrecord;
            IFR_Int4  executecount = m_paramdata->executecount;
            IFRPacket_RequestPacket  requestpacket = m_paramdata->requestpacket;
            IFRPacket_RequestSegment segment       = m_paramdata->segment;
            // as we shred the parameters immediately after this, we have to change the
            // reference in the request segment to the newly created copy.
            segment.setRequestPacket(requestpacket);

            IFRPacket_LongDataPart   datapart      = m_paramdata->datapart;
            IFR_Bool streamstruncated              = m_paramdata->streamstruncated;

            // now we got all, and we can shred the parameter descriptor.
            clearParamData();

            // if the command is mass-capable, we may add some data.
            if(massCommand && !streamstruncated) {

                // We must step one step forward. The input longs are cleared by
                // the filldata method.
                ++inputcursor;
                datapart.moveRecordBase();

                IFR_Retcode filldata_rc = executeBatchFillData(requestpacket,
                                                               segment,
                                                               datapart,
                                                               inputcursor,
                                                               firstrecord,
                                                               recordSize,
                                                               massCommand,
                                                               executecount,
                                                               0,
                                                               error());
                // now we might have been catched by ourself - in case the
                // next row is also trapped with DATA AT EXECUTE parameters
                // As the fill method did set up the descriptor for the new row
                // correctly, we can just call ourself recursive
                if(filldata_rc == IFR_NEED_DATA) {
                    DBUG_RETURN(nextParameter(parameterindex, parameterdata));
                }
                // check that the conversion did work.
                if(filldata_rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(filldata_rc));
                }
            }

            IFR_Bool firstloop=true;

            do {
                // In the first run we have a filled packet, so don't put any harm. But
                // in the following loops we need to re-initialize the packet for a
                // 'fresh' batch execute
                if(!firstloop) {
                    firstrecord = inputcursor;
                    IFR_Retcode rc = executeBatchInitPacket(requestpacket, segment, datapart, executecount, error());
                    if(rc != IFR_OK) {
                        DBUG_RETURN(rc);
                    }
                    rc=executeBatchFillData(requestpacket,
                                            segment,
                                            datapart,
                                            inputcursor,
                                            firstrecord,
                                            recordSize,
                                            massCommand,
                                            executecount,
                                            0,
                                            error());
                    // now we might have been catched by ourself - in case the
                    // next row is also trapped with DATA AT EXECUTE parameters
                    // As the fill method did set up the descriptor for the new row
                    // correctly, we can just call ourself recursive
                    if(rc == IFR_NEED_DATA) {
                        DBUG_RETURN(nextParameter(parameterindex, parameterdata));
                    }
                    // check that the conversion did work.
                    if(rc != IFR_OK) {
                        DBUG_RETURN(clearParamForReturn(rc));
                    }
                }
                // is 1 if not mass command, and the number of records otherwise.
                datapart.setArgCount(MIN(inputcursor, m_rowarraysize - 1) - firstrecord + 1);



                segment.closePart();
                segment.close();

                IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, datapart.getBufferLength());
                IFR_Retcode rc = executeBatchSendCommand(requestpacket,
                                                         segment,
                                                         inputcursor,
                                                         firstrecord,
                                                         executecount,
                                                         massCommand);
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }

                // If in the first loop, we need to increase the input cursor now,
                // and switch to 'normal processing'.
                if(firstloop) {
                    firstloop = false;
                    ++inputcursor;
                }

            } while(inputcursor < m_rowarraysize);
            DBUG_RETURN(clearParamForReturn(IFR_OK));
        } else {
            // we have a pending putval. If we are in the last row, we have to do this and to return.
            if(m_paramdata->inputcursor + 1 >=m_rowarraysize) {
                IFR_Retcode putval_rc = handlePutvalForLateBinding(true);
                DBUG_RETURN(clearParamForReturn(putval_rc));
            } else {
                IFR_Retcode putval_rc = handlePutvalForLateBinding(false);
                if(putval_rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(putval_rc));
                }
            }

            IFR_Bool  massCommand = m_parseinfo->isMassCommand();
            IFR_UInt4 recordSize  = m_parseinfo->getInputParameterRowSize();
            IFR_Int4  inputcursor = m_paramdata->inputcursor;
            IFR_Int4  firstrecord = m_paramdata->firstrecord;
            IFR_Int4  executecount = m_paramdata->executecount;
            IFRPacket_RequestPacket  requestpacket = m_paramdata->requestpacket;
            IFRPacket_RequestSegment segment       = m_paramdata->segment;
            IFRPacket_LongDataPart   datapart      = m_paramdata->datapart;

            // now we got all, and we can shred the parameter descriptor.
            clearParamData();
            // step to next line, as data part is shredded immediately after, we need
            // no moveRecordBase
            inputcursor++;

            do {
                firstrecord = inputcursor;
                IFR_Retcode rc = executeBatchInitPacket(requestpacket, segment, datapart, executecount, error());
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }

                rc=executeBatchFillData(requestpacket,
                                        segment,
                                        datapart,
                                        inputcursor,
                                        firstrecord,
                                        recordSize,
                                        massCommand,
                                        executecount,
                                        0,
                                        error());
                // now we might have been catched by ourself - in case the
                // next row is also trapped with DATA AT EXECUTE parameters
                // As the fill method did set up the descriptor for the new row
                // correctly, we can just call ourself recursive
                if(rc == IFR_NEED_DATA) {
                    DBUG_RETURN(nextParameter(parameterindex, parameterdata));
                }
                // check that the conversion did work.
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }
                datapart.setArgCount(MIN(m_rowarraysize - 1, inputcursor) - firstrecord + 1);
                segment.closePart();
                segment.close();
                IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, datapart.getBufferLength());
                rc = executeBatchSendCommand(requestpacket,
                                             segment,
                                             inputcursor,
                                             firstrecord,
                                             executecount,
                                             massCommand);
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }
            } while(inputcursor < m_rowarraysize);
            DBUG_RETURN(clearParamForReturn(IFR_OK));
        }
    } else {
        IFR_Retcode rc;
        IFR_Int2 lastindex=m_paramdata->index();

        rc = m_paramdata->next(parameterindex, m_parseinfo->hasVariableInput(), error());

        if(rc != IFR_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }

        if(m_paramdata->isexecute && m_parseinfo->hasVariableInput()) {
            rc = executeBatchFillData(m_paramdata->requestpacket,
                                      m_paramdata->segment,
                                      m_paramdata->datapart,
                                      m_paramdata->inputcursor,
                                      m_paramdata->firstrecord,
                                      m_parseinfo->getInputParameterRowSize(),
                                      m_parseinfo->isMassCommand(),
                                      m_paramdata->executecount,
                                      lastindex + 1,
                                      error());
        }

        if(rc == IFR_NOT_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }
        parameterindex = m_paramdata->index();

        // look if we are just switching from non LONG. In that case, if
        // we have already truncated the direct supplied LONG data, we will continue with
        if(!m_parseinfo->getParameterInfos()[lastindex]->isLong()
           && m_parseinfo->getParameterInfos()[parameterindex]->isLong()) {


            if(m_parseinfo->hasVariableInput() && m_paramdata->isexecute) {
                   IFR_Retcode streamrc = this->handleStreamsForExecute(m_paramdata->datapart);
                   if(streamrc != IFR_OK && streamrc != IFR_DATA_TRUNC) {
                       rc = streamrc;
                       if(rc != IFR_OK) {
                           DBUG_RETURN(clearParamForReturn(rc));
                       }
                   }
                   if(streamrc == IFR_DATA_TRUNC) {
                       m_paramdata->streamstruncated = true;
                   }
               }


            if(m_paramdata->streamstruncated) {
                m_paramdata->datapart.setArgCount(MIN(m_rowarraysize - 1, m_paramdata->inputcursor) -
                                                  m_paramdata->firstrecord + 1);
                m_paramdata->segment.closePart();
                m_paramdata->segment.close();
                IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, m_paramdata->datapart.getBufferLength());
                rc = executeBatchSendCommand(m_paramdata->requestpacket,
                                             m_paramdata->segment,
                                             m_paramdata->inputcursor,
                                             m_paramdata->firstrecord,
                                             m_paramdata->executecount,
                                             m_parseinfo->isMassCommand());
                if(rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(rc));
                }
            }
        }

        parameterdata = (void *) m_paramvector[parameterindex].data(m_paramdata->inputcursor, m_bindingtype);
        parameterindex ++;
        DBUG_PRINT(parameterindex);
        DBUG_RETURN(IFR_NEED_DATA);
    }
}



//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::nextParameterPutData(IFR_Int2& parameterindex,
                                       void *& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, nextParameterPutData);
    if(!m_paramdata->putdatasuccess) {
        error().setRuntimeError(IFR_ERR_PARAMETER_NOT_SET_I, (IFR_Int4)m_paramdata->index());
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    closeCurrentPutData();

    if(m_paramdata->isLast()) {
        IFR_Retcode rc;
        if(m_paramdata->isexecute) {
            rc=handleExecuteForLateBinding();
        } else {
            rc = handlePutvalForLateBinding(true);
        }
        DBUG_RETURN(clearParamForReturn(rc));
    } else {
        IFR_Retcode rc;
        IFR_Int2 lastindex=m_paramdata->index();

        if(m_paramdata->isexecute && m_parseinfo->hasVariableInput()) {
            IFR_Bool hasLOBsDummy;
            rc=executeFillDirectData(m_paramdata->segment,
                                     m_paramdata->datapart,
                                     m_paramdata->streamstruncated,
                                     true,
                                     hasLOBsDummy,
                                     lastindex + 1);
        }

        rc = m_paramdata->next(parameterindex, m_parseinfo->hasVariableInput(), error());
        if(rc == IFR_NOT_OK) {
            DBUG_RETURN(clearParamForReturn(rc));
        }
        parameterindex = m_paramdata->index();

        // look if we are just switching from non LONG. In that case, if
        // we have already truncated the direct supplied LONG data, we will continue with
        if(!m_parseinfo->getParameterInfos()[lastindex]->isLong()
           && m_parseinfo->getParameterInfos()[parameterindex]->isLong()) {

               if(m_parseinfo->hasVariableInput() && m_paramdata->isexecute) {
                   IFR_Retcode streamrc = this->handleStreamsForExecute(m_paramdata->datapart);
                   if(streamrc != IFR_OK && streamrc != IFR_DATA_TRUNC) {
                       rc = streamrc;
                       if(rc != IFR_OK) {
                           DBUG_RETURN(clearParamForReturn(rc));
                       }
                   }
                   if(streamrc == IFR_DATA_TRUNC) {
                       m_paramdata->streamstruncated = true;
                   }
               }

               if(m_paramdata->streamstruncated) {
                   rc = handleExecuteForLateBinding();
                   if(rc != IFR_OK) {
                       DBUG_RETURN(clearParamForReturn(rc));
                   }
               }
        }

        parameterdata = (void *) m_paramvector[parameterindex].data();
        parameterindex ++;

        DBUG_RETURN(IFR_NEED_DATA);
    }
}



//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::putData(void *parameterdata, IFR_Length *lengthindicator)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, putData);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::PUTDATA" << m_CursorName << " "
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(assertCurrentParseInfo()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(m_status != Status_PutData_C &&
       m_status != Status_PutDataBatch_C) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFR_Int2 current_index=m_paramdata->index();

    if(m_paramdata->integralfinished) {
        error().setRuntimeError(IFR_ERR_NOPIECEWISE_DATA_WRITE_I, (IFR_Int4) current_index );
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    if(m_paramdata->nullordefault) {
        error().setRuntimeError(IFR_ERR_NONULLDATAAPPEND_I, (IFR_Int4) current_index );
        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
    }

    IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[current_index];
    IFR_Retcode rc;

    IFR_Length    lengthindicator_value;
    IFR_Length*   used_lengthindicator=lengthindicator;

    // Look up the next input long. Switching to next parameter
    // will reinitialize the 'lastputval' field with 0,
    // so that we do this only once for a parameter.
    if(c->isLong() && m_paramdata->lastputval==0) {
        m_paramdata->lastputval = findInputLong(current_index+1, 0);
    }

    while(true) {
        IFR_Length previousreadoffset=m_paramdata->currentreadoffset;
        rc = c->putData(m_paramdata->datapart,
                        parameterdata,
                        used_lengthindicator,
                        m_paramvector[current_index],
                        m_paramdata->currentreadoffset,
                        m_paramdata->nullordefault,
                        m_paramdata->integralfinished,
                        *this,
                        m_paramdata->lastputval);

        if(error()) {
          DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
        }

        // we don't need that info here
        if(rc == IFR_NO_DATA_FOUND) {
            rc = IFR_OK;
        }

        // Look if it is full.
        if(rc == IFR_DATA_TRUNC && c->isLong()) {
            if(m_paramdata->currentreadoffset != previousreadoffset) {
                m_paramdata->putvalstreamtruncated = true;
            }

            // execute
            if(m_paramdata->isexecute) {
                if(m_status == Status_PutData_C) {
                    IFR_Retcode execute_rc = handleExecuteForLateBinding();
                    if(execute_rc != IFR_OK) {
                        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                    }
                    c = m_parseinfo->getParameterInfos()[current_index];
                } else {
                    m_paramdata->datapart.setArgCount(MIN(m_rowarraysize - 1, m_paramdata->inputcursor) -
                                                      m_paramdata->firstrecord + 1);

                    m_paramdata->segment.closePart();
                    m_paramdata->segment.close();
                    IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, m_paramdata->datapart.getBufferLength());
                    IFR_Retcode execute_rc = executeBatchSendCommand(m_paramdata->requestpacket,
                                                                     m_paramdata->segment,
                                                                     m_paramdata->inputcursor,
                                                                     m_paramdata->firstrecord,
                                                                     m_paramdata->executecount,
                                                                     m_parseinfo->isMassCommand());

                    if(execute_rc != IFR_OK) {
                        DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                    }
                    m_paramdata->isexecute=false;
                }
            } else {
                IFR_Retcode putval_rc = handlePutvalForLateBinding(false);
                if(putval_rc != IFR_OK) {
                    DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
                }
            }

            // do this only if we have written at least one byte
            if(m_paramdata->currentreadoffset != previousreadoffset) {
                IFR_Length writtenBytes=m_paramdata->currentreadoffset - previousreadoffset;

                char *tmp = (char *)parameterdata;
                tmp += writtenBytes;
                parameterdata = (void *) tmp;

                if(used_lengthindicator) {
                    lengthindicator_value = *used_lengthindicator;
                    used_lengthindicator = &lengthindicator_value;

                    if(lengthindicator_value != IFR_NTS) {
                        lengthindicator_value -= writtenBytes;
                        if(lengthindicator_value < 0) {
                            rc = IFR_NOT_OK;
                            break;
                        }
                    }
                }
            }

            m_paramdata->currentreadoffset = 0;

            continue;
        }

        break;
    }

    if(!error()) {
        m_paramdata->putdatasuccess=true;
    }

    //<<< SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE << "INPUT PARAMETER:" << endl
                      << "APPLICATION" << endl
                      << IFR_SQLTRACE_DATA_HEADER << endl;
        m_paramvector[current_index].sqlTraceParameterData(IFR_TRACE_STREAM,
                                                           current_index+1,
                                                           rc,
                                                           0, 0,
                                                           (unsigned char *)parameterdata,
                                                           lengthindicator,
                                                           true);
        IFR_SQL_TRACE << endl;
    }
    //>>> SQL TRACE

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::getObject (IFR_Int4 paramindex,
                             void* paramaddr,
                             IFR_Length parambytelength,
                             IFR_HostType paramtype,
                             IFR_Length* paramlengthindicator,
                             IFR_Bool terminate,
                             IFR_Length* paramposindicator)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getObject);
    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::GETOBJECT " << m_CursorName << endl;
    //<<< SQL TRACE ENTRY
    IFR_Retcode rc        = IFR_OK;
    IFR_Bool    memory_ok = true;
    error().clear();

    if (!m_Connection->isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        return (IFR_NOT_OK);
    }

    if (parambytelength < 0){
        error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, paramindex, (IFR_Int4)parambytelength);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if (paramindex == 0) {
        error().setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX_I, (IFR_Int4)paramindex);
        DBUG_RETURN(IFR_NOT_OK);
    }


    IFR_Parameter parameter (paramtype,
                             terminate,
                             paramaddr,
                             paramlengthindicator,
                             parambytelength,
                             paramposindicator);
    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << "PARAMETER" << endl
                         << IFR_SQLTRACE_PARAMETER_HEADER << endl;
        parameter.sqlTraceParameter(IFR_TRACE_STREAM, paramindex);
        IFR_TRACE_STREAM << endl;
    }
    //<<< SQL TRACE


    IFRConversion_Converter *info = m_parseinfo->getParameterInfos()[paramindex - 1];

    IFRPacket_DataPart part;
    if ((rc = getCurrentData (part)) != IFR_OK) {
        DBUG_RETURN(rc);
    }

    if(!info->isLong()) {
        if(m_lastgetobjcol != paramindex) {
            m_lastgetobjcol = paramindex;
            m_offset        = 1;
        }

        if(paramposindicator) {
            m_offset = *paramposindicator;
        }

        if(m_offset == 0) {
            m_offset = 1;
        }

        IFR_Length dataoffset = 0;
        rc = info->getData(part,
                           (void *) parameter.data(),
                           paramlengthindicator,
                           parameter,
                           dataoffset,
                           m_offset,
                           *this,
                           parambytelength,
                           0);

    } else {
        // find the getval and fill the output.
        IFRConversion_Getval *getval = findOutputLong(paramindex,
                                                      1);
        if(getval == 0) {
            getval = info->createGetval (part,
                                         &parameter,
                                         *this,
                                         1,
                                         0,
                                         0);
            if(getval == 0) {
                if(error()) {
                    DBUG_RETURN(IFR_NOT_OK);
                }
            } else {
                addOutputLong(getval, memory_ok);
                if(!memory_ok) {
                    IFRUtil_Delete(getval, allocator);
                    error().setMemoryAllocationFailed();
                    DBUG_RETURN(IFR_NOT_OK);
                }
            }
        } else {
            if(!parameter.isBinary() && getval->isBinary() && m_Connection->odbcBinToHex()) {
                getval->setBinaryToHex(true);
            } else {
                getval->setBinaryToHex(false);
            }
        }

        if(m_lastgetobjcol != paramindex) {
            m_lastgetobjcol = paramindex;
            m_offset        = 1;
        }

        IFR_Length  dataoffset = 0;
        IFR_Length  datalength = parameter.getBytesLength();
        IFR_Length *lengthindicator = parameter.getLengthIndicator();

        if(lengthindicator) {
            *lengthindicator = 0;
        }

        // check for positioned reading, and reset the read position
        const IFR_Length *posindicator = parameter.getPosIndicator();
        if(posindicator) {
            m_offset = *posindicator;
        }

        if(m_offset == 0) {
            m_offset = 1;
        }

        IFRPacket_DataPart *longdata = getval->getLongData();

        rc = info->getData(longdata?*longdata:part,
                           (void *) parameter.data(),
                           lengthindicator,
                           parameter,
                           dataoffset,
                           m_offset,
                           *this,
                           datalength,
                           getval);

        if(rc == IFR_NEED_DATA) {
            IFRPacket_RequestPacket   requestpacket(*this);
            IFRPacket_ReplyPacket     replypacket;
            IFRPacket_RequestSegment  segment;
            IFRPacket_LongDataPart    longdatapart;

            while(true) {
                getConnection()->getRequestPacket(requestpacket, error());
                getvalInitPacket(requestpacket, segment, longdatapart);
                getval->putDescriptor(longdatapart);
                segment.closePart();
                segment.close();

                IFR_Retcode sqlaexecute_rc = getConnection()->sqlaexecute(requestpacket,
                                                                          replypacket,
                                                                          IFR_Connection::AppendNotAllowed_C,
                                                                          error());
                if (sqlaexecute_rc != IFR_OK) {
                    DBUG_RETURN(sqlaexecute_rc);
                }

                IFRPacket_ReplySegment replysegment(replypacket);
                IFR_Int4 updated_longcount;
                updateOutputLongs(replysegment, *this, updated_longcount, this->m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);

                if ((rc = replysegment.getPart (longdatapart)) != IFR_OK) {
                    error().setRuntimeError(IFR_ERR_MISSINGLONGDATAPART);
                    DBUG_RETURN(rc);
                }

                IFR_Length *lengthindicator=parameter.getLengthIndicator();
                if(lengthindicator && *lengthindicator) {
                    lengthindicator = 0;
                }
                rc = info->getData(longdatapart,
                                   (void *) parameter.data(),
                                   lengthindicator,
                                   parameter,
                                   dataoffset,
                                   m_offset,
                                   *this,
                                   datalength,
                                   getval);
                if(rc ==  IFR_OK || rc == IFR_DATA_TRUNC) {
                    break;
                } else if(rc != IFR_NEED_DATA) {
                    DBUG_RETURN(rc);
                }
                replypacket.releaseLock();
            }
        }
    }
    //<<< SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << "DATA" << endl
                         << IFR_SQLTRACE_DATA_HEADER << endl;
        parameter.sqlTraceParameterData(IFR_TRACE_STREAM, 1, rc, 0, 0, 0, 0, true);
        IFR_SQL_TRACE << endl;
    }
    //>>> SQL TRACE
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::getData(IFR_LOB    *lob,
                          void       *data,
                          IFR_Length *lengthindicator,
                          IFR_Length  bytelength,
                          IFR_Length *posindicator,
                          IFR_Bool    terminate)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getData);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(bytelength);
    DBUG_PRINT(posindicator);
    DBUG_PRINT(terminate);

    IFR_Int2 column = lob->getColumn();
    IFR_HostType datahosttype = lob->getDataHostType();
    DBUG_RETURN(getObject(column,
                          data,
                          bytelength,
                          datahosttype,
                          lengthindicator,
                          terminate,
                          posindicator));
}

//----------------------------------------------------------------------
IFR_Length
IFR_PreparedStmt::getLength(IFR_LOB* lob)
{
    error().clear();
    IFR_Int2 column = lob->getColumn();
    IFR_Int4 row    = lob->getRow();


    if(row != 1) {
        error().setRuntimeError(IFR_ERR_INVALID_ROWSET_POS_I, row);
        return -1;
    }

    if(column < 1 || column >= m_parseinfo->getParameterCount()) {
        error().setRuntimeError(IFR_ERR_INVALID_PARAMETERINDEX_I, (IFR_Int4)column);
        return -1;
    }

    IFRConversion_Converter *info = m_parseinfo->getParameterInfos()[column - 1];
        
    IFRPacket_DataPart part;
    if(getCurrentData (part) != IFR_OK) {
        return -1;
    }

    IFRConversion_Getval *getval = findOutputLong(column, 
                                                  row);
    IFR_Bool memory_ok = true;
    if(getval == 0) {
        getval = info->createGetval (part,
                                     0,
                                     *this,
                                     1,
                                     0,
                                     0);
        if(getval == 0) {
            return -1;
        } else {
            addOutputLong(getval, memory_ok);
            if(!memory_ok) {
                IFRUtil_Delete(getval, allocator);
                error().setMemoryAllocationFailed();
                return -1;
            }
        }
    } else {
        if((lob->getDataHostType() != IFR_HOSTTYPE_BINARY)
           && getval->isBinary()
           && m_Connection->odbcBinToHex()) {
            getval->setBinaryToHex(true);
        } else {
            getval->setBinaryToHex(false);
        }
    }

    IFR_Length result = getval->getMaximumOutputLength();
    if(result == -1) {
        IFRPacket_RequestPacket   requestpacket(*m_resultset);
        IFRPacket_ReplyPacket     replypacket;
        IFRPacket_RequestSegment  segment;
        IFRPacket_LongDataPart    longdatapart;

        IFR_Retcode packet_rc = getConnection()->getRequestPacket(requestpacket, error());
        if(packet_rc != IFR_OK) {
            getval->clearLongData();
            return -1;
        }
        getvalInitPacket(requestpacket, segment, longdatapart);
        getval->putDescriptorForGetLength(longdatapart);
        segment.closePart();
        segment.close();

        IFR_Retcode sqlaexecute_rc = getConnection()->sqlaexecute(requestpacket,
                                                                  replypacket,
                                                                  IFR_Connection::AppendNotAllowed_C,
                                                                  error());
        if (sqlaexecute_rc != IFR_OK) {
            getval->clearLongData();
            return -1;
        } else if(error()) {
            getval->clearLongData();
            return -1;
        }

        IFRPacket_ReplySegment replysegment(replypacket);
        IFR_Int4 updated_longcount;
        updateOutputLongs(replysegment, *this, updated_longcount, m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);
        result = getval->getMaximumOutputLength();
    }
    
    return lob->getLengthValue(result,
                               getval->getDatatype(),
                               getval->isBinaryToHex());

}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::closeLOB(IFR_LOB *lob)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, closeLOB);
    if(lob->isInput()) {
        IFRConversion_Putval *pv = findInputLong(lob->getColumn(), lob->getRow());
        if(pv == 0) {
            DBUG_RETURN(IFR_NOT_OK);
        }

        IFR_size_t lobs_count = getLOBCount();
        int openlobscount=0;
        for(IFR_size_t i=0; i<lobs_count; ++i) {
            IFR_LOB *lob=getLOB(i);
            if(lob->isInput() && lob->isOpen()) {
                ++openlobscount;
            }
        }

        IFRPacket_RequestPacket requestpacket(*this);
        IFRPacket_ReplyPacket   replypacket;
        m_Connection->getRequestPacket(requestpacket, error());
        IFRPacket_RequestSegment requestsegment;
        IFRPacket_LongDataPart longdata;

        IFR_Retcode rc=putvalInitPacket(requestpacket, requestsegment, longdata);
        if(rc != IFR_OK) {
            DBUG_RETURN(rc);
        }
        pv->putCloseDescriptor(longdata, openlobscount == 1);
        if(openlobscount == 1) {
            m_status = Status_LobsClosed_C;
        }
        requestsegment.closePart();
        requestsegment.close();
        IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute(requestpacket,
                                                               replypacket,
                                                               IFR_Connection::AppendNotAllowed_C,
                                                               error());
        if(sqlaexecute_rc != IFR_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        } else if(error()) {
            DBUG_RETURN(IFR_NOT_OK);
        }

        // Throw all long descriptor stuff away when the last
        // LOB object is closed.
        if(openlobscount == 1) {
            clearParameters(false, false);
        }
        DBUG_RETURN(IFR_OK);
    } else {
        IFRConversion_Getval *gv = findOutputLong(lob->getColumn(), lob->getRow());
        if(gv == 0 ) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(gv->isClosed()) {
            DBUG_RETURN(IFR_OK);
        }
        gv->close();
        DBUG_RETURN(IFR_OK);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::putData(IFR_LOB *lob,
                          void *parameterdata,
                          IFR_Length* lengthindicator)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, putData_LOB);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::PUTDATA"
                  << " [0x" << (void*)this << "]" << endl;
    //<<< SQL TRACE ENTRY

    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(assertCurrentParseInfo()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(m_status != Status_Keep_C) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }


    // Get converter instance.
    IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[lob->getColumn()-1];
    IFRConversion_Putval   *pv=findInputLong(lob->getColumn(), lob->getRow());

    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE << "INPUT PARAMETER:" << endl
                      << "APPLICATION" << endl
                      << IFR_SQLTRACE_DATA_HEADER << endl;
        IFR_UInt2 col = lob->getColumn();
        m_paramvector[col-1].sqlTraceParameterData(IFR_TRACE_STREAM,
                                                   lob->getColumn(),
                                                   IFR_OK,
                                                   0, 0,
                                                   (unsigned char *)parameterdata,
                                                   lengthindicator,
                                                   true);
        IFR_SQL_TRACE << endl;
    }
    //<<< SQL TRACE


    IFR_Retcode rc = IFR_NOT_OK;
    {
        IFRPacket_RequestPacket requestpacket(*this);
        IFRPacket_ReplyPacket   replypacket;
        m_Connection->getRequestPacket(requestpacket, error());
        IFRPacket_RequestSegment requestsegment;
        IFRPacket_LongDataPart   datapart;

        IFR_Length readoffset = 0;
        IFR_Length used_length;
        IFR_Length *used_lengthindicator = lengthindicator;
        IFR_Bool   nullordefault = false;
        IFR_Bool   integral      = false;

        while(true) {
            rc = putvalInitPacket(requestpacket, requestsegment, datapart);
            if(rc != IFR_OK) {
                goto exit_abort;
            }
            IFR_Length previousoffset = readoffset;
            IFR_Retcode putval_rc = c->putData(datapart,
                                               parameterdata,
                                               used_lengthindicator,
                                               m_paramvector[lob->getColumn()-1],
                                               readoffset,
                                               nullordefault,
                                               integral,
                                               *this,
                                               pv);
            // if the 'putdata' resulted in an error, return.
            if(error()) {
                rc = IFR_NOT_OK;
                goto exit_abort;
            }
            if(putval_rc == IFR_NO_DATA_FOUND) {
                rc = IFR_OK;
                goto finish_ok;
            }
            // can't happen
            if(nullordefault) {
                rc = IFR_NOT_OK;
                goto exit_abort;
            }
            // Send the PUTVAL
            requestsegment.closePart();
            requestsegment.close();
            IFRPacket_ReplyPacket replypacket;
            rc = m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::AppendNotAllowed_C, error());
            // Check command error.
            if(rc == IFR_NOT_OK) {
                goto exit_abort;
            }
            // Check possible SQL error
            if(error()) {
                rc = IFR_NOT_OK;
                goto exit_abort;
            }
            IFRPacket_ReplySegment replysegment(replypacket);
            // Update the PUTVAL descriptors.
            rc = getChangedPutvalDescriptors(replysegment);
            if(rc != IFR_OK) {
                goto exit_abort;
            }
            // Adjust the data offset
            if(previousoffset != readoffset) {
                IFR_Length writtenBytes = readoffset - previousoffset;
                char *tmp = (char *)parameterdata;
                tmp += writtenBytes;
                parameterdata = (void *) tmp;
                if(used_lengthindicator) {
                    used_length = *used_lengthindicator;
                    used_lengthindicator = &used_length;

                    if(used_length != IFR_NTS) {
                        used_length -= writtenBytes;
                        if(used_length < 0) {
                            rc = IFR_NOT_OK;
                            goto exit_abort;
                        }
                    }
                }
            }
            // Check if there was no DATA_TRUNC at insert.
            if(putval_rc == IFR_OK) {
                break;
            }
            readoffset = 0;
        }
    }

finish_ok:
    DBUG_RETURN(rc);

exit_abort:
    IFR_ErrorHndl tmperror(allocator, getConnection());
    tmperror.assign(error(), false);
    abortPutval(pv);
    error().assign(tmperror, false);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::abortPutval(IFRConversion_Putval *pv)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, abortPutval);

    IFRPacket_RequestPacket  requestpacket(*this);
    IFRPacket_ReplyPacket    replypacket;
    IFRPacket_RequestSegment requestsegment;
    IFRPacket_LongDataPart   datapart;
    IFR_Retcode              rc;

    // Invalidate (but not delete) all LOBs
    invalidateLOBs();

    m_Connection->getRequestPacket(requestpacket, error());
    rc=putvalInitPacket(requestpacket, requestsegment, datapart);

    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    }

    pv->putDescriptor(datapart); // longdatapart.addDescriptor increases argument count
    pv->setStreamValMode(IFRPacket_LongDescriptor::Error_C);
    requestsegment.closePart();
    requestsegment.close();
    IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute(requestpacket,
                                                           replypacket,
                                                           IFR_Connection::AppendNotAllowed_C,
                                                           error());
    if(sqlaexecute_rc != IFR_OK || error()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(IFR_OK);

}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::getCurrentData (IFRPacket_DataPart& part)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getCurrentData);
    if (!m_copieddata)
      DBUG_RETURN(IFR_NOT_OK);
    part = (*m_copieddata);
    if(part.isValid()) {
        DBUG_RETURN(IFR_OK);
    } else {
        DBUG_RETURN(IFR_NOT_OK);
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::closeCurrentPutData()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, closeCurrentPutData);

    // Only if there was a putval.
    if(m_paramdata->lastputval != 0) {
        IFRConversion_Putval *pv = m_paramdata->lastputval;
        if(!m_paramdata->requestpacket.isValid()) {
            IFR_Retcode rc=putvalInitPacket(m_paramdata->requestpacket,
                                            m_paramdata->segment,
                                            m_paramdata->datapart);
            if(rc != IFR_OK) {
                DBUG_RETURN(rc);
            }
        }
        if(m_paramdata->isexecute) {
            pv->setStreamValMode(IFRPacket_LongDescriptor::AllData_C);
        } else {
            if(m_paramdata->putvalstreamtruncated) {
                pv->setStreamValMode(IFRPacket_LongDescriptor::LastData_C);
            } else {
                pv->setStreamValMode(IFRPacket_LongDescriptor::AllData_C);
            }
        }
    }

    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
void
IFR_PreparedStmt::clearParamData()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, clearParamData);
    m_status=Status_Other_C;
    IFRUtil_Delete(m_paramdata, allocator);
    m_paramdata = 0;
}



//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::prepare(const char *sql, IFR_StringEncoding encoding)
{
  return this->prepare(sql, IFR_NTS, encoding);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::prepare(const char *sql, const IFR_Length sqlLength, IFR_StringEncoding encoding)
{
    IFR_Bool memory_ok=true;
    IFR_String sqlcommand(sql, sqlLength, encoding, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        return IFR_NOT_OK;
    }
    return this->prepare(sqlcommand);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::prepare(const IFR_String& sql)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, prepare);
    DBUG_PRINT(sql);

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::PARSE " << m_CursorName << " "
                  << currenttime << " [0x" << (void*)this << "]" << endl
                  << "SQL COMMAND:" << sql << endl;
    IFR_SQL_TRACE_IF((sql.getEncoding() != IFR_StringEncodingAscii))
      << "ENCODING : " << sql.getEncoding() << endl;
    //<<< SQL TRACE ENTRY

    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFR_STMT_PROFILE_COUNT_INC(IFR_PREPARECOUNT);

    error().clear();

    ClearCommandInfo cmdInf (&m_commandInfo, m_commandInfoState);

    IFR_Retcode rc = this->doParse (sql);

    if(rc != IFR_OK) {
        m_commandInfoState = CommandInfoError_C;
    } else if(m_commandInfoState == CommandInfoSet_C) {
        m_commandInfoState = CommandInfoExecuted_C;
    }

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::getStatement(char *buffer,
                               IFR_StringEncoding& encoding,
                               IFR_Length bytelength,
                               IFR_Length *length)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getStatement);
    DBUG_PRINT(bytelength);
    DBUG_PRINT(length);

    if (!m_parseinfo) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NOTPREPARED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    const IFR_String& sqlcmd = m_parseinfo->getSQLCommand();
    encoding = sqlcmd.getEncoding();
    IFR_Length usedlength = sqlcmd.getLength();
    IFR_Int4 terminatorsize = 1;
    if(encoding==IFR_StringEncodingUCS2 ||
       encoding==IFR_StringEncodingUCS2Swapped) {
        ++terminatorsize;
    }
    if(length) {
        *length = usedlength;
    }
    IFR_Retcode rc=IFR_OK;
    if(usedlength > bytelength - terminatorsize) {
        usedlength = bytelength - terminatorsize;
        rc = IFR_DATA_TRUNC;
    }
    memcpy(buffer, sqlcmd.getBuffer(), usedlength);
    for(IFR_Int4 i=0; i<terminatorsize; ++i) {
        buffer[usedlength +i]='\0';
    }
    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeBatchInitPacket(IFRPacket_RequestPacket& requestpacket,
                                         IFRPacket_RequestSegment& segment,
                                         IFRPacket_LongDataPart& longdatapart,
                                         IFR_Int4 resultCount,
                                         IFR_ErrorHndl & used_error)
{
    IFRPacket_ParseIDPart parseidpart;
    IFRPacket_ResultCountPart resultcountpart;
    return executeBatchInitPacket(requestpacket, segment, longdatapart, resultCount, parseidpart, resultcountpart, used_error);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeBatchInitPacket(IFRPacket_RequestPacket& requestpacket,
                                         IFRPacket_RequestSegment& segment,
                                         IFRPacket_LongDataPart& longdatapart,
                                         IFR_Int4 resultCount,
                                         IFRPacket_ParseIDPart& parseidpart,
                                         IFRPacket_ResultCountPart& resultcountpart,
                                         IFR_ErrorHndl & used_error)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeBatchInitPacket);
    DBUG_PRINT(resultCount);

    IFR_Retcode rc = IFR_OK;

    m_parseinfo->lock();

    IFR_Bool massCmd = m_parseinfo->isMassCommand();
    DBUG_PRINT(massCmd);
    // because of parse again handling we operate not on a root packet.
    m_Connection->getRequestPacket(requestpacket, used_error, IFRPacket_RequestPacket::Dynamic_C);
    segment = IFRPacket_RequestSegment(requestpacket, IFRPacket_CommandMessageType::Execute_C);
    if(massCmd) {
        requestpacket.setMassCommand();
    }

    if(m_Connection->getAutoCommit()) {
        segment.setCommitImmediately();
    }

    IFR_ParseID *parseid_ptr=(massCmd ? m_parseinfo->getMassParseID() : m_parseinfo->getParseID());
    segment.closePart();
    rc = segment.addPart(parseidpart);
    if(rc == IFR_OK) {
        rc = parseidpart.addParseID(*parseid_ptr);
    }
    if(rc != IFR_OK) {
        used_error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
        m_parseinfo->unlock();
        DBUG_RETURN(IFR_NOT_OK);
    }
    segment.closePart();
    if(massCmd) {
        // segment.closePart();
        rc = segment.addPart(resultcountpart);
        if(rc == IFR_OK) {
            if(resultCount == -1) {
                rc = resultcountpart.setUndefResultCount();
            } else {
                rc = resultcountpart.setResultCount(resultCount);
            }
        }
        if(rc != IFR_OK) {
            used_error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
            m_parseinfo->unlock();
            DBUG_RETURN(IFR_NOT_OK);
        }
        segment.closePart();
    } else {
        resultcountpart.Invalidate();
    }
    IFR_Int2 paramcount=m_parseinfo->getDirectParameterCount();
    DBUG_PRINT(paramcount);
    if(paramcount > 0) {
        if(segment.addPart(m_parseinfo->hasVariableInput()?IFRPacket_PartKind::VarData_C:IFRPacket_PartKind::Data_C,
                           longdatapart) != IFR_OK) {
            m_parseinfo->unlock();
            used_error.setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(massCmd && resultCount == -1) {
            longdatapart.setFirstPacket();
        }
        longdatapart.setRecordSize(m_parseinfo->getInputParameterRowSize());
    }

    m_parseinfo->unlock();

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeBatch()
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeBatch);


    if(m_commandInfoState != CommandInfoNotSet_C) {
        m_commandInfoState = CommandInfoExecuted_C;
    }

    switch(m_status) {
    case Status_Other_C:
        break;
    case Status_LobsClosed_C:
        clearParameters(false);
        break;
    case Status_Keep_C:
    default:
        error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    // check whether we have to do the mass command stuff or not ...
    if(m_rowarraysize == 1) {
        DBUG_RETURN(execute());
    }

    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::EXECUTE BATCH " << m_CursorName << " "
                  << currenttime << " [0x" << (void*)this << "]" << endl;
    IFR_SQL_TRACE << "BATCH SIZE: " << m_rowarraysize << endl;
    //<<< SQL TRACE ENTRY

    // Initialise the row status array with NOT OK,
    // we will only change this for successfully executed
    // statements later
    for(IFR_UInt4 i=0; i<(IFR_UInt4)m_rowarraysize; ++i) {
        m_rowstatusarray[i]=IFR_EXECUTE_FAILED;
    }

    if(assertOpen()) {
        DBUG_RETURN(IFR_NOT_OK);
    }

    // check that command is parsed ...
    if(!m_parseinfo) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NOTPREPARED);
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_STMT_PROFILE_COUNT_INC(IFR_EXECUTECOUNT);
    this->functionCode2Statistic(m_parseinfo->getFunctionCode());

    //<<< SQL TRACE
    IFR_SQL_TRACE << "PARSE ID: " << *m_parseinfo << endl;
    IFR_SQL_TRACE << "SQL COMMAND: " << (m_parseinfo->getSQLCommand()) << endl;
    IFR_SQL_TRACE_IF((m_parseinfo->getSQLCommand().getEncoding() != IFR_StringEncodingAscii))
      << "ENCODING : " << m_parseinfo->getSQLCommand().getEncoding() << endl;
    //>>> SQL TRACE

    // check that command is not a select
    if(m_parseinfo->isQuery()) {
        error().setRuntimeError(IFR_ERR_SQLCMD_RESULTSET);
        DBUG_RETURN(IFR_NOT_OK);
    }

    error().clear();

    // check that there are only input params
    IFR_Int2 paramcount=m_parseinfo->getDirectParameterCount();
    IFR_Bool no_async_op = paramcount == 0;
    IFRConversion_ConverterList& shortinfos=m_parseinfo->getParameterInfos();
    for(IFR_Int2 j=0; j<paramcount; ++j) {
        if(shortinfos[j]->supportsOutput()) {
            error().setRuntimeError(IFR_ERR_SQLCMD_OUTPUTPARAM);
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(shortinfos[j]->isLong()) {
            no_async_op = true;
        }
    }

    IFR_Retcode rc = IFR_OK;
    IFR_Int4 inputcursor=0;
    IFR_Int4 executeCount=-1;

    IFRPacket_RequestPacket requestpacket(*this);
    IFRPacket_RequestSegment segment;
    IFRPacket_LongDataPart datapart;
    int recursioncounter=0;
    IFR_Int4   firstrecord;
    IFR_Bool   massCmd=m_parseinfo->isMassCommand();
    IFR_UInt4  recordSize = m_parseinfo->getInputParameterRowSize();
    IFR_Retcode conversion_rc = IFR_OK;
    IFR_ErrorHndl tmperror(allocator, getConnection());

    IFR_BatchFillOperation *batchfill_operation = 0;

    while(inputcursor < m_rowarraysize) {
        if(batchfill_operation == 0 || !batchfill_operation->isExecuted()) {
            firstrecord=inputcursor;
            rc = executeBatchInitPacket(requestpacket, segment, datapart, executeCount, error());
            if(rc != IFR_OK) {
                IFRUtil_Delete(batchfill_operation, allocator);
                DBUG_RETURN(rc);
            }

            // append a data part, if this has parameters
            if(paramcount != 0) {
                IFR_Int4 last_inputcursor=inputcursor;
                conversion_rc = executeBatchFillData(requestpacket,
                                                     segment,
                                                     datapart,
                                                     inputcursor,
                                                     firstrecord,
                                                     recordSize,
                                                     massCmd,
                                                     executeCount,
                                                     0,
                                                     error());

                switch(conversion_rc) {
                case IFR_NEED_DATA:
                    rc = conversion_rc;
                    break;
                case IFR_OK:
                    break;
                default:
                    if(last_inputcursor != inputcursor) {
                        tmperror.assign(error(), false);
                        error().clear();
                    } else {
                        rc=conversion_rc;
                    }
                }

                if(rc != IFR_OK) {
                    if(rc != IFR_NEED_DATA) {
                        clearInputLongs();
                    }
                    IFRUtil_Delete(batchfill_operation, allocator);
                    DBUG_RETURN(rc);
                }

                segment.closePart();
            } else {
                // commands w/o parameters are not mass-capable
                ++inputcursor;
            }

            if(!no_async_op) {
                if(batchfill_operation == 0) {
                    batchfill_operation = new IFR_ALLOCATOR(allocator) IFR_BatchFillOperation(this,
                                                                                              inputcursor,
                                                                                              requestpacket,
                                                                                              segment,
                                                                                              paramcount,
                                                                                              massCmd,
                                                                                              recordSize);
                } else {
                    batchfill_operation->reset(inputcursor);
                }
            }

            IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, datapart.getBufferLength());
            rc = executeBatchSendCommand(requestpacket,
                                         segment,
                                         inputcursor,
                                         firstrecord,
                                         executeCount,
                                         massCmd,
                                         batchfill_operation);

            if(rc == IFR_OK && batchfill_operation && batchfill_operation->isExecuted()) {
                batchfill_operation->setResultCount(executeCount);
            }
        } else {

            // pick up the pieces left by the operation
            IFR_Int4 last_inputcursor = batchfill_operation->last_inputcursor;
            inputcursor = batchfill_operation->inputcursor;
            firstrecord = batchfill_operation->firstrecord;
            conversion_rc = batchfill_operation->rc;
            if(batchfill_operation->error) {
                error().assign(batchfill_operation->error, false);
            }

            // same as above, with same implications.
            switch(conversion_rc) {
            case IFR_NEED_DATA:
                rc = conversion_rc;
                break;
            case IFR_OK:
                break;
            default:
                if(last_inputcursor != inputcursor) {
                    tmperror.assign(error(), false);
                    error().clear();
                } else {
                    rc=conversion_rc;
                }
            }

            if(rc != IFR_OK) {
                if(rc != IFR_NEED_DATA) {
                    clearInputLongs();
                }
                IFRUtil_Delete(batchfill_operation, allocator);
                DBUG_RETURN(rc);
            }

            batchfill_operation->switchPackets();


            IFR_STMT_PROFILE_COUNT_8_ADD(IFR_DATAWRITESIZE, batchfill_operation->datalength);
            IFR_Int4 single_executecount = 0;
            rc = executeBatchSendCommand(*batchfill_operation->snd_packet,
                                         *batchfill_operation->snd_segment,
                                         inputcursor,
                                         firstrecord,
                                         single_executecount,
                                         massCmd,
                                         batchfill_operation);

            executeCount = single_executecount;
            if(batchfill_operation->isExecuted()) {
                batchfill_operation->setResultCount(executeCount);
            }
        }

        if(rc!=IFR_OK) {
            if(rc != IFR_NEED_DATA) {
                clearInputLongs();
            }
            IFRUtil_Delete(batchfill_operation, allocator);
            DBUG_RETURN(rc);
        }
        if(conversion_rc != IFR_OK) {
            error().assign(tmperror, false);
            clearInputLongs();
            IFRUtil_Delete(batchfill_operation, allocator);
            DBUG_RETURN(conversion_rc);
        }
    }
    m_RowsAffected = executeCount;
    clearInputLongs(); // The last input long is still open here ...
    IFRUtil_Delete(batchfill_operation, allocator);
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_ParameterMetaData*
IFR_PreparedStmt::getParameterMetaData()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getParameterMetaData);
    clearError();
    DBUG_RETURN(static_cast<IFR_ParameterMetaData*> (this->m_parseinfo));
}

//----------------------------------------------------------------------
IFR_ResultSetMetaData* IFR_PreparedStmt::getResultSetMetaData()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getResultSetMetaData);
    clearError();
    if(m_parseinfo==0) {
        error().setRuntimeError(IFR_ERR_SQLCMD_NOTPREPARED);
        DBUG_RETURN((IFR_ResultSetMetaData*)0);
    }

    if(!m_parseinfo->getFunctionCode().isQuery()) {
        DBUG_RETURN((IFR_ResultSetMetaData*)0);
    }

    if (this->m_parseinfo->getDirectColumnCount() == IFR_ParseInfo::UnknownColumnCount_C
        && this->m_parseinfo->getFunctionCode() != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC) {
        if(this->m_parseinfo->describeParseID (this->getCommandEncoding(), *this) != IFR_OK) {
            DBUG_RETURN((IFR_ResultSetMetaData*)0);
        }
    }

    DBUG_RETURN(static_cast<IFR_ResultSetMetaData*> (this->m_parseinfo));
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::addApplicationInfo(IFRPacket_RequestSegment& requestsegment)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, addApplicationInfo);

    if(m_parseinfo && !m_parseinfo->isParsedWithParamDesc()) {
        DBUG_RETURN(IFR_OK);
    }

    IFR_size_t paramvector_size=m_paramvector.GetSize();
    DBUG_PRINT(paramvector_size);

    if(paramvector_size == 0) {
        DBUG_RETURN(IFR_OK);
    }

    // application_info_request part: 16 byte part header
    //                                + 4 byte for each parameter.
    // If it does not fit, just give up. and let them out.
    if(requestsegment.remainingBytes() - 8 < (IFR_Int4) (paramvector_size * 4 + 16)) {
        m_parsed_with_paramdesc = false;
        DBUG_RETURN(IFR_OK);
    }

    IFRPacket_ApplParamPart applparam_part;
    IFR_Retcode rc=requestsegment.addPart(applparam_part);

    for(unsigned int i=0; i<paramvector_size; ++i) {
        applparam_part.addArgument(m_paramvector[i].getPreferredSQLType(),
                                   m_paramvector[i].getPreferredPrecision(),
                                   m_paramvector[i].getPreferredLength()
            );
    }
    requestsegment.closePart();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Int4
IFR_PreparedStmt::getParameterInfoHash()
{
    if(m_param_info_hash >=0) {
        return m_param_info_hash;
    } else {
        IFR_size_t paramvec_size = m_paramvector.GetSize();
        if(paramvec_size == 0) {
            m_param_info_hash = 0;
            return m_param_info_hash;
        }
        IFR_UInt4 hash = paramvec_size;
        for(IFR_size_t i=0; i<paramvec_size; ++i) {
            IFR_UInt4 tmp = (IFR_UInt4)m_paramvector[i].getPreferredSQLType();
            if(tmp) {
                hash = (hash << 5) - hash + tmp;
            }
            tmp = (IFR_UInt4)m_paramvector[i].getPreferredPrecision();
            if(tmp) {
                hash = (hash << 5) - hash + tmp;
            }
            tmp = (IFR_UInt4) m_paramvector[i].getPreferredLength();
            if(tmp) {
                hash = (hash << 5) - hash + tmp;
            }
        }
        hash = hash & 0x7FFFFFFF;
        m_param_info_hash = (IFR_Int4) hash;
        return m_param_info_hash;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_PreparedStmt::parseAsMassCommand(const IFR_String& sql) const
{
    if(IFR_Statement::isQuery(sql)) {
        return getRowArraySize() > 1;
    } else {
        return true;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleStreamsForExecute(IFRPacket_DataPart& dataPart)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleStreamsForExecute);

    IFR_Retcode rc = IFR_OK;

    IFR_Int2 inputlong_count = getInputLongCount();

    for(IFR_Int2 i = 0; i<inputlong_count; ++i) {
        IFRConversion_Putval *pv = getInputLong(i);
        if(!pv->hasDataAtExecute()) {
            rc = pv->transferStream(dataPart);
            if(rc != IFR_OK) {
                break;
            }
        }
    }

    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::getChangedPutvalDescriptors (IFRPacket_ReplySegment& replySegment)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getChangedPutvalDescriptors);
    IFRPacket_LongDataPart longdatapart;
    if (replySegment.getPart (longdatapart) != IFR_OK) {
        DBUG_RETURN(IFR_OK);
    }

    IFR_Int2 longdata_arguments=longdatapart.getPartArguments();
    IFR_Int2 valindex;
    char  *descrptr=longdatapart.getOutputData(0) + 1;
    // We have saved our index in VALIND, thus we can use it to
    // select the correct descriptor.
    for(IFR_Int2 i=0; i<longdata_arguments; ++i) {
        memcpy(&valindex, descrptr + IFRPacket_LongDescriptor::ValInd_O, sizeof(IFR_Int2));
        IFRConversion_Putval *pv = 	getInputLong(valindex);
        if(pv) {
            pv->updateDescriptor(descrptr);
        } else {
            error().setRuntimeError(IFR_ERR_INVALID_VALINDEX_I, (IFR_Int4)valindex);
            DBUG_RETURN(IFR_NOT_OK);
        }
        descrptr += IFRPacket_LongDescriptor::Size + 1;
    }

    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleStreamsForPutval(IFRPacket_ReplyPacket& firstReplyPacket,
                                         IFRPacket_ReplySegment& replySegment)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleStreamsForPutval);

	IFRPacket_RequestPacket  requestpacket(*this);
	IFRPacket_RequestSegment requestsegment;
    IFRPacket_LongDataPart   longdata;

    DBUG_RETURN(handleStreamsForPutval(firstReplyPacket,
                                       replySegment,
                                       requestpacket,
                                       requestsegment,
                                       longdata,
                                       true));
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleStreamsForPutval(IFRPacket_ReplyPacket& firstReplyPacket,
                                         IFRPacket_ReplySegment& replySegment,
                                         IFRPacket_RequestPacket& requestpacket,
                                         IFRPacket_RequestSegment& requestsegment,
                                         IFRPacket_LongDataPart& longdata,
                                         IFR_Bool ignore_data_at_execute)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleStreamsForPutval);
    // get the descriptors from the response of
    // the execute, and unlock the reply packet
    IFR_Retcode rc = getChangedPutvalDescriptors(replySegment);
    firstReplyPacket.releaseLock();
    if(rc!=IFR_OK) {
    	DBUG_RETURN(rc);
    }

    IFRConversion_ConverterList& conv=m_parseinfo->getParameterInfos();
    IFR_Int2            paramcount=m_parseinfo->getDirectParameterCount();

	IFRPacket_ReplyPacket   replypacket;
	m_Connection->getRequestPacket(requestpacket, error(), IFRPacket_RequestPacket::Dynamic_C);
	requestsegment = IFRPacket_RequestSegment(requestpacket,
                                            IFRPacket_CommandMessageType::Putval_C);
	requestsegment.addPart(longdata);

	IFR_Bool sendNeeded=false;
	IFR_Bool mustClose = false;
    IFR_Bool hasdataatexecute = false;

    IFR_Int2 inputlong_count = getInputLongCount();
    for(IFR_Int2 i=0; i<inputlong_count; ++i) {
        IFRConversion_Putval *pv = getInputLong(i);
        if(pv->hasDataAtExecute()) {
            if(!ignore_data_at_execute) {
                hasdataatexecute = true;
            }
            continue; // we cannot process it, so coni
        }

        // if the PUTVAL is already written, skip
        // otherwise continue until it is there.
        IFR_Bool empty = false;

        while(!pv->atEnd() && !empty) {
            empty = false;

            IFR_Retcode rc=pv->transferStream(longdata, empty);

            if(rc == IFR_DATA_TRUNC) {

                // finish the segment
                requestsegment.closePart ();
                requestsegment.close ();

                // send, check for error.
                IFR_STMT_PROFILE_COUNT_8_ADD(IFR_LONGDATAWRITESIZE, longdata.getBufferLength());
                IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute
                    (requestpacket, replypacket, IFR_Connection::AppendNotAllowed_C, error());
                if(sqlaexecute_rc != IFR_OK || error()) {
                    DBUG_RETURN(IFR_NOT_OK);
                }

                IFRPacket_ReplySegment replysegment(replypacket);
                // Let's get the changed descriptors.
                IFR_Retcode desc_rc=getChangedPutvalDescriptors(replysegment);
                if(desc_rc != IFR_OK) {
                    DBUG_RETURN(desc_rc);
                }

                sendNeeded = false;
                replypacket.releaseLock();
                m_Connection->getRequestPacket(requestpacket, error());
                requestsegment = IFRPacket_RequestSegment(requestpacket,
                                                          IFRPacket_CommandMessageType::Putval_C);
                requestsegment.addPart(longdata);

            } else if(rc == IFR_OK) {
                // All LONG data did fit. This means, there is (or may be)
                // still room in the packet for more data here, but we have
                // to mark that there is data left over to be sended.
                sendNeeded=true;
                if(i == inputlong_count - 1) {
                    mustClose=true;
                }
            }
        }
    }

    IFR_Bool requiresTrailingPacket=false;

    if(sendNeeded && !hasdataatexecute) {
        if(mustClose) {
            if(longdata.closePutval() != IFR_OK) {
                requiresTrailingPacket=true;
            }
        }

        // finish the segment
        requestsegment.closePart ();
        requestsegment.close ();

        IFR_STMT_PROFILE_COUNT_8_ADD(IFR_LONGDATAWRITESIZE, longdata.getBufferLength());
        IFR_Retcode sqlaexecute_rc = m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::AppendNotAllowed_C, error());
        if(sqlaexecute_rc != IFR_OK || error()) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        // Maybe we have to send the close command alone.
        if(requiresTrailingPacket) {
            replypacket.releaseLock();
            IFR_Retcode rc=sendTrailingPutvalClose();
            if(rc != IFR_OK) {
                DBUG_RETURN(rc);
            }
        }
    }
    if(hasdataatexecute) {
        DBUG_RETURN(IFR_NEED_DATA);
    } else {
        DBUG_RETURN(IFR_OK);
    }
}

//---------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::handleStreamsForGetval (IFRPacket_ReplyPacket& oldreplypacket,
                                                      IFRPacket_DataPart& part,
                                                      IFRConversion_ConverterList& cv,
                                                      IFR_Retcode *columnstatus)
{
  DBUG_METHOD_ENTER(IFR_PreparedStmt, handleStreamsForGetval);

  IFR_Int4 infocount = cv.size();
  IFR_Int4 paramvector_size=m_paramvector.GetSize();
  IFR_Bool memory_ok = true;
  IFR_Retcode     rc = IFR_OK;
  IFR_Int4 i;

  for(i=0; i<infocount; ++i) {
      if(cv[i]->isLong()) {
          // !!! We create a GETVAL instance for all LONG values, to close them
          //     later when reading is done. Some LONGs may be opened due to the
          //     SELECT statement but never touched, but we have to close them,
          //     too.
          // !!!
          IFRConversion_Getval *getval = 0;
          if(i < paramvector_size) {
              getval = cv[i]->createGetval(part,
                                           &m_paramvector[i],
                                           *this,
                                           1,
                                           0,
                                           0);
          } else {
              getval = cv[i]->createGetval(part, 0, *this, 1, 0, 0);
          }
          if(getval == 0) {
              if(error()) {
                  DBUG_RETURN(IFR_NOT_OK);
              }
          } else {
              addOutputLong(getval, memory_ok);
              if(!memory_ok) {
                  IFRUtil_Delete(getval, allocator);
                  closeOutputLongs();
                  oldreplypacket.releaseLock();
                  error().setMemoryAllocationFailed();
                  DBUG_RETURN(IFR_NOT_OK);
              }
          }
      }
  }

  IFR_Int4 outputlongs_size = getOutputLongCount();
  IFRUtil_Vector<IFRConversion_GetvalInfo> getvalinfo(allocator);

  for(i=0; i<outputlongs_size; ++i) {
      IFRConversion_Getval *getval = getOutputLong(i);
      IFR_Int4 column = getval->getColumn();

    // Skip those GETVALs which are added only so that they can later
      // be closed.
      if(column > paramvector_size || m_paramvector[column - 1].isNotSet()) {
          continue;
      }
      IFR_Parameter* pcurrentParameter = &(m_paramvector[column - 1]);
      // We leave out also all parameters that are LOBs.
      if(pcurrentParameter->isLOB()) {
          continue;
      }

      IFRConversion_Converter *converter = cv[column-1];

      IFR_Length  offset          = 0;
      IFR_Length dataoffset       = 0;
      IFR_Length  datalength      = pcurrentParameter->getBytesLength();
      IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator();
      if(lengthindicator) {
          *lengthindicator = 0;
      }

      const IFR_Length *posindicator = pcurrentParameter->getPosIndicator();
      if(posindicator) {
          offset = *posindicator;
      }
      rc = converter->getData(part,
                              (void *) pcurrentParameter->data(),
                              lengthindicator,
                              *pcurrentParameter,
                              dataoffset,
                              offset,
                              *this,
                              datalength,
                              getval);
      if(columnstatus) {
          columnstatus[column -1] = rc;
      }
      if(rc == IFR_NEED_DATA) {
          IFRConversion_GetvalInfo gvinfo(converter, getval, dataoffset, datalength);
          getvalinfo.InsertEnd(gvinfo, memory_ok);
          if(!memory_ok) {
              // Order is important, as 'close output longs' may send a trailing getval command.
              oldreplypacket.releaseLock();
              closeOutputLongs();
              error().setMemoryAllocationFailed();
              DBUG_RETURN(IFR_NOT_OK);
          }
      } else if(rc != IFR_OK && rc != IFR_DATA_TRUNC) {
          oldreplypacket.releaseLock();
          DBUG_RETURN(rc);
      }

  }

  oldreplypacket.releaseLock();

  IFR_Length getvalinfo_size = getvalinfo.GetSize();

  if(getvalinfo_size != 0) {
      IFRPacket_RequestPacket requestpacket(*this);
      IFRPacket_ReplyPacket replypacket;
      IFRPacket_RequestSegment segment;
      IFRPacket_LongDataPart longdatapart;

      // The getvalinfo size decreases while reading the longs.
      while(getvalinfo_size != 0) {
          getConnection()->getRequestPacket(requestpacket, error());
          getvalInitPacket(requestpacket, segment, longdatapart);
          // Put all remaining longs in.
          for(IFR_Int4 j=0; j<getvalinfo_size; ++j) {
              getvalinfo[j].getval->putDescriptor(longdatapart);
          }
          segment.closePart();
          segment.close();

          IFR_Retcode sqlaexecute_rc = getConnection()->sqlaexecute(requestpacket,
                                                                    replypacket,
                                                                    IFR_Connection::AppendNotAllowed_C, error());
          if (sqlaexecute_rc != IFR_OK) {
              DBUG_RETURN(sqlaexecute_rc);
          } else if(error()) {
              DBUG_RETURN(IFR_NOT_OK);
          }

          IFRPacket_ReplySegment replysegment(replypacket);
          IFR_Int4 updated_longcount;
          updateOutputLongs(replysegment, *this, updated_longcount, this->m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);

          if ((rc = replysegment.getPart (longdatapart)) != IFR_OK) {
              error().setRuntimeError(IFR_ERR_MISSINGLONGDATAPART);
              DBUG_RETURN(rc);
          }

          for(IFR_Int4 i = 0; i < getvalinfo_size; ++i) {
              IFRConversion_GetvalInfo* pcurrentinfo = &(getvalinfo[i]);
              IFR_Int4 col = pcurrentinfo->getval->getColumn();
              IFR_Parameter* pcurrentParameter = &(m_paramvector[col - 1]);
              IFR_Length old_dataoffset = pcurrentinfo->offset;
              IFR_Length offset_dummy = 0;
              IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator();

              // The length indicator is either NULL or contains what we have
              // already read before ...

              rc = pcurrentinfo->converter->getData(longdatapart,
                                                    (void *) pcurrentParameter->data(),
                                                    lengthindicator,
                                                    *pcurrentParameter,
                                                    pcurrentinfo->offset,
                                                    (offset_dummy=0),
                                                    *this,
                                                    pcurrentinfo->datalength,
                                                    pcurrentinfo->getval);
              if(columnstatus) {
                  columnstatus[col - 1] = rc;
              }
              // Check whether anything has been written, and reduce the 'updated long count' by one.
              if(pcurrentinfo->offset != old_dataoffset) {
                  --updated_longcount;
              }
              if(rc == IFR_OK || rc == IFR_DATA_TRUNC) {
                  // After the fetch, the read offset is set back to 1.
                  pcurrentinfo->getval->setReadOffset(1);
                  // this long is finished, exchange it with the last LONG that is to be processed
                  if(i == getvalinfo_size - 1) {
                      --getvalinfo_size;
                  } else {
                      getvalinfo[i] = getvalinfo[getvalinfo_size - 1];
                      --getvalinfo_size;
                      --i;               // we must run again through the same as we changed order
                  }

                  if(rc == IFR_OK) {
                      IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator();
                      if(lengthindicator && *lengthindicator == IFR_NO_TOTAL) {
                          *lengthindicator = pcurrentinfo->offset;
                      }
                  }

              } else if(rc != IFR_NEED_DATA) {
                  DBUG_RETURN(rc);
              }
              // If all other longs will not return data, we will not bother them, but make the
              // next getval. Often, only one long is in the GETVAL, because its large.
              if(updated_longcount == 0) {
                  break;
              }
          }
          replypacket.releaseLock();
      }
  }

  DBUG_RETURN(rc);
}

//---------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::getvalInitPacket(IFRPacket_RequestPacket& requestpacket,
					       IFRPacket_RequestSegment& segment,
					       IFRPacket_LongDataPart &datapart)
{
  DBUG_METHOD_ENTER(IFR_PreparedStmt, getvalInitPacket);

  segment = IFRPacket_RequestSegment(requestpacket,
				     IFRPacket_CommandMessageType::Getval_C);

  IFR_Retcode rc = segment.addPart(datapart);
  if (rc != IFR_OK) {
    error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
    DBUG_RETURN(rc);
  }
    DBUG_RETURN(IFR_OK);
}

//---------------------------------------------------------------------
IFR_Retcode IFR_PreparedStmt::createResultSet (IFR_String& tableName,
                                               IFR_String& cursorName,
                                               IFR_Bool rowNotFound,
                                               IFRConversion_ConverterList& info,
                                               IFRUtil_Vector<IFR_String>* colName,
                                               IFR_Bool closeOnLast,
                                               IFR_FetchChunk *firstChunk)
{
  DBUG_METHOD_ENTER(IFR_PrepardStatement, createResultSet);
  DBUG_PRINT(tableName);
  DBUG_PRINT(cursorName);
  DBUG_PRINT(rowNotFound);
  IFR_Retcode rc = IFR_OK;
  IFR_Bool memory_ok=true;
  IFR_FetchInfo *fetchInfo = new IFR_ALLOCATOR(allocator) IFR_FetchInfo (*this, info, colName, memory_ok);
  if(fetchInfo == 0) {
      m_resultset = 0;
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  } else if(!memory_ok) {
      IFRUtil_Delete(fetchInfo, allocator);
      m_resultset = 0;
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }

  if (info.size() == 0 || colName == 0) {
      IFR_ParseInfo *pi = this->m_parseinfo;
      IFR_Int2 colcnt = pi->getDirectColumnCount();
      if (colcnt != IFR_ParseInfo::UnknownColumnCount_C) {
          fetchInfo->setMetaDataRefFromParseinfo(m_parseinfo->getColumnInfos(),colcnt);
      } else {
          // The kernel does/did sometimes on parsed statements not send the needed
          // information. If SELECT/FETCH optimization is on, the cursor cannot be
          // described, as it is gone when the statement is/was executed.
          if(m_parseinfo->getFunctionCode() != IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC) {
              if(this->m_parseinfo->describeParseID (this->getCommandEncoding(), *this) != IFR_OK) {
                  IFRUtil_Delete(fetchInfo, allocator);
                  DBUG_RETURN(IFR_NOT_OK);
              }
              fetchInfo->setMetaDataRefFromParseinfo(m_parseinfo->getColumnInfos(), m_parseinfo->getDirectColumnCount());
          } else {
              rc = fetchInfo->describe();
              if(rc != IFR_OK) {
                  error().assign(fetchInfo->error(), true); // submit error from fetchinfo
                  IFRUtil_Delete(fetchInfo, allocator);
                  DBUG_RETURN(IFR_NOT_OK);
              }
          }
      }
  }

  this->m_cursorstate = CursorStateUsed_C;
  this->m_resultset = new IFR_ALLOCATOR(allocator) IFR_ResultSet (*this->m_Connection,
                                                                  *fetchInfo,
                                                                  *this,
                                                                  this->m_FetchSize,
                                                                  this->m_MaxRows,
                                                                  this->m_ResultSetConcurrency,
                                                                  this->m_ResultSetType,
                                                                  rowNotFound,
                                                                  firstChunk,
                                                                  closeOnLast,
                                                                  memory_ok);
  if(this->m_resultset == 0) {
      error().setMemoryAllocationFailed();
      rc=IFR_NOT_OK;
  } else if(!memory_ok) {
      IFRUtil_Delete(m_resultset, allocator);
      m_resultset=0;
      error().setMemoryAllocationFailed();
      rc=IFR_NOT_OK;
  }

  DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeBatchFillData(IFRPacket_RequestPacket&  requestpacket,
                                       IFRPacket_RequestSegment& requestsegment,
                                       IFRPacket_LongDataPart&   datapart,
                                       IFR_Int4              &   inputcursor,
                                       IFR_Int4              &   firstrecord,
                                       IFR_UInt4                 recordsize,
                                       IFR_Bool                  massCommand,
                                       IFR_Int4                  executecount,
                                       IFR_Int2                  startindex,
                                       IFR_ErrorHndl&            used_error

    )
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeBatchFillData);
    DBUG_PRINT(inputcursor);
    DBUG_PRINT(firstrecord);
    DBUG_PRINT(recordsize);
    DBUG_PRINT(massCommand);


    IFR_Int2 paramcount=m_parseinfo->getDirectParameterCount();
    IFR_Int2 inputparametercount = m_parseinfo->getInputParameterCount();
    IFR_Bool dataAtExecute = false;
    IFR_Retcode rc=IFR_OK;
    IFR_Bool firsttime = (inputcursor == 0);
    IFR_Bool memory_ok=true;
    IFR_Bool sqlTraceTableHeader=false;
    IFR_Bool variable_input = m_parseinfo->hasVariableInput();
    IFR_Bool need_clear_data = false;
    IFR_Int4 startrow = inputcursor;
    IFR_Int4 last_length = 0;

    while(inputcursor < m_rowarraysize &&
          ((inputcursor - firstrecord) < MAX_IFR_INT2) &&
          ((startindex != 0) ||
           (startindex == 0) && datapart.hasRoomFor(recordsize)) ) {

        // We have always only the long values of the current row,
        // and we stop if we find data at execute (and stuck on that
        // row for that time.
        clearInputLongs();

        datapart.setCurrentRecord(inputcursor - firstrecord);
        // make room for the complete record, even in case that due to
        // data at execute it is not filled yet.
        // we store the length it had before here, so
        // it can be rolled back
        last_length = datapart.extendRow(recordsize);

        //>>> SQL TRACE
        if(startindex==0 && IFR_SQL_TRACE_ENABLED) {
            if(!sqlTraceTableHeader) {
                IFR_TRACE_STREAM << "INPUT PARAMETERS:" << endl
                                 << "APPLICATION" << endl
                                 << IFR_SQLTRACE_DATA_HEADER << endl;
                sqlTraceTableHeader = true;
            }
            {
                IFR_Int4 trc_inputcursor = inputcursor + 1;
                if(trc_inputcursor == 1  ||
                   trc_inputcursor % 10 == 0 ||
                   inputcursor == m_rowarraysize-1 ||
                   ((inputcursor - firstrecord) == MAX_IFR_INT2 - 1)) {
                    IFR_TRACE_STREAM << "ROWNO: " << trc_inputcursor << endl;
                }
            }

        }
        //<<< SQL TRACE
        if(startindex == 0) {
            datapart.setInputParameterCount(inputparametercount);
        }

        // we have set the start index to 0 in the last round
        if(need_clear_data) {
            clearParamData();
            need_clear_data = false;
        }

        IFR_Bool dae_integral_found = false;

        for(IFR_Int2 i=startindex; i<paramcount; ++i) {
            IFR_Int4 position = m_parseinfo->getParameterInfos().parameterForInputPosition(i);
            IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[position];
            if(c->supportsInput()) {

                IFR_Bool dataAtExecuteField=m_paramvector[position].hasDataAtExecute(inputcursor, m_bindingtype);
                IFR_Bool c_is_long = c->isLong();
                if(dataAtExecuteField) {
                    dataAtExecute = true;
                    if(!c_is_long) {
                        dae_integral_found = true;
                    }
                }
                if(!dataAtExecute && !c_is_long) {
                    if( (!variable_input) ||
                        (variable_input && !dataAtExecuteField) ) {
                        rc=c->translateInput(datapart,
                                             m_paramvector[position],
                                             *this,
                                             inputcursor,
                                             m_bindingtype);
                        if(rc != IFR_OK) {
                            goto conversion_error;
                        }
                    }
                }
                //<<< SQL TRACE
                if(startindex==0 && IFR_SQL_TRACE_ENABLED) {
                    m_paramvector[position].sqlTraceParameterData(IFR_TRACE_STREAM,
                                                                  position+1,
                                                                  dataAtExecute?IFR_OK:rc,
                                                                  inputcursor,
                                                                  m_bindingtype,
                                                                  0,
                                                                  0,
                                                                  dataAtExecute?false:true);
                    IFR_TRACE_STREAM << endl;
                }
                //>>> SQL TRACE
                if(rc == IFR_NOT_OK) {
                    DBUG_RETURN(IFR_NOT_OK);
                }
                if(c_is_long &&
                   ((!variable_input) || (variable_input && !dae_integral_found))) {
                    IFRConversion_Putval *putval = c->createPutval(datapart,
                                                                   m_paramvector[position],
                                                                   *this,
                                                                   inputcursor,
                                                                   m_bindingtype);
                    // error is set in this case already
                    if(putval == 0) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    memory_ok = true;
                    addInputLong(putval, memory_ok);
                    if(!memory_ok) {
                        IFRUtil_Delete(putval, allocator);
                        used_error.setMemoryAllocationFailed();
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }

                if(dataAtExecuteField && startindex == 0) {
                    if(m_paramdata == 0) {
                        m_paramdata = new IFR_ALLOCATOR(allocator)
                            IFR_DataAtExecuteDescriptor(requestpacket,
                                                        requestsegment,
                                                        datapart,
                                                        inputcursor,
                                                        firstrecord,
                                                        executecount,
                                                        allocator);
                    }

                    if(c->isLong()) {
                        m_paramdata->addLongParameter(position, memory_ok);
                        if(!memory_ok) {
                            used_error.setMemoryAllocationFailed();
                            DBUG_RETURN(IFR_NOT_OK);
                        }
                    } else {
                        m_paramdata->addParameter(position, memory_ok);
                        if(!memory_ok) {
                            used_error.setMemoryAllocationFailed();
                            DBUG_RETURN(IFR_NOT_OK);
                        }
                    }
                } else if(startindex!=0 && dataAtExecuteField) {
                    // if we call it from the middle of a nextparameter/putdata sequence,
                    // we will leave at the first data at execute.
                    DBUG_RETURN(IFR_OK);
                }
            }
        }

        // The next row must start at the beginning, and parameter data for
        // the current row is now obsolete when coming up to here.
        if(startindex != 0) {
            startindex = 0;
            need_clear_data = true;
        }

        // If there are LONG data to insert, append it.
        if (this->m_parseinfo->hasLongs ()) {
            if(!dataAtExecute) {
                rc = this->handleStreamsForExecute (datapart);
            } else {
                // we append the long data if we are just switching to a new row ...
                if((!variable_input) ||
                   (variable_input && need_clear_data)) {
                    rc = this->handleStreamsForExecute(m_paramdata->datapart);
                    if(rc == IFR_DATA_TRUNC) {
                        m_paramdata->streamstruncated = true;
                    }
                }
            }
            DBUG_PRINT(rc);
        }

        // In case a late bound parameter was detected before, we must leave.
        if(dataAtExecute) {
            if(massCommand) {
                if(inputcursor == m_rowarraysize) {
                    datapart.setLastPacket();
                } else if(firsttime) {
                    datapart.setNextPacket();
                }
            }
            datapart.setArgCount(inputcursor - firstrecord);
            m_status = Status_ParamDataBatch_C;
            DBUG_RETURN(IFR_NEED_DATA);
        }

        datapart.moveRecordBase ();
        ++inputcursor;

        // if this is not a mass command, we must not deliver >1 data field.
        if(!massCommand) {
            break;
        }

        rc = IFR_OK;
        continue;

        // we come here in case of a conversion error above
    conversion_error:
        // the length here set will work for all kinds, as it was aquired
        // before extending the part by adding a record (normal input) resp.
        // the beginning of data insertion (variable input).
        datapart.setLength(last_length);

        // in case we are in some recursion, we need to transfer the error.
        if(&(error()) != &used_error) {
            used_error.assign(error(), false);
            error().clear();
        }

        break;
    }

    // if mass command we have also to mark the last
    // data part, or the next packet if it is
    // in the middle
    if(massCommand) {
        if(inputcursor == m_rowarraysize) {
            datapart.setLastPacket();
        } else if(firsttime) {
            datapart.setNextPacket();
        }
        datapart.setArgCount(MIN(m_rowarraysize - 1, inputcursor - 1) -  firstrecord  + 1);
    } else {
        datapart.setArgCount(1);
    }

    DBUG_RETURN(rc);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::executeBatchSendCommand(IFRPacket_RequestPacket& requestpacket,
                                          IFRPacket_RequestSegment& segment,
                                          IFR_Int4& inputcursor,
                                          IFR_Int4& firstrecord,
                                          IFR_Int4& executecount,
                                          IFR_Bool  masscommand,
                                          IFR_BatchFillOperation *operation)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, executeBatchSendCommand);
    DBUG_PRINT(inputcursor);
    DBUG_PRINT(firstrecord);
    DBUG_PRINT(executecount);
    DBUG_PRINT(masscommand);

    IFRPacket_ReplyPacket replypacket;
    IFR_Retcode rc=IFR_OK;
    IFR_Bool    memory_ok=true;

    for(IFR_Int4 retryCounter=0; retryCounter < PARSE_RETRY ; ++retryCounter) {
        IFR_size_t original_size=requestpacket.Length();
        IFR_Retcode sqlaexecute_rc=m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::AppendNotAllowed_C, error(), operation);

        if(sqlaexecute_rc==IFR_OK) {
            IFRPacket_ReplySegment replysegment(replypacket);
            if(replysegment.getSQLError(error(), allocator)) {
                // we might have gotten an error that was produced by a parse again
                IFR_Bool parseagainhandled=false;
                if(error().isParseAgain() && retryCounter != PARSE_RETRY - 1) {
                    parseagainhandled=true;
                    replypacket.releaseLock();
                    // get the BUFPOS forall data fields, just in case they will not
                    // match.
                    IFRUtil_Vector<IFR_Int4> bufpos_vector(allocator);
                    IFR_Int2 parameterCount = m_parseinfo->getDirectParameterCount();
                    for(IFR_Int2 i=0; i<parameterCount; ++i) {
                        IFRConversion_Converter *c=m_parseinfo->getParameterInfos()[i];
                        bufpos_vector.InsertEnd(c->getBufpos(), memory_ok);
                        if(!memory_ok) {
                            error().setMemoryAllocationFailed();
                            break;
                        }
                    }

                    IFR_Retcode parse_retcode=IFR_NOT_OK;
                    if(memory_ok) {
                        parse_retcode=doParseAgain(ColumnChange_Forbidden_C, true);
                    }

                    if(parse_retcode==IFR_OK) {
                        // Squeeze the new data into the old packet.
                        // Take care it is the right parse id
                        IFR_ParseID *parseid_ptr=(masscommand ? m_parseinfo->getMassParseID() : m_parseinfo->getParseID());
                        parse_retcode = requestpacket.replaceParseIDAdjustData(parseid_ptr,
                                                                               original_size,
                                                                               bufpos_vector,
                                                                               m_parseinfo->getParameterInfos(),
                                                                               *this);
                        if(parse_retcode == IFR_OK) {
                            if(operation && operation->isExecuted()) {
                                operation->setParseID(*parseid_ptr);
                            }
                            error().clear();
                            continue;
                        }
                    }
                    // otherwise the error leaks through
                }

                // We got an error, we have to update the status array
                // in autocommit mode, we do not need to modify the status
                // array, as all was rolled back in this packet
                //
                // We also must do this only if we are really in a batch.
                //
                // We must not do this if we had parse again issues before.
                if(!(getConnection()->getAutoCommit()) && masscommand && !parseagainhandled) {
                    IFR_Int4 succeededrecords = replysegment.getErrorPos() - 1 + firstrecord;
                    IFR_Int4 startpos = firstrecord;
                    // update status array now with IFR_SUCCESS_NO_INFO until
                    // (not including) the error pos or with 0 for zero rows
                    // affected if the returncode was 100
                    IFR_Int4 rowstatus = IFR_SUCCESS_NO_INFO;
                    if (error().getErrorCode() == 100) {
                      succeededrecords++;
                      rowstatus = 0;
                    }
                    for(IFR_Int4 i = startpos; i < succeededrecords; ++i) {
                        m_rowstatusarray[i] = rowstatus;
                    }
                }
                clearParamData();
                IFR_Int4 resultcount;
                IFR_Retcode rescountpresent = replysegment.getResultCount(resultcount);
                if(rescountpresent == IFR_OK) {
                    m_RowsAffected = resultcount;
                }
                for(int i=0;i<m_rowarraysize; ++i) {
                    if(m_rowstatusarray[i]==IFR_SUCCESS_NO_INFO) {
                        if(error().getErrorCode() == 100) {
                            error().clear();
                        }
                        DBUG_RETURN(IFR_SUCCESS_WITH_INFO);
                    }
                }
                // Do not deliver an error code 100, but instead a 'no data found'.
                if(error().getErrorCode() == 100) {
                    error().clear();
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                } else {
                    DBUG_RETURN(IFR_NOT_OK);
                }
            } else {
                // get the result count from the array,
                // and update the execution count
                IFR_Int4 resultcount;
                IFR_Retcode rescountpresent = replysegment.getResultCount(resultcount);
                if(rescountpresent == IFR_OK) {
                    if(masscommand) {
                        executecount = resultcount;
                    } else {
                        executecount += resultcount;
                    }
                }
                // handle first/last serial update
                {
                    IFRPacket_DataPart serialData;
                    if(replysegment.getSerialPart(serialData) == IFR_OK) {
                        if(m_lastserial == 0 || firstrecord == 0) {
                            setSerialPart(serialData);
                        } else {
                            updateSerialPart(serialData);
                        }
                    }
                }

                if(m_paramdata == 0) {
                    if (this->m_parseinfo->hasLongs () ) {
                        rc = this->handleStreamsForPutval (replypacket, replysegment);
                    }
                    if(rc != IFR_OK) {
                        DBUG_RETURN(rc);
                    }
                } else {
                    m_paramdata->isexecute=false;
                    if(this->m_parseinfo->hasLongs() && m_paramdata->streamstruncated) {
                        IFR_Retcode putval_rc = this->handleStreamsForPutval (replypacket,
                                                                              replysegment,
                                                                              m_paramdata->requestpacket,
                                                                              m_paramdata->segment,
                                                                              m_paramdata->datapart);
                        DBUG_PRINT(putval_rc);
                        if(putval_rc != IFR_OK && putval_rc != IFR_NEED_DATA) {
                            clearParamData();
                            DBUG_RETURN(putval_rc);
                        }
                    } else if(m_parseinfo->hasLongs()) {
                        getChangedPutvalDescriptors(replysegment);
                        replypacket.releaseLock();
                        putvalInitPacket(m_paramdata->requestpacket,
                                         m_paramdata->segment,
                                         m_paramdata->datapart);
                    }
                }

                for(IFR_Int4 i=firstrecord; i<inputcursor; ++i) {
                    m_rowstatusarray[i] = IFR_SUCCESS_NO_INFO;
                }

            }
            break;  // parse again retry loop ...
        } else { // sqlaexecute_rc was not good
            // we have a connection error.
            DBUG_RETURN(clearParamForReturn(IFR_NOT_OK));
        }

    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::clearParamForReturn(IFR_Retcode rc)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, clearParamForReturn);
    clearParamData();
    clearInputLongs();
    closeOutputLongs();
    invalidateLOBs();
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Bool
IFR_PreparedStmt::isQuery() const
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, isQuery);
    ((IFR_PreparedStmt *)this)->clearError();
    DBUG_RETURN(m_parseinfo != 0 && m_parseinfo->isQuery());
}

//----------------------------------------------------------------------
void
IFR_PreparedStmt::resetResults()
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, resetResults);
    IFR_Statement::resetResults();
    clearLOBs();
    closeOutputLongs();
    m_lastgetobjcol = 0;
    m_offset        = 1;
    return;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_PreparedStmt::assertCurrentParseInfo()
{
    if(m_parseinfo) {
        if(!m_parseinfo->getParseID()->isValid(getConnection()->getConnectCount())) {
            error().setRuntimeError(IFR_ERR_SQLCMD_RECONNECT);
            return true;
        }
    }
    return false;
}


//----------------------------------------------------------------------
struct IFR_ABAPInputStreamHandle
{
    IFR_Int4 abaptabid;
    IFR_Int4 requiredrows;
    IFR_Int4 masklength;
    char     *mask;
};

//----------------------------------------------------------------------
static inline void IFR_PreparedStmt_destroyinputstreams(IFR_ABAPInputStreamHandle *s,
                                                        IFR_Length slength,
                                                        SAPDBMem_IRawAllocator& allocator)
{
    for(IFR_ABAPInputStreamHandle *i=s; i<s+slength; ++i) {
        if(i->mask) {
            allocator.Deallocate(i->mask);
        }
    }
    return;
}

//----------------------------------------------------------------------
IFR_Int4
IFR_PreparedStmt::getABAPTableParameterIndex(IFR_Int4 abaptabid)
{
    IFR_Int4 parameterCount = m_parseinfo->getDirectParameterCount();
    IFR_Int4 parameterindex;
    for(parameterindex = 0; parameterindex < parameterCount; ++parameterindex) {
        if(m_paramvector[parameterindex].isABAPTable(abaptabid)) {
            break;
        }
    }
    if(parameterindex == parameterCount) {
        return -1;
    }
    return parameterindex;
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::handleABAPStreams(IFRPacket_PartKind::PartKind partkind, IFRPacket_ReplyPacket& replypacket)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, handleABAPStreams);
    IFR_Retcode rc = IFR_OK;

    for(;;) {
        if(partkind == IFRPacket_PartKind::AbapIStream_C) {
#define IFR_MAX_STREAMS 16
            IFR_ABAPInputStreamHandle inputstreams[IFR_MAX_STREAMS];
            memset(inputstreams, 0, sizeof(inputstreams));
            IFR_size_t inputstreams_count = 0;
            {
                IFRPacket_ReplySegment replysegment(replypacket);
                IFRPacket_PartEnum partenum(replysegment);

                while(partenum.hasMoreElements() && inputstreams_count < IFR_MAX_STREAMS) {
                    if(partenum.getPartKind() == IFRPacket_PartKind::AbapIStream_C) {
                        IFRPacket_AbapIStreamPart istreampart;
                        partenum.getPart(istreampart);
                        inputstreams[inputstreams_count].requiredrows = istreampart.getPartArguments();
                        istreampart.getABAPTabId(inputstreams[inputstreams_count].abaptabid);
                        inputstreams[inputstreams_count].masklength = istreampart.getMaskLength();
                        if(inputstreams[inputstreams_count].masklength) {
                            inputstreams[inputstreams_count].mask =
                                (char *) allocator.Allocate(inputstreams[inputstreams_count].masklength);
                            memcpy(inputstreams[inputstreams_count].mask, istreampart.getMask(),
                                   inputstreams[inputstreams_count].masklength);
                        }
                        ++inputstreams_count;
                    }
                    partenum.next();
                }
            }
            replypacket.releaseLock();

            IFRPacket_RequestPacket requestpacket(*this);
            m_Connection->getRequestPacket(requestpacket, error());
            IFRPacket_RequestSegment segment(requestpacket, IFRPacket_CommandMessageType::Dbs_C);
            IFR_Bool last = false;
            for(IFR_size_t i=0; i<inputstreams_count && !last; ++i) {
                IFRPacket_DataPart datapart;
                rc = segment.addPart(datapart);
                if(rc != IFR_OK) {
                    error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                    sendABAPErrorPacket(replypacket);
                    IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);
                    DBUG_RETURN(rc);
                }
                //>>> SQL TRACE ENTRY
                IFR_SQL_TRACE << endl << "::READ STREAM FROM APPLICATION " << currenttime
                              << " [0x" << (void*)this << "]" << endl
                              << "TABLE ID: " << inputstreams[i].abaptabid << endl;
                //<<< SQL TRACE ENTRY
                IFR_Int4 paramindex = getABAPTableParameterIndex(inputstreams[i].abaptabid);
                if(paramindex == -1) {
                    error().setRuntimeError(IFR_ERR_STREAMHANDLE_NOTFOUND_I, inputstreams[i].abaptabid);
                    sendABAPErrorPacket(replypacket);
                    IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);
                    DBUG_RETURN(IFR_NOT_OK);
                }
                IFRConversion_Converter *c = m_parseinfo->getParameterInfos()[paramindex];
                rc = c->putABAPStream(m_paramvector[paramindex], datapart,
                                      inputstreams[i].mask,
                                      inputstreams[i].masklength,
                                      inputstreams[i].requiredrows,
                                      last,
                                      *this);
                if(last) {
                    segment.rollbackLastPart();
                }
                if(rc != IFR_OK) {
                    sendABAPErrorPacket(replypacket);
                    IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);
                    DBUG_RETURN(rc);
                }
                segment.closePart();
                if(!last) {
                    last = (segment.Size() - segment.Length()) <= sizeof(tsp1_part_header) + 8;
                }
            }
            if(rc == IFR_OK) {
                segment.closePart();
                segment.close();
                rc = m_Connection->sqlaexecute(requestpacket,
                                               replypacket,
                                               IFR_Connection::StreamMode_C,
                                               error());
                // send no ABAP error packet when the 'send' itself failed.
                if(rc != IFR_OK) {
                    IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);
                    DBUG_RETURN(rc);
                }
            }

            if(rc != IFR_OK) {
                sendABAPErrorPacket(replypacket);
                IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);
                DBUG_RETURN(rc);
            }


            IFR_PreparedStmt_destroyinputstreams(inputstreams, inputstreams_count, allocator);

        } else if(partkind == IFRPacket_PartKind::AbapOStream_C) {
            IFRPacket_ReplySegment replysegment(replypacket);
            IFRPacket_PartEnum     partenum(replysegment);
            IFRPacket_AbapOStreamPart ostreampart;
            IFRPacket_DataPart        datapart;
            IFR_Int4                  status = 0;
            IFR_Int4                  abaptabid = 0;
            IFRUtil_Vector<IFR_Int4>  abap_rc_vec(allocator);

            //  check for corrupted output stream parts before iteration
            //  and use of the callback function ...
            {
                int s = 0;
                IFRPacket_ReplySegment re(replypacket);
                IFRPacket_PartEnum pe(re);
                while(pe.hasMoreElements()) {
                    partkind = pe.getPartKind();
                    if(s == 0 && partkind == IFRPacket_PartKind::AbapOStream_C) {
                        s = 1;
                    } else if(s == 1 && partkind == IFRPacket_PartKind::Data_C) {
                        s = 0;
                    } else {
// Trace for stream problem.
#if 0
                        char *p = (char*)replypacket.GetRawPacket();
                        p-=sizeof(teo003_RteHeaderRecord);
                        IFR_Int4 ps = this->m_Connection->getPacketSize()+sizeof(teo003_RteHeaderRecord);

                        IFR_UNCOND_TRACESTREAM(this) << endl << "INVALID PACKET" << endl;
                        IFR_UNCOND_TRACESTREAM(this).hexFromFor("BINARY DUMP OF PACKET",
                                                                p,
                                                                0,
                                                                ps);
//                         IFR_UNCOND_TRACESTREAM(this) << "LOGICAL DUMP" << endl;
//                         IFR_UNCOND_TRACESTREAM(this) << replypacket << endl;
#endif
                        replypacket.releaseLock();
                        error().setRuntimeError(IFR_ERR_STREAM_OUTPUT_SEQUENCE_ERROR_S, IFRPacket_Part::getPartKindName(partkind));
#if 0
                        m_Connection->setRteDumpRequest();
#endif
                        sendABAPErrorPacket(replypacket);
#if 0
                        m_Connection->stopKerneltrace();
#endif
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    pe.next();
                }
            }

            while(partenum.hasMoreElements()) {
                partkind = partenum.getPartKind();
                if(status == 0 && partkind == IFRPacket_PartKind::AbapOStream_C) {
                    status = 1;
                    partenum.getPart(ostreampart);
                } else if(status == 1 && partkind == IFRPacket_PartKind::Data_C) {
                    partenum.getPart(datapart);
                    status = 0;
                    ostreampart.getABAPTabId(abaptabid);
                    IFR_Int4 paramindex = getABAPTableParameterIndex(abaptabid);
                    //>>> SQL TRACE ENTRY
                    IFR_SQL_TRACE << endl << "::WRITE STREAM TO APPLICATION " << currenttime
                                  << " [0x" << (void*)this << "]" << endl
                                  << "TABLE ID: " << abaptabid << endl;
                    //<<< SQL TRACE ENTRY
                    if(paramindex == -1) {
                        error().setRuntimeError(IFR_ERR_STREAMHANDLE_NOTFOUND_I, abaptabid);
                        sendABAPErrorPacket(replypacket);
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    IFRConversion_Converter *c = m_parseinfo->getParameterInfos()[paramindex];
                    IFR_Int4 abap_rc;
                    IFR_Retcode rc = c->getABAPStream(m_paramvector[paramindex],
                                                      datapart,
                                                      ostreampart.getMaskLength(),
                                                      ostreampart.getMask(),
                                                      abap_rc,
                                                      *this);
                    if(rc == IFR_OK) {
                        IFR_Bool memory_ok_ignored = true;
                        abap_rc_vec.InsertEnd(abap_rc, memory_ok_ignored);
                    } else {
                        replypacket.releaseLock();
                        sendABAPErrorPacket(replypacket);
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                } else {
                    
//                     IFR_UNCOND_TRACESTREAM(this)  << "CORRUPTED PACKET FOUND " << endl
//                                                   << replypacket << endl;
                    replypacket.releaseLock();
                    char errbuffer[128];
                    sp77sprintf(errbuffer, 128, "after callback %s", IFRPacket_Part::getPartKindName(partkind));
                    errbuffer[127] = '\0';
                    error().setRuntimeError(IFR_ERR_STREAM_OUTPUT_SEQUENCE_ERROR_S, errbuffer);
                    sendABAPErrorPacket(replypacket);
                    DBUG_RETURN(IFR_NOT_OK);
                }
                partenum.next();
            }

            replypacket.releaseLock();
            // send the return packets
            {
                IFRPacket_RequestPacket requestpacket(*this);
                m_Connection->getRequestPacket(requestpacket, error());
                IFRPacket_RequestSegment segment(requestpacket, IFRPacket_CommandMessageType::Dbs_C);
                IFR_size_t abap_rc_vec_size = abap_rc_vec.GetSize();
                for(IFR_size_t i=0; i<abap_rc_vec_size; ++i) {
                    IFRPacket_DataPart datapart;
                    segment.addPart(datapart);
                    datapart.AddData(&(abap_rc_vec[i]), sizeof(IFR_Int4));
                    datapart.setArgCount(1);
                    segment.closePart();
                }
                segment.close();
                rc = m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::StreamMode_C, error());
                if(rc != IFR_OK) {
                    DBUG_RETURN(rc);
                }
            }

        }
        // look for next stream part, or finish
        {
            IFRPacket_ReplySegment replysegment(replypacket);
            // update the partkind from the reply packet
            partkind = replysegment.hasABAPStreams();
            if(partkind == IFRPacket_PartKind::Nil_C) {
                DBUG_RETURN(IFR_OK);
            }
        }
    }
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_PreparedStmt::sendABAPErrorPacket(IFRPacket_ReplyPacket& replypacket)
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, sendABAPErrorPacket);
    // Take the current error, and temporarily clear the error
    IFR_ErrorHndl tmperror(allocator, getConnection());
    tmperror.assign(error(), false); // temporary switching ...
    error().clear();

    IFRPacket_RequestPacket requestpacket(*this);
    m_Connection->getRequestPacket(requestpacket, error());
    IFRPacket_RequestSegment segment(requestpacket, IFRPacket_CommandMessageType::Dbs_C);
    IFRPacket_ErrorTextPart errortextpart;
    IFR_Retcode rc = segment.addPart(errortextpart);
    errortextpart.setErrorText(tmperror);
    segment.setSQLState(tmperror.getSQLState());
    segment.setErrorCode(tmperror.getErrorCode());
    segment.closePart();
    segment.close();

    if(rc != IFR_OK) {
        error().assign(tmperror, false); // all stuff already counted ...
        DBUG_RETURN(rc);
    }

    rc = m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::StreamMode_C, error());

    error().assign(tmperror, false);
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFR_PreparedStmt& statement)
{
    char str[64];
    sp77sprintf(str, 64, "IFR_PreparedStmt@%p", &statement);
    return s << str;
}

IFR_END_NAMESPACE
