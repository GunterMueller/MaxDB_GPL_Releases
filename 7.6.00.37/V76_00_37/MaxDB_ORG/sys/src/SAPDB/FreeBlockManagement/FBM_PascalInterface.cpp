/*!**************************************************************************

  module      : FBM_PascalInterface.cpp
  
  -------------------------------------------------------------------------
  
  responsible : TorstenS

  auhtor      : AlexanderK
      
  special area: PascalInterface of the FreeBlockManagement (FBM)
  description : For the declarations of the functions see hbd11.h. 
                This modul, hbd11.h and vbd11 can be removed after all pascal sources
                using this functions are replaced by the c++ counterparts.
          
  last changed: 2000-03-10  11:00
  see also    : 
    
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

#include "gbd00.h"
#include "gsp03.h"
#include "gsp03_3.h"
#include "hbd11.h"
#include "FreeBlockManagement/FBM_Manager.hpp"

/*===========================================================================*
 *  FUNCTION DEFINITION                                                      *
 *===========================================================================*/

externC tsp00_Int4 
b11_blocks_used (tsp00_Int2 DevNo)
{
    return FBM_Manager::Instance().NumBlocksUsed(DevNo);
}
/*------------------------------------------------------------------------*/

externPascal pasbool b11_has_clustered_blocks_for_backup (
    tsp00_TaskId                         taskid)
{
    return 0 != FBM_Manager::Instance().GetNumberOfClusteredBlocksToBackup(taskid);
}
