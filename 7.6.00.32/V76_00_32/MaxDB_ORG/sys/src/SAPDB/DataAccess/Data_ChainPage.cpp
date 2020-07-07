/*!***************************************************************************

  module      : Data_ChainPage.cpp
  responsible : UweH
  special area: DataAccess
  created     : 2004-01-16
  copyright   : Copyright by SAP AG, 2000
  description : implements methods of class Data_ChainPage

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

#include "DataAccess/Data_ChainPage.hpp"
#include "DataAccess/Data_Messages.hpp"
#include "DataAccess/Data_Exceptions.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"

//--------------------------------------------------------------------------------
bool Data_ChainPage::Remove (Data_ChainPage       &prevPage,
                             SAPDBErr_MessageList &errlist)
{
    if ( prevPage.IsAssigned() )
        prevPage.SetNextPageNo(NextPageNo());

    if ( ! m_pAccessManager->FreePage (*this, errlist) )
    {
        AppendContextInfo(__CONTEXT__,errlist, "m_pAccessManager->FreePage() failed");
        return false;
    }

    return true;
}
//--------------------------------------------------------------------------------
bool Data_ChainPage::Append (Data_ChainPage&       NewPage,
                             SAPDBErr_MessageList &errlist)
{
    if ( ! IsAssigned() )
    {
        errlist = Data_Exception(__CONTEXT__, DATA_COULD_NOT_APPEND_NEW_PAGE);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    if ( ! m_pAccessManager->NewPage (NewPage, RecoveryMode(), errlist) )
    {
        errlist = errlist + Data_Exception(__CONTEXT__, DATA_COULD_NOT_APPEND_NEW_PAGE);
        AppendContextInfo(__CONTEXT__,errlist);
        return false;
    }

    NewPage.SetNextPageNo ( NextPageNo()     );
    NewPage.SetRootPageNo ( RootPageNo()     );
    SetNextPageNo         ( NewPage.PageNo() );
    return true;
}
//--------------------------------------------------------------------------------
void Data_ChainPage::AppendContextInfo (SAPDB_Char  const * const  filename,
                                        SAPDB_UInt4 const          linenumber,
                                        SAPDBErr_MessageList      &errlist,
                                        SAPDB_Char  const * const  title) const
{
    Data_Page::AppendContextInfo (filename,linenumber,errlist, title);
    if ( ! IsAssigned() )
        return;
    errlist = errlist + 
              Data_Exception( filename,linenumber, DATA_CHAIN_PAGE_INFO,
                              title==NULL?"Data_ChainPage":title,
                              SAPDB_ToString(NextPageNo()),
                              SAPDB_ToString(LastPageNo()) );
}
