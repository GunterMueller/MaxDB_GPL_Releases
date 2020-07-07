/*!
  @file           DBMSrvKnl_SQLRecord.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Specification

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef _DBMSrvKnl_SQLRecord_HPP_
#define _DBMSrvKnl_SQLRecord_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "vsp001.h"
#include "SAPDB/ToolsCommon/Tools_DynamicUTF8String.hpp"

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
 */
//class  PIn_ReplySegment;
class  DBMSrvKnl_Session;

/*! @brief SQL Record Object */
class DBMSrvKnl_SQLRecord {

  public:
    /*! @brief   RefreshMode */
    typedef enum {
      refreshAll,
      refreshWithoutRecordCounter,
      resetAll
    } RefreshMode;

    /*! @brief destructor */
    ~DBMSrvKnl_SQLRecord  ( );

    /*! @brief      returns a field contents

                    If there no valid conversion from DB datatype to SAPDB_Int4
                    or any other error occured a value of 0 will be returned.
                    Check also isNull() and fieldType() if you won't trust this
                    behavior.

        @param      nField  [IN] - number of field

        @return     field value
     */
    SAPDB_Int4  fieldAsInt ( const SAPDB_Int4 nField  ) const;

    /*! @brief      returns a field contents

                    If there no valid conversion from DB datatype to SAPDB_Int4
                    or any other error occured a value of 0 will be returned.
                    Check also isNull() and fieldType() if you won't trust this
                    behavior.

        @param      szField [IN] - name of field

        @return     field value
     */
    SAPDB_Int4  fieldAsInt ( const Tools_DynamicUTF8String & szField ) const;

    /*! @brief      returns a field contents

                    If there no valid conversion from DB datatype to _TCHAR *
                    or any other error occured a empty String will be returned.
                    Check also isNull() and fieldType() if you won't trust this
                    behavior.

                    The return value is a temporarely allocated buffer. It will
                    only valid until the next call of fieldAsChar or until the
                    destruction of the object.

      @param        nField       [IN] - number of field
      @param        cStringQuote [in] - character for quoting strings
                                        0 means no quoting
      @return       field value
     */
    Tools_DynamicUTF8String fieldAsChar ( const SAPDB_Int4 nField,
                                          const char       cStringQuote = 0) const;

    /*! @brief      returns a field contents

                    If there no valid conversion from DB datatype to _TCHAR *
                    or any other error occured a empty String will be returned.
                    Check also isNull() and fieldType() if you won't trust this
                    behavior.

                    The return value is a temporarely allocated buffer. It will
                    only valid until the next call of fieldAsChar or until the
                    destruction of the object.

      @param        szField      [IN] - name of field
      @param        cStringQuote [in] - character for quoting strings
                                        0 means no quoting
      @return       field value
     */
    Tools_DynamicUTF8String fieldAsChar ( const Tools_DynamicUTF8String & szField,
                                          const char                      cStringQuote = 0) const;

    /*! @brief      returns a field type
        @param      nField       [IN] - number of field
        @return     field type
     */
    tsp00_DataType fieldType ( const SAPDB_Int4  nField  ) const;

    /*! @brief      returns a field type
        @param      szField      [IN] - name of field
        @return     field type
     */
    tsp00_DataType fieldType ( const Tools_DynamicUTF8String &  szField ) const;

    /*! @brief      returns a the nullvalue flag
        @param      nField       [IN] - number of field
        @return     true if null, otherwise false
     */
    bool isNull ( const SAPDB_Int4 nField  ) const;

    /*! @brief      returns a the nullvalue flag
        @param      szField      [IN] - name of field
        @return     true if null, otherwise false
     */
    bool isNull ( const Tools_DynamicUTF8String &  szField ) const;

    /*! @brief      return the maximal size of a field value
        @param      nField       [IN] - number of field
        @return     size of field
     */
    size_t size ( const SAPDB_Int4 nField  ) const;

    /*! @brief      return the maximal size of a field value
        @param      szField      [IN] - name of field
        @return     size of field
     */
    size_t size ( const Tools_DynamicUTF8String &  szField  ) const;

    /*! @brief      return the size of a column regarding the columname and the value size
        @param      nField       [IN] - number of field
        @return     size of field
     */
    size_t maxSize ( const SAPDB_Int4 nField  ) const;

    /*! @brief      return the size of a column regarding the columname and the value size
        @param      szField      [IN] - name of field
        @return     size of field
     */
    size_t maxSize ( const Tools_DynamicUTF8String &  szField  ) const;

    /*! @brief      check the existence of a field
        @param      nField       [IN] - number of field
        @return     true if null, otherwise false
     */
    bool exists ( const SAPDB_Int4 nField  ) const;

    /*! @brief      check the existence of a field
        @param      szField      [IN] - name of field
        @return     true if null, otherwise false
     */
    bool exists ( const Tools_DynamicUTF8String &  szField ) const;

    /*! @brief      returns a field name

        @param      nField  [IN] - number of field, enumeration of fileds is
                                   starting with 1

                    This is a temporarely allocated buffer. It will only valid
                    until the next call of fieldName or until the destruction
                    of the object.

        @return     field name
     */
    Tools_DynamicUTF8String fieldName ( const SAPDB_Int4 nField  ) const;

    /*! @brief      returns a field number
        @param      szField  [IN] - name of field
        @return     field number
     */
    SAPDB_Int4 fieldNumber ( const Tools_DynamicUTF8String & szField  ) const;

    /*! @brief      gets the nex record
        @param      bEnd [OUT]     - no record available
        @param      oMsgList [OUT] - error message 
        @return     success or not
     */
    bool getNextRecord ( bool & bEnd, DBMSrvMsg_Error & oMsgList);

    /*! @brief      returns the number of fields
        @return     number of fields
     */
    SAPDB_Int4 fields ( );

  private:
    /*! @brief constructor
     */
    DBMSrvKnl_SQLRecord  ( );
    /*! @brief set kernel session
        @param  pSession [IN]  - kernel session object
     */
    void SetSession ( DBMSrvKnl_Session * pSession );
    /*! @brief      gets the first record
        @param      bEnd [OUT]     - no record available
        @param      oMsgList [OUT] - error message 
        @return     success or not
     */
    bool getFirstRecord ( bool & bEnd, DBMSrvMsg_Error & oMsgList );
    /*! @brief      gets the next/first record
        @param      bEnd   [OUT]   - no record available
        @param      bFirst [IN]    - first or next record?
        @param      oMsgList [OUT] - error message 
        @return     success or not
     */
    bool getRecord ( bool & bEnd, bool bFirst, DBMSrvMsg_Error & oMsgList);
    /*! @brief      refresh the record data
        @param      bResetRecordCounter  [IN] - reset the current record or not
     */
    void RefreshRecordData
      ( RefreshMode oRefreshMode );
    /*! @brief check for string */
    bool IsString
      ( tsp00_DataType aDataType ) const;
    /*! @brief check for long */
    bool IsLong
      ( tsp00_DataType aDataType ) const;

    /*! @brief private member */
    DBMSrvKnl_Session *    pKernelSession;
    /*! @brief private member */
    SAPDB_Int4             nParamCount;
    /*! @brief private member */
    tsp1_param_info *      pParamInfo;
    /*! @brief private member */
    SAPDB_Int4             nCurrentRecord;
    /*! @brief private member */
    SAPDB_Int4             nMaxRecords;
    /*! @brief private member */
    SAPDB_Int4             nRecordSize;

    friend class DBMSrvKnl_Session;
}; // end class DBMSrvKnl_SQLRecord

#endif /* _DBMSrvKnl_SQLRecord_HPP_ */
