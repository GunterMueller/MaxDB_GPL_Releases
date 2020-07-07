/****************************************************************************/
/*!

  @file        SysView_MemoryHolders.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "MemoryHolders" view class.

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
#include "hgg01_3.h"
#include "hsp77.h"
#include "hak40.h"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_MemoryHolders.hpp"
#include "SQLManager/SQLMan_Context.hpp"

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

void SysView_MemoryHolders::Create()
{

	m_Table->AppendCol (ITOCT_FIXED,       SV_CHUNKCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED,       SV_CHUNKSIZE, 20);
	m_Table->AppendCol (ITOCT_LONGUNICODE, SV_CALLSTACK, 0);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MemoryHolders::GetColCount()
{
	return SV_CC_MEMORYHOLDERS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MemoryHolders::EstimateRows()
{
	return SV_ER_MEMORYHOLDERS;
}

/*---------------------------------------------------------------------------*/

void SysView_MemoryHolders::Execute()
{
    LVCMem_Allocator::SymbolVector::Iterator    iSymbolVec;
    SAPDB_UInt4                                 ChunkCount;
    SAPDB_UInt4                                 ChunkSize;
    SAPDB_Int                                   CallStackID;
    LVCMem_Allocator::SymbolVector              Symbols(LVCMem_Allocator::LVCAllocatorInstance());
    SAPDB_Char                                  format[12];
    SAPDB_Char                                  formatWithOffset[20];
    SAPDB_Char                                  *pCurrentLine;


    #if defined(BIT64)
    strcpy(format, "0x%016lx %s");
    #else
    strcpy(format, "0x%08lx %s");
    #endif

    strcpy(formatWithOffset, format);
    strcat(formatWithOffset, " + 0x%x");

    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    {
        for (LVCMem_Allocator::ContentIterator iContent = LVCMem_Allocator::LVCAllocatorInstance().Begin(); iContent; ++iContent)
        {

            iContent.Current (ChunkCount, ChunkSize, CallStackID, Symbols);

            m_Table->MoveToCol (ITOVT_UINT4, &ChunkCount, 0);
            m_Table->MoveToCol (ITOVT_UINT4, &ChunkSize, 0);

            SAPDB_Char buf[8192];
            int        bufLen = 0;
            for (iSymbolVec = Symbols.Begin(); iSymbolVec != Symbols.End(); ++iSymbolVec)
            {
                pCurrentLine = buf + bufLen;

                if (iSymbolVec->m_offset < 0)
                {
                    sp77sprintf (pCurrentLine, 
                        sizeof(buf) - bufLen - 2,
                        format,
                        (SAPDB_Long)((char *)iSymbolVec->m_pcAddr - (char *)0),
                        iSymbolVec->m_pSymb);
                }
                else
                {
                    sp77sprintf (pCurrentLine, 
                        sizeof(buf) - bufLen - 2,
                        formatWithOffset,
                        (SAPDB_Long)((char *)iSymbolVec->m_pcAddr - (char *)0),
                        iSymbolVec->m_pSymb,
                        iSymbolVec->m_offset);
                }

                bufLen += (SAPDB_Int)strlen(pCurrentLine);
                buf[bufLen  ] = 13;
                buf[bufLen+1] = 10;
                bufLen += 2;
            }
            if (g01unicode)
            {
                if (bufLen > sizeof(buf) / 2)
                {
                    bufLen = sizeof(buf) / 2;
                }
                SAPDB_Char* pSource = &buf[bufLen - 1];
                SAPDB_Char* pDest   = &buf[2 * bufLen - 1];
                for (int ix = 0; ix < bufLen; ++ix)
                {
                    *pDest-- = *pSource--;
                    *pDest-- = 0;
                }
                bufLen *= 2;
            }
            m_Table->MoveToLongColContainer (buf, bufLen);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
