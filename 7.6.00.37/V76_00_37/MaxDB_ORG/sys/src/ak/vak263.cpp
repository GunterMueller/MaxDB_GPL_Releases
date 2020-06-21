/* @lastChanged: "1998-06-02  10:18"

  * @filename:   vak263.cpp
  * @purpose:    "Stream Handle Functions"
  * @release:    7.1.0.0
  * @see:        ""
  *
  * @Copyright (c) 1998-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

/* ========================================================================== */
/* ==========           REPLACEMENTS BY THE PREPROCESSOR            ========= */
/* ========================================================================== */

/* ========================================================================== */
/* ==========              DECLARATION OF TYPES                     ========= */
/* ========================================================================== */

/* ========================================================================== */
/* ==========              DECLARATION OF FUNCTIONS                 ========= */
/* ========================================================================== */

#include "vak001.h"
#include "hak263.h"
#include "hgg01_3.h"  // g01unicode, g01code
#include "hak061.h"
#include "hak07.h"
#include "hgg17.h"
#include "hak12.h"
#include "hak34.h"
#include "Oms/OMS_Types.hpp"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

inline int clientOffset(const tgg00_StackEntry* pStackEntry)
{
  return (pStackEntry+1)->ecol_pos();
}

/*----------------------------------------------------------------------------*/

inline int cppOffset(const tgg00_StackEntry* pStackEntry)
{
  return pStackEntry->ecol_pos();
}

/*----------------------------------------------------------------------------*/

inline void swapInt2 (tsp00_Uint2& i2) {
  unsigned char* p  = REINTERPRET_CAST(unsigned char*,&i2);
  unsigned char aux = *p;
  *p     = *(p+1);
  *(p+1) = aux;
}

/*----------------------------------------------------------------------------*/

inline void swapInt4 (tsp00_Int4& i4) 
{
    tsp00_Int4 swapped;
    unsigned char* pSource =  REINTERPRET_CAST(unsigned char*,&i4);
    unsigned char* pDest   =  REINTERPRET_CAST(unsigned char*,&swapped);
    for (int ix = 0; ix < 4; ++ix)
    {
        pDest[ix] = pSource[3-ix];
    }
    i4 = swapped;
}

/*----------------------------------------------------------------------------*/

static void buildNullTerminatedIdentifier(const tsp00_KnlIdentifier& id, char* pDest, int lenInBytes = -1) 
{
  // construct Null terminated string from identifier. It is assumed, that
  // identifier contains ascii7 only !

  int len;
  int charSize = (g01unicode) ? 2 : 1;
  if (-1 == lenInBytes)
  {
    len = a061identifier_len(id) / charSize;
  }
  else
  {
    len = lenInBytes / charSize;
  }
  char* pSource = (char*) &id[charSize - 1];

  while (len > 0)
  {
    *pDest = *pSource;
    ++pDest;
    pSource += charSize;
    --len;
  }
  *pDest = 0;
}

/*----------------------------------------------------------------------------*/

static void evalDBProcAndParameterName(tak_all_command_glob& acv,
                                tsp00_KnlIdentifier&  dbproc,
                                int                   paramNo,
                                char*                 pParameterName)
{
  tak_param_info_ptr pInfo;
  a12FindParameter (acv, acv.a_curr_user_id, dbproc, paramNo, pInfo);
  if (NULL != pInfo) 
  {
    int len = pInfo->param_name_len;
    if (len > sizeof(tsp00_KnlIdentifier)) 
    {
      len = sizeof(tsp00_KnlIdentifier);
    }
    buildNullTerminatedIdentifier(*REINTERPRET_CAST(tsp00_KnlIdentifier*, &pInfo->param_name[0]),
      pParameterName, len);
  }
  else {
    sprintf(pParameterName, "Parameter(%d)", paramNo);
  }
}
 
/*----------------------------------------------------------------------------*/
 
