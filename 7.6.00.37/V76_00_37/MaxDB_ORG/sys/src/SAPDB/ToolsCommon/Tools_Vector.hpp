/*! 
  -----------------------------------------------------------------------------
 
  module: Tools_Vector.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: universal stuff for web access
   
  description:  a vector class like STL vector

  version:      7.3 and higher 
  -----------------------------------------------------------------------------
 
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

#ifndef _TOOLS_VECTOR_HPP_
#define _TOOLS_VECTOR_HPP_

#include <stddef.h>

/*
  -----------------------------------------------------------------------------
  macros
  -----------------------------------------------------------------------------
 */
#define Tools_VECTOR_SIZE_STEP 10
#define Tools_EMPTY            -1

/*
  -----------------------------------------------------------------------------
  Class:        Tools_Vector
  -----------------------------------------------------------------------------

  description:  Vector

  -----------------------------------------------------------------------------
 */
template <class E> class Tools_Vector
{
  public:
    Tools_Vector( );
    virtual ~Tools_Vector( );

    /* \brief add a copy of aElement to the end of the vector
       \return false, if memory could not be allocated as needed, true otherwise */
    bool      push_back(const E & aElement);
    void      pop_back();
    bool      empty() {return (m_nCurrent == Tools_EMPTY);}

    typedef E * iterator;
    iterator  begin() const {return &( m_theVector[0]);}
    iterator  end() const {return &(m_theVector[m_nCurrent + 1]); }

    E & back();

    /*! \brief a type used for any vector size related variables */
    typedef size_t size_type;

    /*! \brief a type used for any vector size related variables */
    typedef E & reference;

    /*! \brief function returning the actual numbers of elements in the vector
        \return number of elements in the vector*/
    size_t size() const {return m_nCurrent+1;}

    /*! \brief function for determining the capacity of the vector
        \return number of elements that the vector will manage without reallocating any memory */
    size_t capacity() const {return m_nSize;}

    reference operator[](size_type n) const {return m_theVector[n];}

  private:
    E   * m_theVector;
    int   m_nCurrent;
    int   m_nSize;

    //avoid implicit copying
    Tools_Vector(const Tools_Vector& otherVector) {}
};

/* EndClass: Tools_Vector */

/*
  =============================================================================
  implementation 
  =============================================================================
 */


/*
  -----------------------------------------------------------------------------
  public constructor Tools_Vector :: Tools_Vector
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Vector<E> :: Tools_Vector ( )
{
  m_nSize    = Tools_VECTOR_SIZE_STEP;
  m_nCurrent = Tools_EMPTY;

  m_theVector = new E[m_nSize + 1];

} // end Tools_Vector :: Tools_Vector

/*
  -----------------------------------------------------------------------------
  public destructor Tools_Vector :: ~Tools_Vector
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Vector<E> :: ~Tools_Vector ( )
{
  if (m_theVector != 0) {
    delete [] m_theVector;
    m_theVector = 0;
  } // end if
} // end Tools_Vector :: ~Tools_Vector

/*
  -----------------------------------------------------------------------------
  public function Tools_Vector :: push_back
  -----------------------------------------------------------------------------
 */
template <class E>
inline bool Tools_Vector<E> :: push_back ( const E & aElement )
{
    bool rc=true;
    int  nIndex = 0;

    if ((m_nSize - 1) == m_nCurrent)
    {
        m_nSize += Tools_VECTOR_SIZE_STEP;

        E *  m_newVector = new E[m_nSize + 1];

        if(0==m_newVector)
            rc=false;
        else
        {
            for (nIndex = 0;nIndex < (m_nSize - Tools_VECTOR_SIZE_STEP);++nIndex)
                m_newVector[nIndex] = m_theVector[nIndex];

            delete [] m_theVector;
            m_theVector = m_newVector;

            m_theVector[++m_nCurrent] = aElement;
        }
    }
    else
        m_theVector[++m_nCurrent] = aElement;

    return rc;
} // end Tools_Vector :: push_back


//  -----------------------------------------------------------------------------
//  public function Tools_Vector::pop_back()
//  -----------------------------------------------------------------------------

template <class E> inline void Tools_Vector<E>::pop_back()
{
    m_nCurrent--;
}


//  -----------------------------------------------------------------------------
//  public function Tools_Vector::back()
//  -----------------------------------------------------------------------------

template <class E> inline E & Tools_Vector<E>::back()
{
    return m_theVector[m_nCurrent];
}

#endif // _TOOLS_VECTOR_HPP_
