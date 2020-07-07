/*!
  @defgroup Table This is the component for accessing tables.
  The interface is defined in: Table_Table.hpp\n\n
  
  The component test is implemented in Table_Test.cpp
  where you can also find an example to call the interface.
*/
/*!
  @file           Table_Table.hpp
  @ingroup		  Table
  @author         UweH
  @brief          This is the main interface between SQL and Data Manager.
                  The class Table_Table offers operations on one table.

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
#ifndef Table_Table_H
#define Table_Table_H

#include "Table/Table_Types.hpp"

#include "Table/Table_UnsortedResult.hpp"
#include "FileDirectory/FileDir_FileNo.hpp"
#include "KernelCommon/Kernel_IndexDescription.hpp"

class Kernel_BlobDescription;
class Kernel_IndexDescription;
class Msg_List;
class SAPDBFields_IFieldSequence;
class Log_Transaction;

class Table_KeyDescription;
class Table_SortedResult;

/*!
  @class     Table_Table
  @brief     This realizes all access to a single table.
             You can enable the interace methode trace with "CHECK TOPIC TABLE" and level 5.
             Additional checks can be enabled with "TRACE TOPIC TABLE".
 */
class Table_Table
{
public:

    /// This is used to access an existing table. tableId must be valid.
    Table_Table( Log_Transaction &transaction,
                 Table_Id         tableId );

    /// This is used if a new table should be created. Look at Create()
    Table_Table( Log_Transaction &transaction );

    /// returns true if a table id is known
    bool IsValid () const
    {
        return m_TableId.IsValid();
    }

    /// This is used by Create().
    enum CreateResult
    {
        createOk,
        createError,
        createLimitReached,
        CreateResult_MAX
	};
    
    /// for output of the CreateResult
    static const char *CreateResultToString[CreateResult_MAX];
    
    /// This is used by Create().
    enum Visibility
    {
        visibilityGlobal,
        visibilityLocal,
        visibility_MAX
    };

    /// for output of the Visibility
    static const char *VisibilityToString[visibility_MAX];
    
    /// This is used by Create().
    enum RemoveKind
    {
        removeKindNone,
        removeKindEndOfCommand,
        removeKindEndOfTrans,
        removeKind_MAX
    };

    /// for output of the RemoveKind
    static const char *RemoveKindToString[removeKind_MAX];
    
    /*!
        @brief  If createOk is returned a new table identified by newTableId was created.
        @param  visibility [in] is it a local temp table ?
        @param  removeKind [in] is it automatically removed ?
        @param  newTableId [out] 
        @param  errlist [out] some error hints
        @return CreateResult

        If any error occur, all persistent changes done are rollbacked and the newTableId becomes invalid.
        errlist contains the some hints what was going wrong.
     */
    CreateResult Create (Visibility            visibility,
                         RemoveKind            removeKind,
                         Table_Id             &newTableId,
                         Msg_List &errlist );

    /*!
        @brief  If createOk is returned a new table identified by newTableId was created.
        @param  blobDescription [in] optional
        @param  visibility [in] is it a local temp table ?
        @param  removeKind [in] is it automatically removed ?
        @param  newTableId [out] 
        @param  errlist [out] some error hints
        @return CreateResult

        If any error occur, all persistent changes done are rollbacked and the newTableId becomes invalid.
        errlist contains the some hints what was going wrong.
     */
    CreateResult Create (const Kernel_BlobDescription &blobDescription,
                               Visibility              visibility,
                               RemoveKind              removeKind,
                               Table_Id               &newTableId,
                               Msg_List   &errlist );

    /// This is used by Drop()
    enum DropResult
    {
        dropOk,
        dropError
    };
    
    /*!
        @brief  If dropOk is returned the table is removed.
        @param  errlist [out] some error hints
        @return DropResult
       
       This removes the associated table (primary file and secondary files).
     */
    DropResult Drop ( Msg_List &errlist );

    /// This is used by AddIndex()
    enum AddIndexResult
    {
        addIndexOk,
        addIndexError
    };
    
    /*!
        @brief This adds an index which is described.
        @param  description [in] index description
        @param  errlist [out] some error hints
        @return AddIndexResult
    */
    AddIndexResult AddIndex ( const Kernel_IndexDescription &description,
                                    Msg_List    &errlist );

    /// This is used by DeleteIndex()
    enum DeleteIndexResult
    {
        deleteIndexOk,
        deleteIndexError
    };
    
    /*!
        @brief This deletes an index which is described by number
        @param  indexno [in]
        @param  errlist [out] some error hints
        @return DeleteIndexResult
    */
    DeleteIndexResult DeleteIndex ( Kernel_IndexDescription::IndexNumber  indexno,
                                    Msg_List                 &errlist );



    /// This is used by Insert()
    enum InsertResult
    {
        insertOk,
        insertError,
        insertDuplicate,
        insertLimitReached
    };
    
    /*!
        @brief This inserts a given fieldsequence into the table.
        @param  fields [in]
        @param  errlist [out] some error hints
        @return InsertResult
    */
    InsertResult Insert ( const SAPDBFields_IFieldSequence &fields,
                                Msg_List       &errlist );

    /*!
    @brief  This select returns an unsorted result set.
    @param  result  [out] 
    @param  errlist [out]
    @return false on any error
    */
    bool Select ( Table_UnsortedResult &result,
                  Msg_List &errlist );

    /// This is used by Select() [2]
    enum SelectResult
    {
        selectOk,
        selectStartKeyNotFound
    };
    
    /*!
    @brief  This simple select returns an unsorted result set.
    @param  result  [out] 
    @param  errlist [out]
    @return false on any error
    */
    SelectResult Select ( Table_KeyDescription &startKey,
                          Table_KeyDescription &stopKey,
                          Table_SortedResult   &result,
                          Msg_List &errlist );

    /// Writes members to knltrace
    void WriteToTrace ( const char * title = 0 );
    
private:

    /// This is the context in which a table can be accessed.
    Log_Transaction &m_Transaction;

    /// This is identifies this table.
    Table_Id m_TableId;
};
#endif  /* Table_Table_H */
