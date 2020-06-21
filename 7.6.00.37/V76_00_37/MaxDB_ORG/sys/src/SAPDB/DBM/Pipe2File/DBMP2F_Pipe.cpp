/*******************************************************************************
  module:       DBMP2F_Pipe.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class implementing a pipe device
  last change:  2002-06-25 01:12
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_Pipe.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_Pipe
// -----------------------------------------------------------------------------

DBMP2F_Pipe::DBMP2F_Pipe(const char *                            DeviceName,
                         DBMP2F_DeviceDirection::DeviceDirection TheDirection)
:Pipe(DeviceName, (TheDirection==DBMP2F_DeviceDirection::Read)?tni34_AVirtualPipe::In:tni34_AVirtualPipe::Out, tni34_AVirtualPipe::DoNotOpenNow),
 DBMP2F_NamedDevice(DeviceName, TheDirection)
{
}

int DBMP2F_Pipe::Open()
{
    return Pipe.Open();
}

int DBMP2F_Pipe::TransferData(DBMP2F_TransferBuffer &         Buffer,
                              unsigned int                    BytesToTransfer,
                              unsigned int                  & BytesTransfered)
{
    long BytesToTransferAsLong=BytesToTransfer;
    long BytesTransferedAsLong=0;
    int  rc=0;

    BytesTransfered=0;

    if(Direction==DBMP2F_DeviceDirection::Read)
        rc=Pipe.Read(Buffer.GiveStartOfFreeSpace(), BytesToTransferAsLong, BytesTransferedAsLong);
    else
        rc=Pipe.Write(Buffer.GiveStartOfUsedSpace(), BytesToTransferAsLong, BytesTransferedAsLong);

    if(rc)
    {
        BytesTransfered=BytesTransferedAsLong;    // 0<=BytesTransferedAsLong<=BytesToTransfer should hold
        ChangeUsedBufferSpace(Buffer, BytesTransfered);
    }

    return rc;
}

int DBMP2F_Pipe::Close()
{
    return Pipe.Close();
}

int DBMP2F_Pipe::IsGood()
{
    return (0!=Name && Pipe.IsGood());
}
