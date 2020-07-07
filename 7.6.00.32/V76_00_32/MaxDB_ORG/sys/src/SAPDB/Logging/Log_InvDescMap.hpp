/*!
  @file           Log_InvDescMap.hpp
  @author         JuergenA
  @author         UweH
  @ingroup        Logging
  @brief          defines a class containing the index descriptions

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

#ifndef LOG_INV_DESC_MAP_H
#define LOG_INV_DESC_MAP_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "Logging/Log_IRecordColumnMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"
#include "ggg00.h"

/// id for an inv description
typedef SAPDB_UInt1 Log_InvDescId;
/// index for an index entry
typedef SAPDB_Int   Log_InvEntryIndex;

/*!
@class Log_InvUtilities
@brief functions for index handling
*/
class Log_InvUtilities
{
protected:
    

    /// these types of indices are supported
    typedef enum
    {
        EveryDesc, UniqueDesc, UniqueDescWithoutLateUnique, LateUnique
    } InvDescIterKind;

    /// builds the tree id of an multipl index
    void BuildInvFileId (
        Log_InvDescId       InvDescId,
        const tgg00_FileId &PrimFileId,
        tgg00_FileId       &InvFileId) const;

    /// return value: the index id of the stack entry
    Log_InvDescId GetInvDescId (const tgg00_StackEntry &StackEntry) const
    {
        return ( StackEntry.ecol_tab()[0] );  
    }  

    /// return value: true, if the entry is descending
    inline bool IsDescendingInvEntry (const tgg00_StackEntry &StackEntry) const;

    /// return value: true, if the entry is part of an unique index wich has to be checked later
    inline bool IsLateUniqueEntry (const tgg00_StackEntry &StackEntry) const;

    /// return value: true, if the entry is part of an unique index
    inline bool IsUniqueEntry (const tgg00_StackEntry &StackEntry) const;
    
    /// return value: true, if the entry is part of an unique index
    bool IsUniqueEntryWithoutLateUnique (const tgg00_StackEntry &StackEntry) const
    {
        return ( IsUniqueEntry(StackEntry) && ! IsLateUniqueEntry(StackEntry) );
    }

    /// return value: nil index
    static Log_InvEntryIndex NilIndex ()
    {
        return (SAPDB_MAX_INT4);
    }
    
    /// return value: nil value of Log_InvDescId 
    static Log_InvDescId NilInvDescId ()
    {
        return (0);
    }

};


/// a list of description entries
class Log_InvDescMap: private Log_InvUtilities
{

private:

    /// the allocator used inside this object
    SAPDBMem_IRawAllocator *m_pRawAllocator;
    /// pointer to the description of the index
    const tgg00_StackEntry *m_pInvDescMap;
    tgg00_StackEntry       *m_pAllocatedInvDescMap;
    SAPDB_Int               m_AllocatedEntries;
    SAPDB_Int               m_ValidEntries;
    SAPDB_Int               m_UniqueEntries;
    SAPDB_Int               m_LateUniqueEntries;
    
	/// initializes or resets the number of [late] unique entries.
    void FindNumberOfUniqueEntries();
        
public:

    inline Log_InvDescMap ();
	/// deallocate InvDescMap
    ~Log_InvDescMap ();

    void AddCompoundIndexColumnsToBeforeImage (
        Log_IRecordColumnMap &RecordColumnMap,
        const tgg00_Rec      &Image,
        bool                 &IsOk);

