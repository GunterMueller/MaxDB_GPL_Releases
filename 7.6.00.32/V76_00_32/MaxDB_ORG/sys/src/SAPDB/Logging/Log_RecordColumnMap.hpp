/*!
  @file           Log_RecordColumnMap.hpp
  @author         JuergenA
  @author         UweH
  @ingroup        Logging
  @brief          defines a class containing the description and the value of fixed/var/longvar columns

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

*/
#ifndef LOG_RECORD_COLUMN_MAP_H
#define LOG_RECORD_COLUMN_MAP_H

#include "Logging/Log_IRecordColumnMap.hpp"

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "ggg00.h"

/// Definition of the different column-types. See also Log_ColumnTypeStrings.
typedef enum
{
        Log_FixKeyColumn,                   ///< a key-column with a fixed size
        Log_VarKeyColumn,                   ///< a key-column with a variable size
        Log_FixColumn,                      ///< a fixed size column
        Log_VarColumn,                      ///< a variable sized column
        Log_VarLongColumn,                  ///< a variable sized LONG column
		// Log_Column, +++ UH do not uncomment until rest of code is updated +++
        Log_ColumnLastEnum,                 ///< last defined enumeration type
        Log_NilColumn                       ///< not definde column-type
} Log_ColumnType;

extern const char* Log_ColumnTypeStrings[Log_NilColumn+1];

/*!
   @class          Log_ColumnEntry
   @brief          one entry of a column list
*/
class Log_ColumnEntry
{
private:

    SAPDB_Int4              m_OffsetOrNumber;
    SAPDB_Int4              m_ValueLength;
    const SAPDB_Byte       *m_pValue;
    SAPDB_Byte             *m_pAllocatedValue;
    SAPDBMem_IRawAllocator *m_pAllocator;
    Log_ColumnEntry        *m_pNext;

public:
    
    Log_ColumnEntry ()
    : m_OffsetOrNumber  (0),
      m_ValueLength     (0),
      m_pValue          (NULL),
      m_pAllocatedValue (NULL),
      m_pNext           (NULL),
      m_pAllocator      (NULL)
    { }

    ~Log_ColumnEntry()
    {
        if ( m_pAllocator != 0 )
        {
            m_pAllocator->Deallocate(m_pAllocatedValue);
        }
    }
    /*!
       @brief          allocate an entry and initializes it
       @param          RawAllocator [in out] used to allocate a value with trailing defined bytes
       @param          ColumnOffset [in] the offset of the fixed column behing the record key
       @param          FixedColumnLen [in] the length of the fixed column contained in the record
       @param          ValueLength [in] the truncated length of the new value
       @param          pValue [in] the pointer to the new value
       @param          Error [out] 
     */
    void AllocateAndInitFixCol (
        SAPDBMem_IRawAllocator &RawAllocator,
        SAPDB_Int4              ColumnOffset,
        SAPDB_Int4              FixedColumnLen,
        SAPDB_Int4              ValueLength,
        const SAPDB_Byte       *pValue,
        tgg00_BasisError       &Error);
    /*!
       @brief          allocate an entry and initializes it as an undef column
       @param          RawAllocator [ref] used to allocate the undef value
       @param          OffsetOrNumber [in] the offset or the number of the column
       @param          FixedColumnLen [in] the length of a fixed column otherwise zero
       @param          Error [out] 
     */
    void AllocateAndInitUndefCol (
        SAPDBMem_IRawAllocator &RawAllocator,
        SAPDB_Int4              OffsetOrNumber,
        SAPDB_Int4              FixedColumnLen,
        tgg00_BasisError       &Error);
	/// returns the length of the column
    SAPDB_Int4 GetLength () const
    { 
        return (m_ValueLength);
    }  

