/*!========================================================================

  @file         RTESys_SecurityAttributes.cpp
  @ingroup      Runtime
  @author       StefanP

  @brief        Local Communication

  @since        2005-09-23  12:11
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
#if defined (_WIN32)        /*&if $OSSPEC=WIN32*/
#include    "gos00.h"       /* nocheck */

#if defined(KERNEL)
# include    "gos00k.h"     /* nocheck */
#endif

#endif                      /*&endif*/

#include    "RunTime/System/RTESys_SecurityAttributes.h"
#include    "RunTime/System/RTESys_SecurityAttributes.hpp"
#include    "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include    "RunTime/RTE_Messages.hpp"

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
RTESys_SecurityAttributes   *RTESys_SecurityAttributes::m_Instance = NULL; 


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

 
RTESys_SecurityAttributes & RTESys_SecurityAttributes::Instance()
{
    SAPDBErr_MessageList    messageList;

    SAPDBTRACE_ROUTINE_DEBUG("RTESys_SecurityAttributes::Instance", Runtime_Trace, 9);

    if (NULL == m_Instance)
    {
        m_Instance = new (RTEMem_Allocator::Instance()) RTESys_SecurityAttributes ();
        if (NULL == m_Instance)
        {
            RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, "RTESys_SecurityAttributes"));
        }

        if (!m_Instance->Initialize (messageList))
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SECURITY_INIT_ATTRIBUTES);
            RTE_Crash (messageList);
        }
    }

    return *m_Instance;
}

//--------------------------------------------------------------------------------

SAPDB_Bool RTESys_SecurityAttributes::Initialize (SAPDBErr_MessageList        &   messageList)
{
#if defined (_WIN32)
    ACCESS_RIGHTS_REC       Access;
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;


    SAPDBTRACE_ROUTINE_DEBUG("RTESys_SecurityAttributes::Initialize", Runtime_Trace, 9);

    // --- initialize Windows NT security
    rc = sql49c_set_sync_access ();
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SECURITY_SET_SYNCHRONIZE_ACCESS, 
            SAPDB_ToString (rc));
        return false;
    }

    //World Access
    m_WorldSA.nLength           = sizeof(m_WorldSA);
    m_WorldSA.bInheritHandle    = FALSE;

    memset (&Access, 0, sizeof (Access));   // - this must be done first!
    Access.World.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;

    rc = sql49c_alloc_and_init_SD( &Access, &m_WorldSA.lpSecurityDescriptor );
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SECURITY_INIT_DESCRIPTOR, 
            "World", SAPDB_ToString (rc));
        return false;
    }

    //Lock Access
    m_LockSA.nLength            = sizeof(m_LockSA);
    m_LockSA.bInheritHandle     = FALSE;

    memset ( &Access, 0, sizeof (Access) );   // - this must be done first!
    rc = sql49c_alloc_and_init_SD(&Access, &m_LockSA.lpSecurityDescriptor);
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SECURITY_INIT_DESCRIPTOR, 
            "Lock", SAPDB_ToString (rc));
        return false;
    }

    //No access Attribute
    m_NoSA.nLength              = 0;
    m_NoSA.bInheritHandle       = FALSE;
    m_NoSA.lpSecurityDescriptor = NULL;


    //Operator/Admin Access
    m_OperatorAdminSA.nLength           = sizeof(m_OperatorAdminSA);
    m_OperatorAdminSA.bInheritHandle    = FALSE;

    memset (&Access, 0, sizeof (Access));   // - this must be done first!
    Access.Creator.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.Admin.ulAccess          = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.System.ulAccess         = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.SAPDBOp.ulAccess        = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
    Access.DomSAPDBOp.ulAccess     = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;

    rc = sql49c_alloc_and_init_SD( &Access, &m_OperatorAdminSA.lpSecurityDescriptor );

    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SECURITY_INIT_DESCRIPTOR, 
                                           "Operator/Admin", SAPDB_ToString (rc));
        return false;
    }
#else
    m_WorldSA = 0666;
    m_WorldGroupWriteSA = 0622;
    m_GroupSA = 0660;
    m_OwnSA = 0600;
    m_LockSA = 0;
    m_NoSA = 0;     

#endif
    return true;
}

//--------------------------------------------------------------------------------

void    RTESys_SecurityAttributes::Release  ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTESys_SecurityAttributes::Release", Runtime_Trace, 9);
#if defined (_WIN32)
    if (NULL != m_WorldSA.lpSecurityDescriptor)
    {
        FREE_MEM (m_WorldSA.lpSecurityDescriptor);
        m_WorldSA.lpSecurityDescriptor = NULL;
    }

    if (NULL != m_LockSA.lpSecurityDescriptor)
    {
        FREE_MEM (m_LockSA.lpSecurityDescriptor);
        m_LockSA.lpSecurityDescriptor = NULL;
    }
#endif
    return;
}

//--------------------------------------------------------------------------------

#if defined (_WIN32)

extern "C" RTE_SECURITY_ATTRIBUTES *  GetOperatorAdminSA()
{
  return &(RTESys_SecurityAttributes::Instance()).GetOperatorAdminSA();
}

#endif