static void incompatibleAbapDatatypes(ABTypes abapType, tsp00_DataType_Enum paramDataType, char* errorMsg) {
  strcpy(errorMsg, "incompatible datatypes (");
  switch (abapType) {
  case ABTYPC :
    strcat(errorMsg, "ABTYPC");
    break;
  case ABTYPDATE :
    strcat(errorMsg, "ABTYPDATE");
    break;
  case ABTYPP :
    strcat(errorMsg, "ABTYPP");
    break;
  case ABTYPTIME :
    strcat(errorMsg, "ABTYPTIME");
    break;
  case ABTYPX :
    strcat(errorMsg, "ABTYPX");
    break;
  case ABTYPFLOAT :
    strcat(errorMsg, "ABTYPFLOAT");
    break;
  case ABTYPINT :
    strcat(errorMsg, "ABTYPINT");
    break;
  case ABTYPINT2 :
    strcat(errorMsg, "ABTYPINT2");
    break;
  default :
    sprintf(errorMsg, "incompatible datatypes (%d", abapType);
  }
  strcat(errorMsg, ",");
  integer len = (integer)strlen(errorMsg);
  int     pos = (int)strlen(errorMsg);
  g17paramspec_to_line (paramDataType, 0, 0, len, errorMsg);
  // remove length and fraction info
  while ((errorMsg[pos]) && (errorMsg[pos] != '(')) {
    ++pos;
  }
  if ('(' == errorMsg[pos]) {
    errorMsg[pos] = 0;
  }
  strcat(errorMsg, ")");
}

/*----------------------------------------------------------------------------*/

inline static void checkABAPDataTypes (OmsTypeABAPColDesc& colDesc,   
                                tsp00_DataType_Enum paramDataType, 
                                tsp00_Int2          paramLength,
                                bool                isUnicodeClient,
                                bool                swap, 
                                tgg00_BasisError&   e,
                                char*               errorMsg) 
{
  int abapType  = colDesc.abap_type;
  colDesc.inout = OMS_SwapAndModifyInfo::copy;
  switch (abapType) {
  case ABTYPC :
    {
      if (/* (paramDataType != dchb) && PTS 1125121, PG */
          (paramDataType != dcha) && (paramDataType != dwyde)) {
        e = e_incompatible_datatypes;
      }
      else 
      {
        if (isUnicodeClient)
        {
          // transform length in bytes to length in characters
          colDesc.length = colDesc.length / 2;
        }
        if (colDesc.length != paramLength) {
          sprintf(errorMsg, "different length : ABAP(%d), C++(%d)", colDesc.length, paramLength);
          e = e_incompatible_datatypes;
          return;
        }
      }
      break;
    }
  case ABTYPDATE :
    {
      if ((paramDataType != dchb) && (paramDataType != dcha)) {
        e = e_incompatible_datatypes;
      }
      break;
    }
  case ABTYPP :
    {
      if ( paramLength != 8 && paramLength != 15 ) {
        /* PTS 1122649, PG: new type OmsTypePacked_15_3 */  
        sprintf(errorMsg, "wrong length for packed_8_3 or packed_15_3: C++(%d)", paramLength);
        e = e_incompatible_datatypes;
        return;
      }
      if (swap)
      {
//        colDesc.abap_type = OMS_SwapAndModifyInfo::swap_2;
      }
      break;
    }
  case ABTYPTIME :
    {
      break;
    }
  case ABTYPX :
    {
      break;
    }
  case ABTYPTABH : 
    {
      break;
    }
  case ABTYPNUM : 
    {
      if (paramDataType != dcha)
      {
        e = e_incompatible_datatypes;
      }
      else 
      {
        if (isUnicodeClient)
        {
          // transform length in bytes to length in characters
          colDesc.length = colDesc.length / 2;
        }
        if (colDesc.length != paramLength) {
          sprintf(errorMsg, "different length : ABAP(%d), C++(%d)", colDesc.length, paramLength);
          e = e_incompatible_datatypes;
          return;
        }
      }
      break;
    }
  case ABTYPFLOAT :
    {
      if (paramDataType != dfloat) {
        e = e_incompatible_datatypes;
      }
      if (swap)
      {
        colDesc.inout = OMS_SwapAndModifyInfo::swap_8;
      }
      break;
    }
  case ABTYPINT :
    {
      if (paramDataType != dinteger) {
        e = e_incompatible_datatypes;
      }
      if (swap)
      {
        colDesc.inout = OMS_SwapAndModifyInfo::swap_4;
      }
      break;
    }
  case ABTYPINT2 :
    {
      if (paramDataType != dsmallint) {
        e = e_incompatible_datatypes;
      }
      if (swap)
      {
        colDesc.inout = OMS_SwapAndModifyInfo::swap_2;
      }
      break;
    }
  case ABTYPINT1 :
    {
      /* PTS 1125121, PG */
      if (paramDataType != dchb) {
        e = e_incompatible_datatypes;
      }
      break;
    }
  case ABTYPW :
    {
      break;
    }
  }
  if (e != e_ok) {
    incompatibleAbapDatatypes((ABTypes) abapType, paramDataType, errorMsg);
  }
}

