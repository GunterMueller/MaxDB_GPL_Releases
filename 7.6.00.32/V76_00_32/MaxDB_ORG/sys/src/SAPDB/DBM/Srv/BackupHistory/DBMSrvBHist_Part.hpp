/*!
    \file    DBMSrvBHist_Part.hpp
    \author  TiloH
    \ingroup backup history handling by the DBMServer
    \brief   a class for handling one part of a copy of a backup

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

#if !defined(DBMSrvBHist_Part_HPP)
#define DBMSrvBHist_Part_HPP


//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "SAPDB/DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "SAPDB/DBM/Srv/BackupHistory/DBMSrvBHist_LineProducer.hpp"
#include "gcn00.h"
#include "gcn003.h"
#include "SAPDB/DBM/Srv/BackupTools/DBMSrvBTools_ToolEnum.hpp"


//-----------------------------------------------------------------------------
// forward declarations
//-----------------------------------------------------------------------------

class DBMSrvBHist_PartMediaColumnWidths;


//-----------------------------------------------------------------------------
// class DBMSrvBHist_Part
//-----------------------------------------------------------------------------

class DBMSrvBHist_Part: public DBMSrvBHist_LineProducer
{
  public:
    enum EBIDStatus {EBIDStatusUnknown,
                     EBIDStatusNone,
                     EBIDStatusAvailable,
                     EBIDStatusUnavailable};

    enum MediaColumn{MediumNameColumn      =0,
                     MediumTypeColumn      =1,
                     MediumOverwriteColumn =2,
                     MediumSizeColumn      =3,
                     MediumBlockSizeColumn =4,
                     MediumKindColumn      =5,
                     MediumLocationColumn  =6,
                     BackupToolStringColumn=7,
                     NoColumn              =8};

    enum PartOrigin{PartFromMediaFile, PartFromEBIDFile, PartFromExternalBackupTool};

    DBMSrvBHist_Part(PartOrigin TheOrigin);
    ~DBMSrvBHist_Part();

    bool SetToMedium(
        const char      * MediaHistoryFileLine,
        DBMSrvMsg_Error & error);
    bool SetToEBID(
        const char      * ExternalHistoryFileLine,
        DBMSrvMsg_Error & error);

    tcn00_Error SetToEBIDInfo(DBMSrvBHist_Part::EBIDStatus   NewStatus,
                              const char                   * NewKey,
                              size_t                         NewKeyLength,
                              const char                   * NewLabel,
                              size_t                         NewLabelLength,
                              const char                   * NewEBID,
                              size_t                         NewEBIDLength,
                              const char                   * NewType,
                              const char                   * NewDateTime);
    tcn00_Error Check(DBMSrvBHist_Part * OtherPart, int & Matched);
    void SetEBIDStatus(const DBMSrvBHist_Part &OtherPart);
    void SetEBIDStatusToUnavailable();
    void SetAdded();
    void UnsetAdded();

    int IsALog() const;
    int IsAReplace() const;
    int IsNewFormat() const;
    int IsAdded() const;

    int HasSameKeyAndLabelAs(const DBMSrvBHist_Part &OtherPart) const;
    int HasSameEBIDAs(const DBMSrvBHist_Part &OtherPart) const;

    const char *          GiveKey() const;
    const char *          GiveLabel() const;
    DBMSrvBTools_ToolEnum GiveUsedBackupTool() const;
    int                   GivePartNumber() const;
    int                   GiveDBMReturnCode() const;
    const char *          GiveDBMReturnText() const;
    const char *          GiveEBID() const;
    const char *          GiveMediumName() const;
    const char *          getBackupDateTime() const;
    const char *          getBackupType() const;
    const char *          getMediumBlockSize() const;
    const char *          getMediumKind() const;
    const char *          getMediumLocation() const;
    const char *          getMediumOverwrite() const;
    const char *          getMediumSize() const;
    const char *          getMediumType() const;

    bool PrintMediumInto(char                                    *& Buffer,
                         size_t                                   & MaxUsableBufferSize,
                         const DBMSrvBHist_PartMediaColumnWidths  & ColumnWidths);
    bool PrintEBIDInto(char *& Buffer, size_t & MaxUsableBufferSize, int WithExtendedInfo);

    void EnlargeColumnWidths(DBMSrvBHist_PartMediaColumnWidths & ColumnWidths);

    int MatchesFileName(const char * AbsoluteName, const char * DefaultDirectory);
    int ContainsFileBackup(const char * BaseName);

    int         IsAvailable() const;
    tcn00_Error IsAvailableExtended(const char * DefaultDirectory, int & Available);
    int         IsAvailableExtended() const;
    tcn00_Error ConstructMedium(tcn003_Medium *& Medium, const char * TheBackupType) const;

    PartOrigin GivePartOrigin() const;

  private:

    char * Key;
    char * Label;
    char * MediumName;      // from dbm.mdf or dbm.ebf
    char * MediumType;      //         -"-
    char * MediumOverwrite; //         -"-
    char * MediumSize;      //         -"-
    char * MediumBlockSize; //         -"-
    char * MediumKind;      //         -"-
    char * MediumLocation;  //         -"-


    char * EBID;
                            // extra information needed only for restore without history
    char * BackupType;      // type of the save like ACTION in dbm.knl
    char * BackupDateTime;  // date and time in a format like STAMP1 or STAMP2 in dbm.knl

    char * BackupToolString;// used backup tool as contained in the new dbm.ebf format
    char * DBMReturnText;
    int    DBMReturnCode;   // result of a backup as contained in the new dbm.ebf format
    int    PartNumber;

    EBIDStatus Status;

    int    PartAdded;

    int    ExtendedAvailableFlag;

    PartOrigin Origin;
};

//-----------------------------------------------------------------------------
// class DBMSrvBHist_PartMediaColumnWidths
//-----------------------------------------------------------------------------

class DBMSrvBHist_PartMediaColumnWidths
{
  public:
    DBMSrvBHist_PartMediaColumnWidths();

    void   SetColumnWidthsToZero();
    void   SetColumnWidthsToDefaults();

    void   EnlargeColumnWidthTo(DBMSrvBHist_Part::MediaColumn Column, size_t NewWidth);
    size_t GiveWidthOf(DBMSrvBHist_Part::MediaColumn Column) const;

  private:
    size_t ColumnWidth[DBMSrvBHist_Part::NoColumn];
};

#endif
