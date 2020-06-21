/*!
  \file           DBMSrvBHist_RestorePlanner.hpp
  \author         TiloH
  \ingroup        backup history handling by the DBMServer
  \brief          defines a class for determining a plan for a recovery
                  from a backup history

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


#if !defined (_DBMSRVBHIST_RESTOREPLANNER_HPP_)
#define _DBMSRVBHIST_RESTOREPLANNER_HPP_

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_RestorePlan.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp"

////////////////////////////////////////////////////////////////////////////////
//!  class DBMSrvBHist_RestorePlanner
//!
//! \brief This class determines a restore plan from a given backup history
//!        
////////////////////////////////////////////////////////////////////////////////

class DBMSrvBHist_RestorePlanner
{
  public:
    DBMSrvBHist_RestorePlanner();

    static tcn00_Error AutomaticRecovery(VControlDataT * vcontrol,
                                         CommandT      * command,
                                         char          * replyData,
                                         int           * replyLen,
                                         int             replyLenMax);

    const DBMSrvBHist_RestorePlan * DeterminePlanFor(DBMSrvBHist_History & History,
                                                     const char          * UntilDate,
                                                     const char          * UntilTime);
    tcn00_Error                     FindRestoreStrategy(const teo00_Uint8   FirstLogPage,
                                                        const teo00_Uint8   UsedLogPage,
                                                        const int           LogOnly,
                                                        const char        * DefaultDirectory,
                                                        const char        * FirstInternalBackupID,
                                                        int                 IsRestartable,
                                                        int                 IsForPointInTime);

  private:
    DBMSrvBHist_RestorePlan RestorePlan;
    char                    LastBackupType[ME_SAVETYPE_LENGTH_MXCN003+1];

    tcn00_Error RestoreABackup(const DBMSrvBHist_RestorePlanNode * Backup,
                               int                                 OnlyCheck,
                               const char                        * UntilDate,
                               const char                        * UntilTime,
                               VControlDataT                     * vcontrol,
                               char                              * replyData,
                               int                               * replyLen,
                               int                                 replyLenMax,
                               int                               & BackupResult);

    int linkTogether(DBMSrvBHist_RestorePlanNode *previous, DBMSrvBHist_RestorePlanNode * following);
    int takeoverLinks(DBMSrvBHist_RestorePlanNode *previous, DBMSrvBHist_RestorePlanNode * supplier);

    static tcn00_Error DetermineRestartInfo(VControlDataT * vcontrol,
                                            char          * replyData,
                                            int           * replyLen,
                                            int             replyLenMax,
                                            teo00_Uint8   & UsedLogPage,
                                            teo00_Uint8   & FirstLogPage,
                                            int           & Restartable);

    static tcn00_Error DBOnline(VControlDataT * vcontrol,
                                char          * replyData,
                                int           * replyLen,
                                int             replyLenMax,
                                const char    * UntilDate,
                                const char    * UntilTime);
};

#endif
