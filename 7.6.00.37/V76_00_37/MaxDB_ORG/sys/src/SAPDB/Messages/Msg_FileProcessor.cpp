/*!
  @file    Msg_FileProcessor.cpp
  @author  Robin
  @ingroup Messages
  @brief   Message file processing class
  
  processing of message list files (pseudo-xml to well-formed xml)

  -------------------------------------------------------------------------

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

#include "heo01.h"
#include "Messages/Msg_IOutput.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "SAPDBCommon/SAPDB_Types.h"
#include "RunTime/System/RTESys_IO.h"
#include "Messages/Msg_OutputFile.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"

#include "Messages/Msg_FileProcessor.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

#include "SAPDBCommon/SAPDB_Names.h"

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>  // to be removed, for printf() only...

//#define CHECK_CONSISTENCY

/*! define for file tokenization */
#define START 0
/*! define for file tokenization */
#define END  -1

/*! define for file tokenization */
#define GARBAGE     false
/*! define for file tokenization */
#define NOT_FOUND   false

#ifdef WIN32
/*! define for platform spedific line ending */
#define LINE_ENDING "\r\n"
#else
/*! define for platform spedific line ending */
#define LINE_ENDING "\n"
#endif

/*! define for xml tag not found in other header files... */
#define MSG_FILE_ID_TAG             "ID"
/*! define for xml tag not found in other header files... */
#define MSG_FILE_SHORTTEXT_TAG      "ShortText"
/*! define for xml tag not found in other header files... */
#define MSG_FILE_DESCRIPTION_TAG    "Description"
/*! define for xml tag not found in other header files... */
#define MSG_FILE_ACTION_TAG         "Action"
/*! define for xml tag not found in other header files... */
#define MSG_CDATA_BEGIN_TAG         "![CDATA["

/*! define for generating files that are compatible to old knldiag files */
#define ENDOFSTARTUP                "======================================= end of startup part ====================\n"
/*! define for generating files that are compatible to old knldiag files */
#define BEGIN_OF_DIAG_WRITE_CYCLE   "============================================ begin of write cycle ==================\r\n"

/*! define for generating files that are compatible to old knldiag files */
#define KERNELEXIT                  "+++++++++++++++++++++++++++++++++++++++ Kernel Exit ++++++++++++++++++++++++++++\n"

/*! the offset from the end of the file where searching for the last written message list starts in continuously written files */
#define SEARCH_OFFSET_  4096    

/*!
  @class Tag
  @brief helper class for storing data about xml tags: length and name. Used for fast comparison (first compare lengths, only if they match, compare names)
 */
class Tag
{
public:
    /*! the name of the tag */
    const SAPDB_Char *m_name;
    /*! the length of the name of the tag */
    SAPDB_UInt  m_lengthOfName;

    /*!
        @brief  ctor
        @param  name    [in]    the name of the tag
    */
    Tag(const SAPDB_Char *name):m_name(name),m_lengthOfName((SAPDB_UInt)strlen(name)){};
};

/*! internal tag for message short text */
Tag tagText         (MSG_LIST_TEXT_TAG);
/*! internal tag for message id number */
Tag tagId           (MSG_LIST_ID_TAG);
/*! internal tag for message component name */
Tag tagComponent    (MSG_LIST_COMPONENT_TAG);
/*! internal tag for message type */
Tag tagType         (MSG_LIST_MESSAGE_TYPE_TAG);
/*! internal tag for message timestamp */
Tag tagTimeStamp    (MSG_LIST_TIMESTAMP_TAG);
/*! internal tag for message number */
Tag tagNumber       (MSG_LIST_MESSAGE_NUMBER_TAG);
/*! internal tag for message thread id number */
Tag tagThreadId     (MSG_LIST_THREAD_ID_TAG);
/*! internal tag for message task id number */
Tag tagTaskId       (MSG_LIST_TASK_ID_TAG);
/*! internal tag for message label */
Tag tagLabel        (MSG_LIST_LABEL_TAG);

/*!
  @class StringWithLeadingSpaces
  @brief helper class for generating a string with a given number of leading spaces
 */
class StringWithLeadingSpaces
{
public:
    /*!
        @brief  ctor
        @param  string  [in]    the string to fill up
        @param  length  [in]    the length of the result string
    */
    StringWithLeadingSpaces(const SAPDB_Char *string,SAPDB_UInt length)
    {
        SAPDB_UInt spaces;
        if(strlen(string) >= length)
            spaces = 1;
        else
            spaces = SAPDB_MIN(length - (SAPDB_UInt)strlen(string),29);
        SAPDB_strncpy_zero_terminated(&m_buf[0],"                              ",spaces+1);
        SAPDB_strncpy_zero_terminated(&m_buf[0]+spaces,string,length-spaces+1);
    }
    /*!
        @brief cast operator (const SAPDB_Char *)
        @return pointer to output string
    */
    operator const SAPDB_Char * () const { return ( m_buf ) ; }
private:
    SAPDB_Char m_buf[30];
};

/*!
  @class StringWithTrailingSpaces
  @brief helper class for generating a string with a given number of trailing spaces
 */
class StringWithTrailingSpaces
{
public:
    /*!
        @brief  ctor
        @param  string  [in]    the string to fill up
        @param  length  [in]    the length of the result string
    */
    StringWithTrailingSpaces(const SAPDB_Char *string,SAPDB_UInt length)
    {
        SAPDB_UInt spaces;
        if(strlen(string) >= length)
            spaces = 1;
        else
            spaces = SAPDB_MIN(length - (SAPDB_UInt)strlen(string),19);
        SAPDB_strncpy_zero_terminated(&m_buf[0],string,length-spaces+1);
        SAPDB_strncpy_zero_terminated(&m_buf[0]+strlen(string),"                    ",spaces);
    }
    /*!
    @brief cast operator (const SAPDB_Char *)
    @return pointer to output string
    */
    operator const SAPDB_Char * () const { return ( m_buf ) ; }
private:
    SAPDB_Char m_buf[20];
};



void Msg_FileProcessor::DoOutputSingleString(const SAPDB_Char *string)
{
    if(m_destinationFile)
    {
        tsp00_VfReturn_Param ReturnStatus;         
        RTE_FileOffset writtenSizeInBytes; 
        RTESys_IOWrite ( m_destinationFile, 
                    string, 
                    strlen(string), 
                    writtenSizeInBytes, 
                    ReturnStatus ); 
    }
    else
    {
        printf("%s",string);
    }
}

void Msg_FileProcessor::DoOutput   ( const SAPDB_Char *string1
                , const SAPDB_Char *string2
                , const SAPDB_Char *string3
                , const SAPDB_Char *string4
                , const SAPDB_Char *string5
                , const SAPDB_Char *string6
                , const SAPDB_Char *string7
                , const SAPDB_Char *string8
                , const SAPDB_Char *string9
                , const SAPDB_Char *string10
                , const SAPDB_Char *string11
                , const SAPDB_Char *string12
                , const SAPDB_Char *string13
                , const SAPDB_Char *string14
                )
{
    if(string1)DoOutputSingleString(string1);
    if(string2)DoOutputSingleString(string2);
    if(string3)DoOutputSingleString(string3);
    if(string4)DoOutputSingleString(string4);
    if(string5)DoOutputSingleString(string5);
    if(string6)DoOutputSingleString(string6);
    if(string7)DoOutputSingleString(string7);
    if(string8)DoOutputSingleString(string8);
    if(string9)DoOutputSingleString(string9);
    if(string10)DoOutputSingleString(string10);
    if(string11)DoOutputSingleString(string11);
    if(string12)DoOutputSingleString(string12);
    if(string13)DoOutputSingleString(string13);
    if(string14)DoOutputSingleString(string14);
}

/*!
  @class Msg_Position
  @brief helper class for description of a message
 */
class Msg_Position
{
public:
    /*! offset of the text  */
    RTE_FileOffset  m_start;
    /*! length of the text  */
    SAPDB_UInt4     m_length;
};

/*!
  @class Msg_Component
  @brief helper class for description of a message
 */
class Msg_Component
{
public:
    /*! flag that indicates if this component is availalbe */
    SAPDB_Bool       m_Available;
    /*! pointer to the next component in the list */
    Msg_Component   *m_pNext;
    /*! name of the component */
    SAPDB_Char      *m_pComponentName;
    /*! address wher the contents of the file are stored */
    SAPDB_Char      *m_fileBuffer;
    /*! biggest message id stored */
    SAPDB_UInt8      m_biggestId;
    /*! pointer to the array of short text positions */
    Msg_Position    *m_pTextPositionArray;
    /*! pointer to the array of description positions */
    Msg_Position    *m_pDescriptionPositionArray;
    /*! pointer to the array of action positions */
    Msg_Position    *m_pActionPositionArray;
};

/*! the number of known tags */
static const SAPDB_UInt  numberOfKnownTags = 10;

/*! the names of the tags known */
static const SAPDB_Char * const knownTagNames[numberOfKnownTags] = 
{   
    MSG_LIST_MESSAGE_LIST_TAG,
    MSG_LIST_MESSAGE_TAG,
    MSG_FILE_SHORTTEXT_TAG, 
    MSG_LIST_MESSAGE_TYPE_TAG,
    MSG_LIST_FILE_NAME_TAG, 
    MSG_LIST_LINE_NUMBER_TAG,
    MSG_LIST_TIMESTAMP_TAG,
    MSG_LIST_ARGUMENTS_TAG,
    MSG_FILE_DESCRIPTION_TAG,
    MSG_FILE_ACTION_TAG
};

/*!
  @class Attribute
  @brief helper class for description of a message
 */
struct Attribute
{
    RTE_FileOffset  beginOfName;
    SAPDB_UInt      lengthOfName;
    RTE_FileOffset  beginOfValue;
    SAPDB_UInt      lengthOfValue;
    Attribute      *pnext;
};

/*!
  @class Msg_Element
  @brief helper class for constructing a list of messages
 */
struct Msg_Element
{
    /*! if the element is a tag: which one? (coded as id) */
    Msg_FileProcessor::xmlTagID     tagID;
    /*! begin or end tag? */
    Msg_FileProcessor::xmlTagKind   tagKind;
    /*! if the element is text: the offset of the beginning of the text */
    RTE_FileOffset                  textBegin;
    /*! if the element is text: the length of the text */
    SAPDB_UInt                      textLength;
    /*! pointer to the attributes of the element, if any */
    Attribute                       *pAttributes;
    /*! pointer to the next element in the list */
    Msg_Element                     *pNextElement;
    /*! information about the part of the file (header/cyclic/footer) (deprecated) */
    Msg_FileProcessor::Part         part;

    /*!
        @brief  default ctor
    */
    Msg_Element()
        :tagID(Msg_FileProcessor::tagUNKNOWN)
        ,tagKind(Msg_FileProcessor::End)
        ,textBegin(0)
        ,textLength(0)
        ,pAttributes(0)
        ,pNextElement(0)
        ,part(Msg_FileProcessor::None)
    {};

    /*!
        @brief  dtor
    */
    ~Msg_Element()
    {
        Attribute *pNextAttribute = pAttributes;
        Attribute *pNextnextAttribute;
        while( pNextAttribute )
        {
            pNextnextAttribute = pNextAttribute->pnext;
            (RTEMem_Allocator::Instance()).Deallocate(pNextAttribute);
            pNextAttribute = pNextnextAttribute;
        }
    }
};

/*!
  @class Msg_Message
  @brief helper class for constructing a list of messages
 */
struct Msg_Message
{
    /*! the creation time of the message */
    SAPDB_UInt4             Time;
    /*! the creation year of the message */
    SAPDB_UInt              Year;
    /*! the creation month of the message */
    SAPDB_UInt              Month;
    /*! the creation day of the message */
    SAPDB_UInt              Day;
    /*! the offset of the beginning of the text of the message in the file */
    RTE_FileOffset          beginOfText;
    /*! the length of the text of the message in the file */
    SAPDB_UInt              lengthOfText;
    /*! the offset of the beginning of the text of the message in the file */
    RTE_FileOffset          beginOfId;
    /*! the length of the text of the message in the file */
    SAPDB_UInt              lengthOfId;
    /*! the offset of the beginning of the component of the message in the file */
    RTE_FileOffset          beginOfComponent;
    /*! the length of the component of the message in the file */
    SAPDB_UInt              lengthOfComponent;
    /*! the offset of the beginning of the type (error/warning/info) of the message in the file */
    RTE_FileOffset          beginOfType;
    /*! the length of the type (error/warning/info) of the message in the file */
    SAPDB_UInt              lengthOfType;
    /*! the offset of the beginning of the thread which wrote the message in the file */
    RTE_FileOffset          beginOfThread;  // this info is found 
    /*! the length of the thread which wrote the message in the file */
    SAPDB_UInt              lengthOfThread; // in the Message List!
    /*! the offset of the beginning of the task which wrote the message in the file */
    RTE_FileOffset          beginOfTask;    // this info is found 
    /*! the length of the task which wrote the message in the file */
    SAPDB_UInt              lengthOfTask;   // in the Message List!
    /*! the offset of the beginning of the label of the message in the file */
    RTE_FileOffset          beginOfLabel;
    /*! the length of the label of the message in the file */
    SAPDB_UInt              lengthOfLabel;
    /*! the part of the file the message belongs to (header/cyclic/footer) (deprecated) */
    Msg_FileProcessor::Part part;
    /*! the attribute list */
    Attribute               *pAttributes;
    /*! the number of elements in the attribute list */
    SAPDB_UInt4             numberOfAttributes;
    /*! the offset of the beginning of the attributes of the message in the file */
    RTE_FileOffset          beginOfAttributes;
    /*! the length of the attributes of the message in the file */
    SAPDB_UInt              lengthOfAttributes;
    /*! flag that indicates if the message is the first one in its message list */
    SAPDB_Bool              firstMessageInList;
    /*! the offset of the end of the xml code of the message */
    RTE_FileOffset          endOfXMLCode;

