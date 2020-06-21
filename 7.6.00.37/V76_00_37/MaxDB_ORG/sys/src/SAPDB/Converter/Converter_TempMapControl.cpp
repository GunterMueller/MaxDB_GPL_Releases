/*!
  @file     Converter_TempMapControl.cpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Manages the concurrent access to the structure mapping
            temporary pageNo's to their physical block address
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "Converter/Converter_TempMapControl.hpp"
#include "Converter/Converter_Map.hpp"
#include "Converter/Converter_MapError.hpp"
#include "Converter/Converter_MapSection.hpp"
#include "Converter/Converter_Version.hpp"

#include "DataAccess/Data_Types.hpp"

#include "FreeBlockManagement/FBM_IManager.hpp"

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"


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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Converter_ReturnCode
Converter_TempMapControl::GetBlockAddress(
    const RTETask_ITask &task,
    const Data_PageNo   pageNo,
    IOMan_BlockAddress  &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "TempMapControl::GetBlockAddress", Converter_Trace, 5 );

    const SAPDB_UInt sectionNo = CalculateSectionNo( pageNo );

    Converter_MapSection section( task, m_SectionData[ sectionNo ] );

    return( m_Map.GetBlockAddress( pageNo, block ));
}

/*---------------------------------------------------------------------------*/

Converter_ReturnCode
Converter_TempMapControl::GetNewPositionForWrite(
    const RTETask_ITask  &task,
    const Data_PageNo    pageNo,
    const SAPDB_Bool     bReqSequential,
    IOMan_BlockAddress   &newBlock )
{
    SAPDBTRACE_ROUTINE_DEBUG( "TempMapControl::GetNewPositionForWrite", Converter_Trace, 5 );

    IOMan_BlockAddress  oldBlock;
    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    newBlock = FBM_IManager::Instance().GetFreeBlock( task.ID(), bReqSequential );

    Converter_MapSection section( task, m_SectionData[ sectionNo ] );

    const Converter_ReturnCode retCode =
        m_Map.SetTempBlockAddress( pageNo, newBlock, oldBlock );

    if( Converter_Ok != retCode )
    {
        Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode );
        RTE_Crash( mapError );
    }

    if ( oldBlock.IsValid() )
    {
        FBM_IManager::Instance().SetBlockStateToFree( task.ID(), oldBlock );
    }
    return( Converter_Ok );
}

/*---------------------------------------------------------------------------*/

void
Converter_TempMapControl::SetPositionForWriting (
    const RTETask_ITask         &task,
    const Converter_Version     &pageConverterVersion,
    const Data_PageNo           pageNo,
    const IOMan_BlockAddress    &block )
{
    SAPDBTRACE_ROUTINE_DEBUG( "TempMapControl::SetPositionForWriting", Converter_Trace, 5 );

    IOMan_BlockAddress  oldBlock;
    const SAPDB_UInt    sectionNo = CalculateSectionNo( pageNo );

    {
        Converter_MapSection section( task, m_SectionData[ sectionNo ] );

        const Converter_ReturnCode retCode =
            m_Map.SetTempBlockAddress( pageNo, block, oldBlock );

        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode);
            RTE_Crash( mapError );
        }
    }

    if( pageConverterVersion.IsInvalid()){
        return;
    }

    SAPDBERR_ASSERT_STATE ( oldBlock.IsValid() );

    FBM_IManager::Instance().SetBlockStateToFree( task.ID(), oldBlock );
}

/*---------------------------------------------------------------------------*/

void
Converter_TempMapControl::FreePageNo(
    const RTETask_ITask  &task,
    const Data_PageNo    pageNo )
{
    SAPDBTRACE_ROUTINE_DEBUG( "TempMapControl::FreeTempPageNo", Converter_Trace, 5 );

    IOMan_BlockAddress   oldBlock;
    const SAPDB_UInt     sectionNo = CalculateSectionNo( pageNo );

    {
        Converter_MapSection section( task, m_SectionData[ sectionNo ]);

        const Converter_ReturnCode retCode =
            m_Map.FreeTempPageNo( section, pageNo, oldBlock );

        if( Converter_Ok != retCode )
        {
            Converter_MapError mapError( __CONTEXT__ , GetRecoveryMode(), pageNo, retCode );
            RTE_Crash( mapError );
        }
    }
    if( ! oldBlock.IsValid())
        return;

    FBM_IManager::Instance().SetBlockStateToFree( task.ID(), oldBlock );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
