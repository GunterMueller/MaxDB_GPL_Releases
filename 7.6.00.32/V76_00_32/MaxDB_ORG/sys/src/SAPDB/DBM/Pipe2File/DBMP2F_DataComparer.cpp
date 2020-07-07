/*******************************************************************************
  module:       DBMP2F_DataComparer.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class for comparing two buffers
  last change:  2002-06-24 23:59
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_DataComparer.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_DataComparer
// -----------------------------------------------------------------------------

DBMP2F_DataComparer::DBMP2F_DataComparer()
 :DifferingBytes(0)
{
}

int DBMP2F_DataComparer::ProcessData(DBMP2F_TransferBuffer & Buffer1, DBMP2F_TransferBuffer & Buffer2, unsigned int MaxLength)
{
    DifferingBytes+=Buffer1.Compare(Buffer2, MaxLength);

    Buffer1.FreeUsedSpace(MaxLength);
    Buffer2.FreeUsedSpace(MaxLength);

    return (0==DifferingBytes);
}

unsigned int DBMP2F_DataComparer::BytesToTransfer(DBMP2F_TransferBuffer & Buffer)
{
    return Buffer.GiveUsedLength();
}

char * DBMP2F_DataComparer::TransferPosInBuffer(DBMP2F_TransferBuffer & Buffer)
{
    return Buffer.GiveStartOfUsedSpace();
}
