/*!
  @file         Kernel_Main.cpp
  @author       UweH
  @brief        This is called by the RTE to call the kernel code
  @since        2004-11-09

  Earlier known as VAK91 created 1983-07-29
*/
/*!
\if EMIT_LICENCE

  copyright:    Copyright (c) 2001-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "hak102.h"  // a102akdump
#include "hak93.h"   // a93_user_commands,a93_implicit_commands
#include "heo51.h"    // vfinish,vinit
#include "heo58.h"    // vinit_akdump_ptr
#include "vak001.h"   // tak_all_command_glob
#include "vsp001.h"   // tsp1_utility_startup,tsp1_packet_ptr
#include "Transaction/Trans_Context.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "KernelCommon/Kernel_Types.hpp"
#include "KernelCommon/Kernel_Main.hpp"
#include "KernelCommon/Kernel_Task.hpp"
#include "KernelCommon/Kernel_Initialization.hpp"
#include "RunTime/RTE_OldKernelCommunication.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
// ----------------------------------------------------------------------------
static void Connect ( tak_all_command_glob &acv,
                      bool                  withoutCommunication,
                      pasbool              &alloc_ok,
                      tsp01_CommErr        &sql_comm_err )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Connect", Common_Trace, 5);

    alloc_ok     = true;
    sql_comm_err.becomes(commErrOk_esp01);

    // PTS 1117012 E.Z.
    if ( withoutCommunication )
    {
        // the utility session will not be supplied with a packet,
        // if an implicit RESTART or SHUTDOWN is requested by the RTE
        SAPDBMem_IRawAllocator &allocator = Trans_Context::GetContext(acv.a_transinf.tri_trans).Allocator();
        const SAPDB_UInt bufferSize = 4 * sizeof (tsp00_Buf);
        acv.a_in_packet = reinterpret_cast<tsp1_packet_ptr>(allocator.Allocate(bufferSize));
        if ( acv.a_in_packet == 0 )
        {
            alloc_ok = false;
            return;
        }
        acv.a_in_packet->sp1_header.sp1h_varpart_size = bufferSize - sizeof (acv.a_in_packet->sp1_header);
    }
    else
    {
        tsp00_Int4  net_packet_size;
        vconnect (acv.a_transinf.tri_trans.trTaskId_gg00, net_packet_size, sql_comm_err);
        if ( sql_comm_err != commErrOk_esp01 )
            return;
    }
    alloc_ok = Kernel_AllocateSessionBuffers(acv);
} 
// ----------------------------------------------------------------------------
static void SQLTask ( tak_all_command_glob &acv,
                             pasbool              &alloc_ok,
                             bool                  connect_needed )
{
    SAPDBTRACE_ROUTINE_DEBUG ("SQLTask", Common_Trace, 5);

    pasbool           do_release = false;
    tsp01_CommErr     sql_comm_err;
    tgg00_CurrSeqInfo sequenceInfo;
 
    do_release = false;
    if ( connect_needed )
    {
        Connect (acv, false, alloc_ok, sql_comm_err );
        if ( ! alloc_ok )
            return;
    }
    else
    {
        sql_comm_err.becomes(commErrOk_esp01);
    }
    // (* PTS 1110096 E.Z. *)
    sequenceInfo.csi_cnt                       = 0;
    acv.a_transinf.tri_trans.trSeqInfoPtr_gg00 = &sequenceInfo;
    acv.a_transinf.tri_trans.trAcvPtr_gg00     = &acv;

    if ( sql_comm_err == commErrOk_esp01 )
    {
        while ( ! do_release )
        {
            a93_user_commands (acv, alloc_ok, do_release);
        }
        vrelease (acv.a_transinf.tri_trans.trTaskId_gg00);
        Kernel_DeallocateSessionBuffers (acv, false);
    }
}
// ----------------------------------------------------------------------------
static Trans_Context& Init(RTETask_ITask       &task,
                           tgg00_TransContext &oldtrans,
                           Msg_List           &errlist)
{
    Trans_Context          &trans = Trans_Context::GetContext(oldtrans);
    SAPDBMem_RawAllocator *pAllocator = Kernel_CreateTransContextAllocator(task);
    tsp00_RteCommAddr      rteCommPtr;

    if ( pAllocator == 0 )
        RTE_Crash(errlist);
        
    vinit (rteCommPtr, RTE_COMM_MXSP00);
    
    trans.Initialize(task, errlist, *pAllocator, rteCommPtr);

    Kernel_Task::Instance().SetTransContext(task, trans);

    return trans;
}

// ----------------------------------------------------------------------------
void Kernel_Main(RTETask_ITask &task)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_Main", Common_Trace, 5);

    Msg_List errlist;

    if (sizeof(tgg00_TransContext) != sizeof(Trans_Context))
        RTE_Crash(errlist);

    if ( task.Type() != RTETask_ITask::User
         &&
         task.Type() != RTETask_ITask::Utility )
    {
        tgg00_TransContext  oldtrans;
        Trans_Context      &trans = Init(task, oldtrans, errlist);
        
        Kernel_NonUserTask ( task, trans, errlist );
        
        Kernel_Task::Instance().DelTransContext (task);
        
        Kernel_DeleteTransContextAllocator(trans);
    }
    else
    {
        tak_all_command_glob acv; 
        pasbool              alloc_ok = true;

        /* to avoid to work with a copy of the transcontext, the tri_trans within the   */
        /* acv is used as trans context within user tasks                               */
        /* in the future the acv should only maintain a pointer to the trans context FF */
        Trans_Context      &trans = Init(task, acv.a_transinf.tri_trans, errlist);

        /* tri_trans is destroyed by kernel_initSqlContext, therefore save and restore it */
        tgg00_TransContext savetrans = acv.a_transinf.tri_trans;
        Kernel_InitSqlContext (acv, acv.a_transinf.tri_trans);
        acv.a_transinf.tri_trans = savetrans;

        // (* h.b. CR 1103950 *)
        vinit_akdump_ptr ( &acv, (void(*)())a102akdump );

        Kernel_Task::Instance().SetSQLManContext (task,SQLMan_Context::AcvToContext(acv));
        
        if ( task.Type() == RTETask_ITask::Utility )
        {
            tsp1_utility_startup utility_startup;
            tsp01_CommErr        sql_comm_err;
            tsp1_packet_ptr      internal_packet_ptr;
             
            vutil_startup(utility_startup);
            
            Connect (acv, utility_startup != sp1us_connect, alloc_ok, sql_comm_err);
                             
            if ( alloc_ok && sql_comm_err == commErrOk_esp01 )
            {
                if ( utility_startup == sp1us_connect )
                    SQLTask (acv, alloc_ok, false);
                else
                {
                    internal_packet_ptr = acv.a_in_packet;
                    a93_implicit_commands (acv, utility_startup);
                    acv.a_in_packet = internal_packet_ptr;
                }
                Kernel_DeallocateSessionBuffers (acv, utility_startup != sp1us_connect);
            }
        }
        else
        {
            acv.a_in_session = false;
            acv.a_authentication = NULL;
            SQLTask (acv, alloc_ok, true);
        }

        Kernel_Task::Instance().DelSQLManContext (task);

        Kernel_Task::Instance().DelTransContext (task);

        Kernel_DeleteTransContextAllocator(trans);
        
        vfinish();
    }
}
