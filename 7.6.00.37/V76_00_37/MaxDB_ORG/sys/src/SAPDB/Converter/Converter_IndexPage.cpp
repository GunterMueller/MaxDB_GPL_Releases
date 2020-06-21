/*!*****************************************************************************

  module:       Converter_IndexPage.cpp

  ------------------------------------------------------------------------------

  responsible:  TorstenS

  author:       TillL

  special area: Converter

  description:  see .hpp

  see also:     

  Copyright (c) 2000-2004 SAP AG

  ------------------------------------------------------------------------------

  ========== licence begin  GPL
  Copyright (c) 2000-2004 SAP AG

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

*******************************************************************************/


#include "Converter/Converter_IndexPage.hpp"

#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"
#include "RunTime/RTE_Message.hpp"


/*******************************************************************************

   class: Converter_IndexPage

*******************************************************************************/

Converter_IndexPage::PageEntryCount 
Converter_IndexPage::CalcEntryCount() const
{
    SAPDB_UInt hs    = GetHeaderSize();
    SAPDB_UInt ts    = GetTrailerSize();
    SAPDB_UInt as    = GetEntrySize();
    SAPDB_UInt us    = m_Frame.GetLength() - hs - ts;

    SAPDB_UInt count = SAPDB_BITS_PER_BYTE * us / (SAPDB_BITS_PER_BYTE * as + 1);

    if (count % (SAPDB_BITS_PER_BYTE * sizeof(Converter_FlagInfo::FlagInfoData)) != 0)
        count -= 1;
    
    return count;
}
// -----------------------------------------------------------------------------

SAPDB_Bool
Converter_IndexPage::CheckAfterRead() const
{
    if( ! Converter_Page::CheckAfterRead())
        return( SAPDB_FALSE );

    if( Converter_IndexPageType != ReadConverterPageType() )
    {
        Converter_Exception errMsg( __CONTEXT__, CONVERTER_ERR_BAD_INDEX_PAGE_TYPE );
        RTE_Message( errMsg );

        return( SAPDB_FALSE );
    }
    return( SAPDB_TRUE );
}


