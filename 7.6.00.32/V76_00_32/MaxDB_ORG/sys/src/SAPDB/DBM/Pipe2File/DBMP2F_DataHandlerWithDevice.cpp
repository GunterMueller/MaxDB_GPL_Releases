/*!
  \file     DBMP2F_DataHandlerWithDevice.cpp
  \author   TiloH
  \ingroup  pipe2file
  \brief    class for handling a device
  
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


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <stdio.h>
#include <time.h>

#include "SAPDB/DBM/Pipe2File/DBMP2F_DataHandlerWithDevice.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_DataHandlerWithDevice
// -----------------------------------------------------------------------------

DBMP2F_DataHandlerWithDevice::DBMP2F_DataHandlerWithDevice(DBMP2F_Device * InitialDevice,
                                                           int             TheDelayTime,
                                                           teo00_Uint8     TheDelayPoint,
                                                           DBMP2F_Event  * DelayEvent,
                                                           int             TheNumberOfWaits,
                                                           teo00_Uint8     WaitAfterBytes,
                                                           DBMP2F_Event  * WaitEvent,
                                                           DBMP2F_Event  * ShowSpeedEvent)
    :Device(InitialDevice),
     DelayTime(TheDelayTime),
     DelayPoint(TheDelayPoint),
     NumberOfWaits(TheNumberOfWaits),
     WaitPoint(WaitAfterBytes),
     WaitAfter(WaitAfterBytes),
     ShowSpeed(0),
     StartTime(0),
     IsGoodFlag(1)
{
    if(0!=DelayEvent && !EventChecker.AddEventWithReactions(DelayEvent))
    {
        IsGoodFlag=0;
        delete DelayEvent;
    }

    if(0!=WaitEvent && !EventChecker.AddEventWithReactions(WaitEvent))
    {
        IsGoodFlag=0;
        delete DelayEvent;
    }

    if(0!=ShowSpeedEvent)
    {
        if(!EventChecker.AddEventWithReactions(ShowSpeedEvent))
        {
            IsGoodFlag=0;
            delete ShowSpeedEvent;
        }
        else
	    ShowSpeed=1;
    }
}

DBMP2F_DataHandlerWithDevice::~DBMP2F_DataHandlerWithDevice()
{
    if(0!=Device)
        delete Device;
}

int DBMP2F_DataHandlerWithDevice::IsGood()
{
    return IsGoodFlag;
}

int DBMP2F_DataHandlerWithDevice::Open()
{
    return Device->Open();
}

int DBMP2F_DataHandlerWithDevice::Close()
{
    return Device->Close();
}

int DBMP2F_DataHandlerWithDevice::ProcessData(DBMP2F_TransferBuffer & Buffer, unsigned int MaximalTransferLength)
{
    int rc=0;
    unsigned int BytesTransfered=0;
    unsigned int BytesToBeTransfered=BytesToTransfer(Buffer);
    int          DelayNow=0;
    int          WaitNow=0;

    if(ShowSpeed && 0==StartTime)
        StartTime=time(0);
    
    if(0!=MaximalTransferLength)
    {
        if(MaximalTransferLength>BytesToBeTransfered)
            fprintf(stderr, "Internal error.\n");
        else
            BytesToBeTransfered=MaximalTransferLength;
    }

    //check for a delay
    if(0!=DelayTime && TotalBytesTransfered+BytesToBeTransfered>=DelayPoint)   //is a delay necessary
    {
        if(DelayPoint==TotalBytesTransfered)    //if 0 bytes must be transfered before the delay
            CheckForDelayReaction();            //just wait and don't change BytesToBeTransfered
        else
        {
            BytesToBeTransfered=(unsigned int)(DelayPoint-TotalBytesTransfered);    //(unsigned int) is ok, because TotalBytesTransfered+BytesToBeTransfered>=DelayPoint
            DelayNow=1;                                                             //remember after the transfer, that we must do a delay
        }
    }

    //check for a wait
    while(0<NumberOfWaits && TotalBytesTransfered+BytesToBeTransfered>=WaitPoint && !WaitNow)  //is a delay necessary?
    {
        if(WaitPoint==TotalBytesTransfered) //if 0 bytes must be transfered before the wait
            CheckForWaitReaction();
        else
        {
            BytesToBeTransfered=(unsigned int)(WaitPoint-TotalBytesTransfered); //(unsigned int) is ok, because TotalBytesTransfered+BytesToBeTransfered>=WaitPoint
            WaitNow=1;                                                          //remember after the transfer, that we must do a wait
            DelayNow=0;                                                         //as we reduced BytesToBeTransfered again, we must forget about any delay this time
        }
    }

    rc=Device->TransferData(Buffer, BytesToBeTransfered, BytesTransfered);

    if(rc)
    {
        if(0==BytesTransfered)
            SetEndOfDevice();

        TotalBytesTransfered+=BytesTransfered;
    }

    //take care of a delay
    if(rc && DelayNow && TotalBytesTransfered>=DelayPoint)  //we have crossed the delaypoint with the last successful transfer
        CheckForDelayReaction();

    //take care of a wait
    if(rc && WaitNow && TotalBytesTransfered>=WaitPoint)
        CheckForWaitReaction();

    if(rc && ShowSpeed)
        EventChecker.ReactionsFor(DBMP2F_Event::AShowSpeed, TotalBytesTransfered, StartTime);

    return rc;
}

void DBMP2F_DataHandlerWithDevice::SetEndOfDevice()
{
}

void DBMP2F_DataHandlerWithDevice::CheckForDelayReaction()
{
    EventChecker.ReactionsFor(DBMP2F_Event::ADelay); //do whatever should be done on that delay
    DelayTime=0;                                    //prevent further delays
}

void DBMP2F_DataHandlerWithDevice::CheckForWaitReaction()
{
    EventChecker.ReactionsFor(DBMP2F_Event::AWait);
    NumberOfWaits--;
    WaitPoint+=WaitAfter;
}
