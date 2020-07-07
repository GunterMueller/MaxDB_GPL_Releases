/*!***************************************************************************

  module      : RTE_Crash.cpp

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: RTE
  description : Crash Message output and crash call


  last changed: 2001-01-11  17:59
  see also    : 

  -------------------------------------------------------------------------

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




*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_Message.hpp"
#include "Messages/Msg_List.hpp"
#include "geo007_1.h"
#include "geo60.h"
#if defined(_WIN32)
extern "C" void _vabort(bool writeCore, SAPDB_ULong line, const SAPDB_Char *file);
#else
extern "C" void vabort(bool writeCore);
#endif /* WIN32 */
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

static const SAPDB_Char *FindFilename(const SAPDB_Char *fileName)
{
    const SAPDB_Char *lastChar = fileName;
    const SAPDB_Char *lastEntry = fileName;

    while ( 0 != *lastChar )
    {
#if defined(_WIN32)
        if ( *lastChar == '\\' || *lastChar == '/' )
        {
            do
            {
                ++lastChar;
            } while ( *lastChar == '\\' || *lastChar == '/' );

            if ( *lastChar ) lastEntry = lastChar; 
        }
#else
        if ( *lastChar == '/' )
        {
            do
            {
                ++lastChar;
            } while ( *lastChar == '/' );

            if ( *lastChar ) lastEntry = lastChar; 
        }
#endif
        ++lastChar;
    }

    return lastEntry;
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void RTE_Crash( const Msg_List &lastWords )
{
    RTE_Message(lastWords, MSG_DIAG_CONSOLE);

#if defined(_WIN32)
    if ( lastWords.IsEmpty() )
    {
        _vabort( true, __LINE__, FindFilename(__FILE__) );
    }
    else
    {
        _vabort( true, lastWords.LineNumber(), FindFilename(lastWords.FileName()) );
    }
#else
    if ( lastWords.IsEmpty() )
    {
        MSGD(( ERR_KERNEL_VABORT_EXEPT_CHK, FindFilename(__FILE__), __LINE__ ));
    }
    else
    {
        MSGD(( ERR_KERNEL_VABORT_EXEPT_CHK, FindFilename(lastWords.FileName()), lastWords.LineNumber() ));
    }
    vabort( true );
#endif /* _WIN32 */
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/