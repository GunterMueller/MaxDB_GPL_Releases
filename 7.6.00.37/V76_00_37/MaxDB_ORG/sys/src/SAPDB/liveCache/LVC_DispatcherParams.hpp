/*!
 * \file    LVC_DispatcherParams.hpp
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

#include "Oms/OMS_Defines.h"
#include "gsp00.h"
#include "hsp26.h"

typedef tsp1_dcom_param_info *dcom_param_info_ptr;

extern "C"
{
  /// Function for getting information about the object.
  typedef void ( __stdcall *LVC_DispIntrospectObject) (
    LPCLSID        *lpClsId,
    LPIID          *lpIId,
    unsigned short *lpNum,
    long           *lpVersion );

  /// Dispatcher access type.
  enum LVC_DispAccessType { LVC_Disp_get_by_name, LVC_Disp_get_by_index };

  /// Function for getting information about method.
  typedef void ( __stdcall *LVC_DispIntrospectMethod) (
    LVC_DispAccessType accessType,
    char            **lpMethod,
    unsigned short  Num,
    unsigned short  *lpIdx,
    unsigned short  *lpParamCount,
    void            **lplpIntrospecFunc );

  /// Function for getting information about method for Unicode-capable methods.
  typedef void ( __stdcall *LVC_DispIntrospectMethodMO) (
    LVC_DispAccessType accessType,
    char            **lpMethod,
    unsigned short  Num,
    unsigned short  *lpIdx,
    unsigned short  *lpParamCount,
    void            **lplpIntrospecFunc_MO );
}

/// Function for getting information about parameter.
typedef void ( __stdcall *LVC_DispIntrospectFunc) (
  unsigned short ParmIdx,
  char           **lplpMode,
  char           **lplpIOtype,
  short int      *lpDatatype,
  short int      *lpSubdatatype,
  short int      *lpLength,
  short int      *lpDim,
  long           *lpOffset,
  unsigned char  *lpFrac,
  LPGUID         *lplpGUID,
  char           **lplpParmName );

/// Function for getting information about parameter for Unicode-capable methods.
typedef void ( __stdcall *LVC_DispIntrospectFuncMO) (
  unsigned short ParmIdx,
  char           **lplpMode,
  char           **lplpIOtype,
  short int      *lpDatatype,
  short int      *lpSubdatatype,
  short int      *lpLength,
  short int      *lpDim,
  int            *lpOffsetServer,
  int            *lpOffsetAscClient,
  int            *lpOffsetUniClient,
  unsigned char  *lpFrac,
  LPGUID         *lplpGUID,
  char           **lplpParmName );

/// Maximum # of parameters in COM routine.
#define LVC_Disp_MAXPARAMS 256

/// Dispatcher parameter name.
typedef BSTR                  LVC_DispParamNames[LVC_Disp_MAXPARAMS];

/// Dispatcher parameters read from a COM object.
class LVC_DispatcherParams {
public:

  /// Create new dispatcher parameter processor.
  LVC_DispatcherParams(tsp_dcom_paraminfo_parms *parms);

  /*!
   * \brief Get method info.
   *
   * \param parms parameter buffer,
   * \param pNumParms returns count of method parameters.
   */
  HRESULT GetMethod(
    tsp_dcom_paraminfo_parms *parms,
    unsigned short           *pNumParms,
    BOOLEAN                  &is_last_func);

  /*!
   * \brief Get interface.
   *
   * \param parms parameter buffer,
   * \param pNumParms returns count of method parameters.
   */
  HRESULT GetInterface(
    tsp_dcom_paraminfo_parms *parms,
    unsigned short           *pNumParms);

  /*!
   * \brief Get parameter names.
   *
   * \param ParmNames array to fill with parameter names,
   * \param cArgs count of items to fill in.
   */
  void DescribeParamNames(
    LVC_DispParamNames    &ParmNames,
    unsigned int          cArgs);

  /*!
   * \brief Get information about one parameter.
   *
   * \param parms buffer where to put info,
   * \param idx parameter index.
   */
  HRESULT OneParameter(
    tsp_dcom_paraminfo_parms *parms,
    unsigned short           idx);

  /// Set error info.
  void SetError(long e) 
  {
    lpSegment->sp1r_returncode()  = -1;
    lpSegment->sp1r_errorpos()    = e;
  }

  /// Return part header.
  tsp1_part_header *GetPartHeader() 
  {
    return lpPartHdr;
  }
private:
  /// Initialize parameter info buffer (low-level).
  void InitParamInfo(tsp1_dcom_param_info &paramInfo);

  /// Initialize parameter info buffer (high-level).
  HRESULT NewParamInfo(int vt);

  tsp1_segment_ptr            lpSegment;
  tsp1_part_header            *lpPartHdr;
  tsp_dcom_paraminfo_parms    *Parms;
  dcom_param_info_ptr         lpParamInfo;
  dcom_param_info_ptr         lpStreamParamInfo;
  int                         OutParams;
  //tsp00_C4                    session;
  LVC_DispIntrospectObject    co_IntroSpectObject;
  LVC_DispIntrospectMethod    co_IntroSpectMethod;
  LVC_DispIntrospectMethodMO  co_IntroSpectMethod_MO;
  LVC_DispIntrospectFunc      co_IntroSpectFunction;
  LVC_DispIntrospectFuncMO    co_IntroSpectFunction_MO;
};
