/*!
 * \file    SQL_Utilities.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Utilities
 */

/*

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



*/


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_Defines.h"
#include "geo00.h"

#ifndef SQL_UTILITIES_HPP
#include "CppSQL/SQL_Utilities.hpp"
#endif

#include "hsp81.h"

#if defined (EXT_CLIENT_SQL)
#include "Oms/OMS_DbpError.hpp"
#include "hsp77.h"
#include "Oms/OMS_Oid.hpp"
#endif


/************************************************************************/
/* Implementation of class DbpError for external SQL class              */
/************************************************************************/

#if defined (EXT_CLIENT_SQL)

static DbpErrorCallbackInterface * dbpErrorCallback = NULL;

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* file, unsigned int line)
  : m_errorKind(ErrorKind)
  , m_isWyde   (false)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
{
  m_oid = OmsObjectId();
  
  if (file == NULL){
    m_errorText[0] = 0;
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s:%d", file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
}

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
{
  m_oid = OmsObjectId();

  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
}

/*----------------------------------------------------------------------*/
DbpError::DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const DbpTypeUnicode* errtext, const char* file, unsigned int line) 
  : m_errorKind(ErrorKind)
  , m_isWyde   (true)
  , m_errorNo  (ErrorNo)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
{
  m_oid = OmsObjectId();

  //if (file == NULL){
    sp81UCS2strcpy ((tsp81_UCS2Char*)(m_errorText), (tsp81_UCS2Char*)errtext);
  //}
  //else {
  //  sp77sprintfUnicode(m_errorText, sizeof(errorText), "%=s (%s:%d)", ??what_kind_of_encoding??, errtext, file, line);
  //}

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
}

/*----------------------------------------------------------------------*/

DbpError::DbpError (DbpErrorKinds      ErrorKind, 
                    long               ErrorNo, 
                    const char        *errtext, 
                    const OmsObjectId &oid, 
                    const char        *file, 
                    unsigned int       line)
  : m_errorKind(ErrorKind)
  , m_isWyde (false)
  , m_errorNo (ErrorNo)
  , m_oid(oid)
  , m_cancelledByProgram   (false)
  , m_provokedByTestKernel (false)
{
  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
}

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
                    , m_oid(oid)
                    , m_cancelledByProgram   (false)
                    , m_provokedByTestKernel (false)
{
  if (file == NULL){
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s", errtext); 
  }
  else {
    sp77sprintf(m_errorText, sizeof(m_errorText),"%s (%s:%d)", errtext, file, line); 
  }

  if (dbpErrorCallback) 
    dbpErrorCallback->dbpCheckError(*this);
}

/*----------------------------------------------------------------------*/

//DbpError::DbpError( DbpErrorKinds ErrorKind, long ErrorNo,
//                    const DbpTypeUnicode * errtext,
//                    const OmsObjectId & oid) {
//  m_errorKind = ErrorKind;
//  m_errorNo   = ErrorNo;
//  m_oid       = reinterpret_cast<const OmsTypeOid&>(oid);
//  sp81UCS2strcpy( (tsp81_UCS2Char*)(m_errorText),
//                  (tsp81_UCS2Char*)errtext );
//  if (dbpErrorCallback) {
//    dbpErrorCallback->dbpCheckError(*this);
//  }
//}

/*----------------------------------------------------------------------*/

DbpError::DbpError(long ErrorNo) {
  m_errorKind    = USER_DEFINED;
  m_errorNo      = ErrorNo;
  m_errorText[0] = 0;
  m_cancelledByProgram   = false;
  m_provokedByTestKernel = false;
  if (dbpErrorCallback) {
    dbpErrorCallback->dbpCheckError(*this);
  }
}

/*----------------------------------------------------------------------*/

DbpError::DbpError( long ErrorNo, const char * errtext) {
  m_errorKind = USER_DEFINED;
  m_errorNo   = ErrorNo;
  m_cancelledByProgram   = false;
  m_provokedByTestKernel = false;
  strcpy( m_errorText, errtext );
  if (dbpErrorCallback) {
    dbpErrorCallback->dbpCheckError(*this);
  }
}

/*----------------------------------------------------------------------*/

DbpError::DbpError( long ErrorNo, const DbpTypeUnicode * errtext) {
  m_errorKind = USER_DEFINED_UNICODE;
  m_errorNo   = ErrorNo;
  m_cancelledByProgram   = false;
  m_provokedByTestKernel = false;
  sp81UCS2strcpy( (tsp81_UCS2Char*)(m_errorText),
                  (tsp81_UCS2Char*)errtext );
  if (dbpErrorCallback) {
    dbpErrorCallback->dbpCheckError(*this);
  }
}

/*----------------------------------------------------------------------*/

void DbpError::dbpSetCallback(DbpErrorCallbackInterface * cb) {
  dbpErrorCallback = cb;
}

/*----------------------------------------------------------------------*/

