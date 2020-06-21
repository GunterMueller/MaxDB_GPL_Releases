/*******************************************************************************
  module:       DBMP2F_TransferBuffer.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements the buffers for pipe2file
  last change:  2002-06-25 08:14
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferBuffer.hpp"

#include <string.h>


// -----------------------------------------------------------------------------
// member functions DBMP2F_TransferBuffer
// -----------------------------------------------------------------------------

DBMP2F_TransferBuffer::DBMP2F_TransferBuffer(unsigned int Length)
    :Buffer(0),
     BufferLength(0),
     UsedSpace(0),
     UsedSpaceLength(0)
{
    if(0<Length)
    {
        Buffer=new char [2*Length];

        if(0!=Buffer)
        {
            BufferLength=Length;
            UsedSpace=Buffer;
        }
    }
}

DBMP2F_TransferBuffer::~DBMP2F_TransferBuffer()
{
    if(0!=Buffer)
        delete [] Buffer;
}

int DBMP2F_TransferBuffer::IsGood()
{
    int rc=1;

    if(BufferLength>0 && 0==Buffer)
        rc=0;

    return rc;
}

unsigned int DBMP2F_TransferBuffer::GiveLength()
{
    return BufferLength;
}

unsigned int DBMP2F_TransferBuffer::GiveUsedLength()
{
    return UsedSpaceLength;
}

char * DBMP2F_TransferBuffer::GiveStartOfUsedSpace()
{
    return UsedSpace;
}

char * DBMP2F_TransferBuffer::GiveStartOfFreeSpace()
{
    return UsedSpace+UsedSpaceLength;
}

void DBMP2F_TransferBuffer::AddUsedSpace(unsigned int BytesNewlyUsed)
{
    UsedSpaceLength+=BytesNewlyUsed;    //UsedSpace did not change
}

void DBMP2F_TransferBuffer::FreeUsedSpace(unsigned int BytesNotLongerUsed)
{
    if(UsedSpaceLength>BytesNotLongerUsed)
    {
        UsedSpace+=BytesNotLongerUsed;
        UsedSpaceLength-=BytesNotLongerUsed;
    }
    else
    {
        UsedSpace=Buffer;
        UsedSpaceLength=0;
    }
}

unsigned int DBMP2F_TransferBuffer::Compare(DBMP2F_TransferBuffer & SecondBuffer)
{
    unsigned int DifferingBytes=0;
    unsigned int MinLength=UsedSpaceLength;

    if(MinLength>SecondBuffer.UsedSpaceLength)
        MinLength=SecondBuffer.UsedSpaceLength;

    if(MinLength<UsedSpaceLength)
        DifferingBytes+=UsedSpaceLength-MinLength;
    else
        if(MinLength<SecondBuffer.UsedSpaceLength)
            DifferingBytes+=SecondBuffer.UsedSpaceLength-MinLength;

    if(0!=memcmp(UsedSpace, SecondBuffer.UsedSpace, MinLength))   //do the two buffers differ in places used in both?
    {
        unsigned int i;                                     //if so, count the differing bytes

        for(i=0; i<MinLength; i++)
            if(UsedSpace[i]!=SecondBuffer.UsedSpace[i])
                DifferingBytes++;
    }

    return DifferingBytes;
}

unsigned int DBMP2F_TransferBuffer::Compare(DBMP2F_TransferBuffer & SecondBuffer, unsigned int MaxLength)
{
    unsigned int DifferingBytes=0;
    unsigned int MinLength;
    unsigned int LimitedLengthBuf1=UsedSpaceLength;
    unsigned int LimitedLengthBuf2=SecondBuffer.UsedSpaceLength;

    if(MaxLength<LimitedLengthBuf1)
        LimitedLengthBuf1=MaxLength;    //don't use more than MaxLength this time

    if(MaxLength<LimitedLengthBuf2)
        LimitedLengthBuf2=MaxLength;    //don't use more than MaxLength this time

    //determine min(LimitedLengthBuf1, LimitedLengthBuf2)
    MinLength=LimitedLengthBuf1;

    if(MinLength>LimitedLengthBuf2)
        MinLength=LimitedLengthBuf2;

    if(MinLength<LimitedLengthBuf1)
        DifferingBytes+=LimitedLengthBuf1-MinLength;
    else
        if(MinLength<LimitedLengthBuf2)
            DifferingBytes+=LimitedLengthBuf2-MinLength;

    if(0!=memcmp(UsedSpace, SecondBuffer.UsedSpace, MinLength))   //do the two buffers differ in places used in both?
    {
        unsigned int i;                                     //if so, count the differing bytes

        for(i=0; i<MinLength; i++)
            if(UsedSpace[i]!=SecondBuffer.UsedSpace[i])
                DifferingBytes++;
    }

    return DifferingBytes;
}
