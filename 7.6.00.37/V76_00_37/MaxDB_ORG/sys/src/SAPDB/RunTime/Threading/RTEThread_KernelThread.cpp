/*!========================================================================

  @file         RTEThread_KernelThread.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel thread base class.

                The following class allows to create and manage kernel threads.

  @since        2005-01-12  16:12
  @sa           

  ==========================================================================

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Threading/RTEThread_KernelThread.hpp"
#include    "geo50_0.h"


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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

#if defined (_WIN32)    

 DWORD RTEThread_KernelThread::ExceptionFilter( LPEXCEPTION_POINTERS pExceptPtrs ) 
 { 
     return sql50k_excepthandler( pExceptPtrs );
 }

 /*---------------------------------------------------------------------------*/

 SAPDB_Int4 RTEThread_KernelThread::ExceptionHandler() 
 { 
     return 1; 
 };
#endif

/*---------------------------------------------------------------------------*/

SAPDB_Int4 RTEThread_KernelThread::ThreadMain()
{
    SAPDB_Int4    returnCode = 0;

    m_ThreadHasStated = true;

    returnCode  = KernelThreadMain();
    
    return returnCode;
}


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/