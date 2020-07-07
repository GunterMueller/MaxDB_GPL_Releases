/*!
  @file           DBMCli_Config.cpp
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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/DBMCli_State.hpp"

/*! @ brief constructor */
DBMCli_Config :: DBMCli_Config ( )
{
  this->pDatabase;
} // end DBMCli_Config :: DBMCli_Config

/*! @ brief destructor */
DBMCli_Config :: ~DBMCli_Config ( )
{
} // end DBMCli_Config :: ~DBMCli_Config

/*! @brief Read a value */
SAPDB_Bool DBMCli_Config :: GetValue 
    ( const DBMCli_String        & sName,
            DBMCli_String        & sValue,
            Msg_List & oMsgList)
{
  return GetValue ( sName, sValue, oMsgList, false);
} // DBMCli_Config :: GetValue 


/*! @brief Read a value in raw format */
SAPDB_Bool DBMCli_Config :: GetRawValue 
    ( const DBMCli_String        & sName,
            DBMCli_String        & sValue,
            Msg_List & oMsgList)
{
  return GetValue ( sName, sValue, oMsgList, true);
} // DBMCli_Config :: GetRawValue 

/*! @brief Read a value in requested format */
SAPDB_Bool DBMCli_Config :: GetValue 
    ( const DBMCli_String        & sName,
            DBMCli_String        & sValue,
            Msg_List & oMsgList,
            SAPDB_Bool             bRaw)
{
  oMsgList.ClearMessageList();

  if (this->pDatabase->Execute(DBMCli_String("dbm_configget ") + (bRaw? "-raw " : "") + sName, oMsgList)) {
    DBMCli_Result & oResult = this->pDatabase->GetResult();
    if (oResult.Check(oMsgList)) {
      // read the value
      oResult.SetPos(0);
      oResult.GetLine(sValue); // skip OK
      oResult.GetLine(sValue);
    } // end if
  } // end if

  return oMsgList.IsEmpty();
} // end DBMCli_Config :: GetValue 

/*! @brief Write a value */
SAPDB_Bool DBMCli_Config :: PutValue 
    ( const DBMCli_String        & sName,
      const DBMCli_String        & sValue,
            Msg_List & oMsgList)
{
  return PutValue ( sName, sValue, oMsgList, false);
} 

/*! @brief Write a value in raw format */
SAPDB_Bool DBMCli_Config :: PutRawValue 
    ( const DBMCli_String        & sName,
      const DBMCli_String        & sValue,
            Msg_List & oMsgList)
{
  return PutValue ( sName, sValue, oMsgList, true);
} 

/*! @brief Write a value in requested format */
SAPDB_Bool DBMCli_Config :: PutValue 
    ( const DBMCli_String        & sName,
      const DBMCli_String        & sValue,
            Msg_List & oMsgList,
            SAPDB_Bool             bRaw)
{
  oMsgList.ClearMessageList();

  this->pDatabase->Execute(DBMCli_String("dbm_configset ") + (bRaw? "-raw " : "") + sName +  " " + sValue, oMsgList);

  return oMsgList.IsEmpty();
} 

