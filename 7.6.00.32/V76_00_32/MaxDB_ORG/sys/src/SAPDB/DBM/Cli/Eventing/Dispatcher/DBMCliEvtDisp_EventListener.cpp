/*!
  @file           DBMCliEvtDisp_EventListener.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          Listener for database kernel events

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
#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_EventListener.hpp"

/*! @brief Constructor  */
DBMCliEvtDisp_EventListener::DBMCliEvtDisp_EventListener
    ( const DBMCli_String  & sServer,
      const DBMCli_String  & sDatabase,
      const DBMCli_String  & sUserPwd,
      Msg_List & oMessageList)
    : DBMCli_Database(sServer, sDatabase, sUserPwd, oMessageList)
{

} // end DBMCli_EventListener::DBMCli_EventListener

/*! @brief public member  */
bool DBMCliEvtDisp_EventListener::Run
     ( )
{
  m_oMessageList.ClearMessageList();

  while (DBMCli_Session::Execute("event_wait", m_oMessageList) ) {
    DBMCli_Result & oResult = DBMCli_Session::GetResult();
    if (!oResult.Check(m_oMessageList)) {
      // generate error and return
      return false;
    } // end if
    DBMCliEvtCm_Event oEvent(oResult);

    if (!EventProc(oEvent)) {
      return true;
    } // end if

    m_oMessageList.ClearMessageList();
  } // end while

  return m_oMessageList.IsEmpty();
} // end DBMCli_EventListener::Run

/*! @brief public member  */
bool DBMCliEvtDisp_EventListener::EventProc
    ( const DBMCliEvtCm_Event & oEvent )
{
  return true;
} // end DBMCli_EventListener::EventProc
