/*!
  @file           DBMSrvPar_Iterator.hpp
  @author         Bernd Vorsprach - bernd.vorsprach@sap.com
  @brief          DBMServer database parameters handling - Implementation

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
#include "SAPDB/DBM/Srv/Parameters/DBMSrvPar_Iterator.hpp"

/*******************************************************************************/
/*************** DBMSrvPar_Iterator ********************************************/
/*******************************************************************************/

/*-----------------------------------------------------------------------------*/
/*! @brief private constructor */
DBMSrvPar_Iterator::DBMSrvPar_Iterator
  ( DBMSrvPar_Parameters * pParameters )
  : m_pParameters(pParameters),
    m_pCurrent(pParameters==NULL?NULL:pParameters->pFirst)
{
} // end DBMSrvPar_Iterator::DBMSrvPar_Iterator

/*-----------------------------------------------------------------------------*/
/*! @brief public operator ++ */
DBMSrvPar_Iterator & DBMSrvPar_Iterator::operator++()
{
  m_pCurrent = m_pCurrent==NULL?NULL:m_pCurrent->pNext;
  return *this;
} // end DBMSrvPar_Iterator::operator*

void DBMSrvPar_Iterator::reset() 
{
  m_pCurrent = m_pParameters==NULL?NULL:m_pParameters->pFirst;
}


/*******************************************************************************/
/*************** DBMSrvPar_ClassIterator ***************************************/
/*******************************************************************************/

/*-----------------------------------------------------------------------------*/
/*! @brief private constructor */
DBMSrvPar_ClassIterator::DBMSrvPar_ClassIterator
  ( DBMSrvPar_Parameters * pParameters,
    SAPDB_UInt4            nClasses)
  : m_pParameters(pParameters),
    m_pCurrent(pParameters==NULL?NULL:pParameters->pFirst),
    m_nClasses(nClasses)
{
  while ((m_pCurrent != NULL) && ((m_pCurrent->nClasses & m_nClasses) == 0) ) {
    m_pCurrent = m_pCurrent->pNext;
  } // end while

} // end DBMSrvPar_ClassIterator::DBMSrvPar_ClassIterator

/*-----------------------------------------------------------------------------*/
/*! @brief public operator ++ */
DBMSrvPar_ClassIterator & DBMSrvPar_ClassIterator::operator++()
{
  m_pCurrent = m_pCurrent==NULL?NULL:m_pCurrent->pNext;
  while ((m_pCurrent != NULL) && ((m_pCurrent->nClasses & m_nClasses) == 0) ) {
    m_pCurrent = m_pCurrent->pNext;
  } // end while
  return *this;
} // end DBMSrvPar_ClassIterator::operator*

void DBMSrvPar_ClassIterator::reset() 
{
  m_pCurrent = m_pParameters==NULL?NULL:m_pParameters->pFirst;
  while ((m_pCurrent != NULL) && ((m_pCurrent->nClasses & m_nClasses) == 0) ) {
    m_pCurrent = m_pCurrent->pNext;
  } // end while
}
