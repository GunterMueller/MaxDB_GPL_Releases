/*! 
  -----------------------------------------------------------------------------
 
  module: Tools_Session.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: session handling for web access
   
  description:  a session handling container

  version:      7.3 and higher 
  
  Copyright (c) 2000-2005 SAP AG
 
  -----------------------------------------------------------------------------


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

#ifndef _Tools_Session_HPP_
#define _Tools_Session_HPP_

#include <time.h>
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

template <class E> class Tools_SessionElement;
template <class E> class Tools_Session;

/*
  -----------------------------------------------------------------------------
  Class:        Tools_SessionElement
  -----------------------------------------------------------------------------
  description:  session container element
  -----------------------------------------------------------------------------
 */
template <class E>
class  Tools_SessionElement {

private:
  Tools_SessionElement(SAPDB_UInt4 nSessionID)
    {
      m_pNext      = NULL; 
      m_pObject    = NULL; 
      m_nSessionID = nSessionID;
      time(&m_tLastAccess); 
    } // end Tools_SessionElement::Tools_SessionElement

  ~Tools_SessionElement()
    {
      if (m_pObject != NULL) delete m_pObject; 
    } // end Tools_SessionElement::~Tools_SessionElement

  Tools_SessionElement   * m_pNext;
  E                      * m_pObject;
  time_t                   m_tLastAccess;
  SAPDB_UInt4              m_nSessionID;

  friend class Tools_Session<E>;
};
/* EndClass: Tools_SessionElement */

/*!
  -----------------------------------------------------------------------------
  Class:        Tools_Session
  -----------------------------------------------------------------------------
  description:  session container 
  -----------------------------------------------------------------------------
 */
template <class E>
class Tools_Session {

public:

  /*!
    -------------------------------------------------------------------------
    function:     Tools_Session
    -------------------------------------------------------------------------
    description:  Constructor

    arguments:    tTimeout [IN] - timeout for the session handling
                                  At each access to the session object call a 
                                  "garbage collection"  which removes all elements 
                                  which were not accessed a period > timeout

                                  A timeout of 0 means no garbage collection.
    -------------------------------------------------------------------------
  */
  Tools_Session  (time_t tTimeout = 0);

  /*!
    -------------------------------------------------------------------------
    function:     ~Tools_Session
    -------------------------------------------------------------------------
    description:  Destructor
    -------------------------------------------------------------------------
  */
  ~Tools_Session ();

  /*!
    -------------------------------------------------------------------------
    function:     add
    -------------------------------------------------------------------------
    description:  adds a element to the container

    arguments:    nSessionID [OUT] - returns the new session ID
                                     You may place this ID in your element.

    return value: the new element or NULL in case of errors
    -------------------------------------------------------------------------
  */
  E    * add    (SAPDB_UInt4 & nSessionID);

  /*!
    -------------------------------------------------------------------------
    function:     get
    -------------------------------------------------------------------------
    description:  retrieves an element from the session container

    arguments:    nSessionID [IN] - your sessionID

    return value: the element or NULL in case of errors (ID not exists)
    -------------------------------------------------------------------------
  */
  E    * get    (SAPDB_UInt4 nSessionID);

  SAPDB_Bool sessionidvalid    (SAPDB_UInt4 nSessionID);

  /*!
    -------------------------------------------------------------------------
    function:     remove
    -------------------------------------------------------------------------
    description:  removes an element from the session container

    arguments:    nSessionID [IN] - your sessionID
    -------------------------------------------------------------------------
  */
  void   remove (SAPDB_UInt4 nSessionID);

  /*!
    -------------------------------------------------------------------------
    function:     AsciiToSessionID
    -------------------------------------------------------------------------
  */
  static SAPDB_UInt4   AsciiToSessionID (const char * pSource);

