/*!
  @file           vos37.cpp
  @author         FerdinandF
  @brief          GenInspectionCode Utility
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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




/* ================================================================================ */
/* ==========              REPLACEMENTS BY THE PREPROCESSOR               ========= */
/* ================================================================================ */

#define MAXPARAMS_OS37 256
#define MAX_BUFFER_LENGTH 64000
#define INITGUID

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */
#ifdef NO_INLINES
# define _INLINE
#else
# define _INLINE	inline
#endif

#define RC_OK                0
#define RC_ERROR             -1
#define PATH_LEN	           256
#define C_OLD_TSP_RTE_HEADER 20
#define MAX_MSG_LINE_LEN     512

#include <stdio.h>
#include <string.h>
#include <objbase.h>
#include <oaidl.h>
#undef  ROUTINE_DBG_MSP00
#include "gsp00.h"
#include "vsp001.h"
#include "liveCacheTypes.h"  /* no check */
#include "ggg00.h"
#include "hos37.h"
#include  "SAPDBCommon/SAPDB_string.h"

#define ERRMSG(_arg)                {sql37_errmsg     _arg;}

DEFINE_GUID(GUID_LC_CHAR, 
            0xA6DE9A83, 0x6173, 0x11d1, 0xA9, 0x43, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_WCHAR, 
            0xA6DE9A84, 0x6173, 0x11d1, 0xA9, 0x43, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_BYTE, 
            0xA6DE9A85, 0x6173, 0x11d1, 0xA9, 0x43, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_BOOL, 
            0xA6DE9A87, 0x6173, 0x11d1, 0xA9, 0x43, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_INT1,
            0x7b0a5bd0, 0x68f1, 0x11d2, 0xa9, 0x80, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31);
DEFINE_GUID(GUID_LC_ABAP_TAB_HANDLE,
            0x33B799A1, 0x85BD, 0x11d1, 0xA9, 0x3F, 0x00, 0xA0, 0xC9, 0x44, 0x90, 0x7B);
DEFINE_GUID(GUID_OMSTYPESTREAMHANDLE, 
            0xee4b31fb, 0xcf89, 0x11d1, 0xa9, 0x55, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31);
DEFINE_GUID(GUID_LC_PARAM_DESC, 
            0x74f2c121, 0x27ec, 0x11d3, 0xa9, 0x96, 0x0, 0xa0, 0xc9, 0x43, 0x7, 0x31);
/* neue Typen mit 7.2.01.05 */
DEFINE_GUID(GUID_LC_UINT1, 
            0x92F1D481, 0xA63E, 0x11d3, 0xa9, 0x97, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_SCHEMA_HANDLE, 
            0xF2AD8221, 0xA641, 0x11d3, 0xa9, 0x97, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);
DEFINE_GUID(GUID_LC_CONTAINER_NO, 
            0xF2AD8222, 0xA641, 0x11d3, 0xa9, 0x97, 0x00, 0xA0, 0xC9, 0x43, 0x11, 0xA5);



#ifdef  _DEBUG
#undef  ROUTINE_DBG_MSP00
#define ROUTINE_DBG_MSP00(x)  printf(">>Routine: "x"\n"); 
#else
#undef  ROUTINE_DBG_MSP00
#define ROUTINE_DBG_MSP00(x)
#endif

static _INLINE void sql37_errmsg (long  lMsgID,
                                  char  *pszFormatStr, ...);

class CHRESULT_OS37
{
private:
  HRESULT         m_hr;
  char            m_ModuleName[64];
  
public:
  CHRESULT_OS37(char* pcModuleName)
  {
    strcpy(&m_ModuleName[0],pcModuleName);
  }

  bool    succeeded()
  {
    if (SUCCEEDED(m_hr)) return true;
    return false;
  }

  bool    succeeded( const char* s)
  {
    if (SUCCEEDED(m_hr)) return true;
    ERRMSG(( HRESULT_ERROR_OS37, m_hr, m_ModuleName, s ));
    return false;
  }

  bool    failed()
  {
    if (FAILED(m_hr)) return true;
    return false;
  }

  bool    failed( const char* s)
  {
    if (FAILED(m_hr)) 
    {
      ERRMSG(( HRESULT_ERROR_OS37, m_hr, m_ModuleName, s ));
      return true;
    }
    else
      return false;
  }

  bool    noError()
  {
    if (m_hr == NO_ERROR) return true;
    return false;
  }

  bool    noError( const char* s)
  {
    if (m_hr == NO_ERROR) return true;
    ERRMSG(( HRESULT_ERROR_OS37, m_hr, m_ModuleName, s ));
    return false;
  }

  HRESULT hresult()
  {
    return m_hr;
  }

  void    operator= (HRESULT hr)
  {
    m_hr = hr;
  };

};


typedef char t_part_data[sizeof(tsp00_KnlIdentifier) + 2];
typedef TYPEATTR             *TYPEATTRPTR;
typedef FUNCDESC             *FUNCDESCPTR;
typedef ELEMDESC             *ELEMDESCPTR; 
typedef tsp1_dcom_param_info *dcom_param_info_ptr;
typedef BSTR                  ParamNames_os37[MAXPARAMS_OS37];
struct tos37_ParamGlob {
    tsp1_segment_ptr            lpSegment;
    tsp1_part_header            *lpPartHdr;
    tsp_dcom_paraminfo_parms    *Parms;
    dcom_param_info_ptr         lpParamInfo;
    dcom_param_info_ptr         lpStreamParamInfo;
    int                         OutParams;
    long                        offset;
    int                         alignment;
    char                        Method_c[sizeof (tsp00_KnlIdentifier) + 1];
};

#define ERROR_NO_STREAM ERROR_FILE_NOT_FOUND

/* ========================================================================== */
/* ==========              DECLARATION OF CONSTANTS                 ========= */
/* ========================================================================== */

/* ========================================================================== */
/* ==========               EXTERN FUNCTION PROTOTYPES              ========= */
/* ========================================================================== */

externC void s26nextpart (tsp1_part_ptr*);

externC void s26first_segment_init (tsp1_packet_ptr, 
                                    tsp1_segment_kind, 
                                    tsp1_segment_ptr*);

externC void s26new_part_init (tsp1_packet_ptr,
                               tsp1_segment*,
                               tsp1_part_ptr*);

externC void s26finish_part (tsp1_packet_ptr,
                             tsp1_part*);

/* ========================================================================== */
/* ==========               LOCAL FUNCTION PROTOTYPES               ========= */
/* ========================================================================== */

typedef  char t_Path[ PATH_LEN ];

static _INLINE HRESULT sql37_paraminfo   (tsp_dcom_paraminfo_parms *parms, 
                                          char* cTypeLib ) ;

static _INLINE HRESULT os37_GenerateOutput ( t_Path cInputTlb,
                                             t_Path cOutputFile,
                                             t_Path cType );

static _INLINE int os37_InitializeOutput ( tsp_dcom_paraminfo_parms* parms, 
                                           t_Path cOutputFile,
                                           FILE **outStream_h,
                                           FILE **outStream_cpp );

static _INLINE HRESULT os37_InitializeTypeLib ( tsp_dcom_paraminfo_parms* parms, 
                                                char* cTypeLib,
                                                t_Path cType );

static _INLINE int os37_ProcessCommandLine ( int argc , 
                                             char * argv[],
                                             t_Path cInputTlb,
                                             t_Path cOutputFile,
                                             t_Path cType );

static _INLINE void os37_ProcInspectInfo ( tsp_dcom_paraminfo_parms* parms, 
                                           int num,
                                           FILE **outStream_h,
                                           FILE **outStream_cpp );

static _INLINE void os37_TerminateOutput ( tsp_dcom_paraminfo_parms* parms,
                                           int num,
                                           FILE **outStream_h,
                                           FILE **outStream_cpp );

static _INLINE HRESULT os37_CheckFirstMethodParam (tos37_ParamGlob &parm_glob);

static _INLINE HRESULT os37_DescribeAlias (tos37_ParamGlob &param_glob, 
                                           ITypeInfo *lpITypeInfo,
                                           TYPEATTR  *lpTypeAttr);

static _INLINE void os37_DescribeParamNames (tos37_ParamGlob &param_glob,
                                             FUNCDESC           *lpFuncDesc,
                                             ParamNames_os37    &ParmNames,
                                             unsigned int       cArgs);

static _INLINE HRESULT os37_DescribeRecord (tos37_ParamGlob &param_glob, 
                                            ITypeInfo *lpITypeInfo,
                                            TYPEATTR  *lpTypeAttr);

static _INLINE HRESULT os37_DescribeStructure (tos37_ParamGlob &param_glob, 
                                               HREFTYPE           user_hRefType);

static _INLINE HRESULT os37_GetAndCheckGuid (tos37_ParamGlob &param_glob, 
                                             ITypeInfo *lpITypeInfo,
                                             HREFTYPE  user_hRefType,
                                             tsp00_C16 &TypeGuid);

static _INLINE HRESULT os37_GetIID (tsp_dcom_paraminfo_parms &parms,
                                    TYPEATTR                 *lpTypeAttr);

static _INLINE HRESULT os37_GetInterface (tsp_dcom_paraminfo_parms &parms,
                                          TYPEATTRPTR              &lpTypeAttr,
                                          FUNCDESCPTR              &lpFuncDesc, 
                                          char                     *cTypeLib);

static _INLINE HRESULT os37_GetMethod (tsp_dcom_paraminfo_parms &parms,
                                       TYPEATTRPTR              &lpTypeAttr,
                                       FUNCDESCPTR              &lpFuncDesc,
                                       BOOLEAN                  &is_last_func);

static _INLINE HRESULT os37_GetTypeAttr (tos37_ParamGlob       &param_glob,
                                         HREFTYPE              user_hRefType,
                                         ITypeInfo             **lpITypeInfo,
                                         TYPEATTR              **lpTypeAttr);

static _INLINE void os37_Init (tos37_ParamGlob       &param_glob, 
                               tsp_dcom_paraminfo_parms *parms);

static _INLINE void os37_InitDcomParamInfo(tsp1_dcom_param_info &paramInfo);

static _INLINE BOOLEAN os37_IsNilGuid (tsp00_C16 &guid);

static _INLINE BOOLEAN os37_IsOmsDatatype (GUID  &guid,
                                           short &datatype);

static _INLINE BOOLEAN os37_IsScalar(int DataType);

static _INLINE BOOLEAN os37_IsStreamType (tos37_ParamGlob       &param_glob, 
                                          ITypeInfo                *lpITypeInfo,
                                          TYPEATTR                 *lpTypeAttr,
                                          CHRESULT_OS37           &hr);

static _INLINE HRESULT os37_NewParamInfo(tos37_ParamGlob &param_glob,
                                         int                vt);

static _INLINE HRESULT os37_OneParameter (tos37_ParamGlob         &param_glob, 
                                          tsp_dcom_paraminfo_parms &parms,
                                          ELEMDESCPTR              &lpParmDesc,
                                          long                     paramNo );

static _INLINE HRESULT os37_PtrType(tos37_ParamGlob          &param_glob, 
                                    tsp_dcom_paraminfo_parms &parms,
                                    ELEMDESCPTR              &lpParmDesc,
                                    long                      paramNo );

static _INLINE void os37_SetError (long e,
                                   tos37_ParamGlob &param_glob);

static _INLINE HRESULT os37_UserDefinedType (tos37_ParamGlob           &param_glob, 
                                             tsp_dcom_paraminfo_parms  &parms,
                                             HREFTYPE                  hreftype,                    
                                             long                      paramNo );

/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */

