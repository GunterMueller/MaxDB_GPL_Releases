#ifndef SQLMAN_CONTEXT_HPP
#define SQLMAN_CONTEXT_HPP

/*!
  @file           SQLMan_Context.hpp
  @author         ThomasA
  @brief          defines SQL manager context.

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


#include "vak001.h"
#include "SQLManager/SQLMan_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
// #include "SQLManager/SQLMan_ContextSessionMembers.hpp"

class SQLMan_ContextSessionMembers;
class KSQL_Runtime;
class PIn_Part;
class DBProc_Debugger;
class SharedSQL_SQLCache;
class Catalog_ISessionCache;
class SQLMan_MessBlock;
class SQLMan_SchemaMap;
class SQLMan_SessionInfo;
class SQLMan_TempFileCollection;
class SysView_PseudoLongHandler;
class Proc_SessionList;
class Msg_List;
class Trans_Context;

/*! 
    @struct SQLMan_SyntaxTreeMembers
    @brief  Structure that contains all information relevant for the syntax tree.
            It is used by SQLMan_Context.GetSyntaxTreeMembers / .SetSyntaxTreeMembers
*/
struct SQLMan_SyntaxTreeMembers
{
public:
    tak_ap_max_tree_ptr SyntaxTreePtr;
    SAPDB_Int2          ScvIndex;
    SAPDB_Int2          MaxNode;
    SAPDB_Int2          APMaxNode;
    SAPDB_Int2          FirstHintNode;
};

/*!
  @class          SQLMan_Context
  @brief          implements a capsule for the pascal type tak_all_command_glob
 */
class SQLMan_Context : public tak_all_command_glob
{
public :
   /*!
      @brief constructor, create a context
    */
      SQLMan_Context(SQLMan_TransContext&);
   /*!
      @brief destructor, destructs c++ members
    */
      ~SQLMan_Context();
   /*!
      @brief returns the clients integer swap
    */
    inline tsp00_SwapKind_Param ClientSwap() const;
   /*!
      @brief deletes the syntax tree and frees corresponding memory
    */
    void DeleteSyntaxTree();
   /*!
      @brief destroys c++ members ot the context
    */
    void DestroyCppMembers();
   /*!
      @brief enables or disables packet vtrace
    */
   inline bool EnablePacketVtrace(bool doEnable);
   /*!
      brief returns the application type of the session
    */
   inline SQLMan_ApplType GetApplicationType() const;
   /*!
      brief returns the current schema name
    */
   inline const SQLMan_Identifier& GetCurrentSchemaName() const;
   /*!
      brief returns the current user name
    */
   inline const SQLMan_Identifier& GetCurrentUserName() const;
   /*!
      brief returns the current user id
    */
   inline const SQLMan_Surrogate& GetCurrentUserId() const;
   /*!
      brief returns true if the current user is of type unoprivate
    */
   inline SAPDB_Bool IsStandardUser() const;
   /*!
      brief returns true if the current user is of type uprivate
    */
   inline SAPDB_Bool IsResourceUser() const;
   /*!
      brief returns true if the current user is of type udba
    */
   inline SAPDB_Bool IsDbaUser() const;
   /*!
      brief returns true if the current user is of type usysdba
    */
   inline SAPDB_Bool IsSysdbaUser() const;
   /*!
      brief returns true if the current user is of type ucontroluser
    */
   inline SAPDB_Bool IsControlUser() const;
   /*!
      brief returns the current sql command identification
    */
    inline SAPDB_Int4 GetCurrentCommandId() const;
   /*!
     @brief Returns the default code (ASCII, UNICODE)
     */
    inline SAPDB_UInt1 GetDefaultCode() const;
   /*!
      brief returns the isolation level
    */
    inline SAPDB_UInt GetIsolationLevel() const;
   /*!
      brief returns the current nls settings
    */
    void GetNlsParams(SQLMan_NlsParams& nlsParams) const;
   /*!
     @brief deletes the current schema map
    */
   void DestroySchemaMap();
   /*!
      @brief returns the schema map
    */
   SQLMan_SchemaMap* GetSchemaMap();
   /*!
      brief returns the session info object
    */
    SQLMan_SessionInfo& GetSessionInfo();
   /*!
      brief returns the user timeout
    */
    inline SAPDB_Int GetUserTimeout() const;
   /*!
      @brief initialises the c++ members of the context
    */
    void InitCppMembers();
   /*!
      @brief  quick check of current error code
      @return true, if error code = 0; otherwise false
    */
    inline bool IsOk() const;
   /*!
      @brief returns the current SQLMan_Context
    */
    static SQLMan_Context* GetContext();
   /*!
      @brief  returns the session allocator
      @return a reference to the session allocator
    */
    inline SAPDBMem_RawAllocator& GetAllocator() const;
    /*!
       @brief returns registered db-procedure debug context
     */
    DBProc_Debugger* GetDBProcDebugger() const;
    /*!
       @brief returns a reference to the sessions error list
     */
    Msg_List& GetErrorList() const;

