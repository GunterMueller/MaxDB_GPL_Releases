/*!
  @file           IFR_RowSet.cpp
  @author         ThomasS
  @ingroup        IFR_Fetch
  @brief          Implements class for rowset processing
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


#include "Interfaces/Runtime/IFR_RowSet.h"
#include "SAPDB/Interfaces/Runtime/IFR_Statement.h"

IFR_BEGIN_NAMESPACE

IFR_RowSet::IFR_RowSet (IFR_ResultSet *rs, IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(*rs->m_Connection),
 IFR_ConnectionItem(rs->m_Connection),
 IFR_LOBHost(rs->m_Connection->allocator),
 IFR_GetvalHost(rs->m_Connection, rs->m_Connection->allocator),
 m_resultset(rs),
 m_lastgetobjcol(0),
 m_offset(1)
{
    DBUG_METHOD_ENTER(IFR_RowSet, IFR_RowSet);
    error().clear();
}

//----------------------------------------------------------------------
IFR_RowSet::~IFR_RowSet ()
{
    DBUG_METHOD_ENTER(IFR_RowSet, ~IFR_RowSet);
}

//----------------------------------------------------------------------
IFR_Connection*
IFR_RowSet::getConnection() const
{
    // It is necessary to get the connection from the result set, as it is not stored
    // directly here.
    return m_resultset->m_Connection;
}

//----------------------------------------------------------------------
IFR_GetvalHost*
IFR_RowSet::getGetvalHost()
{
    return this;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_RowSet::setPos (IFR_UInt4 pos)
{
  DBUG_METHOD_ENTER(IFR_RowSet, setPos);
  DBUG_PRINT(pos);
  //<<< SQL TRACE
  IFR_SQL_TRACE << endl << "::SETPOS " << currenttime
                << " [0x" << (void*)this << "]" << endl;
  IFR_SQL_TRACE << "POSITION: " << pos << endl;
  //>>> SQL TRACE

  IFR_Retcode rc = IFR_OK;
  error().clear();

  if (!m_resultset->m_Connection->isConnected()) {
    error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
    return (IFR_NOT_OK);
  }

  if ((rc = m_resultset->assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }

  if (m_resultset->m_PositionState != IFR_ResultSet::IFR_POSITION_INSIDE) {
    if (m_resultset->m_PositionState == IFR_ResultSet::IFR_POSITION_BEFORE_FIRST) {
      m_resultset->error().setRuntimeError (IFR_ERR_RESULTSET_BEFOREFIRST);
    } else {
      m_resultset->error().setRuntimeError (IFR_ERR_RESULTSET_AFTERLAST);
    }
    DBUG_RETURN(IFR_NOT_OK);
  }

  m_lastgetobjcol = 0;
  m_offset        = 1;

  if ((pos == 0) || (pos > (IFR_UInt4)m_resultset->m_rowsetsize)) {
    m_resultset->error().setRuntimeError(IFR_ERR_INVALID_ROWSET_POS_I, pos);
    DBUG_RETURN(IFR_NOT_OK);
  }

  if (m_resultset->m_rowsetsize == 1) {
    DBUG_RETURN(m_resultset->mfAbsolute(m_resultset->m_CurrentChunk->getLogicalPos ()));
  }

  DBUG_RETURN(m_resultset->mfAbsolute(m_resultset->m_rowsetstartrow+pos-1));
}

IFR_Retcode
IFR_RowSet::getObject(IFR_Int4 columnindex,
                      void* paramaddr,
                      IFR_Length parambytelength,
                      IFR_HostType paramtype,
                      IFR_Length* paramlengthindicator,
                      IFR_Bool terminate,
                      IFR_Length* paramposindicator,
                      IFR_Int4 rowToUse)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_RowSet, getObject);

    //>>> SQL TRACE
    IFR_SQL_TRACE << endl << "::GETOBJECT " << m_resultset->m_FetchInfo->m_CursorName << endl;

    //<<< SQL TRACE
    IFR_Parameter parameter (paramtype,
                             terminate,
                             paramaddr,
                             paramlengthindicator,
                             parambytelength,
                             paramposindicator);
    //>>> SQL TRACE
    if(IFR_SQL_TRACE_ENABLED) {
        IFR_TRACE_STREAM << "COLUMN" << endl
                         << IFR_SQLTRACE_PARAMETER_HEADER << endl;
        parameter.sqlTraceParameter(IFR_TRACE_STREAM, columnindex);
        IFR_TRACE_STREAM << endl;
    }
    //<<< SQL TRACE

    if(rowToUse == -1) {
        if (this->m_resultset->m_CurrentChunk)
          rowToUse = this->m_resultset->m_CurrentChunk->getLogicalPos();
        else
          rowToUse = getStartRow();
    } else {
        if(rowToUse >= getStartRow() + m_resultset->m_FetchedRows ||
           rowToUse < getStartRow()) {
            m_resultset->error().setRuntimeError(IFR_ERR_INVALID_ROWSET_POS_I, rowToUse);
            error().assign(m_resultset->error(), false); // already counted in resultset
            DBUG_RETURN(IFR_NOT_OK);
        }
    }


    IFR_Retcode rc        = IFR_OK;
    IFR_Bool    memory_ok = true;
    error().clear();

    if (!m_resultset->m_Connection->isConnected()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        return (IFR_NOT_OK);
    }

    if ((rc = m_resultset->assertNotClosed ()) != IFR_OK) {
        DBUG_RETURN(rc);
    }

    if (parambytelength < 0){
        error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, columnindex, parambytelength);
        DBUG_RETURN(IFR_NOT_OK);
    }


    IFRConversion_Converter *info;
    if ((info = m_resultset->findColumnInfo (columnindex)) == 0) {
        m_resultset->error().setRuntimeError(IFR_ERR_INVALID_COLUMNINDEX_I, columnindex);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFRPacket_DataPart part;
    if ((rc = m_resultset->getCurrentData (part)) != IFR_OK) {
        error().assign(m_resultset->error(), false); // already counted in resultset
        DBUG_RETURN(rc);
    }


    if(!info->isLong()) {

        if(m_lastgetobjcol != columnindex) {
            m_lastgetobjcol = columnindex;
            m_offset        = 1;
        }

        if(paramposindicator) {
            m_offset = *paramposindicator;
            if(m_offset == 0) {
                m_offset = 1;
            }
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
        IFRConversion_Getval *getval = findOutputLong(columnindex,
                                                      rowToUse);
        if(getval == 0) {
            getval = info->createGetval (part,
                                         &parameter,
                                         *this,
                                         rowToUse,
                                         rowToUse - getStartRow(),
                                         0);
            if(getval == 0) {
                if(m_resultset->error()) {
                    DBUG_RETURN(IFR_NOT_OK);
                } else {
                    DBUG_RETURN(IFR_OK);
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
            if(!parameter.isBinary() && getval->isBinary() &&m_resultset->m_Connection->odbcBinToHex()) {
                getval->setBinaryToHex(true);
            } else {
                getval->setBinaryToHex(false);
            }
        }

        // Reset the read position if the last call was not the same
        // column.
        if(m_lastgetobjcol != columnindex) {
            m_lastgetobjcol = columnindex;
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
            if(m_offset == 0) {
                m_offset = 1;
            }
        }

        // If the LONG is not positioned at the beginning, it usually
        // has stored its last own data as copy.
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
            IFRPacket_RequestPacket   requestpacket(*m_resultset);
            IFRPacket_ReplyPacket     replypacket;
            IFRPacket_RequestSegment  segment;
            IFRPacket_LongDataPart    longdatapart;

            while(true) {
                IFR_Retcode packet_rc = m_resultset->getConnection()->getRequestPacket(requestpacket, error());
                if(packet_rc != IFR_OK) {
                    getval->clearLongData();
                    DBUG_RETURN(IFR_NOT_OK);
                }
                m_resultset->getvalInitPacket(requestpacket, segment, longdatapart);
                getval->putDescriptor(longdatapart);
                segment.closePart();
                segment.close();

                IFR_Retcode sqlaexecute_rc = m_resultset->getConnection()->sqlaexecute(requestpacket,
                                                                                       replypacket,
                                                                                       IFR_Connection::AppendNotAllowed_C,
                                                                                       error());
                if (sqlaexecute_rc != IFR_OK) {
                    getval->clearLongData();
                    DBUG_RETURN(sqlaexecute_rc);
                } else if(error()) {
                    getval->clearLongData();
                    DBUG_RETURN(IFR_NOT_OK);
                }

                IFRPacket_ReplySegment replysegment(replypacket);
                IFR_Int4 updated_longcount;
                updateOutputLongs(replysegment, *this, updated_longcount, this->m_resultset->m_Statement->m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);

                if ((rc = replysegment.getPart (longdatapart)) != IFR_OK) {
                    error().setRuntimeError(IFR_ERR_MISSINGLONGDATAPART);
                    getval->clearLongData();
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

                if(rc ==  IFR_OK) {
                    if(*paramlengthindicator && *paramlengthindicator == IFR_NO_TOTAL) {
                        *paramlengthindicator = dataoffset;
                    }
                    break;
                } else if(rc == IFR_DATA_TRUNC) {
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
        parameter.sqlTraceParameterData(IFR_TRACE_STREAM, 1, rc, 0, 0, 0, 0, false);
        IFR_SQL_TRACE << endl;
    }
    //>>> SQL TRACE
    DBUG_RETURN(rc);
}

IFR_Retcode
IFR_RowSet::fetch ()
{
  DBUG_METHOD_ENTER(IFR_RowSet, fetch);
  DBUG_RETURN(m_resultset->fetch ());
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_RowSet::closeLOB(IFR_LOB* lob)
{
    DBUG_METHOD_ENTER(IFR_RowSet, closeLOB);
    if(lob->isInput()) {
        DBUG_RETURN(IFR_NOT_OK);
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
IFR_RowSet::getData(IFR_LOB    *lob,
                    void       *data,
                    IFR_Length *lengthindicator,
                    IFR_Length  bytelength,
                    IFR_Length *posindicator,
                    IFR_Bool    terminate)
{
    DBUG_METHOD_ENTER(IFR_RowSet, getData);
    DBUG_PRINT(lengthindicator);
    DBUG_PRINT(bytelength);
    DBUG_PRINT(posindicator);
    DBUG_PRINT(terminate);

    IFR_Int2 column = lob->getColumn();
    IFR_Int4 row    = lob->getRow();
    IFR_HostType datahosttype = lob->getDataHostType();
    DBUG_RETURN(getObject(column,
                          data,
                          bytelength,
                          datahosttype,
                          lengthindicator,
                          terminate,
                          posindicator,
                          row));
}

//----------------------------------------------------------------------
IFR_Length
IFR_RowSet::getLength(IFR_LOB *lob)
{
    error().clear();
    IFR_Int2 column = lob->getColumn();
    IFR_Int4 row    = lob->getRow();


    if(row > m_resultset->m_FetchedRows ||
       row < 1) {
        m_resultset->error().setRuntimeError(IFR_ERR_INVALID_ROWSET_POS_I, row + getStartRow() - 1);
        error().assign(m_resultset->error(), false); // already counted in resultset
        return -1;
    }

    IFRConversion_Converter *info;
    if ((info = m_resultset->findColumnInfo (column)) == 0) {
        m_resultset->error().setRuntimeError(IFR_ERR_INVALID_COLUMNINDEX_I, column);
        return -1;
    }

    IFRPacket_DataPart part;
    if (m_resultset->getCurrentData (part) != IFR_OK) {
        error().assign(m_resultset->error(), false); // already counted in resultset
        return -1;
    }

    IFRConversion_Getval *getval = findOutputLong(column, row);
    IFR_Bool memory_ok = true;
    if(getval == 0) {
        getval = info->createGetval (part,
                                     0,
                                     *this,
                                     row,
                                     row - getStartRow(),
                                     0);
        if(getval == 0) {
            if(m_resultset->error()) {
                return -1;
            } else {
                return -1;
            }
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
           && m_resultset->m_Connection->odbcBinToHex()) {
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

        IFR_Retcode packet_rc = m_resultset->getConnection()->getRequestPacket(requestpacket, error());
        if(packet_rc != IFR_OK) {
            getval->clearLongData();
            return -1;
        }
        m_resultset->getvalInitPacket(requestpacket, segment, longdatapart);
        getval->putDescriptorForGetLength(longdatapart);
        segment.closePart();
        segment.close();

        IFR_Retcode sqlaexecute_rc = m_resultset->getConnection()->sqlaexecute(requestpacket,
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
        updateOutputLongs(replysegment, *this, updated_longcount, this->m_resultset->m_Statement->m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);
        result = getval->getMaximumOutputLength();
    }


    return lob->getLengthValue(result,
                               getval->getDatatype(),
                               getval->isBinaryToHex());

}



//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, const IFR_RowSet& rowset)
{
    char str[64];
    sp77sprintf(str, 64, "IFR_RowSet@%p", &rowset);
    return s << str;
}


IFR_END_NAMESPACE