extern "C" void  sqlabort(){} ;
long  sqlk_rangeviolation ( long              val,
                           long              minVal,
                           long              maxVal,
                           int               lineno,
                           const char       *fileName)
{ int x=0; x=x/1; return -1; };

void main ( int argc , char * argv[] )
{
  long     iRet;

  t_Path cInputTlb;
  t_Path cOutputFile;
  t_Path cType;

  /*
   * Process the command line parameters.
   */
 
  iRet = os37_ProcessCommandLine ( argc , argv, cInputTlb, cOutputFile, cType ) ;
  if ( iRet != RC_OK )
  {
    ERRMSG(( WRONG_PARAMETERS_OS37 ));
  }
  else
    iRet = os37_GenerateOutput( cInputTlb, cOutputFile, cType );

}

void sql37_errmsg (long  lMsgID,
                   char  *pszFormatStr, ...)
{
  char        szMsg    [MAX_MSG_LINE_LEN];
  va_list     args ;

  va_start ( args, pszFormatStr);
  // --- Build Message Text
  vsprintf ( szMsg, pszFormatStr , args );

  va_end ( args );

  printf ("GenInspectionCode Error %d \"%s\"\n", lMsgID, szMsg );
}

HRESULT sql37_paraminfo (tsp_dcom_paraminfo_parms * parms, char* cTypeLib )
{
  ROUTINE_DBG_MSP00 ("sql37k_paraminfo");

  CHRESULT_OS37      hr("sql37k_paraminfo");
  tsp1_packet_ptr    lpPacket;
  ITypeInfo          *lpITypeInfo;
  FUNCDESC           *lpFuncDesc;
  unsigned int       i, cArgs;
  MEMBERID           MemID;
  ParamNames_os37    ParmNames;
  ELEMDESCPTR        lpParmDesc;
  TYPEATTRPTR        pTypeAttr;
  BOOLEAN            is_function  = 1;
  BOOLEAN            is_last_func = FALSE;
  tos37_ParamGlob    parm_glob;
  char               *puc;

  
  //DebugBreak();
  os37_Init (parm_glob, parms);
 
  /*                                                                   */
  /* Visual Basic uses as ProgID: '<Project>.<Class>'                  */
  /*            as Componentname: '<Class>'                            */
  /*            as Interfacename: '_<Class>'                           */
  /*               as Methodname: '<Method>'                           */
  /*                                                                   */
  /* <Project>.<Class> is used as ProgID to be found in the Registry   */
  /*                   from the ProgID the Components CLSID is found   */
  /*                   from the CLSID the TypeLibID is extracted       */
  /*                                                                   */
  /* Within the TypeLib the CLSID is used to localize the Component    */
  /* description.                                                      */
  /* It is assumed, that the Component has ONLY ONE Interface.         */
  /* Then this Interface is searched for the <Method>.                 */
  /*                                                                   */
  lpFuncDesc = NULL;
  pTypeAttr  = NULL;

  if (NULL == parms->typeinfo_ptr)
  {
    hr = os37_GetInterface (*parms, pTypeAttr, lpFuncDesc, cTypeLib);
    if (hr.succeeded() &&  pTypeAttr)
    {
      hr = os37_GetIID (*parms, pTypeAttr);
      SAPDB_memcpy (&parm_glob.Method_c[0], &parms->method[0], sizeof (tsp00_KnlIdentifier));
      parm_glob.Method_c[sizeof (tsp00_KnlIdentifier)] = 0;
      puc = strchr(parm_glob.Method_c, ' ');
      if ( puc ) *puc = '\0';
    }
  }
  else
  {
    hr = os37_GetMethod (*parms, pTypeAttr, lpFuncDesc, is_last_func); 
    if (hr.succeeded()) { 
      MemID       = lpFuncDesc->memid;
      lpITypeInfo = (ITypeInfo *) parms->typeinfo_ptr;
      hr          = lpITypeInfo->GetNames (MemID, ParmNames, MAXPARAMS_OS37, &cArgs);
      hr.succeeded("GetNames");
      SAPDB_memcpy (&parm_glob.Method_c[0], &parms->method[0], sizeof (tsp00_KnlIdentifier));
      parm_glob.Method_c[sizeof (tsp00_KnlIdentifier)] = 0;
      puc = strchr(parm_glob.Method_c, ' ');
      if ( puc ) *puc = '\0';
    }
    if (hr.succeeded()) 
    {
      lpParmDesc = lpFuncDesc->lprgelemdescParam;
      for (i = 1; (i < cArgs) && (hr.succeeded()); i++)   
      {
        hr = os37_OneParameter (parm_glob, *parms, lpParmDesc, i );
        if ((hr.succeeded()) && (dbo_method == parms->db_obj_type) && (1 == i))
          hr = os37_CheckFirstMethodParam (parm_glob);
        if (hr.failed())
          ERRMSG(( INVALID_PARAMETER_OS37, parm_glob.Method_c, i ));
      }
    }
    if (hr.succeeded()) {   
      s26finish_part ((tsp1_packet_ptr)parms->packet, (tsp1_part_ptr) parm_glob.lpPartHdr);
      os37_DescribeParamNames (parm_glob, lpFuncDesc, ParmNames, cArgs);
      lpPacket = (tsp1_packet_ptr)parms->packet;
      lpPacket->sp1_header.sp1h_varpart_len =   sizeof(lpPacket->sp1_header) +
        lpPacket->sp1_segm().sp1s_segm_header().sp1s_segm_len();
    }
    else
      os37_SetError (hr.hresult(), parm_glob);
    lpITypeInfo = (ITypeInfo *) parms->typeinfo_ptr;
    if (NULL != lpFuncDesc)
      lpITypeInfo->ReleaseFuncDesc (lpFuncDesc);
    if (NULL != pTypeAttr)
      lpITypeInfo->ReleaseTypeAttr (pTypeAttr);
    
    if (is_last_func || hr.failed() /*|| (parms->db_obj_type != dbo_method)*/)
    {
      
     if (NULL != lpITypeInfo)
       lpITypeInfo->Release ();
      parms->typeinfo_ptr = NULL;
    }
  }
  return hr.hresult();
}

HRESULT 
sql37k_getinterface (CLSID&             ClsID,
                     tsp00_KnlIdentifier *lpMethod,
                     ITypeLib           *lpITLib,
                     unsigned short     *lpFuncIdx,
                     TYPEATTR           **lplpTypeAttr,
                     FUNCDESC           **lplpFuncDesc,
                     ITypeInfo          **lplpTypeInfo,
                     tsp00_C4           session,
                     tsp00_C64            errtext)
{
  ROUTINE_DBG_MSP00 ("sql37k_getinterface");

  CHRESULT_OS37        hr("sql37k_getinterface");
  ITypeInfo             *lpITypeInfo_co, *lpITypeInfo_i;
  TYPEATTR              *lpTypeAttr_co, *lpTypeAttr_i;
  HREFTYPE              hRefType;
  FUNCDESC              *lpFuncDesc;
  int                   i, len, MethodLen, num;
  BSTR                  wFuncName, wInterfaceName;
  tsp00_KnlIdentifier    Name;
  char                  found = 0;
  char                  Method_c[sizeof (tsp00_KnlIdentifier) + 1];
  TYPEKIND              typekind;

  SAPDB_memcpy (&Method_c[0], lpMethod, sizeof (tsp00_KnlIdentifier));
  Method_c[sizeof (tsp00_KnlIdentifier)] = 0;
  MethodLen = (int)strlen (Method_c);

  do
    MethodLen--;
  while (((*lpMethod)[MethodLen] == ' ') && (MethodLen > 0));
  MethodLen++;

  num = lpITLib->GetTypeInfoCount();
  typekind = TKIND_ENUM;
  for ( i=0; (i < num) && (SUCCEEDED(hr.hresult())) && (typekind != TKIND_COCLASS); )
  {
    hr = lpITLib->GetTypeInfoType (i, &typekind);
    if (typekind != TKIND_COCLASS) i++;
  }
  if ( typekind == TKIND_COCLASS ) 
    hr = lpITLib->GetTypeInfo (i, &lpITypeInfo_co);

  lpITLib->Release ();
  if ( hr.failed("GetTypeInfoOfGuid"))
  {
    if (hr.hresult() == TYPE_E_ELEMENTNOTFOUND)
    {
      printf (errtext, "{%8.8X-%4.4X-%4.4X-%2.2X%2.2X-%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X}\n",
        ClsID.Data1, ClsID.Data2, ClsID.Data3,
        ClsID.Data4[0], ClsID.Data4[1], ClsID.Data4[2],
        ClsID.Data4[3], ClsID.Data4[4], ClsID.Data4[5],
        ClsID.Data4[6], ClsID.Data4[7]);
    }
    return (hr.hresult());
  }
  /* get attributes of Component                                  */
  hr = lpITypeInfo_co->GetTypeAttr (&lpTypeAttr_co);
  if ( hr.failed("GetTypeAttr"))
  {
    lpITypeInfo_co->Release ();
    return (hr.hresult());
  }

  /*
   *  only one Interface is allowed here, because there isn't any 
   *  way yet to specify the interface name within the 
   *  CREATE DBFUNCTION statement.
   */

  if (lpTypeAttr_co->cImplTypes != 1)
  {
    ERRMSG(( TOO_MANY_INTERFACES_OS37 ));
    hr = E_NOTIMPL;
  }
  
  ClsID = lpTypeAttr_co->guid;
  
  if (hr.succeeded())
  {
    hr = lpITypeInfo_co->GetRefTypeOfImplType (0, &hRefType);
    if (hr.succeeded("GetRefTypeOfImplType 0"))
    {
      /* get ITYpeInfo Interfacepointer for Interface description */
      hr = lpITypeInfo_co->GetRefTypeInfo (hRefType, &lpITypeInfo_i);
      if (hr.succeeded("GetRefTypeOfImplType hReftype"))
      {
        hr = lpITypeInfo_i->GetTypeAttr (&lpTypeAttr_i);
        if (hr.succeeded("GetTypeAttr"))
        {
          hr = lpITypeInfo_i->GetDocumentation (MEMBERID_NIL,
            &wInterfaceName,
            NULL,
            NULL,
            NULL);
          if (hr.succeeded("GetDocumentation 1"))
          {
            SysFreeString (wInterfaceName);
            
            /*  search Method */
            for (i = 0; ((i < lpTypeAttr_i->cFuncs) & hr.succeeded()); i++)
            {
              hr = lpITypeInfo_i->GetFuncDesc (i, &lpFuncDesc);
              if (hr.succeeded("GetFuncDesc"))
              {
                hr = lpITypeInfo_i->GetDocumentation (lpFuncDesc->memid,
                  &wFuncName,
                  NULL,
                  NULL,
                  NULL);
                if (hr.succeeded("GetDocumentation 2"))
                {
                  len = (int)wcslen (wFuncName);
                  if (len <= sizeof(Name)) {
                    wcstombs ((char*)&Name[0], wFuncName, len);
                    SysFreeString (wFuncName);
                    {
                      /* function found */
                      *lpFuncIdx = i + 3;       /* + 3 IUnknown functions */
                      *lplpTypeAttr = lpTypeAttr_i;
                      *lplpFuncDesc = lpFuncDesc;
                      *lplpTypeInfo = lpITypeInfo_i;
                      found = 1;
                      break;
                    }
                  }
                }
                lpITypeInfo_i->ReleaseFuncDesc (lpFuncDesc);
              }
            }
          }
        }
        if (found == 0)
        {
          lpITypeInfo_i->ReleaseTypeAttr (lpTypeAttr_i);
          lpITypeInfo_i->Release ();
        }
      }
    }
  }
  lpITypeInfo_co->ReleaseTypeAttr (lpTypeAttr_co);
  lpITypeInfo_co->Release ();
  
  if ((found != 1) && (*lpMethod)[0] != ' ')
  {
    SAPDB_memcpy (errtext, lpMethod, MethodLen);
    ERRMSG(( METHOD_NOT_FOUND_OS37 ));
    hr = DISP_E_MEMBERNOTFOUND;
  }
  return (hr.hresult());
}

