/****************************************************************************/
/*!

  @file        SysView_Locks.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "Locks" view class.

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
#include "hak01.h"
#include "hak06.h"
#include "hak071.h"
#include "hak36.h"
#include "hak40.h"
#include "hak42.h"
#include "vkb521.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_Locks.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/SAPDB_RangeCode.hpp" // Kernel_move_and_fill
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*!
    @brief   constant to be output in case the lock is on file systemcatalog2
*/
#define n_sys2_cat         "SYS_2_CATALOG     "

/*!
    @brief   constant to be output in case the lock is on an object file
*/
#define n_obj_file         "OBJECT            "


/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_Locks::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Locks::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED, SV_TRANSACTIONID, 15);
	m_Table->AppendCol (ITOCT_FIXED, SV_LOCKREQUESTTIMEOUT, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_LOCKMODE, 16);
	m_Table->AppendCol (ITOCT_CHAR, SV_LOCKSTATE, 10);
	m_Table->AppendCol (ITOCT_CHAR, SV_REQUESTMODE, 16);
	m_Table->AppendCol (ITOCT_CHAR, SV_REQUESTSTATE, 10);
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_SCHEMANAME, 32);
	m_Table->AppendCol (ITOCT_CHARUNICODE, SV_TABLENAME, 32);
	m_Table->AppendCol (ITOCT_CHARBYTE, SV_TABLEID,  8);
	m_Table->AppendCol (ITOCT_FIXED, SV_ROWIDLENGTH, 4);
	m_Table->AppendCol (ITOCT_CHARBYTE, SV_ROWIDHEX, 64);
	m_Table->AppendCol (ITOCT_CHAR, SV_ROWID, 128);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Locks::GetColCount(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Locks::GetColCount", SysView_Trace, 5);

	return SV_CC_LOCKS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Locks::EstimateRows(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Locks::EstimateRows", SysView_Trace, 5);

	return SV_ER_LOCKS;
}

/*---------------------------------------------------------------------------*/

void SysView_Locks::Execute()
{

    SAPDBTRACE_METHOD_DEBUG ("SysView_Locks::Execute", SysView_Trace, 5);

    ckb521_SQLLockReader                    reader (m_Context.GetAllocator(), m_Context.TransContext());
    ckb521_SQLLockReader::SQLLockElement    *elem;
    SAPDB_Char                              sTemp[256];
    SAPDB_UInt4                             length;
    SAPDB_UInt4                             i;

    m_Table->GetCatalogTable();
    
    if (m_Context.IsOk())
    {
                
        if (reader.Begin())
        {
            while (NULL != (elem = reader.Next()))
            {
                m_Table->MoveToCol (ITOVT_TRANSNO, &elem->sleTransID_kb521, 0);
                m_Table->MoveToCol (ITOVT_INT4, &elem->sleLockReqTimeout_kb521, 0);
                m_Table->MoveToCol (ITOVT_LOCKREQMODE, &elem->sleLockMode_kb521, 0);
                m_Table->MoveToCol (ITOVT_LOCKREQSTATE, &elem->sleLockState_kb521, 0);
                m_Table->MoveToCol (ITOVT_LOCKREQMODE, &elem->sleReqMode_kb521, 0);
                m_Table->MoveToCol (ITOVT_LOCKREQSTATE, &elem->sleReqState_kb521, 0);
                SetOwnerTableName  (elem->sleTblID_kb521);
                m_Table->MoveToCol (ITOVT_SURROGATE, &elem->sleTblID_kb521, 0);
                m_Table->MoveToCol (ITOVT_INT2, &elem->sleRowIDLen_kb521, 0);
                
                if (elem->sleRowIDUndef_kb521)
                {
                    m_Table->MoveToCol(ITOVT_NULL, NULL, 0);
                    m_Table->MoveToCol(ITOVT_NULL, NULL, 0);
                }
                else
                {
                    if (elem->sleRowIDLen_kb521 < sizeof(elem->sleRowIDHex_kb521))
                    {
                        length = elem->sleRowIDLen_kb521;
                    }
                    else
                    {
                        length = sizeof(elem->sleRowIDHex_kb521);
                    }
                
                    m_Table->MoveToCol (ITOVT_CHARPTR, elem->sleRowIDHex_kb521.asCharp(), length);

                    sprintf (sTemp, "x'");

                    for (i=0; i<length; i++)
                    {
                        sprintf(sTemp,"%s%02X", sTemp, elem->sleRowIDHex_kb521[i]);
                    }

                    sprintf(sTemp, "%s'", sTemp);

                    m_Table->MoveToCol (ITOVT_CHARPTR, sTemp, (SAPDB_Int)strlen(sTemp));

                }
            }
        }
    }
}

void SysView_Locks::SetOwnerTableName (tgg00_Surrogate      &TblID)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Locks::SetOwnerTableName", SysView_Trace, 5);

    pasbool                 table_found;
    tgg00_SysInfoKey        sysk;
    tgg00_Rec               sysbuf;
    tsp00_KnlIdentifier     owner;
    tsp00_KnlIdentifier     tablename;
    SAPDB_UInt4             tabname_pos;
    SAPDB_Char              obj_class_char;
    SAPDB_UInt4             help_tabname_len;

    SAPDB_Bool              c_get_all    = true;
    SAPDB_Bool              c_with_owner = true;


    a06_systable_get (m_Context, d_release, TblID, m_Table->GetPascalObjPtr()->a4p_arr.pbasep, c_get_all, table_found);

    if (m_Context.ReturnCode() == a071_return_code (e_old_fileversion, m_Context.a_sqlmode))
    {
        m_Context.ResetReturnCode();
    }

    if (table_found)
    {
        a40table_column_to_shbuf (m_Context, *(m_Table->GetPascalObjPtr()), 
            m_Table->GetPascalObjPtr()->a4p_arr.pbasep, -1, !c_with_owner);
    }
    else
    {
        if (m_Context.ReturnCode() == a071_return_code (e_lock_collision, m_Context.a_sqlmode))
        {
            m_Context.ResetReturnCode();

            sysk = a01defaultkey;
            SAPDB_MemCopyNoCheck ((SAPDB_Char *)sysk.stableid(), TblID, sizeof(sysk.stableid()));
            m_Table->GetPascalObjPtr()->a4p_arr.pbasep = reinterpret_cast<tak_systembuffer *>(&sysbuf);

            a36get_catalog_record (m_Context.TransContext(), sysk, *(m_Table->GetPascalObjPtr()->a4p_arr.pbasep));

            if (e_ok == m_Context.a_transinf.tri_trans.trError_gg00)
            {
                table_found = true;
                
                a06determine_username (m_Context, m_Table->GetPascalObjPtr()->a4p_arr.pbasep->sbase().bauthid, owner);

                m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &owner, 0);

                tablename   =   a01_il_b_identifier;
                tabname_pos =   sizeof (tak_baserecord) -
                                sizeof (m_Table->GetPascalObjPtr()->a4p_arr.pbasep->sbase().bcolumn()) -
                                sizeof (m_Table->GetPascalObjPtr()->a4p_arr.pbasep->sbase().bptr) -
                                sizeof (m_Table->GetPascalObjPtr()->a4p_arr.pbasep->sbase().btablen) + 1;
    
                SAPDB_RangeMove( __FILE__,   1,
                       sizeof (sysbuf.buf()), sizeof (tablename), sysbuf.buf(),
                       tabname_pos + 1, tablename, 1, sysbuf.buf()[tabname_pos],
                       m_Context.a_return_segm->sp1r_returncode());

                m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &tablename, 0);
                
                m_Table->GetPascalObjPtr()->a4p_arr.pbasep = NULL;
            }
        }
    }

    if (!table_found)
    {
        if (g01unicode)
        {
            help_tabname_len = (SAPDB_Int)(mxsp_c18 / 2);
        }
        else
        {
            help_tabname_len = mxsp_c18;
        }
 
        m_Table->MoveToCol(ITOVT_NULL, NULL, 0);

        if ( Catalog_Interface::GetInstance().IsSys2CatalogTable(TblID) )
        {
            m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)n_sys2_cat, help_tabname_len);
        }
        else
        {
            obj_class_char  = TblID[0];
            TblID[0]        = '\0';

            if( (memcmp(TblID, cgg_zero_id, sizeof(cgg_zero_id)) == 0) &&
                (obj_class_char > 0))
            {
                m_Table->MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)n_obj_file, help_tabname_len);
            }
            else
            {
                m_Table->MoveToCol(ITOVT_NULL, NULL, 0);
            }

        }
    }
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
