/*!
    \file    DBMSrvBHist_Backup.cpp
    \author  TiloH
    \ingroup backup history handling by the DBMServer
    \brief   implementing a class for managing the information of a backpup

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2006 SAP AG

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

#include "DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"
#include "hcn36.h"
#include "hcn90.h"
#include "gcn003.h"

size_t DBMSrvBHist_Backup_ColumnMinLength[DBMSrvBHist_Backup::NoColumn]
    ={12, //Key
       9, //Label
       9, //Action
      19, //DBStamp1
      19, //DBStamp2
      19, //StartDate
      19, //StopDate
      10, //FirstLogPage
      10, //LastLogPage
       3, //LogRequired
      64, //MediumName
      10, //PagesTransfered
      10, //Volumes
      10, //ReturnCode
      40};//ReturnText

DBMSrvBHist_LineProducer::Alignment DBMSrvBHist_Backup_ColumnAlignment[DBMSrvBHist_Backup::NoColumn]
    ={DBMSrvBHist_LineProducer::Left,  //Key
      DBMSrvBHist_LineProducer::Left,  //Label
      DBMSrvBHist_LineProducer::Left,  //Action
      DBMSrvBHist_LineProducer::Left,  //DBStamp1
      DBMSrvBHist_LineProducer::Left,  //DBStamp2
      DBMSrvBHist_LineProducer::Left,  //StartDate
      DBMSrvBHist_LineProducer::Left,  //StopDate
      DBMSrvBHist_LineProducer::Right, //FirstLogPage
      DBMSrvBHist_LineProducer::Right, //LastLogPage
      DBMSrvBHist_LineProducer::Left,  //LogRequired
      DBMSrvBHist_LineProducer::Left,  //MediumName
      DBMSrvBHist_LineProducer::Right, //PagesTransfered
      DBMSrvBHist_LineProducer::Right, //Volumes
      DBMSrvBHist_LineProducer::Right, //ReturnCode
      DBMSrvBHist_LineProducer::Left}; //ReturnText

const char * DBMSrvBHist_Backup::DBMSrvBHist_Backup_LBLDAT="DAT";
const char * DBMSrvBHist_Backup_LBLLOG="LOG";
const char * DBMSrvBHist_Backup_LBLPAG="PAG";

const char * DBMSrvBHist_Backup::DBMSrvBHist_Backup_ACTSAV="SAVE";
const char * DBMSrvBHist_Backup_ACTRES="RESTORE";
const char * DBMSrvBHist_Backup_ACTHIS="HISTLOST";

const char * ColumnKeyWords[DBMSrvBHist_Backup::NoColumn]={"KEY",
                                                           "LABEL",
                                                           "ACTION",
                                                           "STAMP1",
                                                           "STAMP2",
                                                           "START",
                                                           "STOP",
                                                           "FIRSTLOG",
                                                           "LASTLOG",
                                                           "LOG",
                                                           "MEDIA",
                                                           "PAGES",
                                                           "VOLUMES",
                                                           "RC",
                                                           "ERROR"};

const tcn36_DateTimeFormat DBMSrvBHist_Backup_HistoryDateTimeFormat("yyyy-mm-dd HH:MM:SS");

DBMSrvBHist_Backup::Column DBMSrvBHist_Backup::ColumnFor(const char * KeyWord, size_t KeyWordLength)
{
    Column rc=NoColumn;

    for(int i=KeyColumn; NoColumn==rc && i<NoColumn; i++)
    {
        if(0==strncmp(KeyWord, ColumnKeyWords[i], KeyWordLength) &&
           strlen(ColumnKeyWords[i])==KeyWordLength)
        {
            rc=Column(i);
        }
    }

    return rc;
}

DBMSrvBHist_Backup::DBMSrvBHist_Backup()
{
    for(int i=0; i<NoColumn; i++)
        ColumnArray[i]=0;

    StartLogPage      =0;
    StopLogPage       =0;
    ReturnCode        =0;
    ReturnCodeDBKernel=0;
    TimeStamp         =0;

    HasCopyFromMediaFile=0;
};

DBMSrvBHist_Backup::~DBMSrvBHist_Backup()
{
    for(Tools_List<DBMSrvBHist_Copy *>::iterator i=Copies.begin(); i!=Copies.end(); i++)
        delete (*i);

    for(int j=0; j<NoColumn; j++)
        cn36_StrDealloc(ColumnArray[j]);

    cn36_StrDealloc(TimeStamp);
}

bool DBMSrvBHist_Backup::SetTo(
    const char      * HistoryFileLine,
    DBMSrvMsg_Error & error)
{
    bool         rc=true;
    const char * CurPos=HistoryFileLine;

    for(int i=0; rc && i<NoColumn; i++)
    {
        const char * Separator=strchr(CurPos, '|'); //all columns of dbm.knl must end with a '|'

        if(0==Separator) 
        {
            char * shortForm=0;

            if(0<strlen(CurPos) && ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, CurPos, error))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_SEPARATORMISSING,
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__STRING, shortForm),
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORMISSING__SEPARATOR, "|")));
            }

            if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, HistoryFileLine, error))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FORMAT_SEPARATORNOP,
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNOP__SEPARATOR, "|"),
                                            Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNOP__NUMBER, NoColumn),
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNOP__STRING, shortForm),
	                                        Msg_Arg(SDBMSGTAG_DBMSRV_FORMAT_SEPARATORNOP__NUMBER_FOUND, i)));

                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_INVALID_ENTRY,
                                            Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_INVALID_ENTRY__LINE2, shortForm)));
            }

            cn36_StrDealloc(shortForm);

            rc=false;
        }
        else
        {
            if(!ToolsCStr_Str::NAllocCpy(ColumnArray[i], CurPos, Separator-CurPos, error))
                rc=false;

            CurPos=Separator+1;
        }
    }

    if(rc)
    {
        if(!ToolsCStr_Str::ToUInt8(StartLogPage, ColumnArray[StartLogPageColumn], error) ||    //start log page ...
           !ToolsCStr_Str::ToUInt8(StopLogPage, ColumnArray[StopLogPageColumn], error) ||      //stop log page ...
           !ToolsCStr_Str::ToInt(ReturnCode, ColumnArray[ReturnCodeColumn], error)        )    //rc must all be integer values
        {
            char * shortForm=0;

            if(ToolsCStr_Str::AllocCpyWithoutLeadingTrailingSpaces(shortForm, HistoryFileLine, error))
            {
                error.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_BHIST_INVALID_ENTRY,
                                               Msg_Arg(SDBMSGTAG_DBMSRV_BHIST_INVALID_ENTRY__LINE2, shortForm)));
            }

            cn36_StrDealloc(shortForm);

            rc=false;
        }
        else
            ReturnCodeDBKernel=ReturnCode;

        if(rc && IsAHistLost() && !ToolsCStr_Str::AllocCpy(ColumnArray[DBStamp1Column], ColumnArray[StartDateColumn], error))
            rc=false;

        if(rc && !ToolsCStr_Str::Alloc(TimeStamp, strlen(ColumnArray[DBStamp1Column]), error))
            rc=false;

        if(rc)
        {
            char * Dest=TimeStamp, *Source=ColumnArray[DBStamp1Column];

            //copy all digits (and only digits) from DBStamp1 to TimeStamp
            for(; '\0'!=(*Source); ++Source)
            {
                if(isdigit(*Source))
                    (*Dest++)=*Source;
            }
            
            *Dest='\0';
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_Backup::AddPart(DBMSrvBHist_Part * Part)
{
    tcn00_Error rc=OK_CN00;

    if(Copies.empty() ||                                                                        //not even one copy with one part so far
       (DBMSrvBHist_Part::PartFromMediaFile==Part->GivePartOrigin() && !HasCopyFromMediaFile))  //current medium part is from dbm.mdf but all copys are from dbm.ebp
    {
        DBMSrvBHist_Copy *c=new DBMSrvBHist_Copy;

        if(0==c || !Copies.push_back(c))
            rc=ERR_MEM_CN00;

        if(OK_CN00==rc)
            rc=Copies.front()->AddPart(Part);

        if(DBMSrvBHist_Part::PartFromMediaFile==Part->GivePartOrigin())
            HasCopyFromMediaFile=1;
    }
    else
        if(DBMSrvBHist_Part::PartFromMediaFile==Part->GivePartOrigin())
            rc=Copies.front()->AddPart(Part);               //add all parts from dbm.mdf to the first copy
        else                                                //DBMSrvBHist_Backup::PartFromEBIDFile==Origin
        {
            int                                      Matched=0;
            Tools_List<DBMSrvBHist_Copy *>::iterator Copy=Copies.begin();

            //check against all parts from dbm.mdf
            if(HasCopyFromMediaFile)
            {
                rc=(*Copy)->CheckPart(Part, Matched);  //check if part contains the EBID of a line from dbm.mdf
                ++Copy;                                //point to the first copy not from dbm.mdf
            }

            if(Part->IsNewFormat())
            {
                //is it a part of the last copy from dbm.ebf?
                if(OK_CN00==rc && !Matched && Copy!=Copies.end())
                    rc=(*Copy)->AddMatchingPart(Part, Matched);

                //if nothing of the above, part must be the first part of a new copy
                if(OK_CN00==rc && !Matched)
                {
                    DBMSrvBHist_Copy *c=new DBMSrvBHist_Copy;

                    if(0==c || !Copies.insert(Copy, c))
                        rc=ERR_MEM_CN00;

                    if(OK_CN00==rc)
                        rc=c->AddPart(Part);
                }
            }
        }

    return rc;
}

int DBMSrvBHist_Backup::IsAHistLost() const
{
    return (0==strncmp(DBMSrvBHist_Backup_ACTHIS, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTHIS)));
}

int DBMSrvBHist_Backup::IsABackup() const
{
    return (0==strncmp(DBMSrvBHist_Backup_ACTSAV, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTSAV)));
}

int DBMSrvBHist_Backup::IsARestore() const
{
    return (0==strncmp(DBMSrvBHist_Backup_ACTRES, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTRES)));
}

int DBMSrvBHist_Backup::IsAData() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLDAT, ColumnArray[LabelColumn],  strlen(DBMSrvBHist_Backup_LBLDAT)));
}

int DBMSrvBHist_Backup::IsADataBackup() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLDAT, ColumnArray[LabelColumn], strlen(DBMSrvBHist_Backup_LBLDAT)) &&
            0==strncmp(DBMSrvBHist_Backup_ACTSAV, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTSAV))  );
}

int DBMSrvBHist_Backup::IsADataRestore() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLDAT, ColumnArray[LabelColumn], strlen(DBMSrvBHist_Backup_LBLDAT)) &&
            0==strncmp(DBMSrvBHist_Backup_ACTRES, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTRES))  );
}

int DBMSrvBHist_Backup::IsALog() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLLOG, ColumnArray[LabelColumn], strlen(DBMSrvBHist_Backup_LBLLOG)));
}

int DBMSrvBHist_Backup::IsALogBackup() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLLOG, ColumnArray[LabelColumn], strlen(DBMSrvBHist_Backup_LBLLOG)) &&
            0==strncmp(DBMSrvBHist_Backup_ACTSAV, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTSAV))  );
}

int DBMSrvBHist_Backup::IsAPages() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLPAG, ColumnArray[LabelColumn],  strlen(DBMSrvBHist_Backup_LBLPAG)));  
}

int DBMSrvBHist_Backup::IsAPagesBackup() const
{
    return (IsAPages() && IsABackup());  
}

int DBMSrvBHist_Backup::IsAPagesRestore() const
{
    return (0==strncmp(DBMSrvBHist_Backup_LBLPAG, ColumnArray[LabelColumn], strlen(DBMSrvBHist_Backup_LBLPAG)) &&
            0==strncmp(DBMSrvBHist_Backup_ACTRES, ColumnArray[ActionColumn], strlen(DBMSrvBHist_Backup_ACTRES))  );
}

tcn00_Error DBMSrvBHist_Backup::IsAvailable(const char * DefaultDirectory, int & Available) const
{
    tcn00_Error rc=OK_CN00;

    Available=0;

    for(Tools_List<DBMSrvBHist_Copy *>::const_iterator h=Copies.begin(); OK_CN00==rc && !Available && h!=Copies.end(); ++h)
    {
        rc=(*h)->IsAvailableExtended(DefaultDirectory, Available);
    }

    return rc;
}

int DBMSrvBHist_Backup::WasSuccessful() const
{
    return (0==ReturnCode);
}

int DBMSrvBHist_Backup::WasSuccessfulForDB() const
{
    return (0==ReturnCodeDBKernel);
}

int DBMSrvBHist_Backup::EqualsInLabelWith(DBMSrvBHist_Backup *o) const
{
    return (0==strcmp(ColumnArray[LabelColumn], o->ColumnArray[LabelColumn]));
}

int DBMSrvBHist_Backup::EqualsInDBStamp1With(DBMSrvBHist_Backup *o) const
{
    return (0==strcmp(ColumnArray[DBStamp1Column], o->ColumnArray[DBStamp1Column]));
}

int DBMSrvBHist_Backup::EqualsInDBStamp2With(DBMSrvBHist_Backup *o) const
{
    return (0==strcmp(ColumnArray[DBStamp2Column], o->ColumnArray[DBStamp2Column]));
}

int DBMSrvBHist_Backup::HasKey(const char * KeyToCompare) const
{
    return (0==strcmp(ColumnArray[KeyColumn], KeyToCompare));
}

int DBMSrvBHist_Backup::HasSameKeyAs(const DBMSrvBHist_Backup &OtherBackup) const
{
    return HasKey(OtherBackup.ColumnArray[KeyColumn]);
}

int DBMSrvBHist_Backup::HasKeyAndLabel(const char * KeyToCompare, const char * LabelToCompare) const
{
    return (0==strcmp(ColumnArray[KeyColumn], KeyToCompare) && 0==strcmp(ColumnArray[LabelColumn], LabelToCompare));
}

int DBMSrvBHist_Backup::HasInternalBackupID(const char * IBID) const
{
    int rc=0;

    size_t LengthIBID=strlen(IBID);
    size_t LengthKey=strlen(ColumnArray[KeyColumn]);
    size_t LengthLabel=strlen(ColumnArray[LabelColumn]);

    if(LengthIBID==(LengthKey+1+LengthLabel) &&
       0==strncmp(IBID, ColumnArray[KeyColumn], LengthKey) &&
       0==strcmp(IBID+LengthKey+1, ColumnArray[LabelColumn])  )
    {
        rc=1;
    }

    return rc;
}

int DBMSrvBHist_Backup::HasTimeStamp(const char * TimeStampToCompare) const
{
    return (0==strcmp(TimeStamp, TimeStampToCompare));
}

int DBMSrvBHist_Backup::MatchesLabel(const char * StartOfLabelToCompare) const
{
    return (0==strncmp(StartOfLabelToCompare, ColumnArray[LabelColumn], strlen(StartOfLabelToCompare)));
}

int DBMSrvBHist_Backup::MatchesAction(const char * StartOfActionToCompare) const
{
    return (0==strncmp(StartOfActionToCompare, ColumnArray[ActionColumn], strlen(StartOfActionToCompare))); 
}

int DBMSrvBHist_Backup::HasSameLabelAndTimeStampsAs(const DBMSrvBHist_Backup & OtherBackup) const
{
    return (0==strcmp(ColumnArray[LabelColumn], OtherBackup.ColumnArray[LabelColumn]) &&
            0==strcmp(ColumnArray[DBStamp1Column], OtherBackup.ColumnArray[DBStamp1Column]) && 
            0==strcmp(ColumnArray[DBStamp2Column], OtherBackup.ColumnArray[DBStamp2Column])    );            
}

int DBMSrvBHist_Backup::HasHigherLogPageAs(const DBMSrvBHist_Backup & OtherBackup) const
{
    return (GiveMaxLogPage()>OtherBackup.GiveMaxLogPage());
}

int DBMSrvBHist_Backup::HasContentOlderThan(const char * Date, const char *Time) const
{
    int rc=0;

    const char *source;
    char ThisDate[20];
    char ThisTime[20];

    SAPDB_strcpy(ThisDate, "00000101");
    SAPDB_strcpy(ThisTime, "000000");

    if(IsAData() || IsAPages())   
        source=ColumnArray[DBStamp1Column];
    else
        if(IsALog())
        {
            if(0==strcmp("0000-00-00 00:00:00", ColumnArray[DBStamp1Column]))
                source="9999-12-31 23:59:59";
            else
                source=ColumnArray[DBStamp1Column];
        }

    int dest=0;

    //split DBStamp1 into date and time and strip it off any nondigits
    while(dest<19 && '\0'!=(*source) && !isspace(*source))
    { 
        if(isdigit(*source))
            ThisDate[dest++]=(*source);

        source++;
    }

    source=cn36_FirstNonWhiteSpaceOf(source);

    dest=0;
    while(dest<19 && '\0'!=(*source) && !isspace(*source))
    {
        if(isdigit(*source))
            ThisTime[dest++]=(*source);

         source++;
    }

    if(strcmp(ThisDate, Date)<0)   //ThisDate is older than Date
        rc=1;
    else
        if(0==strcmp(ThisDate, Date) && strcmp(ThisTime, Time)<0)   //dates are equal but ThisTime is older than Time
            rc=1;

    return rc;
}

const char * DBMSrvBHist_Backup::GiveKey() const
{
    return ColumnArray[KeyColumn];
}

const char * DBMSrvBHist_Backup::GiveLabel() const
{
    return ColumnArray[LabelColumn];
}

const char * DBMSrvBHist_Backup::GiveBackupType() const
{
    if(IsAData())
        return BA_KEY_DATA_CN003;
    else
        if(IsAPages())
            return BA_KEY_PAGES_CN003;
        else
            if(IsALog())
                return BA_KEY_LOG_CN003;
            else
                return "";
}

const char * DBMSrvBHist_Backup::GiveTimeStamp() const
{
    return TimeStamp;
}
const char * DBMSrvBHist_Backup::GiveStartDate() const
{
    return ColumnArray[StartDateColumn];
}

const char * DBMSrvBHist_Backup::GiveDBStamp1() const
{
    return ColumnArray[DBStamp1Column];
}

const char * DBMSrvBHist_Backup::GiveMediumName() const
{
    return ColumnArray[MediumNameColumn];
}

const char * DBMSrvBHist_Backup::GiveVolumes() const
{
    return ColumnArray[VolumesColumn];
}

teo00_Uint8 DBMSrvBHist_Backup::GiveStartLogPage() const
{
    return StartLogPage;
}

const char * DBMSrvBHist_Backup::GiveStartLogPageAsString() const
{
    return cn36_FirstNonWhiteSpaceOf(ColumnArray[StartLogPageColumn]);
}

teo00_Uint8 DBMSrvBHist_Backup::GiveStopLogPage() const
{
    return StopLogPage;
}

const char * DBMSrvBHist_Backup::GivePagesTransfered() const
{
    return ColumnArray[PagesTransferedColumn];
}

teo00_Uint8 DBMSrvBHist_Backup::GiveMaxLogPage() const
{
    if(IsALog())
        return GiveStopLogPage();
    else
        return GiveStartLogPage();
}

char * DBMSrvBHist_Backup::ConstructEBIDOfFirstAvailableCopy() const
{
    char * rc=0;
    int    PassedFirstAvailable=0;

    for(Tools_List<DBMSrvBHist_Copy *>::const_iterator h=Copies.begin(); !PassedFirstAvailable && h!=Copies.end(); ++h)
    {
        if((*h)->IsAvailableExtended())
        {
            PassedFirstAvailable=1;
            rc=(*h)->ConstructEBIDString();
        }
    }

    return rc;
}

char * DBMSrvBHist_Backup::constructInternalBackupID(DBMSrvMsg_Error & err) const
{
    char * rc=0;

    if(ToolsCStr_Str::Alloc(rc, strlen(GiveKey())+1+strlen(GiveLabel()), err))
    {
        SAPDB_strcpy(rc, GiveKey());
        SAPDB_strcat(rc, "|");
        SAPDB_strcat(rc, GiveLabel());
    }

    return rc;
}

tcn00_Error DBMSrvBHist_Backup::ConstructMediumOfFirstAvailableCopy(tcn003_Medium *& Medium, tcn003_MediaGroup *& MediaGroup) const
{
    tcn00_Error rc=OK_CN00;
    int         PassedFirstAvailable=0;

    for(Tools_List<DBMSrvBHist_Copy *>::const_iterator h=Copies.begin(); !PassedFirstAvailable && h!=Copies.end(); ++h)
    {
        if((*h)->IsAvailableExtended())
        {
            PassedFirstAvailable=1;
            rc=(*h)->ConstructMedium(Medium, MediaGroup, GiveBackupType());
        }
    }

    if(!PassedFirstAvailable)
        rc=ERR_CN00; //should not happen

    return rc;
}

bool DBMSrvBHist_Backup::PrintBackupInto(char                                *& Buffer,
                                         size_t                               & MaxUsableBufferSize,
                                         Tools_List<Column>                   & Columns,
                                         const DBMSrvBHist_BackupColumnWidths & ColumnWidths)
{
    bool Full=true;

    size_t PlaceNeeded=0;
    Tools_List<Column>::iterator h;

    //count the needed place
    for(h=Columns.begin(); h!=Columns.end(); ++h)
        PlaceNeeded+=LengthOfPart(ColumnArray[(*h)], ColumnWidths.GiveWidthOf(*h));

    PlaceNeeded+=strlen(LINE_SEPSTRING_CN00);

    //print wished strings into Buffer if there is enough place for all
    if(PlaceNeeded<=MaxUsableBufferSize)
    {
        Full=false; //it does fit into buffer, buffer is not full

        for(h=Columns.begin(); h!=Columns.end(); ++h)
            AppendFilledUpPart(Buffer, ColumnArray[(*h)], ColumnWidths.GiveWidthOf(*h), MaxUsableBufferSize, DBMSrvBHist_Backup_ColumnAlignment[(*h)]);

        SAPDB_strcpy(Buffer, LINE_SEPSTRING_CN00);
        Buffer+=strlen(LINE_SEPSTRING_CN00);
        MaxUsableBufferSize-=strlen(LINE_SEPSTRING_CN00);
    }

    return Full;
}

bool DBMSrvBHist_Backup::PrintMediaAndEBIDsInto(char                                   *& Buffer,
                                                size_t                                  & MaxUsableBufferSize,
                                                bool                                      WithMedia,
                                                bool                                      WithEBIDs,
                                                const DBMSrvBHist_PartMediaColumnWidths & ColumnWidthsMedia)
{
    bool rc=false; //if this function is called, the buffer is not empty

    //try to write media and EBID lines of all copies into Buffer
    for(Tools_List<DBMSrvBHist_Copy *>::iterator h=Copies.begin(); !rc && h!=Copies.end(); ++h)
        rc=(*h)->PrintMediaAndEBIDsInto(Buffer, MaxUsableBufferSize, WithMedia, WithEBIDs, ColumnWidthsMedia);

    return rc;
}

void DBMSrvBHist_Backup::EnlargeColumnWidths(Tools_List<Column>                & Columns,
                                             DBMSrvBHist_BackupColumnWidths    & ColumnWidthsBackup,
                                             DBMSrvBHist_PartMediaColumnWidths & ColumnWidthsMedia)
{
    for(Tools_List<Column>::iterator h=Columns.begin(); h!=Columns.end(); ++h) //for all active columns
        ColumnWidthsBackup.EnlargeColumnWidthTo((*h), strlen(ColumnArray[(*h)]));

    for(Tools_List<DBMSrvBHist_Copy *>::iterator Copy=Copies.begin(); Copy!=Copies.end(); ++Copy)
        (*Copy)->EnlargeColumnWidths(ColumnWidthsMedia);
}

void DBMSrvBHist_Backup::MarkUsedExternalBackupTools(DBMSrvBTools_ToolEnum * BackupToolType)
{
    for(Tools_List<DBMSrvBHist_Copy *>::iterator Copy=Copies.begin(); Copy!=Copies.end(); ++Copy)
        (*Copy)->MarkUsedExternalBackupTools(BackupToolType);
}

int DBMSrvBHist_Backup::MatchesFileNameElseReduce(const char * AbsoluteName, const char * DefaultDirectory)
{
    int rc=0;

    Tools_List<DBMSrvBHist_Copy *>::iterator h=Copies.begin();

    while(h!=Copies.end())
    {
        if((*h)->MatchesFileNameElseReduce(AbsoluteName, DefaultDirectory))
        {
            rc=1;
            ++h;
        }
        else
        {
            delete (*h);
            h=Copies.erase(h); //erase every copy that does not match at all
        }
    }

    return rc;
}

int DBMSrvBHist_Backup::ContainsFileBackup(const char                 * BaseName,
                                           const char                 * labelFromBackup,
                                           const tcn36_DateTimeString & backupStartDateTime)
{
    int rc=0;

    if(0==cn36_StrUprCmp(labelFromBackup, GiveLabel()))                       //label must match
    {
        tcn36_DateTimeString Start(ColumnArray[StartDateColumn], DBMSrvBHist_Backup_HistoryDateTimeFormat);
        tcn36_DateTimeString Stop(ColumnArray[StopDateColumn], DBMSrvBHist_Backup_HistoryDateTimeFormat);

        if(Start<=backupStartDateTime && backupStartDateTime<=Stop) //backup must have been started between Start and Stop
        {
            for(Tools_List<DBMSrvBHist_Copy *>::iterator h=Copies.begin(); !rc && h!=Copies.end(); ++h)
                rc=(*h)->ContainsFileBackup(BaseName);
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_Backup::AdjustReturnCode()
{
    int rc=OK_CN00;

    if(0==ReturnCode && Copies.begin()!=Copies.end()) //kernel said it was OK, and we have information from dbm.mdf or dbm.ebf
    {
        int FoundSuccessfulCopy=0;

        for(Tools_List<DBMSrvBHist_Copy *>::iterator h=Copies.begin(); !FoundSuccessfulCopy && h!=Copies.end(); ++h)
        {
            if(0==(*h)->DBMReturnCode())
                FoundSuccessfulCopy=1;
        }

        if(!FoundSuccessfulCopy)
        {
            char DBMReturnCodeAsString[50];

            ReturnCode=(*Copies.begin())->DBMReturnCode();

            sprintf(DBMReturnCodeAsString, "%d", int(ReturnCode));

            if(!cn36_StrAllocCpy(ColumnArray[ReturnCodeColumn], DBMReturnCodeAsString) ||
               !cn36_StrAllocCpy(ColumnArray[ReturnTextColumn], (*Copies.begin())->DBMReturnText()))
            {
                rc=ERR_MEM_CN00;
            }
        }
    } //else ... nothing from dbm.mdf or dbm.ebf -> just leave the ReturnCode untouched

    return rc;
}

//-----------------------------------------------------------------------------
// member functions of class DBMSrvBHist_Backup
//-----------------------------------------------------------------------------

DBMSrvBHist_BackupColumnWidths::DBMSrvBHist_BackupColumnWidths()
{
    SetColumnWidthsToDefaults();
}

void DBMSrvBHist_BackupColumnWidths::SetColumnWidthsToZero()
{
    for(int i=0; i<DBMSrvBHist_Backup::NoColumn; i++)
        ColumnWidth[i]=0;
}

void DBMSrvBHist_BackupColumnWidths::SetColumnWidthsToDefaults()
{
    for(int i=0; i<DBMSrvBHist_Backup::NoColumn; i++)
        ColumnWidth[i]=DBMSrvBHist_Backup_ColumnMinLength[i];
}

void DBMSrvBHist_BackupColumnWidths::EnlargeColumnWidthTo(DBMSrvBHist_Backup::Column Column, size_t NewWidth)
{
    if(ColumnWidth[Column]<NewWidth)
        ColumnWidth[Column]=NewWidth;
}

size_t DBMSrvBHist_BackupColumnWidths::GiveWidthOf(DBMSrvBHist_Backup::Column Column) const
{
    return ColumnWidth[Column];
}
