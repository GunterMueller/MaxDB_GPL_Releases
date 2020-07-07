/*!
  @file           geo00_3.h
  @author         RaymondR
  @special area   
  @brief          
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



#ifndef GEO00_3_H
#define GEO00_3_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define TIME_VAL_ARRAY_SIZE             3
#define NUM_OF_TASK_MOVE_STAT_RECS      10

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

#define TASK_MOVE_TIME_COMP_EQUAL        0
#define TASK_MOVE_TIME_COMP_GREATER      1
#define TASK_MOVE_TIME_COMP_LESS        -1

/* --- The time between the '_oldtime' and '_newtime' is treated as equal if the
       difference is within a '_percent' percent range. */
#define TASK_MOVE_TIME_COMP(_oldtime, _newtime, _percent)                                        \
          (_oldtime > _newtime ? ((_oldtime - _newtime) > ((_oldtime*_percent)/100) ? -1 : 0)  : \
                                 ((_newtime - _oldtime) > ((_oldtime*_percent)/100) ?  1 : 0) )



#define PREV_TIME_VAR(_var)  (_var == 0 ? TIME_VAL_ARRAY_SIZE - 1 : _var - 1)
#define NEXT_TIME_VAR(_var)  (_var == TIME_VAL_ARRAY_SIZE - 1 ? 0 : _var + 1)

#define PREV_ENTRY(_var, _max)  (_var == 0 ? _max - 1 : _var - 1)
#define NEXT_ENTRY(_var, _max)  (_var == _max - 1 ? 0 : _var + 1)


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#endif  /* GEO00_3_H */
