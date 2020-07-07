/*! 
  -----------------------------------------------------------------------------
 
  module: hls33_backup.h
 
  -----------------------------------------------------------------------------
 
  responsible:  d033893

  special area: SAP DB LOADER
 
  description:  declaration of backup tool access
                
  version:      7.4.

  -----------------------------------------------------------------------------
 
  Copyright (c) 1997-2005 SAP AG-2001
 
  -----------------------------------------------------------------------------



    ========== licence begin  GPL
    Copyright (c) 1997-2005 SAP AG

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




*/

#ifndef HLS33_BACKUP
#define HLS33_BACKUP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "hcn35a.h"
#include "hcn35d.h"

#ifdef UNIX
#undef UNIX
#endif

#include "ToolsCommon/Tools_DynamicUTF8String.hpp"  // unicode support

/*===========================================================================*
 *  FORWARD DECLARATIONS                                                     *
 *===========================================================================*/
class BackupInfo;
class BackupController;
class BackupConfigurationHandler;
class BackupControllerErrorHandler;
class BackupControllerLogWriter;

/*===========================================================================*
 *    BackupControllerFactory
 *===========================================================================*/
class BackupControllerFactory
{
  public:
	void ConstructBackupController( BackupController *& pBackupController,
		                            BackupInfo       *  pBackupInfo       );
	void DestroyBackupController  ( BackupController *  pBackupController );
};	
/*===========================================================================*
 *    BackupController
 *===========================================================================*/
class BackupController: public tcn35a_DBBackupController
{
  public:
      BackupController(tcn35d_BackupInfo             * pBackupInfo,
		               tcn35_BackupToolConnector     * pBackupToolConnector,
					   BackupConfigurationHandler    * pBackupConfigurationHandler,
                       BackupControllerErrorHandler  * pBackupControllerErrorHandler,
                       BackupControllerLogWriter     * pBackupControllerLogWriter );

	 void BeginBackupOperation();
     void EndBackupOperation();

	 Tools_DynamicUTF8String         m_BackupId;

  private:
	 tcn35d_BackupInfo             * m_pBackupInfo;
	 tcn35_BackupToolConnector     * m_pBackupToolConnector;
	 BackupConfigurationHandler    * m_pBackupConfigurationHandler;
	 BackupControllerErrorHandler  * m_pBackupControllerErrorHandler;
	 BackupControllerLogWriter     * m_pBackupControllerLogWriter;

  protected:
	 int  StartDBAction()						  { return 1;}
     int  Prepare()								  { return 1;}
     int  DBIsReady()							  { return 1;}
	 void DBPostOperation()						  { ;}
     int  DBReplyAvailable(bool OutputMessagesForNonEvents=true) { return 1;}
     void FillReplyBuffer()						  { ;}
	 void TerminateDBRequest()					  { ;}
	 void WriteHistoryFile();
	 void ReceiveBackupRestoreRequestReplyFromDB(){ ;}
	 int  DBMayBeUsingLogDevspaceForLogRestore  (){ return 0;}
	 int  DBActionWasSuccessful()                 { return 1;}
	 void SignalEndToDB(int elapsedTimeout)       { ;}

};
/*===========================================================================*
 *    BackupInfo
 *===========================================================================*/
class BackupInfo :public tcn35d_BackupInfo 
{
   public:
	 BackupInfo(Tools_DynamicUTF8String		TheBackupTool,
		        Tools_DynamicUTF8String		TheBackupId,
				ADataDirection				TheDataDirection,
				ADataType					TheDataType,
				const char *				TheMediaName,
				int							TheNumberOfPipes,
				char       *				ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D],
				int							TheBlockSize,
				int							ThePagesPerBlock,
				long						TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D],
				const char *				TheDefaultDirectory,
				const char *				TheNodeName,
				const char *				TheDBName,
				const char *				TheSourceNodeName=0,
				const char *				TheSourceDBName=0,
				const char *				ThePathOfBackupHistory=0,
				const char *				ThePathOfExternalBackupHistory=0);
	 
	 ~BackupInfo();

	 Tools_DynamicUTF8String m_BackupTool;
	 Tools_DynamicUTF8String m_BackupId;
};
/*===========================================================================*
 *    BackupConfigurationHandler
 *===========================================================================*/
class BackupConfigurationHandler  : public tcn35d_ConfigurationVariableHandler
{
	public:
	  BackupConfigurationHandler();
	 ~BackupConfigurationHandler();
};
/*===========================================================================*
 *    BackupControllerErrorHandler
 *===========================================================================*/
class BackupControllerErrorHandler : public tcn35d_DBBackupControllerErrorHandler
{
	public:
	  BackupControllerErrorHandler( BackupControllerLogWriter * pBackupControllerLogWriter );
     ~BackupControllerErrorHandler();
};
/*===========================================================================*
 *    BackupControllerLogWriter
 *===========================================================================*/
class BackupControllerLogWriter    : public tcn35d_DBBackupControllerLogWriter
{
	public:
	  BackupControllerLogWriter();
	 ~BackupControllerLogWriter();

	 int Write(MessageModifier Modifier);
	 int Write(const char * Message, MessageModifier Modifier=tcn35d_DBBackupControllerLogWriter::UsePrefix);
     int Write(tni34_AFile & TheFileToMove, const char * FileDescription);

	  Tools_DynamicUTF8String m_LogFileName;
};

#endif

/*===========================================================================*
 *    END
 *===========================================================================*/