  /*!
    -------------------------------------------------------------------------
    function:     SessionIDToAscii
    -------------------------------------------------------------------------
  */
  static Tools_DynamicUTF8String SessionIDToAscii (SAPDB_UInt4  nSessionID);

private:
  typedef Tools_SessionElement<E> TheElement;

  void                     garbageCollection();

  TheElement             * m_pAnchor;
  time_t                   m_tLastGarbageCollection;
  time_t                   m_tTimeout;
  SAPDB_UInt4              m_nLastSessionID;

};
/*! EndClass: Tools_Session */

/*
  -----------------------------------------------------------------------------
  public constructor Tools_Session::Tools_Session
  -----------------------------------------------------------------------------
*/
template <class E>
Tools_Session<E>::Tools_Session(time_t tTimeout)
{
  m_nLastSessionID = 0;
  m_pAnchor        = NULL;
  m_tTimeout       = tTimeout;
  time(&m_tLastGarbageCollection);
} // end Tools_Session::Tools_Session

/*
  -----------------------------------------------------------------------------
  public destructor Tools_Session::~Tools_Session
  -----------------------------------------------------------------------------
*/
template <class E>
Tools_Session<E>::~Tools_Session()
{
  TheElement * pCurrent = m_pAnchor;
  TheElement * pNext    = NULL;

  while (pCurrent != NULL) {
    pNext = pCurrent->m_pNext;
    delete pCurrent;
    pCurrent = pNext;
  } // end while

} // end Tools_Session::~Tools_Session

/*
  -----------------------------------------------------------------------------
  public function Tools_Session::add
  -----------------------------------------------------------------------------
*/
template <class E>
E * Tools_Session<E>::add(SAPDB_UInt4 & nSessionID)
{
  garbageCollection();

  TheElement  * pElement      = NULL;
  SAPDB_UInt4   nNewSessionID = m_nLastSessionID + 1;

  nSessionID = 0;

  // compute new session id
  while (get(nNewSessionID) != NULL) {
    ++nNewSessionID;
  } // end if
  
  // create element
  if (get(nNewSessionID) == NULL) {
    m_nLastSessionID = nNewSessionID;
    pElement = new TheElement (nNewSessionID);
    if (pElement != NULL) {
      pElement->m_pObject = new E();
      if (pElement->m_pObject == NULL) {
        delete pElement;
        pElement = NULL;
      } // end if
    } // end if
  } // end if

  // maintain list
  if (pElement != NULL) {
    nSessionID = nNewSessionID;

    TheElement * pCurrent = m_pAnchor;
    TheElement * pLast    = NULL;
    if (pCurrent == NULL) {
      m_pAnchor = pElement;
    } else {
      while (pCurrent != NULL) {
        pLast = pCurrent;
        pCurrent = pCurrent->m_pNext;
      } // end while
      pLast->m_pNext = pElement;
    } // end if
  } // end if

  return (pElement != NULL) ? pElement->m_pObject : NULL;
} // end Tools_Session::add

/*
  -----------------------------------------------------------------------------
  public function Tools_Session::remove
  -----------------------------------------------------------------------------
*/
template <class E>
void Tools_Session<E>::remove(SAPDB_UInt4 nSessionID)
{
  garbageCollection();

  TheElement * pCurrent = m_pAnchor;
  TheElement * pLast    = NULL;
  while (pCurrent != NULL) {
    if (pCurrent->m_nSessionID  == nSessionID) {
      if (pLast == NULL) {
        m_pAnchor = pCurrent->m_pNext;
      } else {
        pLast->m_pNext = pCurrent->m_pNext;
      } // end if
      delete pCurrent;
      break;
    } else {
      pLast = pCurrent;
      pCurrent = pCurrent->m_pNext;
    } // end if
  } // end while

} // end Tools_Session::remove

