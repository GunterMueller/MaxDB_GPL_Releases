/*!
  @file           IFR_LOB.h
  @author         D039759
  @ingroup
  @brief          LOB locators.
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
#ifndef IFR_LOB_H
#define IFR_LOB_H

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"

IFR_BEGIN_NAMESPACE

class IFR_PutvalHost;
class IFR_GetvalHost;
struct IFR_LOBData;

/**
 * Wrapper class for a 'large object'. A LOB is a handle to a large
 * object either inserted or updated, or returned by a statement.
 * A LOB is accessed after the statement itself is executed.
 */
class IFR_LOB
{
public :
    enum Status {
        Status_Valid_C         = 0,
        Status_Closed_C        = 1,
        Status_Invalid_C       = 2,
        Status_NullOrDefault_C = 3
    };

    /**
     * Constructor.
     * @param column The column of the LOB.
     * @param row The row of the LOB.
     * @param lobhosttype The host type of the LOB.
     * @param citem The link to the connection (and GETVAL/PUTVAL host instance).
     */
    IFR_LOB(IFR_Int2 column,
            IFR_Int4 row,
            IFR_HostType lobhosttype,
            IFR_ConnectionItem& citem,
            IFR_Bool input);

    /**
     * Destructor.
     */
    ~IFR_LOB();

    /**
     * Write data into the LONG associated with this LOB.
     * @param data The data.
     * @param lengthindicator The length or indicator value.
     * @return @c IFR_OK on success.
     */
    IFR_Retcode putData(void *data, IFR_Length* lengthindicator);

    IFR_Retcode getData(void *data,
                        IFR_Length *lengthindicator,
                        IFR_Length  bytelength,
                        IFR_Length *posindicator,
                        IFR_Bool    terminate);

    IFR_Length getLength();

    /**
     * Closes the LOB. Also, the associated LONG value is closed.
     */
    IFR_Retcode close();


    /**
     * Gets the column of this LOB.
     * @return The column of the LOB.
     */
    inline IFR_Int2 getColumn() const
    {
        return m_column;
    }

    /**
     * Gets the row of the LOB.
     * @return The row of the LOB.
     */
    inline IFR_Int4 getRow() const
    {
        return m_row;
    }

    inline void invalidate() { m_status = Status_Invalid_C; }

    inline void setNull() { m_status = Status_NullOrDefault_C; }

    inline IFR_Bool isOpen() { return m_status == Status_Valid_C; }

    inline IFR_Bool isInput() { return m_putvalhost != 0; }

    inline IFR_Bool isOutput() { return m_getvalhost != 0; }

    IFR_HostType getDataHostType() const;

    static void sqlTrace(IFR_TraceStream& s, IFR_LOBData& lobdata);

    IFR_Length getPosition() const
    {
        return m_position;
    }

    IFR_Length getLengthValue(IFR_Length  length,
                              IFR_SQLType datatype,
                              IFR_Bool    binhex);
private:
    IFR_Bool assertOpen(void);

    IFR_Int2            m_column;
    IFR_Int4            m_row;

    IFR_HostType        m_lobhosttype;
    IFR_ConnectionItem& m_clink;
    IFR_GetvalHost     *m_getvalhost;
    IFR_PutvalHost     *m_putvalhost;
    IFR_Int1            m_status;
    IFR_Length          m_position;
};

/**
 * This is what the user actually has bound. For extra safety, we write the originating
 * connection item into the LOB. This is to check the validity of the LOB before
 * accessing member functions (on a possibly deleted pointer).
 */
struct IFR_LOBData
{
    IFR_LOB             *lobdata;
    IFR_ConnectionItem  *connectionitem;
};

IFR_END_NAMESPACE

#endif
