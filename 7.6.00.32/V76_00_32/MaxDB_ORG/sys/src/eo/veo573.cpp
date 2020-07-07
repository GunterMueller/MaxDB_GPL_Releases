/*!
  @file           veo573.cpp
  @author         RaymondR
  @special area   Global scope new and delete.
  @brief          Global scope new and delete.
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "geo570.h"
#include "geo573.h"

#if defined(AIX)
# include <new>
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void *operator new ( size_t    Size ) THROW_NEW_GEO573
{ 
  return teo570_GlobalAlloc::eo570_Allocate(Size,__FILE__,__LINE__); 
};

void operator delete ( void * Buffer ) THROW_DELETE_GEO573
{
  if ( Buffer != NULL) teo570_GlobalAlloc::eo570_Deallocate(Buffer,__FILE__,__LINE__);
};

void *operator new[] ( size_t Size ) THROW_NEW_GEO573
{
  return teo570_GlobalAlloc::eo570_Allocate(Size,__FILE__,__LINE__); 
}

void operator delete[]  ( void * Buffer ) THROW_DELETE_GEO573
{
  if ( Buffer != NULL) teo570_GlobalAlloc::eo570_Deallocate(Buffer,__FILE__,__LINE__);
}

#if defined (_WIN32) 
void *__cdecl operator new(size_t Size, const std::nothrow_t& ) throw()
{
  char *p;
  try 
  {
      p = (char *)teo570_GlobalAlloc::eo570_Allocate(Size,__FILE__,__LINE__); 
  }
  catch (std::bad_alloc)
  {
      p = NULL;
  }
 return p;
} 

void *__cdecl operator new[](size_t Size, const std::nothrow_t& ) throw()
{
  char *p;
  try 
  {
      p = (char *)teo570_GlobalAlloc::eo570_Allocate(Size,__FILE__,__LINE__); 
  }
  catch (std::bad_alloc)
  {
      p = NULL;
  }
 return p;
} 

void __cdecl operator delete( void * Buffer, const std::nothrow_t& ) throw()
{
  if ( Buffer != NULL) teo570_GlobalAlloc::eo570_Deallocate(Buffer,__FILE__,__LINE__);
}

void __cdecl operator delete[]( void * Buffer, const std::nothrow_t& ) throw()
{
  if ( Buffer != NULL) teo570_GlobalAlloc::eo570_Deallocate(Buffer,__FILE__,__LINE__);
}
#endif

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/