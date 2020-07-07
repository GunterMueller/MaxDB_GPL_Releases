/*!
  @file           Catalog_RequestScope.hpp
  @author         ThomasA 
  @ingroup        Catalog
  @brief          defines switch and tracing while catalog access is running

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

#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hgg01.h"
#include "hgg01_3.h"

#if defined(SAPDB_SLOW)
#include "hta01.h"
#endif

/*!
   @brief used to turn off switch or vtrace while catalog access is running
 */
class Catalog_RequestScope
{
public :
    /*!
       @brief constructor, disables switch and tracing depending on current state
     */
    Catalog_RequestScope(SQLMan_Context& context)
        : m_context(context)
#if defined(SAPDB_SLOW)
        , m_restore(false)
#endif
        , m_enableVtrace(false)
        , m_restoreVtraceState(false)
    {
        if (!Catalog_Trace.TracesLevel(1))
        {
#if defined(SAPDB_SLOW)
            const bool doTurnOff = true;
            if (g01maxuser() <= 4)
            {
                t01SetSingleUser();
                t01SaveSwitchAdjustments (doTurnOff);
                m_restore = true;
            }
#endif
            if (g01maxuser() <= 4)
            {
                // assuming single user test
                m_restoreVtraceState = true;
                m_vtraceState        = g01vtrace;
                SAPDB_MemFillNoCheck (&g01vtrace, 0, sizeof(g01vtrace));
            }
            m_enableVtrace = m_context.EnablePacketVtrace(false);
        }
        
    }
    /*!
       @brief destructor, restores switch and tracing state
    */
    ~Catalog_RequestScope()
    {
#if defined(SAPDB_SLOW)
        if (m_restore)
        {
            t01RestoreSwitchAdjustments();
        }
#endif
        if (m_enableVtrace)
        {
            m_context.EnablePacketVtrace(true);
        }
        if (m_restoreVtraceState)
        {
            g01vtrace = m_vtraceState;
        }
    }
    /*!
       @brief tells whether any vtrace is enabled
     */
    bool AnyVtrace() const
    {
        if (m_restoreVtraceState)
        {
            return (m_vtraceState.vtrAny_gg00 != 0);
        }
        else
        {
            return (g01vtrace.vtrAny_gg00 != 0);
        }
    }
private :
    SQLMan_Context& m_context;
#if defined(SAPDB_SLOW)
    bool              m_restore;
#endif
    bool              m_enableVtrace;
    bool              m_restoreVtraceState;
    tgg00_VtraceState m_vtraceState;
};