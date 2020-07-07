/*!========================================================================

  @file         RTE_GlobalNameSpaceNT.c
  @ingroup      Runtime
  @author       StefanP

  @brief        Building global namespace name 

  @since        2003-10-16  10:42
  @sa           

  ==========================================================================

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
 ============================================================================*/


#if defined (_WIN32)

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "gos00.h"


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


/*===========================================================================*
  *  METHODS                                                                  *
 *===========================================================================*/

externC SAPDB_Bool    RTE_BuildGlobalNameSpaceName 
( 
    SAPDB_Char const * const    name,
    SAPDB_Char * const          globalName, 
    SAPDB_UInt4 * const         pLengthGlobalNameBuffer
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
#define     GLOBAL_NAMESPACE_NAME   "Global\\"

    SAPDB_UInt4 neededLength = strlen (name) + 1; //'+1' cause of the terminating '\0'

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/

    if ( sql02_validate_product_suite( PSUITE_TERMINAL_SERVER ) == true )
    {
        neededLength += strlen (GLOBAL_NAMESPACE_NAME);
        if (*pLengthGlobalNameBuffer < neededLength)
        {
            *pLengthGlobalNameBuffer = neededLength;
            return false;
        }

        strcpy (globalName, GLOBAL_NAMESPACE_NAME);
        strcat (globalName, name);
    }
    else
    {
        if (*pLengthGlobalNameBuffer < neededLength)
        {
            *pLengthGlobalNameBuffer = neededLength;
            return false;
        }

        strcpy (globalName, name);
    }

    return true;
}


#endif  /* NT */
