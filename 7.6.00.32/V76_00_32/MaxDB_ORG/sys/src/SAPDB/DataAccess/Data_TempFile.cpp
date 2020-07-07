/***************************************************************************/
/*!

  @file        Data_TempFile.cpp
  @author      UweH
  @ingroup     DataAccess
  @brief       This is the implementation of Data_TempFile
  @since       2001-05-12
  @sa          see also ....
*/
/*!
\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


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
#include "DataAccess/Data_TempFile.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*---------------------------------------------------------------------------*/
Data_TempFile::Data_TempFile ( tgg00_TransContext &trans,
                               const SAPDB_UInt4   spaceSize )
: Data_BaseFile ( Data_FixSizeTempFile,
                  Data_PageId(Data_PageNo(), Data_PageRecoveryMode(Data_Dynamic,Data_NotRecoverable)),
                  Data_PageAccessManager(trans, Data_FixSizeTempFile, 
                  Data_PageRecoveryMode(Data_Dynamic,Data_NotRecoverable), Data_PageNo()) ),
  m_Container   (m_PageAccessManager, m_RootId, Data_PageNo(), spaceSize)
{}

/*---------------------------------------------------------------------------*/
Data_TempFile::Data_TempFile ( const Data_TempFile& newFile )
: Data_BaseFile ( newFile.m_Type, newFile.m_RootId, newFile.m_PageAccessManager ),
  m_Container   ( m_PageAccessManager, m_RootId, Data_PageNo(), newFile.GetSpaceSize() )
{}

/*---------------------------------------------------------------------------*/
bool Data_TempFile::Create(SAPDBErr_MessageList &errlist)
{
    if ( IsCreated() )
    {
        WriteToTrace("Already created");
        return false;
    }
    m_PageAccessManager.Invalidate();
    m_RootId.Invalidate();
    PageIterator RootPageIter (m_PageAccessManager);
    return m_Container.Create(RootPageIter,errlist);
}

/*---------------------------------------------------------------------------*/
bool Data_TempFile::Drop(SAPDBErr_MessageList &errlist)
{
    if(!m_Container.Drop(errlist))
        return false;
    m_RootId.Invalidate();
    return true;
}

/*---------------------------------------------------------------------------*/
bool Data_TempFile::Verify (bool                  isCold,
                            SAPDBErr_MessageList &errlist)
{
    // ++++++
    return true;
}

/*---------------------------------------------------------------------------*/
bool Data_TempFile::Append (const SAPDB_Byte     *space,
                            SAPDBErr_MessageList &errlist )
{
    if ( space == 0 )
        return false;
        
    RecordIterator iter (PageAccessManager());

    if ( ! m_Container.ReserveSpace (iter,errlist,false) )
        return false;

    if ( ! iter.IsValid() )
        return false;

	(void) SAPDB_MemCopyNoCheck ( *iter, space, GetSpaceSize() );

    if(!iter.Invalidate (errlist))
        return false;

    if(!m_Container.UnLock(errlist))
        return false;

    return true;
}

/*---------------------------------------------------------------------------*/
bool Data_TempFile::GetFirst ( RecordIterator       &iter,
                               SAPDBErr_MessageList &errlist )
{
    return m_Container.Begin (iter, Data_ForRead, errlist)
           &&
           iter.IsValid();
}