/*----------------------------------------------------------------------------*/

static void incompatibleStreamDatatypes(OmsStreamTypes streamType, tsp00_DataType_Enum paramDataType, char* errorMsg) {
    strcpy(errorMsg, "incompatible datatypes (");
    switch (streamType) {
          case STYPE_CHAR :
              strcat (errorMsg, "STYPE_CHAR");
              break;
          case STYPE_I4  :
              strcat (errorMsg, "STYPE_I4");
              break;
          case STYPE_INT :
              strcat (errorMsg, "STYPE_INT");
              break;
          case STYPE_UI2 :
              strcat (errorMsg, "STYPE_UI2");
              break;
          case STYPE_UI4 :
              strcat (errorMsg, "STYPE_UI4");
              break;
          case STYPE_UINT:
              strcat (errorMsg, "STYPE_UINT");
              break;
          case STYPE_I2  :
              strcat (errorMsg, "STYPE_I2");
              break;
          case STYPE_I1 :
              strcat (errorMsg, "STYPE_I1");
              break;
          case STYPE_UI1:
              strcat (errorMsg, "STYPE_UI1");
              break;
          case STYPE_R4 :
              strcat (errorMsg, "STYPE_R4");
              break;
          case STYPE_R8 :
              strcat (errorMsg, "STYPE_R8");
              break;
          default :
              sprintf(errorMsg, "incompatible datatypes (%d", streamType);
    }
    strcat(errorMsg, ",");
    integer len = (integer)strlen(errorMsg);
    int     pos = (int)strlen(errorMsg);
    g17paramspec_to_line (paramDataType, 0, 0, len, errorMsg);
    // remove length and fraction info
    while ((errorMsg[pos]) && (errorMsg[pos] != '(')) {
        ++pos;
    }
    if ('(' == errorMsg[pos]) {
        errorMsg[pos] = 0;
    }
    strcat(errorMsg, ")");
}

/*----------------------------------------------------------------------------*/

inline static void checkStreamDataTypes (OmsTypeABAPColDesc& colDesc,
                                  tsp00_DataType_Enum paramDataType,
                                  bool                swap, 
                                  tgg00_BasisError&   e,
                                  char*               errorMsg) 
{
    OmsStreamTypes streamType = (OmsStreamTypes) colDesc.abap_type;
    colDesc.inout = OMS_SwapAndModifyInfo::copy;
    switch (streamType) {
    case STYPE_CHAR :
        {
            if ((paramDataType != dchb) && (paramDataType != dcha)) {
                e = e_incompatible_datatypes;
                return;
            }
            break;
        }
    case STYPE_WYDE:
        {
            if((dwyde != paramDataType)) {
                e = e_incompatible_datatypes;
            }  
            // swap is corrected in caller
            break;
        }
    case    STYPE_I4  :
    case    STYPE_INT :
        {
            if (paramDataType != dinteger) {
                e = e_incompatible_datatypes;
                return;
            }
            if (swap)
            {
                colDesc.inout = OMS_SwapAndModifyInfo::swap_4;
            }
            break;
        }
    case  STYPE_UI2 :
    case  STYPE_UI4 :
    case  STYPE_UINT:
        {
            if (paramDataType != dfixed) {
                e = e_incompatible_datatypes;
                return;
            }
            if (swap)
            {
                colDesc.inout = (STYPE_UI2 == streamType) ? OMS_SwapAndModifyInfo::swap_4 : OMS_SwapAndModifyInfo::swap_2;
            }
            break;
        }
    case  STYPE_I2  :
        {
            if (paramDataType != dsmallint) {
                e = e_incompatible_datatypes;
                return;
            }
            if (swap)
            {
                colDesc.inout = OMS_SwapAndModifyInfo::swap_2;
            }
            break;
        }
    case STYPE_I1 :
    case STYPE_UI1:
        {
            break;
        }
    case STYPE_R4 :
    case STYPE_R8 :
        {
            if (paramDataType != dfloat) {
                e = e_incompatible_datatypes;
                return;
            }
            if (swap)
            {
                colDesc.inout = (STYPE_R8 == streamType) ? OMS_SwapAndModifyInfo::swap_8 : OMS_SwapAndModifyInfo::swap_4;
            }
            break;
        }

    default :
        e = e_not_implemented;
    }
}

