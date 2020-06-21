/*****************************************************************************/
/*!
  @file Kernel_Dump.cpp

  @author TorstenS

  last changed: 2001-06-08  16:57

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG

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
/*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo58.h"          // vtracewriter_alive

#include "KernelCommon/Kernel_Dump.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

struct Kernel_DumpInfoType
{
    SAPDB_Char  dmpLabel[KERNEL_DUMP_LABEL_LENGTH+1];
    SAPDB_UInt2 dmpCode;
};

Kernel_DumpInfoType Kernel_DumpInfo[] =         // see also vxt08!!
{
    { "NIL     ", 2000 }, // => DmpNil
    { "CONVMGM ", 2001 }, // => DmpConverterManager
    { "CONVMAPC", 2002 }, // => DmpConverterMapControl
    { "CONVMAPS", 2003 }, // => DmpConverterMapSection
    { "CONVMAPE", 2004 }, // => DmpConverterMapEntry
    { "CONVIDX ", 2005 }, // => DmpConverterIndex
    { "FBMMGM  ", 2010 }, // => DmpFBMManager
    { "FBMDEV  ", 2011 }, // => DmpFBMDataDevice
    { "PAGERCNT", 2020 }, // => DmpPagerController
    { "FILEDIR ", 2030 }, // => DmpFileDirectory
    { "SQLTFCOL", 1810 }  // => DmpSQLTempFileCollection
};

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

Kernel_Dump::Kernel_Dump( 
    const tsp00_VFilename  &FileName,
    const Data_PageFrame   &PageFrame,
    const SAPDB_Int4        dumpFileNo,
    const SAPDB_Int4        dumpPageNo)
    :
    m_File( FileName, dumpFileNo ),
    m_Page( PageFrame ),
    m_PageNo( dumpPageNo ),
    m_LastEntryId( DmpNil )
{}

/*---------------------------------------------------------------------------*/

void 
Kernel_Dump::Flush()
{
    if( m_Page.IsUsed() )
    {
        vtracewriter_alive();
        m_Page.PrepareForWrite(); // fill the dump page with 0xFE
        m_File.Write( m_Page );
        m_Page.InitializeFrame();
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Kernel_Dump::InsertEntry(
                         Kernel_DumpEntryId             DumpEntryId,
                         const Kernel_DumpPage::Entry   &DumpEntry )
{
    if( m_File.ErrorOccured() )
        return( false );

    SAPDBERR_ASSERT_STATE(( DmpNil < DumpEntryId ) && ( DumpEntryId < DmpEnd ));

    if(
        ( m_Page.IsSpaceAvailable( DumpEntry.GetLength())) &&
        ( m_LastEntryId == DumpEntryId                  )
        )
    {
        return( m_Page.Insert( DumpEntry ));
    }

    Flush();
    
    m_Page.InitializeFrame( m_PageNo, Kernel_DumpInfo[ DumpEntryId ].dmpLabel, 
        Kernel_DumpInfo[ DumpEntryId ].dmpCode );
    
    m_LastEntryId = DumpEntryId;
    ++m_PageNo;

    return( m_Page.Insert( DumpEntry ));
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool
Kernel_Dump::InsertPage( const Kernel_Page  &Page )
{
    if( m_File.ErrorOccured() )
        return( false );

    Flush();

    m_LastEntryId = DmpNil; // full page dumps has no label
    ++m_PageNo;

    return( m_Page.InsertPage( Page ));
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
