/****************************************************************************/
/*!

  @file        SysView_PascalTableObj.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements all "append column" functions.

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
#include "ggg91.h"
#include "ggg92.h"
#include "hgg06.h"
#include "gak40.h"
#include "hak40.h"
#include "hak42.h"
#include "vak001.h"
#include "hsp81.h"
#include "hak06.h"
#include "hak07.h"
#include "gkb05.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_UTF8Basis.hpp"
#include "SystemViews/SysView_Defines.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SystemViews/SysView_PascalTableObj.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_ContextSessionMembers.hpp"


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

SysView_PascalTableObj::SysView_PascalTableObj( SQLMan_Context  &context,
                                                tak40_show_glob &a41v)
: m_Context(context),
  m_Show(a41v)
{}


tak40_show_glob * SysView_PascalTableObj::GetPascalObjPtr()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::GetPascalObjPtr", SysView_Trace, 5);

    return (&m_Show);
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::GetCatalogTable ( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::GetCatalogTable", SysView_Trace, 5);

    a40get_catalog_table (m_Context, m_Show);
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::AppendCol (SysView_ITOColType  iType,
                                        const char          *sColName, 
                                        int                 iColSize)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::AppendCol", SysView_Trace, 5);

    switch (iType)
    {
    case ITOCT_CHAR:
        a40AppVarCol  (m_Show, *(tsp00_C32 *)sColName, dcha, iColSize, 0);
        break;

    case ITOCT_CHARBYTE:
        a40AppVarCol  (m_Show, *(tsp00_C32 *)sColName, dchb, iColSize, 0);
        break;

    case ITOCT_CHARUNICODE:
        a40AppVarCol  (m_Show, *(tsp00_C32 *)sColName, dunicode, iColSize, 0);
        break;

    case ITOCT_FIXED:
        a40AppVarCol  (m_Show, *(tsp00_C32 *)sColName, dfixed, iColSize, 0);
        break;

    case ITOCT_LONGASCII :
        a40AppVarCol (m_Show, *(tsp00_C32 *)sColName, dstra, mxsp_long_desc, 0);
        break;

    case ITOCT_LONGUNICODE :
        if(g01unicode)
        {
            a40AppVarCol (m_Show, *(tsp00_C32 *)sColName, dstruni, mxsp_long_desc, 0);
        }
        else
        {
            a40AppVarCol (m_Show, *(tsp00_C32 *)sColName, dstra, mxsp_long_desc, 0);
        }
        break;

    case ITOCT_TIMESTAMP:
        a40AppVarCol  (m_Show, *(tsp00_C32 *)sColName, dtimestamp, mxsp_timestamp, 0);
        break;

    default:
        a07_b_put_error (m_Context, e_invalid_datatype, static_cast<SAPDB_Int>(iType));
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveToCol ( const SysView_ITOVarType iType,
                                         const void               *pVal,
                                         const SAPDB_Int          iLength)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol", SysView_Trace, 5);

    SAPDB_Int2 int2Temp;

    // HP and Linux64 need definition of User before the switch, all other in case only
    tsp00_KnlIdentifier User; 

    // cast const away because of all the following casts...
    void *cpVal = const_cast<void*>(pVal);

    switch (iType)
    {

    case ITOVT_NULL:
        a40move (m_Context, m_Show, NULL, 0);
        break;

    case ITOVT_INT2:
        int2Temp = *(static_cast<SAPDB_Int2 *>(cpVal));
        a40move_i4 (m_Context, m_Show, static_cast<SAPDB_Int4>(int2Temp), !C_IS_UNDEF, 0, !C_ZERO_AS_UNDEF );
        break;
    case ITOVT_INT4:
        a40move_i4 (m_Context, m_Show, *(static_cast<SAPDB_Int4 *>(cpVal)), !C_IS_UNDEF, 0, !C_ZERO_AS_UNDEF );
        break;
    
    case ITOVT_UINT4:
        a40move_ui4 (m_Context, m_Show, *(reinterpret_cast<SAPDB_UInt4 *>(cpVal)), !C_IS_UNDEF, 0, !C_ZERO_AS_UNDEF );
        break;

    case ITOVT_INT8:
    case ITOVT_UINT8:
        MoveInt8 (*(reinterpret_cast<SAPDB_Int8 *>(cpVal)));
        break;

    case ITOVT_CHARPTR:
        a40move_const (m_Context, m_Show, reinterpret_cast<SAPDB_Char *>(cpVal), iLength );
        break;

    case ITOVT_UTF8PTR:
        MoveUTF8Ptr (reinterpret_cast<SAPDB_UTF8 *>(cpVal), iLength );
        break;

    case ITOVT_TRANSNO:
        MoveTransNo (*(reinterpret_cast<tgg91_TransNo *>(cpVal)));
        break;

    case ITOVT_LOCKREQMODE:
        a42new_lock_req_mode (m_Context, m_Show, *(reinterpret_cast<tgg00_LockReqMode_Enum *>(cpVal)));
        break;

    case ITOVT_LOCKREQSTATE:
        a42new_lock_req_state (m_Context, m_Show, *(reinterpret_cast<tgg00_LockReqState *>(cpVal)));
        break;

    case ITOVT_DBPROC:
        // DDT : TODO !! is there anything like a06determine_username for dbprocedurenames ???
        if ( *(reinterpret_cast<tgg00_Surrogate *>(cpVal)) != Catalog_ObjectId() ) // what is the default values for "no procedure" ?
            a40move (m_Context, m_Show, *(reinterpret_cast<tgg00_Surrogate *>(cpVal)), SURROGATE_MXGG00);
        else
            a40move (m_Context, m_Show, NULL, 0);
        break;
    case ITOVT_SURROGATE:
        a40move (m_Context, m_Show, *(reinterpret_cast<tgg00_Surrogate *>(cpVal)), SURROGATE_MXGG00);
        break;

    case ITOVT_LONGCOLREF:
        a40long_descriptor(m_Context, m_Show, *(reinterpret_cast<tgg00_Surrogate *>(cpVal)), iLength);
        break;

    case ITOVT_KNLIDENTIFIER:
        a40move (m_Context, m_Show, reinterpret_cast<tsp00_KnlIdentifier *>(cpVal), sizeof(*(reinterpret_cast<tsp00_KnlIdentifier *>(cpVal))));
        break;

    case ITOVT_UNICODEPTR:
        a40move (m_Context, m_Show, reinterpret_cast<tsp00_KnlIdentifier *>(cpVal), iLength);
        break;

    case ITOVT_TRANSLOCKMODE:
        MoveTransLockMode (*(reinterpret_cast<tkb05_TransLockMode_Enum *>(cpVal)));
        break;

    case ITOVT_PARSEID:
        MoveParseID (*(reinterpret_cast<tak_parsid *>(cpVal)));
        break;

    case ITOVT_SQLMODE:
        MoveSQLMode (*(reinterpret_cast<tsp00_SqlMode_Enum *>(cpVal)));
        break;

    case ITOVT_DATETIMEFORMAT:
        MoveDateTimeFormat (*(reinterpret_cast<tgg00_DateTimeFormat_Enum *>(cpVal)));
        break;

    case ITOVT_BOOL:
        MoveBool (*(reinterpret_cast<SAPDB_Bool *>(cpVal)));
        break;

    case ITOVT_YESNO:
        MoveYesNo (*(reinterpret_cast<SAPDB_Bool *>(cpVal)));
        break;

    case ITOVT_ONOFF:
        MoveOnOff (*(reinterpret_cast<SAPDB_Bool *>(cpVal)));
        break;

    case ITOVT_QUERYREWRITEMODE:
        MoveQueryRewriteMode (*(reinterpret_cast<tak_queryrewrite_mode_Enum *>(cpVal)));
        break;

    case ITOVT_CODETYPE:
        MoveCodeType (*(reinterpret_cast<tsp00_CodeType *>(cpVal)));
        break;

    case ITOVT_SCHEMA:
        // fall through : don't distinguish between USER and SCHEMA 
    case ITOVT_USER:
        if ( *(reinterpret_cast<tgg00_Surrogate *>(cpVal)) != Catalog_ObjectId() ) 
        {
            a06determine_username (m_Context, *(reinterpret_cast<tgg00_Surrogate*>(cpVal)), User);
            a40move (m_Context, m_Show, &User, sizeof(User));
        }
        else
            a40move (m_Context, m_Show, NULL, 0);
        break;
    default:
        a07_b_put_error (m_Context, e_move_error, static_cast<SAPDB_Int>(iType));
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveInt8 ( SAPDB_Int8 &val )
{
    a40move_counter (m_Context, m_Show, (tsp00_8ByteCounter) val, !C_IS_UNDEF, 0, !C_ZERO_AS_UNDEF);
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveUTF8Ptr ( SAPDB_UTF8 *val, SAPDB_Int length )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (UTF8 *)", SysView_Trace, 5);

    SAPDB_Int      byteLen = 0;

    byteLen = length;
    
    if (g01unicode)
    {
        SAPDB_UTF8Basis::UTF8ConstPointer   srcAt;
        tsp81_UCS2Char*                     destAt;
        SAPDB_UInt      					charCnt = 0;
        SAPDB_UInt                          byteCnt = 0;
        SAPDB_UTF8Basis::ConversionResult   res;
        SAPDBMem_IRawAllocator              *mAlloc;
        
        mAlloc = reinterpret_cast<SAPDBMem_IRawAllocator *> (m_Context.a_transinf.tri_trans.trAllocator_gg00);

        res = SAPDB_UTF8Basis::Bytelen( (SAPDB_UTF8 *)val, (SAPDB_UTF8 *)(val + byteLen), srcAt, byteLen, byteCnt, charCnt);
        
        if (!( e_ok == HandleUTF8Error( res )))
        {
            return;
        }

        tsp81_UCS2Char* pUCS2 = reinterpret_cast<tsp81_UCS2Char*> (mAlloc->Allocate ((charCnt+1) * 2));  

        res = SAPDB_UTF8Basis::KernelConvertToUTF16 ((SAPDB_UTF8 *)val, (SAPDB_UTF8 *)(val + byteLen), srcAt, pUCS2, pUCS2 + charCnt, destAt, false);

        if (!( e_ok == HandleUTF8Error( res )))
        {
            return;
        }
    
        a40move (m_Context, m_Show, reinterpret_cast<void *> (pUCS2), (2 * charCnt));
    
        mAlloc->Deallocate (pUCS2);
    }
    else
    {
        a40move_const (m_Context, m_Show, reinterpret_cast<void *> (val), byteLen);
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveTransNo ( tgg91_TransNo &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (TransNo)", SysView_Trace, 5);

    SAPDB_Int8      counter;
    SAPDB_UInt      i;

    if (gg06IsNilTrans( val ))
    {
        a40move (m_Context, m_Show, NULL, 0);
    }
    else
    {
        counter = 0;

        for (i=0; i<6; i++)
        {
            counter = (counter << 8) + val[i];
        }

        MoveInt8 (counter);
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveTransLockMode ( tkb05_TransLockMode_Enum &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (TransLockMode)", SysView_Trace, 5);

    switch (val)
    {
    case tlmFree_ekb05:
        a40move (m_Context, m_Show, NULL, 0);
        break;
    case tlmShare_ekb05:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"SHARE", 5);
        break;
    case tlmExcl_ekb05:
    case tlmEotExcl_ekb05:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"EXCLUSIVE", 9);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveParseID ( tak_parsid &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (ParseID)", SysView_Trace, 5);

    a40move (m_Context, m_Show, &val, sizeof(val));
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveSQLMode ( tsp00_SqlMode_Enum &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (SQLMode)", SysView_Trace, 5);

    switch (val)
    {
    case sqlm_ansi:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ANSI", 4);
        break;
    case sqlm_db2:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"DB2", 3);
        break;
    case sqlm_oracle:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ORACLE", 6);
        break;
    case sqlm_internal:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"INTERNAL", 8);
        break;
    default:
        MoveToCol (ITOVT_NULL, &val, 0);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveDateTimeFormat ( tgg00_DateTimeFormat_Enum &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (DateTimeFormat)", SysView_Trace, 5);

    switch (val)
    {
    case dtf_none:  
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"INDEPENDENT", 11);
        break;
    case dtf_normal: 
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"INTERNAL", 8);
        break;
    case dtf_iso:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ISO", 3);
        break;
    case dtf_usa:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"USA", 3);
        break;
    case dtf_eur:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"EUR", 3);
        break;
    case dtf_jis:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"JIS", 3);
        break;
    case dtf_oracle_date: 
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ORACLE", 6);
        break;
    default:
        MoveToCol (ITOVT_NULL, &val, 0);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveCodeType ( tsp00_CodeType &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (CodeType)", SysView_Trace, 5);

    switch (val)
    {
    case csp_ascii:  
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ASCII", 5);
        break;
    case csp_codeneutral:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"NEUTRAL", 7);
        break;
    case csp_unicode_swap:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"UNICODE SWAP", 12);
        break;
    case csp_unicode:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"UNICODE", 7);
        break;
    case csp_unicode_native:    
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"UNICODE NATIVE", 14);
        break;
    case csp_utf8: 
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"UTF8", 4);
        break;
    case csp_instance_code: 
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"INSTANCE", 8);
        break;
    default:
        MoveToCol (ITOVT_NULL, &val, 0);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveBool ( SAPDB_Bool &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (Bool)", SysView_Trace, 5);

    switch (val)
    {
    case SAPDB_TRUE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"TRUE", 4);
        break;
    case SAPDB_FALSE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"FALSE", 5);
        break;
    }
}

/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveYesNo ( SAPDB_Bool &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (YesNo)", SysView_Trace, 5);

    switch (val)
    {
    case SAPDB_TRUE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"YES", 3);
        break;
    case SAPDB_FALSE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"NO", 2);
        break;
    }
}
/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveOnOff ( SAPDB_Bool &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToCol (OnOff)", SysView_Trace, 5);

    switch (val)
    {
    case SAPDB_TRUE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"ON", 2);
        break;
    case SAPDB_FALSE:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"OFF", 3);
        break;
    }
}
/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveQueryRewriteMode ( tak_queryrewrite_mode_Enum &val )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveQueryRewriteMode", SysView_Trace, 5);

    switch (val)
    {
    case No_Rewrite:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"NO", 2);
        break;
    case Statement_Rewrite:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"STATEMENT", 9);
        break;
    case Operator_Rewrite:
        MoveToCol (ITOVT_CHARPTR, (SAPDB_Char *)"OPERATOR", 8);
        break;
    }
}
/*---------------------------------------------------------------------------*/

