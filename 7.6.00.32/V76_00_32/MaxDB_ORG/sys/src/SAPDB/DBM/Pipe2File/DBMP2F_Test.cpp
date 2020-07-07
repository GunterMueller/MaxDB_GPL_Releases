/*******************************************************************************
  module:       DBMP2F_Test.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements a dummy data generator for pipe2file
  last change:  2002-06-25 08:10
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_Test.hpp"
#include "SAPDBCommon/SAPDB_string.h"


// -----------------------------------------------------------------------------
// member functions DBMP2F_Test
// -----------------------------------------------------------------------------

int DBMP2F_Test::TestBufferSize=1024;

DBMP2F_Test::DBMP2F_Test(DBMP2F_DeviceDirection::DeviceDirection TheDirection,
                         teo00_Uint8                             FileSizeAsInt)
    :DBMP2F_Device(TheDirection),
     CompleteTestSize(FileSizeAsInt),
     TestSize(FileSizeAsInt),
     TestData(0),
     CurrentPos(0)
{
    TestData=new char [TestBufferSize];

    if(0!=TestData)
    {
        int i;
        
        for(i=0; i<TestBufferSize; i++)
            TestData[i]=char(i);
    }
    else
        fprintf(stderr, "Out of memory.\n");
}

DBMP2F_Test::~DBMP2F_Test()
{
    if(0!=TestData)
        delete [] TestData;
}

int DBMP2F_Test::Open()
{
    int rc=1;

    if(0==TestData)
        rc=0;
    else
    {
        CurrentPos=0;
        TestSize=CompleteTestSize;
    }

    return rc;
}

int DBMP2F_Test::TransferData(DBMP2F_TransferBuffer & Buffer,
                              unsigned int            BytesToTransfer,
                              unsigned int          & BytesTransfered)
{
    int rc;

    if(Direction==DBMP2F_DeviceDirection::Read)
    {
        rc=1;
        BytesTransfered=0;

        while(TestSize>0 && 0<BytesToTransfer)  //as long as test is not over and something should be read/constructed
        {
            unsigned int BytesToCopy=TestBufferSize-CurrentPos;

            if(BytesToCopy>TestSize)
                BytesToCopy=(unsigned int)TestSize; //cast ok, because of if(...)

            if(BytesToCopy>BytesToTransfer)
                BytesToCopy=BytesToTransfer;

            SAPDB_memcpy(Buffer.GiveStartOfFreeSpace()+BytesTransfered, TestData+CurrentPos, BytesToCopy);

            BytesTransfered+=BytesToCopy;

            CurrentPos+=BytesToCopy;
            CurrentPos%=TestBufferSize;

            BytesToTransfer-=BytesToCopy;

            TestSize-=BytesToCopy;
        }

        ChangeUsedBufferSpace(Buffer, BytesTransfered);
    }
    else
        if(Direction==DBMP2F_DeviceDirection::Write)
        {
            rc=0;
            BytesTransfered=0;
        }

    return rc;
}

int DBMP2F_Test::Close()
{
    return 1;
}

int DBMP2F_Test::IsGood()
{
    return (0!=TestData);
}
