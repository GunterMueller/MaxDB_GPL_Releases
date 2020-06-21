/*!
  @file           PIn_SpecialParts.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          Sub classes to represent common part types

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

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"
#include "SAPDB/PacketInterface/PIn_SpecialParts.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "hsp40.h"
#include "hsp77.h"

/*! @brief constructor */
PIn_SpecialPart::PIn_SpecialPart 
  ( PIn_Part & oPart, tsp1_part_kind_Param partKind  )
  : PIn_Part( oPart )
{
  if (this->GetPartKind() != partKind) {
    this->Invalidate();
  } // end if
  
} // end PIn_SpecialPart::PIn_SpecialPart

/* ======================================================== */

/*! @brief constructor */
PIn_MessageListPart::PIn_MessageListPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_message_list)
{
} // end PIn_MessageListPart::PIn_MessageListPart

/*! @brief destructor */
PIn_MessageListPart::~PIn_MessageListPart 
  ( )
{
} // end PIn_MessageListPart::~PIn_MessageListPart

/*! @brief public member */
SAPDBErr_MessageList const & PIn_MessageListPart::MessageList
  ( )
{
  SAPDB_Bool wasTruncated; // ignored
  Msg_List errList; // ignored

  this->oMessageList.Load( this->Length(),
                           (void *)this->GetReadData(),
                           wasTruncated,
                           errList );
  // wouldn't at least a message be fine... 
  // But for now we can live with ignoring erros sinces messages are handled as optional even in kernel...
  return this->oMessageList;
} // end PIn_MessageListPart::MessageList

/* ======================================================== */

/*! @brief constructor */
PIn_ErrorTextPart::PIn_ErrorTextPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_errortext)
{
} // end PIn_ErrorTextPart::PIn_ErrorTextPart

/*! @brief destructor */
PIn_ErrorTextPart::~PIn_ErrorTextPart 
  ( )
{
} // end PIn_ErrorTextPart::~PIn_ErrorTextPart

/* ======================================================== */

/*! @brief constructor */
PIn_SessionInfoPart::PIn_SessionInfoPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_session_info_returned)
{
} // end PIn_SessionInfoPart::PIn_SessionInfoPart

/*! @brief destructor */
PIn_SessionInfoPart::~PIn_SessionInfoPart 
  ( )
{
} // end PIn_SessionInfoPart::~PIn_SessionInfoPart

/*! @brief public member */
int PIn_SessionInfoPart::IsUnicode 
  ( )
{
  if (this->IsValid()) {
    return this->GetReadData()[0];
  } // end if

  return -1;
} // end PIn_SessionInfoPart::IsUnicode

/*! @brief public member */
int
PIn_SessionInfoPart::getPageSize()
{
    SAPDB_Int4 _lPageSize = 0;
    if (this->IsValid())
    {
        // page size information is 5 Bytes long
        SAPDB_Byte szPageSize[5];
        memcpy(szPageSize, this->GetReadData(2193), 5);

        // In case of error the value _lPageSize is not changed
        IFRUtil_VDNNumber::numberToInt4(szPageSize, _lPageSize, 5);
    } // end if

  return _lPageSize;
} // end PIn_SessionInfoPart::IsUnicode

/* ======================================================== */

/*! @brief constructor */
PIn_UserInfoPart::PIn_UserInfoPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_user_info_returned)
{
  int nSize = 0;

  nSize = PIn_UserInfoPart_SYSDBALength;
  GetField(1, szSYSDBA, nSize);

  nSize = PIn_UserInfoPart_TypeLength;
  GetField(3, szType, nSize);

  // User group
  nSize = PIn_UserInfoPart_SYSDBALength;
  GetField(4, szUserGroup, nSize);
} // end PIn_UserInfoPart::PIn_UserInfoPart

/*! @brief destructor */
PIn_UserInfoPart::~PIn_UserInfoPart 
  ( )
{
} // end PIn_UserInfoPart::~PIn_UserInfoPart

