/*
    @file           Tools_EventLogFile.cpp
    @author         MarcW
    @brief          logging into Windows Event Log - Implementation

\if EMIT_LICENCE

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


\endif
*/

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include <windows.h>
#include <TCHAR.h>
#include <stdio.h>

#include "heo02.h"
#include "ToolsCommon/Tools_EventLogFile.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"
#include "ToolsCommon/Tools_WinEvtLog.h"

// implementation of Tools_EventLogFile
const Tools_DynamicUTF8String Tools_EventLogFile::sRegistryRoot
        ("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");

Tools_EventLogFile::Tools_EventLogFile(
        const Tools_DynamicUTF8String& aHeaderText1,
        const Tools_DynamicUTF8String& aHeaderText2,
        const Tools_DynamicUTF8String& aSourceName,
        const Tools_DynamicUTF8String& aResourceLibrary,
        Msg_List* EvtList)
        : sHeaderText1(aHeaderText1),
          sHeaderText2(aHeaderText2),
          sSourceName(aSourceName),
          hEventLog(NULL),
          tin105_LogFile(LOG_ALL_MIN105)
{
    // headers should appear in separate lines
    sHeaderText1 += "\r\n";
    sHeaderText2 += "\r\n\r\n";
    modifyRegistry(aResourceLibrary);

    if( bConnected && EvtList != NULL ) {
        // write events if we get some
        writeEvents(*EvtList);
    }
}

Tools_EventLogFile::Tools_EventLogFile(
        const int nEventsToLog,
        const Tools_DynamicUTF8String& aHeaderText1,
        const Tools_DynamicUTF8String& aHeaderText2,
        const Tools_DynamicUTF8String& aSourceName,
        const Tools_DynamicUTF8String& aResourceLibrary,
        Msg_List* EvtList)
        : sHeaderText1(aHeaderText1),
          sHeaderText2(aHeaderText2),
          sSourceName(aSourceName),
          hEventLog(NULL),
          tin105_LogFile(nEventsToLog)
{
    // headers should appear in separate lines
    sHeaderText1 += "\r\n";
    sHeaderText2 += "\r\n\r\n";
    modifyRegistry(aResourceLibrary);

    if( bConnected && EvtList != NULL ) {
        // write events if we get some
        writeEvents(*EvtList);
    }
}

Tools_EventLogFile::Tools_EventLogFile(
        const Tools_DynamicUTF8String& aSourceName,
        const Tools_DynamicUTF8String& aResourceLibrary)
        : sHeaderText1(""),
          sHeaderText2(""),
          sSourceName(aSourceName),
          hEventLog(NULL),
          bConnected(false),
          tin105_LogFile(LOG_ALL_MIN105)
{
    modifyRegistry(aResourceLibrary);
}


bool Tools_EventLogFile::modifyRegistry(
        const Tools_DynamicUTF8String& aResourceLibrary) {

    Tools_DynamicUTF8String temp;

    if( !aResourceLibrary.Empty() ) {
        // if the subtree, that we create here, exists
        // already, the result is the same.
        // add source name as a subkey under the Application 
        // key in the EventLog registry key. 
        HKEY hNewRegNode;
        Tools_DynamicUTF8String sNewRegNode(sRegistryRoot);
        sNewRegNode += sSourceName;
        bConnected = (ERROR_SUCCESS ==
                    RegCreateKey(
                        HKEY_LOCAL_MACHINE,
                        sNewRegNode.CharPtr(),
                        &hNewRegNode));

        if( !bConnected )
            return false;

        // add subkey with path to resource library
        temp = aResourceLibrary;
        bConnected = (ERROR_SUCCESS ==
                    RegSetValueEx(
                        hNewRegNode,
                        "EventMessageFile",
                        0,
                        REG_EXPAND_SZ,
                        (LPBYTE)temp.CharPtr(),
                        temp.Length() + 1));

        if( !bConnected )
            return false;
        
        // add subkey with supported message types
        DWORD dwData = STATUS_SEVERITY_INFORMATIONAL |
                    STATUS_SEVERITY_WARNING       |
                    STATUS_SEVERITY_ERROR;

        bConnected = (ERROR_SUCCESS ==
                    RegSetValueEx(
                        hNewRegNode,
                        "TypesSupported",
                        0,
                        REG_DWORD,
                        (LPBYTE) &dwData,
                        sizeof(DWORD)));
    }

    temp = sSourceName;
    hEventLog = RegisterEventSource(
                     NULL,
                     temp.CharPtr());
    
    bConnected = (hEventLog != NULL);

    return bConnected;
}

bool Tools_EventLogFile::writeRawText(const Tools_DynamicUTF8String& text, EntryType entryType ) {

    if( !bConnected )
        return false;

    DWORD msgTemplate;
    WORD eventLogEntryType;

    switch( entryType ) {
        case ErrorEntry:
            msgTemplate = MSG_EVTHNDL_ERROR_TEMPLATE;
            eventLogEntryType = EVENTLOG_ERROR_TYPE;
            break;
        case InfoEntry:
            msgTemplate = MSG_EVTHNDL_INFO_TEMPLATE;
            eventLogEntryType = EVENTLOG_INFORMATION_TYPE;
            break;
        default:
            msgTemplate = MSG_EVTHNDL_WARNING_TEMPLATE;
            eventLogEntryType = EVENTLOG_WARNING_TYPE;
            break;
    }

    char szText[1024];
    Tools_DynamicUTF8String temp(text);
    SAPDB_strcpy(szText, temp.CharPtr());

    static LPCTSTR strings[3];
    strings[0] = sHeaderText1.CharPtr();
    strings[1] = sHeaderText2.CharPtr();
    strings[2] = szText;

    return 0!=ReportEvent(hEventLog, // event log handle 
              eventLogEntryType,     // event type 
              0,                     // no category 
              msgTemplate,           // event identifier 
              NULL,
              3,                     // 3 substitution strings 
              0,
              strings,               // pointer to string array 
              NULL);

}

Tools_EventLogFile::~Tools_EventLogFile() {
    if( bConnected )
        DeregisterEventSource(hEventLog);
}
        
bool Tools_EventLogFile::writeOneEventList
      ( const Msg_List & EvtLst )
{

    const Msg_List * pNextEvt = &EvtLst;
    _TCHAR                   szBuffer[1024];
    bool                     bStatus(true);

    if( bConnected ) {
        while (pNextEvt != NULL) {
            //write to event log here
            sprintf(szBuffer, "MsgID: %d\nLabel: %s\nMessage: %s",
                    (int) pNextEvt->ID(),
                    pNextEvt->Component(),
                    pNextEvt->Message());
            EntryType entryType;
            switch( pNextEvt->Type() ) {
                case Msg_List::Error:
                    entryType = ErrorEntry;
                    break;
                case Msg_List::Info:
                    entryType = InfoEntry;
                    break;
                default:
                    entryType = WarningEntry;
                    break;
            }

            Tools_DynamicUTF8String temp(szBuffer);
            bStatus = bStatus && writeRawText(temp, entryType);
            pNextEvt  = pNextEvt->NextMessage();
        }
    }    
    return bStatus;
}