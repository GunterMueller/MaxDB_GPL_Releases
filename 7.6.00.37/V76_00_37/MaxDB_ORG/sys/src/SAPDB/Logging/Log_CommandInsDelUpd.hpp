/*!
  @file           Log_CommandInsDelUpd.hpp
  @author         UweH
  @author         JuergenA
  @ingroup        LogVolume
  @brief          defines classes to handle the insert, delete and update commands

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


#ifndef LOG_COMMAND_INS_DEL_UPD_H
#define LOG_COMMAND_INS_DEL_UPD_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "ConsistentRead/CnsRead_Manager.hpp"
#include "Logging/Log_Types.hpp"
#include "Logging/Log_ActionInsDelRec.hpp"
#include "Logging/Log_ActionUpdateRec.hpp"
#include "Logging/Log_InvDescMap.hpp"
#include "Logging/Log_ReadWriteActionImage.hpp"

#include "ggg00.h"


/*===========================================================================*
*  CLASSES, STRUCTURES, TYPES, UNIONS ...                                    *
*============================================================================*/

/*!
    @brief index handling
*/
class Log_InvHandling
{
private:
    
    const tgg00_FileId *m_pPrimFileId;
    const tgg00_Rec    *m_pOldRec;
    const tgg00_Rec    *m_pNewRec;

    Log_InvDescMap      m_InvDescMap;
    bool                m_bDelInvPostponed;

protected:
    
    
    
    /// returns the filename 
    const tgg00_Filename &GetFilename () const
    {
        return ( m_pPrimFileId->fileName_gg00() );
    }
    
    /// returns the inversion-map
    const Log_InvDescMap *GetInvDescMap () const
    {
        return ( &m_InvDescMap );
    }
    
    /// returns a pointer for write-access to the map
    Log_InvDescMap *GetWriteAccessInvDescMap ()
    {
        return ( &m_InvDescMap );
    }
    
    /// returns the old record value
    const tgg00_Rec *GetOldRec () const
    {
        return (m_pOldRec);
    }
    
    /// returns the value of the newer record
    const tgg00_Rec *GetNewRec () const
    {
        return (m_pNewRec);
    }
    
    /// initialize the afterImage with a specific record
    inline void InitAfterImage ( const tgg00_FileId &PrimFileId,
                                 const tgg00_Rec    *pNewRec);
    
    /// initialize the beforeImage with a specific record
    inline void InitBeforeImage ( const tgg00_FileId &PrimFileId,
                                  const tgg00_Rec    *pOldRec);
    
    /// initialize the beforeImage with a specific record
    inline void InitBeforeImageRecord (const tgg00_Rec    *pOldRec)
    {
        m_pOldRec = pOldRec;
    }

public:

    /// empty inv description map
    inline Log_InvHandling ();
    
    /// assigns an existing StackDesc to the inv description map 
    inline void AssignInvDescMap (const tgg00_StackDesc &StackDesc);
    
    /// inserts all secondary keys of the after image into the inv trees
    void AddInv (tgg00_TransContext &TransContext) const;

    /// unique check of all secondary keys execept those marked to be checked later
    /// The check requires an initialized after image.
    void CheckUniqueIndex (tgg00_TransContext &TransContext) const;

    /// returns true, if InvDescMap contains an index which has to be checked later
    bool ContainsLateUniqueCheck () const
    {
        return ( m_InvDescMap.ContainsLateUniqueCheck() );
    }

    /// returns : true, if InvDescMap contains an unique index
    bool ContainsUniqueIndex () const
    {
        return ( m_InvDescMap.ContainsUniqueIndex() );
    }
    
    /// deletes all secondary keys of the before image from the inv trees
    void DelInv (tgg00_TransContext &TransContext) const;
   

    /// Sets the internal state, that DelInv has to be called outside any BD-function 
    /// @return true, if DelInv has to be called outside any BD-function 
    inline bool ExecuteOutsideBd ();

