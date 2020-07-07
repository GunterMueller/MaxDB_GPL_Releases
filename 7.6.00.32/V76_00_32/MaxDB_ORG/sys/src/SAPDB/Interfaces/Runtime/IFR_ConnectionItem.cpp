/*!
  @file           IFR_ConnectionItem.cpp
  @author         D039759
  @ingroup        IFR_Connection
  @brief          
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
//----------------------------------------------------------------------
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_SQLWarning.h"
#include "Interfaces/Runtime/IFR_Connection.h"

//----------------------------------------------------------------------
IFR_ConnectionItem::IFR_ConnectionItem(IFR_Connection* connection)
:m_warning(0),
 m_error(connection->allocator, connection),
 m_allocator(&(connection->allocator)),
 m_closeflag(false)
{
}

//----------------------------------------------------------------------
IFR_ConnectionItem::IFR_ConnectionItem
(SAPDBMem_IRawAllocator& allocator)
:m_warning(0),
 m_error(allocator, 0),
 m_allocator(&allocator),
 m_closeflag(false)
{}


//----------------------------------------------------------------------
void
IFR_ConnectionItem::clearError()
{
    error().clear();
}

//----------------------------------------------------------------------            
void
IFR_ConnectionItem::clearWarnings()
{
    IFR_SQLWarning *w=warning();
    while(w) {
        IFR_SQLWarning *w_next=w->getNextWarning();
        IFRUtil_Delete(w, *m_allocator);
        w = w_next;
    }
    warning()=0;  // clear out the warning itself.
}

//----------------------------------------------------------------------
IFR_Retcode 
IFR_ConnectionItem::assertOpen ()
{
    if (m_closeflag) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        return IFR_NOT_OK;
    }
    
    IFR_Connection *c=getConnection();
    if (c == 0 || !c->isConnected() || c->isClosed()) {
        error().setRuntimeError(IFR_ERR_SESSION_NOT_CONNECTED);
        return IFR_NOT_OK;
    }
    return IFR_OK;
}

//----------------------------------------------------------------------
IFR_SQLWarning*&
IFR_ConnectionItem::warning()
{
    return m_warning;
}

//----------------------------------------------------------------------
IFR_ConnectionItem::~IFR_ConnectionItem()
{
    clearWarnings();
    m_closeflag = true;
}

//----------------------------------------------------------------------
IFR_PutvalHost *
IFR_ConnectionItem::getPutvalHost()
{
    return 0;
}

//----------------------------------------------------------------------
IFR_GetvalHost *
IFR_ConnectionItem::getGetvalHost()
{
    return 0;
}