/*----------------------------------------------------------------------------*/
/* PTS 1107791 */
static void buildStreamDescriptorElement(tsp00_DataType           dataType,
                                       int                 offset,
                                       int                 length,
                                       bool                swap,
                                       OmsTypeABAPColDesc& colDesc,
                                       tgg00_BasisError&   e)
{
  e                 = e_ok;
  colDesc.abap_type = 0;
  colDesc.inout     = OMS_SwapAndModifyInfo::copy;;
  colDesc.dec       = 0;
  switch(dataType) {
  case dfixed :
    {
      switch (length) {
      case 2 : /* 2 digits => 1 Byte Integer */
        return;
      case 5 : /* 5 digits => 2 Byte Integer */
        if (swap)
        {
          colDesc.inout = OMS_SwapAndModifyInfo::swap_2;
        }
        colDesc.length    = 2;
        break;
      case 10 : /* 10 digits => 4 Byte Integer */
        if (swap)
        {
          colDesc.inout = OMS_SwapAndModifyInfo::swap_4;
        }
        colDesc.length    = 4;
        break;
      case 19 : /* 19 digits => 8 Byte Integer */
        if (swap)
        {
          colDesc.inout = OMS_SwapAndModifyInfo::swap_8;
        }
        colDesc.length    = 8;
        break;
      default :
        {
          e = e_invalid;
        }
      }
      colDesc.offset = offset;
      return;
    }
  case dfloat :
    if (6 == length) {
      if (swap)
      {
        colDesc.inout = OMS_SwapAndModifyInfo::swap_4;
      }
      colDesc.length    = 4;
    }
    else {
      if (15 == length) {
        if (swap)
        {
          colDesc.inout = OMS_SwapAndModifyInfo::swap_8;
        }
        colDesc.length    = 8;
      }
      else {
        e = e_invalid;
      }
    }
    colDesc.offset = offset;
    return;
  case dinteger :
    if (10 == length) {
      if (swap)
      {
        colDesc.inout = OMS_SwapAndModifyInfo::swap_4;
      }
      colDesc.length = 4;
    }
    else {
      if (19 == length) {
        if (swap)
        {
          colDesc.inout = OMS_SwapAndModifyInfo::swap_8;
        }
        colDesc.length    = 8;
      }
      else {
        e = e_invalid;
      }
    }
    colDesc.offset    = offset;
    return;
  case dsmallint :
    if (swap)
    {
      colDesc.inout = OMS_SwapAndModifyInfo::swap_2;
    }
    colDesc.length    = sizeof(tsp00_Int2);
    colDesc.offset    = offset;
    return;
  default :
    colDesc.length    = length;
    colDesc.offset    = offset;
    break;
  }
}

/*----------------------------------------------------------------------------*/

inline static void ak263BuildStreamSwapInfo(OmsTypeABAPColDesc& desc)
{
  switch (desc.abap_type)
  {
  case ABTYPC :
    {
      desc.inout = OMS_SwapAndModifyInfo::swap_2;
      break;
    }
  case STYPE_I2  :
    case STYPE_UI2 :
    {
      desc.inout = OMS_SwapAndModifyInfo::swap_2;
      desc.length    = 1;
      break;
    }
  case STYPE_INT  :
  case STYPE_UINT : 
  case STYPE_I4   :
    case STYPE_UI4  :
  case STYPE_R4   :
    {
       desc.inout = OMS_SwapAndModifyInfo::swap_4;
       break;
    }
  case STYPE_I8  :
    case STYPE_UI8 :
  case STYPE_R8  :
    {
      desc.inout = OMS_SwapAndModifyInfo::swap_8;
      break;
    }
  }
}

