/*!
  @file           DBMCliEvtDisp_MsgDumper.hpp
  @author         MarcW
  @brief          dump a Msg_List to a FILE*

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

*/

#include "DBM/Cli/Eventing/Dispatcher/DBMCliEvtDisp_MsgDumper.hpp"
#include "DBM/Logging/DBMLog_Logger.hpp"

void DBMCliEvtDisp_MsgDumper::dump(const Msg_List& aDumpee, FILE* aOutFile) {

    if( aDumpee.IsEmpty() )
        return;

    SAPDB_UInt4 space(0);
    SAPDB_UInt4 dummy(0);
    const Msg_List* aMessage(&aDumpee);

    while( aMessage != NULL ) {
        // to find out how much space is required
        aMessage->MessageWithInsertedArguments(0, NULL, space, false); // includes terminator

        char* messageWithInsertedTags=new char[++space];
        // get the message
        aMessage->MessageWithInsertedArguments(
            space,
            messageWithInsertedTags,
            dummy,
            false);

        char formatString[512];
        sprintf(formatString, "%%s\n"); // default format

        if( 0 == SAPDB_strcmp(aMessage->Component(), SDBMSG_DBMED_COMPONENT ) ) {
            // this is message from our own component, maybe we have a special formatting rule
            switch(aMessage->ID()) {
                case DBMEd_CMDLIST_REPLYLINE_CONDITION:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                case DBMEd_CMDLIST_REPLYLINE_COMMAND:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                case DBMEd_CMDSTATE_REPLYLINE_USERID:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                case DBMEd_CMDSTATE_REPLYLINE_HOST:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                case DBMEd_CMDSTATE_REPLYLINE_CONFIG:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                case DBMEd_CMDSTATE_REPLYLINE_LOG:
                    sprintf(formatString, "\t%%s\n"); // indent this message
                    break;
                default:
                    // do nothing
                    break;
            }
        }

        fprintf( aOutFile, formatString, messageWithInsertedTags);
        delete[] messageWithInsertedTags;
        aMessage = aMessage->NextMessage();
    }

    // if it's no info message, we try to log it
    if( aDumpee.Type() != Msg_List::Info )
        DBMLog_Logger::getInstance().addMessageList(aDumpee);
}
