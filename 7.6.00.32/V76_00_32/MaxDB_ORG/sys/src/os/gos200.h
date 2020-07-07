/*!
  @file           gos200.h
  @author         RaymondR
  @brief          MSCS error handling
  @see            

\if EMIT_LICENCE

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



\endif
*/




#ifndef GOS200_H
#define GOS200_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined( __cplusplus )
 #include <afx.h>
 #include <malloc.h>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if !defined(ERROR_RESOURCE_TYPE_NOT_FOUND)
# define ERROR_RESOURCE_TYPE_NOT_FOUND                         1813L
#endif

#if !defined(ERROR_DEPENDENT_RESOURCE_EXISTS)
# define ERROR_DEPENDENT_RESOURCE_EXISTS                       5001L
#endif

#if !defined(ERROR_DEPENDENCY_ALREADY_EXISTS)
# define ERROR_DEPENDENCY_ALREADY_EXISTS                       5003L
#endif

#if !defined(ERROR_RESOURCE_NOT_FOUND)
# define ERROR_RESOURCE_NOT_FOUND                              5007L
#endif

#if !defined(ERROR_GROUP_NOT_FOUND)
# define ERROR_GROUP_NOT_FOUND                                 5013L
#endif

#if !defined(ERROR_RESOURCE_ONLINE)
# define ERROR_RESOURCE_ONLINE                                 5019L
#endif

#if !defined(ERROR_QUORUM_RESOURCE)
//  The cluster could not make the specified resource a
//  quorum resource because it is not capable of being a 
//  quorum resource.
#define ERROR_QUORUM_RESOURCE                                  5020L
#endif

#if !defined(ERROR_INVALID_STATE)
//  The group or resource is not in the correct state to
//  perform the requested operation.
# define ERROR_INVALID_STATE                                   5023L
#endif

#if !defined(ERROR_RESOURCE_PROPERTIES_STORED)
//  The properties were stored but not all changes will take 
//  effect until the next time the resource is brought online.
# define ERROR_RESOURCE_PROPERTIES_STORED                      5024L
#endif


#if !defined(ERROR_NETWORK_NOT_FOUND)
# define ERROR_NETWORK_NOT_FOUND                               5045L
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GOS200_H */
