/*!
    @file           DBMCli_EventHandlerMainWinLogger.cpp
    @author         MarcW
    @brief          logging kernel events in Windows Event Log

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
#include "ToolsCommon/Tools_EventLogFile.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "DBM/Cli/Eventing/Handler/DBMCliEvtHndl_Base.hpp"
#include "DBM/Cli/Eventing/Common/DBMCliEvtCm_Branding.hpp"
#include "DBM/Cli/DBMCli_Logo.hpp"

/*!
    @brief class for displaying events

    This event handler does nothing but write all event attributes of the events,
    that is receives to the event log
*/
class DBMCliEvtHndl_WinLogEvent: public DBMCliEvtHndl_Base {
    public:
        /*!
            @brief constructor

            Takes a command line and a mesage list and just forwards this to
            DBMCli_EventHandler.
            @see DBMCli_EventHandler
        */
        DBMCliEvtHndl_WinLogEvent
                (int argc, char * argv[], Msg_List& oMessageList)
            : logger(NULL),
              DBMCliEvtHndl_Base(argc, argv, oMessageList)
        {
            DBMCli_String sName, sDesc;
            //source
            sName = "source";
            sDesc = "event source for windows event log";
            Tools_Parameter pSource(sName, sDesc, false);
            Tools_ParameterParser parser;
            parser.addFormalParameter(pSource);
            parser.setActualParameters( argc, argv );
            DBMCli_String source(pSource.getValue());

            Tools_DynamicUTF8String header1((const char*) source);
            Tools_DynamicUTF8String header2("Event received");
            Tools_DynamicUTF8String sSource((const char*) source);
            logger = new Tools_EventLogFile(
                header1,
                header2,
                sSource); 
        }

        /*! @brief implementation how to handle the event */
        void HandleEvent() {
            if( pEvent == NULL || !logger->checkFile())
                return;

            char text[1024];
            Tools_EventLogFile::EntryType etype(Tools_EventLogFile::InfoEntry);

            // special severity for WARNING and ERROR events
            if( pEvent->GetName() == "WARNING" ) {
                etype = Tools_EventLogFile::WarningEntry;
            }
            else if( pEvent->GetName() == "ERROR" ) {
                etype = Tools_EventLogFile::ErrorEntry;
            }
            // special severity for dispatcher generated events
            else if( pEvent->GetName().GetLength() >= 9 && pEvent->GetName().Left(9) == DBMCli_String("DISPWARN:") ) {
                etype = Tools_EventLogFile::WarningEntry;
            }
            else if( pEvent->GetName().GetLength() >= 8 && pEvent->GetName().Left(8) == DBMCli_String("DISPERR:") ) {
                etype = Tools_EventLogFile::ErrorEntry;
            }

            sprintf(text,
                    "Name: %s\r\nPriority: %d\r\nDate: %s\r\nTime: %s\r\nValue1: %d\r\nValue2: %d\r\nText: %s\r\nCount: %d",
                    (const char *) pEvent->GetName(),
                    (int) pEvent->GetPriority(),
                    (const char *) pEvent->GetDate().Get(DBMCLI_DT_NUM_DATE_FMT ),
                    (const char *) pEvent->GetTime().Get(DBMCLI_DT_NUM_TIME_FMT ),
                    (int) pEvent->GetValue1(),
                    (int) pEvent->GetValue2(),
                    (const char *) pEvent->GetText(),
                    (int) pEvent->GetCount());
            logger->writeRawText(Tools_DynamicUTF8String(text), etype);
        };

        ~DBMCliEvtHndl_WinLogEvent() {
            delete logger;
        }

    private:
        Tools_EventLogFile* logger;
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
    oLogo.PrintLogo(DBMCliEvtCm_Branding::sProductName + ", Log Event Data to Windows Event Log", true);
    oLogo.PrintVersion(DBMCliEvtCm_Branding::sVersionLabel, true);

    Msg_List   oMessageList;
    Msg_List* pMsg(&oMessageList);

    Tools_ParameterParser parserRegister;
    Tools_ParameterParser parserHandleEvt;
    DBMCli_String sName, sDesc;
    // check if user wants to register a dll
    sName = "source";
    sDesc = "event source for windows event log";
    Tools_Parameter pSource(sName, sDesc, true);
    parserRegister.addFormalParameter(pSource);
    sName = "libpath";
    sDesc = "path to resource library";
    Tools_Parameter pPath(sName, sDesc, true);
    parserRegister.addFormalParameter(pPath);
    parserRegister.setActualParameters( argc, argv );
    if( parserRegister.isParameterlistValid() ) {
        // parameters are okay to register the dll
        Tools_EventLogFile elf(pSource.getValue(),pPath.getValue());
        printf("library registered for eventing\n");
        exit(0);
    }
    else {
        sName = "evt";
        sDesc = "serialized event";
        Tools_Parameter hEvt(sName, sDesc, true);
        parserHandleEvt.addFormalParameter(hEvt);
        sName = "source";
        sDesc = "event source for windows event log";
        Tools_Parameter hSource(sName, sDesc, true);
        parserHandleEvt.addFormalParameter(hSource);
        parserHandleEvt.setActualParameters( argc, argv );
        if( parserHandleEvt.isParameterlistValid() ) {
            // create the event handler class
            DBMCliEvtHndl_WinLogEvent* pEventHandler =
                new DBMCliEvtHndl_WinLogEvent(argc, argv, oMessageList);

            if( pEventHandler != NULL ) {
                // handle the event
                pEventHandler->HandleEvent();
                delete pEventHandler;
            }
            exit(0);
        }
        else {
            // none of the two lists is valid...
            parserHandleEvt.printUsage(stderr);
            parserRegister.printUsage(stderr);
        }
    }
}

