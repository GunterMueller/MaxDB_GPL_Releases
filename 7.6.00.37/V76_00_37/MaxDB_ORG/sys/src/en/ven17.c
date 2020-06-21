/*!---------------------------------------------------------------------
  @file           ven17.c
  @author         JoergM
  @brief          User storage allocation: sqlallocate/sqlfree

  first created  2000-04-20 17:24
\if EMIT_LICENCE
\endif
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
---------------------------------------------------------------------*/


#define MOD__ "ven17c:"

#include "gen00.h"
#include "geo00.h"
#include "geo001.h"
#include "geo57.h"
#include "geo007_1.h"
#include "hen40.h"

/* exported functions */

externC  void sqlallocat ( tsp00_Int4    sizeInBytes,
                           UCHAR     **objPointer ,
                           tsp00_Bool *ok );
externC  void sqlreallocat ( tsp00_Int4     sizeInBytes , 
                             UCHAR      **objPointer ,
                             tsp00_Bool  *ok );
externC	void sqlfree ( UCHAR *objPointer );

externC teo00_Int4 sql57k_pmalloc( const unsigned long uLineNumber, 
				   const char *pModuleName, 
				   void **pPtr,
				   size_t sizeInBytes);

externC teo00_Int4 sql57k_prealloc( const unsigned long uLineNumber, 
				    const char *pModuleName, 
				    void **pPtr, 
				    size_t sizeInBytes);

externC teo00_Int4 sql57k_pfree( const unsigned long uLineNumber, 
				 const char *pModuleName, 
				 void *ptr);
/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sqlallocat"
externC	void	sqlallocat ( 
tsp00_Int4   sizeInBytes,
UCHAR      **objPointer ,
tsp00_Bool *ok )
{
  DBGPAS;

  *ok = (sql57k_pmalloc(__LINE__,__FILE__,(void **)objPointer, sizeInBytes)
        == NO_ERROR_EO001);
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sqlreallocat"
externC	void	sqlreallocat ( 
tsp00_Int4   sizeInBytes ,
UCHAR      **objPointer ,
tsp00_Bool *ok )
{
  DBGPAS;

  *ok = (sql57k_prealloc(__LINE__,__FILE__,(void **)objPointer, sizeInBytes)
      == NO_ERROR_EO001);
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sqlfree"
externC	void	sqlfree ( 
UCHAR *objPointer )
{
  DBGPAS;

  (void)sql57k_pfree(__LINE__,__FILE__,objPointer);
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sqlvalidatealloc"
void	sqlvalidatealloc ( 
tsp00_Uint1*    Buffer,
tsp00_BoolAddr  Ok )
{
  DBGPAS;

  /* Dummy: Control structures are ALWAYS ok ;-)... */
  *Ok = ((1+1)==2);
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql57k_pmalloc"
externC teo00_Int4 sql57k_pmalloc(
const unsigned long uLineNumber, 
const char *pModuleName, 
void **pPtr,
size_t sizeInBytes)
{
  void *ptr;

  DBGIN;

  if ( !pPtr )
  {
    MSGD (( ERR_MALLOC_SYS_ERR, uLineNumber, pModuleName, 
			sizeInBytes, "NULL pointer argument" ));
    return EFAULT;
  }
  ptr = (void *)malloc(sizeInBytes);
  if ( !ptr )
  {
    MSGD (( ERR_MALLOC_SYS_ERR, uLineNumber, pModuleName, 
			sizeInBytes, sqlerrs() ));
    MSGD (( INFO_END_OF_DATA_SECTION , sbrk(0) ));
  }
#if defined(DEBUG_MEM_ALLOC)
  MSGD(( 11999, INFO_TYPE, "ALLOC   ", "'%4ld':'%s' - 0x%lx, sizeInBytes %ld", 
                uLineNumber, pModuleName, ptr, sizeInBytes));
#endif /* DEBUG_MEM_ALLOC */
  *pPtr = ptr;

  DBGOUT;

  return ( ptr != NULL ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql57k_prealloc"
externC teo00_Int4 sql57k_prealloc(
const unsigned long uLineNumber, 
const char *pModuleName, 
void **pPtr, 
size_t sizeInBytes)
{
  void *ptr;

  DBGIN;

  if ( !pPtr )
  {
    MSGD (( ERR_MALLOC_SYS_ERR, uLineNumber, pModuleName, 
			sizeInBytes, "NULL pointer argument" ));
    return EFAULT;
  }
  ptr = (void *)realloc(*pPtr, sizeInBytes);
  if ( !ptr )
  {
    MSGD (( ERR_REALLOC_SYS_ERR, uLineNumber, pModuleName, 
			sizeInBytes, sqlerrs() ));
    MSGD (( INFO_END_OF_DATA_SECTION , sbrk(0) ));
  }
#if defined(DEBUG_MEM_ALLOC)
  MSGD(( 11999, INFO_TYPE, "REALLOC ", 
		"'%4ld':'%s' - (0x%lx)0x%lx, sizeInBytes %ld", 
     uLineNumber, pModuleName, *pPtr, ptr, sizeInBytes));
#endif /* DEBUG_MEM_ALLOC */
  *pPtr = ptr;

  DBGOUT;

  return ( ptr != NULL ? NO_ERROR_EO001 : ERROR_NOT_ENOUGH_MEMORY_EO001 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"sql57k_pfree"
externC teo00_Int4 sql57k_pfree(
const unsigned long uLineNumber, 
const char *pModuleName, 
void *ptr)
{
  DBGIN;

#if defined(DEBUG_MEM_ALLOC)
  MSGD(( 11999, INFO_TYPE, "FREE    ", "'%4ld':'%s' - 0x%lx", 
                uLineNumber, pModuleName, ptr));
#endif /* DEBUG_MEM_ALLOC */
  if ( ptr ) 
  {
    free(ptr);
  }

  DBGOUT;

  return NO_ERROR_EO001;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

