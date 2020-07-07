/*!
 * \file    OMS_Exceptions.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Exceptions
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

#ifndef __OMS_EXCEPTIONS_HPP
#define __OMS_EXCEPTIONS_HPP

#include "Oms/OMS_DbpError.hpp"

/*---------------------------------------------------------------*/
/// Exception for duplicate key
/*!
** The OmsDuplicateKey class is derived from DbpError, and is thrown 
** by the OMS if the uniqueness of the key is violated when the new 
** operator of a class derived from OmsKeyedObject is executed.
**
** \see \ref example_OmsDuplicateKey
*/
class AFX_EXT_CLASS OmsDuplicateKey : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsDuplicateKey(long e, const OMS_ObjectId8& oid, const char* pFileName=0, unsigned int line=0); 
  OmsDuplicateKey(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName=0, unsigned int line=0); 
#endif
  /// Constructors for application layer
  OmsDuplicateKey(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for lock-timeout
/*!
** The OmsLockTimeout class is derived from DbpError and is thrown  
** by the OMS if the permissible wait time is exceeded when an 
** instance lock is requested.
**
** \see \ref example_OmsLockTimeout
*/
class AFX_EXT_CLASS OmsLockTimeout : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsLockTimeout(const OMS_ObjectId8& oid, const char* pFileName=0, unsigned int line=0);
  OmsLockTimeout(const OMS_ObjectId8& oid, const char* msg, const char* pFileName=0, unsigned int line=0);
#endif
  /// Constructors for application layer
  OmsLockTimeout(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for duplicate key or lock timeout
/*!
** The OmsKeyInUse class is derived from OmsDuplicateKey and
** is thrown by the OMS if the uniqueness of the key 
** is violated. In contrast to the duplicate key exception, this exception
** is also thrown, when the object with the key is created in a concurrent
** transaction and this transaction is not committed, yet.
*/
class AFX_EXT_CLASS OmsKeyInUse : public OmsDuplicateKey 
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsKeyInUse(long e, const OMS_ObjectId8& oid, const char* pFileName=0, unsigned int line=0); 
  OmsKeyInUse(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName=0, unsigned int line=0); 
#endif
  /// Constructors for application layer
  OmsKeyInUse(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for out-of-date
/*!
** The OmsOutOfDate class is derived from DbpError and is thrown 
** by the OMS if, when an instance lock is requested, it is determined 
** that the consistent view of the instance differs from the current 
** state in the liveCache.
**
** \see \ref example_OmsOutOfDate
*/
class AFX_EXT_CLASS OmsOutOfDate : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsOutOfDate(const char* pFileName=0, unsigned int line=0);
  OmsOutOfDate(const OMS_ObjectId8& oid, const char* pFileName=0, unsigned int line=0);
  OmsOutOfDate(const OMS_ObjectId8& oid, const char* msg, const char* pFileName=0, unsigned int line=0);
#endif
  /// Constructors for application layer
  OmsOutOfDate(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for object not found
/*!
** The OmsObjectNotFound class is derived from DbpError, and is thrown 
** by the OMS if the deref fails. There are several reasons for such an
** error (object does not exist, consistent view has been dropped,...)
** and therefore the origional error code is supplied to the constructor.
** So it is possible either to catch the OmsObjectNotFound exception or
** for a more detailed error handling to catch the error as DpbError and 
** check the error-code.
*/
class AFX_EXT_CLASS OmsObjectNotFound : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsObjectNotFound(long e, const OMS_ObjectId8& oid, const char* pFileName=0, unsigned int line=0); 
  OmsObjectNotFound(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName=0, unsigned int line=0); 
#endif
  /// Constructors for application layer
  OmsObjectNotFound(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};

/*---------------------------------------------------------------*/
/// Exception for invalid object
/*!
** The OmsInvalidObject class is derived from DbpError, and is thrown 
** by the OMS if the object cannot be accessed (e.g. by using omsDeRef() or checkOid() ) 
** due to internal corruption. 
** So it is possible either to catch the OmsObjectNotFound exception or
** for a more detailed error handling to catch the error as DpbError and 
** check the error-code.
*/
class AFX_EXT_CLASS OmsInvalidObject : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsInvalidObject(DbpError &dbpError); 
#endif
  /// Constructors for application layer
  OmsInvalidObject( const OmsObjectId& oid, const char* msg, const char* pFileName, unsigned int line ); 
};

/*---------------------------------------------------------------*/
/// Exception for overflow
/*!
** The OmsOverflow class is derived from DbpError, and is thrown 
** by the OMS if the conversion of DbpCurrency or DbpLongCurrency would
** lead to an overflow.
*/
class AFX_EXT_CLASS OmsOverflow : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsOverflow(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line); 
#endif
  /// Constructors for application layer
  OmsOverflow(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for errors when accessing container directory
/*!
** The OmsContainerError class is derived from DbpError, and is thrown 
** by the OMS if e.g a class is not registered, or a container does not
** exist,...
*/
class AFX_EXT_CLASS OmsContainerError : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsContainerError(long e, const OMS_ObjectId8& oid, const char* msg, const char* pFileName, unsigned int line); 
#endif
  /// Constructors for application layer
  OmsContainerError(const char* msg, const OmsObjectId& oid, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for cancellation
/*!
** The OmsCancelled class is derived from DbpError, and is thrown 
** by the OMS if the processing of the current action is stopped 
** because the cancel-flag in the kernel is set.
*/
class AFX_EXT_CLASS OmsCancelled : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsCancelled(long e, const char* msg, const char* pFileName, unsigned int line, bool cancelledByProgram, bool provokedByTestkernel); 
#endif
  /// Constructors for application layer
  OmsCancelled(const char* msg, const char* pFileName, unsigned int line); 
};


/*---------------------------------------------------------------*/
/// Exception for provoked error (testkernel)
/*!
** The OmsProvokedError class is derived from DbpError, and is thrown 
** by the OMS if the error is provoked by the special exception handling
** of the testkernel.
*/
class AFX_EXT_CLASS OmsProvokedError : public DbpError
{
public:
#ifdef OMS_IMPLEMENTATION
  /// Constructors for oms-layer
  OmsProvokedError(long e, const char* msg, const char* pFileName, unsigned int line, bool provokedByTestkernel); 
#endif
  /// Constructors for application layer
  OmsProvokedError(const char* msg, const char* pFileName, unsigned int line); 
};


#endif // __OMS_EXCEPTIONS_HPP
