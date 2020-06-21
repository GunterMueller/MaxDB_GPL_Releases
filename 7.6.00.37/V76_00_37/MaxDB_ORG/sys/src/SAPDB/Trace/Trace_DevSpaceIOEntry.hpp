/****************************************************************************

  module      : Trace_DevSpaceIOEntry.hpp

  -------------------------------------------------------------------------

  responsible : TorstenS

  special area: 
  description : 


  last changed: 2001-01-26  14:35
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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


*****************************************************************************/


#ifndef TRACE_DEVSPACEIOENTRY_HPP
#define TRACE_DEVSPACEIOENTRY_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "ggg11.h"
#include "gsp00.h"
#include "vsp002.h"

#include "DataAccess/Data_Types.hpp"
#include "IOManager/IOMan_BlockAddress.hpp"
#include "Logging/Log_Types.hpp"
#include "Trace/Trace_Entry.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *

 *===========================================================================*/

inline
void FillDataIOEntry( 
                 tgg11_BdIoVtrace         &Entry,
                 tgg00_PageType2_Enum     PageType2,
                 tsp00_PageNo             PageNo,
                 const IOMan_BlockAddress &Block,
                 tsp00_Uint1              BufNo = 0 )
{
    Entry.bdioDevType_gg11   = tsp2_dev_type::fromConst( sp2dt_data );
    Entry.bdioPagetype_gg11  = tgg00_PageType::fromConst( ptData_egg00 );
    Entry.bdioPagetype2_gg11 = tgg00_PageType2::fromInt( PageType2 );
    Entry.bdioBufNo_gg11     = BufNo;
    Entry.bdioPno_gg11       = PageNo;
    Entry.bdioDevOffset_gg11 = Block.GetBlockNo();
    Entry.bdioDevNo_gg11     = Block.GetDeviceNo();
}

/*---------------------------------------------------------------------------*/

inline
void FillLogIOEntry( 
                 tgg11_BdIoVtrace         &Entry,
                 tgg00_PageType2_Enum     PageType2,
                 Log_IOSequenceNo         PageNo, // PTS 1124727 mb 2003-01-30
                 const IOMan_BlockAddress &Block,
                 tsp00_Uint1              BufNo = 0 )
{
    Entry.bdioDevType_gg11   = tsp2_dev_type::fromConst( sp2dt_log );
    Entry.bdioPagetype_gg11  = tgg00_PageType::fromConst( ptLog_egg00 );
    Entry.bdioPagetype2_gg11 = tgg00_PageType2::fromInt( PageType2 );
    Entry.bdioBufNo_gg11     = BufNo;
    Entry.bdioPno_gg11       = PageNo.RawValue();
    Entry.bdioDevOffset_gg11 = Block.GetBlockNo();
    Entry.bdioDevNo_gg11     = Block.GetDeviceNo();
}
/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class Trace_ReadConverterPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_ReadConverterPageEntry(
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15read_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillDataIOEntry( m_Entry, pt2Converter_egg00, NIL_PAGE_NO_GG00 , Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};

/*---------------------------------------------------------------------------*/

class Trace_ReadDataPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_ReadDataPageEntry(
        tgg00_PageType2_Enum        PageType2,
        Data_PageNo                 PageNo,
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15read_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillDataIOEntry( m_Entry, PageType2, tsp00_PageNo( PageNo ), Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};

/*---------------------------------------------------------------------------*/

class Trace_ReadLogPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_ReadLogPageEntry(
        tgg00_PageType2_Enum        PageType2,
        Log_IOSequenceNo            PageNo,
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15read_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillLogIOEntry( m_Entry, PageType2, PageNo , Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};
/*---------------------------------------------------------------------------*/


class Trace_WriteConverterPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_WriteConverterPageEntry(
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15write_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillDataIOEntry( m_Entry, pt2Converter_egg00, NIL_PAGE_NO_GG00, Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};

/*---------------------------------------------------------------------------*/

class Trace_WriteDataPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_WriteDataPageEntry(
        tgg00_PageType2_Enum        PageType2,
        Data_PageNo                 PageNo,
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15write_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillDataIOEntry( m_Entry, PageType2, tsp00_PageNo( PageNo ), Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};

/*---------------------------------------------------------------------------*/

class Trace_WriteLogPageEntry : public Trace_CommonEntry
{
public:
    
    Trace_WriteLogPageEntry(
        tgg00_PageType2_Enum        PageType2,
        Log_IOSequenceNo            PageNo,
        const IOMan_BlockAddress    &Block,
        tsp00_Uint1                 BufNo = 0
        )
        :Trace_CommonEntry( tgg00_Debug::fromConst( bd ), 
        tgg00_VtraceType::fromConst( b15write_page ), 
        REINTERPRET_CAST( tsp00_BytePtr, &m_Entry ), sizeof( m_Entry ))
    {
        FillLogIOEntry( m_Entry, PageType2, PageNo, Block, BufNo );
    }
    
private:
    
    tgg11_BdIoVtrace    m_Entry;
};
#endif  /* TRACE_DEVSPACEIOENTRY_HPP */
