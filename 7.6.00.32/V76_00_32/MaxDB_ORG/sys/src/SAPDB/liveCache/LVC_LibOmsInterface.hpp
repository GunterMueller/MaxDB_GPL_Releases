/****************************************************************************

  module      : LVC_LibOmsInterface.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache memory management 
  description : Block allocator for LVCMem_Allocator

                This allocator is based on RteMem_BlockAllocator.

  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




*****************************************************************************/

#ifndef LVC_LIBOMSINTERFACE_HPP
#define LVC_LIBOMSINTERFACE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Oms/OMS_LibOmsInterface.hpp"

class LVC_LibOmsInterface
{
  public :
    inline static OMS_LibOmsInterface* Instance();
    static void SetInterface(OMS_LibOmsInterface* Interface);
  private :
    static OMS_LibOmsInterface* m_interface; 
};

inline OMS_LibOmsInterface* LVC_LibOmsInterface::Instance()
{
  return m_interface;
}

#endif

