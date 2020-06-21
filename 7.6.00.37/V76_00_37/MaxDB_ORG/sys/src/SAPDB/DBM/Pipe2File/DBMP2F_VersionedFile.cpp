/*******************************************************************************
  module:       DBMP2F_VersionedFile.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements a versioned files for pipe2file
  last change:  2002-06-25 08:20
  version:      7.4.3
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG



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




*******************************************************************************/

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <stdio.h>

#include "SAPDB/DBM/Pipe2File/DBMP2F_VersionedFile.hpp"
#include "SAPDBCommon/SAPDB_string.h"


// -----------------------------------------------------------------------------
// member functions DBMP2F_VersionedFile
// -----------------------------------------------------------------------------

DBMP2F_VersionedFile::DBMP2F_VersionedFile(const char *                              DeviceName,
                                           DBMP2F_DeviceDirection::DeviceDirection   TheDirection,
                                           teo00_Uint8                               MaxSize,
                                           DBMP2F_Event                            * EndOfSubDeviceEvent)
:DBMP2F_NamedDevice(DeviceName, TheDirection),
 FileIsOpen(0),
 File(0),
 CurrentSuffix(0),
 CurrentName(0),
 Suffix(0),
 MaximalSize(MaxSize),
 CurrentSize(0),
 IsLastFile(0),
 IsGoodFlag(1)
{
    if(cn36_StrAlloc(CurrentName, strlen(DeviceName)+1+30))
    {
        SAPDB_strcpy(CurrentName, DeviceName);
        Suffix=CurrentName+strlen(CurrentName);
        *(Suffix++)='.';
    }

    if(0!=EndOfSubDeviceEvent && !EventChecker.AddEventWithReactions(EndOfSubDeviceEvent))
    {
        IsGoodFlag=0;
        delete EndOfSubDeviceEvent; //last chance
    }
}

DBMP2F_VersionedFile::~DBMP2F_VersionedFile()
{
    if(FileIsOpen)
        Close();

    if(0!=File)
        delete File;

    cn36_StrDealloc(CurrentName);
}

int DBMP2F_VersionedFile::Open()
{
    IsLastFile=0;       //with every real open we start at the first file <name>.0
    CurrentSuffix=0;

    return OpenNextFile();
}

int DBMP2F_VersionedFile::TransferData(DBMP2F_TransferBuffer & Buffer,
                                       unsigned int            BytesToTransfer,
                                       unsigned int          & BytesTransfered)
{
    int rc=1;

    BytesTransfered=0;

    while(rc && 0<BytesToTransfer && !IsLastFile)
    {
        if(!FileIsOpen)
            rc=OpenNextFile();

        if(rc)
        {
            long BytesToTransferAsLong=BytesToTransfer;
            long BytesTransferedAsLong=0;

            if(Direction==DBMP2F_DeviceDirection::Write)
            {
                if(CurrentSize+BytesToTransferAsLong>MaximalSize)
                    BytesToTransferAsLong=long(MaximalSize-CurrentSize);
    
                rc=File->Write(Buffer.GiveStartOfUsedSpace(), BytesToTransferAsLong, BytesTransferedAsLong);
            }
            else
                rc=File->Read(Buffer.GiveStartOfFreeSpace(), BytesToTransferAsLong, BytesTransferedAsLong);
 
            if(rc)
            {
                ChangeUsedBufferSpace(Buffer, (unsigned int)BytesTransferedAsLong);

                BytesTransfered+=BytesTransferedAsLong;    // 0<=BytesTransferedAsLong<=BytesToTransfer should hold
                BytesToTransfer-=BytesTransferedAsLong;

                CurrentSize+=BytesTransferedAsLong;
            }

            if(Direction==DBMP2F_DeviceDirection::Write)
            {
                if(CurrentSize>=MaximalSize) //close the file if maximalSize is reached during a write
                    rc=Close();
            }
            else    //on read
            {
                if(0==BytesTransferedAsLong)
                    rc=Close();
            }
        }
    } 

    return rc;
}

int DBMP2F_VersionedFile::IsGood()
{
    return (IsGoodFlag && 0!=CurrentName && 0!=Suffix && (0==File || File->IsGood()));
}

int DBMP2F_VersionedFile::OpenNextFile()
{
    int rc=1;

    if(FileIsOpen)
        rc=Close();

    if(rc)
    {
        if(cn36_UInt8ToStrN(Suffix, 30, CurrentSuffix))
        {
            File=new tni34_AVirtualFile(CurrentName, Direction==DBMP2F_DeviceDirection::Read?tni34_AVirtualFile::FromDisk:tni34_AVirtualFile::ToDiskTruncate, tni34_AVirtualFile::DoNotOpenNow);

            if(0==File)
            {
                rc=0;
                fprintf(stderr, "Out of memory.\n");
            }
            else
                if(File->IsGood())
                {
                    if(File->Open())
                        FileIsOpen=1;
                    else
                        fprintf(stderr, "Could not open file %s.\n", (const char *)CurrentName);
                }
                else
                    rc=0;
        }
        else
        {
            rc=0;
            fprintf(stderr, "Internal error.\n"); //this can only be reached if 30 digits are not enough for the suffix :o 
        }
    }

    return rc;
}

int DBMP2F_VersionedFile::Close()
{
    int rc=1;

    if(0!=File)
    {
        if(!File->Close())
            rc=0;

        FileIsOpen=0;

        delete File;
        File=0;

        CurrentSuffix++;
        CurrentSize=0;

        if(!EventChecker.ReactionsFor(DBMP2F_Event::AEndOfSubDevice, CurrentName, IsLastFile))
            rc=0;
    }

    return rc;
}
