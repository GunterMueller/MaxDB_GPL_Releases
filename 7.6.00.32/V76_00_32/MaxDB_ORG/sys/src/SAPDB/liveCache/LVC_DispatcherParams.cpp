/*!
 * \file    LVC_DispatcherParams.cpp
 * \author  IvanS, ThomasA, FerdiF
 * \brief   OMS Stored Procedure parameter query module.
 */
/*

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



*/

#include "liveCache/LVC_DispatcherParams.hpp"
#include "liveCache/LVC_LoadComponentLibrary.hpp" /* PTS 1121178 FF 19-Mar-2003 */
#include "geo00d.h"
#include "vak001.h"
#include "geo60.h"
#include "geo007_1.h"

/*----------------------------------------------------------------------------*/

typedef char t_part_data[sizeof(tsp00_KnlIdentifier) + 2];

/*----------------------------------------------------------------------------*/

inline static void
LVC_DispGetDebugContext (tak_all_command_glob *acv,
                      tgg91_SessionNo &session,
                      tsp00_Addr      &sink_addr,
                      tsp1_packet_ptr &packet)
{
  session   = acv->a_transinf.tri_trans.trSessionId_gg00;
  sink_addr = REINTERPRET_CAST(tsp00_Addr, &acv->a_kernel_sink);
  packet    = acv->a_dbp_packet_ptr;
}

/*----------------------------------------------------------------------------*/

LVC_DispatcherParams::LVC_DispatcherParams(tsp_dcom_paraminfo_parms *parms)
{
  tsp1_packet_ptr     lpPacket;
  tsp1_part_ptr       lpPart;

  memset (parms->errtext, ' ', sizeof (parms->errtext));
  OutParams = 0;
  Parms     = parms;
  s26first_segment_init ( REINTERPRET_CAST(tsp1_packet_ptr,parms->packet),
    sp1sk_return, lpSegment);
  lpSegment->sp1r_returncode() = 0;
  lpPacket = REINTERPRET_CAST(tsp1_packet_ptr,parms->packet);
  lpPacket->sp1_header.sp1h_varpart_len =   sizeof(lpPacket->sp1_header) +
    sizeof(lpPacket->sp1_segm().sp1s_segm_header());
  s26new_part_init ( REINTERPRET_CAST(tsp1_packet_ptr,parms->packet),
    *lpSegment, lpPart);
  lpPartHdr                     = &lpPart->sp1p_part_header();
  lpPartHdr->sp1p_part_kind.becomes(sp1pk_shortinfo);
  lpPartHdr->sp1p_attributes.clear();
  lpPartHdr->sp1p_arg_count = 0;
  lpParamInfo = REINTERPRET_CAST(tsp1_dcom_param_info *, & lpPart->sp1p_buf());
  //memcpy(&session[0],&parms->session[0],sizeof(session));
}

/*----------------------------------------------------------------------------*/