	/// returns the number of the variable column
    SAPDB_Int4 GetNumber () const
    { 
        return (m_OffsetOrNumber);
    }
	/// returns the pointer to the following column entry
    Log_ColumnEntry *GetNext () const
    { 
        return (m_pNext);
    }  
	/// returns the offset of the fixed column behind the key
    SAPDB_Int4 GetOffset () const
    { 
        return (m_OffsetOrNumber);
    }
	/// returns pointer to the new value of the column
    const SAPDB_Byte *GetValue () const
    { 
        return (m_pValue);
    }
    /*!
       @brief          initializes the column Entry without allocation
       @param          OffsetOrNumber [in] the offset or the number of the column
       @param          ValueLength [in] 
       @param          pValue [in] 
     */
    void Init (SAPDB_Int4        OffsetOrNumber,
               SAPDB_Int4        ValueLength,
               const SAPDB_Byte *pValue)
    {
        m_OffsetOrNumber = OffsetOrNumber;
        m_ValueLength    = ValueLength;
        m_pValue         = pValue;
    }
    /*!
       @brief          inserts this entry into the map
       @param          pPrevEntry [in] inserts the entry behind PrevEntry
       @param          pFirstListEntry [in out] 
     */
    void Insert (
        Log_ColumnEntry *pPrevEntry,
        Log_ColumnEntry *&pFirstListEntry);
    /*!
       @return         true, if this entry is greater than the other entry
       @param          OtherEntry [in] 
     */
    bool operator > (const Log_ColumnEntry &OtherEntry) const
    {
        return (m_OffsetOrNumber > OtherEntry.m_OffsetOrNumber);
    }
    /*!
       @brief          removes this entry from the map
       @param          pPrevEntry [in] removes the entry behind PrevEntry
       @param          pFirstListEntry [in out] 
     */
    void Remove (
        Log_ColumnEntry *pPrevEntry,
        Log_ColumnEntry *&pFirstListEntry);
    void WriteToTrace(const char *title = NULL) const;
};
/*!
   @class          Log_ColumnList
   @brief          a double chained list of column entries without any allocation
*/
class Log_ColumnList
{
private:
    
    Log_ColumnEntry *m_pFirst;
    SAPDB_Int4       m_EntryCount;
    SAPDB_Int4       m_SumValueLength;
    Log_ColumnType   m_ColumnType;
    
public:
    
    Log_ColumnList ()
    : m_pFirst         (NULL),
      m_EntryCount     (0),
      m_SumValueLength (0),
      m_ColumnType     (Log_NilColumn)
    { }
	/// returns the type of the columns contained in this list
    Log_ColumnType GetColumnType () const
    {
        return (m_ColumnType);
    }
	/// return the pointer to the first entry of the list
    SAPDB_Int4 GetEntryCount () const
    {
        return (m_EntryCount);
    }
	/// return the pointer to the first entry of the list
    Log_ColumnEntry *GetFirst () const
    {
        return (m_pFirst);
    }
	/// returns the pointer to the first entry of the list
    SAPDB_Int4 GetSumValueLength () const
    {
        return (m_SumValueLength);
    }

    void Init (Log_ColumnType ColumnType)
    {
        m_ColumnType = ColumnType;
    }
    /*!
       @brief          Inserts a column entry in front of the column list (for fixed columns only)
       @param          pEntry [in] the pointer to the entry to be inserted
     */
    void InsertOnTop (Log_ColumnEntry *pEntry)
    {
        pEntry->Insert (NULL, m_pFirst);
    
        ++m_EntryCount;
    
        m_SumValueLength += pEntry->GetLength();
    }
    /*!
       @brief          Inserts a column entry into the sorted column list (for variable columns)
       @param          pEntry [in] the pointer to the entry to be inserted
     */
    void InsertSorted (Log_ColumnEntry *pEntry);
	/// returns true, if the list is empty
    bool IsEmpty () const
    {
        return (NULL == m_pFirst);
    }
    /*!
       @brief          Removes a column entry from the column list
       @param          pEntry [in] 
       @param          pPrevEntry [in] 
     */
    void Remove (Log_ColumnEntry *pEntry,
                 Log_ColumnEntry *pPrevEntry)
    {
        m_SumValueLength -= pEntry->GetLength();
    
        pEntry->Remove (pPrevEntry, m_pFirst);
    
        --m_EntryCount;
    }

