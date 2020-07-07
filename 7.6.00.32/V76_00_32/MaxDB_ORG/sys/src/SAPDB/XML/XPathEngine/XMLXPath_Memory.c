/*!***************************************************************************

  module      : XMLXPath_Memory.c

  -------------------------------------------------------------------------

  responsible : FerdiF

  special area: XML

  description : memory allocation / deallocation
 
  last changed: 2001-05-31  18:24
  
  see also    : 

  -------------------------------------------------------------------------

  Copyright (c) 2000-2005 SAP AG



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




*****************************************************************************/
#include <string.h>
#include "SAPDB/SAPDBCommon/SAPDB_Types.h"
#include "SAPDB/XML/XPathEngine/XMLXPath_Memory.h"
#include "SAPDBCommon/SAPDB_string.h"

/*#define MEM_SIZE_CHECK*/

#if defined(MEM_SIZE_CHECK)
static int g_memory_act_used = 0;
static int g_memory_max_used = 0;
#endif

/*------------------------------------------------------------------------------*/
void* 
XMLXPath_Mem_Malloc( size_t  size )
{
#if defined(MEM_SIZE_CHECK)
    XMLXPath_Mem_ContextPtr pCtx = malloc( size + sizeof( XMLXPath_Mem_Context ));
    if ( pCtx )
    {
        g_memory_act_used += size;
        if ( g_memory_act_used > g_memory_max_used )
            g_memory_max_used = g_memory_act_used;
        pCtx->size = size;
        ++pCtx;
    }
    return pCtx;
#else
  return malloc( size );
#endif
}

/*------------------------------------------------------------------------------*/
void  
XMLXPath_Mem_Free ( void*  p )
{
#if defined(MEM_SIZE_CHECK)
    XMLXPath_Mem_ContextPtr pCtx = p;
    --pCtx;
    g_memory_act_used -= pCtx->size;
    free( pCtx );
#else
    free( p );
#endif
}

/*------------------------------------------------------------------------------*/
void* 
XMLXPath_Mem_Realloc( void *p, size_t sz )
{
#if defined(MEM_SIZE_CHECK)
    int old_sz;
    XMLXPath_Mem_ContextPtr memCtx = p;
    void *pNew;

    --memCtx;
    pNew = XMLXPath_Mem_Malloc( sz );
    old_sz = memCtx->size;
    if ( memCtx->size > sz ) old_sz = sz;
    SAPDB_memcpy( pNew, p, old_sz );
    XMLXPath_Mem_Free( p );
    return pNew;
#else
    return realloc( p, sz );
#endif
}
