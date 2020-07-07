/*!
  -----------------------------------------------------------------------------

  module: vls33_backup.cpp

  -----------------------------------------------------------------------------

  responsible:  d033893

  special area: SAP DB LOADER

  description:  implementation of backup tool access

  version:      7.6.

  -----------------------------------------------------------------------------

  Copyright (c) 1997-2005 SAP AG-2002

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gls00.h"
#include "heo01.h"
#include "hls11.h"
#include "hls98msg.h"
#include "hls33_backup.h"

/*===========================================================================*
 *     BackupControllerFactory::ConstructBackupController
 *===========================================================================*/
void BackupControllerFactory::ConstructBackupController(
					          BackupController   *& pBackupController,
		                      BackupInfo         *  pBackupInfo)
{
	tsp00_Int4 rc = errOK_els00;

	// BackupControllerFactory
    BackupConfigurationHandler   *pBackupConfigurationHandler   = NULL;
	BackupControllerLogWriter    *pBackupControllerLogWriter    = NULL;
	BackupControllerErrorHandler *pBackupControllerErrorHandler = NULL;
	tcn35_BackupToolConnector    *pBackupToolConnector          = NULL;

	// BackupConfigurationHandler
	pBackupConfigurationHandler   = new BackupConfigurationHandler  ();
	
	// BackupControllerLogWriter
    pBackupControllerLogWriter    = new BackupControllerLogWriter   ();
	
	// BackupControllerErrorHandler
    pBackupControllerErrorHandler = new BackupControllerErrorHandler(pBackupControllerLogWriter);
	
	// BackupToolConnector
	tcn35_BackupToolConnector::ConstructBackupToolConnector(pBackupToolConnector,
		                                                    DBMSrvBTools_ToolEnum(
															REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,pBackupInfo->m_BackupTool.StrPtr())))),
		                                                    pBackupInfo,
															pBackupControllerLogWriter,
													        pBackupControllerErrorHandler,
		                                                    pBackupConfigurationHandler,
															REINTERPRET_CAST(SAPDB_Char *,(CONST_CAST (unsigned char* ,pBackupInfo->m_BackupId.StrPtr()))));
	// BackupController
    pBackupController             = new BackupController   (pBackupInfo,
														    pBackupToolConnector,
														    pBackupConfigurationHandler,
														    pBackupControllerErrorHandler,
														    pBackupControllerLogWriter);
}
/*===========================================================================*
 *     BackupControllerFactory::DestroyBackupController
 *===========================================================================*/
void BackupControllerFactory::DestroyBackupController( 
							  BackupController *pBackupController)
{
}
/*===========================================================================*
 *     BackupInfo::BackupInfo 
 *===========================================================================*/
BackupInfo::BackupInfo( 
		    Tools_DynamicUTF8String TheBackupTool,
			Tools_DynamicUTF8String TheBackupId,
			ADataDirection		    TheDataDirection,
			ADataType			    TheDataType,
			const char *		    TheMediaName,
			int					    TheNumberOfPipes,
			char       *		    ThePipeNames[MAX_NUMBER_OF_DATAPIPES_CN35D],
			int					    TheBlockSize,
			int					    ThePagesPerBlock,
			long				    TheCountInPages[MAX_NUMBER_OF_DATAPIPES_CN35D],
			const char *		    TheDefaultDirectory,
			const char *		    TheNodeName,
			const char *		    TheDBName,
			const char *		    TheSourceNodeName,
			const char *		    TheSourceDBName,
			const char *		    ThePathOfBackupHistory,
			const char *		    ThePathOfExternalBackupHistory )
		   
			:tcn35d_BackupInfo (	TheDataDirection,
									TheDataType,
									TheMediaName,
									TheNumberOfPipes,
									ThePipeNames,
									NULL,
									TheBlockSize,
									ThePagesPerBlock,
									TheCountInPages,
									TheDefaultDirectory,
									TheNodeName,
									TheDBName,
									TheSourceNodeName,
									TheSourceDBName,
									ThePathOfBackupHistory,
									ThePathOfExternalBackupHistory),

            m_BackupTool      (     TheBackupTool ),
            m_BackupId        (     TheBackupId )
{
}
/*===========================================================================*
 *     BackupInfo::~BackupInfo 
 *===========================================================================*/
BackupInfo::~BackupInfo()
{
}
/*===========================================================================*
 *     BackupController::BackupController
 *===========================================================================*/