    void WriteToTrace(const char *title = NULL) const
    {
        Kernel_VTrace trace;
        if ( title != NULL )
            trace << title << FlushLine;
        trace << "Type: " << Log_ColumnTypeStrings[m_ColumnType] << " "
              << m_EntryCount << " entries, SumValueLength: " << m_SumValueLength
              << FlushLine;
        Log_ColumnEntry *currEntry      = m_pFirst;
        SAPDB_Int        currEntryCount = 0;
        while ( currEntry != NULL
                &&
                currEntryCount <= m_EntryCount )
        {
            currEntry->WriteToTrace();
            currEntry = currEntry->GetNext();
            ++currEntryCount;
        }
    }
};
/*!
   @class          Log_RecordColumnMap
   @brief          a list containing the description and the value of fixed/var/longvar columns
 */
class Log_RecordColumnMap: public Log_IRecordColumnMap, public Log_ReadWriteActionImage
{
    
private:
    
    struct PersistentColEntry
    {
        SAPDB_UInt2 OffsetOrNumber;
        SAPDB_UInt2 ValueLength;
        PersistentColEntry()
        {
            OffsetOrNumber = 0;
            ValueLength = 0;
        }
    };

    struct RecColMapEntryHeader
    {
        SAPDB_Int4     ImageLen;
        SAPDB_Int4     LengthAllValues;
        SAPDB_UInt2    ImageEntries;
        SAPDB_UInt2    FixColCnt;
        SAPDB_UInt2    VarColCnt;
        SAPDB_UInt2    VarLongColCnt;
        RecColMapEntryHeader()
        {
            ImageLen        = 0;
            LengthAllValues = 0;
            ImageEntries    = 0;
            FixColCnt       = 0;
            VarColCnt       = 0;
            VarLongColCnt   = 0;
        }
        void WriteToTrace(const char *title = NULL) const
        {
            Kernel_VTrace trace;
            if ( title != NULL )
                trace << title << FlushLine;
            trace << "ImageLen: "          << ImageLen
                  << ", LengthAllValues: " << LengthAllValues
                  << ", ImageEntries: "    << ImageEntries
                  << FlushLine;
            trace << "FixColCnt: "         << FixColCnt
                  << ", VarColCnt: "       << VarColCnt
                  << ", VarLongColCnt: "   << VarLongColCnt
                  << FlushLine;
        }
    };

    RecColMapEntryHeader    m_EntryHeader;
    SAPDBMem_IRawAllocator *m_pRawAllocator;
    Log_ColumnEntry        *m_pAllocatedMap;
    Log_ColumnList          m_ColList [Log_ColumnLastEnum];
    SAPDB_Int4              m_AllocatedEntries;
    SAPDB_Int4              m_UsedEntries;
    
    SAPDB_Byte             *m_pAllValuesForRead;
    SAPDBMem_IRawAllocator *m_pAllValuesAllocator;

    SAPDB_UInt2             m_PseudoNullValLen;

