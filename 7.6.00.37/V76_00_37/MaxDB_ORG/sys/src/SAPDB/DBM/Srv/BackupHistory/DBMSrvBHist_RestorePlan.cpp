/*!
  \file           DBMSrvBHist_RestorePlan.cpp
  \author         TiloH
  \ingroup        backup history handling by the DBMServer
  \brief          implements a class for holding a plan laying out a recovery
                  with the help of the backup history

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

#include <limits.h>
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlan.hpp"

DBMSrvBHist_RestorePlan::DBMSrvBHist_RestorePlan()
{
}

DBMSrvBHist_RestorePlan::~DBMSrvBHist_RestorePlan()
{
    //last chance to clear up the memory this list is managing
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator h=ListOfAllNodes.begin();

    for(; h!=ListOfAllNodes.end(); ++h)
    {
        delete(*h);
        *h=0;
    }
};

int DBMSrvBHist_RestorePlan::push_back(DBMSrvBHist_RestorePlanNode * newNode)
{
    return ListOfAllNodes.push_back(newNode);
}

Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator DBMSrvBHist_RestorePlan::begin()
{
    return ListOfAllNodes.begin();
}

Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator DBMSrvBHist_RestorePlan::end()
{
    return ListOfAllNodes.end();
}

Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator DBMSrvBHist_RestorePlan::rbegin()
{
    return ListOfAllNodes.rbegin();
}

Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator DBMSrvBHist_RestorePlan::rend()
{
    return ListOfAllNodes.rend();
}

int DBMSrvBHist_RestorePlan::empty()
{
    return ListOfAllNodes.empty();
}

DBMSrvBHist_RestorePlanNode * DBMSrvBHist_RestorePlan::back()
{
    return ListOfAllNodes.back();
}

DBMSrvBHist_RestorePlanNode * DBMSrvBHist_RestorePlan::GetFirstNeededBackup() const
{
    return FirstBackup;
}

void DBMSrvBHist_RestorePlan::SetFirstNeededBackup(DBMSrvBHist_RestorePlanNode * NewFirstBackup)
{
    FirstBackup=NewFirstBackup;
}



DBMSrvBHist_RestorePlanNode::DBMSrvBHist_RestorePlanNode(const DBMSrvBHist_Backup * theBackup)
:backup(theBackup),
 NextDuringRestore(0),
 Visited(0),
 FirstAfterHistlost(0)
{
}

const DBMSrvBHist_Backup * DBMSrvBHist_RestorePlanNode::getBackup() const
{
    return backup;
}

int DBMSrvBHist_RestorePlanNode::addPreviousBackup(DBMSrvBHist_RestorePlanNode *newPreviousNode)
{
    return PreviousBackups.push_front(newPreviousNode);
}

int DBMSrvBHist_RestorePlanNode::addFollowingBackup(DBMSrvBHist_RestorePlanNode *newFollowingNode)
{
    return FollowingBackups.push_front(newFollowingNode);
}

const Tools_List<DBMSrvBHist_RestorePlanNode *> & DBMSrvBHist_RestorePlanNode::GiveFollowingBackups() const
{
    return FollowingBackups;
}

void DBMSrvBHist_RestorePlanNode::InitNode()
{
    NextDuringRestore=0;
    Level=UINT_MAX;
    Visited=0;
}

tcn00_Error DBMSrvBHist_RestorePlanNode::AddToNextLevel(unsigned int                                NextLevel,
                                                        Tools_List<DBMSrvBHist_RestorePlanNode *> & ListNextLevel,
                                                        DBMSrvBHist_RestorePlanNode               * NextNode)
{
    tcn00_Error rc=OK_CN00;

    if(ListNextLevel.push_back(this))
    {
        Level=NextLevel;
        NextDuringRestore=NextNode;
    }
    else
        rc=ERR_MEM_CN00;

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanNode::AddNonMinimalPreviousToNextLevel(unsigned int CurrentLevel, Tools_List<DBMSrvBHist_RestorePlanNode *> & ListNextLevel, const char * DefaultDirectory)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator i=PreviousBackups.begin();

    for(; OK_CN00==rc && i!=PreviousBackups.end(); ++i)                                     //add all possible previous backups ...
    {
        if(CurrentLevel<(*i)->Level)                                                        //... that have not been visited before
        {
            int IsAvailable=0;

            rc=(*i)->getBackup()->IsAvailable(DefaultDirectory, IsAvailable);

            if(OK_CN00==rc && IsAvailable)                                                  // ...and that are available (files must exist)
                rc=(*i)->AddToNextLevel(CurrentLevel, ListNextLevel, this);
        }
    }

    return rc;
}

tcn00_Error DBMSrvBHist_RestorePlanNode::AddUnvisitedFollowingToList(Tools_List<DBMSrvBHist_RestorePlanNode *> & List)
{
    tcn00_Error rc=OK_CN00;

    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator i=FollowingBackups.begin();

    for(; OK_CN00==rc && i!=FollowingBackups.end(); ++i) //add all possible following backups ...
        if(!(*i)->Visited)                      //... that have not been visited before
        {
            if(List.push_back(*i))
                (*i)->Visited=1;
            else
                rc=ERR_MEM_CN00;
        }

    return rc;
}

const DBMSrvBHist_RestorePlanNode * DBMSrvBHist_RestorePlanNode::NextNeededBackup() const
{
    return NextDuringRestore;
}

void DBMSrvBHist_RestorePlanNode::SetAsFirstAfterHistlost()
{
    FirstAfterHistlost=1;
}

int DBMSrvBHist_RestorePlanNode::WasFirstAfterHistlost() const
{
    return FirstAfterHistlost;
}