/*! @brief retireve a filed */
bool PIn_UserInfoPart::GetField
        ( int    nNumber,
          char * szBuffer,
          int  & nSizeInOut )
{
  bool bReturn   = false;
  int  nElement  = 0;
  int  nPosition = 0;
  int  nSize     = 0;
  int  nSizeIn   = nSizeInOut;

  nSizeInOut = -1;

  memset(szBuffer, 0, nSizeIn);

  if (this->IsValid()) {
    while ((nElement < nNumber) && (nPosition < this->Length())) {
      ++nElement;
      nSize = this->GetReadData()[nPosition];
      ++nPosition;
      if (nElement == nNumber) {
        nSizeInOut = nSize;
        if (nSize < nSizeIn)
        {
          if (0 != nSize)
          {
            memcpy(szBuffer, &this->GetReadData()[nPosition], nSize);
          }
          bReturn = true;
        } // end if
        break;
      } // end if
      nPosition = nPosition + nSize;
    } // end while
  } // end if

  return bReturn;
} // end PIn_UserInfoPart::GetField

/* ======================================================== */

/*! @brief constructor */
PIn_DataPart::PIn_DataPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_data)
{
} // end PIn_DataPart::PIn_DataPart

/*! @brief destructor */
PIn_DataPart::~PIn_DataPart 
  ( )
{
} // end PIn_DataPart::~PIn_DataPart

/*! @brief public member */
const tsp00_Byte * PIn_DataPart::GetValueAdress
  ( tsp1_param_info * pInfo )
{
  const tsp00_Byte      * pValue = NULL;
  tsp00_LongDescriptor    aDescriptor;

  switch (pInfo->sp1i_data_type) {
    case dstra:
    case dstrb:
      // long data type found
      pValue = (const tsp00_Byte *) this->GetReadData() + pInfo->sp1i_bufpos() - 1;
      pValue = (*pValue == csp_undef_byte) ? NULL : pValue + 1;

      if (pValue != NULL) {
        memcpy(&aDescriptor, (pValue), sizeof(tsp00_LongDescriptor));
        pValue = (const tsp00_Byte *) this->GetReadData() + aDescriptor.ld_valpos () - 1;
        pInfo->sp1i_length = aDescriptor.ld_vallen();
      } // end if
      break;
    default:
      // other data type
      pValue = (const tsp00_Byte *) this->GetReadData() + pInfo->sp1i_bufpos() - 1;
      pValue = (*pValue == csp_undef_byte) ? NULL : pValue + 1;
      break;
  } // end switch

  return pValue;
} // end PIn_DataPart::GetValueAdress

/*! @brief public member */
bool PIn_DataPart::IsNull
  ( tsp1_param_info * pInfo )
{
  return (this->GetValueAdress(pInfo) == NULL);
} // end PIn_DataPart::IsNull

/*! @brief public member */
bool PIn_DataPart::GetValueAsInt
  ( tsp1_param_info * pInfo,
    tsp00_Int4      & nValue )
{
  const tsp00_Byte * pAddr    = this->GetValueAdress(pInfo);
  bool               bReturn  = true;
  tsp00_NumError     nError;

  nError.becomes(num_ok);

  if (pAddr != NULL) {
    switch (pInfo->sp1i_data_type) {
      case dfixed:
      case dfloat:
      case dvfloat:
      case dsmallint:
      case dinteger:
//        s40glint ((tsp00_Number*)pAddr, 1, pInfo->sp1i_in_out_len,  nValue, nError);
        s40glint ((tsp00_Number*)pAddr, 1, pInfo->sp1i_length,  nValue, nError);
        bReturn = (nError == num_ok);
        break;
      case dboolean:
        nValue  = (*pAddr == 0) ? 0 : 1;
        break;
      default:
        bReturn = false;
        break;
    } // end switch
  } else {
    bReturn = false;
  } // end if

  return bReturn;
} // end PIn_DataPart::GetValueAsInt

