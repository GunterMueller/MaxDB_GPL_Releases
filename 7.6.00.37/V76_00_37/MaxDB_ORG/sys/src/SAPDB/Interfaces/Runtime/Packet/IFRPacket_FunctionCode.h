/*!
  @file           IFRPacket_FunctionCode.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          
  @see            

\if EMIT_LICENCE

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
#ifndef IFRPACKET_FUNCTIONCODE_H
#define IFRPACKET_FUNCTIONCODE_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_Trace.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Packet
 * The function codes of SAPDB. The kind of statement can be
 * identified by the function code, after a parse or execute.
 */
class IFRPacket_FunctionCode 
{
public:
    /**
     * The function codes.
     */
    enum FunctionCode {
        Nil_FC                 = csp1_nil_fc,
        CreateTable_FC         = csp1_create_table_fc,
        SetRole_FC             = csp1_set_role_fc,
        Insert_FC              = csp1_insert_fc,
        Select_FC              = csp1_select_fc,
        Update_FC              = csp1_update_fc,
        DropRole_FC            = csp1_drop_role_fc,
        DropView_FC            = csp1_drop_view_fc,
        DropTable_FC           = csp1_drop_table_fc,
        Delete_FC              = csp1_delete_fc,
        CreateView_FC          = csp1_create_view_fc,
        DropUser_FC            = csp1_drop_user_fc,
        CreateRole_FC          = csp1_create_role_fc,
        CreateSequence_FC      = csp1_create_sequence_fc,
        AlterSequence_FC       = csp1_alter_sequence_fc,
        Unused1_FC             = csp1_unused1_fc,
        DropSequence_FC        = csp1_drop_sequence_fc,
        CreateSchema_FC        = csp1_create_schema_fc,
        CreateCluster_FC       = csp1_create_cluster_fc,
        CreateUser_FC          = csp1_create_user_fc,
        CreateIndex_FC         = csp1_create_index_fc,
        DropIndex_FC           = csp1_drop_index_fc,
        DropCluster_FC         = csp1_drop_cluster_fc,
        ValidateIndex_FC       = csp1_validate_index_fc,
        CreateProcedure_FC     = csp1_create_procedure_fc,
        AlterProcedure_FC      = csp1_alter_procedure_fc,         
        AlterTable_FC          = csp1_alter_table_fc,             
        Explain_FC             = csp1_explain_fc,                 
        Grant_FC               = csp1_grant_fc,                   
        Revoke_FC              = csp1_revoke_fc,                  
        CreateSynonym_FC       = csp1_create_synonym_fc,          
        DropSynonym_FC         = csp1_drop_synonym_fc,            
        AlterSystemSwithLog_FC = csp1_alter_system_swith_log_fc,  
        SetTransaction_FC      = csp1_set_transaction_fc,         
        PlSqlExecute_FC        = csp1_pl_sql_execute_fc,          
        LockTable_FC           = csp1_lock_table_fc,              
        DropSchema_FC          = csp1_drop_schema_fc,                 
        Rename_FC              = csp1_rename_fc,                  
        Comment_FC             = csp1_comment_fc,                 
        Audit_FC               = csp1_audit_fc,                   
        Noaudit_FC             = csp1_noaudit_fc,                 
        AlterIndex_FC          = csp1_alter_index_fc,             
        CreateExternalDatabase_FC = csp1_create_external_database_fc,
        DropExternalDatabase_FC = csp1_drop_external_database_fc,  
        CreateDatabase_FC      = csp1_create_database_fc,         
        AlterDatabase_FC       = csp1_alter_database_fc,          
        CreateRollbackSegment_FC = csp1_create_rollback_segment_fc, 
        AlterRollbackSegment_FC = csp1_alter_rollback_segment_fc,  
        DropRollbackSegment_FC = csp1_drop_rollback_segment_fc,   
        CreateTsTablespace_FC  = csp1_create_ts_tablespace_fc,    
        AlterTsTablespace_FC   = csp1_alter_ts_tablespace_fc,     
        DropTsTablespace_FC    = csp1_drop_ts_tablespace_fc,      
        AlterSession_FC        = csp1_alter_session_fc,           
        AlterUser_FC           = csp1_alter_user_fc,              
        Commit_FC              = csp1_commit_fc,                  
        Rollback_FC            = csp1_rollback_fc,                
        Savepoint_FC           = csp1_savepoint_fc,               
        CreateControlFile_FC   = csp1_create_control_file_fc,     
        AlterTracing_FC        = csp1_alter_tracing_fc,           
        CreateTrigger_FC       = csp1_create_trigger_fc,          
        AlterTrigger_FC        = csp1_alter_trigger_fc,           
        DropTrigger_FC         = csp1_drop_trigger_fc,            
        AnalyzeTable_FC        = csp1_analyze_table_fc,           
        AnalyzeIndex_FC        = csp1_analyze_index_fc,           
        AnalyzeCluster_FC      = csp1_analyze_cluster_fc,         
        CreateProfile_FC       = csp1_create_profile_fc,          
        DropProfile_FC         = csp1_drop_profile_fc,            
        AlterProfile_FC        = csp1_alter_profile_fc,           
        DropProcedure_FC       = csp1_drop_procedure_fc,          
        Unused3_FC             = csp1_unused3_fc,                 
        AlterResourceCost_FC   = csp1_alter_resource_cost_fc,     
        CreateSlSnapshotLog_FC = csp1_create_sl_snapshot_log_fc,  
        AlterSlSnapshotLog_FC  = csp1_alter_sl_snapshot_log_fc,   
        DropSlSnapshotLog_FC   = csp1_drop_sl_snapshot_log_fc,    
        CreateSnapshot_FC      = csp1_create_snapshot_fc,         
        AlterSnapshot_FC       = csp1_alter_snapshot_fc,          
        DropSnapshot_FC        = csp1_drop_snapshot_fc,           

