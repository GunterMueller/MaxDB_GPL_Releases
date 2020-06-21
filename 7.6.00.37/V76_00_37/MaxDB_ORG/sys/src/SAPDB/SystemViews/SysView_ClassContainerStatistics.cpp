/****************************************************************************/
/*!

  @file        SysView_ClassContainerStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "ClassContainerStatistics" view class.

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
#include "hbd04.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_ClassContainerStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Iterator.hpp"
#include "FileDirectory/FileDir_Oms.hpp"

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

void SysView_ClassContainerStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARBYTE ,SV_FILEID, 8);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDSIZEPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ALLOCATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CREATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DELETEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTVIAKEYCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTVIAKEYRANGECOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_PAGECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED ,SV_FREEPAGECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED ,SV_KEYLEAFPAGECOUNT, 10);
    m_Table->AppendCol (ITOCT_FIXED ,SV_OCCUPANCY, 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainerStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_CLASSCONTAINERSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_ClassContainerStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_CLASSCONTAINERSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_ClassContainerStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatistics::Execute", SysView_Trace, 5);
    m_Table->GetCatalogTable();
    if (m_Context.IsOk())
    {
        FileDir_IDirectory& fileDir = FileDir_IDirectory::Instance(m_Context.TransContext().trIndex_gg00);
        FileDir_Iterator objFileIter(m_Context.GetAllocator());

        objFileIter.AddFileTypeToFilter(Data_OmsFixedFile);
        objFileIter.AddFileTypeToFilter(Data_OmsVarFile);
        objFileIter.AddFileTypeToFilter(Data_OmsKeyFile);
        objFileIter.AddHideDeletedFilesFilter();

        fileDir.GetSharedFileDirIterator(objFileIter);
        FileDir_Oms fd_smart;
        while (objFileIter.GetNext(fd_smart)) 
        {
            m_Table->MoveToCol (ITOVT_SURROGATE, fd_smart.GetFileNo().GetCharArray(), 0);
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            AddColumn();
            tgg00_FileId objFileId;
            fd_smart.BuildFileId(objFileId);
            SAPDB_Int4 numAllPages;
            SAPDB_Int4 numFreePages;
            SAPDB_Int4 numEmptyPages;
            SAPDB_Int4 numKeyLeaves;
            SAPDB_Int4 occupancy;
            bd04GetObjFileStatistics (m_Context.TransContext(), objFileId, numAllPages, 
                numFreePages, numEmptyPages, numKeyLeaves, occupancy);  
            if (e_ok == m_Context.TransContext().trError_gg00)
            {
                AddColumn(numAllPages);
                AddColumn(numFreePages);
                AddColumn(numKeyLeaves);
                AddColumn(occupancy);
            }
            else
            {
                for (int ix = 0; ix < 4; ++ix)
                {
                     AddColumn();
                }
            }
        }
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_ClassContainerStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_ClassContainerStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
