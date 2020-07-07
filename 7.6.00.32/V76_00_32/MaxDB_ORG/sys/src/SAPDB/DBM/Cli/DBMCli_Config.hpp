/*!
  @file           DBMCli_Config.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          database configuration object

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

#ifndef _DBMCli_Config_HPP_
#define _DBMCli_Config_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Common.hpp"

/*
  -----------------------------------------------------------------------------
  defines
  -----------------------------------------------------------------------------
*/

/*
  -----------------------------------------------------------------------------
  forward declarations
  -----------------------------------------------------------------------------
*/
class DBMCli_Database;

/*!
  @brief     Class to handle DBMServer configuration values
 */
class DBMCli_Config 
{
  public:

    /*! @brief constructor */
    DBMCli_Config  ( );

    /*! @brief destructor */
    ~DBMCli_Config ( );

    /*! @brief Set the database member. */
    void SetDatabase ( DBMCli_Database * p ) { this->pDatabase = p; }

    /*! @brief Get the database member. */
    DBMCli_Database & GetDatabase ( ) { return * this->pDatabase; }

    /*! @brief Read a value */
    SAPDB_Bool GetValue 
      ( const DBMCli_String        & sName,
              DBMCli_String        & sValue,
              Msg_List & oMsgList);

    /*! @brief Read a value in raw format */
    SAPDB_Bool GetRawValue 
      ( const DBMCli_String        & sName,
              DBMCli_String        & sValue,
              Msg_List & oMsgList);

    /*! @brief Write a value */
    SAPDB_Bool PutValue 
      ( const DBMCli_String        & sName,
        const DBMCli_String        & sValue,
              Msg_List & oMsgList);

    /*! @brief Write a value in raw format */
    SAPDB_Bool PutRawValue 
      ( const DBMCli_String        & sName,
        const DBMCli_String        & sValue,
              Msg_List & oMsgList);

  private:
    /*! @brief database member object */
    DBMCli_Database * pDatabase;

    /*! @brief Read a value in requested format */
    SAPDB_Bool GetValue 
          ( const DBMCli_String        & sName,
                  DBMCli_String        & sValue,
                  Msg_List & oMsgList,
                  SAPDB_Bool             bRaw);

    /*! @brief Write a value in requested format */
    SAPDB_Bool PutValue 
          ( const DBMCli_String        & sName,
            const DBMCli_String        & sValue,
                  Msg_List & oMsgList,
                  SAPDB_Bool             bRaw);

}; // end DBMCli_Config

#endif // _DBMCli_Config_HPP_