    /*!
       @brief returns the current error position
     */
    SAPDB_Int GetErrorPos() const;
    
    /*!
       @brief sets the current error position
     */
    void SetErrorPos(const SAPDB_Int errorPos);

    /*!
       @brief returns if sessionons error list is empty
     */
    bool IsErrorListEmpty() const;
    /*!
       @brief resets the sessions error list
     */
    void ResetErrorList();
    /*!
       @brief returns the runtime for the SQLDBC component
    */
    KSQL_Runtime& GetSQLDBCRuntime();
    /*!
       @brief returns a reference to the sessions catalog cache
     */
     Catalog_ISessionCache& GetSessionCatalogCache();
    /*!
       @brief returns a reference to the sessions SharedSQL_SQLCache
     */
    SharedSQL_SQLCache& GetSharedSQL_SQLCache();
   /*!
      @brief returns the task identification
    */
    inline RTE_TaskId GetTaskId() const;
    /*!
       @brief returns the collection of grouped temporary files
    */
    SQLMan_TempFileCollection& GetTempFileCollection();
    /*!
       @brief returns sessions in UDE servers
    */
    Proc_SessionList * GetUDESessions();
   /*!
      @brief tells whether a given statement is still active, i.e. has been started but not yet finished
      @param statementId identification of the inquired statement
    */
    bool IsStatementActive(SAPDB_Int statementId) const;
   /*!
      @brief returns a reference to the context mess block
    */
    inline SQLMan_MessBlock& GetMessBlock();
   /*!
      @brief returns a reference to the pseudo long handler, which handles long columns of system views.
    */
    SysView_PseudoLongHandler& GetPseudoLongHandler() const;
   /*!
      @brief  returns a pointer to the current return part
      @return pointer to the current return part
    */
    inline tsp1_part * GetCurrentReturnPartPtr();
   /*!
      @brief  returns a pointer to the statement specific part of the context
      @return pointer to the statement specific part
    */
    inline void* GetStatementPartPtr();
   /*!
      @brief          returns the size of the statement specific part of the context
      @return         sizeof the statement specific part
    */
    inline int GetStatementPartSize() const;
   /*!
      @brief  returns a pointer to the sql statement
      @return pointer to the sql statement
    */
    inline void* GetSQLStatementPtr();
   /*!
      @brief          returns the size of the sql statement 
      @return         size of the sql statement
    */
    inline int GetSQLStatementSize() const;
   /*!
      @brief increments the syntax tree, i.e. provides new nodes to be used by the parser
    */
   void IncrementSyntaxTree();
   /*!
      @brief initializes interface runtime task trace context
    */
   void InitInterfaceTaskTraceContext();
   /*!
      @brief returns true, if the current statement is a select ... into
    */
   inline bool IsSelectIntoStatement() const;
   /*!
      @brief           creates a new syntax tree
      @param size [in] initial number of tree nodes
      @return          none
    */
    void  NewSyntaxTree(int size);

    inline void  GetSyntaxTreeMembers( SQLMan_SyntaxTreeMembers& STM ) const;
    inline SQLMan_SyntaxTreeMembers GetSyntaxTreeMembers() const;
    inline void  SetSyntaxTreeMembers( const SQLMan_SyntaxTreeMembers& STM );