    /// returns true, if InvDescMap contains any index
    bool ExistsAnyIndex () const
    {
        return ( ! m_InvDescMap.IsEmpty() );
    }

    /// returns true, if DelInv has to be called outside any BD-function 
    bool IsExecutionPostponed () const
    {
        return (m_bDelInvPostponed);
    }

    /// locks all unique secondary keys in exclusive mode and performes a unique check
    /// The check requires an initialized after image. Keys are not checked,
    /// if they are marked to be checked later.
    /// The keys of the before and after images are locked,
    /// if both images are initialized.
    void LockAndCheckUniqueIndex ( tgg00_TransContext  &TransContext,
                                   bool                 forInsert,
                                   tgg00_LockReqMode   &GrantedMode) const;
    
    /// locks all unique secondary keys of the initialized images in exclusive mode
    void LockUniqueIndex (
        tgg00_TransContext  &TransContext,
        tgg00_LockReqMode   &GrantedMode,
        bool                 BeforeImageOnly = false) const;
    
};


//===================================================================================
/*!  
    @brief handling of the delete command
*/
class Log_DeleteCmd: public Log_InvHandling
{
private:
    
    Log_ActionInsDelRec  m_BeforeImage;
    Log_ActionInsDelRec  m_AfterImage;
    
public:
    
    /// empty Log_DeleteCmd
    inline Log_DeleteCmd ();
    
    /// initializes the before and after image
    /// @param   PrimFileId [in] 
    /// @param   pOldRec    [in] pointer to the old record
    /// @param   MarkDeleted [in] mark the record as deleted
    inline void Init (
        const tgg00_FileId &PrimFileId,
        const tgg00_Rec    *pOldRec,
        bool                MarkDeleted);

    /// writes the before image, deletes all secondary keys and the record
    void PostponedExecution (
        tgg00_TransContext &TransContext,
        tgg00_FileId       &PrimFileId) const;
    
    /// writes the after image
    void WriteAfterImage (tgg00_TransContext &trans) const
    {
        m_AfterImage.WriteAfterImage (trans);
    }

    /// writes the before image
    void WriteBeforeImage (tgg00_TransContext &trans) const
    {
        m_BeforeImage.WriteBeforeImage (trans);
    }
    /// writes the before image
    void WriteBeforeImage (
        tgg00_TransContext &TransContext,
        tgg00_ObjTransInfo &TransInfo) const
    {
        m_BeforeImage.WriteBeforeImage (TransContext, TransInfo);
    }
};

/// handling of the insert command
class Log_InsertCmd: public Log_InvHandling
{ 
private:

    Log_ActionInsDelRec  m_BeforeImage;     ///< the image before the action had taken place
    Log_ActionInsDelRec  m_AfterImage;      ///< the image after the action had taken place
    
public:

    /// @brief empty Log_InsertCmd
    inline Log_InsertCmd ();

    /// assigns the before image, must be marked as deleted
    /// @param  pBeforeImage [in] pointer to before image
    inline void AssignBeforeImage (
        const tgg00_Rec* pBeforeImage);

    /// initializes the before and after image
    /// @param     PrimFileId [in] 
    /// @param     pNewRec    [in] pointer to the new record
    inline void Init (
        const tgg00_FileId &PrimFileId,
        const tgg00_Rec    *pNewRec);

    /// writes the after image
    void WriteAfterImage (tgg00_TransContext &trans) const
    {
        m_AfterImage.WriteAfterImage (trans);
    }

    /// writes the before image
    void WriteBeforeImage (tgg00_TransContext &trans, tgg00_ObjTransInfo& TransHistoryInfo) const
    {
        m_BeforeImage.WriteBeforeImage (trans, TransHistoryInfo);
    }
};

/// handling of the delete command
class Log_UpdateCmd: public Log_InvHandling
{
private:

    Log_ActionUpdateRec  m_BeforeImage;               ///< the image before the replace had took place
    Log_ActionUpdateRec  m_AfterImage;                ///< the image after the replace had took place
    bool                 m_checkCompoundIndexColumns; ///< true, if compound index columns have to be collected for garbage collection
    
public:

