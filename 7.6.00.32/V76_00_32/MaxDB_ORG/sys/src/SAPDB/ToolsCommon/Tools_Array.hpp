/*! 
  -----------------------------------------------------------------------------
 
  module: Tools_Array.hpp
 
  -----------------------------------------------------------------------------
 
  responsible:  BerndV
 
  special area: universal stuff for web access
   
  description:  a string class like STL string

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

#ifndef _Tools_Array_HPP_
#define _Tools_Array_HPP_

/*
  -----------------------------------------------------------------------------
  Class:        Tools_Array
  -----------------------------------------------------------------------------

  description:  Array

  -----------------------------------------------------------------------------
 */
template <class E>
class Tools_Array
{
public:

  typedef int       size_type;
  typedef       E & reference;
  typedef const E & const_reference;

  explicit Tools_Array  ( );
           Tools_Array  (const Tools_Array & aArray);
          ~Tools_Array  ( );

  inline reference       operator [] ( size_type nPos)
    {
      if (nPos >= this->m_theSize) {
        reallocate(nPos);
      } // end if
      m_theMax = (m_theMax < nPos) ? nPos : m_theMax;
      return this->m_theArray[nPos];
    } // end Tools_Array<E>::operator []

  inline const_reference operator [] ( size_type nPos) const
    {
      if (nPos >= this->m_theSize) {
        return this->m_theEmpty;
      } else {
        return this->m_theArray[nPos];
      } // end if
    } // end Tools_Array<E>::operator []

  Tools_Array & operator = (const Tools_Array & aArray);
  size_type     size       ( ) const { return m_theMax + 1;}
  void          clear      ( );

protected:
  
  void reallocate(size_type nNewSize = 0);

  E           m_theEmpty;
  E         * m_theArray;
  size_type   m_theSize;
  size_type   m_theMax;

};

/* EndClass: Tools_Array */

/*
  =============================================================================
  implementation class Tools_Array
  =============================================================================
 */

/*
  -----------------------------------------------------------------------------
  private macros
  -----------------------------------------------------------------------------
 */
#define Tools_ARRAY_SIZE_STEP 10

/*
  -----------------------------------------------------------------------------
  public constructor Tools_Array :: Tools_Array
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Array<E> :: Tools_Array ( )
                                : m_theArray(0),
                                  m_theSize(0),
                                  m_theMax(-1)
{

} // end Tools_Array :: Tools_Array
/*
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Array<E> :: Tools_Array (const Tools_Array & aArray)
                                : m_theArray(0),
                                  m_theSize(0),
                                  m_theMax(-1)
{
  *this = aArray;
} // end Tools_Array :: Tools_Array

/*
  -----------------------------------------------------------------------------
  public operator Tools_Array :: =
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Array<E> & Tools_Array<E>::operator = (  const Tools_Array & aArray )
{
  for (size_type nIndex = 0; nIndex < aArray.m_theSize; ++nIndex) {
    (*this)[nIndex] = aArray[nIndex];
  } // end if
  return *this;
} // end Tools_Array::operator =

/*
  -----------------------------------------------------------------------------
  public destructor Tools_Array :: ~Tools_Array
  -----------------------------------------------------------------------------
 */
template <class E>
inline Tools_Array<E> :: ~Tools_Array ( )
{
  clear();
} // end Tools_Array :: ~Tools_Array

/*
  -----------------------------------------------------------------------------
  public operator Tools_Array :: []
  -----------------------------------------------------------------------------
 */
/*
template <class E>
inline typename Tools_Array<E>::reference Tools_Array<E>::operator [] ( size_type nPos ) 
{
  if (nPos >= this->m_theSize) {
    reallocate(nPos);
  } // end if
  m_theMax = (m_theMax < nPos) ? nPos : m_theMax;
  return this->m_theArray[nPos];
} // end Tools_Array<E>::operator []
*/
/*
  -----------------------------------------------------------------------------
 */
/*
template <class E>
inline typename  Tools_Array<E>::const_reference Tools_Array<E>::operator [] ( size_type nPos ) const
{
  if (nPos >= this->m_theSize) {
    return this->m_theEmpty;
  } else {
    return this->m_theArray[nPos];
  } // end if
} // end Tools_Array<E>::operator []
*/

/*
  -----------------------------------------------------------------------------
  public function Tools_Array :: clear
  -----------------------------------------------------------------------------
 */
template <class E>
inline void Tools_Array<E> :: clear ( )
{
  if (this->m_theArray != 0) {
    delete [] this->m_theArray;
  } // end if
  this->m_theArray  = 0;
  this->m_theSize   = 0;
  this->m_theMax    = -1;
} // end Tools_Array<E> :: clear

/*
  -----------------------------------------------------------------------------
  private function Tools_Array :: reallocate
  -----------------------------------------------------------------------------
 */
template <class E>
inline void Tools_Array<E> :: reallocate (size_type nNewSize )
{
  size_type newSize = ((nNewSize / Tools_ARRAY_SIZE_STEP) * Tools_ARRAY_SIZE_STEP) + Tools_ARRAY_SIZE_STEP;

  if (newSize != this->m_theSize) {
    E* newArray = new E[newSize + 1];

    for (size_type nIndex = 0;nIndex < newSize && nIndex < this->m_theSize; ++nIndex) {
      newArray[nIndex] = this->m_theArray[nIndex];
    } // end for

    if (this->m_theArray != 0) {
      delete [] this->m_theArray;
    } // end if

    this->m_theSize  = newSize;
    this->m_theArray = newArray;
  } // end if
} // end Tools_Array :: reallocate

#endif // _Tools_Array_HPP_
