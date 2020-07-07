/*
  @file         gbd496.h
  @author       TorstenS
  @ingroup      create index
  @brief        This modul is used to handle the temporary files
                which come into being during the parallel index
                creation to hold the temporary sorted index keys

\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef GBD496_H
#define GBD496_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

// Content of include files

#include "ggg00.h"   // PASCAL: GG_kernel_constants_and_types

#include "gbd494.h"  // CPP   : FunnelHandling
#include "gbd495.h"  // CPP   : DoubleQueueHandling

#include "Container/Container_Vector.hpp"
#include "DataAccess/Data_Types.hpp"
#include "KernelCommon/Kernel_Common.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  FORWARD DECLARATION                                                      *
 *===========================================================================*/

class cbd497_StackDescInfo;
class SAPDBMem_IRawAllocator;
class Trans_Context;

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


class cbd496_FileHandling
{
public:

    cbd496_FileHandling(
        SAPDBMem_IRawAllocator& allocator,
        cbd494_SortableInvTree& invTree );
   

    ~cbd496_FileHandling(){
        m_FileList.Delete();
    }
    // in case of an error no base error is set
    bool Initialize( const SAPDB_UInt maxResultFiles );

    bool ConvertFile(
        Trans_Context&      trans,
        const SAPDB_UInt    fileIndex,
        const Data_PageNo&  tempInvRoot );

    bool CreateFile(
        Trans_Context&  trans,
        SAPDB_UInt&     newFileIndex,
        tgg00_FileId&   queueId,
        Data_PageNo&    primQueueRoot,
        Data_PageNo&    secQueueRoot );


    void DeleteFile( 
        Trans_Context&      trans,
        const SAPDB_UInt    fileIndex );

    void DeleteAllFiles( Trans_Context&  trans );

    void MergeFiles(
        Trans_Context&              trans,
        const pasbool               bLastMergeStep,
        const cbd497_StackDescInfo& stackDescInfo,
        tgg00_IndexCounter&         indexCounter );
    
    void SetFilePageCount(
          const SAPDB_UInt    fileIndex,
          const SAPDB_Int4    pageCount );      
        
private:

    class cbd496_File
    {
    public:

        void AssignDoubleQueueFile(
            const Data_PageNo&  primQueueRoot,
            const Data_PageNo&  secQueueRoot )
        {
            m_PrimQueueRoot = primQueueRoot;
            m_SecQueueRoot  = secQueueRoot;
            m_PageCount     = 2; // two anchor pages
        }

        void AssignTempInvFile( const Data_PageNo&  tempInvRoot ){
            m_PrimQueueRoot = tempInvRoot;
            m_SecQueueRoot.Invalidate();
            m_PageCount = 1; // one anchor page
        }

        void Deassign()
        {
            m_PrimQueueRoot.Invalidate();
            m_SecQueueRoot.Invalidate();
            m_PageCount = 0;
        }

        bool IsAssigned() const{
            return m_PrimQueueRoot.IsValid();
        }

        bool IsDoubleQueueFile() const{
            return m_PrimQueueRoot.IsValid() && m_SecQueueRoot.IsValid();
        }

        Data_PageNo GetPrimQueueRoot() const
        {
            SAPDBERR_ASSERT_STATE( this->IsDoubleQueueFile());
            return m_PrimQueueRoot;
        }

        Data_PageNo GetSecQueueRoot() const
        {
            SAPDBERR_ASSERT_STATE( this->IsDoubleQueueFile());
            return m_SecQueueRoot;
        }

        Data_PageNo GetTempInvRoot() const
        {
            SAPDBERR_ASSERT_STATE( ! m_SecQueueRoot.IsValid());
            return m_PrimQueueRoot;
        }

        static SAPDB_UInt GetSize()
        {
#       ifdef BIT64
            return SAPDB_ALIGN_8BYTE( sizeof( cbd496_File ));
#       else
            return SAPDB_ALIGN_4BYTE( sizeof( cbd496_File ));
#       endif
        }
        
        SAPDB_Int4 GetPageCount() const{
            return m_PageCount;
        }
        
        void SetPageCount( const SAPDB_Int4 pageCount ){
            m_PageCount = pageCount;
        }

    private:

        // root page of the page chain  which contains all primary
        // keys in ascending order. Note that if m_PrimQueueRoot
        // is valid and m_SecQueueRoot is invalid the current file
        // is a temporary index file
        Data_PageNo    m_PrimQueueRoot;

        // the root page of page chain which contains all secondary
        // keys in ascending order
        Data_PageNo    m_SecQueueRoot;
        
        // number of pages in page chain; this value must be set explicit
        SAPDB_Int4  m_PageCount;
    };

    typedef Container_Vector< cbd496_File > FileList;

    FileList::IndexType GetLastIndex() const{
        return m_FileList.GetCapacity()-1;
    }

    FileList::IndexType GetFirstSortFileIndex() const{
        return 0;
    }

    FileList::IndexType GetLastSortFileIndex() const{
        return this->GetLastIndex() - m_NumResultFiles - m_NumOverflowFiles;
    }

    FileList::IndexType GetLastResultFileIndex() const{
        return  this->GetLastIndex() - m_NumOverflowFiles;
    }

    FileList::IndexType GetFirstOverflowFileIndex() const{
        return this->GetLastResultFileIndex() + 1;
    }

    FileList::IndexType GetLastOverflowFileIndex() const{
        return this->GetFirstOverflowFileIndex() + 1;
    }

    FileList::IndexType GetTargetFileIndex() const{
        return m_TargetFileIndex;
    }

    void DeleteFile( cbd496_File& file );

    // in case of an error false is returned and trans error is set
    bool InitializeFile(
        Trans_Context&  trans,
        cbd496_File&    file,
        tgg00_FileId&   doubleQueueFileId,
        Data_PageNo&    primQueuePno,
        Data_PageNo&    secQueuePno );

    void Merge(
        Trans_Context&              trans,
        const bool                  bLastMergeStep,
        const  FileList::IndexType  lastFileIndex,
        const cbd497_StackDescInfo& stackDescInfo );

    tbd_current_tree& GetResultFileCurrent( Trans_Context& trans );
    
private:

    // Assignment operator is not supported
    cbd496_FileHandling& operator=( const cbd496_FileHandling &fh );

    // Copy constructor is not supported
    cbd496_FileHandling( const cbd496_FileHandling &fh );

private:

    const FileList::IndexType   m_NumResultFiles;   // 62
    const FileList::IndexType   m_NumOverflowFiles; //  2

    cbd494_SortableInvTree  &m_InvTree;
    
    // use to build temporary files which will be added to the funnel merge
    tbd_current_tree    m_ResultFileCurrent;

    // Index of the temp target file within m_Filelist, if an
    // additional temp merge step is necessary
    SAPDB_UInt  m_TargetFileIndex;

    // number of double queue result files to be sorted
    SAPDB_UInt  m_NumOpenQueuesFiles;

    // number of temp index result files to be sorted
    SAPDB_UInt  m_NumOpenTreesFiles;

    // number of funnel merge calls to build one index
    // Normaly this should be one
    SAPDB_UInt  m_MergeCount;

    // Collection of files, which contains the temporary results of
    // the parallel index creation
    FileList  m_FileList;

    // Merger which is used to sort temporary result files.
    // This is a member data, because so it is ensure that the memory
    // is available if it is needed!
    cbd494_Funnel   m_Funnel;
};

/*===========================================================================*
*  DEFINITION OF METHODS                                                    *
*===========================================================================*/


#endif  /* GBD496_H */
