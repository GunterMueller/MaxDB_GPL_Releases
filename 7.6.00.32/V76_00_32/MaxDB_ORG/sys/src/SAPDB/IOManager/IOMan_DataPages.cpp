/*
  @file           IOMan_DataPages.cpp
  @author         TorstenS
  @ingroup        IOManagement
  @brief          This module is used to define a variant of the template 
                  class IOMan_KernelPage

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "IOManager/IOMan_DataPages.hpp"
#include "IOManager/IOMan_Types.hpp"
#include "FrameControl/FrameCtrl_ICommon.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

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



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

IOMan_ClusteredDataPages::IOMan_ClusteredDataPages(
    SAPDBMem_IRawAllocator&   allocator,
    const SAPDB_UInt          bufferSize,
    tbd_node* const           pBuffer )
        :
        IOMan_DataPages( allocator, UTF8( "IOMan::ClusteredDataPages" )),
        m_BlockSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
        m_BufferSize( bufferSize ),
        m_pBuffer( pBuffer ),
        m_PageNoList( allocator ),
        m_NodePointerList( allocator )
{}

/*---------------------------------------------------------------------------*/

IOMan_ClusteredDataPages::IOMan_ClusteredDataPages(
    SAPDBMem_IRawAllocator&   allocator,
    Data_PageFrame&           frames )
        :
        IOMan_DataPages( allocator, UTF8( "IOMan::ClusteredDataPages" )),
        m_BlockSize( FrameCtrl_ICommon::GetInstance().GetPageSize()),
        m_BufferSize( frames.GetLength()),
        m_pBuffer( reinterpret_cast< tbd_node* >( frames.GetDataPtr())),
        m_PageNoList( allocator ),
        m_NodePointerList( allocator )
{}

/*---------------------------------------------------------------------------*/

void
IOMan_ClusteredDataPages::AppendPage(
    const Data_PageNo   pageNo,
    Data_BasePage&      page,
    const bool          bDoReadPage )
{
   
    m_PageNoList.InsertEnd( pageNo );
    m_NodePointerList.InsertEnd(( tbd_node* )page.GetFramePtr());
    const SAPDB_UInt  index = m_NodePointerList.GetSize()-1;
    
    SAPDBERR_ASSERT_STATE( 0 != m_pBuffer );
    SAPDBERR_ASSERT_STATE( m_BlockSize * ( index + 1 ) <= m_BufferSize );
    
    if( bDoReadPage )
    { // for clustered io use continous memory chunk
        tbd_node* pNode = &( *m_pBuffer )+ index;
        page.AssignNodePtr( pNode );  
    }
    else
    { // empty page frame indicates that this page is not io relevant
        page.ReleaseFrame();
    }
    if( IOMan_Trace.TracesLevel( 6 ))
    {
        Kernel_VTrace() << "IOMan_Cluster::Append - pageNo: " << pageNo << " pFrame: " 
               << SAPDB_ToString( m_NodePointerList[ index ], _T_p ) << FlushLine;
    }
    IOMan_DataPages::Append( page );
}

/*---------------------------------------------------------------------------*/

void
IOMan_ClusteredDataPages::CopyFrames()
{
    IOMan_DataPages::Pages::Iterator      iter    = m_Pages.Begin();
    IOMan_DataPages::Pages::ConstIterator endIter = m_Pages.End();

    for( ; iter != endIter; ++iter )
    {
        if( ! ( *iter ).IsAssigned()){
            continue;
        }
        const SAPDB_UInt index = m_Pages.GetIndex( iter );
        SAPDBERR_ASSERT_STATE( m_PageNoList[ index ] == ( *iter ).PageNo());
        const void* const pSource  = ( *iter ).GetFramePtr();     // pointer of io memory chunk
        void* const pDest          = m_NodePointerList[ index ];  // pointer of data cache frame
        if( IOMan_Trace.TracesLevel( 6 ))
        {
            Kernel_VTrace() << "IOMan_Cluster::Copy - pageNo: " << m_PageNoList[ index ] << " from: " 
                      << SAPDB_ToString( pSource, _T_p ) << " to: "
                      << SAPDB_ToString( pDest, _T_p ) << FlushLine;
        }
        SAPDB_MemCopyNoCheck( pDest, pSource, m_BlockSize );
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