    /*! the next message in the list */
    Msg_Message *pNextMessage;

    /*!
        @brief  default ctor
    */
    Msg_Message()
        :pNextMessage(NULL)
        ,numberOfAttributes(0)
        ,lengthOfText(0)
        ,lengthOfId(0)
        ,lengthOfThread(0)
        ,lengthOfAttributes(0)
        ,lengthOfType(0)
        ,Time(0)
        ,Year(0)
        ,Month(0)
        ,Day(0)
        ,beginOfText(0)
        ,beginOfId(0)
        ,beginOfComponent(0)
        ,lengthOfComponent(0)
        ,beginOfType(0)
        ,beginOfThread(0)
        ,beginOfTask(0)
        ,part(Msg_FileProcessor::None)
        ,pAttributes(0)
        ,beginOfAttributes(0)
        ,beginOfLabel(0)
        ,firstMessageInList(false)
    {};

    /*!
        @brief  compare two messages by their creation date
        @param  other   [in]    the second message
        @return false if first message is older than second message, true if not
    */
    bool operator >= (const Msg_Message &other)
    {
        if(Year > other.Year)
            return true;
        else if(Year == other.Year)
        {
            if(Month > other.Month)
                return true;
            else if(Month == other.Month)
            {
                if(Day > other.Day)
                    return true;
                else if(Day == other.Day)
                {
                    if(Time >= other.Time)
                        return true;
                }
            }
        }
        return false;
    }

    /*!
        @brief  compare two messages by their creation date
        @param  other   [in]    the second message
        @return true if first message is younger than second message, false if not
    */
    bool operator > (const Msg_Message &other)
    {
        if(Year > other.Year)
            return true;
        else if(Year == other.Year)
        {
            if(Month > other.Month)
                return true;
            else if(Month == other.Month)
            {
                if(Day > other.Day)
                    return true;
                else if(Day == other.Day)
                {
                    if(Time > other.Time)
                        return true;
                }
            }
        }
        return false;
    }
};

/*!
  @class Msg_MessageList
  @brief helper class for storing a message list
 */
struct Msg_MessageList
{
    /*! the number of the message list */
    SAPDB_UInt              MessageListNo;
    /*! the part of the file the message belongs to (header/cyclic/footer) (deprecated) */
    Msg_FileProcessor::Part part;
    /*! the offset of the beginning of the thread which wrote the message in the file */
    RTE_FileOffset          beginOfThread;
    /*! the length of the thread which wrote the message in the file */
    SAPDB_UInt              lengthOfThread;
    /*! the offset of the beginning of the task which wrote the message in the file */
    RTE_FileOffset          beginOfTask;
    /*! the length of the task which wrote the message in the file */
    SAPDB_UInt              lengthOfTask;
    /*! pointer to the first message in the list */
    Msg_Message             *pFirstMessage;
    /*! pointer to the next message list in the chain */
    Msg_MessageList         *pNextMessageList;

    /*! the offset of the beginning of the xml code of the message list in the file */
    RTE_FileOffset          beginOfXMLCode;
    /*! the length of the xml code of the message list in the file */
    SAPDB_UInt              lengthOfXMLCode;

    /*! 
        @brief  default ctor 
    */
    Msg_MessageList()
        :pNextMessageList(NULL)
        ,pFirstMessage(NULL)
        ,lengthOfXMLCode(0)
    {};
};

/*!
  @class MessageAllocator
  @brief helper class for allocation of a chain of messages
 */
class MessageAllocator
{
public:
    /*!
        @brief  initialize the allocator
        @param  numberOfItems   [in]    the number of items in each block
    */
    void Initialize(SAPDB_UInt numberOfItems)
    {
        m_numberOfItemsPerBlock = numberOfItems;
        m_pNextAllocatedMessage = (Msg_Message *)RTEMem_Allocator::Instance().Allocate(m_numberOfItemsPerBlock * sizeof(Msg_Message));
        m_numberOfAllocatedMessages = 0;
    }

    /*!
        @brief  return a pointer to which a message can be stored
        @return the pointer
    */
    Msg_Message *AllocateMessage(void)
    {
        m_numberOfAllocatedMessages++;
        if(m_numberOfAllocatedMessages > m_numberOfItemsPerBlock)
        {
            m_pNextAllocatedMessage = (Msg_Message *)RTEMem_Allocator::Instance().Allocate(m_numberOfItemsPerBlock * sizeof(Msg_Message));
            m_numberOfAllocatedMessages = 0;
        }
        if(!m_pNextAllocatedMessage)
        {
            return NULL;
        }
        else
        {
            new(m_pNextAllocatedMessage)Msg_Message;
            return m_pNextAllocatedMessage++;
        }
    }

    /*!
        @brief default ctor
    */
    MessageAllocator():m_pNextAllocatedMessage(NULL),m_numberOfAllocatedMessages(0){}
private:
    Msg_Message *m_pNextAllocatedMessage;
    SAPDB_UInt m_numberOfItemsPerBlock;
    SAPDB_UInt m_numberOfAllocatedMessages;
};

/*! the allocator used for allocating message information */
static MessageAllocator messageAllocator;

/*! define for internal state during file tokenization */
#define NO_ATTRIBUTE        0
/*! define for internal state during file tokenization */
#define IN_ATTRIBUTE_NAME   1
/*! define for internal state during file tokenization */
#define WAITING_FOR_VALUE   2
/*! define for internal state during file tokenization */
#define WAITING_FOR_BECOMES 3
/*! define for internal state during file tokenization */
#define IN_ATTRIBUTE_VALUE  4

/*!
  @class Msg_FileScanner
  @brief helper class for tokenization of a file containing xml message lists
 */
class Msg_FileScanner
{
public:
    /*!
        @brief  ctor
        @param  prefilledBuffer [in]    address of a buffer which contains text to be tokenized
        @param  bufferSize      [in]    size of the buffer
        @param  maxOffset       [in]    if not zero: the maximum offset to read from the buffer
    */
    Msg_FileScanner( SAPDB_Byte *prefilledBuffer, RTE_FileOffset bufferSize,RTE_FileOffset maxOffset=0 )
        : m_sourceFile( 0 )
        , m_searchOffset( 0 )
        , m_endOffset( bufferSize )
        , m_numberOfBufferTurnarounds ( 0 )
        , m_maxOffset( maxOffset )
        , m_singleBufferSize( bufferSize )
        , m_buffer( prefilledBuffer )
        , m_deallocateBufferInDestructor( false )
    {
        m_endOffset = (SAPDB_UInt4)bufferSize;
        if( (m_maxOffset>0) && (CurrentPositionInFile() + bufferSize > m_maxOffset) )
        {
            m_endOffset = (SAPDB_UInt4)(m_maxOffset - CurrentPositionInFile());
        }
    }

    /*!
        @brief  ctor
        @param  sourceFile      [in]    handle of a file to read text to be tokenized from
        @param  bufferSize      [in]    size of the buffer
        @param  maxOffset       [in]    if not zero: the maximum offset to read from the buffer
    */
    Msg_FileScanner( RTE_FileHandle sourceFile, RTE_FileOffset bufferSize,RTE_FileOffset maxOffset=0 )
        : m_sourceFile( sourceFile )
        , m_searchOffset( 0 )
        , m_endOffset( 2 * bufferSize )
        , m_numberOfBufferTurnarounds ( 0 )
        , m_maxOffset( maxOffset )
        , m_singleBufferSize( bufferSize )
        , m_deallocateBufferInDestructor( true )
    {
        m_buffer = (SAPDB_Byte *)(RTEMem_Allocator::Instance()).Allocate((SAPDB_ULong)(2 * m_singleBufferSize));
        if(!m_buffer)
        {
            return;
        }
        memset(m_buffer,0,2 * (size_t)m_singleBufferSize);

        tRTESys_IOPosition BytesRead;
        tsp00_VfReturn_Param ReturnStatus;         
        RTESys_IORead(  m_sourceFile,
                        m_buffer,
                        m_singleBufferSize,
                        BytesRead,
                        ReturnStatus );
        if( m_singleBufferSize != BytesRead || vf_ok != ReturnStatus )
        {
            m_endOffset = (SAPDB_UInt4)BytesRead;
        }
        if( (m_maxOffset>0) && (CurrentPositionInFile() + BytesRead > m_maxOffset) )
        {
            SAPDB_Int i = 21;
            m_endOffset = (SAPDB_UInt4)(m_maxOffset - CurrentPositionInFile());
        }
    }

    /*!
        @brief  dtor
    */
    ~Msg_FileScanner()
    {
        if( m_deallocateBufferInDestructor )
        {
            (RTEMem_Allocator::Instance()).Deallocate(m_buffer);
        }
    }

    /*!
        @brief  search for a given tag
        @param  tag [in]    the tag to search for
        @param  err [out]   contains messages if errors occurred
        @return true if the tag could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SearchTag(const SAPDB_Char *tag,Msg_List &err)
    {
        SAPDB_UInt lengthOfSearchString = (SAPDB_UInt)strlen(tag)+1;

        SAPDB_UInt offsetInTag;
        do
        {
            if('<' == m_buffer[m_searchOffset])
            {
                break;
            }
            if(!isspace(m_buffer[m_searchOffset]))
            {
                return false;
            }
            if(!NextOffset(err))
                break;
        }
        while(1);    
        // check if the string at m_searchOffset matches the desired end tag
        for(offsetInTag=0 ; offsetInTag<lengthOfSearchString ; offsetInTag++)
        {
            if( 0 == offsetInTag )  // first character
            {
                if( '<' != m_buffer[m_searchOffset] )   // must be '<'
                {
                    break;
                }
            }
            else    // all the other characters must match the respective ones of the tag
            {
                if(toupper(m_buffer[m_searchOffset]) != toupper(tag[offsetInTag-1]))
                {
                    break;
                }
            }
            if(!NextOffset(err))
                break;
        }
        if(offsetInTag == lengthOfSearchString)
        {
            return true;
        }
        else
        {
            return false;
        }

    }

    /*!
        @brief  get the next element in the text
        @param  pElement    [out]   the element found (if any)
        @param  err         [out]   contains messages if errors occurred
        @return true if an element could be found, false if not (problem or end of text)
    */
    SAPDB_Bool NextElement(Msg_Element *&pElement,Msg_List &err)
    {
        if(NULL == m_endOffset)
            return false;
        pElement = NULL;
        // skip leading whitespaces
        do
        {
            if(!isspace(m_buffer[m_searchOffset]))
            {
                break;
            }
            if(!NextOffset(err))
                return false;
        }
        while(1);    
        if('<' != m_buffer[m_searchOffset]) // plain text or tag ?
        {   // plain text.
            pElement = new (RTEMem_Allocator::Instance())Msg_Element;
            pElement->pAttributes = NULL;
            pElement->textBegin = CurrentPositionInFile();
            pElement->tagID = Msg_FileProcessor::plainText;
            do
            {
                if('<' == m_buffer[m_searchOffset])
                {
                    break;
                }
                if(!NextOffset(err))
                    return false;

            }
            while(1);
            pElement->textLength = (SAPDB_UInt)(CurrentPositionInFile() - pElement->textBegin);
            return true;
        }
        // not plain text --> must be a tag
        pElement = new (RTEMem_Allocator::Instance())Msg_Element;
        pElement->pAttributes = NULL;
        pElement->tagID = Msg_FileProcessor::tagUNKNOWN;

        NextOffset(err);
        if(('<' == m_buffer[m_searchOffset])) // '<': tag overwritten by other tag (illegal, ignore)
            return true;
// a tag has been found. Check if it is a begin or end tag
        pElement->textBegin = CurrentPositionInFile()-1;    // CurrentPostion() is just after the '<'
        if('/' == m_buffer[m_searchOffset])
        {
            pElement->tagKind = Msg_FileProcessor::End;
            if(!NextOffset(err))
                return false;
            if(('<' == m_buffer[m_searchOffset])) // '<': tag overwritten by other tag (illegal, ignore)
                return true;
        }
        else
        {
            pElement->tagKind = Msg_FileProcessor::Begin;
        }
// check the name of the tag
        SAPDB_UInt positionInStrings = 0;
        SAPDB_UInt numberOfCandidates = numberOfKnownTags;
        SAPDB_Bool knownTagIsCandidate[numberOfKnownTags];
        for(SAPDB_UInt index = 0;index < numberOfKnownTags;index++)
        {
            knownTagIsCandidate[index]=true;
        }

        do
        {
            for(SAPDB_UInt index = 0;index < numberOfKnownTags;index++)
            {
                if(knownTagIsCandidate[index] && (knownTagNames[index][positionInStrings] != m_buffer[m_searchOffset]))
                {
                    knownTagIsCandidate[index] = false;
                    numberOfCandidates--;
                }
            }
            NextOffset(err);
            if(('<' == m_buffer[m_searchOffset])) // '<': tag overwritten by other tag (illegal, ignore)
                return true;
            positionInStrings++;
            if(isspace(m_buffer[m_searchOffset])||('>' == m_buffer[m_searchOffset])) // space: tag with attributes, '>': tag without attributes
            {
                for(SAPDB_UInt index = 0;index < numberOfKnownTags;index++)
                {
                    if(knownTagIsCandidate[index] && (knownTagNames[index][positionInStrings] != 0))
                    {
                        knownTagIsCandidate[index] = false;
                        numberOfCandidates--;
                    }
                }
                break;
            }
        }
        while(1);
        // the end of the tag has been reached.
        if( 0 == numberOfCandidates )
        {
            pElement->tagID = Msg_FileProcessor::tagUNKNOWN;
        }
        else if ( 1 == numberOfCandidates )
        {
            for(SAPDB_UInt index = 0;index < numberOfKnownTags;index++)
            {
                if(knownTagIsCandidate[index])
                {
                    pElement->tagID = (Msg_FileProcessor::xmlTagID)index;
                    break;
                }
            }
        }

        // read to the end of the tag
        SAPDB_UInt state = NO_ATTRIBUTE;
        do
        {
            if('>' == m_buffer[m_searchOffset])
            {
                break;
            }
            else if('=' == m_buffer[m_searchOffset])
            {
                if( IN_ATTRIBUTE_NAME == state )
                {
                    // end of name!
                    pElement->pAttributes->lengthOfName = (SAPDB_UInt)(CurrentPositionInFile() - pElement->pAttributes->beginOfName);
                    state = WAITING_FOR_VALUE;
                }
            }
            else if( '"'  == m_buffer[m_searchOffset])
            {
                if( IN_ATTRIBUTE_VALUE == state )
                {
                    // end of value!
                    pElement->pAttributes->lengthOfValue = (SAPDB_UInt)(CurrentPositionInFile() - pElement->pAttributes->beginOfValue);
                    state = NO_ATTRIBUTE;
                }
                else if ( WAITING_FOR_VALUE == state )
                {
                    // begin of value!
                    pElement->pAttributes->beginOfValue = CurrentPositionInFile()+1;
                    state = IN_ATTRIBUTE_VALUE;
                }
            }
            else if(!isspace(m_buffer[m_searchOffset]) && ('/' != m_buffer[m_searchOffset]))
            {
                if( NO_ATTRIBUTE == state )
                {
                    Attribute *pnewAttribute;
                    pnewAttribute = new(RTEMem_Allocator::Instance()) Attribute ;

                    pnewAttribute->pnext = pElement->pAttributes;
                    pElement->pAttributes = pnewAttribute;

                    pElement->pAttributes->beginOfName = CurrentPositionInFile();
                    state = IN_ATTRIBUTE_NAME;
                }
            }
            else
            {
                if( IN_ATTRIBUTE_NAME == state )
                {
                    // end of name!
                    pElement->pAttributes->lengthOfName = (SAPDB_UInt)(CurrentPositionInFile() - pElement->pAttributes->beginOfName);
                    state = WAITING_FOR_BECOMES;
                }
            }
            if(!NextOffset(err))
                return false;
            if(('<' == m_buffer[m_searchOffset])) // '<': tag overwritten by other tag (illegal, ignore)
                return true;
        }
        while(1);    
        if(!NextOffset(err))
            return false;
        pElement->textLength = (SAPDB_UInt)(CurrentPositionInFile() - pElement->textBegin);
        return true;
    }


