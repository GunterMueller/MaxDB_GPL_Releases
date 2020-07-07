/*!
  @file           IFR_RowSet.h
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
#ifndef IFR_ROWSET_H
#define IFR_ROWSET_H

#include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/Runtime/IFR_ResultSet.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Getval.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "SAPDB/Interfaces/Runtime/IFR_LOBHost.h"

IFR_BEGIN_NAMESPACE

/**
   @brief A class for representing a database rowset
 **/
class IFR_RowSet 
    : public IFRUtil_RuntimeItem,
      public IFR_ConnectionItem,
      public IFR_GetvalHost
{
  friend class IFR_ResultSet; //!< to use private constructor in <code>IFR_ResultSet</code>
  friend SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_RowSet *rowset);
  friend IFR_TraceStream* IFR_GetTraceStream(const IFR_RowSet *rowset);
public:
    /**
     * Destructor. 
     **/
    virtual ~IFR_RowSet ();

    /**
     * Returns the connection object for this resultset. 
     */
    IFR_Connection *getConnection() const;

    virtual IFR_GetvalHost *getGetvalHost();
    
    /**
     * @brief Sets the cursor to row <code>row</code> in rowset.
     * @param row the row number within the rowset
     * @return <code>IFR_OK</code> if the cursor is on a row
     *         <code>IFR_NO_DATA_FOUND</code> otherwise;
     *         <code>IFR_NOT_OK</code> if a database access error occurs,
     *         there is no current row, or the result set type is 
     *         <code>IFR_TYPE_FORWARD_ONLY</code>
     */
    IFR_Retcode setPos (IFR_UInt4 pos);

  /**
   * Retrieves and converts the value of the designated column in the current row of this 
   * <code>IFR_ResultSet</code> object as a <code>IFR_BYTE</code> with the given length and
   * the given <code>IFR_HostType</code>.
   * 
   * @param columnIndex the first column is 1, the second is 2, ...
   * @param buf pointer the output buffer.
   * @param buflen length the output buffer.
   * @param datatype hostvartype of the output buffer.
   * @param lengthindicator pointer to input length or output status indicator.
   * @param terminate indicates if the output buffer should be null terminated or not
   * @param startpos start position in long column to read data from (in bytes). Negative
   *        startpos counts from the end of the long column.
   *
   * @return <code>IFR_OK</code>, if all worked fine; <code>IFR_NOT_OK</code> if a database access 
   * error occurs.
   */  
  IFR_Retcode getObject (IFR_Int4 columnIndex, 
                         void* buf, 
                         IFR_Length buflen, 
                         IFR_HostType datatype, 
                         IFR_Length* lengthindicator, 
                         IFR_Bool terminate,
                         IFR_Length* startpos = 0,
                         IFR_Int4    row = -1);
    
  
    /**
     * @brief Fills the bound columns with fetched dat
     * @return <code>IFR_OK</code> if fill was successful
     *         <code>IFR_NOT_OK</code> otherwise
     */
    IFR_Retcode fetch ();
    

    virtual IFR_Retcode closeLOB(IFR_LOB* lob);
    
    virtual IFR_Retcode getData(IFR_LOB    *lob,
                                void       *data,
                                IFR_Length *lengthindicator,
                                IFR_Length  bytelength,
                                IFR_Length *posindicator,
                                IFR_Bool    terminate);
    
    virtual IFR_Length getLength(IFR_LOB *lob);

    /**
     * Returns the start row of this row set within the result set.
     * @return The start row.
     */
    inline IFR_Int4 getStartRow() const
    {
        return m_resultset->m_rowsetstartrow;
    }
    
protected:
    /**
     * Constructor
     */
    IFR_RowSet (IFR_ResultSet *rs, IFR_Bool& memory_ok); 
    
    IFR_UInt4      m_lastgetobjcol;  //!< column index from last getObject() call
    IFR_ResultSet *m_resultset;      //!< the corresponding resultset
private:
    IFR_Length     m_offset;         //!< The offset for piecewise reading of data
};

/**
 *  Traces the rowset object. 
 */
IFR_TraceStream& operator <<(IFR_TraceStream& s, 
                             const IFR_RowSet& rowset);


IFR_END_NAMESPACE

#endif // IFR_ROWSET_H
