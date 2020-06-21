/*!
  @file           IFR_FetchInfo.cpp
  @author         D039759
  @ingroup        IFR_Fetch
  @brief          Implements fetch info (stores result meta data)
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
#ifdef HPUX
#include <sys/param.h>
#endif

#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_FetchInfo.h"
#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_PartEnum.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_StackAllocator.h"

IFR_FetchInfo::IFR_FetchInfo (IFR_Statement& statement,
                              IFRConversion_ConverterList& info,
		                      IFRUtil_Vector<IFR_String>* colName,
                              IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(statement),
 m_Statement(statement), 
 m_CursorName(statement.getCursorName(), memory_ok),
 m_columninfo(allocator)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, IFR_FetchInfo);
    if(memory_ok) {
        if(info.size() !=0 && colName!=0) {
            this->setMetaData(info, colName);
        } 
    }
}

//----------------------------------------------------------------------
IFR_FetchInfo::~IFR_FetchInfo ()
{
}

IFR_Retcode IFR_FetchInfo::describe () 
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, describe);
    // return if we have already all information
    if (this->m_columninfo.size() != 0) {
        DBUG_RETURN(IFR_OK);
    }
    IFR_Bool memory_ok=true;
    IFR_Connection *connection = m_Statement.getConnection();
    IFRPacket_RequestPacket requestPacket(*this);
    IFR_Retcode rc=connection->getRequestPacket (requestPacket, error());
    if(rc != IFR_OK) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFRUtil_StackAllocator<256> describe_allocator;
    requestPacket.setEncoding(m_Statement.getCommandEncoding());
    
    IFR_String sqlCmd(requestPacket.getEncoding(), describe_allocator);
    sqlCmd.append("DESCRIBE \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    sqlCmd.append(this->m_CursorName, memory_ok);
    sqlCmd.append("\"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    IFR_SQL_TRACE << "::DESCRIBE CURSOR " << m_CursorName << " " << currenttime << endl;

    IFRPacket_RequestSegment segment(requestPacket, IFRPacket_CommandMessageType::Dbs_C);      
    IFRPacket_CommandPart command;
    if (IFR_OK != segment.addPart(command)) {
      this->error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
      DBUG_RETURN(IFR_NOT_OK);
    }
    if (IFR_OK != command.setText(sqlCmd, this->error())) {
      DBUG_RETURN(IFR_NOT_OK);
    }
    segment.closePart();
    segment.close();
    
    IFRPacket_ReplyPacket replyPacket;
    rc = connection->sqlaexecute (requestPacket, replyPacket, IFR_Connection::AppendNotAllowed_C, error());
    if(rc != IFR_OK) {
        DBUG_RETURN(rc);
    } else if(error()) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFRPacket_ReplySegment replysegment(replyPacket);
    IFRPacket_PartEnum partEnum (replysegment);
    
    IFRConversion_ConverterList infos(allocator);
    IFRUtil_Vector<IFR_String> *columnNames = 0;
    
    while (partEnum.hasMoreElements ()) {
        int knd = partEnum.getPartKind ();
        switch(knd) {
        case IFRPacket_PartKind::Columnnames_C:
            replysegment.parseColumnNames (&columnNames, allocator, memory_ok);
            if(!memory_ok) {
                error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
            break;
        case IFRPacket_PartKind::Shortinfo_C: {
            IFRPacket_ShortInfoPart part;
            partEnum.getPart(part);
            part.parseShortFields (infos, m_Statement);
            break;
        }
        case IFRPacket_PartKind::VarDataShortInfo_C: {
            IFRPacket_VarDataShortInfoPart part;
            partEnum.getPart(part);
            part.parseShortFields(infos, m_Statement);
            break;
        }
        }
        partEnum.next();
    }
    this->setMetaData (infos, columnNames);
    IFRUtil_Delete(columnNames, allocator);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_FetchInfo::setMetaData (IFRConversion_ConverterList& info,
                            IFRUtil_Vector<IFR_String>* colName)
{
  DBUG_METHOD_ENTER(IFR_FetchInfo, setMetaData);
  
  IFR_String currentName(allocator);
  int currentFieldEnd;
  
  IFR_Bool memory_ok=true;
  m_RecordSize = 0;
  m_columninfo.swap(info, memory_ok);
  
  for (IFR_UInt4 i = 0; i < colName->GetSize (); i++) {
      m_columninfo[i]->setName ((*colName)[i], memory_ok);
      if(!memory_ok) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
      m_columninfo[i]->setIndex(i+1);
      currentFieldEnd = m_columninfo[i]->getIOLength () + m_columninfo[i]->getBufpos () - 1;
      this->m_RecordSize = MAX (this->m_RecordSize, currentFieldEnd);
  }
  DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_FetchInfo::setMetaDataRefFromParseinfo (IFRConversion_Converter **info,
                                            IFR_UInt4 columncount) 
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, copyMetaDataFromParseinfo);
    
    int currentFieldEnd;
    m_RecordSize = 0;
    IFR_Bool memory_ok = true;
    // no indirect pos as only output parameters are interesting
    m_columninfo.copyref(info, columncount, false, memory_ok);

    if(!memory_ok) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    for (IFR_UInt4 i = 0; i < columncount; i++) {
        info[i]->setIndex(i+1);
        currentFieldEnd = info[i]->getIOLength () + info[i]->getBufpos () - 1;
        this->m_RecordSize = MAX (this->m_RecordSize, currentFieldEnd);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode IFR_FetchInfo::executeFetchAbsolute (IFR_Int4 position, 
                                                 IFR_Int4 fetchSize, 
                                                 IFRPacket_ReplyPacket& replyPacket)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchAbsolute);
    DBUG_PRINT(position);
    DBUG_PRINT(fetchSize);
    IFR_Bool memory_ok=true;
    IFR_String cmd ("FETCH ABSOLUTE ", IFR_StringEncodingAscii, allocator, memory_ok);
    cmd.expand(128, memory_ok); 
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    char pos[14];
    sp77sprintf (pos, sizeof(pos), "%d", position);
    
    cmd.append(pos, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    if(m_CursorName.getStrLen() > 0) {
        cmd.append(" \"",IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        cmd.append(m_CursorName, memory_ok);
        cmd.append("\"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    }
    cmd.append(" INTO ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(s_fetchparamstring, IFR_StringEncodingAscii, m_columninfo.size() * 3 - 2, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(executeFetchCommand (cmd, fetchSize, replyPacket));
}

IFR_Retcode IFR_FetchInfo::executeFetchRelative (IFR_Int4 position, 
                                                 IFR_Int4 fetchSize, 
                                                 IFRPacket_ReplyPacket& replyPacket)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchRelative);
    IFR_Bool memory_ok=true;
    IFR_String cmd ("FETCH RELATIVE ", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    char pos[14];
    sp77sprintf (pos, sizeof(pos),"%d", position);
    cmd.append(pos, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(" \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(m_CursorName, memory_ok);
    cmd.append("\" INTO ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(s_fetchparamstring, IFR_StringEncodingAscii, m_columninfo.size() * 3 - 2, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(executeFetchCommand (cmd, fetchSize, replyPacket));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_FetchInfo::executeFetchFirst(IFR_Int4 fetchSize, 
                                 IFRPacket_ReplyPacket& replyPacket)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchFirst);
    IFR_Bool memory_ok=true;
    IFR_String cmd ("FETCH FIRST \"", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    cmd.append(this->m_CursorName, memory_ok);
    cmd.append("\" INTO ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(s_fetchparamstring, IFR_StringEncodingAscii, m_columninfo.size() * 3 - 2, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(executeFetchCommand (cmd, fetchSize, replyPacket));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_FetchInfo::executeFetchNext(IFR_Int4 fetchSize, 
                                 IFRPacket_ReplyPacket& replyPacket)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchNext);
    IFR_Bool memory_ok=true;
    IFR_String cmd ("FETCH NEXT \"", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    cmd.append(this->m_CursorName, memory_ok);
    cmd.append("\" INTO ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(s_fetchparamstring, IFR_StringEncodingAscii, m_columninfo.size() * 3 - 2, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    DBUG_RETURN(executeFetchCommand (cmd, fetchSize, replyPacket));
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_FetchInfo::executeFetchLast (IFR_Int4 fetchSize, 
                                 IFRPacket_ReplyPacket& replyPacket)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchLast);
    IFR_Bool memory_ok=true;
    IFR_String cmd ("FETCH LAST \"", IFR_StringEncodingAscii, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    cmd.append(this->m_CursorName, memory_ok);
    cmd.append("\" INTO ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    cmd.append(s_fetchparamstring, IFR_StringEncodingAscii, m_columninfo.size() * 3 - 2, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
        
    DBUG_RETURN(executeFetchCommand (cmd, fetchSize, replyPacket));
}

//----------------------------------------------------------------------
const char *
IFR_FetchInfo::s_fetchparamstring = 
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, "
    "?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ";

//----------------------------------------------------------------------
IFR_Retcode 
IFR_FetchInfo::executeFetchCommand (IFR_String& command, 
                                    IFR_Int4 fetchSize, 
                                    IFRPacket_ReplyPacket& replyPacket)
                                 
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, executeFetchCommand);
    DBUG_PRINT(command);
    DBUG_PRINT(fetchSize);
    
    //>>> SQL TRACE
    IFR_SQL_TRACE << endl << "::FETCH " << m_CursorName << " "
                  << currenttime << endl
                  << "SQL COMMAND: " << command << endl
                  << "FETCH SIZE: " << fetchSize << endl;
    //<<< SQL TRACE

    IFR_Retcode rc = IFR_OK;
    IFR_String errText(allocator);
    IFR_StringEncoding cmdEncoding = m_Statement.getCommandEncoding(); 
    IFR_String realCommandBuffer(cmdEncoding, allocator);
    IFR_String *realcommand=&command;
    IFR_Bool   memory_ok = true;
    if(command.getEncoding() != cmdEncoding) {
        realCommandBuffer.append(command, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        realcommand = &realCommandBuffer;
    }

    IFRPacket_RequestPacket request(*this);
    IFR_Bool reset = true;

    IFR_Connection *connection = m_Statement.getConnection();
    rc=connection->getRequestPacket (request, error());
    if(rc!=IFR_OK) {
        return rc;
    }
    m_Statement.m_counter_4[IFR_StatementProfile::IFR_FETCHCOUNT_STMT]++;

    int currentSQLMode = request.switchSqlMode (IFR_INTERNAL);
    request.setEncoding(cmdEncoding);
    IFRPacket_RequestSegment segment (request, sp1m_dbs, reset);
    if(segment.isValid()) {
        IFRPacket_CommandPart commandpart;
        if((rc = segment.addPart(commandpart)) != IFR_OK) {
            error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
            request.switchSqlMode(currentSQLMode);
            DBUG_RETURN(rc);
        }
        if (IFR_OK != commandpart.setText(*realcommand, error())) {
          request.switchSqlMode(currentSQLMode);
          DBUG_RETURN(IFR_NOT_OK);
        } 
        segment.closePart ();
        if (m_Statement.m_resultset->hasPositionedRead ()) {
            IFRPacket_LongDemandPart longdemandpart;
            if((rc = segment.addPart(longdemandpart)) != IFR_OK) {
                error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                request.switchSqlMode(currentSQLMode);
                DBUG_RETURN(rc);
            }
            const IFRUtil_Vector<IFR_Parameter> *paramvec = m_Statement.m_resultset->getParamVector ();
            IFR_size_t paramvec_size = paramvec->GetSize();
            for (unsigned int i = 0; i < paramvec_size; i++) {
                if (m_columninfo[i]->isLong ()) {
                    IFR_Parameter param = (*paramvec)[i];
                    const IFR_Length *pos = param.getPosIndicator ();
                    if (pos) {
                        longdemandpart.addPosParams (true, 
                                                     *pos, 
                                                     (*paramvec)[i].getBytesLength ());
                    } else {
                        longdemandpart.addPosParams (true, 
                                                     1, 
                                                     (*paramvec)[i].getBytesLength ());
                    }
                }
            }
          segment.closePart ();
        }
        error ().clear ();
        if (fetchSize > 1) {
            request.setMassCommand();
        } else {
            fetchSize = 1;
        }
        segment.addFetchSize(fetchSize);
        segment.close();
        IFR_Retcode rc_sqlaexecute;
        if((rc_sqlaexecute=connection->sqlaexecute (request, replyPacket, 
                                                    IFR_Connection::AppendNotAllowed_C, error())) != IFR_OK) {
            DBUG_RETURN(rc_sqlaexecute);
        } 
        IFRPacket_ReplySegment replysegment(replyPacket);
        if (replysegment.getSQLError (error(), allocator)) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        request.switchSqlMode(currentSQLMode);
        DBUG_RETURN(IFR_OK);
    }
    request.switchSqlMode(currentSQLMode);
    DBUG_RETURN(IFR_NOT_OK);
}

//----------------------------------------------------------------------
IFRConversion_Converter * 
IFR_FetchInfo::findColInfo (IFR_Int2 columnIndex)
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, findColInfo);
    if(columnIndex < 0  || (IFR_UInt4)columnIndex > m_columninfo.size()) {
        return 0;
    } else {
        return m_columninfo[columnIndex-1]; 
    }
}

//----------------------------------------------------------------------
IFR_Int2
IFR_FetchInfo::getColumnCount()
{
    DBUG_METHOD_ENTER(IFR_FetchInfo, getColumnCount);
    DBUG_RETURN((IFR_Int2) m_columninfo.size());
}