void LVC_DispatcherParams::InitParamInfo(tsp1_dcom_param_info &paramInfo)
{
  paramInfo.sp1i_dcom_mode.clear();
  paramInfo.sp1i_dcom_mode.addElement(sp1ot_mandatory);
  paramInfo.sp1i_dcom_io_type.becomes(sp1io_inout);
  paramInfo.sp1i_dcom_datatype     = 0;
  paramInfo.sp1i_dcom_sub_datatype = 0;
  paramInfo.sp1i_dcom_length       = 0;
  paramInfo.sp1i_dcom_frac         = 0;
  paramInfo.sp1i_dcom_dim          = 0;
  memset (paramInfo.sp1i_dcom_type_uid, 0, sizeof (paramInfo.sp1i_dcom_type_uid));
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_DispatcherParams::NewParamInfo(int vt)
{
  if (0 < lpPartHdr->sp1p_arg_count)
    if ((long) ( sizeof (tsp1_part_header) /* PTS 1110591 part_header size added */
      + lpPartHdr->sp1p_buf_len 
      + sizeof (tsp1_dcom_param_info)) >
      (long) lpPartHdr->sp1p_buf_size)
      return GEO00D_DCOM_BUFFER_LIMIT;
    else
      lpParamInfo++;
  lpPartHdr->sp1p_arg_count++;
  lpPartHdr->sp1p_buf_len += sizeof (tsp1_dcom_param_info);
  InitParamInfo(*lpParamInfo);
  lpParamInfo->sp1i_dcom_datatype = vt;
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_DispatcherParams::OneParameter(
  tsp_dcom_paraminfo_parms *parms,
  unsigned short           idx)
{
  CHRESULT_EO00D hr(parms->session,"OneParameter");
  char    *lpcMode;
  char    *lpcType;
  char    *lpcName;
  GUID    *lpGuid;

  hr = NewParamInfo(0);
  if (!hr.succeeded())
    return hr.hresult();

  if ( co_IntroSpectFunction_MO )
    co_IntroSpectFunction_MO ( idx, &lpcMode, &lpcType,
    &lpParamInfo->sp1i_dcom_datatype,
    &lpParamInfo->sp1i_dcom_sub_datatype,
    &lpParamInfo->sp1i_dcom_length,
    &lpParamInfo->sp1i_dcom_dim,
    &lpParamInfo->sp1i_dcom_cpp_offset,
    &lpParamInfo->sp1i_dcom_ascii_offset,
    &lpParamInfo->sp1i_dcom_unicode_offset,
    &lpParamInfo->sp1i_dcom_frac,
    &lpGuid,
    &lpcName );
  else
  {
    long offset;
    co_IntroSpectFunction ( idx, &lpcMode, &lpcType,
      &lpParamInfo->sp1i_dcom_datatype,
      &lpParamInfo->sp1i_dcom_sub_datatype,
      &lpParamInfo->sp1i_dcom_length,
      &lpParamInfo->sp1i_dcom_dim,
      &offset,
      &lpParamInfo->sp1i_dcom_frac,
      &lpGuid,
      &lpcName );
    lpParamInfo->sp1i_dcom_cpp_offset      = offset;
    lpParamInfo->sp1i_dcom_ascii_offset    = offset;
    lpParamInfo->sp1i_dcom_unicode_offset  = offset;
  }

  lpParamInfo->sp1i_dcom_mode.clear();
  if ( 0 == strcmp( "MAN", lpcMode ))
    lpParamInfo->sp1i_dcom_mode.addElement(sp1ot_mandatory);
  if ( 0 == strcmp( "OPT", lpcMode ))
    lpParamInfo->sp1i_dcom_mode.addElement(sp1ot_optional);
  if ( 0 == strcmp( "DEF", lpcMode ))
    lpParamInfo->sp1i_dcom_mode.addElement(sp1ot_default);
  if ( 0 == strcmp( "ESC", lpcMode ))
    lpParamInfo->sp1i_dcom_mode.addElement(sp1ot_escape_char);

  if ( 0 == strcmp( "I/O", lpcType ))
    lpParamInfo->sp1i_dcom_io_type.becomes(sp1io_inout);
  if ( 0 == strcmp( "OUT", lpcType ))
    lpParamInfo->sp1i_dcom_io_type.becomes(sp1io_output);
  if ( 0 == strcmp( "IN", lpcType ))
    lpParamInfo->sp1i_dcom_io_type.becomes(sp1io_input);

  memcpy( &lpParamInfo->sp1i_dcom_type_uid[0], lpGuid, sizeof(GUID) );

  return S_OK;
};

/*----------------------------------------------------------------------------*/

HRESULT LVC_DispatcherParams::GetMethod(
                                        tsp_dcom_paraminfo_parms *parms,
                                        unsigned short           *pNumParms,
                                        BOOLEAN                  &is_last_func)
{
  CHRESULT_EO00D hr(parms->session, "LVC_DispatcherParams::GetMethod");
  HINSTANCE      hInstance;
  int            len;
  unsigned short DispID;
  LPCLSID        pClsID;
  LPIID          pIID;
  unsigned short num;
  long           version;
  char*          pc;

  hInstance = (HINSTANCE)parms->typeinfo_ptr;
  co_IntroSpectObject = (LVC_DispIntrospectObject)sql36GetProcAddress(hInstance,"Co_IntrospectObject");
  if ( NULL == co_IntroSpectObject )
  {
    hr = GEO00D_DCOM_INSPEC_ROUTINE_NOT_FOUND;
    MSGD ((ERR_DCOM_HRESULT, "LVC_DispatcherParams::GetMethod:GetProcAddress(Co_IntrospectObject)", hr.hresult() ));
    return hr.hresult();
  }

  co_IntroSpectObject ( &pClsID, &pIID, &num, &version );

  co_IntroSpectMethod = (LVC_DispIntrospectMethod)sql36GetProcAddress(hInstance,"Co_IntrospectMethod");
  if ( NULL == co_IntroSpectMethod )
  {
    hr = GEO00D_DCOM_INSPEC_ROUTINE_NOT_FOUND;
    MSGD ((ERR_DCOM_HRESULT, "LVC_DispatcherParams::GetMethod:GetProcAddress(Co_IntrospectMethod)", hr.hresult() ));
    return hr.hresult();
  }
  if ( co_IntroSpectMethod_MO ) 
    co_IntroSpectMethod_MO ( LVC_Disp_get_by_index, &pc, parms->func_idx - 1, &DispID , pNumParms, (void **)&co_IntroSpectFunction_MO );
  else
    co_IntroSpectMethod ( LVC_Disp_get_by_index, &pc, parms->func_idx - 1, &DispID , pNumParms, (void **)&co_IntroSpectFunction );

  if (( NULL == co_IntroSpectFunction_MO ) && ( NULL == co_IntroSpectFunction ))
  {
    hr = GEO00D_DCOM_METHOD_NOT_FOUND;
    MSGD ((ERR_DCOM_HRESULT, "LVC_DispatcherParams::GetMethod:co_IntroSpectMethod", hr.hresult() ));
    return hr.hresult();
  }
  if ( parms->func_idx  == num ) is_last_func = TRUE;
  parms->dispid = DispID;
  memset (&parms->method[0], ' ', sizeof (parms->method));
  len = (int)strlen (pc);
  if (len > sizeof(parms->method))
    len = sizeof(parms->method);
  memcpy (parms->method, pc, len);

  return 0;
}

/*----------------------------------------------------------------------------*/

HRESULT LVC_DispatcherParams::GetInterface (
  tsp_dcom_paraminfo_parms *parms,
  unsigned short           *pNumParms)
{
  CHRESULT_EO00D hr(parms->session, "GetInterface");

  HINSTANCE      hInstance;
  char           cMethod[sizeof (tsp00_KnlIdentifier) + 1];
  LPCLSID        pClsID;
  LPIID          pIID;
  unsigned short numMethods, dispid;
  long           version;
  char           *pc;
  int            len;

  co_IntroSpectObject      = NULL;
  co_IntroSpectMethod      = NULL;
  co_IntroSpectMethod_MO   = NULL;
  co_IntroSpectFunction    = NULL;
  co_IntroSpectFunction_MO = NULL;

  parms->typeinfo_ptr = NULL;

  tsp00_C32   MD5Mark;
  hr = LVC_LoadComponentLibrary(parms->library_name, parms->package_id, (HANDLE*)&hInstance, MD5Mark);
  if ( S_OK == hr.hresult() )
  {
    if (dbo_package == parms->db_obj_type) 
      memcpy(&parms->MD5_footprint, &MD5Mark, sizeof(MD5Mark));
    else
      if ( memcmp(&parms->MD5_footprint, &MD5Mark, sizeof(MD5Mark)) )
        hr = GEO00D_DCOM_MD5_MARK_DOES_NOT_MATCH; 
  }

  /* map ven36.cpp error code to a HResult */
  if (ERROR_SUCCESS == hr.hresult())
  {
    parms->typeinfo_ptr = (char*)hInstance;

    co_IntroSpectObject = (LVC_DispIntrospectObject)sql36GetProcAddress(hInstance,"Co_IntrospectObject");
    if ( NULL == co_IntroSpectObject )
    {
      hr = GEO00D_DCOM_INSPEC_ROUTINE_NOT_FOUND;
      MSGD ((ERR_DCOM_HRESULT, "eo36Loadtypelibfromprogid:GetProcAddress(Co_IntrospectObject)", hr.hresult() ));
      return hr.hresult();
    }

    co_IntroSpectObject ( &pClsID, &pIID, &numMethods, &version );
    memcpy(cMethod, parms->method, sizeof(parms->method) );
    memcpy (&parms->coclsid[0], pClsID, sizeof (CLSID));   /* PTS 1121178 FF 24-Mar-2003 */
    memcpy (&parms->iid[0], pIID, sizeof (IID));
    pc = strchr( cMethod, ' ' );
    *pc = '\0';

    co_IntroSpectMethod_MO = (LVC_DispIntrospectMethodMO)sql36GetProcAddress(hInstance,"Co_IntrospectMethod_MO");
    if ( NULL == co_IntroSpectMethod_MO )
    {
      co_IntroSpectMethod = (LVC_DispIntrospectMethod)sql36GetProcAddress(hInstance,"Co_IntrospectMethod");
      if ( NULL == co_IntroSpectMethod )
      {
        hr = GEO00D_DCOM_INSPEC_ROUTINE_NOT_FOUND;
        MSGD ((ERR_DCOM_HRESULT, "eo36Loadtypelibfromprogid:GetProcAddress(Co_IntrospectMethod)", hr.hresult() ));
        return hr.hresult();
      }
    }

    pc = &cMethod[0];
    if ( co_IntroSpectMethod_MO ) 
      co_IntroSpectMethod_MO ( LVC_Disp_get_by_name, &pc, 0, &dispid , pNumParms, (void **)&co_IntroSpectFunction_MO );
    else
      co_IntroSpectMethod ( LVC_Disp_get_by_name, &pc, 0, &dispid , pNumParms, (void **)&co_IntroSpectFunction );
    parms->dispid = dispid;
    if (( NULL == co_IntroSpectFunction_MO ) && ( NULL == co_IntroSpectFunction ))
    {
      hr = GEO00D_DCOM_METHOD_NOT_FOUND;
      char msg_buf[128];
      sp77sprintf(msg_buf, sizeof(msg_buf), "eo36Loadtypelibfromprogid:co_IntroSpectMethod(%.64s)",&cMethod[0]);
      MSGD ((ERR_DCOM_HRESULT, msg_buf, hr.hresult() ));
      return hr.hresult();
    }
    memset (&parms->method[0], ' ', sizeof (parms->method));
    len = (int)strlen (pc);
    if (len > sizeof(parms->method))
      len = sizeof(parms->method);
    memcpy (parms->method, pc, len);
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

void LVC_DispatcherParams::DescribeParamNames(
  LVC_DispParamNames  &ParmNames,
  unsigned int        cArgs)
{
  HRESULT hr = ERROR_SUCCESS;  /* PTS 1110591 */
  unsigned int     ix;
  unsigned int     len;
  tsp1_part_ptr    lpPart;
  t_part_data      *lpPart_data;
  tsp1_part_header *lpPartHeader;
  tsp1_dcom_param_info ParamInfo;
  char    *lpcMode;
  char    *lpcType;
  char    *lpcName;
  GUID    *lpGuid;

  s26new_part_init ( REINTERPRET_CAST(tsp1_packet_ptr,Parms->packet),
                     *lpSegment, lpPart);
  lpPartHeader = &lpPart->sp1p_part_header();
  lpPartHeader->sp1p_part_kind.becomes(sp1pk_columnnames);
  lpPartHeader->sp1p_attributes.clear();
  lpPartHeader->sp1p_arg_count = cArgs;
  lpPart_data = (t_part_data *) & lpPart->sp1p_buf();
  lpPartHeader->sp1p_buf_len = 0;
  for (ix = 1; ix < ( cArgs + 1 ); ix++)
  {
    if ( co_IntroSpectFunction_MO )
      co_IntroSpectFunction_MO ( ix, &lpcMode, &lpcType,
      &ParamInfo.sp1i_dcom_datatype,
      &ParamInfo.sp1i_dcom_sub_datatype,
      &ParamInfo.sp1i_dcom_length,
      &ParamInfo.sp1i_dcom_dim,
      &lpParamInfo->sp1i_dcom_cpp_offset,
      &lpParamInfo->sp1i_dcom_ascii_offset,
      &lpParamInfo->sp1i_dcom_unicode_offset,
      &ParamInfo.sp1i_dcom_frac,
      &lpGuid,
      &lpcName );
    else
    {
      long offset;
      co_IntroSpectFunction ( ix, &lpcMode, &lpcType,
      &ParamInfo.sp1i_dcom_datatype,
      &ParamInfo.sp1i_dcom_sub_datatype,
      &ParamInfo.sp1i_dcom_length,
      &ParamInfo.sp1i_dcom_dim,
      &offset,
      &ParamInfo.sp1i_dcom_frac,
      &lpGuid,
      &lpcName );
      lpParamInfo->sp1i_dcom_cpp_offset      = offset;
      lpParamInfo->sp1i_dcom_ascii_offset    = offset;
      lpParamInfo->sp1i_dcom_unicode_offset  = offset;
    }

    len = (int)strlen (lpcName);
    /* +++           PTS 1110591                                          +++ */
    if (( s26partlen (*lpPart) - (int)sizeof (tsp1_part_header) + (int)len ) > 
      lpPart->sp1p_buf_size() )
    {
      hr = GEO00D_DCOM_BUFFER_LIMIT;
      SetError(hr);
    }
    else
    {
    /* ---           PTS 1110591                                        --- */
      (*lpPart_data)[0] = len;
      lpPartHeader->sp1p_buf_len = lpPartHeader->sp1p_buf_len + len + 1;
      lpPart_data = (t_part_data *)(((char *) lpPart_data) + 1);
      memcpy ((char *) lpPart_data, lpcName, len);
      lpPart_data = (t_part_data *)(((char *) lpPart_data) + len);
    }  /* PTS 1110591  */
  }
  if ( ERROR_SUCCESS == hr )  /* PTS 1110591  */
    s26finish_part ( REINTERPRET_CAST(tsp1_packet_ptr,Parms->packet), *lpPart);
}

/*----------------------------------------------------------------------------*/
