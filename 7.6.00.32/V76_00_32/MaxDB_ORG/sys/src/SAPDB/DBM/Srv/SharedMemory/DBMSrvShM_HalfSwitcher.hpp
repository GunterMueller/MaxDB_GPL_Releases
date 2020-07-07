/*!
    @file           DBMSrvShM_HalfSwitcher.hpp
    @author         MarcW
    @brief          switching shm parts

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

#if !defined(DBMSrvShM_HalfSwitcher_HPP)
#define DBMSrvShM_HalfSwitcher_HPP

#include <stddef.h>

/*!
    @brief class for administration of the two parts fo the shm

    This critical base administration data for the shm 
    must be maintained in sync.
*/
class DBMSrvShM_HalfSwitcher {
public:
    size_t getMemSize() {return m_MemSize;}
    void setMemSize( const size_t newMemSize ) {m_MemSize = newMemSize;}
    size_t getDataSize() {return m_DataSize;}
    void setDataSize( const size_t newDataSize ) {m_DataSize = newDataSize;}
    size_t getOffset() {return m_Offset;}
    void setOffset( const size_t newOffset ) {m_Offset = newOffset;}
    bool isUpperHalf() { return m_Offset==0;}
private:
    // current absolute size of the shm
    size_t m_MemSize;
    // size of datapart which is in use
    size_t m_DataSize;
    // offset from beginning of file to first admin field (which is the first
    // eventing admin field)
    size_t m_Offset;
};

#endif //DBMSrvShM_HalfSwitcher_HPP