/*
  -----------------------------------------------------------------------------
  public function Tools_Session::get
  -----------------------------------------------------------------------------
*/
template <class E>
E * Tools_Session<E>::get(SAPDB_UInt4 nSessionID)
{
  garbageCollection();

  TheElement * pCurrent = m_pAnchor;
  while (pCurrent != NULL) {
    if (pCurrent->m_nSessionID == nSessionID) {
      time(&(pCurrent->m_tLastAccess));
      break;
    } // end if
    pCurrent = pCurrent->m_pNext;
  } // end while

  return (pCurrent != NULL) ? pCurrent->m_pObject : NULL;
} // end Tools_Session::get

template <class E>
SAPDB_Bool Tools_Session<E>::sessionidvalid(SAPDB_UInt4 nSessionID)
{
  garbageCollection();

  TheElement * pCurrent = m_pAnchor;
  while (pCurrent != NULL) {
    if (pCurrent->m_nSessionID == nSessionID) {
      time(&(pCurrent->m_tLastAccess));
      break;
    } // end if
    pCurrent = pCurrent->m_pNext;
  } // end while

  return (pCurrent != NULL) ? TRUE : FALSE;
} // end Tools_Session::get


/*
  -----------------------------------------------------------------------------
  private function Tools_Session::garbageCollection
  -----------------------------------------------------------------------------
*/
template <class E>
void Tools_Session<E>::garbageCollection()
{
  time_t tNow;

  time(&tNow);

  if ((m_tTimeout > 0) && ((tNow - m_tLastGarbageCollection) > m_tTimeout)) {
    TheElement * pCurrent = m_pAnchor;
    TheElement * pNext    = NULL;
    while (pCurrent != NULL) {
      pNext = pCurrent->m_pNext;
      if ((tNow - pCurrent->m_tLastAccess) > m_tTimeout) {
        remove (pCurrent->m_nSessionID);
      } // end if
      pCurrent = pNext;
    } // end while
  } // end if

} // end Tools_Session::garbageCollection

/*
  -----------------------------------------------------------------------------
  private function Tools_Session :: AsciiToSessionID
  -----------------------------------------------------------------------------
 */
template <class E>
SAPDB_UInt4 Tools_Session<E>:: AsciiToSessionID(const char * pSource) 
{
  SAPDB_UInt4     nSessionID = 0;
  unsigned char * pTarget    = (unsigned char*) &nSessionID;

  size_t nMaxSource = strlen(pSource);
  size_t nMaxTarget = sizeof(nSessionID);
  size_t nSource = 0;
  size_t nTarget = 0;

  while ((nTarget < nMaxTarget) && (nSource < nMaxSource)) {
    pTarget[nTarget] = pSource[nSource] - 48;
    ++nSource;
    if (nSource < nMaxSource) {
      pTarget[nTarget] = (pTarget[nTarget] * 10) + (pSource[nSource] - 48);
      ++nSource;
      if (nSource < nMaxSource) {
        pTarget[nTarget] = (pTarget[nTarget] * 10) + (pSource[nSource] - 48);
        ++nSource;
        ++nTarget;
      } // end if
    } // end if
  } // end while

  return nSessionID;
} // end Tools_Session :: AsciiToSessionID

/*
  -----------------------------------------------------------------------------
  private function Tools_Session :: SessionIDToAscii
  -----------------------------------------------------------------------------
 */
template <class E>
Tools_DynamicUTF8String Tools_Session<E>:: SessionIDToAscii (SAPDB_UInt4 nSessionID)
{
  unsigned char * pSource    = (unsigned char*) &nSessionID;
  size_t          nMaxSource = sizeof(nSessionID);
  size_t          nSource    = 0;
  char            szTarget[50];
  char          * pTarget    = &szTarget[0];;
  
  for (nSource = 0; nSource < nMaxSource; ++nSource) {
    sprintf(pTarget, "%03d", (int) pSource[nSource]);
    pTarget += 3;
  } // end for

  return szTarget;
} // end Tools_Session :: SessionIDToAscii


#endif // _Tools_Session_HPP_