/* ========================================================================== */
/* ==========           IMPLEMENTATION OF LOCAL FUNCTIONS           ========= */
/* ========================================================================== */

void os37_Align (long &value, int alignment)
{
    if (value % alignment != 0) {
        value += (alignment - value % alignment);
    }
}

/*----------------------------------------------------------------------------*/

HRESULT os37_CheckFirstMethodParam (tos37_ParamGlob &parm_glob) 
{
  ROUTINE_DBG_MSP00 ("os37_CheckFirstMethodParam");
  
  if (os37_IsNilGuid (parm_glob.Parms->udttypeguid)) 
  {
    if (VT_USERDEFINED != parm_glob.lpParamInfo->sp1i_dcom_datatype)
    {
      ERRMSG(( P1_NO_USERDEFINED_TYPE_OS37, parm_glob.Method_c, 
        parm_glob.lpParamInfo->sp1i_dcom_datatype));
      return STG_E_INVALIDPARAMETER;
    }
    if (sp1io_input == parm_glob.lpParamInfo->sp1i_dcom_io_type)
    {
      ERRMSG(( P1_IO_TYPE_INPUT_OS37, parm_glob.Method_c )); 
      return STG_E_INVALIDPARAMETER;
    }
    parm_glob.Parms->udttypeguid = parm_glob.lpParamInfo->sp1i_dcom_type_uid; 
  }
  else 
  {
    if (sp1io_output == parm_glob.lpParamInfo->sp1i_dcom_io_type)
    {
      ERRMSG(( P1_IO_TYPE_OUTPUT_OS37, parm_glob.Method_c )); 
      return STG_E_INVALIDPARAMETER;
    }
    if (parm_glob.Parms->udttypeguid != parm_glob.lpParamInfo->sp1i_dcom_type_uid)
    {
      ERRMSG(( P1_WRONG_GUID_OS37, parm_glob.Method_c )); 
      return STG_E_INVALIDPARAMETER;
    }
  }
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT os37_DescribeAlias (tos37_ParamGlob &param_glob, 
                            ITypeInfo *lpITypeInfo,
                            TYPEATTR  *lpTypeAttr)
{
  ROUTINE_DBG_MSP00 ("os37_DescribeAlias");
  
  CHRESULT_OS37        hr("os37_DescribeAlias");
  TYPEATTR  *lpArrayTypeAttr;
  
  hr = S_OK;
  if  (os37_IsScalar(lpTypeAttr->tdescAlias.vt)) { 
    param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
    hr = os37_NewParamInfo(param_glob, lpTypeAttr->tdescAlias.vt);
  }
  else 
  {
    switch (lpTypeAttr->tdescAlias.vt) 
    {
    case VT_CARRAY :
      if (1 == lpTypeAttr->tdescAlias.lpadesc->cDims) 
      {
        switch (lpTypeAttr->tdescAlias.lptdesc->vt) {
        case VT_I1  :
        case VT_UI1 :
          param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
          hr = os37_NewParamInfo(param_glob, VT_CARRAY);
          if (hr.succeeded()) {
            param_glob.lpParamInfo->sp1i_dcom_sub_datatype =
              lpTypeAttr->tdescAlias.lptdesc->vt;
            param_glob.lpParamInfo->sp1i_dcom_dim = 
              (short) lpTypeAttr->tdescAlias.lpadesc->rgbounds[0].cElements;
            param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
            param_glob.offset += param_glob.lpParamInfo->sp1i_dcom_dim;
          }
          break;
        case VT_USERDEFINED :
          hr = os37_GetTypeAttr (param_glob, 
            lpTypeAttr->tdescAlias.lptdesc->hreftype, &lpITypeInfo, &lpArrayTypeAttr);
          if (hr.succeeded()) 
          {
            if ((TKIND_ALIAS == lpArrayTypeAttr->typekind) &&
              ((VT_I1 == lpArrayTypeAttr->tdescAlias.vt) || (VT_UI1 == lpArrayTypeAttr->tdescAlias.vt))) {
              param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
              hr = os37_NewParamInfo(param_glob, VT_CARRAY);
              if (hr.succeeded()) {
                if (!os37_IsOmsDatatype (lpArrayTypeAttr->guid,
                  param_glob.lpParamInfo->sp1i_dcom_sub_datatype))
                  param_glob.lpParamInfo->sp1i_dcom_sub_datatype =
                  lpArrayTypeAttr->tdescAlias.vt;
                param_glob.lpParamInfo->sp1i_dcom_dim = 
                  (short) lpTypeAttr->tdescAlias.lpadesc->rgbounds[0].cElements;
                param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
                param_glob.offset += param_glob.lpParamInfo->sp1i_dcom_dim;
              }
            }
            else
            {
              ERRMSG(( UNSUPPORTED_VT_CARRAY1_OS37, param_glob.Method_c, lpArrayTypeAttr->typekind )); 
              hr = STG_E_INVALIDPARAMETER;
            }
            lpITypeInfo->ReleaseTypeAttr(lpArrayTypeAttr);
            lpITypeInfo->Release();
            break;
          }
        default :
          ERRMSG(( UNSUPPORTED_VT_CARRAY2_OS37, param_glob.Method_c, lpTypeAttr->tdescAlias.lptdesc->vt )); 
          hr = STG_E_INVALIDPARAMETER;
        }
      } 
      else
      {
        ERRMSG(( UNSUPPORTED_VT_CARRAY3_OS37, param_glob.Method_c, lpTypeAttr->tdescAlias.lpadesc->cDims ));
        hr = STG_E_INVALIDPARAMETER;
      }
      break;
    case VT_USERDEFINED :
      hr = os37_DescribeStructure (param_glob, lpTypeAttr->tdescAlias.hreftype);
      break;
    default :
      ERRMSG(( UNSUPPORTED_VT_TYPE_OS37, param_glob.Method_c, lpTypeAttr->tdescAlias.vt ));
      hr = STG_E_INVALIDPARAMETER;
    }
  }
  hr.succeeded();
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

void os37_DescribeParamNames (tos37_ParamGlob &param_glob,
                             FUNCDESC           *lpFuncDesc,
                             ParamNames_os37    &ParmNames,
                             unsigned int       cArgs)
{
  ROUTINE_DBG_MSP00 ("os37_DescribeParamNames");
  
  unsigned int     ix;
  unsigned int     len;
  tsp1_part_ptr    lpPart;
  t_part_data      *lpPart_data;
  tsp1_part_header *lpPartHdr;
  ELEMDESCPTR      lpParmDesc;
  
  s26new_part_init ((tsp1_packet_ptr)param_glob.Parms->packet, param_glob.lpSegment, &lpPart);
  lpPartHdr = &lpPart->sp1p_part_header();
  lpPartHdr->sp1p_part_kind.becomes(sp1pk_columnnames);
  lpPartHdr->sp1p_attributes.clear();
  lpPartHdr->sp1p_arg_count = cArgs;
  lpPart_data = (t_part_data *) & lpPart->sp1p_buf();
  lpParmDesc = lpFuncDesc->lprgelemdescParam;
  lpPartHdr->sp1p_buf_len = 0;
  for (ix = 1; ix < cArgs; ix++) {
    len = (int)wcslen (ParmNames[ix]);
    (*lpPart_data)[0] = len;
    lpPartHdr->sp1p_buf_len = lpPartHdr->sp1p_buf_len + len + 1;
    lpPart_data = (t_part_data *)(((char *) lpPart_data) + 1);
    wcstombs ((char *) lpPart_data, ParmNames[ix], len);
    lpPart_data = (t_part_data *)(((char *) lpPart_data) + len);
    SysFreeString (ParmNames[ix]);
  }
  s26finish_part ((tsp1_packet_ptr)param_glob.Parms->packet, lpPart);
}

/*----------------------------------------------------------------------------*/

HRESULT os37_DescribeRecord (tos37_ParamGlob &param_glob, 
                             ITypeInfo *lpITypeInfo,
                             TYPEATTR  *lpTypeAttr)
{
  ROUTINE_DBG_MSP00 ("os37_DescribeRecord");
  
  CHRESULT_OS37        hr("os37_DescribeRecord");
  long      ix;
  long      size;
  long      start_offset;
  long      old_alignment;
  long      alignment;
  TYPEATTR  *lpArrayTypeAttr;
  VARDESC   *lpVarDesc;
  ITypeInfo *lpITypeInfo_loc;        /*+CHG*+*/
  
  size                 = lpTypeAttr->cbSizeInstance;
  alignment            = lpTypeAttr->cbAlignment;
  old_alignment        = param_glob.alignment;
  param_glob.alignment = alignment;
  os37_Align (param_glob.offset, alignment);
  start_offset = param_glob.offset; 
  for (ix = 0; ix < lpTypeAttr->cVars; ix++) {
    hr = lpITypeInfo->GetVarDesc (ix, &lpVarDesc);
    if (!hr.succeeded("GetVarDesc"))
      break;
    if (os37_IsScalar(lpVarDesc->elemdescVar.tdesc.vt)) { 
      param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
      hr = os37_NewParamInfo(param_glob, lpVarDesc->elemdescVar.tdesc.vt);
    }
    else {
      switch (lpVarDesc->elemdescVar.tdesc.vt) {
      case VT_USERDEFINED :
        hr = os37_DescribeStructure (param_glob, lpVarDesc->elemdescVar.tdesc.hreftype);
        break;
      case VT_CARRAY :
        if (1 == lpVarDesc->elemdescVar.tdesc.lpadesc->cDims) {
          switch (lpVarDesc->elemdescVar.tdesc.lptdesc->vt) {
          case VT_I1  :
          case VT_UI1 :
            param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
            hr = os37_NewParamInfo(param_glob, VT_CARRAY);
            if (hr.succeeded()) {
              param_glob.lpParamInfo->sp1i_dcom_sub_datatype =
                lpVarDesc->elemdescVar.tdesc.lptdesc->vt;
              param_glob.lpParamInfo->sp1i_dcom_dim = 
                (short) lpVarDesc->elemdescVar.tdesc.lpadesc->rgbounds[0].cElements;
              param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
              param_glob.offset += param_glob.lpParamInfo->sp1i_dcom_dim;
            }
            break;
          case VT_USERDEFINED :
            lpITypeInfo_loc = lpITypeInfo;  /*+CHG+*/
            lpITypeInfo_loc->AddRef();      /*+CHG+*/
            hr = os37_GetTypeAttr (param_glob, 
              lpVarDesc->elemdescVar.tdesc.lptdesc->hreftype, &lpITypeInfo, &lpArrayTypeAttr);
            if (hr.succeeded()) {
              if ((TKIND_ALIAS == lpArrayTypeAttr->typekind) &&
                ((VT_I1 == lpArrayTypeAttr->tdescAlias.vt) || (VT_UI1 == lpArrayTypeAttr->tdescAlias.vt))) 
              {
                param_glob.lpStreamParamInfo->sp1i_dcom_dim++;
                hr = os37_NewParamInfo(param_glob, VT_CARRAY);
                if (hr.succeeded()) {
                  if (!os37_IsOmsDatatype (lpArrayTypeAttr->guid,
                    param_glob.lpParamInfo->sp1i_dcom_sub_datatype))
                    param_glob.lpParamInfo->sp1i_dcom_sub_datatype =
                    lpArrayTypeAttr->tdescAlias.vt;
                  param_glob.lpParamInfo->sp1i_dcom_dim =
                    (short) lpVarDesc->elemdescVar.tdesc.lpadesc->rgbounds[0].cElements;
                  param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
                  param_glob.offset += param_glob.lpParamInfo->sp1i_dcom_dim;
                }
              }
              else
              {
                ERRMSG(( UNSUPPORTED_VT_CARRAY1_OS37, param_glob.Method_c, lpArrayTypeAttr->typekind )); 
                hr = STG_E_INVALIDPARAMETER;
              }
              lpITypeInfo->ReleaseTypeAttr(lpArrayTypeAttr);
              lpITypeInfo->Release();
              lpITypeInfo = lpITypeInfo_loc; /*+CHG+*/
              break;
            }
          default :
            ERRMSG(( UNSUPPORTED_VT_CARRAY2_OS37, param_glob.Method_c, lpTypeAttr->tdescAlias.lptdesc->vt )); 
            hr = STG_E_INVALIDPARAMETER;
          }
        } 
        else
        {
          ERRMSG(( UNSUPPORTED_VT_CARRAY3_OS37, param_glob.Method_c, lpVarDesc->elemdescVar.tdesc.lpadesc->cDims ));
          hr = STG_E_INVALIDPARAMETER;
        }
        break;
      default :
        ERRMSG(( UNSUPPORTED_VT_TYPE_OS37, param_glob.Method_c, lpTypeAttr->tdescAlias.vt ));
        hr = STG_E_INVALIDPARAMETER;
      }
      
    }
    lpITypeInfo->ReleaseVarDesc(lpVarDesc);
    if (!hr.succeeded())
      return hr.hresult();
  }
/*  if (param_glob.offset - start_offset != size) {
    hr = STG_E_INVALIDPARAMETER; 
    return hr.hresult();
  }   ????????*/ 
  param_glob.alignment = old_alignment;
  os37_Align (param_glob.offset, alignment);
//  param_glob.offset += start_offset + size - param_glob.offset;
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT os37_DescribeStructure (tos37_ParamGlob &param_glob, 
                                HREFTYPE        user_hRefType)
{
  ROUTINE_DBG_MSP00 ("os37_DescribeStructure");
  
  CHRESULT_OS37        hr("os37_DescribeStructure");
  ITypeInfo *lpITypeInfo;
  TYPEATTR  *lpTypeAttr;
  
  lpITypeInfo = (ITypeInfo *) param_glob.Parms->typeinfo_ptr; 
  hr = os37_GetTypeAttr (param_glob, user_hRefType, &lpITypeInfo, &lpTypeAttr);
  if (hr.succeeded()) {
    switch (lpTypeAttr->typekind) {
    case TKIND_ALIAS :
      hr = os37_DescribeAlias (param_glob, lpITypeInfo, lpTypeAttr);
      break;
    case TKIND_RECORD :
      hr = os37_DescribeRecord (param_glob, lpITypeInfo, lpTypeAttr);
      break;
    default :
      ERRMSG(( RECORD_EXSPECTED_OS37,  param_glob.Method_c, lpTypeAttr->typekind ));
      hr = STG_E_INVALIDPARAMETER;
      hr.succeeded();
    }
    lpITypeInfo->ReleaseTypeAttr (lpTypeAttr);
    lpITypeInfo->Release();
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_GetAndCheckGuid (tos37_ParamGlob &param_glob, 
                              ITypeInfo *lpITypeInfo,
                              HREFTYPE  user_hRefType,
                              tsp00_C16 &TypeGuid)
{
  ROUTINE_DBG_MSP00 ("os37_GetAndCheckGuid");
  
  CHRESULT_OS37        hr("os37_GetAndCheckGuid");
  TYPEATTR *lpTypeAttr;
  
  hr = lpITypeInfo->GetRefTypeInfo (user_hRefType, &lpITypeInfo);
  if (hr.succeeded("GetRefTypeInfo")) {
    hr = lpITypeInfo->GetTypeAttr (&lpTypeAttr);
    if (hr.succeeded("GetTypeAttr")) {
      if (0 == memcmp (&lpTypeAttr->guid,
        "\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00",
        sizeof (&lpTypeAttr->guid)))
      {
        ERRMSG(( MISSING_GUID_OS37,  param_glob.Method_c ));
        hr = STG_E_INVALIDPARAMETER;
      }
      else 
        SAPDB_memcpy (&TypeGuid, &lpTypeAttr->guid, sizeof(TypeGuid));
      lpITypeInfo->ReleaseTypeAttr(lpTypeAttr);
    }
    lpITypeInfo->Release();
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_GetIID (tsp_dcom_paraminfo_parms &parms,
                    TYPEATTR                 *lpTypeAttr)
{
  ROUTINE_DBG_MSP00 ("os37_GetIID");
  
  CHRESULT_OS37 hr("os37_GetIID");
  ITypeInfo      *lpITypeInfo;
  HREFTYPE       hRefType;
  ITypeInfo      *lpITypeInfo_i;
  TYPEATTR       *lpTypeAttr_i;
  
  hr = S_OK;
  lpITypeInfo = (ITypeInfo *) parms.typeinfo_ptr;
  if (true /*PTS 1112663 parms.in_proc_server*/) {
    hr = lpITypeInfo->GetRefTypeOfImplType (0, &hRefType);
    if (hr.succeeded("GetRefTypeOfImplType"))
      /* get ITYpeInfo Interfacepointer for Interface description */
      hr = lpITypeInfo->GetRefTypeInfo (hRefType, &lpITypeInfo_i);
    if (hr.succeeded("GetRefTypeInfo")) {
      hr = lpITypeInfo_i->GetTypeAttr (&lpTypeAttr_i);
      if (hr.succeeded("GetTypeAttr")) {
        SAPDB_memcpy (&parms.iid[0], &lpTypeAttr->guid, sizeof (IID));
        lpITypeInfo_i->ReleaseTypeAttr (lpTypeAttr_i);
      }
      lpITypeInfo_i->Release ();
    }
  }
  else
    SAPDB_memcpy (&parms.iid[0], &lpTypeAttr->guid, sizeof (IID));
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_GetInterface (tsp_dcom_paraminfo_parms &parms,
                          TYPEATTRPTR              &lpTypeAttr,
                          FUNCDESCPTR              &lpFuncDesc, 
                          char                     *cTypeLib)
{
  ROUTINE_DBG_MSP00 ("os37_GetInterface");
  
  CHRESULT_OS37 hr("os37_GetInterface");
  unsigned short DispID;
  ITypeLib       *lpITLib;
  
  int            ix;
  CLSID          ClsID;
  wchar_t		     wszValue[256];			
  
  parms.typeinfo_ptr = NULL;
  
  /*
  * Load TypeLib
  */
  for ( ix = 0; ix <= (int)strlen(cTypeLib); ix++ ) 
    wszValue[ix] = cTypeLib[ix]; 
  
  hr = LoadTypeLib( &wszValue[0], &lpITLib );
  
  if (ERROR_SUCCESS == hr.hresult()) {
    
    
    hr = sql37k_getinterface (ClsID,
      (tsp00_KnlIdentifier *) & parms.method,
      lpITLib,
      &DispID,
      &lpTypeAttr,
      &lpFuncDesc,
      (ITypeInfo **)&parms.typeinfo_ptr,
      parms.session,
      parms.errtext);
    parms.dispid = DispID;
  }
  else 
    ERRMSG(( LOADTYPELIB_ERROR_OS37,  hr.hresult(), &cTypeLib[0] ));
  
  SAPDB_memcpy (&parms.coclsid[0], &ClsID, sizeof (CLSID));
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_GetMethod (tsp_dcom_paraminfo_parms &parms,
                       TYPEATTRPTR              &lpTypeAttr,
                       FUNCDESCPTR              &lpFuncDesc,
                       BOOLEAN                  &is_last_func)
{
  ROUTINE_DBG_MSP00 ("os37_GetMethod");
  
  CHRESULT_OS37 hr("os37_GetMethod");
  int            len;
  int            l_FuncIdx;
  unsigned short DispID;
  ITypeInfo      *lpITypeInfo;
  BSTR           wFuncName;
  
  lpITypeInfo = (ITypeInfo *) parms.typeinfo_ptr;
  hr = lpITypeInfo->GetTypeAttr (&lpTypeAttr);
  if (hr.succeeded("GetTypeAttr")) {
    l_FuncIdx    = parms.func_idx - 1;
    DispID       = l_FuncIdx + 3;
    parms.dispid = DispID;
    if (l_FuncIdx == (lpTypeAttr->cFuncs - 1))
      is_last_func = TRUE;
    hr = lpITypeInfo->GetFuncDesc (l_FuncIdx, &lpFuncDesc);
    hr.succeeded("GetFuncDesc"); 
  }
  if (hr.succeeded()) 
  {
    hr = lpITypeInfo->GetDocumentation (lpFuncDesc->memid,
      &wFuncName,
      NULL,
      NULL,
      NULL);
    hr.succeeded("GetDocumentation"); 
  }
  if (hr.succeeded()) {
    memset (&parms.method[0], ' ', sizeof (parms.method));
    len = (int)wcslen (wFuncName);
    if (len > sizeof(parms.method))
      len = sizeof(parms.method);
    wcstombs ((char*)&parms.method[0], wFuncName, len);
    SysFreeString (wFuncName);
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_GetTypeAttr (tos37_ParamGlob       &param_glob,
                          HREFTYPE              user_hRefType,
                          ITypeInfo             **lpITypeInfo,
                          TYPEATTR              **lpTypeAttr)
{
  ROUTINE_DBG_MSP00 ("os37_GetTypeAttr");
  
  CHRESULT_OS37 hr("os37_GetTypeAttr");
  short     datatype;
  ITypeInfo *lpITypeInfo_loc; /* *CHG* */
  
  hr = S_OK;
  if (user_hRefType != -1) {
    lpITypeInfo_loc = *lpITypeInfo; /* *CHG* */
    hr = lpITypeInfo_loc->GetRefTypeInfo (user_hRefType, lpITypeInfo);
    /* hr = (*lpITypeInfo)->GetRefTypeInfo (user_hRefType, lpITypeInfo); *CHG* */
    if (hr.succeeded("GetRefTypeInfo")) { 
      hr = (*lpITypeInfo)->GetTypeAttr (lpTypeAttr);
      if (!hr.succeeded("GetTypeAttr")) {
        /* (*lpITypeInfo)->Release(); *CHG */
        lpITypeInfo_loc->Release(); /* CHG */
        lpITypeInfo = NULL;
        return hr.hresult();
      }
      if ((TKIND_ALIAS == (*lpTypeAttr)->typekind)             &&
        (VT_CARRAY != (*lpTypeAttr)->tdescAlias.vt)           &&
        (!os37_IsScalar((*lpTypeAttr)->tdescAlias.vt))        && 
        ((*lpTypeAttr)->tdescAlias.hreftype != user_hRefType) &&
        (!os37_IsOmsDatatype ((*lpTypeAttr)->guid, datatype))) {
        user_hRefType = (*lpTypeAttr)->tdescAlias.hreftype;
        (*lpITypeInfo)->ReleaseTypeAttr(*lpTypeAttr);
        /* (*lpITypeInfo)->Release(); *CHG* */
        hr = os37_GetTypeAttr (param_glob, user_hRefType, lpITypeInfo, lpTypeAttr);
      }
    }
    if ((ITypeInfo *) (param_glob.Parms->typeinfo_ptr) != lpITypeInfo_loc ) /*+*CHG*+*/
      lpITypeInfo_loc->Release(); /*+CHG+*/
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

void os37_Init (tos37_ParamGlob       &param_glob, 
                   tsp_dcom_paraminfo_parms *parms)
{
  ROUTINE_DBG_MSP00 ("os37_Init");
  
  tsp1_segment_kind   segm_kind;
  tsp1_packet_ptr     lpPacket;
  tsp1_part_ptr       lpPart;   
  char                *puc;
  
  memset (parms->errtext, ' ', sizeof (parms->errtext));
  param_glob.OutParams = 0;
  param_glob.offset    = 0;
  param_glob.alignment = 4;
  param_glob.Parms     = parms;
  segm_kind.becomes(sp1sk_return);
  s26first_segment_init ((tsp1_packet_ptr)parms->packet, segm_kind, &param_glob.lpSegment);
  param_glob.lpSegment->sp1r_returncode() = 0; 
  lpPacket = (tsp1_packet_ptr)parms->packet;
  lpPacket->sp1_header.sp1h_varpart_len =   sizeof(lpPacket->sp1_header) +
    sizeof(lpPacket->sp1_segm().sp1s_segm_header());
  lpPacket->sp1_header.sp1h_varpart_size = MAX_BUFFER_LENGTH - lpPacket->sp1_header.sp1h_varpart_len;
  s26new_part_init ((tsp1_packet_ptr) parms->packet, param_glob.lpSegment, &lpPart);
  param_glob.lpPartHdr                     = &lpPart->sp1p_part_header();
  param_glob.lpPartHdr->sp1p_part_kind.becomes(sp1pk_shortinfo);
  param_glob.lpPartHdr->sp1p_attributes.clear();
  param_glob.lpPartHdr->sp1p_arg_count = 0;
  param_glob.lpParamInfo = (tsp1_dcom_param_info *) & lpPart->sp1p_buf();
  SAPDB_memcpy (&param_glob.Method_c[0], &parms->method[0], sizeof (tsp00_KnlIdentifier));
  param_glob.Method_c[sizeof (tsp00_KnlIdentifier)] = 0;
  puc = strchr(param_glob.Method_c, ' ');
  if ( puc ) *puc = '\0';
}

/*----------------------------------------------------------------------------*/

void os37_InitDcomParamInfo(tsp1_dcom_param_info &paramInfo) 
{
  ROUTINE_DBG_MSP00 ("os37_InitDcomParamInfo");
  
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

BOOLEAN os37_IsNilGuid (tsp00_C16 &guid) 
{
  ROUTINE_DBG_MSP00 ("os37_IsNilGuid");

  return ((0 == memcmp (&guid,
    "\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00\00",
    sizeof (guid))) ? TRUE : FALSE);
}

/*----------------------------------------------------------------------------*/

BOOLEAN os37_IsOmsDatatype (GUID  &guid,
                           short &datatype)
{
  ROUTINE_DBG_MSP00 ("os37_IsOmsDatatype");
  
  if (0 == memcmp (&GUID_LC_CHAR, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_char;
    return TRUE;
  }
  if (0 == memcmp (&GUID_LC_WCHAR, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_wchar;
    return TRUE;
  }
  if (0 == memcmp (&GUID_LC_BYTE, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_byte;
    return TRUE;
  }
  if (0 == memcmp (&GUID_LC_BOOL, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_bool;
    return TRUE;
  }
  if (0 == memcmp (&GUID_LC_INT1, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_int1;
    return TRUE;
  }
  if (0 == memcmp (&GUID_LC_UINT1, &guid, sizeof(guid))) {                                                         
    datatype = csp1_vt_lc_uint1;
    return TRUE;
  }
  return FALSE;
}

/*----------------------------------------------------------------------------*/

BOOLEAN os37_IsScalar(int DataType) 
{
  ROUTINE_DBG_MSP00 ("os37_IsScalar");

  switch (DataType) 
  {
  case VT_I1:
  case VT_UI1:
  case VT_I2:
  case VT_UI2:
  case VT_I4:
  case VT_UI4:
  case VT_I8:
  case VT_UI8:
  case VT_INT:
  case VT_UINT:
  case VT_R4:
  case VT_R8:
    return TRUE;
  default :
    return FALSE;
  }
}

/*----------------------------------------------------------------------------*/

BOOLEAN os37_IsStreamType (tos37_ParamGlob          &param_glob, 
                           ITypeInfo                *lpITypeInfo,
                           TYPEATTR                 *lpTypeAttr,
                           CHRESULT_OS37           &hr)
{
  ROUTINE_DBG_MSP00 ("os37_IsStreamType");

  int        dcom_datatype;
  tsp00_C16  guid;
  VARDESC    *lpVarDesc1;
  VARDESC    *lpVarDesc2;
  HREFTYPE   user_hRefType;
  
  if (dbo_dbproc != param_glob.Parms->db_obj_type) return FALSE;
  if (2 != lpTypeAttr->cVars) return FALSE;
  hr = lpITypeInfo->GetVarDesc (0, &lpVarDesc1);
  if (hr.succeeded()) 
  {
    if (VT_PTR != lpVarDesc1->elemdescVar.tdesc.vt)
      hr = ERROR_NO_STREAM;
    else
      if (VT_USERDEFINED != lpVarDesc1->elemdescVar.tdesc.lptdesc->vt)
        hr = ERROR_NO_STREAM;
      else
        hr = lpITypeInfo->GetVarDesc (1, &lpVarDesc2);
      if (hr.hresult() == 0) 
      {
        if (VT_USERDEFINED != lpVarDesc2->elemdescVar.tdesc.vt)
          hr = ERROR_NO_STREAM;
        else 
        {
          hr = os37_GetAndCheckGuid (param_glob, lpITypeInfo, 
            lpVarDesc2->elemdescVar.tdesc.hreftype, guid);
          if ( STG_E_INVALIDPARAMETER == hr.hresult() )
            hr = ERROR_NO_STREAM;
          else {
            if (0 == memcmp (&GUID_LC_ABAP_TAB_HANDLE, &guid, sizeof(guid))) {
              dcom_datatype = csp1_vt_lc_abap_tab_handle;
            }
            else {
              if (0 == memcmp (&GUID_OMSTYPESTREAMHANDLE, &guid, sizeof(guid))) {
                dcom_datatype = csp1_vt_lc_stream_handle;
              }
              else {
                hr = ERROR_NO_STREAM;
              }
            }
          }
        }
        lpITypeInfo->ReleaseVarDesc (lpVarDesc2);
        
        param_glob.lpParamInfo->sp1i_dcom_datatype = dcom_datatype;
        param_glob.lpStreamParamInfo               = param_glob.lpParamInfo;
        user_hRefType = lpVarDesc1->elemdescVar.tdesc.lptdesc->hreftype;
        lpITypeInfo->ReleaseVarDesc (lpVarDesc1);
        hr = os37_DescribeStructure (param_glob, user_hRefType); 
      }
  }
  if (ERROR_NO_STREAM == hr.hresult()) 
  {
    hr = S_OK;
    return FALSE;
  }
  else
    return (S_OK == hr.hresult());
}
                          
/*----------------------------------------------------------------------------*/

HRESULT os37_NewParamInfo(tos37_ParamGlob &param_glob,
                          int                vt)
{
  ROUTINE_DBG_MSP00 ("os37_NewParamInfo");

  long alignment;
  if (0 < param_glob.lpPartHdr->sp1p_arg_count) {
    if ((long) (param_glob.lpPartHdr->sp1p_buf_len + sizeof (tsp1_dcom_param_info)) >
      (long) param_glob.lpPartHdr->sp1p_buf_size) {
      return STG_E_INVALIDPARAMETER; 
    }
    else {
      param_glob.lpParamInfo++;
    }
  }
  param_glob.lpPartHdr->sp1p_arg_count++;
  param_glob.lpPartHdr->sp1p_buf_len += sizeof (tsp1_dcom_param_info);
  os37_InitDcomParamInfo(*param_glob.lpParamInfo);
  param_glob.lpParamInfo->sp1i_dcom_datatype = vt;
  switch (vt) {
  case VT_I1:
  case VT_UI1:
    param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
    param_glob.offset += 1;
    break;
  case VT_I2 :
  case VT_UI2:
    if (param_glob.alignment < 2) {
      alignment = param_glob.alignment;
    }
    else {
      alignment = 2;
    }
    os37_Align (param_glob.offset, alignment);
    param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
    param_glob.offset += 2;
    break;
  case VT_I4   :
  case VT_UI4  :
  case VT_INT  :
  case VT_UINT :
  case VT_R4   :
    if (param_glob.alignment < 4) {
      alignment = param_glob.alignment;
    }
    else {
      alignment = 4;
    }
    os37_Align (param_glob.offset, alignment);
    param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
    param_glob.offset += 4;
    break;
  case VT_I8:
  case VT_UI8:
  case VT_R8:
    if (param_glob.alignment < 8) {
      alignment = param_glob.alignment;
    }
    else {
      alignment = 8;
    }
    os37_Align (param_glob.offset, alignment);
    param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
    param_glob.offset += 8;
    break;
  }
  return S_OK;
}

/*----------------------------------------------------------------------------*/

HRESULT os37_OneParameter (tos37_ParamGlob         &param_glob, 
                          tsp_dcom_paraminfo_parms &parms,
                          ELEMDESCPTR              &lpParmDesc,
                          long                     paramNo )
                          
{
  ROUTINE_DBG_MSP00 ("os37_OneParameter");

  CHRESULT_OS37 hr("os37_OneParameter");
  USHORT  Flags;
  
  hr = os37_NewParamInfo(param_glob, 0);
  if (!hr.succeeded()) return hr.hresult();
  Flags = lpParmDesc->idldesc.wIDLFlags;
  /* Check for in, out or inout parameter */
  if ((Flags & PARAMFLAG_FIN) != 0)
    param_glob.lpParamInfo->sp1i_dcom_io_type.becomes(sp1io_input);
  if ((Flags & PARAMFLAG_FOUT) != 0) 
  {
    param_glob.OutParams++;
    if ((dbo_dbproc != parms.db_obj_type) && (param_glob.OutParams > 1))
    {
      ERRMSG(( TOO_MANY_OUTPARMS_OS37, param_glob.Method_c ));
      return STG_E_INVALIDPARAMETER;
    }
    param_glob.lpParamInfo->sp1i_dcom_io_type.becomes(((Flags & PARAMFLAG_FIN) != 0) ? sp1io_inout : sp1io_output);
  }
  param_glob.offset = 0;
  param_glob.lpParamInfo->sp1i_dcom_datatype = lpParmDesc->tdesc.vt;
  param_glob.lpParamInfo->sp1i_dcom_cpp_offset   = 0;
  if (!os37_IsScalar(lpParmDesc->tdesc.vt)) 
  {
    switch (lpParmDesc->tdesc.vt) 
    {
    case VT_USERDEFINED :
      hr = os37_UserDefinedType (param_glob, parms, lpParmDesc->tdesc.hreftype, paramNo);
      break;
    case VT_PTR :
      hr = os37_PtrType (param_glob, parms, lpParmDesc, paramNo);
      break;
    default :
      ERRMSG(( UNSUPPORTED_VT_TYPE2_OS37, 
        param_glob.Method_c, paramNo, lpParmDesc->tdesc.vt ));
      hr = STG_E_INVALIDPARAMETER;
    }
  }
  lpParmDesc++;
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

HRESULT os37_PtrType(tos37_ParamGlob          &param_glob, 
                     tsp_dcom_paraminfo_parms &parms,
                     ELEMDESCPTR              &lpParmDesc,
                     long                      paramNo )
{
  ROUTINE_DBG_MSP00 ("os37_PtrType");
  
  CHRESULT_OS37 hr("os37_PtrType");
  TYPEATTR  *lpTypeAttr;
  ITypeInfo *lpITypeInfo;
  
  hr = S_OK;
  
  if (NULL == lpParmDesc->tdesc.lptdesc)
  {
    ERRMSG(( UNSUPPORTED_TLIBSTRUCT_OS37, param_glob.Method_c, paramNo ));
    hr = STG_E_INVALIDPARAMETER;
    hr.succeeded();
    return hr.hresult();
  }
  param_glob.lpParamInfo->sp1i_dcom_sub_datatype = lpParmDesc->tdesc.lptdesc->vt;
  if (sp1io_input == param_glob.lpParamInfo->sp1i_dcom_io_type) {
    param_glob.lpParamInfo->sp1i_dcom_mode.delElement(sp1ot_mandatory); 
    param_glob.lpParamInfo->sp1i_dcom_mode.addElement(sp1ot_optional);
  }
  if (!os37_IsScalar(lpParmDesc->tdesc.lptdesc->vt)) 
  {
    if (VT_CARRAY == lpParmDesc->tdesc.lptdesc->vt) 
    {
      switch (lpParmDesc->tdesc.lptdesc->lptdesc->vt) 
      {
      case VT_I1 :
      case VT_UI1 :
        if (1 == lpParmDesc->tdesc.lptdesc->lpadesc->cDims)
          param_glob.lpParamInfo->sp1i_dcom_dim = 
          (short) lpParmDesc->tdesc.lptdesc->lpadesc->rgbounds[0].cElements;
        else
        {
          ERRMSG(( UNSUPPORTED_VT_CARRAY4_OS37, 
            param_glob.Method_c, paramNo, lpParmDesc->tdesc.lptdesc->lpadesc->cDims ));
          hr = STG_E_INVALIDPARAMETER;
        }
        break;
      case VT_USERDEFINED :
        lpITypeInfo = (ITypeInfo *) parms.typeinfo_ptr;
        hr = os37_GetTypeAttr ( param_glob, 
          lpParmDesc->tdesc.lptdesc->lptdesc->hreftype,
          &lpITypeInfo, &lpTypeAttr);
        if (hr.succeeded()) 
        {
          if (os37_IsOmsDatatype (lpTypeAttr->guid, param_glob.lpParamInfo->sp1i_dcom_sub_datatype)) 
          {
            param_glob.lpParamInfo->sp1i_dcom_dim =
              (short) lpParmDesc->tdesc.lptdesc->lpadesc->rgbounds[0].cElements;
            if (csp1_vt_lc_wchar == param_glob.lpParamInfo->sp1i_dcom_sub_datatype)
              param_glob.lpParamInfo->sp1i_dcom_dim *= 2;
            param_glob.lpParamInfo->sp1i_dcom_datatype = VT_CARRAY;
            param_glob.lpParamInfo->sp1i_dcom_cpp_offset = param_glob.offset;
            param_glob.offset += param_glob.lpParamInfo->sp1i_dcom_dim;
          }
          else
          {
            ERRMSG(( INVALID_PARAMETER_OS37, param_glob.Method_c, paramNo ));
            hr = STG_E_INVALIDPARAMETER;
          }
          lpITypeInfo->ReleaseTypeAttr(lpTypeAttr);
          lpITypeInfo->Release();
        }
        break;    
      default :
        ERRMSG(( INVALID_PARAMETER_OS37, param_glob.Method_c, paramNo ));
        hr = STG_E_INVALIDPARAMETER;
      }
    }
    else
      if (VT_USERDEFINED == lpParmDesc->tdesc.lptdesc->vt) 
      {
        HREFTYPE                  hreftype; /* %%% */
        
        lpITypeInfo = (ITypeInfo *) parms.typeinfo_ptr;
        hr = os37_GetAndCheckGuid (param_glob, lpITypeInfo, 
          lpParmDesc->tdesc.lptdesc->hreftype, param_glob.lpParamInfo->sp1i_dcom_type_uid);
        if (!hr.succeeded())
          return hr.hresult();
        if (0 == memcmp (&GUID_LC_PARAM_DESC, &param_glob.lpParamInfo->sp1i_dcom_type_uid, 
          sizeof(param_glob.lpParamInfo->sp1i_dcom_type_uid))) 
        {
          param_glob.lpParamInfo->sp1i_dcom_datatype = csp1_vt_lc_kb71_param_desc;
          return S_OK;
        }
        hreftype = lpParmDesc->tdesc.lptdesc->hreftype;
        hr = os37_GetTypeAttr ( param_glob, 
          lpParmDesc->tdesc.lptdesc->hreftype,
          &lpITypeInfo, &lpTypeAttr);
        if (hr.succeeded())
        {
          if (!os37_IsStreamType (param_glob, lpITypeInfo, lpTypeAttr, hr)) 
          {
            if ( hr.succeeded() ) 
            {
              if (TKIND_ALIAS == lpTypeAttr->typekind)
              {
                if (os37_IsScalar(lpTypeAttr->tdescAlias.vt)) {
                  param_glob.lpParamInfo->sp1i_dcom_dim = 1; 
                  if (!os37_IsOmsDatatype (lpTypeAttr->guid, param_glob.lpParamInfo->sp1i_dcom_sub_datatype)) { 
                    param_glob.lpParamInfo->sp1i_dcom_sub_datatype = lpTypeAttr->tdescAlias.vt;
                  }
                }
                else 
                {
                  param_glob.lpParamInfo->sp1i_dcom_dim = (short) lpTypeAttr->cbSizeInstance;
                  if (VT_CARRAY == lpTypeAttr->tdescAlias.vt) 
                  {
                    if  (lpTypeAttr->tdescAlias.lptdesc->vt == VT_USERDEFINED ) /*+CHG*+*/
                      hr = os37_UserDefinedType (param_glob, parms, 
                      lpTypeAttr->tdescAlias.lptdesc->hreftype, paramNo); 
                    else                                                        /*+CHG*+*/
                      param_glob.lpParamInfo->sp1i_dcom_datatype = 
                      lpTypeAttr->tdescAlias.lptdesc->vt;                     /*+CHG*+*/
                    if ((hr.succeeded()) && 
                      (1 == lpTypeAttr->tdescAlias.lpadesc->cDims)) 
                    {
                      param_glob.lpParamInfo->sp1i_dcom_sub_datatype =
                        param_glob.lpParamInfo->sp1i_dcom_datatype;
                      param_glob.lpParamInfo->sp1i_dcom_datatype = csp1_vt_carray;
                      param_glob.lpParamInfo->sp1i_dcom_dim = (short)
                        lpTypeAttr->tdescAlias.lpadesc->rgbounds[0].cElements;
                      if (csp1_vt_lc_wchar == param_glob.lpParamInfo->sp1i_dcom_sub_datatype)
                        param_glob.lpParamInfo->sp1i_dcom_dim *= 2;
                    } 
                    else
                      if (STG_E_INVALIDPARAMETER == hr.hresult()) 
                      {
                        hr = S_OK;
                      }
                  }
                }       
              }       
              if (TKIND_RECORD == lpTypeAttr->typekind)
              {
                dcom_param_info_ptr         lpParamInfo;
                param_glob.lpStreamParamInfo             = param_glob.lpParamInfo;
                param_glob.lpParamInfo->sp1i_dcom_length = (short) lpTypeAttr->cbSizeInstance;
                lpParamInfo                              = param_glob.lpParamInfo;
                hr = os37_DescribeStructure (param_glob, hreftype);
                lpParamInfo->sp1i_dcom_length = (short)param_glob.offset;
                param_glob.lpStreamParamInfo = NULL;
              }
            }
          }
          lpITypeInfo->ReleaseTypeAttr(lpTypeAttr);
          lpITypeInfo->Release();
        }       
      }
  }
  else {
    if (sp1io_input == param_glob.lpParamInfo->sp1i_dcom_io_type) {
      ERRMSG(( IN_SCALAR_BYPTR_OS37, param_glob.Method_c, paramNo, lpParmDesc->tdesc.lptdesc->vt ));
      hr = STG_E_INVALIDPARAMETER;    
    }
  }
  return hr.hresult();
}

/*----------------------------------------------------------------------------*/

void os37_SetError (long e,
                    tos37_ParamGlob &param_glob)
{
  ROUTINE_DBG_MSP00 ("os37_SetError");

  param_glob.lpSegment->sp1r_returncode()  = -1;
  param_glob.lpSegment->sp1r_errorpos()    = e;
}

/*----------------------------------------------------------------------------*/

HRESULT os37_UserDefinedType (tos37_ParamGlob           &param_glob, 
                              tsp_dcom_paraminfo_parms  &parms,
                              HREFTYPE                  hreftype,                    
                              long                      paramNo )
{ 
  ROUTINE_DBG_MSP00 ("os37_UserDefinedType");
  
  CHRESULT_OS37 hr("os37_UserDefinedType");
  ITypeInfo *lpITypeInfo;
  TYPEATTR  *lpTypeAttr;
  
  hr = S_OK;
  lpITypeInfo = (ITypeInfo *) parms.typeinfo_ptr;
  // look for user defined skalars, i.e
  // typedef [uuid(..), public] long UDTLONG
  hr = os37_GetTypeAttr (param_glob, hreftype, &lpITypeInfo, &lpTypeAttr);
  if (hr.succeeded()) 
  {
    if ((TKIND_ALIAS == lpTypeAttr->typekind) &&
      os37_IsScalar(lpTypeAttr->tdescAlias.vt)) 
    {
      if (!os37_IsOmsDatatype (lpTypeAttr->guid, param_glob.lpParamInfo->sp1i_dcom_datatype)) 
        param_glob.lpParamInfo->sp1i_dcom_datatype = lpTypeAttr->tdescAlias.vt;
    }
    else 
    {
      if (TKIND_RECORD == lpTypeAttr->typekind) 
      {
        ERRMSG(( CSTRUCT_BYVALUE_OS37, param_glob.Method_c, paramNo ));
      }
      else
      {
        ERRMSG(( SCALAR_BYVALUE_OS37, param_glob.Method_c, paramNo, lpTypeAttr->tdescAlias.vt ));
      }
      hr = STG_E_INVALIDPARAMETER;
    }
    lpITypeInfo->ReleaseTypeAttr(lpTypeAttr);
    lpITypeInfo->Release();        
  }
  return hr.hresult();
}
static _INLINE HRESULT os37_GenerateOutput ( t_Path cInputTlb,
                                            t_Path cOutputFile,
                                            t_Path cType  )
{
  ROUTINE_DBG_MSP00 ("os37_GenerateOutput");

  tsp_dcom_paraminfo_parms parms;
  tsp1_packet_ptr          packet_ptr;
  unsigned char            buffer[MAX_BUFFER_LENGTH];
  FILE                     *outStream_h, *outStream_cpp;
  long                     rc;
  int                      num = 0;

  parms.packet = (tsp00_MoveObjPtr)&buffer;

  rc = os37_InitializeTypeLib( &parms, cInputTlb, cType );
  if ( rc == RC_OK )
    rc = os37_InitializeOutput( &parms, cOutputFile, &outStream_h, &outStream_cpp );

  packet_ptr = (tsp1_packet_ptr)parms.packet; 
  while ( parms.typeinfo_ptr && ( rc == RC_OK ))
  {
    sql37_paraminfo( &parms, cInputTlb );
    num++;
    rc = packet_ptr->sp1_segm().sp1r_returncode();
    if ( rc == RC_OK ) 
    {
      if ( num > 1 ) 
        fprintf( outStream_cpp, ",\n");
      os37_ProcInspectInfo ( &parms, num, &outStream_h, &outStream_cpp );
      parms.func_idx++;
    }
    else 
    {
      rc = packet_ptr->sp1_segm().sp1r_errorpos();
    }
  }
  if ( rc == RC_OK )
    os37_TerminateOutput( &parms, num, &outStream_h, &outStream_cpp );
  else
  {
    ERRMSG(( TERMINATE_MSG_OS37,  rc ));
  }
  return rc;
}


/*---------------------------------------------------------------------------*/

static _INLINE int os37_InitializeOutput ( tsp_dcom_paraminfo_parms* parms, 
                                            t_Path cOutputFile,
                                            FILE **outStream_h,
                                            FILE **outStream_cpp )
{
  ROUTINE_DBG_MSP00 ("os37_InitializeOutput");
  
  char  buffer[256];
  CLSID clsid;

  t_Path  outfile;
  strcpy(outfile,cOutputFile);
  strcat(outfile,".h");
  if( (*outStream_h = fopen( outfile, "w+t" )) == NULL ) return RC_ERROR;
  strcpy(outfile,cOutputFile);
  strcat(outfile,".cpp");
  if( (*outStream_cpp = fopen( outfile, "w+t" )) == NULL ) return RC_ERROR;

  memset( buffer, '\0', sizeof(buffer) );
  fprintf( *outStream_h, "#include \"lcbasetypes.h\"\n" );
  fprintf( *outStream_h, "\n" );
  fprintf( *outStream_h, "static const   CLSID CLSID_Intro =\n" );
  SAPDB_memcpy(&clsid.Data1 ,parms->coclsid, sizeof(clsid));
  fprintf( *outStream_h, "{0x%8.8X, 0x%4.4X, 0x%4.4X,\n{0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X}};\n",
            clsid.Data1, clsid.Data2, clsid.Data3,
            clsid.Data4[0], clsid.Data4[1], clsid.Data4[2],
            clsid.Data4[3], clsid.Data4[4], clsid.Data4[5],
            clsid.Data4[6], clsid.Data4[7] );
  fprintf( *outStream_h, "\n" );
  fprintf( *outStream_h, "static const   IID   IID_Intro =\n" );
  SAPDB_memcpy(&clsid.Data1 ,parms->iid, sizeof(clsid));
  fprintf( *outStream_h, "{0x%8.8X, 0x%4.4X, 0x%4.4X,\n{0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X}};\n",
            clsid.Data1, clsid.Data2, clsid.Data3,
            clsid.Data4[0], clsid.Data4[1], clsid.Data4[2],
            clsid.Data4[3], clsid.Data4[4], clsid.Data4[5],
            clsid.Data4[6], clsid.Data4[7] );
  fprintf( *outStream_h, "\n" );
  fprintf( *outStream_h, "typedef void ( __stdcall *CO_INTROSPECTFUNCTION) ( \n" );
  fprintf( *outStream_h, "  unsigned short       ParmIdx,\n");
  fprintf( *outStream_h, "  char                 **lplpMode,\n" );
  fprintf( *outStream_h, "  char                 **lplpIOtype,\n" );
  fprintf( *outStream_h, "  short int            *lpDatatype,\n" );
  fprintf( *outStream_h, "  short int            *lpSubdatatype,\n" );
  fprintf( *outStream_h, "  short int            *lpLength,\n" );
  fprintf( *outStream_h, "  short int            *lpDim,\n" );
  fprintf( *outStream_h, "  long                 *lpOffset,\n" );
  fprintf( *outStream_h, "  unsigned char        *lpFrac,\n" );
  fprintf( *outStream_h, "  LPGUID               *lplpGUID,\n" );
  fprintf( *outStream_h, "  char                 **lplpParmName);\n");
  fprintf( *outStream_h, "\n" );
  fprintf( *outStream_h, "typedef struct\n" );
  fprintf( *outStream_h, "{\n" );
  fprintf( *outStream_h, "  unsigned short        FuncIdx;\n" );
  fprintf( *outStream_h, "  char                  *lpMethod;\n" );
  fprintf( *outStream_h, "  unsigned short        ParamCount;\n" );
  fprintf( *outStream_h, "  CO_INTROSPECTFUNCTION lpFunction;\n" );
  fprintf( *outStream_h, "} t_FuncBufDesc;\n" );
  fprintf( *outStream_h, "\n" );
  fprintf( *outStream_h, "typedef struct\n" );
  fprintf( *outStream_h, "{\n" );
  fprintf( *outStream_h, "  char                 *Mode;\n" );
  fprintf( *outStream_h, "  char                 *IOtype;\n" );
  fprintf( *outStream_h, "  short int            Datatype;\n" );
  fprintf( *outStream_h, "  short int            Subdatatype;\n" );
  fprintf( *outStream_h, "  short int            Length;\n" );
  fprintf( *outStream_h, "  short int            Dim;\n" );
  fprintf( *outStream_h, "  long  int            Offset;\n" );
  fprintf( *outStream_h, "  unsigned char        Frac;\n" );
  fprintf( *outStream_h, "  GUID                 guid;\n" );
  fprintf( *outStream_h, "  char                 *lpParmName;\n" );
  fprintf( *outStream_h, "} t_ParmsDesc;\n" );
  fprintf( *outStream_cpp, "\n" );
  /* +++ PTS 1109939  +++ */
  char *s1 = strrchr( &cOutputFile[0], '\\' );
  if ( s1 ) {
    strcpy( buffer, s1 + 1 );
  }
  else
  {
    s1 = strrchr( &cOutputFile[0], '/' );
    if ( s1 )
      strcpy( buffer, s1 + 1 );
    else
      strcpy( buffer, &cOutputFile[0] );
  }
  fprintf( *outStream_cpp, "#include \"%s.h\"\n", buffer );
  /* --- PTS 1109939  --- */
  /* fprintf( *outStream_cpp, "#include \"%.64s.h\"\n", cOutputFile ); PTS 1109939 */
  fprintf( *outStream_cpp, "\n" );
  fprintf( *outStream_cpp, "const t_FuncBufDesc CO_FunctionDesc[] = \n" );
  fprintf( *outStream_cpp, "{\n" );

  return RC_OK;
}

/*---------------------------------------------------------------------------*/

static _INLINE HRESULT os37_InitializeTypeLib ( tsp_dcom_paraminfo_parms* parms, 
                                                char* cTypeLib,
                                                t_Path cType )
{
  ROUTINE_DBG_MSP00 ("os37_InitializeTypeLib");

  tsp1_packet_ptr packet_ptr;
  HRESULT hr;

  packet_ptr = (tsp1_packet_ptr)parms->packet; 
  packet_ptr->sp1_header.sp1h_varpart_size =
      sizeof (tgg00_Rec) - C_OLD_TSP_RTE_HEADER -
      sizeof (tsp1_packet_header);
  packet_ptr->sp1_header.sp1h_varpart_len = 0;
  packet_ptr->sp1_header.sp1h_no_of_segm  = 0;
  memset(parms->prog_id,' ',sizeof(parms->prog_id));
  memset(parms->method,' ',sizeof(parms->method));
  memset(parms->coclsid,' ',sizeof(parms->coclsid));
  memset(parms->iid,' ',sizeof(parms->iid));
  memset(parms->udttypeguid,' ',sizeof(parms->udttypeguid));
  parms->typeinfo_ptr    = 0;
  parms->session_context = 0;

  parms->dispid           = 0;
  parms->const_dispid     = 0;
  memset(parms->session,' ',sizeof(parms->session));
  parms->const_type       = 0;
  parms->const_typeid     = 0;
  parms->func_idx         = 0;
  /*parms->in_proc_server   = 0; PTS 1112663 */
  if ( 0 == strcmp(cType,"DBPROC") )
    parms->db_obj_type.becomes(dbo_dbproc);
  if ( 0 == strcmp(cType,"UDT") )
    parms->db_obj_type.becomes(dbo_method);

  memset(parms->errtext,' ',sizeof(parms->errtext));

  hr = sql37_paraminfo( parms, cTypeLib );
  parms->func_idx         = 1;

  return hr;
}


/*---------------------------------------------------------------------------*/

static _INLINE int os37_ProcessCommandLine ( int argc , 
                                             char * argv[],
                                             t_Path cInputTlb,
                                             t_Path cOutputFile,
                                             t_Path cType )
{
  ROUTINE_DBG_MSP00 ("os37_ProcessCommandLine");
  if ( argc != 4 ) return RC_ERROR;

  strcpy( cInputTlb, argv[ 1 ] );
  strcpy( cOutputFile, argv[ 2 ] );
  strcpy( cType, argv[ 3 ] );
  if ( strcmp(cType,"UDT") && strcmp(cType,"DBPROC") )
    return RC_ERROR;
  else
    return RC_OK;
}

/*---------------------------------------------------------------------------*/

static _INLINE void os37_ProcInspectInfo ( tsp_dcom_paraminfo_parms* parms, 
                                           int num,
                                           FILE **outStream_h,
                                           FILE **outStream_cpp )
{
  ROUTINE_DBG_MSP00 ("os37_ProcInspectInfo");

  typedef tsp1_dcom_param_info *dcom_param_info_ptr;
  typedef char t_part_data[sizeof(tsp00_KnlIdentifier) + 2];

  char              *pc, *pcMode, *pcIOtype, *pPart2;
  int               parmCount;
  tsp1_packet_ptr   packet_ptr; 
  tsp1_part_header  part_header_ptr;
  GUID              guid;
  dcom_param_info_ptr lpParamInfo;
  tsp1_part_ptr     lpPart2;
  t_part_data       partData;
  int               iSubRecCnt, len;

  fprintf( *outStream_h, "\n");
  fprintf( *outStream_h, "static void __stdcall co_IntrospectFunction%d (\n", num ); 
  fprintf( *outStream_h, "  unsigned short       ParmIdx,\n");
  fprintf( *outStream_h, "  char                 **lplpMode,\n");
  fprintf( *outStream_h, "  char                 **lplpIOtype,\n");
  fprintf( *outStream_h, "  short int            *lpDatatype,\n");
  fprintf( *outStream_h, "  short int            *lpSubdatatype,\n");
  fprintf( *outStream_h, "  short int            *lpLength,\n");
  fprintf( *outStream_h, "  short int            *lpDim,\n");
  fprintf( *outStream_h, "  long                 *lpOffset,\n");
  fprintf( *outStream_h, "  unsigned char        *lpFrac,\n");
  fprintf( *outStream_h, "  LPGUID               *lplpGUID,\n");
  fprintf( *outStream_h, "  char                 **lplpParmName)\n");
  fprintf( *outStream_h, "{\n");

  packet_ptr = (tsp1_packet_ptr)parms->packet; 
  part_header_ptr = packet_ptr->sp1_segm().sp1p_part_header();
  lpParamInfo = (tsp1_dcom_param_info *) &packet_ptr->sp1_segm().sp1p_buf();
  lpPart2    = &packet_ptr->sp1_segm().sp1p_part();
  s26nextpart (&lpPart2);
  pPart2     = (char*)&lpPart2->sp1p_buf();
  iSubRecCnt = 0;

  parmCount  = part_header_ptr.sp1p_arg_count;
  if ( parmCount > 0 )
  {
    fprintf( *outStream_h, "  static const t_ParmsDesc CO_ParmDesc[] = \n");
    fprintf( *outStream_h, "  {\n");

    for ( int i=0; i < parmCount; i++ )
    {
      pcMode = "";
      if ( lpParamInfo->sp1i_dcom_mode.includes(sp1ot_mandatory) ) pcMode = "MAN";
      if ( lpParamInfo->sp1i_dcom_mode.includes(sp1ot_optional) ) pcMode = "OPT";
      if ( lpParamInfo->sp1i_dcom_mode.includes(sp1ot_default) ) pcMode = "DEF";
      if ( lpParamInfo->sp1i_dcom_mode.includes(sp1ot_escape_char) ) pcMode = "ESC";

      pcIOtype = "";
      if ( lpParamInfo->sp1i_dcom_io_type == sp1io_input ) pcIOtype = "IN";
      if ( lpParamInfo->sp1i_dcom_io_type == sp1io_output ) pcIOtype = "OUT";
      if ( lpParamInfo->sp1i_dcom_io_type == sp1io_inout ) pcIOtype = "I/O";

      if ( 0 == iSubRecCnt )
      {
         len = *pPart2;
         ((char*)pPart2)++;
         SAPDB_memcpy(partData,pPart2,len);
         pPart2 = ((char*)pPart2) + len;
         partData[len] = '\0';
         if ((lpParamInfo->sp1i_dcom_datatype == csp1_vt_lc_abap_tab_handle) && (lpParamInfo->sp1i_dcom_sub_datatype == 29))
           iSubRecCnt = lpParamInfo->sp1i_dcom_dim;
         if ((lpParamInfo->sp1i_dcom_datatype == csp1_vt_lc_stream_handle) && (lpParamInfo->sp1i_dcom_sub_datatype == 29))
           iSubRecCnt = lpParamInfo->sp1i_dcom_dim;
         if ((lpParamInfo->sp1i_dcom_datatype == VT_PTR) && (lpParamInfo->sp1i_dcom_sub_datatype == 29))
           iSubRecCnt = lpParamInfo->sp1i_dcom_dim;
      }
      else
      {
        partData[0] = '\0';
        iSubRecCnt--;
      }
      SAPDB_memcpy(&guid.Data1 ,lpParamInfo->sp1i_dcom_type_uid, sizeof(guid));
      fprintf( *outStream_h, "   (char*)\"%s\", (char*)\"%s\", %d, %d, %d, %d, %d, %d, {0x%8.8X, 0x%4.4X, 0x%4.4X, {0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X, 0x%2.2X}}, (char*)\"%s\"",
        pcMode,
        pcIOtype,
        lpParamInfo->sp1i_dcom_datatype,
        lpParamInfo->sp1i_dcom_sub_datatype,
        lpParamInfo->sp1i_dcom_length,
        lpParamInfo->sp1i_dcom_dim,
        lpParamInfo->sp1i_dcom_cpp_offset,
        lpParamInfo->sp1i_dcom_frac,
        guid.Data1, guid.Data2, guid.Data3, 
        guid.Data4[0], guid.Data4[1], guid.Data4[2],
        guid.Data4[3], guid.Data4[4], guid.Data4[5], 
        guid.Data4[6], guid.Data4[7], partData );
        lpParamInfo++;
        if ( i == ( parmCount - 1 ))
          fprintf( *outStream_h, "\n");
        else
          fprintf( *outStream_h, ",\n");
     }
    fprintf( *outStream_h, "  };\n");
    
    fprintf( *outStream_h, "\n");
    fprintf( *outStream_h, "  if (( ParmIdx > (sizeof(CO_ParmDesc) / sizeof(t_ParmsDesc))) || ( ParmIdx < 1 ))\n");
    fprintf( *outStream_h, "  {\n");
    fprintf( *outStream_h, "    *lplpMode = NULL;\n");
    fprintf( *outStream_h, "  }\n");
    fprintf( *outStream_h, "  else\n");
    fprintf( *outStream_h, "  {\n");
    fprintf( *outStream_h, "    *lplpMode       = CO_ParmDesc[ParmIdx - 1].Mode;\n");
    fprintf( *outStream_h, "    *lplpIOtype     = CO_ParmDesc[ParmIdx - 1].IOtype;\n");
    fprintf( *outStream_h, "    *lpDatatype     = CO_ParmDesc[ParmIdx - 1].Datatype;\n");
    fprintf( *outStream_h, "    *lpSubdatatype  = CO_ParmDesc[ParmIdx - 1].Subdatatype;\n");
    fprintf( *outStream_h, "    *lpLength       = CO_ParmDesc[ParmIdx - 1].Length;\n");
    fprintf( *outStream_h, "    *lpDim          = CO_ParmDesc[ParmIdx - 1].Dim;\n");
    fprintf( *outStream_h, "    *lpOffset       = CO_ParmDesc[ParmIdx - 1].Offset;\n");
    fprintf( *outStream_h, "    *lpFrac         = CO_ParmDesc[ParmIdx - 1].Frac;\n");
    fprintf( *outStream_h, "    *lplpGUID       = (GUID*)&CO_ParmDesc[ParmIdx - 1].guid;\n");
    fprintf( *outStream_h, "    *lplpParmName   = CO_ParmDesc[ParmIdx - 1].lpParmName;\n");
    fprintf( *outStream_h, "  }\n");
    
  }
  else
  {
    fprintf( *outStream_h, "    *lplpMode = NULL;\n");
  }
  
  fprintf( *outStream_h, "};\n");
  fprintf( *outStream_h, "\n");
  
  pc = strchr( parms->method, ' ' );
  *pc = '\0';
  fprintf( *outStream_cpp, "  %d, (char*)\"%.64s\", %d, &co_IntrospectFunction%d", 
    parms->dispid, &parms->method[0], parmCount, num );
  return;
}

/*---------------------------------------------------------------------------*/

static _INLINE void os37_TerminateOutput ( tsp_dcom_paraminfo_parms* parms, 
                                           int num,
                                           FILE **outStream_h,
                                           FILE **outStream_cpp )
{
  ROUTINE_DBG_MSP00 ("os37_TerminateOutput");

  fprintf( *outStream_h, "\n");
  fprintf( *outStream_h, "#define PROCEDURE_NUM %d\n", num);
  fprintf( *outStream_h, "#define VERSION_NUMBER 1\n");
  fclose( *outStream_h );
  
  if ( num == 0 ) /* in case there are no interface methods */
    fprintf( *outStream_cpp,"  0, \" \", 0, NULL \n");

  fprintf( *outStream_cpp, "};\n"); 
  fprintf( *outStream_cpp, "\n");
  fprintf( *outStream_cpp, "extern \"C\" void __stdcall Co_IntrospectObject (  LPCLSID      *lpClsId,\n"); 
  fprintf( *outStream_cpp, "                                       LPIID          *lpIId,\n");
  fprintf( *outStream_cpp, "                                       unsigned short *lpNum,\n");
  fprintf( *outStream_cpp, "                                       long           *lpVersion )\n");
  fprintf( *outStream_cpp, "{\n");
  fprintf( *outStream_cpp, "  *lpClsId        = (LPCLSID)&CLSID_Intro;\n"); 
  fprintf( *outStream_cpp, "  *lpIId          = (LPIID)&IID_Intro;\n");
  fprintf( *outStream_cpp, "  *lpNum          = PROCEDURE_NUM;\n"); 
  fprintf( *outStream_cpp, "  *lpVersion      = VERSION_NUMBER;\n"); 
  fprintf( *outStream_cpp, "};\n"); 
  fprintf( *outStream_cpp, "\n");
  fprintf( *outStream_cpp, "  enum Co_AccessType\n"); 
  fprintf( *outStream_cpp, "{\n");
  fprintf( *outStream_cpp, "    get_by_name,\n");
  fprintf( *outStream_cpp, "    get_by_index\n"); 
  fprintf( *outStream_cpp, "};\n"); 
  fprintf( *outStream_cpp, "\n");
  fprintf( *outStream_cpp, "extern \"C\" void __stdcall Co_IntrospectMethod ( Co_AccessType   accessType, \n"); 
  fprintf( *outStream_cpp, "                                      char            **lplpMethod, \n"); 
  fprintf( *outStream_cpp, "                                      unsigned short  Num,\n");
  fprintf( *outStream_cpp, "                                      unsigned short  *lpIdx,\n");
  fprintf( *outStream_cpp, "                                      unsigned short  *lpParamCount,\n");
  fprintf( *outStream_cpp, "                                      CO_INTROSPECTFUNCTION *lplpIntrospecFunc )\n");
  fprintf( *outStream_cpp, "{\n");
  fprintf( *outStream_cpp, "  *lplpIntrospecFunc = NULL;\n");
  fprintf( *outStream_cpp, "  if (accessType == get_by_name)\n");
  fprintf( *outStream_cpp, "  {\n");
  fprintf( *outStream_cpp, "	  if ( **lplpMethod == '\\0' )\n");
  fprintf( *outStream_cpp, "    {\n");
  fprintf( *outStream_cpp, "        *lpIdx             = CO_FunctionDesc[0].FuncIdx;\n");
  fprintf( *outStream_cpp, "        *lpParamCount      = CO_FunctionDesc[0].ParamCount;\n");
  fprintf( *outStream_cpp, "        *lplpIntrospecFunc = CO_FunctionDesc[0].lpFunction;\n");
  fprintf( *outStream_cpp, "        *lplpMethod        = CO_FunctionDesc[0].lpMethod;\n");
  fprintf( *outStream_cpp, "    }\n");
  fprintf( *outStream_cpp, "    else \n");
  fprintf( *outStream_cpp, "    for ( int i=0; i < PROCEDURE_NUM; i++ )\n");
  fprintf( *outStream_cpp, "      if ( 0 == strcmp(*lplpMethod, CO_FunctionDesc[i].lpMethod ))\n");
  fprintf( *outStream_cpp, "      {\n");
  fprintf( *outStream_cpp, "        *lpIdx             = CO_FunctionDesc[i].FuncIdx;\n");
  fprintf( *outStream_cpp, "        *lpParamCount      = CO_FunctionDesc[i].ParamCount;\n");
  fprintf( *outStream_cpp, "        *lplpIntrospecFunc = CO_FunctionDesc[i].lpFunction;\n");
  fprintf( *outStream_cpp, "      }\n");
  fprintf( *outStream_cpp, "  }\n");
  fprintf( *outStream_cpp, "  else\n");
  fprintf( *outStream_cpp, "  {\n");
  fprintf( *outStream_cpp, "    if (( Num <= PROCEDURE_NUM ) && ( Num >= 0 ))\n");
  fprintf( *outStream_cpp, "      {\n");
  fprintf( *outStream_cpp, "        *lpIdx             = CO_FunctionDesc[Num].FuncIdx;\n");
  fprintf( *outStream_cpp, "        *lpParamCount      = CO_FunctionDesc[Num].ParamCount;\n");
  fprintf( *outStream_cpp, "        *lplpIntrospecFunc = CO_FunctionDesc[Num].lpFunction;\n");
  fprintf( *outStream_cpp, "        *lplpMethod        = CO_FunctionDesc[Num].lpMethod;\n");
  fprintf( *outStream_cpp, "      }\n");
  fprintf( *outStream_cpp, "  }\n");
  fprintf( *outStream_cpp, "};\n"); 
  fprintf( *outStream_cpp, "\n");
  fclose( *outStream_cpp );

  return;
}


