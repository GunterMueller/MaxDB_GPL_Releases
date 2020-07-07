/*!**************************************************************************

  module      : FBM_DataVolume.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  auhtor      : AlexanderK

  special area: FreeBlockManagement (FBM)
  description : 


  last changed: 2000-03-10  10:00
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

#include "FreeBlockManagement/FBM_DataVolume.hpp"
#include "FreeBlockManagement/FBM_Dump.hpp"
#include "FreeBlockManagement/FBM_Exception.hpp"
#include "FreeBlockManagement/FBM_Messages.hpp"
#include "KernelCommon/Kernel_Dump.hpp"
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"


/*===========================================================================*
 *  DEFINES                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASS METHOD DEFINITION                                                  *
 *===========================================================================*/

/*---------------------------------------------------------------------------*
 *  CLASS FBM_DataVolume                                              *
 *---------------------------------------------------------------------------*/

bool
FBM_DataVolume::InitVolume(const IOMan_ClusterCount numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_DataVolume::InitVolume", FBM_Trace, 5);

    SAPDBERR_ASSERT_STATE( 0 < m_MaxBlockNo );

    /* set members to their start values */
    m_LastUsedBlock                   = 0;
    m_NumBlocksUsed                   = 0;
    m_NumBlocksFreeAfterSVP           = 0;
    m_NumBlocksMarkedForBackup        = 0;
    m_NumBlocksBackupFreeAfterSVP     = 0;
    m_NumBlocksToAddUntilOptIsReached = 0;

    /* initialize the list containing the block states */
    /* and set all block states in the list to free    */
    if(! m_BlockStateList.Create( m_MaxBlockNo + 1 ))
    {
        FBM_Exception errMsg( __CONTEXT__, FBM_ERR_NO_MORE_MEMORY );
        RTE_Crash( errMsg );
    }

	return true;

}

/*---------------------------------------------------------------------------*/

void
FBM_DataVolume::DumpUsedBlocks() const
{
    SAPDB_Int blockNo;

    for (blockNo = 0; blockNo < m_BlockStateList.Capacity(); blockNo++)
    {
        if (GetBlockState (blockNo) != BlockState_Free)
        {
            FBM_BlockInfo blkInfo(__CONTEXT__, blockNo, GetBlockState (blockNo));
            RTE_Message(blkInfo);
        }
    }
}