    /// empty Log_UpdateCmd
    inline Log_UpdateCmd ();

    /// allocates all column entries for further calls of InsertColumn
    inline bool AllocateColumnMap (SAPDBMem_IRawAllocator &RawAllocator,
                                   SAPDB_Int4              MaxEntries);
    
    /// initializes the before and after image
    void CreateNewRec (
        SAPDBMem_IRawAllocator &RawAllocator,                                            
        const tgg00_FileId     &PrimFileId,
        const tgg00_Rec        *pOldRecord,
        tgg00_Rec              *pNewRecord,
        SAPDB_Int4              NewRecSize,
        tgg00_BasisError       &Error);
    
    /// returns true, if the entry exists in the record column map of the after image
    bool ExistsEntry (const tgg00_StackEntry &StackEntry) const
    {
        return ( m_AfterImage.ExistsEntry (StackEntry) );
    }

    /// initializes the after image using an existing new record.
    void InitAfterImageWithExistingNewRec (
        const tgg00_FileId &PrimFileId,
        const tgg00_Rec    *pNewRecord)
    {    
        Log_InvHandling::InitAfterImage (PrimFileId, pNewRecord);
    }

    /// initializes the before image. The after image has to be initialized already.
    inline void InitBeforeImageForExistingNewRec (
        SAPDBMem_IRawAllocator &RawAllocator,
        const tgg00_Rec        *pOldRecord,
        tgg00_BasisError       &Error);

    /// inserts a column description with its value
    /// @param       StackEntry  [in]  description of the record column
    /// @param       pValue      [in]  pointer to the value
    /// @param       ValueLength [in]  the length of the value
    /// @param       Error       [out]
    void InsertColumn (
        const tgg00_StackEntry &StackEntry,
        const SAPDB_Byte       *pValue,
        SAPDB_Int4              ValueLength,
        tgg00_BasisError       &Error)
    {
        m_AfterImage.InsertColumn (StackEntry, pValue, ValueLength, Error);
    }

    /// writes the before image, deletes all secondary keys and replaces the record
    void PostponedExecution (
        tgg00_TransContext &TransContext,
        tgg00_FileId       &PrimFileId) const;

    /// set the flag for compound index column check
    inline void SetCheckCompoundIndexColumnsFlag();
    /// writes the after image
    void WriteAfterImage (tgg00_TransContext &TransContext) const
    {
        m_AfterImage.WriteAfterImage (TransContext);
    }

    /// writes the before image
    void WriteBeforeImage (tgg00_TransContext &TransContext, tgg00_ObjTransInfo &TransInfo) const
    {    
        m_BeforeImage.WriteBeforeImage (TransContext, TransInfo);
    }
};


inline Log_InvHandling::Log_InvHandling ():

m_pPrimFileId      (NULL),
m_pOldRec          (NULL),
m_pNewRec          (NULL),
m_InvDescMap       (),
m_bDelInvPostponed (false)
{ }

//---------------------------------------------------------------------------

inline void Log_InvHandling::InitAfterImage (const tgg00_FileId &PrimFileId,
                                             const tgg00_Rec    *pNewRec)
{
    m_pPrimFileId = &PrimFileId;
    m_pNewRec     = pNewRec;
}

//---------------------------------------------------------------------------

inline void Log_InvHandling::InitBeforeImage (const tgg00_FileId &PrimFileId,
                                              const tgg00_Rec    *pOldRec)
{
    m_pPrimFileId = &PrimFileId;
    m_pOldRec     = pOldRec;
}

//---------------------------------------------------------------------------

inline void Log_InvHandling::AssignInvDescMap (const tgg00_StackDesc &StackDesc)
{
    if (StackDesc.mmult_cnt <= 0)  return;
    
    m_InvDescMap.Assign (
        &((*StackDesc.mst_addr())[StackDesc.mmult_pos-1]),
        StackDesc.mmult_cnt, 
        true); // LookForUniqueEntries
}

