/*******************************************************************************
  module:       DBMP2F_File.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class for handling file devices
  last change:  2002-06-25 00:54
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

#include "DBM/Pipe2File/DBMP2F_NamedDevice.hpp"

#include "hni34.h"


// -----------------------------------------------------------------------------
// class DBMP2F_File
// -----------------------------------------------------------------------------

class DBMP2F_File: public DBMP2F_NamedDevice
{
  public:
    DBMP2F_File(const char *                            DeviceName,
                DBMP2F_DeviceDirection::DeviceDirection TheDirection);

    int Open();
    int TransferData(DBMP2F_TransferBuffer & Buffer,
                     unsigned int            BytesToTransfer,
                     unsigned int          & BytesTransfered);
    int Close();

    int IsGood();

  private:
    tni34_AVirtualFile File;
};
