/*!
  @file           Catalog_TableDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about a table

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

#ifndef CATALOG_TABLEDESCRIPTOR_HPP
#define CATALOG_TABLEDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_ColumnDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_IndexIterator.hpp"
#include "SQLManager/Catalog/Catalog_IndexDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_TableStatisticsDescriptor.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "Container/Container_Vector.hpp"

class SQLMan_Context;
class Catalog_TableObject;
struct tak_privilege;

/*!
   @brief defines a descriptor which allows to get information about tables stored in the catalog
 */
class Catalog_TableDescriptor
{
private:
    enum AllReturnCodes
    {
        rcOk = 0,
        rcFileNotFound,
        rcSystemError
    };
public :
    /*! 
	   @brief defines a vector containing external column numbers of the table described by the table descriptor
     */
	typedef Container_Vector<SAPDB_Int2> ColumnNoVector;
    /*!
       @brief iterates over all columns of a table (in definition order).
              Example :
              Catalog_TableDescriptor descriptor("DBA","DUAL");
              if (descriptor)
              {
                  for (Catalog_TableDescriptor::ColumnIterator iter = descriptor.Begin(); iter != descriptor.End(); ++iter)
                  {
                      int dataLength = (*iter).GetDataLength();
                  }
              }
     */
    class ColumnIterator 
    {
    public :
        /*!
           @brief steps to the next column
         */
        void operator++();
        /*!
          @brief true, if the iterator points to a column; otherwise false
         */
        operator bool() const;
        /*!
           @brief returns true, if 2 iterators are equal
         */
        bool operator== (const ColumnIterator& Compare) const;
        /*!
           @brief returns true, if 2 iterators are not equal
         */
        bool operator!= (const ColumnIterator& Compare) const;
        /*!
          @brief yields a column descriptor for the current column
         */
        const Catalog_ColumnDescriptor operator*() const;
    private :
        /*!
           @brief constructor
         */
        ColumnIterator(const Catalog_TableDescriptor& TableDesc, int First);
        int                            m_current;   //!< current column index
        const Catalog_TableDescriptor& m_tableDesc; //!< table reference
        friend class Catalog_TableDescriptor;
    };
    /*!
       @brief iterates over all key columns of a table (in definition order).
              Example :
              Catalog_TableDescriptor descriptor("DBA","DUAL");
              if (descriptor)
              {
                  for (Catalog_TableDescriptor::KeyColumnIterator iter = descriptor.BeginKey(); iter != descriptor.KeyEnd(); ++iter)
                  {
                      int dataLength = (*iter).GetDataLength();
                  }
              }
     */
    class KeyColumnIterator 
    {
    public :
        /*!
           @brief steps to the next key column
         */
        void operator++();
        /*!
          @brief true, if the iterator points to a column; otherwise false
         */
        operator bool() const;
        /*!
           @brief returns true, if 2 iterators are equal
         */
        bool operator== (const KeyColumnIterator& Compare) const;
        /*!
           @brief returns true, if 2 iterators are not equal
         */
        bool operator!= (const KeyColumnIterator& Compare) const;
        /*!
          @brief yields a column descriptor for the current column
         */
        const Catalog_ColumnDescriptor operator*() const;
    private :
        /*!
           @brief constructor
         */
        KeyColumnIterator(const Catalog_TableDescriptor& TableDesc, int First);
        int                            m_current;   //!< current column index
        const Catalog_TableDescriptor& m_tableDesc; //!< table reference
        friend class Catalog_TableDescriptor;
    };


    ///*!
    //   @brief iterates over all columns of a table (in definition order) that are
    //          shown in "select * from table".
    //          Example :
    //          Catalog_TableDescriptor descriptor("DBA","DUAL");
    //          if (descriptor)
    //          {
    //              for (Catalog_TableDescriptor::ColumnIteratorForStarResolving iter = descriptor.BeginKey(); iter != descriptor.KeyEnd(); ++iter)
    //              {
    //                  int dataLength = (*iter).GetDataLength();
    //              }
    //          }
    // */
    //class ColumnIteratorForStarResolving
    //{
    //public :
    //    /*!
    //       @brief steps to the next key column
    //     */
    //    void operator++();
    //    /*!
    //      @brief true, if the iterator points to a column; otherwise false
    //     */
    //    operator bool() const;
    //    /*!
    //       @brief returns true, if 2 iterators are equal
    //     */
    //    bool operator== (const ColumnIteratorForStarResolving& Compare) const;
    //    /*!
    //       @brief returns true, if 2 iterators are not equal
    //     */
    //    bool operator!= (const ColumnIteratorForStarResolving& Compare) const;
    //    /*!
    //      @brief yields a column descriptor for the current column
    //     */
    //    const Catalog_ColumnDescriptor operator*() const;
    //private :
    //    /*!
    //       @brief constructor
    //     */
    //    ColumnIteratorForStarResolving(const Catalog_TableDescriptor& TableDesc, int First, int Last);
    //    void find();
    //    int                            m_current;   //!< current column index
    //    int                            m_endpos;    //!< bmaxcol
    //    const Catalog_TableDescriptor& m_tableDesc; //!< table reference
    //    friend class Catalog_TableDescriptor;
    //};

