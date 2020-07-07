/*!
  @ingroup        XML_XPathIndex
  @file           XMLXPath_Memory.h
  @author         FerdiF
  @brief          memory allocation / deallocation
*/
/*

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



*/

#ifndef XMLXPATH_MEMORY_H
#define XMLXPATH_MEMORY_H

#include <stdlib.h>

/*!
   define XMLXPath_Mem_Context
*/
typedef struct
{
    SAPDB_UInt     size;        /*!< size of allocated memory */
    char           filler[12];  /*!< ???                      */
} XMLXPath_Mem_Context;

/*!
   define a pointer for XMLXPath_Mem_Context
*/
typedef XMLXPath_Mem_Context *XMLXPath_Mem_ContextPtr;

/*!
   @brief     allocate memory
   @param     size [in]     size of memory to allocate
   @return    ptr to allocated memory or NULL if error
 */
void* XMLXPath_Mem_Malloc(size_t  size);

/*!
   @brief     free memory allocated by XMLXPath_Mem_Malloc
   @param     p [in]     ptr to allocated memory
 */
void  XMLXPath_Mem_Free (void* p);

/*!
   @brief     resize already allocate memory
   @param     p [in]     ptr to already allocated memory
   @param     size [in]  size of memory to return
   @return    ptr to re-allocated memory or NULL if error
 */
void* XMLXPath_Mem_Realloc(void *p, size_t size);

#endif /* XMLXPATH_MEMORY_H */
