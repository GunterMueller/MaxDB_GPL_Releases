/*!
    \file    vcn383.cpp
    \author  TiloH
    \ingroup cn layer
    \brief   implementation of functions for handling history file of
             Backint for MaxDB

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2006 SAP AG

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
#include "hcn383.h"
#include "hcn385.h"

#include "SAPDBCommon/SAPDB_string.h"

#include <stdio.h>

int cn383_WriteInformationToHistoryFile(const char                     * HistoryFileName,
                                        ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                        const char                     * UserID)
{
    int rc=1;

    tni34_ATextFile OwnHistory(HistoryFileName, tni34_AFile::ToDiskAppend);

    if(!OwnHistory.IsGood())
        rc=0;
    else
    {
        ni31_ListElem<tcn381_InputEntry *> *File;
        ni31_ListElem<char *> *PartialBID;
        size_t MaxLineLength=0;
        size_t LineLength;

        // loop through the list of files and determine the maximal line length
        for(File=TheInputFileList.First(); 0!=File; File=File->Next())
            if(0!=UserID && 0!=File->GiveInfo()->BID && 0!=File->GiveInfo()->Name && 0!=File->GiveInfo()->StageName)
            {
                LineLength=strlen(UserID)+1
                           +strlen(File->GiveInfo()->BID)+1
                           +1+1
                           +strlen(File->GiveInfo()->Name)+1
                           +strlen(File->GiveInfo()->StageName)+1
                           +40+1                                    //enough place for an unsigned 64-bit integer
                           +40+1;                                   //enough place for an integer 
            
                for(PartialBID=File->GiveInfo()->ListOfPartialBIDs.First(); 0!=PartialBID && 0!=PartialBID->GiveInfo(); PartialBID=PartialBID->Next())
                    LineLength+=strlen(PartialBID->GiveInfo())+1;

                if(MaxLineLength<LineLength)
                    MaxLineLength=LineLength;
            }

        //get space for the largest line
        char * Buf=new char [MaxLineLength+1];
        
        if(0==Buf)
        {
            rc=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
        }
        else
        {
            char * EndOfBuf;
            long BytesWritten;

            //loop again and write the information
            for(File=TheInputFileList.First(); 0!=File; File=File->Next())
                if(0!=UserID && 0!=File->GiveInfo()->BID && 0!=File->GiveInfo()->Name && 0!=File->GiveInfo()->StageName)
                {
                    sprintf(Buf, "%s %s %c %s %s ", UserID,
                                                    File->GiveInfo()->BID,
                                                    File->GiveInfo()->IsPipe? 'P':'F',
                                                    File->GiveInfo()->Name,
                                                    File->GiveInfo()->StageName);

                    EndOfBuf=cn36_TerminatingZeroOf(Buf);

                    if(!cn36_UInt8ToStrN(EndOfBuf, MaxLineLength-(EndOfBuf-Buf), File->GiveInfo()->StageSize))
                    {
                        rc=0;   // we should not get here, because there must be enough space for the number
                        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                    }

                    EndOfBuf=Buf+strlen(Buf);
                    sprintf(EndOfBuf, " %d", int(File->GiveInfo()->NumberOfParts));

                    EndOfBuf=Buf+strlen(Buf);

                    int NumberOfPartialBIDsAdded=0;

                    for(PartialBID=File->GiveInfo()->ListOfPartialBIDs.First(); 0!=PartialBID && 0!=PartialBID->GiveInfo(); PartialBID=PartialBID->Next())
                    {
                        sprintf(EndOfBuf, " %s", PartialBID->GiveInfo());
                        EndOfBuf=EndOfBuf+strlen(EndOfBuf);
                    
                        NumberOfPartialBIDsAdded++;
                    }

                    if(1<=File->GiveInfo()->NumberOfParts &&  // check that the line will pass the integrity test of function GetInformationFromOwnHistory()
                       16>=strlen(UserID) &&
                       16>=strlen(File->GiveInfo()->BID) &&
                       File->GiveInfo()->NumberOfParts==NumberOfPartialBIDsAdded)
                    {
                        if(strlen(Buf)>=MaxLineLength)
                        {
                            rc=0; //the string was larger than the largest expected string -> program panic
                            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
                        }
                        else
                        {
                            if(!OwnHistory.WriteLine(Buf, BytesWritten))
                            {
                                rc=0; 
                            }
                        }
                    }
                    else
                        rc=0;   //TODO give error message
                }

            delete [] Buf;
        }

        if(!OwnHistory.Close())
            rc=0;
    }

    return rc;
}

int cn383_GetInformationFromOwnHistory(const char                     * PathOfHistoryFile,
                                       ni31_List<tcn381_InputEntry *> & TheInputFileList,
                                       const char                     * UserID,
                                       const char                     * FunctionString,
                                       ni31_List<tcn381_InputEntry *> * BackUpsInQuestionList,
                                       tcn386_BackintOut              & Output)
{
    int rc=1;
    
    ni31_ListElem<tcn381_InputEntry *> * h;
    tcn381_InputEntry * IE;

    Output.Write("Reading history file ", PathOfHistoryFile, ".");

    if(!strcmp(FunctionString, FunctionStringInquire_cn38) && 0==BackUpsInQuestionList)
        rc=0;
    else
    {    
        tni34_ATextFile OwnHistory(PathOfHistoryFile, tni34_AFile::FromDisk, 0, 0);

        //if Inquire && History is unavailable, do nothing and return rc=1, else open history and proceed
        if(strcmp(FunctionString, FunctionStringInquire_cn38) || OwnHistory.Exists())
        {
            if(!OwnHistory.Open() || !OwnHistory.IsGood())
                rc=0;
            else
            {
                const char * Line;
        
                const int    NumberOfColumns=8;                  // UID BID Type OriginalName StageName StageSize NumberOfParts PartialBID(n) PartialBID(n-1) ...

                enum {IndexUID=0,
                      IndexBID,
                      IndexType,
                      IndexName,
                      IndexStageName,
                      IndexStageSize,
                      IndexNOParts,
                      IndexPartialBIDn};

                const char * Column[NumberOfColumns];
                const char * EndOfColumn[NumberOfColumns];
                size_t       LengthOfColumn[NumberOfColumns];
                int          NumberOfPartialBIDs,
                             NumberOfPartialBIDsFound;
                teo00_Uint8  OriginalStageSize;
                const char * CurrentPosition;
                int i;

                while(rc && OwnHistory.ReadLine(Line) && 0!=Line)
                {
                    if('\0'!=(*cn36_FirstNonWhiteSpaceOf(Line)))
                    {
                        // get the fixed columns from the line
                        CurrentPosition=Line;

                        for(i=0; i<NumberOfColumns; i++)
                        {
                            Column[i]=cn36_FirstNonWhiteSpaceOf(CurrentPosition);
                            EndOfColumn[i]=cn36_FirstWhiteSpaceOf(Column[i]);
                            LengthOfColumn[i]=EndOfColumn[i]-Column[i];

                            CurrentPosition=EndOfColumn[i];
                        }

                        //get number of PartialBIDs in the line
                        if(0!=LengthOfColumn[NumberOfColumns-1])
                        {
                            const char * EndOfCurrentBID;
                            NumberOfPartialBIDsFound=1;

                            while('\0'!=*CurrentPosition)
                            {
                                CurrentPosition=cn36_FirstNonWhiteSpaceOf(CurrentPosition);

                                if('\0'!=*CurrentPosition)
                                    NumberOfPartialBIDsFound++;

                                EndOfCurrentBID=cn36_FirstWhiteSpaceOf(CurrentPosition);

                                if(16<EndOfCurrentBID-CurrentPosition)
                                {
                                    NumberOfPartialBIDsFound=-1;
                                    rc=0;
                                    CurrentPosition+=1+strlen(CurrentPosition); //force the end of the loop
                                }
                                else
                                    CurrentPosition=EndOfCurrentBID;
                            }
                        }
                        else
                            NumberOfPartialBIDsFound=0;

                        // if the format of line is correct ...
                        if( rc &&                                                                   // all ok so far
                            1<=NumberOfPartialBIDsFound &&                                          // at least one PartialBID found and no PartialBID had more than 16 characters
                            16>=LengthOfColumn[IndexUID] &&                                         // UID has not more than 16 characters
                            16>=LengthOfColumn[IndexBID] &&                                         // BID has not more than 16 characters
                            1==LengthOfColumn[IndexType] &&                                         // Type consist of just one character
                            ('P'==Column[IndexType][0] || 'F'==Column[IndexType][0]) &&             // its one of the possible characters
                            cn36_StrNToUInt8(OriginalStageSize, Column[IndexStageSize], LengthOfColumn[IndexStageSize]) && // size of the stage is a number
                            cn36_StrNToInt(NumberOfPartialBIDs, Column[IndexNOParts], LengthOfColumn[IndexNOParts]) &&    // number of parts is a number
                            NumberOfPartialBIDs==NumberOfPartialBIDsFound )                         //found exactly as many PartialBIDs as specified
                        {
                            if(!cn36_StrStrNCmp(UserID, Column[IndexUID], LengthOfColumn[IndexUID]))  //the User ID must be the same
                            {
                                if(!strcmp(FunctionString, FunctionStringInquire_cn38))
                                {
                                    // ... check, if it is inquired
                                    int LineNeededForCurrentInput=0;

                                    for(h=TheInputFileList.First(); rc && 0!=h && !LineNeededForCurrentInput; h=h->Next()) // test every input against the current Line
                                    {
                                        IE=h->GiveInfo(); // a shortcut
                        
                                        if(IE->EqualsToBIDAndName(Column[IndexBID], LengthOfColumn[IndexBID], Column[IndexName], LengthOfColumn[IndexName])) // the information of the current line fits to the inquire input IE
                                        {
                                            tcn381_InputEntry * NewEntry=new tcn381_InputEntry(Column[IndexType],
                                                                                               LengthOfColumn[IndexType],
                                                                                               Column[IndexName],                 // the name
                                                                                               LengthOfColumn[IndexName],
                                                                                               Column[IndexBID],                  // the BID
                                                                                               LengthOfColumn[IndexBID],
                                                                                               Column[IndexStageName],            // the stage name
                                                                                               LengthOfColumn[IndexStageName],
                                                                                               Column[IndexStageSize],            // the stage size
                                                                                               LengthOfColumn[IndexStageSize],
                                                                                               Column[IndexPartialBIDn]);

                                            if(0==NewEntry || !NewEntry->IsGood)
                                            {
                                                rc=0;
                                                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                                            }
                                            else
                                                BackUpsInQuestionList->Add(NewEntry);
                            
                                            LineNeededForCurrentInput=1; //end search, this Backup is already in question
                                        }
                                    }
                                }
                                else
                                    if(!strcmp(FunctionString, FunctionStringRestore_cn38))
                                    {
                                        // get the missing information from the history
                                        for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next()) // test every input against the current Line
                                        {
                                            IE=h->GiveInfo(); // a shortcut

                                            if(IE->EqualsToBIDAndName(Column[IndexBID], LengthOfColumn[IndexBID], Column[IndexName], LengthOfColumn[IndexName])) // the information of the current line fits to the inquire input IE
                                            {
                                                // only the information of the last (newest) matching history line will survive 
                                                rc=IE->FillHistoryInformation(Column[IndexType],
                                                                              LengthOfColumn[IndexType],
                                                                              Column[IndexStageName],
                                                                              LengthOfColumn[IndexStageName],
                                                                              Column[IndexStageSize],
                                                                              LengthOfColumn[IndexStageSize],
                                                                              Column[IndexPartialBIDn]);
                                            }
                                        }
                                    }
                                    else
                                        rc=0;
                            }
                        }
                        else
                            rc=0;
                    }
                    //else just ignore empty lines
                }

                OwnHistory.Close();
            }
        }
    }

    //on restore, check for input lines using the #NULL feature
    if(!strcmp(FunctionString, FunctionStringRestore_cn38))
    {
        ni31_ListElem<tcn381_InputEntry *> * h;

        for(h=TheInputFileList.First(); rc && 0!=h; h=h->Next()) // test every input line
        {
            IE=h->GiveInfo(); // a shortcut

            if(!strcmp("#NULL", IE->BID)) //#NULL was specified instead of a concrete BID
            {
                if(0==IE->ListOfPartialBIDs.First())
                    rc=0;
                else
                    SAPDB_strcpy(IE->BID, IE->ListOfPartialBIDs.First()->GiveInfo()); //substitute #NULL with the BID of the last save for the name
            }
        }
    }

    return rc;
}
