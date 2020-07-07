/*******************************************************************************
  module:       DBMP2F_TransferController.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  a class for copying data
  last change:  2002-06-25 08:16
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferController.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_TransferController
// -----------------------------------------------------------------------------

DBMP2F_TransferController::DBMP2F_TransferController(unsigned int    BufferSize,
                                                     DBMP2F_Device * SourceDevice,
                                                     DBMP2F_Device * SinkDevice,
                                                     int             DelayTime,
                                                     teo00_Uint8     DelayPoint,
                                                     DBMP2F_Event  * DelayEvent,
                                                     int             NumberOfWaits,
                                                     teo00_Uint8     WaitAfterBytes,
                                                     DBMP2F_Event  * WaitEvent,
                                                     DBMP2F_Event  * ShowSpeedEvent)
  :Buffer(BufferSize),
   Source(SourceDevice, DelayTime, DelayPoint, DelayEvent, NumberOfWaits, WaitAfterBytes, WaitEvent),
   Sink(SinkDevice, ShowSpeedEvent)
{
}

DBMP2F_TransferController::~DBMP2F_TransferController()
{
}

int DBMP2F_TransferController::DoTransfer(void)
{
    int rc=1,
        OpenedDataSource=0,
        OpenedDataSink=0;

    //open the data source and data sink
    if(Source.Open())
        OpenedDataSource=1;
    else
    {
        rc=0;
        fprintf(stderr, "Could not open data source.\n");
    }

    if(rc)
    {
        if(Sink.Open())
            OpenedDataSink=1;
        else
        {
            rc=0;
            fprintf(stderr, "Could not open data sink.\n");
        }
    }

    //copy the data from the source to the sink
    while(rc && !Source.ReachedEnd())
    {
        if(!Source.ProcessData(Buffer)) //reads data to Buffer
            rc=0;
        else
            if(!Sink.ProcessData(Buffer))   //writes data from Buffer
                rc=0;
    }

    //close source and sink
    if(OpenedDataSource && !Source.Close())
    {
        rc=0;
        fprintf(stderr, "Could not close data source.\n");
    }

    if(OpenedDataSink && !Sink.Close())
    {
        rc=0;
        fprintf(stderr, "Could not close data sink.\n");
    }

    return rc;
}

int DBMP2F_TransferController::IsGood()
{
    return Buffer.IsGood();
}
