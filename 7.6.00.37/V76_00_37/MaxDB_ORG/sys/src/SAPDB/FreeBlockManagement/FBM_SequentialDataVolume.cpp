/*!**************************************************************************

  module      : FBM_SequentialDataVolume.cpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  author      : AlexanderK

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

#include "FreeBlockManagement/FBM_SequentialDataVolume.hpp"
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
 *  CLASS FBM_SequentialDataVolume                                              *
 *---------------------------------------------------------------------------*/

bool
FBM_SequentialDataVolume::InitVolume (const IOMan_ClusterCount numCluster)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SequentialDataVolume::InitVolume", FBM_Trace, 5);

    SAPDBERR_ASSERT_STATE( 0 < m_LastBlock );

    /* set members to their start values */
    m_LastUsedBlock.Invalidate();
    m_DeviceIsInCompressingMode = false;
	
	return true;
}

/*---------------------------------------------------------------------------*/

FBM_BlockState
FBM_SequentialDataVolume::GetBlockState(IOMan_BlockNo BlockNo) const
{
    if (m_LastUsedBlock.IsInvalid())  // first time
    {
        return BlockState_Free;
    }

    if (BlockNo <= m_LastUsedBlock)
    {
        return BlockState_Occupied;
    }
    else
    {
        return BlockState_Free;
    }
}

/*---------------------------------------------------------------------------*/

void
FBM_SequentialDataVolume::SetBlockStateToFree (IOMan_BlockNo BlockNo)
{
    SAPDBERR_ASSERT_STATE (SAPDB_FALSE);
}

/*---------------------------------------------------------------------------*/

void
FBM_SequentialDataVolume::SetBlockStateToOccupied (IOMan_BlockNo BlockNo)
{
    if ((m_LastUsedBlock.IsInvalid()) || (m_LastUsedBlock < BlockNo))
    {
        m_LastUsedBlock = BlockNo;
    }
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
FBM_SequentialDataVolume::GetNumBlocksUsed() const
{
    if (m_LastUsedBlock.IsInvalid())
    {
        return 0;
    }
    else
    {
        return m_LastUsedBlock + 1;
    }
}

/*---------------------------------------------------------------------------*/

IOMan_BlockCount
FBM_SequentialDataVolume::GetNumBlocksFree() const
{
    if (m_LastUsedBlock.IsInvalid())
    {
        return m_LastBlock + 1;
    }
    else
    {
        return m_LastBlock  - m_LastUsedBlock;
    }
}

/*---------------------------------------------------------------------------*/

RTE_VolumeAccessMode
FBM_SequentialDataVolume::VolMode() const
{
    return m_VolMode;
}

/*---------------------------------------------------------------------------*/


IOMan_BlockNo
FBM_SequentialDataVolume::GetMultFreeBlocks  (const IOMan_BlockCount  NumBlocksWanted,
                                    IOMan_BlockCount       &NumBlocksSupplied)
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SequentialDataVolume::GetMultFreeBlocks", FBM_Trace, 5);

    SAPDBERR_ASSERT_STATE (m_LastUsedBlock.IsValid());

    IOMan_BlockNo FreeBlockNo;

    /* this routine is called to get  a cluster of free blocks.  */
    /* before the routine is called it is already obvious that   */
    /* there must be at least one free block on this device      */

    if ((IOMan_BlockNo)(NumBlocksWanted + m_LastUsedBlock) >= m_LastBlock )
    {
        NumBlocksSupplied = m_LastBlock - m_LastUsedBlock;
    }
    else
    {
        NumBlocksSupplied = NumBlocksWanted;
    }
        
    FreeBlockNo     =  m_LastUsedBlock + 1;
    m_LastUsedBlock += NumBlocksSupplied;

    return FreeBlockNo;
}

/*---------------------------------------------------------------------------*/

IOMan_BlockNo
FBM_SequentialDataVolume::GetFreeBlock  ()
{
    SAPDBTRACE_ROUTINE_DEBUG  ("FBM_SequentialDataVolume::GetFreeBlock", FBM_Trace, 5);

    SAPDBERR_ASSERT_STATE ( m_LastUsedBlock.IsValid());

    /* this routine is called to get the adress of a free block. */
    /* before the routine is called it is already obvious that   */
    /* there must be at least one free block on this device      */

    /* find free BlockNo */
    m_LastUsedBlock++;

    /* write found block no into vtrace */
    SAPDBTRACE_WRITELN( FBM_Trace, 5, "FreeSBlockNo: " << m_LastUsedBlock );

    return m_LastUsedBlock;
}

/*---------------------------------------------------------------------------*/

void
FBM_SequentialDataVolume::Dump(
    Kernel_Dump  &dump,
    const IOMan_VolumeNo devNo) const
{
    struct FBMDataDevice   fbmDev;

    fbmDev.dmpDevNo                           = devNo;
    fbmDev.dmpLastUsedBlock                   = m_LastUsedBlock;
    fbmDev.dmpFirstFreeBlock                  = 0;
    fbmDev.dmpNumBlocks                       = m_LastBlock + 1;
    fbmDev.dmpNumBlocksUsed                   = m_LastUsedBlock + 1;
    fbmDev.dmpNumBlocksMarkedForBackup        = 0;
    fbmDev.dmpNumBlocksFreeAfterSVP           = 0;
    fbmDev.dmpNumBlocksBackupFreeAfterSVP     = 0;
    fbmDev.dmpNumBlocksToAddUntilOptIsReached = 0;
    fbmDev.dmpNumberOfSections                = 0;
    fbmDev.dmpActBlockNoForBackup             = 0;
    fbmDev.dmpStartBlockNoToSearchFreeBlock   = 0;
    fbmDev.dmpVolumeToDrop                    = false;
    fbmDev.dmpFiller3                         = false;
    fbmDev.dmpFiller2                         = 0;

    dump.InsertEntry( Kernel_Dump::DmpFBMDataDevice,
                      Kernel_DumpPage::Entry( &fbmDev, sizeof( fbmDev )));
}
