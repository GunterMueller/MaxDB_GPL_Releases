/*!
 * \file    OMS_MassDerefIterators.hpp
 * \author  MarkusSi, Roterring
 * \brief   Iterators for mass deref
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#ifndef __OMS_MASSDEREFITERATORS_HPP
#define __OMS_MASSDEREFITERATORS_HPP

#include "Oms/OMS_AFX.h"
#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_GUID.hpp"
#include <string.h>


// Work-around for AIX-compiler problem.
// Because of dependency-problems the definition is
// repeated in OMS_MassDerefIterators.hpp
#if !defined(__MY_FILE__)
#if defined(AIX)
#define __MY_FILE__ (const char*)__FILE__
#else
#define __MY_FILE__ __FILE__
#endif
#endif


template<class T> class OmsOid;
class OmsAbstractObject;
class OmsHandle;
class OmsDeref;
class OmsDerefForUpd;



/*===========================================================================*/
/// Base class for Interface for mass deref 
/*!
** Base class for Interface for mass deref  
**
** \see \ref omsMassDeref_selfDefinedIterator
**
** \since 7.4.4  PTS 1120478
*/
class AFX_EXT_CLASS OmsIDerefIter 
{
public:
  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual long* omsGetErrAddr() const  { return NULL; }

    /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  virtual ClassID omsGetGuid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual const OmsAbstractObject** omsGetObjAddr() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of the current entry
  virtual const OmsObjectId& omsGetOid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Stores the result of the dereference-operation
  virtual void omsSetResult(const OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                            const OmsAbstractObject *pObj, long *pErrAddr, long errorNo) = 0;

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  virtual void operator++ () = 0;

  /*---------------------------------------------------------------------------*/
  /// Return false if the iterator has reached its end; otherwise true.
  virtual operator bool() const = 0;
};





