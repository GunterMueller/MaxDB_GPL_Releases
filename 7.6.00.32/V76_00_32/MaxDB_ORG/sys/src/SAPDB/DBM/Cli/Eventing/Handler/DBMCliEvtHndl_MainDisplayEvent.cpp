/*!
    @file           DBMCliEvtHndl_MainDisplayEvent.cpp
    @author         MarcW
    @brief          Main program for DBMCli display event handler

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
#include <signal.h>
#include <stdio.h>

#include "ToolsCommon/Tools_ParameterParser.hpp"
#include "ToolsCommon/Tools_PipeCall.hpp"
#include "DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp"
#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Branding.hpp"
#include "DBM/Cli/DBMCli_Logo.hpp"

/*!
    @brief class for displaying events

    This event handler does nothing but write all event attributes of the events,
    that is receives to stdout.
*/
class DBMCliEvtHndl_DisplayEvent: public DBMCliEvtHndl_Base {
    public:
        /*!
            @brief constructor

            Takes a command line and a mesage list and just forwards this to
            DBMCli_EventHandler.
            @see DBMCli_EventHandler
        */
        DBMCliEvtHndl_DisplayEvent
                (int argc, char * argv[], Msg_List& oMessageList)
            :DBMCliEvtHndl_Base(argc, argv, oMessageList)
        {
        }

        /*! @brief implementation how to handle the event */
        void HandleEvent() {
            if( pEvent == NULL )
                return;
            fprintf(stdout, "Event received:\n", (const char *) pEvent->serialize());
            fprintf(stdout, "Name        : %s\n", (const char *) pEvent->GetName());
            fprintf(stdout, "Priority    : %d\n", (int)          pEvent->GetPriority());
            fprintf(stdout, "Date        : %s\n", (const char *) pEvent->GetDate().Get(DBMCLI_DT_NUM_DATE_FMT ));
            fprintf(stdout, "Time        : %s\n", (const char *) pEvent->GetTime().Get(DBMCLI_DT_NUM_TIME_FMT ));
            fprintf(stdout, "Value1      : %d\n", (int)          pEvent->GetValue1());
            fprintf(stdout, "Value2      : %d\n", (int)          pEvent->GetValue2());
            fprintf(stdout, "Text        : %s\n", (const char *) pEvent->GetText());
            fprintf(stdout, "Count       : %d\n", (int)          pEvent->GetCount());
            fprintf(stdout, "\n");
        };
};

/*!
    @brief main function

    This program is normally called from the dispatcher. At least an event should be on the
    command line (with -evt, -d, -u, -n). Reads arguments from command line
    and initializes the event handling class.
*/
int main (int argcIn, char* argvIn[])
{
    int        argc            = 0; 
    char **    argv            = NULL;
    int nRC = Tools_PipeCall::ReadArguments(
            argcIn,
            argvIn,
            argc,
            argv);

    if( nRC != 0 ) {
        fprintf(stderr, "%s: could not read arguments from commandline. reason: %s\n",
            argvIn[0],
            Tools_PipeCall::ErrorReason());
        fflush(stderr);
        exit(1);
    }

    // print a logo
    DBMCli_Logo oLogo;
    oLogo.PrintLogo(DBMCliEvtCm_Branding::sProductName + ", Display event data", true);
    oLogo.PrintVersion(DBMCliEvtCm_Branding::sVersionLabel, true);

    Msg_List   oMessageList;
    Msg_List* pMsg(&oMessageList);

    // create the event handler class
    DBMCliEvtHndl_DisplayEvent* pEventHandler = new DBMCliEvtHndl_DisplayEvent(argc, argv, oMessageList);

    if( pEventHandler != NULL ) {
        // handle the event
        pEventHandler->HandleEvent();
        delete pEventHandler;
    }
}

