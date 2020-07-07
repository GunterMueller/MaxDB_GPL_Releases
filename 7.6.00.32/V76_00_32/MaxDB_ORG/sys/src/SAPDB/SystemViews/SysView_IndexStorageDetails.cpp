/****************************************************************************/
/*!

  @file        SysView_IndexStorageDetails.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "IndexStorageDetails" view class.

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
#include "hak40.h"
#include "hbd03.h"
#include "hak722.h"

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_IndexStorageDetails.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"
#include "FileDirectory/FileDir_Index.hpp"


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

void SysView_IndexStorageDetails::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IndexStorageDetails::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARUNICODE ,"SCHEMANAME", 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,"TABLENAME", 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,"INDEXNAME", 32);
	m_Table->AppendCol (ITOCT_FIXED ,"TREELEVELCOUNT", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MINSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"MINUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGSECONDARYKEYLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MINSECONDARYKEYLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXSECONDARYKEYLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGROWCOUNTPERSECONDARYKEY", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"MINROWCOUNTPERSECONDARYKEY", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXROWCOUNTPERSECONDARYKEY", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"CLUSTERCOUNT", 10);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IndexStorageDetails::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IndexStorageDetails::GetColCount", SysView_Trace, 5);

    return 22;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_IndexStorageDetails::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IndexStorageDetails::EstimateRows", SysView_Trace, 5);

    return 3000;
}

/*---------------------------------------------------------------------------*/

void SysView_IndexStorageDetails::AddIndexStatistics (Catalog_TableDescriptor& Table)
{
    if (Table)
    {
        if (Table.IsSynonym() || Table.IsView())
        {
            return;
        }
        FileDir_IDirectory& fdir = FileDir_IDirectory::SessionInstance(m_Context.TransContext());
        FileDir_Table pFileDir;
        Msg_List      msgList;

        FileDir_FileNo fileNo;
        Table.GetFileNo(fileNo);
        FileDir_ReturnCode rc = fdir.GetTableFile(m_Context.TransContext().trTaskId_gg00, fileNo, pFileDir, msgList);
        if (FileDir_Okay != rc)
        {
            return;
        }
        if (!m_Context.IsOk())
        {
            return;
        }
        const bool cWithKey = true;
        Catalog_IndexIterator iter = Table.GetIndexIterator(!cWithKey);

        tak40_int2_arr_ptr workBuf;
        a722init_epos_reccol_relation(m_Context, workBuf);

        if (!m_Context.IsOk())
        {
            return;
        }

        while (iter)
        {
            const Catalog_IndexDescriptor& indexDesc = *iter;
            FileDir_FileNo indexFileNo = pFileDir.GetIndexFileNo(indexDesc.GetIndexNo());
            FileDir_Index pIndexFileDir;
            rc = fdir.GetIndexFile(m_Context.TransContext().trTaskId_gg00, indexFileNo, pIndexFileDir, msgList);
            if (FileDir_Okay != rc)
            {
                return;
            }
            tgg00_FileId indexFileId;
            pIndexFileDir.BuildFileId (indexFileId);
            const bool cWithSelectivity = true;
            SQLMan_Identifier indexName = indexDesc.GetName();
           
            if (a722test_col_qual (m_Context, *m_Table->GetPascalObjPtr(), 
                *const_cast<tak_oldidentifier*>(reinterpret_cast<const tak_oldidentifier*> (&cak_ln_indexname[0])), ' ', 
                reinterpret_cast<tsp00_MoveObjPtr>(&indexName), sizeof(indexName), workBuf))
            {
                AddColumn (Table.GetSchemaName());
                AddColumn (Table.GetTableName());
                AddColumn (indexName);

                tak40_show_glob& a40glob  = *m_Table->GetPascalObjPtr();

                if ( a40glob.a4strategy.str_catalog().scat_col_needed[3] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[5] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[6] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[7] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[8] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[9] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[10] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[11] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[12] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[13] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[14] ||
                     a40glob.a4strategy.str_catalog().scat_col_needed[15] )
                {

                    tgg00_SampleInfo indexStatistics;
                    bd03InvStatistics (m_Context.TransContext(), indexFileId, !cWithSelectivity, indexStatistics);
                    if (e_ok != m_Context.TransContext().trError_gg00)
                    {
                        return;
                    }
                    AddColumn (indexStatistics.hight);
                    if (indexStatistics.sec_key_cnt > 0)
                    {
                        AddColumn (indexStatistics.ave_sep_length);
                        AddColumn (indexStatistics.min_sep_length);
                        AddColumn (indexStatistics.max_sep_length);
                    }
                    else
                    {
                        for (int ix = 0; ix < 3; ++ix)
                        {
                            AddColumn();
                        }
                    }
                    AddColumn (indexStatistics.leafcov);
                    AddColumn (indexStatistics.min_leafcov);
                    AddColumn (indexStatistics.max_leafcov);
                    if (indexStatistics.sec_key_cnt > 0)
                    {
                        AddColumn (indexStatistics.avg_sec_key_len);
                        AddColumn (indexStatistics.min_sec_key_len);
                        AddColumn (indexStatistics.max_sec_key_len);
                        AddColumn (indexStatistics.avg_prim_per_list / 10);
                        AddColumn (indexStatistics.min_prim_per_list);
                        AddColumn (indexStatistics.max_prim_per_list);
                    }
                    else
                    {
                        for (int ix = 0; ix < 6; ++ix)
                        {
                            AddColumn();
                        }
                    }

                    // root == leaf
                    if ((indexStatistics.sam_clustercount == 0) &&
                        (indexStatistics.hight == 0))
                           indexStatistics.sam_clustercount = 1;
                    
                    AddColumn (indexStatistics.sam_clustercount);
                }
                else
                {

                    tsp00_Int4 leafCount;
                    tsp00_Int4 clusterCount;
                    
                    bd03CountClusteredPages (m_Context.TransContext(), indexFileId, leafCount, clusterCount);

                    for (int ix = 0; ix < 13; ++ix)
                    {
                        AddColumn();
                    }

                    AddColumn(clusterCount);
                }
            }
            ++iter;
        }
        a722finalize_epos_reccol_relation ( m_Context, workBuf );
    }
}