/* ========================================================================== */
/* ==========              DEFINITION OF GLOBAL FUNCTIONS           ========= */
/* ========================================================================== */

externC
void ak263CheckHandle (tak_all_command_glob& acv,
            tsp00_Int4&           st_ix,
            ak263_StreamHandle*   phandle,
            int                   handleLen,
            bool                  isABAP,
            tgg00_BasisError&     e,
            tsp00_Int4            paramNo,
            tsp00_KnlIdentifier&  methodName)

{ 
      const pasbool isError = true;
 
      tsp00_DataType          paramDataType;
      int                     ix;
      int                     col_count = 0;
      bool                    swap;
      tsp00_Int4              errIndex = 0;
      OmsTypeABAPColDesc      *colDesc;
      char                    errorMsg[256];
      char                    cMethodName[sizeof(tsp00_KnlIdentifier)+1];
      char                    cParamName [sizeof(tsp00_KnlIdentifier)+1];
 
      /* DEC OSF1 cxx had problem with tsp00_swap_kind and tsp00_SwapKind mixup */
      bool isUnicodeClient = 
        ((csp_unicode      == acv.a_cmd_packet_header.sp1h_mess_code) ||
        ( csp_unicode_swap == acv.a_cmd_packet_header.sp1h_mess_code));
      swap = ( ((int)acv.a_out_packet->sp1_header.sp1h_mess_swap) 
            != ((int)g01code.kernel_swap));
      if (swap) {
        // swap handle
        swapInt2 (phandle->tabHandle.rowSize);
        swapInt2 (phandle->tabHandle.colCount);
        swapInt4 (phandle->tabHandle.rowCount);
        if ((int)(sizeof(*phandle) - sizeof(phandle->tabHandle.colDesc)  +
          phandle->tabHandle.colCount * sizeof(phandle->tabHandle.colDesc[0])) > handleLen) {
          phandle->tabHandle.colCount = (unsigned short)(handleLen - 
            (sizeof(*phandle) - sizeof(phandle->tabHandle.colDesc))) / sizeof(phandle->tabHandle.colDesc[0]);
        }
        for (ix = 0; ix < phandle->tabHandle.colCount; ix++) {
          colDesc = &phandle->tabHandle.colDesc[ix];
          swapInt2 (colDesc->dec);
          swapInt2 (colDesc->length);
          swapInt2 (colDesc->offset);
        }
      }
      bool isApplicationHandle = (0 != phandle->tabHandle.colCount); 
      OmsTypeABAPColDesc* abapColDesc = &phandle->tabHandle.colDesc[0];
      tgg00_StackEntry*   st_curr     = &((*acv.a_mblock.mb_st())[st_ix]);
      tgg00_StackEntry*   st_upb      = &((*acv.a_mblock.mb_st())
             [acv.a_mblock.mb_qual()->mqual_pos() - 1 + acv.a_mblock.mb_qual()->mqual_cnt() - 1]);
      e = e_ok;
      do {
        if  (st_curr > st_upb) {
          break;
        }
        else
          if (st_curr->etype() != st_op) {
            break;
          }
          else {
            ++errIndex;
            st_ix += 2;
            paramDataType = st_curr->eparamdatatype();
            ++col_count;
            if (isABAP) {
              if (col_count > phandle->tabHandle.colCount) {
                sprintf(errorMsg, "too many members in C++ definition");
                e = e_too_many_columns;
              }
            }
            if (isABAP) {
              if (abapColDesc->offset != clientOffset(st_curr))
              {
                sprintf(errorMsg, "different offsets : ABAP(%d), expected(%d)", abapColDesc->offset, clientOffset(st_curr));
                e = e_incompatible_datatypes;
              }
              else {
                checkABAPDataTypes (*abapColDesc,  
                  paramDataType, st_curr->elen_var(), isUnicodeClient, swap, e, errorMsg);
              }
            }
            else {
              if (!isApplicationHandle) {
                /* PTS 1107791 */
                buildStreamDescriptorElement(paramDataType, 
                  clientOffset(st_curr), st_curr->elen_var(), swap, *abapColDesc, e);
              }
              else {
                 checkStreamDataTypes (*abapColDesc, paramDataType, swap, e, errorMsg);
              }
            }
            if (e_ok != e) {
              char OpMsg[400];
              buildNullTerminatedIdentifier(methodName, cMethodName);
              evalDBProcAndParameterName(acv, methodName, paramNo, cParamName);   
              tsp00_KnlIdentifier errorIdentifier;
              SAPDB_MemFillNoCheck(&errorIdentifier[0], ' ', sizeof(errorIdentifier));
              sprintf((char*) &errorIdentifier[0], "(Param %d,Member %d)", paramNo, errIndex);
              a07_const_b_put_error(acv, e, 1, (tsp00_MoveObjPtr) &errorIdentifier[0], (int) strlen(errorIdentifier));
              sprintf(OpMsg, "%d %s::%s(member%d) : %s", acv.a_returncode, 
                cMethodName, cParamName, errIndex, errorMsg); 
              a34OpMsg (isError, *REINTERPRET_CAST(tsp00_MoveObj*, OpMsg), (int)strlen(OpMsg));
              return;
            }
            abapColDesc->dec = cppOffset(st_curr);
            switch (paramDataType)
            {
            case dwyde :
                {
                    abapColDesc->abap_type = ABTYPWYDE;
                    if ((!isUnicodeClient) && (!acv.a_pseudoUnicodeClient))
                    {
                        abapColDesc->inout = OMS_SwapAndModifyInfo::asciiClientWydeCpp;
                    }
                    else
                    {
                        if (swap)
                        {
                            abapColDesc->inout = OMS_SwapAndModifyInfo::swap_2;
                        }
                        else
                        {
                            // member will be just copied, transform length in character 
                            // to length in bytes
                            abapColDesc->length = abapColDesc->length * 2;
                        }
                    }
                    break;
                }
            case dcha :
              {
                if (isUnicodeClient)
                {
                  if (csp_unicode == acv.a_cmd_packet_header.sp1h_mess_code)
                  {
                    abapColDesc->inout = OMS_SwapAndModifyInfo::unicodeClientAsciiCpp;
                  }
                  else
                  {
                    abapColDesc->inout = OMS_SwapAndModifyInfo::swappedUnicodeClientAsciiCpp;
                  }
                }
                break;
              }
            default :
              {
              }
            }
            ++abapColDesc;
            st_curr += 2;
          }
      }
      while (true);
      if (isABAP) {
        if (col_count != phandle->tabHandle.colCount) 
        {
          buildNullTerminatedIdentifier(methodName, cMethodName);
          evalDBProcAndParameterName(acv, methodName, paramNo, cParamName);
          e = e_too_few_columns;
          a07_b_put_error(acv, e, 1);
          sprintf(errorMsg, "%d in dbproc %s, parameter %s :", 
            acv.a_returncode, cMethodName, cParamName);
          a34OpMsg (isError, *REINTERPRET_CAST(tsp00_MoveObj*, errorMsg), (int)strlen(errorMsg));
          sprintf(errorMsg, "too few members in C++ definition, ABAP(%d),C++(%d)", 
            phandle->tabHandle.colCount, col_count);
          a34OpMsg (isError, *REINTERPRET_CAST(tsp00_MoveObj*, errorMsg), (int)strlen(errorMsg));
        }
      }
      else
      {
        phandle->tabHandle.colCount = col_count;
      }
}

