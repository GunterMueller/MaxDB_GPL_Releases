/*******************************************************************************
  module:       DBMP2F_TransferController.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  a class for copying data
  last change:  2002-06-25 08:18
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


#if !defined(_DBMP2F_TransferController_hpp_)
#define _DBMP2F_TransferController_hpp_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Pipe2File/DBMP2F_Controller.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferBuffer.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_DataSource.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_DataSink.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_TransferController
// -----------------------------------------------------------------------------

class DBMP2F_TransferController: public DBMP2F_Controller
{
  public:
    DBMP2F_TransferController(unsigned int    BufferSize,
                              DBMP2F_Device * SourceDevice,
                              DBMP2F_Device * SinkDevice,
                              int             DelayTime,
                              teo00_Uint8     DelayPoint,
                              DBMP2F_Event  * DelayEvent,
                              int             NumberOfWaits,
                              teo00_Uint8     WaitAfterBytes,
                              DBMP2F_Event  * WaitEvent,
                              DBMP2F_Event  * ShowSpeedEvent);
    ~DBMP2F_TransferController();

    int DoTransfer(void);
    int IsGood();

  private:
    DBMP2F_DataSource       Source;
    DBMP2F_DataSink         Sink;
    DBMP2F_TransferBuffer   Buffer;
};

#endif
