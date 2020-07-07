/*!
 * \file    OMS_CriticalSection.hpp
 * \author  IvanS, MarkusS, PeterG
 * \brief   OMS critical sections
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



*/

#ifndef __OMS_CRITICALSECTION_HPP
#define __OMS_CRITICALSECTION_HPP

class OMS_Session;
class OmsHandle;

#include "Oms/OMS_AFX.h"

/*----------------------------------------------------------------------------------*/
/// Class for synchronizing different tasks
/*!
** This class provides a mechanism for synchronizing different liveCache tasks that
** access global variables of a component. By means of an OmsCriticalSection, a
** code segment in which a global resource is manipulated and can, therefore, only
** be processed by exactly one liveCache task at any one time, is protected. To
** ensure that a critical segment is released even in the event of an exception,
** OmsCriticalSection is implemented as a class.
**
** \see \ref oms_OmsCriticalSectionAPI 
** \see \ref example_OmsCriticalSection 
*/
class AFX_EXT_CLASS OmsCriticalSection {
public :
  /*--------------------------------------------------------------------------------*/
  ///Constructor
  /*!
  ** Constructor of the OmsCritcalSection class. To be able to synchronize different 
  ** critical sections independently of one another, there are several critical 
  ** sections, numbered from 1 to _OMS_REGIONS. 
  **
  ** \param handle pointer to the current handle,
  ** \param SectionId identifies the critical section.
  */
  OmsCriticalSection(OmsHandle *handle, int SectionId = 0);

  /*--------------------------------------------------------------------------------*/
  /// Enters the critical section 
  /*!
  ** Enters the critical section specified in the constructor. If a concurrent 
  ** liveCache task T is located in this section when the call is made, the invoking 
  ** task is blocked until T releases the section by invoking omsLeave.
  */
  void omsEnter();

  /*--------------------------------------------------------------------------------*/
  /// Leaves the critical section
  /*!
  ** Leaves the critical section identified by SectionId. A concurrent liveCache task,
  ** which is waiting to enter the critical section, can then enter it.
  */
  void omsLeave();

  /*--------------------------------------------------------------------------------*/
  /// Destructor
  /*!
  ** Destructor of OmsCriticalSection. If it is ascertained in the constructor that 
  ** the current task is still in the critical section, omsLeave is executed implicitly. 
  ** In this way, it is also ensured that when an exception occurs within the critical
  ** section, the critical section is left in the correct manner.
  */
  ~OmsCriticalSection();

private :
  OMS_Session*     m_session;
  int              m_regionId;
  bool             m_inSection;
};

#endif // __OMS_CRITICALSECTION_HPP
