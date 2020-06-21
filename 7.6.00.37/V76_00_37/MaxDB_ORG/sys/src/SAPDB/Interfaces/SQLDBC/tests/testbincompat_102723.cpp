#define SQLDBC_H_MD5 30b548892821c553b68723226be0da3a
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
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_45)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::insertRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_46)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::updateRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_47)(int )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::updateRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_48)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::deleteRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_49)(int )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::deleteRow;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_50)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::nextParameter;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_51)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::nextParameterByIndex;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_UpdatableRowSet::* var_52)(void * , SQLDBC_Length * )  =  &::SQLDBC::SQLDBC_UpdatableRowSet::putData;
    SQLDBC_ResultSetMetaData * (::SQLDBC::SQLDBC_ResultSet::* var_53)()  =  &::SQLDBC::SQLDBC_ResultSet::getResultSetMetaData;
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_ResultSet::* var_54)() const  =  &::SQLDBC::SQLDBC_ResultSet::getResultCount;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_55)(const SQLDBC_UInt4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::bindColumn;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_56)(const SQLDBC_UInt4 , const SQLDBC_HostType , void * , SQLDBC_Length * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::bindColumn;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_57)(SQLDBC_size_t )  =  &::SQLDBC::SQLDBC_ResultSet::setBindingType;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_58)()  =  &::SQLDBC::SQLDBC_ResultSet::clearColumns;
    void (::SQLDBC::SQLDBC_ResultSet::* var_59)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_ResultSet::setFetchSize;
    void (::SQLDBC::SQLDBC_ResultSet::* var_60)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_ResultSet::setRowSetSize;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_ResultSet::* var_61)() const  =  &::SQLDBC::SQLDBC_ResultSet::getRowSetSize;
    SQLDBC_RowSet * (::SQLDBC::SQLDBC_ResultSet::* var_62)()  =  &::SQLDBC::SQLDBC_ResultSet::getRowSet;
    SQLDBC_UpdatableRowSet * (::SQLDBC::SQLDBC_ResultSet::* var_63)()  =  &::SQLDBC::SQLDBC_ResultSet::getUpdatableRowSet;
    SQLDBC_Statement * (::SQLDBC::SQLDBC_ResultSet::* var_64)()  =  &::SQLDBC::SQLDBC_ResultSet::getStatement;
    SQLDBC_Bool (::SQLDBC::SQLDBC_ResultSet::* var_65)()  =  &::SQLDBC::SQLDBC_ResultSet::isUpdatable;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_66)()  =  &::SQLDBC::SQLDBC_ResultSet::first;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_67)()  =  &::SQLDBC::SQLDBC_ResultSet::next;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_68)()  =  &::SQLDBC::SQLDBC_ResultSet::previous;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_69)()  =  &::SQLDBC::SQLDBC_ResultSet::last;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_70)(int )  =  &::SQLDBC::SQLDBC_ResultSet::absolute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_71)(int )  =  &::SQLDBC::SQLDBC_ResultSet::relative;
    void (::SQLDBC::SQLDBC_ResultSet::* var_72)()  =  &::SQLDBC::SQLDBC_ResultSet::close;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_ResultSet::* var_73)() const  =  &::SQLDBC::SQLDBC_ResultSet::getRowNumber;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_74)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_ResultSet::* var_75)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_ResultSet::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_76)(const char * , const SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::execute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_77)(const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::execute;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_78)(const char * )  =  &::SQLDBC::SQLDBC_Statement::execute;
    void (::SQLDBC::SQLDBC_Statement::* var_79)(SQLDBC_Int2 )  =  &::SQLDBC::SQLDBC_Statement::setResultSetFetchSize;
    void (::SQLDBC::SQLDBC_Statement::* var_80)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_Statement::setMaxRows;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_Statement::* var_81)() const  =  &::SQLDBC::SQLDBC_Statement::getMaxRows;
    SQLDBC_ResultSet * (::SQLDBC::SQLDBC_Statement::* var_82)()  =  &::SQLDBC::SQLDBC_Statement::getResultSet;
    void (::SQLDBC::SQLDBC_Statement::* var_83)(const char * , SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::setCursorName;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_84)(const char * , SQLDBC_Length , SQLDBC_Int4 )  =  &::SQLDBC::SQLDBC_Statement::setCommandInfo;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_85)(char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_Statement::getCursorName;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_86)(char * , const SQLDBC_StringEncoding , const SQLDBC_Length , SQLDBC_Length * ) const  =  &::SQLDBC::SQLDBC_Statement::getTableName;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Statement::* var_87)() const  =  &::SQLDBC::SQLDBC_Statement::isQuery;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Statement::* var_88)() const  =  &::SQLDBC::SQLDBC_Statement::getKernelVersion;
    const SQLDBC_Int4 (::SQLDBC::SQLDBC_Statement::* var_89)() const  =  &::SQLDBC::SQLDBC_Statement::getRowsAffected;
    void (::SQLDBC::SQLDBC_Statement::* var_90)(SQLDBC_Statement::ResultSetType )  =  &::SQLDBC::SQLDBC_Statement::setResultSetType;
    const SQLDBC_Statement::ResultSetType (::SQLDBC::SQLDBC_Statement::* var_91)() const  =  &::SQLDBC::SQLDBC_Statement::getResultSetType;
    void (::SQLDBC::SQLDBC_Statement::* var_92)(SQLDBC_Statement::ConcurrencyType )  =  &::SQLDBC::SQLDBC_Statement::setResultSetConcurrencyType;
    const SQLDBC_Statement::ConcurrencyType (::SQLDBC::SQLDBC_Statement::* var_93)() const  =  &::SQLDBC::SQLDBC_Statement::getResultSetConcurrencyType;
    const SQLDBC_Int4 * (::SQLDBC::SQLDBC_Statement::* var_94)() const  =  &::SQLDBC::SQLDBC_Statement::getRowStatus;
    const SQLDBC_UInt4 (::SQLDBC::SQLDBC_Statement::* var_95)() const  =  &::SQLDBC::SQLDBC_Statement::getBatchSize;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_96)(const char * , SQLDBC_Length , SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_97)(const char * , SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_98)(const char * )  =  &::SQLDBC::SQLDBC_Statement::addBatch;
    const SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_99)()  =  &::SQLDBC::SQLDBC_Statement::executeBatch;
    void (::SQLDBC::SQLDBC_Statement::* var_100)()  =  &::SQLDBC::SQLDBC_Statement::clearBatch;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Statement::* var_101)(SQLDBC_Int4 , SQLDBC_HostType , void * , SQLDBC_Length * , SQLDBC_Length , SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_Statement::getLastInsertedKey;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_102)(const char * , const SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_103)(const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_104)(const char * )  =  &::SQLDBC::SQLDBC_PreparedStatement::prepare;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_105)()  =  &::SQLDBC::SQLDBC_PreparedStatement::execute;
    SQLDBC_ParameterMetaData * (::SQLDBC::SQLDBC_PreparedStatement::* var_106)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getParameterMetaData;
    SQLDBC_ResultSetMetaData * (::SQLDBC::SQLDBC_PreparedStatement::* var_107)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getResultSetMetaData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_108)(SQLDBC_UInt4 )  =  &::SQLDBC::SQLDBC_PreparedStatement::setBatchSize;
    SQLDBC_UInt4 (::SQLDBC::SQLDBC_PreparedStatement::* var_109)()  =  &::SQLDBC::SQLDBC_PreparedStatement::getPreferredBatchSize;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_110)()  =  &::SQLDBC::SQLDBC_PreparedStatement::clearParameters;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_111)(const SQLDBC_UInt2 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::bindParameter;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_112)(const SQLDBC_UInt2 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::bindParameterAddr;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_113)(SQLDBC_size_t )  =  &::SQLDBC::SQLDBC_PreparedStatement::setBindingType;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_114)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_PreparedStatement::nextParameter;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_115)(SQLDBC_Int2 & , void * & )  =  &::SQLDBC::SQLDBC_PreparedStatement::nextParameterByIndex;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_116)(void * , SQLDBC_Length * )  =  &::SQLDBC::SQLDBC_PreparedStatement::putData;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_117)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::getObject;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_PreparedStatement::* var_118)(const SQLDBC_Int4 , const SQLDBC_HostType , void * , SQLDBC_Length * , const SQLDBC_Length , SQLDBC_Length , const SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_PreparedStatement::getObject;
    {
        ::SQLDBC::SQLDBC_ConnectProperties* var_119 = new ::SQLDBC::SQLDBC_ConnectProperties();
        delete var_119;
    }
    {
        ::SQLDBC::SQLDBC_ConnectProperties* var_120 = new ::SQLDBC::SQLDBC_ConnectProperties(::SQLDBC::SQLDBC_ConnectProperties());
        delete var_120;
    }
    void (::SQLDBC::SQLDBC_ConnectProperties::* var_122)(const char * , const char * )  =  &::SQLDBC::SQLDBC_ConnectProperties::setProperty;
    const char * (::SQLDBC::SQLDBC_ConnectProperties::* var_123)(const char * , const char * ) const  =  &::SQLDBC::SQLDBC_ConnectProperties::getProperty;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_124)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , SQLDBC_StringEncoding , SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_125)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_126)(const char * , const char * , const char * , const char * , const SQLDBC_StringEncoding )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_127)(const char * , const char * , const char * , const char * )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_128)(const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const char * , SQLDBC_Length , const SQLDBC_StringEncoding , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_129)(const char * , const char * , const char * , const char * , const SQLDBC_StringEncoding , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_130)(const char * , const char * , const char * , const char * , const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_131)()  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_132)(const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Connection::connect;
    SQLDBC_Statement * (::SQLDBC::SQLDBC_Connection::* var_133)()  =  &::SQLDBC::SQLDBC_Connection::createStatement;
    SQLDBC_PreparedStatement * (::SQLDBC::SQLDBC_Connection::* var_134)()  =  &::SQLDBC::SQLDBC_Connection::createPreparedStatement;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_135)()  =  &::SQLDBC::SQLDBC_Connection::commit;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_136)()  =  &::SQLDBC::SQLDBC_Connection::rollback;
    void (::SQLDBC::SQLDBC_Connection::* var_137)(SQLDBC_Statement * )  =  &::SQLDBC::SQLDBC_Connection::releaseStatement;
    void (::SQLDBC::SQLDBC_Connection::* var_138)(SQLDBC_PreparedStatement * )  =  &::SQLDBC::SQLDBC_Connection::releaseStatement;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_139)()  =  &::SQLDBC::SQLDBC_Connection::cancel;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_140)()  =  &::SQLDBC::SQLDBC_Connection::close;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_141)()  =  &::SQLDBC::SQLDBC_Connection::commitWorkRelease;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_142)()  =  &::SQLDBC::SQLDBC_Connection::disconnect;
    void (::SQLDBC::SQLDBC_Connection::* var_143)(SQLDBC_Bool )  =  &::SQLDBC::SQLDBC_Connection::setAutoCommit;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_144)() const  =  &::SQLDBC::SQLDBC_Connection::getAutoCommit;
    void (::SQLDBC::SQLDBC_Connection::* var_145)(SQLDBC_SQLMode )  =  &::SQLDBC::SQLDBC_Connection::setSQLMode;
    SQLDBC_Retcode (::SQLDBC::SQLDBC_Connection::* var_146)(SQLDBC_Int4 )  =  &::SQLDBC::SQLDBC_Connection::setTransactionIsolation;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Connection::* var_147)() const  =  &::SQLDBC::SQLDBC_Connection::getTransactionIsolation;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_148)() const  =  &::SQLDBC::SQLDBC_Connection::isConnected;
    SQLDBC_Int4 (::SQLDBC::SQLDBC_Connection::* var_149)() const  =  &::SQLDBC::SQLDBC_Connection::getKernelVersion;
    SQLDBC_Bool (::SQLDBC::SQLDBC_Connection::* var_150)() const  =  &::SQLDBC::SQLDBC_Connection::isUnicodeDatabase;
    SQLDBC_DateTimeFormat :: Format (::SQLDBC::SQLDBC_Connection::* var_151)() const  =  &::SQLDBC::SQLDBC_Connection::getDateTimeFormat;
    {
        ::SQLDBC::SQLDBC_Environment* var_152 = new ::SQLDBC::SQLDBC_Environment(::SQLDBC::GetClientRuntime(0, 0));
        delete var_152;
    }
    SQLDBC_Connection * (::SQLDBC::SQLDBC_Environment::* var_153)() const  =  &::SQLDBC::SQLDBC_Environment::createConnection;
    SQLDBC_Connection * (::SQLDBC::SQLDBC_Environment::* var_154)(SAPDBMem_IRawAllocator & ) const  =  &::SQLDBC::SQLDBC_Environment::createConnection;
    void (::SQLDBC::SQLDBC_Environment::* var_155)(SQLDBC_Connection * ) const  =  &::SQLDBC::SQLDBC_Environment::releaseConnection;
    const char * (::SQLDBC::SQLDBC_Environment::* var_156)()  =  &::SQLDBC::SQLDBC_Environment::getLibraryVersion;
    SAPDBMem_IRawAllocator & (::SQLDBC::SQLDBC_Environment::* var_157)()  =  &::SQLDBC::SQLDBC_Environment::getAllocator;
    void (::SQLDBC::SQLDBC_Environment::* var_158)(const SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Environment::setTraceOptions;
    void (::SQLDBC::SQLDBC_Environment::* var_159)(SQLDBC_ConnectProperties & )  =  &::SQLDBC::SQLDBC_Environment::getTraceOptions;
    SQLDBC_IRuntime * (*glob1)(char *, const SQLDBC_Int4) = &GetClientRuntime;
    SQLDBC_IRuntime * (*glob2)(char *, const SQLDBC_Int4) = &GetSingleThreadClientRuntime;
    return 0;
}
