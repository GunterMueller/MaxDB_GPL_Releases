/*******************************************************************************
  module:       DBMP2F_VersionedFile.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements a versioned files for pipe2file
  last change:  2002-06-25 08:21
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_NamedDevice.hpp"

#include "hcn36.h"
#include "hni34.h"


// -----------------------------------------------------------------------------
// class DBMP2F_VersionedFile
// -----------------------------------------------------------------------------

class DBMP2F_VersionedFile: public DBMP2F_NamedDevice
{
  public:
    DBMP2F_VersionedFile(const char *                              DeviceName,
                         DBMP2F_DeviceDirection::DeviceDirection   TheDirection,
                         teo00_Uint8                               MaxSize,
                         DBMP2F_Event                            * EndOfSubDeviceEvent);
    ~DBMP2F_VersionedFile();

    int Open();
    int TransferData(DBMP2F_TransferBuffer &         Buffer,//char *         Buffer,
                     unsigned int   BytesToTransfer,
                     unsigned int & BytesTransfered);
    int Close();

    int IsGood();

  private:
    int OpenNextFile();

    int                  FileIsOpen;
    tni34_AVirtualFile * File;
    char               * CurrentName;
    char               * Suffix;
    teo00_Uint8          CurrentSuffix;
    teo00_Uint8          MaximalSize; //used only during creation
    teo00_Uint8          CurrentSize;
    int                  IsLastFile;

    int                  IsGoodFlag;
};
