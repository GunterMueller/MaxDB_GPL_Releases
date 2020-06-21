/*!
    \file    DBMSrvBHist_Backup.hpp
    \author  TiloH
    \ingroup backup history handling by the DBMServer
    \brief   a class for managing the information of a backup

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


#if !defined (_DBMSRVBHIST_BACKUP_HPP_)
#define _DBMSRVBHIST_BACKUP_HPP_

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_Copy.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp"
#include "gcn00.h"
#include "gcn003.h"
#include "SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp"
#include "hcn36.h"


//-----------------------------------------------------------------------------
//forward declarations
//-----------------------------------------------------------------------------

class DBMSrvBHist_BackupColumnWidths;

//-----------------------------------------------------------------------------
// class DBMSrvBHist_Backup
//-----------------------------------------------------------------------------

class DBMSrvBHist_Backup: public DBMSrvBHist_LineProducer
{
  public:
    enum Column{KeyColumn            =0,
                LabelColumn          =1,
                ActionColumn         =2,
                DBStamp1Column       =3,
                DBStamp2Column       =4,
                StartDateColumn      =5,
                StopDateColumn       =6,
                StartLogPageColumn   =7,
                StopLogPageColumn    =8,
                LogRequiredColumn    =9,
                MediumNameColumn     =10,
                PagesTransferedColumn=11,
                VolumesColumn        =12,
                ReturnCodeColumn     =13,
                ReturnTextColumn     =14,
                NoColumn             =15};

    static Column ColumnFor(const char * KeyWord, size_t KeyWordLength);

    DBMSrvBHist_Backup();
    ~DBMSrvBHist_Backup();

    bool SetTo(
        const char      * HistoryFileLine,
        DBMSrvMsg_Error & error);

    tcn00_Error AddPart(DBMSrvBHist_Part * Part);

    static const char * DBMSrvBHist_Backup_LBLDAT;
    static const char * DBMSrvBHist_Backup_ACTSAV;

    int IsAHistLost() const;
    int IsABackup() const;
    int IsARestore() const;

    int IsAData() const;          //backup or restore //TODO check usage
    int IsADataBackup() const;
    int IsADataRestore() const;

    int IsALog() const;           //backup or restore //TODO check usage
    int IsALogBackup() const;

    int IsAPages() const;
    int IsAPagesBackup() const;
    int IsAPagesRestore() const;

    tcn00_Error IsAvailable(const char * DefaultDirectory, int & Available) const;

    int WasSuccessful() const;
    int WasSuccessfulForDB() const;

    int EqualsInLabelWith(DBMSrvBHist_Backup *o) const;
    int EqualsInDBStamp1With(DBMSrvBHist_Backup *o) const;
    int EqualsInDBStamp2With(DBMSrvBHist_Backup *o) const;

    int HasKey(const char * KeyToCompare) const;
    int HasSameKeyAs(const DBMSrvBHist_Backup & OtherBackup) const;
    int HasKeyAndLabel(const char * KeyToCompare, const char * LabelToCompare) const;
    int HasInternalBackupID(const char * IBID) const;
    int HasTimeStamp(const char * TimeStampToCompare) const;
    int MatchesLabel(const char * StartOfLabelToCompare) const;   //partial match without the label number
    int MatchesAction(const char * StartOfActionToCompare) const; //partial match without db state

    int HasSameLabelAndTimeStampsAs(const DBMSrvBHist_Backup & OtherBackup) const;
    int HasHigherLogPageAs(const DBMSrvBHist_Backup & OtherBackup) const;
    int HasContentOlderThan(const char * Date, const char * Time) const;

    const char * GiveKey() const;
    const char * GiveLabel() const;
    const char * GiveBackupType() const;
    const char * GiveTimeStamp() const;
    const char * GiveStartDate() const;
    const char * GiveDBStamp1() const;
    const char * GiveMediumName() const;
    const char * GiveVolumes() const;
    teo00_Uint8  GiveStartLogPage() const;
    const char * GiveStartLogPageAsString() const;
    teo00_Uint8  GiveStopLogPage() const;
    teo00_Uint8  GiveMaxLogPage() const;
    const char * GivePagesTransfered() const;

    char * ConstructEBIDOfFirstAvailableCopy() const;
    char * constructInternalBackupID(DBMSrvMsg_Error & err) const;
    tcn00_Error ConstructMediumOfFirstAvailableCopy(tcn003_Medium *& Medium, tcn003_MediaGroup *& MediaGroup) const;

    bool PrintBackupInto(char                                *& Buffer,
                         size_t                               & MaxUsableBufferSize,
                         Tools_List<Column>                   & Columns,
                         const DBMSrvBHist_BackupColumnWidths & ColumnWidths);

    bool PrintMediaAndEBIDsInto(char                                   *& Buffer,
                                size_t                                  & MaxUsableBufferSize,
                                bool                                      WithMedia,
                                bool                                      WithEBIDs,
                                const DBMSrvBHist_PartMediaColumnWidths & ColumnWidthsMedia);

    void EnlargeColumnWidths(Tools_List<Column>                & Columns,
                             DBMSrvBHist_BackupColumnWidths    & ColumnWidthsBackup,
                             DBMSrvBHist_PartMediaColumnWidths & ColumnWidthsMedia);

    void MarkUsedExternalBackupTools(DBMSrvBTools_ToolEnum * BackupToolType);

    int MatchesFileNameElseReduce(const char * AbsoluteName, const char * DefaultDirectory);
    int ContainsFileBackup(const char                 * BaseName,
                           const char                 * labelFromBackup,
                           const tcn36_DateTimeString & backupStartDateTime);

    tcn00_Error AdjustReturnCode();

  private:
    Tools_List<DBMSrvBHist_Copy *> Copies;

    teo00_Uint8 StartLogPage;
    teo00_Uint8 StopLogPage;
    int         ReturnCode;
    int         ReturnCodeDBKernel;
    char *      TimeStamp;

    char * ColumnArray[NoColumn];

    int HasCopyFromMediaFile;
};

//-----------------------------------------------------------------------------
// class DBMSrvBHist_BackupColumnWidths
//-----------------------------------------------------------------------------

class DBMSrvBHist_BackupColumnWidths
{
  public:
    DBMSrvBHist_BackupColumnWidths();

    void   SetColumnWidthsToZero();
    void   SetColumnWidthsToDefaults();

    void   EnlargeColumnWidthTo(DBMSrvBHist_Backup::Column Column, size_t NewWidth);
    size_t GiveWidthOf(DBMSrvBHist_Backup::Column Column) const;

  private:
    size_t ColumnWidth[DBMSrvBHist_Backup::NoColumn];
};

#endif
