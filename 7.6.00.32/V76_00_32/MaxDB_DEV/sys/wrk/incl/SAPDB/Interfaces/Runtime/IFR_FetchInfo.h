/*!
  @file           IFR_FetchInfo.h
  @author         D039759 D030044
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
#ifndef IFR_FETCHINFO_H
#define IFR_FETCHINFO_H

#include "Interfaces/Runtime/Util/IFRUtil_Vector.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Getval.h"
#include "Interfaces/Runtime/IFR_ResultSetMetaData.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"

IFR_BEGIN_NAMESPACE

class IFR_Statement;
class IFRPacket_ReplyPacket;

/**
 * @ingroup IFR_Fetch
 * This class controls how data is fetched after a statement that did 
 * produce a result set.
 */
class IFR_FetchInfo 
    : public IFRUtil_RuntimeItem,
      public IFR_ResultSetMetaData 
{
public:

  /**
   * Constructor. Creates a new fetch info for a specific
   * previously issued command that will produce a result set.
   * @param statement The statement wich generates the result.
   * @param info pointer to an array containing the conversion objects - might be null.
   * @param colName vector strings containing the column names - might be null.
   */
  IFR_FetchInfo (IFR_Statement& statement,
                 IFRConversion_ConverterList& info,
                 IFRUtil_Vector<IFR_String>* colName,
                 IFR_Bool& memory_ok);

    /**
     * @brief Destructor. 
     */
    ~IFR_FetchInfo ();

    /**
     * Executes an FETCH ABSOLUTE.
     * @param position The absolute position.
     * @param fetchSize The number of records to include in this fetch.
     * @param replyPacket The reply packet returned from the database.
     *
     * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeFetchAbsolute (IFR_Int4 position, 
                                      IFR_Int4 fetchSize, 
                                      IFRPacket_ReplyPacket& replyPacket);


  /**
   * Executes an FETCH RELATIVE.
   * @param position The relative position.
   * @param fetchSize The number of records to include in this fetch.
   * @param replyPacket The reply packet returned from the database.
   *
   * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
   */
    IFR_Retcode executeFetchRelative (IFR_Int4 position, 
                                      IFR_Int4 fetchSize, 
                                      IFRPacket_ReplyPacket& replyPacket);
        


    /**
     * Executes an FETCH FIRST.
     * @param fetchSize The number of records to include in this fetch.
     * @param replyPacket The reply packet returned from the database.
     *
     * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeFetchFirst (IFR_Int4 fetchSize, 
                                   IFRPacket_ReplyPacket& replyPacket);
    
    /**
     * Executes an FETCH NEXT
     * @param fetchSize The number of records to include in this fetch.
     * @param replyPacket The reply packet returned from the database.
     *
     * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeFetchNext (IFR_Int4 fetchSize, 
                                  IFRPacket_ReplyPacket& replyPacket);
                                   
    
    /**
     * Executes an FETCH LAST.
     * @param fetchSize The number of records to include in this fetch.
     * @param replyPacket The reply packet returned from the database.
     *
     * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeFetchLast (IFR_Int4 fetchSize, 				
                                  IFRPacket_ReplyPacket& replyPacket);

  /**
   * Executes a describe operation on the result table.
   *
   * @return <code>IFR_OK</code> if describe succed; <code>IFR_NOT_OK</code> otherwise.
   */
  IFR_Retcode describe ();

  /**
   * Returns the size of on database row in the order interface, including
   * the defined bytes.
   *
   * @return The size of one Row in bytes.
   */
  inline IFR_Int4 getRecordSize()
  {
      return m_RecordSize;
  }

  /**
   * Returns a reference to to data converter class. The data converter class
   * is produced by parsing the shortfield info and contains the converter
   * for each column of the row.
   *
   * @return A reference to a array of data converters.
   */
  inline IFRConversion_ConverterList& getColInfo()
  {
      return m_columninfo;
  }

  /**
   * Get a reference to the error that is stored here.
   * @return The error instance that is stored in this item.
   */
  inline IFR_ErrorHndl& error()
  {
      return m_error;
  }

  /**
   * Get a reference to the error that is stored here.
   * @return The error instance that is stored in this item (as constant reference).
   */
  inline const IFR_ErrorHndl& error () const
  {
      return m_error;
  }

  /**
   * Set the metadata for the row. 
   *
   * The metadata for a row is:
   * - The data converter
   * - The column names
   * - The calculated recordsize
   * @param info pointer to an array containing the conversion objects - might be null.
   * @param colName vector strings containing the column names - might be null.
   */
    IFR_Retcode setMetaData (IFRConversion_ConverterList& info,
                             IFRUtil_Vector<IFR_String>* colName);
  
    /**
     * Set the metadata for the row taken from the parseinfo as reference. 
     *
     * The metadata for a row is:
     * - The data converter
     * - The column names
     * - The calculated recordsize
     * @param info pointer to an array containing the conversion objects taken from parseinfo.
     * @param columncount number of columns in the row.
     */
    IFR_Retcode setMetaDataRefFromParseinfo (IFRConversion_Converter **info,
                                             IFR_UInt4 columncount);
		    
    /**
     * Gets the number of columns.
     */
    virtual IFR_Int2 getColumnCount();		    
    
    /**
     * Internal method to avoid virtual function calls.
     */
    inline IFR_Int2 getDirectColumnCount() const
    {
        return (IFR_Int2) m_columninfo.size();
    }
		    
protected:
    virtual IFRConversion_Converter * findColInfo   (IFR_Int2 columnIndex);

private:

    /**
     * Executes a given FETCH command.
     * @param command the command.
     * @param fetchSize The number of records to include in this fetch.
     * @param replyPacket the reply packet returned from the database.
     *
     * @return <code>IFR_OK</code> if fetch succeed; <code>IFR_NOT_OK</code> otherwise.
     */
    IFR_Retcode executeFetchCommand (IFR_String& command, 
                                     IFR_Int4 fetchSize, 
                                     IFRPacket_ReplyPacket& replyPacket);
    
    
    friend class IFR_ResultSet;
    friend class IFR_RowSet;

    IFR_String m_CursorName;                   //!< cursor name for building FETCH commands
    IFR_Statement &m_Statement;                //!< statement wich generates the result
    IFRConversion_ConverterList m_columninfo;  //!< Column infos.
    IFR_ErrorHndl m_error;                     //!< Error handler
    IFR_Int4 m_RecordSize;                     //!< physical row size
    static const char *s_fetchparamstring;     //!< FETCH parameter string template.
};

IFR_END_NAMESPACE

#endif // IFR_FETCHINFO_H