    /*!
        @brief  search for a given numeric attribute in the text and return the value if found
        @param  attributeName   [in]    the name of the attribute to search for
        @param  attributeValue  [out]   contains the value of the attribute if found
        @param  err             [out]   contains messages if errors occurred
        @return true if an element could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SearchNumericAttribute(const SAPDB_Char *attributeName,SAPDB_UInt8 &attributeValue,Msg_List &err)
    {
        SAPDB_Char attributeValueAsText[256];
        SAPDB_Bool returnValue = SearchTextAttribute(attributeName,attributeValueAsText,256,err);
        attributeValue = atoi(attributeValueAsText);
        return returnValue;

    }

    /*!
        @brief  search for a given attribute in the text and return the value (as text) if found
        @param  attributeName       [in]    the name of the attribute to search for
        @param  pattributeValue     [in]    address of a buffer to write the attribute value to
        @param  destinationLength   [in]    size of the buffer
        @param  err                 [out]   contains messages if errors occurred
        @return true if an element could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SearchTextAttribute(const SAPDB_Char *attributeName,SAPDB_Char *pattributeValue,SAPDB_UInt4 destinationLength,Msg_List &err)
    {
        SAPDB_UInt lengthOfSearchString = (SAPDB_UInt)strlen(attributeName);
        const SAPDB_Char *searchString = attributeName;

        SAPDB_UInt offsetInTag;
        for(;;)
        {
            for(offsetInTag=0 ; offsetInTag<lengthOfSearchString ; offsetInTag++)
            {
                if(toupper(m_buffer[m_searchOffset]) != toupper(searchString[offsetInTag]))
                {
                    break;
                }
                if(!NextOffset(err))
                    break;
            }
            if(offsetInTag == lengthOfSearchString)
            {
                break;
            }
            else
            {
                offsetInTag = 0;
            }
            if(!NextOffset(err))
                break;
        }

        if(offsetInTag == lengthOfSearchString)
        {
            do
            {
                if('=' == m_buffer[m_searchOffset])
                {
                    break;
                }
                if(!isspace(m_buffer[m_searchOffset]))
                {
                    return false;
                }
                if(!NextOffset(err))
                    break;
            }
            while(1);
            NextOffset(err);

            do
            {
                if('"' == m_buffer[m_searchOffset])
                {
                    break;
                }
                if(!isspace(m_buffer[m_searchOffset]))
                {
                    return false;
                }
                if(!NextOffset(err))
                    break;
            }
            while(1);
            NextOffset(err);

            SAPDB_UInt beginOfValue = m_searchOffset;
            SAPDB_UInt offsetInDestination = 0;
            do
            {
                if('"' == m_buffer[m_searchOffset])
                {
                    break;
                }
                pattributeValue[offsetInDestination] = m_buffer[m_searchOffset];
                offsetInDestination++;
                if(destinationLength == offsetInDestination)
                    break;

                if(!NextOffset(err))
                    break;
            }
            while(1);
            if(destinationLength != offsetInDestination)
            {
                pattributeValue[offsetInDestination] = 0;
            }
            NextOffset(err);

            return true;
        }

        // kann ich noch nicht
        return false;
    }

    /*!
        @brief  ignore all text until a given begin tag is found
        @param  tag [in]    the tag to search for
        @param  err [out]   contains messages if errors occurred
        @return true if the tag could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SkipToBeginTag(const SAPDB_Char *tag,Msg_List &err)
    {
        SAPDB_UInt lengthOfSearchString = (SAPDB_UInt)strlen(tag)+1;
        SAPDB_Char *searchString = (SAPDB_Char *)alloca(lengthOfSearchString+1);
        SAPDB_sprintf(searchString,lengthOfSearchString+1,"<%s",tag);

        SAPDB_UInt offsetInTag;
        for(;;)
        {
            for(offsetInTag=0 ; offsetInTag<lengthOfSearchString ; offsetInTag++)
            {
                if(toupper(m_buffer[m_searchOffset]) != toupper(searchString[offsetInTag]))
                {
                    break;
                }
                if(!NextOffset(err))
                    break;
            }
            if(offsetInTag == lengthOfSearchString)
            {
                if(isspace(m_buffer[m_searchOffset]) || ('>' == m_buffer[m_searchOffset]))
                    return true;
            }
            offsetInTag = 0;    // continue...
            if(!NextOffset(err))
                break;
        }
        return false;
    }

    /*!
        @brief  ignore all text until a given end tag is found
        @param  tag     [in]    the tag to search for
        @param  textEnd [out]   the offset of the end of the text ended by this tag (position before the '<' of the end tag)
        @param  err     [out]   contains messages if errors occurred
        @return true if the tag could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SkipToEndTag(const SAPDB_Char *tag,RTE_FileOffset &textEnd, Msg_List &err )
    {
        SAPDB_UInt lengthOfSearchString = (SAPDB_UInt)strlen(tag)+3;
        SAPDB_UInt offsetInTag;
        textEnd = CurrentPositionInFile();
        for(;;)
        {
            if(isspace(m_buffer[m_searchOffset]))
            {
                if(!NextOffset(err))
                    break;
                continue;
            }
            // check if the string at m_searchOffset matches the desired end tag
            for(offsetInTag=0 ; offsetInTag<lengthOfSearchString ; offsetInTag++)
            {
                if( 0 == offsetInTag )  // first character
                {
                    if( '<' != m_buffer[m_searchOffset] )   // must be '<'
                    {
                        break;
                    }
                }
                else if( 1 == offsetInTag ) // second character
                {
                    if( '/' != m_buffer[m_searchOffset] )   // must be '/'
                    {
                        break;
                    }
                }
                else if( lengthOfSearchString-1 == offsetInTag )  // last character 
                {
                    if( '>' != m_buffer[m_searchOffset] )   // must be '>'
                    {
                        break;
                    }
                }
                else    // all the other characters must match the respective ones of the tag
                {
                    if(toupper(m_buffer[m_searchOffset]) != toupper(tag[offsetInTag-2]))
                    {
                        break;
                    }
                }
                if(!NextOffset(err))
                    break;
            }
            if(offsetInTag == lengthOfSearchString)
            {
                return true;
            }
            else
            {
                offsetInTag = 0;
            }
            if(!NextOffset(err))
                break;
            textEnd = CurrentPositionInFile();
        }
        return false;
    }

    /*!
        @brief  ignore all text until a CDATA section end ("]]>") is found
        @param  textEnd [out]   the offset of the end of the text of the CDATA section (position before the first ']' of "]]>")
        @param  err     [out]   contains messages if errors occurred
        @return true if the tag could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SkipToCDATAEnd(RTE_FileOffset &textEnd, Msg_List &err )
    {
        const SAPDB_Char *tag = "]]>";
        SAPDB_UInt lengthOfSearchString = (SAPDB_UInt)strlen(tag);
        SAPDB_UInt offsetInTag;
        textEnd = CurrentPositionInFile();
        for(;;)
        {
            if(isspace(m_buffer[m_searchOffset]))
            {
                if(!NextOffset(err))
                    break;
                continue;
            }
            // check if the string at m_searchOffset matches the desired end tag
            for(offsetInTag=0 ; offsetInTag<lengthOfSearchString ; offsetInTag++)
            {
                {
                    if(toupper(m_buffer[m_searchOffset]) != toupper(tag[offsetInTag]))
                    {
                        break;
                    }
                }
                if(!NextOffset(err))
                    break;
            }
            if(offsetInTag == lengthOfSearchString)
            {
                return true;
            }
            else
            {
                offsetInTag = 0;
            }
            if(!NextOffset(err))
                break;
            textEnd = CurrentPositionInFile();
        }
        return false;
    }

    /*!
        @brief  ignore all text until the end of the current tag (the '>') is found
        @param  err     [out]   contains messages if errors occurred
        @return true if the tag could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SkipToTagEnd( Msg_List &err )
    {
        for(;;)
        {
            if(m_buffer[m_searchOffset] == '>')
            {
                if(!NextOffset(err))
                    return false;
                return true;
            }
            if(!NextOffset(err))
                break;
        }
        return false;
    }

    /*!
        @brief  ignore all text until a non-whitespace is found
        @param  err     [out]   contains messages if errors occurred
        @return true if any non-whitespace could be found, false if not (problem or end of text)
    */
    SAPDB_Bool SkipWhiteSpaces( Msg_List &err )
    {
        for(;;)
        {
            if(!isspace(m_buffer[m_searchOffset]))
            {
                return true;
            }
            if(!NextOffset(err))
                break;
        }
        return false;
    }

    /*!
        @brief  the current position in the text
        @return the position
    */
    RTE_FileOffset CurrentPositionInFile(void)
    {
        return m_searchOffset + (2*m_singleBufferSize*m_numberOfBufferTurnarounds);
    }
private:
    SAPDB_Byte *m_buffer;
    RTE_FileOffset m_singleBufferSize;
    RTE_FileOffset m_endOffset;
    SAPDB_UInt4 m_searchOffset;
    RTE_FileOffset m_maxOffset;
    SAPDB_UInt4 m_numberOfBufferTurnarounds;
    RTE_FileHandle  m_sourceFile;
    SAPDB_Bool m_deallocateBufferInDestructor;

    SAPDB_Bool NextOffset( Msg_List &err )
    {
        m_searchOffset++;
        if(m_searchOffset > m_endOffset)
        {
            return false;
        }
        if(m_singleBufferSize == m_searchOffset)
        {
            tRTESys_IOPosition BytesRead;
            tsp00_VfReturn_Param ReturnStatus;         

            if( 0 == m_sourceFile)  // this means that the buffer should have been filled completely by the caller. 
                                    // An attempt to read more cannot be correct...
            {
                err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"try to read after end of buffer");
                return false;
            }
            RTESys_IORead(  m_sourceFile,
                            m_buffer + m_singleBufferSize,
                            m_singleBufferSize,
                            BytesRead,
                            ReturnStatus );
            if( m_singleBufferSize != BytesRead || vf_ok != ReturnStatus )
            {
                m_endOffset = (SAPDB_UInt4)BytesRead + m_singleBufferSize;
            }
            if( (m_maxOffset>0) && (CurrentPositionInFile() + BytesRead > m_maxOffset) )
            {
                SAPDB_Int i = 21;
                m_endOffset = (SAPDB_UInt4)(m_maxOffset - CurrentPositionInFile()) + m_singleBufferSize;
            }
        }
        if(2 * m_singleBufferSize == m_searchOffset)
        {
            if( 0 == m_sourceFile)  // this means that the buffer should have been filled completely by the caller. 
                                    // An attempt to read more cannot be correct...
            {
                err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"try to read after end of buffer");
                return false;
            }
            tRTESys_IOPosition BytesRead;
            tsp00_VfReturn_Param ReturnStatus;         
            RTESys_IORead(  m_sourceFile,
                            m_buffer,
                            m_singleBufferSize,
                            BytesRead,
                            ReturnStatus );
            if( m_singleBufferSize != BytesRead || vf_ok != ReturnStatus )
            {
                m_endOffset = (SAPDB_UInt4)BytesRead;
            }
            if( (m_maxOffset>0) && (CurrentPositionInFile() + BytesRead > m_maxOffset) )
            {
                SAPDB_Int i = 21;
                m_endOffset = (SAPDB_UInt4)(m_maxOffset - CurrentPositionInFile());
            }
            m_searchOffset = 0;
            m_numberOfBufferTurnarounds++;
        }
        return true;
    }
};

