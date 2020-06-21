/*!========================================================================
  
  @file         RTE_GlobalNameSpaceNT.h
  @author       StefanP
  @ingroup      Runtime
  @brief        Building global namespace name
                

  @since        2003-10-17  11:16
  @sa           

  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTE_GLOBALNAMESPACENT_H
#define RTE_GLOBALNAMESPACENT_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_CompilerFeatures.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
/*!
  @brief		Build global name space name
				If the given buffer length '*pLengthGlobalNameBuffer' is not sufficient to hold the resulting name 
				(globalName) the function returns 'false' and pLengthGlobalNameBuffer holds the length needed
				to hold the resulting name.
				This behavior can be used to call the function first with '*pLengthGlobalNameBuffer' set to 0 and
				allocate memory for 'globalName' as needed afterwards. A second call to this function than succeed
				with the global name.

  @param	    name [in]							-	Name to be converted
  @param	    globalName [out]					-	Name converted (if necessary (e.g Terminal Server), else given name)
  @param	    pLengthGlobalNameBuffer [in/out]	-	Length of the given buffer for the global name (in)
														Length of the resulting globalName including terminating '\0' (out)
  @return		true/false (see above)	
*/

externC SAPDB_Bool    RTE_BuildGlobalNameSpaceName     (SAPDB_Char const * const   name,
														SAPDB_Char * const         globalName, 
														SAPDB_UInt4 * const        pLengthGlobalNameBuffer);
#endif  /* RTE_GLOBALNAMESPACENT_H */
