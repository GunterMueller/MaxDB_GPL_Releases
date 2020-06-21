/*!
  @file           Log_HistoryDirectory.hpp
  @author         UweH
  @ingroup        History
  @brief          defines the classes Log_HistoryDirectory

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
#ifndef Log_HistoryDirectory_H
#define Log_HistoryDirectory_H

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "DataAccess/Data_ChainFixSizeSpace.hpp"
#include "DataAccess/Data_BaseFile.hpp"

class Log_HistoryFile;

/*!
@class Log_HistoryDirectory
@brief This stores anchors to undo file.
*/
class Log_HistoryDirectory : public Data_BaseFile
{

public:
    
    typedef Data_PageFixSizeSpace             PageType;             ///< shortcut
    typedef Data_ChainFixSizeSpace<PageType>  Container;            ///< shortcut
    typedef Container::Iterator               Iterator;             ///< shortcut
    typedef Data_ChainIterator<PageType>      PageIterator;         ///< shortcut

    /// This describes a history file and is used to write into directory.
    struct HistoryFileInfo
    {
        Data_PageNo   root;            ///< 4 bytes long
        Data_PageNo   last;            ///< 4 bytes long
        tgg91_TransNo oldestTransNo;   ///< 6 bytes long
        tgg91_TransNo youngestTransNo; ///< 6 bytes long
                                       //  20 bytes long
        /// contructor: invalidates the object at first
        HistoryFileInfo()
        {
            Invalidate();
        }
        /// returns true, if the root is valid
        bool IsValid() const
        {
            return root.IsValid();
        }
        /// invalidates all members
        void Invalidate()
        {
            root.Invalidate();
            last.Invalidate();
            oldestTransNo.gg90SetNil();
            youngestTransNo.gg90SetNil();
        }
    };

public:

    /*!
        @brief       constructor
        @param       Trans               [in/out] Transactioncontext
        @param       Root                [in]     If the RootPageNo is valid, an existing file is assumed.
    */
    Log_HistoryDirectory (tgg00_TransContext &Trans,
                          const Data_PageNo   Root );
              
    virtual bool Create (SAPDBErr_MessageList &errlist);
    virtual bool Drop   (SAPDBErr_MessageList &errlist);
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist);
    /*!
        @brief       A new entry is created and appended to the file.
        @param       info [in] describes the file to be added
    */
    bool AppendHistoryFile (const HistoryFileInfo &info,
                            SAPDBErr_MessageList  &errlist);
    /// result of GetFirstHistoryFileInfo
    enum GetResult
    {
        getResultMoreToRead,
        getResultLastRead,
        getResultError
    };
    /*! 
        @brief       The first entry of the file is read.
        @param       fileinfo [out] describes the first found file
        @return      GetResult
    */
    GetResult GetFirstHistoryFileInfo (HistoryFileInfo      &fileinfo,
                                      SAPDBErr_MessageList &errlist);
    /*! 
        @brief       The next entry of the file is read.
        @param       fileinfo [out] describes the first found file
        @return      GetResult
    */
    GetResult GetNextHistoryFileInfo (HistoryFileInfo      &fileinfo,
                                      SAPDBErr_MessageList &errlist);
    /*!
        @brief       For migration of wrong page type.
        @param       trans [in/out]
    */
    bool MigratePageType (tgg00_TransContext   &trans,
                          SAPDBErr_MessageList &errlist);

private:

    /// The container for this file.
    Container m_Container;

    /// The iterator for the container.
    Iterator  m_Iterator;

    /// this allocator is used inside this object
    SAPDBMem_IRawAllocator &m_Allocator;
};
#endif  /* Log_HistoryDirectory_H */
