/*!
  @file           DBMSrvPar_Iterator.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Specification

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

#ifndef _DBMSrvPar_Iterator_HPP_
#define _DBMSrvPar_Iterator_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Parameters.hpp"

/*! @brief parameter iterator object */
class DBMSrvPar_Iterator {

public:
  /*! @brief constructor */
  DBMSrvPar_Iterator
    ( DBMSrvPar_Parameters  * pParameters );

  /*! @brief returns element */
  DBMSrvPar_Parameter * operator*()
  { return m_pCurrent; }
  
  /*! @brief advances iterator to next element */
  DBMSrvPar_Iterator  & operator++();
  
  /*! @brief to start iteration again */
  void reset();

private:
  DBMSrvPar_Parameters            * m_pParameters;
  DBMSrvPar_Parameter             * m_pCurrent;

  friend class DBMSrvPar_Parameters;
}; // end class DBMSrvPar_Iterator

/*! @brief parameter class iterator object */
class DBMSrvPar_ClassIterator {

public:
  /*! @brief constructor */
  DBMSrvPar_ClassIterator
    ( DBMSrvPar_Parameters * pParameters,
      SAPDB_UInt4            nClasses );

  /*! @brief returns element */
  DBMSrvPar_Parameter * operator*()
  { return m_pCurrent; }
  
  /*! @brief advances iterator to next element */
  DBMSrvPar_ClassIterator  & operator++();

  /*! @brief to start iteration again */
  void reset();

private:
  DBMSrvPar_Parameters            * m_pParameters;
  DBMSrvPar_Parameter             * m_pCurrent;
  SAPDB_UInt4                       m_nClasses;

  friend class DBMSrvPar_Parameters;
}; // end class DBMSrvPar_ClassIterator

#endif /* _DBMSrvPar_Iterator_HPP_ */

