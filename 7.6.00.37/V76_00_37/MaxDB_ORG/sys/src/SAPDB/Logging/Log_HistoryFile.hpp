/*!
  @file           Log_HistoryFile.hpp
  @author         UweH
  @ingroup        History
  @brief          Manages the log volume with queue and writer

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

#ifndef Log_HistoryFile_H
#define Log_HistoryFile_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg91.h"
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ChainFixSizeSpace.hpp"
#include "DataAccess/Data_BaseFile.hpp"
#include "Logging/Log_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

class Log_UndoFile;
class Data_PageAccessManager;
class SAPDBMem_IRawAllocator;

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*---------------------------------------------------------------------------*/
/*! 
    @brief This stores anchors to undo file.
*/
class Log_HistoryFile : public Data_BaseFile
{

public:
    
    typedef Data_PageFixSizeSpace             PageType;         ///< shortcut
    typedef Data_ChainFixSizeSpace<PageType>  Container;        ///< shortcut
    typedef Data_ChainIterator<PageType>      PageIterator;     ///< shortcut

    /// This describes an undofile
    struct UndoFileInfo
    {
        Data_PageNo   root;       ///< 4 Byte reference to the root page
        Data_PageNo   last;       ///< 4 Byte reference to the last page
        tgg91_TransNo transno;    ///< 6 Byte Transactioncontext
        SAPDB_UInt2   pagecount;  ///< 2 Byte number of pages in this file
        UndoFileInfo()
        {
            root.Invalidate();
            last.Invalidate();
            transno.gg90SetNil();
            pagecount = 0;
        }
    };

    /// points to UndoFileInfo's
    class Iterator
    {
    public:
        /// constructor
        Iterator (Container::Iterator& iter)
        : m_Iterator (iter)
        {}
        
        /// access a undofile by a dereference of the iterator
        UndoFileInfo& operator * ()
        {
            return *(reinterpret_cast<UndoFileInfo*>(*m_Iterator));
        }

        /// increment operation
        bool PlusPlus(SAPDBErr_MessageList &errlist)
        {
            return m_Iterator.PlusPlus(errlist);
        }
        
        /// release resources inorder to prevent a deadlock with the savepoint
        bool Break(SAPDBErr_MessageList &errlist)
        {
            return m_Iterator.Break(errlist);
        }

        /// get the ressources again and continue working
        bool Continue(SAPDBErr_MessageList &errlist)
        {
            return m_Iterator.Continue(errlist);
        }

        /// check for valid values
        bool IsValid() const
        {
            return m_Iterator.IsValid();
        }
        
        /// set to the invalid-value
        bool Invalidate(SAPDBErr_MessageList &errlist)
        {
            return m_Iterator.Invalidate(errlist);
        }
        
        /// get the iterator over the surrounding container
        Container::Iterator & GetContainerIter()
        {
            return m_Iterator;
        }
        
        void WriteToTrace(const char * title) const
        {
            m_Iterator.WriteToTrace(title);
        }
        
    private:
        /// this contains the informations about the undo-files
        Container::Iterator &m_Iterator; 
    };
    
public:

    /// If the root is valid, an existing file is assumed.
    Log_HistoryFile ( tgg00_TransContext &Trans,
                      const Data_PageNo   Root,
                      const Data_PageNo   Last);
              
    /// If the root is valid, an existing file is assumed.
    Log_HistoryFile (Data_PageAccessManager &Pam,
                     SAPDBMem_IRawAllocator &Allocator,
                     const Data_PageNo       Root,
                     const Data_PageNo       Last);
              
    virtual bool Create (SAPDBErr_MessageList &errlist);
    virtual bool Drop   (SAPDBErr_MessageList &errlist);
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist);

    /// pageno of last page
    Data_PageNo LastPageNo()
    {
        return m_Container.LastPageNo();
    }

    /// A new entry is created and appended to the file.
    bool AppendUndoFile (Log_UndoFile         &file,
                         SAPDBErr_MessageList &errlist);

    /// iter to first info, which my be invalid
    Iterator& GetFirstUndoFileInfo (SAPDBErr_MessageList &errlist);

    /// This removes the referenced entry
    bool RemoveUndoFileInfo (Iterator             &iter,
                             tgg91_TransNo        &newOldestTransno,
                             SAPDBErr_MessageList &errlist);

private:

    /// The container for this file.
    Container m_Container;

    /// The iterator for the container.
    Container::Iterator  m_ContainerIterator;

    /// the iterator for accessing undoFiles
    Iterator  m_Iterator;

    /// the allocator used in this object
    SAPDBMem_IRawAllocator &m_Allocator;
};


/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINITION OF INLINE METHODS                                              *
 *===========================================================================*/
                                         
#endif  /* Log_HistoryFile_H */