/*! @brief public member */
bool PIn_DataPart::GetValueAsChar
  ( tsp1_param_info *  pInfo,
    const char      *& pValue,
    tsp00_Int4       & nLength,
    ConversionNeeded & oConversion )
{
  const tsp00_Byte * pAddr    = this->GetValueAdress(pInfo);
  bool               bReturn  = true;
  tsp00_NumError     nError;
  static char        szValue[1000];
  const char       * pChars = "0123456789ABCDEF";      

  nError.becomes(num_ok);
  oConversion = ConvertUnknown;

  if (pAddr != NULL) {
    switch (pInfo->sp1i_data_type) {
      case dfixed:
      case dsmallint:
      case dinteger:
      case dfloat:
      case dvfloat:
        if (IFRUtil_VDNNumber::numberToString((tsp00_Byte *) pAddr,
                                              &szValue[0], 
                                              sizeof(szValue),
                                              IFR_StringEncodingAscii,
                                              (pInfo->sp1i_data_type != dvfloat) && (pInfo->sp1i_data_type != dfloat),
                                              pInfo->sp1i_length,
                                              pInfo->sp1i_frac) != IFR_OK) {
          bReturn = false;
        } else {
          nLength = (tsp00_Int4) strlen(szValue);
          pValue  = &szValue[0];
          oConversion = ConvertNever;
        } // end if
        break;

      case dboolean:
        if (*pAddr == 0) {
          strcpy(szValue, "false");
          nLength = (tsp00_Int4) strlen(szValue);
          pValue  = &szValue[0];
        } else {
          strcpy(szValue, "true");
          nLength = (tsp00_Int4) strlen(szValue);
          pValue  = &szValue[0];
        } // end if
        oConversion = ConvertNever;
        break;
      case dchb:
      case dvarcharb:
      case dstrb:
        nLength = 0;
        szValue[0] = 'x';
        szValue[1] = '\'';
        for (nLength = 0; nLength < pInfo->sp1i_length; ++nLength) {
          if (2*nLength > 996) {
            break;
          } // end if
          szValue[2*nLength + 2] = pChars[pAddr[nLength]>>4];
          szValue[2*nLength + 3] = pChars[(pAddr[nLength] & 0x0F)];
        } // end for
        szValue[2*nLength + 2] = '\'';
        nLength = 2*nLength + 3;
        pValue  = &szValue[0];
        oConversion = ConvertNever;
        break;
      case dcha:
      case dvarchara:
      case ddate:
      case dtime:
      case dtimestamp:
      case dstra:
        nLength = pInfo->sp1i_length;
        pValue  = (const char *) pAddr;
        oConversion = ConvertSometimes;
        break;
      case dunicode:
      case dvarcharuni:
        nLength = pInfo->sp1i_length;
        pValue  = (const char *) pAddr;
        oConversion = ConvertAlways;
        break;
      default:
        bReturn = false;
        break;
    } // end switch
  } else {
    strcpy(szValue, "(null)");
    nLength = (tsp00_Int4) strlen(szValue);
    pValue  = &szValue[0];
    oConversion = ConvertNever;
  } // end if

  return bReturn;
} // end PIn_DataPart::GetValueAsChar


/* ======================================================== */

/*! @brief constructor */
PIn_ColumnNamesPart::PIn_ColumnNamesPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_columnnames)
{
    m_nColumnNamesCount = GetColumnNamesCount();
    m_nPartOffset       = 0;
}
// end PIn_ColumnNamesPart::PIn_ColumnNamesPart

/*! @brief destructor */
PIn_ColumnNamesPart::~PIn_ColumnNamesPart()
{
}
// end PIn_ColumnNamesPart::~PIn_ColumnNamesPart

/*! @brief public member */
char* 
PIn_ColumnNamesPart::GetNextColumnName(char *pszBuffer, SAPDB_UInt4& nBufferLength)
{
//TODO Error handling?
    char* pszReturn = NULL;

    if (0 == m_nColumnNamesCount)
    {
        m_nPartOffset = 0;
        m_nColumnNamesCount = GetColumnNamesCount();
    }
    else//if (0 != m_nColumnNamesCount)
    {
        char cLength = *this->GetReadData(m_nPartOffset);
        m_nPartOffset += sizeof(cLength);

        memcpy(pszBuffer, this->GetReadData(m_nPartOffset), cLength);

        m_nPartOffset += cLength;


        nBufferLength   = cLength;
        pszReturn       = pszBuffer;
        --m_nColumnNamesCount;
    }
    return pszReturn;
}


