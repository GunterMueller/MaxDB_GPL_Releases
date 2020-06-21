/*!**************************************************************************

  module      : hbd95_1.h

  -------------------------------------------------------------------------

  responsible : d025251

  special area: auxiliary function for the roll forward
  description : 


  last changed: 2001-05-30  13:05
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


#ifndef HBD95_1_H
#define HBD95_1_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "gbd910.h"
#include "liveCache/LVC_ErrorTraceObject.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!***************************************************************************
function   : bd95RemovePageFromObjFile

description: This function removes a page from an object file and sets the page free

arguments  : Trans                     [in/out]  TransContext   
             NptrsPage                 [in]      pointer to the page to release

return value: none
  
******************************************************************************/

externCpp void
bd95RemovePageFromObjFile (tgg00_TransContext       &Trans,
                           const Converter_Version  &converterVersion,
						   cbd910_ObjDataNode       &NptrsPage);


/*!***************************************************************************
function   : bd95RemoveObjFile

description: This function removes an obejct file

arguments  : Current                   [in/out]     

return value: none
  
******************************************************************************/

externCpp void
bd95RemoveObjFile (cbd05_Current  &Current);

#endif  /* HBD95_1_H */