    /*!
       @brief          adds all secondary keys of the record to the inv trees
       @param          TransContext [in out] 
       @param          IsUndo [in]
       @param          IsRedo [in]
       @param          PrimFileId [in] file id of the primary table
       @param          pRec [in] pointer to the record which is used to build secondary keys
     */
    void AddInv (
        tgg00_TransContext &TransContext,
        bool                IsUndo,
        bool                IsRedo,
        bool                inSavepoint,
        const tgg00_FileId &PrimFileId,
        const tgg00_Rec    *pRec)           const;
    /*!
       @brief          allocates a list of empty index descripion entries
       @param          Entries [in] 
       @param          pRawAllocator [in out] 
       @param          IsOk [out] 
     */
    void Allocate (
        SAPDB_Int               Entries,
        SAPDBMem_IRawAllocator *pRawAllocator,
        bool                   &IsOk);
    /*!
       @brief          assigns the pointer of an existing stack entry list to pInvDescList
       @param          pExistingInvDescMap [in] the pointer must be aligned to stack entries
       @param          Entries [in] 
       @param          LookForUniqueEntries [in] if true, the list is scanned to mark unique entries
     */
    void Assign (
        const void *pExistingInvDescMap,
        SAPDB_Int   Entries,
        bool        LookForUniqueEntries);
    /*!
       @brief          builds a StackDesc for a further call of b03create_index
       @param          StackDesc [out] 
     */
    void BuildStackDesc (tgg00_StackDesc &StackDesc) const;
    /*!
       @brief          unique check of all secondary keys execept those marked to be checked later
       @param          TransContext [in out] 
       @param          PrimFileId [in] file id of the primary table
       @param          pRec [in] pointer to the record which is used to build secondary keys
     */
    void CheckUniqueIndex (
        tgg00_TransContext  &TransContext,
        const tgg00_FileId  &PrimFileId,
        const tgg00_Rec     *pRec)        const;
	/// returns true, if the InvDescMap contains an unique index
    bool ContainsUniqueIndex () const
    {
        return (m_UniqueEntries > 0);
    }
	/// returns true, if the InvDescMap contains an index wich has to be checked later
    bool ContainsLateUniqueCheck () const
    {
        return (m_LateUniqueEntries > 0);
    }
    /*!
       @brief          deletes all secondary keys of the record from the inv trees
       @param          TransContext [in out] 
       @param          IsUndoOrRedo [in] 
       @param          PrimFileId [in] file id of the primary table
       @param          pRec [in] pointer to the record which is used to build secondary keys
     */
    void DelInv (
        tgg00_TransContext &TransContext,
        bool                IsUndoOrRedo,
        bool                inSavepoint,
        bool                IsGarbageCollection,
        const tgg00_FileId &PrimFileId,
        const tgg00_Rec    *pRec)          const;
    /*!
       @brief in case of a function based index, the number of entries describing the index is returned; otherwise 0.
       @param Log_InvEntryIndex [in] 
     */
    inline int FunctionBasedIndexDescCnt (Log_InvEntryIndex EntryIndex) const; // PTS 1120019
    /*!
       @brief          creates the first key contained in the inv description map.
       @param          TransContext [in out] 
       @param          pRec [in] 
       @param          InvDescId [out] 
       @param          SecondaryKey [out] 

        This function is designed for maps which contains one description only.
     */
    void GetFirstKey (tgg00_TransContext &TransContext,
                      const tgg00_Rec    *pRec,
                      Log_InvDescId      &InvDescId,
                      tgg00_Lkey         &SecondaryKey,
                      bool               &allColumnsAreNull ); // PTS 1121337 UH 2003-03-27)
	/// returns the pointer to the inv description map
    const tgg00_StackEntry *GetInvDescMap () const
    {
        return (m_pInvDescMap);
    }
	/// returns the length of all valid stack entries of the inv description map
    SAPDB_UInt GetPersistentLength () const
    {
        return ( m_ValidEntries * sizeof (tgg00_StackEntry) );
    }
	/// returns the number of valid entries of the inv description map
    SAPDB_Int GetValidEntries () const
    {
        return (m_ValidEntries);
    }
    /*!
       @brief          the number of defined entries of the InvDescMap
       @param          StackEntry [in] 
     */
    inline void Insert (const tgg00_StackEntry &StackEntry);
    /*!
       @return         the id of the index description
       @param          EntryIndex [in] 
     */
    inline Log_InvDescId InvDescId (Log_InvEntryIndex EntryIndex) const;
	/// returns true, if the inv description map is empty
    bool IsEmpty () const
    {
        return (m_ValidEntries <= 0);
    }
	/// returns true, if the EntryIndex refers to a valid InvDescEntry
    bool IsValidIndex (Log_InvEntryIndex EntryIndex) const
    {
        return (EntryIndex < m_ValidEntries);
    }
	/// returns number of entries with late unique check
    SAPDB_Int LateUniqueCheckEntries () const
    {
        return (m_LateUniqueEntries);
    }
    /*!
       @brief          locks all unique secondary keys in exclusive mode and performes an unique check
       @param          TransContext [in out] 
       @param          PrimFileId [in] file id of the primary table
       @param          pRec [in] pointer to the record which is used to build secondary keys
       @param          forInsert [in]
       @param          GrantedMode [out] 

        Keys are not checked if they are marked to be checked later
     */
    void LockAndCheckUniqueIndex (
        tgg00_TransContext  &TransContext,
        const tgg00_FileId  &PrimFileId,
        const tgg00_Rec     *pRec,
        bool                 forInsert,
        tgg00_LockReqMode   &GrantedMode) const;
    /*!
       @brief          locks all unique secondary keys in exclusive mode
       @param          TransContext [in out] 
       @param          PrimFileId [in] internal id of the primary table
       @param          pRec [in] pointer to the record which is used to build secondary keys
       @param          GrantedMode [out] 
     */
    void LockUniqueIndex (
        tgg00_TransContext  &TransContext,
        const tgg00_FileId  &PrimFileId,
        const tgg00_Rec     *pRec,
        tgg00_LockReqMode   &GrantedMode) const;
    /*!
       @brief          reads the stack entries from the specified space
       @param          ImageReader [in out] 
       @param          errlist [out] filled with error hints on error
       @return         false on any error
     */
    bool ReadImagePersistent (Log_ActionImageReader &ImageReader,
                              SAPDB_UInt4            PersistentLen,
                              SAPDBErr_MessageList  &errlist);
    /*!
       @brief          removes those inv desc entries, which are not contained in the record column map
       @param          RawAllocator [in out] 
       @param          RecordColumnMap [in] 
       @param          IsOk [out] 
     */
    void RemoveRedundantInvDescEntries (
        SAPDBMem_IRawAllocator     &RawAllocator,
        const Log_IRecordColumnMap &RecordColumnMap,
        bool                       &IsOk);
    /*!
       @return         the pointer to the stack entry
       @param          EntryIndex [in] 
     */
    const tgg00_StackEntry &StackEntry (Log_InvEntryIndex EntryIndex) const
    {
        return ( *(m_pInvDescMap+EntryIndex) );
    }
	/// returns number of unique index entries
    SAPDB_Int UniqueEntries () const
    {
        return (m_UniqueEntries);
    }
    /*!
       @brief          writes the valid stack entries into the before or after image
       @param          ImageWriter [in out] 
       @param          errlist [out] filled with error hints on error
       @return         false on any error
     */
    bool WriteImagePersistent (Log_ActionImageWriter &ImageWriter,
                               SAPDBErr_MessageList  &errlist)       const;
};


