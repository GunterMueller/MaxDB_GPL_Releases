/*!========================================================================

  @file         RTESys_ProcessState.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Check if a process is dead or alive

                Determine the state of a process specified by given 
                process id.

  @since        2003-10-13  16:08
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


#ifndef RTESYS_PROCESSSTATE_HPP
#define RTESYS_PROCESSSTATE_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"
#include    "SAPDBCommon/SAPDB_ToString.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!---------------------------------------------------------------------------
  @class   RTESys_ProcessState
  @brief   Allows to check if a process is dead or alive

            
           Determine the state of a process specified by given 
           process id.
           
 ----------------------------------------------------------------------------*/

class RTESys_ProcessState
{
public:          
    /*!
    @brief    Process states
              DON'T FORGET SAPDB_ToStringClass BELOW!!!!
    */
  enum  ProcState { Unknown = 1,             /* We could no determine the state
                                                ( e.g. access to the process is denied ) */  
                    Alive,                   /* The procces is up and running */
                    Died                     /* The process died */
                  };
public:
    /*!
    @brief         Constructor.

    @param         pid  [in] - Pid to the process to by checked
    */
    RTESys_ProcessState( RTE_OSPid pid = 0 );


    /*!
    @brief         Destructor.
    */
    virtual             ~RTESys_ProcessState();

    /*!
    @brief         Open the process

    @param         pid  [in] - Pid to the process to by opened
    
    @return        none
    */

    void                Open (RTE_OSPid pid);

    /*!
    @brief         Close the process

    @return        none
    */

    void                Close();

    /*!
    @brief         Determine the state of the process

    @return        One of the above process states
    */
    ProcState           State ();

private:
    // Do not allow to use copy constructor outside
	RTESys_ProcessState     (const RTESys_ProcessState & state);

    RTE_OSPid           m_Pid;
    ProcState           m_OpenState;

#   if defined (_WIN32)
     HANDLE             m_ProcHandle;
#   endif
};

/*!
  @brief convert RTESys_ProcessState::ProcState into readable string
  @param procState [in] the process state value to convert
 */
inline SAPDB_ToStringClass SAPDB_ToString( RTESys_ProcessState::ProcState procState )
{
    switch (procState)
    {
    case RTESys_ProcessState::Unknown            : return SAPDB_ToStringClass("Unknown");
    case RTESys_ProcessState::Alive           : return SAPDB_ToStringClass("Alive");
    case RTESys_ProcessState::Died           : return SAPDB_ToStringClass("Died");
    }
    return SAPDB_ToStringClass((SAPDB_Int4)procState);
}

#endif  /* RTESYS_PROCESSSTATE_HPP */
