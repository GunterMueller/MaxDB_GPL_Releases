/*!**********************************************************************

  module: hwd12hpool.h

  -----------------------------------------------------------------------

  responsible:  Holger Bischoff
				Markus Oezgen

  special area: WebAgent

  description:  Handle pool

  see also:     WebAgent user manual

  -----------------------------------------------------------------------

  copyright:               Copyright (c) 2000-2005 SAP AG



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




************************************************************************/

#ifndef HWD12HPOOL_H
#define HWD12HPOOL_H

/*=======================================================================
 *  INCLUDES
 *=====================================================================*/

#include "gwd00.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*!**********************************************************************

  Chapter: twd12HandlePoolP

  description:
  A twd12HandlePoolP can be used to store frequently used handles.
  The pool manages construction and destruction of handles. The pool
  only knows the constructor- and the destructor-function of the handle.
  The pool may be used in a multithreaded environment because the access
  is serialized through semaphores. Handles are only freed on destruction
  of the whole pool. There is no shrinkage mechanism (yet).
  
************************************************************************/

typedef struct st_wa_handle_pool *twd12HandlePoolP;

typedef void* (*twd12Constructor)();

typedef void  (*twd12Destructor)( void *handle );

/*!**********************************************************************
  Function: wd12CreateHandlePool

  description: Create new handle-pool.

  arguments:
    contructor  [in]  The constructor function to create a new handle.
    destructor  [in]  The destructor function to destroy the handle.
    
  return value: The new handle pool.
  */
twd12HandlePoolP wd12CreateHandlePool( twd12Constructor  constructor,
                                       twd12Destructor   destructor );

    
/*!**********************************************************************
  Function: wd12DestroyHandlePool

  description: 
  Call destructor function for every handle in the pool and destroy the
  pool.
  
  arguments:
    hpool  [in]  The pool to destroy.
    
  return value:
  */    
void wd12DestroyHandlePool( twd12HandlePoolP hpool );

    
/*!**********************************************************************
  Function: wd12GetHandle
  
  description: 
  Return a handle from the pool. If the pool is empty a new handle is
  created with the constructor function.
  
  arguments:
    hpool  [in]  The handle pool.
    
  return value: The requested handle.
  */
void *wd12GetHandle( twd12HandlePoolP hpool );

    
/*!**********************************************************************
  Function: wd12PutHandle

  description: 
  Put a handle back to the pool.
  
  arguments:
    hpool  [in]  The handle pool.
    handle [in]  The handle that should be stored in the pool.
    
  return value:
  */    
void wd12PutHandle( twd12HandlePoolP hpool, void *handle );
    
/*!**********************************************************************

  EndChapter: twd12HandlePoolP

************************************************************************/

#if defined(__cplusplus)
}
#endif

#endif
