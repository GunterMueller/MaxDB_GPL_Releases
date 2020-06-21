/*!***************************************************************************

  module:       CnsRead_SwizzledPointer.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  Defines a swizzled pointer 
  last changed: 2000-08-10  12:27
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


*****************************************************************************/

#ifndef CNSREAD_SWIZZLEDPOINTER_HPP
#define CNSREAD_SWIZZLEDPOINTER_HPP

#include "SAPDBCommon/SAPDB_Types.hpp" 

#if defined(SUN) || defined (AIX) || defined (HPUX) || defined(SPARC)
#define HI_BYTE 0
#else
#define HI_BYTE 1
#endif

/*!
   @brief defines a swizzled pointer. A swizzled Pointer may be layed over any structure having a size
          of at least the size of a pointer and having at least 2 byte alignment. 
          The first element of the structure interpreted as a
          2 byte integer must not occupy the range from 2**14 to 2**16
          Under these conditions it is possible to decide whether the structure contains a swizzled
          pointer.
 */
template<class T>
class CnsRead_SwizzledPointer
{
public :
    void Assign(T* p)
    {
        unsigned char* pChar  = reinterpret_cast<unsigned char*>(&p);
        SAPDB_UInt2* pUint    = reinterpret_cast<SAPDB_UInt2*>(&p);
#if defined(WIN32)
        m_bytes[0]            = pChar[1];
        m_bytes[1]            = (pChar[0] >> 1) | 0x80; 
        m_swizzledPointer[1]  = pUint[1];
#else
        m_bytes[0] = (pChar[3] >> 1) | 0x80;
        m_bytes[1] = pChar[0];
        m_bytes[2] = pChar[1];
        m_bytes[3] = pChar[2];
#endif
#if defined(BIT64)
        m_swizzledPointer[2]  = pUint[2];
        m_swizzledPointer[3]  = pUint[3];
#endif
    }

    T* operator->()
    {
        return this->GetPointer();  
    }
    bool IsPointer() const
    {
#if !defined(WIN32)
        return false;
#else
        return (0 != (m_bytes[HI_BYTE] & 0x80));
#endif
    }
    T* GetPointer() const
    {
        if (this->IsPointer())
        {
            T* p;
            unsigned char* pChar  = reinterpret_cast<unsigned char*>(&p);
            SAPDB_UInt2* pUint    = reinterpret_cast<SAPDB_UInt2*>(&p);
#if defined(WIN32)
            pChar[0] = m_bytes[1] << 1;
            pChar[1] = m_bytes[0];
            pUint[1] = m_swizzledPointer[1];
#else
            pChar[3] = m_bytes[0] << 1;
            pChar[0] = m_bytes[1];
            pChar[1] = m_bytes[2];
            pChar[2] = m_bytes[3];
#endif
#if defined(BIT64)
            pUint[2] = m_swizzledPointer[2];
            pUint[3] = m_swizzledPointer[3];
#endif
            return p;  
        }
        else
        {
            return (T*) 0;
        }
    }
private :
    union {
      SAPDB_UInt2    m_swizzledPointer[sizeof(void*) / sizeof(SAPDB_UInt2)];
      unsigned char  m_bytes[sizeof(sizeof(void*))];
    };
};

#endif
