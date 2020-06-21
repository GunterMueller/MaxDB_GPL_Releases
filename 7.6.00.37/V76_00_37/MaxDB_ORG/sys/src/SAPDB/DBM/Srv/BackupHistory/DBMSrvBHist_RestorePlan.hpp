/*!
  \file           DBMSrvBHist_RestorePlan.hpp
  \author         TiloH
  \ingroup        backup history handling by the DBMServer
  \brief          defines a class for holding a plan laying out a recovery
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

#if !defined (_DBMSRVBHIST_RESTOREPLAN_HPP_)
#define _DBMSRVBHIST_RESTOREPLAN_HPP_

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp"


class DBMSrvBHist_RestorePlanNode;


////////////////////////////////////////////////////////////////////////////////
//!  class DBMSrvBHist_RestorePlan
//!
//! \brief This class determines a restore plan from a given backup history
//!        
////////////////////////////////////////////////////////////////////////////////

class DBMSrvBHist_RestorePlan
{
  public:
    DBMSrvBHist_RestorePlan();
    ~DBMSrvBHist_RestorePlan();

    int push_back(DBMSrvBHist_RestorePlanNode * newNode);
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator begin();
    Tools_List<DBMSrvBHist_RestorePlanNode *>::iterator end();
    Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator rbegin();
    Tools_List<DBMSrvBHist_RestorePlanNode *>::reverse_iterator rend();
    int empty();
    DBMSrvBHist_RestorePlanNode * back();

    DBMSrvBHist_RestorePlanNode * GetFirstNeededBackup() const;
    void SetFirstNeededBackup(DBMSrvBHist_RestorePlanNode * NewFirstBackup);

  private:
    Tools_List<DBMSrvBHist_RestorePlanNode *> ListOfAllNodes;
    DBMSrvBHist_RestorePlanNode *             FirstBackup;
};


class DBMSrvBHist_RestorePlanNode
{
  public:

    DBMSrvBHist_RestorePlanNode(const DBMSrvBHist_Backup * theBackup);

    const DBMSrvBHist_Backup * getBackup() const;
    int                        addPreviousBackup(DBMSrvBHist_RestorePlanNode *newPeviousNode);
    int                        addFollowingBackup(DBMSrvBHist_RestorePlanNode *newFollowingNode);

    const Tools_List<DBMSrvBHist_RestorePlanNode *> & GiveFollowingBackups() const;

    void                          InitNode();
    tcn00_Error                   AddToNextLevel(unsigned int                                NextLevel,
                                                 Tools_List<DBMSrvBHist_RestorePlanNode *> & ListNextLevel,
                                                 DBMSrvBHist_RestorePlanNode               * NextNode);
    tcn00_Error                   AddNonMinimalPreviousToNextLevel(unsigned int                                Level,
                                                                   Tools_List<DBMSrvBHist_RestorePlanNode *> & ListNextLevel,
                                                                   const char                                * DefaultDirectory);
    tcn00_Error                   AddUnvisitedFollowingToList(Tools_List<DBMSrvBHist_RestorePlanNode *> & List);

    const DBMSrvBHist_RestorePlanNode * NextNeededBackup() const;

    void SetAsFirstAfterHistlost();
    int  WasFirstAfterHistlost() const;

  private:
    const DBMSrvBHist_Backup * backup;                              //!< the pointer to the corresponding backup 

    Tools_List<DBMSrvBHist_RestorePlanNode *> FollowingBackups;     //!< a list representing the backups that can be restored immediatly after (*this.backup)
    Tools_List<DBMSrvBHist_RestorePlanNode *> PreviousBackups;      //!< a list representing the backups that can be restored immediatly before (*this.backup)
    int                                       FirstAfterHistlost;   //!< 1 if backup is the first one after a "HISTLOST"

    const DBMSrvBHist_RestorePlanNode * NextDuringRestore;          //!< pointing to the following backup of the backup strategy

    unsigned int Level;                                             //!< distance to the last backup of the backup strategy
    int          Visited;                                           //!< a flag, if this node was visited during a breadth-first search
};

#endif