/*----------------------------------------------------------------------------*/

tsp00_Int4 ak263EvalHandleLength (tsp00_Int4 ColCount) {
  ak263_StreamHandle h;
  h.tabHandle.colCount = 0; /* just to suppress compiler warning */
  return (sizeof (h) - sizeof(h.tabHandle.colDesc) + 
    ColCount * sizeof(h.tabHandle.colDesc[0])); 
}

/*-----------------------------------------------------------------------------*/

void ak263NumberToInt8 (
    tsp00_MoveObjPtr                   pNumber,
    int                                inOutLength,
    tsp00_8ByteCounter  VAR_VALUE_REF  result,
    tsp00_NumError      VAR_VALUE_REF  e)
{
    IFR_Int8*   pRes = reinterpret_cast<IFR_Int8*>(&result);
    IFR_Retcode ret = IFRUtil_VDNNumber::numberToInt8(reinterpret_cast<unsigned char*> (pNumber), *pRes, inOutLength);
    if (IFR_OK == ret) 
    {
        e.becomes (num_ok);
    }
    else
    {
        e.becomes(num_trunc);
    }
}

/*-----------------------------------------------------------------------------*/

void ak263NumberToUInt8 (
    tsp00_MoveObjPtr                   pNumber,
    int                                inOutLength,
    tsp00_8ByteCounter  VAR_VALUE_REF  result,
    tsp00_NumError      VAR_VALUE_REF  e)
{
    IFR_UInt8*   pRes = reinterpret_cast<IFR_UInt8*>(&result);
    IFR_Retcode ret = IFRUtil_VDNNumber::numberToUInt8(reinterpret_cast<unsigned char*> (pNumber), *pRes, inOutLength);
    if (IFR_OK == ret) 
    {
        e.becomes (num_ok);
    }
    else
    {
        e.becomes(num_trunc);
    }
}