    /*!
       @brief default constructor
     */
    Catalog_TableDescriptor();
    /*!
       @brief copy constructor
     */
    Catalog_TableDescriptor(const Catalog_TableDescriptor& Source);
    /*!
       @brief contructor. Creates a descriptor for the table or view identified by Schema and TableName. If the table
              does not exist, the descriptor is invalid (operator bool returns false).
              If some unexpected error occurred, it is recorded in the Contect (sql code + error list)
              The destructor considers the MaxDB name resolution, i.e. if no Schema is given we look in the
              current schema first, then in the schema of the dba etc.
              All privileges returned by the descriptor are relative to the current user 
     */
    Catalog_TableDescriptor (SQLMan_Context& Context, 
        const SQLMan_Identifier& Schema, 
        const SQLMan_Identifier& TableName, 
        bool                     ConsiderResultTables, 
        bool                     ConsiderSYSDDTables);

     /*!
       @brief contructor. Creates a descriptor for the table or view identified by Schema and TableName. If the table
              does not exist, the descriptor is invalid (operator bool returns false).
              If some unexpected error occurred, it is recorded in the Contect (sql code + error list)
              The destructor considers the MaxDB name resolution, i.e. if no Schema is given we look in the
              current schema first, then in the schema of the dba etc.
              All privileges returned by the descriptor are relative to the given authorization CurrentAuthorizationId 
     */
    Catalog_TableDescriptor (SQLMan_Context& Context, 
        const SQLMan_Identifier& Schema, 
        const SQLMan_Identifier& TableName,
        const Catalog_ObjectId&  CurrentAuthorizationId,
        bool                     ConsiderResultTables, 
        bool                     ConsiderSYSDDTables);

    /*!
       @brief constructor, yields a table description for a given table identification
     */
    Catalog_TableDescriptor (SQLMan_Context& Context, const Catalog_ObjectId& TableId);  
    /*!
       @brief constructor, yields a table description for a given file no.
     */
    Catalog_TableDescriptor (SQLMan_Context& Context, const FileDir_FileNo& FileNo);
    /*!
       @brief destructor, releases referenc to catalog
     */
    virtual ~Catalog_TableDescriptor();
    /*!
       @brief returns true, if at least one index is defined on the table
     */
    bool AnyIndexExists() const;
    /*!
       @brief yields an iterator pointing to the first table column
     */
    ColumnIterator Begin() const;
    ///*!
    //   @brief yields an iterator for '*' resolving
    // */
    //ColumnIteratorForStarResolving BeginStarResolving() const;
    /*!
       @brief yields an iterator pointing to the first key column
    */
    KeyColumnIterator BeginKey() const;
    /*!
       @brief yields an iterator pointing behind the last table column
     */
    ColumnIterator End() const;
    ///*!
    //   @brief yields an star resolving iterator pointing behind the last table column
    // */
    //ColumnIteratorForStarResolving EndStarResolving() const;
    /*!
       @brief yields an iterator pointing behind the last key column
     */
    KeyColumnIterator EndKey() const;
    /*!
       @brief yields an iterator pointing to the required column. If the column does not exist,
              the iterator is invalid (operator bool returns false)
     */
    ColumnIterator FindColumn(const SQLMan_Identifier& ColumnName) const;
    /*!
       @brief searches for a column using the iterator and returns true, if the column exists
     */
    SAPDB_Bool ContainsColumn(const SQLMan_Identifier& ColumnName) const;

	//SAPDB_Int2 ExtColNo2LogicalColNo( SAPDB_Int2 ExtColNo ) const;

	//SAPDB_Int2 LogicalColNo2ExtColNo( SAPDB_Int2 LogicalColNo ) const;