    /*!
      @brief  registers the current statement in the active statement directory
      @return the statement identification of the current statement, -1 if the statement couldn't be registered due to lack of memory.
    */
    SAPDB_Int4 RegisterCurrentStatement();
    /*!
      @brief unregisters the given statement from the active statement directory
    */
    void UnregisterStatement(SAPDB_Int4 statementId);
    /*!
      @brief resets the current error code (a_returncode) to 0;
    */
    inline void ResetReturnCode();
    /*!
       @brief          restores a message block saved by SaveMessBlock
       @param pmessblock [in] a pointer to the saved message block. This must be the
                              result of a previous  SaveMessBlock call
       @return         none
     */
    void  RestoreMessBlock(void* pMessBlock);
    /*!
       @brief          returns the current sql error code
       @return         the current sql error code
     */
    inline int ReturnCode() const;
    /*!
       @brief          stores the current message block in the heap
       @return         a pointer to the saved message block, null in case
                       of insufficient memory
     */
    void* SaveMessBlock(bool DeepCopy);
    /*!
       @brief returns the data/dime format of the current session
     */
    inline SQLMan_DateTimeFormat GetDateTimeFormat() const;
    /*!
       @brief set the data/dime format of the current session
     */
    inline int SetDateTimeFormat (int newFormat);
    /*!
       @brief          sets the InDDLTrigger property
       @return         none
     */
    inline void SetInDDLTrigger(bool setTrue = true);
    /*!
      @brief set the session transaction isolation
      @param newIsolation a set describing the new required isolation
      @return the previous transaction isolation
     */
    inline tgg00_HandlingSet SetIsolation (tgg00_HandlingSet newIsolation);
    /*!
       @brief after this method has been called, the task does not use the shared catalog cache any more
     */
    inline void SetLocalCacheUsageOnly();
    /*!
       @brief sets the member a_createSharedSQL dependent on the parameter SHAREDSQL
     */
    void SetSharedSQL();
    /*!
       @brief returns a user defined error to the application. This is normnaly done
              from a db-procedure call.
       @param rc [in] the error code to be returned
       @param msg [in] a pointer to the error text. The text must start with the
              correct defined byte (ascii blank or unicode defined byte)
       @param msgLength [in] the length of the message in characters (not bytes)
     */
    void SetStopMsg (SAPDB_Int2 rc,
                     const void* msg,
                     int         msgLength);
    /*!
       @brief copy a part from a UDE reply to a client reply.
       @param part [in] the part to be copied

       This is used to copy message list part etc.
     */
    void CopyResultPart (const PIn_Part & part);
   /*!
      @brief          sets the InSession property
      @return         none
    */
    inline bool SetInSession(bool);
    /*!
       @brief registers a dbproc debugger
     */
    void  SetDBProcDebugger(DBProc_Debugger*);
    /*!
       @brief stores a no more memory error
     */
    void SetNoMoreMemoryError(const Msg_List& MsgList);
    /*!
       @brief sets the current in sql packet
     */
    inline void SetPacket(tsp1_packet_ptr pPacket);
    /*!
       @brief sets the sql return code
     */
    inline void SetReturnCode (SAPDB_Int2 sqlCode);

	inline SQLMan_QueryRewriteMode QueryRewriteMode();
	inline SQLMan_QueryRewriteMode QueryRewriteDone();

    /*!
       @brief returns the sessions sqlmode
    */
    inline SQLMan_SqlMode GetSessionSqlMode() const;
    /*!
       @brief sets the sessions sqlmode
    */
    inline void SetSessionSqlMode(tsp00_SqlMode_Enum sqlMode);
    /*!
       @brief returns the current transaction context contained in SQLMan_Context
    */
    inline SQLMan_TransContext& TransContext();
    /*!
       @brief returns the current transaction context contained in SQLMan_Context
    */
    Trans_Context& GetTransContext();
    /*!
      @brief translates an basis error into an sql code and stores it in context
    */
    void ThrowError (SAPDB_Int2 error, int errorPos = 1, int keyWord = -1);
     /*!
      @brief translates an basis error into an sql code and stores it in context
    */
    void ThrowError (SAPDB_Int2 error, int errorPos, const SQLMan_Identifier& Identifier);

    /*!
	  @brief adds or deletes the given Warning within a_transinf.tri_trans.trWarning_gg00 dependend on the parameter 'on'
    */
    inline void SetTransInfoWarning( tsp00_Warnings_Enum Warning, bool on=true );

   /*!
      @brief          cast from acv to SQLMan_context
      @acv [in]       a reference to the acv
      @return         reference to context
    */
    inline static SQLMan_Context& AcvToContext(tak_all_command_glob& acv);
private :
    /*inline*/ SQLMan_ContextSessionMembers& CppMembers();                  //!< returns the c++ members of the context
    /*inline*/ const SQLMan_ContextSessionMembers& CppMembersConst() const; //!< returns the c++ members of the context
};

//inline SQLMan_ContextSessionMembers& SQLMan_Context::CppMembers()
//{
//    return *REINTERPRET_CAST(SQLMan_ContextSessionMembers*, a_cpp_session_members);
//}

//inline const SQLMan_ContextSessionMembers& SQLMan_Context::CppMembersConst() const
//{
//    return *REINTERPRET_CAST(const SQLMan_ContextSessionMembers*, a_cpp_session_members);
//}

