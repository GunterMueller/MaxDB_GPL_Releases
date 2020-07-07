/*!
    \file    DBMSrvMsg_Error.cpp
    \author  MarcW
    \ingroup error, warning and information message handling for DBM Server
    \brief   definition of a class for error messages

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2006 SAP AG

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

#include <stdlib.h>

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

const char * DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_ERROR      ="ERR_";
const char * DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_WARNING    ="WRN_";
const char * DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_INFORMATION="INF_";

DBMSrvMsg_Error::DBMSrvMsg_Error()
    :DBMSrvMsg_Base(),
     Msg_List()
{
}

#define DBMSRVMSG_ERROR_MSGLISTARGS \
    Msg_List::Error,\
        0==strcmp(component, SDBMSG_DBMSRV_COMPONENT)?((MessageID) (-25000 + aID)):aID,\
        component,\
        file,\
        line,\
        aText,\
        argCheck,\
        messageVersion,\
        arg0,\
        arg1,\
        arg2,\
        arg3,\
        arg4,\
        arg5,\
        arg6,\
        arg7,\
        arg8

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has1Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IOptArg const * arg0,
    Msg_IOptArg const * arg1,
    Msg_IOptArg const * arg2,
    Msg_IOptArg const * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has2Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IOptArg const * arg1,
    Msg_IOptArg const * arg2,
    Msg_IOptArg const * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has3Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IOptArg const * arg2,
    Msg_IOptArg const * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has4Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IOptArg const * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has5Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has6Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IArg const    * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has7Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IArg const    * arg4,
    Msg_IArg const    * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has8Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IArg const    * arg4,
    Msg_IArg const    * arg5,
    Msg_IArg const    * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has9Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IArg const    * arg4,
    Msg_IArg const    * arg5,
    Msg_IArg const    * arg6,
    Msg_IArg const    * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has10Args     * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IArg const    * arg1,
    Msg_IArg const    * arg2,
    Msg_IArg const    * arg3,
    Msg_IArg const    * arg4,
    Msg_IArg const    * arg5,
    Msg_IArg const    * arg6,
    Msg_IArg const    * arg7,
    Msg_IArg const    * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS)
{
}

DBMSrvMsg_Error::DBMSrvMsg_Error(
    const Msg_List    & subMessage,
    int                 aID,
    const char        * component,
    const char        * file,
    int                 line,
    const char        * aText,
    Msg_Has2Args      * argCheck,
    const Msg_IArg    * messageVersion,
    Msg_IArg const    * arg0,
    Msg_IOptArg const * arg1,
    Msg_IOptArg const * arg2,
    Msg_IOptArg const * arg3,
    Msg_IOptArg const * arg4,
    Msg_IOptArg const * arg5,
    Msg_IOptArg const * arg6,
    Msg_IOptArg const * arg7,
    Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(subMessage)
{
    this->MarkAsSubMessages();
    this->Overrule(Msg_List(DBMSRVMSG_ERROR_MSGLISTARGS));
}


DBMSrvMsg_Error::DBMSrvMsg_Error(
    int          classicErrorNumber,
    const char * classicErrorText)
  : DBMSrvMsg_Base(),
    Msg_List(
        Msg_List::Error,
        (MessageID) classicErrorNumber,
        SDBMSG_DBMSRV_COMPONENT,
        __FILE__,  //we never use it ...
        __LINE__,  //we never use it ...
        classicErrorText)
{
}

void DBMSrvMsg_Error::append(const DBMSrvMsg_Error & other)
{
    DBMSrvMsg_Error temp=other;

    temp.Overrule(*this);
    *this=temp;
}

bool DBMSrvMsg_Error::isDBMSrvMessage(const Msg_List & message)
{
    int errorIDAsInt=message.ID()+25000;

    return (0==strcmp(SDBMSG_DBMSRV_COMPONENT, message.Component()) && 0<errorIDAsInt && errorIDAsInt<LAST_ID);
}

const char * DBMSrvMsg_Error::getErrorName(const Msg_List & message)
{
    if(isDBMSrvMessage(message))
        return DBMSrvMsg_ErrorTextArray::getErrorText(DBMSrvMsg_Error::ErrorID(message.ID()+25000)).getName(); //the cast to ...::ErrorID should work always, as is guaranteed by isDBMSrvMessage()
    else
        return 0;
}

bool operator==(const DBMSrvMsg_Error::ErrorID& aErrid, const Msg_List  & aMsgList)
{
    return (aMsgList.ID() + 25000) == int(aErrid);
}

bool operator==(const DBMSrvMsg_Error::ErrorID & aErrid, const Msg_List * aMsgList)
{
    return (aMsgList->ID() + 25000) == int(aErrid);
}

bool operator!=(const DBMSrvMsg_Error::ErrorID & aErrid, const Msg_List * aMsgList)
{
    return !(aErrid==aMsgList);
}

void DBMSrvMsg_Error::dumpToStdOut() {
    SAPDB_UInt4 bufferSize(0);
    char* buffer(NULL);
    Msg_List errOut;
    Msg_List* current(this);
    printf("++++ begin msg dump\n");
    buffer = new char[10000];
    current->XML_Output(10000, buffer, bufferSize, errOut);
    printf("%s\n", buffer);
    delete[] buffer;
/*    
    while( current != NULL ) {
        current->MessageWithInsertedArguments(bufferSize, buffer, bufferSize, true);
        //current->XML_Output(bufferSize, buffer, bufferSize, errOut);
        buffer = new char[bufferSize+1];
        current->MessageWithInsertedArguments(bufferSize, buffer, bufferSize, true);
        //current->XML_Output(bufferSize, buffer, bufferSize, errOut);
        printf("%s\n", buffer);
        delete[] buffer;
        bufferSize = 0;
        buffer = NULL;
        //printf("++ %s\n", current->Message());
        //printf("++++ one done , %d %d %d\n", current->NumOfMessages(), current->NumOfSubMessages(), current->NumOfDetails());
        current = current->NextMessage();
    }
*/
    printf("++++ end msg dump\n");
}

