/*!
    @file           DBMSrvShM_AlignmentHelper.hpp
    @author         MarcW
    @brief          calculation of aligned addresses in memory

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

*/

#if !defined(DBMSrvShM_AlignmentHelper_HPP)
#define DBMSrvShM_AlignmentHelper_HPP

#include <stddef.h>

/*!
    @brief class for administration of the two parts fo the shm

    This critical base administration data for the shm 
    must be maintained in sync.
*/
class DBMSrvShM_AlignmentHelper {
public:
    DBMSrvShM_AlignmentHelper()
        : m_BlockSize(sizeof(void*)) {
    }

    size_t alignedSizeOf(size_t objectSize) {
        if( objectSize % m_BlockSize == 0 )
            return objectSize;
        else if( objectSize == 0 )
            return 0;
        else if ( objectSize < m_BlockSize )
            return m_BlockSize;
        else
            return ((objectSize / m_BlockSize) * m_BlockSize) + m_BlockSize;
    }

    void* nextAlignedAddress( void* startAddress, size_t objectSize ) {
        // align the start
        size_t temp = (size_t)startAddress;
        temp = alignedSizeOf(temp);
        // return aligned start plus aligned object size
        return (void*)(temp + alignedSizeOf(objectSize));
    }

private:
    const size_t m_BlockSize;
};

#endif //DBMSrvShM_AlignmentHelper_HPP
