/*!---------------------------------------------------------------------
  @file           RTE_SpeedInfo.cpp
  @author         JoergM, RobinW
  @brief          DBM: SAPDB Instance and Database Registration and Management Interfaces
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
---------------------------------------------------------------------*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_SpeedInfo.hpp"
#ifdef _WIN32
#include "gos00.h" /* nocheck */
#endif




//! speed definition for No speed 
RTE_SpeedInfo speedNone  (""       ,              -1, true,"");
#ifdef _WIN32
//! speed definition for FAST (productive)
RTE_SpeedInfo speedFast  (RTE_FAST ,KERNEL_TYPE_FAST, true,DB_FAST_KERN_FILE_NAME);
//! speed definition for QUICK (semi productive)
RTE_SpeedInfo speedQuick (RTE_QUICK,KERNEL_TYPE_QUICK,true,DB_QUICK_KERN_FILE_NAME);
//! speed definition for SLOW (debug)
RTE_SpeedInfo speedSlow  (RTE_SLOW ,KERNEL_TYPE_SLOW, true,DB_SLOW_KERN_FILE_NAME);
//! speed definition for TEST (testing)
RTE_SpeedInfo speedTest  (RTE_TEST ,KERNEL_TYPE_TEST, true,DB_TEST_KERN_FILE_NAME);
#else
//! speed definition for FAST (productive)
RTE_SpeedInfo speedFast  (RTE_FAST ,               0, true,"kernel");
//! speed definition for QUICK (semi productive)
RTE_SpeedInfo speedQuick (RTE_QUICK,               1, true,"quickknl");
//! speed definition for SLOW (debug)
RTE_SpeedInfo speedSlow  (RTE_SLOW ,               2, true,"slowknl");
//! speed definition for TEST (testing)
RTE_SpeedInfo speedTest  (RTE_TEST ,               3, true,"omststknl");
#endif
