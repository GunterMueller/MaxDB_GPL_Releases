/*!========================================================================

  @file         RTESys_ProcessState.cpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Check if a process is dead or alive

                Determine the state of a process specified by given 
                process id.

  @since        2004-03-23  14:35
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

#include "RunTime/System/RTESys_ProcessState.hpp"

#if !defined ( _WIN32 )
# include <signal.h>
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
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTESys_ProcessState::RTESys_ProcessState( RTE_OSPid pid ) 
                     : m_Pid(0),
                       m_OpenState(Unknown)
#                     if defined ( _WIN32 )
                       ,m_ProcHandle(0)
#                     endif
{
    if ( pid )
        Open ( pid );
}

/*---------------------------------------------------------------------------*/

RTESys_ProcessState::~RTESys_ProcessState()
{
    Close ();
}

/*---------------------------------------------------------------------------*/

void    RTESys_ProcessState::Open ( RTE_OSPid pid ) 
{
    Close(); // try close old process handle

#  if defined ( _WIN32 )
    // Try open process
    m_ProcHandle = OpenProcess ( SYNCHRONIZE, FALSE, pid );

    if ( NULL == m_ProcHandle )
    {
        if ( GetLastError() == ERROR_INVALID_PARAMETER )
            m_OpenState = Died;      // process not found 
    }
#endif
    m_Pid = pid;
}

/*---------------------------------------------------------------------------*/

void RTESys_ProcessState::Close()
{
#  if defined ( _WIN32 )
    if ( NULL != m_ProcHandle )
    {
        CloseHandle( m_ProcHandle );
        m_ProcHandle = NULL;
    }

    m_OpenState = Unknown;
#  endif

    m_Pid = 0;
}

/*---------------------------------------------------------------------------*/

RTESys_ProcessState::ProcState  RTESys_ProcessState::State()
 {
     ProcState state;

#    if defined ( _WIN32 )
      if ( NULL != m_ProcHandle )
      {
          if(WaitForSingleObject( m_ProcHandle, 0) != WAIT_TIMEOUT )
          {
              state = Died;
          }
          else
              state = Alive;
      }
      else
          state = m_OpenState;  // we don't know it, use the open state

#    else  /*  UNIX */

      if ( m_Pid > 1 )
      {
          if ( kill ( m_Pid, 0 ) != 0 )
          {
              if ( errno == ESRCH )
                  state = Died;
              else
                  state = Alive;
          }
          else
              state = Alive;
      }
      else
          state = m_OpenState;  // we don't know it, use the open state
#    endif

     return state;
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/