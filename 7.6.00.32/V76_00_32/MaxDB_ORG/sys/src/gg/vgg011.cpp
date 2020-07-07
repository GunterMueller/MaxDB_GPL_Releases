/* @lastChanged: "2004-03-30"
 
  * @filename:   vgg011.cpp
  * @purpose:    "GG_param_functions"
  * @release:    7.6.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"
 
 
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

 
*/
 
/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/
 
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "hak01.h"
#include "hak10.h"
#include "hak101.h"
#include "hak341.h"
#include "hak545.h"
#include "hbd20_8.h"
#include "hbd03.h"  // bd03SetIndexLeafCaching
#include "gbd490.h"
#include "hkb03_1.h"
#include "hgg01_3.h"
#include "heo58.h"  // tsp0058_RTEInfo
#include "hsp20.h"  // s20int4_to_buf_swap()
#include "KernelCommon/DatabaseEvents/KernelDBE_CallbackHandler.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Kernel.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Join.hpp"
#include "KernelCommon/ParameterNames/KernelParam_QueryRewrite.hpp"
#include "KernelCommon/ParameterNames/KernelParam_SharedSQL.hpp"
#include "KernelCommon/ParameterNames/KernelParam_Pager.hpp"
#include "KernelCommon/ParameterNames/KernelParam_DataAccess.hpp"
#include "KernelCommon/ParameterNames/KernelParam_DataCache.hpp"
#include "SAPDBCommon/DatabaseEvents/SAPDBDBE_KernelParameterNames.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/Diagnostics/SAPDBDiag_Topic.hpp"
#include "KernelCommon/Kernel_TraceTopics.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "Join/Join_HashAccessAllocator.hpp"
#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/SQLMan_Connect.hpp"
#include "SQLManager/SharedSQL/SharedSQL_SQLCache.hpp"
#include "Logging/Log_Volume.hpp"
 
/*===========================================================================*
*  DEFINES                                                                  *
*===========================================================================*/
 
#define cgg01_param_ascii       "ASCII"
#define cgg01_param_europe_date "EUR"
#define cgg01_param_internal    "INTERNAL"
#define cgg01_param_iso_date    "ISO"
#define cgg01_param_jis_date    "JIS"
#define cgg01_param_unicode     "UNICODE"
#define cgg01_param_usa_date    "USA"
 
/*==========================================================================*
*  LOCAL FUNCTIONS                                                          *
*===========================================================================*/
 
static void g011_register_diagnose_topics ();
 
/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */
 
/*----------------------------------------------------------------------------*/
 
void gg011set_trace_any ()
{
g01vtrace.vtrAny_gg00 =
    g01vtrace.vtrAll_gg00            ||
    g01vtrace.vtrAk_gg00             ||
    g01vtrace.vtrAkDelete_gg00       ||
    g01vtrace.vtrAkInsert_gg00       ||
    g01vtrace.vtrAkPacket_gg00       ||
    g01vtrace.vtrAkSelect_gg00       ||
    g01vtrace.vtrAkShortPacket_gg00  ||
    g01vtrace.vtrAkUpdate_gg00       ||
    g01vtrace.vtrBdIndex_gg00        ||
    g01vtrace.vtrBdObject_gg00       ||
    g01vtrace.vtrBdPrim_gg00         ||
    g01vtrace.vtrBdString_gg00       ||
    g01vtrace.vtrIoTrace_gg00        ||
    g01vtrace.vtrKbLock_gg00         ||
    g01vtrace.vtrStrategy_gg00       ||
    g01vtrace.vtrTime_gg00           ||
    g01vtrace.vtrOmsNew_gg00         ||
    g01vtrace.vtrOmsGet_gg00         ||
    g01vtrace.vtrOmsUpd_gg00         ||
    g01vtrace.vtrOmsFree_gg00;
}
 
static SAPDB_Bool gg011evaluate_bool(RTEConf_Parameter::Name name,
                                     RTEConf_Parameter::String value,
                                     pasbool &boolValue,
                                     SAPDBErr_MessageList &err)
{
    // real strcmp()s here would be less performant - but nicer
    if((0 == strcmp((SAPDB_Char *)value,"YES")) || (0 == strcmp((SAPDB_Char *)value,"yes")))
    {
        boolValue = true;
        return true;
    }
    if((0 == strcmp((SAPDB_Char *)value,"NO")) || (0 == strcmp((SAPDB_Char *)value,"no")))
    {
        boolValue = false;
        return true;
    }
    err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM_EXPECTED_X,SAPDB_ToString((SAPDB_Char *)value),
        SAPDB_ToString((SAPDB_Char *)name),"'YES' or 'NO'");
    return false;   // neither 'YES' nor 'NO' --> error
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011evaluate_default_code(RTEConf_Parameter::Name name,
                                             SAPDB_Char *conf_param_compval,
                                             SAPDBErr_MessageList &err)
{
    if(0 == strcmp(conf_param_compval,cgg01_param_ascii))
    {
        g01code.char_default = csp_ascii;
        return true;
    }
    else if(0 == strcmp(conf_param_compval,cgg01_param_unicode))
    {
        g01code.char_default = csp_unicode;
        return true;
    }
    else if(0 == strcmp(conf_param_compval,cgg01_param_internal))
    {
        g01code.char_default = csp_codeneutral;
        return true;
    }
    err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM_EXPECTED_X,SAPDB_ToString(conf_param_compval),
        SAPDB_ToString((SAPDB_Char *)name),"'ASCII' or 'UNICODE'");
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011evaluate_date_time_format(RTEConf_Parameter::Name name,
                                                 SAPDB_Char *conf_param_value,
                                                 SAPDBErr_MessageList &err)
{
    if(0 == strcmp(conf_param_value,cgg01_param_europe_date))
    {
        g01code.date_time.becomes(dtf_eur);
        return true;
    }
    else if(0 == strcmp(conf_param_value,cgg01_param_internal))
    {
        g01code.date_time.becomes(dtf_normal);
        return true;
    }
    else if(0 == strcmp(conf_param_value,cgg01_param_iso_date))
    {
        g01code.date_time.becomes(dtf_iso);
        return true;
    }
    else if(0 == strcmp(conf_param_value,cgg01_param_jis_date))
    {
        g01code.date_time.becomes(dtf_jis);
        return true;
    }
    else if(0 == strcmp(conf_param_value,cgg01_param_usa_date))
    {
        g01code.date_time.becomes(dtf_usa);
        return true;
    }
    err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM_EXPECTED_X,SAPDB_ToString(conf_param_value),
        SAPDB_ToString((SAPDB_Char *)name),"'INTERNAL', 'ISO', 'EUR', 'JIS' or 'USA'");
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
 