/*! SearchTag() uses two buffers. This defines the size of each of them */
#define SINGLE_BUFFERSIZE_FOR_SEARCH_TAG 4011

/*!
  @class Msg_DescriptionStorage
  @brief helper class for storing message descriptions
 */
class Msg_DescriptionStorage
{
public:
    /*!
        @brief a reference to the one and only storage for descriptions and actions
    */
    static Msg_DescriptionStorage& Instance()
    {
        SAPDBMEM_STATIC_RAW_ALLOCATE(MsgDescriptionStorage_InstanceData, sizeof(Msg_DescriptionStorage));
        if(!m_Instance)
        {
            m_Instance = new Msg_DescriptionStorage();
        }
        return *m_Instance;
    }

    /*!
        @brief  get short text, description and action for a given message
        @param  pComponentName          [in]    the component of the message
        @param  messageID               [in]    the id of the message
        @param  bufferForText           [out]   pointer of the short text for this message - NULL if not available
        @param  bufferForDescription    [out]   pointer of the description for this message - NULL if not available
        @param  bufferForAction         [out]   pointer of the action for this message - NULL if not available
    */
    SAPDB_Bool GetShortTextAndDescription   (SAPDB_Char *pComponentName
                                            ,SAPDB_UInt8 messageID
                                            ,SAPDB_Char *&bufferForText
                                            ,SAPDB_Char *&bufferForDescription
                                            ,SAPDB_Char *&bufferForAction)
    {
        Msg_Component *pComponent = SearchInList(pComponentName);
        SAPDB_UInt neededSizeForText = 0;
        SAPDB_UInt neededSizeForDescription = 0;
        SAPDB_UInt neededSizeForAction = 0;
        if(!pComponent)
        {
            pComponent = new(RTEMem_Allocator::Instance())Msg_Component;
            pComponent->m_pNext = m_pFirstComponent;
            m_pFirstComponent = pComponent;
            SAPDB_Char *pNewComponent = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(strlen(pComponentName)+1);
            if(!pNewComponent)
            {
                return false;
            }
            SAPDB_sprintf(pNewComponent,(SAPDB_UInt)strlen(pComponentName)+1,"%s",pComponentName);
            pComponent->m_pComponentName = pNewComponent;

            tsp01_RteError RteError;
            tsp00_Pathc MsgPath;
            sqlGetIndependentMsgPath      ( MsgPath,
                                            TERM_WITH_DELIMITER_EO01,
                                            &RteError ) ;
            RTE_FileHandle hendl;
            RTE_Path fileName;
            tsp00_VfReturn_Param ReturnStatus;
            SAPDB_sprintf(fileName,sizeof(fileName),"%sSDBMsg_%s.xml",MsgPath.asCharp(),pComponentName);
            RTESys_IOOpen ( hendl,
                            fileName,
                            RTESys_IOReadOnly,
                            false,
                            0,
                            ReturnStatus );
            if(vf_ok == ReturnStatus)
            {
                /* TODO: build index... */
                SAPDB_UInt4 biggestId;
                Msg_List errList;
                SearchBiggestMessageID( hendl,biggestId,errList );
                

                pComponent->m_biggestId = biggestId;
                pComponent->m_pTextPositionArray = (Msg_Position *)(RTEMem_Allocator::Instance()).Allocate(biggestId*sizeof(Msg_Position));
                memset(pComponent->m_pTextPositionArray,0,biggestId*sizeof(Msg_Position));
                if(!pComponent->m_pTextPositionArray)
                {
                    return false;
                }

                pComponent->m_pDescriptionPositionArray = (Msg_Position *)(RTEMem_Allocator::Instance()).Allocate(biggestId*sizeof(Msg_Position));
                memset(pComponent->m_pDescriptionPositionArray,0,biggestId*sizeof(Msg_Position));
                if(!pComponent->m_pDescriptionPositionArray)
                {
                    return false;
                }

                pComponent->m_pActionPositionArray = (Msg_Position *)(RTEMem_Allocator::Instance()).Allocate(biggestId*sizeof(Msg_Position));
                memset(pComponent->m_pActionPositionArray,0,biggestId*sizeof(Msg_Position));
                if(!pComponent->m_pActionPositionArray)
                {
                    return false;
                }

                RTE_FileOffset newPosition;
                RTESys_IOSeek ( hendl,
                                0,
                                RTESys_IOSeekSet,
                                newPosition,
                                ReturnStatus );
                ReadPositions   ( hendl
                                , pComponent->m_pTextPositionArray
                                , pComponent->m_pDescriptionPositionArray
                                , pComponent->m_pActionPositionArray
                                , errList );

                RTE_FileOffset fileSize;
                RTESys_IOSeek ( hendl,
                                0,
                                RTESys_IOSeekEnd,
                                fileSize,
                                ReturnStatus );
                pComponent->m_fileBuffer = (SAPDB_Char*)(RTEMem_Allocator::Instance()).Allocate((SAPDB_ULong)fileSize);
                if(!pComponent->m_fileBuffer)
                {
                    return false;
                }
                RTESys_IOSeek ( hendl,
                                0,
                                RTESys_IOSeekSet,
                                newPosition,
                                ReturnStatus );
                tRTESys_IOPosition BytesRead;
                RTESys_IORead(  hendl,
                                pComponent->m_fileBuffer,
                                fileSize,
                                BytesRead,
                                ReturnStatus );
                RTESys_IOClose(hendl,ReturnStatus);
                
                pComponent->m_Available = true;
            }
            else
            {
                pComponent->m_Available = false;
            }
        }
        if(pComponent->m_Available)
        {
            if( (messageID > pComponent->m_biggestId) || (0 == messageID ))
            {
                return false;
            }

            neededSizeForText = pComponent->m_pTextPositionArray[messageID-1].m_length;
            if( 0 == neededSizeForText )
            {
                return false;
            }
            bufferForText = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(neededSizeForText+1);
            SAPDB_strncpy_zero_terminated(bufferForText,pComponent->m_fileBuffer + pComponent->m_pTextPositionArray[messageID-1].m_start,neededSizeForText);

            neededSizeForDescription = pComponent->m_pDescriptionPositionArray[messageID-1].m_length;
            if( neededSizeForDescription > 1 )
            {
                bufferForDescription = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(neededSizeForDescription+1);
                SAPDB_strncpy_zero_terminated(bufferForDescription,pComponent->m_fileBuffer + pComponent->m_pDescriptionPositionArray[messageID-1].m_start,neededSizeForDescription);
            }

            neededSizeForAction = pComponent->m_pActionPositionArray[messageID-1].m_length;
            if( neededSizeForAction > 1 )
            {
                bufferForAction = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(neededSizeForAction+1);
                SAPDB_strncpy_zero_terminated(bufferForAction,pComponent->m_fileBuffer + pComponent->m_pActionPositionArray[messageID-1].m_start,neededSizeForAction);
            }
        }
        return pComponent->m_Available;
    }

private:
    static Msg_DescriptionStorage *m_Instance;

    Msg_Component *m_pFirstComponent;

    Msg_DescriptionStorage()
        : m_pFirstComponent(NULL)
    {
    }

    SAPDB_Bool SearchBiggestMessageID( RTE_FileHandle hendl,SAPDB_UInt4 &biggestId,Msg_List &errList )
    {
        Msg_FileScanner scanner( hendl,SINGLE_BUFFERSIZE_FOR_SEARCH_TAG );
        SAPDB_UInt8 id;
        RTE_FileOffset textEnd;

        biggestId = 0;
        for(;;)
        {
            if(GARBAGE == scanner.SkipToBeginTag(MSG_LIST_MESSAGE_TAG,errList))
                break;

            if(NOT_FOUND == scanner.SearchNumericAttribute(MSG_FILE_ID_TAG,id,errList))
                break;

            if(id > biggestId)
                biggestId = (SAPDB_UInt4)id;

            if(GARBAGE == scanner.SkipToEndTag(MSG_LIST_MESSAGE_TAG,textEnd,errList))
                break;
        }
        return true;
    }

    SAPDB_Bool ReadPositions( RTE_FileHandle hendl,Msg_Position *pTextPositionArray,Msg_Position *pDescriptionPositionArray,Msg_Position *pActionPositionArray,Msg_List &errList )
    {
        Msg_FileScanner scanner( hendl,SINGLE_BUFFERSIZE_FOR_SEARCH_TAG );
        SAPDB_UInt8 id;
        RTE_FileOffset textEnd;
        for(;;)
        {
            if(GARBAGE == scanner.SkipToBeginTag(MSG_LIST_MESSAGE_TAG,errList))
                break;

            if(NOT_FOUND == scanner.SearchNumericAttribute(MSG_FILE_ID_TAG,id,errList))
                break;


            if(GARBAGE == scanner.SkipToBeginTag(MSG_FILE_SHORTTEXT_TAG,errList))
                break;

            if(GARBAGE == scanner.SkipToTagEnd(errList))
                break;
//TODO: skip whitespaces
            if(GARBAGE == scanner.SkipWhiteSpaces(errList))
                break;

            pTextPositionArray[id-1].m_start = scanner.CurrentPositionInFile();
            if(GARBAGE == scanner.SkipToEndTag(MSG_FILE_SHORTTEXT_TAG,textEnd,errList))
                break;
            pTextPositionArray[id-1].m_length = (SAPDB_UInt4)(textEnd - pTextPositionArray[id-1].m_start + 1);

            Msg_Element *pElement;
            
            if(!scanner.NextElement(pElement,errList))
                break;

            if (( Msg_FileProcessor::tagDESCRIPTION == pElement->tagID ) && ( Msg_FileProcessor::Begin == pElement->tagKind ))
            {
                pElement->~Msg_Element();
                (RTEMem_Allocator::Instance()).Deallocate(pElement);

                //skip over <![CDATA[
                if(GARBAGE == scanner.SkipToBeginTag(MSG_CDATA_BEGIN_TAG,errList))
                    break;
                
                if(GARBAGE == scanner.SkipWhiteSpaces(errList))
                    break;

                pDescriptionPositionArray[id-1].m_start = scanner.CurrentPositionInFile();
                
                //skip until ]]> is found
                if(GARBAGE == scanner.SkipToCDATAEnd(textEnd, errList ))
                    break;

                pDescriptionPositionArray[id-1].m_length = (SAPDB_UInt4)(textEnd - pDescriptionPositionArray[id-1].m_start + 1);

                if(GARBAGE == scanner.SkipToEndTag(MSG_FILE_DESCRIPTION_TAG,textEnd,errList))
                    break;

                if(!scanner.NextElement(pElement,errList))
                    break;
            }

            if (( Msg_FileProcessor::tagACTION == pElement->tagID ) && ( Msg_FileProcessor::Begin == pElement->tagKind ))
            {
                pElement->~Msg_Element();
                (RTEMem_Allocator::Instance()).Deallocate(pElement);

                //skip over <![CDATA[
                if(GARBAGE == scanner.SkipToBeginTag(MSG_CDATA_BEGIN_TAG,errList))
                    break;
                if(GARBAGE == scanner.SkipWhiteSpaces(errList))
                    break;

                pActionPositionArray[id-1].m_start = scanner.CurrentPositionInFile();
                
                //skip until ]]> is found
                if(GARBAGE == scanner.SkipToCDATAEnd(textEnd, errList ))
                    break;

                pActionPositionArray[id-1].m_length = (SAPDB_UInt4)(textEnd - pActionPositionArray[id-1].m_start + 1);

                if(GARBAGE == scanner.SkipToEndTag(MSG_FILE_ACTION_TAG,textEnd,errList))
                    break;

                if(!scanner.NextElement(pElement,errList))
                    break;
            }

/*********************************/
            if (!(( Msg_FileProcessor::tagMSG == pElement->tagID ) && ( Msg_FileProcessor::End == pElement->tagKind )))
                break;
            pElement->~Msg_Element();
            (RTEMem_Allocator::Instance()).Deallocate(pElement);
        }
        return true;
    }

    Msg_Component *SearchInList(SAPDB_Char *pComponentName)
    {
        Msg_Component *pComponent = m_pFirstComponent;
        while(NULL != pComponent)
        {
            if(0 == strcmp(pComponentName,pComponent->m_pComponentName))
            {
                break;
            }
            pComponent = pComponent->m_pNext;
        }
        return pComponent;
    }

};
Msg_DescriptionStorage *Msg_DescriptionStorage::m_Instance=NULL;

/*
  Processing of generated pseudo-xml message list files  
 */
Msg_FileProcessor::Msg_FileProcessor(const SAPDB_Char *fileName,Msg_IOutput::OutputStyle style,SAPDB_Bool longColumns)
        : m_style(style)
        , m_pFirstElementOfHeader(NULL)
        , m_pFirstElementOfCyclic(NULL)
        , m_pFirstElementOfFooter(NULL)
        , m_pFirstMessageList(NULL)     // relevant if sorted by message list no
        , m_pLastMessageList(NULL)      // relevant if sorted by message list no
        , m_pFirstMessage(NULL)         // relevant if sorted by message time stamp
        , m_pLastMessage(NULL)          // relevant if sorted by message time stamp
        , m_pInsertPosition(NULL)
        , m_destinationFile(0)
        , m_longColumns(longColumns)
        , m_elementCounter(0)
{
    SAPDB_strncpy_zero_terminated(m_fileName,fileName,sizeof(RTE_Path));
    memset(&m_fileBuffer[0],0,sizeof(m_fileBuffer));
}

