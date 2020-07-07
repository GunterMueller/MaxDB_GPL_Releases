/*****************************************************************************/
/*!
  @file         FileDir_FileNo.cpp
  @author       MartinKi
  @ingroup      FileDir_FileNo_Files

  @brief        Implementation file for classes FileDir_FileNo and
                FileDir_FileNoToString.

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
/*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "FileDirectory/FileDir_FileNo.hpp"

#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_OStream.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_Types.hpp"

#include <string.h>

/*==========================================================================*
 *  DEFINES                                                                 *
 *==========================================================================*/

/// Character string holding all hex digits.
static const SAPDB_Char hexDigitsC [] = "0123456789abcdef";

char FileDir_FileNo::tempFileNoMaskByte = '\x80';

/*==========================================================================*
 *  MACROS                                                                  *
 *==========================================================================*/

/*==========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                  *
 *==========================================================================*/

/**************************************************************************/

int FileDir_FileNo::HexString(char* buffer, int bufLength) const
{
    int  bufpos = 0;
    for (int i = 0; i < 8; ++i) {
        if ( bufpos+2 > bufLength ) {
            break;
        }
        buffer[bufpos++] = hexDigitsC [
            static_cast<unsigned char>(m_fileNo.charArray[i]) / 16 ];
        buffer[bufpos++] = hexDigitsC [
            static_cast<unsigned char>(m_fileNo.charArray[i]) % 16 ];
    }
    buffer[ bufpos ] = '\0';
    return bufpos;
}

/**************************************************************************/

SAPDB_OStream& operator<<(SAPDB_OStream& os, const FileDir_FileNo& fileNo)
{
    os << FileDir_FileNoToString( fileNo );
    return os;
}

/**************************************************************************/
/**************************************************************************/

FileDir_FileNoToString::FileDir_FileNoToString(const FileDir_FileNo& fileNo)
{
    fileNo.HexString( m_buffer, FileDir_FileNo::c_hexBufLength );
}

/**************************************************************************/