BackupController::BackupController(
		          tcn35d_BackupInfo             * pBackupInfo,
  				  tcn35_BackupToolConnector     * pBackupToolConnector,
				  BackupConfigurationHandler    * pBackupConfigurationHandler ,
                  BackupControllerErrorHandler  * pBackupControllerErrorHandler,
                  BackupControllerLogWriter     * pBackupControllerLogWriter)

				 :tcn35a_DBBackupController     ( pBackupToolConnector,
				                                  pBackupControllerErrorHandler, 
											      pBackupControllerLogWriter, 
											      pBackupInfo, 
												  pBackupConfigurationHandler),

			      m_pBackupInfo                  (pBackupInfo),
				  m_pBackupToolConnector         (pBackupToolConnector),
				  m_pBackupConfigurationHandler  (pBackupConfigurationHandler),
				  m_pBackupControllerErrorHandler(pBackupControllerErrorHandler),
				  m_pBackupControllerLogWriter   (pBackupControllerLogWriter)
{
}
/*===========================================================================*
 *     BackupController::BeginBackupOperation
 *===========================================================================*/
void BackupController::BeginBackupOperation()
{
	tcn35a_DBBackupController::BeginBackupOperation();
}
/*===========================================================================*
 *     BackupController::EndBackupOperation
 *===========================================================================*/
void BackupController::EndBackupOperation()
{
	LoaderMessages *pMsgInstance = LoaderMessages::Instance();

	tcn35a_DBBackupController::EndBackupOperation();

	if ( NULL != ExternalBackupID[0] )
	{
	     Tools_DynamicUTF8String NewLogFileEntry;

	     m_BackupId.ConvertFromASCII_Latin1( ExternalBackupID[0],
											 ExternalBackupID[0] +
									 _tcslen(ExternalBackupID[0]));

		 pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgCommandExecuting_els98,m_BackupId.StrPtr());
	}
}
/*===========================================================================*
 *     BackupController::WriteHistoryFile
 *===========================================================================*/
void BackupController::WriteHistoryFile()
{
	int NumberOfExternalBackupIDsFound=0;

    for( int i=0; i<MAX_NUMBER_OF_TOOLPROCESSES_CN35; i++)
	{
		if(NULL!=ExternalBackupID[i])
		{
			delete ExternalBackupID[i];
			ExternalBackupID[i]=NULL;
		}
	}
    if(!BackupToolConnector->CollectExternalBackupIDs(ExternalBackupID, NumberOfExternalBackupIDsFound) ||
        NumberOfExternalBackupIDsFound!=BackupInfo->NumberOfPipes())
		exit(1);
}
/*===========================================================================*
 *   BackupConfigurationHandler::BackupConfigurationHandler  
 *===========================================================================*/
BackupConfigurationHandler::BackupConfigurationHandler()
{
}
/*===========================================================================*
 *   BackupConfigurationHandler::~BackupConfigurationHandler  
 *===========================================================================*/
BackupConfigurationHandler::~BackupConfigurationHandler()
{
}
/*===========================================================================*
 *   BackupControllerErrorHandler::BackupControllerErrorHandler()
 *===========================================================================*/
BackupControllerErrorHandler::BackupControllerErrorHandler(
							  BackupControllerLogWriter * pBackupControllerLogWriter)
{
}
/*===========================================================================*
 *   BackupControllerErrorHandler::~BackupControllerErrorHandler  
 *===========================================================================*/
BackupControllerErrorHandler::~BackupControllerErrorHandler()
{
}
/*
===========================================================================*
 *   BackupControllerLogWriter::BackupControllerLogWriter
 *===========================================================================*/
BackupControllerLogWriter::BackupControllerLogWriter()
{
    LoaderMessages *pMsgInstance = LoaderMessages::Instance();

	Tools_DynamicUTF8String logFileName;
	char szLogFileName[256];
    szLogFileName[0] = '\0';
	pMsgInstance->ls98GetLogFileName(szLogFileName);
	m_LogFileName.ConvertFromASCII_Latin1(szLogFileName,
		                                  szLogFileName + 
								  _tcslen(szLogFileName  )); 

}
/*
===========================================================================*
 *   BackupControllerLogWriter::Write
 *===========================================================================*/
int BackupControllerLogWriter::Write(const char * Message, MessageModifier Modifier)
{
	LoaderMessages *pMsgInstance = LoaderMessages::Instance();
    pMsgInstance->ls98PrintMessage(layDataExtract_els98, msgCommandExecuting_els98,Message);

	return 0;
}
/*
===========================================================================*
 *   BackupControllerLogWriter::Write
 *===========================================================================*/
int BackupControllerLogWriter::Write(MessageModifier Modifier)
{
	return 0;
}
/*
===========================================================================*
 *   BackupControllerLogWriter::Write
 *===========================================================================*/
int BackupControllerLogWriter::Write(tni34_AFile & TheFileToMove, const char * FileDescription)
{
	return 0;
}
/*===========================================================================*
 *   BackupControllerLogWriter::~BackupControllerLogWriter   
 *===========================================================================*/
BackupControllerLogWriter::~BackupControllerLogWriter()
{
}

/*===========================================================================*
 *     END
 *===========================================================================*/