SAPDB_Bool Msg_FileProcessor::SearchWritePosition(RTE_FileOffset &writePosition,SAPDB_UInt8 &biggestSequenceNumber,SAPDB_Bool continuous,Msg_List &errList)
{
    Part part;
    if(Msg_IOutput::Triple == m_style)
        part = Cyclic;
    else
        part = Complete;
    if(OpenSourceFile(part,errList))
    {
        tsp00_VfReturn_Param ReturnStatus;
        SAPDB_UInt8 lengthOfGarbage;
        if( continuous )    // If we know that the file has been written continuously (not cyclically), the message list with the biggest sequence number must be
        {                   // at the end of the file. So it it not necessary to search the whole file from the beginning
            RTE_FileOffset searchStartPosition = 0;
            RTE_FileOffset writeOffset;
            SAPDB_Int4 attempts = 0;

            RTE_FileOffset fileSize;
            RTESys_IOSeek ( m_sourceFile,
                            0,
                            RTESys_IOSeekEnd,
                            fileSize,
                            ReturnStatus );
            // Start searching for the message list with the biggest sequence number at a given offset from the end of the file.
            // If no complete message list is found, start again at twice the given offset from the end of the file.
            // Repeat so until a message list is found or the beginning of the file is reached.
            do
            {
                attempts++;
                RTESys_IOSeek ( m_sourceFile,
                                SAPDB_MAX(0, fileSize - (SEARCH_OFFSET_ * attempts)),
                                RTESys_IOSeekSet,
                                searchStartPosition,
                                ReturnStatus );

                if(!SearchLastWrittenMessageList(0,writeOffset,biggestSequenceNumber,lengthOfGarbage,errList))
                    return false;
            }
            while( 0 == biggestSequenceNumber && searchStartPosition > 0 );
            writePosition = searchStartPosition + writeOffset;
        }
        else
        {
            if(!SearchLastWrittenMessageList(0,writePosition,biggestSequenceNumber,lengthOfGarbage,errList))
                return false;
        }
        CloseSourceFile(errList);
    }
    else
    {
        biggestSequenceNumber = 0;
        writePosition = 0;
    }
    return true;
}

void Msg_FileProcessor::GetTextFromFile(Part part,RTE_FileOffset begin,SAPDB_UInt length,SAPDB_Char *destination,SAPDB_UInt sizeOfDestination)
{
    tsp00_VfReturn_Param ReturnStatus;
    RTE_FileOffset NewPosition;
    RTE_FileHandle fileHandle;
    switch(part)
    {
    case Header:
        fileHandle = m_headerFile;
        break;
    case Cyclic:
        fileHandle = m_cyclicFile;
        break;
    case Footer:
        fileHandle = m_footerFile;
        break;
    case Complete:
        fileHandle = m_completeFile;
        break;
    default:
        fileHandle = INVALID_HANDLE_RTESYS_IO; // only for -Wall
        break;
    }
    RTESys_IOSeek ( fileHandle,begin,RTESys_IOSeekSet,NewPosition,ReturnStatus );
    RTE_FileOffset ReceivedSizeInBytes/*=0*/;
    RTESys_IORead ( fileHandle,destination,SAPDB_MIN(length,sizeOfDestination-1),ReceivedSizeInBytes,ReturnStatus );
    destination[ReceivedSizeInBytes]=0;
}

void Msg_FileProcessor::OutputTextFromFile(Part part,RTE_FileOffset begin,SAPDB_UInt length)
{
    tsp00_VfReturn_Param ReturnStatus;
    SAPDB_Char *space = (SAPDB_Char *)alloca(length+1);
    RTE_FileOffset NewPosition;
    RTE_FileHandle fileHandle;
    switch(part)
    {
    case Header:
        fileHandle = m_headerFile;
        break;
    case Cyclic:
        fileHandle = m_cyclicFile;
        break;
    case Footer:
        fileHandle = m_footerFile;
        break;
    default:
        fileHandle = INVALID_HANDLE_RTESYS_IO; // only for -Wall
        break;
    }
    RTESys_IOSeek ( fileHandle,begin,RTESys_IOSeekSet,NewPosition,ReturnStatus );
    RTE_FileOffset ReceivedSizeInBytes;
    RTESys_IORead ( fileHandle,space,length,ReceivedSizeInBytes,ReturnStatus );
    space[length]=0;
  
    DoOutput(SAPDB_ToString(space));
}

SAPDB_Bool Msg_FileProcessor::AttributeMatchesTag(Part part,Attribute &attribute,Tag &tag)
{
    if(attribute.lengthOfName != tag.m_lengthOfName)
        return false;
    return 0 == strncmp(m_fileBuffer[part]+attribute.beginOfName,tag.m_name,attribute.lengthOfName);
}


Msg_Message *Msg_FileProcessor::ProcessMSG(Msg_Element *pElement,RTE_FileOffset beginOfThread,SAPDB_UInt lengthOfThread,RTE_FileOffset beginOfTask,SAPDB_UInt lengthOfTask)
{
    Msg_Message *pNewMessage = messageAllocator.AllocateMessage();
    if( !pNewMessage )
    {
        return NULL;
    }
    pNewMessage->part = pElement->part;
    pNewMessage->beginOfThread  = beginOfThread;
    pNewMessage->lengthOfThread = lengthOfThread;
    pNewMessage->beginOfTask    = beginOfTask;
    pNewMessage->lengthOfTask   = lengthOfTask;
    while(pElement && !( tagMSG == pElement->tagID && Begin == pElement->tagKind))
    {
        pElement = pElement->pNextElement;
    }

    Attribute *pCurrentAttribute = pElement->pAttributes;
    while(pCurrentAttribute)
    {
        if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagText))
        {
            pNewMessage->beginOfText = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfText = pCurrentAttribute->lengthOfValue;
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagId))
        {
            pNewMessage->beginOfId = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfId = pCurrentAttribute->lengthOfValue;
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagComponent))
        {
            pNewMessage->beginOfComponent = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfComponent = pCurrentAttribute->lengthOfValue;
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagType))
        {
            pNewMessage->beginOfType = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfType = pCurrentAttribute->lengthOfValue;
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagLabel))
        {
            pNewMessage->beginOfLabel = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfLabel = pCurrentAttribute->lengthOfValue;
        }
        pCurrentAttribute = pCurrentAttribute->pnext;
    }

    while(pElement && !( tagMSG_ARGS == pElement->tagID && Begin == pElement->tagKind))
    {
        pElement = pElement->pNextElement;
    }

    pNewMessage->pAttributes = pElement->pAttributes;
    pCurrentAttribute = pElement->pAttributes;
    pNewMessage->beginOfAttributes = (RTE_FileOffset)((SAPDB_MAX_UINT4));
    SAPDB_UInt4 end = 0;
    while(pCurrentAttribute)
    {
        pNewMessage->beginOfAttributes = SAPDB_MIN( pNewMessage->beginOfAttributes, (SAPDB_UInt4)pCurrentAttribute->beginOfName );
        end = SAPDB_MAX ( end, (SAPDB_UInt4)pCurrentAttribute->beginOfValue + pCurrentAttribute->lengthOfValue );
        pNewMessage->numberOfAttributes++;
        if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagTimeStamp))
        {
            SAPDB_Char  *TimeStamp = m_fileBuffer[pElement->part] + pCurrentAttribute->beginOfValue;
            pNewMessage->Year = (TimeStamp[0]-'0') *     1000
                              + (TimeStamp[1]-'0') *      100
                              + (TimeStamp[2]-'0') *       10
                              + (TimeStamp[3]-'0');
            pNewMessage->Month= (TimeStamp[5]-'0') *       10
                              + (TimeStamp[6]-'0');
            pNewMessage->Day  = (TimeStamp[8]-'0') *       10
                              + (TimeStamp[9]-'0');
            pNewMessage->Time = (TimeStamp[11]-'0') * 36000000
                              + (TimeStamp[12]-'0') *  3600000
                              + (TimeStamp[14]-'0') *   600000
                              + (TimeStamp[15]-'0') *    60000
                              + (TimeStamp[17]-'0') *    10000
                              + (TimeStamp[18]-'0') *     1000
                              + (TimeStamp[20]-'0') *      100
                              + (TimeStamp[21]-'0') *       10
                              + (TimeStamp[22]-'0');
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagLabel))
        {
            pNewMessage->beginOfLabel = pCurrentAttribute->beginOfValue;
            pNewMessage->lengthOfLabel = pCurrentAttribute->lengthOfValue;
        }
        pCurrentAttribute = pCurrentAttribute->pnext;
    }
    pNewMessage->lengthOfAttributes = (SAPDB_UInt)(end-pNewMessage->beginOfAttributes);
    while(pElement && !( tagMSG == pElement->tagID && End == pElement->tagKind))
    {
        pElement = pElement->pNextElement;
    }
    pNewMessage->endOfXMLCode = pElement->textBegin + pElement->textLength; // for xml output
    return pNewMessage;
}

Msg_MessageList *Msg_FileProcessor::AnalyzeMSGLAttributes(Msg_MessageList *pNewMessageList,Msg_Element *pElement,RTE_FileOffset &beginOfCurrentThread,SAPDB_UInt &lengthOfCurrentThread,RTE_FileOffset &beginOfCurrentTask,SAPDB_UInt &lengthOfCurrentTask)
{
    beginOfCurrentThread    = 0;    // assume that the MSGL has no thread attribute until one is found
    lengthOfCurrentThread   = 0;
    beginOfCurrentTask      = 0;    // assume that the MSGL has no thread attribute until one is found
    lengthOfCurrentTask     = 0;
    Attribute *pCurrentAttribute = pElement->pAttributes;
    while(pCurrentAttribute)
    {
        if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagNumber))
        {
            pNewMessageList->MessageListNo = atoi(m_fileBuffer[pElement->part]+pCurrentAttribute->beginOfValue);
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagThreadId))
        {
            beginOfCurrentThread = pCurrentAttribute->beginOfValue;
            lengthOfCurrentThread = pCurrentAttribute->lengthOfValue;
        }
        else if(AttributeMatchesTag(pElement->part,*pCurrentAttribute,tagTaskId))
        {
            beginOfCurrentTask = pCurrentAttribute->beginOfValue;
            lengthOfCurrentTask = pCurrentAttribute->lengthOfValue;
        }
        pCurrentAttribute = pCurrentAttribute->pnext;
    }
    return pNewMessageList;
}


Msg_MessageList *Msg_FileProcessor::ProcessMSGL(Msg_Element *&pElement)
{
    RTE_FileOffset beginOfThread;
    SAPDB_UInt     lengthOfThread=0;
    RTE_FileOffset beginOfTask;
    SAPDB_UInt     lengthOfTask=0;
    SAPDB_Bool     firstMessageInList = true;

    Msg_MessageList *pNewMessageList = new (RTEMem_Allocator::Instance()) Msg_MessageList;
    AnalyzeMSGLAttributes(pNewMessageList,pElement,beginOfThread,lengthOfThread,beginOfTask,lengthOfTask);

    pNewMessageList->part           = pElement->part;
    pNewMessageList->beginOfThread  = beginOfThread;    
    pNewMessageList->lengthOfThread = lengthOfThread;
    pNewMessageList->beginOfTask    = beginOfTask;
    pNewMessageList->lengthOfTask   = lengthOfTask;

    pNewMessageList->beginOfXMLCode = pElement->textBegin;  // for xml format
    Msg_Message *pLastMessage = NULL;
    pElement = pElement->pNextElement;
    while(pElement && !( tagMSGL == pElement->tagID && End == pElement->tagKind))
    {
      
        if(( tagMSG == pElement->tagID && Begin == pElement->tagKind))
        {
            Msg_Message *pNewMessage = ProcessMSG(pElement,beginOfThread,lengthOfThread,beginOfTask,lengthOfTask);
            pNewMessage->firstMessageInList = firstMessageInList;
            firstMessageInList = false;
            if( !pLastMessage )
            {
                pNewMessageList->pFirstMessage = pNewMessage;
            }
            else
            {
                pLastMessage->pNextMessage = pNewMessage;
            }
            pLastMessage = pNewMessage;
        }
        if(( tagMSGL == pElement->tagID && Begin == pElement->tagKind)) 
        // it may happen, that a message list is not written completely but followed by another one 
        // (kernel terminated while writing message list, watchdog writes "last words"). In this case, ignore the incomplete one
        {
            AnalyzeMSGLAttributes(pNewMessageList,pElement,beginOfThread,lengthOfThread,beginOfTask,lengthOfTask);

            pNewMessageList->part           = pElement->part;
            pNewMessageList->beginOfThread  = beginOfThread;    
            pNewMessageList->lengthOfThread = lengthOfThread;
            pNewMessageList->beginOfTask    = beginOfTask;
            pNewMessageList->lengthOfTask   = lengthOfTask;

            pNewMessageList->beginOfXMLCode = pElement->textBegin;  // for xml format
        }
        pElement = pElement->pNextElement;
    }
    if(pElement)
    {
        pNewMessageList->lengthOfXMLCode = (SAPDB_UInt)(pElement->textBegin-pNewMessageList->beginOfXMLCode) + pElement->textLength + 1;  // for xml format
    }
    return pNewMessageList;
}

