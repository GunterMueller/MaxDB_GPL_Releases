/*!
  @file           Rst_Types.hpp
  @author         UweH
  @ingroup        Restart
  @brief          some typs used commonly in this component

\if EMIT_LICENCE
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

\endif
*/
#ifndef RST_TYPES_HPP
#define RST_TYPES_HPP
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Container/Container_Vector.hpp"
/// kind of usage
enum Rst_RedoKind
{
    Rst_RedoKindNormal,
    Rst_RedoKindRestoreLog,
    Rst_RedoKindStandby
};
/// This is a list of transactions to check if a given transid is included there
class Rst_Transactions // PTS 1134103 UH 2005-03-07 new
{
public:
    /// default constructor
    Rst_Transactions ()
    : m_pTransNoList(0)
    , m_ElemCount   (0)
    {
        m_YoungestTransNo.gg90SetNil();
    }
    /// default destructor
    ~Rst_Transactions ()
    {
        Delete();
    }
    /// returns true if given transno can be found in the list of transactions
    bool Includes(const tgg91_TransNo &transno)
    {
        if ( 0 == m_pTransNoList )
            return false;
        return SearchInList(transno);
    }
    /// adds a new transno
    bool Add ( const tgg91_TransNo &transno )
    {
        if ( 0 == m_pTransNoList || m_ElemCount > m_pTransNoList->GetSize() )
            return false;
        (*m_pTransNoList)[m_ElemCount] = transno;
        ++m_ElemCount;
        if ( ! m_YoungestTransNo.gg90IsNil() && transno > m_YoungestTransNo )
            m_YoungestTransNo = transno;
        return true;
    }
    /// initialize and allocate structures
    bool Init ( SAPDBMem_IRawAllocator &allocator,
                SAPDB_UInt              expectedCount )
    {
        if ( m_pTransNoList != 0 )
            return false;
        m_pTransNoList = new (allocator) Container_Vector<tgg91_TransNo> (allocator);
        if ( m_pTransNoList == 0 )
            return false;
        if ( expectedCount != 0 )
            if ( ! m_pTransNoList->Initialize(expectedCount) )
            {
                Delete();
                return false;
            }
        return true;
    }
    /// Deallocates all structures
    void Delete()
    {
        if ( m_pTransNoList != 0 )
            destroy(m_pTransNoList,m_pTransNoList->GetAllocator());
        m_ElemCount = 0;
        m_YoungestTransNo.gg90SetNil();
    }
public:
    /// list of transno's
    typedef Container_Vector<tgg91_TransNo> TransNoList;
    /// iterator for list over transno's
    typedef TransNoList::Iterator           Iterator;
private:
    bool SearchInList(const tgg91_TransNo &transno)
    {
        if ( 0 == m_pTransNoList || 0 == m_ElemCount )
            return false;
    
        if ( transno > m_YoungestTransNo )
            return false;
    
        for ( Iterator iter = m_pTransNoList->Begin(); iter != m_pTransNoList->End(); ++iter )
            if ( transno == *iter )
                return true;
    
        return false;
    }
private:
    Container_Vector<tgg91_TransNo> *m_pTransNoList;
    tgg91_TransNo                    m_YoungestTransNo;
    SAPDB_UInt                       m_ElemCount;
};
#endif  // RST_TYPES_HPP
