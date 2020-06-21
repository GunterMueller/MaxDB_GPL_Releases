/*!
  @file           IFR_Statement.cpp
  @author         BurkhardD
  @ingroup        IFR_Statement
  @brief          Implements class for sql processing
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
// Prevents a Warning 5004 Uninitialized variable "static_#classcvt#693
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif


#include "Interfaces/Runtime/IFR_Common.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"
#include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/IFR_ResultSet.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_PartEnum.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_FunctionCode.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "SAPDB/Interfaces/Runtime/IFR_PreparedStmt.h"

#include <ctype.h>

IFR_BEGIN_NAMESPACE

#define MAP_PROFILE_COUNTER_STMT(x) (counter_4[IFR_EnvironmentProfile:: x##_ENV]+= m_counter_4[x##_STMT])
#define MAP_PROFILE_COUNTER_8_STMT(x) (counter_8[IFR_EnvironmentProfile:: x##_8_ENV]+= m_counter_8[x##_8_STMT])

IFR_Statement::IFR_Statement (IFR_Connection &conn, IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(conn),
 IFR_ConnectionItem(&conn),
 m_RowsAffected(-1),
 m_ResultSetConcurrency (READ_ONLY),
 m_ResultSetHoldability (HOLD_CURSORS_OVER_COMMIT),
 m_FetchSize (SAPDB_MAX_INT2),
 m_MaxRows (0),
 m_QueryTimeout (0),
 m_CursorName(allocator),
 m_commandInfoState(CommandInfoNotSet_C),
 m_commandInfo(allocator),
 m_TableName(allocator),
 m_resultset(NULL), 
 m_Connection(&conn),
 m_CommandEncoding(IFR_StringEncodingUnknown), 
 m_cursorstate(CursorStateNotUsed_C),
 m_rowarraysize(1),
 m_rowstatusarray(new IFR_ALLOCATOR(allocator) IFR_Int4[1]),
 m_rowstatussize(1),
 m_batchelements(0),
 m_lastserial(0)
{
     DBUG_METHOD_ENTER(IFR_Statement, IFR_Statement);
     conn.nextCursorName(m_CursorName, memory_ok);
     //<<< MEMCHECK
     if(!memory_ok  || m_rowstatusarray==0) {
        conn.error().setMemoryAllocationFailed();
        return;
     }
     //>>> MEMCHECK
     DBUG_PRINT(&conn);
     DBUG_PRINT(m_CursorName);
     m_rowstatusarray[0]=IFR_EXECUTE_FAILED;
     m_ResultSetType = (IFR_Statement::ResultSetType) conn.getDefaultResultSetType();
}

//----------------------------------------------------------------------
IFR_Statement::~IFR_Statement ()
{
    DBUG_METHOD_ENTER(IFR_Statement, ~IFR_Statement);
    if(m_cursorstate == CursorStateUsed_C) {
        DBUG_PRINT(m_CursorName);
        IFR_Bool memory_ok = true;
        getConnection()->dropCursor(m_CursorName, memory_ok);
        if(!memory_ok) {
            getConnection()->error().setMemoryAllocationFailed();
        }
    }
    IFRUtil_Delete(m_resultset, allocator);
    IFRUtil_DeleteArray(m_rowstatusarray, m_rowarraysize, IFR_ALLOCATOR(allocator));
    IFRUtil_Delete(m_batchelements, allocator);
    IFRUtil_Delete(m_lastserial, allocator);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::getCursorName(char *buffer, 
                             IFR_StringEncoding encoding, 
                             IFR_Length bufferSize,
                             IFR_Length *bufferLength) const
{
    DBUG_METHOD_ENTER(IFR_Statement, getCursorName);
    DBUG_PRINT(bufferLength);
    ((IFR_Statement *)this)->clearError();
    DBUG_RETURN(m_CursorName.convertInto(buffer, encoding, bufferSize, bufferLength));
}

//----------------------------------------------------------------------
const IFR_String &
IFR_Statement::getCursorName() const 
{
    DBUG_METHOD_ENTER(IFR_Statement, getCursorName);
    DBUG_RETURNC(const IFR_String&, m_CursorName);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::getTableName(char *buffer, 
                            IFR_StringEncoding encoding, 
                            IFR_Length bufferSize,
                            IFR_Length *bufferLength) const
{
    DBUG_METHOD_ENTER(IFR_Statement, getTableName);
    DBUG_PRINT(bufferLength);
    ((IFR_Statement *)this)->clearError();
    DBUG_RETURN(m_TableName.convertInto(buffer, encoding, bufferSize, bufferLength));
}

//----------------------------------------------------------------------
const IFR_String &
IFR_Statement::getTableName() const 
{
    DBUG_METHOD_ENTER(IFR_Statement, getTableName);
    DBUG_RETURN(m_TableName);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::sendCommand (IFRPacket_RequestPacket &requestPacket,
                            const IFR_String& sqlCmd, 
                            IFR_Bool masscommand, 
                            IFR_Bool parseonly,
                            IFR_Bool parseAgain,
                            IFR_Connection::AppendMode appendFlag,
                            IFRPacket_ReplyPacket& replyPacket)
{
  DBUG_METHOD_ENTER(IFR_Statement, sendCommand);
  DBUG_PRINT(sqlCmd);
  DBUG_PRINT(masscommand);
  DBUG_PRINT(parseonly);
  DBUG_PRINT(parseAgain);
  DBUG_PRINT(appendFlag);

  if (requestPacket.isValid()) {
      /* set encoding of packet */
      if (!setCommandEncoding(sqlCmd.getEncoding(), requestPacket.isSwapped())) {        
        // @todo: check if the command may be converted to ASCII 
        IFR_StringEncoding encoding = sqlCmd.getEncoding();
        if (encoding == IFR_StringEncodingUCS2
            || encoding == IFR_StringEncodingUCS2Swapped ) {
          const char *c = sqlCmd.getBuffer();
          int i = sqlCmd.getLength()/2;
          if (encoding == IFR_StringEncodingUCS2Swapped) {
            /* zero bytes are on equal byte positions */
            c++;
          }
          for(; i > 0;i--,c+=2) {
              if (*c & (char)0xFF) {
                  this->error().setRuntimeError(IFR_ERR_SQLCMD_NOT_ASCII);
                  DBUG_RETURN(IFR_NOT_OK);
              }
          }
          DBUG_TRACE << "SQL command is converted from UCS2 to ASCII." << endl;
          setCommandEncoding(IFR_StringEncodingAscii, requestPacket.isSwapped());
        } else if(encoding == IFR_StringEncodingUTF8) {
            const char *c=sqlCmd.getBuffer();
            while(*c) {
                if(*c & 0x80) {
                    this->error().setRuntimeError(IFR_ERR_SQLCMD_NOT_ASCII);
                    DBUG_RETURN(IFR_NOT_OK);
                }
                ++c;
            }
            DBUG_TRACE << "SQL command is converted from UTF8 to ASCII." << endl;
            setCommandEncoding(IFR_StringEncodingAscii, requestPacket.isSwapped());
        } else {
          this->error().setRuntimeError(IFR_ERR_SQLCMD_NOT_ASCII);
          DBUG_RETURN(IFR_NOT_OK);
        }
      }
      requestPacket.setEncoding(getCommandEncoding());

      /* set message type */
      int messagetype = (parseonly) ? IFRPacket_CommandMessageType::Parse_C 
          : IFRPacket_CommandMessageType::Dbs_C;
      IFRPacket_RequestSegment segment(requestPacket, messagetype);      
      
      segment.setProducer (m_Connection->getProducer());

      /* set masscommand */
      if(masscommand) {
          requestPacket.setMassCommand();
      }
      if(parseAgain) {
          IFR_STMT_PROFILE_COUNT_INC(IFR_PARSEAGAINCOUNT);
          requestPacket.setParseAgain();
      }
      if(this->m_Connection->isWithInfoSet()) {
          requestPacket.setWithInfo();
      }
      if(m_Connection->getAutoCommit()) {
          segment.setCommitImmediately();
      }
      /*
      if(messagetype = IFRPacket_CommandMessageType::Parse_C) {
          segment.setParseWithPrepare();
      }
      */
      // puts the the SQL command
      IFRPacket_CommandPart command;
      if (IFR_OK != segment.addPart(command)) {
        this->error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
        DBUG_RETURN(IFR_NOT_OK);
      }
      if (IFR_OK != command.setText(sqlCmd, error())) {
        DBUG_RETURN(IFR_NOT_OK);
      }
      segment.closePart();
      
      // if the command is parsed, we may want to add application parameter descriptions of the
      // bound parameters.
      if(parseonly) {
        if (IFR_OK != addApplicationInfo(segment)) {
          this->error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
          DBUG_RETURN(IFR_NOT_OK);
        }
      }

      // The cursor name is bound at execute, not before.
      if (!parseonly) {
          // sets the resultname 
          IFRPacket_TableNamePart cursor;
          if(IFR_OK != segment.addPart(cursor)) {
            error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
            DBUG_RETURN(IFR_NOT_OK);
          }
          if (IFR_OK != cursor.setText(this->m_CursorName, error())) {
            DBUG_RETURN(IFR_NOT_OK);
          }
          segment.closePart();
          
          segment.setScrollableOption(getConnection()->getScrollableOption(m_ResultSetType));
          if(m_cursorstate == CursorStateUsedClosed_C) {
              segment.setNoResultSetCloseNeeded();
          }         
      } else if (m_commandInfo.getLength() !=0) {
          IFRPacket_CommandInfoPart comInf;
          if(IFR_OK != segment.addPart(comInf)) {
            error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
            DBUG_RETURN(IFR_NOT_OK);
          }
          if (IFR_OK != comInf.addCommandInfo(this->m_commandInfo, this->m_commandLineNumber, error())) {
            DBUG_RETURN(IFR_NOT_OK);
          }
          segment.closePart();
      }
      segment.close();

      IFR_Retcode rc = m_Connection->sqlaexecute (requestPacket, replyPacket, appendFlag, error());
      DBUG_RETURN(rc);
  } else {
      error().setRuntimeError(IFR_ERR_INVALID_REQUESTPACKET);
      DBUG_RETURN(IFR_NOT_OK);
  }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::sendSQL (const IFR_String &sql, 
                        IFRPacket_ReplyPacket& replyPacket,
                        IFR_Bool masscommand, 
                        IFR_Bool parseonly,
                        IFR_Bool parseAgain,
                        IFR_Connection::AppendMode appendFlag)
{
    DBUG_METHOD_ENTER(IFR_Statement, sendSQL);
    DBUG_PRINT(sql);
    DBUG_PRINT(masscommand);
    DBUG_PRINT(parseonly);
    DBUG_PRINT(parseAgain);
    DBUG_PRINT(appendFlag);
    
    IFR_String actualSQL(sql.getAllocator());
    bool touchedCommand = false;
    IFR_Retcode rc=IFR_NOT_OK;
    IFR_Bool memory_ok=true;
    // copy SQL statement only if necessary
    if (m_ResultSetConcurrency == UPDATABLE  ||  m_ResultSetType == SCROLL_INSENSITIVE) {
        actualSQL.assign(sql, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        touchedCommand = true;
    }

    if (this->m_ResultSetConcurrency == UPDATABLE) {
        actualSQL.append(" FOR UPDATE OF ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    if (this->m_ResultSetType == SCROLL_INSENSITIVE) {
        actualSQL.append(" FOR REUSE ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_PRINT(touchedCommand);
    IFRPacket_RequestPacket requestPacket(*this);
    rc= m_Connection->getRequestPacket (requestPacket, error());
    if(rc != IFR_OK) {
        rc = IFR_NOT_OK;
    }
    else {
        if (touchedCommand)
            rc = this->sendCommand (requestPacket, actualSQL, masscommand, parseonly, parseAgain, appendFlag, replyPacket);    
        else
            rc = this->sendCommand (requestPacket, sql, masscommand, parseonly, parseAgain, appendFlag, replyPacket);    
    }
    DBUG_RETURN(rc);
}

IFR_Retcode 
IFR_Statement::parseResult (const IFR_String* command,
                            IFRPacket_ReplyPacket &replyPacket, 
                            IFR_Bool parseonly)
{
  DBUG_METHOD_ENTER(IFR_Statement, parseResult);
  IFR_Retcode rc = IFR_OK;
  IFR_String tableName(allocator);
  IFR_Bool memory_ok = true;
  bool rowNotFound = false;
  bool isQuery = false;
  IFR_Bool tablename_found = false;
  IFRConversion_ConverterList infos(allocator);
  IFR_String oldCursorName(allocator);
  // oldCursorName is used as dummy.
  IFRUtil_Vector<IFR_String> columNames(allocator, 0, oldCursorName, memory_ok);
  IFRPacket_ReplySegment segm(replyPacket);
  if (segm.FunctionCode().isQuery()) {
      isQuery = true;
  }

  IFR_Bool segm_resultset_closed = segm.isResultSetClosed();

  this->functionCode2Statistic(segm.FunctionCode());
  if(m_cursorstate == CursorStateUsed_C || isQuery) {
      // need to record old name cause result table part will 
      // overwrite
      oldCursorName.assign(m_CursorName, memory_ok);
      if(!memory_ok) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
  }
  DBUG_PRINT(isQuery);
  IFR_FetchChunk *select_fetch_chunk = 0;
  IFR_Bool datapart_found = false;
  IFRPacket_PartEnum partEnum (segm);
  while (partEnum.hasMoreElements ()) {
      IFRPacket_PartKind::PartKind PartKind = partEnum.getPartKind();
    DBUG_PRINT(PartKind);
    switch(PartKind) {
    case (IFRPacket_PartKind::Columnnames_C):{
        IFRPacket_ColumnNamesPart part;
        partEnum.getPart(part);
        part.getColumnNames(columNames, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
      break;
    }  
    case (IFRPacket_PartKind::Shortinfo_C): {
        IFRPacket_ShortInfoPart part;
        partEnum.getPart(part);
        part.parseShortFields(infos, *this);
        break;
    }
    case (IFRPacket_PartKind::VarDataShortInfo_C): {
        IFRPacket_VarDataShortInfoPart part;
        partEnum.getPart(part);
        part.parseShortFields(infos, *this);
        break;
    }
    case (IFRPacket_PartKind::Resultcount_C): {
        IFR_Int4 ResCount;
        if (segm.getResultCount(ResCount) == IFR_OK)
            this->m_RowsAffected = ResCount;        
        DBUG_PRINT(ResCount);
        //>>> SQL TRACE
        IFR_SQL_TRACE << "RESULT COUNT: " << ResCount << endl;
        //<<< SQL TRACE
        break;
    }
    case (IFRPacket_PartKind::Resulttablename_C): {
        IFRPacket_TableNamePart part;
        partEnum.getPart(part);
        part.getResultName(this->m_CursorName, memory_ok);
        // The cursor will be only changed if this is 
        // not a parse that delivered a cursor name of length 
        // 0.
        if(parseonly && this->m_CursorName.getStrLen()==0) {
            m_CursorName.assign(oldCursorName, memory_ok);
        }
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }

        DBUG_PRINT(this->m_CursorName);
        //>>> SQL TRACE
        IFR_SQL_TRACE << "CURSOR NAME: " << m_CursorName << endl;
        //<<< SQL TRACE
        break;
    }
    case (IFRPacket_PartKind::Serial_C): {
        IFRPacket_DataPart serialData;
        partEnum.getPart(serialData);
        if(setSerialPart(serialData) == IFR_NOT_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        break;
    }
    case (IFRPacket_PartKind::Errortext_C): {
        segm.getSQLError(error(), allocator);
        if (error ().getErrorCode () == 100) {
            error().clear();
            //>>> SQL TRACE
            IFR_SQL_TRACE << "*** ROW NOT FOUND ***" << endl;
            //<<< SQL TRACE
            if (this->m_RowsAffected != 0) {
              this->m_RowsAffected = 0;
              IFR_SQL_TRACE << "RESULT COUNT RESET: " << this->m_RowsAffected << endl;
            }
            rowNotFound = true;
            rc = IFR_NO_DATA_FOUND;
        } else {
            rc = IFR_NOT_OK;
        }
        break;
    }
    case (IFRPacket_PartKind::Tablename_C):{
        IFRPacket_TableNamePart part;
        partEnum.getPart(part);
        part.getResultName(this->m_TableName, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        DBUG_PRINT(this->m_TableName);
        //>>> SQL TRACE
        IFR_SQL_TRACE << "TABLE NAME: " << m_TableName << endl;
        //<<< SQL TRACE
        tablename_found = true;
        break;
    }
    case (IFRPacket_PartKind::ParsidOfSelect_C):{
      // @todo; Handling for ParsidOfSelect_C
        error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("ParsidOfSelect"));
        DBUG_RETURN(IFR_NOT_OK);
        break;
    }
    case (IFRPacket_PartKind::Data_C): {
        datapart_found = true;
        break;
    }
    default: {
        break;
    }
    }
    partEnum.next();
  }
  IFR_Bool closeOnLast = false;
  if(datapart_found) {
      select_fetch_chunk = new IFR_ALLOCATOR(allocator)
          IFR_FetchChunk(IFR_FetchChunk::IFR_TYPE_FIRST,
                         1,
                         replyPacket,
                         -1,
                         m_MaxRows,
                         -1, //unknown rows in result
                         0, // no current chunk
                         memory_ok,
                         &(this->m_counter_4[IFR_FETCHROWCOUNT_STMT]),
                         &(this->m_counter_8[IFR_DATAREADSIZE_8_STMT]));
      closeOnLast = true;
      if(!memory_ok) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
    }else if (isQuery && ! rowNotFound){
      IFR_STMT_PROFILE_COUNT_INC(IFR_NOTSELECTFETCHOPTIMIZED);
    }
  
  if(m_Connection->getSelectFetchStrategy() == IFR_Connection::SelectFetchFetchImmediate_C) {
      PIn_ReplySegment second_segment = replyPacket.GetNextSegment(segm);
      if(second_segment.IsValid() && second_segment.FindPart(sp1pk_data).IsValid()) {
          select_fetch_chunk = new IFR_ALLOCATOR(allocator)
              IFR_FetchChunk(second_segment,
                             replyPacket,
                             m_MaxRows,
                             memory_ok,
                             &(this->m_counter_4[IFR_FETCHROWCOUNT_STMT]),
                             &(this->m_counter_8[IFR_DATAREADSIZE_8_STMT]));
          if(!memory_ok) {
              error().setMemoryAllocationFailed();
              DBUG_RETURN(IFR_NOT_OK);
          }
      }
  }
     

  // we KNOW that this is the last time we need the reply packet.
  DBUG_PRINT(rowNotFound);
  replyPacket.releaseLock();
  IFR_Bool cursorwasused = (m_cursorstate == CursorStateUsed_C); // was the cursor used before this statement
  if (((rc == IFR_OK) || (rc == IFR_NO_DATA_FOUND)) && isQuery) {
      createResultSet (tableName, this->m_CursorName, rowNotFound, infos, &columNames, 
                       closeOnLast,
                       select_fetch_chunk);
      // on row not found and select-fetch optimize, forget the cursor
      if(rowNotFound && m_ResultSetType == ::SQLDBC::SQLDBC_Statement::FORWARD_ONLY
         && m_Connection->getSelectFetchStrategy() == IFR_Connection::SelectFetchScrollable_C) {
          if(segm_resultset_closed) {
              m_cursorstate = CursorStateUsedClosed_C;
          } else {
              m_cursorstate = CursorStateUsed_C;
          }
      }
  }
  // now drop the old cursor, if
  if((!isQuery && m_cursorstate == CursorStateUsed_C)) { // this is not a query and the cursor was used      
      getConnection()->dropCursor(oldCursorName, memory_ok);
      if(!memory_ok) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
      this->m_cursorstate = CursorStateNotUsed_C;
  } else if(isQuery && cursorwasused && IFR_String::compare(this->m_CursorName, oldCursorName, memory_ok)) {
     // or is a query, cursor was used before and name has changed.
      getConnection()->dropCursor(oldCursorName, memory_ok);
      if(!memory_ok) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
  }

  if(isQuery && rowNotFound && !tablename_found && (m_ResultSetConcurrency == UPDATABLE) && command!=0) {
      IFR_PreparedStmt *tmp_ps = getConnection()->createPreparedStatement();
      if(tmp_ps == 0) {
          error().setMemoryAllocationFailed();
          DBUG_RETURN(IFR_NOT_OK);
      }
      tmp_ps->setResultSetConcurrencyType(UPDATABLE);
      IFR_Retcode prep_rc = tmp_ps->prepare(*command);
      if(prep_rc != IFR_OK) {
          error().assign(tmp_ps->error(), false);
          rc = prep_rc;
      } else {
          m_TableName.assign(tmp_ps->getTableName(), memory_ok);
          if(!memory_ok) {
              error().setMemoryAllocationFailed();
              rc = IFR_NOT_OK;
          }
      }
      getConnection()->releaseStatement(tmp_ps);
  }
  
  DBUG_RETURN(rc);
}

IFR_Retcode IFR_Statement::createResultSet (IFR_String& tableName,
                                            IFR_String& cursorName,
                                            IFR_Bool rowNotFound,
                                            IFRConversion_ConverterList& info,
                                            IFRUtil_Vector<IFR_String>* colName,
                                            IFR_Bool closeOnLast,
                                            IFR_FetchChunk *firstChunk)
{
  DBUG_METHOD_ENTER(IFR_Statement, createResultSet);
  DBUG_PRINT(tableName);
  DBUG_PRINT(cursorName);
  DBUG_PRINT(rowNotFound);
  
  IFR_Retcode rc = IFR_OK;
  IFR_Bool    memory_ok = true;
  IFR_FetchInfo *fetchInfo = new IFR_ALLOCATOR(allocator) IFR_FetchInfo (*this, info, colName, memory_ok);
  if(fetchInfo == 0) {
      error().setMemoryAllocationFailed();
      this->m_resultset = 0;
      DBUG_RETURN(IFR_NOT_OK);
  } else if(!memory_ok) {
      IFRUtil_Delete(fetchInfo, allocator);
      error().setMemoryAllocationFailed();
      this->m_resultset = 0;
      DBUG_RETURN(IFR_NOT_OK);
  }
  if (info.size() == 0 || colName == 0) {
      rc = fetchInfo->describe();
      if(rc != IFR_OK) {
          error().assign(fetchInfo->error(), true); // count, as not in result set for now
          IFRUtil_Delete(fetchInfo, allocator);
          this->m_resultset = 0;
          DBUG_RETURN(IFR_NOT_OK);
      }
  }
  this->m_cursorstate = CursorStateUsed_C;
  this->m_resultset = new IFR_ALLOCATOR(allocator) IFR_ResultSet (*this->m_Connection, 
                                                                  *fetchInfo, 
                                                                  *this, 
                                                                  this->m_FetchSize, 
                                                                  this->m_MaxRows, 
                                                                  m_ResultSetConcurrency, 
                                                                  m_ResultSetType, 
                                                                  rowNotFound, 
                                                                  firstChunk,
                                                                  closeOnLast,
                                                                  memory_ok);
  if(this->m_resultset==0) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  } else if(!memory_ok) {
      IFRUtil_Delete(this->m_resultset, allocator);
      this->m_resultset = 0;
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  } else {
      DBUG_RETURN(rc);
  }
}

int IFR_Statement::getResultSetType ()
{
    DBUG_METHOD_ENTER(IFR_Statement, getResultSetType);
    clearError();
    DBUG_RETURN(m_ResultSetType);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::execute(const char *sql, const IFR_StringEncoding encoding, IFR_Bool recycleObjects)
{
  return this->execute(sql, IFR_NTS, encoding, recycleObjects);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::execute(const char *sql, const IFR_Length sqlLength, const IFR_StringEncoding encoding, IFR_Bool recycleObjects)
{
    DBUG_METHOD_ENTER(IFR_Statement, execute);
    IFR_Bool memory_ok=true;
    IFR_String sql_statement(sql, sqlLength, encoding, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        DBUG_RETURN(execute(sql_statement, recycleObjects));
    }
}


IFR_Retcode IFR_Statement::execute(const IFR_String &sql, IFR_Bool recycleObjects)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_Statement, execute);
    DBUG_PRINT(sql);
    DBUG_PRINT(recycleObjects);

  //>>> SQL TRACE ENTRY
  IFR_SQL_TRACE << endl << "::EXECUTE DBS " << m_CursorName << " "
                << currenttime << " [0x" << (void*)this << "]" << endl
                << "SQL COMMAND : " << sql << endl;
  IFR_SQL_TRACE_IF((sql.getEncoding() != IFR_StringEncodingAscii)) << "ENCODING : " << sql.getEncoding() << endl;
  //<<< SQL TRACE ENTRY  

  if(assertOpen()) {
    DBUG_RETURN(IFR_NOT_OK);
  }

  IFR_STMT_PROFILE_COUNT_INC(IFR_EXECUTECOUNT);
  if (sql.getLength() == 0) {
    error().setRuntimeError(IFR_ERR_EMPTY_SQL_STATEMENT);    
    DBUG_RETURN(IFR_NOT_OK);
  }
  this->error().clear();

  this->resetResults();
  IFR_Retcode rc = IFR_OK;
  IFRPacket_ReplyPacket replyPacket;
  rc = this->sendSQL(sql, replyPacket, false, false, false, 
                     recycleObjects?IFR_Connection::AppendAllowed_C:IFR_Connection::AppendNotAllowed_C);
  if (rc == IFR_OK) {
      rc = this->parseResult (&sql, replyPacket, false);
  }
  DBUG_RETURN(rc);
}

void IFR_Statement::setMaxRows(IFR_UInt4 max) 
{
    DBUG_METHOD_ENTER(IFR_Statement, setMaxRows);
    DBUG_PRINT(max);
    m_MaxRows = max;
    clearError();
}

IFR_UInt4 IFR_Statement::getMaxRows() const 
{
    DBUG_METHOD_ENTER(IFR_Statement, getMaxRows);
    ((IFR_Statement *)this)->clearError();
    DBUG_RETURN(this->m_MaxRows);
}

const int  IFR_Statement::getRowsAffected() const 
{
    DBUG_METHOD_ENTER(IFR_Statement, getRowsAffected);
    IFR_SQL_TRACE << endl << "::GET ROWS AFFECTED " 
                  << "[0x" << (void*)this << "]" << endl;
    IFR_SQL_TRACE << "ROWS: " << this->m_RowsAffected << endl;
    ((IFR_Statement *)this)->clearError();
    DBUG_RETURN(this->m_RowsAffected);
}

IFR_Statement::ConcurrencyType IFR_Statement::getResultSetConcurrencyType () 
{
    DBUG_METHOD_ENTER(IFR_Statement, getResultSetConcurrencyType);
    clearError();
    DBUG_RETURN(m_ResultSetConcurrency);
}

void IFR_Statement::setResultSetConcurrencyType (ConcurrencyType type) 
{
    DBUG_METHOD_ENTER(IFR_Statement, setResultSetConcurrencyType);
    m_ResultSetConcurrency = type;
    clearError();
}

void IFR_Statement::setResultSetType (ResultSetType type) 
{
    DBUG_METHOD_ENTER(IFR_Statement, setResultSetType);
    m_ResultSetType = type;
    clearError();
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_Statement::setCursorName(const char *_name, 
                             IFR_Length length,
                             IFR_StringEncoding encoding) 
{
    DBUG_METHOD_ENTER(IFR_Statement, setCursorName);

    IFR_Bool memory_ok = true;
    IFR_String name(_name, length, encoding, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_PRINT(name);

    if(m_cursorstate == CursorStateUsed_C) {
        getConnection()->dropCursor(m_CursorName, memory_ok);
        if(!memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    } 
    
    m_CursorName.assign(name, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    m_cursorstate = CursorStateUsed_C;
    clearError();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
#define COMMANDINFO_MAXLEN 40
IFR_Retcode IFR_Statement::setCommandInfo(const char *_commandInfo, 
                               SQLDBC_Length bufferLength, 
                               SQLDBC_Int4 lineNumber)   {
    DBUG_METHOD_ENTER(IFR_Statement, setCommandInfo);
    DBUG_PRINT(lineNumber);

    IFR_Bool memory_ok = true;
    if (bufferLength == SQLDBC_NULL_DATA) {
      m_commandInfo.clear();
      m_commandLineNumber = 0;
      DBUG_RETURN(IFR_OK);
    }
    
    if (bufferLength > COMMANDINFO_MAXLEN){
        error().setRuntimeError(IFR_ERR_COMMAND_INFO_TOOLONG, bufferLength, COMMANDINFO_MAXLEN);
        DBUG_RETURN(IFR_DATA_TRUNC);
    }

    m_commandInfo.setBuffer(_commandInfo, bufferLength, IFR_StringEncodingAscii, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    DBUG_PRINT(m_commandInfo);
    m_commandLineNumber=lineNumber;
    m_commandInfoState = CommandInfoSet_C;
    clearError();
    DBUG_RETURN(IFR_OK);
}               
//----------------------------------------------------------------------
IFR_Bool
IFR_Statement::isQuery() const
{
    DBUG_METHOD_ENTER(IFR_Statement, isQuery);
    ((IFR_Statement*)this)->clearError();
    DBUG_RETURN(m_resultset != NULL);
}


IFR_ResultSet* IFR_Statement::getResultSet() const 
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_Statement, getResultSet);
    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::GET RESULT SET " 
                  << "[0x" << (void*)this << "]" << endl;
    IFR_SQL_TRACE << "CURSOR NAME: " << m_CursorName << " [0x" << (void*)m_resultset << "]" << endl;
    //<<< SQL TRACE ENTRY  
    ((IFR_Statement*)this)->clearError();
    DBUG_RETURN(m_resultset);
}

void IFR_Statement::setFetchSize (IFR_Int2 rows) {
    DBUG_METHOD_ENTER(IFR_Statement, setFetchSize);
    DBUG_PRINT(rows);
    if (rows <= 0) {
        this->m_FetchSize = -1;
    }
    this->m_FetchSize = SAPDB_MAX_INT2;
}

IFR_Int2 IFR_Statement::getFetchSize() const {
  DBUG_METHOD_ENTER(IFR_Statement, getFetchSize);
  DBUG_RETURN(m_FetchSize);
}

//----------------------------------------------------------------------
IFR_Connection * 
IFR_Statement::getConnection() const 
{
    return m_Connection;
}

//----------------------------------------------------------------------
IFR_Bool
IFR_Statement::isQuery(const IFR_String &sqlcommand)
{
    // the general sp routines do not contain a comparision
    // that matches our needs for non copying and 
    // case-insensitiveness
    char *buffer       =(char*)sqlcommand.getBuffer();
    char *endbuf = buffer + sqlcommand.getLength();
    IFR_Int4  step   =1;          
    IFR_Int4  checknullpos = 0;

    IFR_StringEncoding encoding=sqlcommand.getEncoding();
    // we can handle UTF8 equally for this check, as we check only for 
    // ISO Latin 1 spaces
    if(encoding == IFR_StringEncodingUCS2) {
        step = 2;
        ++buffer;
        checknullpos = -1; 
    } else if(encoding == IFR_StringEncodingUCS2Swapped) {
        step = 2;
        checknullpos=1;
    }
    
    // skip spaces and (
    while(buffer < endbuf) {
        if((*buffer < 0x20 || *buffer=='(') &&
           (checknullpos==0 || *(buffer + checknullpos)==0)) {
            buffer += step;
        } else {
            break;
        }
    }
    
    // now check whether we have 'SELECT' following (case-insensitive)
    // check the letter, and check that it is in Latin1
#define CHECK_CHAR(upper, lower)                                \
    if(buffer < endbuf)                                         \
        if((*buffer == upper || *buffer==lower) &&              \
           (checknullpos==0 || *(buffer + checknullpos)==0)) {  \
            buffer += step;                                     \
        } else {                                                \
            return false;                                       \
        }                                                       \
    else                                                        \
        return false;                                           

    CHECK_CHAR('S', 's');
    CHECK_CHAR('E', 'e');
    CHECK_CHAR('L', 'l');
    CHECK_CHAR('E', 'e');
    CHECK_CHAR('C', 'c');
    CHECK_CHAR('T', 't');
    
    return true;
#undef CHECK_CHAR        
}

//----------------------------------------------------------------------
IFR_StringEncoding IFR_Statement::getCommandEncoding() const
{
    DBUG_METHOD_ENTER(IFR_Statement, getCommandEncoding);
    DBUG_RETURN(m_CommandEncoding);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::addApplicationInfo(IFRPacket_RequestSegment& requestsegment)
{
    DBUG_METHOD_ENTER(IFR_Statement, addApplicationInfo);
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Bool 
IFR_Statement::setCommandEncoding(IFR_StringEncoding encoding, IFR_Bool swapped)
{
  DBUG_METHOD_ENTER(IFR_Statement, setCommandEncoding);
  DBUG_PRINT(encoding);
  DBUG_PRINT(swapped);
    if (encoding != IFR_StringEncodingAscii && !m_Connection->isUnicodeDatabase()) {
        DBUG_RETURN(false);
    } 
    if(encoding == IFR_StringEncodingAscii ) {
        m_CommandEncoding = IFR_StringEncodingAscii;
    } else if(encoding != IFR_StringEncodingAscii && m_Connection->isUnicodeDatabase()) {
        if(swapped) {
            m_CommandEncoding = IFR_StringEncodingUCS2Swapped;
        } else {
            m_CommandEncoding = IFR_StringEncodingUCS2;
        }
    }
    DBUG_PRINT(m_CommandEncoding);
    DBUG_RETURN(true);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::setRowArraySize(IFR_Length rowarraysize)
{
    DBUG_METHOD_ENTER(IFR_Statement, setRowArraySize);
    
    if(rowarraysize == 0 || rowarraysize < 0 || rowarraysize > MAX_IFR_UINT4) {
        error().setRuntimeError(IFR_ERR_INVALID_ROWARRAYSIZE);
        DBUG_RETURN(IFR_NOT_OK);
    }

    error().clear();

    // expand the status array, if necessary
    if(m_rowstatussize < rowarraysize) {
        IFR_Length usefornew=32;
        IFR_Int4  c=0;
        while(c < 26 && usefornew < rowarraysize) {
            ++c;
            usefornew = usefornew << 1;
        }
        if(usefornew < rowarraysize) {
            usefornew = rowarraysize;
        }
        IFR_Int4 *tmp = new IFR_ALLOCATOR(allocator) IFR_Int4[usefornew];
        // copy only the information
        memcpy(tmp, m_rowstatusarray, sizeof(IFR_Int4) * m_rowarraysize); 
        for(IFR_Length j = m_rowarraysize; j<rowarraysize; ++j) {
            tmp[j]=IFR_EXECUTE_FAILED;
        }
        IFRUtil_DeleteArray(m_rowstatusarray, m_rowarraysize, IFR_ALLOCATOR(allocator));
        m_rowstatusarray = tmp;
        m_rowstatussize = usefornew;
        m_rowarraysize = rowarraysize;
    } else {
        m_rowarraysize = rowarraysize;
    }
    
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Length
IFR_Statement::getRowArraySize() const
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getRowArraySize);
    ((IFR_Statement*)this)->clearError();
    DBUG_RETURN(m_rowarraysize);
}

//----------------------------------------------------------------------
const IFR_Int4*
IFR_Statement::getRowStatus() const
{
    DBUG_METHOD_ENTER(IFR_PreparedStmt, getRowStatus);
    ((IFR_Statement*)this)->clearError();
    DBUG_RETURN(m_rowstatusarray);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::addBatch(const char *sql, 
                        const IFR_Length sqlLength,
                        const IFR_StringEncoding encoding)
{
    DBUG_METHOD_ENTER(IFR_Statement, addBatch);
    IFR_Bool memory_ok=true;
    IFR_String statement(sql, sqlLength, encoding, allocator, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(isQuery(statement)) {
        error().setRuntimeError(IFR_ERR_SQLCMD_RESULTSET);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(m_batchelements == 0) {
        IFR_String tmp(allocator);
        m_batchelements = new IFR_ALLOCATOR(allocator) IFRUtil_Vector<IFR_String>(allocator, 
                                                                                  0,
                                                                                  tmp,
                                                                                  memory_ok);
        if(m_batchelements == 0) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    m_batchelements->InsertEnd(statement, memory_ok);
    if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    setRowArraySize(m_batchelements->GetSize());
    clearError();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::addBatch(const char *sql, 
                        const IFR_StringEncoding encoding)
{
    DBUG_METHOD_ENTER(IFR_Statement, addBatch);
    DBUG_RETURN(addBatch(sql, IFR_NTS, encoding));
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::clearBatch()
{
    DBUG_METHOD_ENTER(IFR_Statement, clearBatch);
    IFRUtil_Delete(m_batchelements, allocator);
    m_batchelements = 0;
    setRowArraySize(1);
    clearError();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::getLastInsertedKey(IFR_Int4 tag,
                                  IFR_HostType hosttype,
                                  void *paramAddr,
                                  IFR_Length *lengthIndicator,
                                  IFR_Length size,
                                  IFR_Bool terminate)
{
    DBUG_METHOD_ENTER(IFR_Statement, getLastInsertedKey);
    //>>> SQL TRACE ENTRY
    IFR_SQL_TRACE << endl << "::GET LAST SERIAL " << m_CursorName << endl;
    //<<< SQL TRACE ENTRY
    
    IFR_Retcode rc        = IFR_OK;
    IFR_Bool    memory_ok = true;
    error().clear();

    if (!m_Connection->isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if (size < 0){
        error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, 1, size);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(tag != SQLDBC_FIRST_INSERTED_SERIAL && 
       tag != SQLDBC_LAST_INSERTED_SERIAL) {
        error().setRuntimeError(IFR_ERR_INVALID_SERIAL_VALUE_TAG);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    if(m_lastserial == 0) {
        IFR_SQL_TRACE << "*** NOT FOUND ***" << endl;
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }

    IFR_Parameter parameter(hosttype,
                            terminate,
                            paramAddr,
                            lengthIndicator,
                            size);
    
    const char *tag_str;
    switch(tag) {
    case SQLDBC_FIRST_INSERTED_SERIAL:
        tag_str = "FST";
        break;
    case SQLDBC_LAST_INSERTED_SERIAL:
        tag_str = "LST";
        break;
    default:
        tag_str = "???";
    }
    
    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << "PARAMETER" << endl
                         << IFR_SQLTRACE_PARAMETER_HEADER << endl;
        parameter.sqlTraceParameter(IFR_TRACE_STREAM, tag_str);
        IFR_TRACE_STREAM << endl;
    }
    //<<< SQL TRACE
    
    IFRConversion_Converter * info = getConnection()->findSpecialParameter(tag);
    if(info == 0) {
        IFR_SQL_TRACE << "*** NOT FOUND ***" << endl;
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    IFR_Length  dataoffset = 0;
    IFR_Length  offset     = 0;
    rc = info->translateOutput(*m_lastserial,
                               parameter,
                               *this,
                               0,
                               0);
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
// Gives a crude, but fast enough upper bound on the string size
// of a string in a specific encoding
static IFR_size_t
get_stringsize(IFR_String& src, IFR_StringEncoding destEncoding)
{
    IFR_StringEncoding srcEncoding=src.getEncoding();
    if(srcEncoding == destEncoding ||
       (srcEncoding == IFR_StringEncodingUCS2 && destEncoding == IFR_StringEncodingUCS2Swapped) ||
       (srcEncoding == IFR_StringEncodingUCS2Swapped && destEncoding == IFR_StringEncodingUCS2)) {
        return src.getStrLen();
    } else if(destEncoding == IFR_StringEncodingAscii) {
        if(srcEncoding == IFR_StringEncodingUCS2 ||
           srcEncoding == IFR_StringEncodingUCS2Swapped) {
            return src.getStrLen() / 2;
        } else {
            return src.getStrLen();
        }
    } else if(destEncoding == IFR_StringEncodingUCS2 ||
              destEncoding == IFR_StringEncodingUCS2Swapped) {
        return src.getStrLen() * 2;
    } else if(destEncoding == IFR_StringEncodingUTF8) {
        if(srcEncoding == IFR_StringEncodingAscii) {
            return src.getStrLen() * 3;
        } else {
            return (src.getStrLen() * 3 / 2) + 1;
        }
    }
    return 0;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::executeBatch()
{
    DBUG_METHOD_ENTER(IFR_Statement, executeBatch);
    
    error().clear();
    
    if(m_batchelements == 0) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    
    resetResults();
    
    IFR_StringEncoding batchEncoding = IFR_StringEncodingAscii;
    IFR_Length num_batchelements = m_batchelements->GetSize();
    for(IFR_Int4 i=0; i< num_batchelements; ++i) {
        if((*m_batchelements)[i].getEncoding() != IFR_StringEncodingAscii) {
            batchEncoding = IFR_StringEncodingUCS2;
            break;
        }
    }

    
    IFR_Int4    current = 0;
    IFR_Retcode rc;
    bool errorSet = false;
    
    while(current < num_batchelements) {
        {
            IFRPacket_RequestPacket requestpacket(*this);
            IFRPacket_ReplyPacket   replypacket;
            
            // If we can not set the command encoding, we will fall back to ASCII,
            // and bail out at the time we cannot convert the batch items.
            
            rc = m_Connection->getRequestPacket(requestpacket, error());
            if(rc != IFR_OK) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            if(!setCommandEncoding(batchEncoding, requestpacket.isSwapped())) {
                setCommandEncoding(IFR_StringEncodingAscii, requestpacket.isSwapped());
            }
            requestpacket.setEncoding(getCommandEncoding());
            IFR_Bool full=false;
            IFR_Int4 packetStart=current;
            IFR_StringEncoding packetEncoding = requestpacket.getEncoding();
            
            do {
                {
                    IFR_size_t partdatasize = get_stringsize((*m_batchelements)[current], packetEncoding);
                    if(partdatasize < 256) { 
                        partdatasize = 256; 
                    }
                    
                    // The room we need is either
                    // - enough size for this statement, and for this error text, OR
                    // - enough size for all error texts for all segments                    
                    if(((IFR_size_t)requestpacket.remainingBytes() <=
                       sizeof(tsp1_segment_header) +   // one segment
                       sizeof(tsp1_part_header) +      // one part
                       partdatasize +                  //  of size of the statement, or the errortext
                       8)
                       || 
                       ((IFR_size_t)requestpacket.dataSize() <=
                        (current - packetStart + 1) *    // number of segments                   
                        (sizeof(tsp1_segment_header) +   // one segment
                         sizeof(tsp1_part_header) +      // one part
                         256) +                           // error text length
                        8)                             // plus alignment safety                           
                        ) {   
                        if(packetStart == current) {
                            error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                            DBUG_RETURN(IFR_NOT_OK);
                        } else {
                            full = true;
                            continue;
                        }
                    }


                    IFRPacket_RequestSegment currentSegment 
                        = IFRPacket_RequestSegment(requestpacket,
                                                   IFRPacket_CommandMessageType::Dbs_C,
                                                   requestpacket.getSQLMode(),
                                                   false,
                                                   packetStart != current);
                    if(packetStart==current && !currentSegment.isValid()) {
                        error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                        DBUG_RETURN(IFR_NOT_OK);
                    } else if(!currentSegment.isValid()) {
                        full = true;
                        continue;
                    }
                    if(m_Connection->getAutoCommit()) {
                        currentSegment.setCommitImmediately();
                    }
                    
                    IFRPacket_CommandPart command;
                    rc = currentSegment.addPart(command);
                    if(rc == IFR_OK) {
                        rc = command.setText( (*m_batchelements)[current], error() );
                    }
                    if(rc != IFR_OK) {
                        if(packetStart==current) {
                            error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
                            DBUG_RETURN(IFR_NOT_OK);
                        } else {
                            full = true;
                            continue;
                        }
                    }
                    currentSegment.closePart();
                    currentSegment.close();
                    ++current;
                }
            } while(!full && current<num_batchelements);
            rc = m_Connection->sqlaexecute(requestpacket, replypacket, IFR_Connection::AppendNotAllowed_C, error());
            if(rc != IFR_OK) {
                DBUG_RETURN(rc);
            } else if(error()) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            IFRPacket_ReplySegment lastSegment = IFRPacket_ReplySegment(PIn_ReplySegment());
            for(IFR_Int i=packetStart; i<current; ++i) {
                IFRPacket_ReplySegment replysegment =  (i==packetStart)
                    ? IFRPacket_ReplySegment(replypacket.GetFirstSegment())
                    : IFRPacket_ReplySegment(replypacket.GetNextSegment(lastSegment));
                if(!replysegment.IsValid()) {
                    m_rowstatusarray[i] = IFR_EXECUTE_FAILED;
                    if(!errorSet) {
                        error().setRuntimeError(IFR_ERR_INVALID_REPLYPACKET);
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
                if(replysegment.ErrorCode() == 0) {
                    if(replysegment.getResultCount(m_rowstatusarray[i]) == IFR_NO_DATA_FOUND) {
                        m_rowstatusarray[i] = IFR_SUCCESS_NO_INFO;
                    }
                } else {
                    m_rowstatusarray[i] = IFR_EXECUTE_FAILED;
                    if(!errorSet) {
                        replysegment.getSQLError(error(), allocator);
                        errorSet = true;
                    }
                }
                lastSegment = replysegment;
            }
        }
    }
    if(errorSet) {
        DBUG_RETURN(IFR_NOT_OK);
    } else {
        DBUG_RETURN(IFR_OK);
    }
}

//----------------------------------------------------------------------
void
IFR_Statement::resetResults()
{
    DBUG_METHOD_ENTER(IFR_Statement, resetResults);
    IFRUtil_Delete(m_resultset, allocator);
    IFRUtil_Delete(m_lastserial, allocator);
    m_lastserial = 0;
    m_resultset = 0;
    m_RowsAffected = 0;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::setSerialPart(IFRPacket_DataPart& serialData)
{
    DBUG_METHOD_ENTER(IFR_Statement, setSerialPart);
    if(serialData.isValid ()) {
        IFR_size_t copysize=sizeof(tsp1_part_header) + serialData.Length();
        IFR_Byte *bsource=(IFR_Byte*) serialData.GetRawPart();
        IFR_Byte *bdest = (IFR_Byte *) IFR_ALLOCATOR(allocator).Allocate(copysize);
        if(bdest == 0) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        memcpy(bdest, bsource, copysize);
        if(m_lastserial) {
            IFRUtil_Delete(m_lastserial, allocator);
        }
        m_lastserial=new IFR_ALLOCATOR(allocator) CopiedDataPart(bdest,
                                                                 serialData.getEncoding(),
                                                                 allocator);
        if(m_lastserial == 0) {
            allocator.Deallocate(bdest);
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_Statement::updateSerialPart(IFRPacket_DataPart& serialData)
{
    DBUG_METHOD_ENTER(IFR_Statement, setSerialPart);
    if(serialData.isValid () && m_lastserial) {
        // Copy the 2nd number on the 1st for batch
        char *src = serialData.getOutputData(21);
        char *dest = m_lastserial->getOutputData(21);
        memcpy(dest, src, 21);
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
void IFR_Statement::functionCode2Statistic(IFRPacket_FunctionCode::FunctionCode fcode){
  switch(fcode){
        case (IFRPacket_FunctionCode::Insert_FC ) :
        case (IFRPacket_FunctionCode::Minsert_FC) :{
          //insert
          IFR_STMT_PROFILE_COUNT_INC(IFR_INSERTCOUNT);
          break;
        }

        case (IFRPacket_FunctionCode::Select_FC      ) :
        case (IFRPacket_FunctionCode::Explain_FC     ) :
        case (IFRPacket_FunctionCode::Show73_FC      ) :
        case (IFRPacket_FunctionCode::SelectDirect_FC) :
        case (IFRPacket_FunctionCode::SelectFirst_FC ) :
        case (IFRPacket_FunctionCode::SelectLast_FC  ) :
        case (IFRPacket_FunctionCode::SelectNext_FC  ) :
        case (IFRPacket_FunctionCode::SelectPrev_FC  ) :
        case (IFRPacket_FunctionCode::SelectInto_FC  ) :
        case (IFRPacket_FunctionCode::Mselect_FC     ) :
        case (IFRPacket_FunctionCode::MselectInto_FC ) :{
          //select
          IFR_STMT_PROFILE_COUNT_INC(IFR_SELECTCOUNT);
          break;
        }

        case (IFRPacket_FunctionCode::Update_FC ) :
        case (IFRPacket_FunctionCode::Mupdate_FC) :{
          //update
          IFR_STMT_PROFILE_COUNT_INC(IFR_UPDATECOUNT);
          break;
        }
        
        case (IFRPacket_FunctionCode::Delete_FC ) : 
        case (IFRPacket_FunctionCode::Mdelete_FC) :{
          //delete
          IFR_STMT_PROFILE_COUNT_INC(IFR_DELETECOUNT);
          break;
        }

        case (IFRPacket_FunctionCode::PlSqlExecute_FC              ) :
        case (IFRPacket_FunctionCode::DBProcWithResultSetExecute_FC) :{
          //call dbproc
          IFR_STMT_PROFILE_COUNT_INC(IFR_CALLDBPROCEDURECOUNT);
          break;
        }

        case (IFRPacket_FunctionCode::FetchFirst_FC    ) :
        case (IFRPacket_FunctionCode::FetchLast_FC     ) :
        case (IFRPacket_FunctionCode::FetchNext_FC     ) :
        case (IFRPacket_FunctionCode::FetchPrev_FC     ) :
        case (IFRPacket_FunctionCode::FetchPos_FC      ) :
        case (IFRPacket_FunctionCode::FetchSame_FC     ) :
        case (IFRPacket_FunctionCode::FetchRelative_FC ) :
        case (IFRPacket_FunctionCode::MfetchFirst_FC   ) :
        case (IFRPacket_FunctionCode::MfetchLast_FC    ) :
        case (IFRPacket_FunctionCode::MfetchNext_FC    ) :
        case (IFRPacket_FunctionCode::MfetchPrev_FC    ) :
        case (IFRPacket_FunctionCode::MfetchPos_FC     ) :
        case (IFRPacket_FunctionCode::MfetchSame_FC    ) :
        case (IFRPacket_FunctionCode::MfetchRelative_FC) :{
          //fetch
          IFR_STMT_PROFILE_COUNT_INC(IFR_FETCHCOUNT);
          break;
        }

        case (IFRPacket_FunctionCode::CreateTable_FC    ) :
        case (IFRPacket_FunctionCode::SetRole_FC        ) :
        case (IFRPacket_FunctionCode::DropRole_FC       ) :
        case (IFRPacket_FunctionCode::DropView_FC       ) :
        case (IFRPacket_FunctionCode::DropTable_FC      ) :
        case (IFRPacket_FunctionCode::CreateView_FC     ) :
        case (IFRPacket_FunctionCode::DropUser_FC       ) :
        case (IFRPacket_FunctionCode::CreateRole_FC     ) :
        case (IFRPacket_FunctionCode::CreateSequence_FC ) :
        case (IFRPacket_FunctionCode::AlterSequence_FC  ) :
        case (IFRPacket_FunctionCode::DropSequence_FC   ) :
        case (IFRPacket_FunctionCode::CreateSchema_FC   ) :
        case (IFRPacket_FunctionCode::CreateCluster_FC  ) :
        case (IFRPacket_FunctionCode::CreateUser_FC     ) :
        case (IFRPacket_FunctionCode::CreateIndex_FC    ) :
        case (IFRPacket_FunctionCode::DropIndex_FC      ) :
        case (IFRPacket_FunctionCode::DropCluster_FC    ) :
        case (IFRPacket_FunctionCode::CreateProcedure_FC) :
        case (IFRPacket_FunctionCode::AlterProcedure_FC ) :        
        case (IFRPacket_FunctionCode::AlterTable_FC     ) :        
        case (IFRPacket_FunctionCode::Grant_FC          ) :        
        case (IFRPacket_FunctionCode::Revoke_FC         ) :        
        case (IFRPacket_FunctionCode::CreateSynonym_FC  ) :        
        case (IFRPacket_FunctionCode::DropSynonym_FC    ) :        
        case (IFRPacket_FunctionCode::DropSchema_FC     ) :            
        case (IFRPacket_FunctionCode::AlterIndex_FC     ) :        
        case (IFRPacket_FunctionCode::CreateTrigger_FC  ) :        
        case (IFRPacket_FunctionCode::AlterTrigger_FC   ) :        
        case (IFRPacket_FunctionCode::DropTrigger_FC    ) :        
        case (IFRPacket_FunctionCode::DropProcedure_FC  ) : {
          //DDL
          break;
        }       
  }
}

//======================================================================
// IFR_StatementProfile
//======================================================================
IFR_StatementProfile::IFR_StatementProfile()
{
  this->resetCounters();
}
//----------------------------------------------------------------------

void IFR_StatementProfile::resetCounters(){
      memset(&m_counter_8[0], 0, sizeof(m_counter_8));
      memset(&m_counter_4[0], 0, sizeof(m_counter_4));
}


//----------------------------------------------------------------------
void IFR_StatementProfile::submitCounters(IFR_UInt8 *counter_8, IFR_UInt4 *counter_4){
  IFR_Statement* stmt = static_cast <IFR_Statement*> (this);
  m_counter_4[IFR_SQLERRORCOUNT_STMT] += stmt->error().getErrorCount();
  m_counter_4[IFR_ROWNOTFOUNDCOUNT_STMT] += stmt->error().getRowNotFoundCount();
  if (stmt->m_resultset){
    m_counter_4[IFR_SQLERRORCOUNT_STMT] += stmt->m_resultset->error().getErrorCount();
    m_counter_4[IFR_ROWNOTFOUNDCOUNT_STMT] += stmt->error().getRowNotFoundCount();
  }

  MAP_PROFILE_COUNTER_8_STMT(IFR_DATAREADSIZE);
  MAP_PROFILE_COUNTER_8_STMT(IFR_DATAWRITESIZE);
  MAP_PROFILE_COUNTER_8_STMT(IFR_LONGDATAREADSIZE);
  MAP_PROFILE_COUNTER_8_STMT(IFR_LONGDATAWRITESIZE);

  MAP_PROFILE_COUNTER_STMT(IFR_NOTSELECTFETCHOPTIMIZED);
  MAP_PROFILE_COUNTER_STMT(IFR_PREPARECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_PARSEINFOHITCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_PARSINFOMISSCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_PARSEAGAINCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_PARSEINFOSQUEEZECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_EXECUTECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_INSERTCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_UPDATECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_SELECTCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_CALLDBPROCEDURECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_DELETECOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_FETCHCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_SQLERRORCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_ROWNOTFOUNDCOUNT);
  MAP_PROFILE_COUNTER_STMT(IFR_FETCHROWCOUNT);
  this->resetCounters();
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFR_Statement& statement)
{
    char str[64];
    sp77sprintf(str, 64, "IFR_Statement@%p", &statement);
    return s << str;
}

IFR_END_NAMESPACE