SAPDB_Bool Msg_FileProcessor::ScanSourceFile(Msg_List &err)
{
    Msg_Element *pFirstElement = NULL;

    Msg_FileScanner scanner((SAPDB_Byte *)m_fileBuffer[m_currentPart],m_fileSize[m_currentPart]);
    Msg_Element *pElement;
    Msg_Element *pLastElement = NULL;
    do
    {
        if(!scanner.NextElement(pElement,err))
            break;
        m_elementCounter++;
        pElement->part = m_currentPart;
        if( NULL == pFirstElement )
        {
            pFirstElement = pElement;
        }
        
        if(NULL != pLastElement )
        {
            pLastElement->pNextElement = pElement;
        }
        pLastElement = pElement;
    }
    while(1);

    switch(m_currentPart)
    {
    case Header:
    case Complete:
        m_pFirstElementOfHeader = pFirstElement;
        break;
    case Cyclic:
        m_pFirstElementOfCyclic = pFirstElement;
        break;
    case Footer:
        m_pFirstElementOfFooter = pFirstElement;
        break;
    }
    return true;
}


void Msg_FileProcessor::CheckStructureConsistency(Msg_Message *pMessage)
{
#ifdef CHECK_CONSISTENCY
    SAPDB_UInt messageCounter = 0;
    while(pMessage)
    {
        messageCounter++;
        printf("checking %d",messageCounter);
        if(pMessage->numberOfAttributes > 1000000)
            printf("numberOfAttributes big");
        if(pMessage->lengthOfText > 1000000)
            printf("lengthOfText big");
        if(pMessage->lengthOfId > 1000000)
            printf("lengthOfId big");
        if(pMessage->lengthOfThread > 1000000)
            printf("lengthOfThread big");
        if(pMessage->lengthOfAttributes > 1000000)
            printf("lengthOfAttributes big");
        if(pMessage->lengthOfType > 1000000)
            printf("lengthOfType big");
        printf("\n");
        pMessage = pMessage->pNextMessage;
    }
#endif
}


void Msg_FileProcessor::AnalyzeStructure(Msg_Element *pElement,SortMode sortMode)
{
    SAPDB_Bool insertMode = false;
    RTE_FileOffset  beginOfCurrentThread  = 0;
    SAPDB_UInt      lengthOfCurrentThread = 0;
    RTE_FileOffset  beginOfCurrentTask  = 0;
    SAPDB_UInt      lengthOfCurrentTask = 0;
    while(pElement)
    {
        if( MessageListsByNo == sortMode )
        {
            if( (tagMSGL == pElement->tagID) && (Begin == pElement->tagKind))
            {
                Msg_MessageList *pNewMessageList = ProcessMSGL(pElement);
                if(!pNewMessageList)    // incomplete message list found
                    continue;

                if( !m_pLastMessageList )   // first list?
                {
                    m_pFirstMessageList = pNewMessageList;
                    m_pLastMessageList  = pNewMessageList;
                }
                else
                {
                    if ( pNewMessageList->MessageListNo >= m_pLastMessageList->MessageListNo )   // bigger sequence no than last list?
                    {   // insert at the end of the chain
                        if( pNewMessageList->MessageListNo > m_pLastMessageList->MessageListNo+1 )  // if there is a gap
                        {   // it is a possible insert position
                            m_pInsertPosition = m_pLastMessageList;
                        }
                        m_pLastMessageList->pNextMessageList = pNewMessageList;
                        m_pLastMessageList = pNewMessageList;
                    }
                    else if( m_pInsertPosition &&    // fits at insert position ?
                                pNewMessageList->MessageListNo > m_pInsertPosition->MessageListNo &&
                                pNewMessageList->MessageListNo < m_pInsertPosition->pNextMessageList->MessageListNo )
                    {   // insert it at the insert position
                        pNewMessageList->pNextMessageList = m_pInsertPosition->pNextMessageList;
                        m_pInsertPosition->pNextMessageList = pNewMessageList;
                        if ( pNewMessageList->MessageListNo == m_pInsertPosition->MessageListNo+1 )
                        {
                            m_pInsertPosition = pNewMessageList;
                        }
                        if( pNewMessageList->MessageListNo+1 == pNewMessageList->pNextMessageList->MessageListNo )  // if the gap has been filled
                        {
                            m_pInsertPosition = NULL;
                        }
                    }
                    else
                    {
                        // search from the beginning
                        Msg_MessageList *pCurrentPosition = m_pFirstMessageList;
                        while( pCurrentPosition )
                        {
                            if ( pCurrentPosition->pNextMessageList && 
                                pCurrentPosition->pNextMessageList->MessageListNo > pNewMessageList->MessageListNo )
                            {
                                break;
                            }
                            pCurrentPosition = pCurrentPosition->pNextMessageList;
                        }
                        if( pCurrentPosition )
                        {
                            pNewMessageList->pNextMessageList = pCurrentPosition->pNextMessageList;
                            pCurrentPosition->pNextMessageList = pNewMessageList;

                            if ( pCurrentPosition->MessageListNo+1 < pNewMessageList->MessageListNo )
                            {
                                m_pInsertPosition = pCurrentPosition;
                            }
                            if ( pNewMessageList->MessageListNo+1 < pNewMessageList->pNextMessageList->MessageListNo )
                            {
                                m_pInsertPosition = pNewMessageList;
                            }
                        }
                        else
                        {
                            pNewMessageList->pNextMessageList = m_pFirstMessageList;
                            m_pFirstMessageList = pNewMessageList;
                        }
                    }
                }
            }
        }
        else
        {
            if( (Begin == pElement->tagKind))
            {
                if( (tagMSGL == pElement->tagID))
                {
                    Msg_MessageList currentMessageList;
                    AnalyzeMSGLAttributes(&currentMessageList,pElement,beginOfCurrentThread,lengthOfCurrentThread,beginOfCurrentTask,lengthOfCurrentTask);
                }
                else if( (tagMSG == pElement->tagID))
                {
                    Msg_Message *pNewMessage = ProcessMSG(pElement,beginOfCurrentThread,lengthOfCurrentThread,beginOfCurrentTask,lengthOfCurrentTask);
                    if(!m_pLastMessage)
                    {
                        m_pFirstMessage = m_pLastMessage = pNewMessage;
                    }
                    else if( *pNewMessage >= *m_pLastMessage )
                    {
                        m_pLastMessage->pNextMessage = pNewMessage;
                        m_pLastMessage = pNewMessage;
                    }
                    else
                    {
                        Msg_Message *pInsertPositionForMessage = m_pFirstMessage;
                        while(pInsertPositionForMessage && pInsertPositionForMessage->pNextMessage)
                        {
                            if( *(pInsertPositionForMessage->pNextMessage) > *pNewMessage ) // >= compares the time stamps!
                            {
                                break;
                            }
                            pInsertPositionForMessage = pInsertPositionForMessage->pNextMessage;
                        }
                        if(!pInsertPositionForMessage->pNextMessage)
                        {
                            m_pLastMessage = pInsertPositionForMessage;
                        }
                        pNewMessage->pNextMessage = pInsertPositionForMessage->pNextMessage;
                        pInsertPositionForMessage->pNextMessage = pNewMessage;
                    }
#ifdef CHECK_CONSISTENCY
                    CheckStructureConsistency(m_pFirstMessage);
#endif
                }
            }
        }
        pElement = pElement->pNextElement;
    }
}

void Msg_FileProcessor::ReSortByMSGLNO(Msg_Element *&pFirstElement)
{
    Msg_Element *pElement = pFirstElement;
    Msg_Element *pLastElement = NULL;
    Msg_Element *pOldFirstElement = pFirstElement;
    SAPDB_UInt lastMSGNO=0;
    SAPDB_Bool sequenceHasChanged = false;
    while(pElement)
    {
        if( (tagMSGL == pElement->tagID) && (Begin == pElement->tagKind))
        {
            SAPDB_Char buffer[20];
            Attribute *pCurrentAttribute = pElement->pAttributes;
            while(pCurrentAttribute)
            {
                GetTextFromFile(pElement->part,pCurrentAttribute->beginOfName,pCurrentAttribute->lengthOfName,buffer,20);
                if(0 == strcmp(buffer,MSG_LIST_MESSAGE_NUMBER_TAG))
                {
                    GetTextFromFile(pElement->part,pCurrentAttribute->beginOfValue,pCurrentAttribute->lengthOfValue,buffer,20);
                    SAPDB_UInt currentMSGNO = atoi(buffer);
                    if(currentMSGNO < lastMSGNO)
                    {
                        sequenceHasChanged = true;
                        pFirstElement = pElement;
                        if(pLastElement)
                        {
                            pLastElement->pNextElement = NULL;
                        }
                    }
                    lastMSGNO = currentMSGNO;
                }
                pCurrentAttribute = pCurrentAttribute->pnext;
            };
        }
        pLastElement = pElement;
        pElement = pElement->pNextElement;
    }
    if( sequenceHasChanged )
    {
        pLastElement->pNextElement = pOldFirstElement;
    }
}

/*!
  @class CharacterAndRepresentation
  @brief helper class for storing ascii characters and their xml representation
 */
struct CharacterAndRepresentation
{
    /*! the ascii character */
    SAPDB_Char character;
    /*! the xml representation */
    const SAPDB_Char *pRepresentation;
};

/*! ascii characters and their xml representation */
static const CharacterAndRepresentation charactersAndTheirRepresentation[] = 
{ 
    {'&',   "&amp;"},
    {'\"',  "&quot;"},
    {'>',   "&gt;"},
    {'<',   "&lt;"},
    {'\'',  "&apos;"}
};

/*! macro for calculating the number of elements of an array */
#define NUMBER_OF_ELEMENTS_(ARRAY) (sizeof((ARRAY))/sizeof(ARRAY[0]))

void Msg_FileProcessor::ResolveEncodedCharacters(SAPDB_Char *processedText)
{
    SAPDB_UInt4 neededSizeForText = (SAPDB_UInt)strlen(processedText);
    SAPDB_UInt i=0;
    do
    {
        SAPDB_Char *pPosOfAmpersand;
        SAPDB_Char *pPosOfSemicolon;
        if('&' == processedText[i])
        {
            pPosOfAmpersand = processedText+i;
            do
            {
                i++;
                if(!processedText[i])
                {
                    break;
                }
                if(';' == processedText[i])
                {
                    pPosOfSemicolon = processedText+i;
                    for(SAPDB_UInt index=0;index < NUMBER_OF_ELEMENTS_(charactersAndTheirRepresentation);index++)
                    {
                        if(0 == strncmp(pPosOfAmpersand,charactersAndTheirRepresentation[index].pRepresentation,pPosOfSemicolon-pPosOfAmpersand+1))
                        {
                            *pPosOfAmpersand = charactersAndTheirRepresentation[index].character;
                            SAPDB_MemCopyNoCheck(pPosOfAmpersand+1,pPosOfSemicolon+1,neededSizeForText-(pPosOfSemicolon-processedText));
                            neededSizeForText -= (pPosOfSemicolon-pPosOfAmpersand);
                            neededSizeForText++;
                            i = pPosOfAmpersand-processedText;
                            break;
                        }
                    }
                    break;
                }
            }
            while(1);
        }
        i++;
    }
    while(processedText[i]);
}

void Msg_FileProcessor::ProcessString(const SAPDB_Char *text,SAPDB_Char *processedText, SAPDB_UInt lengthOfProcessedText, SAPDB_Char const **tagNameArray, SAPDB_Char const **tagValueArray,SAPDB_UInt numberOfAttributes)
{
    SAPDB_UInt4 neededSizeForText;
    Msg_List::SubstituteArgumentTags( text
                                    , numberOfAttributes
                                    , tagNameArray
                                    , tagValueArray
                                    , lengthOfProcessedText
                                    , processedText
                                    , neededSizeForText
                                    , false/* do NOT append unused message arguments at the end of the text */ );

    ResolveEncodedCharacters(processedText);
}

#define LINE_INDENT_    "                                                                   "

enum Action{None,LineBreak,EmptyLineAndBreak,ListStart,ListEnd};

struct Markup
{
    const SAPDB_Char    *pTag;
    SAPDB_UInt          tagLength;
    Action              action;
};

static Markup markups[]=
{
    {"br>", 3,  LineBreak},
    {"/p>", 3,  EmptyLineAndBreak},
    {"li>", 3,  ListStart},
    {"/li>",4,  ListEnd},
    {"/b>", 3,  None},
    {"b>",  2,  None},
    {"p>",  2,  None}
};