	/// initializes the persistent entry header
    void CalculatePersistentImage ();
    /*!
       @brief          creates a variable [long] column part using the old record and the column map
       @param          ColList [in out] m_VarColList or m_VarLongColList
       @param          OffsetFollowingOldPart [in out] has to be initialized with FirstVarColOffset
                                             when calling first time to create the Log_VarColumn part
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          Error [out] 
     */
    void CreateAnyVarColPart (
        const Log_ColumnList &ColList,
        SAPDB_Int4           &OffsetFollowingOldPart,
        const tgg00_Rec      *pOldRec,
        tgg00_Rec            *pNewRec,
        SAPDB_Int4            NewRecSize,
        tgg00_BasisError     &Error)     const;
    /*!
       @brief          creates a variable [long] column part using the old record and the column map
       @param          ColList [in out] m_VarColList or m_VarLongColList
       @param          OffsetFollowingOldPart [in out] has to be initialized with FirstVarColOffset
                                             when calling first time to create the Log_VarColumn part
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          BeforeImage [in out] 
       @param          Error [out] 

        Redundant columns are removed from this after image and the before image is created. 
     */
    void CreateAnyVarColPartAndBeforeImage (
        Log_ColumnList      &ColList,
        SAPDB_Int4          &OffsetFollowingOldPart,
        const tgg00_Rec     *pOldRec,
        tgg00_Rec           *pNewRec,
        SAPDB_Int4           NewRecSize,
        Log_RecordColumnMap &BeforeImage,
        tgg00_BasisError    &Error);
    /*!
       @brief          creates the part of fixed columns using the old record and the column map
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          BeforeImage [in out] 
       @param          Error [out] 

        Redundant columns are removed from this after image and the before image is created.
        If a key column is altered, the error e_key_update occurres and a delete of the
        old record and an insert of the created new record has to be triggered.
     */
    void CreateFixColPartAndBeforeImage (
        const tgg00_Rec     *pOldRec,
        tgg00_Rec           *pNewRec,
        SAPDB_Int4           NewRecSize,
        Log_RecordColumnMap &BeforeImage,
        tgg00_BasisError    &Error);
    /*!
       @brief          creates the header, the key and the fixed columns using the old record and the map
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          WithoutVarParts [in] indicates, that the entry does not know anything about var parts
       @param          Error [out] 
     */
    void CreateKeyAndFixColPart (
        const tgg00_Rec  *pOldRec,
        tgg00_Rec        *pNewRec,
        bool              WithoutVarParts,
        tgg00_BasisError &Error)         const;
    /*!
       @brief          creates the part of key columns using the old record and the column map
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          KeyUpdated [out] 
       @param          Error [out] 

        Redundant columns are removed from this after image.
        This function does not create any before image.
        If a key column is altered, the flag KeyUpdated is set.
     */
    void CreateKeyPart (
        const tgg00_Rec  *pOldRec,
        tgg00_Rec        *pNewRec,
        SAPDB_Int4        NewRecSize,
        bool             &KeyUpdated,
        tgg00_BasisError &Error);

    virtual void AppendColumn (
        const tgg00_StackEntry &StackEntry,
        const tgg00_Rec        &Image,
        tgg00_BasisError       &Error);
	/// returns the pointer to a free column entry
    Log_ColumnEntry* GetFreeEntry ()
    {
        if (m_UsedEntries >= m_AllocatedEntries) return (NULL);
        
        ++ m_UsedEntries;
        
        return (m_pAllocatedMap + m_UsedEntries - 1);
    }
    /*!
       @brief          inserts a value except key values into the specified list
       @param          ColumnType [in] 
       @param          OffsetOrNumber [in] the offset or the number of the column
       @param          ValueLength [in] 
       @param          pValue [in] 
       @param          Error [out] 

        This function is used to create a before image of a column
        or to insert a column created by CreateColDescForExistingNewRec.
     */
    void InsertValue (
        Log_ColumnType    ColumnType,
        SAPDB_Int4        OffsetOrNumber,
        SAPDB_Int4        ValueLength,
        const SAPDB_Byte *pValue,
        tgg00_BasisError &Error);

    /*!
       @brief          inserts an variable undef value into the specified variable column list
       @param          ColumnType [in] 
       @param          OffsetOrNumber [in] the offset or the number of the column
       @param          Error [out] 

        This function is used to create a before image of a column which does not exist
        in the old record.
     */
    void InsertVarUndefValue (
        Log_ColumnType    ColumnType,
        SAPDB_Int4        OffsetOrNumber,
        tgg00_BasisError &Error);
    
protected:
    
    /// CTOR
    Log_RecordColumnMap (Log_ActionType ActionType);
    
