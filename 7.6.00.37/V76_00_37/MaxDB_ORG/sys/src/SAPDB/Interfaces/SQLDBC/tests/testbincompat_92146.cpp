#define SQLDBC_H_MD5 a64ef4927756ba2194a3e3a24a802712
#ifdef VMAKE
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#else
#include "SQLDBC.h" /*nocheck*/
#endif

using namespace SQLDBC;

int main(int argc, char **argv)
{
    {
        ::SQLDBC::SQLDBC_ErrorHndl* var_1 = new ::SQLDBC::SQLDBC_ErrorHndl();
        delete var_1;
    }
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_ErrorHndl::* var_2)() const  =  &::SQLDBC::SQLDBC_ErrorHndl::getErrorCode;
    const char * (::SQLDBC::SQLDBC_ErrorHndl::* var_3)() const  =  &::SQLDBC::SQLDBC_ErrorHndl::getSQLState;
    const char * (::SQLDBC::SQLDBC_ErrorHndl::* var_4)() const  =  &::SQLDBC::SQLDBC_ErrorHndl::getErrorText;
    SQLDBC_Bool (::SQLDBC::SQLDBC_ErrorHndl::* var_5)() const  =  &::SQLDBC::SQLDBC_ErrorHndl::operator SQLDBC_Bool ;
    SQLDBC_SQLWarning * (::SQLDBC::SQLDBC_SQLWarning::* var_6)() const  =  &::SQLDBC::SQLDBC_SQLWarning::getNextWarning;
    SQLDBC_SQLWarningCode (::SQLDBC::SQLDBC_SQLWarning::* var_7)() const  =  &::SQLDBC::SQLDBC_SQLWarning::getWarningCode;
    SQLDBC_ErrorHndl & (::SQLDBC::SQLDBC_ConnectionItem::* var_8)()  =  &::SQLDBC::SQLDBC_ConnectionItem::error;
    void (::SQLDBC::SQLDBC_ConnectionItem::* var_9)()  =  &::SQLDBC::SQLDBC_ConnectionItem::clearError;
    SQLDBC_SQLWarning * (::SQLDBC::SQLDBC_ConnectionItem::* var_10)()  =  &::SQLDBC::SQLDBC_ConnectionItem::warning;
    void (::SQLDBC::SQLDBC_ConnectionItem::* var_11)()  =  &::SQLDBC::SQLDBC_ConnectionItem::clearWarnings;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_LOB::* var_12)(void * , SQLDBC_Length * )  =  &::SQLDBC::SQLDBC_LOB::putData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_LOB::* var_13)()  =  &::SQLDBC::SQLDBC_LOB::close;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_LOB::* var_14)(void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_LOB::getData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_LOB::* var_15)(void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_LOB::getData;
    SQLDBC_Length (::SQLDBC::SQLDBC_LOB::* var_16)()  =  &::SQLDBC::SQLDBC_LOB::getLength;
    SQLDBC_Length (::SQLDBC::SQLDBC_LOB::* var_17)()  =  &::SQLDBC::SQLDBC_LOB::getPosition;
    SQLDBC_Length (::SQLDBC::SQLDBC_LOB::* var_18)()  =  &::SQLDBC::SQLDBC_LOB::getPreferredDataSize;
    SQLDBC_Int2 (::SQLDBC::SQLDBC_ParameterMetaData::* var_19)()  =  &::SQLDBC::SQLDBC_ParameterMetaData::getParameterCount;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ParameterMetaData::* var_20)(SQLDBC_Int2 , char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_ParameterMetaData::getParameterName;
    SQLDBC_SQLType (::SQLDBC::SQLDBC_ParameterMetaData::* var_21)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getParameterType;
    SQLDBC_ParameterMetaData::ParameterMode (::SQLDBC::SQLDBC_ParameterMetaData::* var_22)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getParameterMode;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ParameterMetaData::* var_23)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getParameterLength;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ParameterMetaData::* var_24)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getPrecision;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ParameterMetaData::* var_25)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getScale;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ParameterMetaData::* var_26)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::getPhysicalLength;
    SQLDBC_ParameterMetaData::ParameterNullBehavior (::SQLDBC::SQLDBC_ParameterMetaData::* var_27)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ParameterMetaData::isNullable;
    SQLDBC_Int2 (::SQLDBC::SQLDBC_ResultSetMetaData::* var_28)()  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getColumnCount;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSetMetaData::* var_29)(SQLDBC_Int2 , char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getColumnName;
    SQLDBC_SQLType (::SQLDBC::SQLDBC_ResultSetMetaData::* var_30)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getColumnType;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSetMetaData::* var_31)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getColumnLength;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSetMetaData::* var_32)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getPrecision;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSetMetaData::* var_33)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getScale;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSetMetaData::* var_34)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::getPhysicalLength;
    SQLDBC_ResultSetMetaData::ColumnNullBehavior (::SQLDBC::SQLDBC_ResultSetMetaData::* var_35)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::isNullable;
    SQLDBC_Bool (::SQLDBC::SQLDBC_ResultSetMetaData::* var_36)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSetMetaData::isWritable;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_RowSet::* var_37)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_RowSet::setPos;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_RowSet::* var_38)()  =  &::SQLDBC::SQLDBC_RowSet::fetch;
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_RowSet::* var_39)() const  =  &::SQLDBC::SQLDBC_RowSet::getRowsAffected;
    const SQLDBC_Int4 * (::SQLDBC::SQLDBC_RowSet::* var_40)() const  =  &::SQLDBC::SQLDBC_RowSet::getRowStatus;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_RowSet::* var_41)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_RowSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_RowSet::* var_42)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_RowSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_43)()  =  &::SQLDBC::SQLDBC_UpdatableRowSet::insertAllRows;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_44)()  =  &::SQLDBC::SQLDBC_UpdatableRowSet::insertOneRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_45)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::updateRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_46)(int )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::updateRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_47)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::deleteRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_48)(int )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::deleteRow;
    SQLDBC_ResultSetMetaData * (::SQLDBC::SQLDBC_ResultSet::* var_49)()  =  &::SQLDBC::SQLDBC_ResultSet::getResultSetMetaData;
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSet::* var_50)() const  =  &::SQLDBC::SQLDBC_ResultSet::getResultCount;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_51)(const SQLDBC_UInt4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::bindColumn;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_52)(const SQLDBC_UInt4 , const SQLDBC_HostType , void * , SQLDBC_Length * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::bindColumn;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_53)()  =  &::SQLDBC::SQLDBC_ResultSet::clearColumns;
    void (::SQLDBC::SQLDBC_ResultSet::* var_54)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSet::setFetchSize;
    void (::SQLDBC::SQLDBC_ResultSet::* var_55)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_ResultSet::setRowSetSize;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_ResultSet::* var_56)() const  =  &::SQLDBC::SQLDBC_ResultSet::getRowSetSize;
    SQLDBC_RowSet * (::SQLDBC::SQLDBC_ResultSet::* var_57)()  =  &::SQLDBC::SQLDBC_ResultSet::getRowSet;
    SQLDBC_UpdatableRowSet * (::SQLDBC::SQLDBC_ResultSet::* var_58)()  =  &::SQLDBC::SQLDBC_ResultSet::getUpdatableRowSet;
    SQLDBC_Statement * (::SQLDBC::SQLDBC_ResultSet::* var_59)()  =  &::SQLDBC::SQLDBC_ResultSet::getStatement;
    SQLDBC_Bool (::SQLDBC::SQLDBC_ResultSet::* var_60)()  =  &::SQLDBC::SQLDBC_ResultSet::isUpdatable;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_61)()  =  &::SQLDBC::SQLDBC_ResultSet::first;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_62)()  =  &::SQLDBC::SQLDBC_ResultSet::next;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_63)()  =  &::SQLDBC::SQLDBC_ResultSet::previous;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_64)()  =  &::SQLDBC::SQLDBC_ResultSet::last;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_65)(int )  =  &::SQLDBC::SQLDBC_ResultSet::absolute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_66)(int )  =  &::SQLDBC::SQLDBC_ResultSet::relative;
    void (::SQLDBC::SQLDBC_ResultSet::* var_67)()  =  &::SQLDBC::SQLDBC_ResultSet::close;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_ResultSet::* var_68)() const  =  &::SQLDBC::SQLDBC_ResultSet::getRowNumber;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_69)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_70)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_71)(const char * , const SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::execute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_72)(const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::execute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_73)(const char * )  =  &::SQLDBC::SQLDBC_Statement::execute;
    void (::SQLDBC::SQLDBC_Statement::* var_74)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_Statement::setResultSetFetchSize;
    void (::SQLDBC::SQLDBC_Statement::* var_75)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_Statement::setMaxRows;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_Statement::* var_76)() const  =  &::SQLDBC::SQLDBC_Statement::getMaxRows;
    SQLDBC_ResultSet * (::SQLDBC::SQLDBC_Statement::* var_77)()  =  &::SQLDBC::SQLDBC_Statement::getResultSet;
    void (::SQLDBC::SQLDBC_Statement::* var_78)(const char * , SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::setCursorName;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_79)(const char * , SQLDBC_Length , SQLDBC_Int4 )  =  &::SQLDBC::SQLDBC_Statement::setCommandInfo;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_80)(char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_Statement::getCursorName;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_81)(char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_Statement::getTableName;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Statement::* var_82)() const  =  &::SQLDBC::SQLDBC_Statement::isQuery;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Statement::* var_83)() const  =  &::SQLDBC::SQLDBC_Statement::getKernelVersion;
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_Statement::* var_84)() const  =  &::SQLDBC::SQLDBC_Statement::getRowsAffected;
    void (::SQLDBC::SQLDBC_Statement::* var_85)(SQLDBC_Statement::ResultSetType )  =  &::SQLDBC::SQLDBC_Statement::setResultSetType;
    const SQLDBC_Statement::ResultSetType (::SQLDBC::SQLDBC_Statement::* var_86)() const  =  &::SQLDBC::SQLDBC_Statement::getResultSetType;
    void (::SQLDBC::SQLDBC_Statement::* var_87)(SQLDBC_Statement::ConcurrencyType )  =  &::SQLDBC::SQLDBC_Statement::setResultSetConcurrencyType;
    const SQLDBC_Statement::ConcurrencyType (::SQLDBC::SQLDBC_Statement::* var_88)() const  =  &::SQLDBC::SQLDBC_Statement::getResultSetConcurrencyType;
    const SQLDBC_Int4 * (::SQLDBC::SQLDBC_Statement::* var_89)() const  =  &::SQLDBC::SQLDBC_Statement::getRowStatus;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_Statement::* var_90)() const  =  &::SQLDBC::SQLDBC_Statement::getBatchSize;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_91)(const char * , SQLDBC_Length , SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_92)(const char * , SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_93)(const char * )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_94)()  =  &::SQLDBC::SQLDBC_Statement::executeBatch;
    void (::SQLDBC::SQLDBC_Statement::* var_95)()  =  &::SQLDBC::SQLDBC_Statement::clearBatch;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_96)(SQLDBC_Int4 , SQLDBC_HostType , void * , SQLDBC_Length * , SQLDBC_Length , SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_Statement::getLastInsertedKey;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_97)(const char * , const SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_98)(const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_99)(const char * )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_100)()  =  &::SQLDBC::SQLDBC_PreparedStatement::execute;
    SQLDBC_ParameterMetaData * (::SQLDBC::SQLDBC_PreparedStatement::* var_101)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getParameterMetaData;
    SQLDBC_ResultSetMetaData * (::SQLDBC::SQLDBC_PreparedStatement::* var_102)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getResultSetMetaData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_103)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_PreparedStatement::setBatchSize;
    SQLDBC_UInt4 (::SQLDBC::SQLDBC_PreparedStatement::* var_104)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getPreferredBatchSize;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_105)()  =  &::SQLDBC::SQLDBC_PreparedStatement::clearParameters;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_106)(const SQLDBC_UInt2 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::bindParameter;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_107)(const SQLDBC_UInt2 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::bindParameterAddr;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_108)(SQLDBC_size_t )  =  &::SQLDBC::SQLDBC_PreparedStatement::setBindingType;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_109)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_PreparedStatement::nextParameter;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_110)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_PreparedStatement::nextParameterByIndex;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_111)(void * , SQLDBC_Length * )  =  &::SQLDBC::SQLDBC_PreparedStatement::putData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_112)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_113)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::getObject;
    {
        ::SQLDBC::SQLDBC_ConnectProperties* var_114 = new ::SQLDBC::SQLDBC_ConnectProperties();
        delete var_114;
    }
    {
        ::SQLDBC::SQLDBC_ConnectProperties* var_115 = new ::SQLDBC::SQLDBC_ConnectProperties(::SQLDBC::SQLDBC_ConnectProperties());
        delete var_115;
    }
    void (::SQLDBC::SQLDBC_ConnectProperties::* var_117)(const char * , const char * )  =  &::SQLDBC::SQLDBC_ConnectProperties::setProperty;
    const char * (::SQLDBC::SQLDBC_ConnectProperties::* var_118)(const char * , const char * ) const  =  &::SQLDBC::SQLDBC_ConnectProperties::getProperty;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_119)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , SQLDBC_StringEncoding , SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_120)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_121)(const char * , const char * , const char * , const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_122)(const char * , const char * , const char * , const char * )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_123)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const SQLDBC_StringEncoding , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_124)(const char * , const char * , const char * , const char * , const SQLDBC_StringEncoding , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_125)(const char * , const char * , const char * , const char * , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_126)()  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_127)(const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Statement * (::SQLDBC::SQLDBC_Connection::* var_128)()  =  &::SQLDBC::SQLDBC_Connection::createStatement;
    SQLDBC_PreparedStatement * (::SQLDBC::SQLDBC_Connection::* var_129)()  =  &::SQLDBC::SQLDBC_Connection::createPreparedStatement;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_130)()  =  &::SQLDBC::SQLDBC_Connection::commit;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_131)()  =  &::SQLDBC::SQLDBC_Connection::rollback;
    void (::SQLDBC::SQLDBC_Connection::* var_132)(SQLDBC_Statement * )  =  &::SQLDBC::SQLDBC_Connection::releaseStatement;
    void (::SQLDBC::SQLDBC_Connection::* var_133)(SQLDBC_PreparedStatement * )  =  &::SQLDBC::SQLDBC_Connection::releaseStatement;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_134)()  =  &::SQLDBC::SQLDBC_Connection::cancel;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_135)()  =  &::SQLDBC::SQLDBC_Connection::close;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_136)()  =  &::SQLDBC::SQLDBC_Connection::commitWorkRelease;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_137)()  =  &::SQLDBC::SQLDBC_Connection::disconnect;
    void (::SQLDBC::SQLDBC_Connection::* var_138)(SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_Connection::setAutoCommit;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_139)() const  =  &::SQLDBC::SQLDBC_Connection::getAutoCommit;
    void (::SQLDBC::SQLDBC_Connection::* var_140)(SQLDBC_SQLMode )  =  &::SQLDBC::SQLDBC_Connection::setSQLMode;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_141)(SQLDBC_Int4 )  =  &::SQLDBC::SQLDBC_Connection::setTransactionIsolation;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Connection::* var_142)() const  =  &::SQLDBC::SQLDBC_Connection::getTransactionIsolation;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_143)() const  =  &::SQLDBC::SQLDBC_Connection::isConnected;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Connection::* var_144)() const  =  &::SQLDBC::SQLDBC_Connection::getKernelVersion;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_145)() const  =  &::SQLDBC::SQLDBC_Connection::isUnicodeDatabase;
    SQLDBC_DateTimeFormat :: Format (::SQLDBC::SQLDBC_Connection::* var_146)() const  =  &::SQLDBC::SQLDBC_Connection::getDateTimeFormat;
    {
        ::SQLDBC::SQLDBC_Environment* var_147 = new ::SQLDBC::SQLDBC_Environment(::SQLDBC::GetClientRuntime(0, 0));
        delete var_147;
    }
    SQLDBC_Connection * (::SQLDBC::SQLDBC_Environment::* var_148)() const  =  &::SQLDBC::SQLDBC_Environment::createConnection;
    SQLDBC_Connection * (::SQLDBC::SQLDBC_Environment::* var_149)(SAPDBMem_IRawAllocator & ) const  =  &::SQLDBC::SQLDBC_Environment::createConnection;
    void (::SQLDBC::SQLDBC_Environment::* var_150)(SQLDBC_Connection * ) const  =  &::SQLDBC::SQLDBC_Environment::releaseConnection;
    const char * (::SQLDBC::SQLDBC_Environment::* var_151)()  =  &::SQLDBC::SQLDBC_Environment::getLibraryVersion;
    SAPDBMem_IRawAllocator & (::SQLDBC::SQLDBC_Environment::* var_152)()  =  &::SQLDBC::SQLDBC_Environment::getAllocator;
    void (::SQLDBC::SQLDBC_Environment::* var_153)(const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Environment::setTraceOptions;
    void (::SQLDBC::SQLDBC_Environment::* var_154)(SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Environment::getTraceOptions;
    SQLDBC_IRuntime * (*glob1)(char *, const SQLDBC_Int4) = &GetClientRuntime;
    SQLDBC_IRuntime * (*glob2)(char *, const SQLDBC_Int4) = &GetSingleThreadClientRuntime;
    return 0;
}