SAPDB_UInt4
PIn_ColumnNamesPart::GetColumnNamesCount() const
{
    return this->GetRawHeader()->sp1p_arg_count;
}


/* ======================================================== */

/*! @brief constructor */
PIn_KeyPart::PIn_KeyPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_key)
{

    // Key is at maximum 1024 Bytes long
    if (this->IsValid())
    {
        const SAPDB_Byte* ptr = GetReadData();

        SAPDB_Int2 swapTest = 1;
        SAPDB_Bool bSwapped = (*(char *)&swapTest) ? SAPDB_TRUE : SAPDB_FALSE;
        if (SAPDB_TRUE == bSwapped)
        {
            (REINTERPRET_CAST(char*,&m_nKeyLength))[1] = ptr[0];
            (REINTERPRET_CAST(char*,&m_nKeyLength))[0] = ptr[1];
        }
        else
        {
            memcpy(&m_nKeyLength, ptr, sizeof(SAPDB_Int2));
        }

        m_pszKey = &ptr[sizeof(SAPDB_Int2)];

    }
}
// end PIn_KeyPart::PIn_KeyPart

/*! @brief destructor */
PIn_KeyPart::~PIn_KeyPart()
{
}
// end PIn_KeyPart::~PIn_KeyPart

/*! @brief public member */
void
PIn_KeyPart::GetKey(SAPDB_Byte *pszKeyBuf, SAPDB_UInt4& nKeyLength) const
{
    if (0 == m_nKeyLength)
    {
        nKeyLength = 0;
    }
    else
    {
        nKeyLength = m_nKeyLength;
        memcpy(pszKeyBuf, m_pszKey, m_nKeyLength);
    }
}


/* ======================================================== */

/*! @brief constructor */
PIn_SurrogatePart::PIn_SurrogatePart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_surrogate)
{

    // A surrogate is always built like this:
    //  --------------------------------------------------------------------------------------------------
    //  | 1 byte surrogate value length | surrogate value, usually 9 bytes long (including defined byte) |
    //  --------------------------------------------------------------------------------------------------
    if (this->IsValid())
    {
        const SAPDB_Byte* ptr = GetReadData();

        m_nSurrogateLength = *ptr;
        m_pszSurrogate     = &ptr[sizeof(SAPDB_Char)];
    }
}
// end PIn_SurrogatePart::PIn_SurrogatePart

/*! @brief destructor */
PIn_SurrogatePart::~PIn_SurrogatePart()
{
}
// end PIn_SurrogatePart::~PIn_SurrogatePart

/*! @brief public member */
void
PIn_SurrogatePart::GetSurrogate(SAPDB_Byte *pszStampBuf, SAPDB_UInt4& nStampLength) const
{
    nStampLength = m_nSurrogateLength;
    memcpy(pszStampBuf, m_pszSurrogate, m_nSurrogateLength);
}



/* ======================================================== */

/*! @brief constructor */
PIn_BDInfoPart::PIn_BDInfoPart 
  ( PIn_Part & oPart )
  : PIn_SpecialPart(oPart, sp1pk_bdinfo),
    m_nBDInfoLength(48)
{

    // A surrogate is always built like this:
    //  --------------------------------------------------------------------------------------------------
    //  | 1 byte surrogate value length | surrogate value, usually 9 bytes long (including defined byte) |
    //  --------------------------------------------------------------------------------------------------
    if (this->IsValid())
    {
        m_pszBDInfo = GetReadData();
    }
}
// end PIn_BDInfoPart::PIn_BDInfoPart

/*! @brief destructor */
PIn_BDInfoPart::~PIn_BDInfoPart()
{
}
// end PIn_BDInfoPart::~PIn_BDInfoPart

/*! @brief public member */
void
PIn_BDInfoPart::GetBDInfo(SAPDB_Byte *pszBDInfoBuf) const
{
    memcpy(pszBDInfoBuf, m_pszBDInfo, m_nBDInfoLength);
}