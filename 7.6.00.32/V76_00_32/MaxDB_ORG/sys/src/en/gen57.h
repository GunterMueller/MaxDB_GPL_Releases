/*!
  @file           gen57.h
  @author         JoergM
  @special area   internal kernel malloc / free routines
  @brief          to be able to locate memory consumer these routines are
              able to protocol to calling module if DEBUG_MEM_ALLOC
              is enabled.
  @see            example.html ...

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



#ifndef GEN57_H
#define GEN57_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo00.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#define MEMORY_ALIGNMENT_EN57 ((long)16)

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/*
 * WARNING: Double implementation in ven17c and ven57c to keep
 * kernel LZU and user LZU separated! Both use same name and calling convention
 */
externC teo00_Int4 sql57k_pmalloc(const unsigned long uLineNumber, const char *pModuleName, 
                    void **pPtr, size_t size);

externC teo00_Int4 sql57k_prealloc(const unsigned long uLineNumber, const char *pModuleName, 
                    void **pPtr, size_t size);

externC teo00_Int4 sql57k_pfree(const unsigned long uLine, const char *pModuleName, 
                  void *ptr);

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEN57_H */