/*==========================================================================*
*  DEFINITION OF INLINE METHODS                                             *
*===========================================================================*/

inline bool Log_InvUtilities::IsDescendingInvEntry (const tgg00_StackEntry &StackEntry) const
{
    return (        
        (op_order_desc             == StackEntry.eop() ) ||
        (op_unique_desc            == StackEntry.eop() ) ||
        (op_late_desc_unique_check == StackEntry.eop() ) );
}

//---------------------------------------------------------------------------

inline bool Log_InvUtilities::IsLateUniqueEntry (const tgg00_StackEntry &StackEntry) const
{
    return (        
        (op_late_asc_unique_check  == StackEntry.eop() ) ||
        (op_late_desc_unique_check == StackEntry.eop() ) );
}

//---------------------------------------------------------------------------

inline bool Log_InvUtilities::IsUniqueEntry (const tgg00_StackEntry &StackEntry) const
{
    return (
        (op_unique                 == StackEntry.eop() ) ||
        (op_unique_desc            == StackEntry.eop() ) ||
        (op_unique_expr_upd        == StackEntry.eop() ) ||
        (op_desc_unique_expr_upd   == StackEntry.eop() ) ||
        (op_late_asc_unique_check  == StackEntry.eop() ) ||
        (op_late_desc_unique_check == StackEntry.eop() ) );
}

//---------------------------------------------------------------------------

inline Log_InvDescMap::Log_InvDescMap ():
    m_pRawAllocator        (NULL),
    m_pInvDescMap          (NULL),
    m_pAllocatedInvDescMap (NULL),
    m_AllocatedEntries     (0),
    m_ValidEntries         (0),
    m_UniqueEntries        (0),
    m_LateUniqueEntries    (0)
{ }

//---------------------------------------------------------------------------

inline Log_InvDescId Log_InvDescMap::InvDescId (Log_InvEntryIndex EntryIndex) const
{
    if (NilIndex() == EntryIndex )
    {
        return ( NilInvDescId() );
    }

    return ( GetInvDescId( *(m_pInvDescMap+EntryIndex) ) );
}

//---------------------------------------------------------------------------

inline void Log_InvDescMap::Insert (const tgg00_StackEntry &StackEntry)
{
    if  (m_AllocatedEntries > m_ValidEntries)
    {
        *(m_pAllocatedInvDescMap+m_ValidEntries) = StackEntry;
        ++m_ValidEntries;
        
        if ( IsUniqueEntry     (StackEntry) ) ++ m_UniqueEntries;
        if ( IsLateUniqueEntry (StackEntry) ) ++ m_LateUniqueEntries;
    }
}

//---------------------------------------------------------------------------

inline int Log_InvDescMap::FunctionBasedIndexDescCnt (Log_InvEntryIndex EntryIndex) const
{
    if (NilIndex() != EntryIndex )
    {
        if (st_func == (m_pInvDescMap+EntryIndex)->etype())
        {
            return (m_pInvDescMap+EntryIndex)->epos();
        }
    }
    return 0;
}

#endif  /* LOG_INV_DESC_MAP_H */
