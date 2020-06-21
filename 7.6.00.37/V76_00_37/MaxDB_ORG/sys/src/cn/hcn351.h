/*!
    \file      hcn351.h
    \author    TiloH
    \ingroup   cn layer
    \brief     connection between dbmsrv and external backup tool ADSM

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1999-2006 SAP AG

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

#if !defined (_HCN351_H_)
#define _HCN351_H_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hcn35.h"
#include "hcn35a.h"

#if defined (WIN32)
    #include<windows.h>
#elif defined(UNIX)
    #include<stdio.h>
#else
#error hcn351.h only coded for UNIX and WIN32 (define one of them.)
#endif


// -----------------------------------------------------------------------------
// class tcn351_ADSMConnection
// -----------------------------------------------------------------------------

class tcn351_ADSMConnector:public tcn35_BackupToolConnector
{
  public:
    tcn351_ADSMConnector(tcn35d_BackupInfo                     * BackupInfo,
                         tcn35d_DBBackupControllerLogWriter    * TheLogWriterToUse,
                         tcn35d_DBBackupControllerErrorHandler * TheErrorHandlerToUse,
                         tcn35d_ConfigurationVariableHandler   * TheConfigurationVariableHandler,
                         char                                  * TheExternalBackupIDList=0);
    ~tcn351_ADSMConnector();

    virtual const char * GiveToolName() const;
    virtual const char * GiveLongToolName() const;
    virtual const DBMSrvBTools_ToolEnum ToolEnum() const;

  private:
    int CheckToolInstallation(int & IsInstalled);
    int PrepareFirst();
    int PrepareAgain();
    int StartTool();
    int SignalEndToTool(int DBWasSuccessful);
    int CleanUpTool();

    int CollectExternalBackupIDs(char * ExternalBackupID[MAX_NUMBER_OF_TOOLPROCESSES_CN35], int & NumberOfEBIDsFound);

    int GiveNextAvailableBackup(int          & EBIDIsAvailable,
                                const char * & EBID,
                                size_t       & EBIDLength,
                                tcn35d_BackupInfo::ADataType    & DataType,
                                const char * & DateTimeString,
                                int          & ReachedEnd);

    int GiveInformationOfLine(const char                   *   Line,
                              const char                   * & EBIDInLine,
                              size_t                         & EBIDLength,
                              tcn35d_BackupInfo::ADataType   & DataType,
                              const char                   * & DateTimeString,
                              int                            & LineContainsEBID);

    char * Label;           // TODO: should be of type string<> 
    int    BuildLabel();
    char * ADSMDirectory;   // TODO: should be of type string<>
    char * ADSMProgram;     // TODO: should be of type string<>

    tni34_APipe * ADSMInput;

    int    BuildBackupToolCall(); // TODO: move to base-class

    
    static struct DataTypeActionStringMapEntry
    {
        tcn35d_BackupInfo::ADataType   DataType;
        const char                   * ActionString;
    }DataTypeActionStringMap[5];
    static const int NumberOfDataTypeActionStringMapEntries;
    
    int DataTypeForActionString(tcn35d_BackupInfo::ADataType & DataType,
                                const char                   * StartOfActionString,
                                size_t                         LengthOfActionString);
    int ActionStringForDataType(const char                   * & ActionString,
                                tcn35d_BackupInfo::ADataType     DataType);

    ni31_List<char *>            TempEBIDList;
    tcn35d_BackupInfo::ADataType TempDataType;
    int ConstructEBIDFromParallelLabel(char       *& NewEBID,
                                       int           NumberOfEBID,
                                       int           NumberOfPieces,
                                       const char *  Label,
                                       size_t        LengthWithoutPieces);
    int ConstructParallelLabelFromEBID(char       *& ParallelLabel,
                                       const char *  EBID,
                                       const char *  PieceSection,
                                       int           NumberOfPieces);
    int IsPiecesPart(const char * Part, size_t partLength, int & NumberOfPieces);
    int IsPiecePart(const char * Part, int & NumberOfPieces);
};

#endif
