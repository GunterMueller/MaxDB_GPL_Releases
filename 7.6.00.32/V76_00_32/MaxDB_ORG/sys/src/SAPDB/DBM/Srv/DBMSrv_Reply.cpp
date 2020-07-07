/*! 
    \file    DBMSrv_Reply.cpp
    \author  TiloH
    \ingroup common classes for the DBMServer
    \brief   definition of a class handling the reply buffer of DBMServer
             commands

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


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <errno.h>
#include <stdio.h>

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"
#include "ToolsCommon/Tools_List.hpp"
#include "hcn36.h"


//-----------------------------------------------------------------------------
// file global variables
//-----------------------------------------------------------------------------

const size_t DummyBufferSize_DBMSrv_Reply=1204;
char         DummyBuffer_DBMSrv_Reply[DummyBufferSize_DBMSrv_Reply]; //<! a small dummy buffer, that is always available
int          DummyBufferLength;

//-----------------------------------------------------------------------------
// members of class DBMSrv_Reply
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// public static members
//-----------------------------------------------------------------------------
const char * DBMSrv_Reply::STRING_OK              = ANSWER_OK_CN00;
const char * DBMSrv_Reply::STRING_ERR             = ANSWER_ERR_CN00;
const char * DBMSrv_Reply::STRING_SEPARATOR_LINE  = LINE_SEPSTRING_CN00;
const char * DBMSrv_Reply::STRING_SEPARATOR_TOKEN = TOKEN_SEPSTRING_CN00;
const char * DBMSrv_Reply::STRING_CONTINUE        = "CONTINUE";
const char * DBMSrv_Reply::STRING_END             = "END";

//-----------------------------------------------------------------------------
// public constructor
//-----------------------------------------------------------------------------
DBMSrv_Reply::DBMSrv_Reply()
  :replyData(defaultBuffer),
   constReplyData(defaultBuffer),
   replyLength(&defaultLength),
   maximalReplyLength(1), //space for terminating zero byte
   defaultLength(0)
{
    defaultBuffer[0]='\0';
}

DBMSrv_Reply::DBMSrv_Reply(char * ReplyData,
                           int  * ReplyLength,
                           int    ReplyLenMax)
  :replyData(ReplyData),
   constReplyData(ReplyData),
   replyLength(ReplyLength),
   maximalReplyLength(ReplyLenMax),
   defaultLength(0)
{
    defaultBuffer[0]='\0';
}

DBMSrv_Reply::DBMSrv_Reply(const char * constantReplyData)
    :replyData(0),                          //by seting replyData to 0, only 'const' functions will work properly
     constReplyData(constantReplyData),
     replyLength(&defaultLength),
     maximalReplyLength(1),                 //to achieve an available space of 0 and thereby avoid any changes to the buffer
     defaultLength(0)
{
    defaultBuffer[0]='\0';
}

tcn00_Error DBMSrv_Reply::ReturnCode 
  (  ) const
{
  tcn00_Error nCode = OK_CN00;

  if (*replyLength > 0) {
    if (strncmp(replyData, STRING_ERR, strlen(STRING_ERR)) == 0) {
      const char* pError(strstr(replyData, LINE_SEPSTRING_CN00));
      const char* pHelp(NULL);
      char buf[128];
      if (pError != NULL) {
        pError = pError + strlen(LINE_SEPSTRING_CN00);
        // find the error number
        pHelp = strstr(pError, TOKEN_SEPSTRING_CN00);
        if ((pHelp != NULL) && ((pHelp-pError) < (sizeof(buf)-1))) {
          strncpy(buf, pError, (size_t) (pHelp-pError));
          buf[(size_t) (pHelp-pError)] = '\0';
          nCode = atoi(buf);
        } // end if
      } // end if
    } // end if
  } // end if
 
  return nCode;
} // end DBMSrv_Reply::ReturnCode


void DBMSrv_Reply::setTo(char * ReplyData,
                         int  * ReplyLength,
                         int    ReplyLenMax)
{
    replyData=ReplyData;
    constReplyData=ReplyData;
    replyLength=ReplyLength;
    maximalReplyLength=ReplyLenMax;
}

void DBMSrv_Reply::setToGlobalDummy()
{
    setTo(DummyBuffer_DBMSrv_Reply, &DummyBufferLength, DummyBufferSize_DBMSrv_Reply);
}

char * DBMSrv_Reply::giveData()
{
    return replyData;
}

const char * DBMSrv_Reply::giveData() const
{
    return constReplyData;
}

int * DBMSrv_Reply::giveLength()
{
    return replyLength;
}

int DBMSrv_Reply::getCurrentLength() const
{
    return *replyLength;
}

bool DBMSrv_Reply::adaptLength()
{
    *replyLength=(int)strlen(constReplyData);

    if(*replyLength >= maximalReplyLength) //at least the terminating zero is outside
        return false;
    else
        return true;
}

int DBMSrv_Reply::giveMaximalLength() const
{
    return maximalReplyLength;
}

int DBMSrv_Reply::giveAvailableSpace() const
{
    return maximalReplyLength-1-(*replyLength);
}

DBMSrv_Reply::Position DBMSrv_Reply::getEndOfFirstLine() const
{
    return getEndOfLine(constReplyData);
}

DBMSrv_Reply::Position DBMSrv_Reply::getEndOfLine(Position positionInLine) const
{
    Position end=strstr(positionInLine, DBMSrv_Reply::STRING_SEPARATOR_LINE);

    return (0==end?positionInLine+strlen(positionInLine):end);
}

DBMSrv_Reply::Position DBMSrv_Reply::getStartOfReplyText() const
{
    const char * firstLineStart=constReplyData;
    const char * firstLineEnd=getEndOfFirstLine();

    if((firstLineEnd-firstLineStart==strlen(STRING_OK) && 0==strncmp(DBMSrv_Reply::STRING_OK, firstLineStart, strlen(STRING_OK))) ||
       (firstLineEnd-firstLineStart==strlen(STRING_ERR) && 0==strncmp(DBMSrv_Reply::STRING_ERR, firstLineStart, strlen(STRING_ERR))))
    {
        return getStartOfNextLine(constReplyData);
    }
    else
        return constReplyData;
}

DBMSrv_Reply::Position DBMSrv_Reply::getStartOfNextLine(Position positionInLine) const
{
    Position pos=getEndOfLine(positionInLine);

    if('\0'!=(*pos))    //if there is a line break
        pos+=strlen(DBMSrv_Reply::STRING_SEPARATOR_LINE);
    
    return pos;
}

void DBMSrv_Reply::getStartAndEndOfLine(unsigned int lineNumber, Position & start, Position & end) const
{
    start=constReplyData; //start at the beginning of the buffer

    while(0<lineNumber) //skip all previous lines
    {
        start=getStartOfNextLine(start);
        lineNumber--;
    }

    end=getEndOfLine(start);    //determine end of current line
}

void DBMSrv_Reply::getStartAndEndOfNextLine(Position positionInLine, Position & start, Position & end) const
{
    start=getStartOfNextLine(positionInLine);
    end=getEndOfLine(start);
}

void DBMSrv_Reply::startWithOK()
{
    startWithLine(DBMSrv_Reply::STRING_OK);
}

void DBMSrv_Reply::startWithOKIfRCIsOk(tcn00_Error rc)
{
    if(OK_CN00==rc)
        startWithLine(DBMSrv_Reply::STRING_OK);
}

tcn00_Error DBMSrv_Reply::startWithError(const tcn00_Error Error,
                                         int               ExtError,
                                         const char *      ExtErrorText,
                                         size_t            ExtErrorTextLength,
                                         int               ExtError2,
                                         const char *      ExtError2Text)
{
    char NumberBuf[50];
    int i=0;

    //search the error in the global error array
    struct
    {
        tcn00_Error   Value;
        const char  * Text;
    }errorArray[]=ERROR_TO_TEXT_CN00;

    while((ERR_UNKNOWN_CN00 != errorArray[i].Value) && (Error != errorArray[i].Value))
        i++;

    //write the error text to the reply buffer
    startWithLine((Error == OK_CN00) ? DBMSrv_Reply::STRING_OK : DBMSrv_Reply::STRING_ERR);
    sprintf(NumberBuf, "%d", (int)Error); //50 chars should be sufficient
    appendStrings(NumberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
    appendLine(errorArray[i].Text);

    if(0!=ExtErrorText && 0<ExtErrorTextLength)
    {
        sprintf(NumberBuf, "%d", (int)ExtError); //50 chars should be sufficient
        appendStrings(NumberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
        appendLineN(ExtErrorText, ExtErrorTextLength);
    }

    if(0!=ExtError2Text)
    {
        sprintf(NumberBuf, "%d", (int)ExtError2); //50 chars should be sufficient
        appendStrings(NumberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);
        appendLine(ExtError2Text);
    }

    return Error;
}

tcn00_Error DBMSrv_Reply::startWithSQLError(tcn00_Error  Error,
                                            const char*  SQLMessage,
                                            const int    SQLError)
{    
    return startWithError(Error, SQLError, SQLMessage, strlen(SQLMessage));
}


tcn00_Error DBMSrv_Reply::startWithRTEError(tcn00_Error  Error,
                                            const char * RTEMessage,
                                            int          RTEMessageLength,
                                            int          RTEError)
{
    size_t MessageLength=CStringLengthFromPascalString(RTEMessage, RTEMessageLength); //check if there is a terminating zero or trailing white spaces in RTEMessage

    return startWithError(Error, RTEError, RTEMessage, MessageLength);
}

tcn00_Error DBMSrv_Reply::startWithNewRTEError(
    const tcn00_Error      Error,
    const tsp01_RteError & RTEError)
{
    return startWithError(Error,
                          RTEError.RteErrCode,
                          RTEError.RteErrText,
                          strlen(RTEError.RteErrText),
                          RTEError.OsErrCode,
                          RTEError.OsErrText);
}

//-----------------------------------------------------------------------------
// public member function DBMSrv_Reply::startWithErrno
//-----------------------------------------------------------------------------
tcn00_Error DBMSrv_Reply::startWithErrno (  tcn00_Error  Error,
                                            const int    nErrno,
                                            const int    nExtError,  
                                            const char * szExtError )
{
  // the errno array
  // The available errnos are different on different platforms. Therefore
  // exists a ifdef confidition for every errno.
  struct {
    int          nValue;
    const char * szText;
  }               errnoArray[] = {
#ifdef E2BIG
  { E2BIG     , "E2BIG : argument list to long"             },
#endif
#ifdef EACCES
  { EACCES    , "EACCES : permission denied"                },
#endif
#ifdef EAGAIN
  { EAGAIN    , "EAGAIN : No more processes or not enough memory or maximum nesting level reached" },
#endif
#ifdef EBADF
  { EBADF     , "EBADF : bad file number"                   },
#endif
#ifdef ECHILD
  { ECHILD    , "ECHILD : no spawned processes"             },
#endif
#ifdef EDEADLOCK
  { EDEADLOCK , "EDEADLOCK : resource deadlock would occur" },
#endif
#ifdef EDOM
  { EDOM      , "EDOM : math argument"                      },
#endif
#ifdef EEXIST
  { EEXIST    , "EEXIST : file exist"                       },
#endif
#ifdef EINVAL
  { EINVAL    , "EIVAL : invalid argument"                  },
#endif
#ifdef EMFILE
  { EMFILE    , "EMFILE : too many open files"              },
#endif
#ifdef ENOENT
  { ENOENT    , "ENOENT : no such file or directory"        },
#endif
#ifdef ENOEXEC
  { ENOEXEC   , "ENOEXEC : exec format error"               },
#endif
#ifdef ENOMEM
  { ENOMEM    , "ENOMEM : not enough memory"                },
#endif
#ifdef ENOSPC
  { ENOSPC    , "ENOSPC : no space left on device"          },
#endif
#ifdef ERANGE
  { ERANGE    , "ERANGE : result too large"                 },
#endif
#ifdef EXDEV
  { EXDEV     , "EXDEV : cross-device link"                 },
#endif
  { 0 , NULL       },
  { 0 , "unknown errno" }};

  char NumberBuf[50];

  // DBMServer error
  this->startWithError(Error);

  // errno error
  int i;
  for (i = 0; (errnoArray[i].szText != NULL) && (nErrno != errnoArray[i].nValue); ++i);
  i = (errnoArray[i].szText == NULL) ? i + 1 : i;
  sprintf(NumberBuf, "%d", (int)nErrno); //50 chars should be sufficient
  appendStringsAndLine(NumberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN, errnoArray[i].szText);
  
  // addtional error
  if(szExtError != NULL)
  {
    sprintf(NumberBuf, "%d", (int)nExtError); //50 chars should be sufficient
    appendStringsAndLine(NumberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN, szExtError);
  }

  return Error;
} // end DBMSrv_Reply::startWithErrno

tcn00_Error DBMSrv_Reply::startWithMessageList(const Msg_List & MsgList)
{
    // OK or ERR ?
    startWithLine(MsgList.ID() == cn00_1_ErrId(OK_CN00_1) ? DBMSrv_Reply::STRING_OK : DBMSrv_Reply::STRING_ERR);

    if(MsgList.IsEmpty())
        return OK_CN00;

    appendMessageList(MsgList);

    return MsgList.ID();
}

void DBMSrv_Reply::appendMessageList(const Msg_List & MsgList)
{
    const Msg_List * Msg=&MsgList; //an iterator

    if(Msg->IsEmpty())
        return;

    //append all messages of the list
    while(0!=Msg)
    {
        char         numberBuf[50];
        const char * errorName=DBMSrvMsg_Error::getErrorName(*Msg);

        if(DBMSrvMsg_Error::DBMSrv_INTERNAL_TRANSPORT_MSG!=Msg) //add just the text, if it is our special message for tunneling strings
        {
            sprintf(numberBuf, "%d", (int)Msg->ID()); //50 chars should be sufficient for an integer
            appendStrings(numberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN);

            //if (*Msg) is a message created through DBMSrvMsg_Error, prepend the type prefix, the message name and the classic ": " in front of the message text
            if(0!=errorName)
            {
                Msg_List::MessageType   type=Msg->Type();
                const char            * prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_INFORMATION; // "INF_"

                if(Msg_List::Error==type)
                    prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_ERROR; // "ERR_"
                else
                    if(Msg_List::Warning==type)
                        prefix=DBMSrvMsg_Error::STRING_MESSAGE_PREFIX_WARNING; // "WRN_"
                    //else: just use the default "INF_"

                appendStrings(prefix, errorName, ": ");
            }
        }

        //append the message text itself
        if( (maximalReplyLength-1) > (*replyLength) ) //is there space to add something and a terminating zero?
        {
            SAPDB_UInt4 neededSpace=0;

            //we hope Msg_List honores the maximal length argument (1st argument)
            Msg->MessageWithInsertedArguments(
                maximalReplyLength-1-(*replyLength),
                replyData+(*replyLength),
                neededSpace,
                false);                                 //do not add unmatched Msg_Arg's as comma separated list

            //zero terminate the string in all cases
            if(int(neededSpace)>maximalReplyLength-1-(*replyLength))
                neededSpace=maximalReplyLength-1-(*replyLength); //this case should not happen, in a slow-DBM Server we would throw with assert
            
            replyData[(*replyLength)+neededSpace]='\0';

            //adjust replyLength to ensure validity of the object, use strlen to be robust
            (*replyLength)+=(int)strlen(replyData+(*replyLength));
        }

        //messages are separated by new lines, so we add one
        appendLine();

        Msg=Msg->NextMessage();
    } // end while
} // end DBMSrv_Reply::appendMessageList

void DBMSrv_Reply::startWithLine(const char * Line)
{
    *replyLength=0;
    appendLine(Line);
}

void DBMSrv_Reply::appendLine()
{
    appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
}

void DBMSrv_Reply::appendLine(const char * Line)
{
    appendString(Line);
    appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
}

void DBMSrv_Reply::appendLineN(const char * Line, size_t LineSize)
{
    appendStringN(Line, LineSize);
    appendString(DBMSrv_Reply::STRING_SEPARATOR_LINE);
}

void DBMSrv_Reply::appendLines(const char * Line1, const char * Line2)
{
    appendLine(Line1);
    appendLine(Line2);
}

void DBMSrv_Reply::appendLines(const char * Line1, const char * Line2, const char * Line3)
{
    appendLine(Line1);
    appendLine(Line2);
    appendLine(Line3);
}

void DBMSrv_Reply::appendIndentedWrappedLineN(const char   * LineStart,
                                              const size_t   LineLength,
                                              const size_t   Indent,
                                              const size_t   WrapAt,
                                              size_t         PositionInOutputLine)
{
    appendIndentedWrappedLineN(LineStart, LineLength, Indent, WrapAt, 0, PositionInOutputLine);
}

void DBMSrv_Reply::appendIndentedWrappedLineN(const char   * LineStart,
                                              const size_t   LineLength,
                                              const size_t   Indent,
                                              const size_t   WrapAt,
                                              const char   * LineBreakString,
                                              size_t         PositionInOutputLine)
{
    appendIndentedWrappedStringN(
        LineStart,
        LineLength,
        Indent,
        WrapAt,
        LineBreakString,
        PositionInOutputLine,
        true);

    appendLine();
    PositionInOutputLine=0; //we have a new output line after the line break
}

void DBMSrv_Reply::appendIndentedWrappedStringN(
        const char   * stringStart,
        const size_t   stringLength,
        const size_t   indent,
        const size_t   wrapAt,
        const char   * lineBreakingString,
        size_t       & positionInOutputLine,
        bool           forceAWordIntoFirstLine)
{
    bool         notInFirstLine=false; //we start in the first line
    bool         firstWordInLine=true;
    const char * position=cn36_FirstNonWhiteSpaceOf(stringStart, stringLength);

    while((size_t)(position-stringStart) < (int)stringLength) //as long as string is not finished
    {
        //indent the lines by adding the right number of spaces
        if(positionInOutputLine<indent)
        {
            appendCharacterRepeatedly(' ', indent-positionInOutputLine);
            positionInOutputLine=indent;
        }

        //write at least one word to each output line (that assures that the function is terminating)
        if(forceAWordIntoFirstLine || notInFirstLine)
        {
            size_t lengthOfNextWord=calculateLengthOfNextWord(position, stringLength-(position-stringStart), lineBreakingString);

            appendStringN(position, lengthOfNextWord);
            positionInOutputLine+=lengthOfNextWord;
            position+=lengthOfNextWord;
            position=cn36_FirstNonWhiteSpaceOf(position, stringLength-(position-stringStart));
        }

        //are we right behind the indent?
        if(positionInOutputLine==indent)
            firstWordInLine=true;
        else
            firstWordInLine=false;

        while(positionInOutputLine<wrapAt &&                 //there is still space in the output line -> add the next word
              (size_t)(position-stringStart) < stringLength) //and there is another word left in Line              
        {
            if(0==lineBreakingString ||                                             //no special line breaks
               strlen(lineBreakingString)>stringLength-(position-stringStart) ||    //or they do not fit in the rest of the string
               0!=strncmp(lineBreakingString, position, strlen(lineBreakingString)))//or they are not there
            {
                //next word is no linebreak
                size_t lengthOfNextWord=calculateLengthOfNextWord(position, stringLength-(position-stringStart), lineBreakingString);
                size_t neededSpace=(firstWordInLine?0:1)+lengthOfNextWord;

                if(positionInOutputLine+neededSpace<wrapAt &&    //another space and word can be added before a needed line wrap
                   (size_t)(position-stringStart) < stringLength)//and there is another word left in the string
                {
                    if(firstWordInLine)
                        firstWordInLine=false;
                    else
                        appendChar(' ');

                    appendStringN(position, lengthOfNextWord);

                    positionInOutputLine+=1+lengthOfNextWord;
                    position+=lengthOfNextWord;
                    position=cn36_FirstNonWhiteSpaceOf(position, stringLength-(position-stringStart));
                }
                else
                    positionInOutputLine=wrapAt; //just end the loop
            }
            else
            {
                positionInOutputLine=wrapAt; //just end the loop
                position+=strlen(lineBreakingString);
                position=cn36_FirstNonWhiteSpaceOf(position, stringLength-(position-stringStart));
            }
        }

        if((size_t)(position-stringStart) < stringLength) //something, that is not a space is left for the next line
        {
            appendLine();
            positionInOutputLine=0; //we have a new output line after the line break
        }

        notInFirstLine=true;
    }
}


//-----------------------------------------------------------------------------
// public member function DBMSrv_Reply::startWithString
//-----------------------------------------------------------------------------
void DBMSrv_Reply::startWithString ( const char * String )
{
    *replyLength=0;
    appendString(String);
} // end DBMSrv_Reply::startWithString

void DBMSrv_Reply::appendString(const char *String)
{
    appendStringN(String, strlen(String));
}

void DBMSrv_Reply::appendStringN(const char *String, size_t StringSize)
{
    if( (maximalReplyLength-1) > (*replyLength) ) //is there space to add something and a terminating zero?
    {
        size_t SpaceInReply=maximalReplyLength-1-(*replyLength);
        
        if(StringSize>SpaceInReply) //take the minimum
            StringSize=SpaceInReply;

        strncpy(replyData+(*replyLength), String, StringSize); 
        replyData[StringSize+(*replyLength)]='\0'; // make sure we have a 0-terminator

        (*replyLength)+=(int)strlen(replyData+(*replyLength));
    }
}

void DBMSrv_Reply::appendStrings(const char *String1, const char *String2)
{
    appendString(String1);
    appendString(String2);
}

void DBMSrv_Reply::appendStrings(const char *String1, const char *String2, const char *String3)
{
    appendString(String1);
    appendString(String2);
    appendString(String3);
}

void DBMSrv_Reply::appendStringAndLine(const char *String1, const char *Line2)
{
    appendString(String1);
    appendLine(Line2);
}

void DBMSrv_Reply::appendStringAndLineN(const char *String1, const char *Line2, size_t Line2Length)
{
    appendString(String1);
    appendLineN(Line2, Line2Length);
}

void DBMSrv_Reply::appendStringsAndLine(const char *String1, const char *String2, const char *Line3)
{
    appendString(String1);
    appendString(String2);
    appendLine(Line3);
}

void DBMSrv_Reply::appendStringsAndLine(const char * string1, const char * string2, const char * string3, const char * line)
{
    appendString(string1);
    appendString(string2);
    appendString(string3);
    appendLine(line);
}

void DBMSrv_Reply::appendStringsStringNAndLine(const char   * String1,
                                               const char   * String2,
                                               const char   * String3,
                                               const char   * String4,
                                               const size_t   String4Length,
                                               const char   * Line)
{
    appendString(String1);
    appendString(String2);
    appendString(String3);
    appendStringN(String4, String4Length);
    appendLine(Line);
}

void DBMSrv_Reply::appendStringWithMinWidth(const char *String, size_t MinimalWidth)
{
    size_t LengthOfString=strlen(String);

    appendString(String);

    if(LengthOfString<MinimalWidth)
        appendCharacterRepeatedly(' ', MinimalWidth-LengthOfString);
}

void DBMSrv_Reply::appendStringNWithMinWidth(const char *String, size_t StringLength, size_t MinimalWidth)
{
    appendStringN(String, StringLength);

    if(StringLength<MinimalWidth)
        appendCharacterRepeatedly(' ', MinimalWidth-StringLength);
}

void DBMSrv_Reply::appendTabulator()
{
    appendString(VALUE_SEPSTRING_CN00); //same as appendString("\t");
}

void DBMSrv_Reply::appendChar(const char Character)
{
    if( (maximalReplyLength-1) > (*replyLength) ) //is there space to add something and a terminating zero?
    {
        replyData[(*replyLength)++]=Character;
        replyData[*replyLength]='\0';
    }
}

void DBMSrv_Reply::appendCharacterRepeatedly(const char Character, size_t NumberOfAppends)
{
    if(NumberOfAppends+(*replyLength) >= (size_t)maximalReplyLength)
       NumberOfAppends=maximalReplyLength-1-(*replyLength);

    if(0<NumberOfAppends)
    {
        ToolsMeMa_GuardedOperation::MemFill(
            replyData+(*replyLength),
            Character,
            NumberOfAppends,
            __FILE__,
            __LINE__);

        (*replyLength)+=(int)NumberOfAppends;
        replyData[*replyLength]='\0';
    }
}

DBMSrv_Reply::Position DBMSrv_Reply::giveCurrentEnd() const
{
    return constReplyData+strlen(constReplyData);
}

char * DBMSrv_Reply::giveCurrentEnd()
{
    return replyData+(*replyLength);
}

void DBMSrv_Reply::skipAt(Position SkipPosition)
{
    size_t SkipLength=SkipPosition-replyData;

    if(SkipLength<(size_t)maximalReplyLength && 0!=replyData)
    {
        replyData[SkipLength]='\0';
        (*replyLength)=(int)strlen(replyData);
    }
}

void DBMSrv_Reply::insertString(Position InsertPosition, const char * String)
{
    size_t InsPos=InsertPosition-replyData;
    size_t StringLength=strlen(String);
    
    if( size_t(maximalReplyLength) > InsPos+StringLength && //is there enough space to insert the string and a terminating zero?
        size_t(*replyLength) > InsPos                    && //inserting in front of the terminating zero?
        0!=replyData )                                      //insert only into changeable strings
    {
        strncpy(replyData+InsPos, String, StringLength); //copy the string without the terminating zero

        if( InsPos+StringLength > size_t(*replyLength) ) //inserted string has overwritten the terminating zero
        {
            *(replyData+InsPos+StringLength)='\0';     //write a new terminating zero
            *replyLength=int(InsPos+StringLength);     //adjust the length member
        }
        //else nothing to do, old reply from InsertPosition was already longer than inserted string
    }
}

void DBMSrv_Reply::appendStartOfHSSNodeAnswerFor(const char * standbyName)
{
    appendStringAndLine("node: ", standbyName);
}

void DBMSrv_Reply::appendMessageListForHSSNode(const Msg_List & messageList)
{
    const Msg_List* message=&messageList;

    char numberBuf[50];

    while(0!=message)
    {
        sprintf(numberBuf, "%d", (int)message->ID()); //50 chars should be sufficient
        appendStringsAndLine("    ", numberBuf, DBMSrv_Reply::STRING_SEPARATOR_TOKEN, message->Message());    

        message=message->NextMessage();
    }
}

void DBMSrv_Reply::appendOKForHSSNode()
{
    appendStringAndLine("    ", DBMSrv_Reply::STRING_OK);
}

bool DBMSrv_Reply::startsWithOKAndContinueFlag() const
{
    bool rc=false;

    const char * firstLineStart=constReplyData;
    const char * firstLineEnd=getEndOfFirstLine();

    if(firstLineEnd-firstLineStart==strlen(STRING_OK) && 0==strncmp(DBMSrv_Reply::STRING_OK, firstLineStart, strlen(STRING_OK)))
    {
        const char * secondLineStart=getStartOfNextLine(firstLineEnd);
        const char * secondLineEnd  =getEndOfLine(secondLineStart);

        if(secondLineEnd-secondLineStart==strlen(DBMSrv_Reply::STRING_CONTINUE) &&
           strncmp(DBMSrv_Reply::STRING_CONTINUE, secondLineStart, strlen(DBMSrv_Reply::STRING_CONTINUE)))
        {
            rc=true;
        }
    }

    return rc;
}

size_t DBMSrv_Reply::calculateLengthOfNextWord(const char *WordStart, size_t MaxWordLength, const char * LineBreakString)
{
    size_t rc=cn36_FirstWhiteSpaceOf(WordStart, MaxWordLength)-WordStart;

    if(0!=LineBreakString)
    {
        const char * NextArtificialLineBreak=strstr(WordStart, LineBreakString);

        if(0!=NextArtificialLineBreak && (size_t)(NextArtificialLineBreak-WordStart)<rc)
            rc=NextArtificialLineBreak-WordStart;  //words end at artificial line breaks
    }

    return rc;
}

size_t DBMSrv_Reply::CStringLengthFromPascalString(const char * SourcePascalString,
                                                   const int    PascalStringMaximalLength)
{
    size_t CStringLength=0;
    int  i;

    // at first looking for zero termination in source
    for(i=0; i<PascalStringMaximalLength-1 && SourcePascalString[i]!='\0'; i++);

    if('\0'==SourcePascalString[i])
        CStringLength=i;
    else
    {
        bool FoundLastNonSpace=false;

        CStringLength=PascalStringMaximalLength;
        
        while(CStringLength>0 && !FoundLastNonSpace)
        {
            if(' '==SourcePascalString[CStringLength-1])
                CStringLength--;
            else
                FoundLastNonSpace=true;
        }
    }

    return CStringLength;
}
