/****************************************************************************

module      : OMS_DbpError.cpp

-------------------------------------------------------------------------

responsible : ThomasA

special area: liveCache Error Class
description : Defines Functions of an error thrown by OMS and SQL class

last changed: 2000-01-10  9:34

-------------------------------------------------------------------------

copyright:    Copyright (c) 1999-2005 SAP AG




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




*****************************************************************************/


/*===========================================================================*
*  INCLUDES                                                                 *
*===========================================================================*/


#include "Oms/OMS_DbpError.hpp"
#include "Oms/OMS_Oid.hpp"
#include "hsp81.h"
#include "geo573.h"
#include "hsp77.h"

#if !defined(KERNEL)
#include "Oms/OMS_Globals.hpp" // nocheck
#endif

/*===========================================================================*
*  METHODS                                                                  *
*===========================================================================*/

static DbpErrorCallbackInterface *dbpErrorCallback = NULL;

/*----------------------------------------------------------------------*/

void dbpErrorCallbackFunc()
{
  // this causes call to callback in constructor
  DbpError tmp(-28000, "bad_alloc");
}

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* file, unsigned int line)
  : m_errorKind(ErrorKind)
  , m_isWyde   (false)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid()
{
  if (file == NULL){
    m_errorText[0] = 0;
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s:%d", file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds      ErrorKind, 
                    long               ErrorNo, 
                    const char        *errtext, 
                    const char        *file, 
                    unsigned int       line,
                    bool               cancelledByProgram, 
                    bool               provokedByTestKernel)     
  : m_errorKind(ErrorKind)
  , m_isWyde   (false)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (cancelledByProgram)
  , m_provokedByTestKernel (provokedByTestKernel)
  , m_oid()
{
  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds      ErrorKind, 
                    long               ErrorNo, 
                    const char        *errtext, 
                    const OMS_ObjectId8 &oid, 
                    const char        *file, 
                    unsigned int       line)
  : m_errorKind(ErrorKind)
  , m_isWyde (false)
  , m_errorNo (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid(oid)
{
  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (
  DbpErrorKinds      ErrorKind, 
  long               ErrorNo, 
  const char        *errtext, 
  const OmsObjectId &oid, 
  const char        *file, 
  unsigned int       line)
  : m_errorKind(ErrorKind)
  , m_isWyde (false)
  , m_errorNo (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid(oid)
{
  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const DbpTypeUnicode* errtext, const char* file, unsigned int line) 
  : m_errorKind(ErrorKind)
  , m_isWyde   (true)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid()
{
  //if (file == NULL){
    sp81UCS2strcpy ((tsp81_UCS2Char*)(m_errorText), (tsp81_UCS2Char*)errtext);
  //}
  //else {
  //  sp77sprintfUnicode(m_errorText, sizeof(errorText), "%=s (%s:%d)", ??what_kind_of_encoding??, errtext, file, line);
  //}

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

//DbpError::DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const DbpTypeUnicode* errtext, const OmsObjectId& oid)
//  : m_errorKind(ErrorKind)
//  , m_isWyde (true)
//  , m_errorNo (ErrorNo)
//  , m_oid(reinterpret_cast<const OmsTypeOid&>(oid))
//{
////  if (file == NULL){
//      sp81UCS2strcpy ((tsp81_UCS2Char*)(m_errorText), (tsp81_UCS2Char*)errtext);
////  }
////  else {
////    sp77sprintfUnicode(sp77encodingUCS2, m_errorText, sizeof(errorText), "%=s (%s:%d)", ??what_kind_of_encoding??, errtext, file, line);
////  }
//
//  if (dbpErrorCallback) 
//    dbpErrorCallback->dbpCheckError(*this);
//};

/*----------------------------------------------------------------------*/

DbpError::DbpError (long ErrorNo)
  : m_errorKind(USER_DEFINED)
  , m_isWyde   (false)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid()
{
  m_errorText[0] = 0;
  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (long ErrorNo, const char* errtext) 
  : m_errorKind(USER_DEFINED)
  , m_isWyde   (false)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid()
{
  strcpy (m_errorText, errtext);
  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

DbpError::DbpError (long ErrorNo, const DbpTypeUnicode* errtext)
  : m_errorKind(USER_DEFINED_UNICODE)
  , m_isWyde   (true)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
  , m_oid()
{
  sp81UCS2strcpy ((tsp81_UCS2Char*)(m_errorText), (tsp81_UCS2Char*)errtext);
  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
};

/*----------------------------------------------------------------------*/

void DbpError::dbpSetCallback(DbpErrorCallbackInterface *cb)
{
  if (cb) {
    dbpErrorCallback = cb;
    geo573_BadAllocHandler::setCallback(dbpErrorCallbackFunc);
  } else {
    dbpErrorCallback = NULL;
    geo573_BadAllocHandler::setCallback(NULL);
  }
#if !defined(KERNEL)
  if (cb)
    OMS_Globals::GetKernelInterface()->SetErrorCallback(cb);
  else
    OMS_Globals::GetKernelInterface()->SetErrorCallback(NULL);
#endif
}

/*----------------------------------------------------------------------*/

DbpErrorCallbackInterface *DbpError::dbpGetCallback()
{
  return dbpErrorCallback;
}

/*----------------------------------------------------------------------*/

long DbpError::dbpError () const
{ 
  return m_errorNo; 
};

/*----------------------------------------------------------------------*/

short DbpError::dbpDBError () const
{
  return (m_errorKind == DB_ERROR) ? (short)m_errorNo : 0; 
};

/*----------------------------------------------------------------------*/

long  DbpError::dbpHresultError() const
{ 
  return (m_errorKind == HRESULT_ERROR) ? m_errorNo : 0; 
};

/*----------------------------------------------------------------------*/

const char* DbpError::dbpErrorText() const 
{
  return (m_isWyde ? NULL : &m_errorText[0]); 
};

/*----------------------------------------------------------------------*/

const OmsTypeWyde* DbpError::dbpErrorTextWyde() const  
{ 
  if (!m_isWyde)
  {
    int length = (int) strlen(&m_errorText[0]);
    if (length >= sizeof(m_errorText) / 2)
    {
      length = sizeof(m_errorText) / 2 - 1; /* PTS 1132823, PG */
      m_errorText[length] = 0;              /* PTS 1132823, PG */
    }
    int idx = length;
    OmsTypeWyde* pWyde = REINTERPRET_CAST(OmsTypeWyde*, &m_errorText[0]);
    while (idx >= 0)
    {
      pWyde[idx] = (unsigned char) m_errorText[idx];
      --idx;
    }
    m_isWyde = true;
  }
  return REINTERPRET_CAST(OmsTypeWyde*, &m_errorText[0]); 
};

/*----------------------------------------------------------------------*/

const DbpTypeUnicode* DbpError::dbpErrorTextUnicode() const
{
  return (DbpTypeUnicode*)&m_errorText[0]; 
};

/*----------------------------------------------------------------------*/

const OmsObjectId& DbpError::dbpErrorOid() const 
{ 
  return m_oid;
};