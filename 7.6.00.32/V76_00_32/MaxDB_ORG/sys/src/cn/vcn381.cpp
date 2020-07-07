/*******************************************************************************
  module:       vcn381.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: cn layer
  description:  implementation of basic classes for Backint for SAP DB
  last change:  2000-11-29 16:00
  version:      7.2.5
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2000-2005 SAP AG


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



*******************************************************************************/

#include "hcn36.h"
#include "hcn381.h"
#include "hcn385.h"

#include "SAPDBCommon/SAPDB_string.h"

#include <stdio.h>

tcn381_APipeToFileConverter::tcn381_APipeToFileConverter(void)
    :CompleteStageName(0),
    EndOfStageName(0),
    DestinationStageDirectory(0),
    CompleteDestinationStage(0),
    EndOfCompleteDestinationStage(0),
    Process(0),
    InPipe(0),
    OutPipe(0),
    WasRunning(0),
    State(StageIsFree),
    InputInfo(0)
{
}

tcn381_APipeToFileConverter::~tcn381_APipeToFileConverter()
{
    if(0!=CompleteStageName)
        delete [] CompleteStageName;

    if(0!=DestinationStageDirectory)
        delete [] DestinationStageDirectory;
    
    if(0!=CompleteDestinationStage)
        delete [] CompleteDestinationStage;
}



tcn381_InputEntry::tcn381_InputEntry(int          IsAPipe,
                                     const char * TheName)
{
    InitAllMembers();
    IsPipe=IsAPipe;
    SetName(TheName, strlen(TheName));
}

tcn381_InputEntry::tcn381_InputEntry(int          IsAPipe,
                                     const char * TheName,
                                     size_t       NameLength)
{
    InitAllMembers();
    IsPipe=IsAPipe;
    SetName(TheName, NameLength);
}

tcn381_InputEntry::tcn381_InputEntry(const char * ALine,
                                     const char * Function)
{
    InitAllMembers();

    const int    MaxNumberOfColumnsPerLine=3;
    const char * Column[MaxNumberOfColumnsPerLine],
               * CurrentPosition=ALine;

    size_t LengthOfColumn[MaxNumberOfColumnsPerLine];
    int i;

    for(i=0; i<MaxNumberOfColumnsPerLine; i++)
    {
        Column[i]=cn36_FirstNonWhiteSpaceOf(CurrentPosition);
        CurrentPosition=cn36_FirstWhiteSpaceOf(Column[i]);
        LengthOfColumn[i]=CurrentPosition-Column[i];
    }
        
    int IndexTheName,
        IndexPipeKeyWord,
        IndexTheBID,
        IndexDestination;

    if(!strcmp(Function, FunctionStringBackup_cn38))
    {
        IndexTheName=0;
        IndexPipeKeyWord=1;

        if(!cn36_StrStrNCmp("#PIPE", Column[IndexPipeKeyWord], LengthOfColumn[IndexPipeKeyWord]))
            IsPipe=1;
        else
            IsPipe=0;

        SetName(Column[IndexTheName], LengthOfColumn[IndexTheName]);

        if(!IsPipe)
            Set(StageName, Column[IndexTheName],  LengthOfColumn[IndexTheName]);
    }
    else
        if(!strcmp(Function, FunctionStringInquire_cn38))
        {
            IsPipe=-1;
            IndexTheBID=0;
            IndexTheName=1;

            if(0<LengthOfColumn[IndexTheBID] && LengthOfColumn[IndexTheBID]<=16)
            {
                cn36_StrNCpy(BID, Column[IndexTheBID], LengthOfColumn[IndexTheBID]);
                SetName(Column[IndexTheName], LengthOfColumn[IndexTheName]);
            }
            else
                IsGood=0;
        }
        else
            if(!strcmp(Function, FunctionStringRestore_cn38))
            {
                IndexTheBID=0;
                IndexTheName=1;
                IndexDestination=2;

                if(0<LengthOfColumn[IndexTheBID] && LengthOfColumn[IndexTheBID]<=16)    // we have to assure
                {
                    cn36_StrNCpy(BID, Column[IndexTheBID], LengthOfColumn[IndexTheBID]);
                    SetName(Column[IndexTheName], LengthOfColumn[IndexTheName]);
                    
                    if(0<LengthOfColumn[IndexDestination])
                        Set(DestinationName, Column[IndexDestination], LengthOfColumn[IndexDestination]);
                }
                else
                    IsGood=0;
            }
}

