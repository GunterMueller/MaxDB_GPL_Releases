/****************************************************************************/
/*!

  @file        SysView_ReaderWriterLockInformation.cpp

  -------------------------------------------------------------------------

  @author      Robin

  @ingroup     SystemViews
  @brief       This module implements the "ReaderWriterLockInformation" view class.

  @see

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_ReaderWriterLockInformation.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "RunTime/Synchronisation/RTESync_RWRegion.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/Synchronisation/RTESync_SpinlockPoolRegister.hpp"

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
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_ReaderWriterLockInformation::Create(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockInformation::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHAR, SV_READERWRITERLOCKADDRESS, 20);
	m_Table->AppendCol (ITOCT_CHAR, SV_SPINLOCKPOOLNAME, 40);
    m_Table->AppendCol (ITOCT_FIXED,SV_READERWRITERLOCKINDEX, 20);
    m_Table->AppendCol (ITOCT_FIXED,SV_LASTENTEREDTASKID, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockInformation::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatistics::GetColCount", SysView_Trace, 5);

	return SV_CC_READERWRITERLOCKINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ReaderWriterLockInformation::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockStatistics::EstimateRows", SysView_Trace, 5);

	return SV_ER_READERWRITERLOCKINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_ReaderWriterLockInformation::Execute(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockInformation::Execute", SysView_Trace, 5);

    RTESync_IRWRegionReader *pReader;
    RTESync_SpinlockPoolReader  SpinlockPoolReader;
    SAPDB_UTF8              spinLockPoolName[44];
    SAPDB_UInt8             uint8val;
    SAPDB_Int4              int4val;

    m_Table->GetCatalogTable();
        
    RTESync_LockedScope lockedScope(RTESync_SpinlockPoolRegister::Instance().Spinlock());   // http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1138984

    if (SpinlockPoolReader.First( RTESync_Spinlock::normal, RTESync_SpinlockPoolRegister::unsynchronized ))   // unsynchronized because we have locked it by ourselves above
    {
        while(SpinlockPoolReader.Next() && (m_Context.IsOk()))
        {
            RTESync_RWRegionRegister *pRWRegionRegister;

            if(SpinlockPoolReader.RWRegionRegister(pRWRegionRegister))  // if the caller supplied us with a spinlock name (to be developed...), it should be compared with the name of the current spinlock here
            {
                pReader = RTESync_GetRWRegionReader(*pRWRegionRegister,RTEMem_Allocator::Instance());

                if (pReader->First())
                {
                    while(pReader->Next() && (m_Context.IsOk()))
                    {
                        pReader->Address(uint8val);
                        SAPDB_ToStringClass address = SAPDB_ToString(uint8val,SAPDB_ToStringClass::hex,SAPDB_ToStringClass::showbase);
                        m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)address,address.Length());

                        pReader->SpinlockPoolName (spinLockPoolName);
                        m_Table->MoveToCol (ITOVT_CHARPTR, spinLockPoolName,(SAPDB_Int)strlen((SAPDB_Char *)spinLockPoolName));

                        pReader->Index(uint8val);
                        m_Table->MoveToCol (ITOVT_INT8, &uint8val, 0);

                        pReader->LastEnteredTask(int4val);
                        if( -1 == int4val)
                        {
                            m_Table->MoveToCol(ITOVT_NULL, NULL, 0); 
                        }
                        else
                        {
                            m_Table->MoveToCol (ITOVT_INT4, &int4val, 0);
                        }
                    }
                }
                (RTEMem_Allocator::Instance()).Deallocate(pReader);
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_ReaderWriterLockInformation::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ReaderWriterLockInformation::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