	/// deallocate the column lists
    ~Log_RecordColumnMap ();
	/// returns the length for the NULL-values not existing in record
    void AddNullValLen (const SAPDB_Int4 NullValLen) 
    {
        m_PseudoNullValLen += NullValLen;
    }
    /*!
       @brief          allocates and inserts columns specifying the difference between the old and new record
       @param          RawAllocator [ref] 
       @param          pOldRecord [in] 
       @param          pNewRecord [in] the values of the created column entries point into this record
       @param          BeforeImage [in out] corresponding before image
       @param          Error [out] 

        This function applies to the after image and creates a corresponding before image.
     */
    void CreateColDescForExistingNewRec (
        SAPDBMem_IRawAllocator &RawAllocator,
        const tgg00_Rec        *pOldRecord,
        const tgg00_Rec        *pNewRecord,
        Log_RecordColumnMap    &BeforeImage,
        tgg00_BasisError       &Error);
    /*!
       @brief          creates the new record in case of undo or redo
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the new record. The Length has to be initialized.
       @param          Error [out] 
     */
    void CreateNewRecord (
        const tgg00_Rec  *pOldRec,
        tgg00_Rec        *pNewRec,
        tgg00_BasisError &Error)     const;
    /*!
       @brief          creates the new record using the old record and the after image column map
       @param          pOldRec [in] pointer to the existing record
       @param          pNewRec [out] pointer to the record space reserved to build the new record
       @param          NewRecSize [in] size of the record space
       @param          BeforeImage [in out] 
       @param          Error [out] 

        This function applies to the after image and creates a corresponding before image.
        Redundant columns are removed from the images.
        In case of a key update, the new record is created but the before image is incomplete.
     */
    void CreateNewRecordAndBeforeImage (
        const tgg00_Rec     *pOldRec,
        tgg00_Rec           *pNewRec,
        SAPDB_Int4           NewRecSize,
        Log_RecordColumnMap &BeforeImage,
        tgg00_BasisError    &Error);
	/// returns the length for the NULL-values not existing in record
    SAPDB_UInt2 GetNullValLen () const
    {
        return (m_PseudoNullValLen);
    }
	/// returns the length of all valid stack entries of the inv description map
    SAPDB_UInt GetPersistentColumnMapLength () const
    {
        return ( m_EntryHeader.ImageLen );
    }
    /*!
       @brief          reads the stack entries from the specified space
       @param          ImageReader [in out] 
       @param          PersistentLen [in] 
       @param          errlist [out] 
       @return         false on any error
     */
    bool ReadColumnMapPersistent (Log_ActionImageReader &ImageReader,
                                  SAPDB_Int4             PersistentLen,
                                  SAPDBErr_MessageList  &errlist);
    /*!
       @brief          writes the valid stack entries into the before or after image
       @param          ImageWriter [in out] 
       @param          errlist [out] 
       @return         false on any error
     */
    bool WriteColumnMapPersistent (Log_ActionImageWriter &ImageWriter,
                                   SAPDBErr_MessageList  &errlist)       const;
public:
    
    /*!
       @brief          allocates all column entries for further calls of InsertColumn
       @param          RawAllocator [ref] 
       @param          MaxEntries [in] 
       @return         false if allocation failed.
     */
    bool AllocateColumnMap (SAPDBMem_IRawAllocator &RawAllocator,
                            SAPDB_Int4              MaxEntries);
    /*!
       @return  true, if the entry exists in the record column map
       @param   StackEntry [in] 
     */
    virtual bool ExistsEntry (const tgg00_StackEntry &StackEntry) const;
    /*!
       @brief          inserts a column description with its value
       @param          StackEntry [in] description of the record column
       @param          pValue [in] pointer to the value
       @param          ValueLength [in] the length of the value
       @param          Error [out] 
     */
    virtual void InsertColumn (
        const tgg00_StackEntry &StackEntry,
        const SAPDB_Byte       *pValue,
        SAPDB_Int4              ValueLength,
        tgg00_BasisError       &Error);

    virtual void WriteToTrace(const char *title = NULL) const
    {
        Kernel_VTrace trace;
        if ( title != NULL )
            trace << title << FlushLine;
        m_EntryHeader.WriteToTrace("m_EntryHeader:");
        m_ColList [Log_FixKeyColumn].WriteToTrace();
        m_ColList [Log_VarKeyColumn].WriteToTrace();
        m_ColList [Log_FixColumn   ].WriteToTrace();
        m_ColList [Log_VarColumn   ].WriteToTrace();
        trace << "m_AllocatedEntries: "   << m_AllocatedEntries
              << ", m_UsedEntries: "      << m_UsedEntries
              << ", m_PseudoNullValLen: " << m_PseudoNullValLen
              << FlushLine;
    }
};
#endif  /* LOG_RECORD_COLUMN_MAP_H */