DbpErrorCallbackInterface * DbpError::dbpGetCallback() {
  return dbpErrorCallback;
}

/*----------------------------------------------------------------------*/

long DbpError::dbpError() const {
  return m_errorNo;
}

/*----------------------------------------------------------------------*/

short DbpError::dbpDBError() const {
  return (m_errorKind == DB_ERROR) ? (/*ia64*/short)m_errorNo : 0;
}

/*----------------------------------------------------------------------*/

short DbpError::dbpRteError() const {
  return 0;
}

/*----------------------------------------------------------------------*/

long DbpError::dbpHresultError() const {
  return (m_errorKind == HRESULT_ERROR) ? m_errorNo : 0;
}

/*----------------------------------------------------------------------*/

const char * DbpError::dbpErrorText() const {
  return &m_errorText[0];
}

/*----------------------------------------------------------------------*/

const DbpTypeUnicode * DbpError::dbpErrorTextUnicode() const {
  return (DbpTypeUnicode*)&m_errorText[0];
}

#endif


/************************************************************************/
/* Implementation of several helper functions                           */
/************************************************************************/

#if !defined(EXT_CLIENT_SQL)
extern "C" void  sqlabort() { int x=0; x=1/x; }
#endif

/*----------------------------------------------------------------------*/

unsigned int dbpStrLen( const DbpTypeUnicode *s ) {
  return sp81UCS2strlen((const tsp81_UCS2Char*)s);
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrCat( DbpTypeUnicode * dest, 
                            const DbpTypeUnicode * src) {
  return (DbpTypeUnicode *)sp81UCS2strcat( (tsp81_UCS2Char*)dest,
                                           (tsp81_UCS2Char*)src );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrnCat( DbpTypeUnicode * dest,
                             const DbpTypeUnicode * src,
                             unsigned int maxlen ) {
  return (DbpTypeUnicode *)sp81UCS2strncat( (tsp81_UCS2Char*)dest,
                                            (tsp81_UCS2Char*)src, maxlen);
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrChr( const DbpTypeUnicode * s,
                            const DbpTypeUnicode c ) {
  tsp81_UCS2Char UCS2char;
  UCS2char.s = c;
  return (DbpTypeUnicode *)sp81UCS2strchr( (tsp81_UCS2Char*)s, UCS2char );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrnChr( const DbpTypeUnicode * s,
                             const DbpTypeUnicode c,
                             unsigned int maxlen ) {
  tsp81_UCS2Char UCS2char;
  UCS2char.s = c;
  return (DbpTypeUnicode *)sp81UCS2strnchr( (tsp81_UCS2Char*)s,
                                            UCS2char, maxlen );
}

/*----------------------------------------------------------------------*/

/* PTS 1111387 
   dbpStrCmp doesn't return pointer to string DbpTypeUnicode */

int dbpStrCmp( DbpTypeUnicode * dest, const DbpTypeUnicode *src) {
  return /*(DbpTypeUnicode *)*/ sp81UCS2strcmp( (tsp81_UCS2Char*)dest,
                                                (tsp81_UCS2Char*)src );
}

/*----------------------------------------------------------------------*/

/* PTS 1111387 
   dbpStrnCmp doesn't return pointer to string DbpTypeUnicode * */

int dbpStrnCmp( DbpTypeUnicode * dest, const DbpTypeUnicode * src,
                unsigned int maxlen ) {
  return /*(DbpTypeUnicode *)*/ sp81UCS2strncmp( (tsp81_UCS2Char*)dest,
                                                 (tsp81_UCS2Char*)src,
                                                 maxlen );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrCpy( DbpTypeUnicode * dest, 
                            const DbpTypeUnicode * src) {
  return (DbpTypeUnicode *)sp81UCS2strcpy( (tsp81_UCS2Char*)dest,
                                           (tsp81_UCS2Char*)src );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrnCpy( DbpTypeUnicode * dest,
                             const DbpTypeUnicode * src,
                             unsigned int maxlen ) {
  return (DbpTypeUnicode *) sp81UCS2strncpy( (tsp81_UCS2Char*)dest,
                                             (tsp81_UCS2Char*)src,
                                             maxlen );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrrChr( const DbpTypeUnicode * s,
                             const DbpTypeUnicode c ) {
  tsp81_UCS2Char UCS2char;
  UCS2char.s = c;
  return (DbpTypeUnicode *)sp81UCS2strrchr( (tsp81_UCS2Char*)s,
                                            UCS2char );
}

/*----------------------------------------------------------------------*/

DbpTypeUnicode * dbpStrnrChr( const DbpTypeUnicode * s,
                              const DbpTypeUnicode c,
                              unsigned int maxlen ) {
  tsp81_UCS2Char UCS2char;
  UCS2char.s = c;
  return (DbpTypeUnicode *)sp81UCS2strnrchr( (tsp81_UCS2Char*)s,
                                             UCS2char, maxlen );
}

