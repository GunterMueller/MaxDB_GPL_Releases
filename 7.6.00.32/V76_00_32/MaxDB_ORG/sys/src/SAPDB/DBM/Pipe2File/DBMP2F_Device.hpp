/*******************************************************************************
  module:       DBMP2F_Device.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  base class for devices
  last change:  2002-06-25 00:38
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

#if !defined(_DBMP2F_DEVICE_HPP_)
#define _DBMP2F_DEVICE_HPP_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Pipe2File/DBMP2F_DeviceDirection.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferBuffer.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_EventChecker.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_Device
// -----------------------------------------------------------------------------

class DBMP2F_Device
{
  public:
    DBMP2F_Device(DBMP2F_DeviceDirection::DeviceDirection TheDirection);
    virtual ~DBMP2F_Device();

    virtual int Open()=0;
    virtual int TransferData(DBMP2F_TransferBuffer & Buffer,
                             unsigned int   BytesToTransfer,
                             unsigned int & BytesTransfered)=0;
    virtual int Close()=0;

    virtual int IsGood()=0;

  protected:
    DBMP2F_DeviceDirection Direction;
    DBMP2F_EventChecker    EventChecker;

    void inline ChangeUsedBufferSpace(DBMP2F_TransferBuffer & Buffer, unsigned int BytesTransfered)
    {
        if(Direction==DBMP2F_DeviceDirection::Read)
            Buffer.AddUsedSpace(BytesTransfered);
        else
            Buffer.FreeUsedSpace(BytesTransfered);
    }
};

#endif