        MaxOci_FC              = csp1_max_oci_fc,                 

        SelectDirect_FC        = csp1_select_direct_fc,           
        SelectFirst_FC         = csp1_select_first_fc,            
        SelectLast_FC          = csp1_select_last_fc,             
        SelectNext_FC          = csp1_select_next_fc,             
        SelectPrev_FC          = csp1_select_prev_fc,             
        FetchFirst_FC          = csp1_fetch_first_fc,             
        FetchLast_FC           = csp1_fetch_last_fc,              
        FetchNext_FC           = csp1_fetch_next_fc,              
        FetchPrev_FC           = csp1_fetch_prev_fc,              
        FetchPos_FC            = csp1_fetch_pos_fc,               
        FetchSame_FC           = csp1_fetch_same_fc,              
        StringCommand_FC       = csp1_string_command_fc,          
        Connect_FC             = csp1_connect_fc,                 
        DropParseid_FC         = csp1_drop_parseid_fc,            
        Close_FC               = csp1_close_fc,                   
        Show73_FC              = csp1_show_73_fc,                 
        Usage_FC               = csp1_usage_fc,                   
        Serverdb_FC            = csp1_serverdb_fc,                
        Monitor_FC             = csp1_monitor_fc,                 
        Set_FC                 = csp1_set_fc,                     
        Link_FC                = csp1_link_fc,                    
        CreateDomain_FC        = csp1_create_domain_fc,           
        DropDomain_FC          = csp1_drop_domain_fc,             
        Describe_FC            = csp1_describe_fc,                
        AlterPassword_FC       = csp1_alter_password_fc,          

        Putval_FC              = csp1_putval_fc,                  
        Getval_FC              = csp1_getval_fc,                  
        Diagnose_FC            = csp1_diagnose_fc,                
        Unlock_FC              = csp1_unlock_fc,                  
        Refresh_FC             = csp1_refresh_fc,                 
        ClearSnapshotLog_FC    = csp1_clear_snapshot_log_fc,      
        NextStamp_FC           = csp1_next_stamp_fc,              
        ExistsTable_FC         = csp1_exists_table_fc,            
        CommitRelease_FC       = csp1_commit_release_fc,          
        RollbackRelease_FC     = csp1_rollback_release_fc,        
        DrdaNativeDdl_FC       = csp1_drda_native_ddl_fc,         
        SelectInto_FC          = csp1_select_into_fc,
        FetchRelative_FC       = csp1_fetch_relative_fc,          
        DBProcWithResultSetExecute_FC  = csp1_cursor_pl_sql_execute_fc,   

        MinMasscmd_FC          = csp1_min_masscmd_fc,             

        Minsert_FC = csp1_minsert_fc,                 
        Mselect_FC = csp1_mselect_fc,                 
        Mupdate_FC = csp1_mupdate_fc,                 
        Mdelete_FC = csp1_mdelete_fc,                 
        MlockObjects_FC = csp1_mlock_objects_fc,           
        MfetchFirst_FC = csp1_mfetch_first_fc,            
        MfetchLast_FC = csp1_mfetch_last_fc,             
        MfetchNext_FC = csp1_mfetch_next_fc,             
        MfetchPrev_FC = csp1_mfetch_prev_fc,             
        MfetchPos_FC = csp1_mfetch_pos_fc,              
        MfetchSame_FC = csp1_mfetch_same_fc,             
        MselectInto_FC = csp1_mselect_into_fc,            
        MfetchRelative_FC = csp1_mfetch_relative_fc,         
        MaxMasscmd_FC = csp1_max_masscmd_fc,             
        MasscmdFcOffset = csp1_masscmd_fc_offset
    };