//---------------------------------------------------------------------------

inline bool Log_InvHandling::ExecuteOutsideBd ()
{
    if ( m_InvDescMap.IsEmpty() ) return (false);
    
    m_bDelInvPostponed = true;
    return (true);
}

//---------------------------------------------------------------------------

inline Log_DeleteCmd::Log_DeleteCmd ():

Log_InvHandling (),
m_BeforeImage   ((CnsRead_Manager::IsConsistentEnabled()) ? Log_MarkedDeleted : Log_DeleteRecord, true),
m_AfterImage    ((CnsRead_Manager::IsConsistentEnabled()) ? Log_MarkedDeleted : Log_DeleteRecord, false)
{ }

//---------------------------------------------------------------------------

inline void Log_DeleteCmd::Init (const tgg00_FileId &PrimFileId,
                                 const tgg00_Rec    *pOldRec,
                                 bool                MarkDeleted)
{
    Log_InvHandling::InitBeforeImage (PrimFileId, pOldRec);
    
    m_BeforeImage.Init (PrimFileId.fileName_gg00(), pOldRec, Log_InvHandling::GetInvDescMap(), MarkDeleted);
    
    m_AfterImage.Init
        (PrimFileId.fileName_gg00(), &(pOldRec->recKey_gg00()), Log_InvHandling::GetInvDescMap());
}

//---------------------------------------------------------------------------

inline Log_InsertCmd::Log_InsertCmd ():

Log_InvHandling (),
m_BeforeImage   (Log_InsertRecord, true),
m_AfterImage    (Log_InsertRecord, false)
{ }

//---------------------------------------------------------------------------

inline void Log_InsertCmd::AssignBeforeImage (const tgg00_Rec* pBeforeImage)
{
    m_BeforeImage.AssignBeforeImage(pBeforeImage);
}

//---------------------------------------------------------------------------
inline void Log_InsertCmd::Init (const tgg00_FileId &PrimFileId,
                                 const tgg00_Rec    *pNewRec)
{
    Log_InvHandling::InitAfterImage (PrimFileId, pNewRec);

    m_BeforeImage.Init
        (PrimFileId.fileName_gg00(), &(pNewRec->recKey_gg00()), Log_InvHandling::GetInvDescMap());
    
    m_AfterImage.Init  (PrimFileId.fileName_gg00(), pNewRec, Log_InvHandling::GetInvDescMap());
}

//---------------------------------------------------------------------------

inline Log_UpdateCmd::Log_UpdateCmd ():

Log_InvHandling (),
m_BeforeImage   (),
m_AfterImage    ()
{ }

//---------------------------------------------------------------------------

inline bool Log_UpdateCmd::AllocateColumnMap (SAPDBMem_IRawAllocator &RawAllocator,
                                              SAPDB_Int4              MaxEntries)
{
    if(!m_AfterImage.AllocateColumnMap (RawAllocator, MaxEntries))
        return false;
    return m_BeforeImage.AllocateColumnMap (RawAllocator, MaxEntries);
}

//---------------------------------------------------------------------------

inline void Log_UpdateCmd::InitBeforeImageForExistingNewRec (SAPDBMem_IRawAllocator &RawAllocator,
                                                             const tgg00_Rec        *pOldRecord,
                                                             tgg00_BasisError       &Error)
{    
    if (e_ok != Error) return;
    
    Log_InvHandling::InitBeforeImageRecord (pOldRecord);
    
    m_AfterImage.InitAfterBeforeImageWithExistingNewRec (
        RawAllocator,
        Log_InvHandling::GetFilename(),
        Log_InvHandling::GetInvDescMap(),
        pOldRecord,
        Log_InvHandling::GetNewRec(),
        m_BeforeImage,
        Error);
}

//---------------------------------------------------------------------------

inline void Log_UpdateCmd::SetCheckCompoundIndexColumnsFlag()
{
    m_checkCompoundIndexColumns = true;
}

#endif  /* LOG_COMMAND_INS_DEL_UPD_H */
