/*!
  @file RTE_KernelMain.h
  @author JoergM
  @ingroup Kernel Runtime
  @brief RTE Kernel Main function and global state changes

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
*/



#ifndef RTE_KERNELMAIN_H
#define RTE_KERNELMAIN_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "SAPDBCommon/SAPDB_Types.h"
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#ifndef externC
# if defined(__cplusplus)
#define externC extern "C" 
#  else
#define externC extern
# endif
#endif
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*!---------------------------------------------
  @brief First function called
   This function is called before other action is done. It is responsible for
   initializing RTE_IGlobal, RTE_ISystem and the allocators RTEMem_Allocator and
   RTEMem_BlockAllocator.
   It also initializes the GUID generator.

  @param argc [in] Number of arguments passed
  @param argv [in] Argument vector

  @return != 0 if initialization failed (to be used as exit code), 0 if initialization succeeded
 */
externC int RTE_KernelMain(int argc, char *argv[]);

/*!---------------------------------------------
  @brief Notify new kernel state
  
   This function can be called whenever kernel state changes. It will issue a message into knldiag
   if state changed and store the new state in kernel global section.
   If needed it will also inform the service control manager, modify icons etc. (i.e. on NT only)

  @param newState [in] the new state
 */
externC void RTE_SetDatabaseState( SAPDB_UInt4 newState );

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#endif  /* RTE_KERNELMAIN_H */
