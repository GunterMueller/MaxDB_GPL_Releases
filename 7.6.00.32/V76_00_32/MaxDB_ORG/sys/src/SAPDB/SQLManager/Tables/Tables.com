 
#
# include dependencies (do not edit)
#
SQLTab_SysCheckTableLog.cpp inc=\
    :SAPDB/SQLManager/SQLMan_SavedErrorScope.hpp,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysCheckTableLog.hpp,heo52.h,\
    hgg01_3.h
SQLTab_SysCheckTableLog.hpp inc=\
    :SAPDB/SQLManager/KernelSQL/KSQL_Statement.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLTab_SysUpdStatLog.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_FieldMap.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_IFieldSequences.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_string.h,:SAPDB/SAPDBCommon/SAPDB_Types.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatLog.hpp
SQLTab_SysUpdStatLog.hpp inc=\
    :SAPDB/SQLManager/KernelSQL/KSQL_Statement.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLTab_SysUpdStatWanted.cpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdStatWanted.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_ColumnDescriptor.hpp,\
    :SAPDB/SQLManager/SQLMan_InternalTrans.hpp,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/SQLMan_ChangedUserScope.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,hbd01.h,hbd02.h,hak01.h,hak06.h,\
    hak28.h,ggg00,hgg01.h,hgg01_2.h,hgg01_3.h,heo67.h
SQLTab_SysUpdStatWanted.hpp inc=:SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp,ggg00
SQLTab_SysUpdateCounterWanted.cpp inc=\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpdateCounterWanted.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_SQLMan.h,\
    :SAPDB/SQLManager/Catalog/Catalog_TableDescriptor.hpp,\
    :SAPDB/SQLManager/Catalog/Catalog_Types.hpp,\
    :SAPDB/SQLManager/SQLMan_InternalTrans.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_Tracing.hpp,\
    :SAPDB/SQLManager/ErrorHandling/SQLManErr_Interface.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatementScope.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/FileDirectory/FileDir_IDirectory.hpp,\
    :SAPDB/FileDirectory/FileDir_Index.hpp,\
    :SAPDB/FileDirectory/FileDir_Table.hpp,\
    :SAPDB/Transaction/Trans_Context.hpp,:SAPDB/Messages/Msg_List.hpp,\
    :SAPDB/Messages/Msg_Ucs2Arg.hpp,hgg01_3.h
SQLTab_SysUpdateCounterWanted.hpp inc=:SAPDB/SQLManager/SQLMan_Types.hpp,\
    :SAPDB/FileDirectory/FileDir_FileNo.hpp,\
    :SAPDB/Container/Container_Vector.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_Types.hpp
SQLTab_SysUpgradeHistory.cpp inc=\
    :SAPDB/SQLManager/Tables/SQLTab_SysUpgradeHistory.hpp,\
    :SAPDB/SQLManager/SQLMan_Context.hpp,\
    :SAPDB/SQLManager/SQLMan_SQLStatementContext.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_Connection.hpp,\
    :SAPDB/SQLManager/KernelSQL/KSQL_PreparedStatement.hpp,\
    :SAPDB/KernelCommon/Kernel_IAdminConfig.hpp,\
    :SAPDB/KernelCommon/Kernel_Migration.hpp,\
    :SAPDB/SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp,\
    :SAPDB/SAPDBCommon/Messages/SDBMsg_KernelCommon.h,\
    :SAPDB/RunTime/RTE_Message.hpp,hgg11.h,hak362.h
SQLTab_SysUpgradeHistory.hpp inc=

