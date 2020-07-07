/*!
  @file           Log_ActionObject.hpp
  @ingroup        ActionHandling
  @author         UweH
  @brief          defines class Log_ActionObject

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



#ifndef Log_ActionObject_H
#define Log_ActionObject_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"  // tgg00_TransContext, tgg00_FileId
#include "ggg92.h"  // tgg92_KernelOid, tgg92_ObjHeader

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Assertions.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"

#include "DataAccess/Data_PageSplitSpace.hpp"
#include "DataAccess/Data_ISplitSpace.hpp"

#include "Logging/Log_Types.hpp"
#include "Logging/Log_IAction.hpp"

#include "FileDirectory/FileDir_Types.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Log_AfterImage;
class Log_BeforeImage;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
   @brief          Handle for manipulating OMS objects.
 */


class Log_ActionObject : public Log_IAction
{

public:

    /// constructor
    Log_ActionObject ()
    : Log_IAction  (Log_NoOp),
      m_pKey       (0),
      m_pBody      (0),
      m_pAllocator (0)
    {}
    
    /// destructor
    ~Log_ActionObject ()
    {
        if ( 0 != m_pAllocator )
        {
            if ( 0 != m_pBody )
                destroy (m_pBody,*m_pAllocator);
            if ( 0 != m_pKey )
                // PTS 1136035 mb 2005-06-08
                destroy (m_pKey,*m_pAllocator);
        }
    }
    
    /// Initialize the new action
    bool InitNew (const tgg00_FileId    &fileid,
                  const tgg92_KernelOid &oid,
                  const tgg91_PageRef   &version,
                  SAPDB_Int4             keySize,
                  SAPDB_Byte            *pKey,
                  SAPDB_Int1             contVarObject,  /* PTS 1127661 FF */
                  SAPDBErr_MessageList  &errlist);

    /// This is used to write.
    void InitDelete (FileDir_FileNo        &fileno,
                     const tgg92_KernelOid &oid,
                     const tgg91_PageRef   &version,
                     bool                   reused,          /* PTS 1127661 FF */
                     SAPDB_Int1             contVarObject);  /* PTS 1127661 FF */

    /// This is used to write.
    bool InitInsUpd (Log_ActionType         actiontype,
                     const tgg00_FileId    &fileid,
                     const tgg92_KernelOid &oid,
                     const tgg91_PageRef   &version,
                     SAPDB_Int4             keySize,
                     SAPDB_Int4             bodySize,
                     SAPDB_Byte            *pBody,
                     SAPDB_Int1             contVarObject,   /* PTS 1127661 FF */
                     bool                   reused,         /* PTS 1127661 FF */
                     SAPDBErr_MessageList  &errlist);

    /// this has at least to be copied in one step
    static SAPDB_UInt GetMinimalLengthToCopyEntry()
    {
        return sizeof (Head);
    }

    /// return the minimum length for creating a persistent entry
    virtual SAPDB_UInt GetMinimalPersistentLength() const
    {
        return GetMinimalLengthToCopyEntry();
    }

    /// Get the length of the action
    virtual SAPDB_UInt GetPersistentLength() const
    {
        if ( m_Head.bodySize > 0 )
            return GetMinimalPersistentLength() + Data_PageSplitSpace::AlignRecordSize (m_Head.bodySize);
        if ( m_Head.keySize > 0 )
            return GetMinimalPersistentLength() + Data_PageSplitSpace::AlignRecordSize (m_Head.keySize);
        return GetMinimalPersistentLength();
    }

    /// The Space must be initialized with the value given by GetPersistentLength().
    virtual bool WritePersistentFormat (Data_SplitSpaceWriter &Writer,
                                        SAPDBErr_MessageList  &errlist) const;

    /// all internal components are initialized with the reader
    virtual bool ReadPersistentFormat (Data_SplitSpaceReader  &Reader,
                                       SAPDBMem_IRawAllocator &Allocator,
                                       SAPDBErr_MessageList   &errlist);

    ///  action is written to the trace file
    virtual void WriteToTrace (const char * Title) const;

    /// append some info to the errlist
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const;

    virtual void Redo (tgg00_TransContext &TransContext, Log_IImage &Image, SAPDBErr_MessageList &Errlist) const;