/*===========================================================================*/
/// Wrapper class to dereference several oids
/*!
** Wrapper class to dereference several oids in a single call.
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are given to the iterator
** Dependend on whether variables to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the corresponding variables.
** If the process stopps on the first error, then all objects, which have been
** dereferenced, can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDeref_setIterator
**
** \since 7.4.4  PTS 1120478
*/
class  OmsDeref : public OmsIDerefIter
{
private:
  mutable OmsDeref         *m_start;
	mutable OmsDeref         *m_current;
  mutable OmsDeref         *m_next;
	OmsObjectId	              m_oid;
  ClassID                   m_guid;
	const OmsAbstractObject **m_ppObj;
  long                     *m_pError;

public:
  /*---------------------------------------------------------------------------*/
  /// Constructor template
  /*! 
  ** Constructor 
  **
  ** \param oid   [in] Oid which should be dereferenced
  ** \param ppObj [out] Address of the pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  */
  template<class T>
  OmsDeref(OmsOid<T> &oid, const T** ppObj)
	: m_oid(oid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
  , m_next(NULL)
	{
    m_start   = this;
    m_current = this;
    *m_ppObj  = NULL;
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor template
  /*! 
  ** Constructor 
  **
  ** \param oid   [in] Oid which should be dereferenced
  ** \param ppObj [out] Address of the pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Address where the error code should be stored. 
  */
  template<class T>
  OmsDeref(OmsOid<T> &oid, const T** ppObj, long *pError)
	: m_oid(oid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(pError)
  , m_next(NULL)
	{
    m_start   = this;
    m_current = this;
    *m_ppObj  = NULL;
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return m_current->m_pError;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  ClassID omsGetGuid() const {
    return m_current->m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  const OmsAbstractObject** omsGetObjAddr() const {
    return m_current->m_ppObj;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of current entry 
  const OmsObjectId& omsGetOid() const { 
    return m_current->m_oid;
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(const OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                    const OmsAbstractObject *pObj, long *pErrAddr, long errorNo) {
    const char* msg = "OmsDeref::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, oid, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    m_current = m_current->m_next;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return (m_current != NULL) ;
  }

  /*---------------------------------------------------------------------------*/
  /// Append new object to existing chain of omsDeref objects
  OmsDeref &operator<<(const OmsDeref &x) 
	{
    m_next      = const_cast<OmsDeref*>(&x);
    x.m_start   = m_start;
    x.m_current = m_current;
    return const_cast<OmsDeref&>(x);
	}

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line);
};





/*===========================================================================*/
/// Wrapper class to dereference an array of oids
/*!
** Wrapper class to dereference an array of oids in a single call
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are stored in the array. 
** Dependend on whether an array to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the array.
** If the process stopps on the first error, then all objects, which have been
** dereferenced, can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDeref_arrayIterator
**
** \since 7.4.4  PTS 1120478
*/
class OmsDerefArray : public OmsIDerefIter
{
private:
  int                  m_currIdx;
  int                  m_maxIdx;
  ClassID              m_guid;
  OmsObjectId         *m_pOid;
  long                *m_pError;
  const OmsAbstractObject  **m_ppObj;


public:
  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param pOid  [in] Array with pointers to the oids which should be dereferenced
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppoid[i].
  **        If an error occured then the pointer equals NULL. As the mass deref stopps 
  **        after the first error, all pointers of objects, which not have been considered
  **        yet, are equal NULL, too.
  ** \param cnt [in] Number of oids given to the mass deref.
  */
  template<class T>
  OmsDerefArray(OmsOid<T> *pOid, const T** ppObj, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_pOid(pOid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param pOid [in] Array with pointers to the oids which should be dereferenced
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Array with error codes. The error code written in pError[i]
  **        corresponds to the oid stored in ppOid[i]
  ** \param cnt [in] Number of oids given to the mass deref.
  */
  template<class T>
  OmsDerefArray(OmsOid<T> *pOid, const T** ppObj, long *pError, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_pOid(pOid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(pError)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return (m_pError != NULL ? &(m_pError[m_currIdx]) : NULL);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  ClassID omsGetGuid() const {
    return m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  const OmsAbstractObject** omsGetObjAddr() const {
    return &(m_ppObj[m_currIdx]);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of current entry 
  const OmsObjectId& omsGetOid() const { 
    return m_pOid[m_currIdx];
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(const OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                    const OmsAbstractObject *pObj, long *pErrAddr, long errorNo) {
    const char* msg = "OmsDerefArray::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, oid, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    ++m_currIdx;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return m_currIdx < m_maxIdx;
  }

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line);
};




//****************************************************************************
//*****                     Classes FOR UPDATE                          ******
//****************************************************************************


/*===========================================================================*/
/// Base class for mass deref for update
/*!
** Base class for mass deref for update  
**
** \see \ref omsMassDeref_selfDefinedIterator
**
** \since 7.4.4  PTS 1120478
*/
class AFX_EXT_CLASS OmsIDerefIterForUpd 
{
public:
  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual long* omsGetErrAddr() const { return NULL; }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  virtual ClassID omsGetGuid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual OmsAbstractObject** omsGetObjAddr() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of the current entry
  virtual const OmsObjectId& omsGetOid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Stores the result of the dereference-operation
  virtual void omsSetResult(OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                            OmsAbstractObject *pObj, long *pErrAddr, long errorNo) = 0;

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  virtual void operator++ () = 0;

  /*---------------------------------------------------------------------------*/
  /// Return false if the iterator has reached its end; otherwise true.
  virtual operator bool() const = 0;
};





/*===========================================================================*/
/// Wrapper class to dereference several oids for update
/*!
** Wrapper class to dereference several oids for update in a single call.
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are given to the iterator. 
** Dependend on whether variables to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the corresponding variables.
** If the process stopps on the first error, then all objects, which have been
** dereferenced, can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDeref_setIterator
**
** \since 7.4.4  PTS 1120478
*/
class  OmsDerefForUpd : public OmsIDerefIterForUpd
{
private:
  mutable OmsDerefForUpd   *m_start;
	mutable OmsDerefForUpd   *m_current;
  mutable OmsDerefForUpd   *m_next;
	OmsObjectId	              m_oid;
  ClassID                   m_guid;
	OmsAbstractObject       **m_ppObj;
  long                     *m_pError;

public:
  /*---------------------------------------------------------------------------*/
  /// Constructor template
  /*! 
  ** Constructor 
  **
  ** \param oid [in] Oid which should be dereferenced
  ** \param ppObj [out] Address of the pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  */
  template<class T>
  OmsDerefForUpd(OmsOid<T> &oid, T** ppObj)
	: m_oid(oid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
  , m_next(NULL)
	{
    m_start   = this;
    m_current = this;
    *m_ppObj  = NULL;
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor template
  /*! 
  ** Constructor 
  **
  ** \param oid [in] Oid which should be dereferenced
  ** \param ppObj [out] Address of the pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Address where the error code should be stored. 
  */
  template<class T>
  OmsDerefForUpd(OmsOid<T> &oid, T** ppObj, long *pError)
	: m_oid(oid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(pError)
  , m_next(NULL)
	{
    m_start   = this;
    m_current = this;
    *m_ppObj  = NULL;
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return m_current->m_pError;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  ClassID omsGetGuid() const {
    return m_current->m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  OmsAbstractObject** omsGetObjAddr() const {
    return m_current->m_ppObj;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of current entry 
  const OmsObjectId& omsGetOid() const { 
    return m_current->m_oid;
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                    OmsAbstractObject *pObj, long *pErrAddr, long errorNo) {
    const char* msg = "OmsDerefForUpd::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, oid, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    m_current = m_current->m_next;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return (m_current != NULL) ;
  }

  /*---------------------------------------------------------------------------*/
  /// Append new object to existing chain of omsDeref objects
  OmsDerefForUpd &operator<<(const OmsDerefForUpd &x) 
	{
    m_next      = const_cast<OmsDerefForUpd*>(&x);
    x.m_start   = m_start;
    x.m_current = m_current;
    return const_cast<OmsDerefForUpd&>(x);
	}

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line);
};





/*===========================================================================*/
/// Wrapper class to dereference an array of oids
/*!
** Wrapper class to dereference an array of oids in a single call
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are stored in the array. 
** Dependend on whether an array to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the array.
** If the process stopps on the first error, then all objects, which have been
** dereferenced, can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDeref_arrayIterator
**
** \since 7.4.4  PTS 1120478
*/
class OmsDerefArrayForUpd : public OmsIDerefIterForUpd
{
private:
  int                  m_currIdx;
  int                  m_maxIdx;
  ClassID              m_guid;
  OmsObjectId         *m_pOid;
  OmsAbstractObject  **m_ppObj;
  long                *m_pError;

public:
  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param pOid [in] Array with pointers to the oids which should be dereferenced
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppoid[i].
  **        If an error occured then the pointer equals NULL. As the mass deref stopps 
  **        after the first error, all pointers of objects, which not have been considered
  **        yet, are equal NULL, too.
  ** \param cnt [in] Number of oids given to the mass deref.
  */
  template<class T>
  OmsDerefArrayForUpd(OmsOid<T> *pOid, T** ppObj, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_pOid(pOid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param pOid [in] Array with pointers to the oids which should be dereferenced
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the oid stored in ppOid[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Array with error codes. The error code written in pError[i]
  **        corresponds to the oid stored in ppOid[i]
  ** \param cnt [in] Number of oids given to the mass deref.
  */
  template<class T>
  OmsDerefArrayForUpd(OmsOid<T> *pOid, T** ppObj, long *pError, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_pOid(pOid)
  , m_guid(T::omsGetGuid())
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(pError)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return (m_pError != NULL ? &(m_pError[m_currIdx]) : NULL);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  ClassID omsGetGuid() const {
    return m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  OmsAbstractObject** omsGetObjAddr() const {
    return &(m_ppObj[m_currIdx]);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of current entry 
  const OmsObjectId& omsGetOid() const { 
    return m_pOid[m_currIdx];
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(OmsAbstractObject **ppAddr, OmsObjectId &oid, 
                    OmsAbstractObject *pObj, long *pErrAddr, long errorNo) {
    const char* msg = "OmsDerefArrayForUpd::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, oid, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    ++m_currIdx;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return m_currIdx < m_maxIdx;
  }

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, OmsObjectId &oid, const char* pFile, unsigned int line);
};





/*===========================================================================*/
/// Base class for Interface for mass deref via key
/*!
** Base class for Interface for mass deref via key
**
** \see \ref omsMassDerefKey_selfDefinedIterator
**
** \since 7.4.4  PTS 1122194
*/
class AFX_EXT_CLASS OmsIDerefKeyIter 
{
public:
  /*---------------------------------------------------------------------------*/
  /// Returns the container number of the current entry 
  virtual const OmsContainerNo omsGetContainerNo() const = 0;

  /*---------------------------------------------------------------------------*/
  // Returns the address into which the dereferenced address should be stored 
  virtual long* omsGetErrAddr() const { return NULL; }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  virtual ClassID omsGetGuid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of the current entry
  virtual const unsigned char* omsGetKey() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual const OmsAbstractObject** omsGetObjAddr() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the schema handle of the current entry 
  virtual const OmsSchemaHandle omsGetSchema() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Stores the result of the dereference-operation
  virtual void omsSetResult(const OmsAbstractObject **ppAddr, const unsigned char *pKey, 
                            size_t keyLen, const OmsAbstractObject *pObj, long *pErrAddr, 
                            long errorNo) = 0;

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  virtual void operator++ () = 0;

  /*---------------------------------------------------------------------------*/
  /// Return false if the iterator has reached its end; otherwise true.
  virtual operator bool() const = 0;
};



/*===========================================================================*/
/// Wrapper class to dereference an array of keys
/*!
** Wrapper class to dereference an array of keys in a single call
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are stored in the array. 
** Dependend on whether an array to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the array.
** In the process stopps on the first error, then objects which are dereferenced
** can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDerefKey_arrayIterator
**
** \since 7.4.4  PTS 1122194
*/
class OmsDerefKeyArray : public OmsIDerefKeyIter
{
private:
  int                  m_currIdx;
  int                  m_maxIdx;
  ClassID              m_guid;
  OmsSchemaHandle      m_schema;
  OmsContainerNo       m_containerNo;
  unsigned char      **m_ppKey;
  long                *m_pError;
  const OmsAbstractObject  **m_ppObj;


public:
  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param ppKey [in] Array with pointers to the keys which should be dereferenced
  ** \param schema [in] Schema in which the keys should be dereferenced (the schema is
  **        the same for all keys in the array)
  ** \param containerNo [in] Container in which the keys should be dereferenced .
  **        (the container number the same for all keys in the array)
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the key stored in ppKey[i].
  **        If an error occured then the pointer equals NULL. As the mass deref stopps 
  **        after the first error, all pointers of objects, which not have been considered
  **        yet, are equal NULL, too.
  ** \param cnt [in] Number of keys given to the mass deref.
  */
  template<class T>
  OmsDerefKeyArray(unsigned char** ppKey, OmsSchemaHandle schema, 
                   OmsContainerNo containerNo, const T** ppObj, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_ppKey(ppKey)
  , m_guid(T::omsGetGuid())
  , m_schema(schema)
  , m_containerNo(containerNo)
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param ppKey [in] Array with pointers to the keys which should be dereferenced
  ** \param schema [in] Schema in which the keys should be dereferenced (the schema is
  **        the same for all keys in the array)
  ** \param containerNo [in] Container in which the keys should be dereferenced .
  **        (the container number the same for all keys in the array)
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the key stored in ppKey[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Array with error codes. The error code written in pError[i]
  **        corresponds to the key stored in ppKey[i]
  ** \param cnt [in] Number of keys given to the mass deref.
  */
  template<class T>
  OmsDerefKeyArray(unsigned char** ppKey, OmsSchemaHandle schema, 
                   OmsContainerNo containerNo, const T** ppObj, long *pError, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_ppKey(ppKey)
  , m_guid(T::omsGetGuid())
  , m_schema(schema)
  , m_containerNo(containerNo)
  , m_ppObj(reinterpret_cast<const OmsAbstractObject**>(ppObj))
  , m_pError(pError)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the container number of current entry (constant for all objects)
  const OmsContainerNo omsGetContainerNo() const { 
    return m_containerNo;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return (m_pError != NULL ? &(m_pError[m_currIdx]) : NULL);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  const OmsAbstractObject** omsGetObjAddr() const {
    return &(m_ppObj[m_currIdx]);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry (constant for all objects)
  ClassID omsGetGuid() const {
    return m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the key of the current entry 
  const unsigned char* omsGetKey() const {
    return m_ppKey[m_currIdx];
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the schema of current entry (constant for all objects)
  const OmsSchemaHandle omsGetSchema() const { 
    return m_schema;
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(const OmsAbstractObject **ppAddr, const unsigned char* pKey,
                    size_t keyLen, const OmsAbstractObject *pObj, long *pErrAddr, 
                    long errorNo) {
    const char* msg = "OmsDerefKeyArray::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, pKey, keyLen, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    ++m_currIdx;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return m_currIdx < m_maxIdx;
  }

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, const unsigned char* pKey, size_t keyLen, const char* pFile, unsigned int line);
};



/*===========================================================================*/
/// Base class for Interface for mass deref via key for update
/*!
** Base class for Interface for mass deref via key for update
**
** \see \ref omsMassDerefKey_selfDefinedIterator
**
** \since 7.4.4  PTS 1122194
*/
class AFX_EXT_CLASS OmsIDerefKeyIterForUpd 
{
public:
  /*---------------------------------------------------------------------------*/
  /// Returns the container number of the current entry 
  virtual const OmsContainerNo omsGetContainerNo() const = 0;

  /*---------------------------------------------------------------------------*/
  // Returns the address into which the dereferenced address should be stored 
  virtual long* omsGetErrAddr() const { return NULL; }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry 
  virtual ClassID omsGetGuid() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the oid of the current entry
  virtual const unsigned char* omsGetKey() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  virtual OmsAbstractObject** omsGetObjAddr() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Returns the schema handle of the current entry 
  virtual const OmsSchemaHandle omsGetSchema() const = 0;

  /*---------------------------------------------------------------------------*/
  /// Stores the result of the dereference-operation
  virtual void omsSetResult(OmsAbstractObject **ppAddr, const unsigned char *pKey, 
                            size_t keyLen, OmsAbstractObject *pObj, long *pErrAddr, 
                            long errorNo) = 0;

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  virtual void operator++ () = 0;

  /*---------------------------------------------------------------------------*/
  /// Return false if the iterator has reached its end; otherwise true.
  virtual operator bool() const = 0;
};


/*===========================================================================*/
/// Wrapper class to dereference an array of keys for update
/*!
** Wrapper class to dereference an array of keys for update
**
** \attention The objects are not necessarily dereferenced in the order in which
** they are stored in the array. 
** Dependend on whether an array to store the errors is given or not, either the
** processing stopps on the first error, or all keys are dereferenced and the
** corresponding error codes are stored in the array.
** If the process stopps on the first error, then all objects, which have been
** dereferenced, can be recognized by pointers not equal NULL.
**
** \see \ref omsMassDerefKey_arrayIterator
**
** \since 7.4.4  PTS 1122194
*/
class OmsDerefKeyArrayForUpd : public OmsIDerefKeyIterForUpd
{
private:
  int                  m_currIdx;
  int                  m_maxIdx;
  ClassID              m_guid;
  OmsSchemaHandle      m_schema;
  OmsContainerNo       m_containerNo;
  unsigned char      **m_ppKey;
  long                *m_pError;
  OmsAbstractObject  **m_ppObj;


public:
  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param ppKey [in] Array with pointers to the keys which should be dereferenced
  ** \param schema [in] Schema in which the keys should be dereferenced (the schema is
  **        the same for all keys in the array)
  ** \param containerNo [in] Container in which the keys should be dereferenced .
  **        (the container number the same for all keys in the array)
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the key stored in ppKey[i].
  **        If an error occured then the pointer equals NULL. As the mass deref stopps 
  **        after the first error, all pointers of objects, which not have been considered
  **        yet, are equal NULL, too.
  ** \param cnt [in] Number of keys given to the mass deref.
  */
  template<class T>
  OmsDerefKeyArrayForUpd(unsigned char** ppKey, OmsSchemaHandle schema, 
                         OmsContainerNo containerNo, T** ppObj, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_ppKey(ppKey)
  , m_guid(T::omsGetGuid())
  , m_schema(schema)
  , m_containerNo(containerNo)
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(NULL)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Constructor-Template 
  /*! 
  ** Constructor 
  **
  ** \param ppKey [in] Array with pointers to the keys which should be dereferenced
  ** \param schema [in] Schema in which the keys should be dereferenced (the schema is
  **        the same for all keys in the array)
  ** \param containerNo [in] Container in which the keys should be dereferenced .
  **        (the container number the same for all keys in the array)
  ** \param ppObj [out] Array with pointers to the dereferenced objects in the oms-cache.
  **        The pointer, stored in ppObj[i] corresponds to the key stored in ppKey[i].
  **        If an error occured then the pointer equals NULL. 
  ** \param pError [in] Array with error codes. The error code written in pError[i]
  **        corresponds to the key stored in ppKey[i]
  ** \param cnt [in] Number of keys given to the mass deref.
  */
  template<class T>
  OmsDerefKeyArrayForUpd(unsigned char** ppKey, OmsSchemaHandle schema, 
                         OmsContainerNo containerNo, T** ppObj, long *pError, int cnt)
	: m_currIdx(0)
  , m_maxIdx(cnt)
  , m_ppKey(ppKey)
  , m_guid(T::omsGetGuid())
  , m_schema(schema)
  , m_containerNo(containerNo)
  , m_ppObj(reinterpret_cast<OmsAbstractObject**>(ppObj))
  , m_pError(pError)
	{
    memset(m_ppObj, 0, m_maxIdx * sizeof(void*));
	}

  /*---------------------------------------------------------------------------*/
  /// Returns the container number of current entry (constant for all objects)
  const OmsContainerNo omsGetContainerNo() const { 
    return m_containerNo;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  long* omsGetErrAddr() const {
    return (m_pError != NULL ? &(m_pError[m_currIdx]) : NULL);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the address into which the dereferenced address should be stored 
  OmsAbstractObject** omsGetObjAddr() const {
    return &(m_ppObj[m_currIdx]);
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the class guid of the current entry (constant for all objects)
  ClassID omsGetGuid() const {
    return m_guid;
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the key of the current entry 
  const unsigned char* omsGetKey() const {
    return m_ppKey[m_currIdx];
  }

  /*---------------------------------------------------------------------------*/
  /// Returns the schema of current entry (constant for all objects)
  const OmsSchemaHandle omsGetSchema() const { 
    return m_schema;
  }

  /*---------------------------------------------------------------------------*/
  /// Stores the dereferenced address or raises an error if one has occured
  void omsSetResult(OmsAbstractObject **ppAddr, const unsigned char* pKey,
                    size_t keyLen, OmsAbstractObject *pObj, long *pErrAddr, 
                    long errorNo) {
    const char* msg = "OmsDerefKeyArray::omsSetResult ";
 
    if (pErrAddr == NULL) {
      if (errorNo != 0) {
        throwDbpError(errorNo, msg, pKey, keyLen, __MY_FILE__, __LINE__);
      }
      else {
        *ppAddr = pObj;
      }
    }
    else {
      *pErrAddr = errorNo;
      *ppAddr   = pObj;
    }
  }

  /*---------------------------------------------------------------------------*/
  /// Increments to the next oid
  void operator++ (){
    ++m_currIdx;
  }

  /*---------------------------------------------------------------------------*/
  /// Return true if omsGet() would return a valid oid; otherwise false.
  operator bool () const {
    return m_currIdx < m_maxIdx;
  }

private:
  // Method is located in the cpp-file because of compilation problems
  AFX_EXT_CLASS
  void throwDbpError(long errorNo, const char* msg, const unsigned char* pKey, size_t keyLen, const char* pFile, unsigned int line);
};



#endif // __OMS_MASSDEREFITERATORS_HPP
