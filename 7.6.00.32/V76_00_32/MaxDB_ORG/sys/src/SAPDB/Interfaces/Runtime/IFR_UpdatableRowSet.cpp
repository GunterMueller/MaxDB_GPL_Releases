/*!
  @file           IFR_UpdatableRowSet.cpp
  @author         TTK
  @ingroup        IFR_Fetch
  @brief          Implements class for updatable rowset processing
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

#include "Interfaces/Runtime/IFR_UpdatableRowSet.h"
#include "Interfaces/Runtime/IFR_PreparedStmt.h"

//----------------------------------------------------------------------
IFR_UpdatableRowSet::IFR_UpdatableRowSet (IFR_ResultSet *resultSet,
                                          IFR_Bool& memory_ok)
:IFR_LOBHost(resultSet->m_Connection->allocator),
 IFR_RowSet (resultSet, memory_ok),
 m_status(STATUS_NONE),
 m_statement_type(UNKNOWN_STATEMENT),
 m_statement(0),
 m_parameterset(0),
 m_cursorpos(0)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, IFR_UpdatableRowSet);
    if(memory_ok) {
        m_parameterset_size = resultSet->m_FetchInfo->getColumnCount();
        m_parameterset = (unsigned char *)m_resultset->allocator.Allocate(m_parameterset_size);
        if(m_parameterset == 0) {
            memory_ok = false;
        } else {
            memset(m_parameterset, 0, m_parameterset_size);
        }
    }
}

//----------------------------------------------------------------------
IFR_UpdatableRowSet::~IFR_UpdatableRowSet()
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, ~IFR_UpdatableRowSet);
    if(m_statement) {
        m_resultset->m_Connection->releaseStatement(m_statement);
    }
    if(m_parameterset) {
        m_resultset->allocator.Deallocate(m_parameterset);
    }
}


//----------------------------------------------------------------------
IFR_Retcode IFR_UpdatableRowSet::insertRow(SQLDBC_UInt4 position)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, insertRow);
    DBUG_PRINT(position);

    if(m_status == STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(!isResultSetUpdatable()) {
        // sets error in this case
        DBUG_RETURN(IFR_NOT_OK);
    }

    // setup start and end position
    m_index = position;
    if(m_index) {
        m_finish = position;
    } else {
        m_index  = 1;
        m_finish = m_resultset->getRowSetSize();
    }

    DBUG_RETURN(handleExecute(INSERT_STATEMENT));
};

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::handleExecute(StatementType statement_type)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, handleExecute);
    IFR_Retcode rc = IFR_OK;
    unsigned char *parameterset = (unsigned char *)alloca(m_parameterset_size);
    while(m_index <= m_finish) {
        if(m_resultset->m_rowstatusarray[m_index - 1] == IFR_ROW_IGNORE) {
            ++m_index;
            continue;
        } else if(m_resultset->m_rowstatusarray[m_index - 1] == IFR_NOROW && statement_type != UPDATE_STATEMENT) {
            m_resultset->error().setRuntimeError(IFR_ERR_INVALID_ROWSET_POS_I, (IFR_Int4)m_index);
            clearStatement();
            setRemainingToFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }

        IFR_Bool empty;
        IFR_Bool memory_ok = true;
        rc = buildParameterSet(m_index, parameterset, empty);
        if(rc != IFR_OK) {
            m_status = STATUS_NONE;
            clearStatement();
            m_resultset->error().setRuntimeError(IFR_ERR_ROWSET_NO_METADATA);
            m_resultset->m_rowstatusarray[m_index - 1] = rc;
            setRemainingToFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        if(empty && statement_type != DELETE_STATEMENT) {
            m_status = STATUS_NONE;
            clearStatement();
            m_resultset->error().setRuntimeError(IFR_ERR_NO_DATA_FOR_UPDATE);
            m_resultset->m_rowstatusarray[m_index - 1] = IFR_NOT_OK;
            setRemainingToFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        rc = buildStatement(statement_type, parameterset, memory_ok);
        if(rc != IFR_OK) {
            clearStatement();
            m_resultset->m_rowstatusarray[m_index - 1] = rc;
            setRemainingToFailed();
            DBUG_RETURN(rc);
        }
        IFR_Int4 cursorpos = 0;
        rc = bindColumnsAsParameters(m_index, &cursorpos);
        if(rc != IFR_OK) {
            clearStatement();
            m_resultset->m_rowstatusarray[m_index - 1] = rc;
            setRemainingToFailed();
            DBUG_RETURN(rc);
        }
        rc = m_statement->execute();
        if(rc == IFR_NEED_DATA) {
            m_status = STATUS_DATA_EXECUTE;
            DBUG_RETURN(rc);
        } else if(rc != IFR_OK) {
            m_resultset->error().assign(m_statement->error(), false);
            m_resultset->m_rowstatusarray[m_index - 1] = rc;
            clearStatement();
            setRemainingToFailed();
            DBUG_RETURN(rc);
        }
        finishRow();
    }
    m_status = STATUS_NONE;
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
void
IFR_UpdatableRowSet::finishRow()
{
    switch(m_statement_type) {
    case INSERT_STATEMENT:
        m_resultset->m_rowstatusarray[m_index - 1] = IFR_ADDED;
        break;
    case UPDATE_STATEMENT:
        m_resultset->m_rowstatusarray[m_index - 1] = IFR_UPDATED;
        break;
    case DELETE_STATEMENT:
        m_resultset->m_rowstatusarray[m_index - 1] = IFR_DELETED;
        break;
    }
    ++m_index;
}

//----------------------------------------------------------------------
void
IFR_UpdatableRowSet::setRemainingToFailed()
{
    for(IFR_Int4 i=m_index + 1; i<=m_finish; ++i) {
        m_resultset->m_rowstatusarray[i - 1] = IFR_NOT_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::updateRow (SQLDBC_UInt4 position)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, updateRow);
    DBUG_PRINT(position);

    if(m_status == STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(!isResultSetUpdatable()) {
        // sets error in this case
        DBUG_RETURN(IFR_NOT_OK);
    }

    // setup start and end position
    m_index = position;
    if(m_index) {
        m_finish = position;
    } else {
        m_index  = 1;
        m_finish = m_resultset->getRowSetSize();
    }

    DBUG_RETURN(handleExecute(UPDATE_STATEMENT));
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::deleteRow (SQLDBC_UInt4 position)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, updateRow);
    DBUG_PRINT(position);

    if(m_status == STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_DATA_EXPECTED);
        DBUG_RETURN(IFR_NOT_OK);
    }

    if(!isResultSetUpdatable()) {
        // sets error in this case
        DBUG_RETURN(IFR_NOT_OK);
    }

    // setup start and end position
    m_index = position;
    if(m_index) {
        m_finish = position;
    } else {
        m_index  = 1;
        m_finish = m_resultset->getRowSetSize();
    }

    DBUG_RETURN(handleExecute(DELETE_STATEMENT));
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::nextParameter(IFR_Int2& index, void*& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, nextParameter);
    if(m_status != STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        clearStatement();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc = m_statement->nextParameter(index, parameterdata);
    if(rc == IFR_OK) {
        finishRow();
        rc = handleExecute(m_statement_type);
        if(rc == IFR_NEED_DATA) {
            rc = m_statement->nextParameter(index, parameterdata);
        }
    }
    if(rc != IFR_NEED_DATA && rc != IFR_OK) {
        m_resultset->error().assign(m_statement->error(), false);
        clearStatement();
        m_resultset->m_rowstatusarray[m_index - 1] = rc;
        setRemainingToFailed();
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::nextParameterByIndex(IFR_Int2& index, void*& parameterdata)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, nextParameterByIndex);
    if(m_status != STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        clearStatement();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc = m_statement->nextParameterByIndex(index, parameterdata);
    if(rc == IFR_OK) {
        finishRow();
        rc = handleExecute(m_statement_type);
        if(rc == IFR_NEED_DATA) {
            rc = m_statement->nextParameterByIndex(index, parameterdata);
        }
    }
    if(rc != IFR_NEED_DATA && rc != IFR_OK) {
        m_resultset->error().assign(m_statement->error(), false);
        clearStatement();
        m_resultset->m_rowstatusarray[m_index - 1] = rc;
        setRemainingToFailed();
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::putData(void *parameterdata, IFR_Length *lengthindicator)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, putData);
    if(m_status != STATUS_DATA_EXECUTE) {
        m_resultset->error().setRuntimeError(IFR_ERR_SQLCMD_NO_DATA_EXPECTED);
        clearStatement();
        DBUG_RETURN(IFR_NOT_OK);
    }
    IFR_Retcode rc=m_statement->putData(parameterdata, lengthindicator);
    if(rc != IFR_OK && rc != IFR_DATA_TRUNC) {
        m_resultset->error().assign(m_statement->error(), false);
        clearStatement();
        m_resultset->m_rowstatusarray[m_index - 1] = rc;
        setRemainingToFailed();
    }
    DBUG_RETURN(rc);
}

//----------------------------------------------------------------------
IFR_Bool IFR_UpdatableRowSet::isResultSetUpdatable()
{
    if (m_resultset->m_concurType != IFR_Statement::UPDATABLE) {
        m_resultset->error().setRuntimeError(IFR_ERR_NO_UPDATABLE_ROWSET);
        return IFR_FALSE;
    }
    return IFR_TRUE;
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::buildStatement(StatementType statement_type,
                                    unsigned char *current_parameterset,
                                    IFR_Bool& memory_ok)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, buildStatement);

    if(m_statement == 0) {
        m_statement = m_resultset->m_Connection->createPreparedStatement();
        if(m_statement == 0) {
            memory_ok = false;
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    if(m_statement_type == statement_type) {
        if(memcmp(current_parameterset, m_parameterset, m_parameterset_size)==0) {
            DBUG_RETURN(IFR_OK);
        }
    }

    if(statement_type == INSERT_STATEMENT || statement_type == UPDATE_STATEMENT) {
        m_statement_type = UNKNOWN_STATEMENT;

        IFR_ResultSetMetaData *md = m_resultset->getResultSetMetaData();
        const IFR_Statement    *s  = m_resultset->getStatement();
        if(md == 0 || s == 0) {
            m_resultset->error().setRuntimeError(IFR_ERR_ROWSET_NO_METADATA);
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_StringEncoding encoding = s->getTableName().getEncoding();
        IFR_String command(encoding, m_resultset->allocator);
        if(statement_type == INSERT_STATEMENT) {
            command.append("INSERT ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        } else {
            command.append("UPDATE ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        }
        command.append(s->getTableName(), memory_ok); // tablename is enclosed in quotes.
        command.append(" SET ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            m_resultset->error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        IFR_Bool first = true;
        IFR_Int2 columncount = md->getColumnCount();
        char buffer[256];
        char *columnname =(char *) buffer;
        IFR_Length columnname_length = 0;
        IFR_Length columnname_size = sizeof(buffer);
        for(IFR_Int2 i=1; i<=columncount; ++i) {
            if(current_parameterset[i-1]) {
                if(first) {
                    command.append("\"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                    first = false;
                } else {
                    command.append(", \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                }
                IFR_Retcode rc;
                while( (rc = md->getColumnName(i, columnname, encoding, columnname_size-2, &columnname_length))
                       == IFR_DATA_TRUNC) {
                    if(columnname == buffer) {
                        columnname = (char *) IFR_ALLOCATOR(m_resultset->allocator).Allocate(columnname_length + 2);
                    } else {
                        IFR_ALLOCATOR(m_resultset->allocator).Deallocate(columnname);
                        columnname = (char *) IFR_ALLOCATOR(m_resultset->allocator).Allocate(columnname_length + 2);
                    }
                    if(columnname == 0) {
                        m_resultset->error().setMemoryAllocationFailed();
                        memory_ok = false;
                        DBUG_RETURN(IFR_NOT_OK);
                    }
                }
                if(rc != IFR_OK) {
                    if(columnname != buffer) {
                        IFR_ALLOCATOR(m_resultset->allocator).Deallocate(columnname);
                    }
                    m_resultset->error().setRuntimeError(IFR_ERR_ROWSET_WRONG_COLUMN_ENCODING_I, (IFR_Int4)i);
                    DBUG_RETURN(IFR_NOT_OK);
                }
                command.append(columnname, encoding, IFR_NTS, memory_ok);
                command.append("\"=?", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                if(!memory_ok) {
                    if(columnname != buffer) {
                        IFR_ALLOCATOR(m_resultset->allocator).Deallocate(columnname);
                    }
                    m_resultset->error().setMemoryAllocationFailed();
                    DBUG_RETURN(IFR_NOT_OK);
                }
            }
        }
        if(statement_type == UPDATE_STATEMENT) {
            command.append(" WHERE POS OF \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            command.append(s->getCursorName(), memory_ok);
            command.append("\" IS ?", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                m_resultset->error().setMemoryAllocationFailed();
                DBUG_RETURN(IFR_NOT_OK);
            }
        }
        m_statement->clearParameters();
        IFR_Retcode rc = m_statement->prepare(command.getBuffer(), command.getLength(), command.getEncoding());
        if(rc != IFR_OK) {
            m_resultset->error().assign(m_statement->error(), false);
            DBUG_RETURN(rc);
        }
        m_statement_type = statement_type;
        memcpy(m_parameterset, current_parameterset, m_parameterset_size);
        DBUG_RETURN(IFR_OK);
    } else if(statement_type == DELETE_STATEMENT) {
        if(m_statement_type == DELETE_STATEMENT) {
            DBUG_RETURN(IFR_OK);
        }
        m_statement_type = UNKNOWN_STATEMENT;
        const IFR_Statement    *s  = m_resultset->getStatement();
        IFR_StringEncoding encoding = s->getTableName().getEncoding();
        IFR_String command(encoding, m_resultset->allocator);
        command.append("DELETE FROM ", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        command.append(s->getTableName(), memory_ok);
        command.append(" WHERE POS OF \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        command.append(s->getCursorName(), memory_ok);
        command.append("\" IS ?", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            m_resultset->error().setMemoryAllocationFailed();
            DBUG_RETURN(IFR_NOT_OK);
        }
        m_statement->clearParameters();
        IFR_Retcode rc = m_statement->prepare(command.getBuffer(), command.getLength(), command.getEncoding());
        if(rc != IFR_OK) {
            DBUG_RETURN(rc);
        }
        m_statement_type = statement_type;
        // parameter set not necessary for delete
        DBUG_RETURN(IFR_OK);
    }
    DBUG_RETURN(IFR_NOT_OK);
}


//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::buildParameterSet(SQLDBC_UInt4 position, unsigned char *parameterset, IFR_Bool& empty)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, buildParameterSet);
    const IFRUtil_Vector<IFR_Parameter> *pv = m_resultset->getParamVector();
    IFR_Length pvsize = pv->GetSize();
    memset(parameterset, 0, m_parameterset_size);
    empty = true;
    for(IFR_Length i=0; i<pvsize; ++i) {
        IFR_Parameter *param = (IFR_Parameter *)&( (*pv)[i] );
        if(param->getHostType() == IFR_HOSTTYPE_PARAMETER_NOTSET) {
            continue;
        }
        IFR_Length *li = param->getLengthIndicator(position - 1, 0);
        if(li) {
            if(*li != IFR_IGNORE) {
                parameterset[i] = 1;
                empty = false;
            }
        } else {
            parameterset[i] = 1;
            empty = false;
        }
    }
    DBUG_RETURN(IFR_OK);
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_UpdatableRowSet::bindColumnsAsParameters(IFR_UInt4 position, IFR_Int4* cursorpos)
{
    DBUG_METHOD_ENTER(IFR_UpdatableRowSet, bindColumnsAsParameters);
    IFR_Int4 current_index = 1;
    IFR_Retcode rc=IFR_OK;
    // set the binding type, as it may have changed
    // m_statement->setBindingType(m_resultset->m_bindingtype);
    if(m_statement_type == INSERT_STATEMENT ||
       m_statement_type == UPDATE_STATEMENT) {
        const IFRUtil_Vector<IFR_Parameter> *pv = m_resultset->getParamVector();
        for(IFR_Int4 i=0; i<m_parameterset_size; ++i) {
            if(m_parameterset[i]) {
                IFR_Parameter *param = (IFR_Parameter *)&( (*pv)[i] );
                rc = m_statement->bindParameter(current_index,
                                                param->getHostType(),
                                                (char *)param->data(position - 1, m_resultset->m_binding_type),
                                                param->getLengthIndicator(position - 1, m_resultset->m_binding_type),
                                                param->getBytesLength(),
                                                param->shouldTerminated());
                if(rc != IFR_OK) {
                    m_resultset->error().assign(m_statement->error(), false);
                    m_statement->clearParameters();
                    DBUG_RETURN(IFR_NOT_OK);
                }
                ++current_index;
            }
        }
    }
    if(m_statement_type == DELETE_STATEMENT ||
       m_statement_type == UPDATE_STATEMENT) {
        *cursorpos = m_cursorpos = m_resultset->m_rowsetstartrow + position - 1;
        rc = m_statement->bindParameter(current_index,
                                        IFR_HOSTTYPE_INT4,
                                        &m_cursorpos,
                                        0,
                                        sizeof(IFR_Int4),
                                        false);
        if(rc != IFR_OK) {
            m_resultset->error().assign(m_statement->error(), false);
            m_statement->clearParameters();
            DBUG_RETURN(IFR_NOT_OK);
        }
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
void
IFR_UpdatableRowSet::clearStatement()
{
    m_resultset->m_Connection->releaseStatement(m_statement);
    memset(m_parameterset, 0, m_parameterset_size);
    m_statement_type = UNKNOWN_STATEMENT;
    m_statement = 0;
}