inline tsp00_SwapKind_Param SQLMan_Context::ClientSwap() const
{
    return a_out_packet->sp1_header.sp1h_mess_swap;
}

inline bool SQLMan_Context::EnablePacketVtrace(bool doEnable)
{
    bool isEnabled = (0 != a_packetVtrace);
    a_packetVtrace = doEnable;
    return isEnabled;
}

inline SQLMan_ApplType SQLMan_Context::GetApplicationType() const
{
    return a_comp_type;
}

inline const SQLMan_Identifier& SQLMan_Context::GetCurrentSchemaName() const
{
    return *reinterpret_cast<const SQLMan_Identifier*>(&a_curr_schema);
}

inline const SQLMan_Identifier& SQLMan_Context::GetCurrentUserName() const
{
    return *reinterpret_cast<const SQLMan_Identifier*>(&a_curr_user_name);
}

inline const SQLMan_Surrogate& SQLMan_Context::GetCurrentUserId() const
{
    return *reinterpret_cast<const SQLMan_Surrogate*>(&a_curr_user_id);
}

inline SAPDB_Bool SQLMan_Context::IsStandardUser() const
{
    return a_current_user_kind == unoprivate;
}

inline SAPDB_Bool SQLMan_Context::IsResourceUser() const
{
    return a_current_user_kind == uprivate;
}

inline SAPDB_Bool SQLMan_Context::IsDbaUser() const
{
    return a_current_user_kind == udba;
}

inline SAPDB_Bool SQLMan_Context::IsSysdbaUser() const
{
    return a_current_user_kind == usysdba;
}

inline SAPDB_Bool SQLMan_Context::IsControlUser() const
{
    return a_current_user_kind == ucontroluser;
}

inline SAPDB_UInt SQLMan_Context::GetIsolationLevel() const
{
    return a_iso_level;
}

inline SAPDB_Int SQLMan_Context::GetUserTimeout() const
{
    return a_user_timeout;
}

inline SAPDB_Int4 SQLMan_Context::GetCurrentCommandId() const
{
    return a_command_id;
}

inline SAPDB_UInt1 SQLMan_Context::GetDefaultCode() const
{
    return a_defaultcode;
}

inline bool SQLMan_Context::IsOk() const
{
    return (0 == a_returncode);
}

inline SAPDBMem_RawAllocator& SQLMan_Context::GetAllocator() const
{
    return *REINTERPRET_CAST(SAPDBMem_RawAllocator*, a_transinf.tri_trans.trAllocator_gg00);
}

//inline bool SQLMan_Context::IsStatementActive(SAPDB_Int statementId) const
//{
//    return CppMembersConst().m_activeStatementDirectory.IsActive(statementId);
//}

inline SQLMan_MessBlock& SQLMan_Context::GetMessBlock()
{
    return *REINTERPRET_CAST(SQLMan_MessBlock*, &a_mblock);
}

inline tsp1_part * SQLMan_Context::GetCurrentReturnPartPtr()
{
	return a_curr_retpart;
}

//inline KSQL_Runtime& SQLMan_Context::GetSQLDBCRuntime()
//{
//    return CppMembers().m_sqlRuntime;
//}

inline void* SQLMan_Context::GetStatementPartPtr()
{
    return this;
}

inline int SQLMan_Context::GetStatementPartSize() const
{
    int partSize = (int) ((char*) &a_end_of_statement_part - (char*) this);
    return partSize;
}

inline void* SQLMan_Context::GetSQLStatementPtr()
{
    return a_cmd_part->sp1p_buf();
}
inline int SQLMan_Context::GetSQLStatementSize() const
{
    return a_cmd_part->sp1p_buf_len();
}

inline void SQLMan_Context::GetSyntaxTreeMembers( SQLMan_SyntaxTreeMembers& STM ) const
{
    STM.SyntaxTreePtr   = a_ap_tree;
    STM.ScvIndex        = a_scv_index;
    STM.MaxNode         = a_maxnode;
    STM.APMaxNode       = a_ap_max_node;
    STM.FirstHintNode   = a_first_hint_node;
}

inline SQLMan_SyntaxTreeMembers SQLMan_Context::GetSyntaxTreeMembers() const
{
    SQLMan_SyntaxTreeMembers STM;
    STM.SyntaxTreePtr   = a_ap_tree;
    STM.ScvIndex        = a_scv_index;
    STM.MaxNode         = a_maxnode;
    STM.APMaxNode       = a_ap_max_node;
    STM.FirstHintNode   = a_first_hint_node;
    return STM;
}