static SAPDB_Bool gg011evaluate_optimize_queryrewrite(RTEConf_Parameter::Name name,
                                                     SAPDB_Char *conf_param_value,
                                                     SAPDBErr_MessageList &err)
{
    if(0 == strcmp(conf_param_value,"NO"))
    {
        a01QueryRewriteMode.becomes(No_Rewrite);
        return true;
    }
    else if(0 == strcmp(conf_param_value,"STATEMENT"))
    {
        a01QueryRewriteMode.becomes(Statement_Rewrite);
        return true;
    }
    else if(0 == strcmp(conf_param_value,"OPERATOR"))
    {
        a01QueryRewriteMode.becomes(Operator_Rewrite);
        return true;
    }
    err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM_EXPECTED_X,SAPDB_ToString(conf_param_value),
        SAPDB_ToString((SAPDB_Char *)name),"'NO', 'STATEMENT', or 'OPERATOR'");
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011evaluate_controluserid(RTEConf_Parameter::Name name,
                                               SAPDB_Char *conf_param_value,
                                               SAPDBErr_MessageList &err)
{
    if (0 == *conf_param_value)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM,SAPDB_ToString(conf_param_value),
            SAPDB_ToString(KERNELPARAM_CONTROLUSERID));
        return false;
    }
    else
    {
        g01controluser.gg04cu_user.c2p((SAPDB_Char *)conf_param_value);
        return true;
    }
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011int4_conf_param_get(RTEConf_Parameter::Name conf_param_name,
                                           tsp00_Int4 &conf_param,
                                           SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Integer integerValue;
    if((RTEConf_ParameterAccess::Instance())->GetInteger(conf_param_name,integerValue,err))
    {
        conf_param = (tsp00_Int4)integerValue;
        return true;
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011int2_conf_param_get(RTEConf_Parameter::Name conf_param_name,
                                           tsp00_Int2 &conf_param,
                                           SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::Integer integerValue;
    if((RTEConf_ParameterAccess::Instance())->GetInteger(conf_param_name,integerValue,err))
    {
        conf_param = (tsp00_Int2)integerValue;
        return true;
    }
    return false;
}
/*---------------------------------------------------------------------------*/
 
static void gg011InvalidateAllSharedSQL()
{
    if (a101_SharedSQL_GetStatusParameter())
    {
        SQLMan_Context* pContext = SQLMan_Context::GetContext();
        if (pContext)
        {
            pContext->GetSharedSQL_SQLCache().Invalidate(/*NotifyApplication*/false);
        }
    }
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011bool_get_conf_param(RTEConf_Parameter::Name conf_param_name,
                                           pasbool &conf_param,
                                           SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::String stringValue;
    if((RTEConf_ParameterAccess::Instance())->GetString(conf_param_name,stringValue,err))
    {
        gg011evaluate_bool(conf_param_name,stringValue,conf_param,err);
        return true;
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011_conf_param_get_default_code(SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::String value;
    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_DEFAULT_CODE,value,err))
    {
        return gg011evaluate_default_code(KERNELPARAM_DEFAULT_CODE,(SAPDB_Char *)value,err);
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011_conf_param_get_date_time_format (SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::String value;
    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_DATE_TIME_FORMAT,value,err))
    {
        return gg011evaluate_date_time_format(KERNELPARAM_DATE_TIME_FORMAT,(SAPDB_Char *)value,err);
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011_conf_param_get_optimize_queryrewrite (SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::String value;
    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_OPTIMIZE_QUERYREWRITE,value,err))
    {
        return gg011evaluate_optimize_queryrewrite(KERNELPARAM_OPTIMIZE_QUERYREWRITE,(SAPDB_Char *)value,err);
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011_conf_param_get_controlpassword (SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::CryptInfo value;
    if((RTEConf_ParameterAccess::Instance())->GetCryptInfo((RTEConf_Parameter::Name)KERNELPARAM_CONTROLPASSWORD,value,err))
    {
        SAPDB_MemCopyNoCheck(&g01controluser.gg04cu_password,&value,sizeof(g01controluser.gg04cu_password));
        return true;
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011_conf_param_get_controluserid (SAPDBErr_MessageList &err)
{
    RTEConf_Parameter::String value;
    if((RTEConf_ParameterAccess::Instance())->GetString((RTEConf_Parameter::Name)KERNELPARAM_CONTROLUSERID,value,err))
    {
        return gg011evaluate_controluserid(KERNELPARAM_CONTROLUSERID,(SAPDB_Char *)value,err);
    }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011timeout_init(SAPDBErr_MessageList &err)
{
    // PTS ? UH 06-02-2006 removed range checks which are defined in cserv.pcf
    
    tsp00_Int4 newValue;
    
    if( ! gg011int4_conf_param_get(KERNELPARAM_SESSION_TIMEOUT,newValue,err) )
        return false;

    g01timeout.timeCmd_gg00 = newValue;
    
    if( ! gg011int4_conf_param_get(KERNELPARAM_REQUEST_TIMEOUT,newValue,err) )
        return false;

    g01timeout.timeReq_gg00 = newValue;
    
    return true;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011code_init(SAPDBErr_MessageList &err)
{
    tsp00_C4 s4;
    tsp00_Int4 i4;
    g01code.ctype = (unsigned char)'\0';
    i4 = 1;
    s20int4_to_buf_swap(i4 , sw_normal , s4 , 1 , sw_normal);
    if ((int ) s4[3] == 1)
    {
        g01code.kernel_swap.becomes(sw_normal);
    }
    else
    {
        if ((int ) s4[0] == 1)
        {
            g01code.kernel_swap.becomes(sw_full_swapped);
        }
        else
        {
            g01code.kernel_swap.becomes(sw_part_swapped);
        }
    }
    if(gg011_conf_param_get_default_code(err))
    {
        return gg011_conf_param_get_date_time_format(err);
        }
    return false;
}
 
/*---------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_default_code(RTEConf_Parameter::Name name,
                                           RTEConf_Parameter::Type type,
                                           void *newValue,
                                           void *context,
                                           SAPDBErr_MessageList &err)
{
    return gg011evaluate_default_code(name,(SAPDB_Char *)newValue,err);
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_date_time_format(RTEConf_Parameter::Name name,
                                               RTEConf_Parameter::Type type,
                                               void *newValue,
                                               void *context,
                                               SAPDBErr_MessageList &err)
{
    return gg011evaluate_date_time_format(name,(SAPDB_Char *)newValue,err);
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_stop_errorcode(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 intValue;
    if(RTEConf_Parameter::t_Integer != type)
        {
        return false;
        }
    g01vtrace.vtrStopRetcode_gg00  = 0;
    g01vtrace.vtrRetcodeCheck_gg00 = false;
    intValue = (tsp00_Int4)(*(RTEConf_Parameter::Integer *)newValue);
    if(0 != intValue)
        {
        if((-32000 <= intValue) &&
            (32000 >= intValue))
            {
            g01vtrace.vtrStopRetcode_gg00 = intValue;
            g01vtrace.vtrRetcodeCheck_gg00 = true;
            return true;
            }
        return false;
        }
    return true;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_monitor_rowno(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 i4;
 
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }
    i4 = (tsp00_Int4)(*(RTEConf_Parameter::Integer *)newValue);
    a545sm_set_rowno (i4);
    return true;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_pseudo_int4(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    // nothing will be done as the parameter will be always read directly from the
    // parameter class and no further change/check has to be done
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }
    return true;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_int4(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }
    *((tsp00_Int4 *)context) = (tsp00_Int4)(*(RTEConf_Parameter::Integer *)newValue);
    return true;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_bool(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if(RTEConf_Parameter::t_String != type)
    {
        return false;
    }
    return gg011evaluate_bool(name,(RTEConf_Parameter::String)newValue,*((pasbool*)context),err);
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_trace_bool(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    SAPDB_Bool res;
 
    if(RTEConf_Parameter::t_String != type)
    {
        return false;
    }
    res = gg011evaluate_bool(name,(RTEConf_Parameter::String)newValue,*((pasbool*)context),err);
    if(res)
        {
        gg011set_trace_any();
        }
    return res;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_bool_reset_sharedsql(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if (gg011update_bool(name, type, newValue, context, err))
    {
        gg011InvalidateAllSharedSQL();
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_int4_reset_sharedsql(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if (gg011update_int4(name, type, newValue, context, err))
    {
        gg011InvalidateAllSharedSQL();
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_oms_trace_int4(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        tsp00_KnlIdentifier traceLevel;
        SAPDB_MemFillNoCheck(&traceLevel[0], ' ', sizeof(traceLevel));
        // copy name without prefix trace_
        int copyLen = strlen((const char*) &name[0]) - 6;
        SAPDB_MemCopyNoCheck(&traceLevel[0], &name[6], copyLen);
        traceLevel[copyLen] = 0;
        return ak341SetOmsTraceLevel((const char*) &traceLevel[0], value != 0);
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_callstack_level_int4(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        ak341HeapCallStackMonitoring(value);
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_optimize_join_parallel_servers_int4(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        gg01optimize_parallel_server = a101_SetMaxParallelServers(value);
        gg011InvalidateAllSharedSQL();
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_optimize_queryrewrite(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if (gg011evaluate_optimize_queryrewrite(name, (SAPDB_Char*)newValue, err))
    {
        gg011InvalidateAllSharedSQL();
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_controluserid(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    if (gg011evaluate_controluserid(name, (SAPDB_Char*)newValue, err))
    {
        a01setl_identifier (a01controluser, g01controluser.gg04cu_user);
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_controlpassword(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    SAPDB_MemCopyNoCheck(&g01controluser.gg04cu_password,newValue,sizeof(g01controluser.gg04cu_password));
    return true;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_sharedsql(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    SAPDB_Bool value;
    if (gg011update_bool(name, type, newValue, &value, err))
    {
		gg011InvalidateAllSharedSQL();
        a101_SharedSQL_SetStatusParameter (value);
        return true;
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_hashtable(RTEConf_Parameter::Name name,
                                   RTEConf_Parameter::Type type,
                                   void *newValue,
                                   void *context,
                                   SAPDBErr_MessageList &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
        {
        if (0 == strcmp ((SAPDB_Char *)name, (SAPDB_Char *)KERNELPARAM_MAX_HASHTABLE_MEMORY) )
            {
                return Join_HashAccessAllocator::Instance()
                    .SetMaxHashTableMemory( value );
            }
        else
            if (0 == strcmp ((SAPDB_Char *)name, (SAPDB_Char *)KERNELPARAM_MAX_SINGLE_HASHTABLE_SIZE))
            {
                return Join_HashAccessAllocator::Instance()
                    .SetMaxHashTableSize( value );
            }
    else
        return false;
        }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_memory_protection(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    SAPDB_Bool value;
 
    if( gg011update_bool( name, type, newValue, &value, err ))
    {
        bd20ProtectDataCacheFrames (value);
        gg01DataCacheMemoryProtection = value;
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_updstat_parallel_servers(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        SQLMan_UpdateStatistics::SetNoOfParallelServers( value );
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------*/

static SAPDB_Bool gg011update_updstat_sample_algo(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        SQLMan_UpdateStatistics::SetSampleAlgoNo( value );
        return true;
    }
    return false;
}
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_log_queue_count(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    tsp00_Int4 value;
    if (gg011update_int4(name, type, newValue, &value, err))
    {
        return Log_Volume::Instance().SetQueueCount( value, err );
    }
    return false;
}
 
/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_IndexFileListSize(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    tsp00_Int4 value;
    if( gg011update_int4(name, type, newValue, &value, err )){
        cbd490_CreateIndexParallel::SetIndexFileListSize( value );
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------------*/
 
static SAPDB_Bool gg011update_IndexLeafCaching(
                                                RTEConf_Parameter::Name name,
                                                RTEConf_Parameter::Type type,
                                                void                    *newValue,
                                                void                    *context,
                                                SAPDBErr_MessageList    &err)
{
    tsp00_Int4 value;
    if( gg011update_int4(name, type, newValue, &value, err ))
    {
        bd03SetIndexLeafCaching (value);
        return true;
    }
    return false;
}

/*----------------------------------------------------------------------------*/
 
extern "C" void g011register_update_functions(void)
{
    SAPDBErr_MessageList err;
    SAPDBErr_MessageList sumerr;
    SAPDB_Bool ok;
 
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CONTROLUSERID,                   gg011update_controluserid,         NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CONTROLPASSWORD,                 gg011update_controlpassword,       NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_DATE_TIME_FORMAT,                gg011update_date_time_format,      NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_DDLTRIGGER,                      gg011update_bool,                  &gg01_trigger,                  err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_DEFAULT_CODE,                    gg011update_default_code,          NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_LRU_FOR_SCAN,                    gg011update_bool,                  &g01default_lru,                err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OMS_STREAM_TIMEOUT,              gg011update_int4,                  &gg01_oms_stream_timeout,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_AGGREGATION,            gg011update_bool_reset_sharedsql,  &gg01_aggr_optim,               err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_FETCH_REVERSE,          gg011update_bool_reset_sharedsql,  &gg01_diag_reverse_fetch,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_HASHED_RESULTSET,                gg011update_bool,                  &gg01_hashed_resultset,         err);
	if (!ok) sumerr.AppendNewMessage (err);
	ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
		KERNELPARAM_CLUSTERED_LOBS,                  gg011update_bool,                  &gg01_clustered_lobs,           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_HASHED_RESULTSET_CACHESIZE,      gg011update_pseudo_int4,           NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_HASHTABLE,         gg011update_bool_reset_sharedsql,  &gg01_use_join_hashtable,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_HASH_MINIMAL_RATIO,gg011update_int4_reset_sharedsql,  &gg01_join_hash_min_ratio,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_ONEPHASE,          gg011update_bool_reset_sharedsql,  &gg01_one_join_phase,           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_OUTER,             gg011update_bool_reset_sharedsql,  &gg01_outer_join_ordered,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_MINSIZE,  gg011update_int4_reset_sharedsql,  &gg01_join_parall_minsize,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_MIN_MAX,                gg011update_bool_reset_sharedsql,  &gg01_diag_minmax_optim,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_FIRST_ROWS,             gg011update_bool_reset_sharedsql,  &gg01_optim_first_rows,         err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_OPERATOR_JOIN,          gg011update_bool_reset_sharedsql,  &gg01_operator_join,            err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_OPERATOR_JOIN_COSTFUNC, gg011update_bool_reset_sharedsql,  &gg01_operator_join_costfunc,   err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_OPERATOR_SORT,     gg011update_bool_reset_sharedsql,  &gg01_operator_join_sort,       err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_QUAL_ON_INDEX,     gg011update_bool_reset_sharedsql,       &gg01_qual_on_index,            err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_QUERYREWRITE,           gg011update_optimize_queryrewrite, &a01QueryRewriteMode,           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIM_CACHE,                     gg011update_bool_reset_sharedsql,  &gg01_optim_cache,              err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIM_INV_ONLY,                  gg011update_bool_reset_sharedsql,  &gg01_optim_inv_only,           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIM_JOIN_FETCH,                gg011update_bool_reset_sharedsql,  &gg01_join_prefetch_percent,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_JOIN_SEARCH_LEVEL,               gg011update_bool_reset_sharedsql,  &gg01_join_search_level,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_JOIN_MAXTAB_LEVEL4,              gg011update_bool_reset_sharedsql,  &gg01_join_maxtab_level4,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_JOIN_MAXTAB_LEVEL9,              gg011update_bool_reset_sharedsql,  &gg01_join_maxtab_level9,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIM_MAX_MERGE,                 gg011update_bool_reset_sharedsql,  &gg01_max_merge,                err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_SESSION_TIMEOUT,                 gg011update_int4,                  &g01timeout.timeCmd_gg00,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_SUBTREE_LOCKS,                   gg011update_bool,                  &g01glob.bd_subtree,            err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM__READAHEAD_BLOBS,                gg011update_int4,                  &gg01_readahead_blobs_gt,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM__SERVERDB_FOR_SAP,               gg011update_bool_reset_sharedsql,  &g01glob.db_is_for_sapr3,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MAX_HASHTABLE_MEMORY,            gg011update_hashtable,             NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MAX_SINGLE_HASHTABLE_SIZE,       gg011update_hashtable,             NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_PROTECT_DATACACHE_MEMORY,        gg011update_memory_protection,     NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_UPDATESTAT_PARALLEL_SERVERS,     gg011update_updstat_parallel_servers, NULL,                        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_UPDATESTAT_SAMPLE_ALGO,         gg011update_updstat_sample_algo, NULL,                        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_SHAREDSQL,                       gg011update_sharedsql,             NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    /* PTS 1130973 FF */
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MAX_RETENTION_TIME,              gg011update_int4,                  &gg01_max_retention_time,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    /* PTS 1130973 FF */
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MIN_RETENTION_TIME,              gg011update_int4,                  &gg01_min_retention_time,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    /* PTS 1130965 FF */
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM__TRANS_THRESHOLD_VALUE,          gg011update_int4,                  &gg01_trans_threshold_value,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_TABLE_WIDTH,               gg011update_bool,                  &gg01_checktablewidth,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_COLUMNCOMPRESSION,               gg011update_bool,                  &gg01_columncompression,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_SHOW_MAX_KB_STACK_USE,           gg011update_bool,                  &gg01_kb_stack_measurement,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    // PTS 1135304 mb 2005-05-19
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_LOG_QUEUE_COUNT,                 gg011update_log_queue_count,       NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_IDXFILE_LIST_SIZE,               gg011update_IndexFileListSize,     NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_INDEX_LEAF_CACHING,              gg011update_IndexLeafCaching,     NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);

    g011_register_diagnose_topics ();
 
    // g01vtrace
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_AK,          gg011update_trace_bool,      &g01vtrace.vtrAk_gg00,          err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_DEFAULT,     gg011update_trace_bool,      &g01vtrace.vtrAll_gg00,         err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_DELETE,      gg011update_trace_bool,      &g01vtrace.vtrAkDelete_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_INDEX,       gg011update_trace_bool,      &g01vtrace.vtrBdIndex_gg00,     err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_INSERT,      gg011update_trace_bool,      &g01vtrace.vtrAkInsert_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_LOCK,        gg011update_trace_bool,      &g01vtrace.vtrKbLock_gg00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_LONG,        gg011update_trace_bool,      &g01vtrace.vtrBdString_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OBJECT,      gg011update_trace_bool,      &g01vtrace.vtrBdObject_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OBJECT_ADD,  gg011update_trace_bool,      &g01vtrace.vtrOmsNew_gg00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OBJECT_ALTER,gg011update_trace_bool,      &g01vtrace.vtrOmsUpd_gg00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OBJECT_FREE, gg011update_trace_bool,      &g01vtrace.vtrOmsFree_gg00,     err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OBJECT_GET,  gg011update_trace_bool,      &g01vtrace.vtrOmsGet_gg00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OPTIMIZE,    gg011update_trace_bool,      &g01vtrace.vtrStrategy_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_ORDER,       gg011update_trace_bool,      &g01vtrace.vtrAkPacket_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_ORDER_STANDARD,gg011update_trace_bool,    &g01vtrace.vtrAkShortPacket_gg00,err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_PAGES,       gg011update_trace_bool,      &g01vtrace.vtrIoTrace_gg00,     err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_PRIMARY_TREE,gg011update_trace_bool,      &g01vtrace.vtrBdPrim_gg00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_SELECT,      gg011update_trace_bool,      &g01vtrace.vtrAkSelect_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_TIME,        gg011update_trace_bool,      &g01vtrace.vtrTime_gg00,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_UPDATE,      gg011update_trace_bool,      &g01vtrace.vtrAkUpdate_gg00,    err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_STOP_ERRORCODE,gg011update_stop_errorcode,NULL,                           err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // kb03Check
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_BACKUP,      gg011update_bool,            &kb03Check.chkBackup_kb00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_KB_REGIONS,  gg011update_bool,            &kb03Check.chkRegion_kb00,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_LOCK,        gg011update_bool,            &kb03Check.chkLock_kb00,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_LOCK_SUPPLY, gg011update_bool,            &kb03Check.chkLockSupply_kb00,  err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_REGIONS,     gg011update_bool,            &g01vtrace.vtrCheck_gg00,       err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_TRANSLIST,   gg011update_bool,            &kb03Check.chkTrans_kb00,       err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    //g01glob
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_DATACACHE,   gg011update_bool,            &g01glob.datacachecheck,        err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_TREE,        gg011update_bool,            &g01glob.treecheck,             err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_TREE_LOCKS,  gg011update_bool,            &g01glob.bd_lock_check,         err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CHECK_TASK_SPECIFIC_CATALOGCACHE,gg011update_bool,&a10chk,                      err);
    if (!ok) sumerr.AppendNewMessage (err);
 
 
    //monitor
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MONITOR_SELECTIVITY,gg011update_int4,           &gg01_sm_selectivity,      err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MONITOR_READ,      gg011update_int4,            &gg01_sm_reads,            err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MONITOR_TIME,      gg011update_int4,            &gg01_sm_milliseconds,     err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_MONITOR_ROWNO,     gg011update_monitor_rowno,   NULL,                      err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // oms trace
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_CONTAINERDIR, gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_CONTEXT    , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_ERROR      , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_FLUSHCACHE , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_INTERFACE  , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_KEY        , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_KEYRANGE   , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_LOCK       , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_MEMORY     , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_NEWOBJ     , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_SESSION    , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_STREAM    , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_VAROBJECT  , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_TRACE_OMS_VERSION    , gg011update_oms_trace_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // heap callstack level
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_CALLSTACKLEVEL, gg011update_callstack_level_int4, NULL, err);
    // parallel join servers
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_SERVERS, gg011update_optimize_join_parallel_servers_int4, NULL, err);
    if (!ok) sumerr.AppendNewMessage (err);
    ok=SQLMan_Connect::RegisterAuthenticationParameters (err);
    if (!ok) sumerr.AppendNewMessage (err);
	ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
	    KERNELPARAM_CLUSTER_WRITE_THRESHOLD,      gg011update_int4,            NULL,            err);
	if (!ok) sumerr.AppendNewMessage (err);
 
    // register callback for event category DBFILLINGABOVELIMIT
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
            SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGABOVELIMIT,
            KernelDBE_CallbackHandler::eventCategoryUpdated,
            &KernelDBE_CallbackHandler::contextProviders[sp31ei_db_filling_above_limit],
            err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // register callback for event category DBFILLINGABELOWLIMIT
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
            SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGBELOWLIMIT,
            KernelDBE_CallbackHandler::eventCategoryUpdated,
            &KernelDBE_CallbackHandler::contextProviders[sp31ei_db_filling_below_limit],
            err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // register callback for event category LOGABOVELIMIT
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
            SAPDBDBE_KERNELPARAMETERNAME_LOGABOVELIMIT,
            KernelDBE_CallbackHandler::eventCategoryUpdated,
            &KernelDBE_CallbackHandler::contextProviders[sp31ei_log_above_limit],
            err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    // temporarily included switch
    ok=(RTEConf_ParameterAccess::Instance())->RegisterUpdateFunction(
        KERNELPARAM_USESELECTFETCH,                  gg011update_bool_reset_sharedsql,  &gg01_useselectfetch,           err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    ok=(RTEConf_ParameterAccess::Instance())->CheckUpdateFunctions(err);
    if (!ok) sumerr.AppendNewMessage (err);
 
    RTE_Message(sumerr);
 
}
 
/*----------------------------------------------------------------------------*/
 
extern "C" void g011get_conf_param(tgg00_BasisError *b_err)
{
    SAPDBErr_MessageList err;
    SAPDBErr_MessageList sumerr;
    SAPDB_Bool success = true;
 
    RTEConf_Parameter::String value;
    if((RTEConf_ParameterAccess::Instance())->GetString(KERNELPARAM_INSTANCE_TYPE,value,err))
    {
        SAPDB_Int4 conf_param_length;
        conf_param_length = (SAPDB_Int4)strlen((SAPDB_Char *)value);
        switch (conf_param_length)
        {
            case 3 :
                if (strcmp( (SAPDB_Char *)value , "LVC") == 0)
                {
                    g01glob.kglInstanceType_gg00.becomes(itLVC_egg00);
                }
                break;
            case 4 :
                if (strcmp( (SAPDB_Char *)value , "OLTP") == 0)
                {
                    g01glob.kglInstanceType_gg00.becomes(itOLTP_egg00);
                }
                break;
            case 7 :
                if (strcmp((SAPDB_Char *)value , "ARCHIVE") == 0)
                {
                    g01glob.kglInstanceType_gg00.becomes(itARCHIVE_egg00);
                }
                break;
            default :
                g01glob.kglInstanceType_gg00.becomes(itOLTP_egg00);
                break;
        }
    }
    else
    {
        success = false;
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM_EXPECTED_X,SAPDB_ToString(value),
            SAPDB_ToString((SAPDB_Char *)KERNELPARAM_INSTANCE_TYPE),"'OLTP', 'LVC' or 'ARCHIVE'");
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM__SERVERDB_FOR_SAP,g01glob.db_is_for_sapr3,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM__UNICODE,g01unicode,err);
    if (success)
        {
        success = gg011code_init(err);
        }
    else
        sumerr.AppendNewMessage (err);
 
    success = gg011_conf_param_get_controluserid(err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011_conf_param_get_controlpassword(err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_LOG_BACKUP_TO_PIPE,g01log_backup_to_pipe,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MAXBACKUPDEVS,gg01_no_of_backup_devs,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_BACKUP_BLOCK_CNT,gg01_backup_block_count,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__DELAY_LOGWRITER,gg01_delay_lw,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__RESTART_TIME,g01restart_time,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MAXCPU,gg01_maxcpu,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_USED_MAX_LOG_QUEUE_COUNT,gg01_usedmaxlogqueuecount,err);  // PTS 1135304 mb 2005-05-19
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_LOG_QUEUE_COUNT,gg01_logqueuecount,err);  // PTS 1135304 mb 2005-05-19
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MAXUSERTASKS,gg01_maxuser,err);
    if (!success) sumerr.AppendNewMessage (err);
    if (gg01_maxuser > 32767)
    {
        success = false;
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM,SAPDB_ToString(gg01_maxuser),
            SAPDB_ToString(KERNELPARAM_MAXUSERTASKS));
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__TRANS_RGNS,gg01_trans_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__TAB_RGNS,gg01_tab_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__OMS_RGNS,gg01_oms_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_HEAP_LIMIT,gg01_oms_heap_limit,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_HEAP_COUNT,gg01_oms_heap_count,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_HEAP_BLOCKSIZE,gg01_oms_heap_blocksize,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_HEAP_THRESHOLD,gg01_oms_heap_usage_threshold,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_VERS_THRESHOLD,gg01_oms_vers_free_threshold,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__ROW_RGNS,gg01_row_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MAXTRANS,gg01_maxtrans,err);
    if (!success) sumerr.AppendNewMessage (err);
    if (gg01_maxtrans < 1 || gg01_maxtrans > 32767)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_VALUE_FOR_PARAM,SAPDB_ToString(gg01_maxtrans),
            SAPDB_ToString(KERNELPARAM__MAXTRANS));
        success = false;
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MAXLOCKS,gg01_maxlocks,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__LOCK_SUPPLY_BLOCK,gg01_lock_supply_block,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_DEADLOCK_DETECTION,gg01_deadlock_detection,err);
    if (!success) sumerr.AppendNewMessage (err);
    if (success)
    {
        success = gg011timeout_init(err);
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OMS_STREAM_TIMEOUT,gg01_oms_stream_timeout,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_JOIN_TABLEBUFFER,gg01_join_tablebuffer,err);
    if (!success) sumerr.AppendNewMessage (err);
    if(success = gg011bool_get_conf_param(KERNELPARAM_LRU_FOR_SCAN,g01default_lru,err))
    {
        g01default_lru = !g01default_lru;
    }
    if (!success) sumerr.AppendNewMessage (err);
    if(success = gg011int4_conf_param_get(KERNELPARAM__PACKET_SIZE,gg01_packet_size,err))
    {
        gg01_packet_size = gg01_packet_size / 16 * 16;
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MBLOCK_DATA_SIZE,gg01_mb_data_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MBLOCK_QUAL_SIZE,gg01_mb_qual_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MBLOCK_STACK_SIZE,gg01_mb_stack_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MBLOCK_STRAT_SIZE,gg01_mb_strat_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__WORKSTACK_SIZE,gg01_kb_work_st_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__WORKDATA_SIZE,gg01_kb_workbuf_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__CAT_CACHE_MINSIZE,gg01_ccs_catalog_cache_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_CAT_CACHE_SUPPLY,gg01_ccp_catalog_cache_pages,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__MAXGARBAGE_COLL,gg01_maxgarbage_collector,err);
    if (!success) sumerr.AppendNewMessage (err);
    if(success = gg011int4_conf_param_get(KERNELPARAM__MAXTASK_STACK,gg01_maxuserstack,err))
    {
        gg01_maxuserstack =(gg01_maxuserstack<<10);
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__DW_IO_AREA_SIZE,gg01_dw_io_area_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__DW_IO_AREA_FLUSH,gg01_dw_io_area_flush,err);
    if (!success) sumerr.AppendNewMessage (err);
 
    /* the cache size is only checked here. The temporary storage is in the XPARAM structure */
    tsp00_Int4 cache_size;
    success = gg011int4_conf_param_get(KERNELPARAM_CACHE_SIZE,cache_size,err);
    if (!success) sumerr.AppendNewMessage (err);
    if (cache_size < 800)
    {
        err = SAPDBErr_MessageList(RTE_CONTEXT,RTEERR_CONF_INVALID_CACHE_SIZE,SAPDB_ToString(cache_size));
        success = false;
    }
    if (!success) sumerr.AppendNewMessage (err);
 
    success = gg011int4_conf_param_get(KERNELPARAM__DW_LRU_TAIL_FLUSH,gg01_dw_lru_tail_flush,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__DATA_CACHE_RGNS,gg01_data_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MAXPAGER,gg01_maxdatawriter,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_SEQUENCE_CACHE,gg01_sequence_cache_pages,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_CONVERTER_REGIONS,gg01_converter_regions,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OPTIM_MAX_MERGE,gg01_max_merge,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIM_INV_ONLY,gg01_optim_inv_only,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIM_CACHE,gg01_optim_cache,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OPTIM_JOIN_FETCH,gg01_join_prefetch_percent,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_JOIN_SEARCH_LEVEL,gg01_join_search_level,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int2_conf_param_get(KERNELPARAM_JOIN_MAXTAB_LEVEL4,gg01_join_maxtab_level4,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int2_conf_param_get(KERNELPARAM_JOIN_MAXTAB_LEVEL9,gg01_join_maxtab_level9,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM__READAHEAD_BLOBS,gg01_readahead_blobs_gt,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_LOG_SEGMENT_SIZE,g01autosave_distance,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_HASHED_RESULTSET,gg01_hashed_resultset,err);
    if (!success) sumerr.AppendNewMessage (err);
	success = gg011bool_get_conf_param(KERNELPARAM_CLUSTERED_LOBS,gg01_clustered_lobs,err);
	if (!success) sumerr.AppendNewMessage (err);
	 
    success = gg011bool_get_conf_param(KERNELPARAM_DDLTRIGGER,gg01_trigger,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_AGGREGATION,gg01_aggr_optim,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_COLUMNCOMPRESSION,gg01_columncompression,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_TABLE_WIDTH,gg01_checktablewidth,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_SHOW_MAX_KB_STACK_USE,gg01_kb_stack_measurement,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_FETCH_REVERSE,gg01_diag_reverse_fetch,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_JOIN_HASHTABLE,gg01_use_join_hashtable,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OPTIMIZE_JOIN_HASH_MINIMAL_RATIO,gg01_join_hash_min_ratio,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_JOIN_ONEPHASE,gg01_one_join_phase,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_JOIN_OUTER,gg01_outer_join_ordered,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_MINSIZE,gg01_join_parall_minsize,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_MIN_MAX,gg01_diag_minmax_optim,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_FIRST_ROWS,gg01_optim_first_rows,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_OPERATOR_JOIN,gg01_operator_join,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_OPERATOR_JOIN_COSTFUNC,gg01_operator_join_costfunc,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_JOIN_OPERATOR_SORT,gg01_operator_join_sort,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_OPTIMIZE_QUAL_ON_INDEX,gg01_qual_on_index,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011_conf_param_get_optimize_queryrewrite(err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_SUBTREE_LOCKS,g01glob.bd_subtree,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_PROTECT_DATACACHE_MEMORY,gg01DataCacheMemoryProtection,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_AK,g01vtrace.vtrAk_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_DEFAULT,g01vtrace.vtrAll_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_DELETE,g01vtrace.vtrAkDelete_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_INDEX,g01vtrace.vtrBdIndex_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_INSERT,g01vtrace.vtrAkInsert_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_LOCK,g01vtrace.vtrKbLock_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_LONG,g01vtrace.vtrBdString_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OBJECT,g01vtrace.vtrBdObject_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OBJECT_ADD,g01vtrace.vtrOmsNew_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OBJECT_ALTER,g01vtrace.vtrOmsUpd_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OBJECT_FREE,g01vtrace.vtrOmsFree_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OBJECT_GET,g01vtrace.vtrOmsGet_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_OPTIMIZE,g01vtrace.vtrStrategy_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_ORDER,g01vtrace.vtrAkPacket_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_ORDER_STANDARD,g01vtrace.vtrAkShortPacket_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_PAGES,g01vtrace.vtrIoTrace_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_PRIMARY_TREE,g01vtrace.vtrBdPrim_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_SELECT,g01vtrace.vtrAkSelect_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_TIME,g01vtrace.vtrTime_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_TRACE_UPDATE,g01vtrace.vtrAkUpdate_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int2_conf_param_get(KERNELPARAM_TRACE_STOP_ERRORCODE,g01vtrace.vtrStopRetcode_gg00,err);
    if(0 != g01vtrace.vtrStopRetcode_gg00)
    {
        g01vtrace.vtrRetcodeCheck_gg00 = true;
    }
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_BACKUP,kb03Check.chkBackup_kb00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_KB_REGIONS,kb03Check.chkRegion_kb00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_LOCK,kb03Check.chkLock_kb00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_LOCK_SUPPLY,kb03Check.chkLockSupply_kb00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_REGIONS,g01vtrace.vtrCheck_gg00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_TASK_SPECIFIC_CATALOGCACHE,a10chk,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_TRANSLIST,kb03Check.chkTrans_kb00,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_DATACACHE,g01glob.datacachecheck,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_TREE,g01glob.treecheck,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_CHECK_TREE_LOCKS,g01glob.bd_lock_check,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MONITOR_SELECTIVITY,gg01_sm_selectivity,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MONITOR_READ,gg01_sm_reads,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_MONITOR_TIME,gg01_sm_milliseconds,err);
    if (!success) sumerr.AppendNewMessage (err);
    tsp00_Int4 monitor_rowno;
    success = gg011int4_conf_param_get(KERNELPARAM_MONITOR_ROWNO,monitor_rowno,err);
    if (!success) sumerr.AppendNewMessage (err);
    a545sm_set_rowno (monitor_rowno);
 
    // just to check for the existence of the parameters
    // real read is done in vak341 AFTER loading the DLL
    tsp00_Int4 dummyI4;
    success = gg011int4_conf_param_get(KERNELPARAM_TRACE_OMS_CONTAINERDIR, dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get(KERNELPARAM_TRACE_OMS_CONTEXT     , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_ERROR      , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_FLUSHCACHE , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_INTERFACE  , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_KEY        , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_KEYRANGE   , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_LOCK       , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_MEMORY     , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_NEWOBJ     , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_SESSION    , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_STREAM    ,  dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_VAROBJECT  , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011int4_conf_param_get( KERNELPARAM_TRACE_OMS_VERSION    , dummyI4, err);
    if (!success) sumerr.AppendNewMessage (err);
    gg011set_trace_any();
    /* PTS 1130973 FF */
    success = gg011int4_conf_param_get( KERNELPARAM_MAX_RETENTION_TIME   , gg01_max_retention_time, err);
    if (!success) sumerr.AppendNewMessage (err);
    /* PTS 1130973 FF */
    success = gg011int4_conf_param_get( KERNELPARAM_MIN_RETENTION_TIME   , gg01_min_retention_time, err);
    if (!success) sumerr.AppendNewMessage (err);
    /* PTS 1130965 FF */
    success = gg011int4_conf_param_get( KERNELPARAM__TRANS_HISTORY_SIZE  , gg01_trans_history_size, err);
    if (!success) sumerr.AppendNewMessage (err);
    /* PTS 1130965 FF */
    success = gg011int4_conf_param_get( KERNELPARAM__TRANS_THRESHOLD_VALUE, gg01_trans_threshold_value, err);
    if (!success) sumerr.AppendNewMessage (err);
    /* PTS 1130965 FF */
    success = gg011bool_get_conf_param( KERNELPARAM_ENABLE_SYSTEM_TRIGGERS, gg01_enable_system_triggers, err);
    if (!success) sumerr.AppendNewMessage (err);
    tsp00_Int4 callStackLevel;
    success = gg011int4_conf_param_get( KERNELPARAM_CALLSTACKLEVEL       , callStackLevel, err);
    if (!success) sumerr.AppendNewMessage (err);
    if (success)
    {
        LVCMem_Allocator::LVCAllocatorInstance().SetCallStackMonitoring(callStackLevel);
    }
    tsp00_Int4 joinMaxSersvers;
    success = gg011int4_conf_param_get( KERNELPARAM_OPTIMIZE_JOIN_PARALLEL_SERVERS, joinMaxSersvers, err);
    if (success)
    {
        // a101_SetMaxParallelServers() will be called in Kernel_Initialization.cpp
        gg01optimize_parallel_server = joinMaxSersvers;
    }
    else
    {
        sumerr.AppendNewMessage (err);
    }
 
    success = gg011bool_get_conf_param(KERNELPARAM_USESELECTFETCH,gg01_useselectfetch,err);
    if (!success) sumerr.AppendNewMessage (err);
    success = gg011bool_get_conf_param(KERNELPARAM_USEVARIABLEINPUT,gg01_usevariableinput,err);
    if (!success) sumerr.AppendNewMessage (err);

    tsp00_Int4 indexLeafCaching = 0;
    success = gg011int4_conf_param_get(KERNELPARAM_INDEX_LEAF_CACHING, indexLeafCaching, err);
    if (success)
        bd03SetIndexLeafCaching(indexLeafCaching); 
    else
        sumerr.AppendNewMessage (err);
 
    if (sumerr.IsEmpty ())
    {
        tsp0058_RTEInfo rteInfo;
        vgetrteinfo(rteInfo);
        gg01_max_ukt = rteInfo.NumOfUKTs;
        SAPDB_MemCopyNoCheck(g01serverdb_ident.svServerdb_gg04, rteInfo.ServerDB,sizeof(g01serverdb_ident.svServerdb_gg04));
        SAPDB_MemCopyNoCheck(g01serverdb_ident.svServernode_gg04, rteInfo.ServerNode,sizeof(g01serverdb_ident.svServernode_gg04));
        *b_err = e_ok;
    }
    else
    {
        *b_err = e_conf_parameter_error;
        RTE_Crash(sumerr);
    }
}
 
/*----------------------------------------------------------------------------*/
 
extern "C" void g011get_event_param()
{
    SAPDBErr_MessageList err;
    SAPDBErr_MessageList sumerr;
    SAPDB_Bool success = true;
    // initialize event category DBFILLINGABOVELIMIT
    success = KernelDBE_CallbackHandler::initializeEventCategory(
                    SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGABOVELIMIT,
                    RTEConf_Parameter::t_String,
                    sp31ei_db_filling_above_limit,
                    err);
    if (!success) sumerr.AppendNewMessage (err);
 
    // initialize event category DBFILLINGBELOWLIMIT
    success = KernelDBE_CallbackHandler::initializeEventCategory(
                    SAPDBDBE_KERNELPARAMETERNAME_DBFILLINGBELOWLIMIT,
                    RTEConf_Parameter::t_String,
                    sp31ei_db_filling_below_limit,
                    err);
    if (!success) sumerr.AppendNewMessage (err);
}
 
/*----------------------------------------------------------------------------*/
 
extern "C" void g011get_log_event_param()
{
    SAPDBErr_MessageList err;
    SAPDBErr_MessageList sumerr;
    SAPDB_Bool success = true;
 
    // initialize event category LOGFILLINGABOVELIMIT
    success = KernelDBE_CallbackHandler::initializeEventCategory(
                    SAPDBDBE_KERNELPARAMETERNAME_LOGABOVELIMIT,
                    RTEConf_Parameter::t_String,
                    sp31ei_log_above_limit,
                    err);
    if (!success) sumerr.AppendNewMessage (err);
 
    if (!sumerr.IsEmpty ())
    {
        RTE_Crash(sumerr);
    }
 
}
 
/*----------------------------------------------------------------------------*/
const char * checkPrefixC = "CHECK_";
const int    checkPrefixLenC = 6;
const char * tracePrefixC = "TRACE_";
const int    tracePrefixLenC = 6;
/*----------------------------------------------------------------------------*/
 
/*
 * callback to change the level of check topics
 * through the kernel parameter layer
 */
static SAPDB_Bool gg011update_check_topic_callback (
    RTEConf_Parameter::Name name,
    RTEConf_Parameter::Type type,
    void                  * newValue,
    void                  * context,
    SAPDBErr_MessageList  & err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }
    SAPDB_Int8 typedValue = * (SAPDB_Int8 *) newValue;
    const SAPDB_Char * topicName = reinterpret_cast<const SAPDB_Char *> (name) + checkPrefixLenC;
    Kernel_SwitchDiagByName (topicName, static_cast<int> (typedValue));
    return true;
}

/*----------------------------------------------------------------------------*/

/*
 * callback to change the level of trace topics
 * through the kernel parameter layer
 */
static SAPDB_Bool gg011update_trace_topic_callback (
    RTEConf_Parameter::Name name,
    RTEConf_Parameter::Type type,
    void                  * newValue,
    void                  * context,
    SAPDBErr_MessageList  & err)
{
    if(RTEConf_Parameter::t_Integer != type)
    {
        return false;
    }
    SAPDB_Int8 typedValue = * (SAPDB_Int8 *) newValue;
    const SAPDB_Char * topicName = reinterpret_cast<const SAPDB_Char *> (name) + checkPrefixLenC;
    Kernel_SwitchTraceByName (topicName, static_cast<int> (typedValue));
    return true;
}

/*----------------------------------------------------------------------------*/

typedef bool (*topicIteratorFP) (
    int requested,
    SAPDBDiag_Topic ** topic);

static void g011_generic_register_diagnose_topics (
    const char                      * prefix,
    int                               prefixLen,
    topicIteratorFP                   topicIterator,
    RTEConf_Parameter::UpdateFuncType updateFunc)
{
    SAPDBErr_MessageList err;
    int                  topicIndex;
    SAPDBDiag_Topic  *   topic;
    RTEConf_ParameterAccess * params = RTEConf_ParameterAccess::Instance();

    topicIndex = 0;
    while (topicIterator (topicIndex, &topic)) {
        SAPDB_UTF8 parameterName [32];
        SAPDB_MemCopyNoCheck (parameterName, prefix, sizeof (parameterName));
        SAPDB_MemCopyNoCheck (parameterName + prefixLen, topic->GetName (),
            sizeof (parameterName) - prefixLen);
        params->RegisterUpdateFunction(
            parameterName, updateFunc, NULL, err);
        if (!err.IsEmpty ()) {
            Kernel_OpError (1, csp3_n_param)
                << reinterpret_cast<const char *> (parameterName)
                << " not found in cserv.pcf";
            SAPDBERR_ASSERT_STATE (err.IsEmpty ());
        }
        RTEConf_Parameter::Integer currentValue;
        if (params->GetInteger(parameterName, currentValue, err)) {
            topic->SetTraceLevel (static_cast<int> (currentValue));
        }
        ++topicIndex;
    }
}

/*----------------------------------------------------------------------------*/

/*
 * register update callbacks for check and trace topics
 */
static void g011_register_diagnose_topics ()
{
    g011_generic_register_diagnose_topics (checkPrefixC, checkPrefixLenC,
        Kernel_DiagTopicsIterator, gg011update_check_topic_callback);
    g011_generic_register_diagnose_topics (tracePrefixC, tracePrefixLenC,
        Kernel_TraceTopicsIterator, gg011update_trace_topic_callback);
}

/*----------------------------------------------------------------------------*/

// end of program
