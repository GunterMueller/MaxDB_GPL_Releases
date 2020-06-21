/*!
  \file    DBMSrvBHist_History.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   defenition of a class for managing the information of the
           backup history files

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

#if !defined(DBMSRVBHIST_HISTORY_HPP)
#define DBMSRVBHIST_HISTORY_HPP


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Backup.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp"
#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"
#include "gcn00.h"
#include "gcn003.h"
#include "hcn36.h"


//-----------------------------------------------------------------------------
// global variable
//-----------------------------------------------------------------------------

const teo00_Uint8 DBMSrvBHist_History_NOLOG=(0x7FFFFFFF);


//-----------------------------------------------------------------------------
// class DBMSrvBHist_History
//-----------------------------------------------------------------------------

class DBMSrvBHist_History
{
  public:
    DBMSrvBHist_History();
    ~DBMSrvBHist_History();

    tcn00_Error Open(VControlDataT * vcontrol,
                     int             CheckAvailabilityOfEBIDs,
                     char          * replyData,
                     int           * replyLen,
                     int             replyLenMax,
                     int             UseMediaHistory=1);
    void Free();

    tcn00_Error AnalyzeLogBackup(tcn003_BackupResult * Result);

    tcn00_Error ReadBasicHistories(const char * DBName,
                                   char       * replyData,
                                   int        * replyLen);

    void        ReduceToFileLogBackupsWithName(const char * AbsoluteName, const char * DefaultDirectory);
    int         ContainsFileBackup(const char                 *  BaseName,
                                   const char                 *  labelFromBackup,
                                   const tcn36_DateTimeString  & backupStartDateTime,
                                   const char                 *& Key,
                                   const char                 *& Label);

    const Tools_List<DBMSrvBHist_Backup *> & GetHistoryList() const;

    void setVControlAndReplyBuffer(VControlDataT * NewVControl,
                                   char          * NewReplyData,
                                   int           * NewReplyLen,
                                   int             NewReplyLenMax);
    void setVControlReplyBufferAndStandardErrorHandler(VControlDataT * NewVControl,
                                                       char          * NewReplyData,
                                                       int           * NewReplyLen,
                                                       int             NewReplyLenMax);

  private:
    enum CloseFileFlag{ReportError, DoNotReportError};

    tcn00_Error ReadHistories(int UseMediaHistory=1);

    tcn00_Error JoinHistoryWithMediaHistory(Tools_List<DBMSrvBHist_Part *> & MediaList);
    tcn00_Error JoinHistoryWithEBIDHistory(Tools_List<DBMSrvBHist_Part *> & EBIDList);
    tcn00_Error JoinEBIDHistoryWithToolData(const DBMSrvBTools_ToolEnum           ToolType,
                                            Tools_List<DBMSrvBHist_Part *>      & EBIDList,
                                            Tools_List<DBMSrvBHist_Part *>      & EBIDsFromTool);

    tcn00_Error CheckAvailabilityOfExternalBackups(Tools_List<DBMSrvBHist_Part *> & EBIDList);

    void setStandardErrorHandler();

    Tools_List<DBMSrvBHist_Backup*> History;
    
    bool CheckEBIDs;

    VControlDataT * VControl;
    DBMSrv_Reply    reply;
};

#endif