    /*!
       @brief returns the identification of the authorization holding the descriptor
     */
    Catalog_ObjectId GetAuthorizationId() const;
     /*!
       @brief yields an iterator pointing to the requested column. If
       the column does not exist the iterator is invalid on return (operator
       bool returns false)
    */
    ColumnIterator GetColumn(const int ColumnNumber) const;
    /*!
      @brief yields the file id of the table
     */
    void GetFileNo (FileDir_FileNo& FileNo) const;
    /*!
       @brief yields the table owner 
     */
    void GetOwnerName (SQLMan_Identifier& Owner) const;
    /*!
       @brief yields the table owner 
     */
    SQLMan_Identifier GetOwnerName () const;
    Catalog_ObjectId  GetOwnerId() const;
    /*!
       @brief yields the schema name 
     */
    void GetSchemaName (SQLMan_Identifier& SchemaName) const;
    /*!
       @brief yields the schema name. 
     */
    SQLMan_Identifier GetSchemaName () const;
    /*!
       @brief returns the schema identification
     */
    Catalog_ObjectId GetSchemaId() const;
    /*!
       @brief yields the table name 
     */
    void GetTableName (SQLMan_Identifier& TableName) const;
    /*!
       @brief yields the table name 
     */
    SQLMan_Identifier GetTableName () const;
    /*!
       @brief returns the table identification
     */
    Catalog_ObjectId GetTableId() const;
    /*!
       @brief returns the table file no
     */
    FileDir_FileNo GetTableFileNo() const;
    /*!
       @brief yields the number of columns of the table incl. dropped ones
     */
    int GetColumnCount() const;
    /*!
       @brief yields the number of key columns of the table
     */
    int GetKeyColumnCount() const;
    //  /*!
    //   @brief yields the number of columns of the table excl. dropped / invisible ones 
    // */
    //int GetColumnCountForStarResolving() const;
    /*!
       @brief yields information about an index
     */
    Catalog_IndexDescriptor GetIndexDescriptor(int IndexId) const;
    /*!
       @brief yields an iterator over all indexes of the table
     */
    Catalog_IndexIterator GetIndexIterator(bool WithKey = false);
    /*!
       @brief returns the number of rows or the percentage of rows to
       sample; if the return value is > 0 then it is the number of
       rows to sample, if it is < 0 it is the percentage of rows to
       sample
     */
    SAPDB_Int GetStatisticsSamplingInfo() const;
    /*!
       @brief yields a descriptor for table statistics
       @param ForUpdate [in] true, the descriptor may be used for statistics modifications
       @param Descriptor [out] the table statistics descriptor
       @return true, if the descriptor is valid; otherwise false
     */
    bool GetTableStatisticsDescriptor(bool ForUpdate, Catalog_TableStatisticsDescriptor& Descriptor) const;
    /*!
      @brief returns the average row length of the table
    */
    SAPDB_Int GetAverageRowLength() const;
    /*!
       @brief returns true, if the current user has any privilege for the table
     */
    bool HasAnyPrivilege() const;
     /*!
       @brief returns true, if the current user has the select privilege for at least one column of the table
     */
    bool HasAnySelectPrivilege() const;
     /*!
       @brief returns true, if the current user has the select privilege for all columns of the table
     */
    bool HasAllSelectPrivileges() const;
      /*!
       @brief returns true, if the current user has the select privilege for the given column
     */
    bool HasSelectPrivilege(const Catalog_ColumnDescriptor& ColumnDescriptor) const;
     /*!
      @brief returns true, if the descriptor describes a table having a user defined key. 
     */
    bool HasUserDefinedKey() const;
    /*!
       @brief returns true, if the descriptor decribes a table having a key extension for clustered table
     */
    bool HasVirtualKey() const;
    /*!
       @brief returns true, if the descriptor decribes a complex view
     */
    bool IsComplexView() const;
    /*!
      @brief returns true, if the given name and the decribed table name are equal
     */
    bool IsEqualTablename(const SQLMan_Identifier& TableName) const;
    /*!
      @brief returns true, if the given schema and table name and the decribed table name are equal
     */
    bool IsEqualTable(const SQLMan_Identifier& Schema, const SQLMan_Identifier& TableName) const;
    /*!
       @brief returns true, if the descriptor decribes a join view
     */
    bool IsJoinView() const;
    /*!
       @brief returns true, if the descriptor decribes a result table (cursor)
     */
    bool IsResultTable() const;
    /*!
      @brief returns true, if the descriptor describes an archive table
    */
    bool IsArchiveTable() const;
    /*!
       @brief returns true, if the descriptor decribes a view
     */
    bool IsView() const;
    /*!
       @brief returns true, if the descriptor decribes a synonym
     */
    bool IsSynonym() const;
     /*!
       @brief returns true, if the descriptor decribes a temporary table
     */
    bool IsTempTable() const;
     /*!
       @brief returns true, if the descriptor decribes a catalog table
     */
    bool IsCatalogTable() const;
     /*!
       @brief returns true, if the given column collection guarantees unique data
     */
	bool IsUniqueColumnSet(const ColumnNoVector& columnSet);
    /*!
       @brief determines the value of column ColumnName for of given row of the table described by the descriptor.
     */
    void LocateColumnValue (const SQLMan_Identifier& ColumnName, const tgg00_Rec& Row, const SAPDB_Byte* &pValue, int &ValueLength) const;
    /*!
       @brief true, if the table contains the SYSKEY
     */
	bool HasSysKey() const;
    /*!
       @brief true, if the decriptor has been contructed by a synonym 
     */
    bool ReferencedViaSynonym() const;
    /*!
       @brief true, if the descriptor describes a table; false otherwise
     */
    operator bool() const;
    /*!
       @brief assignment
     */
    void operator= (const Catalog_TableDescriptor& Source);
    /*!
       @brief possible return codes of method BuildFileId
    */
    enum BuildFileIdRc
    {
        bfRcOk           = rcOk,
        bfRcFileNotFound = rcFileNotFound,
        bfRcSystemError  = rcSystemError
    };
    /*!
       @brief prepares a tgg00_FileId for this table
       @return
         - bfRcOk, on success
         - bfRcFileNotFound, if no entry for the index could be found in
           the file directory
         - bfRcSystemError, on system error; errMsg will be set
    */
    BuildFileIdRc BuildFileId(
        SQLMan_FileId& fileId,
        Msg_List&      errMsg) const;
    /*!
       @brief possible return codes of methods Get...Count
    */
    enum GetFileCounterRc
    {
        gfcRcOk           = rcOk,
        gfcRcFileNotFound = rcFileNotFound,
        gfcRcSystemError  = rcSystemError
    };
    /*!
       @brief returns the current number of records stored in the
       table; this can be expensive if the file directory counters
       have not yet been initialized
       @return
         - gfcRcOk, on success
         - gfcRcFileNotFound, if no entry for the index could be found in
           the file directory
         - gfcRcSystemError, on system error; errMsg will be set
    */
    GetFileCounterRc GetRecordCount(
        SAPDB_Int& recordCount,
        Msg_List&  errMsg) const;
    /*!
       @brief returns the current number of leaf pages of the
       table; this can be expensive if the file directory counters
       have not yet been initialized
       @return
         - gfcRcOk, on success
         - gfcRcFileNotFound, if no entry for the index could be found in
           the file directory
         - gfcRcSystemError, on system error; errMsg will be set
    */
    GetFileCounterRc GetLeafPageCount(
        SAPDB_Int& leafPageCount,
        Msg_List&  errMsg) const;
    /*!
       @brief returns the current number of leaf pages and records
       stored in the table; this can be expensive if the file
       directory counters have not yet been initialized

       This method should be preferred over calling GetRecordCount and
       GetLeafPageCount if both values should be determined.

       @return
         - gfcRcOk, on success
         - gfcRcFileNotFound, if no entry for the index could be found in
           the file directory
         - gfcRcSystemError, on system error; errMsg will be set
    */
    GetFileCounterRc GetLeafPageAndRecordCount(
        SAPDB_Int& leafPageCount,
        SAPDB_Int& recordCount,
        Msg_List&  errMsg) const;
    /*!
	   @brief returns the current state of the flag
     */
	bool GetSysUpdStatWantedInsertedFlag () const;
	/*!
	   @brief sets the flag according to the parameter doSet. The flag indicates whether an insert into the table SYSUPDSTATWANTED has already been made
	          for the table identified by the descriptor.
     */
	void SetSysUpdStatWantedInsertedFlag (bool DoSet);
protected :
    /*!
       @brief checks if the descriptor is valid. Sets an error into the context, if the descriptor is invalid
     */
    static bool TableValid (SQLMan_Context* pContext, Catalog_TableObject* pTable);
    /*!
       @brief returns all privileges of the current user for this table
     */
    void GetPriv (tak_privilege& Priv, bool selectRequiredOnly) const;
    void Release ();
    SQLMan_Context*      m_pContext;               //!< pointer to the sql manager context
    Catalog_TableObject* m_pTable;                 //!< pointer to the catalog table object describing the table
    Catalog_ObjectId     m_synonym;                //!< surrogate of the synonym, if present
    Catalog_ObjectId     m_currentAuthorisationId; //!< authorization of the user looking to the table
    friend class ColumnIterator;     //!< allow ColumnIterator access to a table descriptor
    friend class KeyColumnIterator;  //!< allow KeyColumnIterator access to a table descriptor
//    friend class ColumnIteratorForStarResolving; //!< allow ColumnIteratorForStarResolving access to a table descriptor
};

#endif