void Msg_FileProcessor::ProcessAndOutputStringWithFormatting(const SAPDB_Char *text,SAPDB_UInt lengthOfProcessedText, SAPDB_Char const **tagNameArray, SAPDB_Char const **tagValueArray,SAPDB_UInt numberOfAttributes)
{
    SAPDB_Char *processedText = (SAPDB_Char *)RTEMem_Allocator::Instance().Allocate(lengthOfProcessedText);
    SAPDB_UInt4 neededSizeForText;
    Msg_List::SubstituteArgumentTags( text
                                    , numberOfAttributes
                                    , tagNameArray
                                    , tagValueArray
                                    , lengthOfProcessedText
                                    , processedText
                                    , neededSizeForText
                                    , false/* do NOT append unused message arguments at the end of the text */ );

    ResolveEncodedCharacters(processedText);
    SAPDB_Char *pBeginOfCurrentLine = processedText;
    SAPDB_UInt i = 0;

    // now interpret the markup tags (as possible...)
    DoOutput(LINE_INDENT_);
    SAPDB_Bool inList = false;
    while ( i < neededSizeForText )
    {
        if( '<' == processedText[i] )
        {
            // may be a valid markup tag
            for(SAPDB_UInt markupIndex = 0; markupIndex < NUMBER_OF_ELEMENTS_(markups); markupIndex++)
            {
                if(0 == strncmp(&processedText[i+1],markups[markupIndex].pTag,markups[markupIndex].tagLength))
                {
                    // actions to be done if any valid tag has been found:
                    // - output the text block up to here
                    processedText[i] = 0;           
                    DoOutput(pBeginOfCurrentLine);

                    // - the next text block starts after the tag
                    i += markups[markupIndex].tagLength;
                    pBeginOfCurrentLine = &processedText[i]+1;

                    // now the tag specific action
                    switch( markups[markupIndex].action )
                    {
                    case LineBreak:
                        DoOutput(LINE_ENDING,LINE_INDENT_);
                        if( inList )    // line breaks may occurr within lists. In this case the list indentation should be kept.
                        {
                            DoOutput("     ");
                        }
                        break;
                    case EmptyLineAndBreak:
                        DoOutput(LINE_ENDING,LINE_ENDING,LINE_INDENT_);
                        break;
                    case ListStart:
                        DoOutput("  +  ");
                        inList = true;
                        break;
                    case ListEnd:
                        DoOutput(LINE_ENDING,LINE_INDENT_);
                        inList = false;
                        break;
                    }
                }
            }
        }
        if('\n' == processedText[i])
        {
            processedText[i] = 0;           
            DoOutput(pBeginOfCurrentLine);

            // - the next text block starts after the cr/lf
            pBeginOfCurrentLine = &processedText[i]+1;

            DoOutput(LINE_ENDING,LINE_INDENT_);
        }
        if('\r' == processedText[i])
        {
            processedText[i] = 0;           
            DoOutput(pBeginOfCurrentLine);

            // - the next text block starts after the cr/lf
            pBeginOfCurrentLine = &processedText[i]+1;
        }
        i++;
    }
    DoOutput(pBeginOfCurrentLine,LINE_ENDING);
}

void Msg_FileProcessor::OutputMessageListHeader(Msg_MessageList *pCurrentMessageList,Format format)
{
    SAPDB_Char *thread = NULL;
    if ( 0 != pCurrentMessageList->lengthOfThread )
    {
        thread = (SAPDB_Char *)alloca(pCurrentMessageList->lengthOfThread+1);
        SAPDB_strncpy_zero_terminated(thread,m_fileBuffer[pCurrentMessageList->part]+pCurrentMessageList->beginOfThread,pCurrentMessageList->lengthOfThread+1);
    }

    SAPDB_Char *task = NULL;
    if ( 0 != pCurrentMessageList->lengthOfTask )
    {
        task = (SAPDB_Char *)alloca(pCurrentMessageList->lengthOfTask+1);
        SAPDB_strncpy_zero_terminated(task,m_fileBuffer[pCurrentMessageList->part]+pCurrentMessageList->beginOfTask,pCurrentMessageList->lengthOfTask+1);
    }

    if(Plain == format)
    {
        if( thread )
            DoOutput    ("Thread ",StringWithLeadingSpaces(thread,6));
        else
            DoOutput    ("Thread ",StringWithLeadingSpaces("-",6));

        if( task )
            DoOutput    (" Task ",StringWithLeadingSpaces(task,3));
        else
            DoOutput    (" Task ",StringWithLeadingSpaces("-",3));
//        DoOutput        (LINE_ENDING);
    }
}

void Msg_FileProcessor::OutputXmlMessageList(Msg_MessageList *pCurrentMessageList)
{
    SAPDB_Bool topLevelOnly = false;    // maybe set by an argument in the future...
    if( topLevelOnly )
    {
        SAPDB_Char *msgl = (SAPDB_Char *)alloca((size_t)(pCurrentMessageList->pFirstMessage->endOfXMLCode - pCurrentMessageList->beginOfXMLCode+1));
        SAPDB_strncpy_zero_terminated(msgl,m_fileBuffer[pCurrentMessageList->part]+pCurrentMessageList->beginOfXMLCode,pCurrentMessageList->pFirstMessage->endOfXMLCode - pCurrentMessageList->beginOfXMLCode+1);
        DoOutput(msgl);
        DoOutput("</",MSG_LIST_MESSAGE_LIST_TAG,">");
    }
    else
    {
        SAPDB_Char *msgl = (SAPDB_Char *)alloca((size_t)(pCurrentMessageList->lengthOfXMLCode+1));
        SAPDB_strncpy_zero_terminated(msgl,m_fileBuffer[pCurrentMessageList->part]+pCurrentMessageList->beginOfXMLCode,pCurrentMessageList->lengthOfXMLCode+1);
        DoOutput(msgl);
    }
}

SAPDB_Bool Msg_FileProcessor::ReadCompleteSourceFile(Msg_List &err)
{
    RTE_Path sourceFileName; 
    RTE_FileHandle *pfileHandle;
    tsp00_VfReturn_Param ReturnStatus;
    const SAPDB_Char *suffix;
    switch(m_currentPart) 
    { 
    case Complete: 
        suffix = SAPDB_OUTPUTFILE_COMPLETE_SUFFIX; 
        pfileHandle = &m_completeFile;
        break;
    case Header:
        suffix = SAPDB_OUTPUTFILE_STARTING_PHASE_SUFFIX;
        pfileHandle = &m_headerFile;
        break;
    case Cyclic:
        suffix = SAPDB_OUTPUTFILE_RUNNING_PHASE_SUFFIX;
        pfileHandle = &m_cyclicFile;
        break;
    case Footer:
        pfileHandle = &m_footerFile;
        suffix = SAPDB_OUTPUTFILE_TERMINATING_PHASE_SUFFIX;
        break;
    default:
        pfileHandle = 0; // only for -Wall
        suffix = ""; // only for -Wall
        break;
    }
    SAPDB_sprintf(sourceFileName,sizeof(RTE_Path),"%s%s",m_fileName,suffix);
    RTESys_IOOpen ( *pfileHandle,
                    sourceFileName,
                    RTESys_IOReadOnlyShared,
                    false,
                    0,
                    ReturnStatus );
    if(vf_notok == ReturnStatus)
    {
        err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"file open failed",sourceFileName);
        return false;
    }
    RTESys_IOSeek ( *pfileHandle,
                    0,
                    RTESys_IOSeekEnd,
                    m_fileSize[m_currentPart],
                    ReturnStatus );
    if(vf_notok == ReturnStatus)
    {
        err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek in file failed",sourceFileName);
        return false;
    }

    RTE_FileOffset newPosition;
    RTESys_IOSeek ( *pfileHandle,
                    0,
                    RTESys_IOSeekSet,
                    newPosition,
                    ReturnStatus );
    if( vf_notok == ReturnStatus || 0 != newPosition )
    {
        err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"seek in file failed",sourceFileName);
        return false;
    }

    m_fileBuffer[m_currentPart] = (SAPDB_Char*)(RTEMem_Allocator::Instance()).Allocate((SAPDB_ULong)m_fileSize[m_currentPart]);
    if(!m_fileBuffer[m_currentPart])
    {
        err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"memory allocation failed wanted... ",SAPDB_ToString((SAPDB_UInt8)(m_fileSize[m_currentPart])));
        return false;
    }

    tRTESys_IOPosition BytesRead;
    RTESys_IORead(  *pfileHandle,
                    m_fileBuffer[m_currentPart],
                    m_fileSize[m_currentPart],
                    BytesRead,
                    ReturnStatus );

    RTESys_IOClose ( *pfileHandle, 
                    ReturnStatus ); 

    if ( vf_notok == ReturnStatus )
    {
        err = Msg_List(RTE_CONTEXT,RTEERR_GENERAL,"file close failed",sourceFileName);
        return false;
    }
    return true;
}

void Msg_FileProcessor::OutputMessage(Msg_Message *pCurrentMessage,Format format,SAPDB_Bool verbose)
{
    SAPDB_UInt hours = pCurrentMessage->Time / 3600000;
    pCurrentMessage->Time %= 3600000;
    SAPDB_UInt minutes = pCurrentMessage->Time / 60000;
    pCurrentMessage->Time %= 60000;
    SAPDB_UInt seconds = pCurrentMessage->Time / 1000;
    pCurrentMessage->Time %= 1000;

    if(Plain == format)
    {
        DoOutput    ("  ");
    }
    DoOutput(SAPDB_ToString(pCurrentMessage->Year,4)
            ,"-",
            SAPDB_ToString(pCurrentMessage->Month,2,_T_z)
            ,"-"
            ,SAPDB_ToString(pCurrentMessage->Day,2,_T_z)
            ," "
            ,SAPDB_ToString(hours,2,_T_z)
            ,":"
            ,SAPDB_ToString(minutes,2,_T_z)
            ,":"
            ,SAPDB_ToString(seconds,2,_T_z)
            );

    SAPDB_Char *id = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfId+1);
    SAPDB_strncpy_zero_terminated(id,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfId,pCurrentMessage->lengthOfId+1);

    SAPDB_Char **tagNameArray   = (SAPDB_Char **)alloca(pCurrentMessage->numberOfAttributes*sizeof(SAPDB_Char *));
    SAPDB_Char **tagValueArray  = (SAPDB_Char **)alloca(pCurrentMessage->numberOfAttributes*sizeof(SAPDB_Char *));

    SAPDB_Char *tags = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfAttributes+1);
    SAPDB_strncpy_zero_terminated(tags,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfAttributes,pCurrentMessage->lengthOfAttributes+1);

    SAPDB_Char *component = NULL;
    SAPDB_Char *label = NULL;
    if( pCurrentMessage->beginOfLabel ) // label is only set for messages generated by "deprecated" routines like vmessage()
    {
        label = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfLabel+1);
        SAPDB_strncpy_zero_terminated(label,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfLabel,pCurrentMessage->lengthOfLabel+1);
    }
    else
    {
        component = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfComponent+1);
        SAPDB_strncpy_zero_terminated(component,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfComponent,pCurrentMessage->lengthOfComponent+1);
    }

    Attribute *pCurrentAttribute = pCurrentMessage->pAttributes;

    SAPDB_ULong messageID = atoi(id);
    if(m_longColumns)
    {
        SAPDB_Char *thread = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfThread+1);
        SAPDB_strncpy_zero_terminated(thread,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfThread,pCurrentMessage->lengthOfThread+1);

        SAPDB_Char *task = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfTask+1);
        SAPDB_strncpy_zero_terminated(task,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfTask,pCurrentMessage->lengthOfTask+1);

        const SAPDB_Char *messageType = "";
        if(0 != pCurrentMessage->lengthOfType)
        {
            SAPDB_Char *messageTypeFromFile = (SAPDB_Char *)alloca(pCurrentMessage->lengthOfType+1);
            SAPDB_strncpy_zero_terminated(messageTypeFromFile,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfType,pCurrentMessage->lengthOfType+1);
            if(0 == strcmp(messageTypeFromFile,"Error"))
            {
                messageType = "ERR";
            }
            else if(0 == strcmp(messageTypeFromFile,"Warning"))
            {
                messageType = "WNG";
            }
        }

        if(Plain == format)
        {
            DoOutput    (StringWithLeadingSpaces(messageType,4)
                        ," "
                        ,StringWithTrailingSpaces(component ? component : label,11)
                        ,StringWithLeadingSpaces(id,6)
                        ,": "
                        );
        }
        else
        {
            DoOutput    (StringWithLeadingSpaces(thread,11)
                        ,StringWithLeadingSpaces(messageType,4)
                        ,StringWithLeadingSpaces(id,6)
                        ," "
                        ,StringWithTrailingSpaces(component ? component : label,9));
        }
    }
    DoOutput        (" ");


    SAPDB_Char *text = NULL;
    SAPDB_Char *description = NULL;
    SAPDB_Char *action = NULL;

    SAPDB_UInt lengthOfProcessedText;

    if( verbose && component &&(Msg_DescriptionStorage::Instance()).GetShortTextAndDescription(component,messageID,
            text,
            description,
            action))
    {
        lengthOfProcessedText = (SAPDB_UInt)strlen(text);
    }
    else
    {
        if(!pCurrentMessage->firstMessageInList)
        {
            text = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(pCurrentMessage->lengthOfText+3);
            SAPDB_sprintf(text,pCurrentMessage->lengthOfText+3,"- %s",m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfText);
        }
        else
        {
            text = (SAPDB_Char *)(RTEMem_Allocator::Instance()).Allocate(pCurrentMessage->lengthOfText+1);
            SAPDB_strncpy_zero_terminated(text,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfText,pCurrentMessage->lengthOfText+1);
        }
        lengthOfProcessedText = pCurrentMessage->lengthOfText;
    }

    SAPDB_UInt lengthOfProcessedArguments=0;
    SAPDB_UInt4 argumentIndex;
    for( argumentIndex=0; argumentIndex<pCurrentMessage->numberOfAttributes; argumentIndex++ )
    {
        tagNameArray[argumentIndex] = tags + pCurrentAttribute->beginOfName - pCurrentMessage->beginOfAttributes;
        *(tagNameArray[argumentIndex] + pCurrentAttribute->lengthOfName) = 0;

        tagValueArray[argumentIndex] = tags + pCurrentAttribute->beginOfValue - pCurrentMessage->beginOfAttributes;
        *(tagValueArray[argumentIndex] + pCurrentAttribute->lengthOfValue) = 0;

        lengthOfProcessedArguments += pCurrentAttribute->lengthOfName;
        lengthOfProcessedArguments += pCurrentAttribute->lengthOfValue;
        lengthOfProcessedArguments++;

        pCurrentAttribute = pCurrentAttribute->pnext;
    }

    lengthOfProcessedText += lengthOfProcessedArguments;

    SAPDB_Char *processedText = (SAPDB_Char *)alloca(lengthOfProcessedText+1);
    ProcessString(text,processedText, lengthOfProcessedText,(SAPDB_Char const **)tagNameArray,(SAPDB_Char const **)tagValueArray,pCurrentMessage->numberOfAttributes);
    DoOutput(processedText,LINE_ENDING);
    (RTEMem_Allocator::Instance()).Deallocate(text);
    if( description )
    {
        DoOutput(LINE_ENDING,LINE_INDENT_,"DESCRIPTION:",LINE_ENDING,LINE_ENDING);
        SAPDB_UInt lengthOfProcessedDescription = (SAPDB_UInt)strlen(description)+lengthOfProcessedArguments;
        ProcessAndOutputStringWithFormatting(description,lengthOfProcessedDescription,(SAPDB_Char const **)tagNameArray,(SAPDB_Char const **)tagValueArray,pCurrentMessage->numberOfAttributes);
        (RTEMem_Allocator::Instance()).Deallocate(description);
    }
    if( action )
    {
        DoOutput(LINE_ENDING,LINE_INDENT_,"ACTION:",LINE_ENDING,LINE_ENDING);
        SAPDB_UInt lengthOfProcessedAction = (SAPDB_UInt)strlen(action)+lengthOfProcessedArguments;
        ProcessAndOutputStringWithFormatting(action,lengthOfProcessedAction,(SAPDB_Char const **)tagNameArray,(SAPDB_Char const **)tagValueArray,pCurrentMessage->numberOfAttributes);
        (RTEMem_Allocator::Instance()).Deallocate(action);
    }
}



