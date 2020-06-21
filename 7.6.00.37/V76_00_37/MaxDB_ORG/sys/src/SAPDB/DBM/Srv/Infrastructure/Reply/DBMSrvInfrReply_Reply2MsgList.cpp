/*!
    \file    DBMSrvInfrReply_Reply2MsgList.cpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   declaration of a class converting a DBM Server reply into a DBM
             Server message list

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#include "hcn36.h"

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_SQLError.hpp"

bool DBMSrvInfrReply_Reply2MsgList::transform(DBMSrv_Reply & reply, DBMSrvMsg_Error & errorList)
{
    bool rc=false;
    char * buffer=reply.giveData();

    errorList.ClearMessageList(); //a reply is seen as complete answer and therefore a complete message list in itself

    DBMSrv_Reply::Position pError=reply.getStartOfNextLine(buffer); //ignore first line with "OK" and "ERR"

    while(0!=pError && '\0'!=pError[0])
    {
        DBMSrv_Reply::Position eol=reply.getEndOfLine(pError);
        // try to find a DBM Server error number in the line
        DBMSrv_Reply::Position pHelp=cn36_StrNStr(pError, eol-pError, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
        int errorNumber=0;

        //check, if the line is a DBM Server error
        if(0!=pHelp &&
            pHelp!=pError &&
            '-'==pError[0] &&
            cn36_StrNConsistsOfDigits(pError+1, pHelp-pError-1) &&
            cn36_StrNToInt(errorNumber, pError, pHelp-pError) &&
            -25000<errorNumber &&
            -24000>=errorNumber )
        {
            //search ':', that separates the DBM Server error name from the error text
            DBMSrv_Reply::Position startText=cn36_StrNChr(pHelp, ':', eol-pHelp);

            //if there is no ':' use the whole text following the starting "<number>," 
            if(0==startText)
                startText=pHelp+strlen(DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
            else
            {
                startText++;                                                   //ignore the ':'
                startText=cn36_FirstNonWhiteSpaceOf(startText, eol-startText); //ignore any spaces
            }

            if( errorNumber == DBMSrvMsg_Error::get_tcn00_Error(DBMSrvMsg_Error::DBMSrv_SQL) ) {
                // SQL error message: special processing
                bool transformSuccess(false);
                DBMSrvMsg_Error transformError;

                // next line must be the kernel information: number and text
                pError=reply.getStartOfNextLine(pError);
                if( NULL != pError && '\0' != pError[0] ) {
                    DBMSrv_Reply::Position eol=reply.getEndOfLine(pError);
                    DBMSrv_Reply::Position comma=cn36_StrNStr(pError, eol-pError, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
                    if( NULL != comma && eol > comma ) {
                        char oldEOLValue=set(reply, eol, '\0'); //terminate string
                        char oldCommaValue=set(reply, comma, '\0'); //terminate string
                        errorList.append(DBMSrvMsg_SQLError(pError, comma+1));
                        set(reply, eol, oldEOLValue); //undo manipulation
                        set(reply, comma, oldCommaValue); //undo manipulation
                        transformSuccess = true;
                    }
                }

                if( !transformSuccess ) {
                    // append the line the old fashioned way and an extra message to be defined...
                    transformError.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_REPLY_2MSG, Msg_Arg("BUFFER", reply.giveData())));
                    errorList.Overrule(transformError);
                    return false;
                }
            }
            else {
                char oldEOLValue=set(reply, eol, '\0'); //write temporary a terminating zero at the end of the line
                errorList.append(DBMSrvMsg_Error(errorNumber, startText));
                set(reply, eol, oldEOLValue); //restore original character at end of line
            }
        }
        else
        {
            char oldEOLValue=set(reply, eol, '\0'); //write temporary a terminating zero at the end of the line

            errorList.append(DBMSrvMsg_Error(SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG,	Msg_Arg("TAG", pError)));

            set(reply, eol, oldEOLValue); //restore original character at end of line
        }

        pError=reply.getStartOfNextLine(pError); // now pError points to first character in next line
    }

    //check, if constructed list errorList represents reply
    char * tempBuf=0;

    if(cn36_StrAlloc(tempBuf, reply.getCurrentLength()+1))
    {
        int tempLength=0;
        DBMSrv_Reply tempReply(tempBuf, &tempLength, reply.getCurrentLength()+1);

        tempReply.startWithMessageList(errorList);

        if(0==strcmp(tempBuf, reply.giveData()))
            rc=true;
        else
        {
            errorList.ClearMessageList();
            errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_REPLY_2MSG, Msg_Arg("BUFFER", reply.giveData())));
            errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));
        }

        cn36_StrDealloc(tempBuf);
    }
    else
        errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));

    return rc;
}

char DBMSrvInfrReply_Reply2MsgList::set(DBMSrv_Reply & reply, DBMSrv_Reply::Position position, char newValue)
{
    char rc=(*position);

    *(reply.giveData()+(position-reply.giveData()))=newValue; //this line looks so complicated, as position is a 'const char *'

    return rc;
}