    /**
     * Constructs a function code object having <code>Nil_FC</code>.
     */
    inline IFRPacket_FunctionCode()
    :m_functionCode(Nil_FC),
     m_isQuery(false),
     m_isMassCommand(false)
    {}

    
    /* Constructor for handling SQL function codes. 
     * Applications should use the static functions if they only need
     * occasional information on the function code. If a function
     * code is remembered for a longer time, as function code object
     * should be used.
     * @param functioncode The function code as retrieved from 
     *        the database.
     */
    IFRPacket_FunctionCode(int functioncode);

    /**
     * Returns whether this function code identifies a query, i.e. returns a result set.
     * @return <code>true</code> if the function code identifies a query, that is:
     *         <code>SELECT</code>, <code>EXPLAIN</code>, <code>SHOW</code>,
     *         and DB Procedures having a <code>RETURNS CURSOR</code> clause.
     */
    inline IFR_Bool isQuery() const
    {
        return m_isQuery;
    }
    
    /**
     * Returns whether the checked function code identifies a query, i.e. returns a result set.
     * @param functioncode The function code to check.
     * @return <code>true</code> if the function code identifies a query, that is:
     *         <code>SELECT</code>, <code>EXPLAIN</code>, <code>SHOW</code>,
     *         and DB Procedures having a <code>RETURNS CURSOR</code> clause.
     */
    static IFR_Bool isQuery(int functioncode);

    /**
     * Returns whether this function code identifies a mass command, i.e. performs array execution.
     * @return <code>true</code> if the command is a mass command (<code>FETCH</code>,
     *         <code>INSERT</code>, <code>UPDATE</code>, <code>DELETE</code> and some
     *         <code>SELECT</code> commands may be mass commands.)
     */
    inline IFR_Bool isMassCommand() const
    {
        return m_isMassCommand;
    }

    /**
     * Returns whether the function code identifies a mass command, i.e. performs array execution.
     * @param functioncode The function code to check.
     * @return <code>true</code> if the command is a mass command (<code>FETCH</code>,
     *         <code>INSERT</code>, <code>UPDATE</code>, <code>DELETE</code> and some
     *         <code>SELECT</code> commands may be mass commands.)
     */
    static IFR_Bool isMassCommand(int functioncode);
    
    /**
     * Returns whether the function code identifies a command that releases 
     * the connection at the end (<code>COMMIT WORK RELEASE</code>, 
     * <code>ROLLBACK WORK RELEASE</code>).
     * @param functioncode The function code to check.
     * @return @c true, if this is a command that releases the connection.
     */
    IFR_Bool isConnectionReleasing();
    
    /**
     * Returns whether the function code identifies a command that releases 
     * the connection at the end (<code>COMMIT WORK RELEASE</code>, 
     * <code>ROLLBACK WORK RELEASE</code>).
     * @param functioncode The function code to check.
     * @return @c true, if this is a command that releases the connection.
     */
    static IFR_Bool isConnectionReleasing(int functioncode);
    
    /**
     * Check whether this call is a procedure call, and thus can have
     * streams.
     * @return @c true if the call is a procedure call.
     */
    inline IFR_Bool isProcedureCall() const
    {
        return m_functionCode == PlSqlExecute_FC;
    }

    
    /**
     * Returns true if it is a diagnose command.
     */
    inline IFR_Bool isDiagnose()  const
    {
        return m_functionCode == Diagnose_FC;
    }
    
    /**
     * Returns true on EXPLAIN commands.
     */
    inline IFR_Bool isExplain() const 
    {
        return m_functionCode == Explain_FC;
    }
    
    inline operator FunctionCode() const 
    {
        return m_functionCode;
    }  
  
  private:
    FunctionCode m_functionCode;    //!< The function code.
    IFR_Bool m_isQuery;             //!< True if the function code identifies a query.
    IFR_Bool m_isMassCommand;       //!< True if the function code identifies a mass command.
    friend IFR_TraceStream& operator << (IFR_TraceStream& s, const IFRPacket_FunctionCode &p );
};

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFRPacket_FunctionCode &p );

IFR_END_NAMESPACE

#endif // IFRPACKET_FUNCTIONCODE_H
