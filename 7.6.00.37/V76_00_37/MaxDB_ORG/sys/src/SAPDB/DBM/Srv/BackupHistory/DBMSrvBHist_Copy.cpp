/*!
  \file    DBMSrvBHist_Copy.cpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   implementing a class for managing the information of one copy
           of a backup

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

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Copy.hpp"
#include "hcn36.h"


//-----------------------------------------------------------------------------
// member of class DBMSrvBHist_Copy
//-----------------------------------------------------------------------------

DBMSrvBHist_Copy::DBMSrvBHist_Copy()
{
    InternalCopyID=0;
};

DBMSrvBHist_Copy::~DBMSrvBHist_Copy()
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator i=Parts.begin(); i!=Parts.end(); i++)
        delete (*i);

    cn36_StrDealloc(InternalCopyID);
};

tcn00_Error DBMSrvBHist_Copy::AddPart(DBMSrvBHist_Part * Part)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Part *>::iterator p=Parts.begin();

    if(Part->IsAReplace())                              //if part is product of a replace during a backup
        while(p!=Parts.end() && !(*p)->IsAReplace())    //it must be added after "normal" parts
            ++p;

    if(!Parts.insert(p, Part))
        rc=ERR_MEM_CN00;
    else
        Part->SetAdded();

    return rc;
}

tcn00_Error DBMSrvBHist_Copy::AddMatchingPart(DBMSrvBHist_Part * Part, int & Matched)
{
    tcn00_Error rc=OK_CN00;

    if(Part->GivePartNumber()<Parts.front()->GivePartNumber()) //we are adding parts in reversed order, so PartNumber must decrease
    {
        Matched=1;
        rc=AddPart(Part);
    }
    else
        Matched=0;

    return rc;
}

tcn00_Error DBMSrvBHist_Copy::CheckPart(DBMSrvBHist_Part * Part, int & Matched)
{
    tcn00_Error rc=OK_CN00;

    Matched=0;

    for(Tools_List<DBMSrvBHist_Part *>::reverse_iterator p=Parts.rbegin(); OK_CN00==rc && !Matched && p!=Parts.rend(); ++p)
    {
        rc=(*p)->Check(Part, Matched);

        if(OK_CN00==rc && Matched)
        {
            if(Parts.push_front(Part)) //substitute p with Part, but add parts from dbm.ebf in the sequence of dbm.ebf
            {
                if(DBMSrvBHist_Part::PartFromMediaFile==(*p)->GivePartOrigin()) //is *p from dbm.mdf?
                    delete (*p);                                                //last chance to forget part (*p)
                else
                    (*p)->UnsetAdded();

                Parts.erase(p);
                Part->SetAdded();
            }
            else
                rc=ERR_MEM_CN00;
        }
    }

    return rc;
}

bool DBMSrvBHist_Copy::PrintMediaAndEBIDsInto(char                                    *& Buffer,
                                              size_t                                   & MaxUsableBufferSize,
                                              bool                                       WithMedia,
                                              bool                                       WithEBIDs,
                                              const DBMSrvBHist_PartMediaColumnWidths  & ColumnWidths)
{
    bool                                     Full=false; //Buffer is not full
    Tools_List<DBMSrvBHist_Part *>::iterator Part;

    //first print all media lines of this copy of the backup
    if(WithMedia)
    {
        for(Part=Parts.begin(); !Full && Part!=Parts.end(); ++Part)
            Full=(*Part)->PrintMediumInto(Buffer, MaxUsableBufferSize, ColumnWidths);
    }

    //second print all EBID lines of this copy of the backup
    if(WithEBIDs)
    {
        for(Part=Parts.begin(); !Full && Part!=Parts.end(); ++Part)
            Full=(*Part)->PrintEBIDInto(Buffer, MaxUsableBufferSize, 0);  //without extra information about type and date
    }

    return Full;
}

void DBMSrvBHist_Copy::EnlargeColumnWidths(DBMSrvBHist_PartMediaColumnWidths & ColumnWidths)
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator Part=Parts.begin(); Part!=Parts.end(); ++Part)
        (*Part)->EnlargeColumnWidths(ColumnWidths);
}

int DBMSrvBHist_Copy::MatchesFileNameElseReduce(const char * AbsoluteName, const char * DefaultDirectory)
{
    int rc=0;

    Tools_List<DBMSrvBHist_Part *>::iterator Part=Parts.begin();

    while(Part!=Parts.end())
    {
        if((*Part)->MatchesFileName(AbsoluteName, DefaultDirectory))
        {
            rc=1;
            ++Part;
        }
        else
        {
            delete (*Part);
            Part=Parts.erase(Part); //erase every part that does not match
        }
    }

    return rc;
}

int DBMSrvBHist_Copy::ContainsFileBackup(const char * BaseName)
{
    int rc=0;

    for(Tools_List<DBMSrvBHist_Part *>::iterator Part=Parts.begin(); !rc && Part!=Parts.end(); ++Part)
        rc=(*Part)->ContainsFileBackup(BaseName);

    return rc;
}

void DBMSrvBHist_Copy::MarkUsedExternalBackupTools(DBMSrvBTools_ToolEnum * BackupToolType)
{
    for(Tools_List<DBMSrvBHist_Part *>::iterator Part=Parts.begin(); Part!=Parts.end(); ++Part)
    {
        DBMSrvBTools_ToolEnum H=(*Part)->GiveUsedBackupTool(); //get the Type of backup tool for the medium (can be also "None")

        BackupToolType[int(H)]=H;                    //and set flag
    }
}

int DBMSrvBHist_Copy::DBMReturnCode()
{
    int rc=0;

    //check, if all parts of this backup have return code 0
    for(Tools_List<DBMSrvBHist_Part *>::iterator Part=Parts.begin(); 0==rc && Part!=Parts.end(); ++Part)
        rc=(*Part)->GiveDBMReturnCode();

    return rc;
}

const char * DBMSrvBHist_Copy::DBMReturnText()
{
    const char * rc="";

    if(Parts.begin()!=Parts.end())
        rc=(*Parts.begin())->GiveDBMReturnText();

    return rc;
}

int DBMSrvBHist_Copy::IsAvailable() const   //a copy is available, if all parts are available
{
    int rc=1;

    for(Tools_List<DBMSrvBHist_Part *>::const_iterator Part=Parts.begin(); rc && Part!=Parts.end(); ++Part)
    {
        if(!(*Part)->IsAvailable())
            rc=0;
    }

    return rc;
}

int DBMSrvBHist_Copy::IsAvailableExtended() const
{
    int rc=1;

    for(Tools_List<DBMSrvBHist_Part *>::const_iterator Part=Parts.begin(); rc && Part!=Parts.end(); ++Part)
        rc=(*Part)->IsAvailableExtended();

    return rc;
}

tcn00_Error DBMSrvBHist_Copy::IsAvailableExtended(const char * DefaultDirectory, int & Available) const
{
    tcn00_Error rc=OK_CN00;

    Available=1;

    for(Tools_List<DBMSrvBHist_Part *>::const_iterator Part=Parts.begin(); OK_CN00==rc && Available && Part!=Parts.end(); ++Part)
    {
        rc=(*Part)->IsAvailableExtended(DefaultDirectory, Available);
    }

    return rc;
}

char * DBMSrvBHist_Copy::ConstructEBIDString() const
{
    char   * rc=0;
    size_t   Length=0;
    int      NumberOfParts=0;
    Tools_List<DBMSrvBHist_Part *>::const_iterator Part;

    for(Part=Parts.begin(); Part!=Parts.end(); ++Part, ++NumberOfParts)
        Length+=strlen((*Part)->GiveEBID());

    if(0<NumberOfParts)
    {
        if(1<NumberOfParts) //more than one part -> we have to add ", "
            Length+=(strlen(", ")*(NumberOfParts-1));

        if(cn36_StrAlloc(rc, Length))
        {
            Part=Parts.begin();

            SAPDB_strcpy(rc, (*Part)->GiveEBID());

            for(++Part; Part!=Parts.end(); ++Part)
            {
                strcat(rc, ", ");
                strcat(rc, (*Part)->GiveEBID());
            }
        }
    }
    else
        cn36_StrAllocCpy(rc, "");

    return rc;
}

tcn00_Error DBMSrvBHist_Copy::ConstructMedium(tcn003_Medium *& Medium, tcn003_MediaGroup *& MediaGroup, const char * TheBackupType) const
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_Part *>::size_type NumberOfParts=Parts.size();

    if(1==NumberOfParts)
    {
        (*(Parts.begin()))->ConstructMedium(Medium, TheBackupType);
    }
    else
    {
        MediaGroup=new tcn003_MediaGroup;

        if(0!=MediaGroup)
        {
            Tools_List<DBMSrvBHist_Part *>::const_iterator Part=Parts.begin();

            SAPDB_strcpy(MediaGroup->szName, (*Part)->GiveMediumName());
            MediaGroup->pFirst=0;
            MediaGroup->pLast=0;
            MediaGroup->pNext=0;
            MediaGroup->pPrev=0;

            for(; OK_CN00==rc && Part!=Parts.end(); ++Part)
            {
                tcn003_Medium * MediumOfGroup=0;
                rc=(*Part)->ConstructMedium(MediumOfGroup, TheBackupType);

                if(OK_CN00==rc)
                {
                    //insert into group
                    MediumOfGroup->pPrev=MediaGroup->pLast;
                    MediumOfGroup->pNext=0;

                    if(0==MediaGroup->pFirst)
                        MediaGroup->pFirst=MediumOfGroup;

                    MediaGroup->pLast=MediumOfGroup;
                }
            }
        }
        else
            rc=ERR_MEM_CN00;
    }

    return rc;
}
