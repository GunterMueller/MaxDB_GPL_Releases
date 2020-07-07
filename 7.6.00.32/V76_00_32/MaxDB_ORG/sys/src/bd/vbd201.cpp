/****************************************************************************

  module      : vbd201.cpp

  -------------------------------------------------------------------------

  author      : Henrik
  responsible : Henrik

  special area: DataCacheIO
  description : 


  last changed: 2004-03-00  14:00
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG



    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

                      // Content of include files

#include "gbd201.h"
#include "hbd20_5.h"
#include "hgg01.h"
#include "hgg04.h"
#include "heo55k.h"   // vbegexcl
#include "hgg08.h"    // g08data1

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
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



/*===========================================================================*
 *  GLOBAL VARIABLES                                                         *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL VARIABLES                                                          *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL FUNCTIONS (PROTOTYPES)                                             *
 *===========================================================================*/


/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL FUNCTIONS (CODE)                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINITION OF METHODS DECLARED IN gbd201.h (CODE)                        * 
 *===========================================================================*/

bd201_Clusterer::bd201_Clusterer(SAPDBMem_IRawAllocator &allocator):
   m_cBlocks(allocator),
   m_blockCount(allocator)
   {};

void bd201_Clusterer::Initialize() {
    m_cBlocks.Clear();
    m_blockCount.Clear();
}

// loop ueber alle Regions
// solange page in Queue:
//   frontOfFile = pos;
//   hole ersten Controlblock aus der Queue: set to serverIO
//   append an resultChain
//   linksverzeigerung ablaufen, 
//     aus der queue nehmen, serverIO setzen und insert in resultChain[frontOfFile]
//   rechtverzeigerung ablaufen, 
//      aus der queue nehmen, serverIO setzen und append an resultChain
//   fileEnde // append length to FileIndex

void 
bd201_Clusterer::PrepareFlush(const tsp00_TaskId taskId)
{
    SAPDB_Int region = 0;
    
    vbegexcl (taskId, g08ClusterIO);
	
	if (0 != m_blockCount.GetSize())  // still old work todo, don't start again
	{
		vendexcl(taskId, g08ClusterIO);
		return;
	}
	
    tsp00_Int4 dataRegionCount = g01region_cnt (rgnData_egg00);
    for (region = 0; region < dataRegionCount; region++)
    {
        tbd02_pDataCBlock pCBlock;
        tbd02_pDataCBlock nodeIter;
        tbd02_pDataCBlock leftNode;
        tbd02_pDataCBlock rightNode;
        SAPDB_Bool1       found;
     
        while((pCBlock = bd20GetPageFromClusterIOChain(taskId, region)) != NULL)
        {
            SAPDB_Int fileLength = 0;
            SAPDB_Int frontOfFile = m_cBlocks.GetSize();
            
            m_cBlocks.InsertEnd(pCBlock);
            fileLength++;
            
            nodeIter = pCBlock;
            bd20GetLeftChangedNode(taskId, nodeIter, leftNode, found);
            while (found)
            {
                m_cBlocks.Insert(frontOfFile, leftNode);
                fileLength++;
                nodeIter = leftNode;
                bd20GetLeftChangedNode(taskId, nodeIter, leftNode, found);
            }
            
            nodeIter = pCBlock;
            bd20GetRightChangedNode(taskId, nodeIter, rightNode, found);
            while (found)
            {
                m_cBlocks.InsertEnd(rightNode);
                fileLength++;
                nodeIter = rightNode;
                bd20GetRightChangedNode(taskId, nodeIter, rightNode, found);
            }
            
            // end of file
            if (0 != fileLength)
            { 
                m_blockCount.InsertEnd(fileLength);
            }
        }
    }
    vendexcl (taskId, g08ClusterIO);
}
 
void
bd201_Clusterer::GetPagesForIO(const tsp00_TaskId taskId, 
IOMan_DataPages   &pages,
IOMan_BlockCount   numPages,
SAPDB_Bool         inSavePoint)
{
    SAPDB_UInt pagesToCopy;
    SAPDB_UInt loop;
    Data_BasePage Page;
        
    vbegexcl (taskId, g08ClusterIO);
    
    if (0 == m_blockCount.GetSize())
    {
        vendexcl(taskId, g08ClusterIO);
        return;
    }
        
    pagesToCopy = SAPDB_MIN(numPages, m_blockCount[0]);
    for (loop = 0; loop < pagesToCopy; loop++)
    {
        Page.Assign( m_cBlocks[ loop ]->dcb_pFrame_bd02 ,
               !gg04IsStaticPage( m_cBlocks[ loop ]->dcb_pFrame_bd02->nd_pt2() ),
                m_cBlocks[ loop ]->dcb_pFrame_bd02 ->nd_pmode().includes( pmTemp_egg00 ),
                m_cBlocks[ loop ]);
        pages.Append(Page);
    }
    m_cBlocks.Delete(0 , pagesToCopy-1);
    m_blockCount[0] -= pagesToCopy;
    if (0 == m_blockCount[0])
    {
        m_blockCount.DeleteFront();
    }
    vendexcl(taskId, g08ClusterIO);
}

void
bd201_Clusterer::ResetIOState(const tsp00_TaskId taskId, 
                               IOMan_DataPages   &pages,
                               IOMan_BlockCount  startPage,
                               IOMan_BlockCount  pageCount)
{
    typedef IOMan_PagesIterator<Data_BasePage> bd201_Iterator;
    
    tbd2_data_cb_flush_list cbptrList;
    SAPDB_Int  counter = 0;
    
    tsp00_Int4 dataRegionCount = g01region_cnt (rgnData_egg00);
    bd201_Iterator iter( pages );
    for( iter.Begin(startPage); !iter.End() && (counter < pageCount); ++iter, ++counter )
    {
             SAPDB_Int RegOffset  = (*iter).PageNo() % dataRegionCount;
             cbptrList[0] = STATIC_CAST(tbd02_pDataCBlock ,(*iter).GetHint());

             bd20ResetIOState (taskId, RegOffset, 1, cbptrList);
     }
        
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
