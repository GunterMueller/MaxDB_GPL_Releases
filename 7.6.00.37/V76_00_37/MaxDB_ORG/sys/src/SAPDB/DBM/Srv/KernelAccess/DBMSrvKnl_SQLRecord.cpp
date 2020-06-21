/*!
  @file           DBMSrvKnl_SQLRecord.cpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer to Kernel Communication - Implementation

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
#include "SAPDB/DBM/Srv/KernelAccess/DBMSrvKnl_Session.hpp"

/*-----------------------------------------------------------------------------*/
/*! @brief private constructor */
DBMSrvKnl_SQLRecord :: DBMSrvKnl_SQLRecord
    ( )
{
  this->RefreshRecordData(DBMSrvKnl_SQLRecord::resetAll);
} // end DBMSrvKnl_SQLRecord :: DBMSrvKnl_SQLRecord

/*-----------------------------------------------------------------------------*/
/*! @brief destructor */
DBMSrvKnl_SQLRecord :: ~DBMSrvKnl_SQLRecord
    (  )
{
} // end DBMSrvKnl_SQLRecord :: ~DBMSrvKnl_SQLRecord

/*! @brief private member */
void DBMSrvKnl_SQLRecord :: SetSession
    ( DBMSrvKnl_Session * pSession )
{
  this->pKernelSession = pSession;
  this->RefreshRecordData(DBMSrvKnl_SQLRecord::resetAll);
} // end DBMSrvKnl_SQLRecord :: SetSession

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_Int4 DBMSrvKnl_SQLRecord :: fieldAsInt ( const tsp00_Int4 nField  ) const
{
  if (nField <= this->nParamCount && nField > 0) {
    DBMSrvKnl_SQLField oField(&this->pParamInfo[nField-1], DBMSrvKnl_SQLField::typeInt);
    oField.oInfo.sp1i_bufpos() = oField.oInfo.sp1i_bufpos() + ((this->nCurrentRecord-1)*this->nRecordSize);

    if (this->pKernelSession->GetValue(nField, oField)) {
      return oField.iValue;
    } // end if
  } // end if

  return 0;
} // end DBMSrvKnl_SQLRecord :: fieldAsInt

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_Int4 DBMSrvKnl_SQLRecord :: fieldAsInt ( const Tools_DynamicUTF8String & szField ) const
{
  return fieldAsInt(fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: fieldAsInt

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
Tools_DynamicUTF8String DBMSrvKnl_SQLRecord :: fieldAsChar
      ( const tsp00_Int4 nField,
        const char     cStringQuote) const
{
  if (nField <= this->nParamCount && nField > 0) {
    DBMSrvKnl_SQLField oField(&this->pParamInfo[nField-1], DBMSrvKnl_SQLField::typeString);
    oField.oInfo.sp1i_bufpos() = oField.oInfo.sp1i_bufpos() + ((this->nCurrentRecord-1)*this->nRecordSize);

    if (!this->IsLong(oField.oInfo.sp1i_data_type)) {
      if (this->pKernelSession->GetValue(nField, oField)) {
        if (cStringQuote != 0 && !oField.bNull && this->IsString(oField.oInfo.sp1i_data_type)) {
          if (oField.sValue.Length() == 0) {
            oField.sValue.Append(cStringQuote);
          } else {
            oField.sValue.Insert(0, cStringQuote);
          } // end if
          oField.sValue.Append(cStringQuote);
        } // end if
      } // end if
      return oField.sValue;
    } else {
      return "(long)";
    } // end if
  } // end if

  return "";
} // end DBMSrvKnl_SQLRecord :: fieldAsChar

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
Tools_DynamicUTF8String DBMSrvKnl_SQLRecord :: fieldAsChar
      ( const Tools_DynamicUTF8String & szField,
        const char                      cStringQuote) const
{
  return fieldAsChar(fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: fieldAsChar

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_DataType DBMSrvKnl_SQLRecord :: fieldType ( const tsp00_Int4 nField  ) const
{
  tsp00_DataType oDataType;

  if ((nField <= this->nParamCount) && (nField > 0) && (this->pParamInfo != NULL)) {
    oDataType = this->pParamInfo[nField-1].sp1i_data_type;
  } else {
    oDataType.becomes(dunknown);
  } // end if

  return oDataType;
} // end DBMSrvKnl_SQLRecord :: fieldType

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_DataType DBMSrvKnl_SQLRecord :: fieldType ( const Tools_DynamicUTF8String & szField  ) const
{
  return fieldType(fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: fieldType

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
bool DBMSrvKnl_SQLRecord :: isNull ( const tsp00_Int4 nField  ) const
{
  if (nField <= this->nParamCount && nField > 0) {
    DBMSrvKnl_SQLField oField(&this->pParamInfo[nField-1], DBMSrvKnl_SQLField::typeString);
    oField.oInfo.sp1i_bufpos() = oField.oInfo.sp1i_bufpos() + ((this->nCurrentRecord-1)*this->nRecordSize);

    if (this->pKernelSession->GetValue(nField, oField)) {
      return oField.bNull;
    } // end if
  } // end if

  return false;
} // end DBMSrvKnl_SQLRecord :: isNull

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
bool DBMSrvKnl_SQLRecord :: isNull ( const Tools_DynamicUTF8String &  szField ) const
{
  return this->isNull(this->fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: isNull

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
size_t DBMSrvKnl_SQLRecord :: size ( const tsp00_Int4 nField  ) const
{
  return this->pParamInfo[nField-1].sp1i_length;
} // end DBMSrvKnl_SQLRecord :: size

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
size_t DBMSrvKnl_SQLRecord :: size ( const Tools_DynamicUTF8String &  szField  ) const
{
  return this->size(this->fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: size

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
size_t DBMSrvKnl_SQLRecord :: maxSize ( const tsp00_Int4 nField  ) const
{
  return (this->size(nField) > this->fieldName(nField).Length() ? this->size(nField) : this->fieldName(nField).Length());
} // end DBMSrvKnl_SQLRecord :: maxSize

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
size_t DBMSrvKnl_SQLRecord :: maxSize ( const Tools_DynamicUTF8String &  szField  ) const
{
  return this->maxSize(this->fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: maxSize

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
bool DBMSrvKnl_SQLRecord :: exists ( const tsp00_Int4 nField  ) const
{
  return (nField <= this->nParamCount && nField > 0);
} // end DBMSrvKnl_SQLRecord :: exists

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
bool DBMSrvKnl_SQLRecord :: exists ( const Tools_DynamicUTF8String &  szField  ) const
{
  return this->exists(this->fieldNumber(szField));
} // end DBMSrvKnl_SQLRecord :: exists

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_Int4 DBMSrvKnl_SQLRecord :: fieldNumber ( const Tools_DynamicUTF8String &  szField  ) const
{
  tsp00_Int4  nField = 0;
  int         i = 1;

  while (nField == 0 && i <= this->nParamCount) {
    nField = (szField == this->fieldName(i)) ? i : 0;
    ++i;
  } // end if

  return nField;
} // end DBMSrvKnl_SQLRecord :: fieldNumber

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
Tools_DynamicUTF8String DBMSrvKnl_SQLRecord :: fieldName ( const tsp00_Int4 nField  ) const
{
  return this->pKernelSession->FieldName(nField);
} // end DBMSrvKnl_SQLRecord :: fieldName

/*-----------------------------------------------------------------------------*/
/*! @brief get data */
tsp00_Int4 DBMSrvKnl_SQLRecord :: fields ( )
{
  return this->nParamCount;
} // end DBMSrvKnl_SQLRecord :: fields

/*-----------------------------------------------------------------------------*/
/*! @brief check for string */
bool DBMSrvKnl_SQLRecord :: IsString
  ( tsp00_DataType aDataType ) const
{
  bool bRc = false;

  switch(aDataType) {
    // Had I hit all data types???
    case dcha:
    case dunicode:
    case dvarchara:
    case ddate:
    case dtime:
    case dtimestamp:
    case dstra:
      bRc = true;
  } // end switch

  return bRc;

} // DBMSrvKnl_SQLRecord :: IsString

/*-----------------------------------------------------------------------------*/
/*! @brief check for string */
bool DBMSrvKnl_SQLRecord :: IsLong
  ( tsp00_DataType aDataType ) const
{
  bool bRc = false;

  switch(aDataType) {
    // Had I hit all data types???
    case dstra:
    case dstrb:
      bRc = true;
  } // end switch

  return bRc;

} // DBMSrvKnl_SQLRecord :: IsLong

/*-----------------------------------------------------------------------------*/
/*! @brief get record */
bool DBMSrvKnl_SQLRecord :: getNextRecord
  ( bool            & bEnd,
    DBMSrvMsg_Error & oMsgList )
{
  return getRecord(bEnd, false, oMsgList);
} // end DBMSrvKnl_SQLRecord :: getNextRecord

/*-----------------------------------------------------------------------------*/
/*! @brief get record */
bool DBMSrvKnl_SQLRecord :: getFirstRecord
  ( bool            & bEnd,
    DBMSrvMsg_Error & oMsgList )
{
  return getRecord(bEnd, true, oMsgList);
} // end DBMSrvKnl_SQLRecord :: getFirstRecord

/*-----------------------------------------------------------------------------*/
/*! @brief get record */
bool DBMSrvKnl_SQLRecord :: getRecord
  ( bool            & bEnd,
    bool              bFirst,
    DBMSrvMsg_Error & oMsgList)
{
  bool bReturn = true;

  if ((this->nCurrentRecord < this->nMaxRecords) && !bFirst) {
    ++nCurrentRecord;
  } else {

    bReturn = this->pKernelSession->FetchRecordBlock(bEnd, bFirst, oMsgList);

    if (bReturn && !bEnd) {
      this->RefreshRecordData(DBMSrvKnl_SQLRecord::refreshAll);
    } else {
      this->RefreshRecordData(DBMSrvKnl_SQLRecord::resetAll);
    } // end if

  } // end if

  return bReturn;
} // end DBMSrvKnl_SQLRecord :: getRecord

/*-----------------------------------------------------------------------------*/
/*! @brief refresh record */
void DBMSrvKnl_SQLRecord :: RefreshRecordData
  ( RefreshMode oRefreshMode )
{
  switch (oRefreshMode) {
    case refreshAll:
      this->nCurrentRecord = 1;
      // no break, fall trough!!!
    case refreshWithoutRecordCounter:
      this->nParamCount    = this->pKernelSession->ParamCount();
      this->pParamInfo     = this->pKernelSession->ParamInfo();
      this->nMaxRecords    = this->pKernelSession->RecordCount();
      this->nRecordSize    = this->pKernelSession->RecordSize();
      break;
    case resetAll:
      this->nParamCount    = 0;
      this->pParamInfo     = NULL;
      this->nMaxRecords    = 0;
      this->nRecordSize    = 0;
      this->nCurrentRecord = 1;
      break;
  } // end switch
} // end DBMSrvKnl_SQLRecord :: RefreshRecordData