/* \TODO activate when and if Microsoft can overload operators correctly and tcn00_Error is still in use
bool operator==(const DBMSrvMsg_Error::ErrorID aErrid, const SAPDB_UInt4 aError)
{
    return (aError + 25000 - 158) == int(aErrid);
}*/

bool DBMSrvMsg_ErrorTextArray::m_IsInitialized(false);
DBMSrvMsg_ErrorText DBMSrvMsg_ErrorTextArray::m_ErrorTextArray[DBMSrvMsg_Error::LAST_ID+1];

const DBMSrvMsg_ErrorText& DBMSrvMsg_ErrorTextArray::getErrorText(
    const DBMSrvMsg_Error::ErrorID aID) {
    
    if( !m_IsInitialized ) {
        // initialize message array
        initArray();
        m_IsInitialized = true;
    }

    if(int(aID)==int(m_ErrorTextArray[aID].getErrorID()))
        return m_ErrorTextArray[aID];
    else
    {
        //if messages were not inserted in the right order (enum order) into the error text array, we must search for them
        int i;

        for(i=0; i<DBMSrvMsg_Error::LAST_ID && aID!=m_ErrorTextArray[i].getErrorID(); i++);

        return m_ErrorTextArray[i]; //if nothing better is found DBMSrvMsg_Error::LAST_ID is reported
    }
}

void DBMSrvMsg_ErrorTextArray::initArray() {

    //construct local copy of the array
    DBMSrvMsg_ErrorText tempArray[]=
    {
        SDBMSG_DBMSRV_INITIALIZATIONLIST(DBMSrvMsg_ErrorText),

        DBMSrvMsg_ErrorText(DBMSrvMsg_Error::LAST_ID, "ERR: an error occured") // :)
    };

    bool tempArraysEndReached=false;
    int  lastIDInTempArray=0;
    int  i=0;

    //copy temp array into the static member, if there is place
    for(; !tempArraysEndReached && i<DBMSrvMsg_Error::LAST_ID; i++)
    {
        m_ErrorTextArray[i]=tempArray[i];

        if(int(tempArray[i].getErrorID())==int(DBMSrvMsg_Error::LAST_ID))
        {
            tempArraysEndReached=true;
            lastIDInTempArray=i;
        }
    }

    //fill up the static member with copies of (LAST_ID, "ERR: an error occured"), this ensures, that we don't crash,
    //if tempArray has less then LAST_ID elements due to an accident
    for(;i<DBMSrvMsg_Error::LAST_ID; i++)
        m_ErrorTextArray[i]=tempArray[lastIDInTempArray];

    //make sure that the last member of m_ErrorTextArray is (LAST_ID, "ERR: an error occured"), this ensures, that we have
    //no problem, if tempArray contains more than LAST_ID elements due to an accident
    m_ErrorTextArray[DBMSrvMsg_Error::LAST_ID]=DBMSrvMsg_ErrorText(DBMSrvMsg_Error::LAST_ID, "ERR: an error occured");
}

const char * DBMSrvMsg_Error::getTagValueAsString(const char * szTag) const
{
  SAPDB_UTF8 const * pReturn = ArgValue(szTag);
  return (pReturn == NULL) ? "" : (const char *) pReturn;
} // end DBMSrvMsg_Error::getTagValueAsString

int DBMSrvMsg_Error::getTagValueAsInteger(const char * szTag)const
{
  return atoi(getTagValueAsString(szTag));
} // end DBMSrvMsg_Error::getTagValueAsInteger

void DBMSrvMsg_Error::appendTransportMessage(const char* errID, const char* errText) {
    char textBuffer[1024];
    
    // write error ID and comma to buffer
    strncpy(textBuffer, errID, 1024);
    textBuffer[1024-1] = '\0';
    if( strlen(textBuffer) + strlen(DBMSrv_Reply::STRING_SEPARATOR_TOKEN) < 1024 ) {
        strcat(textBuffer, DBMSrv_Reply::STRING_SEPARATOR_TOKEN); // buffer is still 0-terminated
        int spaceLeft(1024-1-((int)strlen(textBuffer)));
        if( spaceLeft > 0 ) {
            strncat( textBuffer, errText, spaceLeft);
            textBuffer[1024-1] = '\0';
        }
    }
    
    DBMSrvMsg_Error transport(
            SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG,
            Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, textBuffer));

    transport.Overrule(*this);
    this->Assign(transport);
}