    virtual void Undo (tgg00_TransContext &TransContext, Log_IImage &Image) const;

    virtual bool GarbageCollectionNeeded() const
    {
        return true;
    }

    /// the ObjectID is returned
    tgg92_KernelOid& GetOid()
    {
        return m_Head.oid;
    }

    /// the ObjectID is returned
    const tgg92_KernelOid& GetOid() const
    {
        return m_Head.oid;
    }

    /// the file-number is returned
    FileDir_FileNo  GetFileNo() const
    {
        return m_Head.fileno;
    }

    /// the size of the body of the action is returned
    SAPDB_UInt2 GetBodySize() const
    {
        return m_Head.bodySize;
    }

    /// returns the size of the key
    SAPDB_UInt2 GetKeySize() const
    {
        return m_Head.keySize;
    }

    /// returns the version
    const tgg91_PageRef& GetObjectVersion() const
    {
        return m_Head.version;
    }

    /// returns a pointer to the key-values
    const SAPDB_Byte* GetKey() const
    {
        return m_pKey;
    }

    /// returns a pointer to the body
    const SAPDB_Byte* GetBody() const
    {
        return m_pBody;
    }

    /// The image must be the pointer to a persistent format of this action.
    static void ExtractOid ( const SAPDB_Byte  *image,
		                     SAPDB_UInt         imagelength,
							 tgg92_KernelOid   &oid )
    {
        SAPDBERR_ASSERT_ARGUMENT( imagelength >= sizeof(Head) );
		oid = (reinterpret_cast <const Head*>(image))->oid;
    }

private:

    /// the size of the alignment
    SAPDB_UInt GetAlignmentLength() const
    {        
        SAPDB_UInt length = GetPersistentLength();
        
        if ( m_Head.bodySize > 0 )
            return length - GetMinimalPersistentLength() - m_Head.bodySize;
        else
            return length - GetMinimalPersistentLength() - m_Head.keySize;
    }

private:

    /// the Head of an Log_Action: Written into undo/redo-files
    /// (totally 28 bytes long)
    struct Head
    {
        enum ActionFlags       /* PTS 1127661 FF */
        {
            /// the object has been deleted and reused by OMS. 
            /// liveCache base received an update/delete with changed frameversion
            Reused           = 1,

            /// VarObject has been flagged as Head/Cont Varobject
            /// 0  not flagged
            /// 1  flagged
            MarkedVarObject  = 2,

            /// 0  VarObject is a HeadVarObject
            /// 1  VarObject is a ContVarObject
            /// explanation: 
            /// omsVarObjects bigger than about 8 kb are split by the OMS layer into 
            /// a sequence of VarObjects. To be able to implement an iterator for 
            /// omsVarObjects all VarObjects are marked as Head/Continous VarObject
            /// Only the very first VarObject is marked as Head VarObject
            /// all the other VarObjects are marked as ContVarObjects
            ContVarObject   = 4
        };

        tgg92_KernelOid      oid;      ///< 8 bytes
        FileDir_FileNo       fileno;   ///< 8 bytes
        SAPDB_UInt2          bodySize; ///< 2 bytes
        SAPDB_UInt2          keySize;  ///< 2 bytes
        tgg91_PageRef        version;  ///< 6 bytes
        SAPDB_UInt1          flags;    ///< 1 byte     /* PTS 1127661 FF */
        SAPDB_UInt1          filler;   ///< 1 byte
        SAPDB_UInt4          filler2;  ///< 4 bytes


        /// assignment operator to avoid 8 byte alignment
        Head & operator = ( const Head & src )
        {
            if ( &src == this )
                return *this;
            SAPDB_MemCopyNoCheck( this, &src, sizeof(*this) );
            return *this;
        }
    };                        

    /// the in-memory used head
    Head m_Head;

    /// a pointer to the key
    SAPDB_Byte *m_pKey;

    /// a pointer to the body of the entry
    SAPDB_Byte *m_pBody;

    /// an allocator used for dynamic memory allocation
    SAPDBMem_IRawAllocator* m_pAllocator;
};


#endif  /* Log_ActionObject_H */
