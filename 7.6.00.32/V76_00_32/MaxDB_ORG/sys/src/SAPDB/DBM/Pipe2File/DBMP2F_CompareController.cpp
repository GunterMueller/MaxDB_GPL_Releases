/*******************************************************************************
  module:       DBMP2F_CompareController.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class for comparing data delivered by two data sources
  last change:  2002-06-24 23:51
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
#include "SAPDB/DBM/Pipe2File/DBMP2F_CompareController.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_CompareController
// -----------------------------------------------------------------------------

DBMP2F_CompareController::DBMP2F_CompareController(unsigned int    BufferSize,
                                                   DBMP2F_Device * SourceDevice1,
                                                   DBMP2F_Device * SourceDevice2,
                                                   int             DelayTime,
                                                   teo00_Uint8     DelayPoint,
                                                   DBMP2F_Event  * DelayEvent,
                                                   int             NumberOfWaits,
                                                   teo00_Uint8     WaitAfterBytes,
                                                   DBMP2F_Event  * WaitEvent,
                                                   DBMP2F_Event  * ShowSpeedEvent,
                                                   DBMP2F_Event  * SourcesDifferEvent)
  :Buffer1(BufferSize),
   Buffer2(BufferSize),
   Source1(SourceDevice1, DelayTime, DelayPoint, DelayEvent, NumberOfWaits, WaitAfterBytes, WaitEvent),
   Source2(SourceDevice2, 0, 0, 0, 0, 0, 0, ShowSpeedEvent),
   IsGoodFlag(1)
{
    if(0!=SourcesDifferEvent && !EventChecker.AddEventWithReactions(SourcesDifferEvent))
    {
        IsGoodFlag=0;
        delete SourcesDifferEvent;
    }
}

int DBMP2F_CompareController::DoTransfer(void)
{
    int rc=1,
        OpenedDataSource1=0,
        OpenedDataSource2=0;

    //open the data sources
    if(Source1.Open())
        OpenedDataSource1=1;
    else
    {
        rc=0;
        fprintf(stderr, "Could not open first data source.\n");
    }

    if(rc)
    {
        if(Source2.Open())
            OpenedDataSource2=1;
        else
        {
            rc=0;
            fprintf(stderr, "Could not open second data source.\n");
        }
    }

    while(1==rc && !Source1.ReachedEnd() && !Source2.ReachedEnd()) //while all ok and both data source didn't end
    {
        if(0==Buffer1.GiveUsedLength() && !Source1.ProcessData(Buffer1))    //if nothing is in Buffer1, read something
            rc=0;

        if(rc && 0==Buffer2.GiveUsedLength())   //0==... should hold anyway
        {
            if(Source1.ReachedEnd())
            {
                if(!Source2.ProcessData(Buffer2))   //if we have reached the end of Source1 try to read as much as possible from Source2 (is Source2 longer then Source1?)
                    rc=0;
            }
            else
            {
                if(!Source2.ProcessData(Buffer2, Buffer1.GiveUsedLength())) //otherwise limit the read on Source2 to prevent overtaking of Source2
                    rc=0;
            }
        }

        if(rc && !Comparer.ProcessData(Buffer1, Buffer2, Buffer2.GiveUsedLength()))   //Buffer2.GiveUsedLength()<=Buffer1.GiveUsedLength() -> Buffer2 is empty after that compare
            rc=2;   //sources differ in at least one byte
    }

    if(rc)  //no real error happend
    {
        if(Source1.ReachedEnd() && !Source2.ReachedEnd())   //test if both sources ended at the same time
            rc=2;
        else
            if(Source2.ReachedEnd() && !Source1.ReachedEnd())
                rc=2;
    }

    if(2==rc)
        EventChecker.ReactionsFor(DBMP2F_Event::AMessage);

    //close both sources
    if(OpenedDataSource1 && !Source1.Close())
    {
        rc=0;
        fprintf(stderr, "Could not close first data source.\n");
    }

    if(OpenedDataSource2 && !Source2.Close())
    {
        rc=0;
        fprintf(stderr, "Could not close second data source.\n");
    }

    return rc;
}

int DBMP2F_CompareController::IsGood()
{
    return (Buffer1.IsGood() && Buffer2.IsGood() && Source1.IsGood() && IsGoodFlag);
}
