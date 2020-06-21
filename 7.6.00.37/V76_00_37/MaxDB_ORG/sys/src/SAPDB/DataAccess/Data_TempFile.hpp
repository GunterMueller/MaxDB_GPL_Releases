/*!
  @file           Data_TempFile.hpp
  @author         UweH
  @ingroup        File
  @brief          temp file handling

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
#ifndef Data_TempFile_HPP
#define Data_TempFile_HPP

#include "DataAccess/Data_BaseFile.hpp"
#include "DataAccess/Data_ChainFixSizeSpace.hpp"

struct tgg00_TransContext;

/// class definition
class Data_TempFile : public Data_BaseFile
{
private:
	/// short form of page type
    typedef Data_PageFixSizeSpace             PageType;
	/// short form of used container type
    typedef Data_ChainFixSizeSpace<PageType>  Container;
	/// short form of used page iter
    typedef Data_ChainIterator<PageType>      PageIterator;
public:
    /// record iterator
    typedef Container::Iterator               RecordIterator;
public:
    /*!
    @brief a temp file handler is created.
    @param trans [in/out] trans context
    @param spaceSize [in] this gives the length of all records in this file.
    
    The used container handles only fix sized spaces.
    */
    Data_TempFile ( tgg00_TransContext      &trans,
                    const Data_RecordLength  spaceSize );
	/*!
    @brief The copy contructor.
    
    The PageAccessManager is shared after the call.
    */
    Data_TempFile ( const Data_TempFile& newFile );
    /*!
    @brief standard destructor which calls Drop()
    */
    ~Data_TempFile ()
    {
        SAPDBErr_MessageList errlist;
        if(!Drop(errlist))
            RTE_Message(errlist);
    }
	/*!
    @param key          [in]
    @param record       [in] the bytes to store
    @param recordLength [in] the length of the given record
    @param errlist
    @return true if given record could be insterted into the file.
    */
    bool Append (const SAPDB_Byte     *space,
                 SAPDBErr_MessageList &errlist );
	/*!
    @param iter         [out]
    @param errlist      [out]
    @return true if record could be accessed.
    @brief  This returns the iterator which points to the first readable space.
    */
    bool GetFirst ( RecordIterator       &iter,
                    SAPDBErr_MessageList &errlist );
	/// returns the size of the pre defined space size
    Data_RecordLength GetSpaceSize () const
    {
        return m_Container.GetSpaceSize();
    }
    /// returns the maximum usable size of one page in the container
    static SAPDB_UInt MaxFreeSpacePerPage()
    {
        return PageType::MaxSpaceSize();
    }
public: /// Data_IFile interface
    virtual bool Create(SAPDBErr_MessageList &errlist);
    virtual bool Drop  (SAPDBErr_MessageList &errlist);
    virtual bool Verify (bool                  isCold,
                         SAPDBErr_MessageList &errlist);
private:
    /// used container
    Container m_Container;
};
#endif // Data_TempFile_HPP
