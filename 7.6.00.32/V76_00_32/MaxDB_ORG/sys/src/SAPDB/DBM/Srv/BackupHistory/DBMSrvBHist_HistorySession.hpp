/*!
  \file    DBMSrvBHist_HistorySession.hpp
  \author  TiloH
  \ingroup backup history handling by the DBMServer
  \brief   declaration of classes handling the DBMServer
           backup history session and functionality working
           on it

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

#if !defined(DBMSrvBHist_HistorySession_hpp)
#define DBMSrvCmd_BackupHistorySession_hpp


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/DBMSrv_Reply.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_ListProducer.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_History.hpp"


//-----------------------------------------------------------------------------
// class DBMSrvBHist_HistorySession
//-----------------------------------------------------------------------------

/*! \brief A class for handling a DBMServer backup history session

    The class provides the functionality of the global history session. It uses
    a DBMSrvBHist_History object for reading and analyzing the backup history
    files. Based on that it implements the functions for listing the backup history.

     \uses DBMSrvBHist_History */
class DBMSrvBHist_HistorySession: public DBMSrvBHist_ListProducer
{
  public:
    //! \brief constructor, just initializing, no backup history information is
    //!        read from the file system
    DBMSrvBHist_HistorySession();

    //! \brief destructor, also freeing all memory used by backup history
    //         information, that is maintained by this object
    ~DBMSrvBHist_HistorySession();

    //! \brief allocate a new gloal history session
    static tcn00_Error allocateGlobalSession(VControlDataT               * vcontrol,
                                             DBMSrv_Reply                & Reply,
                                             DBMSrvBHist_HistorySession *& HistorySession);

    //! \brief get a pointer to the global history session from vcontrol
    static tcn00_Error getGlobalSession(VControlDataT               * vcontrol,
                                        DBMSrv_Reply                & Reply,
                                        DBMSrvBHist_HistorySession *& HistorySession);

    //! \brief frees the history session from the VControlDataT structure
    static void freeGlobalSession(VControlDataT * vcontrol);


    //! \brief read the backup history files and initialise members for the list commands
    tcn00_Error open(VControlDataT * vcontrol,
                     CommandT      * command,
                     DBMSrv_Reply  & Reply);

    //! \brief determine backup history information that must be be outputed
    //!        and list the first part of it into the reply buffer until the buffer
    //!        is full
    tcn00_Error list(VControlDataT * vcontrol,
                     CommandT      * command,
                     DBMSrv_Reply  & Reply);

    //! \brief list the next part of backup history information into the buffer until that is full
    tcn00_Error listNext(VControlDataT * vcontrol,
                         CommandT      * command,
                         DBMSrv_Reply  & Reply);

    //! \brief free the memory used by backup history information
    tcn00_Error close(VControlDataT * vcontrol,
                      CommandT      * command,
                      DBMSrv_Reply  & Reply);

    //! \brief determine the date of the backup history file (dbm.knl)
    tcn00_Error date(VControlDataT * vcontrol,
                     CommandT      * command,
                     DBMSrv_Reply  & Reply);

  private:
    void Free();

    tcn00_Error Open(VControlDataT * vcontrol,
                     int             WithEBIDs,
                     DBMSrv_Reply  & Reply);

    DBMSrvBHist_History             History;    //!< object handling the backup history files and providing the information of those files as list
    Tools_List<DBMSrvBHist_Backup*> OutputList; //!< a list of pointers into the information of History, this list is detemining the output order of the list commands

    Tools_List<DBMSrvBHist_Backup*>::const_iterator FirstBackup;      //!< pointing to the first backup that must be used from the History object

    Tools_List<DBMSrvBHist_Backup*>::iterator CurrentBackup;    //!< element from OutputList that is listed next
    Tools_List<DBMSrvBHist_Backup*>::iterator StartBackup;      //!< first element of OutputList to be listed
    Tools_List<DBMSrvBHist_Backup*>::iterator EndBackup;        //!< last element of OutputList to be listed

    char * ColumnsAsString;
    char * Key;
    char * Label;
    char * Action;
    char * Restore;
    char * Until;

    bool ForRestore;
    bool RcOKOnly;
    bool WithMedia;
    bool WithEBIDs;
    bool ListInverted;

    teo00_Uint8 UsedLogPage;
    teo00_Uint8 FirstLogPage;
    bool Restartable;

    Tools_List<DBMSrvBHist_Backup::Column> Columns;

    DBMSrvBHist_BackupColumnWidths    ColumnWidthsBackup;
    DBMSrvBHist_PartMediaColumnWidths ColumnWidthsMedia;


    tcn00_Error CheckHistoryFile(bool                & Exists,
                                 char                  Date[64],
                                 const tsp00_DbNamec & DBName,
                                 DBMSrv_Reply        & Reply);

    tcn00_Error InitListParameters(DBMSrv_Reply & Reply);

    tcn00_Error FindFirstRestoreItem();
    tcn00_Error CreateOutputList(DBMSrv_Reply & Reply);
    tcn00_Error BasicListHistory(DBMSrv_Reply & Reply);
    bool        PrintCurrentBackupInto(DBMSrv_Reply & Reply/*char * Buffer, size_t MaxUsableBufferSize*/);
    void        EnlargeColumnWidths();
};

#endif
