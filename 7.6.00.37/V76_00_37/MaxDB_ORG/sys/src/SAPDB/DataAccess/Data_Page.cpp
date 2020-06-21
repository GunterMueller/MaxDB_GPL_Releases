/*!***************************************************************************

  module      : Data_Page.cpp
  responsible : UweH
  special area: DataAccess
  created     : 2002-09-06
  Copyright (c) 2000-2005 SAP AG
  description : defines class Data_Page



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

#include "DataAccess/Data_Page.hpp"
#include "DataAccess/Data_Types.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "RunTime/RTE_Message.hpp"
#include "hbd06.h" // b06dump_bad_page
#include "heo51.h" // vgetpid

//--------------------------------------------------------------------------------
Data_Page::~Data_Page()
{
    SAPDBErr_MessageList errlist;
    if ( ! Deassign(errlist) )
        RTE_Message(errlist);
}

//--------------------------------------------------------------------------------
void Data_Page::WriteToTrace (const SAPDB_Char *title) const
{
    Kernel_VTrace trace;
    
    if ( title != NULL )
        trace << title;
    else
        trace << "Data_Page: ";

    if ( IsAssigned() )
        trace << "PageNo: " << PageNo()
              << ", Root: "   << RootPageNo()
              << ", Next: "   << Successor()
              << ", Bottom: " << NodePtr()->nd_bottom()
              << NewLine;
    else
        trace << "not assigned." << NewLine;
        
    trace << "PageNo: " << PageNo()
          << ", AdressMode: " << Data_AddressingModeStrings  [RecoveryMode().GetAddressingMode()]
          << ", RecoverMode: " << Data_RecoverabilityStrings [RecoveryMode().GetRecoverability()]
          << ", AccessMode: "  << Data_AccessModeStrings     [m_AccessMode]
          << ", ReleaseMode: " << Data_ReleaseModeStrings    [m_ReleaseMode]
          << ", LRUInfo: "     << Data_LRUChainInfoStrings   [m_LruChainInfo]
          << NewLine;
}

//--------------------------------------------------------------------------------
void Data_Page::AppendContextInfo (SAPDB_Char  const * const  filename,
                                   SAPDB_UInt4 const          linenumber,
                                   SAPDBErr_MessageList      &errlist,
                                   SAPDB_Char  const * const  title) const
{
    if ( IsAssigned() )
    {
        errlist = errlist + 
              Data_Exception( filename,linenumber, DATA_PAGE_INFO,
                              title==NULL?"Data_Page":title,
                              SAPDB_ToString(PageNo()),
                              Data_AddressingModeStrings [RecoveryMode().GetAddressingMode()],
                              Data_RecoverabilityStrings [RecoveryMode().GetRecoverability()],
                              Data_AccessModeStrings     [m_AccessMode],
                              Data_ReleaseModeStrings    [m_ReleaseMode],
                              SAPDB_ToString(RootPageNo()) );
    }
    else
    {
        errlist = errlist + 
              Data_Exception( filename,linenumber, DATA_PAGE_NOT_ASSIGNED,
                              title==NULL?"Data_Page":title );
    }
}

//--------------------------------------------------------------------------------
bool Data_Page::Verify (bool                  isAdmin,
                        SAPDBErr_MessageList &errlist) const
{
    return true;
}

//--------------------------------------------------------------------------------
void Data_Page::Dump() const
{
    if ( m_pAccessManager != 0 )
        m_pAccessManager->WriteBadFileInfo();

    tsp00_TaskId taskid;

    vgetpid(taskid);

    b06dump_bad_page (taskid, 'p', FILE_EXT_COR_BD00, GetPageID(), const_cast<tbd_nodeptr>(NodePtr()), 1);
}
