/*! 
    \file    DBMSrv_Reply.hpp
    \author  TiloH
    \ingroup common classes for the DBMServer
    \brief   declaration of a class handling the reply buffer for DBMServer
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

#if !defined(DBMSRV_REPLY_HPP)
#define DBMSRV_REPLY_HPP


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "gcn00.h"
#include "gsp01.h"


//-----------------------------------------------------------------------------
// declaration of class DBMSrv_Reply
//-----------------------------------------------------------------------------

//! \brief a class managing the reply of DBMServer commands
class DBMSrv_Reply
{
  public:

    typedef const char * Position; //!< a data type for storing positions within the reply
    /*! constructor

        initialising a reply object without a real reply buffer, before using
        such an object it is useful to call SetTo() */
    DBMSrv_Reply();

    /*! constructor

        initialising a reply object with the needed information
        \param ReplyData   [IN] a pointer to the reply buffer that shall contain the reply
        \param ReplyLength [IN] a pointer to the length of the current reply
        \param ReplyLenMax [IN] the maximal allowed reply length */
    DBMSrv_Reply(char * ReplyData,
                 int  * ReplyLength,
                 int    ReplyLenMax);

    /*! constructor

        initialising a readonly reply object with the needed information, only const functions
        can be used with this object
        \param constantReplyData  [IN] a pointer to the existing constant reply buffer */
    DBMSrv_Reply(const char * constantReplyData);

    /*! destructor */
    virtual ~DBMSrv_Reply() {}

    /*! function for changing the managed reply buffer
        \param ReplyData   [IN] a pointer to the new replay buffer that shall contain the reply
        \param ReplyLength [IN] a pointer to the new length of the current reply
        \param ReplyLenMax [IN] the new maximal allowed reply length */
    void setTo(char * ReplyData,
               int  * ReplyLength,
               int    ReplyLenMax);

    /*! function for changing the managed reply buffer to a global available dummy

        the result is undefined, if more then one reply object is using the global dummy buffer
        at the same time */
    void setToGlobalDummy();

    /*! function for obtaining the reply buffer

        This function should be avoided whenever possible, because it's the long term goal of this class
        to capsulate all operations on the reply buffers
        \return a pointer to the reply buffer
        \todo eliminate all occurences of this function on long term */
    char * giveData();

    /*! function for obtaining a const pointer the reply buffer

        \return a const pointer to the reply buffer */
    const char * giveData() const;

    /*! function for obtaining a reference to the reply length

        This function should be avoided whenever possible, because it's the long term goal of this class
        to capsulate all operations on the reply buffers
        \return a reference to the variable holding the reply length
        \todo eliminate all occurences of this function on long term */
    int * giveLength();

    /*! function for obtaining the length of the reply

        \return the current length of buffer data */
    int getCurrentLength() const;

    /*! function for adapting the length member of the reply object

        This function determines the string length of the current zero terminated
        reply in the buffer and sets the reply length to that value.
        This function must be used whenever the reply buffer is manipulated
        directly with the help of the pointer returned by the 'char * giveData()' function.
        When all operations on the reply buffers are capsulated in this class,
        the adaptLength() function is no longer needed.
        \todo eliminate all occurences of this function when Data() is no longer used either
        
        \return true if new length is still inside the buffer, false if bufferoverflow occured */
    bool adaptLength();

    /*! function for obtaining the maximal allowed reply length of the current buffer
        \return maximal allowed length of a reply */
    int giveMaximalLength() const;

    /*! function for obtaining the free space of the current buffer
        \return number of unused characters of the reply buffer*/
    int giveAvailableSpace() const;

    /*! function for obtaining the end of the first line of the reply
        \return the position of the first character behind the first line (pointer to line separator or terminating zero) */
    Position getEndOfFirstLine() const;

    /*! function for obtaining the end of the line containing a certain Position
        \param  positionInLine [IN] the position in the reply buffer to start searching from
        \return the position of the first character behind the line (pointer to line separator or terminating zero) */
    Position getEndOfLine(Position positionInLine) const;

    /*! function for obtaining the start of the reply text (first line after OK or ERR)
        \return first line of answer without the line containing ERR or OK*/
    Position getStartOfReplyText() const;

    /*! function for obtaining the first character of the next line
        \param  positionInLine [IN] the position in the reply buffer to start searching from for end of line and next line
        \return the position of the first character of the next line, or 0 pointer if there is no next line */
    Position getStartOfNextLine(Position positionInLine) const;

    /*! function for obtaining the start and end of a certain line of the reply
        \param lineNumber [IN]  number of the line to search for
        \param start      [OUT] first character of the line, equals end, if line is empty
        \param end        [OUT] first character behind the line (pointer to line separator or terminating zero) */        
    void getStartAndEndOfLine(unsigned int lineNumber, Position & start, Position & end) const;

    /*! function for obtaining the start and end of the next complete line following a certain position in the reply
        \param positionInLine [IN]  the position in the reply buffer to start searching from for end of line and next line
        \param start          [OUT] first character of the line, equals end, if line is empty
        \param end            [OUT] first character behind the line (pointer to line separator or terminating zero) */        
    void getStartAndEndOfNextLine(Position positionInLine, Position & start, Position & end) const;

    /*! function for obtaining the ReturnCode
        \return the current return code of the DBMServer command*/
    tcn00_Error   ReturnCode() const;

    /*! function for reporting an OK as reply */
    void startWithOK();

    /*! function for reporting an OK as reply if parameter is OK_CN00
        \param rc [IN] current DBMServer return code for the command */
    void startWithOKIfRCIsOk(tcn00_Error rc);

    /*! function for reporting a tcn00_Error as reply
        \param Error              [IN] DBMServer error to report
        \param ExtError           [IN] the number of a second error to report
        \param ExtErrorText       [IN] text of the second error to report
        \param ExtErrorTextLength [IN] length of the text of the second error
        \param ExtError2          [IN] the number of a third error to report
        \param ExtError2Text      [IN] zero terminated text of the third error to report
        \return copy of Error */
    tcn00_Error startWithError(const tcn00_Error Error,
                               int               ExtError          =0,
                               const char *      ExtErrorText      =0,
                               size_t            ExtErrorTextLength=0,
                               int               ExtError2         =0,
                               const char *      ExtError2Text     =0);

    /*! function for reporting a RTEError as reply
        \param Error            [IN] DBMServer error to report
        \param RTEMessage       [IN] RTEMessage to report
        \param RTEMessageLength [IN] length of the RTEMessage
        \param RTEError         [IN] a number of the RTEError
        \return copy of Error */
    tcn00_Error startWithRTEError(tcn00_Error  Error,
                                  const char * RTEMessage,
                                  int          RTEMessageLength,
                                  int          RTEError);

    /*! function for reporting a new RTEError as reply
        \param Error            [IN] DBMServer error to report
        \param RTEError         [IN] RTE error to report
        \return copy of Error */
    tcn00_Error startWithNewRTEError(const tcn00_Error      Error,
                                     const tsp01_RteError & RTEError);

    /*! function for reporting a errno as reply
        \param Error            [IN] DBMServer error to report
        \param nErrno           [IN] errno to report
        \param nExtError        [IN] additional error code
        \param szExtError       [IN] additional error text
        \return copy of Error */
    tcn00_Error startWithErrno (  tcn00_Error  Error,
                                  const int    nErrno,
                                  const int    nExtError  = 0,  
                                  const char * szExtError = NULL );

    /*! function for reporting an SQLError as reply
        \param Error            [IN] DBMServer error to report
        \param SQLMessage       [IN] SQLMessage to report (must contain trailing '\\0')
        \param SQLError         [IN] SQL error code
        \return copy of Error */
    tcn00_Error startWithSQLError(tcn00_Error  Error,
                                  const char*  SQLMessage,
                                  const int    SQLError);

    /*! 
        \brief function for reporting a Msg_List as reply

        When using this method, make sure that the passed Msg_List was built 
        in the correct order as described in header Msg_List.hpp: newer
        messages are added to a Msg_List at the top with the Msg_List::Overule() 
        method.
        
        \see Msg_List
        
        \param MsgList [IN] message list to report
        \return first error ID of the message list */
    tcn00_Error startWithMessageList(const Msg_List &MsgList);

    /*! function for writing the first line of a reply
        \param Line [IN] the line to start the reply with */
    void startWithLine(const char * Line);

    /*! 
        @brief function for appending a Msg_List to the reply

        When using this method, make sure that the passed Msg_List was built 
        in the correct order as described in header Msg_List.hpp: newer
        messages are added to a Msg_List at the top with the Msg_List::Overule() 
        method.

        \see Msg_List

        \param MsgList [IN] message list to report
        \return first error ID of the message list */
    void appendMessageList(const Msg_List &MsgList);

    /*! function for appending a line break to the reply */
    void appendLine(); 

    /*! function for appending a line to the reply
        \param Line [IN] the line to add to the reply */
    void appendLine(const char * Line);

    /*! function for appending a not necessarily zero
        terminated line into the reply
        \param Line     [IN] the line to add to the reply
        \param LineSize [IN] the size of the line in characters */
    void appendLineN(const char * Line, size_t LineSize);

    /*! function for appending two lines to the reply
        \param Line1 [IN] first line to add to the reply
        \param Line2 [IN] second line to add to the reply */
    void appendLines(const char * Line1, const char * Line2);

    /*! function for appending tree lines to the reply
        \param Line1 [IN] first line to add to the reply
        \param Line2 [IN] second line to add to the reply
        \param Line3 [IN] third line to add to the reply*/
    void appendLines(const char * Line1, const char * Line2, const char * Line3);

    /*! function for appending a not necessarily zero terminated
        string and a line break to reply, so that the resulting
        output is indented and a maximal line size is forcing
        additional line breaks at word boundaries, if need.
        \param LineStart  [IN] start of the string, containing the words
        \param LineLength [IN] length of the string
        \param Indent     [IN] number of spaces used for indent
        \param WrapAt     [IN] number of characters were a line break should happen
        \param PositionInOutputLine [IN] starting position within the first line of output*/
    void appendIndentedWrappedLineN(const char   * LineStart,
                                    const size_t   LineLength,
                                    const size_t   Indent,
                                    const size_t   WrapAt,
                                    size_t         PositionInOutputLine);

    /*! function for appending a not necessarily zero terminated
        string and a line break to reply, so that the resulting
        output is indented and a maximal line size and a special
        line break string is forcing additional line breaks at
        word boundaries
        \param LineStart            [IN] start of the string, containing the words
        \param LineLength           [IN] length of the string
        \param Indent               [IN] number of spaces used for indent
        \param WrapAt               [IN] number of characters were a line break should happen
        \param LineBreakString      [IN] special string for forcing line breaks
        \param PositionInOutputLine [IN] starting position within the first line of output*/
    void appendIndentedWrappedLineN(const char   * LineStart,
                                    const size_t   LineLength,
                                    const size_t   Indent,
                                    const size_t   WrapAt,
                                    const char   * LineBreakString,
                                    size_t         PositionInOutputLine);

    /*! function for appending a not necessarily zero terminated
        string to reply (currently the string must be at least part of
        a zero terminated string), so that the resulting output is indented
        and a maximal line size and a special line break string is forcing
        additional line breaks at word boundaries. If the string contains
        a word longer then WrapAt-Indent, that word is placed on a line of it's own.

        \param stringStart             [IN] start of the string, containing the words
        \param stringLength            [IN] length of the string
        \param indent                  [IN] number of spaces used for indent
        \param wrapAt                  [IN] number of characters were a line break should happen
        \param lineBreakingString      [IN] special string for forcing line breaks
        \param positionInOutputLine    [IN/OUT] position within the current line of output
        \param forceAWordIntoFirstLine [IN] set to true, if the first word must br forced
                                            into the first procedded line at all costs,
                                            false otherwise */
    void appendIndentedWrappedStringN(
        const char   * stringStart,
        const size_t   stringLength,
        const size_t   indent,
        const size_t   wrapAt,
        const char   * lineBreakingString,
        size_t       & positionInOutputLine,
        bool           forceAWordIntoFirstLine);

    /*! function for writing the first string of a reply
        \param String [IN] the string to start the reply with */
    void startWithString(const char * String);

    /*! function for appending a string to the reply
        \param String [IN] string to add to the reply */
    void appendString(const char *String);

    /*! function for appending a not necessarily zero
        terminated string into the reply
        \param String     [IN] string to add to the reply
        \param StringSize [IN] the size of the string in characters */
    void appendStringN(const char *String, size_t StringSize);

    /*! function for appending two strings to the reply
        \param String1 [IN] first string to add to the reply
        \param String2 [IN] second string to add to the reply */
    void appendStrings(const char *String1, const char *String2);

    /*! function for appending three strings to the reply
        \param String1 [IN] first string to add to the reply
        \param String2 [IN] second string to add to the reply
        \param String3 [IN] third string to add to the reply */
    void appendStrings(const char *String1, const char *String2, const char *String3);

    /*! function for appending two strings, followed by a
        line break to the reply
        \param String1 [IN] first string to add to the reply
        \param Line2   [IN] the second string to add to the reply */
    void appendStringAndLine(const char *String1, const char *Line2);

    /*! function for appending two strings (the last one terminated via
        length option), followed by a line break to the reply
        \param String1     [IN] first string to add to the reply
        \param Line2       [IN] the second string to add to the reply, a line break will be added behind that string
        \param Line2Length [IN] the length of the second string */
    void appendStringAndLineN(const char *String1, const char *Line2, size_t Line2Length);

    /*! function for appending tree strings, followed by a
        line break to the reply
        \param String1 [IN] first string to add to the reply
        \param String2 [IN] second string to add to the reply
        \param Line3   [IN] the third string to add to the reply, a line break will be added behind that string */
    void appendStringsAndLine(const char *String1, const char *String2, const char *Line3);

    /*! function for appending four strings, followed by a
        line break to the reply
        \param string1 [IN] first string to add to the reply
        \param string2 [IN] second string to add to the reply
        \param string3 [IN] third string to add to the reply
        \param line    [IN] the fourth string to add to the reply, a line break will be added behind that string */
    void appendStringsAndLine(const char * string1, const char * string2, const char * string3, const char * line);

    /*! function for appending three strings a not zero terminated string
        another string and a line break to the reply
        \param String1       [IN] first string to add to the reply
        \param String2       [IN] second string to add to the reply
        \param String3       [IN] third string to be added to the reply
        \param String4       [IN] fourth not zero terminated string to be added to the reply
        \param String4Length [IN] the length of the fourth string
        \param Line          [IN] the fifth an last string to be added */
    void appendStringsStringNAndLine(const char   * String1,
                                     const char   * String2,
                                     const char   * String3,
                                     const char   * String4,
                                     const size_t   String4Length,
                                     const char   * Line);

    /*! function for appending a string, at least MinimalWidth
        characters are written, if string has less
        than MinimalWidth characters, spaces are added
        \param String       [IN] string to add to the reply
        \param MinimalWidth [IN] guaranteed number of characters,
                                 that are written to reply*/
    void appendStringWithMinWidth(const char *String, size_t MinimalWidth);

    /*! function for appending a not zero terminated string, at least MinimalWidth
        characters are written, if string has less
        than MinimalWidth characters, spaces are added
        \param String       [IN] string to add to the reply
        \param StringLength [IN] length of string String
        \param MinimalWidth [IN] guaranteed number of characters,that are
                                 written to reply*/
    void appendStringNWithMinWidth(const char *String, size_t StringLength, size_t MinimalWidth);

    /*! \brief function for appending a tabulator to the reply */
    void appendTabulator();

    /*! a function adding a character to the reply
        \param Character [IN] the character that is to be added */
    void appendChar(const char Character);

    /*! a function for appending a character a number of times
        \param Character       [IN] the character to append
        \param NumberOfAppends [IN] the number of times Character is to ba appended*/
    void appendCharacterRepeatedly(const char Character, size_t NumberOfAppends);

    /*! function returning the current end of the reply buffer
        \return end of current reply buffer */
    Position giveCurrentEnd() const;

    /*! function returning the current end of the reply buffer
        \return end of current reply buffer
        \todo eliminate all occurences of this function on long term */
    char * giveCurrentEnd();

    /*! function terminating the string at Position
        \param SkipPosition [IN] the position in the reply buffer, where to end the reply at */
    void skipAt(Position SkipPosition);

    /*! function for inserting a string into existing reply
        \param InsertPosition [IN] position where the string is inserted
        \param String         [IN] string to add to the reply */
    void insertString(Position InsertPosition, const char * String);

    /*! function for starting a new "sub"-reply of another HSS node within the
        current reply.

        The "sub"-reply will be preceeded by the line "node: \<node\>", where \<node\> is substituted
        by the string supplied as name of the HSS mode. The "sub"-reply itself will be indented
        with 4 spaces.
        \param standbyName [IN] name of the HSS node */
    void appendStartOfHSSNodeAnswerFor(const char * standbyName);

    /*! append a message list as "sub"-reply for a HSS node
    
        The "sub"-reply should have been started with appendStartOfHSSNodeAnswerFor()
        \see appendStartOfHSSNodeAnswerFor 
        \param messageList [IN] the message list will be appended using an indent of 4 spaces*/
    void appendMessageListForHSSNode(const Msg_List & messageList);

    /*! append an "OK" indented by 4 spaces
        The HSS node "sub"-reply should have been started with appendStartOfHSSNodeAnswerFor()
        \see appendStartOfHSSNodeAnswerFor */
    void appendOKForHSSNode();

    /*! function for checking a reply for the continue flag
        \return true if reply starts with a line containing OK followed
                by a line containing CONTINUE, false otherwise */
    bool startsWithOKAndContinueFlag() const;

  private:
    char        * replyData;
    const char  * constReplyData;
    int         * replyLength;
    int           maximalReplyLength;

    char defaultBuffer[1];
    int  defaultLength;

    size_t calculateLengthOfNextWord(const char *WordStart, size_t MaxWordLength, const char * LineBreakString);

    size_t CStringLengthFromPascalString(const char * SourcePascalString,
                                         const int    PascalStringMaximalLength);

  public:
    static const char * STRING_OK;              //!< string used to mark a successful executed command in the reply
    static const char * STRING_ERR;             //!< string used to mark an unsuccessful executed command in the reply
    static const char * STRING_SEPARATOR_LINE;  //!< string used to separate lines in the reply
    static const char * STRING_SEPARATOR_TOKEN; //!< string often used to separate parts in a reply line
    static const char * STRING_CONTINUE;        //!< string used to mark a reply as first part of the complete command answer
    static const char * STRING_END;             //!< string used to mark a reply as last part of the complete command answer
};

#endif
