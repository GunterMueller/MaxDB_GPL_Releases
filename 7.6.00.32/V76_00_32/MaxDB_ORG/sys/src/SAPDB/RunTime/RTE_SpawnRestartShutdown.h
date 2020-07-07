/*!
  @file           RTE_SpawnRestartShutdown.h
  @author         JoergM
  @ingroup        Runtime
  @brief          External command call for restart / shutdown

\if EMIT_LICENCE

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



\endif
*/



#ifndef RTE_SPAWNRESTARTSHUTDOWN_H
#define RTE_SPAWNRESTARTSHUTDOWN_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(__cplusplus)
extern "C" 
#else
extern
#endif
/*!
   @brief Spawn an external process to restart / shutdown the database
   @param startOrStop [in] if TRUE a restart is wanted, if FALSE a shutdown is wanted
 */
SAPDB_Bool RTE_SpawnRestartShutdown( SAPDB_Bool startOrStop );

#endif /* RTE_SPAWNRESTARTSHUTDOWN_H */
