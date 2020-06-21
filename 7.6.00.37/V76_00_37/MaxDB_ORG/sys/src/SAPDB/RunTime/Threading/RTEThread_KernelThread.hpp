/*!========================================================================

  @file         RTEThread_KernelThread.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Kernel thread base class.

                The following class allows to create and manage kernel threads.

  @since        2005-01-20  11:10
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


#ifndef RTETHREAD_KERNELTHREAD_HPP
#define RTETHREAD_KERNELTHREAD_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/Threading/RTEThread_Thread.hpp"

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
  @class   RTEThread_KernelThread
  @brief   Kernel thread base class.

           The following class allows to create and manage kernel threads.
           The own thread class must be derived from this base class 
           and has to implement the 'KernelThreadMain' and the 
           'DestroyObject' methode.
 ----------------------------------------------------------------------------*/

class RTEThread_KernelThread : public RTEThread_Thread
{
public:
    /*!
       @brief          Constructor
       @param          threadName [in]  - Name of thread
     */
    RTEThread_KernelThread( SAPDB_Char const * const threadName )
        : RTEThread_Thread( threadName, false ),
          m_ThreadHasStated( false )  {}


    /*!
       @brief          Check if the tread has entered the thread main 
                       methode.

       @return         true if the thread main methode has been entered.
     */
    SAPDB_Bool ThreadHasStarted() { return m_ThreadHasStated; };

protected:
    /*!
      @brief kernel threasd main routine

      This routine must be overloaded in kernel therad implementation
      @return exit code ( value != 0 shall indicate error )
     */
    virtual SAPDB_Int4 KernelThreadMain() = 0;

#if defined (_WIN32)    
    virtual DWORD       ExceptionFilter( LPEXCEPTION_POINTERS pExceptPtrs );
    virtual SAPDB_Int4  ExceptionHandler();
#endif

private:
    virtual SAPDB_Int4 ThreadMain();

private:
    SAPDB_Bool        m_ThreadHasStated;
};


#endif  /* RTETHREAD_KERNELTHREAD_HPP */
