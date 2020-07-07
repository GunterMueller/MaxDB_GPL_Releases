/*!
  \file    hcn35b.h
  \author  TiloH
  \ingroup cn layer
  \brief   class for controling backups from the database kernel to an 
           external backup tool
  \see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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


#if !defined (_HCN35B_H_)
#define _HCN35B_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_EBIDList.hpp"
#include "gcn00.h"
#include "gcn003.h"
#include "hcn35a.h"
#include "hcn35e.h"
#include "hcn36.h"
#include "hcn90.h"


class tcn35b_DBMExternalBackupControllerConstructorArguments;
class DBMSrvKnl_Session;

////////////////////////////////////////////////////////////////////////////////
//! \class tcn35b_DBMExternalBackupController
//! \brief This class handles one backup with an external backup tool. If you
//!        need more than one backup, use more than one object.
////////////////////////////////////////////////////////////////////////////////

class tcn35b_DBMExternalBackupController: public tcn35a_DBBackupController
{
  public:

    //! The only constructor.
    //!
    //! \param Args [in] a struct containing all the info needed for a backup
    //!
    tcn35b_DBMExternalBackupController(tcn35b_DBMExternalBackupControllerConstructorArguments & Args);
    ~tcn35b_DBMExternalBackupController();

    int  ListEBIDsOfAvailableBackups(DBMSrvBHist_EBIDList & AvailableEBIDs);

    void        ChangeReplyBuffer(char * TheReplyBuffer, 
                                  int  * TheReplyBufferLength);
    tcn00_Error GiveLastError();
    int         GiveSQLResult();

    virtual int IsBad();

  private:
    int  Prepare();
    int  StartDBAction();
    int  DBIsReady();
    int  CheckForDBReply();
    int  DBMayBeUsingLogDevspaceForLogRestore();
    int  DBActionWasSuccessful();
    void SignalEndToDB(int elapsedTimeout);
    void TerminateDBRequest();
    void WriteHistoryFile();
    void DBPostOperation();
    void FillReplyBuffer();

    int  SendBackupRestoreRequestToKernel(DBMSrvKnl_Session & Session,
                                          const char        * KernelCommand,
                                          int               & SQLCode,
                                          int                 BeQuiet=0,
                                          int                 ForBackup=1);
    int  DBReplyAvailable(bool OutputMessagesForNonEvents=true);
    void ReceiveBackupRestoreRequestReplyFromDB();

    void BackupRestoreIgnore();
    void ReceiveReplyFromDBKernel(DBMSrvKnl_Session   & Session,
                                  int                 & SQLCode,
                                  tcn003_BackupResult & tResult,
                                  bool                & IsNoSQLError,
                                  bool                  verbose,
                                  DBMSrvMsg_Error     & KernelSessionError);

    int  GetStatusOfUtilityTask(tcn003_BackupResult &StatusInformation);

    const char * FindNextSectionInLine(const char * Line, int Number=1);
    const char * GetBackupIDFromDBMKnl(const char *  AbsoluteNameOfControlKnl,
                                       const char *  Label,
                                       const char *  szDBStamp1Date,
                                       const char *  szDBStamp1Time,
                                       const char *& StartDateTime);
    void FreeLogAfterLogBackup();

    tcn35e_ConfigurationVariableHandler * ConfigVarHandler;

    char              * ReplyData;
    int               * ReplyLen;
    DBMSrvKnl_Session * SQLSession;
    char              * KernelCommand;

    tcn003_BackupResult TheDBResult;        // containing result, if StatusOfRequest==RequestReplied or ==CanceledByDBMSrv
    int                 SQLCodeOfRequest;   // containing the SQLCode, if StatusOfRequest==ReceivedReply or ==CanceledByDBMSrv
    bool                SQLErrForRequest;   // true if SQLCodeOfRequest is an error
    tsp00_C256c         SQLErrText;         // containing errmsg if SQLCodeOfRequest is an error
    char *              FirstLogPageNo;     // containing copy of first Log Page number if StatusOfRequest==RequestReplied (maybe just "")
    char *              LastLogPageNo;      // containing copy of last Log Page number if StatusOfRequest==RequestReplied (maybe just "")
    char *              ToolErrorText;      //!< contains the error text of the backup tool error, if StatusOfRequest==ReceivedReply and SQLCode==0

    enum ADBRequestType{BackupOrRestore, DeleteLog};
    ADBRequestType DBRequestType;

    void PrintBackupAnswer();
    int ToolMustNotBeSuccessful();

    static const char * DataTypeToListSaveTypeMap[5];

    char * InternalBackupID;
    char * StartDateTime;
    int    LogRequiredFlag;

    long PageSize;

    tcn35e_DBMBackupControllerErrorHandler * ErrorHandlerAsDBMErrorHandler;
};


////////////////////////////////////////////////////////////////////////////////
//! \brief class constructing tcn35b_DBMExternalBackupController objects
//!
////////////////////////////////////////////////////////////////////////////////

class tcn35b_DBMExternalBackupControllerFactory: public tcn35e_TheDBMBackupControllerFactory
{
  public:
    tcn00_Error ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                      char                               *  TheReplyBuffer, 
                                                      int                                *  TheReplyBufferLength, 
                                                      tcn35d_BackupInfo::ADataDirection     TheDataDirection,
                                                      const char                         *  BackupType,
                                                      const int                             WithoutCheckpoint,
                                                      DBMSrvKnl_Session                  *  TheSQLSession,
                                                      const char                         *  TheKernelCommand,
                                                      const tcn003_Medium                *  pMedium,
                                                      const tcn003_MediaGroup            *  pMediaGroup,
                                                      const char                         *  TheDBName,
                                                      const char                         *  TheSourceNodeName=0,
                                                      const char                         *  TheSourceDBName=0,
                                                      const char                         *  ExternalBackupIDList=0,
                                                      const char                         *  EndOfExternalBackupIDList=0);

    tcn00_Error ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                      char                               *  TheReplyBuffer, 
                                                      int                                *  TheReplyBufferLength, 
                                                      tcn35d_BackupInfo::ADataDirection     TheDataDirection,
                                                      DBMSrvBTools_ToolEnum                 BackupToolType,
                                                      const char                         *  TheDBName,
                                                      const char                         *  TheSourceDBName=0,
                                                      const char                         *  TheSourceNodeName=0);
  private:

    struct InputStructForConstruct
    {
        char                               *  TheReplyBuffer;
        int                                *  TheReplyBufferLength;
        tcn35d_BackupInfo::ADataDirection     TheDataDirection;
        tcn35d_BackupInfo::ADataType          TheDataType;
        DBMSrvKnl_Session                  *  TheSQLSession;
        const char                         *  TheKernelCommand;
        DBMSrvBTools_ToolEnum                 BackupToolType;
        const char                         *  MediumName;
        int                                   NumberOfPipes;
        char                              **  ThePipeNames;
        char                              **  TheMediumTypes;
        long                                  BytesPerBlock;
        long                                  PagesPerBlock;
        long                                  BytesPerPage;
        long                               *  CountInPages;
        const char                         *  TheDBName;
        const char                         *  TheSourceNodeName;
        const char                         *  TheSourceDBName;
        const char                         *  BackupHistory;
        const char                         *  ExternalBackupHistory;
        const char                         *  ExternalBackupIDList;
        const char                         *  EndOfExternalBackupIDList;
    };
      
    tcn00_Error ConstructADBMExternalBackupController(tcn35b_DBMExternalBackupController *& TheNewBackupController,
                                                      InputStructForConstruct             & TheInputPars);  //needed for avoiding compiler warning

    int DataTypeForDBMBackupType(tcn35d_BackupInfo::ADataType          & DataType,
                                 const char                            * DBMBackupType,
                                 int                                     WithoutCheckpoint,
                                 tcn35d_DBBackupControllerErrorHandler & ErrorHandler);
};


////////////////////////////////////////////////////////////////////////////////
//! \brief a globally available factory for tcn35b_DBMExternalBackupController
//! objects
////////////////////////////////////////////////////////////////////////////////
extern tcn35b_DBMExternalBackupControllerFactory TheDBMExternalBackupControllerFactory_cn35b;

#endif