/*---------------------------------------------------------------------------*/

void SysView_IndexStorageDetails::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_IndexStorageDetails::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    if (m_Context.IsOk())
    { 
        tak40_show_glob& a40glob  = *m_Table->GetPascalObjPtr();
        SQLMan_Identifier schemaName(a40glob.a4authname);
        SQLMan_Identifier tableName (a40glob.a4tablen);
        if (!schemaName.IsBlank() && !tableName.IsBlank() && (a40glob.a4tab_len =  sizeof (a40glob.a4tablen)))
        {
            // tablename completely specified
            const bool considerResultTables = true; 
            const bool considerSYSDDTables  = true;
            Catalog_TableDescriptor table(m_Context, schemaName, tableName, !considerResultTables, !considerSYSDDTables);
            AddIndexStatistics (table);
        }
        else
        {
            const bool scanTemp       = true;
            const bool scanPrivate    = true;
            const bool scanNonPrivate = true;
            const bool scanPublic     = true;
            const bool useSynonyms    = true;
            const bool allBase        = true;
            a40init_table_scan (m_Context, a40glob, scanTemp, scanPrivate, scanNonPrivate, scanPublic, !useSynonyms, allBase);
            while (a40next_table (m_Context, a40glob))
            {
                Catalog_TableDescriptor table(m_Context, *reinterpret_cast<Catalog_ObjectId*>(&a40glob.a4p_arr.pbasep->sbase().bsurrogate[0]));
                AddIndexStatistics (table);
            }
            a40FinishTableScan (m_Context, a40glob);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
