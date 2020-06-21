/*!
  @file           Trans_ExtendedMonitoringIter.hpp
  @author         FerdiF
  @ingroup        Transaction
  @since          2004-09-18

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
#ifndef Trans_ExtendedMonitoringIter_HPP
#define Trans_ExtendedMonitoringIter_HPP
//--------------------------------------------------------------------------------------
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "Transaction/Trans_ExtendedMonitoring.hpp"
#include "Transaction/Trans_ExtendedMonitoringContext.hpp"

//--------------------------------------------------------------------------------------
/// This iterator iterates over global transaction entries with a given criteria
class Trans_ExtendedMonitoringIter
{
public:
    /// iterator position defined as invalid 
    static const SAPDB_UInt4 cInvalidPos;

    /*!
    @brief default ctor inititlaizes internal structure
    */
    Trans_ExtendedMonitoringIter(tsp00_TaskId TaskId);

    /// dtor
    ~Trans_ExtendedMonitoringIter() {
        if (m_ctx.m_Comment)
            Trans_ExtendedMonitoring::GetAllocator().Deallocate(m_ctx.m_Comment);
    };

    /// returns true if this points to a valid entry
    bool IsValid()
    {
        return (cInvalidPos != m_next);
    }
    /// sets the iterator to the next transaction which matches the given TransType
    /// This enters a region and does not releases it until next call or no entries found.
    bool Next();

    /// return actual monitoring context hold by iterator 
    Trans_ExtendedMonitoringContext* GetMonitoringContext();

private:
    Trans_ExtendedMonitoringContext  m_ctx;
    SAPDB_UInt4                      m_next;
    tsp00_TaskId                     m_taskId;
    bool                             m_firstDone;
};
#endif // Trans_ExtendedMonitoringIter_HPP
