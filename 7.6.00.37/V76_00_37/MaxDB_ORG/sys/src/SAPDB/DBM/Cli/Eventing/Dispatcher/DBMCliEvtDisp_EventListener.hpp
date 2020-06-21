/*!
  @file           DBMCliEvtDisp_EventListener.hpp
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

#ifndef DBMCliEvtDisp_EventListener_HPP
#define DBMCliEvtDisp_EventListener_HPP

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "DBM/Cli/DBMCli_Database.hpp"
#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"

/*!
  @brief     Event Listener
 */
class DBMCliEvtDisp_EventListener : public DBMCli_Database
{
public:

  /*!
    @brief   Constructor 
    Constructor to create a the object
    @param sServer   [IN] name of server node
    @param sDatabase [IN] name of database instance
    @param sUserPwd  [IN] user,pwd
  */
  DBMCliEvtDisp_EventListener
    ( const DBMCli_String  & sServer,
      const DBMCli_String  & sDatabase,
      const DBMCli_String  & sUserPwd,
      Msg_List & oMessageList);

  /*! @brief Returns the last message */
  const Msg_List & LastMessage () {return m_oMessageList;}

  /*! @brief Runs the event listen loop */
  virtual bool Run ();
 
  /*! @brief Handles one event */
  virtual bool EventProc (const DBMCliEvtCm_Event & oEvent);

protected:
  /*! @brief Returns the message list object */
  Msg_List & GetMsgObj () {return m_oMessageList;}

private:
  /*! @brief Messagelist object */
  Msg_List   m_oMessageList;

}; // end DBMCli_EventListener

#endif //DBMCliEvtDisp_EventListener_HPP
