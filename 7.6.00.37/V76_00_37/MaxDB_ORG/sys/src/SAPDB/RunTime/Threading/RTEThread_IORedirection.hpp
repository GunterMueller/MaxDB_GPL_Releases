/*!---------------------------------------------------------------------
  @file         RTEThread_IORedirection.hpp
  @ingroup      Runtime
  @author       Robin
  @brief        Thread for message redirection
  
  This thread exists to redirect messages from stdout/stderr into
  kernel diagnostic message files

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "Messages/Msg_IOutput.hpp"

#ifndef NO_OLD_DIAG_FILES
#include "gsp03.h"
#include "heo60_2.h"
#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  GLOBAL FUNCTIONS (CODE)                                                  *
 *===========================================================================*/


class RTEThread_IORedirection : public RTEThread_KernelThread
{
public:
    RTEThread_IORedirection()
        : RTEThread_KernelThread("RTEThread_IORedirection")
    {
        pipe(m_pipeFd);

        if ( 2 != dup2( m_pipeFd[1], 2) )
        {
            printf( "redirecting stderr failed with errno %d\n", errno );
        }

        if ( 1 != dup2( m_pipeFd[1], 1) )
        {
            printf( "redirecting stderr failed with errno %d\n", errno );
        }

    }
private:
    SAPDB_Int4 KernelThreadMain()
    {
        char buffer[80];
        int nBytes;
        while(1)
	    {
	        nBytes = read(m_pipeFd[0],(void *)buffer,sizeof(buffer)-1);
            buffer[nBytes]='\0';
            SAPDB_Char *startPos=buffer;
            SAPDB_Char *newLinePos;
            while(newLinePos = strchr(startPos,'\n'))
            {
                *newLinePos = NULL;

                Msg_List theList = Msg_List( Msg_List::Info, 1, MSG_MSGOUTPUT_COMPONENT, __FILE__, __LINE__,startPos);
                Msg_List errList;
                Msg_IOutput::GetDiagnosticOutput().Output( theList, errList );
#ifndef NO_OLD_DIAG_FILES
                eo60k_Message((tsp3_priority_Param)8, sp3m_info,1 , MSG_MSGOUTPUT_COMPONENT,startPos );
#endif
                startPos = newLinePos+1;

            }
            if(startPos < buffer+nBytes )
            {
                Msg_List theList = Msg_List( Msg_List::Info, 1, MSG_MSGOUTPUT_COMPONENT,__FILE__, __LINE__,startPos);
                Msg_List errList;
                Msg_IOutput::GetDiagnosticOutput().Output( theList, errList );
#ifndef NO_OLD_DIAG_FILES
                eo60k_Message((tsp3_priority_Param)8, sp3m_info,1 ,MSG_MSGOUTPUT_COMPONENT,startPos );
#endif
            }
        }
        return 0;
    }

    void DestroyObject()
    {
    }

    int m_pipeFd[2];
};

