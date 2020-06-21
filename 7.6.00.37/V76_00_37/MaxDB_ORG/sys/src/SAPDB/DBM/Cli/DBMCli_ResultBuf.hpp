/*! 
  -----------------------------------------------------------------------------
 
  module: DBMCli_ResultBuf.hpp
 
  -----------------------------------------------------------------------------
  responsible:  MartinR
  special area: Database Manager Client Library
  description:  Result buffer handling.
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

#ifndef _DBMCLI_RESULTBUF_HPP_
#define _DBMCLI_RESULTBUF_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"
#include "DBM/Cli/DBMCli_Stuff.hpp"

/*!
  -----------------------------------------------------------------------------
  Class:        DBMCli_ResultBuf
  -----------------------------------------------------------------------------
  description:  Class to handle the result buffer from dbm-server.
  -----------------------------------------------------------------------------
*/
class DBMCli_ResultBuf : public DBMCli_String
{
  public:

    /*!
      -------------------------------------------------------------------------
      function:     DBMCli_ResultBuf
      -------------------------------------------------------------------------
      description:  Constructor
      arguments:    nSize [IN] - size of result buffer
      prototypes:   2
      -------------------------------------------------------------------------
    */
    DBMCli_ResultBuf ( );
    DBMCli_ResultBuf (SAPDB_Int nSize);

    /*!
      -------------------------------------------------------------------------
      function:     ~DBMCli_ResultBuf
      -------------------------------------------------------------------------
      description:  Destructor
      -------------------------------------------------------------------------
    */
    ~DBMCli_ResultBuf ( );

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
      function:     GetPos
      -------------------------------------------------------------------------
      description:  Get the current position in buffer.
      return value: Current position in buffer.
      -------------------------------------------------------------------------
    */
    SAPDB_Int GetPos ( );

    /*!
      -------------------------------------------------------------------------
      function:     SetPos
      -------------------------------------------------------------------------
      description:  Set the current position in buffer
      arguments:    nPos [IN] - position in result buffer
      -------------------------------------------------------------------------
    */
    void SetPos ( SAPDB_Int nPos );

    /*!
      -------------------------------------------------------------------------
      function:     GetPart
      -------------------------------------------------------------------------
      description:  Get a part from the buffer.
      arguments:    sPart   [OUT] - the part
                    nMaxLen [IN]  - max. len of part string  
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetPart ( DBMCli_String & sPart,
                         SAPDB_Int       nMaxLen = -1 );

    /*!
      -------------------------------------------------------------------------
      function:     GetLine
      -------------------------------------------------------------------------
      description:  Get the next line in buffer.
      arguments:    sLine [OUT] - the line
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetLine ( DBMCli_String & sLine );

    /*!
      -------------------------------------------------------------------------
      function:     GetFullLine
      -------------------------------------------------------------------------
      description:  Get the next full line in buffer.
      arguments:    sLine [OUT] - the line
      return value: If the end of buffer is reached or no full line available 
                    return value is false otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetFullLine ( DBMCli_String & sLine );

    /*!
      -------------------------------------------------------------------------
      function:     GetField
      -------------------------------------------------------------------------
      description:  Get a field in the buffer.
      arguments:    sField      [OUT]  - the field
                    sSeperators [IN]   - the possible field seperators
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetField (       DBMCli_String & sField, 
                          const DBMCli_String & sSeperators );

    /*!
      -------------------------------------------------------------------------
      function:     GetPropVal
      -------------------------------------------------------------------------
      description:  Get a property name and value from the buffer.
      arguments:    sProperty   [OUT]  - the property name
                    sValue      [OUT]  - the property value
                    sSeperators [IN]   - the possible field seperators
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      prototypes:   2
      -------------------------------------------------------------------------
    */
    SAPDB_Bool GetPropVal ( DBMCli_String & sProperty, 
                            DBMCli_String & sValue);

    SAPDB_Bool GetPropVal (       DBMCli_String & sProperty, 
                                  DBMCli_String & sValue,
                            const DBMCli_String & sSeperators );

    /*!
      -------------------------------------------------------------------------
      function:     SkipLine
      -------------------------------------------------------------------------
      description:  Skip a line in the buffer.
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SkipLine ( );

    /*!
      -------------------------------------------------------------------------
      function:     SkipField
      -------------------------------------------------------------------------
      description:  Skip a field in the buffer.
      arguments:    sSeperators [IN]  - the possible field seperators
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SkipField ( const DBMCli_String & sSeperators );

    /*!
      -------------------------------------------------------------------------
      function:     SkipBlanks
      -------------------------------------------------------------------------
      description:  Skip blank chars in the buffer.
      return value: If the end of buffer is reached return value is false
                    otherwise true.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool SkipBlanks ( );

    /*!
      -------------------------------------------------------------------------
      function:     EndOfBuffer
      -------------------------------------------------------------------------
      description:  Check current pos in buffer.
      return value: If the end of buffer is reached return value is true
                    otherwise false.
      -------------------------------------------------------------------------
    */
    SAPDB_Bool EndOfBuffer ( );


  private:
    SAPDB_Int m_nPos;
}; 

/*! EndClass: DBMCli_ResultBuf */

#endif // _DBMCLI_RESULTBUF_HPP_

