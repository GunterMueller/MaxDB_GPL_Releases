/*!
  @file    Msg_FileProcessor.hpp
  @author  Robin
  @ingroup Common
  @brief   Message file processing class
  
  processing of message list files (pseudo-xml)

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

#ifndef MSG_FILEPROCESSOR_HPP
#define MSG_FILEPROCESSOR_HPP

#include "Messages/Msg_IOutput.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "SAPDBCommon/SAPDB_string.h"
#include "RunTime/System/RTESys_IO.h"
#include "Messages/Msg_OutputFile.hpp"



struct Msg_Element;
struct Msg_MessageList;
struct Msg_Message;
struct Attribute;
class Tag;

/*!
  @class Msg_FileProcessor
  @brief Message file processor

  Processing of generated pseudo-xml message list files  
 */
class Msg_FileProcessor
{
public:
    /*!
        @brief constructor. Create Message file processor object 

        @param  fileName    [in]    the name of the file to process
        @param  style       [in]    the style (cyclic or append) of the files to process
        @param  longColumns [in]    if true use long columns (default=true)
    */
    Msg_FileProcessor(const SAPDB_Char *fileName,Msg_IOutput::OutputStyle style,SAPDB_Bool longColumns = true);

    /*!
        @brief constructor. Create Message file processor object 
     */
    ~Msg_FileProcessor()
    {
        if(m_destinationFile)
        {
            SAPDBErr_MessageList errList;
            CloseDestinationFile(errList);
        }
    }

    /*!
        @brief  in the pseudo-xml message file belonging to this object, search for the position
                at which the next message would be written
        @param  writePosition           [out]   the position at which the next message would be written
        @param  biggestSequenceNumber   [out]   the biggest message sequence number found in the file
        @param  continuous              [in]    should be set to true if the message lists in the file 
                                                are written continuously (the message list with the 
                                                biggest sequence number is always at the end of the
                                                file). If you are unsure, set it to false. That would
                                                make the search take more time, but do no harm.
        @param  errList                 [out]   contains any error messages from the search process
    */
    SAPDB_Bool SearchWritePosition(RTE_FileOffset &writePosition,SAPDB_UInt8 &biggestSequenceNumber,SAPDB_Bool continuous,Msg_List &errList);

    /*!
        @brief enum containing the possible sort modes for file processing
    */
    enum SortMode
    {
        MessageListsByNo,MessagesByTimeStamp
    };

    /*!
        @brief enum containing the possible output formats
    */
    enum Format    
    {
        Plain,Xml,Classic
    };
    /*!
        @brief  transform the pseudo-xml message files into a well-formed xml file. The destination files gets
                file extension ".xml"
        @param  errList             [out]   contains any error messages from the search process
        @param  sortMode            [in]    method to sort
        @param  format              [in]    output format (Plain,Xml,Classic)
        @param  classicStyleHeader  [in]    true if header wanted
        @param  verbose             [in]    true if more detailed output
        @return                             true if successful, false if not
    */
    SAPDB_Bool BuildWellFormedXMLFile(SAPDBErr_MessageList &errList,SortMode sortMode,Format format,SAPDB_Bool classicStyleHeader,SAPDB_Bool verbose);

    /*!
        @brief enum containing the possible tags
    */
    enum xmlTagID 
    { tagMSGL=0, tagMSG=1, tagSHORTTEXT=2, tagTYPE=3, tag__FILE__=4, tag__LINE__=5, tagTIMESTAMP=6, tagMSG_ARGS=7, tagDESCRIPTION=8, tagACTION=9, tagUNKNOWN, plainText };

    /*!
        @brief enum containing the possible tag kinds: Begin ("<TAG>") and End ("<End>")
    */
    enum xmlTagKind { Begin,End };

    /*!
        @brief enum containing the possible parts of a protocol file set (in the first implementation, protocols could have three parts: Header, Cyclically overwritten part and Footer)
    */
    enum Part
    {
        None,
        Complete,
        Header,
        Cyclic,
        Footer,
        MaxPart // dummy element. Must be the last one in the enum! Used to determine the size of arrays.
    };

    /*!
        @brief  create the destination file
        @param  destinationFileName [in]    the name of the destination file
        @param  errList             [out]   contains any error messages 
        @return                             true if successful, false if not
    */
    SAPDB_Bool CreateDestinationFile(RTE_Path &destinationFileName,SAPDBErr_MessageList &errList);
 
private:
 

    SAPDB_Bool SearchLastWrittenMessageList(RTE_FileOffset endPosition,RTE_FileOffset &writePosition,SAPDB_UInt8 &biggestSequenceNumber,SAPDB_UInt8 &lengthOfGarbage,Msg_List &errList);

    SAPDB_Bool CloseDestinationFile(SAPDBErr_MessageList &errList);

    SAPDB_Bool OpenSourceFile(Part part,SAPDBErr_MessageList &errList);

    SAPDB_Bool SearchLastWrittenCharacter(RTE_FileOffset &position,Msg_List &errList);

