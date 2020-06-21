/*!
 * \file    OMS_Exceptions.cpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Exceptions
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


#include "Oms/OMS_Exceptions.hpp"
#include "ggg00.h"
#include "ggg01.h"             // OMS errors
#include "Oms/OMS_Defines.h"   // mapped errors

/*----------------------------------------------------------------------*/
/* Implementation of class OmsDuplicateKey                              */
/*----------------------------------------------------------------------*/

OmsDuplicateKey::OmsDuplicateKey(long e, const OMS_ObjectId8& oid, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::DB_ERROR, e, "Duplicate Key", oid, pFileName, line) 
{
}

OmsDuplicateKey::OmsDuplicateKey(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::DB_ERROR, e, msg, oid, pFileName, line) 
{
}

OmsDuplicateKey::OmsDuplicateKey(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::USER_DEFINED, e_duplicate_hash_key, msg, oid, pFileName, line) 
{
}


/*----------------------------------------------------------------------*/
/* Implementation of OmsLockTimeout                                     */
/*----------------------------------------------------------------------*/

OmsLockTimeout::OmsLockTimeout(const OMS_ObjectId8& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e_OMS_lock_request_timeout, "Lock Request Timeout", oid, pFileName, line)
{
}

OmsLockTimeout::OmsLockTimeout(const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e_OMS_lock_request_timeout, msg, oid, pFileName, line)
{
}

OmsLockTimeout::OmsLockTimeout(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_OMS_lock_request_timeout, msg, oid, pFileName, line)
{
}

/*----------------------------------------------------------------------*/
/* Implementation of OmsOutOfDate                                       */
/*----------------------------------------------------------------------*/

OmsOutOfDate::OmsOutOfDate(const OMS_ObjectId8& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e_object_dirty, "Object out of date", oid, pFileName, line) 
{
}

OmsOutOfDate::OmsOutOfDate(const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e_object_dirty, "Object out of date", pFileName, line) 
{
}

OmsOutOfDate::OmsOutOfDate(const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e_object_dirty, msg, oid, pFileName, line) 
{
}

OmsOutOfDate::OmsOutOfDate(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_object_dirty, msg, oid, pFileName, line) 
{
}


/*----------------------------------------------------------------------*/
/* Implementation of class OmsObjectNotFound                            */
/*----------------------------------------------------------------------*/

OmsObjectNotFound::OmsObjectNotFound(long e, const OMS_ObjectId8& oid, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::DB_ERROR, e, "Object not found", oid, pFileName, line) 
{
}

OmsObjectNotFound::OmsObjectNotFound(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::DB_ERROR, e, msg, oid, pFileName, line) 
{
}

OmsObjectNotFound::OmsObjectNotFound(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) : 
  DbpError(DbpError::USER_DEFINED, e_object_not_found, msg, oid, pFileName, line) 
{
}

/*----------------------------------------------------------------------*/
/* Implementation of class OmsInvalidObject                             */
/*----------------------------------------------------------------------*/

OmsInvalidObject::OmsInvalidObject(DbpError &dbpError) : 
DbpError(dbpError) 
{
}

OmsInvalidObject::OmsInvalidObject( const OmsObjectId& oid, const char* msg, const char* pFileName, unsigned int line ) : 
DbpError(DbpError::USER_DEFINED, e_internal_obj_link_corruption, msg, oid, pFileName, line) 
{
}

/*----------------------------------------------------------------------*/
/* Implementation of class OmsKeyInUse                                  */
/*----------------------------------------------------------------------*/

OmsKeyInUse::OmsKeyInUse(long e, const OMS_ObjectId8& oid, const char* pFileName, unsigned int line) :
  OmsDuplicateKey(e, oid, "Key already in use", pFileName, line)
{
}

OmsKeyInUse::OmsKeyInUse(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) :
  OmsDuplicateKey(e, oid, msg, pFileName, line)
{
}

OmsKeyInUse::OmsKeyInUse(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) :
  OmsDuplicateKey(msg, oid, pFileName, line)
{
  this->m_errorNo = e_hash_key_in_use;
}


/*----------------------------------------------------------------------*/
/* Implementation of class OmsOverflow                                  */
/*----------------------------------------------------------------------*/

OmsOverflow::OmsOverflow(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e, msg, oid, pFileName, line) 
{
}

OmsOverflow::OmsOverflow(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_packed_out_of_range, msg, oid, pFileName, line) 
{
}


/*----------------------------------------------------------------------*/
/* Implementation of class OmsContainerError                            */
/*----------------------------------------------------------------------*/

OmsContainerError::OmsContainerError(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::DB_ERROR, e, msg, oid, pFileName, line) 
{
}

OmsContainerError::OmsContainerError(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_unknown_guid, msg, oid, pFileName, line) 
{
}


/*----------------------------------------------------------------------*/
/* Implementation of class OmsCancelled                                 */
/*----------------------------------------------------------------------*/

OmsCancelled::OmsCancelled(long e, const char* msg, const char* pFileName, unsigned int line, bool cancelledByProgram, bool provokedByTestkernel) :
  DbpError(DbpError::DB_ERROR, e, msg, pFileName, line, cancelledByProgram, provokedByTestkernel) 
{
}

OmsCancelled::OmsCancelled(const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_OMS_cancelled, msg, pFileName, line, true) 
{
}

/*----------------------------------------------------------------------*/
/* Implementation of class OmsProvokedError                             */
/*----------------------------------------------------------------------*/

OmsProvokedError::OmsProvokedError(long e, const char* msg, const char* pFileName, unsigned int line, bool provokedByTestkernel) :
  DbpError(DbpError::DB_ERROR, e, msg, pFileName, line, false, provokedByTestkernel) 
{
}

OmsProvokedError::OmsProvokedError(const char* msg, const char* pFileName, unsigned int line) :
  DbpError(DbpError::USER_DEFINED, e_OMS_cancelled, msg, pFileName, line) 
{
}


