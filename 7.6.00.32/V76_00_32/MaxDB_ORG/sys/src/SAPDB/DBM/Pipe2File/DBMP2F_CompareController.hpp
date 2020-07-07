/*******************************************************************************
  module:       DBMP2F_CompareController.hpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  class for comparing data delivered by two data sources
  last change:  2002-06-24 23:44
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

#if !defined(_DBMP2F_CompareController_hpp_)
#define _DBMP2F_CompareController_hpp_


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "SAPDB/DBM/Pipe2File/DBMP2F_Controller.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_TransferBuffer.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_DataSource.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_DataComparer.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_CompareController
// -----------------------------------------------------------------------------

class DBMP2F_CompareController: public DBMP2F_Controller
{
  public:
    DBMP2F_CompareController(unsigned int    BufferSize,
                             DBMP2F_Device * SourceDevice1,
                             DBMP2F_Device * SourceDevice2,
                             int             DelayTime,
                             teo00_Uint8     DelayPoint,
                             DBMP2F_Event  * DelayEvent,
                             int             NumberOfWaits,
                             teo00_Uint8     WaitAfterBytes,
                             DBMP2F_Event  * WaitEvent,
                             DBMP2F_Event  * ShowSpeedEvent,
                             DBMP2F_Event  * SourcesDifferEvent);

    int DoTransfer(void);
    int IsGood();

  private:

    DBMP2F_DataSource     Source1;
    DBMP2F_DataSource     Source2;
    DBMP2F_DataComparer   Comparer;
    DBMP2F_TransferBuffer Buffer1;
    DBMP2F_TransferBuffer Buffer2;

    int                 IsGoodFlag;
    DBMP2F_EventChecker EventChecker;
};

#endif