inline void SQLMan_Context::SetSyntaxTreeMembers( const SQLMan_SyntaxTreeMembers& STM )
{
    a_ap_tree           = STM.SyntaxTreePtr;
    a_scv_index         = STM.ScvIndex;
    a_maxnode           = STM.MaxNode;
    a_ap_max_node       = STM.APMaxNode;
    a_first_hint_node   = STM.FirstHintNode;
}

inline RTE_TaskId SQLMan_Context::GetTaskId() const
{
    return a_transinf.tri_trans.trTaskId_gg00;
}

//inline void SQLMan_Context::InitInterfaceTaskTraceContext()
//{
//    CppMembers().m_sqlRuntime.InitTraceContext();
//}

inline SQLMan_Context& SQLMan_Context::AcvToContext(tak_all_command_glob& acv)
{
    return *REINTERPRET_CAST(SQLMan_Context*, &acv);
}

//inline SAPDB_Int4 SQLMan_Context::RegisterCurrentStatement()
//{
//    if (CppMembers().m_activeStatementDirectory.RegisterStatement(a_command_id))
//    {
//      return a_command_id;
//    }
//    else
//    {
//        return -1;
//    }
//}

inline bool SQLMan_Context::IsSelectIntoStatement() const
{
    if (!a_return_segm) return false;
    return ((csp1_select_into_fc == a_return_segm->sp1r_function_code()) || (csp1_mselect_into_fc == a_return_segm->sp1r_function_code()));
}

inline void SQLMan_Context::ResetReturnCode()
{
    ResetErrorList();
    a_returncode = 0;
}

inline int SQLMan_Context::ReturnCode() const
{
    return a_returncode;
}

inline SQLMan_DateTimeFormat SQLMan_Context::GetDateTimeFormat() const
{
    return a_dt_format;
}

inline int SQLMan_Context::SetDateTimeFormat (int newFormat)
{
    int oldFormat = a_dt_format;
    a_dt_format.becomes(STATIC_CAST(tgg00_DateTimeFormat_Enum, newFormat));
    return oldFormat;
}

inline bool SQLMan_Context::SetInSession(bool inSession)
{
    bool res = ((pasbool) true == a_in_session);
    a_in_session = inSession;
    return res;
}

inline void SQLMan_Context::SetInDDLTrigger(bool setTrue)
{
    a_in_ddl_trigger = setTrue;
}

inline tgg00_HandlingSet SQLMan_Context::SetIsolation (tgg00_HandlingSet newIsolation)
{
    tgg00_HandlingSet oldIsolation  = a_transinf.tri_global_state;
    a_transinf.tri_global_state = newIsolation;
    return oldIsolation;
}

inline void SQLMan_Context::SetLocalCacheUsageOnly()
{
    a_localCacheUsageOnly = true;
}

inline void SQLMan_Context::SetPacket(tsp1_packet_ptr pPacket)
{
    a_in_packet = pPacket;
}

inline void SQLMan_Context::SetReturnCode (SAPDB_Int2 sqlCode)
{
    a_returncode = sqlCode;
}

inline SQLMan_SqlMode  SQLMan_Context::GetSessionSqlMode() const
{
    return a_session_sqlmode;
}

inline void SQLMan_Context::SetSessionSqlMode(tsp00_SqlMode_Enum sqlMode)
{
    a_session_sqlmode.becomes(sqlMode);
}

inline SQLMan_TransContext& SQLMan_Context::TransContext()
{
    return a_transinf.tri_trans;
}

inline SQLMan_QueryRewriteMode SQLMan_Context::QueryRewriteMode()
{
    return a_queryrewrite_mode;
}

inline SQLMan_QueryRewriteMode SQLMan_Context::QueryRewriteDone()
{
    return a_queryrewrite_done;
}

inline void SQLMan_Context::SetTransInfoWarning( tsp00_Warnings_Enum Warning, bool on/*=true*/ )
{
	if ( on )
		a_transinf.tri_trans.trWarning_gg00.addElement(Warning);
	else
		a_transinf.tri_trans.trWarning_gg00.delElement(Warning);
}

inline SAPDB_Int SQLMan_Context::GetErrorPos() const
{
    return a_errorpos;
}

inline void SQLMan_Context::SetErrorPos(const SAPDB_Int errorPos)
{
    a_errorpos = errorPos;
}

//inline void SQLMan_Context::UnregisterStatement(SAPDB_Int4 statementId)
//{
//    CppMembers().m_activeStatementDirectory.UnregisterStatement(statementId);
//}

#endif
