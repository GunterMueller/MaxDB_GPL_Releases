/*!================================================================
 module:        OMS_Types.hpp

 responsible:   ThomasA
 
 special area:  OMS
 
 description:   OMS Type definitions
 
 see:
 
 Copyright (c) 1998-2005 SAP AG 



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 ===================================================================*/



#ifndef OMS_TYPES_HPP
#define OMS_TYPES_HPP

#include "gsp92.h"

struct OMS_MethodCallStatistics
{
	tsp00_8ByteCounter       m_allocatorDelta;
	tsp00_Int4               m_streamRead;
	tsp00_Int4               m_streamWrite;
	tsp00_Int4               m_streamRowRead;
	tsp00_Int4               m_streamRowWrite;
	tsp92_RteTaskDetailsInfo m_RteStatistics;
	void Init(tsp00_8ByteCounter allocatorInit)
	{
		m_allocatorDelta = allocatorInit;
		m_streamRead     = 0; 
		m_streamWrite    = 0;
		m_streamRowRead  = 0;
		m_streamRowWrite = 0;
	}
};

class OMS_SwapAndModifyInfo 
{
public :
  enum { copy = 0, 
         swap_2 = 1, 
         swap_4 = 2, 
         swap_8 = 3, 
         asciiClientWydeCpp = 4,  
         unicodeClientAsciiCpp = 5, 
         swappedUnicodeClientAsciiCpp = 6
  };
};

#endif