tcn381_InputEntry::tcn381_InputEntry(const char * Type,
                                     size_t       TypeLength,
                                     const char * TheName,
                                     size_t       NameLength,
                                     const char * TheBID,
                                     size_t       BIDLength,
                                     const char * TheStageName,
                                     size_t       StageNameLength,
                                     const char * TheStageSize,
                                     size_t       StageSizeLength,
                                     const char * PartialBIDs)
{
    InitAllMembers();

    if(SetTypeTo(Type, TypeLength))
    {
        SetName(TheName, NameLength);

        if(!cn36_StrNToUInt8(StageSize, TheStageSize, StageSizeLength))
            IsGood=0;
        else
        {
            StageName=new char [StageNameLength+20+1];

            if(0==StageName)
            {
                TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
                IsGood=0;
            }
            else
            {
                cn36_StrNCpy(StageName, TheStageName, StageNameLength);

                if(16<BIDLength)
                    IsGood=0;
                else
                    cn36_StrNCpy(BID, TheBID, BIDLength);
    
                SetPartialBIDs(PartialBIDs); // sets already IsGood=0 in case of error;
            }
        }
    }
}

tcn381_InputEntry::~tcn381_InputEntry()
{
    if(0!=Name)
        delete [] Name;
    
    if(0!=StageName)
        delete [] StageName;

    if(0!=RestoreStageName)
        delete [] RestoreStageName;

    if(0!=DestinationName)
        delete [] DestinationName;

    DeletePartialBIDs();
}

int tcn381_InputEntry::SetBID(const char * TheBID,
                              size_t       LengthOfBID)
{
    int rc=0;

    if(16>=LengthOfBID)
    {
        if('\0'==BID[0])
            cn36_StrNCpy(BID, TheBID, LengthOfBID);

        char * CopyOfBID=new char [LengthOfBID+1];

        if(0!=CopyOfBID)
        {
            cn36_StrNCpy(CopyOfBID, TheBID, LengthOfBID);
            ListOfPartialBIDs.Add(CopyOfBID);
            NumberOfParts++;
            rc=1;
        }
        else
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }

    return rc;
}

int tcn381_InputEntry::EqualsToBIDAndName(const char * TheBID,
                                          size_t       LengthOfTheBID,
                                          const char * TheName,
                                          size_t       LengthOfTheName)
{
    int rc=1;

    if(strcmp(BID, "#NULL") &&
       strcmp(TheBID, "#NULL") &&
       cn36_StrStrNCmp(BID, TheBID, LengthOfTheBID))
    {
        rc=0;
    }
    else
    {
        if('\0'!=Name[0] &&
           LengthOfTheName!=0 && '\0'!=TheName[0] &&
           cn36_StrStrNCmp(Name, TheName, LengthOfTheName))
        {
            rc=0;
        }
    }

    return rc;
}
    
int tcn381_InputEntry::EqualsToBIDAndName(const tcn381_InputEntry & Other)
{
    return EqualsToBIDAndName(Other.BID,
                             strlen(Other.BID),
                             Other.Name,
                             strlen(Other.Name));
}

int tcn381_InputEntry::FillHistoryInformation(const char * Type,
                                              size_t       TypeLength,
                                              const char * TheStageName,
                                              size_t       StageNameLength,
                                              const char * TheStageSize,
                                              size_t       StageSizeLength,
                                              const char * PartialBIDs)
{
    int rc=SetTypeTo(Type, TypeLength);

    if(rc)
        rc=SetPartialBIDs(PartialBIDs);

    if(rc)
    {
        if(0!=StageName)
            delete StageName;

        StageName=new char [StageNameLength+20+1];

        if(0==StageName)
        {
            IsGood=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
        }
        else
        {
            cn36_StrNCpy(StageName, TheStageName, StageNameLength);

            rc=cn36_StrNToUInt8(StageSize, TheStageSize, StageSizeLength);

            if(!rc)
                IsGood=0;
        }
    }

    return rc;
}

int tcn381_InputEntry::SetPartialBIDs(const char * PartialBIDs)
{
    size_t    LengthNewBID;
    char * NewBID;

    DeletePartialBIDs();

    while(IsGood && '\0'!=(*PartialBIDs))
    {
        LengthNewBID=cn36_FirstWhiteSpaceOf(PartialBIDs)-PartialBIDs;

        NewBID=new char [LengthNewBID+1];

        if(0==NewBID)
        {
            IsGood=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
        }
        else
        {
            cn36_StrNCpy(NewBID, PartialBIDs, LengthNewBID);
            ListOfPartialBIDs.Add(NewBID);
        }
        
        PartialBIDs=cn36_FirstNonWhiteSpaceOf(PartialBIDs+LengthNewBID);
        NumberOfParts++;
    }

    return IsGood;
}

void tcn381_InputEntry::DeletePartialBIDs(void)
{
    if(0!=NumberOfParts)
    {
        ni31_ListElem<char *> *h;

        for(h=ListOfPartialBIDs.First(); 0!=h; h=h->Next())
            if(0!=h->GiveInfo())
                delete h->GiveInfo();

        ListOfPartialBIDs.DelAll();
        NumberOfParts=0;
    }
}

