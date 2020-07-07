/*!
    \file    DBMSrvBHist_Part.cpp
    \author  TiloH
    \ingroup backup history handling by the DBMServer
    \brief   implementing a class for handling one part of a copy of a backup

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

#include "DBM/Srv/BackupHistory/DBMSrvBHist_Part.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "hcn36.h"
#include "hcn90.h"


//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

size_t DBMSrvBHist_Part_ColumnMinLength[DBMSrvBHist_Part::NoColumn]
    ={ 64,  //MediumName
       10,  //MediumType
        3,  //MediumOverwrite
       10,  //MediumSize
       10,  //MediumBlockSize
        1,  //MediumKind
      256,  //MediumLocation
        4}; //BackupToolStringColumn

DBMSrvBHist_LineProducer::Alignment DBMSrvBHist_Part_ColumnAlignment[DBMSrvBHist_Part::NoColumn]
    ={DBMSrvBHist_LineProducer::Left,  //MediumName
      DBMSrvBHist_LineProducer::Left,  //MediumType
      DBMSrvBHist_LineProducer::Left,  //MediumOverwrite
      DBMSrvBHist_LineProducer::Right, //MediumSize
      DBMSrvBHist_LineProducer::Right, //MediumBlockSize
      DBMSrvBHist_LineProducer::Left,  //MediumKind
      DBMSrvBHist_LineProducer::Left,  //MediumLocation
      DBMSrvBHist_LineProducer::Left}; //BackupToolStringColumn

const int DBMSrvBHist_Part_COLS_MEDIAHIST=9;
const int DBMSrvBHist_Part_COLS_EBIDHIST=16;

const char * DBMSrvBHist_Part_EBIDSTATUS_UNKNOWN    ="UNKNOWN";
const char * DBMSrvBHist_Part_EBIDSTATUS_NONE       ="NONE";
const char * DBMSrvBHist_Part_EBIDSTATUS_AVAILABLE  ="AVAILABLE";
const char * DBMSrvBHist_Part_EBIDSTATUS_UNAVAILABLE="UNAVAILABLE";

const char * DBMSrvBHist_Part_LBLLOG="LOG";

const char * DBMSrvBHist_Part_MEDIUMKIND_REPLACE="R";

const char * DBMSrvBHist_Part_MEDIUMTYPE_UNKNOWN="UNKNOWN";
const char * DBMSrvBHist_Part_MEDIUMTYPE_FILE   ="FILE";
const char * DBMSrvBHist_Part_MEDIUMTYPE_TAPE   ="TAPE";
const char * DBMSrvBHist_Part_MEDIUMTYPE_PIPE   ="PIPE";

const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_FILE="vftFile";
const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_TAPE="vftTapeRew";
const char * DBMSrvBHist_Part_MEDIUMTYPE_HIST_PIPE="vftPipe";


//-----------------------------------------------------------------------------
// member functions of class DBMSrvBHist_Part
//-----------------------------------------------------------------------------

DBMSrvBHist_Part::DBMSrvBHist_Part(PartOrigin TheOrigin)
{
    Key=0;
    Label=0;

    MediumName=0;
    MediumType=0;
    MediumOverwrite=0;
    MediumSize=0;
    MediumBlockSize=0;
    MediumKind=0;
    MediumLocation=0;

    EBID=0;

    BackupType=0;
    BackupDateTime=0;

    BackupToolString=0;
    DBMReturnCode=0;

    PartNumber=0;
    DBMReturnText=0;
    Status=EBIDStatusUnknown;

    PartAdded=0;

    ExtendedAvailableFlag=0;

    Origin=TheOrigin;
};

DBMSrvBHist_Part::~DBMSrvBHist_Part()
{
    cn36_StrDealloc(Key);
    cn36_StrDealloc(Label);

    cn36_StrDealloc(MediumName);
    cn36_StrDealloc(MediumType);
    cn36_StrDealloc(MediumOverwrite);
    cn36_StrDealloc(MediumSize);
    cn36_StrDealloc(MediumBlockSize);
    cn36_StrDealloc(MediumKind);
    cn36_StrDealloc(MediumLocation);

    cn36_StrDealloc(EBID);

    cn36_StrDealloc(BackupType);
    cn36_StrDealloc(BackupDateTime);

    cn36_StrDealloc(BackupToolString);
    cn36_StrDealloc(DBMReturnText);
}

bool DBMSrvBHist_Part::SetToMedium(
    const char      * MediaHistoryFileLine,
    DBMSrvMsg_Error & error)
{
    bool         rc=true;
    const char * CurPos=MediaHistoryFileLine;

    char ** Part[DBMSrvBHist_Part_COLS_MEDIAHIST]={&Key,
                                                   &Label,
                                                   &MediumName,
                                                   &MediumType,
                                                   &MediumOverwrite,
                                                   &MediumSize,
                                                   &MediumBlockSize,
                                                   &MediumKind,
                                                   &MediumLocation};

    for(int i=0; rc && i<DBMSrvBHist_Part_COLS_MEDIAHIST; i++)
    {
        const char * Separator;
        
        if(i<DBMSrvBHist_Part_COLS_MEDIAHIST-1)
            Separator=strchr(CurPos, '|');
        else
            Separator=CurPos+strlen(CurPos);   //the last column of dbm.mdf (the end of each line) is not delimited by a '|'

        if(0==Separator)
        {
            char * shortForm=0;

            if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, CurPos, error))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_SEPARATORMISSING,
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__STRING, shortForm),
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__SEPARATOR, "|")));
            }

            if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, MediaHistoryFileLine, error))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_MED_INVALID_ENTRY,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_MED_INVALID_ENTRY__LINE2, shortForm)));
            }

            cn36_StrDealloc(shortForm);

            rc=false;
        }
        else
        {
            if(!ToolsCStr_Str::NAllocCpy(*Part[i], CurPos, Separator-CurPos, error))
                rc=false;

            if(i<DBMSrvBHist_Part_COLS_MEDIAHIST-1)
                CurPos=Separator+1;
        }
    }

    //set all other strings to empty strings and return code to OK
    if(rc)
    {
        if(!ToolsCStr_Str::AllocCpy(EBID, "", error) ||
           !ToolsCStr_Str::AllocCpy(BackupType, "", error) ||
           !ToolsCStr_Str::AllocCpy(BackupDateTime, "", error) ||
           !ToolsCStr_Str::AllocCpy(BackupToolString, DBMSrvBTools_ToolEnum(DBMSrvBTools_ToolEnum::None).AsString(), error) ||
           !ToolsCStr_Str::AllocCpy(DBMReturnText, "", error)       )
        {
            rc=false;
        }

        DBMReturnCode=0;
    }

    Status=EBIDStatusNone;    // if it was a backup with external backup tool we will find out through dbm.ebf later (if dbm.ebf is complete)

    return rc;
}

bool DBMSrvBHist_Part::SetToEBID(
    const char      * ExternalHistoryFileLine,
    DBMSrvMsg_Error & error)
{
    bool         rc=true;
    int          NumberOfPartsInLine=0;
    const char * Separator=strchr(ExternalHistoryFileLine, '|');

    //count number of parts in ExternalHistoryFileLine
    while(0!=Separator)
    {
        NumberOfPartsInLine++;
        Separator=strchr(Separator+1, '|');
    }

    //check if there are an expected numbers of parts
    if( 3!=NumberOfPartsInLine &&   //if it is neither the first format ("<Key>|<Label>|<EBID>|")
       15!=NumberOfPartsInLine &&   //nor the second format ("<Key>|<Label>|<EBID>|<BackupType>|...|<DBMRC>|<MediumName>|...|<PartNumber>|")
       16!=NumberOfPartsInLine    ) //nor the third one ("<Key>|<Label>|<EBID>|<BackupType>|...|<DBMRC>|<DBMErrorText>|<MediumName>|...|<PartNumber>|")
    {
        char * shortForm=0;

        if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, ExternalHistoryFileLine, error))
        {
            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_SEPARATORNO3,
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__STRING, shortForm),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__SEPARATOR, "|"),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__NUMBER1, 3),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__NUMBER2, 15),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__NUMBER3, 16),
	                                    Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNO3__NUMBER_FOUND, NumberOfPartsInLine)));

            error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_EXT_INVALID_LINE,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_EXT_INVALID_LINE__LINE2, shortForm)));
        }

        cn36_StrDealloc(shortForm);

        rc=false;
    }
    else
    {
        const char * CurPos=ExternalHistoryFileLine;
        char       * PartNumberAsString=0;
        char       * DBMReturnCodeAsString=0;
        char      ** Part[DBMSrvBHist_Part_COLS_EBIDHIST]={&Key,
                                                           &Label,
                                                           &EBID,
                                                           &BackupType,
                                                           &BackupDateTime,
                                                           &BackupToolString,
                                                           &DBMReturnCodeAsString,
                                                           &DBMReturnText,
                                                           &MediumName,
                                                           &MediumType,
                                                           &MediumOverwrite,
                                                           &MediumSize,
                                                           &MediumBlockSize,
                                                           &MediumKind,
                                                           &MediumLocation,
                                                           &PartNumberAsString};
        int i=0;

        //init all strings
        for(i=0; rc && i<DBMSrvBHist_Part_COLS_EBIDHIST; i++)
        {
            if(!ToolsCStr_Str::AllocCpy(*Part[i], "", error))
            {
                rc=false;
            }
        }

        //mark parts missing in older formats
        if(16>NumberOfPartsInLine)  //if it is one of the first two formats
            Part[7]=0;              //no DBMReturnText in older dbm.ebf versions

        //copy the available parts
        for(i=0, Separator=strchr(CurPos, '|'); rc && 0!=Separator; i++)
        {
            //ignore unset parts
            while(i<DBMSrvBHist_Part_COLS_EBIDHIST && 0==Part[i])
                i++;

            //try to copy the value
            if(i<DBMSrvBHist_Part_COLS_EBIDHIST && !ToolsCStr_Str::NAllocCpy(*Part[i], CurPos, Separator-CurPos, error))
            {
                rc=false;
            }

            //go to the next part
            CurPos=Separator+1;
            Separator=strchr(CurPos, '|');
        }

        if(rc)
        {
            if('\0'==PartNumberAsString[0])
                PartNumber=0;

            if('\0'==DBMReturnCodeAsString[0])
                DBMReturnCode=0;

            if(('\0'!=PartNumberAsString[0] && !ToolsCStr_Str::ToInt(PartNumber, PartNumberAsString, error)) ||        //if set PartNumberAsString and DBMReturnCodeAsString must represent integers
               ('\0'!=DBMReturnCodeAsString[0] && !ToolsCStr_Str::ToInt(DBMReturnCode, DBMReturnCodeAsString, error)))
            {
                char * shortForm=0;

                if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, ExternalHistoryFileLine, error))
                {
                    error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_EXT_INVALID_LINE,
                                        Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_EXT_INVALID_LINE__LINE2, shortForm)));
                }

                cn36_StrDealloc(shortForm);

                rc=false;
            }
        }
     
        if(rc)
        {
            //set the EBIDStatus according to used backup tool type
            if(DBMSrvBTools_ToolEnum::None!=DBMSrvBTools_ToolEnum(BackupToolString))
                Status=EBIDStatusUnknown; // if it was a tool ->EBID is unknown
            else
                Status=EBIDStatusNone;    // if it was no tool ->no EBID at all
        }

        cn36_StrDealloc(PartNumberAsString);    //last chance
        cn36_StrDealloc(DBMReturnCodeAsString);
    }

    return rc;
}

tcn00_Error  DBMSrvBHist_Part::SetToEBIDInfo(DBMSrvBHist_Part::EBIDStatus   NewStatus,
                                             const char                   * NewKey,
                                             size_t                         NewKeyLength,
                                             const char                   * NewLabel,
                                             size_t                         NewLabelLength,
                                             const char                   * NewEBID,
                                             size_t                         NewEBIDLength,
                                             const char                   * NewType,
                                             const char                   * NewDateTime)
{
    tcn00_Error  rc=OK_CN00;

    if(!cn36_StrNAllocCpy(Key, NewKey, NewKeyLength) ||
       !cn36_StrNAllocCpy(Label, NewLabel, NewLabelLength) ||
       !cn36_StrAllocCpy(MediumName, "") ||
       !cn36_StrAllocCpy(MediumType, "") ||
       !cn36_StrAllocCpy(MediumOverwrite, "") ||
       !cn36_StrAllocCpy(MediumSize, "") ||
       !cn36_StrAllocCpy(MediumBlockSize, "") ||
       !cn36_StrAllocCpy(MediumKind, "") ||
       !cn36_StrAllocCpy(MediumLocation, "") ||
       !cn36_StrNAllocCpy(EBID, NewEBID, NewEBIDLength) ||
       !cn36_StrAllocCpy(BackupType, NewType) ||
       !cn36_StrAllocCpy(BackupDateTime, NewDateTime) ||
       !cn36_StrAllocCpy(BackupToolString, "") ||
       !cn36_StrAllocCpy(DBMReturnText, "")                  )
    {
        rc=ERR_MEM_CN00;
    }
    else
    {
        DBMReturnCode=0;   //we don't really know the return code if we just asked the backup tool
        Status=NewStatus;
        PartNumber=0;
    }

    return rc;
}

tcn00_Error DBMSrvBHist_Part::Check(DBMSrvBHist_Part * OtherPart, int & Matched)
{
    tcn00_Error  rc=OK_CN00;

    Matched=0;

    if('\0'==EBID[0])
    {
        if(!OtherPart->IsNewFormat() ||                                    //old style dbm.ebf lines do match
           (0==cn36_StrStrSpaceCmp(OtherPart->MediumName, MediumName) &&   //new style line must match in all the new fields too
            0==cn36_StrStrSpaceCmp(OtherPart->MediumType, MediumType) &&
            0==cn36_StrStrSpaceCmp(OtherPart->MediumOverwrite, MediumOverwrite) &&
            0==cn36_StrStrSpaceCmp(OtherPart->MediumSize, cn36_FirstNonWhiteSpaceOf(MediumSize)) &&            //size and
            0==cn36_StrStrSpaceCmp(OtherPart->MediumBlockSize, cn36_FirstNonWhiteSpaceOf(MediumBlockSize)) &&  //blocksize are right aligned
            0==cn36_StrStrSpaceCmp(OtherPart->MediumKind, MediumKind) &&
            0==cn36_StrStrSpaceCmp(OtherPart->MediumLocation, MediumLocation)))
        {
            Matched=1;
        }

        if(Matched && !OtherPart->IsNewFormat()) //get missing information from *this, as we are going to replace *this by *OtherPart
        {
            if(!cn36_StrAllocCpy(OtherPart->MediumName, MediumName) ||
               !cn36_StrAllocCpy(OtherPart->MediumType, MediumType) ||
               !cn36_StrAllocCpy(OtherPart->MediumOverwrite, MediumOverwrite) ||
               !cn36_StrAllocCpy(OtherPart->MediumSize, MediumSize) ||
               !cn36_StrAllocCpy(OtherPart->MediumBlockSize, MediumBlockSize) ||
               !cn36_StrAllocCpy(OtherPart->MediumKind, MediumKind) ||
               !cn36_StrAllocCpy(OtherPart->MediumLocation, MediumLocation)      )
            {
                rc=ERR_MEM_CN00;
            }
        }
    }

    return rc;
}

void DBMSrvBHist_Part::SetEBIDStatus(const DBMSrvBHist_Part & OtherPart)
{
    Status=OtherPart.Status;
}

void DBMSrvBHist_Part::SetEBIDStatusToUnavailable()
{
    Status=EBIDStatusUnavailable;
}

void DBMSrvBHist_Part::SetAdded()
{
    PartAdded=1;
}

void DBMSrvBHist_Part::UnsetAdded()
{
    PartAdded=0;
}

int DBMSrvBHist_Part::IsALog() const
{
    return (0==strncmp(DBMSrvBHist_Part_LBLLOG, Label, strlen(DBMSrvBHist_Part_LBLLOG)));
}

int DBMSrvBHist_Part::IsAReplace() const
{
    return (0==strcmp(MediumKind, DBMSrvBHist_Part_MEDIUMKIND_REPLACE));
}

int DBMSrvBHist_Part::IsNewFormat() const
{
    return ('\0'!=MediumLocation[0]); //parts in new format must have a non empty location 
}

int DBMSrvBHist_Part::IsAdded() const
{
    return PartAdded;
}

int DBMSrvBHist_Part::HasSameKeyAndLabelAs(const DBMSrvBHist_Part &OtherPart) const
{
    return (0==strcmp(Key, OtherPart.Key) && 0==strcmp(Label, OtherPart.Label));
}

int DBMSrvBHist_Part::HasSameEBIDAs(const DBMSrvBHist_Part &OtherPart) const
{
    return (0==strcmp(EBID, OtherPart.EBID));
}

const char * DBMSrvBHist_Part::GiveKey() const
{
    return Key;
}

const char * DBMSrvBHist_Part::GiveLabel() const
{
    return Label;
}

DBMSrvBTools_ToolEnum DBMSrvBHist_Part::GiveUsedBackupTool() const
{
    return DBMSrvBTools_ToolEnum(BackupToolString);
}

int DBMSrvBHist_Part::GivePartNumber() const
{
    return PartNumber;
}

int DBMSrvBHist_Part::GiveDBMReturnCode() const
{
    return DBMReturnCode;
}

const char * DBMSrvBHist_Part::GiveDBMReturnText() const
{
    return DBMReturnText;
}

const char * DBMSrvBHist_Part::GiveEBID() const
{
    return EBID;
}

const char * DBMSrvBHist_Part::GiveMediumName() const
{
    return MediumName;
}

const char * DBMSrvBHist_Part::getBackupDateTime() const
{
    return BackupDateTime;
}

const char * DBMSrvBHist_Part::getBackupType() const
{
    return BackupType;
}

const char * DBMSrvBHist_Part::getMediumBlockSize() const
{
    return MediumBlockSize;
}

const char * DBMSrvBHist_Part::getMediumKind() const
{
    return MediumKind;
}

const char * DBMSrvBHist_Part::getMediumLocation() const
{
    return MediumLocation;
}

const char * DBMSrvBHist_Part::getMediumOverwrite() const
{
    return MediumOverwrite;
}

const char * DBMSrvBHist_Part::getMediumSize() const
{
    return MediumSize;
}

const char * DBMSrvBHist_Part::getMediumType() const
{
    return MediumType;
}

bool DBMSrvBHist_Part::PrintMediumInto(char                                    *& Buffer,
                                       size_t                                   & MaxUsableBufferSize,
                                       const DBMSrvBHist_PartMediaColumnWidths  & ColumnWidths)
{
    bool Full=true;

    size_t NeededSize=strlen("M:");

    NeededSize+=LengthOfPart(MediumName,       ColumnWidths.GiveWidthOf(MediumNameColumn));
    NeededSize+=LengthOfPart(MediumType,       ColumnWidths.GiveWidthOf(MediumTypeColumn));
    NeededSize+=LengthOfPart(MediumOverwrite,  ColumnWidths.GiveWidthOf(MediumOverwriteColumn));
    NeededSize+=LengthOfPart(MediumSize,       ColumnWidths.GiveWidthOf(MediumSizeColumn));
    NeededSize+=LengthOfPart(MediumBlockSize,  ColumnWidths.GiveWidthOf(MediumBlockSizeColumn));
    NeededSize+=LengthOfPart(MediumKind,       ColumnWidths.GiveWidthOf(MediumKindColumn));
    NeededSize+=LengthOfPart(MediumLocation,   ColumnWidths.GiveWidthOf(MediumLocationColumn));
    NeededSize+=LengthOfPart(BackupToolString, ColumnWidths.GiveWidthOf(BackupToolStringColumn));

    NeededSize+=strlen(LINE_SEPSTRING_CN00);

    if(NeededSize<=MaxUsableBufferSize)
    {
        Full=false;

        SAPDB_strcpy(Buffer, "M:");
        MaxUsableBufferSize-=strlen(Buffer);
        Buffer+=strlen(Buffer);

        AppendFilledUpPart(Buffer, MediumName,       ColumnWidths.GiveWidthOf(MediumNameColumn),       MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumNameColumn]);
        AppendFilledUpPart(Buffer, MediumType,       ColumnWidths.GiveWidthOf(MediumTypeColumn),       MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumTypeColumn]);
        AppendFilledUpPart(Buffer, MediumOverwrite,  ColumnWidths.GiveWidthOf(MediumOverwriteColumn),  MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumOverwriteColumn]);
        AppendFilledUpPart(Buffer, MediumSize,       ColumnWidths.GiveWidthOf(MediumSizeColumn),       MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumSizeColumn]);
        AppendFilledUpPart(Buffer, MediumBlockSize,  ColumnWidths.GiveWidthOf(MediumBlockSizeColumn),  MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumBlockSizeColumn]);
        AppendFilledUpPart(Buffer, MediumKind,       ColumnWidths.GiveWidthOf(MediumKindColumn),       MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumKindColumn]);
        AppendFilledUpPart(Buffer, MediumLocation,   ColumnWidths.GiveWidthOf(MediumLocationColumn),   MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[MediumLocationColumn]);
        AppendFilledUpPart(Buffer, BackupToolString, ColumnWidths.GiveWidthOf(BackupToolStringColumn), MaxUsableBufferSize, DBMSrvBHist_Part_ColumnAlignment[BackupToolStringColumn]);

        SAPDB_strcpy(Buffer, LINE_SEPSTRING_CN00);
        Buffer+=strlen(LINE_SEPSTRING_CN00);
        MaxUsableBufferSize-=strlen(LINE_SEPSTRING_CN00);
    }

    return Full;
}

bool DBMSrvBHist_Part::PrintEBIDInto(char *& Buffer, size_t & MaxUsableBufferSize, int WithExtendedInfo)
{
    bool Full=true;

    size_t NeededSize=0;

    // 1.) calculate the size of the EBID line
    if(!WithExtendedInfo)           // if it's a line for the backup history ...
        NeededSize+=strlen("E:");   // the line starts with "E:"
    
    switch(Status)                  // the status is starting the line or id following "E:"
    {
        default:
        case EBIDStatusUnknown:     NeededSize+=strlen(DBMSrvBHist_Part_EBIDSTATUS_UNKNOWN);
            break;
        case EBIDStatusNone:        NeededSize+=strlen(DBMSrvBHist_Part_EBIDSTATUS_NONE);
            break;
        case EBIDStatusAvailable:   NeededSize+=strlen(DBMSrvBHist_Part_EBIDSTATUS_AVAILABLE);
            break;
        case EBIDStatusUnavailable: NeededSize+=strlen(DBMSrvBHist_Part_EBIDSTATUS_UNAVAILABLE);
            break;
    }

    NeededSize+=strlen(EBID);                                           // the status is followd by the EBID
    NeededSize+=(2*DBMSrvBHist_LineProducer::ColumnSeparatorLength);    // after Status and EBID a '|' is written as seperator

    if(WithExtendedInfo)                                                // if line is printed during backup_ext_ids_list
    {
        NeededSize+=strlen(BackupType);                                 // EBID is followed by the backup type
        NeededSize+=strlen(BackupDateTime);                             // and that is followed by the backup date and time    
        NeededSize+=(2*DBMSrvBHist_LineProducer::ColumnSeparatorLength);// both are followed by the seperator '|'
    }

    NeededSize+=strlen(LINE_SEPSTRING_CN00);    // the line ends with a '\n'

    //2.) print the line into the buffer
    if(NeededSize<=MaxUsableBufferSize) // does the line fit into the buffer? (terminating '\0' was taken care of during calculation of MaxUsableBufferSize)
    {
        Full=false; //as line does fit, the buffer is not yet full

        if(WithExtendedInfo)
            Buffer[0]='\0';         //make sure the strcat's work
        else
            SAPDB_strcpy(Buffer, "E:");   // "E:"

        switch(Status)              // <status>
        {
            default:
            case EBIDStatusUnknown:     strcat(Buffer, DBMSrvBHist_Part_EBIDSTATUS_UNKNOWN);
                break;
            case EBIDStatusNone:        strcat(Buffer, DBMSrvBHist_Part_EBIDSTATUS_NONE);
                break;
            case EBIDStatusAvailable:   strcat(Buffer, DBMSrvBHist_Part_EBIDSTATUS_AVAILABLE);
                break;
            case EBIDStatusUnavailable: strcat(Buffer, DBMSrvBHist_Part_EBIDSTATUS_UNAVAILABLE);
                break;
        }

        strcat(Buffer, DBMSrvBHist_LineProducer::ColumnSeparator);  // '|'
        strcat(Buffer, EBID);                                       // <EBID>
        strcat(Buffer, DBMSrvBHist_LineProducer::ColumnSeparator);  // '|'

        if(WithExtendedInfo)
        {
            strcat(Buffer, BackupType);                                 // <backup_type>
            strcat(Buffer, DBMSrvBHist_LineProducer::ColumnSeparator);  // '|'
            strcat(Buffer, BackupDateTime);                             // <date+time>
            strcat(Buffer, DBMSrvBHist_LineProducer::ColumnSeparator);  // '|'
        }

        strcat(Buffer, LINE_SEPSTRING_CN00);                            // '\0'

        Buffer+=NeededSize;                 // set Buffer behind newly generated line
        MaxUsableBufferSize-=NeededSize;    // reduce buffer size by line length                         
    }

    return Full;    //return buffer state
}

void DBMSrvBHist_Part::EnlargeColumnWidths(DBMSrvBHist_PartMediaColumnWidths & ColumnWidths)
{
    ColumnWidths.EnlargeColumnWidthTo(MediumNameColumn     ,strlen(MediumName));
    ColumnWidths.EnlargeColumnWidthTo(MediumTypeColumn     ,strlen(MediumType));
    ColumnWidths.EnlargeColumnWidthTo(MediumOverwriteColumn,strlen(MediumOverwrite));
    ColumnWidths.EnlargeColumnWidthTo(MediumSizeColumn     ,strlen(MediumSize));
    ColumnWidths.EnlargeColumnWidthTo(MediumBlockSizeColumn,strlen(MediumBlockSize));
    ColumnWidths.EnlargeColumnWidthTo(MediumKindColumn     ,strlen(MediumKind));
    ColumnWidths.EnlargeColumnWidthTo(MediumLocationColumn ,strlen(MediumLocation));
}

int DBMSrvBHist_Part::MatchesFileName(const char * AbsoluteName, const char * DefaultDirectory)
{
    int    rc=0;
    size_t FileNameLengthWithoutVersion=0;

    const char * PartToCheck=0;

    if(cn36_IsAbsolutePath(MediumLocation))    //found an absolute path in the history
        PartToCheck=AbsoluteName;              //check it against the complete AbsoluteName
    else                                       //path found in history was relative -> DefaultDirectory+'/'|'\'+MediumLocation must match
        if(0==cn36_PathNCmp(DefaultDirectory, AbsoluteName, strlen(DefaultDirectory))) //AbsoluteName starts with the run directory
        {
            PartToCheck=AbsoluteName+strlen(DefaultDirectory);

            if(cn36_IsPathDelimiter(PartToCheck))                 //and not more than the rundirectory
                PartToCheck=cn36_NextPathPart(PartToCheck);       //jump over the path delimiter behind the rundirectory
            else
                PartToCheck=0;
        }

    if(0!=PartToCheck) //if we have something to compare 
    {
        FileNameLengthWithoutVersion=strlen(PartToCheck);

        if(0==cn36_PathNCmp(MediumLocation, PartToCheck, FileNameLengthWithoutVersion)) //both file names are equal (without the version part)
            rc=1;
    }

    if(rc) //file name does match, check if MediumLocation ends with a version number
    {
        const char * StartOfVersion=MediumLocation+FileNameLengthWithoutVersion;
        size_t       LengthOfVersion=cn36_FirstWhiteSpaceOf(StartOfVersion)-StartOfVersion; //needed as MediumLocation maybe filled up with spaces

        if(2>LengthOfVersion ||                                                //there must be place for a '.' and at least one number
           '.'!=(*StartOfVersion) ||                                           //first character must be a dot
           !cn36_StrNConsistsOfDigits(StartOfVersion+1, LengthOfVersion-1) ||  //and it must be followed by a number (all together: ".<number>" -> a version file)
           '\0'!= *cn36_FirstNonWhiteSpaceOf(StartOfVersion+LengthOfVersion))  //and only spaces may follow the number
        {
            rc=0;
        }
    }

    return rc;
}

int DBMSrvBHist_Part::ContainsFileBackup(const char * BaseName)
{
    int rc=0;

    const char * BaseNameInHistory=cn36_BaseNameFrom(MediumLocation);

    if(0==cn36_PathNCmp(BaseName, BaseNameInHistory, strlen(BaseName)) &&    //if the file in question was found in the stage directory...
       '\0'== *cn36_FirstNonWhiteSpaceOf(BaseNameInHistory+strlen(BaseName)))//check that there are only spaces behind the version
    {
        rc=1;
    }

    return rc;
}

int DBMSrvBHist_Part::IsAvailable() const
{
    return (EBIDStatusNone==Status || EBIDStatusAvailable==Status);
}

int DBMSrvBHist_Part::IsAvailableExtended() const
{
    return ExtendedAvailableFlag;
}

tcn00_Error DBMSrvBHist_Part::IsAvailableExtended(const char * DefaultDirectory, int & Available)
{
    tcn00_Error rc=OK_CN00;
        
    Available=IsAvailable();

    if(0==cn36_StrStrSpaceCmp(DBMSrvBHist_Part_MEDIUMTYPE_HIST_FILE, MediumType)) //is it a FILE?
    {
        char * PathCopy=0;

        if(!cn36_IsAbsolutePath(MediumLocation))
        {
            if(!cn36_ConstructCompletePath(PathCopy, DefaultDirectory, MediumLocation))
                rc=ERR_MEM_CN00;
        }
        else
        {
            if(!cn36_StrAllocCpy(PathCopy, MediumLocation))
                rc=ERR_MEM_CN00;
        }

        *(cn36_StartOfTrailingWhiteSpaces(PathCopy))='\0'; //ignore trailing spaces

        if(OK_CN00==rc)
        {
            tni34_AVirtualFile LogFile(PathCopy, tni34_AVirtualFile::FromDisk, tni34_AVirtualFile::DoNotOpenNow);

            if(!LogFile.IsGood())
                rc=ERR_MEM_CN00;
            
            if(!LogFile.Exists())
                Available=0;
        }

        cn36_StrDealloc(PathCopy);
    }

    ExtendedAvailableFlag=Available;

    return rc;
}

DBMSrvBHist_Part::PartOrigin DBMSrvBHist_Part::GivePartOrigin() const
{
    return Origin;
}


//-----------------------------------------------------------------------------
// member functions of class DBMSrvBHist_PartMediaColumnWidths
//-----------------------------------------------------------------------------

DBMSrvBHist_PartMediaColumnWidths::DBMSrvBHist_PartMediaColumnWidths()
{
    SetColumnWidthsToDefaults();
}

void DBMSrvBHist_PartMediaColumnWidths::SetColumnWidthsToZero()
{
    for(int i=0; i<DBMSrvBHist_Part::NoColumn; i++)
        ColumnWidth[i]=0;
}

void DBMSrvBHist_PartMediaColumnWidths::SetColumnWidthsToDefaults()
{
    for(int i=0; i<DBMSrvBHist_Part::NoColumn; i++)
        ColumnWidth[i]=DBMSrvBHist_Part_ColumnMinLength[i];
}

void DBMSrvBHist_PartMediaColumnWidths::EnlargeColumnWidthTo(DBMSrvBHist_Part::MediaColumn Column, size_t NewWidth)
{
    if(ColumnWidth[Column]<NewWidth)
        ColumnWidth[Column]=NewWidth;
}

size_t DBMSrvBHist_PartMediaColumnWidths::GiveWidthOf(DBMSrvBHist_Part::MediaColumn Column) const
{
    return ColumnWidth[Column];
}
