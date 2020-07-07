/*!
  @file           geo900.h
  @author         RaymondR
  @special area   Process checking
  @brief          Process checking
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



#ifndef GEO900_H
#define GEO900_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include  "geo00.h"
#include  "geo001.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

typedef enum   
{ 
  UnknownOrInvalid = 1,
  Alive,
  Died 
} RTEProc_State;

typedef struct 
{
# if defined (_WIN32)
   HANDLE             m_ProcHandle;
# endif
  teo001_Pid          m_Pid;
} RTEProc_Handle;

/*===========================================================================*
 *  PROTOTYPES                                                               *
 *===========================================================================*/


externC  bool            RTEProc_Open    ( teo001_Pid     Pid,
                                           RTEProc_Handle *pHandle );

externC  bool            RTEProc_Close   ( RTEProc_Handle Handle );

externC  RTEProc_State   RTEProc_GetState( RTEProc_Handle Handle );

#endif  /* GEO900_H */
