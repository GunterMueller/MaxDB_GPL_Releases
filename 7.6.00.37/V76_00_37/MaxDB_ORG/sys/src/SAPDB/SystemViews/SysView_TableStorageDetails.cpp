/****************************************************************************/
/*!

  @file        SysView_TableStorageDetails.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ, ThomasA

  @ingroup     SystemViews
  @brief       This module implements the "TableStorageDetails" view class.

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
#include "hbd01.h"

#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_TableStorageDetails.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"
#include "FileDirectory/FileDir_IDirectory.hpp"
#include "FileDirectory/FileDir_Table.hpp"


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

void SysView_TableStorageDetails::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TableStorageDetails::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_CHARUNICODE ,"SCHEMANAME", 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,"TABLENAME", 32);
	m_Table->AppendCol (ITOCT_FIXED ,"TREELEVELCOUNT", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MINSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXSEPARATORLENGTH", 4);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"MINUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXUSEDSIZEPERCENTAGEINLEAVES", 3);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGROWCOUNTPERLEAF", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"MINROWCOUNTPERLEAF", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXROWCOUNTPERLEAF", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGROWLENGTH", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"MINROWLENGTH", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXROWLENGTH", 5);
	m_Table->AppendCol (ITOCT_FIXED ,"DEFINEDLOBCOLUMNCOUNT", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"AVGLOBCOLUMNLENGTH", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"MINLOBCOLUMNLENGTH", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"MAXLOBCOLUMNLENGTH", 10);
	m_Table->AppendCol (ITOCT_FIXED ,"CLUSTERCOUNT", 10);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TableStorageDetails::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TableStorageDetails::GetColCount", SysView_Trace, 5);

    return 21;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_TableStorageDetails::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TableStorageDetails::EstimateRows", SysView_Trace, 5);

    return 12000;
}

/*---------------------------------------------------------------------------*/

void SysView_TableStorageDetails::AddTableStatistics (Catalog_TableDescriptor& Table)
{
    if (Table)
    {
        if (Table.IsSynonym())
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
        tgg00_FileId fileId;
        pFileDir.BuildFileId (fileId);

        AddColumn (Table.GetSchemaName());
        AddColumn (Table.GetTableName());
        
        tak40_show_glob& a40glob  = *m_Table->GetPascalObjPtr();


        if ( a40glob.a4strategy.str_catalog().scat_col_needed[2] ||
             a40glob.a4strategy.str_catalog().scat_col_needed[3] ||
             a40glob.a4strategy.str_catalog().scat_col_needed[4] ||
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
             a40glob.a4strategy.str_catalog().scat_col_needed[15] ||
             a40glob.a4strategy.str_catalog().scat_col_needed[16] ||
             a40glob.a4strategy.str_catalog().scat_col_needed[17] ||
             a40glob.a4strategy.str_catalog().scat_col_needed[18] )
        {
            tgg00_SampleInfo info;

            b01sfile_statistic (m_Context.TransContext(), fileId, pFileDir.HasShortColumnFile(), info); 

            AddColumn (info.hight);
            if (info.records > 0)
            {
                AddColumn (info.ave_sep_length);
                AddColumn (info.min_sep_length);
                AddColumn (info.max_sep_length);
            }
            else
            {
                for (int ix = 0; ix < 3; ++ix)
                {
                    AddColumn ();
                }
            }
            AddColumn (info.leafcov);
            AddColumn (info.min_leafcov);
            AddColumn (info.max_leafcov);
            if (info.records > 0)
            {
                AddColumn (info.rec_per_page);
                AddColumn (info.min_rec_per_page);
                AddColumn (info.max_rec_per_page);
                AddColumn (info.ave_rec_length);
                AddColumn (info.min_rec_length);
                AddColumn (info.max_rec_length);
                AddColumn (info.defined_stringcols);
                if (info.defined_stringcols > 0)
                {
                    AddColumn (info.ave_len_stringcol);
                    AddColumn (info.min_len_stringcol);
                    AddColumn (info.max_len_stringcol);
                }
                else
                {
                    for (int ix = 0; ix < 3; ++ix)
                    {
                        AddColumn ();
                    }
                }
            }
            else
            {
                for (int ix = 0; ix < 10; ++ix)
                {
                    AddColumn ();
                }
            }
            AddColumn (info.sam_clustercount);
        }
        else
        {
            tsp00_Int4 leafCount;
            tsp00_Int4 clusterCount;
            
            bd01CalculateClusterStatistics(m_Context.TransContext(), fileId, leafCount, clusterCount);

            
            for (int ix = 0; ix < 17; ++ix)
            {
                AddColumn ();
            }

            AddColumn (clusterCount);

        }
    }
}

/*---------------------------------------------------------------------------*/

void SysView_TableStorageDetails::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_TableStorageDetails::Execute", SysView_Trace, 5);
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
            AddTableStatistics (table);
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
                AddTableStatistics (table);
            }
            a40FinishTableScan (m_Context, a40glob);
        }
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
