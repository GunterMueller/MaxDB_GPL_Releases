/*! 
  -----------------------------------------------------------------------------
 
  module: DBMCli_Result.hpp
 
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Result handling
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                           Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*/

#ifndef _DBMCLI_RESULT_HPP_
#define _DBMCLI_RESULT_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_ResultBuf.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_Result
  -----------------------------------------------------------------------------
  description:  Class to handle the result from dbm-server.
  -----------------------------------------------------------------------------
*/
class DBMCli_Result : public DBMCli_ResultBuf
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_Result
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    nSize [IN] - size of result buffer
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_Result ( );
    DBMCli_Result (SAPDB_Int nSize);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_Result
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_Result ( );

    /*!
      -------------------------------------------------------------------------
      function:     Clear
      -------------------------------------------------------------------------
      description:  Clear the result buffer.
      -------------------------------------------------------------------------
    */
    void Clear ( );

    /*!
      -------------------------------------------------------------------------
      function:     Check
      -------------------------------------------------------------------------
      description:  Check the result buffer for error infos and move the current position pointer
                    to the start of the 2nd line if the first line is not "ERR". If the first line is
                    "ERR", all remaining lines are read into oMsgList and the position pointer
                    points behind the last line. This means if you call this method twice on a Result,
                    that starts with "OK", it returns true on the first call and false on the second one.
      arguments:    oMsgList [OUT] - object for indication of errors
      return value: True, if no error occurred, otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool Check ( Msg_List & oMsgList );

    /*!  
      Chapter: Properties
    */

    /*!
      -------------------------------------------------------------------------
      function:     ReturnCode
      -------------------------------------------------------------------------
      description:  Get the return code.
      return value: Return code number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int ReturnCode( ) { return m_nReturnCode; };

    /*!
      -------------------------------------------------------------------------
      function:     ErrorValue
      -------------------------------------------------------------------------
      description:  Get the error value
      return value: Error value text string.
      -------------------------------------------------------------------------

    */
    DBMCli_String & ErrorValue ( ) { return m_sErrorValue; };

    /*!
      -------------------------------------------------------------------------
      function:     ErrorText
      -------------------------------------------------------------------------
      description:  Get the error text
      return value: Error text string.
      -------------------------------------------------------------------------
    */
    DBMCli_String & ErrorText ( ) { return m_sErrorText; };

    /*!
      -------------------------------------------------------------------------
      function:     ExtReturnCode
      -------------------------------------------------------------------------
      description:  Get the extended error code
      return value: Extended return code number.
      -------------------------------------------------------------------------
    */
    SAPDB_Int ExtReturnCode( ) { return m_nExtReturnCode; };

    /*!
      -------------------------------------------------------------------------
      function:     ExtErrorText
      -------------------------------------------------------------------------
      description:  Get the extended error text.
      return value: Extended error text string.
      -------------------------------------------------------------------------
    */
    DBMCli_String & ExtErrorText ( ) { return m_sExtErrorText; };

    /*!  
      EndChapter: Properties
    */

  private:
    void ClearError ( );

  private:
    SAPDB_Int     m_nReturnCode;
    DBMCli_String m_sErrorValue;
    DBMCli_String m_sErrorText;

    SAPDB_Int     m_nExtReturnCode;
    DBMCli_String m_sExtErrorText;
}; 

/*! EndClass: DBMCli_Result */

#endif // _DBMCLI_RESULT_HPP_

