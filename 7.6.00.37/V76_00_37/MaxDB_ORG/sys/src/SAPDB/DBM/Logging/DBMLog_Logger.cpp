/*!
    @file    DBMSrvLog_CommandLogger.cpp
    @author  MarcW
    @brief   command logger

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "DBM/Logging/DBMLog_Logger.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMLog.h"
#include "SAPDBCommon/SAPDB_string.h"

#include <stdio.h>

const char* const DBMLog_Logger::m_DefaultDBMComponent("DBM");

DBMLog_Logger DBMLog_Logger::m_Instance;

void DBMLog_Logger::setDBMComponent(const char* aDBMComponent) {
    if( m_DBMComponent != NULL )
        delete[] m_DBMComponent;

    m_DBMComponent = new char[strlen(aDBMComponent)+1];
    SAPDB_strcpy(m_DBMComponent, aDBMComponent);
}

DBMLog_Logger& DBMLog_Logger::getInstance() {
    return m_Instance;
}

DBMLog_Logger::DBMLog_Logger()
    : m_PanicFileCount(0),
      m_NormalFileCount(0),
      m_InDestructor(false) {
    m_NormalLogger = new tin100_Log();
    m_PanicLogger = new tin100_Log();
}

void DBMLog_Logger::freeResources() {
    if( m_NormalLogger != NULL ) {
        delete m_NormalLogger;
        if( !m_InDestructor )
            m_NormalLogger = new tin100_Log();
    }
    if( m_PanicLogger != NULL ) {
        delete m_PanicLogger;
        if( !m_InDestructor )
            m_PanicLogger = new tin100_Log();
    }
    
    Tools_Vector<DBMLog_LogFile*>::iterator fileIterator = m_RegisteredFiles.begin();
    for( ; fileIterator != m_RegisteredFiles.end(); ++fileIterator ) {
        if( *fileIterator != NULL ) {
            delete *fileIterator;
            *fileIterator = NULL;
        }
    }
    
    m_PanicFileCount = 0;
    m_NormalFileCount = 0;
}

DBMLog_Logger::~DBMLog_Logger() {
    m_InDestructor = true;
    freeResources();
}

void DBMLog_Logger::addFile( DBMLog_LogFile* aLogFile ) {
    if( aLogFile->isForPanic() ) {
        m_PanicLogger->addFile(aLogFile, false);
        ++m_PanicFileCount;
    }
    else {
        m_NormalLogger->addFile(aLogFile, false);
        ++m_NormalFileCount;
    }

    m_RegisteredFiles.push_back(aLogFile);
}

void DBMLog_Logger::addMessageList( const Msg_List& aMessageList) {
    if( !aMessageList.IsEmpty() ) {

        const Msg_List* aMessage(&aMessageList);
        Msg_List outList;
        outList.ClearMessageList();
        while( aMessage != NULL )
        {
            char          fixedBuffer[1025]="";
            char        * messageWithInsertedTags=0;
            SAPDB_UInt4   dummyNeededSpace=0;
            SAPDB_UInt4   space=0;

            //determine size needed for message text with inserted arguments
            aMessage->MessageWithInsertedArguments(0, 0, space, false);

            //if (*pMsgList) is a message created through DBMSrvMsg_Error, prepend the type prefix, the message name and the classic ": " in front of the message text
            //but don't do this, if the message is the special message for tunneling strings 
            const char  * errorName=DBMSrvMsg_Error::getErrorName(*aMessage);
            if(0!=errorName && DBMSrvMsg_Error::DBMSrv_INTERNAL_TRANSPORT_MSG!=aMessage)
            {
                Msg_List::MessageType   type=aMessage->Type();
                const char            * prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_INFORMATION; // "INF_"

                if(Msg_List::Error==type)
                    prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_ERROR; // "ERR_"
                else
                    if(Msg_List::Warning==type)
                        prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_WARNING; // "WRN_"
                    //else: just use the default "INF_"

                space+=SAPDB_UInt4(strlen(prefix)+strlen(errorName)+3); // 2 for ": " and 1 for MessageWithInsertedArguments, as it counts the zero byte

                messageWithInsertedTags=new char[space+1];

                if(0==messageWithInsertedTags) //if there is not enough memory, use the short stack buffer
                {
                    messageWithInsertedTags=fixedBuffer;
                    space=1024;
                }

                sprintf(messageWithInsertedTags, "%s%s: ", prefix, errorName);
            }
            else //for normal messages get just space for a copy of the message text
            {
                space++; //for MessageWithInsertedArguments, as it counts the zero byte
                messageWithInsertedTags=new char[space+1];

                if(0==messageWithInsertedTags) //if there is not enough memory, use the short stack buffer
                {
                    messageWithInsertedTags=fixedBuffer;
                    space=1024;
                }

                messageWithInsertedTags[0]='\0';
            }

            aMessage->MessageWithInsertedArguments(space-SAPDB_UInt4(strlen(messageWithInsertedTags)),
                                                   messageWithInsertedTags+strlen(messageWithInsertedTags),
                                                   dummyNeededSpace,
                                                   false);

            Msg_List currMsg(
                    aMessage->Component(),
                    aMessage->FileName(),
                    aMessage->LineNumber(),
                    aMessage->SystemRC(),
                    aMessage->Type(),
                    aMessage->ID(), // this is allowed
                    messageWithInsertedTags,
                    (SAPDB_UInt4) 0);
                
                
            currMsg.Overrule(outList);
            outList = currMsg;

            if(fixedBuffer!=messageWithInsertedTags) //if a dynamic buffer was used, free the memory
                delete [] messageWithInsertedTags;

            aMessage = aMessage->NextMessage();
        }

        if( m_NormalFileCount==0 || !m_NormalLogger->writeEntry(outList) ) {
            // we could not log the message normally
            // no need to get the message text with replaced arguments since we have no arguments...
            outList.Overrule( Msg_List(Msg_List::Warning, SDBMSG_DBMLOG_UNSAVE) );
            m_PanicLogger->writeEntry(outList);
        }
    }
}
