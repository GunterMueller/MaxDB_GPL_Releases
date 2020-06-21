/*!
  @file     Converter_PageNoIterator.cpp
  @author   Henrik
  @ingroup  Converter
  @brief
*/

/*
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

*/

#include "Converter/Converter_PageNoIterator.hpp"
#include "Converter/Converter_Converter.hpp"

Converter_PageNoIterator::Converter_PageNoIterator(
    IOMan_DeviceNo          volNo,
    const RTETask_ITask     &task,
    SAPDBMem_IRawAllocator  &allocator,
    SAPDB_Bool              iterEnd)
        :
        m_VolNo(volNo),
        m_PermMapControl(Converter_Converter::Instance().GetPermMapControl()),
        m_PermStaticMapControl(Converter_Converter::Instance().GetPermStaticMapControl()),
        m_MapType(Map_Temp),
        m_Allocator( UTF8( "ConverterPageNoIterator" ), allocator ),
        m_SpinLockPool((SAPDB_UTF8*)"Converter_PageNoIter::m_DropSpinLock", 1),
        m_pRwLock( 0 ),
        m_IntraMapIter(Converter_Converter::Instance().GetTempMapControl().SectionBegin(volNo, task)),
        m_IntraMapEndIter(Converter_Converter::Instance().GetTempMapControl().SectionEnd(volNo, task))
{
    // set iterator to END?
    if (iterEnd)
    {
        m_MapType = Map_INVALID;
    }

    m_pRwLock = RTESync_CreateRWRegion( 1, m_SpinLockPool, m_Allocator );

    if ( 0 == m_pRwLock )
        RTE_Crash( SAPDBErr_Exception(__CONTEXT__,SAPDBERR_ASSERT_STATE_FAILED,
                                      "Could not create system key cache resources"));
}
/*---------------------------------------------------------------------------*/

SAPDB_Bool
Converter_PageNoIterator::GetNextPageNo(Data_PageNo &pageNo,
                                        Data_PageRecoveryMode &recMode,
                                        const RTETask_ITask &task )
{
    LockedScope lock(*m_pRwLock);
    IOMan_BlockNo  blockNo;

    // advance subiterator
    if (m_MapType != Map_INVALID)
    {
        m_IntraMapIter.GetNextPageNo(task, pageNo, blockNo);
    }

    // advance local iterator if subiterator is at END
    while ((m_MapType != Map_INVALID) && (m_IntraMapIter == m_IntraMapEndIter) )
    {
        // next map control
        if (m_MapType == Map_Temp)
        {
            m_MapType = Map_Perm;
        } else if (m_MapType == Map_Perm)
        {
            m_MapType = Map_PermStatic;
        }
        else
        {
            m_MapType = Map_INVALID;
        }

        switch(m_MapType)
        {
        case Map_Perm:
            m_IntraMapIter = m_PermMapControl.SectionBegin(m_VolNo, task);
            m_IntraMapEndIter = m_PermMapControl.SectionEnd(m_VolNo, task);
            m_IntraMapIter.GetNextPageNo(task, pageNo, blockNo);
            break;
        case Map_PermStatic:
            m_IntraMapIter = m_PermStaticMapControl.SectionBegin(m_VolNo, task);
            m_IntraMapEndIter = m_PermStaticMapControl.SectionEnd(m_VolNo, task);
            m_IntraMapIter.GetNextPageNo(task, pageNo, blockNo);
            break;
        }
    }


    switch(m_MapType)
    {
    case Map_Temp:
        recMode = Data_PageRecoveryMode( Data_Dynamic, Data_NotRecoverable );
        if( Converter_Trace.TracesLevel(5))
        {
            Kernel_VTrace() << "Converter: Drop iter -> pageNo: " <<  pageNo <<
            " " << "(temp)" << " blockNo: " << blockNo << FlushLine;
        }
        break;
    case Map_Perm:
        recMode = Data_PageRecoveryMode( Data_Dynamic, Data_Recoverable );
        if( Converter_Trace.TracesLevel(5))
        {
            Kernel_VTrace() << "Converter: Drop iter -> pageNo: " <<  pageNo <<
            " " << "(perm)" << " blockNo: " << blockNo << FlushLine;
        }
        break;
    case Map_PermStatic:
        recMode = Data_PageRecoveryMode( Data_Static, Data_Recoverable );
        if( Converter_Trace.TracesLevel(5))
        {
            Kernel_VTrace() << "Converter: Drop iter -> pageNo: " <<  pageNo << " "
            << "(static)" << " blockNo: " << blockNo << FlushLine;
        }
        break;
    default:
        return ( false );
        ;
    }
    return ( true );
}
