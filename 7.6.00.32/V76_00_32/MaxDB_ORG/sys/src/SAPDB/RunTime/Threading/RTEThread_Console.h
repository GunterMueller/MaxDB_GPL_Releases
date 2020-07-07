/*!
  @file           RTEThread_Console.h
  @author         StefanP
  @special area   Console
  @brief          Console Thread
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef RTETHREAD_CONSOLE_H
#define RTETHREAD_CONSOLE_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define RTE_CONSOLE_SHM_SIZE                (64 * 1024)
#define RTE_CONSOLE_SHM_SIZE_AWE            (1024 * 1024)
#define RTE_CONSOLE_TIMEOUT                 30000       /* milliseconds */
/*#define RTE_CONSOLE_TIMEOUT               300000*/
/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*! @name extern "C" Functions */
 /*@{*/

/*!
   @description    Console Thread Main Function

 */
 

externC void *    RTEThread_Console (void *);

/*@}*/

#endif  /* RTETHREAD_CONSOLE_H */