    SAPDB_Bool CloseSourceFile(Msg_List &errList);

    SAPDB_Bool AddSourceToDestinationFile(RTE_FileOffset start,RTE_FileOffset end,SAPDBErr_MessageList &errList) ;
 
    SAPDB_Bool CopyAndMergeToDestinationFile(const long* Buffer,tRTESys_IOPosition BytesToCopy,tRTESys_IOPosition &BytesWritten);

    SAPDB_Bool WriteXMLHeaderToDestinationFile(SAPDBErr_MessageList &errList) ;

    SAPDB_Bool WriteXMLFooterToDestinationFile(SAPDBErr_MessageList &errList);

    void GetTextFromFile(Part part,RTE_FileOffset begin,SAPDB_UInt length,SAPDB_Char *destination,SAPDB_UInt sizeOfDestination);

    void OutputTextFromFile(Part part,RTE_FileOffset begin,SAPDB_UInt length);

    SAPDB_Bool ScanSourceFile(Msg_List &err);

    void CheckStructureConsistency(Msg_Message *pMessage);

    void AnalyzeStructure(Msg_Element *pElement,SortMode sortMode);

    void ReSortByMSGLNO(Msg_Element *&pElement);

    void OutputMessage(Msg_Message *pCurrentMessage,Format format,SAPDB_Bool verbose);

    void OutputMessageListHeader(Msg_MessageList *pCurrentMessage,Format format);

    void OutputXmlMessageList(Msg_MessageList *pCurrentMessage);

    void DoOutput   ( const SAPDB_Char *string1
                    , const SAPDB_Char *string2 = NULL
                    , const SAPDB_Char *string3 = NULL
                    , const SAPDB_Char *string4 = NULL
                    , const SAPDB_Char *string5 = NULL
                    , const SAPDB_Char *string6 = NULL
                    , const SAPDB_Char *string7 = NULL
                    , const SAPDB_Char *string8 = NULL
                    , const SAPDB_Char *string9 = NULL
                    , const SAPDB_Char *string10 = NULL
                    , const SAPDB_Char *string11 = NULL
                    , const SAPDB_Char *string12 = NULL
                    , const SAPDB_Char *string13 = NULL
                    , const SAPDB_Char *string14 = NULL
                    );

    void DoOutputSingleString(const SAPDB_Char *string);

    Msg_MessageList *AnalyzeMSGLAttributes(Msg_MessageList *pNewMessageList,Msg_Element *pElement,RTE_FileOffset &beginOfCurrentThread,SAPDB_UInt &lengthOfCurrentThread,RTE_FileOffset &beginOfCurrentTask,SAPDB_UInt &lengthOfCurrentTask);

    Msg_MessageList *ProcessMSGL(Msg_Element *&pElement);

    Msg_Message *ProcessMSG(Msg_Element *pElement,RTE_FileOffset beginOfThread,SAPDB_UInt lengthOfThread,RTE_FileOffset beginOfTask,SAPDB_UInt lengthOfTask);

    static void ProcessString(const SAPDB_Char *text,SAPDB_Char *processedText, SAPDB_UInt lengthOfProcessedText, SAPDB_Char const **tagNameArray, SAPDB_Char const **tagValueArray,SAPDB_UInt numberOfAttributes);

    void ProcessAndOutputStringWithFormatting(const SAPDB_Char *text,SAPDB_UInt lengthOfProcessedText, SAPDB_Char const **tagNameArray, SAPDB_Char const **tagValueArray,SAPDB_UInt numberOfAttributes);

    static void ResolveEncodedCharacters(SAPDB_Char *processedText);

    SAPDB_Bool AttributeMatchesTag(Part part,Attribute &attribute,Tag &tag);

    SAPDB_Bool ReadCompleteSourceFile(Msg_List &err);

    Msg_Element                 *m_pFirstElementOfHeader;
    Msg_Element                 *m_pFirstElementOfCyclic;
    Msg_Element                 *m_pFirstElementOfFooter;
    RTE_Path                 m_fileName;
    Msg_IOutput::OutputStyle m_style;
    RTE_FileHandle           m_destinationFile;
    RTE_FileHandle           m_sourceFile;
    Part                     m_currentPart;

    RTE_FileHandle           m_headerFile;
    RTE_FileHandle           m_cyclicFile;
    RTE_FileHandle           m_footerFile;
    RTE_FileHandle           m_completeFile;

    Msg_MessageList         *m_pFirstMessageList;
    Msg_MessageList         *m_pLastMessageList;
    Msg_MessageList         *m_pInsertPosition;
    Msg_Message             *m_pFirstMessage;
    Msg_Message             *m_pLastMessage;
    SAPDB_Bool              m_insertMode;
    SAPDB_Bool              m_longColumns;
    SAPDB_UInt              m_elementCounter;
    SAPDB_Char             *m_fileBuffer[MaxPart];
    RTE_FileOffset          m_fileSize[MaxPart];
friend class Msg_FileScanner;
}; /* @endclass Msg_FileProcessor */

#endif /* MSG_FILEPROCESSOR_HPP */