void SysView_PascalTableObj::MoveToLongColContainer ( const void* pBuf, int bufLength)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::MoveToLongColContainer", SysView_Trace, 5);

    SysView_PseudoLongHandler::LongIdentificator longId;
    // copy value into pseudo long file
    m_Context.GetPseudoLongHandler().InsertLong(m_Context, pBuf, bufLength, longId);

    // put long descriptor
    tak_long_descriptor longDesc;
    memset (&longDesc, 0, sizeof(longDesc));
    SAPDB_MemCopyNoCheck(&longDesc.lds_descriptor(), &longId, sizeof(longDesc.lds_descriptor()));
    longDesc.lds_maxlen()     = 0;
    longDesc.lds_show_kind().becomes(sh_pseudo_long);
    longDesc.lds_state().addElement(ld_short_scol);
    longDesc.lds_intern_pos() = 1;
    longDesc.lds_infoset().addElement(ld_is_catalog);
    longDesc.lds_colno()      = longId.m_rowCount;
    longDesc.lds_valmode().becomes(vm_nodata);
    longDesc.lds_valpos()     = -1;
    longDesc.lds_vallen()     = -1;
    // move long descriptor into current row
    a40move (m_Context, m_Show, &longDesc, sizeof(longDesc));
}

/*---------------------------------------------------------------------------*/

int SysView_PascalTableObj::HandleUTF8Error(SAPDB_UTF8Basis::ConversionResult &res)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::HandleUTF8Error", SysView_Trace, 5);

    SAPDB_Int err = e_ok;
    
    switch (res)
    {
    case SAPDB_UTF8Basis::SourceExhausted:
        err = e_unitrans_src_too_short;
        break;
    case SAPDB_UTF8Basis::SourceCorrupted:
        err = e_not_translatable;
        break;
    case SAPDB_UTF8Basis::TargetExhausted:
        err = e_unitrans_dest_too_short;
        break;
    }

    if (e_ok != err)
    {
        a07_b_put_error (m_Context, err, 1);
    }
    
    return (err);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int4 SysView_PascalTableObj::GetShowKind( void )
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_PascalTableObj::GetShowKind", SysView_Trace, 5);

    return(m_Show.a4sh_kind);
}

/*===========================================================================*
*  END OF CODE                                                              *
*===========================================================================*/