void tcn381_InputEntry::SetName(const char * NewName,
                                size_t       NameLength)
{
    Set(Name, NewName, NameLength);
}

void tcn381_InputEntry::SetDestination(const char * NewDest,
                                       size_t       DestLength)
{
    Set(DestinationName, NewDest, DestLength);
}

void tcn381_InputEntry::Set(char * &     MemberToSet,
                            const char * NewValue,
                            size_t       ValueLength)
{
    if(0!=MemberToSet)
        delete [] MemberToSet;

    if(0!=NewValue && 0!=ValueLength)
    {
        MemberToSet=new char [ValueLength+1];

        if(0==MemberToSet)
        {
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            IsGood=0;
        }
        else
            cn36_StrNCpy(MemberToSet, NewValue, ValueLength);
    }
    else
    {
        MemberToSet=new char [1];

        if(0!=MemberToSet)
            MemberToSet[0]='\0';
        else
        {
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
            IsGood=0;
        }
    }
}

void tcn381_InputEntry::InitAllMembers(void)
{
    Name=0;
    StageName=0;
    StageSize=0;
    RestoreStageName=0;
    DestinationName=0;
    IsPipe=0;
    BID[0]='\0';
    ProcessedSuccessful=1;
    NumberOfParts=0;
    RestorePart=0;
    BytesWrittenToDisk=teo00_Uint8(0);
    Converter=0;
    FoundInThisBackintOutput=0;
    FoundPartInThisBackintOutput=0;
    IsGood=1;
}

int tcn381_InputEntry::SetTypeTo(const char * Type,
                                 size_t       TypeLength)
{
    int rc=1;

    if(1==TypeLength)
    {
        switch(Type[0])
        {
            case 'P': IsPipe=1; break;
            case 'F': IsPipe=0; break;
            default : rc=0;
        }
    }
    else
        rc=0;

    if(!rc)
        IsGood=rc;

    return rc;
}

tcn381_OutputLine::tcn381_OutputLine(ErrorType    Type,
                                     const char * Name)
{
    size_t Length=strlen("#NOTFOUND")+1+strlen(Name);

    Content=new char [Length+1];

    if(0==Content)
    {
        IsGood=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }
    else
    {
        IsGood=1;

        if(Type==Error)
            sprintf(Content, "%s %s", "#ERROR", Name);
        else
            sprintf(Content, "%s %s", "#NOTFOUND", Name);
    }
}

tcn381_OutputLine::tcn381_OutputLine(SuccessType  Type,
                                     const char * BID,
                                     const char * Name)
{
    size_t Length=strlen(KeyWordFor(Type))+1+strlen(BID)+1;
    
    if(0!=Name)
        Length+=strlen(Name);

    Content=new char [Length+1];

    if(0==Content)
    {
        IsGood=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }
    else
    {
        IsGood=1;

        if(0!=Name)
            sprintf(Content, "%s %s %s", KeyWordFor(Type), BID, Name);
        else
            sprintf(Content, "%s %s", KeyWordFor(Type), BID);
    }
}

tcn381_OutputLine::tcn381_OutputLine(SuccessType       Type,
                                     const char *      BID,
                                     const char *      Name,
                                     const teo00_Uint8 Size)
{
    char SizeAsString[50];
    size_t Length;
    
    IsGood=1;

    if(!cn36_UInt8ToStrN(SizeAsString, 50, Size))
    {
        IsGood=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
        SAPDB_strcpy(SizeAsString, "0");
    }

    Length=strlen(KeyWordFor(Type))+1+strlen(BID)+1+strlen(Name)+1+strlen(SizeAsString);

    Content=new char [Length+1];

    if(0==Content)
    {
        IsGood=0;
        TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::OutOfMemory));
    }
    else
    {
        sprintf(Content, "%s %s %s %s", KeyWordFor(Type), BID, Name, SizeAsString);

        if(strlen(Content)!=Length)
        {
            IsGood=0;
            TheBackint2ErrorHandler_cn385->Report(tcn385_Backint2Error(tcn385_Backint2Error::InternalError));
        }
    }
}

tcn381_OutputLine::~tcn381_OutputLine()
{
    if(0!=Content)
        delete [] Content;
}

const char * tcn381_OutputLine::KeyWordFor(SuccessType Type)
{
    const char *rc=0;

    switch(Type)
    {
        case Saved:      rc="#SAVED"; break;
        case Restored:   rc="#RESTORED"; break;
        case Found:      rc="#BACKUP"; break;
        case NotDeleted: rc="#NOTDELETED"; break;
    }

    return rc;
}
