/*!
  @file           IFR_UpdatableRowSet.h
  @author         TTK
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
#ifndef IFR_UPDATABLEROWSET_H
#define IFR_UPDATABLEROWSET_H

#include "SAPDB/Interfaces/Runtime/IFR_RowSet.h"

IFR_BEGIN_NAMESPACE

/**
 * @brief A class for representing an updatable database rowset.
 */
class IFR_UpdatableRowSet : public IFR_RowSet
{
friend class IFR_RowSet;
    
public:
    /**
     * Creates a new updatable row set object.
     * @param connection the current connection.
     * @param resultSet  the (parent) result set
     */
    IFR_UpdatableRowSet (IFR_ResultSet *resultSet, IFR_Bool& memory_ok);

    /**
     * Deletes an updatable row set.
     */
    ~IFR_UpdatableRowSet ();

    IFR_Retcode insertRow(SQLDBC_UInt4 position);

    /**
     * Updates the row on a given position in the row array. Position 1
     * is the first position.
     */
    IFR_Retcode updateRow (SQLDBC_UInt4 position);
    

    IFR_Retcode nextParameter(IFR_Int2& index, void*& parameterdata);
    IFR_Retcode nextParameterByIndex(IFR_Int2& index, void*& parameterdata);
    IFR_Retcode putData(void *parameterdata, IFR_Length *lengthindicator);

    /**
     * Deletes the row on a given position in the row array. Position 1
     * is the first position.
     */
    IFR_Retcode deleteRow (SQLDBC_UInt4 position);

private:
    enum StatementType {
        UNKNOWN_STATEMENT    = 0,
        INSERT_STATEMENT,
        UPDATE_STATEMENT,
        DELETE_STATEMENT
    };

    enum Status {
        STATUS_NONE         = 0,
        STATUS_DATA_EXECUTE 
    };

    /**
     * Checks if update or delete is allowed (i.e. if statement was
     * executed with updatable cursor concurrency).
     */
    IFR_Bool isResultSetUpdatable();
    
    IFR_Retcode buildStatement(StatementType statement_type,
                               unsigned char *current_parameterset,
                               IFR_Bool& memory_ok);
    IFR_Retcode buildParameterSet(IFR_UInt4 position, unsigned char *parameterset, IFR_Bool& empty);
    IFR_Retcode bindColumnsAsParameters(IFR_UInt4 position, IFR_Int4 *cursorpos);
    IFR_Retcode handleExecute(StatementType statement_type);
    void clearStatement();
    void finishRow();
    void setRemainingToFailed();
    
    Status            m_status;
    IFR_Length        m_index;
    IFR_Length        m_finish;
    StatementType     m_statement_type;
    IFR_PreparedStmt *m_statement;
    IFR_Length        m_parameterset_size;
    unsigned char    *m_parameterset;
    IFR_Int4          m_rowstatus;
    IFR_Int4          m_cursorpos;
};

IFR_END_NAMESPACE

#endif // IFR_UPDATABLEROWSET_H
