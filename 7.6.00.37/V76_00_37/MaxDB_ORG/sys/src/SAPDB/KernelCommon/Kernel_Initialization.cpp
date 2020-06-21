/*!
  @file         Kernel_Initialization.cpp
  @author       UweH
  @brief        This call all components initializers when the kernel is before state ADMIN
  @since        2004-11-03
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
//-----------------------------------------------------------------------------
#include "KernelCommon/Kernel_Initialization.hpp"
#include "KernelCommon/Kernel_Types.hpp"
#include "KernelCommon/Kernel_Task.hpp"
#include "KernelCommon/Kernel_Trace.hpp"
#include "SAPDBCommon/Messages/SDBMsg_KernelCommon.h"
#include "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "SAPDBCommon/SAPDB_sprintf.h"
#include "RunTime/Tasking/RTETask_ITask.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "RunTime/MemoryManagement/RTEMem_BlockAllocator.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccess.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "Logging/Log_Volume.hpp"
#include "Logging/Log_Transaction.hpp"
#include "Transaction/Trans_Context.hpp"
#include "Trace/Trace_Writer.hpp"
#include "Trace/Trace_Manager.hpp"
#include "ServerTasks/SrvTasks_Coordinator.hpp"
#include "ServerTasks/SrvTasks_Task.hpp"
#include "Pager/Pager_Writer.hpp"
#include "KernelCommon/Kernel_Timeout.hpp"
#include "KernelCommon/Kernel_Administration.hpp"
#include "FrameControl/FrameCtrl_Manager.hpp"
#include "FreeBlockManagement/FBM_Manager.hpp"
#include "IOManager/IOMan_Manager.hpp"
#include "Converter/Converter_Converter.hpp"
#include "Pager/Pager_Controller.hpp"
#include "FileDirectory/FileDir_SharedDirectory.hpp"
#include "liveCache/LVC_InProcDispatcher.hpp"
#include "SQLManager/SQLMan_UpdateStatistics.hpp"
#include "SQLManager/Catalog/Catalog_Interface.hpp"
//-----------------------------------------------------------------------------
#include "ggg00.h"  // tgg00_TransContext
#include "hgg01.h"  // g01kb_work_st_size,g01kb_workbuf_size,g01mb_data_size,g01mb_qual_size,g01mb_stack_size,g01mb_strat_size,g01default_configuration,g01inittabid
#include "hgg01_3.h"  // g01code,g01timeout
#include "hgg02.h"  // g02inittranslate_tables
#include "hgg03.h"  // g03init
#include "hgg08.h"  // g08region_ids
#include "hgg011.h" // g011get_event_param
#include "hgg941.h" // Kernel_InitTrace

#include "hak01.h"  // a01_parser_init,a01_il_b_identifier
#include "hak101.h" // a101_CreateErrorHandler,a101_CreateJoinHashAllocatorSingleton,a101_InitContextMembers,a101_GetErrorMessageList
#include "hak05.h"  // a05init
#include "hak10.h"  // a10ak_cache_init
#include "hak260.h" // a260init
#include "hak362.h" // a362OnlineTaskId
#include "hak40.h"  // a40glob_init
#include "hak41.h"  // a41glob_init
#include "hak506.h" // a506_init_numbercheckarray
#include "hak509.h" // a509size_of_map
#include "hak51.h"  // a51init_cache_sizes
#include "hak54.h"  // a54_init
#include "hak545.h" // a545_init
#include "hak642.h" // a642colinfo_init
#include "hak728.h" // a728_init_strat_texts
#include "hak71.h"  // a71glob_init_accesspath_info
#include "hak720.h" // a720glob_init_eval_stat
#include "hak92.h"  // a92next_pcount
#include "hak96.h"  // a96eventtask
#ifdef SAPDB_SLOW
#include "hak99.h"  // a99init
#endif

#include "hkb05.h" // k05init
#include "hkb71.h" // k71init

#include "gbd490.h"  // cbd490_CreateIndexParallel
#include "gbd493.h"  // cbd493_CreateIndexBlockAllocator
#include "gbd930.h"  // cbd930_GarbageCollector
#include "hbd01.h"   // b01setto_zero_full_and_null
#include "hbd01_1.h" // b01niltree_id
#ifdef TRACE
#include "hbd06.h"  // b06check_rec
#endif
#include "hbd91.h"  // bd91InitOMSGarbageCollection

#include "hkb38.h"  // k38allocate
#include "hkb391.h" // kb391InitializeWaitLists
#include "hkb57.h"  // k57allocate
#include "hkb571.h" // kb571InitializeWaitList

#ifdef SAPDB_SLOW
#include "hta01.h"  // t01sinit
#endif

#include "heo51.h" // vinitok,vgetuktid

//-----------------------------------------------------------------------------
/// This is a global object which is used by all kernel tasks a anchor for their task local storage pointers.
Kernel_Task Kernel_TaskLocalStorage;
//-----------------------------------------------------------------------------
extern cbd930_GarbCollController  bd91GarbCollController;
//-----------------------------------------------------------------------------
static void InitializeAllComponents(Trans_Context &context)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_Initialize::InitializeAllComponents", Common_Trace, 5);

#ifdef TRACE
    b06check_rec := true;
#endif
#ifdef SAPDB_SLOW
    a99init();
    t01sinit();
#endif
    g01default_configuration();
    g08region_ids();
    if ( ! FrameCtrl_Manager::CreateInstance() )
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED));
    if ( ! Trace_Manager::CreateInstance(context.Task().ID()) )
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED));
    if ( ! IOMan_Manager::CreateInstance(context.Task()))
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED));
    FBM_Manager::CreateInstance();
    Converter_Converter::CreateInstance();
    if ( ! Kernel_Administration::CreateInstance() )
        RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED));
    Pager_Controller::GetInstance();
    FileDir_SharedDirectory::CreateInstance();
    Kernel_InitTrace();
    gg01optimize_parallel_server = a101_SetMaxParallelServers(gg01optimize_parallel_server);
    a01_parser_init();
    a10ak_cache_init();
    a101_CreateErrorHandler();
    // (* PTS 1104572 E.Z. *)
    a40glob_init();
    a41glob_init();
    a260init();
    a51init_cache_sizes();
    a506_init_numbercheckarray();
    k05init();
    k71init();
    b01setto_zero_full_and_null();
    SrvTasks_Coordinator::CreateInstance(RTEMem_Allocator::Instance());
    bd91InitOMSGarbageCollection();
    cbd490_CreateIndexParallel::GetIndexFileListSize( true );
    if ( ! cbd493_CreateIndexBlockAllocator::CreateInstance()){
        RTE_Crash( Msg_List( Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED ));
    }
    g02inittranslate_tables();
    g03init();
    g01inittabid();
    a05init();    //  (* PTS 1121403 E.Z. *)
    a642colinfo_init();
    a54_init();
    a545_init();
    a728_init_strat_texts();
    a71glob_init_accesspath_info();
    a720glob_init_eval_stat();
    a362OnlineTaskId = UNDEF_SP00; //(* PTS 1117498, 1120287 M.Ki. *)
    SQLMan_UpdateStatistics::InitKernelParameters(context.ErrorList());
    Catalog_Interface::Initialize();
    kb391InitializeWaitLists();
    Log_Transaction::DetermineLocalRedoBufferSize();
    kb571InitializeWaitList();
    k57allocate (context.OldTrans());
    if ( context.OldTrans().trError_gg00 != e_ok )
    {
        Kernel_AppendBasisError(context.OldTrans().trError_gg00, context.ErrorList(), "k57allocate failed");
        RTE_Crash(context.ErrorList());
    }
    a101_CreateJoinHashAllocatorSingleton();
    g011get_event_param();
    // (* PTS 1003033 E.Z. *)
    // (* PTS 1105614 E.Z. *)
    k38allocate (context.OldTrans().trTaskId_gg00, context.OldTrans().trError_gg00);
    if ( context.OldTrans().trError_gg00 != e_ok )
    {
        Kernel_AppendBasisError(context.OldTrans().trError_gg00, context.ErrorList(), "k38allocate failed");
        RTE_Crash(context.ErrorList());
    }
    Log_Savepoint::CreateInstance(RTEMem_Allocator::Instance());
    // **********
    vinitok();
    // **********

    k38ReserveBackupServerTasks (context.OldTrans());
    if ( context.OldTrans().trError_gg00 != e_ok )
    {
        Kernel_AppendBasisError(context.OldTrans().trError_gg00, context.ErrorList(), "k38ReserveBackupServerTasks failed");
        RTE_Crash(context.ErrorList());
    }

    if ( ! Log_Savepoint::ReserveServerTask (context.Task(),context.ErrorList()) )
        RTE_Crash(context.ErrorList());
}
//-----------------------------------------------------------------------------
void Kernel_Init(RTETask_ITask &task)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_Init", Common_Trace, 5);

    // look into head of Kernel_Main
    
    Msg_List               errlist;
    tgg00_TransContext     oldtrans;
    Trans_Context          &trans = Trans_Context::GetContext(oldtrans);
    SAPDBMem_RawAllocator *pAllocator = Kernel_CreateTransContextAllocator(task);
    tsp00_RteCommRec       rteCommRec;
    
    rteCommRec.to_cancel       = 0;
	rteCommRec.rte_fill1       = 0;
	rteCommRec.virt_reads      = 0;
	rteCommRec.file_root       = 0;
	rteCommRec.file_record_cnt = 0;

    if ( pAllocator == 0 )
    {
        Kernel_AppendBasisError(e_no_more_memory, errlist, "Kernel_CreateTransContextAllocator() failed");
        RTE_Crash(errlist);
    }
        
    Kernel_Task::Instance().Initialize(task);

    trans.Initialize(task, errlist, *pAllocator, &rteCommRec);

    Kernel_Task::Instance().SetTransContext(task, trans);

    InitializeAllComponents(trans);

    Kernel_Task::Instance().DelTransContext (task);
    
    Kernel_DeleteTransContextAllocator(trans);
}
//-----------------------------------------------------------------------------
void Kernel_NonUserTask ( RTETask_ITask &task,
                          Trans_Context &context,
                          Msg_List      &errlist )
{    
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_NonUserTask", Common_Trace, 5);

    context.OldTrans().trSessionId_gg00.gg91BuildTaskSession (task.Type());

    switch ( task.Type() )
    {
        case RTETask_ITask::Logwriter :
        {
            Log_Volume::CreateInstance( task,
                                        RTEMem_Allocator::Instance(),
                                        Log_FrameAllocator::Instance(),
                                        Log_Queue::Synchronized );
            g011get_log_event_param();
            Log_Volume::Instance().GetWriter().Run();
            break;
        }
        case RTETask_ITask::Tracewriter :
        {
            if ( ! Trace_Writer::CreateInstance(task ))
                RTE_Crash(Msg_List(Msg_List::Error,SDBMSG_KERNELCOMMON_HEAP_MEMORY_ALLOCATION_FAILED));
            Trace_Writer::GetInstance().Run();
            break;
        }
        case RTETask_ITask::Timer :
            {
                Kernel_Timeout timeOutTask;
                timeOutTask.Run(context);
            }
            break;
        case RTETask_ITask::Server :
        {
            SrvTasks_Task servertask(context);
            servertask.Run();
            break;
        }
        case RTETask_ITask::Datawriter :
        {
            Pager_Writer *pager = new( RTEMem_Allocator::Instance() ) Pager_Writer(context);
            if ( 0 == pager )
                RTE_Crash(SAPDBErr_Exception(__CONTEXT__, SAPDBERR_ASSERT_STATE_FAILED, "Pager_Writer() alloc failed"));
            pager->Initialize();
            break;
        }
        case RTETask_ITask::Event :
            a96eventtask (context.OldTrans());
            break;
        case RTETask_ITask::GarbageCollector :
        {
            cbd930_GarbageCollector GarbageCollector(context.OldTrans(), bd91GarbCollController);
            break;
        }
    }
}
// ----------------------------------------------------------------------------
#define SMALL_BLOCK_SIZE        8192         // should be Cserv parameters ...
#define SUPPLEMENT_BLOCK_SIZE  (8192 * 16)
// ----------------------------------------------------------------------------
SAPDBMem_RawAllocator* Kernel_CreateTransContextAllocator (RTETask_ITask &task)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_CreateTransContextAllocator", Common_Trace, 5);
    
    // Allocate the memory for the allocator dynamically. This is done because all allocators are
    // registered by RTEMem_AllocatorRegister using a linked list. When the database kernel is
    // stopped, some tasks are killed very hard, removing their task stack. When another returns
    // from a91mainprogam_with_allocator(), the destructor for the allocator is called, which 
    // tries to deregister itself from RTEMem_AllocatorRegister. This means accessing the allocators
    // of tasks that have been killed hard. If the allocators would be on the task stack, this would
    // crash... http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1126560

    SAPDB_UInt InitialBlockSize;
    
    if ( RTETask_ITask::Utility == task.Type()
         ||
         RTETask_ITask::User    == task.Type()
         ||
         RTETask_ITask::Server  == task.Type() )
    {
        RTEConf_Parameter::Integer  paramValue;
        Msg_List                    errlist;
        if( ! RTEConf_ParameterAccess::Instance()->GetInteger( UTF8( "INIT_ALLOCATORSIZE" ), paramValue, errlist ) )
             RTE_Crash( errlist );

        InitialBlockSize = (SAPDB_UInt)paramValue;
    }
    else
    {
        InitialBlockSize = SMALL_BLOCK_SIZE;
    }
    
    SAPDB_UTF8 identifier[40];
    SAPDB_sprintf((char *)identifier, sizeof(identifier), "TransContext T%03d", task.ID());

    return new(RTEMem_Allocator::Instance()) SAPDBMem_RawAllocator(
       identifier,
       RTEMem_BlockAllocator::Instance(), 
       InitialBlockSize,
       SUPPLEMENT_BLOCK_SIZE,
       SAPDBMem_RawAllocator::FREE_RAW_EXTENDS_EXCEPT_INITIAL); // PTS 1124755 TS 2003-10-23
}
// ----------------------------------------------------------------------------
void Kernel_DeleteTransContextAllocator(Trans_Context &trans)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_DeleteTransContextAllocator", Common_Trace, 5);
    // proper end of task - call the destructor explicitly as it has been constructed via placement new
    SAPDBMem_RawAllocator *pAllocator = static_cast <SAPDBMem_RawAllocator*> (&trans.Allocator());
    pAllocator->~SAPDBMem_RawAllocator();
    RTEMem_Allocator::Instance().Deallocate(pAllocator);
    trans.OldTrans().trAllocator_gg00 = 0;
}
// ----------------------------------------------------------------------------
bool Kernel_AllocateSessionBuffers (tak_all_command_glob &acv)
{ 
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_AllocateSessionBuffers", Common_Trace, 5);

    SAPDBMem_IRawAllocator &allocator = Trans_Context::GetContext(acv.a_transinf.tri_trans).Allocator();
    void*                   aux_addr;
 
    if ( acv.a_mb_data_addr == 0 )
	{
        aux_addr = allocator.Allocate(g01mb_data_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_mb_data_size = g01mb_data_size();
        acv.a_mb_data_addr = reinterpret_cast<tgg00_DataPartPtr>(aux_addr);
	}
    if ( acv.a_mb_qual_addr == 0 )
    {
        aux_addr = allocator.Allocate(g01mb_qual_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_mb_qual_size = g01mb_qual_size();
        acv.a_mb_qual_addr = reinterpret_cast<tgg00_QualBufPtr>(aux_addr);
    }
    if ( acv.a_stack_addr == 0 )
    {
        aux_addr = allocator.Allocate(g01mb_stack_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_stack_size = g01mb_stack_size();
        acv.a_stack_addr = reinterpret_cast<tgg00_StackListPtr>(aux_addr);
    }
    if ( acv.a_work_st_addr == 0 )
    {
        aux_addr = allocator.Allocate(g01kb_work_st_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_work_st_max = g01kb_work_st_size() / sizeof (tgg00_StackEntry);
        acv.a_work_st_addr = reinterpret_cast<tgg00_StackListPtr>(aux_addr);
    }
    if ( acv.a_work_buf_addr == 0 )
    {
        aux_addr = allocator.Allocate(g01kb_workbuf_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_work_buf_size = g01kb_workbuf_size();
        acv.a_work_buf_addr = reinterpret_cast<tsp00_MoveObjPtr>(aux_addr);
    }
    if ( acv.a_mb_strat_addr == 0 )
    {
        aux_addr = allocator.Allocate(g01mb_strat_size());
        if ( aux_addr == 0 )
            return false;
        acv.a_mb_strat_size = g01mb_strat_size();
        acv.a_mb_strat_addr = reinterpret_cast<tsp00_MoveObjPtr>(aux_addr);
    }

    acv.a_mblock.mb_data()       = acv.a_mb_data_addr;
    acv.a_mblock.mb_data_size()  = acv.a_mb_data_size;
    acv.a_mblock.mb_qual()       = acv.a_mb_qual_addr;
    acv.a_mblock.mb_qual_size()  = acv.a_mb_qual_size;
    acv.a_mblock.mb_strat()      = acv.a_mb_strat_addr;
    acv.a_mblock.mb_strat_size() = acv.a_mb_strat_size;

    if ( acv.a_input_data_map == 0 )
    {
        aux_addr = allocator.Allocate(a509size_of_map());
        if ( aux_addr == 0 )
            return false;
        acv.a_input_data_map = reinterpret_cast<tsp00_MoveObjPtr>(aux_addr);
    }
	return true;
}
// ----------------------------------------------------------------------------
inline void Deallocate(SAPDBMem_IRawAllocator &allocator,
                       void*                  &memory)
{
	if ( memory != 0 )
    {
        allocator.Deallocate(memory);
        memory = 0;
    }
}
// ----------------------------------------------------------------------------
void Kernel_DeallocateSessionBuffers ( tak_all_command_glob &acv,
                                       bool                  free_packets )
{
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_DeallocateSessionBuffers", Common_Trace, 5);

    if ( acv.a_transinf.tri_trans.trAllocator_gg00 == 0 )
    {
        Msg_List errlist;
        RTE_Crash(errlist);
    }
    if ( acv.a_transinf.tri_trans.trErrorList_gg00 == 0 )
    {
        Msg_List errlist;
        RTE_Crash(errlist);
    }
        
    SAPDBMem_IRawAllocator &allocator = Trans_Context::GetContext(acv.a_transinf.tri_trans).Allocator();

    void* p;

    p = (void*)acv.a_mb_data_addr;
    acv.a_mb_data_addr = 0;
    Deallocate(allocator, p);

    p = (void*)acv.a_mb_strat_addr;
    acv.a_mb_strat_addr = 0;
    Deallocate(allocator, p);
    
    p = (void*)acv.a_mb_qual_addr;
    acv.a_mb_qual_addr = 0;
    Deallocate(allocator, p);
    
    p = (void*)acv.a_stack_addr;
    acv.a_stack_addr = 0;
    Deallocate(allocator, p);
    
    p = (void*)acv.a_work_st_addr;
    acv.a_work_st_addr = 0;
    Deallocate(allocator, p);
    
    p = (void*)acv.a_work_buf_addr;
    acv.a_work_buf_addr = 0;
    Deallocate(allocator, p);
    
    if ( free_packets )
    {
        p = (void*)acv.a_in_packet;
        acv.a_in_packet = 0;
        Deallocate(allocator, p);
    }
    
    p = (void*)acv.a_input_data_map;
    acv.a_input_data_map = 0;
    Deallocate(allocator, p);
} 
// ----------------------------------------------------------------------------
void Kernel_InitSqlContext (tak_all_command_glob &acv, tgg00_TransContext &trans) 
{ 
    SAPDBTRACE_ROUTINE_DEBUG ("Kernel_InitSqlContext", Common_Trace, 5);

    /* kernel_main uses tri_trans for it's tgg00_TransContext */
    /* therefore save trans onto stack                        */
    tgg00_TransContext savetrans = trans;
    Msg_List            errlist;

    if ( ! SAPDB_MemFill( &acv, 0x00, sizeof(acv), "Kernel_Main", __CONTEXT__, errlist) )
        RTE_Crash(errlist);

    acv.a_transinf.tri_trans = savetrans;
    
    acv.a_in_packet             = 0;
    acv.a_out_packet            = 0;
    acv.a_cmd_segm              = 0;
    acv.a_cmd_part              = 0;
    acv.a_data_part             = 0;
    acv.a_data_ptr              = 0;
    acv.a_data_length           = 0;
    acv.a_resname_part          = 0;
    acv.a_rescount_part         = 0;
    acv.a_return_segm           = 0;
    acv.a_ap_tree               = 0;
    acv.a_mb_data_addr          = 0;
    acv.a_mb_strat_addr         = 0;
    acv.a_mb_qual_addr          = 0;
    acv.a_stack_addr            = 0;
    acv.a_work_st_addr          = 0;
    acv.a_work_buf_addr         = 0;
    acv.a_input_data_map        = 0;
    acv.a_err_parm_cnt          = 0;
    acv.a_rollback_info.clear();
    acv.a_viewname              = a01_il_b_identifier;
    acv.a_result_name           = a01_il_b_identifier;
    acv.a_init_ex_kind.becomes(only_syntax);
    acv.a_longdemand_data       = 0;  // (* PTS 1122546 D.T. *)

    if ( ! SAPDB_MemFill( acv.a_modul_name, bsp_c1, sizeof(acv.a_modul_name), "Kernel_Main", __CONTEXT__, errlist) )
        RTE_Crash(errlist);
        
    // (* bsp_identifier is correct for modul_name *)
    acv.a_comp_type.becomes(at_unknown);
    acv.a_connected_name        = a01_il_b_identifier;
    acv.a_curr_user_name        = a01_il_b_identifier;
    acv.a_is_exclusive          = true;
    // (* PTS 1115978 E.Z. *)
    acv.a_acc_user              = a01_il_b_identifier;
    acv.a_acc_groupname         = a01_il_b_identifier;
    acv.a_acc_dbaname           = a01_il_b_identifier;

    tgg00_FileId temp_id;
    
    temp_id                   = b01niltree_id;
    temp_id.fileType_gg00().addElement(ftsTemp_egg00);
    temp_id.fileTfn_gg00().becomes(tfnTemp_egg00);
    acv.a_trigger_tree          = temp_id;
 
    for ( SAPDB_UInt offset = cak_extern_pos-1; offset < cak_aux_pos; ++offset )
        acv.a_resname_addr[offset] = 0;

    acv.a_expl_resname          = a01_il_b_identifier;
    acv.a_show_curr             = temp_id;
    acv.a_pars_curr             = temp_id;
    acv.a_usage_curr            = temp_id;
    acv.a_into_tree             = temp_id;
    acv.a_intern_tree           = temp_id;
    acv.a_intern_res_tree       = temp_id;
    acv.a_intern_cmd_tree       = temp_id;

    // (* PTS 1111797 E.Z. *)
    acv.a_sqlmode.becomes(sqlm_internal);
    acv.a_user_timeout          = g01timeout.timeCmd_gg00;
    acv.a_use_timeout           = true;
    acv.a_dt_format             = g01code.date_time;
    acv.a_sysdir.ci_cache_p     = 0;
    acv.a_scv.sc_missing_kw_pos = cak_is_undefined;
    acv.a_len_row_not_found_text= 0;
    acv.a_max_codewidth         = 1;
    acv.a_session_max_codewidth = 1;
    acv.a_mess_code             = csp_ascii;
    acv.a_session_mess_code     = csp_ascii;
    // acv.a_ak_language           = cak_nls_date_language;
	if ( ! SAPDB_MemCopy( acv.a_ak_language, cak_nls_date_language, sizeof(acv.a_ak_language), "", __CONTEXT__, errlist) )
        RTE_Crash(errlist);
    acv.a_cmd_stack.cst_top     = 0;
    acv.a_explain_kind.becomes(ex_default);
    // (* PTS 1114071 E.Z. *)
    acv.a_max_used_pcount[0]    = csp_defined_byte;
    acv.a_max_used_pcount[1]    = csp_defined_byte;
    acv.a_max_used_pcount[2]    = csp_defined_byte;
    acv.a_first_free_pcount     = 0;
	if ( ! SAPDB_MemCopy( acv.a_challenge, cgg_zero_id, sizeof(acv.a_challenge), "", __CONTEXT__, errlist) )
        RTE_Crash(errlist);
    acv.a_last_free_pcount      = cak_max_pcounts;

    tak_parskey parskey;

    for ( SAPDB_UInt parseCount = 0; parseCount < cak_max_pcounts; ++parseCount )
    {
        a92next_pcount (acv, parskey);
        acv.a_free_pcounts[parseCount] = parskey.p_count;
    }

    acv.a_planHandle            = 0;
    acv.a_first_free_pcount     = 1;

    tsp_dcom_init_sink_parms sink_info;

    sink_info.acv_addr      = reinterpret_cast<tsp00_Addr>(&acv);
    sink_info.sink_addr     = reinterpret_cast<tsp00_Addr>(&(acv.a_kernel_sink));
    sink_info.sink_size     = sizeof (acv.a_kernel_sink);

    vdcom_init_sink (&sink_info);

    acv.a_isInCppDBProcedure    = false; // (* PTS 1110148 *)
    
    vgetuktid(acv.a_allocatorInfo.ai_uktId);
    
    acv.a_allocatorInfo.ai_alloc_sys.ami_Allocated = 0;
    acv.a_allocatorInfo.ai_alloc_usr.ami_Allocated = 0;
    acv.a_space_option = false; // (* PTS 1121403 E.Z. *)
    // (* PTS 1121841 E.Z. *)
    acv.a_isReplicationUser     = false;
    acv.a_isReplicationSession  = false;
    acv.a_withInternalTriggers  = false;
    acv.a_release_instances     = false;
    acv.a_db_full_connection    = true;
    acv.a_localCacheUsageOnly   = false;
    acv.a_modifyBootStrapCatalog= false;
    a101_InitContextMembers(acv);
    acv.a_cmd_id                = 0;
    acv.a_packetVtrace          = true;
    acv.a_parsingForSharedSql   = false;
    acv.a_systemTriggerFailed   = false;
    for ( SAPDB_UInt fieldlistcnt = 0; fieldlistcnt <= cgg_idx_max_valuefieldlist; ++fieldlistcnt )
    {
        acv.a_mblock.mb_fieldlists()[fieldlistcnt] = NULL;
    }

} 