SAPDB_Bool Msg_FileProcessor::BuildWellFormedXMLFile(SAPDBErr_MessageList &errList,SortMode sortMode,Format format,SAPDB_Bool classicStyleHeader,SAPDB_Bool verbose)
{
    if(Msg_IOutput::Triple == m_style)
    {
        m_currentPart = Header;
        if(ReadCompleteSourceFile(errList))
        {
            ScanSourceFile(errList);
        }

        m_currentPart = Cyclic;
        if(ReadCompleteSourceFile(errList))
        {
            ScanSourceFile(errList);
        }

        m_currentPart = Footer;

        if(ReadCompleteSourceFile(errList))
        {
            ScanSourceFile(errList);
        }

        if(classicStyleHeader)
        {
            DoOutput("--------------------------------------------------------------------------------"
                    ,LINE_ENDING
                    ,"Date       Time "
                    ,m_longColumns ? "      TID(hex) Typ MsgID Label" : NULL
                    ,"    Message-Text"
                    ,LINE_ENDING
                    ,"--------------------------------------------------------------------------------"
                    ,LINE_ENDING);
        }
        
        messageAllocator.Initialize(m_elementCounter);

        AnalyzeStructure(m_pFirstElementOfHeader,sortMode);

        if( Msg_FileProcessor::MessageListsByNo == sortMode )
        {
            m_pInsertPosition = m_pLastMessageList;
        }
        AnalyzeStructure(m_pFirstElementOfCyclic,sortMode);

        AnalyzeStructure(m_pFirstElementOfFooter,sortMode);
    }
    else
    {
        m_currentPart = Complete;
        if(ReadCompleteSourceFile(errList))
        {
            ScanSourceFile(errList);
        }
        if(classicStyleHeader)
        {
            DoOutput("--------------------------------------------------------------------------------"
                    ,LINE_ENDING
                    ,"Date       Time "
                    ,m_longColumns ? "      TID(hex) Typ MsgID Label" : NULL
                    ,"    Message-Text"
                    ,LINE_ENDING
                    ,"--------------------------------------------------------------------------------"
                    ,LINE_ENDING);
        }

        if( Xml == format )
        {
            DoOutput("<XML>");
        }
        messageAllocator.Initialize(m_elementCounter);

        AnalyzeStructure(m_pFirstElementOfHeader,sortMode);

        if( Msg_FileProcessor::MessageListsByNo == sortMode )
        {
            m_pInsertPosition = m_pLastMessageList;
        }
    }

    if( Msg_FileProcessor::MessageListsByNo == sortMode )
    {
        Msg_MessageList *pCurrentMessageList = m_pFirstMessageList;
        SAPDB_Bool beginOfWriteCyclePassed = false;
        SAPDB_Bool kernelExitPassed = false;
        SAPDB_ULong messageID = 0; // only for -Wall
        SAPDB_Bool firstMessageInList;
        SAPDB_Char id[21];  // 21? 2^64 has 20 decimal digits. This should be enough

        while(pCurrentMessageList)
        {
            firstMessageInList = true;
            if( Xml == format )
            {
                OutputXmlMessageList(pCurrentMessageList);
            }
            else
            {
                OutputMessageListHeader(pCurrentMessageList,format);
                Msg_Message *pCurrentMessage = pCurrentMessageList->pFirstMessage;
                while(pCurrentMessage)
                {
                    if(classicStyleHeader && (!beginOfWriteCyclePassed || !kernelExitPassed))
                    {
                        SAPDB_strncpy_zero_terminated(id,m_fileBuffer[pCurrentMessage->part]+pCurrentMessage->beginOfId,sizeof(id)-1);
                        messageID = atoi(id);

                        if( 11570 == messageID )    // UNIX style: "end of startup" before the messages with id 11570
                        {
                            DoOutput(ENDOFSTARTUP);
                            beginOfWriteCyclePassed = true;
                        }
                        if( (12845 == messageID) || (12009 == messageID) || (12007 == messageID) || (12005 == messageID)|| (12006 == messageID) )
                        {
                            DoOutput(KERNELEXIT);
                            kernelExitPassed = true;
                        }
                    }
                    if( Plain == format && !firstMessageInList)
                        DoOutput(StringWithLeadingSpaces("",22));

                    OutputMessage(pCurrentMessage,format,verbose);
                    if(classicStyleHeader && !beginOfWriteCyclePassed)
                    {
                        if( 19601 == messageID )    // Windows style: "begin of write cycle" after the messages with id 19601
                        {
                            DoOutput(BEGIN_OF_DIAG_WRITE_CYCLE);
                            beginOfWriteCyclePassed = true;
                        }
                    }
                    pCurrentMessage = pCurrentMessage->pNextMessage;
                    firstMessageInList = false;
                }
            }
            pCurrentMessageList = pCurrentMessageList->pNextMessageList;
        }
    }
    else
    {
        Msg_Message *pCurrentMessage = m_pFirstMessage;
        SAPDB_Bool exitLineWritten = false;

#ifdef CHECK_CONSISTENCY
        CheckStructureConsistency(m_pFirstMessage);
#endif

        while(pCurrentMessage)
        {
            if(classicStyleHeader)
            {
                if(Footer == pCurrentMessage->part && !exitLineWritten)
                {
                    DoOutput("+++++++++++++++++++++++++++++++++++++++ Kernel Exit ++++++++++++++++++++++++++++"
                            ,LINE_ENDING);
                    exitLineWritten = true;
                }
            }
            OutputMessage(pCurrentMessage,format,verbose);
            pCurrentMessage = pCurrentMessage->pNextMessage;
        }
    }
    if(classicStyleHeader)
    {
        DoOutput("-------------------------------------------- current write position ----------------"
                ,LINE_ENDING
                ,"===========================================  end of write cycle  ==================="
                ,LINE_ENDING);
    }
    if( Xml == format )
    {
        DoOutput("</XML>");
    }
    return true;
}

SAPDB_Bool Msg_FileProcessor::SearchLastWrittenMessageList(RTE_FileOffset endPosition,RTE_FileOffset &writePosition,SAPDB_UInt8 &biggestSequenceNumber,SAPDB_UInt8 &lengthOfGarbage,Msg_List &errList)
{
    SAPDB_UInt8 currentSequenceNumber;
    SAPDB_UInt8 maxSequenceNumber = 0;
    writePosition = 0;
    RTE_FileOffset textEnd;
    Msg_FileScanner scanner(m_sourceFile,SINGLE_BUFFERSIZE_FOR_SEARCH_TAG);
    scanner.SkipToBeginTag(MSG_LIST_MESSAGE_LIST_TAG,errList);  // skip possible garbage before the first <MSGL>
    for(;;)
    {
        if(NOT_FOUND == scanner.SearchNumericAttribute(MSG_LIST_MESSAGE_NUMBER_TAG,currentSequenceNumber,errList))
            break;

        if(currentSequenceNumber > maxSequenceNumber)
            maxSequenceNumber = currentSequenceNumber;

        if(GARBAGE == scanner.SkipToEndTag(MSG_LIST_MESSAGE_LIST_TAG,textEnd,errList))
            break;

        writePosition = scanner.CurrentPositionInFile();

        if(GARBAGE == scanner.SearchTag(MSG_LIST_MESSAGE_LIST_TAG,errList))
            break;
    }

    if(GARBAGE != scanner.SkipToBeginTag(MSG_LIST_MESSAGE_LIST_TAG,errList))
    {
        lengthOfGarbage = scanner.CurrentPositionInFile() - writePosition - strlen(MSG_LIST_MESSAGE_LIST_TAG)-2;
    }
    else
    {
        lengthOfGarbage = endPosition - writePosition - 1;  // only garbage, no complete message list at end of file
    }
    biggestSequenceNumber = maxSequenceNumber;
    return true;
}

SAPDB_Bool Msg_FileProcessor::CreateDestinationFile(RTE_Path &destinationFileName,SAPDBErr_MessageList &errList)
{
    tsp00_VfReturn_Param ReturnStatus;
    RTESys_IOUnlink( destinationFileName,ReturnStatus );
    RTESys_IOOpen ( m_destinationFile,
                    destinationFileName,
                    RTESys_IOWriteOnlyExclusive,
                    true,
                    0,
                    ReturnStatus );
    return ( vf_ok == ReturnStatus );
}

SAPDB_Bool Msg_FileProcessor::CloseDestinationFile(SAPDBErr_MessageList &errList)
{
    tsp00_VfReturn_Param ReturnStatus;
    RTESys_IOClose ( m_destinationFile,
                        ReturnStatus );
    return ( vf_ok == ReturnStatus );
}

SAPDB_Bool Msg_FileProcessor::OpenSourceFile(Part part,SAPDBErr_MessageList &errList)
{
    RTE_Path sourceFileName; 
    switch(part) 
    { 
    case Complete: 
        SAPDB_sprintf(sourceFileName,sizeof(RTE_Path),"%s"SAPDB_OUTPUTFILE_COMPLETE_SUFFIX,m_fileName); 
        break;
    case Header:
        SAPDB_sprintf(sourceFileName,sizeof(RTE_Path),"%s"SAPDB_OUTPUTFILE_STARTING_PHASE_SUFFIX,m_fileName);
        break;
    case Cyclic:
        SAPDB_sprintf(sourceFileName,sizeof(RTE_Path),"%s"SAPDB_OUTPUTFILE_RUNNING_PHASE_SUFFIX,m_fileName);
        break;
    case Footer:
        SAPDB_sprintf(sourceFileName,sizeof(RTE_Path),"%s"SAPDB_OUTPUTFILE_TERMINATING_PHASE_SUFFIX,m_fileName);
        break;
    }
    tsp00_VfReturn_Param ReturnStatus;
    RTESys_IOOpen ( m_sourceFile,
                    sourceFileName,
                    RTESys_IOReadOnlyShared,
                    false,
                    0,
                    ReturnStatus );
    m_currentPart = part;

    return ( vf_ok == ReturnStatus );
}

/*! size of the buffers SearchLastWrittenCharacter() uses. */
#define BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER 512

SAPDB_Bool Msg_FileProcessor::SearchLastWrittenCharacter(RTE_FileOffset &position,Msg_List &errList)
{
    tsp00_VfReturn_Param ReturnStatus;         
    SAPDB_Byte buffer[BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER];
    RTE_FileOffset destOffset = -BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER;
    while(1)
    {
        tRTESys_IOPosition BytesRead;
        RTESys_IOSeek ( m_sourceFile,
                        destOffset,
                        RTESys_IOSeekEnd,
                        position,
                        ReturnStatus );
        if( vf_noseek == ReturnStatus )
        {
            RTESys_IOSeek ( m_sourceFile,
                            0,
                            RTESys_IOSeekSet,
                            position,
                            ReturnStatus );

        }
        if( vf_ok != ReturnStatus )
        {
            break;
        }
        RTESys_IORead(  m_sourceFile,
                        buffer,
                        BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER,
                        BytesRead,
                        ReturnStatus );
        if( (vf_ok != ReturnStatus)  )
        {
            break;
        }
        SAPDB_Bool found = false;
        RTE_FileOffset offsetInBuffer;
        for(offsetInBuffer = SAPDB_MIN( BytesRead,BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER ) - 1; offsetInBuffer > 0 ; offsetInBuffer--)
        {
            if(!isspace(buffer[offsetInBuffer]))
            {
                found = true;
                position += offsetInBuffer;
                break;
            }
        }
        if( found )
        {
            break;
        }
        destOffset -= BUFFERSIZE_FOR_SEARCH_LAST_WRITTEN_CHARACTER;
    }
    tRTESys_IOPosition newPosition;
    RTESys_IOSeek ( m_sourceFile,
                    0,
                    RTESys_IOSeekSet,
                    newPosition,
                    ReturnStatus );
    return true;
}

SAPDB_Bool Msg_FileProcessor::CloseSourceFile(Msg_List &errList) 
{ 
    tsp00_VfReturn_Param ReturnStatus;         
    RTESys_IOClose ( m_sourceFile, 
                    ReturnStatus ); 
    return ( vf_ok == ReturnStatus ); 
} 
