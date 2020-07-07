/*******************************************************************************
  module:       DBMP2F_Null.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class implementing a null device
  last change:  2002-06-25 01:06
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_Device.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_Null
// -----------------------------------------------------------------------------

class DBMP2F_Null: public DBMP2F_Device
{
  public:
    DBMP2F_Null(DBMP2F_DeviceDirection::DeviceDirection TheDirection=DBMP2F_DeviceDirection::Write);

    int Open();
    int TransferData(DBMP2F_TransferBuffer & Buffer,
                     unsigned int            BytesToTransfer,
                     unsigned int          & BytesTransfered);
    int Close();

    int IsGood();
};
