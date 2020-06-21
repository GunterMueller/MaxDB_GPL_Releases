/*!
 * \file    OMS_DbpError.hpp
 * \author  IvanS, MarkusS, PeterG
 * \brief   Exception classes
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

#ifndef __OMS_DBPERROR_HPP
#define __OMS_DBPERROR_HPP

#include "Oms/OMS_AFX.h"
#include "livecachetypes.h" // nocheck
#include "Oms/OMS_Oid.hpp"

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

class  OmsHandle;
class  DbpErrorCallbackInterface;

/*!
* \brief Base Class for all errors thrown in OMS
*
* \note All classes thrown in a db-procedure should be derived from 
*  DbpErrorBase.
*/
class AFX_EXT_CLASS DbpErrorBase
{
public :
  DbpErrorBase() {}
};



/*--------------------------------------------------------------------------------*/
/// Exception class for SQL-Class and OMS
/*!
** The DbpError class provides methods that allow errors to be analyzed 
** when they are 'caught'.
*
** \see \ref example_OmsDbpError 
*/
class AFX_EXT_CLASS DbpError : public DbpErrorBase
{
public:
  enum DbpErrorKinds { DB_ERROR, HRESULT_ERROR, USER_DEFINED,
                       DB_ERROR_UNICODE, USER_DEFINED_UNICODE };

#ifdef OMS_IMPLEMENTATION
  DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* file, unsigned int line);
  DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* errtext, const char* file, unsigned int line,
            bool cancelledByProgram=false, bool provokedByTestKernel=false);
  DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* errtext, const OMS_ObjectId8& oid, const char* file, unsigned int line);
  DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const char* errtext, const OmsObjectId& oid, const char* file, unsigned int line);
  DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const DbpTypeUnicode* errtext, const char* file, unsigned int line);
  //DbpError (DbpErrorKinds ErrorKind, long ErrorNo, const DbpTypeUnicode* errtext, const OmsObjectId& oid, const char* file, unsigned int line);
#if !defined(SAPDB_UNICODE)
  // negated, since the other implementation is for OMS client already there
  const OmsTypeWyde* dbpErrorTextWyde() const;
#else
  const char* dbpErrorText() const;
#endif
#endif // OMS_IMPLEMENTATION

  /*--------------------------------------------------------------------------------*/
  /// Constructor
  /*!
  ** Constructor of the DbpError class. Sets the 'user-defined' type in the instance, 
  ** and assigns the ErrorNo value to the error number. The constructor can be used to 
  ** 'throw' error events defined by the application.
  */
  explicit DbpError (long ErrorNo);

  /*--------------------------------------------------------------------------------*/
  /// Constructor
  /*!
  ** Constructor of the DbpError class. In addition to the constructor described above, 
  ** an error text is set in the instance of the DbpError class. This is transferred as 
  ** a null-terminated string. The error text can be read out with the dbpErrorText method.
  */
  DbpError (long ErrorNo, const char* errtext);

  /*--------------------------------------------------------------------------------*/
  /// Constructor
  DbpError (long ErrorNo, const DbpTypeUnicode* errtext);

  /*--------------------------------------------------------------------------------*/
  /// Returns the error number stored in the instance.
  long  dbpError () const;

  /*--------------------------------------------------------------------------------*/
  /// Returns the error number 
  /*!
  ** Returns the error number if the instance of the DbpError class was created by an 
  ** error event that occurred in the liveCache.
  ** In all other cases, the value ‘0’ is returned.
  */
  short dbpDBError () const;

  /*--------------------------------------------------------------------------------*/
  /// Returns the error 
  /*! 
  ** Returns the error number if the instance of the DbpError class was created by an 
  ** error event that occurred in the OMS runtime. 
  ** In all other cases, the value ‘0’ is returned.
  */
  short dbpRteError () const;

  /*--------------------------------------------------------------------------------*/
  /// Returns the error number
  /*!
  ** Returns the error number if the instance of the DbpError class was created by an  
  ** error event that occurred in the component implementation. 
  ** In all other cases, the value ‘0’ is returned
  */
  long  dbpHresultError() const;


#if defined(SAPDB_UNICODE)
  /*--------------------------------------------------------------------------------*/
  /// Returns a pointer to the error text stored in the instance.
  /*!
  ** Returns a pointer to the error text stored in the instance.
  */
  const OmsTypeWyde* dbpErrorTextWyde() const;
#else
  /*--------------------------------------------------------------------------------*/
  /// Returns a pointer to the error text stored in the instance.
  /*!
  ** Returns a pointer to the error text stored in the instance.
  ** If the text is given in UNICODE-format, then NULL is returned.
  */
  const char* dbpErrorText() const;
#endif
  const DbpTypeUnicode* dbpErrorTextUnicode() const;
  const OmsObjectId& dbpErrorOid() const;

  /// Set a new callback interface to pre-process the exception
  static void dbpSetCallback(DbpErrorCallbackInterface *cb);
  /// Query current callback interface
  static DbpErrorCallbackInterface *dbpGetCallback();

  bool DbpCancelledByProgram()   const { return m_cancelledByProgram;   }
  bool DbpProvokedByTestKernel() const { return m_provokedByTestKernel; }

  DbpErrorKinds m_errorKind;
  mutable bool  m_isWyde;
  bool          m_cancelledByProgram;
  bool          m_provokedByTestKernel;
  int           m_errorNo;
  OmsObjectId   m_oid;
  mutable char  m_errorText[320];
};



/// Interface for checking the DbpError in DbpError constructor
class AFX_EXT_CLASS DbpErrorCallbackInterface {
public:
  /*!
   * \brief Check exception before throwing it.
   *
   * This method should process the exception before it is thrown, e.g.,
   * by writing a message and a stack trace into log file or by triggering
   * a debug breakpoint for certain exceptions.
   *
   * Define in an own handler class derived from this one.
   *
   * \see DbpError::dbpSetCallback
   * \see \ref omsdebug_cb
   */
  virtual void dbpCheckError(const class DbpError &e) = 0;

  /*!
   * \brief Method is called, if a dbpError is caught and handled in the liboms.
   *
   * As the callback dbpCheckError is called in every constructor of the
   * DbpError the application should be informed whether the exception is
   * handled in the liboms. 
   * This callback is needed if the application enters a special state after the
   * callback dbpCheckError has been called, and now expects that the own exception 
   * handling resets this state. But as the exception is handled in the liboms,
   * the state will never be reset. Therefore the liboms has to call this callback
   * if the exception is handled by itself.
   */
  virtual void dbpCaughtError(const DbpError& e) {}

  /*!
   * \brief Method is called, if a BAD_ALLOC is caught and handled in the liboms.
   *
   * As the callback dbpCheckError is called in every constructor of the
   * DbpError(BAD_ALLOC also creates a DbpError) the application should be 
   * informed whether the exception is handled in the liboms. 
   */
  virtual void dbpCaughtBadAlloc() {}

  ~DbpErrorCallbackInterface()
  { DbpError::dbpSetCallback(0); }
};

#endif  // __OMS_DBPERROR_HPP
