/*!
  @file           IFR_ResultSet.cpp
  @author         ThomasS
  @ingroup        IFR_Fetch
  @brief          Implements class for result set processing
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


#include <limits.h>
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_ResultSet.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_Statement.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/IFR_UpdatableRowSet.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFR_ResultSet::IFR_ResultSet (IFR_Connection& connection,
                              IFR_FetchInfo& fetchInfo,
                              IFR_Statement& statement,
                              IFR_Int2 fetchSize,
                              int maxRows,
                              IFR_Int4 concurType,
                              IFR_Int4 type,
                              IFR_Bool empty,
                              IFR_FetchChunk *firstChunk,
                              IFR_Bool closeOnLast,
                              IFR_Bool& memory_ok)
:IFRUtil_RuntimeItem(connection),
 IFR_ConnectionItem(&connection),
 m_Connection (&connection),
 m_FetchInfo (&fetchInfo),
 m_Statement (&statement),
 m_MaxRows (maxRows),
 m_IsClosed (false),
 m_ParamVector (new IFR_ALLOCATOR(allocator) IFRUtil_Vector<IFR_Parameter>(allocator)),
 m_rowsetsize (1),
 m_FetchedRows (        0),
 m_rowset (0),
 m_upd_rowset (0),
 m_rowsetstartrow(1),
 m_rowstatusarray(allocator, 1, IFR_NOT_OK, memory_ok),
 m_concurType(concurType),
 m_type(type),
 m_hasposread(false),
 m_close_on_last(closeOnLast),
 m_binding_type(0)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, IFR_ResultSet);

  error().clear ();
  if(!memory_ok) {
      empty=true;
  }

  if (fetchSize >= 1) {
    this->m_FetchSize = fetchSize;
  } else {
    this->m_FetchSize = IFR_DEFAULT_FETCHSIZE;
  }
  this->initializeFields(empty);
  m_rowset = new IFR_ALLOCATOR(allocator) IFR_RowSet (this, memory_ok);
  if(m_rowset == 0 || m_ParamVector == 0 || !memory_ok) {
      IFRUtil_Delete(m_ParamVector, allocator);
      IFRUtil_Delete(m_rowset, allocator);
      m_ParamVector = 0;
      m_rowset      = 0;
      memory_ok = false;
  }
  if(firstChunk) {
      firstChunk->setRecordSize(fetchInfo.getRecordSize()); // must be called before init
      setCurrentChunk(firstChunk);
      this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
  } else if(m_close_on_last && empty) {
      m_Statement->setCursorClosed();
  }
}

void
IFR_ResultSet::initializeFields(IFR_Bool empty)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, initializeFields);
  this->m_CurrentChunk = 0;
  this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
  this->m_PositionStateOfChunk = IFR_POSITION_NOT_AVAILABLE;
  this->m_SafeFetchSize = 1;
  this->m_LargestKnownAbsPos = 1;
  this->m_MaxRowsOutSideResult = 0;
  if(this->m_Empty = empty) {
      this->m_RowsInResultSet = 0;
  } else {
      this->m_RowsInResultSet = -1;
  }
}

IFR_ResultSet::~IFR_ResultSet()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, ~IFR_ResultSet);
  m_ParamVector->Clear ();
  IFRUtil_Delete(m_ParamVector, allocator);
  IFRUtil_Delete(m_CurrentChunk, allocator);
  IFRUtil_Delete(m_FetchInfo, allocator);
  IFRUtil_Delete(m_rowset, allocator);
  IFRUtil_Delete(m_upd_rowset, allocator);
}

IFR_Retcode
IFR_ResultSet::bindColumn (const IFR_UInt4 paramIndex,
			   const IFR_HostType paramType,
			   void *paramAddr,
			   IFR_Length *paramLengthIndicator,
			   const IFR_Length paramLen,
                           IFR_Bool terminate)
{
  IFR_TRACE_UPDATE;
  DBUG_METHOD_ENTER(IFR_ResultSet, bindColumn);
  DBUG_PRINT(paramIndex);
  DBUG_PRINT(paramType);
  DBUG_PRINT(paramAddr);
  DBUG_PRINT(paramLengthIndicator);
  DBUG_PRINT(paramLen);
  DBUG_PRINT(terminate);

  //>>> SQL TRACE
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE << endl << "::BIND COLUMN " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
  }
  //<<< SQL TRACE
  IFR_Parameter parameter=IFR_Parameter (paramType,
                                         terminate,
                                         paramAddr,
                                         paramLengthIndicator,
                                         paramLen);
  //>>> SQL TRACE
  if(IFR_SQL_TRACE_ENABLED) {
      IFR_TRACE_STREAM << IFR_SQLTRACE_PARAMETER_HEADER << endl;
      parameter.sqlTraceParameter(IFR_TRACE_STREAM, paramIndex);
      IFR_TRACE_STREAM << endl;
  }
  //<<< SQL TRACE

  error().clear();
  IFR_Retcode rc = IFR_OK;
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if ((paramIndex == 0) || (paramIndex > (unsigned) this->m_FetchInfo->getColumnCount ())) {
    error ().setRuntimeError (IFR_ERR_INVALID_PARAMETERINDEX);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if(paramAddr == 0 && paramLengthIndicator == 0) {
    error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)paramIndex);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if(paramLen < 0) {
      error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, (IFR_Int4)paramIndex, (IFR_Int4)paramLen);
    DBUG_RETURN(IFR_NOT_OK);
  }
  IFR_Bool memory_ok=true;
  if (m_ParamVector->GetSize () < paramIndex) {
    m_ParamVector->Resize (paramIndex, memory_ok);
  }
  if(!memory_ok) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }
  (*m_ParamVector)[paramIndex-1] = parameter;
  DBUG_RETURN(IFR_OK);
}

IFR_Retcode
IFR_ResultSet::bindColumn (const IFR_UInt4 paramIndex,
                           const IFR_HostType paramType,
                           void *paramAddr,
                           IFR_Length *paramLengthIndicator,
                           IFR_Length *paramPosIndicator,
                           const IFR_Length paramLen,
                           IFR_Bool terminate)
{
  IFR_TRACE_UPDATE;
  DBUG_METHOD_ENTER(IFR_ResultSet, bindColumn);
  DBUG_PRINT(paramIndex);
  DBUG_PRINT(paramType);
  DBUG_PRINT(paramAddr);
  DBUG_PRINT(paramLengthIndicator);
  DBUG_PRINT(paramLen);
  DBUG_PRINT(terminate);
  IFR_Bool memory_ok=true;
  //>>> SQL TRACE
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE << endl << "::BIND COLUMN " << currenttime
                  << " [0x" << (void*)this << "]" << endl;
  }
  //<<< SQL TRACE
  IFR_Parameter parameter=IFR_Parameter (paramType,
                                         terminate,
                                         paramAddr,
                                         paramLengthIndicator,
                                         paramLen,
                                         paramPosIndicator);
  //>>> SQL TRACE
  if(IFR_SQL_TRACE_ENABLED) {
      IFR_TRACE_STREAM << IFR_SQLTRACE_PARAMETER_HEADER << endl;
      parameter.sqlTraceParameter(IFR_TRACE_STREAM, paramIndex);
      IFR_TRACE_STREAM << endl;
  }
  //<<< SQL TRACE
  IFR_Int4 knlver;
  if ((knlver = this->m_Connection->getKernelVersion ()) < 70404) {
    this->error().setRuntimeError(IFR_ERR_WRONG_KERNEL_VERSION_I, knlver);
    DBUG_RETURN(IFR_NOT_OK);
  }

  if ((paramIndex == 0) || (paramIndex > (unsigned) this->m_FetchInfo->getColumnCount ())) {
    error ().setRuntimeError (IFR_ERR_INVALID_PARAMETERINDEX);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if(paramAddr == 0 && paramLengthIndicator == 0) {
    error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)paramIndex);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if(paramLen < 0) {
      error().setRuntimeError(IFR_ERR_NEGATIVE_BUFFERLEN_II, (IFR_Int4)paramIndex, (IFR_Int4)paramLen);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if(paramPosIndicator == 0) {
    error().setRuntimeError(IFR_ERR_NULL_PARAMETERADDR_I, (IFR_Int4)paramIndex);
    DBUG_RETURN(IFR_NOT_OK);
  }
  if (m_ParamVector->GetSize () < paramIndex) {
    m_ParamVector->Resize (paramIndex, memory_ok);
  }
  if(!memory_ok) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }
  (*m_ParamVector)[paramIndex-1] = parameter;
  m_hasposread = true;
  DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::clearColumns()
{
    DBUG_METHOD_ENTER(IFR_ResultSet, clearColumns);
    error().clear();
    IFR_Retcode rc = IFR_OK;
    if ((rc = assertNotClosed ()) != IFR_OK) {
      DBUG_RETURN(rc);
    }
    m_ParamVector->Clear();
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::setBindingType(IFR_size_t size)
{
    IFR_TRACE_UPDATE;
    DBUG_METHOD_ENTER(IFR_ResultSet, setBindingType);
    DBUG_PRINT(size);
    //>>> SQL TRACE
    IFR_SQL_TRACE << endl << "::SET BINDING TYPE "
                  << this->m_FetchInfo->m_CursorName
                  << " [0x" << (void*)this << "]" << endl
                  << "BINDING TYPE: " <<  size << endl;
    //<<< SQL TRACE
    m_binding_type = size;
    return IFR_OK;
}


IFR_Retcode
IFR_ResultSet::setRowSetSize (IFR_UInt4 rowsetsize)
{
  IFR_TRACE_UPDATE;
  DBUG_METHOD_ENTER(IFR_ResultSet, setRowSetSize);
  DBUG_PRINT(rowsetsize);



  if (rowsetsize == 0) {
    error().setRuntimeError (IFR_ERR_INVALID_ROWSETSIZE);
    DBUG_RETURN (IFR_NOT_OK);
  }

  error().clear();
  IFR_Retcode rc = IFR_OK;
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }

  //>>> SQL TRACE
  IFR_SQL_TRACE << endl << "::SET ROWSET SIZE "
                << this->m_FetchInfo->m_CursorName
                << " [0x" << (void*)this << "]" << endl;
  IFR_SQL_TRACE << "SIZE: " << rowsetsize << endl;
  //<<< SQL TRACE

  IFR_Bool memory_ok = true;
  IFR_Length oldsz = m_rowstatusarray.GetSize();
  m_rowstatusarray.Resize(rowsetsize, memory_ok);
  //<<< MEMCHECK
  if(!memory_ok) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }
  for(IFR_Length l=oldsz; l<(IFR_Length)rowsetsize; ++l) {
      m_rowstatusarray[l] = IFR_NOT_OK;
  }
  //>>> MEMCHECK
  m_rowsetsize = rowsetsize;

  DBUG_RETURN(IFR_OK);
}

IFR_UInt
IFR_ResultSet::getRowSetSize ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getRowSetSize);
  //>>> SQL TRACE
  IFR_SQL_TRACE << endl << "::GET ROWSET SIZE "
                << "[0x" << (void*)this << "]" << endl;
  IFR_SQL_TRACE << "SIZE: " << m_rowsetsize << endl;
  //<<< SQL TRACE
  error().clear();
  if (assertNotClosed () != IFR_OK) {
    DBUG_RETURN(0);
  }
  DBUG_RETURN(m_rowsetsize);
}

const IFR_Statement* IFR_ResultSet::getStatement () const
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getStatement);
  DBUG_RETURN(m_Statement);
}

const IFR_Bool IFR_ResultSet::isUpdatable () const
{
  return m_concurType == IFR_Statement::UPDATABLE;
}


const IFRUtil_Vector<IFR_Parameter>* IFR_ResultSet::getParamVector () const
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getParamVector);
  return m_ParamVector;
  //  DBUG_RETURN((IFRUtil_Vector<IFR_Parameter>*) m_ParamVector);
}

const IFR_Int4*
IFR_ResultSet::getRowStatus() const
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getRowStatus);
  ((IFR_ErrorHndl&)error()).clear();
  DBUG_RETURN(m_rowstatusarray.Data());
}

IFR_UInt
IFR_ResultSet::getFetchedRows ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getFetchedRows);
  //>>> SQL TRACE
  IFR_SQL_TRACE << endl << "::GET ROWS AFFECTED "
                << "[0x" << (void*)this << "]" << endl;
  IFR_SQL_TRACE << "ROWS: " << m_FetchedRows << endl;
  //<<< SQL TRACE
  error().clear();
  DBUG_RETURN(m_FetchedRows);
}

IFR_Int4
IFR_ResultSet::getResultCount()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getResultCount);
  //>>> SQL TRACE
  IFR_SQL_TRACE << endl << "::GET RESULT COUNT "
                << "[0x" << (void*)this << "]" << endl;
  IFR_SQL_TRACE << "COUNT: " << m_RowsInResultSet << endl;
  //<<< SQL TRACE
  error().clear();
  if (assertNotClosed () != IFR_OK) {
    DBUG_RETURN(0);
  }
  DBUG_RETURN(m_RowsInResultSet);
}

IFR_RowSet*
IFR_ResultSet::getRowSet ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getRowSet);
  error().clear();
  IFR_Retcode rc = IFR_OK;
  if ((rc = assertNotClosed ()) != IFR_OK) {
      DBUG_RETURN((IFR_RowSet*)0);
  }
  IFR_Bool memory_ok=true;
  createRowSet(memory_ok);  // depending on m_concurType
  if(!memory_ok) {
      error().setMemoryAllocationFailed();
      return 0;
  }
  if(m_rowset) {
    DBUG_RETURN(m_rowset);
  } else {
    DBUG_RETURN(((IFR_RowSet*) m_upd_rowset));
  }
}

void IFR_ResultSet::createRowSet(IFR_Bool& memory_ok)
{
    if (m_rowset != NULL  ||  m_upd_rowset != NULL)
        return;
    if(!memory_ok) {
        return;
    }
    if (m_concurType == IFR_Statement::READ_ONLY) {
        if (m_upd_rowset != NULL) {
            assertWrongConcurrency ();
            return;
        }
        m_rowset = new IFR_ALLOCATOR(allocator) IFR_RowSet (this, memory_ok);
        if(!memory_ok || m_rowset==0) {
            IFRUtil_Delete(m_rowset, allocator);
            m_rowset = 0;
            memory_ok = false;
            return;
        }
    } else {
        if (m_rowset != NULL) {
            assertWrongConcurrency ();
            return;
        }
        m_upd_rowset = new IFR_ALLOCATOR(allocator) IFR_UpdatableRowSet (this, memory_ok);
        if(!memory_ok || m_upd_rowset==0) {
            IFRUtil_Delete(m_upd_rowset, allocator);
            m_upd_rowset = 0;
            memory_ok = false;
            return;
        }
    }
}

IFR_UpdatableRowSet*
IFR_ResultSet::getUpdatableRowSet ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getUpdatableRowSet);

  error().clear();
  if (assertNotClosed () != IFR_OK) {
      DBUG_RETURN((IFR_UpdatableRowSet *)0);
  }
  IFR_Bool memory_ok=true;
  createRowSet(memory_ok);  // depending on m_concurType
  if(!memory_ok) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN((IFR_UpdatableRowSet*)0);
  }
  if(!m_upd_rowset) {
      m_upd_rowset = new IFR_ALLOCATOR(allocator) IFR_UpdatableRowSet (this, memory_ok);
      if(m_upd_rowset==0 || !memory_ok) {
          IFRUtil_Delete(m_upd_rowset, allocator);
          error().setMemoryAllocationFailed();
          m_upd_rowset = 0;
          DBUG_RETURN((IFR_UpdatableRowSet*)0);
      }
  }
  DBUG_RETURN(m_upd_rowset);
}

//----------------------------------------------------------------------
IFR_Connection*
IFR_ResultSet::getConnection() const
{
    // no traceing - infinite recursion
    return m_Connection;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::getvalInitPacket(IFRPacket_RequestPacket& requestpacket,
                                IFRPacket_RequestSegment& segment,
                                IFRPacket_LongDataPart &datapart)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, getvalInitPacket);

    segment = IFRPacket_RequestSegment(requestpacket,
                                       IFRPacket_CommandMessageType::Getval_C);

    IFR_Retcode rc = segment.addPart(datapart);

    if (rc != IFR_OK) {
        error().setRuntimeError(IFR_ERR_PACKET_EXHAUSTED);
        DBUG_RETURN(rc);
    }

    DBUG_RETURN(IFR_OK);
}



//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::fillRowsWithData (IFR_Int4 startrow,
                                 IFR_Int4 count)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, fillRowsWithData);
    DBUG_PRINT(startrow);
    DBUG_PRINT(count);

    if ((startrow < 1) || (count < 1)) {
        error ().setRuntimeError (IFR_ERR_INVALID_INDEX);
        DBUG_RETURN(IFR_NOT_OK);
    }

    IFR_Retcode        rc;
    IFRPacket_DataPart part;

    if((rc = getCurrentData(part)) != IFR_OK) {
        DBUG_RETURN(rc);
    }

    IFR_Bool tableHeaderPrinted=false;
    IFR_Int4 chunk_offset = m_CurrentChunk->getCurrentOffset();
    IFR_Int4 record_size  = part.getRecordSize();
    IFR_Int4 columns      = m_ParamVector->GetSize();
    IFR_Bool memory_ok    = true;
    IFR_Length  offset    = 0; // start offset. this is always 0 for fetch.
    IFRUtil_Vector<IFRConversion_GetvalInfo> getvalinfo(allocator);

    for(IFR_Int4 row=startrow; row<startrow+count; ++row) {
        // <<< SQL TRACE
        if(IFR_SQL_TRACE_ENABLED) {
            if (!tableHeaderPrinted) {
                tableHeaderPrinted = true;
                IFR_TRACE_STREAM << "DATA:" << endl
                                 << "APPLICATION" << endl
                                 << IFR_SQLTRACE_DATA_HEADER << endl;
            }
            IFR_TRACE_STREAM << "ROW: " << (row) << endl;
        }
        //>>> SQL TRACE
        part.setCurrentRowStart(record_size * (chunk_offset + row - startrow));

        IFR_Retcode rc_row = IFR_OK;

        for(IFR_Int4 colidx = 0; colidx < columns; ++colidx) {
            IFRConversion_Converter *info;
            if ((info = this->findColumnInfo (colidx + 1)) == 0) {
                error ().setRuntimeError (IFR_ERR_INVALID_INDEX);
                DBUG_RETURN(IFR_NOT_OK);
            }

            if(!info->isLong()) {

                const IFR_Length *posindicator = (*m_ParamVector)[colidx].getPosIndicator();
                if(posindicator && *posindicator!=1) {
                    error().setRuntimeError(IFR_ERR_NOT_IMPLEMENTED("Binding of data types other than LONG with offset"));
                    DBUG_RETURN(IFR_NOT_OK);
                }

                rc = info->translateOutput(part,
                                           (*m_ParamVector)[colidx],
                                           *this,
                                           row - 1,
                                           m_binding_type);
            } else {
                IFR_Parameter* pcurrentParameter = &( (*m_ParamVector)[colidx] );

                // Create a getval. It will return 0 and set the error if something
                // weird happens, but it will also return 0 for null data and IFR_IGNORE
                // data.
                IFRConversion_Getval *gv = info->createGetval (part,
                                                               pcurrentParameter,
                                                               *this,
                                                               m_rowsetstartrow + row - 1,
                                                               row -1, 
                                                               m_binding_type);

                // gv == 0 may be NULL data or memory allocation error
                if(gv == 0) {
                    if(error()) {
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                } else { // if we got a getval instance, we need to keep it.
                    m_rowset->addOutputLong(gv, memory_ok);
                    if(!memory_ok) {
                        IFRUtil_Delete(gv, allocator);
                        error().setMemoryAllocationFailed();
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    IFR_Length *posindicator = (IFR_Length *) pcurrentParameter->getPosIndicator();
                    if(posindicator && *posindicator == 0) {
                        error().setRuntimeError(IFR_ERR_INVALID_STARTPOSITION_I, (IFR_Int4)0);
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                    IFR_Length  dataoffset      = 0;
                    IFR_Length  datalength      = pcurrentParameter->getBytesLength();
                    IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator(row - 1);
                    // Initialize with 0, so that we later can decide whether it must be saved.
                    if(lengthindicator) {
                        *lengthindicator = 0;
                    }

                    if(posindicator) {
                        gv->setReadOffset(*posindicator);
                    }

                    rc = info->getData (part,
                                        (void *) pcurrentParameter->data(row - 1),
                                        lengthindicator,
                                        *pcurrentParameter,
                                        dataoffset,
                                        (offset=0),
                                        *(getRowSet()),
                                        datalength,
                                        gv);
                    if (rc == IFR_NEED_DATA) {
                        IFRConversion_GetvalInfo gvinfo(info, gv, dataoffset, datalength);
                        getvalinfo.InsertEnd(gvinfo, memory_ok);
                        if(!memory_ok) {
                            error().setMemoryAllocationFailed();
                            DBUG_RETURN(IFR_NOT_OK);
                        }
                        rc = IFR_OK;
                    } else if(rc == IFR_OK || rc==IFR_DATA_TRUNC) {
                        gv->setReadOffset(1);
                    }
                }
            }

            //>>> SQL TRACE
            if(IFR_SQL_TRACE_ENABLED) {
                (*m_ParamVector)[colidx].sqlTraceParameterData(IFR_TRACE_STREAM,
                                                               colidx + 1,
                                                               rc,
                                                               row - 1,
                                                               0,
                                                               0,
                                                               0,
                                                               false);
                IFR_TRACE_STREAM << endl;
            }
            //<<< SQL TRACE


            if ((rc_row != IFR_NOT_OK) && (rc_row != IFR_DATA_TRUNC)) {
                rc_row = rc;
            }
        }
        m_rowstatusarray[row-1] = rc_row;
    }

    IFR_Length getvalinfo_size = getvalinfo.GetSize();
    if(getvalinfo_size != 0) {
        IFRPacket_RequestPacket requestpacket(*this);
        IFRPacket_ReplyPacket replypacket;
        IFRPacket_RequestSegment segment;
        IFRPacket_LongDataPart longdatapart;

        // The getvalinfo size decreases while reading the longs.
        while(getvalinfo_size != 0) {
            IFR_Retcode rc = getConnection()->getRequestPacket(requestpacket, error());
            if(rc != IFR_OK) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            getvalInitPacket(requestpacket, segment, longdatapart);
            // Put all remaining longs in.
            for(IFR_Int4 j=0; j<getvalinfo_size; ++j) {
                getvalinfo[j].getval->putDescriptor(longdatapart);
            }
            segment.closePart();
            segment.close();

            IFR_Retcode sqlaexecute_rc = getConnection()->sqlaexecute(requestpacket,
                                                                      replypacket,
                                                                      IFR_Connection::AppendNotAllowed_C,
                                                                      error());

            if (sqlaexecute_rc != IFR_OK) {
                DBUG_RETURN(sqlaexecute_rc);
            } else if(error()) {
                DBUG_RETURN(IFR_NOT_OK);
            }

            IFRPacket_ReplySegment replysegment(replypacket);
            IFR_Int4 updated_longcount;
            m_rowset->updateOutputLongs(replysegment, *this, updated_longcount, this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_LONGDATAREADSIZE_8_STMT]);

            if ((rc = replysegment.getPart (longdatapart)) != IFR_OK) {
                error().setRuntimeError(IFR_ERR_MISSINGLONGDATAPART);
                DBUG_RETURN(rc);
            }

            for(IFR_Int4 i = 0; i < getvalinfo_size; ++i) {
                IFRConversion_GetvalInfo* pcurrentinfo = &(getvalinfo[i]);
                IFR_Int4   row = pcurrentinfo->getval->getRow() - m_rowsetstartrow + 1;
                IFR_Parameter* pcurrentParameter =  &( (*m_ParamVector)[pcurrentinfo->getval->getColumn() - 1]);
                IFR_Length old_offset = pcurrentinfo->offset;
                IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator(row - 1);
                rc = pcurrentinfo->converter->getData(longdatapart,
                                                      (void *) pcurrentParameter->data(row - 1),
                                                      lengthindicator,
                                                      *pcurrentParameter,
                                                      pcurrentinfo->offset,
                                                      (offset = 0), // for fetch operation the offset is not necessary.
                                                      *(getRowSet()),
                                                      pcurrentinfo->datalength,
                                                      pcurrentinfo->getval);
                // Check whether anything has been written, and reduce the 'updated long count' by one.
                if(pcurrentinfo->offset != old_offset) {
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
                        IFR_Length *lengthindicator = pcurrentParameter->getLengthIndicator(row - 1);
                        if(lengthindicator && *lengthindicator == IFR_NO_TOTAL) {
                            *lengthindicator = pcurrentinfo->offset;
                        }
                    }
                    if(m_rowstatusarray[row - 1] == IFR_OK) {
                        m_rowstatusarray[row - 1] = rc;
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

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::fetch ()
{
  IFR_TRACE_UPDATE;
  DBUG_METHOD_ENTER(IFR_ResultSet, fetch);
  IFR_Retcode rc = IFR_OK;

  // If there is a row set, all LONG/LOB objects are
  // invalidated by the next fetch operation.
  if (m_rowset) {
      m_rowset->closeOutputLongs();
  }

  if (m_PositionState != IFR_POSITION_INSIDE) {
    if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
      error ().setRuntimeError (IFR_ERR_RESULTSET_BEFOREFIRST);
    } else {
      error ().setRuntimeError (IFR_ERR_RESULTSET_AFTERLAST);
    }
    DBUG_RETURN(IFR_NOT_OK);
  }
  
  if(m_type == IFR_Statement::FORWARD_ONLY) {
      if(!(m_CurrentChunk && m_CurrentChunk->setRow(m_rowsetstartrow))) {
          error ().setRuntimeError (IFR_ERR_RESULTSET_IS_FORWARD_ONLY);
          DBUG_RETURN(IFR_NOT_OK);
      }
  } else {
      if(!(m_CurrentChunk && m_CurrentChunk->setRow(m_rowsetstartrow))) {
          if ((rc = this->mfAbsolute (this->m_rowsetstartrow)) == IFR_NOT_OK) {
              DBUG_RETURN(rc);
          }
      }
  }

  IFR_Int4 fetched_rows =MIN(m_rowsetsize,
                             this->m_CurrentChunk->size() - this->m_CurrentChunk->getCurrentOffset());
  rc = fillRowsWithData (1, fetched_rows);
  if (rc == IFR_NOT_OK) {
      DBUG_RETURN(rc);
  }


  if((m_FetchedRows = fetched_rows) == 0) {
      error ().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
      DBUG_RETURN(IFR_NO_DATA_FOUND);
  }
  // Continue to fetch while the rowset is not filled.
  while ((m_FetchedRows < m_rowsetsize) && !m_CurrentChunk->isLast()) {
      // If we know because of the size that the end is reached, stop here.
      if ((m_rowsetstartrow > 0) && rowsInResultSetKnown()) {
          if (m_rowsetstartrow + m_FetchedRows > (unsigned) m_RowsInResultSet) {
              break;
          }
      }
      // If we come backward, and reach position 0, this is the end too
      if ((m_rowsetstartrow < 0) && (m_rowsetstartrow+m_FetchedRows >= 0))
          break;

      // Execute the next fetch command ...
      if(m_type != IFR_Statement::FORWARD_ONLY) {
          rc = mfAbsolute (m_rowsetstartrow+m_FetchedRows);
      } else {
          rc = mfNext();
      }
      if ((rc != IFR_NO_DATA_FOUND) && (rc != IFR_NOT_OK)) {
          // ... and if data is found, fill the output data.
          fetched_rows =MIN(m_rowsetsize - m_FetchedRows,
                            m_CurrentChunk->size() - m_CurrentChunk->getCurrentOffset());
          rc = fillRowsWithData (m_FetchedRows+1, fetched_rows);
          if (rc == IFR_NOT_OK) {
              DBUG_RETURN(rc);
          }
          m_FetchedRows += fetched_rows;
      } else {
          // ... otherwise ignore the case when the end is reached ...
          if (rc == IFR_NO_DATA_FOUND) {
              error ().clear ();
              rc = IFR_OK;
          }
          break;
      }
  }
  return rc;
}

IFR_Retcode
IFR_ResultSet::absolute (int row)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, absolute);
  DBUG_PRINT(this);
  DBUG_PRINT(row);

  error().clear();
  IFR_Retcode rc = IFR_OK;
  if (m_rowset)
    m_rowset->clearOutputLongs ();
  m_rowset->m_offset = 1;
  rc = mfAbsolute (row);
  if (rc == IFR_OK) {
    m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
  }
  DBUG_PRINT(this);
  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::mfAbsolute (IFR_Int4 row)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, mfAbsolute);
    DBUG_PRINT(row);
    IFR_Retcode rc = IFR_OK;
    
    error ().clear ();
    
    clearWarnings ();
    if ((rc = assertNotClosed ()) != IFR_OK) {
        DBUG_RETURN(rc);
    }
    
    if ((rc = assertNotForwardOnly ()) != IFR_OK) {
        DBUG_RETURN(rc);
    }
    
    if (row == 0) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    
    if (this->m_Empty) {
        if (row > 0) {
            this->m_PositionState = IFR_POSITION_AFTER_LAST;
            DBUG_RETURN(IFR_NO_DATA_FOUND);
        } else {
            this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
            DBUG_RETURN(IFR_NO_DATA_FOUND);
        }
    }
    
    if (row > 0) {
        if (maxRowIsSet () && (row > m_MaxRows)) {
            this->m_PositionState = IFR_POSITION_AFTER_LAST;
            error ().setRuntimeError (IFR_ERR_RESULTSET_AFTERLAST);
            DBUG_RETURN(IFR_NOT_OK);
        }
        
        if (rowsInResultSetKnown () && (row > m_RowsInResultSet)) {
            this->m_PositionState = IFR_POSITION_AFTER_LAST;
            DBUG_RETURN(IFR_NO_DATA_FOUND);
        }
        if (m_PositionStateOfChunk != IFR_POSITION_INSIDE) {
            bool result;
            rc = fetchAbsoluteUp (row, result);
            if (!result) {
                this->m_PositionState = IFR_POSITION_AFTER_LAST;
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(rc);
        } else {
            if (m_CurrentChunk->setRow (row)) {
                this->m_PositionState = IFR_POSITION_INSIDE;
                DBUG_RETURN(IFR_OK);
            } else {
                bool result;
                rc = fetchAbsoluteUp (row, result);
                if (!result) {
                    this->m_PositionState = IFR_POSITION_AFTER_LAST;
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                DBUG_RETURN(rc);
            }
        }
    } else {
        if (rowsInResultSetKnown ()) {
            int invertedPos = invertPosition (row);
            if (invertedPos <= 0) {
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                DBUG_RETURN(IFR_OK);
            } else {
                DBUG_RETURN(mfAbsolute (invertedPos));
            }
        }
        
        if (maxRowIsSet () && (-row > m_MaxRows)) {
            this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
            DBUG_RETURN(IFR_OK);
        }
        
        if (m_PositionStateOfChunk != IFR_POSITION_INSIDE) {
            bool result;
            if ((rc = fetchAbsoluteDown (row, result)) != IFR_OK) {
                DBUG_RETURN(rc);
            }
            if (!result) {
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                DBUG_RETURN(IFR_NO_DATA_FOUND);
            }
            DBUG_RETURN(IFR_OK);
        } else {
            if (m_CurrentChunk->setRow (row)) {
                this->m_PositionState = IFR_POSITION_INSIDE;
                DBUG_RETURN(IFR_OK);
            } else {
                bool result;
                if ((rc = fetchAbsoluteDown (row, result)) != IFR_OK) {
                    DBUG_RETURN(rc);
                }
                if (!result) {
                    this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                DBUG_RETURN(IFR_OK);
            }
    }
  }
}

IFR_Retcode
IFR_ResultSet::next()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, next);
  DBUG_PRINT(this);

  // DebugBreak();

  //>>> SQL TRACE ENTRY
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE << endl << "::FETCH NEXT ";
    if (this->m_FetchInfo)
      IFR_SQL_TRACE << this->m_FetchInfo->m_CursorName << " ";
    IFR_SQL_TRACE << currenttime << endl;
  }
  //<<< SQL TRACE ENTRY
  error().clear();
  IFR_Retcode rc = IFR_OK;

  // In case of forward-only result sets, I cannot come back and fetch contents from
  // the row.
  
  if (m_rowset) {
      if(m_PositionState != IFR_POSITION_BEFORE_FIRST) {
          if(m_type == IFR_Statement::FORWARD_ONLY) {
              m_rowset->closeOutputLongs ();
          } else {
              m_rowset->clearOutputLongs ();
          }
      }
  }

  m_rowset->m_offset = 1;
  if (m_rowsetsize > 1) {
      if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
          if(m_type == IFR_Statement::FORWARD_ONLY) {
              rc = mfNext();
          } else {
              rc = mfFirst();
          }
          m_rowsetstartrow = m_CurrentChunk->getLogicalPos();
      } else if (m_PositionState == IFR_POSITION_AFTER_LAST) {
          rc = afterLast();
          if (rc == IFR_OK) {
              rc = IFR_NO_DATA_FOUND;
              error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
              error().clear();
          }
      } else {
          if(m_type == IFR_Statement::FORWARD_ONLY) {
              if(!m_CurrentChunk->setRow(m_rowsetstartrow + m_rowsetsize)) {
                  if(m_CurrentChunk->getEnd() == m_rowsetstartrow + m_rowsetsize - 1) {
                      rc = mfNext();
                      if(rc == IFR_OK) {
                          m_rowsetstartrow = m_CurrentChunk->getLogicalPos();
                      }
                  } else {
                      if(m_CurrentChunk->isLast()) {
                          m_PositionState = IFR_POSITION_AFTER_LAST;
                          rc = IFR_NO_DATA_FOUND;
                          error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                          error().clear();
                      } else {
                          assertNotForwardOnly();
                          rc = IFR_NOT_OK;
                      }
                  }
              } else {
                  m_rowsetstartrow = m_CurrentChunk->getLogicalPos();
              }
          } else {
              if ((rc = mfAbsolute (m_rowsetstartrow+m_rowsetsize)) == IFR_OK) {
                  m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
              }
          }
      }
  } else {
      if ((rc = mfNext()) == IFR_OK) {
          m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
      }
  }
  DBUG_PRINT(this);
  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::mfNext ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, mfNext);
  IFR_Retcode rc = IFR_OK;

  clearWarnings ();
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  // if we have nothing, there is nothing to do.
  if (this->m_Empty) {
    this->m_PositionState = IFR_POSITION_AFTER_LAST;
    return IFR_NO_DATA_FOUND;
  }

  bool result = false;

  // if we are outside, ...
  if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
    // ... check whether we still have it
    if (m_PositionStateOfChunk == IFR_POSITION_INSIDE &&
        m_CurrentChunk->containsRow (1)) {
        m_CurrentChunk->setRow (1);
        this->m_PositionState = IFR_POSITION_INSIDE;
        rc = IFR_OK;
    } else {
        rc = fetchFirst ();
    }
  } else if (m_PositionState == IFR_POSITION_INSIDE) {
      if (m_CurrentChunk->move (m_rowsetsize)) {
          rc = IFR_OK;
      } else {
          if (m_CurrentChunk->isLast ()) {
              this->m_PositionState = IFR_POSITION_AFTER_LAST;
              return IFR_NO_DATA_FOUND;
          }
          rc = fetchNextChunk();
      }
  } else if (m_PositionState == IFR_POSITION_AFTER_LAST) {
      rc = IFR_NO_DATA_FOUND;
  }

  // in case we did a repositioning, we have to clear
  // the warnings
  if (rc) {
    clearWarnings();
  }

  return rc;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::relative (int relativePos)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, relative);
    DBUG_PRINT(this);
    DBUG_PRINT(relativePos);
    
    //>>> SQL TRACE ENTRY
    if (IFR_SQL_TRACE_ENABLED) {
        IFR_SQL_TRACE   << endl << "::FETCH RELATIVE ";
        if (this->m_FetchInfo)
            IFR_SQL_TRACE << this->m_FetchInfo->m_CursorName << " ";
        IFR_SQL_TRACE   << currenttime << endl;
    }
    //<<< SQL TRACE ENTRY
    
    error().clear();
    IFR_Retcode rc = IFR_OK;
    if ((rc = assertNotClosed ()) != IFR_OK) {
        DBUG_RETURN(rc);
    }
    if (m_rowset)
        m_rowset->clearOutputLongs ();

    m_rowset->m_offset = 1;
    
    if(m_type == IFR_Statement::FORWARD_ONLY) {
        if(relativePos < 0) {
            error ().setRuntimeError (IFR_ERR_RESULTSET_IS_FORWARD_ONLY);
            DBUG_RETURN(IFR_NOT_OK);
        }
        if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
            rc = mfNext();
        } else if (m_PositionState == IFR_POSITION_AFTER_LAST) {
            rc = afterLast();
            if (rc == IFR_OK) {
                rc = IFR_NO_DATA_FOUND;
                error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                error().clear();
            }
        } else {
            while(!m_CurrentChunk->setRow(m_rowsetstartrow + relativePos)) {
                if(m_CurrentChunk->isLast()) {
                    m_PositionState = IFR_POSITION_AFTER_LAST;
                    rc = IFR_NO_DATA_FOUND;
                    error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                    error().clear();
                    DBUG_RETURN(rc);
                }
                rc = fetchNextChunk();
                if(rc != IFR_OK) {
                    DBUG_RETURN(rc);
                }
            } 
            m_rowsetstartrow = m_CurrentChunk->getLogicalPos();
        }
    } else {
        if (m_rowsetsize > 1) {
            if (((m_PositionState == IFR_POSITION_BEFORE_FIRST) && (relativePos > 0)) ||
                ((m_PositionState == IFR_POSITION_AFTER_LAST) && (relativePos < 0))) {
                rc = mfAbsolute (relativePos);
            } else {
                if ((m_PositionState == IFR_POSITION_BEFORE_FIRST) && (relativePos <= 0)) {
                    error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                    error().clear();
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                if ((m_PositionState == IFR_POSITION_AFTER_LAST) && (relativePos >= 0)) {
                    error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                    error().clear();
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                if ((m_rowsetstartrow == 1) && (relativePos < 0)) {
                    m_PositionState = IFR_POSITION_BEFORE_FIRST;
                    error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                    error().clear();
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                if ((m_rowsetstartrow > 1) && (relativePos < 0) && (relativePos-m_rowsetstartrow > 0) && 
                    (abs(relativePos) > m_rowsetsize)) {
                    m_PositionState = IFR_POSITION_BEFORE_FIRST;
                    error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
                    error().clear();
                    DBUG_RETURN(IFR_NO_DATA_FOUND);
                }
                if ((rc = mfAbsolute (m_rowsetstartrow+relativePos)) == IFR_OK) {
                    m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
                }
            }
        }
        else {
            if ((rc = mfRelative(relativePos)) == IFR_OK) {
                m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
            }
        }
    }
    DBUG_PRINT(this);
    DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::mfRelative (int relativePos)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, mfRelative);
  DBUG_PRINT(relativePos);
  IFR_Retcode rc = IFR_OK;

  clearWarnings ();
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if (assertNotForwardOnly () != IFR_OK) {
    DBUG_RETURN(IFR_NOT_OK);
  }

  if (this->m_Empty) {
    DBUG_RETURN(IFR_NO_DATA_FOUND);
  }

  if (m_PositionState != IFR_POSITION_INSIDE) {
    if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
      error ().setRuntimeError (IFR_ERR_RESULTSET_BEFOREFIRST);
    } else {
      error ().setRuntimeError (IFR_ERR_RESULTSET_AFTERLAST);
    }
    DBUG_RETURN(IFR_NOT_OK);
  } else {
    int internal = getInternalRow();
    DBUG_PRINT(internal);
    if (internal > 0) {
      if (internal + relativePos <= 0) {
	this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
	DBUG_RETURN(IFR_NO_DATA_FOUND);
      } else {
	DBUG_RETURN(mfAbsolute (internal + relativePos));
      }
    } else {
      if (internal + relativePos >= 0) {
	this->m_PositionState = IFR_POSITION_AFTER_LAST;
	DBUG_RETURN(IFR_NO_DATA_FOUND);
      } else {
	DBUG_RETURN(mfAbsolute (internal + relativePos));
      }
    }
  }
}

IFR_Retcode
IFR_ResultSet::previous ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, previous);
  DBUG_PRINT(this);

  //>>> SQL TRACE ENTRY
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE   << endl << "::FETCH PREVIOUS ";
    if (this->m_FetchInfo)
      IFR_SQL_TRACE << this->m_FetchInfo->m_CursorName << " ";
    IFR_SQL_TRACE   << currenttime << endl;
  }
  //<<< SQL TRACE ENTRY

  error().clear();
  IFR_Retcode rc = IFR_OK;
  if (m_rowset)
    m_rowset->clearOutputLongs ();
  m_rowset->m_offset = 1;
  if (m_rowsetsize > 1) {
    if (m_PositionState == IFR_POSITION_BEFORE_FIRST) {
      rc = beforeFirst ();
      if (rc == IFR_OK) {
	rc = IFR_NO_DATA_FOUND;
	error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
	error().clear();
      }
    } else if (m_rowsetstartrow == 1) {
      rc = beforeFirst ();
      if (rc == IFR_OK) {
	rc = IFR_NO_DATA_FOUND;
	error().setRuntimeError (IFR_ERR_ROW_NOT_FOUND);
	error().clear();
      }
    } else if ((m_rowsetstartrow > 1) && (m_rowsetstartrow <= (unsigned) m_rowsetsize)) {
      rc = mfFirst ();
    } else if ((rc = mfAbsolute (m_rowsetstartrow-m_rowsetsize)) == IFR_OK) {
      m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
    }
  } else {
    if ((rc = mfPrevious ()) == IFR_OK) {
      m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
    }
  }
  DBUG_PRINT(this);
  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::mfPrevious ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, mfPrevious);
  IFR_Retcode rc;

  clearWarnings ();
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if (m_PositionState == IFR_POSITION_AFTER_LAST) {
    DBUG_RETURN(mfAbsolute (-1));
  } else {
    DBUG_RETURN(mfRelative (-1));
  }
}

IFR_Retcode
IFR_ResultSet::first ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, first);
  DBUG_PRINT(this);

  //>>> SQL TRACE ENTRY
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE   << endl << "::FETCH FIRST ";
    if (this->m_FetchInfo)
      IFR_SQL_TRACE << this->m_FetchInfo->m_CursorName << " ";
    IFR_SQL_TRACE   << currenttime << endl;
  }
  //<<< SQL TRACE ENTRY

  error().clear();
  IFR_Retcode rc=IFR_OK;
  if (m_rowset)
    m_rowset->clearOutputLongs ();
  m_rowset->m_offset = 1;
  m_rowsetstartrow = 1;
  rc = mfFirst();
  DBUG_PRINT(this);
  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::mfFirst()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, mfFirst);
  IFR_Retcode rc = IFR_OK;

  clearWarnings ();
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if ((rc = assertNotForwardOnly ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }

  // if we have nothing, there is nothing to do.
  if (this->m_Empty) {
    this->m_PositionState = IFR_POSITION_AFTER_LAST;
    DBUG_RETURN(IFR_NO_DATA_FOUND);
  }

  if (m_PositionStateOfChunk == IFR_POSITION_INSIDE &&
      m_CurrentChunk->containsRow (1)) {
    m_CurrentChunk->setRow(1);
    this->m_PositionState = IFR_POSITION_INSIDE;
    rc = IFR_OK;
  } else {
    rc = fetchFirst ();
  }

  if (rc) {
    clearWarnings();
  }

  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::last ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, last);
  DBUG_PRINT(this);

  //>>> SQL TRACE ENTRY
  if (IFR_SQL_TRACE_ENABLED) {
    IFR_SQL_TRACE   << endl << "::FETCH LAST ";
    if (this->m_FetchInfo)
      IFR_SQL_TRACE << this->m_FetchInfo->m_CursorName << " ";
    IFR_SQL_TRACE   << currenttime << endl;
  }
  //<<< SQL TRACE ENTRY

  error().clear();
  IFR_Retcode rc = IFR_OK;
  if (m_rowset)
    m_rowset->clearOutputLongs ();
  m_rowset->m_offset = 1;
  if (m_rowsetsize > 1) {
    if ((rc = mfLast ()) == IFR_OK) {
      rc = mfRelative (-m_rowsetsize+1);
      m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
    }
  }
  else {
    if ((rc = mfLast ()) == IFR_OK) {
      m_rowsetstartrow = m_CurrentChunk->getLogicalPos ();
    }
  }
  DBUG_PRINT(this);
  DBUG_RETURN (rc);
}

IFR_Retcode
IFR_ResultSet::mfLast ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, mfLast);
  IFR_Retcode rc = IFR_OK;

  clearWarnings ();
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if ((rc = assertNotForwardOnly ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }

  // if we have nothing, there is nothing to do.
  if (this->m_Empty) {
    this->m_PositionState = IFR_POSITION_AFTER_LAST;
    DBUG_RETURN(IFR_NO_DATA_FOUND);
  }

  if (m_PositionStateOfChunk == IFR_POSITION_INSIDE &&
      m_CurrentChunk->setRow(-1)) {
    this->m_PositionState = IFR_POSITION_INSIDE;
    rc = IFR_OK;
  } else {
    rc = fetchLast ();
  }

  if (rc) {
    clearWarnings();
  }

  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::afterLast ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, afterLast);
  IFR_Retcode rc = IFR_OK;
  clearWarnings ();
  if ((rc = assertNotForwardOnly ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  this->m_PositionState = IFR_POSITION_AFTER_LAST;
  DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::beforeFirst ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, beforeFirst);
  IFR_Retcode rc = IFR_OK;
  clearWarnings ();
  if ((rc = assertNotForwardOnly ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN(rc);
  }
  this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
  DBUG_RETURN(rc);
}

void
IFR_ResultSet::close ()
{
    DBUG_METHOD_ENTER(IFR_ResultSet, close);
    error().clear();
    clearWarnings();
    assertNotClosed ();
    this->m_IsClosed = true;
    m_ParamVector->Clear();
    IFRUtil_Delete(m_CurrentChunk, allocator);
    IFRUtil_Delete(m_FetchInfo, allocator);
    this->m_CurrentChunk = 0;
    this->m_FetchInfo = 0;
}

IFR_Retcode
IFR_ResultSet::assertNotForwardOnly ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, assertNotForwardOnly);

  if (this->getType () == IFR_Statement::FORWARD_ONLY) {
    DBUG_PRINT("assertNotForwardOnly");
    error ().setRuntimeError (IFR_ERR_RESULTSET_IS_FORWARD_ONLY);
    return IFR_NOT_OK;
  }
  return IFR_OK;
}

void
IFR_ResultSet::assertWrongConcurrency ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, assertWrongConcurrency);
  DBUG_PRINT("assertWrongConcurrency");

  if (m_concurType == IFR_Statement::READ_ONLY) {
    error ().setRuntimeError (IFR_ERR_ROWSET_IS_UPDATABLE);
  } else {
    error ().setRuntimeError (IFR_ERR_ROWSET_IS_READ_ONLY);
  }
}


bool
IFR_ResultSet::maxRowIsSet ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, maxRowIsSet);
  return m_MaxRows != 0;
}

bool
IFR_ResultSet::rowsInResultSetKnown ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, rowsInResultSetKnown);
  return m_RowsInResultSet != -1;
}

IFR_Retcode
IFR_ResultSet::fetchAbsoluteUp (int physicalRow, bool& res)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, fetchAbsoluteUp);
    DBUG_PRINT(physicalRow);
    IFR_Retcode rc = IFR_OK;
    IFR_Bool memory_ok=true;

    int maxKnownEnd;

    if (maxRowIsSet ()) {
        if (rowsInResultSetKnown ()) {
            maxKnownEnd = MIN(this->m_MaxRows, this->m_RowsInResultSet);
        } else {
            maxKnownEnd = this->m_MaxRows;
        }
    } else {
        if (rowsInResultSetKnown ()) {
            maxKnownEnd = this->m_RowsInResultSet;
        } else {
            maxKnownEnd = INT_MAX;
        }
    }
    DBUG_PRINT(maxKnownEnd);

    int usedFetchSize = this->m_FetchSize;
    int usedPhysicalRow = physicalRow;

    DBUG_PRINT(usedFetchSize);
    if (physicalRow + usedFetchSize > maxKnownEnd && (usedFetchSize <= this->m_SafeFetchSize)) {
        usedPhysicalRow = maxKnownEnd - usedFetchSize + 1;
        DBUG_PRINT(usedPhysicalRow);
    }

    IFRPacket_ReplyPacket reply;

    rc = this->m_FetchInfo->executeFetchAbsolute(usedPhysicalRow, usedFetchSize, reply);
    if (this->m_FetchInfo->error ()) {
        if (this->m_FetchInfo->error ().getErrorCode () == 100) {
            this->m_PositionStateOfChunk = IFR_POSITION_NOT_AVAILABLE;
            this->m_PositionState = IFR_POSITION_AFTER_LAST;
        }
        this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
        res = false;
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }

    rc = setCurrentChunk (new IFR_ALLOCATOR(allocator) IFR_FetchChunk(IFR_FetchChunk::IFR_TYPE_ABSOLUTE_UP,
                                                                      usedPhysicalRow,
                                                                      reply,
                                                                      m_FetchInfo->getRecordSize(),
                                                                      m_MaxRows,
                                                                      this->m_RowsInResultSet,
                                                                      m_CurrentChunk,
                                                                      memory_ok,
                                                                      &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                      &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                      ));

    if(rc != IFR_OK ) {
        DBUG_RETURN(rc);
    } else if(!memory_ok) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }

    // if we moved the window, to avoid fetching behind the last,
    // we need to adjust the logical position.
    if (usedPhysicalRow != physicalRow) {
        if (!m_CurrentChunk->setRow (physicalRow)) {
            this->m_PositionState = IFR_POSITION_AFTER_LAST;
            res = false;
            DBUG_RETURN(IFR_NO_DATA_FOUND);
        }
    }
    res = true;
    DBUG_RETURN(rc);
}

IFR_Retcode
IFR_ResultSet::fetchAbsoluteDown (int physicalRow, bool& res)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, fetchAbsoluteDown);
    DBUG_PRINT(physicalRow);

    IFR_Retcode rc = IFR_OK;
    IFRPacket_ReplyPacket reply;
    IFR_Bool memory_ok = true;
    if (maxRowIsSet ()) {
        if (m_MaxRowsOutSideResult == -1) {
            if (m_RowsInResultSet == -1) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            int absrow = m_MaxRows + physicalRow + 1;
            if (absrow <= 0) {
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                res = false;
                DBUG_RETURN(IFR_OK);
            }
            return mfAbsolute (absrow);
        } else if (m_MaxRowsOutSideResult == 0) {
            rc = this->m_FetchInfo->executeFetchAbsolute (this->m_MaxRows, 1, reply);
            IFR_FetchChunk *chunk=new IFR_ALLOCATOR(allocator) IFR_FetchChunk(IFR_FetchChunk::IFR_TYPE_ABSOLUTE_UP,
                                                                              this->m_MaxRows,
                                                                              reply,
                                                                              m_FetchInfo->getRecordSize(),
                                                                              this->m_MaxRows,
                                                                              this->m_RowsInResultSet,
                                                                              m_CurrentChunk,
                                                                              memory_ok,
                                                                              &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                              &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                              );
            if(!memory_ok || chunk == 0) {
                error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
            if(setCurrentChunk(chunk) != IFR_OK) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            m_CurrentChunk->moveToUpperBound ();
            if (this->m_FetchInfo->error ()) {
                if (this->m_FetchInfo->error ().getErrorCode () != 100) {
                    this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
                    DBUG_RETURN(IFR_NOT_OK);
                }
                this->m_MaxRowsOutSideResult = -1;
                return mfAbsolute (physicalRow);
            }
        } else {
            rc = this->m_FetchInfo->executeFetchAbsolute (physicalRow, this->m_FetchSize, reply);
            if (this->m_FetchInfo->error ()) {
                if (this->m_FetchInfo->error ().getErrorCode () == 100) {
                    this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                    res = false;
                    DBUG_RETURN(IFR_OK);
                } else {
                    this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
                    DBUG_RETURN(IFR_NOT_OK);
                }
            }
            IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) IFR_FetchChunk(IFR_FetchChunk::IFR_TYPE_ABSOLUTE_DOWN,
                                                                                physicalRow,
                                                                                reply,
                                                                                m_FetchInfo->getRecordSize(),
                                                                                this->m_MaxRows,
                                                                                this->m_RowsInResultSet,
                                                                                m_CurrentChunk,
                                                                                memory_ok,
                                                                                &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                                &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                                );
            if(chunk==0 || !memory_ok) {
                error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
            if(setCurrentChunk(chunk)!=IFR_OK) {
                DBUG_RETURN(IFR_NOT_OK);
            }
            if (!m_CurrentChunk->setRow (physicalRow)) {
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                res = false;
                DBUG_RETURN(IFR_OK);
            }
        }
  } else {
        rc = this->m_FetchInfo->executeFetchAbsolute (physicalRow, this->m_FetchSize, reply);

        if (this->m_FetchInfo->error ()) {
            if (this->m_FetchInfo->error ().getErrorCode () == 100) {
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                res = false;
                DBUG_RETURN(IFR_OK);
            } else {
                this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        IFR_Bool memory_ok=true;
        IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) IFR_FetchChunk(IFR_FetchChunk::IFR_TYPE_ABSOLUTE_DOWN,
                                                                            physicalRow,
                                                                            reply,
                                                                            m_FetchInfo->getRecordSize(),
                                                                            this->m_MaxRows,
                                                                            this->m_RowsInResultSet,
                                                                            m_CurrentChunk,
                                                                            memory_ok,
                                                                            &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                            &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                            );
        if(chunk==0 || !memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(setCurrentChunk(chunk) != IFR_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }
        if (!m_CurrentChunk->setRow (physicalRow)) {
            this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
            res = false;
            DBUG_RETURN(IFR_OK);
        }
        res = true;
    }
    DBUG_RETURN(rc);
}

int
IFR_ResultSet::invertPosition (int row)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, invertPosition);
  return m_RowsInResultSet + row + 1;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_ResultSet::setCurrentChunk (IFR_FetchChunk* newChunk)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, setCurrentChunk);
    if(newChunk == 0) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    clearCurrentChunk();
    IFR_SQL_TRACE << endl;
    newChunk->init ();
    this->m_PositionState = this->m_PositionStateOfChunk = IFR_POSITION_INSIDE;
    this->m_CurrentChunk = newChunk;
    this->m_SafeFetchSize = MIN(this->m_FetchSize, MAX (newChunk->size(), this->m_SafeFetchSize));
    updateRowStatistics();
    if((m_CurrentChunk->isLast() && m_close_on_last) || 
       m_CurrentChunk->isResultSetClosed()) {
        m_Statement->setCursorClosed();
    }
    handleOpenedLongsInChunk();
    return IFR_OK;
}

//----------------------------------------------------------------------
void
IFR_ResultSet::handleOpenedLongsInChunk()
{
    DBUG_METHOD_ENTER(IFR_ResultSet, handleOpenedLongsInChunk);
    IFRPacket_DataPart datapart;
    m_CurrentChunk->getCurrentData(datapart);
    getRowSet()->checkOpenLongs(getColInfo(), 
                                m_CurrentChunk->getStart(), 
                                m_CurrentChunk->getEnd(), 
                                m_FetchInfo->getRecordSize(),
                                datapart);
}


void
IFR_ResultSet::clearCurrentChunk()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, clearCurrentChunk);
  IFRUtil_Delete(m_CurrentChunk, allocator);
  m_CurrentChunk=0;
}

void
IFR_ResultSet::updateRowStatistics ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, updateRowStatistics);
  if (!rowsInResultSetKnown ()) {
    if (m_CurrentChunk->isLast () && m_CurrentChunk->isFirst ()) {
      setRowsInResultSet (m_CurrentChunk->size ());
      m_CurrentChunk->setRowsInResultSet (m_RowsInResultSet);
    } else if (m_CurrentChunk->isLast () && m_CurrentChunk->isForward ()) {
      setRowsInResultSet (m_CurrentChunk->getEnd ());
      m_CurrentChunk->setRowsInResultSet (m_RowsInResultSet);
    } else if (m_CurrentChunk->isFirst () && !m_CurrentChunk->isForward ()) {
      setRowsInResultSet (-m_CurrentChunk->getStart ());
      m_CurrentChunk->setRowsInResultSet (m_RowsInResultSet);
    } else if (m_CurrentChunk->isForward ()) {
      m_LargestKnownAbsPos = MAX(m_LargestKnownAbsPos, m_CurrentChunk->getEnd ());
    }
  }
}

void
IFR_ResultSet::setRowsInResultSet (int rows)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, setRowsInResultSet);
  if (m_MaxRows > 0)
    this->m_RowsInResultSet = MIN(rows, m_MaxRows);
  else
    this->m_RowsInResultSet = rows;
}

IFR_Int2
IFR_ResultSet::getFetchSize ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getFetchSize);
  return m_FetchSize;
}

IFR_Retcode
IFR_ResultSet::setFetchSize (IFR_Int2 fetchSize)
{
  DBUG_METHOD_ENTER(IFR_ResultSet, setFetchSize);
  DBUG_PRINT(fetchSize);
  IFR_Retcode rc = IFR_OK;

  if ((rc = this->assertNotClosed ()) != IFR_OK)
    return rc;

  if (fetchSize > 0) {
    this->m_FetchSize = fetchSize;
    this->m_SafeFetchSize = MIN(this->m_SafeFetchSize, fetchSize);
    rc = IFR_OK;
  } else {
    rc = IFR_NOT_OK;
  }

  return rc;
}

IFRConversion_Converter*
IFR_ResultSet::findColumnInfo (int colIndex)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, findColumnInfo);
    DBUG_PRINT(colIndex);

    if ((colIndex < 1) || (colIndex > m_FetchInfo->getColumnCount())) {
        DBUG_RETURN((IFRConversion_Converter*)0);
    } else {
        DBUG_RETURN(m_FetchInfo->getColInfo()[colIndex - 1]);
    }
}


IFR_Retcode
IFR_ResultSet::getCurrentData(IFRPacket_DataPart& part)
{
    DBUG_METHOD_ENTER(IFR_ResultSet, getCurrentData);
    DBUG_PRINT(m_PositionState);
    if (this->m_PositionState == IFR_POSITION_BEFORE_FIRST) {
        error ().setRuntimeError (IFR_ERR_RESULTSET_BEFOREFIRST);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if (this->m_PositionState == IFR_POSITION_AFTER_LAST) {
        error ().setRuntimeError (IFR_ERR_RESULTSET_AFTERLAST);
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(this->m_CurrentChunk==0) {
        DBUG_RETURN(IFR_NO_DATA_FOUND);
    }
    DBUG_RETURN(this->m_CurrentChunk->getCurrentData (part));
}

IFR_Retcode
IFR_ResultSet::assertNotClosed ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, assertNotClosed);
  if (assertOpen () != IFR_OK) {
    DBUG_RETURN(IFR_NOT_OK);
  }
  if (this->m_IsClosed) {
    DBUG_PRINT("assertNotClosed");
    error ().setRuntimeError (IFR_ERR_RESULTSET_IS_CLOSED);
    DBUG_RETURN(IFR_NOT_OK);
  }
  DBUG_RETURN(IFR_OK);
}

bool
IFR_ResultSet::isBeforeFirst ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, isBeforeFirst);
  return (!this->m_Empty && this->m_PositionState == IFR_POSITION_BEFORE_FIRST);
}

bool
IFR_ResultSet::isAfterLast ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, isAfterLast);
  return (!this->m_Empty && this->m_PositionState == IFR_POSITION_AFTER_LAST);
}

bool
IFR_ResultSet::hasPositionedRead ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, hasPositionedRead);
  return m_hasposread;
}

int
IFR_ResultSet::getRow ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getRow);
  error().clear();
  if (assertNotClosed () != IFR_OK) {
    DBUG_RETURN(0);
  }
  if (this->m_PositionState != IFR_POSITION_INSIDE) {
    DBUG_RETURN(0);
  }
  int internalRow = getInternalRow ();
  if (internalRow < 0) {
    getRowsInResult ();
    mfAbsolute (internalRow);
    internalRow = getInternalRow ();
  }
  DBUG_RETURN(internalRow);
}

int
IFR_ResultSet::getInternalRow ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getInternalRow);
  if (m_CurrentChunk != 0) {
    DBUG_RETURN(m_CurrentChunk->getLogicalPos ());
  } else {
    DBUG_RETURN(0);
  }
}

IFR_Retcode
IFR_ResultSet::getRowsInResult ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getRowsInResult);
  int stepsize = 32;
  IFR_Int4  pos = m_LargestKnownAbsPos;
  IFR_Int4  smallestbadpos = MAX_IFR_INT4 - pos;

  IFR_Retcode rc;
  {
      IFRPacket_ReplyPacket reply;
      rc = this->m_FetchInfo->executeFetchAbsolute (m_LargestKnownAbsPos, 1, reply);
  }
  if (this->m_FetchInfo->error ()) {
      if (this->m_FetchInfo->error ().getErrorCode () == 100) {
          if (m_LargestKnownAbsPos == 1) {
              this->m_Empty = true;
              this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
              return IFR_OK;
          } else {
              m_LargestKnownAbsPos = 1;
              rc = getRowsInResult ();
              return rc;
          }
      } else {
          this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
          return IFR_NO_DATA_FOUND;
      }
  }



  while (true) {
    DBUG_PRINT(stepsize);
    if ((m_MaxRows != 0) && (pos + stepsize > m_MaxRows)) {
      stepsize = m_MaxRows - pos;
    }
    {
        IFRPacket_ReplyPacket reply;
        rc = this->m_FetchInfo->executeFetchAbsolute (pos + stepsize, 1, reply);
    }
    if (this->m_FetchInfo->error ()) {
        if (this->m_FetchInfo->error ().getErrorCode () == 100) {
            if (stepsize == 1) {
                setRowsInResultSet (pos);
                clearCurrentChunk();
                this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
                this->m_PositionStateOfChunk = IFR_POSITION_NOT_AVAILABLE;
                return IFR_OK;
            }

            smallestbadpos = pos + stepsize;

            stepsize /= 2;
            if (stepsize == 0)
                stepsize = 1;
            continue;
        } else {
            this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
            return IFR_NO_DATA_FOUND;
        }
    }

    pos = pos + stepsize;

    stepsize *= 2;
    if(stepsize >= (smallestbadpos - pos)) {
        stepsize = (smallestbadpos - pos) / 2;
        if(stepsize == 0) {
            stepsize = 1;
        }
    }

    if ((m_MaxRows != 0) && (pos == m_MaxRows)) {
      setRowsInResultSet (pos);
      clearCurrentChunk();
      this->m_PositionState = IFR_POSITION_BEFORE_FIRST;
      this->m_PositionStateOfChunk = IFR_POSITION_NOT_AVAILABLE;
      return IFR_OK;
    }
  }
}

IFR_Retcode
IFR_ResultSet::fetchNextChunk ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, fetchNextChunk);
  IFR_Retcode rc = IFR_OK;
  IFRPacket_ReplyPacket reply;

  int usedFetchSize = this->m_FetchSize;
  int usedOffset = 1;
  IFR_Bool memory_ok = true;

  if (m_CurrentChunk->isForward ()) {
    // in case we come to an end, we may have to limit the fetch size.
    if (maxRowIsSet ()) {
      usedFetchSize = MIN(this->m_MaxRows - m_CurrentChunk->getEnd () + 1, usedFetchSize);
    }
    if(m_type == IFR_Statement::FORWARD_ONLY) {
        rc = this->m_FetchInfo->executeFetchNext(usedFetchSize, reply);
    } else {
        rc = this->m_FetchInfo->executeFetchRelative (usedOffset, usedFetchSize, reply);
    }
  } else {
    // kernel sits on the opposite edge of the chunk, honor this
    usedOffset += m_CurrentChunk->size();
    rc = this->m_FetchInfo->executeFetchRelative (usedOffset, usedFetchSize, reply);
  }

  if (this->m_FetchInfo->error ()) {
    if(this->m_FetchInfo->error ().getErrorCode () == 100) {
      // fine, we are at the end.
      this->m_CurrentChunk->setLast (true);
      this->updateRowStatistics ();
      // but invalidate it, as it is
      // thrown away by the kernel
      clearCurrentChunk();
      this->m_PositionStateOfChunk = IFR_POSITION_NOT_AVAILABLE;
      this->m_PositionState = IFR_POSITION_AFTER_LAST;
      return IFR_NO_DATA_FOUND;
    }
    this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
    return IFR_NOT_OK;
  }
  IFR_FetchChunk *chunk= new IFR_ALLOCATOR(allocator) IFR_FetchChunk (IFR_FetchChunk::IFR_TYPE_RELATIVE_UP,
                                                                      this->m_CurrentChunk->getEnd () + 1,
                                                                      reply,
                                                                      this->m_FetchInfo->getRecordSize (),
                                                                      this->m_MaxRows,
                                                                      this->m_RowsInResultSet,
                                                                      m_CurrentChunk,
                                                                      memory_ok,
                                                                      &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                      &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                      );
  
  if(!memory_ok) {
      IFRUtil_Delete(chunk, allocator);
  }
  if(chunk==0 || !memory_ok) {
      error().setMemoryAllocationFailed();
      DBUG_RETURN(IFR_NOT_OK);
  }
  if(setCurrentChunk(chunk) != IFR_OK) {
      if(chunk) {
          IFRUtil_Delete(chunk, allocator);
      }
      DBUG_RETURN(IFR_NOT_OK);
  }
  DBUG_RETURN(IFR_OK);
}

IFR_Retcode
IFR_ResultSet::fetchFirst ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, fetchFirst);
  IFR_Retcode rc = IFR_OK;
  IFRPacket_ReplyPacket reply;

  int usedFetchSize = this->m_FetchSize;

  if (maxRowIsSet ()) {
    usedFetchSize = MIN(usedFetchSize, this->m_MaxRows);
  }
  if(m_type != IFR_Statement::FORWARD_ONLY) {
      rc = this->m_FetchInfo->executeFetchFirst (usedFetchSize, reply);
  } else {
      rc = this->m_FetchInfo->executeFetchNext (usedFetchSize, reply);
  }
  if (this->m_FetchInfo->error ()) {
      if(this->m_FetchInfo->error ().getErrorCode() == 100) {
          this->m_Empty = true;
          this->m_PositionState = IFR_POSITION_AFTER_LAST;
          this->m_CurrentChunk = 0;
      } else {
          this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
          return IFR_NOT_OK;
      }
      return IFR_NO_DATA_FOUND;
  }
  IFR_Bool memory_ok = true;
  IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) 
      IFR_FetchChunk (IFR_FetchChunk::IFR_TYPE_FIRST, // fetch first is forward
                      1,    // absolute start position
                      reply,          // reply packet
                      m_FetchInfo->getRecordSize (), // the size for data part navigation
                      m_MaxRows, // the current maxRows setting, for determining the last
                      // condition in that case
                      this->m_RowsInResultSet,
                      m_CurrentChunk,
                      memory_ok,
                      &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                      &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                      );
  if(chunk == 0  || !memory_ok) {
      error().setMemoryAllocationFailed();
      return IFR_NOT_OK;
  }
  if(setCurrentChunk(chunk) != IFR_OK) {
      return IFR_NOT_OK;
  }
  return rc;
}

IFR_Retcode
IFR_ResultSet::fetchLast ()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, fetchLast);
  IFR_Retcode rc = IFR_OK;
  IFRPacket_ReplyPacket reply;

  // in case a max row is set, fetching the last is totally different
  // thing
  if (!maxRowIsSet () || m_MaxRowsOutSideResult == 1) {
    // we have to use the safe fetch size here.
      rc = this->m_FetchInfo->executeFetchLast (m_SafeFetchSize, reply);
    if (this->m_FetchInfo->error ()) {
      if (this->m_FetchInfo->error ().getErrorCode() == 100) {
	this->m_Empty = true;
	this->m_PositionState = IFR_POSITION_AFTER_LAST;
	this->m_CurrentChunk = 0;
	return IFR_NO_DATA_FOUND;
      } else {
          this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
	return IFR_NOT_OK;
      }
    }
    // If we did know a safe fetch size for LAST, we may have landed
    // before the position we wanted to fetch. In this case the chunk
    // starts a few rows before the end
    IFRPacket_ReplySegment segment (reply);
    if (segment.getSQLError (error(), allocator)) {
      return IFR_NOT_OK;
    }
    IFRPacket_DataPart datapart;
    if ((rc = segment.getPart (datapart)) != IFR_OK) {
      return rc;
    }
    IFR_Bool memory_ok = true;
    IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) IFR_FetchChunk (IFR_FetchChunk::IFR_TYPE_LAST,
                                                                         -datapart.getPartArguments(),    // absolute start position
                                                                         reply,          // reply packet
                                                                         m_FetchInfo->getRecordSize(), // the size for data part navigation
                                                                         0, // already ensured that there is no maxRows
                                                                         this->m_RowsInResultSet,
                                                                         m_CurrentChunk,
                                                                         memory_ok,
                                                                         &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                         &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                        );
    if(!memory_ok || chunk == 0) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(setCurrentChunk(chunk) != IFR_OK) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    m_CurrentChunk->moveToUpperBound (); // what we really want is the last position inside
    // this chunk, as this is the position of the last record.
    return IFR_OK;
  } else {
    // we have the good and the messy way. We may know how many
    // records are inside, and thus make something senseful.
    if (rowsInResultSetKnown ()) {
      // determined to what position we have to fetch
      int usedFetchSize = this->m_SafeFetchSize;
      int usedPhysicalRow = this->m_RowsInResultSet - usedFetchSize + 1;

      rc = this->m_FetchInfo->executeFetchAbsolute (usedPhysicalRow, usedFetchSize, reply);
      if (this->m_FetchInfo->error ()) {
	if(this->m_FetchInfo->error ().getErrorCode () == 100) {
	  // something wicked happened. we had a valid
	  // value for the rows, and it was not good.
	  // erase the value for the rows, and retry.
	  this->m_RowsInResultSet = -1;
	  return fetchLast ();
	} else {
      this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
	  return IFR_NOT_OK;
	}
      }
      IFR_Bool memory_ok = true;
      IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) IFR_FetchChunk (IFR_FetchChunk::IFR_TYPE_ABSOLUTE_UP,
                                                                           usedPhysicalRow,
                                                                           reply,
                                                                           m_FetchInfo->getRecordSize(),
                                                                           this->m_MaxRows,
                                                                           this->m_RowsInResultSet,
                                                                           m_CurrentChunk, memory_ok,
                                                                           &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                           &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                          );

    if(!memory_ok || chunk == 0) {
        error().setMemoryAllocationFailed();
        DBUG_RETURN(IFR_NOT_OK);
    }
    if(setCurrentChunk(chunk) != IFR_OK) {
        DBUG_RETURN(IFR_NOT_OK);
    }
    m_CurrentChunk->moveToUpperBound (); // It does not matter whether we
      // did get a few less records, just position at the last.
      return IFR_OK;
    } else {
      // try a fetch at the position the user wants.
      // if this does not find anything, try the next one ...#
        rc = this->m_FetchInfo->executeFetchAbsolute (this->m_MaxRows, 1, reply);
        IFR_Bool memory_ok = true;
        IFR_FetchChunk *chunk = new IFR_ALLOCATOR(allocator) IFR_FetchChunk (IFR_FetchChunk::IFR_TYPE_ABSOLUTE_UP,
                                                                             m_MaxRows,
                                                                             reply,
                                                                             m_FetchInfo->getRecordSize (),
                                                                             m_MaxRows,
                                                                             this->m_RowsInResultSet,
                                                                             m_CurrentChunk,
                                                                             memory_ok,
                                                                             &(this->m_Statement->m_counter_4[IFR_StatementProfile::IFR_FETCHROWCOUNT_STMT]),
                                                                             &(this->m_Statement->m_counter_8[IFR_StatementProfile::IFR_DATAREADSIZE_8_STMT])
                                                                             );
        if(chunk==0 || !memory_ok) {
            error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(setCurrentChunk(chunk) != IFR_OK) {
            DBUG_RETURN(IFR_NOT_OK);
        }

      m_CurrentChunk->moveToUpperBound ();
      if (this->m_FetchInfo->error ()) {
          if (this->m_FetchInfo->error ().getErrorCode () != 100) {
              this->m_error.assign(m_FetchInfo->error(), true); // post fetchinfo error
              return IFR_NOT_OK;
          }
      }
      this->m_MaxRowsOutSideResult = 1; // set indicator
      return fetchLast (); // recursive call will trap in other condition
    }
  }
}

IFR_ResultSetMetaData*
IFR_ResultSet::getResultSetMetaData()
{
  DBUG_METHOD_ENTER(IFR_ResultSet, getResultSetMetaData);
  error().clear();
  IFR_Retcode rc = IFR_OK;
  if ((rc = assertNotClosed ()) != IFR_OK) {
    DBUG_RETURN((IFR_ResultSetMetaData *) 0);
  }
  return static_cast<IFR_ResultSetMetaData*> (this->m_FetchInfo);
}

static const char* getPositionString(int pos)
{
  if (pos==IFR_ResultSet::IFR_POSITION_BEFORE_FIRST)
    return "before";
  if (pos==IFR_ResultSet::IFR_POSITION_INSIDE)
    return "inside";
  if (pos==IFR_ResultSet::IFR_POSITION_AFTER_LAST)
    return "after";
  if (pos==IFR_ResultSet::IFR_POSITION_NOT_AVAILABLE)
    return "not available";
  static char buf[50];
  sprintf(buf, "UNKNOWN(%d)", pos);
  return buf;
}

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ResultSet& result )
{
  s << "start " << result.m_rowsetstartrow;
  s << " size " << result.m_rowsetsize;
  if (result.m_FetchedRows != 0)
    s << " rows " << result.m_FetchedRows;
  if (result.m_RowsInResultSet != -1)
    s << " count " << result.m_RowsInResultSet;
  s << " " << getPositionString(result.m_PositionState);
  s << "/" << getPositionString(result.m_PositionStateOfChunk);
  if (result.m_IsClosed)
    s << " closed";
  return s;
}

IFR_END_NAMESPACE
