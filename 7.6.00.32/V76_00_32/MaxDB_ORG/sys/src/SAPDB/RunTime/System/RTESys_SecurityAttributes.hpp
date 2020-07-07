/*!
  @defgroup     Sys     System
*/

/*!========================================================================
  @ingroup      Sys
  @file         RTESys_SecurityAttributes.hpp
  @author       StefanP

  @brief        Security Attributes for NT and UNIX

  @since        2005-09-23  11:43
  @sa           


  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTESYS_SECURITYATTRIBUTES_HPP
#define RTESYS_SECURITYATTRIBUTES_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"



extern SAPDBTrace_Topic Runtime_Trace;	///<Runtime trace


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class	RTESys_SecurityAttributes
  @brief	Common UNIX and Windows class for different kinds of security attributes (e.g. world access).
			Those attributes can be used to create different kind of objects with special access permissions
			(e.g shared memory, files, semaphores etc.).
			(Singleton class)
 */


class RTESys_SecurityAttributes
{
public:

  /*!
     @brief			Returns reference to single instance of RTESys_SecurityAttributes
  */


    static  RTESys_SecurityAttributes &   Instance ();

  /*!
     @brief          Destructor
  */

    ~RTESys_SecurityAttributes  ()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::~RTESys_SecurityAttributes", Runtime_Trace, 9);
            Release ();
        }

  /*!
     @brief			Release the resources needed for the security attributes
  */

    void    Release ();

  /*!
     @brief			Get security attributes for world access (UNIX: read/write access for u(ser)g(roup)o(ther))
  */

    RTE_SECURITY_ATTRIBUTES &       GetWorldSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetWorldSA", Runtime_Trace, 9);
            return m_WorldSA;
        }

  /*!
     @brief			Get security attributes for lock access, i.e. no further access for anyone
					(UNIX: no read/write/execte access for u(ser)g(roup)o(ther))
  */

    RTE_SECURITY_ATTRIBUTES  &      GetLockSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetLockSA", Runtime_Trace, 9);
            return m_LockSA;
        }

  /*!
     @brief			Default security attributes for initialization or as dummy allocation 
  */

    RTE_SECURITY_ATTRIBUTES &       GetNoSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetNoSA", Runtime_Trace, 9);
            return m_NoSA;
        }

#if defined (_WIN32)
  /*!
     @brief			Get security attributes for operator and administrator world access
  */

    RTE_SECURITY_ATTRIBUTES &       GetOperatorAdminSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetOperatorAdminSA", Runtime_Trace, 9);
            return m_OperatorAdminSA;
        }

#else

  /*!
     @brief			UNIX: Get write access for the world and the group; full write and read access for the user
  */

    RTE_SECURITY_ATTRIBUTES &       GetWorldGroupWriteSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetWorldGroupWriteSA", Runtime_Trace, 9);
            return m_WorldGroupWriteSA;
        }

  /*!
     @brief			UNIX: Get full write and read access for the user and the group; no access for the world
  */

    RTE_SECURITY_ATTRIBUTES &       GetGroupSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetGroupSA", Runtime_Trace, 9);
            return m_GroupSA;
        }

  /*!
     @brief			UNIX: Get full write and read access for the user; no access for the group and the world
  */

    RTE_SECURITY_ATTRIBUTES &       GetOwnSA()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::GetOwnSA", Runtime_Trace, 9);
            return m_OwnSA;
        }


#endif
private:

  /*!
     @brief         Constructor
					The Constructor is privat to this class to avoid instantiation outside 
					of the class.
  */

    RTESys_SecurityAttributes   ()
    {
        SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::RTESys_SecurityAttributes", Runtime_Trace, 9);
    }

  /*!
     @brief     Allocation of the various security attributes

     @param     messageList [out]		- Message list

     @return    true/false
  */

    SAPDB_Bool Initialize (SAPDBErr_MessageList        &   messageList);


    static  RTESys_SecurityAttributes *     m_Instance; 

    RTE_SECURITY_ATTRIBUTES      m_WorldSA;
    RTE_SECURITY_ATTRIBUTES      m_LockSA;
    RTE_SECURITY_ATTRIBUTES      m_NoSA;

#if defined (_WIN32)
    RTE_SECURITY_ATTRIBUTES      m_OperatorAdminSA;
#else
    RTE_SECURITY_ATTRIBUTES      m_GroupSA;
    RTE_SECURITY_ATTRIBUTES      m_WorldGroupWriteSA;
    RTE_SECURITY_ATTRIBUTES      m_OwnSA;
#endif



}; 

#endif  /* RTESYS_SECURITYATTRIBUTES_HPP */

