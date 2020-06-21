/*!
    @file           DBMCliEvtHndl_Base.cpp
    @author         MarcW
    @brief          Event Handler - Implementation

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
#include "Messages/Msg_List.hpp"
#include "ToolsCommon/Tools_ParameterParser.hpp"
#include "DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp"
#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Event.hpp"

DBMCliEvtHndl_Base::DBMCliEvtHndl_Base(int argc, char* argv[], Msg_List& aMessageList)
        :pDatabase(NULL),
         pEvent(NULL),
         eventRec(false),
         pConnectParameters(NULL),
         m_oMessageList(aMessageList)
{
  // check if there is a serialized event on the command line  
  Tools_ParameterParser* pEventParser  = new Tools_ParameterParser();
  
  DBMCli_String sName, sDesc;
  sName = "evt";
  sDesc = "event     (serialized kernel event)";
  Tools_Parameter event(sName, sDesc);

  pEventParser->addFormalParameter(event);
  pEventParser->setActualParameters( argc, argv );
 
  if( pEventParser->isParameterlistValid() ) {
        DBMCli_String evtData( event.getValue() );
        // store event (now available for subclasses
        pEvent = new DBMCliEvtCm_Event(evtData);
        eventRec = true;
  }
  delete pEventParser;

  // now check if there is DB connect data
  //node
  sName = "n";
  sDesc = "node";
  Tools_Parameter node(sName, sDesc, false);
  //dbname
  sName = "d";
  Tools_Parameter db(sName);
  // userpw
  sName = "u";
  Tools_Parameter userpw(sName);

  pConnectParameters  = new Tools_ParameterParser();
  pConnectParameters->addFormalParameter(node);
  pConnectParameters->addFormalParameter(db);
  pConnectParameters->addFormalParameter(userpw);

  pConnectParameters->setActualParameters( argc, argv );

  if( pConnectParameters->isParameterlistValid() ) {
      m_oMessageList.ClearMessageList();
      pDatabase = new DBMCli_Database(node.getValue(),
                                      db.getValue(),
                                      userpw.getValue(),
	                                  m_oMessageList,
                                      false);
        
    if( !m_oMessageList.IsEmpty() ) {
        Msg_List* pMsg(&m_oMessageList);
        // this output can only be seen while testing; when called from dispatcher,
        // stderr is not visible anywhere...
        fprintf(stderr, "could not create database object!\nreason:\n");      
        while( pMsg != NULL ) {
            fprintf(stderr, "%s (errorcode %d)\n", pMsg->Message(), pMsg->ID());
            pMsg = pMsg->NextMessage();
        }
        delete pDatabase;
        pDatabase = NULL;
    }
  } 
}

DBMCliEvtHndl_Base::~DBMCliEvtHndl_Base() {
    if( pDatabase != NULL ) {
        pDatabase->Disconnect();
        delete pDatabase;
    } 
    if( pEvent != NULL )
        delete pEvent;
    if( pConnectParameters != NULL )
        delete pConnectParameters;
}
