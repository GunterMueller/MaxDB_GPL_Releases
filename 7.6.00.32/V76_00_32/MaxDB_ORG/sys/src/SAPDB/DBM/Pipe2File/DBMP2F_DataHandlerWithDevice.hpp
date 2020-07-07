/*******************************************************************************
  module:       DBMP2F_DataHandlerWithDevice.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class for handling a device
  last change:  2002-06-25 00:09
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

#if !defined(_DBMP2F_DataHandlerWithDevice_hpp_)
#define _DBMP2F_DataHandlerWithDevice_hpp_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Pipe2File/DBMP2F_DataHandler.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Device.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferBuffer.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_EventChecker.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Event.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_DataHandlerWithDevice
// -----------------------------------------------------------------------------

class DBMP2F_DataHandlerWithDevice: public DBMP2F_DataHandler
{
  public:
    DBMP2F_DataHandlerWithDevice(DBMP2F_Device * InitialDevice,
                                 int             TheDelayTime=0,
                                 teo00_Uint8     TheDelayPoint=0,
                                 DBMP2F_Event  * DelayEvent=0,
                                 int             TheNumberOfWaits=0,
                                 teo00_Uint8     WaitAfterBytes=0,
                                 DBMP2F_Event  * WaitEvent=0,
                                 DBMP2F_Event  * ShowSpeedEvent=0);

    int IsGood();
    int Open();
    int Close();
    int ProcessData(DBMP2F_TransferBuffer & Buffer, unsigned int MaximalTransferLength=0);

  protected:
    ~DBMP2F_DataHandlerWithDevice();

    virtual unsigned int BytesToTransfer(DBMP2F_TransferBuffer & Buffer)=0;
    virtual void SetEndOfDevice();

  private:
    DBMP2F_Device      * Device;

    int                  DelayTime;
    teo00_Uint8          DelayPoint;

    int                  NumberOfWaits;
    teo00_Uint8          WaitPoint;
    teo00_Uint8          WaitAfter;

    int                  ShowSpeed;
    time_t               StartTime;

    DBMP2F_EventChecker  EventChecker;

    int                  IsGoodFlag;

    void CheckForDelayReaction();
    void CheckForWaitReaction();
};

#endif
