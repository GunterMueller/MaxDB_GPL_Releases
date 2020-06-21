/*!***************************************************************************

  module      : RTE_KernelMain.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE KernelMain
  description : SAPDB Kernel Main Initialisation

  Called immediatly from 'main' function to initialize RTE to defaults.
  This routine does not take any parameters and does not inspect any
  parameter files. It is primarily used to initialize RTE_IGlobal and 
  RTE_ISystem interfaces.

  last changed: 2000-07-11  13:40
  first created:2000-06-06  14:25

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_KernelMain.h"
#include "heo670.h"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/RTE_UniqueId.h"
#include "RunTime/Test/RTETst_RWRegionCode.hpp"
#include "RunTime/Test/RTETst_Default.hpp"
#include "RunTime/RTE_KernelInterfaceInstance.hpp"
#include "RunTime/System/RTESys_AtomicOperation.hpp"
#include "RunTime/Security/Crypto/RTECrypto_EntropyPool.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/

static int KernelMain(int argc, char *argv[]);

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

extern "C" int RTE_KernelMain(int argc, char *argv[])
{
/*
 * Enforce initialization of spinlock pool
 */
  SAPDB_Int8 value = 0;
  SAPDB_Int8 oldValue = 0;
  RTESys_CmpxchgInt8(value, (SAPDB_Int8 const)0, (SAPDB_Int8)1, oldValue);

  return KernelMain(argc, argv);
}

//---------------------

extern "C" void RTE_SetDatabaseState( SAPDB_UInt4 newState )
{
    RTE_KGS::Instance().SetDBState( (RTE_KGS::DbState)newState );
}

/*===========================================================================*
 *  STATIC METHODS                                                           *
 *===========================================================================*/
//---------------------

static int KernelMain(int, char *[])
{
  Msg_List errList;

  // force initialization of RTEMem_RteAllocator
  RTE_IInterface::Instance().Allocator();

  // force initialization of Msg_Registry and allocator within it
  RTE_IInterface::Instance().MsgListAllocator();

  eo670_CTraceStackInit();

  /* Kernel specific initialization handled in constructor */
  RTE_KGS::Instance();

  /* Initialization of the GUID generator */
  RTE_InitIdGenerator();

  /* Registration of Default RTE_Tests */
  RTETst_RegisterDefault(errList);

  /* Registration of ReaderWriter Region RTE_Test */
  RTETst_RegisterRWRegionTest(errList);

  /* Initialization of System Entropy Pool */
  RTECrypto_EntropyPool::getInstance( ).initialize( );
  return 0;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
