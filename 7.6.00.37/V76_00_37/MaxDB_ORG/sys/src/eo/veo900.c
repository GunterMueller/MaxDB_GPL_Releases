/*!
  @file           veo900.c
  @author         RaymondR
  @special area   Process checking
  @brief          Process checking
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


#if !defined ( _WIN32 )
# include <signal.h>
#endif

#include "geo900.h"

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

bool RTEProc_Open( teo001_Pid      Pid,
                         RTEProc_Handle *pHandle )
{
  pHandle->m_Pid = Pid;

# if defined ( _WIN32 )
   pHandle->m_ProcHandle = OpenProcess ( SYNCHRONIZE, FALSE, Pid );
# endif

  return true;
}

/*---------------------------------------------------------------------------*/

bool RTEProc_Close( RTEProc_Handle Handle )
{
# if defined ( _WIN32 )
   if ( Handle.m_ProcHandle != NULL )
     CloseHandle( Handle.m_ProcHandle );
# endif

   return true;
}

/*---------------------------------------------------------------------------*/


RTEProc_State  RTEProc_GetState( RTEProc_Handle Handle )
 {
   RTEProc_State State = UnknownOrInvalid;
   
#  if defined ( _WIN32 )

    if ( Handle.m_ProcHandle == NULL )
    { 
      // --- retry open process
      Handle.m_ProcHandle = OpenProcess ( SYNCHRONIZE, FALSE, Handle.m_Pid );

      if ( Handle.m_ProcHandle   == NULL )
      {
        if ( GetLastError() == ERROR_INVALID_PARAMETER )
          State = Died;
      }
      else
        State = Alive;
    }
    else if( WaitForSingleObject( Handle.m_ProcHandle, 0) != WAIT_TIMEOUT )
      State = Died;
    else
      State = Alive;

#  else  /*  UNIX */

    if ( Handle.m_Pid > 1 )
    {
      if ( kill ( Handle.m_Pid, 0 ) != 0 )
      {
        if ( errno == ESRCH )
           State = Died;
      }
      else
        State = Alive;
    }
#  endif

   return State;
 }


/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/