/*-----------------------------------------------------------------------------*/

void ak263Int8ToNumber (
     tsp00_8ByteCounter                 source,
     tsp00_MoveObjPtr                   pNumber,
     int                                inOutLength,
     tsp00_NumError      VAR_VALUE_REF  e)
{
    IFR_Int8* pSource = reinterpret_cast<IFR_Int8*>(&source);
    IFR_Retcode ret = IFRUtil_VDNNumber::int8ToNumber(*pSource, reinterpret_cast<unsigned char*>(pNumber), (inOutLength - 2) * 2);
    if (IFR_OK == ret) 
    {
        e.becomes (num_ok);
    }
    else
    {
        e.becomes(num_trunc);
    }
}

/*-----------------------------------------------------------------------------*/

void ak263UInt8ToNumber (
     tsp00_8ByteCounter                 source,
     tsp00_MoveObjPtr                   pNumber,
     int                                inOutLength,
     tsp00_NumError      VAR_VALUE_REF  e)
{
    IFR_UInt8* pSource = reinterpret_cast<IFR_UInt8*>(&source);
    IFR_Retcode ret = IFRUtil_VDNNumber::uint8ToNumber(*pSource, reinterpret_cast<unsigned char*>(pNumber), (inOutLength - 2) * 2);
    if (IFR_OK == ret) 
    {
        e.becomes (num_ok);
    }
    else
    {
        e.becomes(num_trunc);
    }
}

/*-----------------------------------------------------------------------------*/

void ak263WriteConversionError (tak_all_command_glob& acv,
                                ak263_StreamHandle*   phandle,
                                int                   paramNo,
                                tsp00_Int4            tabHandle,
                                tsp00_KnlIdentifier&  methodName)
{
    if (phandle->tabHandle.ABAPTabId == tabHandle)
    {
        char cMethodName[sizeof(tsp00_KnlIdentifier)+1];
        char cParamName [sizeof(tsp00_KnlIdentifier)+1];
        char OpMsg[400];
        buildNullTerminatedIdentifier(methodName, cMethodName);
        evalDBProcAndParameterName(acv, methodName, paramNo, cParamName);
        sprintf(OpMsg, "parameter is %s::%s", cMethodName, cParamName); 
        a34OpMsg (true, *REINTERPRET_CAST(tsp00_MoveObj*, OpMsg), (int)strlen(OpMsg));
    }
}

/*-----------------------------------------------------------------------------*/

void ak263WriteTranslationError (
    tak_all_command_glob  VAR_VALUE_REF  acv,
    tsp00_KnlIdentifier   VAR_ARRAY_REF  methodName,
    integer                              paramNo,
    integer                              errorOffset,
    integer                              errorChar)
{
    char cMethodName[sizeof(tsp00_KnlIdentifier)+1];
    char cParamName [sizeof(tsp00_KnlIdentifier)+1];
    char OpMsg[400];
    buildNullTerminatedIdentifier(methodName, cMethodName);
    evalDBProcAndParameterName(acv, methodName, paramNo, cParamName);
    sprintf(OpMsg, "parameter %s::%s not translatable at offset %d : d", cMethodName, cParamName, errorOffset, errorChar); 
    a34OpMsg (true, *REINTERPRET_CAST(tsp00_MoveObj*, OpMsg), (int)strlen(OpMsg));
}