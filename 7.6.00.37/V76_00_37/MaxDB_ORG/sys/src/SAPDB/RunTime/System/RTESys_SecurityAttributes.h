/*!========================================================================

  @file         RTESys_SecurityAttributes.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        This paragraph appears in overviews.

                The rest of the description appears as detailed description

  @since        2005-09-23  11:51
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/


#ifndef RTESYS_SECURITYATTRIBUTES_H
#define RTESYS_SECURITYATTRIBUTES_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"

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

#if defined(__cplusplus)
extern "C" {
#endif


#if defined (_WIN32)
/*! @brief get operator and admin security attribute
    @param  none
    @return none */
externC RTE_SECURITY_ATTRIBUTES *  GetOperatorAdminSA();

#endif



#if defined(__cplusplus)
}
#endif /* __cplusplus */


#endif  /* RTESYS_SECURITYATTRIBUTES_H */
